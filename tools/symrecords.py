"""The two symbol records, joined the only safe way: by address.

This repo keeps two address-keyed name records, and they are not copies of each
other:

  config/**/symbols.txt   what we ASSERT. One file per module (arm9, itcm, dtcm,
                          ovNNN), and the record the ROM build actually links
                          against. Corrections land here first: class-naming
                          waves, D1/D2 identity fixes (#774, #1203), signature
                          fixes proven by migration byte-matches (#1226).
  symbols/verified.tsv    the import mirror (tools/import_symbols.py). Flat
                          addr<TAB>name<TAB>kind, no module column. Rows get
                          restamped to the current config spelling when a
                          correction happens to touch them, and keep the
                          import-time spelling when it does not -- so at any
                          moment hundreds of addresses spell a different name in
                          each record.

Both facts are fine on their own. What is NOT fine is joining the records by
name. On 2026-08-30 a name-keyed "is this body sourced?" sweep took names from
one record and looked them up against work keyed to the other; every divergent
address came back "missing" and the sweep manufactured phantom crack targets out
of functions that were matched, enrolled, and building (`func_ov062_0211bd10`,
`_ZN13TreasureChest6State0Ev`). No name has that failure mode as an address: a
symbol's address is the same fact in both records, in delinks.txt, and in the
provenance ledger.

THE RULE (see also notes/symbol-name-provenance.md):

  * Target-selection, annotation, and is-it-done tooling keys on ADDRESS
    (module-qualified for overlays -- overlay slots alternate, one address can
    be two unrelated functions, e.g. ov002/ov004 both based at 0x020ad3e0).
  * A NAME may only be looked up in the record the artifact in hand actually
    links against. Source in src/ links against config/**/symbols.txt; nothing
    in this tree links against verified.tsv.
  * Anything that wants both records goes through this module, which performs
    the address join and reports disagreement instead of silently picking a
    side. tools/test_symrecords.py pins the list of files allowed to open
    verified.tsv directly.

The CLI enumerates every divergence (banked as symbols/record_divergences.tsv),
explains each one mechanically where it can (class-rename wave, dtor/ctor
variant fix, signature fix, const-qualifier fix), and can print or apply the
restamp of the mechanically-explained verified.tsv rows to the current config
spelling. `--apply` exists so a reviewed PR is reproducible; nothing runs it on
its own.

Usage:
  python tools/symrecords.py                     # summary counts
  python tools/symrecords.py --report            # write symbols/record_divergences.tsv
  python tools/symrecords.py --where 0x0203562c  # both records + source status, one addr
  python tools/symrecords.py --restamp           # print the mechanical restamp plan
  python tools/symrecords.py --restamp --apply   # rewrite those verified.tsv rows
"""
from __future__ import annotations

import argparse
import pathlib
import re
import sys
from collections import defaultdict

_TOOLS_DIR = pathlib.Path(__file__).resolve().parent
if str(_TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(_TOOLS_DIR))

import demangle as DM  # noqa: E402

REPO = _TOOLS_DIR.parent
VERIFIED = REPO / "symbols" / "verified.tsv"
REPORT = REPO / "symbols" / "record_divergences.tsv"

# One line of a config symbols.txt. The kind payload varies -- function(arm,size=0x..),
# data(any), ... -- and is irrelevant here beyond the kind word itself.
CONFIG_LINE_RE = re.compile(r"^(\S+)\s+kind:(\w+)(?:\([^)]*\))?\s+addr:0x([0-9a-fA-F]+)")
PLACEHOLDER_RE = re.compile(r"^(?:func|data)_(?:ov\d+_)?[0-9a-fA-F]{7,8}$")

# Verdicts a restamp may act on. Everything else needs a human: `identity` means the
# records name genuinely different things at that address (the KillByAttack class of
# find), `plain` that at least one side is not a mangled name, `ambiguous-module`
# that overlapping overlays give the flat record two candidate truths.
# `arg-rename` is a class-rename observed inside an argument list (same wave, same
# evidence); `signature-fix` is a REAL arity/type correction and worth a closer look
# in review even though config -- where migration byte-matches land -- is the newer
# record by construction.
# `canonical-spelling` means every part this module compares -- class path, member,
# demangled arguments, const-ness, variant, thunk fields -- is identical and only the
# mangled encoding differs (a hand-spelled name vs the compiler's substitution form,
# e.g. `...EPcjPc` for the emitted `...EPcjS0_`).
MECHANICAL = {"class-rename", "dtor-variant", "ctor-variant", "arg-rename",
              "signature-fix", "qualifier-fix", "canonical-spelling"}

REPORT_COLUMNS = ["addr", "state", "modules", "config_names", "config_kinds",
                  "verified_name", "verified_kind", "verdict", "proposed", "note"]


def is_placeholder(name: str) -> bool:
    return bool(PLACEHOLDER_RE.match(name))


def load_config(repo=None):
    """addr -> [(module, name, kind)] across the full module universe."""
    root = pathlib.Path(repo) if repo is not None else REPO
    import relocs  # noqa: E402  (deferred: pure-data callers and tests skip it)
    out = defaultdict(list)
    for symbols_path, label in relocs.module_universe(repo=root):
        for line in symbols_path.read_text(encoding="utf-8",
                                           errors="ignore").splitlines():
            m = CONFIG_LINE_RE.match(line)
            if m:
                out[int(m.group(3), 16)].append((label, m.group(1), m.group(2)))
    return dict(out)


def load_verified(repo=None):
    """addr -> (name, kind) from the flat import mirror."""
    root = pathlib.Path(repo) if repo is not None else REPO
    out = {}
    for line in (root / "symbols" / "verified.tsv").read_text(
            encoding="utf-8", errors="ignore").splitlines():
        parts = line.rstrip("\n").split("\t")
        if len(parts) >= 3:
            out[int(parts[0], 16)] = (parts[1], parts[2])
    return out


_THUNK_RE = re.compile(r"^_ZT(?:hn\d+|v\d+_n\d+)_(N.*)$")
_SPECIAL_RE = re.compile(r"^_ZT([VIS])(.+)$")
_LEN_RE = re.compile(r"(\d+)")


def _mangled_path(sym: str):
    """(components, is_const) of a mangled definition name, or None to refuse.

    The path is read straight off the mangled spelling because that is where it is
    always literal; tools/demangle.py collapses the enclosing path for ctors and
    dtors (`_ZN9ActorBase9SceneNodeC1Ev` and `_ZN7fBase_c9SceneNodeC1Ev` both come
    back as `SceneNode::SceneNode`), which is exactly the information this module
    exists to compare. Anything this parser does not fully understand -- operators,
    substitutions, templates in the path itself -- makes it refuse rather than
    guess, and a refusal downgrades the row to `plain`: a human looks instead of a
    restamp firing.
    """
    m = _THUNK_RE.match(sym)
    if m:
        sym = "_Z" + m.group(1)
    if sym.startswith("_ZN"):
        body, nested = sym[3:], True
    elif sym.startswith("_Z"):
        body, nested = sym[2:], False
    else:
        return None
    const = nested and body.startswith("K")
    if const:
        body = body[1:]
    comps, i = [], 0
    while i < len(body):
        c = body[i]
        if c.isdigit():
            m = _LEN_RE.match(body, i)
            n = int(m.group(1))
            start = m.end()
            comps.append(body[start:start + n])
            i = start + n
            if not nested:
                return (comps, const)          # _Z<len>name<args>: one component
            if i < len(body) and body[i] == "I":
                return None                    # template in the path: refuse
        elif body[i:i + 2] in ("C1", "C2", "C3", "D0", "D1", "D2"):
            comps.append("@" + body[i:i + 2][0].lower())
            i += 2
        elif c == "E" and nested:
            return (comps, const) if comps else None
        else:
            return None                        # operator / substitution / unknown
    return (comps, const) if (comps and not nested) else None


def _parts(name: str):
    """Comparison key for one symbol, or None for a name we refuse to interpret.

    Raw mangled strings cannot be compared piecewise: Itanium substitutions mean
    `RNS_6SystemEb` and `RN8Particle6SystemEb` are the SAME parameter list spelled
    from different nesting depths. So arguments come demangled (tools/demangle.py)
    while the class path comes off the raw spelling (see _mangled_path).
    """
    if not name.startswith("_Z"):
        return None
    m = _SPECIAL_RE.match(name)
    if m and not _THUNK_RE.match(name):
        path = _mangled_path("_Z" + m.group(2))
        if path is None or any(p.startswith("@") for p in path[0]):
            return None
        return {"special": m.group(1), "classpath": "::".join(path[0])}
    try:
        d = DM.demangle(name)
    except Exception:
        return None
    path = _mangled_path(name)
    if not isinstance(d, dict) or path is None:
        return None
    comps, const = path
    member = comps[-1]
    return {
        "special": None,
        "classpath": "::".join(comps[:-1]),
        "member": member,
        "const": const,
        "args": tuple(d.get("args") or ()),
        "dtor": member == "@d",
        "ctor": member == "@c",
        "variant": d.get("variant"),
        "thunk": (bool(d.get("thunk")), d.get("thunk_offset"),
                  d.get("thunk_virtual_offset")),
    }


def _arg_mapper(cmap):
    """A function rewriting class names inside demangled argument strings.

    `Camera::LookAtExit(Actor &)` vs `(dActor_c &)` is the same rename wave seen
    through an argument list, not a signature change; mapping the verified side's
    classes forward before comparing keeps the two cases distinguishable.
    """
    keys = sorted((k for k, v in cmap.items() if k != v), key=len, reverse=True)
    if not keys:
        return lambda args: args
    pat = re.compile(r"\b(" + "|".join(re.escape(k) for k in keys) + r")\b")
    return lambda args: tuple(pat.sub(lambda m: cmap[m.group(1)], a) for a in args)


def class_rename_map(config, verified):
    """verified classpath -> config classpath, where the mapping is consistent.

    Built only from address pairs whose member name and argument list already
    agree -- the rows where the class is the only thing that moved. Dtor and ctor
    pairs contribute NOTHING here, deliberately: compiler-generated dtor bodies
    are routinely byte-identical across unrelated classes (the #774 lesson), so a
    lone dtor row must not manufacture its own rename evidence -- its class pair
    has to be corroborated by a method row. A verified class seen mapping to two
    different config classes proves nothing mechanical and is dropped entirely,
    and an address where two modules both assert a real name is skipped outright
    -- compare() refuses to explain such a row, so it may not feed one either.

    Two passes: the first from exact argument agreement, the second re-admitting
    rows whose arguments agree once the first pass's renames are applied to them
    (a method of a renamed class taking another renamed class as a parameter).
    """
    cmap = {}
    for _ in range(2):
        argmap = _arg_mapper(cmap)
        seen = defaultdict(set)
        for addr, (vname, _vkind) in verified.items():
            vp = _parts(vname)
            if vp is None or vp["special"] or vp["dtor"] or vp["ctor"]:
                continue
            entries = config.get(addr, ())
            # The same floor compare() applies before it will explain a row.
            # Overlay slots alternate, so one address can be two unrelated
            # functions; when two modules both assert a REAL name there, the
            # mirror's single row cannot say which of them it mirrors.
            # compare() calls that "ambiguous-module" and proposes nothing --
            # an address it refuses to draw a conclusion FOR must not supply
            # evidence to conclusions ABOUT other addresses either.
            if len({n for _m, n, _k in entries if not is_placeholder(n)}) > 1:
                continue
            for _mod, cname, _ckind in entries:
                cp = _parts(cname)
                if cp is None or cp["special"] or cp["dtor"] or cp["ctor"]:
                    continue
                if vp["member"] == cp["member"] and argmap(vp["args"]) == cp["args"]:
                    seen[vp["classpath"]].add(cp["classpath"])
        cmap = {v: next(iter(cs)) for v, cs in seen.items() if len(cs) == 1}
    return cmap


def _verdict(cname, vname, cmap, argmap):
    """Explain one config-name/verified-name disagreement, or fail to."""
    cp, vp = _parts(cname), _parts(vname)
    if cp is None or vp is None:
        return "plain"
    if vp["special"] or cp["special"]:
        if vp["special"] != cp["special"]:
            return "identity"
        if cmap.get(vp["classpath"]) == cp["classpath"]:
            return "class-rename"
        return "identity"
    if vp["thunk"] != cp["thunk"]:
        return "identity"
    pieces = []
    if vp["classpath"] != cp["classpath"]:
        if cmap.get(vp["classpath"]) != cp["classpath"]:
            return "identity"
        pieces.append("class-rename")
    if vp["dtor"] or cp["dtor"]:
        if not (vp["dtor"] and cp["dtor"]):
            return "identity"
        if vp["variant"] != cp["variant"]:
            pieces.append("dtor-variant")
    elif vp["ctor"] or cp["ctor"]:
        if not (vp["ctor"] and cp["ctor"]):
            return "identity"
        if vp["variant"] != cp["variant"]:
            pieces.append("ctor-variant")
    else:
        if vp["member"] != cp["member"]:
            return "identity"
        if vp["args"] != cp["args"]:
            pieces.append("arg-rename" if argmap(vp["args"]) == cp["args"]
                          else "signature-fix")
    if vp["const"] != cp["const"]:
        pieces.append("qualifier-fix")
    if not pieces:
        return "canonical-spelling"
    return "+".join(pieces)


def compare(config, verified):
    """Every address where the records disagree, as report rows (dicts).

    States:
      DIVERGED            config has a real name there; the verified spelling is
                          not among the names any module gives that address
      KIND-MISMATCH       same name, different kind claim
      CONFIG-PLACEHOLDER  config never adopted a name here (func_/data_ slot)
      NO-SLOT             the address is not a symbol start in any module
    """
    cmap = class_rename_map(config, verified)
    argmap = _arg_mapper(cmap)
    name_home = defaultdict(list)
    for addr, entries in config.items():
        for mod, cname, _ckind in entries:
            name_home[cname].append((addr, mod))

    rows = []
    for addr in sorted(verified):
        vname, vkind = verified[addr]
        entries = config.get(addr, [])
        cnames = [n for _m, n, _k in entries]
        note = ""
        if not entries:
            state, verdict, proposed = "NO-SLOT", "no-slot", ""
        elif vname in cnames:
            kinds = {k for _m, n, k in entries if n == vname}
            if vkind in kinds:
                continue
            state, verdict, proposed = "KIND-MISMATCH", "kind", ""
            note = "config kind " + ",".join(sorted(kinds))
        elif all(is_placeholder(n) for n in cnames):
            state, verdict, proposed = "CONFIG-PLACEHOLDER", "placeholder", ""
        else:
            state = "DIVERGED"
            real = sorted({n for n in cnames if not is_placeholder(n)})
            if len(real) > 1:
                verdict, proposed = "ambiguous-module", ""
            else:
                verdict = _verdict(real[0], vname, cmap, argmap)
                proposed = real[0] if is_mechanical(verdict) else ""
            homes = [(a, m) for a, m in name_home.get(vname, ()) if a != addr]
            if homes:
                note = "verified spelling lives at " + ",".join(
                    f"0x{a:08x}({m})" for a, m in sorted(homes))
        rows.append({
            "addr": f"0x{addr:08x}",
            "state": state,
            "modules": ",".join(m for m, _n, _k in entries) or "-",
            "config_names": ";".join(dict.fromkeys(cnames)) or "-",
            "config_kinds": ";".join(dict.fromkeys(k for _m, _n, k in entries)) or "-",
            "verified_name": vname,
            "verified_kind": vkind,
            "verdict": verdict,
            "proposed": proposed,
            "note": note,
        })
    return rows


def is_mechanical(verdict):
    """True iff every '+'-joined piece of the verdict is a mechanical class."""
    return all(p in MECHANICAL for p in verdict.split("+"))


def restamp_plan(rows):
    """(addr, old verified name, new name) for every mechanically-explained row."""
    return [(int(r["addr"], 16), r["verified_name"], r["proposed"])
            for r in rows if r["proposed"]]


def apply_restamp(plan, repo=None):
    """Rewrite only the name field of the planned verified.tsv rows, in place."""
    root = pathlib.Path(repo) if repo is not None else REPO
    path = root / "symbols" / "verified.tsv"
    by_addr = {addr: (old, new) for addr, old, new in plan}
    out, hits = [], 0
    for line in path.read_text(encoding="utf-8").splitlines():
        parts = line.split("\t")
        if len(parts) >= 3 and int(parts[0], 16) in by_addr:
            old, new = by_addr[int(parts[0], 16)]
            if parts[1] == old:
                parts[1] = new
                hits += 1
        out.append("\t".join(parts))
    path.write_text("\n".join(out) + "\n", encoding="utf-8", newline="\n")
    return hits


def write_report(rows, path=None):
    path = pathlib.Path(path) if path is not None else REPORT
    lines = ["\t".join(REPORT_COLUMNS)]
    lines += ["\t".join(r[c] for c in REPORT_COLUMNS) for r in rows]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    return path


def summarize(rows, out=print):
    states = defaultdict(int)
    verdicts = defaultdict(int)
    for r in rows:
        states[r["state"]] += 1
        if r["state"] == "DIVERGED":
            verdicts[r["verdict"]] += 1
    out(f"records disagree on {len(rows)} addresses")
    for s in ("DIVERGED", "KIND-MISMATCH", "CONFIG-PLACEHOLDER", "NO-SLOT"):
        if states.get(s):
            out(f"  {s:<18} {states[s]}")
    if verdicts:
        out("DIVERGED by verdict (mechanical verdicts restampable):")
        for v, n in sorted(verdicts.items(), key=lambda kv: -kv[1]):
            tag = "mechanical" if is_mechanical(v) else "needs a human"
            out(f"  {v:<28} {n:>4}  {tag}")


def where(addr, config, verified, out=print):
    """Both records' view of one address, plus what the build compiles for it."""
    import srcpath as SP  # noqa: E402
    entries = config.get(addr, [])
    out(f"0x{addr:08x}")
    if not entries:
        out("  config:   (not a symbol start in any module)")
    for mod, cname, ckind in entries:
        src = SP.path_for(cname)
        owner = f"  src: {src.relative_to(SP.REPO).as_posix()}" if src else "  src: none"
        out(f"  config:   {mod:<6} {cname}  [{ckind}]{owner}")
    v = verified.get(addr)
    out(f"  verified: {v[0]}  [{v[1]}]" if v else "  verified: (no row)")


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--report", nargs="?", const=str(REPORT), default=None,
                    metavar="PATH", help="write the divergence TSV (default: %(const)s)")
    ap.add_argument("--where", metavar="ADDR",
                    help="show both records + source status for one address")
    ap.add_argument("--restamp", action="store_true",
                    help="print the mechanical verified.tsv restamp plan")
    ap.add_argument("--apply", action="store_true",
                    help="with --restamp: rewrite the planned rows in verified.tsv")
    args = ap.parse_args(argv)

    config = load_config()
    verified = load_verified()

    if args.where:
        where(int(args.where, 16), config, verified)
        return 0

    rows = compare(config, verified)
    summarize(rows)

    if args.report:
        path = write_report(rows, args.report)
        print(f"wrote {pathlib.Path(path).resolve().relative_to(REPO).as_posix()} "
              f"({len(rows)} rows)")
    if args.restamp:
        plan = restamp_plan(rows)
        print(f"\nrestamp plan: {len(plan)} verified.tsv rows -> current config spelling")
        for addr, old, new in plan:
            print(f"  0x{addr:08x}  {old}  ->  {new}")
        if args.apply:
            hits = apply_restamp(plan)
            print(f"applied {hits} of {len(plan)} (re-run --report to refresh the bank)")
        else:
            print("(dry run; --apply rewrites the rows)")
    elif args.apply:
        ap.error("--apply only means something with --restamp")
    return 0


if __name__ == "__main__":
    sys.exit(main())
