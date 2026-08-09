"""Find src/ files that reference a *window-sibling* overlay's symbol -- a
data_ovNNN_ADDR / func_ovNNN_ADDR token whose ovNNN can never be resident at
the same time as the file's own overlay. Since the two modules can't both be
in memory, a reference like that can only be a mislabeled symbol: dsd (or a
human) named the token off the wrong overlay's symbol table because both
overlays happen to define something at the same address (they occupy the
same load window at different times, so their address ranges overlap).

This is a narrower, standalone cousin of tools/overlay_residency.py's E2
rule, built because that tool crashes on import in this checkout (its E6
level-table decode reads garbage from extracted/arm9/arm9.bin here). Do not
import it. ovsweep re-derives only what it needs (E2 ranges + the E3 and E5
exceptions below) straight from extracted/ and config/, and never touches E6.

Two exceptions clear a raw same-window hit before it counts as a real
candidate:

    E3  ov006 always loads ov004 alongside it (hardcoded in the game's
        LoadOrUnloadObjectOverlays). Their address ranges overlap -- E2 would
        flag any ov006-owned file referencing ov004 -- but they ARE
        co-resident whenever ov006 is loaded at all. This is ASYMMETRIC: it
        only clears references made BY ov006-owned code INTO ov004. A file
        owned by ov004 referencing ov006 gets no such pass, because ov004
        loads without ov006 too (nothing hardcodes the reverse).

    E5  a CALL relocation that dsd resolved unambiguously to a single
        overlay proves the caller's module and that overlay were resident
        together at least once (a BL needs its target in memory). This is a
        general override, keyed off config/**/relocs.txt, independent of E3.
        Loads don't count -- a pointer-shaped word can name an address
        nothing ever dereferences.

E3 and E5 are two different exceptions to E2 that happen to intersect at the
ov004/ov006 pair the sample review flagged (ov006 does have a proven CALL
into ov004 in relocs.txt) -- but E3 is the one to cite for that pair,
because it clears EVERY ov006-owned reference into ov004, not just the ones
covered by a call relocation local to that one file. An earlier draft of
this script folded E3's ov004/ov006 language into its E5 block and labeled
the whole thing "E5"; that was a bug in the draft, not in the underlying
model -- keep the two separate, matching overlay_residency.py's numbering.

Own-overlay attribution (which overlay a src file belongs to) tries, in
order:

    1. filename convention -- func_ovNNN_*.c(pp), data_ovNNN_*.c(pp),
       __sinit_ovNNN_*.c(pp) (static-initializer files; an earlier draft's
       filename regex required the file to START with func_/data_ and never
       matched __sinit_, silently skipping all ~278 of them)
    2. a `// @symbol NAME` header comment, resolved against every overlay's
       symbols.txt
    3. sibling-file inference: for a mangled class-method file (_ZN...Ev.cpp)
       with no @symbol tag, find another file for the SAME class (by the
       mangled class-name prefix) that resolves via (1) or (2), and borrow
       its overlay. This is what catches SkiLift::CleanupResources off the
       back of SkiLift::InitResources, for example -- a class's methods all
       live in the same overlay.

Read-only. `scan` mode prints a summary and writes the full candidate list to
a JSON file. `report` mode reformats a previous scan's JSON as a readable
table. Nothing here edits src/.

Usage:
    python tools/ovsweep.py scan [--out PATH]
    python tools/ovsweep.py report --in PATH [--all]
"""
import argparse
import json
import pathlib
import re
import sys
from collections import Counter

REPO = pathlib.Path(__file__).resolve().parent.parent
CFG_OV = REPO / "config" / "arm9" / "overlays"
SRC = REPO / "src"
DEFAULT_OUT = REPO / "tools" / "ovsweep_out.json"


# --------------------------------------------------------------------- E2 ranges
def load_ranges():
    """ovNNN -> (base_address, base_address + code_size + bss_size)."""
    p = REPO / "extracted" / "dsd" / "arm9_overlays" / "overlays.yaml"
    out, cur = {}, None
    for line in p.read_text(encoding="utf-8", errors="ignore").splitlines():
        m = re.match(r"\s*-\s*id:\s*(\d+)", line)
        if m:
            cur = {"id": int(m.group(1))}
            out[cur["id"]] = cur
            continue
        m = re.match(r"\s+(\w+):\s*(.+)", line)
        if m and cur is not None:
            try:
                cur[m.group(1)] = int(m.group(2).strip())
            except ValueError:
                pass
    return {f"ov{o['id']:03d}": (o["base_address"],
                                 o["base_address"] + o["code_size"] + o.get("bss_size", 0))
            for o in out.values() if "base_address" in o}


def conflict(a, b, rng):
    if a == b or a not in rng or b not in rng:
        return False
    (s1, e1), (s2, e2) = rng[a], rng[b]
    return s1 < e2 and s2 < e1


# --------------------------------------------------------------------- E5 proven calls
_RELOC = re.compile(r"^from:0x[0-9a-fA-F]+\s+kind:(\S+)\s+to:0x[0-9a-fA-F]+\s+"
                    r"module:overlay\((\d+)\)$")


def proven_calls():
    """{module: overlays it demonstrably calls}, from config/**/relocs.txt."""
    out = {}
    for p in (REPO / "config").rglob("relocs.txt"):
        src = p.parent.name
        s = out.setdefault(src, set())
        for ln in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = _RELOC.match(ln.strip())
            if m and "call" in m.group(1):
                t = f"ov{int(m.group(2)):03d}"
                if t != src:
                    s.add(t)
    return out


# --------------------------------------------------------------------- symbols.txt parsing
SYM_LINE = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:0x([0-9a-fA-F]+)")


def parse_symbols(path):
    """name -> (kind_str, addr); also a sorted (addr, name, kind) entry list."""
    syms, entries = {}, []
    if not path.is_file():
        return syms, entries
    for line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        m = SYM_LINE.match(line.strip())
        if not m:
            continue
        name, kind, addr = m.group(1), m.group(2), int(m.group(3), 16)
        syms[name] = (kind, addr)
        entries.append((addr, name, kind))
    entries.sort()
    return syms, entries


def size_at(entries, idx):
    """Approximate size via the gap to the next entry (data lacks explicit size)."""
    if idx + 1 < len(entries):
        return entries[idx + 1][0] - entries[idx][0]
    return None


def addr_to_name_size(ov_syms, ov, addr):
    """Symbol (name, kind, size) at exact addr within ov's own table, or None."""
    if ov not in ov_syms:
        return None
    _, entries = ov_syms[ov]
    for i, (a, name, kind) in enumerate(entries):
        if a == addr:
            explicit = re.search(r"size=0x([0-9a-fA-F]+)", kind)
            size = int(explicit.group(1), 16) if explicit else size_at(entries, i)
            return name, kind, size
    return None


# --------------------------------------------------------------------- own-overlay of a src file
REF_RE = re.compile(r"\b(data|func)_ov(\d{3})_([0-9a-fA-F]{6,8})\b")
SYMBOL_TAG_RE = re.compile(r"^//\s*@symbol\s+(\S+)", re.MULTILINE)
FILENAME_OV_RE = re.compile(r"^(?:func|data|__sinit)_ov(\d{3})_")
MANGLED_CLASS_RE = re.compile(r"^_ZN(\d+)([A-Za-z_]\w*)")


def mangled_class_prefix(name):
    """'_ZN7SkiLift16CleanupResourcesEv' -> '_ZN7SkiLift' (the class-name prefix
    shared by every method of that class), or None if name isn't _ZN-mangled."""
    m = MANGLED_CLASS_RE.match(name)
    if not m:
        return None
    n = int(m.group(1))
    return f"_ZN{n}{m.group(2)[:n]}"


class Attributor:
    """Resolves which overlay owns a src file, with a sibling-file fallback for
    mangled class-method files that carry no filename or @symbol evidence of
    their own (own_overlay() alone can't see these; a sibling method file for
    the same class usually can)."""

    def __init__(self, name_to_ov):
        self.name_to_ov = name_to_ov
        self._class_cache = {}   # class prefix -> ov (or None), filled lazily

    def direct(self, fpath, text):
        """Attribution from this file alone: filename, then @symbol tag."""
        m = FILENAME_OV_RE.match(fpath.name)
        if m:
            return f"ov{m.group(1)}"
        m = SYMBOL_TAG_RE.search(text)
        if m:
            sym = m.group(1)
            m2 = FILENAME_OV_RE.match(sym + "_")  # reuse the same prefix test
            if m2:
                return f"ov{m2.group(1)}"
            owners = self.name_to_ov.get(sym)
            if owners and len(owners) == 1:
                return next(iter(owners))
            if owners and len(owners) > 1:
                return ("AMBIGUOUS", tuple(sorted(owners)))
        return None

    def resolve(self, fpath, text, all_files_text):
        own = self.direct(fpath, text)
        if own is not None:
            return own, "direct"
        # sibling-file fallback for mangled class methods
        cls = mangled_class_prefix(fpath.stem)
        if cls is None:
            return None, None
        if cls not in self._class_cache:
            self._class_cache[cls] = self._resolve_class(cls, all_files_text)
        ov = self._class_cache[cls]
        return (ov, "sibling") if ov is not None else (None, None)

    def _resolve_class(self, cls, all_files_text):
        for f2, text2 in all_files_text:
            if f2.stem == cls or not f2.stem.startswith(cls):
                continue
            # only treat it as a same-class sibling if the mangled length tag matches
            # (avoids "_ZN7SkiLift" prefix-matching "_ZN7SkiLifts..." style false hits)
            rest = f2.stem[len(cls):]
            if rest and not re.match(r"^\d", rest) and not rest[0].isupper():
                pass  # method names/dtor tags follow the class name; accept
            d = self.direct(f2, text2)
            if isinstance(d, str):
                return d
        return None


# --------------------------------------------------------------------- sweep
def load_context():
    rng = load_ranges()
    overlays = sorted(rng.keys())
    siblings = {ov: {o for o in overlays if conflict(ov, o, rng)} for ov in overlays}
    calls = proven_calls()

    ov_syms = {}
    for d in sorted(CFG_OV.glob("ov*")):
        sp = d / "symbols.txt"
        if sp.is_file():
            ov_syms[d.name] = parse_symbols(sp)

    name_to_ov = {}
    for ov, (syms, _entries) in ov_syms.items():
        for name in syms:
            name_to_ov.setdefault(name, set()).add(ov)

    return {"rng": rng, "siblings": siblings, "calls": calls, "ov_syms": ov_syms,
            "name_to_ov": name_to_ov}


def is_e3_cleared(own, ref_ov):
    """The ov006->ov004 hardcoded always-co-loaded exception. Asymmetric: only
    fires when the file's own overlay is ov006 and the reference targets ov004."""
    return own == "ov006" and ref_ov == "ov004"


def scan():
    ctx = load_context()
    siblings, calls, ov_syms, name_to_ov = (
        ctx["siblings"], ctx["calls"], ctx["ov_syms"], ctx["name_to_ov"])
    attributor = Attributor(name_to_ov)

    files = [f for f in sorted(SRC.rglob("*"))
             if f.is_file() and f.suffix in (".c", ".cpp")]
    all_files_text = [(f, f.read_text(encoding="utf-8", errors="ignore")) for f in files]

    candidates, e3_cleared, e5_cleared = [], [], []
    scanned = files_with_refs = 0

    for f, text in all_files_text:
        scanned += 1
        refs = list(REF_RE.finditer(text))
        if not refs:
            continue
        files_with_refs += 1
        own, how = attributor.resolve(f, text, all_files_text)

        seen = set()
        for m in refs:
            kind_tok, ov_num, addr_hex = m.group(1), m.group(2), m.group(3)
            ref_ov = f"ov{ov_num}"
            sym_name = f"{kind_tok}_ov{ov_num}_{addr_hex}"
            if sym_name in seen:
                continue
            seen.add(sym_name)

            if not isinstance(own, str):
                continue
            if ref_ov == own:
                continue
            if ref_ov not in siblings.get(own, set()):
                continue  # not a window-sibling; E2 doesn't flag it

            ref_addr = int(addr_hex, 16)
            row = {
                "file": str(f.relative_to(REPO)).replace("\\", "/"),
                "own_overlay": own,
                "own_overlay_source": how,
                "wrong_symbol": sym_name,
                "wrong_overlay": ref_ov,
                "addr": hex(ref_addr),
                "sibling_symbol_info": addr_to_name_size(ov_syms, ref_ov, ref_addr),
                "own_overlay_symbol_at_addr": addr_to_name_size(ov_syms, own, ref_addr),
            }

            if is_e3_cleared(own, ref_ov):
                e3_cleared.append(row)
                continue
            e5 = ref_ov in calls.get(own, set()) or own in calls.get(ref_ov, set())
            if e5:
                e5_cleared.append(row)
                continue
            candidates.append(row)

    print(f"scanned {scanned} src .c/.cpp files, {files_with_refs} had ovNNN refs",
          file=sys.stderr)
    print(f"cleared by E3 (ov006->ov004 hardcoded co-load): {len(e3_cleared)}", file=sys.stderr)
    print(f"cleared by E5 (proven co-resident CALL relocation): {len(e5_cleared)}",
          file=sys.stderr)
    print(f"real candidates: {len(candidates)}", file=sys.stderr)

    e3_pairs = Counter((r["own_overlay"], r["wrong_overlay"]) for r in e3_cleared)
    e5_pairs = Counter((r["own_overlay"], r["wrong_overlay"]) for r in e5_cleared)
    print("pairs cleared by E3:", dict(e3_pairs), file=sys.stderr)
    print("pairs cleared by E5:", dict(e5_pairs), file=sys.stderr)

    return {
        "num_overlays": len(ctx["rng"]),
        "candidates": candidates,
        "e3_cleared_pairs": {f"{a}->{b}": n for (a, b), n in e3_pairs.items()},
        "e5_cleared_pairs": {f"{a}->{b}": n for (a, b), n in e5_pairs.items()},
    }


def cmd_scan(args):
    out = scan()
    outpath = pathlib.Path(args.out)
    outpath.write_text(json.dumps(out, indent=2), encoding="utf-8")
    print(f"wrote {outpath}", file=sys.stderr)
    return 0


def cmd_report(args):
    inpath = pathlib.Path(args.inp)
    data = json.loads(inpath.read_text(encoding="utf-8"))
    rows = data["candidates"]
    print(f"{len(rows)} candidate(s)\n")
    for r in rows:
        mismatch = ""
        own_hit = r.get("own_overlay_symbol_at_addr")
        sib_hit = r.get("sibling_symbol_info")
        if own_hit and sib_hit and own_hit[1] != sib_hit[1]:
            mismatch = "  [kind mismatch: %s vs %s]" % (own_hit[1], sib_hit[1])
        print(f"{r['file']}")
        print(f"    own={r['own_overlay']} ({r['own_overlay_source']})  "
              f"wrong={r['wrong_symbol']} (in {r['wrong_overlay']})  addr={r['addr']}")
        if own_hit:
            print(f"    should be: {own_hit[0]} (kind:{own_hit[1]})" + mismatch)
        print()
    if args.all:
        print("e3_cleared_pairs:", data.get("e3_cleared_pairs"))
        print("e5_cleared_pairs:", data.get("e5_cleared_pairs"))
    return 0


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    p_scan = sub.add_parser("scan", help="sweep src/ and write candidates to JSON")
    p_scan.add_argument("--out", default=str(DEFAULT_OUT),
                        help=f"output JSON path (default: {DEFAULT_OUT})")
    p_scan.set_defaults(func=cmd_scan)

    p_report = sub.add_parser("report", help="print a previous scan's JSON as a table")
    p_report.add_argument("--in", dest="inp", default=str(DEFAULT_OUT),
                          help=f"input JSON path (default: {DEFAULT_OUT})")
    p_report.add_argument("--all", action="store_true",
                          help="also print the E3/E5 cleared-pair summary")
    p_report.set_defaults(func=cmd_report)

    a = ap.parse_args()
    sys.exit(a.func(a))
