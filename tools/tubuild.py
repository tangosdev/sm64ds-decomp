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
    python tools/tubuild.py partial ov045/PoleLift    # one TU compile -> N derived
                                                      #   per-function objects, each
                                                      #   compared against the object the
                                                      #   current build makes (plan sec 9)
    python tools/tubuild.py linkcheck ov002/LevelObjects           # whole-range link
    python tools/tubuild.py linkcheck ov045/PoleLift --partial     # N-object link
    python tools/tubuild.py promote   ...             # only --dry-run (plan sec 7.7)

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
import concurrent.futures
import hashlib
import io
import json
import pathlib
import re
import shutil
import subprocess
import sys
import time

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
import rombuild as RB           # noqa: E402
import rombuild_cache as RBK    # noqa: E402
import rombuild_check as RBC    # noqa: E402
import rombuild_profile as RP   # noqa: E402
import srcpath as SP            # noqa: E402
import tu_map as TM             # noqa: E402

TU_MAP = REPO / "build" / "tu_map.json"
BASELINE_LINK = REPO / "build" / "tu" / "_baseline" / "link"
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


# ========================================================== `linkcheck` -- scratch delinks

_SEC_LINE_RE = re.compile(r'^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)')


def parse_delinks_file(path):
    """(header_lines, [(entry_path, [body_lines])]) preserving every line verbatim.

    enroll.read_delinks answers "which entries carry `complete`" and throws the text
    away; a scratch splice has to rewrite the file, so it needs the lines back. This
    is the same grammar (unindented `path:` opens an entry, indented lines belong to
    it, leading indented lines before the first entry are the module section header)
    read losslessly instead."""
    header, entries, cur = [], [], None
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line.strip():
            if cur is not None:
                cur[1].append(line)
            else:
                header.append(line)
            continue
        if line[0].isspace():
            (cur[1] if cur is not None else header).append(line)
        else:
            cur = (line.strip().rstrip(":"), [])
            entries.append(cur)
    return header, entries


def entry_sections(body_lines):
    """[(name, start, end)] for one delinks entry body."""
    out = []
    for line in body_lines:
        m = _SEC_LINE_RE.match(line)
        if m:
            out.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return out


def entry_is_complete(body_lines):
    return any(l.strip() == "complete" for l in body_lines)


def span_entries(delinks_path, span_start, span_end, expected_legacy):
    """(header, entries, inside, reasons) for the delinks entries tiling a TU span.

    The read-only half of `splice_tu_entry`, factored out because the partial-isolation
    path (plan sec 9) needs exactly these checks and none of the rewriting: it leaves
    delinks.txt alone and substitutes per-function OBJECTS instead, so it must still
    know that the N entries it is about to overwrite are the N the manifest names, are
    `complete` today, and tile the span exactly. Two callers, one definition of "this
    span is safe to act on".

    `reasons` is empty iff the span is safe. The checks are:

      * every entry whose .text starts inside the span is fully inside it;
      * no entry straddles either boundary;
      * their .text ranges tile the span exactly -- abutting, no gap, no overlap;
      * every one of them carries `complete` (otherwise the substitution would ALSO
        be enrolling ranges the current build serves from ROM bytes, and a green
        result would be measuring two changes at once);
      * none of them declares a non-.text section (this round licenses .text only);
      * the set equals the manifest's own `legacy_source` list.
    """
    header, entries = parse_delinks_file(delinks_path)
    reasons = []
    inside, straddling = [], []
    for idx, (rel, body) in enumerate(entries):
        secs = entry_sections(body)
        if not secs:
            continue
        lo = min(s[1] for s in secs)
        hi = max(s[2] for s in secs)
        if hi <= span_start or lo >= span_end:
            continue
        if lo < span_start or hi > span_end:
            straddling.append((rel, lo, hi))
            continue
        inside.append((idx, rel, secs))
    for rel, lo, hi in straddling:
        reasons.append(f"entry {rel} (0x{lo:08x}..0x{hi:08x}) straddles the TU boundary")
    if not inside:
        reasons.append(f"no delinks entry lies inside 0x{span_start:08x}..0x{span_end:08x}")
        return header, entries, inside, reasons

    tiles = sorted((s1, s2, rel, name) for _i, rel, secs in inside for (name, s1, s2) in secs)
    for name, rel in [(n, r) for _s1, _s2, r, n in tiles if n != ".text"]:
        reasons.append(f"entry {rel} declares a non-.text section {name}; this round "
                       f"licenses .text only (plan sec 8)")
    cursor = span_start
    for s1, s2, rel, _n in tiles:
        if s1 != cursor:
            reasons.append(f"range gap/overlap at 0x{cursor:08x}: next entry {rel} starts "
                           f"at 0x{s1:08x}")
        cursor = max(cursor, s2)
    if cursor != span_end:
        reasons.append(f"entries stop at 0x{cursor:08x}, TU span ends at 0x{span_end:08x}")

    incomplete = [rel for _i, rel, _s in inside if not entry_is_complete(entries[_i][1])]
    for rel in incomplete:
        reasons.append(f"entry {rel} is NOT `complete` today (dsd serves it from ROM bytes); "
                       f"substituting the TU would change enrollment and byte provenance "
                       f"in the same step")

    got = {rel for _i, rel, _s in inside}
    want = {p.replace("\\", "/") for p in expected_legacy}
    if got != want:
        for rel in sorted(want - got):
            reasons.append(f"manifest names legacy source {rel}, which is not a delinks "
                           f"entry inside the span")
        for rel in sorted(got - want):
            reasons.append(f"delinks entry {rel} lies inside the span but the manifest "
                           f"does not list it as a legacy source")
    return header, entries, inside, reasons


def splice_tu_entry(delinks_path, span_start, span_end, tu_rel, expected_legacy):
    """Replace the per-function entries tiling [span_start, span_end) with ONE TU entry.

    Refuses -- returns (None, [reasons]) -- rather than producing a plausible-looking
    scratch config, because every failure mode here is silent downstream: dsd fills any
    range it has no object for with retail ROM bytes, so a mis-spliced delinks tree
    links clean and compares green while contributing nothing (see
    notes/unbuildable-files-invisible.md and layout_check's L1). `span_entries` holds
    the checks; this adds the rewrite.
    """
    header, entries, inside, reasons = span_entries(delinks_path, span_start, span_end,
                                                    expected_legacy)
    if reasons:
        return None, reasons

    first = min(i for i, _r, _s in inside)
    drop = {i for i, _r, _s in inside}
    out = list(header)
    for idx, (rel, body) in enumerate(entries):
        if idx == first:
            out.append(f"{tu_rel}:")
            out.append("    complete")
            out.append(f"    .text start:0x{span_start:08x} end:0x{span_end:08x}")
            out.append("")
            continue
        if idx in drop:
            continue
        out.append(f"{rel}:")
        out.extend(body)
    delinks_path.write_text("\n".join(out).rstrip("\n") + "\n", encoding="utf-8", newline="\n")
    return sorted(rel for _i, rel, _s in inside), []


_all_syms_index = None


def all_symbol_homes():
    """{name: [(module, addr), ...]} across EVERY checked-in symbols.txt.

    reloc_audit.build_name_index answers a different question and keeps only the
    first module per name (`setdefault`), which is right for its fallback lookup and
    wrong here: this needs every licensed home a name has, because one of them being
    outside the TU's span is exactly what makes the linker see a duplicate."""
    global _all_syms_index
    if _all_syms_index is None:
        idx = collections.defaultdict(list)
        for module, path in RL.iter_symbol_files(include_itcm_dtcm=True):
            for name, (mod, addr) in RL.iter_syms_pairs(path, module):
                idx[name].append((mod, addr))
        _all_syms_index = dict(idx)
    return _all_syms_index


def complete_ranges(config_root):
    """{module: [(start, end)]} for every `complete` delinks entry in a config tree.

    Used to tell a colliding symbol's OTHER definition apart: inside a complete range
    it comes from an isolated per-function source object (binding preserved from
    mwcc, so a vague-linkage pair is legal), outside one it comes from a dsd gap
    object, which defines every carved-out symbol STB_GLOBAL -- and a strong
    definition makes any second definition a hard multiply-defined error."""
    out = {}
    for dl in sorted(pathlib.Path(config_root).rglob("delinks.txt")):
        label = ("arm9" if dl.parent == pathlib.Path(config_root)
                 else dl.parent.name)
        rows = []
        for _rel, body in parse_delinks_file(dl)[1]:
            if entry_is_complete(body):
                rows.extend((a, b) for _n, a, b in entry_sections(body))
        out[label] = rows
    return out


def audit_tu_object(obj_bytes, entry, span_start, span_end, ranges=None):
    """What the linker is about to be handed, judged against the config, BEFORE linking.

    plan sec 4.5 -- "a compiled object is eligible only when every defined function,
    object, content section, relocation and range is expected by its manifest. Extra
    output is a failure, not something the linker may silently ignore." mwldarm's own
    error list is truncated (it aborts after the first few), so the mechanical audit
    is better evidence than the link log about WHICH extras exist.

    Buckets every defined symbol as:
      LICENSED  named by the manifest, address inside the TU span;
      COLLIDES  the config gives this name a home OUTSIDE the span, so a delink gap
                object or another enrolled source already defines it -- multiply
                defined under `-nodead`;
      HOMELESS  no symbols.txt anywhere names it, so it has no licensed address and
                the linker must invent one inside the TU's contribution.
    """
    inv = elf_inventory(obj_bytes)
    homes = all_symbol_homes()
    licensed = {f["symbol"] for f in entry["functions"]}
    licensed |= {d["symbol"] for d in entry.get("data", [])}
    licensed |= {d["symbol"] for d in entry.get("bss", [])}
    secname = {s["index"]: s["name"] for s in inv["sections"]}
    rows = []
    for s in inv["symbols"]:
        if s["type"] not in ("STT_FUNC", "STT_OBJECT") or s["name"].startswith("$"):
            continue
        name = s["name"]
        where = homes.get(name, [])
        outside = [(m, a) for (m, a) in where if not (span_start <= a < span_end)]
        gap = any(not any(lo <= a < hi for lo, hi in (ranges or {}).get(m, []))
                  for m, a in outside)
        if name in licensed:
            verdict = "LICENSED"
        elif outside:
            verdict = "COLLIDES-GAP" if gap else "COLLIDES-SRC"
        else:
            verdict = "HOMELESS"
        rows.append({"name": name, "bind": s["bind"], "type": s["type"], "size": s["size"],
                     "section": secname.get(s["shndx"], "?"), "verdict": verdict,
                     "homes": [f"{m}:0x{a:08x}" for m, a in outside]})
    extra_secs = [s for s in inv["sections"]
                  if s["size"] and s["type"] in OI.CONTENT and s["name"] != ".text"
                  and not any(s["name"].startswith(p) for p in OI.IGNORE)]
    # Emission order: the licensed functions' .text sections must come out in ROM
    # address order, because the LCF selects `<stem>.o(.text)` and mwldarm lays those
    # sections down in object order. A swap here is invisible to per-function
    # comparison and fatal to a whole-range link (pilot #2 sec 7's first control).
    ordinal = {f["symbol"]: int(f["address"], 16) for f in entry["functions"]}
    emitted = sorted((s["shndx"], s["name"]) for s in inv["symbols"]
                     if s["name"] in ordinal and isinstance(s["shndx"], int))
    order_ok = [ordinal[n] for _i, n in emitted] == sorted(ordinal[n] for _i, n in emitted)
    return rows, extra_secs, emitted, order_ok


def print_object_audit(rows, extra_secs, emitted, order_ok, entry):
    buckets = collections.Counter(r["verdict"] for r in rows)
    print(f"      defined symbols: {buckets['LICENSED']} LICENSED, "
          f"{buckets['COLLIDES-GAP']} COLLIDES-GAP (fatal: the other definition is a dsd "
          f"gap object's, which is STB_GLOBAL), {buckets['COLLIDES-SRC']} COLLIDES-SRC "
          f"(tolerated only while BOTH definitions are vague-linkage), "
          f"{buckets['HOMELESS']} HOMELESS")
    for r in sorted(rows, key=lambda r: (r["verdict"], r["name"])):
        if r["verdict"] == "LICENSED":
            continue
        detail = (f"  already defined at {', '.join(r['homes'])}" if r["homes"]
                  else "  no symbols.txt anywhere names it")
        print(f"      {r['verdict']:12} {r['name']:38} {r['bind']:10} {r['section']:8} "
              f"size 0x{r['size']:x}{detail}")
    unlicensed_secs = [s for s in extra_secs
                       if s["name"] not in {x["name"] for x in entry.get("sections", [])}]
    if unlicensed_secs:
        total = sum(s["size"] for s in unlicensed_secs)
        kinds = collections.Counter(s["name"] for s in unlicensed_secs)
        print(f"      unlicensed content sections: {len(unlicensed_secs)} "
              f"({dict(kinds)}), {total} bytes total")
    print(f"      emitted .text order of the licensed functions: "
          f"{'ROM-ascending, as required' if order_ok else 'NOT ROM-ascending'}")
    if not order_ok:
        print(f"        emitted: {[n for _i, n in emitted]}")
    return buckets


# ====================================== partial TU isolation (plan sec 9, phase D)
#
# The whole-range substitution above hands the linker ONE object for the TU's entire
# .text span. For a key-function TU that cannot work yet, and the reason is not a
# matching problem: the merged object emits its class's vtable STB_GLOBAL, dsd's
# ROM-derived gap object ALSO defines that symbol STB_GLOBAL, and mwldarm aborts
# multiply-defined. ov045/PoleLift's linkcheck records exactly that.
#
# Plan sec 9's route sidesteps vtable ownership entirely instead of solving it:
# compile the consolidated .cpp ONCE, then reduce that single object N times -- once
# per licensed function -- and hand the linker the N derived objects in the same
# positions the N per-function objects occupy today. Each reduction drops every
# section but one, so the vtable/RTTI .data is dropped and its symbols are
# externalised, exactly as objisolate already does for the per-function build. The
# collision cannot arise because no derived object defines a vtable.
#
# What that buys, and it is the whole point: the consolidated source can become
# canonical WITHOUT whole-range linking, so vtable/data ownership (phase F) stops
# being a prerequisite and becomes a later, separate improvement.

def derive_function_objects(obj_bytes, entry):
    """{symbol: (derived_bytes_or_None, objisolate plan)} for every manifest function.

    N reductions of ONE compiled object, via objisolate.derive -- the same plan() that
    decides what the per-function build keeps, drops, externalises and re-addends. This
    function adds no selection logic of its own; if it did, the derived object would
    stop being comparable to the production one, which is the only evidence that makes
    the substitution safe."""
    out = {}
    for f in sorted(entry["functions"], key=lambda x: x["ordinal"]):
        out[f["symbol"]] = OI.derive(obj_bytes, f["symbol"])
    return out


def contribution(raw, keep_symbol):
    """Everything mwldarm consumes from an object that contributes `keep_symbol`.

    Deliberately NOT the whole file. Two objects built from different sources can never
    be byte-identical as FILES -- section indices, string-table order, the symbol table's
    size and the file's length all differ -- and comparing files would make the real
    question ("does the linker see the same thing?") unanswerable. So this extracts the
    link-visible surface and nothing else:

      kept        the section the LCF's `<stem>.o(.text)` selector actually places:
                  its name, type, flags, alignment, size and bytes;
      relocs      the fixups applied to it, as (offset, type, SYMBOL NAME, addend) --
                  by name, because a symbol INDEX is a property of the object's own
                  table and means nothing to another object;
      defined     every symbol this object defines, with its binding, type, value,
                  size and the name+size of the section it lives in. A definition in a
                  zeroed section still resolves for anyone who imports it, so a
                  size-0-in-an-empty-section entry is reported rather than filtered;
      mapping     $a/$t/$d ARM EABI mapping symbols inside the kept section, kept
                  separate because they repeat by name across sections;
      live        every content section still carrying bytes -- the check that the
                  object contributes no surplus anywhere else;
      undef       imports. Split by whether a kept-section relocation names them:
                  a referenced import is load-bearing, an unreferenced one is inert
                  (the tree already ships those today -- every isolated destructor
                  object imports `_ZN<Class>D2Ev`, which nothing in the ROM defines).
    """
    elf = ELFFile(io.BytesIO(raw))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return None
    keep = None
    for s in symtab.iter_symbols():
        if (s.name == keep_symbol and s["st_shndx"] != "SHN_UNDEF"
                and s["st_info"]["type"] == "STT_FUNC"):
            keep = s["st_shndx"]
    if not isinstance(keep, int):
        return None
    ks = secs[keep]

    relocs = []
    for s in secs:
        if isinstance(s, RelocationSection) and s.header["sh_info"] == keep:
            for r in s.iter_relocations():
                sym = symtab.get_symbol(r["r_info_sym"])
                relocs.append((r["r_offset"], r["r_info_type"], sym.name,
                               r["r_addend"] if s.is_RELA() else None))

    defined, mapping, undef = {}, [], []
    for s in symtab.iter_symbols():
        if not s.name:
            continue
        shndx = s["st_shndx"]
        if shndx == "SHN_UNDEF":
            undef.append(s.name)
            continue
        secname = secs[shndx].name if isinstance(shndx, int) else str(shndx)
        secsize = secs[shndx].header["sh_size"] if isinstance(shndx, int) else -1
        row = (s["st_info"]["bind"], s["st_info"]["type"], s["st_value"], s["st_size"],
               secname, secsize)
        if s.name.startswith("$"):
            if shndx == keep:
                mapping.append((s.name, *row))
            continue
        defined[s.name] = row

    live = sorted((x.name, x.header["sh_size"], x.header["sh_addralign"],
                   x.header["sh_flags"])
                  for x in secs if x.header["sh_type"] in OI.CONTENT
                  and x.header["sh_size"]
                  and not any(x.name.startswith(p) for p in OI.IGNORE))
    referenced = {n for _o, _t, n, _a in relocs}
    return {
        "keptSection": (ks.name, ks.header["sh_type"], ks.header["sh_flags"],
                        ks.header["sh_addralign"], ks.header["sh_size"]),
        "keptIndex": keep,
        "keptBytes": ks.data(),
        "relocs": relocs,
        "defined": defined,
        "mapping": sorted(mapping),
        "live": live,
        "undefReferenced": sorted(n for n in undef if n in referenced),
        "undefInert": sorted(n for n in undef if n not in referenced),
    }


# The fields whose equality means "the linker sees the same contribution". `keptIndex`
# is excluded on purpose and is reported instead: a section's INDEX is internal to its
# own object (the derived object keeps the merged object's numbering, e.g. 28 where the
# single-file compile emitted 5), and dsd's linker script selects by section NAME.
# `undefInert` is excluded for the reason its docstring gives and is reported as a
# difference of its own.
_CONTRIB_FIELDS = ("keptSection", "keptBytes", "relocs", "defined", "mapping", "live",
                   "undefReferenced")


def compare_contribution(prod_raw, derived_raw, symbol):
    """Is the derived object's contribution identical to the production object's?

    `prod_raw` is the object the CURRENT build produces for this function: its own
    one-function source compiled by rombuild.compile_one and reduced by
    objisolate.isolate. `derived_raw` is the same function reduced out of the merged TU
    object. Returns a verdict dict; `identical` is true only when every field in
    _CONTRIB_FIELDS agrees exactly."""
    p, d = contribution(prod_raw, symbol), contribution(derived_raw, symbol)
    if p is None or d is None:
        return {"identical": False, "differences": [
            f"{symbol} is not a defined function in "
            f"{'the production object' if p is None else 'the derived object'}"]}
    diffs = []
    for k in _CONTRIB_FIELDS:
        if p[k] == d[k]:
            continue
        if k == "defined":
            for name in sorted(set(p[k]) | set(d[k])):
                if p[k].get(name) != d[k].get(name):
                    diffs.append(f"defined symbol {name}: production {p[k].get(name)} "
                                 f"vs derived {d[k].get(name)}")
        elif k == "keptBytes":
            diffs.append(f"kept .text bytes differ ({len(p[k])} vs {len(d[k])} bytes)")
        else:
            diffs.append(f"{k}: production {p[k]!r} vs derived {d[k]!r}")
    return {
        "identical": not diffs,
        "differences": diffs,
        "symbol": symbol,
        "keptSection": list(p["keptSection"]),
        "keptIndexProduction": p["keptIndex"],
        "keptIndexDerived": d["keptIndex"],
        "relocCount": len(p["relocs"]),
        "extraInertImports": sorted(set(d["undefInert"]) - set(p["undefInert"])),
        "missingInertImports": sorted(set(p["undefInert"]) - set(d["undefInert"])),
        "productionBytes": len(prod_raw),
        "derivedBytes": len(derived_raw),
    }


def unresolvable_imports(names, known=()):
    """Which of `names` no symbols.txt anywhere defines and no sibling already imports.

    An import a relocation does not name cannot change a byte, but it can still make
    mwldarm refuse the link if nothing defines it -- so the ones with no home are worth
    naming before the link rather than after. `known` is the set of imports the objects
    being REPLACED already carry: an unresolvable name that the current build already
    ships is not a new risk (`_ZN8PoleLiftD2Ev` is exactly that -- every isolated
    PoleLift destructor object imports it today and the tree links)."""
    homes = all_symbol_homes()
    return sorted(n for n in names if n not in homes and n not in set(known))


def production_objects(entry, build_root, jobs=1, cache=None):
    """Compile each legacy one-function source EXACTLY as the ROM build does.

    rombuild.compile_one is called directly -- same version pin, same flags, same
    -MD/scratch handling, same objisolate.isolate call afterwards -- so the objects
    this returns are the production comparison target by construction rather than by
    re-implementation. Returns ({symbol: path}, [errors])."""
    vers = RB.versions()
    init_srcs = RB.init_section_sources()
    syms = RB.enrolled_symbols()
    out, errors = {}, []
    rels = [(f["symbol"], f["legacy_source"]) for f in
            sorted(entry["functions"], key=lambda x: x["ordinal"])]
    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, jobs)) as ex:
        results = list(ex.map(
            lambda sr: (sr[0], sr[1], RB.compile_one(sr[1], vers, cache, init_srcs, syms,
                                                     build_root=build_root)),
            rels))
    for sym, rel, (_rel, err, _outcome) in results:
        if err:
            errors.append(f"{rel}: {err}")
            continue
        out[sym] = pathlib.Path(build_root) / pathlib.Path(rel).with_suffix(".o")
    return out, errors


def partial_report(entry, merged_bytes, derived, prod_paths):
    """[(ordinal, symbol, verdict_dict_or_error)] plus the aggregate counts."""
    rows = []
    for f in sorted(entry["functions"], key=lambda x: x["ordinal"]):
        sym = f["symbol"]
        dbytes, plan = derived.get(sym, (None, {"error": "not derived"}))
        if dbytes is None:
            rows.append((f["ordinal"], sym, {"identical": False,
                                             "differences": [f"objisolate refused: "
                                                             f"{plan.get('error')}"]}))
            continue
        ppath = prod_paths.get(sym)
        if ppath is None or not ppath.is_file():
            rows.append((f["ordinal"], sym, {"identical": False,
                                             "differences": ["no production object"]}))
            continue
        v = compare_contribution(ppath.read_bytes(), dbytes, sym)
        v["objisolateError"] = plan.get("error")
        rows.append((f["ordinal"], sym, v))
    return rows


def print_partial_rows(rows, entry):
    n_ok = sum(1 for _o, _s, v in rows if v["identical"])
    print(f"  {'ord':3} {'symbol':42} {'kept .text':18} {'reloc':>5}  verdict")
    for o, sym, v in rows:
        if "keptSection" in v:
            ks = v["keptSection"]
            kept = f"{ks[0]} size 0x{ks[4]:03x}"
            extra = (f"  (+{len(v['extraInertImports'])} inert import(s))"
                     if v.get("extraInertImports") else "")
            missing = (f"  (-{len(v['missingInertImports'])} import(s))"
                       if v.get("missingInertImports") else "")
            print(f"  [{o}] {sym:42} {kept:18} {v['relocCount']:5}  "
                  f"{'IDENTICAL' if v['identical'] else 'DIFFERS'}{extra}{missing}")
        else:
            print(f"  [{o}] {sym:42} {'-':18} {'-':>5}  DIFFERS")
        for d in v["differences"]:
            print(f"        !! {d}")
    print(f"\n  contribution equivalence: {n_ok}/{len(rows)} derived object(s) byte-identical "
          f"to the production compile+isolate contribution")
    return n_ok


# The partial-isolation lifecycle. DELIBERATELY NOT a value of `status`.
#
# plan sec 6's states (mapped -> shadow -> text-verified -> link-verified ->
# data-verified -> promoted) form one ladder, and its rungs above `text-verified` all
# mean "the whole licensed range links as one contribution". Partial isolation is a
# different axis: the source consolidates while the LINKER CONTRIBUTION stays exactly
# as split as it is today. A TU can be partial-link-verified and never become
# link-verified (a key-function TU cannot, until phase F), and a link-verified TU never
# needs partial isolation at all. Writing either state into the other's field would
# claim something untrue in both directions, so `status` keeps its ladder and this
# lives beside it in `partial_isolation.state`.
PARTIAL_STATES = {
    "derived": "N per-function objects were extracted from one compiled TU object; "
               "their equivalence to the production objects was not established",
    "contribution-equivalent": "every derived object's linker-visible contribution -- "
                               "kept section header and bytes, its relocations by symbol "
                               "name, every defined symbol, every content section still "
                               "carrying bytes, and every relocation-referenced import -- "
                               "is byte-identical to what the current per-function "
                               "compile+isolate pipeline produces for the same function",
    "partial-link-verified": "contribution-equivalent, AND a scratch link with the N "
                             "derived objects substituted at the N per-function object "
                             "paths reproduces the module (and the ROM) byte-for-byte, "
                             "with config/**/delinks.txt UNCHANGED",
}


def _record_partial(data, entry, block, state):
    """Write the partial-isolation result onto the manifest entry.

    Never touches `status`: see PARTIAL_STATES for why the two are separate axes."""
    if entry is None:
        return
    block = dict(block)
    block["state"] = state
    block["stateMeaning"] = PARTIAL_STATES[state]
    block["axisNote"] = ("Orthogonal to `status` (plan sec 6). This entry's `status` "
                         "describes whole-range linking; this block describes a source "
                         "consolidation whose linker contribution stays per-function.")
    prev = entry.get("partial_isolation") or {}
    prev.update(block)
    entry["partial_isolation"] = prev
    upsert_manifest_entry(data, entry)
    save_manifest(data)


def cmd_partial(args):
    """Extract N derived per-function objects from ONE compiled TU object and prove
    each is what the production pipeline already produces. No link, no config change."""
    data = load_manifest()
    entry = manifest_entry(data, args.id)
    if entry is None:
        raise SystemExit(f"no manifest entry for {args.id!r} in "
                         f"{MANIFEST.relative_to(REPO).as_posix()}")

    print(f"=== tubuild partial {entry['id']} (plan sec 9 / phase D) ===")
    print("Nothing under config/ or src/ is read-write here; the only writes are under "
          "build/tu/ (gitignored).\n")

    obj_bytes, version, flags, build_dir, obj_path = _compile_tu(entry, args.version)
    out_dir = build_dir / "partial"
    if out_dir.exists() and args.clean:
        shutil.rmtree(out_dir)
    (out_dir / "derived").mkdir(parents=True, exist_ok=True)

    print(f"\n[1/3] one merged object: {obj_path.relative_to(REPO).as_posix()} "
          f"({len(obj_bytes)} bytes)")
    derived = derive_function_objects(obj_bytes, entry)
    refused = {s: p.get("error") for s, (b, p) in derived.items() if b is None}
    for sym, (dbytes, _plan) in derived.items():
        if dbytes is not None:
            (out_dir / "derived" / f"{sym}.o").write_bytes(dbytes)
    print(f"      derived {sum(1 for b, _p in derived.values() if b is not None)}/"
          f"{len(derived)} per-function object(s) via objisolate.derive "
          f"-> {(out_dir / 'derived').relative_to(REPO).as_posix()}")
    for sym, err in refused.items():
        print(f"      REFUSED {sym}: {err}")

    print("\n[2/3] production comparison target: recompiling each legacy source with "
          "tools/rombuild.compile_one")
    prod_root = out_dir / "prod"
    prod_paths, errors = production_objects(entry, prod_root, jobs=args.jobs)
    for e in errors:
        print(f"      FAIL {e}")
    print(f"      {len(prod_paths)}/{len(entry['functions'])} object(s) "
          f"-> {prod_root.relative_to(REPO).as_posix()}/src/")

    print("\n[3/3] contribution comparison\n")
    rows = partial_report(entry, obj_bytes, derived, prod_paths)
    n_ok = print_partial_rows(rows, entry)

    all_extra = sorted({n for _o, _s, v in rows for n in v.get("extraInertImports", [])})
    known = set()
    for sym, p in prod_paths.items():
        if p.is_file():
            c = contribution(p.read_bytes(), sym)
            if c:
                known |= set(c["undefInert"]) | set(c["undefReferenced"])
    homeless = unresolvable_imports(all_extra, known)
    if all_extra:
        print(f"\n  {len(all_extra)} distinct EXTRA import(s) across the derived objects. "
              f"These are symbol-table entries no kept relocation names, so none can "
              f"change a byte; they exist because one merged symbol table serves all "
              f"{len(rows)} reductions. {len(known & set(all_extra))} of them are already "
              f"imported by one of the production objects being replaced.")
        print(f"  imports with no symbols.txt home anywhere AND not already imported by "
              f"the objects being replaced: {homeless or 'none'}")

    # Contribution equivalence says nothing about ENROLLMENT, and the two are easy to
    # confuse: a TU whose legacy entries are not `complete` today has production objects
    # that compile fine and are never linked, because dsd serves those ranges from ROM
    # bytes (notes/eligible-is-not-enrolled.md). Comparing against them is still
    # meaningful -- it is what the pipeline WOULD produce -- but a reader must not read
    # it as "this range is source-built". Stated rather than left implicit.
    n_complete = sum(1 for f in entry["functions"]
                     if is_complete(entry["module"], f["legacy_source"]))
    if n_complete != len(entry["functions"]):
        print(f"\n  NOTE: only {n_complete}/{len(entry['functions'])} of this TU's legacy "
              f"delinks entries carry `complete`, so the ROM build serves the rest from "
              f"ROM bytes today. The comparison above is still against what the "
              f"per-function pipeline produces, but this TU is not fully source-built and "
              f"`linkcheck --partial` will refuse it.")

    identical = n_ok == len(rows) and not refused and not errors
    print()
    if identical:
        print(f"Result: {n_ok}/{len(rows)} CONTRIBUTION-EQUIVALENT. Compiling "
              f"{entry['source']} once and reducing it {len(rows)} times reproduces, "
              f"byte-for-byte, the linker contribution of the {len(rows)} objects the "
              f"ROM build makes today.")
        print("        This is an object-level proof only. It says nothing about the "
              "LINK until `tubuild.py linkcheck --partial` runs.")
    else:
        print(f"Result: {n_ok}/{len(rows)} contribution-equivalent -> NOT proven.")

    report = {
        "id": entry["id"], "toolchain": version, "flags": flags,
        "mergedObject": obj_path.relative_to(REPO).as_posix(),
        "mergedBytes": len(obj_bytes),
        "rows": [{"ordinal": o, "symbol": s, **{k: v for k, v in vd.items()
                                                if k != "keptBytes"}}
                 for o, s, vd in rows],
        "extraInertImports": all_extra,
        "extraImportsWithNoHome": homeless,
    }
    (out_dir / "partial.json").write_text(json.dumps(report, indent=2) + "\n",
                                          encoding="utf-8", newline="\n")
    print(f"report -> {(out_dir / 'partial.json').relative_to(REPO).as_posix()} (gitignored)")

    if not args.no_record:
        _record_partial(data, entry, {
            "round": "tools/tubuild.py partial -- one compile of the consolidated source, "
                     "objisolate.derive per licensed function, compared against "
                     "rombuild.compile_one + objisolate.isolate of each legacy source",
            "derivedObjects": len(rows),
            "contributionEquivalent": f"{n_ok}/{len(rows)}",
            "comparedFields": list(_CONTRIB_FIELDS),
            "legacyEntriesComplete": f"{n_complete}/{len(entry['functions'])}",
            "extraInertImports": len(all_extra),
            "extraImportsWithNoHome": homeless,
            "artefacts": (out_dir / "partial.json").relative_to(REPO).as_posix()
                         + " (gitignored)",
        }, "contribution-equivalent" if identical else "derived")
    return 0 if identical else 1


class ReadOnlyObjectCache(RBK.ObjectCache):
    """rombuild's object cache, used for hits only.

    A scratch link needs every one of the ~10,800 enrolled sources compiled, and the
    tree's warm cache turns that from minutes of mwccarm into a file copy. Writing to
    it would be harmless in principle -- entries are content-addressed, so a scratch
    build stores exactly what a real build would -- but the assignment's rule is that
    a linkcheck touches nothing under build/ outside its own scratch subtree, and an
    LRU `prune` from a throwaway run could evict a real build's entries. So `put` and
    `prune` are no-ops and the -MD scratch directory moves into the TU's own tree.
    The one remaining write is fetch()'s `os.utime` on a served object, which only
    refreshes that entry's LRU timestamp."""

    def __init__(self, root, repo, scratch_dir, enabled=True):
        self._scratch = pathlib.Path(scratch_dir)
        self._scratch.mkdir(parents=True, exist_ok=True)
        super().__init__(root, repo, enabled=enabled)

    @property
    def scratch(self):
        return self._scratch

    def put(self, *_a, **_k):
        return None

    def prune(self, *_a, **_k):
        return 0


def _run_dsd(cmd, what, quiet_patterns=()):
    """rombuild.run, but returning the failure instead of raising, so linkcheck can
    report a phase failure as a linkcheck verdict rather than a traceback."""
    t0 = time.time()
    try:
        out = RB.run(cmd, what, quiet_patterns=quiet_patterns)
        return True, out, time.time() - t0
    except RB.BuildError as exc:
        return False, exc.output, time.time() - t0


def module_dir_in(config_root, module):
    if module == "arm9":
        return pathlib.Path(config_root)
    if module in ("itcm", "dtcm"):
        return pathlib.Path(config_root) / module
    return pathlib.Path(config_root) / "overlays" / module


def module_base(delinks_path):
    header, _entries = EN.read_delinks(delinks_path)
    secs = EN.sections(header)
    return min(s[1] for s in secs) if secs else None


def compare_range(built, retail, base, start, end, label, rows=None):
    """Byte-compare one linked address range. Returns (ok, ndiff, first_diff_addr)."""
    lo, hi = start - base, end - base
    if lo < 0 or hi > len(built) or hi > len(retail):
        print(f"  {label}: RANGE OUT OF BOUNDS (built {len(built)} bytes, "
              f"retail {len(retail)} bytes, want [{lo}, {hi}))")
        return False, hi - lo, start
    b, r = built[lo:hi], retail[lo:hi]
    diffs = [i for i in range(hi - lo) if b[i] != r[i]]
    if not diffs:
        print(f"  {label}: 0x{start:08x}..0x{end:08x}  {hi - lo} bytes  IDENTICAL")
        return True, 0, None
    print(f"  {label}: 0x{start:08x}..0x{end:08x}  {hi - lo} bytes  "
          f"{len(diffs)} DIFFER, first at 0x{base + lo + diffs[0]:08x}")
    return False, len(diffs), base + lo + diffs[0]


def cmd_linkcheck(args):
    data = load_manifest()
    entry = manifest_entry(data, args.id) if args.id else None
    if args.id and entry is None:
        raise SystemExit(f"no manifest entry for {args.id!r} in "
                         f"{MANIFEST.relative_to(REPO).as_posix()}")
    baseline = bool(args.baseline)
    partial = bool(getattr(args, "partial", False))
    if entry is None and not baseline:
        raise SystemExit("linkcheck needs a TU id (or --baseline for the control run)")
    if partial and baseline:
        raise SystemExit("--partial and --baseline are different runs: the baseline "
                         "control substitutes nothing at all, so there is no partial "
                         "variant of it. Run them separately.")

    module = entry["module"] if entry else (args.module or "ov002")
    tu_id = entry["id"] if entry else "_baseline"
    # One canonical baseline tree for the whole repository, not one per module: the
    # control run substitutes nothing, so its result is module-independent and every
    # TU run diffs against the same artefact. The partial run gets its own tree beside
    # the whole-range one so a TU can hold both results at once -- they are different
    # experiments on the same source and neither supersedes the other.
    scratch = (BASELINE_LINK if baseline
               else BUILD_TU / sanitize_id(tu_id) / ("link-partial" if partial else "link"))
    cfg_root = scratch / "config" / "arm9"
    report = {"id": tu_id, "baseline": baseline, "partial": partial, "module": module,
              "scratch": scratch.relative_to(REPO).as_posix(), "phases": {}}

    print(f"=== tubuild linkcheck {tu_id}"
          f"{'  [BASELINE CONTROL -- no TU substitution]' if baseline else ''}"
          f"{'  [PARTIAL -- N derived per-function objects, delinks.txt UNCHANGED]' if partial else ''} ===")
    print(f"scratch tree: {scratch.relative_to(REPO).as_posix()}")
    print("real config/ and the shared build/ outputs are read-only for this command; "
          "everything written below lives under that scratch tree (build/ is gitignored).\n")

    for tool in (RB.DSD, RB.MW / RB.VERSION / "mwccarm.exe",
                 RB.MW / RB.LD_VERSION / "mwldarm.exe"):
        if not tool.is_file():
            raise SystemExit(f"missing {tool} - see notes/setup-mwccarm.md")
    if not (REPO / "extracted" / "dsd" / "config.yaml").is_file():
        raise SystemExit("no extracted ROM - run tools/unpack.py on your own dump first")

    # ---------------------------------------------------------------- scratch config
    print("[1/8] scratch configuration")
    if scratch.exists() and args.clean:
        shutil.rmtree(scratch)
    scratch.mkdir(parents=True, exist_ok=True)
    profile = RP.prepare_profile("stock", out_root=scratch / "config", build_root=scratch)
    print(f"      copied config/arm9 -> {cfg_root.relative_to(REPO).as_posix()} "
          f"({len(profile['modReplacements'])} mod entr(y/ies) redirected to src/, "
          f"{len(profile['modGapFallbacks'])} demoted to ROM gap bytes -- "
          f"rombuild_profile.prepare_profile, the same stock semantics as a real build)")

    span_start = span_end = None
    replaced = []
    if not baseline:
        text_secs = [s for s in entry.get("sections", []) if s["name"] == ".text"]
        if len(text_secs) != 1:
            raise SystemExit(f"{tu_id}: expected exactly one licensed .text section, "
                             f"got {len(text_secs)}")
        span_start = int(text_secs[0]["start"], 16)
        span_end = int(text_secs[0]["end"], 16)
        dl = module_dir_in(cfg_root, module) / "delinks.txt"
        if not dl.is_file():
            raise SystemExit(f"no delinks.txt for module {module} at {dl}")
        legacy_rels = [f["legacy_source"] for f in entry["functions"]]
        if partial:
            # No splice. The whole point of partial isolation is that the CONFIG does
            # not move: the same N entries, the same N object paths, the same tiling of
            # the span -- only the source that produced those objects' bytes changes.
            # The span checks still run, because substituting objects under entries
            # that do not tile the span, or that dsd currently serves from ROM bytes,
            # would be exactly as silent a failure as a bad splice.
            _h, _e, inside, reasons = span_entries(dl, span_start, span_end, legacy_rels)
            if reasons:
                print("\nREFUSED -- the per-function object substitution is not safe:")
                for r in reasons:
                    print(f"  {r}")
                return 1
            replaced = sorted(rel for _i, rel, _s in inside)
            print(f"      delinks.txt NOT modified. {len(replaced)} per-function entr(y/ies) "
                  f"tile 0x{span_start:08x}..0x{span_end:08x}, all `complete`, and each "
                  f"keeps its own object path; the substitution happens at the OBJECT "
                  f"level in [4b] below.")
        else:
            replaced, reasons = splice_tu_entry(
                dl, span_start, span_end, entry["source"], legacy_rels)
            if reasons:
                print("\nREFUSED -- the scratch delinks substitution is not safe:")
                for r in reasons:
                    print(f"  {r}")
                return 1
            print(f"      spliced {len(replaced)} per-function entr(y/ies) in "
                  f"{module}/delinks.txt into one:")
            print(f"        {entry['source']}:  .text start:0x{span_start:08x} "
                  f"end:0x{span_end:08x}  complete")

    # dsd's linker script selects contributions by object BASENAME, so a shadow TU
    # whose stem collides with any other enrolled or delinked object would be
    # mislinked silently. Checked here rather than assumed. Not applicable in partial
    # mode: no new object basename enters the link there.
    if not baseline and not partial:
        stem = pathlib.Path(entry["source"]).stem
        clash = [p for p in RB.enrolled(cfg_root, extra_roots=("src_tu",))
                 if pathlib.PurePosixPath(p).stem == stem and p != entry["source"]]
        if clash:
            print(f"\nREFUSED -- object basename {stem}.o is claimed by {clash}")
            return 1

    # ------------------------------------------------------------------ dsd delink/lcf
    print("[2/8] dsd delink")
    ok, out, dt = _run_dsd([str(RB.DSD), "delink", "-c", str(profile["configYaml"])],
                           "dsd delink", quiet_patterns=("No module for relocation",))
    report["phases"]["delink"] = {"ok": ok, "seconds": round(dt, 1)}
    if not ok:
        print(out[:4000])
        return 1
    print(f"      ok ({dt:.1f}s)")

    print("[3/8] dsd lcf")
    ok, out, dt = _run_dsd([str(RB.DSD), "lcf", "-c", str(profile["configYaml"])], "dsd lcf")
    report["phases"]["lcf"] = {"ok": ok, "seconds": round(dt, 1)}
    if not ok:
        print(out[:4000])
        return 1
    print(f"      ok ({dt:.1f}s) -> {(scratch / 'arm9.lcf').relative_to(REPO).as_posix()}, "
          f"{(scratch / 'objects.txt').relative_to(REPO).as_posix()}")

    # ------------------------------------------------------------------------ compile
    srcs = RB.enrolled(cfg_root, extra_roots=("src_tu",))
    vers = RB.versions()
    if partial:
        pin, note = resolve_tu_version(entry)
        print(f"[4/8] mwccarm: {len(srcs)} enrolled source file(s), -j{args.jobs}")
        print(f"      the shadow TU is NOT one of them -- it is not in delinks.txt in this "
              f"mode. It is compiled once in [4b] at pin {pin or '?'}"
              f"{' (' + note + ')' if note else ''}, and the {len(entry['functions'])} "
              f"per-function objects the build is about to produce are then replaced by "
              f"reductions of it.")
    elif not baseline:
        pin, note = resolve_tu_version(entry)
        used = vers.get(pathlib.Path(entry["source"]).stem, RB.VERSION)
        print(f"[4/8] mwccarm: {len(srcs)} enrolled source file(s), -j{args.jobs}")
        print(f"      shadow TU pin: legacy sources agree on {pin or '?'}"
              f"{' (' + note + ')' if note else ''}; the scratch build compiles "
              f"{entry['source']} as {used} (rombuild keys the version override on the "
              f"FILE STEM, so a promoted TU would need its own "
              f"config/rombuild-versions.txt line if it ever diverges)")
        if pin and pin != used:
            print(f"      REFUSED -- pin disagreement: legacy {pin} vs scratch build {used}")
            return 1
    else:
        print(f"[4/8] mwccarm: {len(srcs)} enrolled source file(s), -j{args.jobs}")

    cache = ReadOnlyObjectCache(REPO / "build" / "objcache", REPO, scratch / "cachetmp",
                                enabled=not args.no_cache)
    init_srcs = RB.init_section_sources()
    syms = RB.enrolled_symbols()
    t0 = time.time()
    failures, outcomes = [], collections.Counter()
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, err, outcome in ex.map(
                lambda s: RB.compile_one(s, vers, cache, init_srcs, syms, build_root=scratch),
                srcs):
            outcomes[outcome] += 1
            if err:
                failures.append((rel, err))
    dt = time.time() - t0
    report["phases"]["compile"] = {"ok": not failures, "seconds": round(dt, 1),
                                   "outcomes": dict(outcomes)}
    print(f"      {outcomes['hit']} reused from cache, "
          f"{outcomes['miss'] + outcomes['uncacheable']} compiled, "
          f"{outcomes['error']} failed ({dt:.1f}s)")
    if failures:
        for rel, err in failures[:10]:
            print(f"      FAIL {rel}: {err}")
        return 1

    # ------------------------------- partial isolation: derive, compare, substitute
    partial_rows = []
    if partial:
        print("[4b/8] partial isolation: one TU compile -> N derived objects, substituted "
              "for the N objects [4/8] just produced")
        obj_bytes, version, tu_flags, _bd, tu_obj_path = _compile_tu(entry)
        derived = derive_function_objects(obj_bytes, entry)
        keep = scratch / "partial"
        (keep / "production").mkdir(parents=True, exist_ok=True)
        (keep / "derived").mkdir(parents=True, exist_ok=True)
        prod_paths, substituted = {}, []
        for f in sorted(entry["functions"], key=lambda x: x["ordinal"]):
            sym, rel = f["symbol"], f["legacy_source"]
            obj = scratch / pathlib.Path(rel).with_suffix(".o")
            if not obj.is_file():
                print(f"      !! the build produced no object at {obj} for {rel}")
                return 1
            # The production object is preserved before it is overwritten -- it is the
            # comparison target AND the thing to restore if this run is ever re-read.
            saved = keep / "production" / f"{sym}.o"
            saved.write_bytes(obj.read_bytes())
            prod_paths[sym] = saved
            dbytes, _plan = derived.get(sym, (None, {}))
            if dbytes is None:
                continue
            (keep / "derived" / f"{sym}.o").write_bytes(dbytes)
            obj.write_bytes(dbytes)
            substituted.append(rel)
        partial_rows = partial_report(entry, obj_bytes, derived, prod_paths)
        print(f"      merged object: {tu_obj_path.relative_to(REPO).as_posix()} "
              f"({len(obj_bytes)} bytes, {version})")
        n_ok = print_partial_rows(partial_rows, entry)
        print(f"      substituted {len(substituted)}/{len(entry['functions'])} object(s) "
              f"in place under {scratch.relative_to(REPO).as_posix()}/src/")
        extra = sorted({n for _o, _s, v in partial_rows
                        for n in v.get("extraInertImports", [])})
        known = set()
        for sym, p in prod_paths.items():
            c = contribution(p.read_bytes(), sym)
            if c:
                known |= set(c["undefInert"]) | set(c["undefReferenced"])
        homeless = unresolvable_imports(extra, known)
        print(f"      {len(extra)} extra inert import(s) across the derived objects; "
              f"{len(homeless)} with no symbols.txt home and not already imported by the "
              f"objects being replaced{': ' + ', '.join(homeless) if homeless else ''}")
        report["partial"] = {
            "mergedObject": tu_obj_path.relative_to(REPO).as_posix(),
            "mergedBytes": len(obj_bytes), "toolchain": version, "flags": tu_flags,
            "contributionEquivalent": f"{n_ok}/{len(partial_rows)}",
            "rows": [{"ordinal": o, "symbol": s,
                      **{k: v for k, v in vd.items() if k != "keptBytes"}}
                     for o, s, vd in partial_rows],
            "extraInertImports": extra, "extraImportsWithNoHome": homeless,
            "substituted": substituted,
        }
        if n_ok != len(partial_rows):
            print(f"      NOTE: {len(partial_rows) - n_ok} derived object(s) are NOT "
                  f"contribution-equivalent. The link below still runs, but a green module "
                  f"would then be evidence about the LINK, not about equivalence.")
        if homeless:
            print("      PREDICTION: an import with no definition anywhere can make "
                  "mwldarm refuse the link. Running it so the verdict is measured.")

    # ------------------------------------------------- pre-link licensing audit
    if not baseline and not partial:
        print("[4b/8] licensing audit of the shadow object (plan sec 4.5), before the link")
        tu_obj = scratch / pathlib.Path(entry["source"]).with_suffix(".o")
        if not tu_obj.is_file():
            print(f"      !! no object at {tu_obj}")
            return 1
        rows, extra_secs, emitted, order_ok = audit_tu_object(
            tu_obj.read_bytes(), entry, span_start, span_end, complete_ranges(cfg_root))
        buckets = print_object_audit(rows, extra_secs, emitted, order_ok, entry)
        report["objectAudit"] = {
            "counts": dict(buckets), "orderOk": order_ok,
            "nonLicensed": [r for r in rows if r["verdict"] != "LICENSED"],
            "unlicensedSections": [{"name": s["name"], "size": s["size"]}
                                   for s in extra_secs],
        }
        homeless_strong = [r for r in rows
                           if r["verdict"] == "HOMELESS" and r["bind"] == "STB_GLOBAL"]
        if buckets["COLLIDES-GAP"]:
            print(f"      PREDICTION: mwldarm will abort with {buckets['COLLIDES-GAP']} "
                  f"multiply-defined symbol(s). Running the link anyway so the verdict is "
                  f"measured rather than asserted.")
        if homeless_strong:
            print(f"      PREDICTION: {len(homeless_strong)} STB_GLOBAL symbol(s) with no "
                  f"licensed address ("
                  f"{', '.join(r['name'] + ' ' + hex(r['size']) for r in homeless_strong)}"
                  f") cannot be deadstripped under `-nodead`, so they take space inside "
                  f"the TU's own linked contribution.")
        if not order_ok:
            print("      PREDICTION: the licensed functions are NOT emitted in ROM address "
                  "order, so the linked range cannot reproduce even if everything above "
                  "were resolved.")

    # -------------------------------------------------------------------------- link
    print("[5/8] mwldarm (scratch module link)")
    ok, out, dt = _run_dsd([*RB.launcher(), str(RB.MW / RB.LD_VERSION / "mwldarm.exe"),
                            *RB.LDFLAGS.split(), f"@{scratch / 'objects.txt'}",
                            str(scratch / "arm9.lcf"), "-o", str(scratch / "final_link.o")],
                           "mwldarm")
    report["phases"]["link"] = {"ok": ok, "seconds": round(dt, 1)}
    if not ok:
        print(f"      LINK FAILED ({dt:.1f}s):")
        print(out[:4000])
        report["phases"]["link"]["output"] = out[:4000]
        report["result"] = "link-failed"
        _write_link_report(scratch, report)
        _record_linkcheck(data, entry, report, baseline)
        return 1
    print(f"      ok ({dt:.1f}s) -> {(scratch / 'final_link.o').relative_to(REPO).as_posix()}")

    stray = sorted(p.name for p in (REPO / "build").glob("*.bin"))
    if stray:
        print(f"      !! module images appeared at build/*.bin: {stray} -- the scratch "
              f"redirection leaked; investigate before trusting this run")
        report["strayOutputs"] = stray

    # ------------------------------------------------------- linked-range comparison
    print("[6/8] linked range + module byte comparison")
    mdir = module_dir_in(cfg_root, module)
    built_p, retail_p = RBC.module_binaries(mdir, cfg_root, scratch / "build")
    range_ok = True
    if not built_p or not built_p.is_file():
        print(f"      no built module image at {built_p}")
        range_ok = False
    else:
        built, retail = built_p.read_bytes(), retail_p.read_bytes()
        base = module_base(mdir / "delinks.txt")
        print(f"      module {module}: built {len(built)} bytes, retail {len(retail)} bytes, "
              f"base 0x{base:08x}")
        if not baseline:
            range_ok, ndiff, first = compare_range(built, retail, base, span_start,
                                                   span_end, f"TU range {tu_id}")
            report["tuRange"] = {"start": f"0x{span_start:08x}", "end": f"0x{span_end:08x}",
                                 "differingBytes": ndiff,
                                 "firstDiff": f"0x{first:08x}" if first else None}
            print("      per-declared-function, inside the linked range:")
            for f in sorted(entry["functions"], key=lambda x: int(x["address"], 16)):
                a, s = int(f["address"], 16), int(f["size"], 16)
                lo, hi = a - base, a - base + s
                nd = sum(1 for x, y in zip(built[lo:hi], retail[lo:hi]) if x != y) \
                    if 0 <= lo and hi <= min(len(built), len(retail)) else s
                print(f"        {'MATCH' if nd == 0 else 'DIFF '}  {f['symbol']:52} "
                      f"0x{a:08x} size 0x{s:03x}"
                      f"{'' if nd == 0 else f'  {nd} byte(s) differ'}")

    analysis = RBC.analyze(cfg_root, "stock", scratch / "build")
    RBC.print_report(analysis)
    report["analysis"] = {k: analysis[k] for k in
                          ("passed", "moduleFidelity", "sourceBuild", "missingModuleBinaries")}
    report["analysis"]["failures"] = analysis["failures"][:40]
    bad_modules = [m["module"] for m in analysis["moduleFidelity"]["results"] if not m["exact"]]
    if bad_modules:
        print(f"      modules NOT byte-exact: {bad_modules}")

    ok, out, dt = _run_dsd([str(RB.DSD), "check", "modules", "-c",
                            str(profile["configYaml"]), "-f"], "dsd check modules")
    report["phases"]["checkModules"] = {"ok": ok, "seconds": round(dt, 1)}
    print(f"      dsd check modules --fail: {'PASS' if ok else 'FAIL'}")
    if not ok:
        print("      " + "\n      ".join(out.splitlines()[-12:]))

    # -------------------------------------------------------------- dsd check symbols
    print("[7/8] dsd check symbols --fail")
    ok, out, dt = _run_dsd([str(RB.DSD), "check", "symbols", "-c", str(profile["configYaml"]),
                            "-e", str(scratch / "final_link.o"), "-f", "-m", "12"],
                           "dsd check symbols")
    errors = sorted({l.strip() for l in out.splitlines() if "[ERROR]" in l})
    report["phases"]["checkSymbols"] = {"ok": ok, "seconds": round(dt, 1),
                                        "errors": errors, "output": out[-4000:]}
    print(f"      {'PASS' if ok else 'FAIL'} ({dt:.1f}s), {len(errors)} error line(s)")
    tail = [l for l in out.splitlines() if l.strip()]
    for line in tail[-25:]:
        print(f"      | {line}")
    symbols_ok = ok
    # rombuild.py has never run this check, and the baseline control shows why: it does
    # not pass on an untouched tree. So the gate that can mean something for a TU is
    # "no error this run that the baseline did not already have" -- the same
    # reproduce-on-main discipline notes/stale-baseline-gates.md insists on.
    base_errors, symbols_new = None, None
    if not baseline:
        base = BASELINE_LINK / "linkcheck.json"
        if base.is_file():
            try:
                base_errors = json.loads(base.read_text(encoding="utf-8")) \
                    ["phases"]["checkSymbols"]["errors"]
            except (ValueError, KeyError, TypeError):
                base_errors = None
        if base_errors is None:
            print("      no baseline symbol-check artefact at "
                  f"{base.relative_to(REPO).as_posix()}; run `linkcheck --baseline` first "
                  f"or this check cannot distinguish a pre-existing failure from yours")
        else:
            symbols_new = [e for e in errors if e not in set(base_errors)]
            gone = [e for e in base_errors if e not in set(errors)]
            print(f"      vs baseline ({len(base_errors)} pre-existing error line(s)): "
                  f"{len(symbols_new)} NEW, {len(gone)} resolved")
            for e in symbols_new[:15]:
                print(f"      NEW | {e}")
    report["symbolsNew"] = symbols_new

    # ------------------------------------------------------------------- ROM build
    module_ok = bool(analysis["passed"]) and range_ok and not bad_modules
    rom_ok = None
    if args.no_rom:
        print("[8/8] full ROM build skipped (--no-rom)")
    elif not module_ok:
        print("[8/8] full ROM build skipped -- the module did not pass (plan sec 7.6 runs "
              "it only 'when the module passes')")
    else:
        print("[8/8] dsd rom config + rom build")
        rom_out = scratch / "sm64ds-tu.nds"
        ok, out, dt = _run_dsd([str(RB.DSD), "rom", "config", "--elf",
                                str(scratch / "final_link.o"), "--config",
                                str(profile["configYaml"])], "dsd rom config")
        if ok:
            ok, out, dt2 = _run_dsd([str(RB.DSD), "rom", "build", "--config",
                                     str(scratch / "build" / "rom_config.yaml"),
                                     "--rom", str(rom_out)], "dsd rom build",
                                    quiet_patterns=("Compressing arm9 overlay",))
            dt += dt2
        rom_ok = ok
        report["phases"]["rom"] = {"ok": ok, "seconds": round(dt, 1)}
        if ok and rom_out.is_file():
            digest = hashlib.sha256(rom_out.read_bytes()).hexdigest()
            print(f"      ok ({dt:.1f}s) -> {rom_out.relative_to(REPO).as_posix()}  "
                  f"{rom_out.stat().st_size} bytes  sha256 {digest}")
            report["rom"] = {"bytes": rom_out.stat().st_size, "sha256": digest}
            stock = REPO / "build" / "sm64ds.nds"
            if stock.is_file():
                same = hashlib.sha256(stock.read_bytes()).hexdigest() == digest
                print(f"      vs this tree's last stock build/sm64ds.nds: "
                      f"{'IDENTICAL' if same else 'DIFFERS'}")
                report["rom"]["matchesStockRom"] = same
        else:
            print(f"      FAILED ({dt:.1f}s)")
            print("      " + "\n      ".join(out.splitlines()[-15:]))

    # ------------------------------------------------------------------------ verdict
    if baseline:
        symbols_verdict = symbols_ok
    elif symbols_new is not None:
        symbols_verdict = not symbols_new
    else:
        symbols_verdict = symbols_ok
    equivalent = all(v["identical"] for _o, _s, v in partial_rows) if partial_rows else False
    verified = bool(module_ok and symbols_verdict and (rom_ok is not False))
    if partial:
        # A partial run's claim is narrower than link-verified's and must not borrow its
        # name: the TU's whole .text range still comes from N objects, not one.
        verified = bool(verified and equivalent and partial_rows)
        report["result"] = "partial-link-verified" if verified else "failed"
    else:
        report["result"] = "link-verified" if verified else "failed"
    print()
    if partial:
        n_ok = sum(1 for _o, _s, v in partial_rows if v["identical"])
        if verified:
            print(f"Result: PARTIAL-LINK-VERIFIED. {entry['source']} was compiled ONCE; "
                  f"{n_ok}/{len(partial_rows)} derived per-function objects are "
                  f"byte-identical in linker contribution to the ones the current build "
                  f"produces, and with all {len(partial_rows)} substituted at their "
                  f"existing object paths the module and the TU range "
                  f"0x{span_start:08x}..0x{span_end:08x} reproduce"
                  f"{', and the full ROM builds' if rom_ok else ''}.")
            print("        config/**/delinks.txt was NOT changed: the same N entries, the "
                  "same N object paths, the same enrolled function count. Only the SOURCE "
                  "that produced those objects' bytes changed.")
            print("        This does NOT make the TU link-verified (plan sec 6): the whole "
                  "range is still linked from N contributions, and the vtable/RTTI "
                  "ownership question is untouched -- it is deferred, not answered.")
        else:
            print(f"Result: NOT partial-link-verified "
                  f"({n_ok}/{len(partial_rows)} contribution-equivalent; see the phases "
                  f"above).")
    elif baseline:
        print(f"BASELINE CONTROL: modules {'PASS' if module_ok else 'FAIL'}, "
              f"dsd check symbols --fail {'PASS' if symbols_ok else 'FAIL'}, "
              f"ROM {'built' if rom_ok else 'not built' if rom_ok is False else 'skipped'}.")
        print("This is the same scratch pipeline with NO TU substitution: anything failing "
              "here is pre-existing and belongs to the tree, not to any TU.")
    elif verified:
        if symbols_ok:
            sym_phrase = "dsd check symbols --fail passes"
        elif symbols_new is not None:
            sym_phrase = (f"dsd check symbols --fail still FAILS on {len(errors)} "
                          f"pre-existing error(s) that the baseline control also has "
                          f"(0 new, so not attributable to this TU -- but the check is "
                          f"NOT green and cannot be reported as such)")
        else:
            sym_phrase = "dsd check symbols --fail FAILED with no baseline to compare to"
        print(f"Result: LINK-VERIFIED. The whole {tu_id} .text range "
              f"0x{span_start:08x}..0x{span_end:08x} reproduces from one object, every "
              f"module is byte-exact, {sym_phrase}"
              f"{', and the full ROM builds' if rom_ok else ''}.")
    else:
        print("Result: NOT link-verified (see the phases above).")
    _write_link_report(scratch, report)
    if partial:
        p = report["partial"]
        _record_partial(data, entry, {
            "round": "tools/tubuild.py linkcheck --partial -- one compile of the "
                     "consolidated source, objisolate.derive per licensed function, each "
                     "derived object substituted for the production per-function object at "
                     "its own existing path, then scratch dsd delink+lcf, whole-tree "
                     "mwccarm, mwldarm link, linked-range and module byte comparison, dsd "
                     "check symbols --fail, full ROM build",
            "delinksChanged": False,
            "derivedObjects": len(partial_rows),
            "contributionEquivalent": p["contributionEquivalent"],
            "comparedFields": list(_CONTRIB_FIELDS),
            "substitutedObjectPaths": p["substituted"],
            "extraInertImports": len(p["extraInertImports"]),
            "extraImportsWithNoHome": p["extraImportsWithNoHome"],
            "result": report["result"],
            "phases": {k: v.get("ok") for k, v in report["phases"].items()},
            "tuRange": report.get("tuRange"),
            "moduleFidelityPassed": bool(module_ok),
            "symbolCheckNewVsBaseline": report.get("symbolsNew"),
            "rom": report.get("rom"),
            "scratch": report["scratch"] + " (gitignored)",
        }, "partial-link-verified" if verified else
           ("contribution-equivalent" if equivalent else "derived"))
    else:
        _record_linkcheck(data, entry, report, baseline)
    return 0 if verified else 1


def _write_link_report(scratch, report):
    path = scratch / "linkcheck.json"
    path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8", newline="\n")
    print(f"report -> {path.relative_to(REPO).as_posix()} (gitignored)")


def _record_linkcheck(data, entry, report, baseline):
    """Record the run on the manifest entry. Promotes shadow->link-verified on a pass;
    NEVER downgrades an earned status on a failure -- a linkcheck failure says nothing
    about whether the text-only result still holds, and pilot #1's report predicted its
    own linkcheck failure while its 7/7 text result stood."""
    if baseline or entry is None:
        return
    audit = report.get("objectAudit") or {}
    entry.setdefault("verification", {})["linkcheck"] = {
        "round": "tools/tubuild.py linkcheck -- scratch dsd delink+lcf, whole-tree mwccarm, "
                 "mwldarm link, linked-range and module byte comparison, dsd check symbols "
                 "--fail, full ROM build",
        "result": report["result"],
        "scratch": report["scratch"] + " (gitignored)",
        "phases": {k: v.get("ok") for k, v in report["phases"].items()},
        "tuRange": report.get("tuRange"),
        "objectAudit": {
            "counts": audit.get("counts"),
            "emittedTextOrderIsRomAscending": audit.get("orderOk"),
            "nonLicensedSymbols": [
                f"{r['verdict']} {r['name']} {r['bind']} {r['section']} size=0x{r['size']:x}"
                + (f" already at {', '.join(r['homes'])}" if r["homes"] else "")
                for r in audit.get("nonLicensed", [])],
            "unlicensedSections": audit.get("unlicensedSections"),
        },
        "symbolCheckNewVsBaseline": report.get("symbolsNew"),
        "linkerOutput": report["phases"].get("link", {}).get("output"),
    }
    if report["result"] == "link-verified" and entry.get("status") == "text-verified":
        entry["status"] = "link-verified"
    save_manifest(data)


# ==================================================================== `promote --dry-run`

# Tracked files a promotion may have to follow, checked for both the legacy source
# PATHS and their file STEMS. Scanned rather than assumed, and split by what a hit
# MEANS: a live-state file has to be edited so the gate keyed on it keeps working,
# while the two provenance logs are address-keyed history whose records must survive
# a promotion with only their `srcPath` field retargeted (tools/cpp_rename.py does
# exactly that for renames) -- deleting a record would erase who matched a function.
_PROMOTE_TRACKERS_LIVE = (
    "config/rombuild-versions.txt", "config/rombuild-exclude.txt",
    "config/layout-known-issues.txt", "config/converted-baseline.json",
    "config/unresolved-baseline.json", "config/port_linkage.json",
    "attribution.json", "contributions.json", "langmode-baseline.json",
    "stranding-baseline.json", "CLAIMS.md",
)
_PROMOTE_TRACKERS_HISTORY = (
    "config/match_provenance.jsonl", "config/match_attempts.jsonl",
)


def cmd_promote(args):
    if not args.dry_run:
        print("tools/tubuild.py promote: only --dry-run is implemented.\n"
              "The mutating path deletes enrolled src/ files and edits tracked\n"
              "config/**/delinks.txt, which plan sec 7.7 requires to be an explicit,\n"
              "reviewable change; it is deliberately not available yet. Re-run with\n"
              "--dry-run to see exactly what it would do.")
        return 2

    data = load_manifest()
    entry = manifest_entry(data, args.id)
    if entry is None:
        raise SystemExit(f"no manifest entry for {args.id!r}")

    status = entry.get("status")
    ok_status = status in ("link-verified", "data-verified")
    module = entry["module"]
    src = REPO / entry["source"]
    dest_rel = entry.get("promoted_source")
    dest = REPO / dest_rel if dest_rel else None
    text_secs = [s for s in entry.get("sections", []) if s["name"] == ".text"]
    span = (int(text_secs[0]["start"], 16), int(text_secs[0]["end"], 16)) if text_secs else None
    legacy = [f["legacy_source"] for f in entry["functions"]]

    print(f"=== tubuild promote --dry-run {entry['id']} ===")
    print("NOTHING IS WRITTEN BY THIS COMMAND.\n")
    print(f"manifest status        : {status}"
          f"{'' if ok_status else '   <<< promotion would be REFUSED (plan sec 7.7 requires link-verified or better)'}")
    print(f"module                 : {module}")
    if span:
        print(f"licensed .text         : 0x{span[0]:08x}..0x{span[1]:08x} "
              f"({span[1] - span[0]} bytes, {len(entry['functions'])} function(s))")

    print("\n-- 1. move the shadow source to its permanent path")
    print(f"   git mv {entry['source']}  ->  {dest_rel}")
    print(f"      source exists       : {src.is_file()}")
    if dest is not None:
        print(f"      destination exists  : {dest.is_file()}"
              f"{'   <<< COLLISION' if dest.is_file() else ''}")
        print(f"      destination dir     : {dest.parent.relative_to(REPO).as_posix()}"
              f"{'  (exists)' if dest.parent.is_dir() else '  (WOULD BE CREATED -- new src/ subdirectory)'}")
        stem = dest.stem
        others = [p.relative_to(REPO).as_posix() for p in SP.iter_sources()
                  if p.stem == stem and p != dest]
        print(f"      layout_check L2 (stem {stem!r} unique under src/): "
              f"{'OK' if not others else 'VIOLATION ' + str(others)}")
        cls = SP.class_of(stem)
        print(f"      layout_check L4 (srcpath.class_of({stem!r}) = {cls!r}): "
              f"{'not a class-named stem, no split-class risk' if cls is None else 'check other homes for ' + cls}")

    print(f"\n-- 2. delete the {len(legacy)} superseded one-function source(s)")
    for rel in legacy:
        p = REPO / rel
        print(f"   git rm {rel}"
              f"{'' if p.is_file() else '   <<< ALREADY ABSENT'}")

    print(f"\n-- 3. edit config/arm9/{'overlays/' + module if module.startswith('ov') else module}/delinks.txt")
    dl = module_dir_in(CFG_ARM9, module) / "delinks.txt"
    print(f"   file: {dl.relative_to(REPO).as_posix()}")
    if span:
        _, entries = parse_delinks_file(dl)
        inside = []
        for rel, body in entries:
            secs = entry_sections(body)
            if secs and span[0] <= min(s[1] for s in secs) < span[1]:
                inside.append((rel, secs, entry_is_complete(body)))
        print(f"   REMOVE {len(inside)} entr(y/ies):")
        for rel, secs, comp in inside:
            rng = ", ".join(f"{n} 0x{a:08x}..0x{b:08x}" for n, a, b in secs)
            print(f"     - {rel}   [{rng}]{'  complete' if comp else '  (NOT complete)'}")
        print("   INSERT 1 entry in their place:")
        print(f"     + {dest_rel}:")
        print("     +     complete")
        print(f"     +     .text start:0x{span[0]:08x} end:0x{span[1]:08x}")
        stale = [rel for rel, _s, _c in inside if rel not in legacy]
        missing = [rel for rel in legacy if rel not in {r for r, _s, _c in inside}]
        if stale or missing:
            print(f"   !! manifest/delinks disagreement: extra={stale} missing={missing}")

    print("\n-- 4. source-path indexes and tracked ledgers")
    print("   tools/srcpath.py: no edit needed (it rglobs src/, so the moved file is found"
          " automatically and the deleted stems simply stop resolving).")
    print(f"   CONSEQUENCE, not a file edit: srcpath.path_for() stops resolving "
          f"{len(legacy)} symbol(s), so tools/enroll.py + tools/eligible.py stop counting "
          f"them as individually eligible. rombuild_check's `complete` entry count drops "
          f"by {len(legacy) - 1} while sourceBytes is unchanged -- plan sec 13 item 7 is "
          f"about BYTES and is satisfied, but any function-count metric moves.")
    stems = [pathlib.PurePosixPath(l).stem for l in legacy]

    def scan(group):
        out = []
        for rel in group:
            p = REPO / rel
            if not p.is_file():
                continue
            text = p.read_text(encoding="utf-8", errors="ignore")
            found = [l for l in legacy if l in text]
            found_stems = [s for s in stems if s in text]
            if found or found_stems:
                out.append((rel, found, found_stems))
        return out

    live_hits = scan(_PROMOTE_TRACKERS_LIVE)
    if live_hits:
        for rel, found, found_stems in live_hits:
            print(f"   EDIT {rel}: {len(found)} path reference(s), {len(found_stems)} stem "
                  f"reference(s)")
            for x in (found + found_stems)[:6]:
                print(f"        {x}")
    else:
        print(f"   scanned {len(_PROMOTE_TRACKERS_LIVE)} tracked live-state ledger/baseline "
              f"file(s) for the legacy paths and stems: no references, none needs editing.")
    for rel, found, found_stems in scan(_PROMOTE_TRACKERS_HISTORY):
        print(f"   RETARGET (do NOT delete records) {rel}: {len(found)} `srcPath` "
              f"reference(s), {len(found_stems)} stem reference(s)")
        for x in (found + found_stems)[:6]:
            print(f"        {x}")
    print("   port/: port/tools reads the ROM-build outputs and src/ by path; a new "
          "src/ subdirectory may need adding to port/CMakeLists.txt's source globs "
          "(not checked mechanically here).")

    print("\n-- 4b. contributor attribution -- the structural cost of a promotion")
    try:
        import chaos_db_ci as CDB
        owners = CDB.first_matchers()
    except Exception as exc:                                     # noqa: BLE001
        print(f"   could not run chaos_db_ci.first_matchers(): {exc}")
        owners = None
    if owners is not None:
        by_author = collections.defaultdict(list)
        for rel in legacy:
            by_author[owners.get(rel, "(no lineage found)")].append(rel)
        print(f"   the gate's own computation (chaos_db_ci.first_matchers, the same one "
              f"tools/prepush_attribution.py runs) credits these {len(legacy)} files to "
              f"{len(by_author)} contributor(s):")
        for who, rels in sorted(by_author.items(), key=lambda kv: (-len(kv[1]), kv[0])):
            print(f"     {who:20} {len(rels)} file(s)")
        print(f"   A TU promotion is a {len(legacy)}-delete + 1-add collapse. Git can pair "
              f"at most ONE delete with the add as a rename, and only if similarity "
              f"survives; the other {len(legacy) - 1} lineages END, and the pusher becomes "
              f"the owner of the merged file. This is NOT the rewrite-then-move hazard "
              f"prepush_attribution.py documents, which a two-commit split fixes -- a "
              f"many-to-one collapse has no commit arrangement that preserves N lineages, "
              f"so `CREDIT LOST` is structural for this workstream and needs an explicit "
              f"policy (an attribution.json override keyed on the surviving path, or the "
              f"attribution-override label) BEFORE the first real promotion.")
    # And a harder one that the credit computation above does not reach, found while
    # investigating whether partial isolation changes the promotion story. It does not
    # depend on git lineage at all, so no commit arrangement and no override touches it.
    print(f"   HARDER, AND NOT WAIVABLE: tools/validate_merge.py binds a function to a "
          f"source by FILE STEM -- function_snapshot() builds {{stem: path}} over src/ and "
          f"looks the SYMBOL NAME up in it. Collapsing these {len(legacy)} files leaves no "
          f"stem equal to any of the {len(legacy)} symbols, so all {len(legacy)} functions "
          f"leave `matched` and the merge gate raises `lost {len(legacy)} matched "
          f"function(s)`. Precisely: the attribution-override label maps to "
          f"--allow-attribution-change, which DOES waive credit 'changed or was lost' "
          f"(the two attribution reasons) -- but `lost N matched function(s)` is a "
          f"separate, unconditional matched-COVERAGE check (validate_merge.py ~line 410), "
          f"gated by no flag at all. No existing override reaches it. Verified against the "
          f"real function_snapshot/attribution_snapshot at HEAD, where all {len(legacy)} "
          f"are matched and credited today.")
    print("   Consequence for sequencing: a whole-TU source collapse cannot land through "
          "CI until validate_merge learns a symbol->source mapping that is not "
          "'stem == symbol name'. config/**/delinks.txt already carries exactly that "
          "mapping (each entry names a source AND the range it owns), and so does "
          "config/tu_manifest.json; neither is consulted. `linkcheck --partial` is "
          "unaffected -- it changes no source path at all.")
    print("   Related, and NOT triggered by this TU but by any key-function TU: dsd "
          "derives vtable/typeinfo ownership from the mangled class name in the "
          "delinks.txt PATH, so replacing src/_ZN<Class>D1Ev.cpp with src/<dir>/<Class>.cpp "
          "changes that derivation (see notes/vtable-rename-must-move-atomically.md).")

    print("\n-- 5. manifest")
    print(f"   {MANIFEST.relative_to(REPO).as_posix()}: entry {entry['id']} "
          f"status {status} -> promoted, source -> {dest_rel}")

    print("\n-- 6. validation a real promotion would then run")
    print("   python tools/layout_check.py")
    print("   python tools/eligible.py            (expect the eligible NAME LIST to lose "
          "the superseded stems; diff names, not counts)")
    print("   python tools/rombuild.py            (module fidelity + source fidelity)")
    print(f"   python tools/tubuild.py linkcheck {entry['id']}   (now against the real tree)")

    print("\nDRY RUN COMPLETE -- no file was created, moved, deleted or edited.")
    return 0 if ok_status else 1


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

    p = sub.add_parser("partial", help="plan sec 9 -- derive one isolated object per "
                                       "licensed function from a single TU compile and "
                                       "compare each against the production per-function "
                                       "object (no link, no config change)")
    p.add_argument("id")
    p.add_argument("--version", default=None)
    p.add_argument("-j", "--jobs", type=int, default=4)
    p.add_argument("--clean", action="store_true",
                   help="delete this TU's build/tu/<id>/partial tree before running")
    p.add_argument("--no-record", action="store_true",
                   help="do not write the result back into the manifest")
    p.set_defaults(func=cmd_partial)

    p = sub.add_parser("linkcheck", help="scratch dsd delink/lcf + real mwldarm link, "
                                         "linked-range/module comparison, dsd check "
                                         "symbols --fail, ROM build (plan sec 7.6)")
    p.add_argument("id", nargs="?")
    p.add_argument("--baseline", action="store_true",
                   help="run the identical scratch pipeline with NO TU substitution -- the "
                        "control that says whether a failure belongs to the TU or the harness")
    p.add_argument("--partial", action="store_true",
                   help="plan sec 9: leave delinks.txt alone and substitute N derived "
                        "per-function objects (one TU compile, objisolate.derive per "
                        "function) for the N objects the build produces, at their own "
                        "existing paths -- instead of replacing the whole range with one "
                        "object")
    p.add_argument("--module", default=None, help="module for --baseline without an id")
    p.add_argument("-j", "--jobs", type=int, default=RB.default_jobs())
    p.add_argument("--no-cache", action="store_true",
                   help="compile every enrolled file instead of reusing build/objcache hits")
    p.add_argument("--no-rom", action="store_true", help="stop after the module comparison")
    p.add_argument("--clean", action="store_true",
                   help="delete this TU's scratch tree before running")
    p.set_defaults(func=cmd_linkcheck)

    p = sub.add_parser("promote", help="plan sec 7.7 -- only --dry-run is implemented")
    p.add_argument("id")
    p.add_argument("--dry-run", action="store_true",
                   help="print every planned file move/deletion and config edit, and exit")
    p.set_defaults(func=cmd_promote)

    args = ap.parse_args()
    if args.manifest is not None:
        global MANIFEST
        MANIFEST = args.manifest
    return args.func(args) or 0


if __name__ == "__main__":
    sys.exit(main())
