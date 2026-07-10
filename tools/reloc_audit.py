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
        for (mod_addr), name in R.load_syms_file(path, module).items():
            mod, addr = mod_addr
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


def object_reloc_dests(obj, func, name_index):
    """[(offset, symname, resolved_module, resolved_addr)] for relocs inside func.

    Returns (None, reason) if the function symbol isn't in the object."""
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    syms = list(symtab.iter_symbols())
    sym = next((s for s in syms if s.name == func), None)
    if sym is None:
        return None, "func-not-in-obj"
    sec = elf.get_section(sym["st_shndx"])
    start, size = sym["st_value"], sym["st_size"]
    rel = elf.get_section_by_name(".rel" + sec.name) or elf.get_section_by_name(".rela" + sec.name)
    dests = []
    if rel is not None:
        for r in rel.iter_relocations():
            o = r["r_offset"] - start
            if not (0 <= o < size):
                continue
            tsym = syms[r["r_info_sym"]]
            res = resolve_candidate(tsym.name, name_index)
            mod, addr = (res if res else (None, None))
            dests.append((o & ~3, tsym.name, mod, addr))
    return dests, size


def winning_object(name, addr, size, mod):
    """Reproduce the match the way reverify does, but return the object that did it.

    Mirrors reverify_corpus.compiles_to so the audit measures exactly what reverify
    blesses -- same sources, same version sweep, same any-symbol acceptance."""
    import reverify_corpus as RV
    import swarm as S
    target = RV.rom_bytes(mod, addr, size)
    if target is None:
        return None, None, "no-module-bin"
    for src in RV.src_texts(name, addr):
        attempts = ([(S.CPP_FLAGS, ".cpp")] if src.startswith("//cpp")
                    else [(M.DEFAULT_FLAGS, ".c"), (S.CPP_FLAGS, ".cpp")])
        for flags, suf in attempts:
            fd, tmp = tempfile.mkstemp(suffix=suf)
            os.close(fd)
            pathlib.Path(tmp).write_text(src)
            try:
                for v in RV.ALL_VERSIONS:
                    obj = M.compile_c(pathlib.Path(tmp), v, flags)
                    if obj is None:
                        continue
                    import probe_versions as PV
                    try:
                        candidate_syms = list(PV.funcs_in(obj).keys())
                    except Exception:
                        candidate_syms = [name]
                    if name not in candidate_syms:
                        candidate_syms = [name] + candidate_syms
                    for sym in candidate_syms:
                        code, relocs = M.extract_func(obj, sym)
                        if code is None or len(code) != len(target):
                            continue
                        ok, _ = M.compare(target, code, relocs, verbose=False)
                        if ok:
                            return obj, sym, None
            finally:
                pathlib.Path(tmp).unlink(missing_ok=True)
    return None, None, "no-repro"


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


def check_destinations(obj, sym, addr, size, mod, name_index, config_relocs, sym_index):
    """Per-reloc destination verdicts for an in-hand object. Shared by the audit
    and the match gate (match.py --strict-relocs).

    Returns (rows, missing) where rows is a list of dicts (one per object reloc in
    the function) and missing is the count of config relocs the candidate lacks.
    Returns (None, reason) if the function symbol isn't in the object."""
    dests, size_or_reason = object_reloc_dests(obj, sym, name_index)
    if dests is None:
        return None, size_or_reason
    cfgmap = config_relocs.get(R.normalize_module(mod), {})
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


def gate_wrong_dests(obj, sym, addr, size, mod):
    """Bank-gate wrapper around check_destinations: WRONG-DEST rows only, with the
    three config indexes built once and cached for the process. Returns [] when the
    object's reloc destinations agree with config, None when the symbol is absent
    from the object (callers treat that as a verification failure)."""
    global _GATE_IDX
    if _GATE_IDX is None:
        _GATE_IDX = (build_name_index(), build_config_relocs(), R.load_all_syms())
    name_index, config_relocs, sym_index = _GATE_IDX
    rows, _missing = check_destinations(obj, sym, addr, size, mod,
                                        name_index, config_relocs, sym_index)
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
                                       name_index, config_relocs, sym_index)
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
