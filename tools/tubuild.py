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
`eligible.py` / `rombuild.py`. It only reads production state (the
committed config/, the extracted ROM, the pinned mwccarm) and writes to
src_tu/, config/tu_manifest.d/, and build/tu/ (gitignored, see .gitignore's
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
import tu_manifest as TUM      # noqa: E402  the manifest's on-disk shape
import tu_map as TM             # noqa: E402

TU_MAP = REPO / "build" / "tu_map.json"
BASELINE_LINK = REPO / "build" / "tu" / "_baseline" / "link"
MANIFEST = TUM.DEFAULT_ROOT
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
    # MANIFEST is normally the config/tu_manifest.d/ directory (one file per
    # entry, so TU PRs stop conflicting on a shared list); `--manifest` may
    # point it at a single scratch .json, which tu_manifest handles too.
    return TUM.load(MANIFEST)


def save_manifest(data):
    TUM.save(data, MANIFEST)


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
_LONG_CALLS_ON_RE = re.compile(r'^\s*#\s*pragma\s+long_calls\s+on\b')
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
            block = "\n".join(lines[i:j + 1])
            nm = re.match(rf'{first_word}\s+(\w+)', stripped)
            dname = nm.group(1) if nm else f"<anonymous {first_word} in this file>"
            if first_word == "typedef":
                # `typedef struct { ... } State300;` -- the word after `typedef`
                # is `struct`, so keying on it collided every anonymous typedef
                # in a TU with every other and the merger silently dropped all
                # but the first (State300 in ov020). The typedef'd NAME is the
                # trailing identifier; key on that.
                tail = re.search(r'(\w+)\s*;\s*$', block)
                if tail:
                    dname = tail.group(1)
            shadow_decls.append((first_word, dname, block))
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


def _is_forward_decl(item):
    """True for a BARE `struct C;`-style forward declaration carried as a shadow
    declaration (kind, name, text). A forward declaration never CONFLICTS with a
    full definition of the same name -- one names the type, the other supplies it.
    RacingPenguin (ov019) had its full `struct C { ... };` conflict-commented away
    because the forward declaration arrived first. Deliberately strict: a line
    like `struct C; typedef void (C::*PMF)();` carries MORE than the forward
    declaration and must still flag, or the extra text is silently lost."""
    if not (isinstance(item, tuple) and len(item) == 3):
        return False
    kind, dname, text = item
    return re.fullmatch(rf'\s*{re.escape(kind)}\s+{re.escape(dname)}\s*;\s*', text) is not None


def _merge_field(ord_rows, parsed, getter, name_of, kind_label, warnings):
    """Union items across a TU's legacy files by name; flag same-name/different-text
    as a conflict rather than picking a winner silently (plan sec 7.3). The one
    silent reconciliation: a forward declaration folds into a full definition of
    the same name (definition wins, whichever order they arrive in)."""
    seen, live, dead = {}, [], []
    for _o, name, _a, _s in ord_rows:
        for item in getter(parsed[name]):
            key = name_of(item)
            if key not in seen:
                seen[key] = (item, name)
                live.append((key, item))
            elif seen[key][0] != item:
                if kind_label == "local declaration":
                    if _is_forward_decl(item) and not _is_forward_decl(seen[key][0]):
                        continue          # new is only a forward decl of the kept definition
                    if _is_forward_decl(seen[key][0]) and not _is_forward_decl(item):
                        # kept was only a forward decl; the definition replaces it in place
                        old = seen[key][0]
                        seen[key] = (item, name)
                        live[live.index((key, old))] = (key, item)
                        continue
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
        out.append("/* TUBUILD NOTE -- #pragma directive(s) were present in the legacy sources")
        out.append(" * of this TU. `#pragma long_calls` is POSITIONAL in mwccarm 2004/b56 and is")
        out.append(" * carried verbatim before its own member below, bracketed with `off` so it")
        out.append(" * cannot leak into later members (dropping it silently costs the pooled")
        out.append(" * cross-overlay tail-call -- a byte diff; see ShutterBob in ov014).")
        out.append(" * Any OTHER pragma is FILE-GLOBAL last-wins (opt_propagation,")
        out.append(" * optimize_for_size) and is still left out: carried into a merged TU it")
        out.append(" * would silently recompile every other member. Decide those by hand:")
        for name, p in pragma_hits:
            carried = "carried below" if _LONG_CALLS_ON_RE.match(p) else "NOT carried -- review"
            out.append(f" *   {name}: {p}   [{carried}]")
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
        if p.get("missing"):
            # No legacy source under src/. The candidate is still worth assembling
            # for its other members; this range stays the ROM's own bytes and
            # `verify` will honestly report the member as MISSING.
            out.append(f"/* SOURCELESS member {name}: no file under src/ supplies it; the")
            out.append(" * ROM's own bytes cover this range. Recover it before promotion. */")
            out.append("")
            continue
        for note in p["notes"]:
            out.append(note)
        # `#pragma long_calls` is POSITIONAL (measured on 2004/b56, notes in the
        # ov014 seed): carried verbatim before its own member and closed after it,
        # so the pooled cross-overlay tail-call it forces cannot leak into later
        # members. Other pragmas are file-global last-wins and are never carried.
        long_calls_here = [pr for pr in p["pragmas"] if _LONG_CALLS_ON_RE.match(pr)]
        for pr in long_calls_here:
            out.append(pr + "  /* carried verbatim from the legacy file (positional) */")
        func_text = p["function_text"]
        if cpp_needed and not p["cpp"]:
            # A legacy .c file's identifier is unmangled by construction; giving the
            # merged C++ TU the same text without linkage protection would let the
            # compiler mangle it a second time (the double-mangling defect).
            # The linkage goes on a BLOCK around the member's whole text: prefixing
            # the first line lands it on a preamble declaration and the definition
            # is emitted mangled -- the TU then silently fails to define its own
            # ROM symbol (tubuild defect 2).
            out.append('extern "C" {  /* .c-derived member: C linkage for the whole block */')
            out.append(func_text.rstrip("\n"))
            out.append("}")
        else:
            out.append(func_text.rstrip("\n"))
        for pr in long_calls_here:
            out.append("#pragma long_calls off  "
                       "/* close the bracket: positional, must not leak downward */")
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

    # Neither a sourceless member nor an unsplittable file refuses the whole
    # candidate any more (tubuild defect 4): the largest TUs were
    # only reachable by hand-assembly because one member's shape aborted create.
    # A sourceless member becomes a banner (verify reports it MISSING, honestly);
    # an unsplittable file is carried VERBATIM -- raw concatenation is exactly
    # what six modules' hand-assembly did, byte-verified 222 functions.
    pre_warnings = []
    parsed = {}
    for _o, name, _a, _s in ord_rows:
        legacy = SP.path_for(name)
        if legacy is None:
            pre_warnings.append(f"SOURCELESS: {name} has no legacy file under src/; a banner "
                                f"marks its slot and verify will report the member MISSING")
            parsed[name] = {"error": None, "cpp": False, "missing": True,
                            "includes": [], "pragmas": [], "macros": [], "externs": [],
                            "shadow_decls": [], "notes": [], "function_text": "",
                            "legacy_path": f"<none for {name}>"}
            continue
        text = legacy.read_text(encoding="utf-8", errors="ignore")
        p = split_legacy_source(text)
        p["legacy_path"] = legacy.relative_to(REPO).as_posix()
        if p["error"]:
            is_cpp = text.startswith("//cpp")
            raw = text.split("\n", 1)[1] if is_cpp and "\n" in text else text
            pre_warnings.append(f"RAW: {p['legacy_path']} ({name}) did not split "
                                f"({p['error']}); carried verbatim -- its own includes/"
                                f"declarations stay inside its member block")
            p = {"error": None, "cpp": is_cpp, "raw": True,
                 "includes": [], "pragmas": [], "macros": [], "externs": [],
                 "shadow_decls": [], "notes": [], "function_text": raw,
                 "legacy_path": p["legacy_path"]}
        parsed[name] = p

    body, warnings = assemble_shadow_source(args.id, ord_rows, parsed)
    warnings = pre_warnings + warnings

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
         f"({TUM.describe(MANIFEST)})")
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
    licensed_data = {row["symbol"] for _sec, row in manifest_owned_symbol_rows(entry)}
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
                         f"{TUM.describe(MANIFEST)}; run `create` first, or "
                         f"add one by hand (see notes/tu-reconstruction-pilot-report.md)")

    obj_bytes, version, flags, build_dir, obj_path = _compile_tu(entry, args.version)
    inv = elf_inventory(obj_bytes)
    audited_bytes, compiler_only, policy_reasons = apply_compiler_only_policy(
        obj_bytes, entry)
    if not policy_reasons and compiler_only.get("deadstripped"):
        print(f"compiler-only     : exact deadstrip {compiler_only['deadstripped']}")
    audit_inv = elf_inventory(audited_bytes) if audited_bytes is not None else inv
    unlicensed_funcs, unlicensed_objs, unlicensed_secs = unlicensed_inventory(
        entry, audit_inv)

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
    audited_bytes, compiler_only, policy_reasons = apply_compiler_only_policy(
        obj_bytes, entry)
    audit_inv = elf_inventory(audited_bytes) if audited_bytes is not None else inv

    name_index = RA.build_name_index()
    config_relocs = RA.build_config_relocs()
    sym_index = RL.load_all_syms()

    print(f"\nTU {entry['id']}\n")
    if not policy_reasons and compiler_only.get("deadstripped"):
        print(f"compiler-only     : exact deadstrip {compiler_only['deadstripped']}\n")
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

    unlicensed_funcs, unlicensed_objs, unlicensed_secs = unlicensed_inventory(
        entry, audit_inv)
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
    if policy_reasons:
        print()
        for reason in policy_reasons:
            print(f"POLICY   {reason}")

    secs = sorted((int(s["start"], 16), int(s["end"], 16), s["name"])
                 for s in entry.get("sections", []))
    overlaps = [(a, b) for a, b in zip(secs, secs[1:]) if a[1] > b[0]]
    if overlaps:
        print(f"\nMANIFEST ERROR: overlapping section claims: {overlaps}")

    declared_syms = {f["symbol"] for f in entry["functions"]}
    defined_syms = {s["name"] for s in audit_inv["symbols"]
                    if s["type"] == "STT_FUNC"}
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
    if policy_reasons:
        print("        compiler-only policy refused -> PROMOTION REFUSED")

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
        "compilerOnlyOutput": compiler_only,
    })
    # Keep the nested `criteria` block honest about what THIS run actually measured --
    # `comparison` above always names reloc_audit now that verify runs it unconditionally,
    # so a stale hand-authored "NOT RUN" left in criteria would contradict it. Overwrite
    # only the fields this run has an opinion on; leave later-phase fields (symbol
    # addresses, module/ROM build) alone since verify never touches those.
    criteria = entry["verification"].setdefault("criteria", {})

    def _keep_richer(key, verdict):
        """A curated criteria value often carries evidence prose ('PASS --
        reloc_audit.check_destinations(): 57 relocations, 57 OK, 0 WRONG-DEST').
        Overwriting it with a bare verdict destroys strictly more informative
        text every run (tubuild defect 1). Keep the existing
        text whenever it already states the SAME verdict and says more; replace
        it only when this run's verdict differs."""
        old = criteria.get(key)
        if isinstance(old, str) and old.split()[0].rstrip(":,") == verdict.split()[0] \
                and len(old) > len(verdict):
            return old
        return verdict

    for key, verdict in {
        "every_declared_function_defined": "PASS" if not any(v == "MISSING" for _o, _s, v in rows) else "FAIL",
        "every_declared_function_bytes_match": "PASS" if all_bytes_ok else "FAIL",
        "declared_function_set_equals_defined_function_set":
            "PASS" if (not (declared_syms - defined_syms) and n_unlicensed == 0
                       and not policy_reasons)
            else f"FAIL-BY-DESIGN -- {n_unlicensed} unlicensed section/symbol(s), "
                 f"{len(policy_reasons)} compiler-only policy error(s)",
        "functions_occur_in_expected_order":
            "PASS" if not bad_pairs else f"PARTIAL -- ordinal pair(s) not in ROM order: {bad_pairs}",
        "relocation_destinations_verified": "PASS" if all_reloc_ok else "FAIL -- see DIFF/objisolate lines above",
    }.items():
        criteria[key] = _keep_richer(key, verdict)
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
_TU_SECTION_NAMES = (".text", ".rodata", ".init", ".ctor", ".data", ".bss")


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


def manifest_section_claims(entry):
    """Return normalized manifest section claims and structural refusal reasons.

    A claim is the exact range one compiler input section may replace in one
    loadable module output section.  ``name`` is the ELF input section and the
    optional ``module_section`` is the delinks/module section containing its retail
    range (defaulting to ``name``).  Keeping those identities separate is necessary
    for ordinary linker mappings such as input ``.rodata`` being placed in output
    ``.data``; it does not weaken the byte, symbol, or relocation checks on the input.

    One claim per input section name is deliberate for this first data phase:
    mwldarm concatenates a TU's repeated input sections of the same name into one
    contribution, while two disjoint ranges would need an explicit input-section
    partition the manifest cannot yet express.  Refusing that ambiguity is safer than
    assigning output by list position.
    """
    rows, reasons, seen = [], [], set()
    raw = entry.get("sections")
    if not isinstance(raw, list) or not raw:
        return [], ["manifest has no section claims"]
    for i, claim in enumerate(raw):
        if not isinstance(claim, dict):
            reasons.append(f"sections[{i}] is not an object")
            continue
        name = claim.get("name")
        if name not in _TU_SECTION_NAMES:
            reasons.append(f"sections[{i}] has unsupported name {name!r}; expected one "
                           f"of {list(_TU_SECTION_NAMES)}")
            continue
        module_section = claim.get("module_section", name)
        if module_section not in _TU_SECTION_NAMES:
            reasons.append(f"sections[{i}] has unsupported module_section "
                           f"{module_section!r}; expected one of "
                           f"{list(_TU_SECTION_NAMES)}")
            continue
        if name in seen:
            reasons.append(f"duplicate {name} claim; this schema supports one contiguous "
                           f"contribution per section name")
            continue
        seen.add(name)
        try:
            start = int(claim["start"], 0) if isinstance(claim["start"], str) \
                else int(claim["start"])
            end = int(claim["end"], 0) if isinstance(claim["end"], str) \
                else int(claim["end"])
        except (KeyError, TypeError, ValueError):
            reasons.append(f"sections[{i}] {name} needs integer/hex start and end")
            continue
        if start >= end:
            reasons.append(f"sections[{i}] {name} is empty/reversed: "
                           f"0x{start:08x}..0x{end:08x}")
            continue
        rows.append({"name": name, "module_section": module_section,
                     "start": start, "end": end})

    if sum(1 for r in rows if r["name"] == ".text") != 1:
        reasons.append("manifest must claim exactly one .text contribution")
    ordered = sorted(rows, key=lambda r: (r["start"], r["end"], r["name"]))
    for left, right in zip(ordered, ordered[1:]):
        if left["end"] > right["start"]:
            reasons.append(f"manifest section claims overlap: {left['name']} "
                           f"0x{left['start']:08x}..0x{left['end']:08x} and "
                           f"{right['name']} 0x{right['start']:08x}..0x{right['end']:08x}")
    return rows, reasons


def validate_claims_in_module(delinks_path, claims):
    """Every claim must be wholly inside its declared module output section."""
    header, _entries = parse_delinks_file(delinks_path)
    containers = collections.defaultdict(list)
    for name, start, end in EN.sections(header):
        containers[name].append((start, end))
    reasons = []
    for c in claims:
        output_name = c.get("module_section", c["name"])
        homes = [(a, b) for a, b in containers.get(output_name, [])
                 if a <= c["start"] and c["end"] <= b]
        if len(homes) != 1:
            mapping = (c["name"] if output_name == c["name"] else
                       f"{c['name']} -> {output_name}")
            reasons.append(f"{mapping} claim 0x{c['start']:08x}..0x{c['end']:08x} is "
                           f"not wholly inside exactly one declared module section")
    return reasons


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


def validate_tu_entry_splice(delinks_path, span_start, span_end, tu_rel,
                             expected_legacy, section_claims=None):
    """Return the fully validated inputs for one destructive TU-entry splice."""
    header, entries, inside, reasons = span_entries(delinks_path, span_start, span_end,
                                                    expected_legacy)
    claims = list(section_claims or
                  ({"name": ".text", "start": span_start, "end": span_end},))
    text = [c for c in claims if c["name"] == ".text"]
    if len(text) != 1 or (text[0]["start"], text[0]["end"]) != (span_start, span_end):
        reasons.append("section claims must contain exactly the .text span passed to the "
                       "legacy tiling check")
    reasons.extend(validate_claims_in_module(delinks_path, claims))

    # Non-text ranges must currently be pure gap ownership.  If any existing file
    # entry touches one, silently adding the TU would create two owners; deciding how
    # to retire a future data-source entry is promotion policy, not scratch plumbing.
    drop_indices = {i for i, _r, _s in inside}
    for idx, (rel, _body) in enumerate(entries):
        if rel == tu_rel and idx not in drop_indices:
            reasons.append(f"TU destination {tu_rel} is already a delinks entry")
    for claim in (c for c in claims if c["name"] != ".text"):
        for idx, (rel, body) in enumerate(entries):
            for name, start, end in entry_sections(body):
                if idx not in drop_indices and max(start, claim["start"]) < min(end, claim["end"]):
                    reasons.append(f"{claim['name']} claim 0x{claim['start']:08x}.."
                                   f"0x{claim['end']:08x} overlaps existing entry {rel}'s "
                                   f"{name} 0x{start:08x}..0x{end:08x}; it is not pure "
                                   f"gap ownership")
    return header, entries, inside, claims, reasons


def splice_tu_entry(delinks_path, span_start, span_end, tu_rel, expected_legacy,
                    section_claims=None):
    """Replace the per-function entries tiling [span_start, span_end) with ONE TU entry.

    Refuses -- returns (None, [reasons]) -- rather than producing a plausible-looking
    scratch config, because every failure mode here is silent downstream: dsd fills any
    range it has no object for with retail ROM bytes, so a mis-spliced delinks tree
    links clean and compares green while contributing nothing (see
    "unbuildable files are invisible to every gate", and layout_check's L1). The
    read-only validator above is also the production promotion preflight, so the two
    paths cannot drift on current delinks ownership rules.
    """
    header, entries, inside, claims, reasons = validate_tu_entry_splice(
        delinks_path, span_start, span_end, tu_rel, expected_legacy, section_claims)
    if reasons:
        return None, reasons

    first = min(i for i, _r, _s in inside)
    drop = {i for i, _r, _s in inside}
    out = list(header)
    for idx, (rel, body) in enumerate(entries):
        if idx == first:
            out.append(f"{tu_rel}:")
            out.append("    complete")
            for claim in claims:
                output_name = claim.get("module_section", claim["name"])
                out.append(f"    {output_name} start:0x{claim['start']:08x} "
                           f"end:0x{claim['end']:08x}")
            out.append("")
            continue
        if idx in drop:
            continue
        out.append(f"{rel}:")
        out.extend(body)
    delinks_path.write_text("\n".join(out).rstrip("\n") + "\n", encoding="utf-8", newline="\n")
    return sorted(rel for _i, rel, _s in inside), []


def add_partitioned_tu_entry(delinks_path, span_start, span_end, tu_rel,
                             expected_legacy, section_claims):
    """Add one scratch-only non-text owner while retaining legacy text entries.

    Partitioned linking needs dsd to keep the existing N text selectors (and their ROM
    order), but to carve one separately reduced TU object into exact data/BSS gaps.
    This helper performs only that additive scratch rewrite.  It refuses before writing
    unless the legacy text entries still tile the manifest span, every non-text claim
    is inside its declared module output section and pure gap ownership, and the TU
    path is not already present.

    The tracked delinks tree is never passed here; ``cmd_linkcheck`` supplies the copy
    below ``build/tu/<id>/link-partitioned/config``.
    """
    _header, entries, inside, reasons = span_entries(
        delinks_path, span_start, span_end, expected_legacy)
    claims = list(section_claims or [])
    text = [c for c in claims if c["name"] == ".text"]
    nontext = [c for c in claims if c["name"] != ".text"]
    if len(text) != 1 or (text[0]["start"], text[0]["end"]) != (span_start, span_end):
        reasons.append("section claims must contain exactly the .text span passed to the "
                       "legacy tiling check")
    if not nontext:
        reasons.append("partitioned linking needs at least one non-.text section claim; "
                       "use --partial for text-only TUs")
    reasons.extend(validate_claims_in_module(delinks_path, claims))

    existing_paths = [rel for rel, _body in entries]
    if tu_rel in existing_paths:
        reasons.append(f"partitioned TU path {tu_rel} is already a delinks entry")

    # Only module output section names reach dsd's LCF selectors.  A compiler input
    # name that differs would require verified section retargeting; merely writing the
    # output name here would make `TU.o(.data)` silently omit an input `.rodata`.
    for claim in nontext:
        output_name = claim.get("module_section", claim["name"])
        if output_name != claim["name"]:
            reasons.append(f"partitioned claim {claim['name']} -> {output_name} needs "
                           "input-section retargeting, which is not implemented")
    output_names = [claim.get("module_section", claim["name"]) for claim in nontext]
    if len(output_names) != len(set(output_names)):
        reasons.append("partitioned linking supports one contribution per module output "
                       f"section, got {output_names}")

    for claim in nontext:
        for _idx, (rel, body) in enumerate(entries):
            for name, start, end in entry_sections(body):
                if max(start, claim["start"]) < min(end, claim["end"]):
                    reasons.append(f"{claim['name']} claim 0x{claim['start']:08x}.."
                                   f"0x{claim['end']:08x} overlaps existing entry "
                                   f"{rel}'s {name} 0x{start:08x}..0x{end:08x}; it is "
                                   f"not pure gap ownership")
    if reasons:
        return None, reasons

    original = delinks_path.read_text(encoding="utf-8").rstrip("\r\n")
    out = [original, "", f"{tu_rel}:", "    complete"]
    for claim in nontext:
        output_name = claim.get("module_section", claim["name"])
        out.append(f"    {output_name} start:0x{claim['start']:08x} "
                   f"end:0x{claim['end']:08x}")
    delinks_path.write_text("\n".join(out) + "\n", encoding="utf-8", newline="\n")
    return sorted(rel for _i, rel, _s in inside), []


def validate_partitioned_link_artifacts(lcf_path, objects_path, entry, claims):
    """Prove dsd generated the intended N-text-plus-one-data linker surface."""
    if not pathlib.Path(lcf_path).is_file() or not pathlib.Path(objects_path).is_file():
        return {"ok": False, "errors": [
            "dsd did not generate both arm9.lcf and objects.txt"],
            "expectedTuSelectors": [], "observedTuSelectors": [],
            "observedSelectors": [], "expectedObjects": [], "observedObjects": [],
            "selectorCount": 0, "objectCount": 0,
            "selectorListSha256": None, "objectListSha256": None}
    lcf_lines = [line.strip() for line in pathlib.Path(lcf_path).read_text(
        encoding="utf-8", errors="replace").splitlines()]
    objects = [line.strip() for line in pathlib.Path(objects_path).read_text(
        encoding="utf-8", errors="replace").splitlines() if line.strip()]
    object_paths = [str(pathlib.Path(line).resolve()) for line in objects]
    object_basenames = [pathlib.Path(path).name for path in object_paths]
    root = pathlib.Path(objects_path).parent.resolve()
    reasons = []

    legacy = [f["legacy_source"] for f in entry.get("functions", [])]
    legacy_stems = [pathlib.PurePosixPath(rel).stem for rel in legacy]
    if len(legacy_stems) != len(set(legacy_stems)):
        reasons.append(f"legacy object basenames are not unique: {legacy_stems}")
    for rel, stem in zip(legacy, legacy_stems):
        selector = f"{stem}.o(.text)"
        count = lcf_lines.count(selector)
        if count != 1:
            reasons.append(f"LCF contains {count} exact lines {selector}, "
                           "expected one")
        expected_obj = str((root / pathlib.Path(rel).with_suffix(".o")).resolve())
        count = object_paths.count(expected_obj)
        if count != 1:
            reasons.append(f"objects.txt contains {count} exact paths {expected_obj} for "
                           f"{rel}, expected one")
        basename = f"{stem}.o"
        count = object_basenames.count(basename)
        if count != 1:
            reasons.append(f"objects.txt contains {count} objects named {basename} for "
                           f"{rel}, expected one linker-visible basename")

    tu_stem = pathlib.PurePosixPath(entry["source"]).stem
    tu_name = f"{tu_stem}.o"
    expected_tu_obj = str((root / pathlib.Path(entry["source"]).with_suffix(".o")).resolve())
    count = object_paths.count(expected_tu_obj)
    if count != 1:
        reasons.append(f"objects.txt contains {count} exact paths {expected_tu_obj}, "
                       "expected one")
    count = object_basenames.count(tu_name)
    if count != 1:
        reasons.append(f"objects.txt contains {count} objects named {tu_name}, expected "
                       "one linker-visible basename")
    tu_selector_re = re.compile(rf"^{re.escape(tu_name)}\(([^)]+)\)$")
    observed_tu = [line for line in lcf_lines if tu_selector_re.match(line)]
    expected_tu = [f"{tu_name}({c.get('module_section', c['name'])})"
                   for c in claims if c["name"] != ".text"]
    if sorted(observed_tu) != sorted(expected_tu):
        reasons.append(f"TU selector set is {observed_tu}, expected exactly {expected_tu}")
    observed_selectors = [line for line in lcf_lines
                          if re.match(r"^[^\s()]+\.o\([^()]+\)$", line)]
    normalized_selectors = sorted(observed_selectors)
    normalized_objects = sorted(object_paths)
    selector_hash = hashlib.sha256(
        ("\n".join(normalized_selectors) + "\n").encode("utf-8")).hexdigest()
    object_hash = hashlib.sha256(
        ("\n".join(normalized_objects) + "\n").encode("utf-8")).hexdigest()
    return {"ok": not reasons, "errors": reasons,
            "expectedTuSelectors": expected_tu,
            "observedTuSelectors": observed_tu,
            "observedSelectors": observed_selectors,
            "expectedObjects": [str((root / pathlib.Path(rel).with_suffix(".o")).resolve())
                                for rel in legacy] + [expected_tu_obj],
            "observedObjects": object_paths,
            "expectedLegacyCount": len(legacy),
            "selectorCount": len(observed_selectors),
            "objectCount": len(object_paths),
            "selectorListSha256": selector_hash,
            "objectListSha256": object_hash}


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
    licensed |= {row["symbol"] for _sec, row in manifest_owned_symbol_rows(entry)}
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
    licensed_sections = {s["name"] for s in entry.get("sections", [])
                         if isinstance(s, dict) and s.get("name")}
    # Return only genuinely unclaimed content.  Repeated same-name sections are one
    # aggregate contribution and are checked for exact size/bytes by
    # verify_owned_sections; their individual defined symbols are still audited above.
    extra_secs = [s for s in inv["sections"]
                  if s["size"] and s["type"] in OI.CONTENT
                  and s["name"] not in licensed_sections
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
          f"(fatal until vague-linkage coalescing has an explicit verified policy), "
          f"{buckets['HOMELESS']} HOMELESS")
    for r in sorted(rows, key=lambda r: (r["verdict"], r["name"])):
        if r["verdict"] == "LICENSED":
            continue
        detail = (f"  already defined at {', '.join(r['homes'])}" if r["homes"]
                  else "  no symbols.txt anywhere names it")
        print(f"      {r['verdict']:12} {r['name']:38} {r['bind']:10} {r['section']:8} "
              f"size 0x{r['size']:x}{detail}")
    if extra_secs:
        total = sum(s["size"] for s in extra_secs)
        kinds = collections.Counter(s["name"] for s in extra_secs)
        print(f"      unlicensed content sections: {len(extra_secs)} "
              f"({dict(kinds)}), {total} bytes total")
    print(f"      emitted .text order of the licensed functions: "
          f"{'ROM-ascending, as required' if order_ok else 'NOT ROM-ascending'}")
    if not order_ok:
        print(f"        emitted: {[n for _i, n in emitted]}")
    return buckets


def object_audit_refusals(rows, extra_secs, order_ok):
    """Fatal pre-link findings after exact compiler-only policy has run.

    No linker behavior is an ownership policy.  In particular, an STB_LOPROC RTTI
    copy that mwldarm happens to coalesce is still an unlicensed definition until the
    manifest has an exact, separately implemented coalescing convention.
    """
    reasons = [f"unlicensed defined symbol {r['name']} ({r['verdict']}, "
               f"{r['section']}, {r['bind']})"
               for r in rows if r["verdict"] != "LICENSED"]
    reasons.extend(f"unlicensed content section {s['name']} size 0x{s['size']:x}"
                   for s in extra_secs)
    if not order_ok:
        reasons.append("licensed .text functions are not emitted in ROM address order")
    return reasons


# ========================================= whole-TU non-text ownership (phases E/F)

_SECTION_SYMBOL_FIELDS = {
    ".rodata": "rodata", ".init": "init", ".ctor": "ctor",
    ".data": "data", ".bss": "bss",
}


def manifest_owned_symbol_rows(entry):
    """All manifest-licensed non-text symbols, annotated with their section name."""
    out = []
    for section, field in _SECTION_SYMBOL_FIELDS.items():
        rows = entry.get(field, [])
        if not isinstance(rows, list):
            continue
        for row in rows:
            if isinstance(row, dict) and row.get("symbol"):
                out.append((section, row))
    return out


def _manifest_addr(row):
    """Configured/public symbol address (a vtable's address point, not its storage)."""
    value = row.get("address", row.get("rom_symbol_address"))
    if value is None:
        return None
    try:
        return int(value, 0) if isinstance(value, str) else int(value)
    except (TypeError, ValueError):
        return None


def _manifest_emitted_addr(row):
    """(object-symbol address, error) with an explicit address-point convention.

    Most ELF symbols are emitted at their configured address.  A CodeWarrior vtable
    is the important exception: its ELF symbol names the storage object while the
    checked-in ``_ZTV`` symbol names the public address point eight bytes later.  The
    manifest must spell both facts; recognizing ``_ZTV`` and subtracting eight here
    would silently reinterpret every existing symbol table.

    Example::

        {"symbol": "_ZTV5Thing", "address": "0x02001008",
         "emitted_storage_address": "0x02001000", "address_point_bias": "0x8"}
    """
    public = _manifest_addr(row)
    has_storage = "emitted_storage_address" in row
    has_bias = "address_point_bias" in row
    if has_storage != has_bias:
        return None, ("emitted_storage_address and address_point_bias must be supplied "
                      "together")
    if not has_storage:
        return public, None
    if public is None:
        return None, "explicit emitted storage needs a valid public address"
    try:
        raw_storage = row["emitted_storage_address"]
        raw_bias = row["address_point_bias"]
        storage = int(raw_storage, 0) if isinstance(raw_storage, str) else int(raw_storage)
        bias = int(raw_bias, 0) if isinstance(raw_bias, str) else int(raw_bias)
    except (TypeError, ValueError):
        return None, "emitted_storage_address/address_point_bias must be integers"
    if storage + bias != public:
        return None, (f"emitted storage 0x{storage:08x} + address-point bias 0x{bias:x} "
                      f"does not equal public address 0x{public:08x}")
    return storage, None


def apply_compiler_only_policy(obj_bytes, entry, homes=None):
    """Apply exact-symbol historical dead stripping, or refuse with named reasons.

    ``compiler_only_output`` is an allow-list, not a pattern language::

        {"symbol": "_ZN1DD2Ev", "disposition": "deadstrip",
         "reason": "compiler-generated D2; no ROM symbol or caller"}

    A policy cannot hide a configured ROM symbol, a licensed function, a shared
    section, or anything a surviving relocation references.  objisolate owns the ELF
    surgery; this layer owns the manifest/config facts.  With no policy, any unlicensed
    function is an explicit refusal rather than linker-dependent behaviour.

    Data rows -- the ``_ZTV``/``_ZTI``/``_ZTS`` a key function drags in -- take the
    same exact-section reduction as functions.  This matters even to the pre-link
    verifier: a vtable can reference a vague duplicate destructor, so attempting to
    remove the duplicate while retaining the data definition is internally
    inconsistent.  Reducing the whole declared compiler-only set in one plan both
    models production and lets objisolate prove that no retained section depends on
    content the policy removes.

    A data row must say WHICH kind it is, and an RTTI/vtable record may not take the
    plain ``deadstrip``.  "A policy cannot hide a configured ROM symbol" holds only
    for symbols whose spelling reaches ``all_symbol_homes()``; a class carrying a
    COINED name reaches nothing, because an _ZTI/_ZTS record is a length-prefixed
    mangled string and a coined name misses on both the prefix and the body.  The
    miss reads as "the ROM has no such record", which is exactly backwards, and a
    plain ``deadstrip`` then reaches neither the duplicate-body proof nor the
    cartridge word compare -- so the record is dropped unverified while every gate
    stays green.  Rename the class to the cartridge's RTTI spelling (the row then
    resolves as ``deadstrip-data`` and is compared) or drop the row.
    """
    inv = elf_inventory(obj_bytes)
    licensed = {f["symbol"] for f in entry.get("functions", [])}
    licensed |= {row["symbol"] for _sec, row in manifest_owned_symbol_rows(entry)}
    extras = {s["name"]: s for s in inv["symbols"]
              if s["type"] == "STT_FUNC" and not s["name"].startswith("$")
              and s["name"] not in licensed}
    emitted_data = {s["name"] for s in inv["symbols"]
                    if s["type"] == "STT_OBJECT" and not s["name"].startswith("$")
                    and s["name"] not in licensed}
    policy = entry.get("compiler_only_output", [])
    reasons, wanted, duplicates, data_rows = [], [], set(), []
    if policy is None:
        policy = []
    if not isinstance(policy, list):
        return None, {}, ["compiler_only_output must be a list"]
    for i, row in enumerate(policy):
        if not isinstance(row, dict):
            reasons.append(f"compiler_only_output[{i}] is not an object")
            continue
        sym = row.get("symbol")
        if not sym:
            reasons.append(f"compiler_only_output[{i}] has no symbol")
            continue
        if row.get("disposition") not in ("deadstrip", "deadstrip-duplicate",
                                          "deadstrip-data"):
            reasons.append(f"compiler_only_output {sym} has unsupported disposition "
                           f"{row.get('disposition')!r}; expected deadstrip, "
                           f"deadstrip-duplicate or deadstrip-data")
            continue
        if sym in emitted_data:
            if row.get("disposition") == "deadstrip-duplicate":
                reasons.append(f"compiler_only_output {sym} is a data object; raw "
                               f"duplicate-body evidence is function-only, so it takes "
                               f"deadstrip-data (homed) or deadstrip (homeless)")
            if not str(row.get("reason", "")).strip():
                reasons.append(f"compiler_only_output {sym} needs a non-empty reason")
            if sym in licensed:
                reasons.append(f"compiler_only_output {sym} is also licensed by the "
                               f"manifest")
            if sym in dict(data_rows):
                reasons.append(f"duplicate compiler_only_output policy for {sym}")
            data_rows.append((sym, row.get("disposition")))
            continue
        if row.get("disposition") == "deadstrip-data":
            reasons.append(f"compiler_only_output {sym} is declared compiler-only data "
                           f"but the object emits no such data object")
            continue
        if row.get("disposition") == "deadstrip-duplicate":
            # The vague-linkage case: the symbol MUST have a ROM home that another
            # source owns. rombuild proves the body against the cartridge before it
            # discards anything; this scratch path only links, so it accepts the
            # declaration and leaves the proof to the production build.
            duplicates.add(sym)
        if not str(row.get("reason", "")).strip():
            reasons.append(f"compiler_only_output {sym} needs a non-empty reason")
        if sym in licensed:
            reasons.append(f"compiler_only_output {sym} is also licensed by the manifest")
        if sym not in extras:
            reasons.append(f"compiler_only_output {sym} is not an emitted unlicensed function")
        if sym in wanted:
            reasons.append(f"duplicate compiler_only_output policy for {sym}")
        wanted.append(sym)

    unhandled = sorted(set(extras) - set(wanted))
    for sym in unhandled:
        reasons.append(f"unlicensed function {sym} has no compiler_only_output policy")
    homes = all_symbol_homes() if homes is None else homes
    for sym in wanted:
        if sym in duplicates:
            if not homes.get(sym):
                reasons.append(f"compiler_only_output {sym} is declared a duplicate "
                               f"but has no configured ROM home")
        elif homes.get(sym):
            reasons.append(f"compiler_only_output {sym} has configured ROM home(s) "
                           f"{homes[sym]}; it is not compiler-only")
    for sym, disposition in data_rows:
        if disposition == "deadstrip" and sym[:4] in ("_ZTI", "_ZTS", "_ZTV"):
            # Mirrors the rombuild.py guard, and must: the intact-object path
            # returns before rombuild's copy runs, so a guard there alone leaves
            # this route uncovered.  A plain ``deadstrip`` is the one disposition
            # never compared against the cartridge, and the ``elif homes.get(sym)``
            # below cannot catch a COINED class name because ``homes`` is keyed on
            # the symbols.txt spelling while _ZTI/_ZTS are LENGTH-PREFIXED mangled
            # strings -- a coined name misses on both the prefix and the body, and
            # the miss reads as "the ROM has no such record".
            reasons.append(f"compiler_only_output {sym} is an RTTI/vtable record "
                           f"banked as a plain deadstrip, which is never compared "
                           f"against the cartridge. If the class carries a coined "
                           f"name, rename it to the cartridge's RTTI spelling so "
                           f"the row resolves as deadstrip-data; if the ROM "
                           f"genuinely has no such record, drop the row")
        elif disposition == "deadstrip-data":
            if not homes.get(sym):
                reasons.append(f"compiler_only_output {sym} is declared compiler-only "
                               f"data but has no configured ROM home; a homeless "
                               f"object is a plain deadstrip")
        elif homes.get(sym):
            reasons.append(f"compiler_only_output {sym} has configured ROM home(s) "
                           f"{homes[sym]}; declare it deadstrip-data")
    data = [sym for sym, _ in data_rows]
    if reasons:
        return None, {"requested": wanted, "unhandled": unhandled, "data": data}, reasons
    if not wanted and not data:
        return obj_bytes, {"requested": [], "deadstripped": [], "data": [],
                           "dataExternalized": []}, []

    out, plan = OI.derive_deadstrip(obj_bytes, wanted + data)
    if out is None:
        return None, {"requested": wanted, "objisolate": plan}, \
            [f"compiler-only deadstrip refused: {plan.get('error')}"]
    return out, {"requested": wanted, "deadstripped": plan.get("dead", []),
                 "droppedSections": plan.get("drop", []), "data": data,
                 "dataExternalized": plan.get("externalise", [])}, []


def manifest_externalized_output(entry):
    """Normalize exact vague-RTTI imports declared by a TU manifest entry.

    This schema intentionally describes individual definitions, not a linkage class.
    It cannot say "all vague symbols" and it cannot externalize vtables.  Each row
    names one dedicated ``_ZTI``/``_ZTS`` section, its unique configured canonical
    home, and every relocation in the canonical copy::

        {"symbol": "_ZTI4Base", "disposition": "canonical-import",
         "section": ".data", "binding": "STB_LOPROC", "size": "0x8",
         "canonical_module": "arm9", "canonical_address": "0x02001000",
         "reason": "inherited RTTI copy",
         "relocations": [{"offset": 0, "type": "R_ARM_ABS32", "kind": "load",
                           "symbol": "_ZTVN3abi17__class_type_infoE", "addend": 8,
                           "target_module": "arm9",
                           "target_address": "0x02002000"}]}

    The nested relocation rows are evidence, not linker aliases.  This scratch-only
    pass drops their sections before linking, so ABI-vtable address-point rewriting
    remains outside its scope and any surviving instance still fails closed.
    """
    raw = entry.get("externalized_output", [])
    if raw is None:
        raw = []
    if not isinstance(raw, list):
        return [], ["externalized_output must be a list"]

    rows, reasons, seen = [], [], set()
    for i, item in enumerate(raw):
        label = f"externalized_output[{i}]"
        if not isinstance(item, dict):
            reasons.append(f"{label} is not an object")
            continue
        symbol = item.get("symbol")
        if not symbol:
            reasons.append(f"{label} has no symbol")
            continue
        if symbol in seen:
            reasons.append(f"duplicate externalized_output policy for {symbol}")
            continue
        seen.add(symbol)
        if not symbol.startswith(("_ZTI", "_ZTS")):
            reasons.append(f"{label} {symbol} is not an _ZTI/_ZTS RTTI object")
        if item.get("disposition") != "canonical-import":
            reasons.append(f"{label} {symbol} has unsupported disposition "
                           f"{item.get('disposition')!r}")
        if item.get("section") != ".data":
            reasons.append(f"{label} {symbol} must name a .data definition")
        if item.get("binding") != "STB_LOPROC":
            reasons.append(f"{label} {symbol} must require STB_LOPROC binding")
        if not str(item.get("reason", "")).strip():
            reasons.append(f"{label} {symbol} needs a non-empty reason")

        try:
            size = int(item["size"], 0) if isinstance(item["size"], str) \
                else int(item["size"])
            address = int(item["canonical_address"], 0) \
                if isinstance(item["canonical_address"], str) \
                else int(item["canonical_address"])
        except (KeyError, TypeError, ValueError):
            reasons.append(f"{label} {symbol} needs valid size and canonical_address")
            continue
        if size <= 0:
            reasons.append(f"{label} {symbol} has non-positive size 0x{size:x}")
        module = item.get("canonical_module")
        if not isinstance(module, str) or not module:
            reasons.append(f"{label} {symbol} needs canonical_module")
            continue
        module = RL.normalize_module(module)

        relocs = item.get("relocations")
        if not isinstance(relocs, list):
            reasons.append(f"{label} {symbol} relocations must be a list")
            continue
        normalized_relocs, offsets = [], set()
        for j, reloc in enumerate(relocs):
            rlabel = f"{label}.relocations[{j}]"
            if not isinstance(reloc, dict):
                reasons.append(f"{rlabel} is not an object")
                continue
            required = ("offset", "type", "kind", "symbol", "addend",
                        "target_module", "target_address")
            missing = [key for key in required if key not in reloc]
            if missing:
                reasons.append(f"{rlabel} is missing {missing}")
                continue
            try:
                offset = int(reloc["offset"], 0) if isinstance(reloc["offset"], str) \
                    else int(reloc["offset"])
                addend = int(reloc["addend"], 0) if isinstance(reloc["addend"], str) \
                    else int(reloc["addend"])
                target = int(reloc["target_address"], 0) \
                    if isinstance(reloc["target_address"], str) \
                    else int(reloc["target_address"])
            except (TypeError, ValueError):
                reasons.append(f"{rlabel} has an invalid offset/addend/target_address")
                continue
            if offset in offsets:
                reasons.append(f"{label} {symbol} has duplicate relocation offset "
                               f"0x{offset:x}")
                continue
            offsets.add(offset)
            if offset < 0 or offset + 4 > size or offset & 3:
                reasons.append(f"{rlabel} offset 0x{offset:x} is not one aligned word "
                               f"inside size 0x{size:x}")
            if reloc["type"] != "R_ARM_ABS32" or reloc["kind"] != "load":
                reasons.append(f"{rlabel} must be R_ARM_ABS32/load")
            if not reloc["symbol"]:
                reasons.append(f"{rlabel} has an empty symbol")
            normalized = dict(reloc)
            normalized.update({"offset": offset, "addend": addend,
                               "target_address": target,
                               "target_module": RL.normalize_module(
                                   reloc["target_module"])})
            normalized_relocs.append(normalized)

        row = dict(item)
        row.update({"size": size, "canonical_address": address,
                    "canonical_module": module, "relocations": normalized_relocs})
        rows.append(row)
    return rows, reasons


def verify_externalized_output(obj_bytes, entry, policies=None, homes=None,
                               config_relocs=None, target_reader=None,
                               name_index=None):
    """Prove exact manifest-listed RTTI copies equal their configured homes."""
    if policies is None:
        policies, structural = manifest_externalized_output(entry)
    else:
        structural = []
    if structural:
        return {"ok": False, "rows": [], "errors": structural}
    if not policies:
        return {"ok": True, "rows": [], "errors": []}

    homes = all_symbol_homes() if homes is None else homes
    config_relocs = RA.build_config_relocs() if config_relocs is None else config_relocs
    target_reader = BP.target_bytes if target_reader is None else target_reader
    name_index = RA.build_name_index() if name_index is None else name_index
    elf = ELFFile(io.BytesIO(obj_bytes))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return {"ok": False, "rows": [], "errors": ["no .symtab"]}
    syms = list(symtab.iter_symbols())
    licensed = {f["symbol"] for f in entry.get("functions", [])}
    licensed |= {row["symbol"] for _sec, row in manifest_owned_symbol_rows(entry)}

    report_rows, reasons, used_sections = [], [], set()
    for policy in policies:
        name = policy["symbol"]
        prefix = f"externalized_output {name}"
        row_reasons = []
        matches = [s for s in syms if s.name == name
                   and s["st_shndx"] not in ("SHN_UNDEF", "SHN_ABS")]
        if name in licensed:
            row_reasons.append("is also licensed by the TU manifest")
        configured = {(RL.normalize_module(mod), addr)
                      for mod, addr in homes.get(name, [])}
        canonical = (policy["canonical_module"], policy["canonical_address"])
        if configured != {canonical}:
            row_reasons.append(f"configured homes are {sorted(configured)}, expected the "
                               f"unique canonical home {canonical}")
        if len(matches) != 1:
            row_reasons.append(f"has {len(matches)} defined ELF symbols, expected one")
            reasons.extend(f"{prefix}: {reason}" for reason in row_reasons)
            report_rows.append({"symbol": name, "ok": False,
                                "errors": row_reasons})
            continue

        sym = matches[0]
        shndx = sym["st_shndx"]
        sec = secs[shndx] if isinstance(shndx, int) and shndx < len(secs) else None
        if sym["st_info"]["bind"] != "STB_LOPROC" \
                or sym["st_info"]["type"] != "STT_OBJECT":
            row_reasons.append(f"ELF binding/type is {sym['st_info']['bind']}/"
                               f"{sym['st_info']['type']}, expected STB_LOPROC/STT_OBJECT")
        if sec is None or sec.name != ".data" \
                or sec.header["sh_type"] != "SHT_PROGBITS" \
                or not sec.header["sh_size"]:
            row_reasons.append("definition is not in a non-empty .data/SHT_PROGBITS section")
        if sec is not None:
            occupants = sorted(s.name for s in syms
                               if s.name and not s.name.startswith("$")
                               and s["st_shndx"] == shndx
                               and s["st_info"]["type"] != "STT_SECTION")
            if occupants != [name]:
                row_reasons.append(f"section[{shndx}] also defines "
                                   f"{[n for n in occupants if n != name]}")
            if shndx in used_sections:
                row_reasons.append(f"shares section[{shndx}] with another policy row")
            used_sections.add(shndx)
            if sym["st_value"] != 0 or sym["st_size"] != policy["size"] \
                    or sec.header["sh_size"] != policy["size"]:
                row_reasons.append(f"value/sizes are value=0x{sym['st_value']:x}, "
                                   f"symbol=0x{sym['st_size']:x}, "
                                   f"section=0x{sec.header['sh_size']:x}, manifest="
                                   f"0x{policy['size']:x}")

        emitted_relocs = {}
        if sec is not None:
            for relsec in secs:
                if not isinstance(relsec, RelocationSection) \
                        or relsec.header["sh_info"] != shndx:
                    continue
                if not relsec.is_RELA():
                    row_reasons.append(f"section[{shndx}] uses REL, not RELA")
                    continue
                for reloc in relsec.iter_relocations():
                    offset = reloc["r_offset"]
                    if offset in emitted_relocs:
                        row_reasons.append(f"duplicate emitted relocation at 0x{offset:x}")
                    target = symtab.get_symbol(reloc["r_info_sym"])
                    emitted_relocs[offset] = {
                        "offset": offset,
                        "type": _ELF_RELOC_NAME.get(reloc["r_info_type"],
                                                    reloc["r_info_type"]),
                        "kind": _NON_TEXT_RELOC_KIND.get(reloc["r_info_type"]),
                        "symbol": target.name, "addend": reloc["r_addend"],
                    }
        expected_relocs = {reloc["offset"]: reloc for reloc in policy["relocations"]}
        if set(emitted_relocs) != set(expected_relocs):
            row_reasons.append(f"relocation offsets are {sorted(emitted_relocs)}, expected "
                               f"{sorted(expected_relocs)}")
        cfgmap = config_relocs.get(policy["canonical_module"], {})
        for offset, expected in sorted(expected_relocs.items()):
            emitted = emitted_relocs.get(offset)
            if emitted is not None:
                for field in ("type", "kind", "symbol", "addend"):
                    if emitted[field] != expected[field]:
                        row_reasons.append(f"relocation +0x{offset:x} {field} is "
                                           f"{emitted[field]!r}, expected "
                                           f"{expected[field]!r}")
                resolved = RA.resolve_candidate(emitted["symbol"], name_index)
                if resolved is None:
                    row_reasons.append(f"relocation +0x{offset:x} symbol "
                                       f"{emitted['symbol']} is unresolved")
                else:
                    candidate_module = (RL.normalize_module(resolved[0])
                                        if resolved[0] is not None else None)
                    candidate_address = resolved[1] + emitted["addend"]
                    # mwcc's raw `_ZTV` relocation is relative to the storage
                    # object, while symbols.txt names the public slot-array address
                    # after the two-word ABI preamble.  This is the same raw-object
                    # convention used by reloc_audit.object_reloc_dests: addend 8
                    # resolves to the configured address point, not eight bytes past
                    # it.  Explicit addend-zero references already use the public
                    # convention and remain unchanged.
                    if emitted["symbol"].startswith("_ZTV") \
                            and emitted["addend"] >= OI.VTABLE_PREAMBLE:
                        candidate_address -= OI.VTABLE_PREAMBLE
                    if (candidate_module, candidate_address) != \
                            (expected["target_module"], expected["target_address"]):
                        row_reasons.append(f"relocation +0x{offset:x} resolves to "
                                           f"{candidate_module}:0x{candidate_address:08x}, "
                                           f"expected {expected['target_module']}:"
                                           f"0x{expected['target_address']:08x}")
            cfg = cfgmap.get(policy["canonical_address"] + offset)
            want_cfg = (expected["kind"], expected["target_address"],
                        expected["target_module"])
            got_cfg = ((cfg[0], cfg[1], RL.normalize_module(cfg[2])) if cfg else None)
            if got_cfg != want_cfg:
                row_reasons.append(f"canonical relocation +0x{offset:x} is {got_cfg}, "
                                   f"expected {want_cfg}")
        configured_offsets = {source - policy["canonical_address"] for source in cfgmap
                              if policy["canonical_address"] <= source
                              < policy["canonical_address"] + policy["size"]}
        if configured_offsets != set(expected_relocs):
            row_reasons.append(f"configured relocation offsets are "
                               f"{sorted(configured_offsets)}, expected "
                               f"{sorted(expected_relocs)}")

        target = target_reader(policy["canonical_module"],
                               policy["canonical_address"], policy["size"])
        if target is None or len(target) != policy["size"]:
            row_reasons.append("canonical target bytes are unavailable or short")
            differing = policy["size"]
        elif sec is None:
            differing = policy["size"]
        else:
            wildcard = {byte for offset in emitted_relocs for byte in range(offset, offset + 4)}
            differing = sum(1 for i, (left, right) in enumerate(zip(sec.data(), target))
                            if i not in wildcard and left != right)
            if differing:
                row_reasons.append(f"has {differing} non-relocated byte(s) different "
                                   "from its canonical home")

        reasons.extend(f"{prefix}: {reason}" for reason in row_reasons)
        report_rows.append({"symbol": name,
                            "canonicalModule": policy["canonical_module"],
                            "canonicalAddress": f"0x{policy['canonical_address']:08x}",
                            "size": policy["size"], "relocCount": len(emitted_relocs),
                            "differingBytes": differing, "ok": not row_reasons,
                            "errors": row_reasons})
    return {"ok": not reasons, "rows": report_rows, "errors": reasons}


def apply_externalized_output_policy(obj_bytes, entry, homes=None,
                                     config_relocs=None, target_reader=None,
                                     name_index=None):
    """Verify then externalize exact RTTI definitions in a scratch TU object."""
    policies, structural = manifest_externalized_output(entry)
    if structural:
        return None, {"requested": [row.get("symbol") for row in policies],
                      "verification": {"ok": False, "rows": [],
                                       "errors": structural}}, structural
    if not policies:
        return obj_bytes, {"requested": [], "externalized": [],
                           "verification": {"ok": True, "rows": [], "errors": []}}, []
    verification = verify_externalized_output(
        obj_bytes, entry, policies=policies, homes=homes,
        config_relocs=config_relocs, target_reader=target_reader,
        name_index=name_index)
    if not verification["ok"]:
        return None, {"requested": [row["symbol"] for row in policies],
                      "verification": verification}, verification["errors"]
    wanted = [row["symbol"] for row in policies]
    out, plan = OI.derive_externalized(obj_bytes, wanted)
    if out is None:
        reason = f"exact RTTI externalization refused: {plan.get('error')}"
        return None, {"requested": wanted, "verification": verification,
                      "objisolate": plan}, [reason]
    return out, {"requested": wanted, "externalized": plan.get("externalise", []),
                 "droppedSections": plan.get("drop", []),
                 "verification": verification}, []


def _align_up(value, alignment):
    alignment = max(1, int(alignment or 1))
    return (value + alignment - 1) & ~(alignment - 1)


def section_contribution(obj_bytes, section_name, start):
    """Lay out one object's repeated input sections as mwldarm's wildcard sees them.

    Returns bytes (zeroes for NOBITS), relocation records with contribution-relative
    offsets, and linked addresses for every symbol in those sections.  Section order,
    alignment, and ``sh_info`` are read from the real ELF rather than inferred from
    section names -- mwccarm emits many identically named `.data` sections in one TU.
    """
    elf = ELFFile(io.BytesIO(obj_bytes))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return None, "no .symtab"
    selected = [(i, s) for i, s in enumerate(secs)
                if s.name == section_name and s.header["sh_type"] in OI.CONTENT
                and s.header["sh_size"]]
    if not selected:
        return None, f"object emits no non-empty {section_name} section"

    image = bytearray()
    offsets = {}
    for shndx, sec in selected:
        absolute = _align_up(start + len(image), sec.header["sh_addralign"])
        image.extend(b"\0" * (absolute - (start + len(image))))
        offsets[shndx] = len(image)
        size = sec.header["sh_size"]
        image.extend(b"\0" * size if sec.header["sh_type"] == "SHT_NOBITS"
                     else sec.data())

    symbols = {}
    syms = list(symtab.iter_symbols())
    for sym in syms:
        shndx = sym["st_shndx"]
        if sym.name and shndx in offsets:
            symbols[sym.name] = {
                "address": start + offsets[shndx] + sym["st_value"],
                "size": sym["st_size"], "bind": sym["st_info"]["bind"],
                "type": sym["st_info"]["type"], "sectionIndex": shndx,
            }

    relocs = []
    for rel in secs:
        shndx = rel.header["sh_info"] if isinstance(rel, RelocationSection) else None
        if shndx not in offsets:
            continue
        if not rel.is_RELA():
            return None, f"section[{shndx}] {section_name} uses REL, not RELA"
        for r in rel.iter_relocations():
            sym = symtab.get_symbol(r["r_info_sym"])
            relocs.append({
                "offset": offsets[shndx] + r["r_offset"],
                "type": r["r_info_type"], "symbol": sym.name,
                "addend": r["r_addend"], "targetSection": sym["st_shndx"],
            })
    return {"bytes": bytes(image), "relocs": relocs, "symbols": symbols,
            "inputSections": [i for i, _s in selected]}, None


_NON_TEXT_RELOC_KIND = {
    1: "arm_call",  # R_ARM_PC24
    2: "load",  # R_ARM_ABS32
    10: "thumb_call",  # R_ARM_THM_CALL (fail closed unless config says the same)
    28: "arm_call",  # R_ARM_CALL
    29: "arm_call",  # R_ARM_JUMP24
}
_ELF_RELOC_NAME = {
    1: "R_ARM_PC24", 2: "R_ARM_ABS32", 10: "R_ARM_THM_CALL",
    28: "R_ARM_CALL", 29: "R_ARM_JUMP24",
}


def manifest_relocation_claims(entry):
    """Exact per-word non-text relocation licenses keyed by absolute source address."""
    out, reasons = {}, []
    raw = entry.get("relocations", [])
    if not isinstance(raw, list):
        return {}, ["relocations must be a list"]
    required = ("section", "source", "type", "kind", "symbol", "addend",
                "target_module", "target_address")
    for i, row in enumerate(raw):
        if not isinstance(row, dict):
            reasons.append(f"relocations[{i}] is not an object")
            continue
        missing = [key for key in required if key not in row]
        if missing:
            reasons.append(f"relocations[{i}] is missing {missing}")
            continue
        try:
            source = int(row["source"], 0) if isinstance(row["source"], str) \
                else int(row["source"])
            addend = int(row["addend"], 0) if isinstance(row["addend"], str) \
                else int(row["addend"])
            target = int(row["target_address"], 0) \
                if isinstance(row["target_address"], str) else int(row["target_address"])
        except (TypeError, ValueError):
            reasons.append(f"relocations[{i}] has an invalid source/addend/target_address")
            continue
        if source in out:
            reasons.append(f"duplicate relocation claim at 0x{source:08x}")
            continue
        normalized = dict(row)
        normalized.update({"source": source, "addend": addend,
                           "target_address": target,
                           "target_module": RL.normalize_module(row["target_module"])})
        out[source] = normalized
    return out, reasons


def _configured_bss_boundaries(module, homes):
    """Independent addresses that may delimit an owned BSS contribution."""
    normalized = RL.normalize_module(module)
    out = {addr for locations in homes.values() for mod, addr in locations
           if RL.normalize_module(mod) == normalized}
    path = module_config_dir(normalized) / "delinks.txt"
    if path.is_file():
        header, _entries = parse_delinks_file(path)
        for name, start, end in EN.sections(header):
            if name == ".bss":
                out.update((start, end))
    return out


def verify_owned_sections(obj_bytes, entry, claims, name_index=None,
                          config_relocs=None, sym_index=None, target_reader=None,
                          symbol_homes=None, bss_boundaries=None,
                          public_address_points=False):
    """Verify licensed non-text layout, bytes, symbols, and relocation destinations.

    Relocated words are wildcarded only after every relocation is independently tied
    to the module's configured source/destination pair.  Candidate addends participate
    in the destination address; this is stricter than the historical function helper,
    which compares only a symbol's base address.  BSS has no ROM bytes, so its proof is
    exact contribution size plus symbol addresses here and the scratch ELF symbol/module
    checks later in ``linkcheck``.
    """
    nontext = [c for c in claims if c["name"] != ".text"]
    if not nontext:
        return {"ok": True, "claimed": 0, "rows": [], "symbolErrors": []}
    name_index = RA.build_name_index() if name_index is None else name_index
    config_relocs = RA.build_config_relocs() if config_relocs is None else config_relocs
    sym_index = RL.load_all_syms() if sym_index is None else sym_index
    target_reader = BP.target_bytes if target_reader is None else target_reader
    cfgmap = config_relocs.get(RL.normalize_module(entry["module"]), {})
    owner_module = RL.normalize_module(entry["module"])
    symbol_homes = all_symbol_homes() if symbol_homes is None else symbol_homes
    bss_boundaries = (_configured_bss_boundaries(owner_module, symbol_homes)
                      if bss_boundaries is None else set(bss_boundaries))

    layouts, rows, reasons = {}, [], []
    expected_relocs, relocation_claim_errors = manifest_relocation_claims(entry)
    reasons.extend(relocation_claim_errors)
    for claim in nontext:
        layout, error = section_contribution(obj_bytes, claim["name"], claim["start"])
        if error:
            reasons.append(error)
            rows.append({"section": claim["name"], "ok": False, "error": error})
            continue
        layouts[claim["name"]] = layout
        want = claim["end"] - claim["start"]
        size_ok = len(layout["bytes"]) == want
        byte_ok, ndiff = True, 0
        if claim["name"] != ".bss" and size_ok:
            target = target_reader(entry["module"], claim["start"], want)
            if target is None or len(target) != want:
                byte_ok, ndiff = False, want
            else:
                wildcard = {r["offset"] & ~3 for r in layout["relocs"]}
                byte_ok, ndiff = M.compare(target, layout["bytes"], wildcard, verbose=False)
        elif not size_ok:
            byte_ok, ndiff = False, 999
        rows.append({"section": claim["name"], "start": f"0x{claim['start']:08x}",
                     "end": f"0x{claim['end']:08x}", "emittedBytes": len(layout["bytes"]),
                     "sizeOk": size_ok, "bytesOk": byte_ok,
                     "differingWords": ndiff, "relocCount": len(layout["relocs"])})
        if not size_ok:
            reasons.append(f"{claim['name']} emits 0x{len(layout['bytes']):x} bytes, "
                           f"manifest claims 0x{want:x}")
        if not byte_ok and size_ok:
            reasons.append(f"{claim['name']} has {ndiff} non-relocated word(s) different")

    linked_symbols = {name: row for layout in layouts.values()
                      for name, row in layout["symbols"].items()}
    expected_rows = manifest_owned_symbol_rows(entry)
    if public_address_points:
        expected_rows += manifest_storage_alias_rows(entry)
    expected_names = {r["symbol"] for _sec, r in expected_rows}
    emitted_names = {name for layout in layouts.values() for name, row in layout["symbols"].items()
                     if row["type"] in ("STT_FUNC", "STT_OBJECT") and not name.startswith("$")}
    for name in sorted(emitted_names - expected_names):
        reasons.append(f"emitted non-text symbol {name} is not licensed by data/bss/"
                       f"rodata/init/ctor")
    for section, expected in expected_rows:
        name = expected["symbol"]
        got = layouts.get(section, {}).get("symbols", {}).get(name)
        public_addr = _manifest_addr(expected)
        want_addr, address_error = _manifest_emitted_addr(expected)
        address_bias = 0
        if public_address_points and name.startswith("_ZTV") \
                and expected.get("emitted_storage_address") is not None:
            try:
                address_bias = (int(expected["address_point_bias"], 0)
                                if isinstance(expected["address_point_bias"], str)
                                else int(expected["address_point_bias"]))
            except (KeyError, TypeError, ValueError):
                address_error = "public-address-point verification needs a valid bias"
            else:
                want_addr = public_addr
        if got is None:
            reasons.append(f"licensed {section} symbol {name} is not emitted there")
            continue
        if address_error:
            reasons.append(f"licensed {section} symbol {name}: {address_error}")
        elif want_addr is None:
            reasons.append(f"licensed {section} symbol {name} has no valid address")
        elif got["address"] != want_addr:
            reasons.append(f"licensed {section} symbol {name} links at "
                           f"0x{got['address']:08x}, manifest emitted address says "
                           f"0x{want_addr:08x}")
        if section == ".bss" and public_addr is not None:
            configured = {(RL.normalize_module(mod), addr)
                          for mod, addr in symbol_homes.get(name, [])}
            if (owner_module, public_addr) not in configured:
                reasons.append(f"licensed .bss symbol {name} at 0x{public_addr:08x} has no "
                               f"independent {owner_module} symbols.txt home")
        if expected.get("size") is not None:
            try:
                want_size = int(expected["size"], 0) if isinstance(expected["size"], str) \
                    else int(expected["size"])
            except (TypeError, ValueError):
                reasons.append(f"licensed {section} symbol {name} has invalid size")
            else:
                if public_address_points and address_bias:
                    want_size -= address_bias
                if want_size <= 0:
                    reasons.append(f"licensed {section} symbol {name} has no bytes after "
                                   f"address-point bias 0x{address_bias:x}")
                elif got["size"] != want_size:
                    reasons.append(f"licensed {section} symbol {name} size 0x{got['size']:x}, "
                                   f"manifest says 0x{want_size:x}")

    for claim in (c for c in nontext if c["name"] == ".bss"):
        if claim["start"] not in bss_boundaries or claim["end"] not in bss_boundaries:
            reasons.append(f".bss claim 0x{claim['start']:08x}..0x{claim['end']:08x} "
                           "does not have independently configured boundary anchors")

    # All owned symbols are now resolvable even when symbols.txt has not named one yet.
    # Relocations use the ELF symbol's storage address; the public address-point bias is
    # carried in the relocation addend and checked below.
    manifest_addrs = {}
    for _sec, row in expected_rows:
        emitted_addr, _error = _manifest_emitted_addr(row)
        if public_address_points and row["symbol"].startswith("_ZTV") \
                and row.get("emitted_storage_address") is not None:
            emitted_addr = _manifest_addr(row)
        manifest_addrs[row["symbol"]] = emitted_addr
    manifest_addrs.update({f["symbol"]: int(f["address"], 0)
                           for f in entry.get("functions", [])})
    reloc_rows = []
    for claim in nontext:
        layout = layouts.get(claim["name"])
        if layout is None:
            continue
        candidate_offsets = set()
        for rel in layout["relocs"]:
            source = claim["start"] + rel["offset"]
            candidate_offsets.add(source)
            base = None
            candidate_module = None
            if rel["symbol"] in linked_symbols:
                base = linked_symbols[rel["symbol"]]["address"]
                candidate_module = owner_module
            elif manifest_addrs.get(rel["symbol"]) is not None:
                base = manifest_addrs[rel["symbol"]]
                candidate_module = owner_module
            else:
                resolved = RA.resolve_candidate(rel["symbol"], name_index)
                if resolved:
                    candidate_module = (RL.normalize_module(resolved[0])
                                        if resolved[0] is not None else None)
                    base = resolved[1]
            cand_addr = base + rel["addend"] if base is not None else None
            cfg = cfgmap.get(source)
            expected = expected_relocs.get(source)
            configured_module = RL.normalize_module(cfg[2]) if cfg else None
            candidate_kind = _NON_TEXT_RELOC_KIND.get(rel["type"])
            if cfg is None:
                verdict = "EXTRA"
            elif expected is None:
                verdict = "UNLICENSED-RELOC"
            elif cand_addr is None:
                verdict = "UNRESOLVED"
            elif expected["section"] != claim["name"]:
                verdict = "WRONG-SECTION"
            elif _ELF_RELOC_NAME.get(rel["type"], rel["type"]) != expected["type"]:
                verdict = "WRONG-TYPE"
            elif rel["symbol"] != expected["symbol"]:
                verdict = "WRONG-SYMBOL"
            elif rel["addend"] != expected["addend"]:
                verdict = "WRONG-ADDEND"
            elif candidate_kind != expected["kind"] or expected["kind"] != cfg[0]:
                verdict = "WRONG-KIND"
            elif candidate_module != expected["target_module"] \
                    or expected["target_module"] != configured_module:
                verdict = "WRONG-MODULE"
            elif cand_addr != expected["target_address"] \
                    or expected["target_address"] != cfg[1]:
                verdict = "WRONG-DEST"
            else:
                verdict = "OK"
            reloc_rows.append({"section": claim["name"], "source": f"0x{source:08x}",
                               "type": rel["type"], "symbol": rel["symbol"],
                               "kind": candidate_kind, "addend": rel["addend"],
                               "expectedType": expected.get("type") if expected else None,
                               "expectedSymbol": expected.get("symbol") if expected else None,
                               "expectedAddend": expected.get("addend") if expected else None,
                               "candidateModule": candidate_module,
                               "candidate": f"0x{cand_addr:08x}" if cand_addr is not None else None,
                               "configuredKind": cfg[0] if cfg else None,
                               "configuredModule": configured_module,
                               "configured": f"0x{cfg[1]:08x}" if cfg else None,
                               "verdict": verdict})
        for source in sorted(a for a in cfgmap
                             if claim["start"] <= a < claim["end"]
                             and a not in candidate_offsets):
            reloc_rows.append({"section": claim["name"], "source": f"0x{source:08x}",
                               "verdict": "MISSING", "configured": f"0x{cfgmap[source][1]:08x}"})
    claimed_ranges = [(c["start"], c["end"]) for c in nontext]
    for source, expected in sorted(expected_relocs.items()):
        if not any(start <= source < end for start, end in claimed_ranges):
            reasons.append(f"relocation claim 0x{source:08x} is outside every owned "
                           "non-.text section")
        elif not any(r["source"] == f"0x{source:08x}" for r in reloc_rows):
            reloc_rows.append({"section": expected["section"],
                               "source": f"0x{source:08x}", "verdict": "MISSING-OBJECT",
                               "configured": f"0x{expected['target_address']:08x}"})
    bad_relocs = [r for r in reloc_rows if r["verdict"] != "OK"]
    for r in bad_relocs:
        reasons.append(f"{r['section']} relocation {r['source']} {r['verdict']}"
                       + (f" ({r.get('symbol')} -> {r.get('candidate')}, expected "
                          f"{r.get('configured')})" if r.get("symbol") else ""))

    return {"ok": not reasons, "claimed": len(nontext), "rows": rows,
            "symbolErrors": [r for r in reasons if "symbol" in r],
            "relocations": reloc_rows, "errors": reasons}


def derive_owned_nontext_object(obj_bytes, entry, claims):
    """Reduce a post-policy TU object to its exact manifest-owned non-text surface.

    ``verify_owned_sections`` is the byte/symbol/relocation license.  This helper
    supplies the mechanical partition after that license exists: it keeps every
    repeated compiler input section named by a non-text claim, permits only symbols
    listed in the matching manifest fields, and delegates all ELF surgery and
    dropped-text import handling to :func:`objisolate.derive_section_partition`.
    """
    nontext = [claim for claim in claims if claim["name"] != ".text"]
    names = [claim["name"] for claim in nontext]
    licensed = sorted(row["symbol"] for section, row in manifest_owned_symbol_rows(entry)
                      if section in set(names))
    deferred = [{"symbol": f["symbol"], "section": ".text", "size": f["size"]}
                for f in entry.get("functions", [])]
    if not names:
        return None, {"requestedSections": [], "licensedSymbols": licensed,
                      "deferredOutputs": deferred}, \
            ["partitioned non-text object needs at least one non-.text claim"]

    out, plan = OI.derive_section_partition(obj_bytes, names, licensed, deferred)
    report = {"requestedSections": names, "licensedSymbols": licensed,
              "deferredOutputs": deferred, "objisolate": plan}
    if out is None:
        return None, report, [f"non-text object partition refused: {plan.get('error')}"]

    inv = elf_inventory(out)
    live = [section for section in inv["sections"]
            if section["size"] and section["type"] in OI.CONTENT
            and not any(section["name"].startswith(p) for p in OI.IGNORE)]
    foreign = [section for section in live if section["name"] not in set(names)]
    report["liveSections"] = live
    if foreign:
        detail = [f"{row['name']}[0x{row['size']:x}]" for row in foreign]
        return None, report, [f"partition retained foreign content section(s): {detail}"]
    return out, report, []


def manifest_storage_alias_rows(entry):
    """Nested storage-prefix aliases licensed only after vtable rebias."""
    rows = []
    for section, row in manifest_owned_symbol_rows(entry):
        alias = row.get("storage_alias")
        if isinstance(alias, dict):
            rows.append((section, {"symbol": alias.get("symbol"),
                                   "address": alias.get("address"),
                                   "size": alias.get("size")}))
    return rows


def linked_symbol_rows(path, names):
    """Exact linked-ELF metadata for the requested names, including output section."""
    path = pathlib.Path(path)
    if not path.is_file():
        return None, f"linked ELF does not exist: {path}"
    raw = path.read_bytes()
    elf = ELFFile(io.BytesIO(raw))
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return None, f"linked ELF has no .symtab: {path}"
    wanted = set(names)
    rows = collections.defaultdict(list)
    for sym in symtab.iter_symbols():
        if sym.name not in wanted:
            continue
        shndx = sym["st_shndx"]
        section_name = elf.get_section(shndx).name if isinstance(shndx, int) else str(shndx)
        rows[sym.name].append({"address": sym["st_value"], "size": sym["st_size"],
                               "binding": sym["st_info"]["bind"],
                               "type": sym["st_info"]["type"],
                               "visibility": sym["st_other"]["visibility"],
                               "sectionIndex": shndx, "section": section_name})
    return {name: rows.get(name, []) for name in wanted}, None


def content_tree_sha256(root):
    """Hash relative paths, lengths, and bytes for a deterministic config snapshot."""
    root = pathlib.Path(root)
    digest = hashlib.sha256()
    for path in sorted((path for path in root.rglob("*") if path.is_file()),
                       key=lambda path: path.relative_to(root).as_posix()):
        rel = path.relative_to(root).as_posix().encode("utf-8")
        raw = path.read_bytes()
        digest.update(len(rel).to_bytes(4, "big"))
        digest.update(rel)
        digest.update(len(raw).to_bytes(8, "big"))
        digest.update(raw)
    return digest.hexdigest()


BASELINE_CONTROL_TOOLS = tuple(
    REPO / "tools" / name for name in (
        "objisolate.py", "reloc_audit.py", "rombuild.py", "rombuild_cache.py",
        "rombuild_check.py", "rombuild_profile.py", "tu_manifest.py",
        "tu_production.py", "tubuild.py"))


def content_files_sha256(paths):
    """Hash an ordered set of tool paths and bytes without timestamps."""
    rows = []
    for path in map(pathlib.Path, paths):
        try:
            label = path.resolve().relative_to(REPO.resolve()).as_posix()
        except ValueError:
            label = path.name
        rows.append((label, path))
    digest = hashlib.sha256()
    for label, path in sorted(rows):
        raw_label = label.encode("utf-8")
        raw = path.read_bytes()
        digest.update(len(raw_label).to_bytes(4, "big"))
        digest.update(raw_label)
        digest.update(len(raw).to_bytes(8, "big"))
        digest.update(raw)
    return digest.hexdigest()


def partition_baseline_fingerprints(linked_elf, config_root=CFG_ARM9,
                                    dsd_path=None, linker_path=None,
                                    rom_inputs=None, control_tools=None,
                                    tracked_config_root=CFG_ARM9):
    """Content identities that bind a baseline report to its actual inputs/output.

    TWO config hashes, deliberately, because the baseline link no longer reads the
    tracked config directly.  ``linkcheck --baseline`` copies ``config/arm9`` into its
    own scratch and then MUTATES the copy (``demote_complete_sources``), so the tree
    that produced ``final_link.o`` is the scratch one, and only the scratch hash can
    answer "were these the inputs to this link?".

    Hashing only the scratch copy, though, silently drops the signal the tracked hash
    used to carry.  A baseline would stay "current" while ``config/arm9`` -- symbols,
    delinks, the whole enrollment surface -- moved underneath it, because an edit to
    the tracked tree cannot touch a preserved scratch copy.  The scratch hash proves
    the report was not tampered with; the tracked hash proves the world it was derived
    from has not moved.  Neither implies the other, so both are recorded and both are
    validated.

    When ``config_root`` is the tracked root -- the default, and every caller that is
    not the scratch baseline -- the two hashes agree by construction.
    """
    linked_elf = pathlib.Path(linked_elf)
    dsd_path = pathlib.Path(dsd_path or RB.DSD)
    linker_path = pathlib.Path(linker_path or (RB.MW / RB.LD_VERSION / "mwldarm.exe"))
    rom_inputs = pathlib.Path(rom_inputs or (REPO / "extracted" / "dsd"))
    control_tools = tuple(control_tools or BASELINE_CONTROL_TOOLS)
    tracked_config_root = pathlib.Path(tracked_config_root or CFG_ARM9)
    required = [linked_elf, dsd_path, linker_path, *control_tools]
    missing = [str(path) for path in required if not path.is_file()]
    if not rom_inputs.is_dir():
        missing.append(str(rom_inputs))
    if not tracked_config_root.is_dir():
        missing.append(str(tracked_config_root))
    if missing:
        raise FileNotFoundError(f"baseline fingerprint input(s) missing: {missing}")
    return {
        "configArm9Sha256": content_tree_sha256(config_root),
        "trackedConfigArm9Sha256": content_tree_sha256(tracked_config_root),
        "romInputsSha256": content_tree_sha256(rom_inputs),
        "controlToolsSha256": content_files_sha256(control_tools),
        "linkedElfSha256": hashlib.sha256(linked_elf.read_bytes()).hexdigest(),
        "linkedElfBytes": linked_elf.stat().st_size,
        "dsdSha256": hashlib.sha256(dsd_path.read_bytes()).hexdigest(),
        "mwldarmSha256": hashlib.sha256(linker_path.read_bytes()).hexdigest(),
    }


def validate_partition_baseline_evidence(report, linked_elf, config_root=CFG_ARM9,
                                         dsd_path=None, linker_path=None,
                                         rom_inputs=None, control_tools=None,
                                         tracked_config_root=CFG_ARM9):
    """Refuse a baseline whose report is detached from current bytes or tools.

    A report written before ``trackedConfigArm9Sha256`` existed does not carry the key,
    so it mismatches and is refused as stale.  That is the intended direction: a report
    that cannot answer "has the tracked config moved?" must not be trusted to say it
    has not.  Regenerating the baseline is the fix, and it is the same fix the tracked
    hash demanded before it was dropped.
    """
    evidence = report.get("baselineEvidence")
    if not isinstance(evidence, dict):
        return None, "baseline report has no content-bound evidence"
    try:
        current = partition_baseline_fingerprints(
            linked_elf, config_root, dsd_path=dsd_path, linker_path=linker_path,
            rom_inputs=rom_inputs, control_tools=control_tools,
            tracked_config_root=tracked_config_root)
    except (OSError, ValueError) as exc:
        return None, f"cannot fingerprint baseline: {exc}"
    mismatched = [key for key, value in current.items() if evidence.get(key) != value]
    if mismatched:
        return None, f"baseline content fingerprint differs for {mismatched}"
    return current["linkedElfSha256"], None


def _baseline_partition_symbols(names):
    """Load a successful, config-current baseline ELF as independent alias evidence."""
    report_path = BASELINE_LINK / "linkcheck.json"
    elf_path = BASELINE_LINK / "final_link.o"
    if not report_path.is_file() or not elf_path.is_file():
        return None, None, "run `tubuild.py linkcheck --baseline` first"
    try:
        report = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        return None, None, f"baseline linkcheck report is unreadable: {exc}"
    if report.get("baseline") is not True \
            or (report.get("phases", {}).get("link") or {}).get("ok") is not True \
            or (report.get("analysis") or {}).get("passed") is not True:
        return None, None, "baseline report does not prove a successful stock module link"
    # Both bindings, and for different reasons: the scratch config proves the preserved
    # baseline inputs were not edited after the link, and CFG_ARM9 proves the tracked
    # config has not drifted since. Validating only the scratch copy would accept a
    # baseline whose tracked inputs no longer exist in that shape.
    baseline_sha256, error = validate_partition_baseline_evidence(
        report, elf_path, config_root=BASELINE_LINK / "config" / "arm9",
        tracked_config_root=CFG_ARM9)
    if error:
        return None, None, error
    rows, error = linked_symbol_rows(elf_path, names)
    if error:
        return None, None, error
    return rows, baseline_sha256, None


def partition_vtable_rebiases(entry, claims, baseline_symbols=None,
                               baseline_sha256=None):
    """Exact public-address-point biases required by retained vtable definitions."""
    claimed = {claim["name"] for claim in claims if claim["name"] != ".text"}
    owner_module = RL.normalize_module(entry["module"])
    homes = all_symbol_homes()
    biases, reasons = {}, []
    compiler_only = {row.get("symbol"): row for row in
                     entry.get("compiler_only_output", []) if isinstance(row, dict)}
    for section, row in manifest_owned_symbol_rows(entry):
        name = row["symbol"]
        if section not in claimed or not name.startswith("_ZTV"):
            continue
        storage, error = _manifest_emitted_addr(row)
        if error or row.get("emitted_storage_address") is None:
            reasons.append(f"retained vtable {name} needs explicit, consistent "
                           f"emitted_storage_address/address_point_bias: {error or 'missing'}")
            continue
        try:
            bias = (int(row["address_point_bias"], 0)
                    if isinstance(row["address_point_bias"], str)
                    else int(row["address_point_bias"]))
            size = int(row["size"], 0) if isinstance(row.get("size"), str) \
                else int(row["size"])
        except (KeyError, TypeError, ValueError):
            reasons.append(f"retained vtable {name} needs valid bias and size")
            continue
        if bias <= 0 or bias >= size:
            reasons.append(f"retained vtable {name} bias 0x{bias:x} is not inside "
                           f"storage size 0x{size:x}")
            continue
        if name in biases:
            reasons.append(f"duplicate retained vtable row for {name}")
            continue
        public = _manifest_addr(row)
        configured = {(RL.normalize_module(module), address)
                      for module, address in homes.get(name, [])}
        expected_home = (owner_module, public)
        if public is None or configured != {expected_home}:
            rendered = [f"{module}:0x{address:08x}"
                        for module, address in sorted(configured)]
            reasons.append(f"retained vtable {name} needs one unique configured public "
                           f"home {owner_module}:"
                           f"{f'0x{public:08x}' if public is not None else 'invalid'}; "
                           f"found {rendered or 'none'}")
            continue
        policy = {"bias": bias, "size": size, "section": section,
                  "storageAddress": storage, "publicAddress": public}
        alias = row.get("storage_alias")
        if alias is not None:
            label = f"retained vtable {name} storage_alias"
            if not isinstance(alias, dict):
                reasons.append(f"{label} must be an object")
                continue
            try:
                alias_name = str(alias["symbol"])
                alias_address = (int(alias["address"], 0)
                                 if isinstance(alias["address"], str)
                                 else int(alias["address"]))
                alias_size = int(alias["size"], 0) if isinstance(alias["size"], str) \
                    else int(alias["size"])
                donor = str(alias["reuse_compiler_only_symbol"])
            except (KeyError, TypeError, ValueError):
                reasons.append(f"{label} needs symbol/address/size/"
                               "reuse_compiler_only_symbol")
                continue
            if not alias_name or alias_name.startswith("_ZTV"):
                reasons.append(f"{label} has invalid symbol {alias_name!r}")
            if alias_address != storage or alias_size != bias:
                reasons.append(f"{label} must exactly cover the preamble "
                               f"0x{storage:08x}..0x{public:08x}")
            if alias.get("binding") != "STB_GLOBAL" or alias.get("type") != "STT_OBJECT" \
                    or alias.get("visibility") != "STV_DEFAULT":
                reasons.append(f"{label} must require GLOBAL/OBJECT/DEFAULT visibility")
            donor_row = compiler_only.get(donor)
            if not donor_row or donor_row.get("disposition") != "deadstrip":
                reasons.append(f"{label} donor {donor} is not an explicit compiler-only "
                               "deadstrip policy")
            alias_homes = {(RL.normalize_module(module), address)
                           for module, address in homes.get(alias_name, [])}
            if alias_homes != {(owner_module, storage)}:
                rendered = [f"{module}:0x{address:08x}"
                            for module, address in sorted(alias_homes)]
                reasons.append(f"{label} needs one unique configured storage home "
                               f"{owner_module}:0x{storage:08x}; found "
                               f"{rendered or 'none'}")
            policy["storageAlias"] = {
                "symbol": alias_name, "address": alias_address, "size": alias_size,
                "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                "visibility": "STV_DEFAULT", "donor": donor,
            }
        biases[name] = policy

    aliases = [(name, policy["storageAlias"]) for name, policy in biases.items()
               if policy.get("storageAlias")]
    if aliases:
        wanted = {name for name, _alias in aliases} | \
                 {alias["symbol"] for _name, alias in aliases}
        if baseline_symbols is None:
            baseline_symbols, baseline_sha256, error = _baseline_partition_symbols(wanted)
            if error:
                reasons.append(f"storage alias baseline proof unavailable: {error}")
                baseline_symbols = {}
        for name, alias in aliases:
            base_alias = (baseline_symbols or {}).get(alias["symbol"], [])
            base_vtable = (baseline_symbols or {}).get(name, [])
            if len(base_alias) != 1 or len(base_vtable) != 1:
                reasons.append(f"storage alias baseline needs one {alias['symbol']} and one "
                               f"{name}; found {len(base_alias)}/{len(base_vtable)}")
                continue
            got_alias, got_vtable = base_alias[0], base_vtable[0]
            expected_alias = (alias["address"], alias["size"], alias["binding"],
                              alias["type"], alias["visibility"])
            actual_alias = (got_alias["address"], got_alias["size"],
                            got_alias["binding"], got_alias["type"],
                            got_alias["visibility"])
            expected_vtable = (biases[name]["publicAddress"],
                               biases[name]["size"] - biases[name]["bias"],
                               "STB_GLOBAL", "STT_OBJECT", "STV_DEFAULT")
            actual_vtable = (got_vtable["address"], got_vtable["size"],
                             got_vtable["binding"], got_vtable["type"],
                             got_vtable["visibility"])
            if actual_alias != expected_alias or actual_vtable != expected_vtable \
                    or got_alias["sectionIndex"] != got_vtable["sectionIndex"]:
                reasons.append(f"storage alias baseline metadata differs for "
                               f"{alias['symbol']}/{name}")
                continue
            biases[name]["storageAlias"]["baseline"] = {
                "elfSha256": baseline_sha256, "alias": got_alias,
                "vtable": got_vtable,
            }
    return biases, reasons


def verify_linked_storage_aliases(linked_elf, biases):
    """Require final-link alias/vtable metadata to reproduce the baseline pair."""
    pairs = [(name, policy["storageAlias"]) for name, policy in biases.items()
             if policy.get("storageAlias")]
    if not pairs:
        return {"ok": True, "rows": [], "errors": []}
    wanted = {name for name, _alias in pairs} | {alias["symbol"] for _name, alias in pairs}
    symbols, error = linked_symbol_rows(linked_elf, wanted)
    if error:
        return {"ok": False, "rows": [], "errors": [error]}
    rows, reasons = [], []
    for name, alias in pairs:
        got_alias = symbols.get(alias["symbol"], [])
        got_vtable = symbols.get(name, [])
        baseline = alias.get("baseline") or {}
        if len(got_alias) != 1 or len(got_vtable) != 1:
            reasons.append(f"linked storage split {alias['symbol']}/{name} occurs "
                           f"{len(got_alias)}/{len(got_vtable)} times")
            continue
        same = (got_alias[0] == baseline.get("alias")
                and got_vtable[0] == baseline.get("vtable")
                and got_alias[0]["sectionIndex"] == got_vtable[0]["sectionIndex"])
        rows.append({"alias": alias["symbol"], "vtable": name,
                     "aliasMetadata": got_alias[0], "vtableMetadata": got_vtable[0],
                     "baselineElfSha256": baseline.get("elfSha256"), "exact": same})
        if not same:
            reasons.append(f"linked storage split {alias['symbol']}/{name} does not "
                           "match baseline metadata")
    return {"ok": not reasons, "rows": rows, "errors": reasons}


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
                         f"{TUM.describe(MANIFEST)}")

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
    # bytes ("eligible is not enrolled"). Comparing against them is still
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


def shared_build_bin_snapshot():
    """Hash shared build/*.bin outputs so scratch redirection leaks are detectable."""
    return {path.name: {"bytes": path.stat().st_size,
                        "sha256": hashlib.sha256(path.read_bytes()).hexdigest()}
            for path in sorted((REPO / "build").glob("*.bin")) if path.is_file()}


def compile_linkcheck_sources(srcs, vers, cache, init_srcs, syms, build_root, jobs,
                              intact_tus_override=None):
    """Compile a scratch linkcheck with the normal production object policies.

    A baseline substitutes no candidate TU, but it still compiles production's
    already-promoted multi-symbol objects. Those objects rely on manifest-backed
    compiler-only policies for exact duplicate functions and data. Omitting the
    policies makes the control fail before it reaches the candidate, even though the
    normal ROM build accepts and verifies the same objects.
    """
    compiler_only = RB.compiler_only_policies(srcs)
    intact_tus = (RB.intact_tu_policies(srcs) if intact_tus_override is None
                  else intact_tus_override)
    failures, outcomes = [], collections.Counter()
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as ex:
        for rel, err, outcome in ex.map(
                lambda s: RB.compile_one(
                    s, vers, cache, init_srcs, syms, build_root=build_root,
                    compiler_only=compiler_only, intact_tus=intact_tus), srcs):
            outcomes[outcome] += 1
            if err:
                failures.append((rel, err))
    return failures, outcomes


def demote_complete_sources(config_root, sources):
    """Make exact enrolled sources ROM-gap-owned in a disposable config tree.

    The section claims remain unchanged; only the indented ``complete`` marker is
    removed. dsd then supplies those ranges from the extracted retail binaries.
    Every requested path must name exactly one complete entry or the control build
    is refused rather than silently compiling the source it was meant to exclude.
    """
    wanted = {str(source).replace("\\", "/") for source in sources}
    found, demoted = set(), set()
    for path in sorted(pathlib.Path(config_root).rglob("delinks.txt")):
        lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
        out, current, changed = [], None, False
        for line in lines:
            stripped = line.strip()
            if line and not line[0].isspace() and stripped.endswith(":"):
                current = stripped[:-1].replace("\\", "/")
                if current in wanted:
                    found.add(current)
            elif line and not line[0].isspace():
                current = None
            if current in wanted and stripped == "complete":
                if current in demoted:
                    return [], [f"{current}: duplicate complete marker"]
                demoted.add(current)
                changed = True
                continue
            out.append(line)
        if changed:
            path.write_text("\n".join(out) + "\n", encoding="utf-8", newline="\n")
    errors = [f"{source}: no delinks entry" for source in sorted(wanted - found)]
    errors.extend(f"{source}: delinks entry is not complete"
                  for source in sorted(found - demoted))
    return sorted(demoted), errors


def linkcheck_symbol_verdict(baseline, command_ok, new_errors):
    """Whether the symbol phase is attributable-clean for this linkcheck.

    The stock control's job is to inventory the tree's existing dsd errors. A
    candidate is clean only when it adds none to that inventory; without a control it
    must make the command itself pass.
    """
    if baseline:
        return True
    if new_errors is not None:
        return not new_errors
    return command_ok


def cmd_linkcheck(args):
    data = load_manifest()
    entry = manifest_entry(data, args.id) if args.id else None
    if args.id and entry is None:
        raise SystemExit(f"no manifest entry for {args.id!r} in "
                         f"{TUM.describe(MANIFEST)}")
    baseline = bool(args.baseline)
    partial = bool(getattr(args, "partial", False))
    partitioned = bool(getattr(args, "partitioned", False))
    if entry is None and not baseline:
        raise SystemExit("linkcheck needs a TU id (or --baseline for the control run)")
    if sum(bool(mode) for mode in (baseline, partial, partitioned)) > 1:
        raise SystemExit("--baseline, --partial, and --partitioned are mutually "
                         "exclusive link experiments")

    module = entry["module"] if entry else (args.module or "ov002")
    tu_id = entry["id"] if entry else "_baseline"
    # One canonical baseline tree for the whole repository, not one per module: the
    # control run substitutes nothing, so its result is module-independent and every
    # TU run diffs against the same artefact. The partial run gets its own tree beside
    # the whole-range one so a TU can hold both results at once -- they are different
    # experiments on the same source and neither supersedes the other.
    scratch = (BASELINE_LINK if baseline else BUILD_TU / sanitize_id(tu_id) /
               ("link-partitioned" if partitioned else "link-partial" if partial else "link"))
    cfg_root = scratch / "config" / "arm9"
    report = {"id": tu_id, "baseline": baseline, "partial": partial,
              "partitioned": partitioned, "module": module,
              "scratch": scratch.relative_to(REPO).as_posix(), "phases": {}}
    shared_bins_before = shared_build_bin_snapshot() if partitioned else {}

    print(f"=== tubuild linkcheck {tu_id}"
          f"{'  [BASELINE CONTROL -- no TU substitution]' if baseline else ''}"
          f"{'  [PARTIAL -- N derived per-function objects, delinks.txt UNCHANGED]' if partial else ''}"
          f"{'  [PARTITIONED -- N derived text objects + one owned non-text object]' if partitioned else ''} ===")
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

    if baseline:
        enrolled_before = RB.enrolled(cfg_root)
        intact_before = RB.intact_tu_policies(enrolled_before)
        demoted, reasons = demote_complete_sources(cfg_root, intact_before)
        if reasons:
            print("\nREFUSED -- strict control could not exclude every intact TU:")
            for reason in reasons:
                print(f"  {reason}")
            return 1
        report["intactTusDemoted"] = [
            {"id": intact_before[source].get("id", source), "source": source}
            for source in demoted]
        if demoted:
            print(f"      strict control demoted {len(demoted)} intact TU source(s) "
                  "to independently extracted ROM gap bytes:")
            for source in demoted:
                print(f"        {source}")

    span_start = span_end = None
    claims = []
    replaced = []
    scratch_rewrite = False
    if not baseline:
        claims, claim_reasons = manifest_section_claims(entry)
        if claim_reasons:
            print("\nREFUSED -- invalid manifest section ownership:")
            for reason in claim_reasons:
                print(f"  {reason}")
            return 1
        text_sec = next(s for s in claims if s["name"] == ".text")
        span_start, span_end = text_sec["start"], text_sec["end"]
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
        elif partitioned:
            # A data-only TU entry adds a new LCF basename while deliberately keeping
            # every legacy text entry. Check the global basename surface before even
            # changing the disposable config: dsd selectors are basename-only.
            stem = pathlib.Path(entry["source"]).stem
            clash = [p for p in RB.enrolled(cfg_root, extra_roots=("src_tu",))
                     if pathlib.PurePosixPath(p).stem == stem
                     and p != entry["source"]]
            if clash:
                print(f"\nREFUSED -- object basename {stem}.o is already claimed by "
                      f"{clash}")
                return 1
            replaced, reasons = add_partitioned_tu_entry(
                dl, span_start, span_end, entry["source"], legacy_rels, claims)
            if reasons:
                print("\nREFUSED -- the partitioned scratch delinks addition is not safe:")
                for r in reasons:
                    print(f"  {r}")
                return 1
            print(f"      retained {len(replaced)} legacy .text entries, in their existing "
                  f"ROM order, and added one SCRATCH-ONLY non-text entry:")
            print(f"        {entry['source']}: complete")
            for claim in (c for c in claims if c["name"] != ".text"):
                output_name = claim.get("module_section", claim["name"])
                print(f"          {output_name:16} 0x{claim['start']:08x}.."
                      f"0x{claim['end']:08x}  relinquished from ROM gap")
            print("      tracked config/**/delinks.txt remains untouched; dsd will now "
                  "generate N legacy .text selectors plus one TU non-text selector.")
        else:
            replaced, reasons = splice_tu_entry(
                dl, span_start, span_end, entry["source"], legacy_rels,
                section_claims=claims)
            if reasons:
                print("\nREFUSED -- the scratch delinks substitution is not safe:")
                for r in reasons:
                    print(f"  {r}")
                return 1
            print(f"      spliced {len(replaced)} per-function entr(y/ies) in "
                  f"{module}/delinks.txt into one:")
            print(f"        {entry['source']}: complete")
            for claim in claims:
                source = "legacy entries" if claim["name"] == ".text" else "ROM gap"
                output_name = claim.get("module_section", claim["name"])
                label = (claim["name"] if output_name == claim["name"] else
                         f"{claim['name']} -> {output_name}")
                print(f"          {label:16} 0x{claim['start']:08x}.."
                      f"0x{claim['end']:08x}  relinquished from {source}")

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
    if partitioned:
        artifact_report = validate_partitioned_link_artifacts(
            scratch / "arm9.lcf", scratch / "objects.txt", entry, claims)
        report["partitionedArtifacts"] = artifact_report
        if not artifact_report["ok"]:
            print("\nREFUSED -- dsd did not generate the exact partitioned selector "
                  "surface:")
            for reason in artifact_report["errors"]:
                print(f"  {reason}")
            report["result"] = "partition-artifact-refused"
            _write_link_report(scratch, report)
            _record_partitioned(data, entry, report)
            return 1
        print("      partitioned LCF/object audit: exactly N legacy .text selectors, "
              "one TU non-text selector/object, no TU .text selector")

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
    failures, outcomes = compile_linkcheck_sources(
        srcs, vers, cache, init_srcs, syms, scratch, args.jobs,
        intact_tus_override={} if baseline else None)
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

    # -------------------- partial/partitioned isolation: derive, compare, substitute
    partial_rows = []
    partition_data_ok = False
    storage_aliases_ok = not partitioned
    vtable_policies = {}
    if partial or partitioned:
        label = "partitioned" if partitioned else "partial"
        print(f"[4b/8] {label} isolation: one TU compile -> N derived text objects, "
              "substituted for the N legacy objects [4/8] just produced")
        if partitioned:
            # The additive scratch delinks entry made rombuild compile this exact TU
            # once in [4].  Recompiling it here would make the claimed one-compile
            # partition untrue, so consume that untouched raw object directly.
            tu_obj_path = scratch / pathlib.Path(entry["source"]).with_suffix(".o")
            if not tu_obj_path.is_file():
                print(f"      !! the build produced no TU object at {tu_obj_path}")
                return 1
            obj_bytes = tu_obj_path.read_bytes()
            version = vers.get(pathlib.Path(entry["source"]).stem, RB.VERSION)
            tu_flags = BP.flags_for(REPO / entry["source"])
        else:
            obj_bytes, version, tu_flags, _bd, tu_obj_path = _compile_tu(entry)
        derived = derive_function_objects(obj_bytes, entry)
        keep = scratch / ("partitioned" if partitioned else "partial")
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

        if partitioned and (n_ok != len(partial_rows)
                            or len(substituted) != len(entry["functions"])):
            reasons = [f"partitioned text substitution is not exact: {n_ok}/"
                       f"{len(partial_rows)} contribution-equivalent, "
                       f"{len(substituted)}/{len(entry['functions'])} substituted"]
            print("      REFUSED -- " + reasons[0])
            report["result"] = "partitioned-text-refused"
            report["partitionedErrors"] = reasons
            _write_link_report(scratch, report)
            _record_partitioned(data, entry, report)
            return 1

        if partitioned:
            print("\n[4c/8] partitioned non-text object: exact policies, ownership "
                  "verification, then content reduction")
            report["partitionedObjects"] = {
                "rawTuSha256": hashlib.sha256(obj_bytes).hexdigest(),
                "rawTuBytes": len(obj_bytes),
                "tuObjectPath": tu_obj_path.relative_to(REPO).as_posix(),
            }
            linked_tu, compiler_only, policy_reasons = \
                apply_compiler_only_policy(obj_bytes, entry)
            report["compilerOnlyOutput"] = compiler_only
            if policy_reasons:
                print("      REFUSED -- unlicensed/compiler-only output policy:")
                for reason in policy_reasons:
                    print(f"        {reason}")
                report["compilerOnlyOutput"]["errors"] = policy_reasons
                report["result"] = "policy-refused"
                _write_link_report(scratch, report)
                _record_partitioned(data, entry, report)
                return 1
            if linked_tu != obj_bytes:
                scratch_rewrite = True
                print(f"      explicitly deadstripped {compiler_only['deadstripped']} "
                      "from the data-source copy")

            externalized_tu, externalized, externalized_reasons = \
                apply_externalized_output_policy(linked_tu, entry)
            report["externalizedOutput"] = externalized
            if externalized_reasons:
                print("      REFUSED -- exact vague RTTI externalization policy:")
                for reason in externalized_reasons:
                    print(f"        {reason}")
                report["externalizedOutput"]["errors"] = externalized_reasons
                report["result"] = "externalization-refused"
                _write_link_report(scratch, report)
                _record_partitioned(data, entry, report)
                return 1
            linked_tu = externalized_tu
            report["partitionedObjects"]["postPolicySha256"] = \
                hashlib.sha256(linked_tu).hexdigest()
            if linked_tu != obj_bytes:
                scratch_rewrite = True
            if externalized.get("externalized"):
                print(f"      externalized {externalized['externalized']} to exact "
                      "configured canonical homes")

            owned_before = verify_owned_sections(linked_tu, entry, claims)
            report["ownedSectionsBeforePartition"] = owned_before
            if not owned_before["ok"]:
                print("      REFUSED -- licensed non-text contribution is not exact:")
                for reason in owned_before.get("errors", []):
                    print(f"        {reason}")
                report["result"] = "data-refused"
                _write_link_report(scratch, report)
                _record_partitioned(data, entry, report)
                return 1

            data_tu, partition_report, partition_reasons = \
                derive_owned_nontext_object(linked_tu, entry, claims)
            report["nontextPartition"] = partition_report
            if partition_reasons:
                print("      REFUSED -- non-text partition is not safe:")
                for reason in partition_reasons:
                    print(f"        {reason}")
                report["nontextPartition"]["errors"] = partition_reasons
                report["result"] = "partition-refused"
                _write_link_report(scratch, report)
                _record_partitioned(data, entry, report)
                return 1

            report["partitionedObjects"]["reducedStorageSha256"] = \
                hashlib.sha256(data_tu).hexdigest()
            biases, bias_reasons = partition_vtable_rebiases(entry, claims)
            vtable_policies = biases
            if bias_reasons:
                print("      REFUSED -- retained vtable address point is not explicit:")
                for reason in bias_reasons:
                    print(f"        {reason}")
                report["result"] = "vtable-rebias-refused"
                report["vtableRebias"] = {"requested": biases,
                                           "errors": bias_reasons}
                _write_link_report(scratch, report)
                _record_partitioned(data, entry, report)
                return 1
            data_tu, bias_report = OI.rebias_object_symbols(data_tu, biases)
            report["vtableRebias"] = bias_report
            if data_tu is None:
                print(f"      REFUSED -- vtable symbol rebias: {bias_report.get('error')}")
                report["result"] = "vtable-rebias-refused"
                _write_link_report(scratch, report)
                _record_partitioned(data, entry, report)
                return 1
            report["partitionedObjects"]["linkedDataSha256"] = \
                hashlib.sha256(data_tu).hexdigest()

            owned = verify_owned_sections(data_tu, entry, claims,
                                           public_address_points=True)
            report["ownedSections"] = owned
            for row in owned["rows"]:
                print(f"      {row['section']:8} {row.get('start', '-')}.."
                      f"{row.get('end', '-')} emitted 0x{row.get('emittedBytes', 0):x}  "
                      f"{'VERIFIED' if row.get('sizeOk') and row.get('bytesOk') else 'DIFF'}  "
                      f"{row.get('relocCount', 0)} reloc(s)")
            if not owned["ok"]:
                print("      REFUSED -- reduction changed the licensed non-text "
                      "contribution:")
                for reason in owned.get("errors", []):
                    print(f"        {reason}")
                report["result"] = "partition-data-refused"
                _write_link_report(scratch, report)
                _record_partitioned(data, entry, report)
                return 1

            tu_obj_path.write_bytes(data_tu)
            scratch_rewrite = True
            partition_data_ok = True
            print(f"      wrote one reduced non-text object at "
                  f"{tu_obj_path.relative_to(REPO).as_posix()}; no live .text remains")

    # ------------------------------------------------- pre-link licensing audit
    if not baseline and not partial and not partitioned:
        print("[4b/8] licensing audit of the shadow object (plan sec 4.5), before the link")
        tu_obj = scratch / pathlib.Path(entry["source"]).with_suffix(".o")
        if not tu_obj.is_file():
            print(f"      !! no object at {tu_obj}")
            return 1
        raw_tu = tu_obj.read_bytes()
        linked_tu, compiler_only, policy_reasons = apply_compiler_only_policy(raw_tu, entry)
        report["compilerOnlyOutput"] = compiler_only
        if policy_reasons:
            print("      REFUSED -- unlicensed/compiler-only output policy:")
            for reason in policy_reasons:
                print(f"        {reason}")
            report["compilerOnlyOutput"]["errors"] = policy_reasons
            report["result"] = "policy-refused"
            _write_link_report(scratch, report)
            _record_linkcheck(data, entry, report, baseline)
            return 1
        if linked_tu != raw_tu:
            scratch_rewrite = True
            tu_obj.write_bytes(linked_tu)
            print(f"      explicitly deadstripped {compiler_only['deadstripped']} from the "
                  f"SCRATCH object only (production source/config untouched)")

        externalized_tu, externalized, externalized_reasons = \
            apply_externalized_output_policy(linked_tu, entry)
        report["externalizedOutput"] = externalized
        if externalized_reasons:
            print("      REFUSED -- exact vague RTTI externalization policy:")
            for reason in externalized_reasons:
                print(f"        {reason}")
            report["externalizedOutput"]["errors"] = externalized_reasons
            report["result"] = "externalization-refused"
            _write_link_report(scratch, report)
            _record_linkcheck(data, entry, report, baseline)
            return 1
        if externalized_tu != linked_tu:
            linked_tu = externalized_tu
            scratch_rewrite = True
            tu_obj.write_bytes(linked_tu)
            print(f"      externalized {externalized['externalized']} to their exact "
                  "configured canonical homes in the SCRATCH object only")

        owned_before = verify_owned_sections(linked_tu, entry, claims)
        report["ownedSectionsBeforeRebias"] = owned_before
        if not owned_before["ok"]:
            print("      REFUSED -- licensed non-text contribution is not exact:")
            for reason in owned_before.get("errors", []):
                print(f"        {reason}")
            report["result"] = "data-refused"
            _write_link_report(scratch, report)
            _record_linkcheck(data, entry, report, baseline)
            return 1

        biases, bias_reasons = partition_vtable_rebiases(entry, claims)
        vtable_policies = biases
        if bias_reasons:
            print("      REFUSED -- retained vtable address point is not explicit:")
            for reason in bias_reasons:
                print(f"        {reason}")
            report["result"] = "vtable-rebias-refused"
            report["vtableRebias"] = {"requested": biases,
                                       "errors": bias_reasons}
            _write_link_report(scratch, report)
            _record_linkcheck(data, entry, report, baseline)
            return 1
        rebased_tu, bias_report = OI.rebias_object_symbols(
            linked_tu, biases, normalize_undefined=True)
        report["vtableRebias"] = bias_report
        if rebased_tu is None:
            print(f"      REFUSED -- vtable symbol rebias: {bias_report.get('error')}")
            report["result"] = "vtable-rebias-refused"
            _write_link_report(scratch, report)
            _record_linkcheck(data, entry, report, baseline)
            return 1
        if rebased_tu != linked_tu:
            linked_tu = rebased_tu
            scratch_rewrite = True
            tu_obj.write_bytes(linked_tu)
            print(f"      rebased {len(bias_report.get('rebased', []))} retained vtable "
                  f"symbol(s) and compensated "
                  f"{len(bias_report.get('relocations', []))} live relocation addend(s) "
                  "in the SCRATCH object only")

        owned = verify_owned_sections(linked_tu, entry, claims,
                                       public_address_points=True)
        report["ownedSections"] = owned
        for row in owned["rows"]:
            print(f"      {row['section']:8} {row.get('start', '-')}..{row.get('end', '-')} "
                  f"emitted 0x{row.get('emittedBytes', 0):x}  "
                  f"{'VERIFIED' if row.get('sizeOk') and row.get('bytesOk') else 'DIFF'}  "
                  f"{row.get('relocCount', 0)} reloc(s)")
        if not owned["ok"]:
            print("      REFUSED -- licensed non-text contribution is not exact:")
            for reason in owned.get("errors", []):
                print(f"        {reason}")
            report["result"] = "data-refused"
            _write_link_report(scratch, report)
            _record_linkcheck(data, entry, report, baseline)
            return 1

        rows, extra_secs, emitted, order_ok = audit_tu_object(
            linked_tu, entry, span_start, span_end, complete_ranges(cfg_root))
        buckets = print_object_audit(rows, extra_secs, emitted, order_ok, entry)
        report["objectAudit"] = {
            "counts": dict(buckets), "orderOk": order_ok,
            "nonLicensed": [r for r in rows if r["verdict"] != "LICENSED"],
            "unlicensedSections": [{"name": s["name"], "size": s["size"]}
                                   for s in extra_secs],
        }
        audit_reasons = object_audit_refusals(rows, extra_secs, order_ok)
        if audit_reasons:
            print("      REFUSED -- object contains output not licensed after exact policy:")
            for reason in audit_reasons:
                print(f"        {reason}")
            report["objectAudit"]["errors"] = audit_reasons
            report["result"] = "object-audit-refused"
            _write_link_report(scratch, report)
            _record_linkcheck(data, entry, report, baseline)
            return 1
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
        if partitioned:
            _record_partitioned(data, entry, report)
        else:
            _record_linkcheck(data, entry, report, baseline)
        return 1
    print(f"      ok ({dt:.1f}s) -> {(scratch / 'final_link.o').relative_to(REPO).as_posix()}")

    if baseline:
        # cfg_root is the scratch copy this link actually consumed (demote_complete_sources
        # rewrote it), so it is what binds the report to its own inputs. tracked_config_root
        # stays CFG_ARM9 by default so the report ALSO carries the tracked config it was
        # derived from -- without that second hash, config/arm9 can move and every
        # consumer still calls this baseline current.
        report["baselineEvidence"] = partition_baseline_fingerprints(
            scratch / "final_link.o", config_root=cfg_root,
            tracked_config_root=CFG_ARM9)

    if partitioned:
        linked_aliases = verify_linked_storage_aliases(
            scratch / "final_link.o", vtable_policies)
        report["linkedStorageAliases"] = linked_aliases
        storage_aliases_ok = linked_aliases["ok"]
        if linked_aliases["ok"]:
            count = len(linked_aliases["rows"])
            print(f"      storage-prefix alias fidelity: {count}/{count} exact vs baseline")
        else:
            print("      storage-prefix alias fidelity: FAIL")
            for reason in linked_aliases["errors"]:
                print(f"        {reason}")

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
            range_reports = []
            for claim in claims:
                if claim["name"] == ".bss":
                    range_reports.append({"section": ".bss",
                                          "start": f"0x{claim['start']:08x}",
                                          "end": f"0x{claim['end']:08x}",
                                          "comparison": "NOBITS -- symbol/module gates"})
                    continue
                ok_claim, ndiff, first = compare_range(
                    built, retail, base, claim["start"], claim["end"],
                    f"TU {claim['name']} range {tu_id}")
                range_ok = range_ok and ok_claim
                record = {"section": claim["name"],
                          "start": f"0x{claim['start']:08x}",
                          "end": f"0x{claim['end']:08x}",
                          "differingBytes": ndiff,
                          "firstDiff": f"0x{first:08x}" if first else None}
                range_reports.append(record)
                if claim["name"] == ".text":
                    report["tuRange"] = {k: v for k, v in record.items()
                                         if k != "section"}
            report["tuRanges"] = range_reports
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
    modules_check_ok = ok
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
    # reproduce-on-main discipline this tree insists on for every stale-baseline gate.
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
    report["symbolsBaseline"] = base_errors

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

    if partitioned:
        shared_bins_after = shared_build_bin_snapshot()
        changed_shared = sorted(name for name in set(shared_bins_before) |
                                set(shared_bins_after)
                                if shared_bins_before.get(name) != shared_bins_after.get(name))
        report["sharedBuildOutputs"] = {
            "before": shared_bins_before, "after": shared_bins_after,
            "changed": changed_shared,
        }
        if changed_shared:
            print(f"      !! shared build/*.bin changed during the scratch pipeline: "
                  f"{changed_shared} -- investigate before trusting this run")
            report["strayOutputs"] = changed_shared

    # ------------------------------------------------------------------------ verdict
    symbols_verdict = linkcheck_symbol_verdict(baseline, symbols_ok, symbols_new)
    equivalent = all(v["identical"] for _o, _s, v in partial_rows) if partial_rows else False
    verified = bool(module_ok and symbols_verdict and (rom_ok is not False))
    if partitioned:
        verified = partitioned_link_ready(
            equivalent=bool(equivalent and partial_rows), data_ok=partition_data_ok,
            storage_aliases_ok=storage_aliases_ok,
            artifacts_ok=report.get("partitionedArtifacts", {}).get("ok") is True,
            module_ok=module_ok, modules_check_ok=modules_check_ok,
            symbols_ok=symbols_verdict, rom_ok=rom_ok,
            rom_identical=report.get("rom", {}).get("matchesStockRom") is True,
            no_stray_outputs=not report.get("strayOutputs"))
        report["result"] = "partitioned-link-verified" if verified else "failed"
    elif partial:
        # A partial run's claim is narrower than link-verified's and must not borrow its
        # name: the TU's whole .text range still comes from N objects, not one.
        verified = bool(verified and equivalent and partial_rows)
        report["result"] = "partial-link-verified" if verified else "failed"
    else:
        has_nontext = bool(not baseline and any(c["name"] != ".text" for c in claims))
        report["result"] = classify_link_result(
            has_nontext, verified, rom_ok, scratch_rewrite)
    print()
    if partitioned:
        n_ok = sum(1 for _o, _s, v in partial_rows if v["identical"])
        if verified:
            print(f"Result: PARTITIONED-LINK-VERIFIED. {entry['source']} was compiled "
                  f"once; {n_ok}/{len(partial_rows)} derived .text objects reproduce at "
                  "the legacy ROM-ordered selectors, the reduced non-text object owns "
                  "only exact manifest ranges, all modules/symbol deltas pass, and the "
                  "full ROM is identical to the stock build.")
            print("        This is scratch-only evidence on an orthogonal axis. Tracked "
                  "src/ and config/**/delinks.txt were not changed, and this result is "
                  "never promotion-ready.")
        else:
            print(f"Result: NOT partitioned-link-verified ({n_ok}/"
                  f"{len(partial_rows)} text contributions; data_ok="
                  f"{partition_data_ok}; storage aliases exact={storage_aliases_ok}; "
                  "full ROM exact="
                  f"{report.get('rom', {}).get('matchesStockRom') is True}).")
    elif partial:
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
        level = report["result"].upper()
        owned_phrase = (f"all {len(claims)} licensed section ranges"
                        if has_nontext else
                        f"the whole .text range 0x{span_start:08x}..0x{span_end:08x}")
        print(f"Result: {level}. {owned_phrase} reproduce from one object, every "
              f"module is byte-exact, {sym_phrase}"
              f"{', and the full ROM builds' if rom_ok else ''}.")
    else:
        print("Result: NOT link-verified (see the phases above).")
    _write_link_report(scratch, report)
    if partitioned:
        _record_partitioned(data, entry, report)
    elif partial:
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


def partitioned_link_ready(*, equivalent, data_ok, storage_aliases_ok, artifacts_ok,
                           module_ok,
                           modules_check_ok, symbols_ok, rom_ok, rom_identical,
                           no_stray_outputs):
    """The deliberately strict, full-ROM-only partitioned-link result gate."""
    return all((equivalent, data_ok, storage_aliases_ok, artifacts_ok, module_ok,
                modules_check_ok,
                symbols_ok, rom_ok is True, rom_identical, no_stray_outputs))


def classify_link_result(has_nontext, pipeline_ok, rom_ok, scratch_rewrite=False):
    """Name a linkcheck result without turning research evidence into promotion state."""
    if not pipeline_ok:
        return "failed"
    base = "data-verified" if has_nontext else "link-verified"
    if rom_ok is not True:
        return "module-" + base
    if scratch_rewrite:
        return "scratch-" + base
    return base


def _write_link_report(scratch, report):
    path = scratch / "linkcheck.json"
    path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8", newline="\n")
    print(f"report -> {path.relative_to(REPO).as_posix()} (gitignored)")


def _compact_partition_artifact(report):
    """Keep the tracked proof small; full linker inventories stay in linkcheck.json."""
    artifact = report.get("partitionedArtifacts") or {}
    return {key: artifact.get(key) for key in (
        "ok", "errors", "expectedTuSelectors", "observedTuSelectors",
        "expectedLegacyCount", "selectorCount", "objectCount",
        "selectorListSha256", "objectListSha256",
    )}


def _partition_attempt_record(report):
    """Concise, reviewable evidence for one scratch partition attempt."""
    partial_report = report.get("partial") or {}
    owned_before = report.get("ownedSectionsBeforePartition") or {}
    owned = report.get("ownedSections") or {}
    partition = report.get("nontextPartition") or {}
    plan = partition.get("objisolate") or {}
    compiler = report.get("compilerOnlyOutput") or {}
    externalized = report.get("externalizedOutput") or {}
    external_verify = externalized.get("verification") or {}
    return {
        "result": report.get("result", "failed"),
        "round": "tools/tubuild.py linkcheck --partitioned -- scratch-only additive "
                 "non-text delinks entry, objisolate-derived per-function text objects, "
                 "exact compiler-only/RTTI policies, exact data partition and vtable "
                 "address-point rebias, whole-tree link and full-ROM comparison",
        "trackedDelinksChanged": False,
        "text": {
            "toolchain": partial_report.get("toolchain"),
            "flags": partial_report.get("flags"),
            "mergedBytes": partial_report.get("mergedBytes"),
            "contributionEquivalent": partial_report.get("contributionEquivalent"),
            "substitutedObjectPaths": partial_report.get("substituted"),
            "rows": [{key: row.get(key) for key in (
                "ordinal", "symbol", "identical", "relocCount", "differences")}
                for row in partial_report.get("rows", [])],
        },
        "compilerOnlyPolicy": {
            "requested": compiler.get("requested"),
            "deadstripped": compiler.get("deadstripped"),
            "droppedSections": compiler.get("droppedSections"),
            "errors": compiler.get("errors"),
        },
        "externalizedPolicy": {
            "requested": externalized.get("requested"),
            "externalized": externalized.get("externalized"),
            "droppedSections": externalized.get("droppedSections"),
            "verificationOk": external_verify.get("ok"),
            "errors": externalized.get("errors") or external_verify.get("errors"),
        },
        "nontextPartition": {
            "requestedSections": partition.get("requestedSections"),
            "licensedSymbols": partition.get("licensedSymbols"),
            "deferredOutputs": partition.get("deferredOutputs"),
            "keptSectionIndices": plan.get("keeps"),
            "droppedSectionIndices": plan.get("drop"),
            "externalizedTextSymbols": plan.get("externalise"),
            "deadTextSymbols": plan.get("dead"),
            "liveSections": partition.get("liveSections"),
            "error": plan.get("error"),
        },
        "ownedBeforePartition": {
            "ok": owned_before.get("ok"), "rows": owned_before.get("rows"),
            "errors": owned_before.get("errors"),
        },
        "ownedAfterRebias": {
            "ok": owned.get("ok"), "rows": owned.get("rows"),
            "errors": owned.get("errors"),
        },
        "vtableRebias": report.get("vtableRebias"),
        "linkedStorageAliases": report.get("linkedStorageAliases"),
        "objectHashes": report.get("partitionedObjects"),
        "artifactAudit": _compact_partition_artifact(report),
        "ranges": report.get("tuRanges"),
        "phases": {key: value.get("ok")
                   for key, value in report.get("phases", {}).items()},
        "moduleFidelityPassed": bool((report.get("analysis") or {}).get("passed")),
        "symbolCheckNewVsBaseline": report.get("symbolsNew"),
        "symbolCheckErrors": (((report.get("phases") or {}).get("checkSymbols") or {})
                              .get("errors")),
        "symbolCheckBaselineErrors": report.get("symbolsBaseline"),
        "rom": report.get("rom"),
        "strayOutputs": report.get("strayOutputs"),
        "scratch": report.get("scratch", "") + " (gitignored)",
    }


def _record_partitioned(data, entry, report):
    """Record an orthogonal attempt, preserving any earlier full-ROM proof."""
    if entry is None:
        return
    block = entry.setdefault("partitioned_link", {})
    attempt = _partition_attempt_record(report)
    block["lastAttempt"] = attempt
    if attempt["result"] == "partitioned-link-verified":
        block["lastVerified"] = attempt
    if block.get("lastVerified"):
        block["state"] = "partitioned-link-verified"
        block["stateMeaning"] = (
            "At least one scratch-only attempt compiled one shadow TU into exact "
            "ROM-ordered text partitions plus exact owned non-text data and reproduced "
            "the stock ROM. The latest attempt is recorded separately; this is never a "
            "production enrollment or promotion state.")
    else:
        block["state"] = attempt["result"]
        block["stateMeaning"] = (
            "The latest scratch-only partition attempt did not earn full-ROM verification. "
            "It is evidence only and never a production enrollment or promotion state.")
    save_manifest(data)


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
        "tuRanges": report.get("tuRanges"),
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
        "symbolCheckErrors": (((report.get("phases") or {}).get("checkSymbols") or {})
                              .get("errors")),
        "symbolCheckBaselineErrors": report.get("symbolsBaseline"),
        "moduleSetSha256": (((report.get("analysis") or {})
                              .get("moduleFidelity") or {})
                             .get("moduleSetSha256")),
        "rom": report.get("rom"),
        "linkerOutput": report["phases"].get("link", {}).get("output"),
    }
    if report["result"] == "link-verified" and entry.get("status") == "text-verified":
        entry["status"] = "link-verified"
    elif report["result"] == "data-verified" and entry.get("status") in (
            "text-verified", "link-verified"):
        entry["status"] = "data-verified"
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


def promotion_refusals(entry):
    """Reasons the current production build cannot consume this verified shadow TU."""
    reasons = []
    status = entry.get("status")
    if status == "partitioned-link-verified":
        reasons.append("partitioned-link-verified is scratch-only and never a production "
                       "promotion status")
    if status not in ("link-verified", "data-verified"):
        reasons.append("manifest status is not link-verified or data-verified")
    if any(s.get("name") != ".text" for s in entry.get("sections", [])
           if isinstance(s, dict)):
        reasons.append("promotion does not yet install tracked non-.text delinks claims; "
                       "rombuild --partitioned-tu supports them only in a generated profile")
    if entry.get("externalized_output"):
        reasons.append("promotion does not yet persist externalized_output policy; "
                       "rombuild --partitioned-tu applies it in the opt-in production path")
    linkcheck = entry.get("verification", {}).get("linkcheck") or {}
    if linkcheck.get("phases", {}).get("rom") is not True:
        reasons.append("the recorded full-ROM phase is not green")
    if (linkcheck.get("rom") or {}).get("matchesStockRom") is not True:
        reasons.append("the recorded full ROM is not proven identical to the stock build")
    if linkcheck.get("result") != status:
        reasons.append("the promotion-ready status is not the recorded linkcheck result")
    return reasons


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
    promotion_blocks = promotion_refusals(entry)
    ok_status = not promotion_blocks
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
          f"{'' if ok_status else '   <<< promotion would be REFUSED'}")
    for reason in promotion_blocks:
        print(f"                          - {reason}")
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
    print("   tools/srcpath.py: no edit needed. Its enrollment index maps every symbol "
          "inside the new complete range to the shared source, so deleted legacy stems "
          "continue to resolve to their production owner.")
    print(f"   COVERAGE: one physical complete entry replaces {len(legacy)}, while "
          "rombuild_check and readability metrics count the enrolled functions inside "
          "the range; function and byte coverage therefore stay invariant.")
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
    # validate_merge used to infer ownership only from filename stems. It now asks the
    # revision's delinks enrollment map first, so a single path can retain matched and
    # byte-verified coverage for every licensed function range it owns. Keep the dry
    # run explicit about what remains; stale blockers are worse than no diagnosis here.
    print("   COVERAGE GATE IS TU-AWARE: tools/validate_merge.py resolves source ownership "
          "from config/**/delinks.txt before falling back to 'stem == symbol name', and "
          "counts every function address inside a complete enrolled range. A shared TU "
          "path therefore does not inherently lose matched or byte-verified coverage.")
    print("   SHARED-SOURCE CONTRACT: run `python tools/cpp_tu_compat.py --require-ready`; "
          "it verifies enrollment, isolation, eligibility, metrics, attribution, and "
          "port symbol resolution for a multi-function source. Any manifest-status or "
          "extra-output refusal printed above remains independently blocking.")
    print("   Related, and NOT triggered by this TU but by any key-function TU: dsd "
          "derives vtable/typeinfo ownership from the mangled class name in the "
          "delinks.txt PATH, so replacing src/_ZN<Class>D1Ev.cpp with src/<dir>/<Class>.cpp "
          "changes that derivation -- a vtable rename must move atomically with it.")

    print("\n-- 5. manifest")
    print(f"   {TUM.describe(MANIFEST)}: entry {entry['id']} "
          f"status {status} -> promoted, source -> {dest_rel}")

    print("\n-- 6. validation a real promotion would then run")
    print("   python tools/layout_check.py")
    print("   python tools/eligible.py            (expect derived placement for each member)")
    print("   python tools/rombuild.py            (module fidelity + source fidelity)")
    print(f"   python tools/tubuild.py linkcheck {entry['id']}   (now against the real tree)")

    print("\nDRY RUN COMPLETE -- no file was created, moved, deleted or edited.")
    return 0 if ok_status else 1


# =================================================================================== main

def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--manifest", type=pathlib.Path, default=None,
                    help="override config/tu_manifest.d/ -- point this at a scratch copy "
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
    modes = p.add_mutually_exclusive_group()
    modes.add_argument("--baseline", action="store_true",
                       help="run the identical scratch pipeline with NO TU substitution -- "
                            "the control that says whether a failure belongs to the TU or "
                            "the harness")
    modes.add_argument("--partial", action="store_true",
                       help="plan sec 9: leave delinks.txt alone and substitute N derived "
                            "per-function objects at their existing paths")
    modes.add_argument("--partitioned", action="store_true",
                       help="scratch-only: retain N ROM-ordered legacy text entries, add "
                            "one TU non-text gap owner, then link N derived text objects "
                            "plus one exact reduced data object")
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
