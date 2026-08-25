"""Audit relocation-destination identity for every banked match.

WHY THIS EXISTS
---------------
The match gate (tools/match.py:compare) wildcards every word that the *candidate*
object relocates: at those offsets it forces ``match = True`` without checking
where the relocation points. reverify_corpus.py inherits the same compare, so the
whole corpus is verified with that blind spot. A candidate can therefore compile
to identical instruction bytes and place a relocation at the right word offset
while pointing it at the WRONG callee / global / overlay, and still "pass".

This tool measures how often that actually happens. For each banked match it:
  1. Recompiles the committed source the same way reverify does and grabs the
     exact object that reproduces the ROM bytes.
  2. Reads each relocation the object emits inside the function: word offset +
     target symbol -> resolved destination (module, addr).
  3. Looks up the ground-truth relocation at that same source address in the
     checked-in config/**/relocs.txt (module-aware, via tools/relocs.py).
  4. Compares destinations and buckets the result.

It changes NOTHING in the match path; it only reports. See the buckets in
classify() for exactly what is and isn't treated as a problem, and read
"ACCURACY / WHAT THIS CANNOT SEE" at the bottom before trusting the headline.

Usage:
  python tools/reloc_audit.py                      # whole corpus (slow)
  python tools/reloc_audit.py --module arm9        # one module
  python tools/reloc_audit.py --module ov006 -j 8
  python tools/reloc_audit.py --limit 200          # first N entries (quick sample)
  python tools/reloc_audit.py --json out.json      # machine-readable detail
"""
import argparse
import io
import json
import os
import pathlib
import re
import sys
import tempfile
from collections import Counter
from concurrent.futures import ThreadPoolExecutor

from elftools.elf.elffile import ELFFile

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import match as M          # noqa: E402
import relocs as R         # noqa: E402
# reverify_corpus is imported lazily inside winning_object(): it pulls in the ROM
# and compile harness, and importing it at top level would create a cycle for
# gate code (match/reverify) that wants only the pure checker below.

# func_0202e78c  or  func_ov006_020bf1a0  -> capture (overlay_id_or_None, addr)
_FUNC_RE = re.compile(r"func_(?:ov(\d+)_)?([0-9a-fA-F]{8})\b")


def build_name_index():
    """name -> (module, addr) from every checked-in symbols.txt.

    Names are not globally unique (overlays reuse them), so this is a best-effort
    fallback used only for symbols whose address is NOT encoded in the name."""
    idx = {}
    for module, path in R.iter_symbol_files(include_itcm_dtcm=True):
        for name, (mod, addr) in R.iter_syms_pairs(path, module):
            idx.setdefault(name, (mod, addr))
    return idx


def build_config_relocs():
    """{module: {from_addr: (kind, to_addr, to_module)}} for every relocs.txt."""
    out = {}
    for module, path in R.iter_reloc_files(include_itcm_dtcm=True):
        out.setdefault(module, {}).update(R.load_relocs_file(path))
    return out


# mwccarm lowers the C `/` and `%` operators (variable divisor) to calls to its
# own runtime helpers -- _u32_div_f / _s32_div_f (and the non-leaf _u32_div /
# _s32_div). Those are the very ITCM routines config records under their ARM EABI
# names (__aeabi_uidiv @ 0x01ffadf0, __aeabi_idiv @ 0x01ffabe4), so the emitted
# name and the config name are two spellings of one address. symbols.txt is
# address-keyed (one name per address) and dozens of sources reference the
# __aeabi_* spelling by name, so the alias cannot live there -- it resolves here.
# Without it every byte-correct match that uses `/` or `%` reads BLIND.
_RUNTIME_ALIASES = {
    "_u32_div_f": "__aeabi_uidiv",
    "_u32_div":   "__aeabi_uidiv",
    "_s32_div_f": "__aeabi_idiv",
    "_s32_div":   "__aeabi_idiv",
    # long-long family: mwccarm lowers u64 `/` and `%` the same way (seen on
    # func_02059a60, campaign 2026-07-18); ITCM records them as __aeabi_ul*.
    "_ll_udiv":   "__aeabi_uldiv",
    "_ll_umod":   "__aeabi_ulmod",
    # 1.2 spells the u64 `%` helper _ull_mod (seen on func_02071510, 2026-07-25).
    "_ull_mod":   "__aeabi_ulmod",
}


def resolve_candidate(symname, name_index):
    """Destination (module_or_None, addr) the candidate symbol refers to, or None.

    Prefer the address encoded in func_<addr> / func_ov<NN>_<addr> names (these are
    unambiguous and module-bearing); fall back to the symbols.txt name index, then
    to the mwccarm runtime-helper alias table."""
    m = _FUNC_RE.fullmatch(symname) or _FUNC_RE.match(symname)
    if m:
        mod = f"ov{int(m.group(1)):03d}" if m.group(1) is not None else None
        return (mod, int(m.group(2), 16))
    if symname in name_index:
        mod, addr = name_index[symname]
        return (mod, addr)
    alias = _RUNTIME_ALIASES.get(symname)
    if alias is not None and alias in name_index:
        return name_index[alias]
    return None


def rel_section_for(elf, shndx):
    """The relocation section that applies to section `shndx`, matched by sh_info.

    Do NOT look this up by name. mwccarm emits ONE section per function and names them
    all ".text", so ".rela.text" is ambiguous and pyelftools' name lookup answers with
    the LAST section of that name -- one fixed function's relocations, whichever
    function was asked about.

    A destructor TU always defines three functions, emitted D2, D0, D1, so the last
    table is D1's. D1 therefore got its own by luck and D0 and D2 got D1's. That is
    exactly backwards from useful: the D0 route is the one this tree has not migrated
    yet, and it was the one being checked against another function's table.

    The byte compare cannot cover for it. Relocated words are wildcarded, which is the
    whole reason this destination check exists.

    tools/linkcheck.py hit the same hazard first and documented it; this is the shared
    copy, and linkcheck imports it rather than keeping a second one."""
    for sec in elf.iter_sections():
        if sec.header["sh_type"] in ("SHT_REL", "SHT_RELA") and sec.header["sh_info"] == shndx:
            return sec
    return None


# objisolate.VTABLE_PREAMBLE; kept a literal so this module stays import-light.
# mwcc's own `_ZTV<C>` symbol addresses the vtable OBJECT -- offset-to-top,
# typeinfo, then the slot array -- while symbols.txt's `_ZTV<C>` IS the slot
# array. The 8-byte gap is exactly what objisolate subtracts when it rewrites a
# vptr store's addend for the ROM link.
_VT_PREAMBLE = 8


def object_reloc_dests(obj, func, name_index, vt_form="raw"):
    """[(offset, symname, resolved_module, resolved_addr)] for relocs inside func.

    Returns (None, reason) if the function symbol isn't in the object.

    Everything is resolved by symbol NAME alone except `_ZTV<C>` data relocs,
    which are ADDEND-AWARE. A synthesized vptr store names the class's one
    `_ZTV` symbol for every block it stores -- primary AND each inherited
    secondary -- and encodes which block in the RELA addend: 8 for the primary
    (the preamble) plus 0x10 per secondary block past it. Resolving those by
    name alone sends every block to the same address and reports WRONG-DEST on
    an object whose linked form is exact (measured on _ZN9dBgCh_LinC1Ev:
    addends 8/0x18 against _ZTV9dBgCh_Lin 0x020992a4, config destinations
    0x020992a4/0x020992b4). The destination the build links is
    sym + addend - _VT_PREAMBLE.

    Branch relocs keep name-only resolution: their nonzero addends are PC-bias
    encoding (-8), not addressing, and must not be added on.

    vt_form says which side of objisolate's rewrite `obj` sits on, because the
    flag cannot be inferred from the object: a raw object's secondary store
    carries 0x18 while an isolated one carries 0x10, and both are >= the
    preamble.
      "raw"      straight from mwccarm (match.py, build_pin.py, bank_harvest,
                 nearmiss_db, symscope); the preamble is still in the addend
      "isolated" post-objisolate (reloc_audit's winning_object path);
                 objisolate already subtracted it
    """
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    syms = list(symtab.iter_symbols())
    sym = next((s for s in syms if s.name == func), None)
    if sym is None:
        return None, "func-not-in-obj"
    start, size = sym["st_value"], sym["st_size"]
    rel = rel_section_for(elf, sym["st_shndx"])
    dests = []
    if rel is not None:
        is_rela = rel.header["sh_type"] == "SHT_RELA"
        for r in rel.iter_relocations():
            o = r["r_offset"] - start
            if not (0 <= o < size):
                continue
            tsym = syms[r["r_info_sym"]]
            res = resolve_candidate(tsym.name, name_index)
            mod, addr = (res if res else (None, None))
            if is_rela and res is not None and tsym.name.startswith("_ZTV"):
                addend = r["r_addend"]
                if vt_form == "isolated":
                    addr += addend
                elif addend >= _VT_PREAMBLE:
                    addr += addend - _VT_PREAMBLE
            dests.append((o & ~3, tsym.name, mod, addr))
    return dests, size


def _as_the_build_links_it(obj, name):
    """Apply objisolate, because the ROM build does and the destinations differ.

    A gate that checks relocation DESTINATIONS has to check the object the linker
    actually consumes. rombuild runs every enrolled object through objisolate, and
    one of the things isolation does is correct the vtable addend: mwcc's `_ZTV<C>`
    addresses the vtable OBJECT, so a vptr store carries an addend that skips the
    offset-to-top and typeinfo words, while symbols.txt's `_ZTV<C>` IS the slot
    array. Isolation subtracts that preamble.

    Without this the gate reads the raw addend and resolves to whatever happens to
    sit there. Observed: `_ZN10ModelAnim2D0Ev`'s secondary vptr store carries
    addend 44, and `_ZTV10ModelAnim2` (0x0208e9b4) + 44 lands on `_ZTI8dFader_c`
    (0x0208e9e0) -- an unrelated class's typeinfo, reported as WRONG-DEST on a file
    whose linked bytes are exactly right. Isolation turns 8 -> 0 and 44 -> 36, which
    is `_ZTV10ModelAnim2` itself and `VTable_Animation_ModelAnim2Thunk` (0x0208e9d8).

    Multiple inheritance is what made this visible: the single-inheritance addend of
    8 resolves to a word still inside the same vtable symbol, so it never looked
    wrong. Fails open -- if isolation cannot plan this object, check it unisolated
    rather than losing the verdict.
    """
    try:
        import objisolate as OI
        fd, tmp = tempfile.mkstemp(suffix=".o")
        os.close(fd)
        p = pathlib.Path(tmp)
        try:
            p.write_bytes(obj)
            if OI.plan(obj, name).get("error"):
                return obj
            OI.isolate(p, name)
            return p.read_bytes()
        finally:
            try:
                p.unlink()
            except OSError:
                pass
    except Exception:                                             # noqa: BLE001
        return obj


def winning_object(name, addr, size, mod, candidate=None, include_dirs=()):
    """Reproduce the match the way reverify does, but return the object that did it.

    Mirrors reverify_corpus.compiles_to so the audit measures exactly what reverify
    blesses -- same sources, same version sweep, same any-symbol acceptance.
    When ``candidate`` is supplied, verify that source file directly instead of
    looking it up under ``src/``. This is the safe bridge for workbench artifacts."""
    import reverify_corpus as RV
    import swarm as S
    target = RV.rom_bytes(mod, addr, size)
    if target is None:
        return None, None, "no-module-bin"
    # Distinguish the three ways this can fail so callers do not report a missing
    # or wrong-length source as "no-repro" (which reads as a false-match red flag).
    # saw_source: src_texts yielded at least one candidate to try.
    # saw_len:    a candidate compiled to a function of the expected length.
    saw_source = saw_len = False
    candidate_path = pathlib.Path(candidate) if candidate is not None else None
    if candidate_path is not None:
        try:
            # errors="replace": the text is only used for the //cpp sniff -- mwccarm reads
            # the file itself -- and a stray non-UTF-8 byte must not raise UnicodeDecodeError
            # and take down a verdict. (Windows cp1252 round-trips have corrupted repo JSONL
            # this way before; do not narrow this to a bare OSError catch.)
            sources = [(candidate_path.read_text(encoding="utf-8", errors="replace"),
                        candidate_path)]
        except OSError:
            sources = []
    else:
        sources = [(src, None) for src in RV.src_texts(name, addr)]
    for src, source_path in sources:
        saw_source = True
        attempts = ([(S.CPP_FLAGS, ".cpp")] if src.startswith("//cpp")
                    else [(M.DEFAULT_FLAGS, ".c"), (S.CPP_FLAGS, ".cpp")])
        for flags, suf in attempts:
            tmp = None
            if source_path is None:
                fd, tmp = tempfile.mkstemp(suffix=suf)
                os.close(fd)
                cfile = pathlib.Path(tmp)
                # encoding is not optional here: 42 files in src/ contain non-ASCII
                # (arrows and box characters in codegen comments), and on Windows the
                # default is cp1252, which raises UnicodeEncodeError and degrades the
                # file's verdict to ERROR - which prepush_linkcheck treats as blocking.
                cfile.write_text(src, encoding="utf-8")
            else:
                cfile = source_path
            try:
                for v in RV.ALL_VERSIONS:
                    obj = M.compile_c(cfile, v, flags, include_dirs)
                    if obj is None:
                        continue
                    obj = _as_the_build_links_it(obj, name)
                    import probe_versions as PV
                    try:
                        candidate_syms = list(PV.funcs_in(obj).keys())
                    except Exception:
                        candidate_syms = [name]
                    # Try the REQUESTED symbol first, always. Any-symbol acceptance is
                    # deliberate (a source may emit its function under a near-miss name),
                    # but it must be the fallback, not the default: when the name is already
                    # in the list this used to leave iteration order to decide, so a TU with
                    # byte-identical siblings returned the wrong one. The _ZThn80_*D0Ev and
                    # *D1Ev thunks are identical except for their branch RELOC, so asking for
                    # D1 handed back D0 and linkcheck then compared D0's relocation against
                    # D1's ROM bytes and called a correct file WRONG.
                    candidate_syms = [name] + [s for s in candidate_syms if s != name]
                    for sym in candidate_syms:
                        code, relocs = M.extract_func(obj, sym)
                        if code is None:
                            continue
                        tgt = target
                        if len(code) != len(tgt):
                            # Split-symbol carrier (notes 9a(3)): a function whose compiled
                            # form extends over the following severed fragment(s), e.g.
                            # func_02072168 emitting func_020729e8's 12-byte epilogue. Accept
                            # ONLY if the full compiled span is byte-identical to the ROM,
                            # overhang included -- strictly stronger than the plain check, so
                            # a wrong-sized near miss cannot slip through. 0x40 bounds the
                            # overhang to fragment scale.
                            if not (len(code) > len(tgt) and len(code) - len(tgt) <= 0x40):
                                continue
                            ext = RV.rom_bytes(mod, addr, len(code))
                            if ext is None or len(ext) != len(code):
                                continue
                            tgt = ext
                        saw_len = True
                        ok, _ = M.compare(tgt, code, relocs, verbose=False)
                        if ok:
                            return obj, sym, None
            finally:
                if tmp is not None:
                    pathlib.Path(tmp).unlink(missing_ok=True)
    if not saw_source:
        return None, None, "no-source"      # no src/<name>.c|.cpp on disk to try
    if not saw_len:
        return None, None, "len-mismatch"   # compiled, but never the target's length
    return None, None, "no-repro"           # right length, but bytes never matched


def classify(cand_name, cand_mod, cand_addr, cfg, sym_index):
    """One reloc's verdict. cfg is (kind, to_addr, to_module) or None.

    The only thing a candidate relocation actually pins is a DESTINATION ADDRESS
    (encoded in a func_<addr> name, or via a named symbol's definition). So the
    verdict is address-anchored against config's ground-truth ``to_addr``:

    OK            candidate targets config's destination address (or, by name, the
                  exact symbol config records there)
    WRONG-DEST    candidate targets a different address than config     <-- the bug
    EXTRA         candidate relocates a word config does not  (wildcard hides it)
    UNRESOLVED    candidate's destination address couldn't be determined

    NOTE on overlay modules: a C++ name like _ZN5EnemyC2Ev may be DEFINED in only
    one overlay yet legitimately referenced from another that maps a different
    function to the same address. When the candidate's address equals config's
    address we treat it as OK -- the address is correct and the module label of a
    name is not authoritative. A *genuine* wrong-overlay target therefore surfaces
    as an address mismatch (WRONG-DEST), not a same-address module disagreement.
    Earlier revisions emitted a WRONG-MODULE verdict here; it proved to be almost
    entirely name-aliasing noise, so it was removed.
    """
    if cfg is None:
        return "EXTRA"
    _kind, to_addr, _to_module = cfg
    cfg_mod = R.normalize_module(_to_module)
    expected_name = sym_index.get((cfg_mod, to_addr))
    if expected_name is not None and cand_name == expected_name:
        return "OK"
    if cand_addr is None:
        return "UNRESOLVED"
    if cand_addr == to_addr:
        return "OK"
    return "WRONG-DEST"


def known_modules():
    """Normalized module IDs that actually have a relocs file.

    The authority for "is this a real module", so a caller cannot verify against a
    module ID that does not exist. Cheap enough to recompute; the paths are globbed
    once per call and no file is read."""
    return {m for m, _path in R.iter_reloc_files(include_itcm_dtcm=True)}


def check_destinations(obj, sym, addr, size, mod, name_index, config_relocs, sym_index,
                       vt_form="raw"):
    """Per-reloc destination verdicts for an in-hand object. Shared by the audit
    and the match gate (match.py --strict-relocs).

    vt_form passes through to object_reloc_dests: which side of objisolate's
    vtable-addend rewrite `obj` sits on. The audit feeds isolated objects
    (winning_object), every other caller compiles fresh and feeds raw.

    Returns (rows, missing) where rows is a list of dicts (one per object reloc in
    the function) and missing is the count of config relocs the candidate lacks.
    Returns (None, reason) if the function symbol isn't in the object, or if `mod`
    is not a module this repo has relocations for.

    That second case used to fall through to an empty cfgmap, which is the most
    dangerous possible failure for a gate: every reloc classified against cfg=None,
    nothing came back WRONG-DEST, and the caller reported a clean strict-reloc pass
    having checked nothing. A wrong callee under `--module arm9/itcm` (the path-style
    spelling; the module ID is `itcm`) verified as a MATCH. Refuse the spelling
    instead -- an unknown module is a caller bug, not an empty result."""
    normalized = R.normalize_module(mod)
    if normalized not in known_modules():
        return None, (f"unknown module {mod!r} (normalizes to {normalized!r}); "
                      f"expected one of arm9, itcm, dtcm, ovNNN")
    dests, size_or_reason = object_reloc_dests(obj, sym, name_index, vt_form)
    if dests is None:
        return None, size_or_reason
    cfgmap = config_relocs.get(normalized, {})
    cand_offsets = {o for o, *_ in dests}
    rows = []
    for o, symname, cmod, caddr in dests:
        cfg = cfgmap.get(addr + o)
        v = classify(symname, cmod, caddr, cfg, sym_index)
        rows.append({"off": f"+0x{o:x}", "cand": symname,
                     "cand_addr": (f"0x{caddr:08x}" if caddr is not None else None),
                     "cfg": (f"0x{cfg[1]:08x}:{cfg[2]}" if cfg else None),
                     "verdict": v})
    # config relocs inside the function that the candidate does NOT relocate:
    # those words were NOT wildcarded, so the byte compare already had to match
    # them literally -- lower risk, but report for completeness.
    missing = sum(1 for from_addr in cfgmap
                  if addr <= from_addr < addr + size and (from_addr - addr) not in cand_offsets)
    return rows, missing


_GATE_IDX = None


def warm_gate_index():
    """Build the gate's three config indexes now, on the calling thread.

    gate_wrong_dests builds them lazily, which is idempotent and so safe to race --
    but a caller fanning it out across threads would have every thread build its own
    copy and throw all but one away. Callers that intend to parallelise call this
    once up front; everyone else can ignore it."""
    global _GATE_IDX
    if _GATE_IDX is None:
        _GATE_IDX = (build_name_index(), build_config_relocs(), R.load_all_syms())
    return _GATE_IDX


def gate_wrong_dests(obj, sym, addr, size, mod, vt_form="raw"):
    """Bank-gate wrapper around check_destinations: WRONG-DEST rows only, with the
    three config indexes built once and cached for the process. Returns [] when the
    object's reloc destinations agree with config, None when the symbol is absent
    from the object (callers treat that as a verification failure). vt_form passes
    through to check_destinations/object_reloc_dests."""
    name_index, config_relocs, sym_index = warm_gate_index()
    rows, _missing = check_destinations(obj, sym, addr, size, mod,
                                        name_index, config_relocs, sym_index,
                                        vt_form)
    if rows is None:
        return None
    return [r for r in rows if r["verdict"] == "WRONG-DEST"]


def audit_entry(entry, name_index, config_relocs, sym_index):
    name = entry["name"]
    addr = int(entry["addr"], 16) if isinstance(entry["addr"], str) else entry["addr"]
    size = entry["size"]
    mod = entry["module"]
    obj, sym, err = winning_object(name, addr, size, mod)
    if obj is None:
        return {"name": name, "module": mod, "addr": f"0x{addr:08x}",
                "verdict": "NO-REPRO", "reason": err, "relocs": []}
    rows, missing = check_destinations(obj, sym, addr, size, mod,
                                       name_index, config_relocs, sym_index,
                                       vt_form="isolated")
    if rows is None:
        return {"name": name, "module": mod, "addr": f"0x{addr:08x}",
                "verdict": "NO-SYM", "reason": missing, "relocs": []}
    worst = _worst([r["verdict"] for r in rows], missing)
    return {"name": name, "module": mod, "addr": f"0x{addr:08x}",
            "verdict": worst, "missing_config_relocs": missing, "relocs": rows}


_ORDER = ["WRONG-DEST", "EXTRA", "UNRESOLVED", "OK"]


def _worst(verdicts, missing):
    for v in _ORDER:
        if v in verdicts:
            if v == "OK":
                return "MISSING" if missing else "CLEAN"
            return v
    return "MISSING" if missing else "CLEAN"


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--module", default=None, help="audit one module (e.g. arm9, ov006)")
    ap.add_argument("--limit", type=int, default=None, help="first N entries only (quick sample)")
    ap.add_argument("-j", "--jobs", type=int, default=8)
    ap.add_argument("--json", default=None, help="write full per-reloc detail here")
    args = ap.parse_args()

    name_index = build_name_index()
    config_relocs = build_config_relocs()
    sym_index = R.load_all_syms()

    entries = [json.loads(l) for l in (REPO / "progress" / "matched.jsonl").read_text().splitlines() if l.strip()]
    if args.module:
        entries = [e for e in entries if e["module"] == args.module]
    if args.limit:
        entries = entries[:args.limit]
    print(f"auditing {len(entries)} banked matches with {args.jobs} workers ...", flush=True)

    results = []
    done = 0
    with ThreadPoolExecutor(max_workers=args.jobs) as ex:
        futs = [ex.submit(audit_entry, e, name_index, config_relocs, sym_index) for e in entries]
        for f in futs:
            results.append(f.result())
            done += 1
            if done % 250 == 0:
                print(f"  {done}/{len(entries)}", flush=True)

    cat = Counter(r["verdict"] for r in results)
    print("\n=== reloc-destination audit ===")
    for k in ["CLEAN", "MISSING", "UNRESOLVED", "EXTRA", "WRONG-DEST",
              "NO-REPRO", "NO-SYM"]:
        if cat.get(k):
            print(f"  {k:13} {cat[k]}")

    flagged = [r for r in results if r["verdict"] == "WRONG-DEST"]
    if flagged:
        print(f"\n--- {len(flagged)} matches with a wrong relocation destination ---")
        for r in flagged[:40]:
            print(f"\n{r['module']:7} {r['addr']} {r['name']}")
            for row in r["relocs"]:
                if row["verdict"] == "WRONG-DEST":
                    print(f"    {row['off']:6} cand {row['cand']} ({row['cand_addr']}) "
                          f"!= config {row['cfg']}")

    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(results, indent=1))
        print(f"\nfull detail -> {args.json}")


if __name__ == "__main__":
    main()
