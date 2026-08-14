#!/usr/bin/env python3
"""Translation-unit reconstruction pipeline: candidate discovery, shadow-TU
generation, pinned-toolchain compilation, and multi-function verification.

See notes/translation-unit-reconstruction-plan.md section 7 for the interface
this generalizes, and notes/tu-reconstruction-pilot-report.md for the worked
example (ov045/PoleLift) done by hand that this tool's `verify` is required to
reproduce. Read both before changing this file.

Subcommands:
    python tools/tubuild.py list                     # candidate TU worklist
    python tools/tubuild.py inspect ov045/PoleLift    # one candidate, full detail
    python tools/tubuild.py create  ov062/Chuckya     # generate a shadow .cpp
    python tools/tubuild.py compile ov045/PoleLift    # compile with the pinned toolchain
    python tools/tubuild.py verify  ov045/PoleLift    # byte + relocation verification
    python tools/tubuild.py linkcheck ...             # NOT YET IMPLEMENTED (plan sec 7.6)
    python tools/tubuild.py promote   ...             # NOT YET IMPLEMENTED (plan sec 7.7)

This tool never touches src/, config/**/delinks.txt, or runs real
`eligible.py --apply` / `rombuild.py`. It only reads production state (the
committed config/, the extracted ROM, the pinned mwccarm) and writes to
src_tu/, config/tu_manifest.json, and build/tu/ (gitignored, see .gitignore's
bare `build/` entry).

Every byte/relocation comparison is delegated to the tree's existing gates --
tools/match.py (compile + relocation-aware compare), tools/objisolate.py
(relocation type/addend correctness -- the check that caught pilot #1's real
bug that match.py alone reported as a false MATCH), tools/reloc_audit.py
(relocation destination identity), and tools/build_pin.py (which compiler
version and flags the real ROM build would use). Nothing here reimplements
compilation or byte comparison.
"""
import argparse
import collections
import io
import json
import pathlib
import re
import subprocess
import sys

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

REPO = pathlib.Path(__file__).resolve().parent.parent
TOOLS = REPO / "tools"
sys.path.insert(0, str(TOOLS))

import build_pin as BP          # noqa: E402
import enroll as EN             # noqa: E402
import match as M               # noqa: E402
import modules as MOD           # noqa: E402
import objisolate as OI         # noqa: E402
import reloc_audit as RA        # noqa: E402
import relocs as RL             # noqa: E402
import srcpath as SP            # noqa: E402
import tu_map as TM             # noqa: E402

TU_MAP = REPO / "build" / "tu_map.json"
MANIFEST = REPO / "config" / "tu_manifest.json"
SRC_TU = REPO / "src_tu"
BUILD_TU = REPO / "build" / "tu"
CFG_ARM9 = REPO / "config" / "arm9"

_CONF_LEVEL = {"low": 0, "medium": 1, "high": 2}


# ============================================================== module/config plumbing

_mods_by_name = None


def all_modules():
    global _mods_by_name
    if _mods_by_name is None:
        _mods_by_name = {m["name"]: m for m in MOD.modules()}
    return _mods_by_name


def module_config_dir(label):
    """Directory holding <label>'s delinks.txt/symbols.txt (modules.py's own layout)."""
    if label == "arm9":
        return CFG_ARM9
    if label in ("itcm", "dtcm"):
        return CFG_ARM9 / label
    return CFG_ARM9 / "overlays" / label


_mod_syms_cache = {}


def module_symbols(label):
    """[(name, kind, addr, size)] for every `kind:WORD(...)` symbol in a module.

    Reuses tu_map.symbols(), which already parses symbols.txt this way -- both the
    function inventory and the `_ZTV/_ZTI/_ZTS present` disqualifier come from one
    parse. Cached per module for the lifetime of the process."""
    if label not in _mod_syms_cache:
        mod = all_modules().get(label)
        _mod_syms_cache[label] = TM.symbols(mod) if mod else []
    return _mod_syms_cache[label]


def module_function_index(label):
    """{name: (addr, size)} for kind:function symbols in a module."""
    return {n: (a, s) for (n, k, a, s) in module_symbols(label) if k == "function"}


def module_symbol_names(label):
    return {n for (n, _k, _a, _s) in module_symbols(label)}


_delinks_cache = {}


def module_delinks_entries(label):
    """{src-rel-posix: complete_bool} for a module, via enroll.read_delinks (reused)."""
    if label not in _delinks_cache:
        path = module_config_dir(label) / "delinks.txt"
        if path.is_file():
            _header, entries = EN.read_delinks(path)
        else:
            entries = {}
        _delinks_cache[label] = entries
    return _delinks_cache[label]


def is_complete(label, legacy_rel):
    if legacy_rel is None:
        return False
    return bool(module_delinks_entries(label).get(legacy_rel.replace("\\", "/"), False))


def mangled_tags(cls):
    """{_ZTV<mangled>, _ZTI<mangled>, _ZTS<mangled>} for an Itanium class name."""
    tag = f"{len(cls)}{cls}"
    return {"_ZTV" + tag, "_ZTI" + tag, "_ZTS" + tag}


def rtti_symbols_for(module, classes):
    names = module_symbol_names(module)
    out = []
    for cls in classes:
        for tag in mangled_tags(cls):
            if tag in names:
                out.append(tag)
    return sorted(out)


# ==================================================================== tu_map.json access

def tu_map_freshness():
    """(exists, staleness_note_or_None). Never regenerates on its own; see load_tu_map."""
    if not TU_MAP.is_file():
        return False, None
    mtime = TU_MAP.stat().st_mtime
    newer = []
    for pat in ("config/**/delinks.txt", "config/**/symbols.txt"):
        for p in REPO.glob(pat):
            try:
                if p.stat().st_mtime > mtime:
                    newer.append(p)
            except OSError:
                continue
    if not newer:
        return True, None
    newer.sort()
    note = (f"build/tu_map.json is older than {len(newer)} config file(s) under "
            f"config/**/ (e.g. {newer[0].relative_to(REPO).as_posix()}); consider "
            f"running `python tools/tu_map.py` to refresh before trusting this worklist")
    return True, note


def load_tu_map():
    """Load build/tu_map.json, regenerating ONLY when the file is entirely missing.

    A stale-but-present map is reported (see tu_map_freshness) and used as-is: this
    tool has no opinion about when a regeneration is warranted, per the assignment
    ("check its age and just note it, don't force a regeneration policy")."""
    exists, note = tu_map_freshness()
    if not exists:
        print(f"{TU_MAP.relative_to(REPO).as_posix()} is missing; regenerating via "
              f"`python tools/tu_map.py` ...", file=sys.stderr)
        r = subprocess.run([sys.executable, str(TOOLS / "tu_map.py")], cwd=REPO)
        if r.returncode != 0 or not TU_MAP.is_file():
            raise SystemExit("tools/tu_map.py failed to produce build/tu_map.json")
        note = None
    data = json.loads(TU_MAP.read_text(encoding="utf-8"))
    return data, note


def tu_tail(unit):
    classes = unit.get("classes") or []
    if len(classes) == 1:
        return classes[0]
    if classes:
        return "+".join(sorted(classes))
    # build/tu_map.json can legitimately emit two distinct unattributed units that
    # START at the same address (observed for itcm: 0x1ff8000..0x1ff8004 and
    # 0x1ff8000..0x1ff870c both exist as separate entries -- itself a quirk of
    # tu_map.py's own absorb_unlabelled() for a module with almost no named
    # symbols, not something this tool changes). Keying only on start would make
    # tu_id_for collide, so an unattributed tail always carries the end address too.
    return f"@{int(unit['start'], 16):08x}-{int(unit['end'], 16):08x}"


def module_unit_ids(module, rec):
    """[(unit, tu_id)] for a module's tu_map units, tu_id's tail disambiguated by
    starting address whenever the plain tail is not unique within the module.

    A plain class-name tail collides for real: plan sec 2 says outright that "a
    large class can have methods defined across several TUs", and ov006's
    dScMgBSC_c (and eleven siblings) does exactly that -- two disjoint,
    non-adjacent address runs both carrying the same class label, because
    tu_map's union-find only merges OVERLAPPING spans. Without this, `list` would
    print two rows with an identical id and `inspect`/`create`/`compile`/`verify`
    would silently resolve to whichever one tu_map happened to list first."""
    units = rec.get("units", [])
    tails = [tu_tail(u) for u in units]
    counts = collections.Counter(tails)
    out = []
    for u, tail in zip(units, tails):
        if counts[tail] > 1:
            tail = f"{tail}@{int(u['start'], 16):08x}"
        out.append((u, f"{module}/{tail}"))
    return out


def iter_candidate_units(tu_map_data, module_filter=None):
    for mod_name, rec in sorted(tu_map_data.get("modules", {}).items()):
        if module_filter and mod_name != module_filter:
            continue
        for u, tid in module_unit_ids(mod_name, rec):
            yield mod_name, rec, u, tid


def find_candidate(tu_map_data, tu_id):
    """(module, module_record, unit) for a tu_map-derived id, or (None, None, None)."""
    if "/" not in tu_id:
        return None, None, None
    module = tu_id.split("/", 1)[0]
    rec = tu_map_data.get("modules", {}).get(module)
    if not rec:
        return None, None, None
    for u, tid in module_unit_ids(module, rec):
        if tid == tu_id:
            return module, rec, u
    return None, None, None


def tu_confidence(tus, i):
    """This TU's own confidence: the worse of its two boundary confidences.

    Reuses tu_map.boundary_confidence exactly (a per-BOUNDARY judgement, scored on
    both sides of unit i rather than in the abstract, per tu_map's own comment on
    why a TU-level score hides a wrong cut)."""
    levels = []
    if i > 0:
        levels.append(TM.boundary_confidence(tus[i - 1], tus[i]))
    if i < len(tus) - 1:
        levels.append(TM.boundary_confidence(tus[i], tus[i + 1]))
    if not levels:
        return "high" if tus[i].get("classes") else "low"
    return min(levels, key=lambda lv: _CONF_LEVEL[lv])


def unit_functions(module, unit):
    """[(ordinal, symbol, addr, size)] in ROM address order for a tu_map unit."""
    fidx = module_function_index(module)
    rows = []
    for name in unit.get("functions", []):
        got = fidx.get(name)
        if got is None:
            continue
        addr, size = got
        rows.append((addr, name, size))
    rows.sort()
    return [(i, name, addr, size) for i, (addr, name, size) in enumerate(rows)]


# ============================================================================ manifest I/O

def load_manifest():
    if not MANIFEST.is_file():
        return {"schema_version": 1,
                "about": ("Recovered translation-unit membership and licensed ranges. "
                          "See notes/translation-unit-reconstruction-plan.md section 6 "
                          "for the entry schema. Nothing here is enrolled."),
                "entries": []}
    return json.loads(MANIFEST.read_text(encoding="utf-8"))


def save_manifest(data):
    MANIFEST.write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n",
                        encoding="utf-8", newline="\n")


def manifest_entry(data, tu_id):
    return next((e for e in data.get("entries", []) if e.get("id") == tu_id), None)


def upsert_manifest_entry(data, entry):
    entries = data.setdefault("entries", [])
    for i, e in enumerate(entries):
        if e.get("id") == entry["id"]:
            entries[i] = entry
            return
    entries.append(entry)


# =================================================================== disqualifier scan

_PRAGMA_RE = re.compile(r'^\s*#\s*pragma\b.*$', re.MULTILINE)
_DTOR_RE = re.compile(r'D[0-2]Ev$')


def function_disqualifiers(module, ordinal_rows):
    """Given [(ordinal, symbol, addr, size)]: pragmas, dtor variants, unmatched functions."""
    out = {"pragmas": {}, "dtor_variants": [], "unmatched": [], "no_legacy_source": []}
    for _o, name, _addr, _size in ordinal_rows:
        if _DTOR_RE.search(name):
            out["dtor_variants"].append(name)
        legacy = SP.path_for(name)
        if legacy is None:
            out["no_legacy_source"].append(name)
            out["unmatched"].append(name)
            continue
        legacy_rel = legacy.relative_to(REPO).as_posix()
        if not is_complete(module, legacy_rel):
            out["unmatched"].append(name)
        text = legacy.read_text(encoding="utf-8", errors="ignore")
        pragmas = _PRAGMA_RE.findall(text)
        if pragmas:
            out["pragmas"][legacy_rel] = pragmas
    return out


# =========================================================================== `list`

def cmd_list(args):
    data, note = load_tu_map()
    if note:
        print(f"NOTE: {note}\n")
    manifest_data = load_manifest()

    rows = []
    sample_pool = []
    for mod_name, rec, unit, tid in iter_candidate_units(data, args.module):
        tus = rec["units"]
        idx = tus.index(unit)
        conf = tu_confidence(tus, idx)
        ord_rows = unit_functions(mod_name, unit)
        dq = function_disqualifiers(mod_name, ord_rows)
        rtti = rtti_symbols_for(mod_name, unit.get("classes") or [])
        entry = manifest_entry(manifest_data, tid)
        rows.append({
            "id": tid, "module": mod_name,
            "start": int(unit["start"], 16), "end": int(unit["end"], 16),
            "n_funcs": len(ord_rows), "n_matched": len(ord_rows) - len(dq["unmatched"]),
            "classes": unit.get("classes") or [],
            "confidence": conf,
            "sinit_vs_tu": rec.get("sinit_vs_tu"), "corroborated": rec.get("corroborated"),
            "pragmas": len(dq["pragmas"]), "dtors": len(dq["dtor_variants"]),
            "rtti": len(rtti), "unmatched": len(dq["unmatched"]),
            "manifest_status": entry.get("status") if entry else None,
        })
        if ord_rows:
            sample_pool.append((mod_name, ord_rows[0]))

    if args.limit:
        rows = rows[:args.limit]

    hdr = (f"{'id':42} {'range':21} {'fn':>3} {'built':>7} {'conf':6} "
          f"{'sinit':11} {'flags':22} status")
    print(hdr)
    print("-" * len(hdr))
    for r in rows:
        flags = []
        if r["pragmas"]:
            flags.append(f"pragma:{r['pragmas']}")
        if r["dtors"]:
            flags.append(f"dtor:{r['dtors']}")
        if r["rtti"]:
            flags.append(f"rtti:{r['rtti']}")
        if r["unmatched"]:
            flags.append(f"unmatched:{r['unmatched']}")
        rng = f"0x{r['start']:07x}-0x{r['end']:07x}"
        built = f"{r['n_matched']}/{r['n_funcs']}"
        sinit = f"{r['sinit_vs_tu'] or '-'}/{'corrob' if r['corroborated'] else '-'}"
        print(f"{r['id']:42} {rng:21} {r['n_funcs']:3d} {built:>7} "
             f"{r['confidence']:6} {sinit:11} {','.join(flags) or '-':22} "
             f"{r['manifest_status'] or '-'}")

    print(f"\n{len(rows)} candidate TU(s) shown "
         f"(module filter: {args.module or 'none'}"
         f"{f', limited to {args.limit}' if args.limit else ''})")
    print("'built' is config/**/delinks.txt `complete` markers, cross-referenced per "
         "function -- NOT byte-verified by itself (see the sample below). 'conf' is the "
         "worse of this TU's two boundary confidences (tu_map.boundary_confidence).")

    n = min(args.verify_sample, len(sample_pool)) if args.verify_sample else 0
    if n:
        import random
        rng_ = random.Random(0)
        sample = rng_.sample(sample_pool, n)
        print(f"\nsampling {n} function(s) against tools/build_pin.verify -- a `complete` "
             f"marker has been wrong before in this project's own history, so it is not "
             f"trusted alone; this is a spot check, not a full audit (use `inspect` for a "
             f"full per-function audit of one TU):")
        agree = 0
        for mod_name, (_o, name, addr, size) in sample:
            legacy = SP.path_for(name)
            if legacy is None:
                print(f"  {mod_name:7} {name:42} NO LEGACY SOURCE")
                continue
            ok, detail = BP.verify(legacy, name, addr, size, mod_name)
            complete = is_complete(mod_name, legacy.relative_to(REPO).as_posix())
            agrees = ok == complete
            agree += agrees
            flag = "" if agrees else f"   <<< DISAGREES WITH 'complete': {detail}"
            print(f"  {mod_name:7} {name:42} complete={str(complete):5} "
                 f"build_pin.verify={str(ok):5}{flag}")
        print(f"sample agreement: {agree}/{n}")


# ========================================================================= `inspect`

def cmd_inspect(args):
    data, note = load_tu_map()
    if note:
        print(f"NOTE: {note}\n")
    module, rec, unit = find_candidate(data, args.id)
    if unit is None:
        mod_guess = args.id.split("/", 1)[0]
        raise SystemExit(f"no candidate TU {args.id!r} in build/tu_map.json (try "
                         f"`python tools/tubuild.py list --module {mod_guess}`)")

    ord_rows = unit_functions(module, unit)
    tus = rec["units"]
    idx = tus.index(unit)
    left = "edge" if idx == 0 else TM.boundary_confidence(tus[idx - 1], tus[idx])
    right = "edge" if idx == len(tus) - 1 else TM.boundary_confidence(tus[idx], tus[idx + 1])

    print(f"TU {args.id}")
    print(f"  module            {module}")
    print(f"  span (.text)      0x{int(unit['start'], 16):08x} .. 0x{int(unit['end'], 16):08x}")
    print(f"  classes           {', '.join(unit.get('classes') or []) or '(unattributed run)'}")
    print(f"  boundary conf.    {tu_confidence(tus, idx)}  (left={left}, right={right})")
    print(f"  module sinits     {rec['sinits']} sinit(s) / {rec['ctor_entries']} .ctor "
         f"entr{'y' if rec['ctor_entries'] == 1 else 'ies'}, sinit_vs_tu={rec['sinit_vs_tu']}, "
         f"corroborated={rec['corroborated']}  (module-wide corroboration, not narrowed to "
         f"this TU -- see notes/tu-reconstruction-pilot-report.md sec 2 for how pilot #1 "
         f"narrowed it to one TU by hand, via which sinit's relocs touch this TU's own "
         f"bss statics; that step is not automated here)")

    print("\n  functions (ROM address order):")
    for o, name, addr, size in ord_rows:
        legacy = SP.path_for(name)
        legacy_s = legacy.relative_to(REPO).as_posix() if legacy else "MISSING"
        complete = is_complete(module, legacy.relative_to(REPO).as_posix()) if legacy else False
        print(f"    [{o}] 0x{addr:08x}  size 0x{size:04x}  {name}")
        print(f"          legacy source: {legacy_s}   complete={complete}")

    dq = function_disqualifiers(module, ord_rows)
    rtti = rtti_symbols_for(module, unit.get("classes") or [])
    print("\n  disqualifying complications:")
    n_pragmas = sum(len(v) for v in dq["pragmas"].values())
    print(f"    pragmas in legacy sources        : {n_pragmas or 'none'}")
    for f, lines in dq["pragmas"].items():
        for ln in lines:
            print(f"       {f}: {ln.strip()}")
    print(f"    D0/D1/D2 destructor variants     : {dq['dtor_variants'] or 'none'}")
    print(f"    _ZTV/_ZTI/_ZTS symbols present   : {rtti or 'none'}")
    if rtti:
        print("       -> this TU would become a class's key-function TU; expect vtable/RTTI "
             "emission as an unlicensed-but-expected side effect (pilot report sec 4-5.1)")
    print(f"    functions without `complete`     : {dq['unmatched'] or 'none'}")
    print(f"    functions with no legacy source  : {dq['no_legacy_source'] or 'none'}")

    print("\n  data/external symbols referenced (config/**/relocs.txt, address-only):")
    mod_obj = all_modules().get(module)
    start, end = int(unit["start"], 16), int(unit["end"], 16)
    sym_index = RL.load_all_syms()
    seen = {}
    if mod_obj:
        for frm, _kind, to, tomod in TM.relocs(mod_obj):
            if start <= frm < end:
                key = (RL.normalize_module(tomod), to)
                seen.setdefault(key, sym_index.get(key, f"0x{to:08x}"))
    if seen:
        for (tomod, to), nm in sorted(seen.items(), key=lambda kv: kv[1]):
            print(f"    {tomod:7} 0x{to:08x}  {nm}")
    else:
        print("    (none recorded)")

    print("\n  build_pin.verify per function (every function -- this is one candidate):")
    all_ok = True
    for o, name, addr, size in ord_rows:
        legacy = SP.path_for(name)
        if legacy is None:
            print(f"    [{o}] {name:42} NO LEGACY SOURCE")
            all_ok = False
            continue
        ok, detail = BP.verify(legacy, name, addr, size, module)
        all_ok = all_ok and ok
        pin = BP.version_for(legacy.relative_to(REPO).as_posix(), name) or "?"
        print(f"    [{o}] {name:42} {'OK  ' if ok else 'FAIL'} (pin {pin})"
             f"{'' if ok else '  ' + str(detail)}")
    print(f"  all functions verify under the build's pin: {all_ok}")

    entry = manifest_entry(load_manifest(), args.id)
    print()
    if entry:
        print(f"  manifest entry: status={entry.get('status')}  source={entry.get('source')}")
    else:
        print("  manifest entry: none yet (`python tools/tubuild.py create "
             f"{args.id}` would generate one)")


# ============================================================== `create` -- source splitter

_CPP_MARKER = "//cpp"
_PRAGMA_LINE_RE = re.compile(r'^\s*#\s*pragma\b.*$')
_INCLUDE_RE = re.compile(r'^\s*#\s*include\s*.+$')
_DEFINE_RE = re.compile(r'^\s*#\s*define\b.*$')
_DECL_KEYWORDS = ("struct", "class", "enum", "typedef", "namespace")
_EXTERN_C_BLOCK_RE = re.compile(r'^\s*extern\s+"C"\s*\{\s*$')


def split_legacy_source(text):
    """Best-effort structural split of a one-function legacy file.

    Per plan sec 7.3, this must NOT blindly concatenate: it separates includes,
    pragmas, macros, extern decls, and local shadow struct/class/typedef/namespace
    blocks from the one function AGENTS.md guarantees the file holds, so a caller
    can union/dedupe the former and reassemble the latter -- flagging conflicts
    rather than resolving them. Returns a dict; `error` is set (and function_text
    is None) when this file's shape does not fit that assumption, which a caller
    must treat as "assemble this one by hand", not guess past.
    """
    lines = text.splitlines()
    n = len(lines)
    cpp = text.startswith(_CPP_MARKER)
    i = 1 if cpp else 0

    includes, pragmas, macros, externs, notes = [], [], [], [], []
    shadow_decls = []          # (kind, name, text)
    body_start = None

    def consume_block(start_i):
        """From a line that opens a brace, the index of the line balancing it back
        to depth 0 (absorbing a lone trailing ';' on the next line, if any)."""
        depth = lines[start_i].count("{") - lines[start_i].count("}")
        j = start_i
        while depth > 0 and j + 1 < n:
            j += 1
            depth += lines[j].count("{") - lines[j].count("}")
        if j + 1 < n and lines[j + 1].strip() == ";":
            j += 1
        return j

    while i < n:
        raw = lines[i]
        stripped = raw.strip()

        if stripped == "":
            i += 1
            continue
        if stripped.startswith("/*"):
            j = i
            while "*/" not in lines[j] and j + 1 < n:
                j += 1
            notes.append("\n".join(lines[i:j + 1]))
            i = j + 1
            continue
        if stripped.startswith("//"):
            notes.append(stripped)
            i += 1
            continue
        if _PRAGMA_LINE_RE.match(raw):
            pragmas.append(stripped)
            i += 1
            continue
        if _INCLUDE_RE.match(raw):
            includes.append(stripped)
            i += 1
            continue
        if _DEFINE_RE.match(raw):
            macros.append(stripped)
            i += 1
            continue
        if _EXTERN_C_BLOCK_RE.match(raw):
            j = consume_block(i)
            for k in range(i + 1, j):
                s = lines[k].strip()
                if s and s != "}":
                    externs.append(s)
            i = j + 1
            continue
        if stripped.startswith("extern "):
            j, buf = i, stripped
            while "{" not in buf and ";" not in buf and j + 1 < n:
                j += 1
                buf += " " + lines[j].strip()
            if "{" in buf:
                body_start = i        # extern "C" prefixed straight onto a definition
                break
            externs.append(re.sub(r"\s+", " ", buf))
            i = j + 1
            continue
        first_word = (stripped.split()[0] if stripped.split() else "").strip("*&")
        if first_word in _DECL_KEYWORDS:
            j = consume_block(i)
            nm = re.match(rf'{first_word}\s+(\w+)', stripped)
            dname = nm.group(1) if nm else f"<anonymous {first_word} in this file>"
            shadow_decls.append((first_word, dname, "\n".join(lines[i:j + 1])))
            i = j + 1
            continue

        body_start = i
        break

    if body_start is None:
        return {"error": "scanned to end of file without finding a function body",
                "cpp": cpp, "includes": includes, "pragmas": pragmas, "macros": macros,
                "externs": externs, "shadow_decls": shadow_decls, "notes": notes,
                "function_text": None}

    function_text = "\n".join(lines[body_start:]).rstrip("\n") + "\n"
    return {"error": None, "cpp": cpp, "includes": includes, "pragmas": pragmas,
            "macros": macros, "externs": externs, "shadow_decls": shadow_decls,
            "notes": notes, "function_text": function_text}


def _extern_name_of(line):
    m = re.search(r'([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*\(', line)
    if m:
        return m.group(1)
    m = re.search(r'([A-Za-z_]\w*)\s*;\s*$', line)
    return m.group(1) if m else line


def _macro_name_of(line):
    m = re.match(r'#\s*define\s+(\w+)', line)
    return m.group(1) if m else line


def _merge_field(ord_rows, parsed, getter, name_of, kind_label, warnings):
    """Union items across a TU's legacy files by name; flag same-name/different-text
    as a conflict rather than picking a winner silently (plan sec 7.3)."""
    seen, live, dead = {}, [], []
    for _o, name, _a, _s in ord_rows:
        for item in getter(parsed[name]):
            key = name_of(item)
            if key not in seen:
                seen[key] = (item, name)
                live.append((key, item))
            elif seen[key][0] != item:
                warnings.append(f"CONFLICT: {kind_label} {key!r} differs between the legacy "
                                f"file that used to hold {seen[key][1]} and the one that held "
                                f"{name}; kept the first, the other is commented out for review")
                dead.append((key, item, name))
    return live, dead


def assemble_shadow_source(tu_id, ord_rows, parsed):
    warnings = []
    cpp_needed = any(parsed[name]["cpp"] for _o, name, _a, _s in ord_rows)

    includes = []
    for _o, name, _a, _s in ord_rows:
        for inc in parsed[name]["includes"]:
            if inc not in includes:
                includes.append(inc)

    live_macros, dead_macros = _merge_field(
        ord_rows, parsed, lambda p: p["macros"], _macro_name_of, "macro", warnings)
    live_externs, dead_externs = _merge_field(
        ord_rows, parsed, lambda p: p["externs"], _extern_name_of, "extern declaration", warnings)
    live_decls, dead_decls = _merge_field(
        ord_rows, parsed, lambda p: p["shadow_decls"], lambda item: (item[0], item[1]),
        "local declaration", warnings)

    pragma_hits = [(name, p) for _o, name, _a, _s in ord_rows for p in parsed[name]["pragmas"]]

    out = []
    if cpp_needed:
        out.append("//cpp")
    out.append("/* SHADOW translation unit -- generated by tools/tubuild.py create.")
    out.append(f" * {tu_id}  ({len(ord_rows)} function(s))")
    out.append(" *")
    out.append(" * NOT ENROLLED, NOT CANONICAL. This file contributes nothing to the ROM")
    out.append(" * build. It is a STARTING POINT (plan sec 7.3): local shadow declarations")
    out.append(" * below were carried verbatim from the legacy files, not reconciled against")
    out.append(" * real project headers -- that judgement call is left to a human/LLM review,")
    out.append(" * the way pilot #1 reconciled PoleLift::Render and ::CleanupResources by hand")
    out.append(" * (see notes/tu-reconstruction-pilot-report.md sec 5.2).")
    out.append(" *")
    out.append(" * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56")
    out.append(" * emits one .text section per function, in the REVERSE of source order, so")
    out.append(" * the highest-address ROM function is written FIRST here. Do not reorder;")
    out.append(" * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented")
    out.append(" * exception (a destructor's D0/D1/D2 group has compiler-chosen order).")
    out.append(" *")
    out.append(" * Assembled from these legacy one-function sources (ROM address order):")
    for o, name, addr, size in ord_rows:
        out.append(f" *   [{o}] 0x{addr:08x}  {parsed[name]['legacy_path']}")
    out.append(" */")
    out.append("")

    if pragma_hits:
        out.append("/* TUBUILD WARNING -- #pragma directive(s) were present in the legacy")
        out.append(" * sources of this TU and were NOT carried into this file automatically.")
        out.append(" * Per notes/translation-unit-reconstruction-plan.md section 10, a pragma")
        out.append(" * that was FUNCTION-scoped in its own one-function file can become")
        out.append(" * TU-scoped once merged and silently change codegen for the OTHER")
        out.append(" * functions here. Decide by hand whether/where each one still applies:")
        for name, p in pragma_hits:
            out.append(f" *   {name}: {p}")
        out.append(" */")
        out.append("")

    if includes:
        out.append("/* Includes: union of the legacy files', first-seen in ROM-ascending")
        out.append(" * processing order. NOT verified for header ordering constraints (e.g. a")
        out.append(" * common.h-before-X rule) -- watch for new compile errors after this. */")
        out.extend(includes)
        out.append("")

    if live_decls or dead_decls:
        out.append("/* Local shadow declarations carried from the legacy files verbatim.")
        out.append(" * NOT reconciled against real project headers -- check include/*.h for")
        out.append(" * each of these before compiling; a real header should usually win. */")
        for _key, item in live_decls:
            kind, dname, text = item
            out.append(f"/* shadow {kind} {dname!r} */")
            out.append(text)
            out.append("")
        for key, item, name in dead_decls:
            kind, dname, text = item
            out.append(f"/* TUBUILD CONFLICT -- alternate body of {kind} {dname!r}, from the "
                       f"legacy file for {name}, NOT applied:")
            out.append(text)
            out.append("*/")
            out.append("")

    if live_macros or dead_macros:
        for _key, line in live_macros:
            out.append(line)
        for key, line, name in dead_macros:
            out.append(f"/* TUBUILD CONFLICT -- alternate #define of {key}, from the legacy "
                       f"file for {name}, NOT applied: {line} */")
        out.append("")

    if live_externs or dead_externs:
        if cpp_needed:
            out.append('extern "C" {')
        for _key, line in live_externs:
            out.append(line)
        for key, line, name in dead_externs:
            out.append(f"/* TUBUILD CONFLICT -- alternate declaration of {key}, from the "
                       f"legacy file for {name}, NOT applied: {line} */")
        if cpp_needed:
            out.append("}")
        out.append("")

    for o, name, addr, size in reversed(ord_rows):
        p = parsed[name]
        out.append("/* " + "-" * 74 + " */")
        out.append(f"/* ROM ordinal {o} -- {name}, 0x{addr:08x}, size 0x{size:x} */")
        out.append("/* " + "-" * 74 + " */")
        for note in p["notes"]:
            out.append(note)
        func_text = p["function_text"]
        if cpp_needed and not p["cpp"]:
            # A legacy .c file's identifier is unmangled by construction; giving the
            # merged C++ TU the same text without linkage protection would let the
            # compiler mangle it a second time (notes/double-mangling-defect.md).
            func_text = 'extern "C" ' + func_text
        out.append(func_text.rstrip("\n"))
        out.append("")

    return "\n".join(out).rstrip("\n") + "\n", warnings


def build_manifest_entry(tu_id, module, rec, unit, ord_rows, out_path, warnings):
    start, end = int(unit["start"], 16), int(unit["end"], 16)
    classes = unit.get("classes") or []
    tus = rec["units"]
    idx = tus.index(unit)
    conf = tu_confidence(tus, idx)
    tail = tu_id.split("/", 1)[1]
    evidence = [
        f"contiguous linker run: 0x{start:07x}..0x{end:07x}, {len(ord_rows)} function(s), "
        f"from build/tu_map.json (tools/tu_map.py)",
        (f"class label(s): {', '.join(classes)}" if classes
         else "no class label (unattributed run; absorbed by call-graph or left orphaned)"),
        f"module sinit corroboration: {rec['sinits']} sinit(s) / {rec['ctor_entries']} "
        f".ctor entries, sinit_vs_tu={rec['sinit_vs_tu']}, corroborated={rec['corroborated']} "
        f"(module-wide, NOT narrowed to this one TU -- see "
        f"notes/tu-reconstruction-pilot-report.md sec 2 for the by-hand narrowing step)",
    ]
    promoted_guess = f"src/{'actors/' if classes else f'unnamed/{module}/'}{tail}.cpp"
    return {
        "id": tu_id,
        "module": module,
        "source": out_path.relative_to(REPO).as_posix(),
        "promoted_source": promoted_guess,
        "status": "shadow",
        "boundary_confidence": conf,
        "boundary_evidence": evidence,
        "sections": [{"name": ".text", "start": f"0x{start:08x}", "end": f"0x{end:08x}"}],
        "functions": [
            {"symbol": name, "address": f"0x{addr:08x}", "size": f"0x{size:08x}",
             "legacy_source": SP.path_for(name).relative_to(REPO).as_posix(), "ordinal": o}
            for o, name, addr, size in ord_rows
        ],
        "data": [],
        "bss": [],
        "notes": (["Generated by tools/tubuild.py create. See the file's own header comment "
                  "for what was and was not reconciled, and "
                  "notes/translation-unit-reconstruction-plan.md section 7.3 for what this "
                  "generator does and does not attempt to resolve automatically."]
                 + [f"tubuild create warning: {w}" for w in warnings]),
    }


def cmd_create(args):
    tu_data, note = load_tu_map()
    if note:
        print(f"NOTE: {note}\n")
    module, rec, unit = find_candidate(tu_data, args.id)
    if unit is None:
        raise SystemExit(f"no candidate TU {args.id!r} in build/tu_map.json")

    ord_rows = unit_functions(module, unit)
    if not ord_rows:
        raise SystemExit(f"{args.id}: tu_map lists no symbols.txt-resolvable functions")

    missing = [name for _o, name, _a, _s in ord_rows if SP.path_for(name) is None]
    if missing:
        raise SystemExit(f"{args.id}: {len(missing)} function(s) have no legacy source under "
                         f"src/, cannot assemble a starting point: {missing}")

    parsed = {}
    for _o, name, _a, _s in ord_rows:
        legacy = SP.path_for(name)
        text = legacy.read_text(encoding="utf-8", errors="ignore")
        p = split_legacy_source(text)
        p["legacy_path"] = legacy.relative_to(REPO).as_posix()
        if p["error"]:
            raise SystemExit(
                f"{args.id}: could not parse {p['legacy_path']} ({name}): {p['error']}\n"
                f"This file's shape does not match tubuild's one-function-per-file "
                f"assumption; assemble this TU by hand instead, the way pilot #1 did "
                f"(see notes/tu-reconstruction-pilot-report.md).")
        parsed[name] = p

    body, warnings = assemble_shadow_source(args.id, ord_rows, parsed)

    # mwccarm selects its C vs C++ FRONT END from the file's EXTENSION, not just
    # from -lang: the same source text compiled as .c under "-lang c99" emits the
    # plain ROM symbol, while the identical text compiled as .cpp mangles it a
    # second time (Itanium-mangling an already-mangled name), even under the same
    # "-lang c99" flag -- verified directly against 2004/b56 while building this
    # tool. build_pin.flags_for's "-lang c++ iff //cpp" logic is necessary but not
    # sufficient here; the OUTPUT FILE's extension has to agree with it too, which
    # every existing one-function file does by construction (AGENTS.md: .c means C,
    # .cpp means //cpp) but a generator merging several legacy files must decide
    # explicitly. See the tubuild.py report for the isolated repro.
    cpp_needed = any(parsed[name]["cpp"] for _o, name, _a, _s in ord_rows)
    tail = args.id.split("/", 1)[1]
    subdir = "actors" if unit.get("classes") else f"overlays/{module}"
    out_path = SRC_TU / subdir / f"{tail}{'.cpp' if cpp_needed else '.c'}"
    other_ext = ".c" if cpp_needed else ".cpp"
    stale = out_path.with_suffix(other_ext)
    if stale.is_file():
        print(f"NOTE: {stale.relative_to(REPO).as_posix()} exists from an earlier run at the "
             f"other extension; this run needs {out_path.suffix} and will NOT delete it "
             f"-- remove it by hand once you've checked which one you want.")
    if out_path.is_file() and not args.force:
        raise SystemExit(f"{out_path.relative_to(REPO).as_posix()} already exists; pass "
                         f"--force to regenerate (this OVERWRITES any hand curation)")
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(body, encoding="utf-8", newline="\n")

    manifest_data = load_manifest()
    entry = build_manifest_entry(args.id, module, rec, unit, ord_rows, out_path, warnings)
    upsert_manifest_entry(manifest_data, entry)
    save_manifest(manifest_data)

    print(f"wrote {out_path.relative_to(REPO).as_posix()}  ({len(ord_rows)} function(s))")
    print(f"manifest entry {entry['id']} -> status=shadow "
         f"({MANIFEST.relative_to(REPO).as_posix()})")
    if warnings:
        print(f"\n{len(warnings)} item(s) need human review before compiling:")
        for w in warnings:
            print(f"  - {w}")
    else:
        print("no cross-file conflicts detected in includes/externs/macros/local decls")


# ============================================================================ ELF inventory

def elf_inventory(obj_bytes):
    elf = ELFFile(io.BytesIO(obj_bytes))
    sections = []
    for idx, s in enumerate(elf.iter_sections()):
        sections.append({"index": idx, "name": s.name, "type": s.header["sh_type"],
                         "size": s.header["sh_size"]})
    symtab = elf.get_section_by_name(".symtab")
    symbols = []
    if symtab is not None:
        for s in symtab.iter_symbols():
            if not s.name or s["st_shndx"] == "SHN_UNDEF":
                continue
            symbols.append({"name": s.name, "shndx": s["st_shndx"], "value": s["st_value"],
                            "size": s["st_size"], "bind": s["st_info"]["bind"],
                            "type": s["st_info"]["type"]})
    relocs = collections.defaultdict(list)
    for s in elf.iter_sections():
        if isinstance(s, RelocationSection):
            for r in s.iter_relocations():
                sym = symtab.get_symbol(r["r_info_sym"]) if symtab else None
                relocs[s.header["sh_info"]].append(
                    {"offset": r["r_offset"], "sym": sym.name if sym else None,
                     "addend": r["r_addend"] if s.is_RELA() else None})
    return {"sections": sections, "symbols": symbols, "relocs": dict(relocs)}


def unlicensed_inventory(entry, inv):
    """(unlicensed_funcs, unlicensed_objs, unlicensed_secs) -- symbols/sections the
    compiled object defines that the manifest does not license. Shared by `compile`
    and `verify` so the two report the same thing."""
    licensed_funcs = {f["symbol"] for f in entry["functions"]}
    licensed_data = ({d["symbol"] for d in entry.get("data", [])}
                     | {d["symbol"] for d in entry.get("bss", [])})
    # ARM EABI mapping symbols ($a/$t/$d) are a normal, expected artefact of every
    # mwccarm object -- they mark ARM/Thumb/data spans for the disassembler, not
    # real functions, and are emitted STT_FUNC/STB_LOCAL even in a single-function
    # object. Reporting them as "unlicensed" would flag every compile.
    func_syms = [s for s in inv["symbols"]
                if s["type"] == "STT_FUNC" and not s["name"].startswith("$")]
    obj_syms = [s for s in inv["symbols"] if s["type"] == "STT_OBJECT"]
    unlicensed_funcs = [s for s in func_syms if s["name"] not in licensed_funcs]
    unlicensed_objs = [s for s in obj_syms if s["name"] not in licensed_data]
    # A section already represented by an unlicensed_funcs/unlicensed_objs symbol is
    # not reported again here -- that would double-count the same fact from two
    # angles. unlicensed_secs is only for content a symbol lookup cannot see at all
    # (no STT_FUNC/STT_OBJECT claims any byte of it), which the symbol-level lists
    # above cannot express.
    claimed_shndx = {s["shndx"] for s in inv["symbols"] if isinstance(s["shndx"], int)}
    nontext = [s for s in inv["sections"] if s["size"] and s["type"] in OI.CONTENT
              and s["name"] != ".text" and s["index"] not in claimed_shndx
              and not any(s["name"].startswith(p) for p in OI.IGNORE)]
    licensed_secnames = {sec["name"] for sec in entry.get("sections", [])} | {".text"}
    unlicensed_secs = [s for s in nontext if s["name"] not in licensed_secnames]
    return unlicensed_funcs, unlicensed_objs, unlicensed_secs


# ================================================================== version/compile plumbing

def resolve_tu_version(entry, override=None):
    """(version, note_or_None) the pinned toolchain would use for this whole shadow TU,
    or (None, reason) if that is not knowable/consistent.

    Derived from each function's LEGACY source's pin (build_pin.version_for, keyed on
    that file's stem, exactly like the real build). If the legacy stems disagree, this
    refuses rather than picking one -- promotion will key on the PROMOTED file's own
    stem instead, which is a real divergence from build_pin's single-function
    assumption; see the written-up caveat in the final report."""
    if override:
        return override, "FORCED by --version -- NOT derived from the build's actual pin"
    if BP.pins() is None:
        return None, (f"{BP.VERSIONS_FILE.relative_to(REPO).as_posix()} is missing, so no "
                      f"pin is knowable (fails closed, like build_pin.pins())")
    seen = collections.defaultdict(list)
    for f in entry["functions"]:
        v = BP.version_for(f["legacy_source"], f["symbol"])
        if v is None:
            return None, f"inconsistent/unknown pin for {f['symbol']} ({f['legacy_source']})"
        seen[v].append(f["symbol"])
    if not seen:
        return BP.DEFAULT_VERSION, None
    if len(seen) > 1:
        detail = "; ".join(f"{v}: {', '.join(syms)}" for v, syms in seen.items())
        return None, f"CONFLICT -- legacy sources disagree on compiler version: {detail}"
    return next(iter(seen)), None


def sanitize_id(tu_id):
    return tu_id.replace("/", "-")


def _compile_tu(entry, version_override=None):
    """Compile the shadow source once. Returns (obj_bytes, version, flags, build_dir,
    obj_path); raises SystemExit with a clear reason on any failure (no compiler, no
    pin, compile error)."""
    src = REPO / entry["source"]
    if not src.is_file():
        raise SystemExit(f"shadow source not found: {entry['source']}")
    version, note = resolve_tu_version(entry, version_override)
    if version is None:
        raise SystemExit(f"cannot determine a compiler version for {entry['id']}: {note}")
    flags = BP.flags_for(src)
    print(f"compiling {entry['source']}  [{version}]")
    print(f"  flags: {flags}")
    if note:
        print(f"  NOTE: {note}")
    obj_bytes = M.compile_c(src, version, flags)
    if obj_bytes is None:
        raise SystemExit(f"{entry['source']} failed to compile under {version}")
    build_dir = BUILD_TU / sanitize_id(entry["id"])
    build_dir.mkdir(parents=True, exist_ok=True)
    obj_path = build_dir / (pathlib.Path(entry["source"]).stem + ".o")
    obj_path.write_bytes(obj_bytes)
    return obj_bytes, version, flags, build_dir, obj_path


# =============================================================================== `compile`

def cmd_compile(args):
    data = load_manifest()
    entry = manifest_entry(data, args.id)
    if entry is None:
        raise SystemExit(f"no manifest entry for {args.id!r} in "
                         f"{MANIFEST.relative_to(REPO).as_posix()}; run `create` first, or "
                         f"add one by hand (see notes/tu-reconstruction-pilot-report.md)")

    obj_bytes, version, flags, build_dir, obj_path = _compile_tu(entry, args.version)
    inv = elf_inventory(obj_bytes)
    unlicensed_funcs, unlicensed_objs, unlicensed_secs = unlicensed_inventory(entry, inv)

    lines = [f"TU {entry['id']}  object {obj_path.relative_to(REPO).as_posix()}",
            f"toolchain: {version}   flags: {flags}", ""]
    lines.append(f"sections ({len(inv['sections'])}):")
    for s in inv["sections"]:
        lines.append(f"  [{s['index']:3}] {s['name']:16} type={s['type']:14} "
                     f"size=0x{s['size']:x}")
    lines.append("")
    lines.append("manifest functions -> section:")
    for f in sorted(entry["functions"], key=lambda x: x["ordinal"]):
        code, _relocs = M.extract_func(obj_bytes, f["symbol"])
        if code is None:
            lines.append(f"  MISSING  {f['symbol']}  (not defined in the compiled object)")
            continue
        sec_idx = next((s["shndx"] for s in inv["symbols"] if s["name"] == f["symbol"]), None)
        sec_name = next((s["name"] for s in inv["sections"] if s["index"] == sec_idx), "?")
        lines.append(f"  ok       {f['symbol']:42} -> section[{sec_idx}] {sec_name:8} "
                     f"size=0x{len(code):x}")
    lines.append("")

    if unlicensed_funcs:
        lines.append(f"UNLICENSED function symbols ({len(unlicensed_funcs)}) -- defined in "
                     f"the object, absent from the manifest's function list:")
        for s in unlicensed_funcs:
            lines.append(f"  {s['name']:42} size=0x{s['size']:x}  bind={s['bind']}")
    if unlicensed_objs:
        lines.append(f"UNLICENSED object/data symbols ({len(unlicensed_objs)}):")
        for s in unlicensed_objs:
            lines.append(f"  {s['name']:42} size=0x{s['size']:x}  bind={s['bind']}")
    if unlicensed_secs:
        lines.append(f"UNLICENSED content sections ({len(unlicensed_secs)}):")
        for s in unlicensed_secs:
            lines.append(f"  [{s['index']:3}] {s['name']:16} size=0x{s['size']:x}")
    if not (unlicensed_funcs or unlicensed_objs or unlicensed_secs):
        lines.append("no unlicensed symbols or sections.")

    text = "\n".join(lines) + "\n"
    (build_dir / "inventory.txt").write_text(text, encoding="utf-8", newline="\n")
    inv_json = {"id": entry["id"], "version": version, "flags": flags,
               "sections": inv["sections"],
               "unlicensed_functions": [s["name"] for s in unlicensed_funcs],
               "unlicensed_objects": [s["name"] for s in unlicensed_objs],
               "unlicensed_sections": [s["name"] for s in unlicensed_secs]}
    (build_dir / "inventory.json").write_text(json.dumps(inv_json, indent=2) + "\n",
                                              encoding="utf-8", newline="\n")
    print()
    print(text)
    print(f"object -> {obj_path.relative_to(REPO).as_posix()}")
    print(f"report -> {(build_dir / 'inventory.txt').relative_to(REPO).as_posix()}")


# ================================================================================ `verify`

def cmd_verify(args):
    data = load_manifest()
    entry = manifest_entry(data, args.id)
    if entry is None:
        raise SystemExit(f"no manifest entry for {args.id!r}; run `create` first or add one "
                         f"by hand (see notes/tu-reconstruction-pilot-report.md)")

    obj_bytes, version, flags, build_dir, obj_path = _compile_tu(entry, args.version)
    inv = elf_inventory(obj_bytes)

    name_index = RA.build_name_index()
    config_relocs = RA.build_config_relocs()
    sym_index = RL.load_all_syms()

    print(f"\nTU {entry['id']}\n")
    rows = []
    all_bytes_ok = all_reloc_ok = True
    sec_order = []
    for f in sorted(entry["functions"], key=lambda x: x["ordinal"]):
        sym, addr, size = f["symbol"], int(f["address"], 16), int(f["size"], 16)
        code, relocs = M.extract_func(obj_bytes, sym)
        if code is None:
            print(f"MISSING  {sym:42} 0x{addr:08x}  size 0x{size:03x}  not in the compiled object")
            all_bytes_ok = False
            rows.append((f["ordinal"], sym, "MISSING"))
            continue
        secidx = next((s["shndx"] for s in inv["symbols"] if s["name"] == sym), None)
        if isinstance(secidx, int):
            sec_order.append((f["ordinal"], secidx))

        tgt = BP.target_bytes(entry["module"], addr, size)
        if tgt is None or len(tgt) != size:
            print(f"DIFF     {sym:42} 0x{addr:08x}  size 0x{size:03x}  no ROM bytes for this range")
            all_bytes_ok = False
            rows.append((f["ordinal"], sym, "DIFF"))
            continue

        ok, ndiff = M.compare(tgt, code, relocs, verbose=False)
        plan = OI.plan(obj_bytes, sym)
        iso_ok = plan.get("error") is None
        dest_rows, _missing = RA.check_destinations(obj_bytes, sym, addr, size, entry["module"],
                                                     name_index, config_relocs, sym_index)
        wrong = [r for r in (dest_rows or []) if r["verdict"] == "WRONG-DEST"]

        verdict = "MATCH" if (ok and iso_ok and not wrong) else "DIFF"
        all_bytes_ok = all_bytes_ok and ok
        all_reloc_ok = all_reloc_ok and iso_ok and not wrong
        extra = []
        if not ok:
            extra.append(f"{ndiff} word(s) differ")
        if not iso_ok:
            extra.append(f"objisolate: {plan.get('error')}")
        if wrong:
            extra.append(f"{len(wrong)} reloc destination(s) WRONG (first: "
                         f"{wrong[0]['cand']} != {wrong[0]['cfg']})")
        suffix = ("  -- " + "; ".join(extra)) if extra else ""
        print(f"{verdict:8} {sym:42} 0x{addr:08x}  size 0x{size:03x}{suffix}")
        rows.append((f["ordinal"], sym, verdict))

    n_match = sum(1 for _o, _s, v in rows if v == "MATCH")
    print(f"\nbyte comparison   : {n_match}/{len(rows)} MATCH  (tools/match.py extract_func + "
         f"compare, relocation-aware)")
    print(f"objisolate check  : {'clean' if all_reloc_ok else 'ONE OR MORE FAILURES'}  "
         f"(tools/objisolate.py plan() -- relocation type/addend; this is the check pilot #1 "
         f"needed to catch a bug match.compare alone reported as MATCH)")

    ordered = sorted(sec_order)
    idxs = [si for _o, si in ordered]
    bad_pairs = [(ordered[i][0], ordered[i + 1][0]) for i in range(len(idxs) - 1)
                if idxs[i] >= idxs[i + 1]]
    if not idxs:
        pass
    elif not bad_pairs:
        print(f"emission order    : all {len(idxs)} function(s) in the expected ROM-ascending "
             f"section order (mwccarm reverses source order; this TU was written in reverse "
             f"to compensate -- see the pilot report sec 3)")
    else:
        print(f"emission order    : {len(bad_pairs)} ordinal pair(s) NOT in ROM order: "
             f"{bad_pairs}  (a destructor's D0/D1/D2 group is ordered by the compiler, not "
             f"by source text, per the pilot report sec 3 -- not necessarily a bug)")

    unlicensed_funcs, unlicensed_objs, unlicensed_secs = unlicensed_inventory(entry, inv)
    n_unlicensed = len(unlicensed_funcs) + len(unlicensed_objs) + len(unlicensed_secs)
    if n_unlicensed:
        print()
        for s in unlicensed_funcs:
            print(f"EXTRA    .text    {s['name']:40} size=0x{s['size']:x}  {s['bind']}  "
                 f"not in manifest")
        for s in unlicensed_objs:
            sec_name = next((sec["name"] for sec in inv["sections"]
                            if sec["index"] == s["shndx"]), "?")
            print(f"EXTRA    {sec_name:8} {s['name']:40} size=0x{s['size']:x}  {s['bind']}  "
                 f"not in manifest")
        for s in unlicensed_secs:
            print(f"EXTRA    {s['name']:8} size=0x{s['size']:x}  not in manifest (no function/"
                 f"data claims it)")

    secs = sorted((int(s["start"], 16), int(s["end"], 16), s["name"])
                 for s in entry.get("sections", []))
    overlaps = [(a, b) for a, b in zip(secs, secs[1:]) if a[1] > b[0]]
    if overlaps:
        print(f"\nMANIFEST ERROR: overlapping section claims: {overlaps}")

    declared_syms = {f["symbol"] for f in entry["functions"]}
    defined_syms = {s["name"] for s in inv["symbols"] if s["type"] == "STT_FUNC"}
    if declared_syms - defined_syms:
        print(f"\nMANIFEST ERROR: declared but not defined: {sorted(declared_syms - defined_syms)}")

    text_verified = (all_bytes_ok and all_reloc_ok and not overlaps
                     and not (declared_syms - defined_syms)
                     and not any(v == "MISSING" for _o, _s, v in rows))
    print()
    if text_verified:
        print(f"Result: {n_match}/{len(rows)} MATCH, objisolate clean, reloc-destinations "
             f"clean -> TEXT-VERIFIED")
    else:
        print(f"Result: {n_match}/{len(rows)} MATCH -> NOT verified (see DIFF/MISSING/ERROR "
             f"lines above)")
    if n_unlicensed:
        print(f"        {n_unlicensed} unlicensed section/symbol(s) present -> PROMOTION "
             f"REFUSED regardless of the above (plan sec 4.5, 8)")

    entry.setdefault("verification", {})
    entry["verification"].update({
        "round": "tools/tubuild.py verify -- text-only, whole shadow TU compiled once",
        "toolchain": f"tools/mwccarm/{version}/mwccarm.exe",
        "flags": flags,
        "object": obj_path.relative_to(REPO).as_posix() + " (gitignored)",
        "comparison": "tools/match.py extract_func+compare (bytes/relocs) AND "
                      "tools/objisolate.py plan() (relocation type/addend) AND "
                      "tools/reloc_audit.py check_destinations() (relocation target identity)",
        "functions_matched": n_match,
        "functions_declared": len(rows),
    })
    # Keep the nested `criteria` block honest about what THIS run actually measured --
    # `comparison` above always names reloc_audit now that verify runs it unconditionally,
    # so a stale hand-authored "NOT RUN" left in criteria would contradict it. Overwrite
    # only the fields this run has an opinion on; leave later-phase fields (symbol
    # addresses, module/ROM build) alone since verify never touches those.
    criteria = entry["verification"].setdefault("criteria", {})
    criteria.update({
        "every_declared_function_defined": "PASS" if not any(v == "MISSING" for _o, _s, v in rows) else "FAIL",
        "every_declared_function_bytes_match": "PASS" if all_bytes_ok else "FAIL",
        "declared_function_set_equals_defined_function_set":
            "PASS" if not (declared_syms - defined_syms) and n_unlicensed == 0
            else f"FAIL-BY-DESIGN -- {n_unlicensed} unlicensed section/symbol(s); see unlicensed_output_observed",
        "functions_occur_in_expected_order":
            "PASS" if not bad_pairs else f"PARTIAL -- ordinal pair(s) not in ROM order: {bad_pairs}",
        "relocation_destinations_verified": "PASS" if all_reloc_ok else "FAIL -- see DIFF/objisolate lines above",
    })
    if text_verified and entry.get("status") == "shadow":
        entry["status"] = "text-verified"
    elif not text_verified and entry.get("status") == "text-verified":
        entry["status"] = "shadow"
        print("\nNOTE: manifest status downgraded text-verified -> shadow: this round did "
             "not reproduce it.")
    upsert_manifest_entry(data, entry)
    save_manifest(data)

    return 0 if text_verified else 1


# ============================================================================= stubs

def cmd_linkcheck(args):
    print("tools/tubuild.py linkcheck: NOT YET IMPLEMENTED "
         "(plan sec 7.6 -- a later phase; see the plan's Phase D/section 9).")
    return 2


def cmd_promote(args):
    print("tools/tubuild.py promote: NOT YET IMPLEMENTED "
         "(plan sec 7.7 -- a later phase; requires linkcheck first).")
    return 2


# =================================================================================== main

def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--manifest", type=pathlib.Path, default=None,
                    help="override config/tu_manifest.json -- point this at a scratch copy "
                         "for tests or exploratory runs so `verify`/`create` don't write back "
                         "into the tracked file (defaults to the real committed manifest)")
    sub = ap.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("list", help="candidate TUs from build/tu_map.json (pilot-selection worklist)")
    p.add_argument("--module", default=None, help="restrict to one module, e.g. ov045")
    p.add_argument("--limit", type=int, default=None)
    p.add_argument("--verify-sample", type=int, default=5,
                   help="sample this many functions with build_pin.verify, in addition to "
                        "trusting delinks.txt `complete` markers (default 5; 0 to skip)")
    p.set_defaults(func=cmd_list)

    p = sub.add_parser("inspect", help="full detail for one candidate TU")
    p.add_argument("id", help='e.g. "ov045/PoleLift"')
    p.set_defaults(func=cmd_inspect)

    p = sub.add_parser("create", help="generate a shadow .cpp under src_tu/ + manifest entry")
    p.add_argument("id")
    p.add_argument("--force", action="store_true", help="overwrite an existing shadow source")
    p.set_defaults(func=cmd_create)

    p = sub.add_parser("compile", help="compile the shadow .cpp with the pinned toolchain")
    p.add_argument("id")
    p.add_argument("--version", default=None, help="force a compiler version (exploratory only)")
    p.set_defaults(func=cmd_compile)

    p = sub.add_parser("verify", help="byte + relocation verification against the manifest and ROM")
    p.add_argument("id")
    p.add_argument("--version", default=None)
    p.set_defaults(func=cmd_verify)

    p = sub.add_parser("linkcheck", help="NOT YET IMPLEMENTED (plan sec 7.6)")
    p.add_argument("id", nargs="?")
    p.set_defaults(func=cmd_linkcheck)

    p = sub.add_parser("promote", help="NOT YET IMPLEMENTED (plan sec 7.7)")
    p.add_argument("id", nargs="?")
    p.set_defaults(func=cmd_promote)

    args = ap.parse_args()
    if args.manifest is not None:
        global MANIFEST
        MANIFEST = args.manifest
    return args.func(args) or 0


if __name__ == "__main__":
    sys.exit(main())
