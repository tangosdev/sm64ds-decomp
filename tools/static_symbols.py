"""Which C++ symbols are static or namespace-scoped, so r0 is NOT `this`.

`tools/evidence_rom.py` records memory accesses whose base register still holds the
value r0 arrived with, and calls that `this`. For an instance method it is. For a
static member function or a namespace-scoped free function, r0 is simply the first
argument -- and every access it makes gets filed against a class whose memory it may
never touch.

Nothing in the Itanium mangling distinguishes the two: `_ZN5Stage14GraphCallback2E...`
looks identical either way, because a non-static method's `this` is implicit and never
appears in the encoding. That is exactly why it needs the source.

**The test.** Compare the mangled parameter count with the C definition's:

    instance method   C has one more parameter than the mangling (the explicit this),
                      or spells `Class::method(...)` so `this` is implicit
    static/namespace  the counts are equal -- r0 is a real argument

Measured over the functions evidence_rom attributes to a bannered header: 278 of
1,615 (17%) are static, across 122 classes. `G2x` is the extreme case -- its methods
take a `volatile u16*` hardware register pointer, so the entire struct is fiction --
and `Stage` contributes 20, `Message` 18, `SaveData` 16.

    python tools/static_symbols.py --report
    python tools/static_symbols.py -o build/static_symbols.json
"""
import argparse
import collections
import json
import pathlib
import re
import sys

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))
import demangle                                     # noqa: E402

SYM = re.compile(r"^(\S+)\s+kind:function\(arm,size=(0x[0-9a-fA-F]+)\)\s+addr:(0x[0-9a-fA-F]+)")


def n_params(sig):
    """Parameter count of a demangled signature, ignoring commas inside <> and ()."""
    a = sig[sig.rfind("(") + 1: sig.rfind(")")].strip()
    if not a or a == "void":
        return 0
    depth, n = 0, 1
    for ch in a:
        if ch in "(<":
            depth += 1
        elif ch in ")>":
            depth -= 1
        elif ch == "," and depth == 0:
            n += 1
    return n


def classify(sym, text):
    """-> ('instance'|'static'|reason, detail). Never guesses: an unparsable
    definition is its own outcome, not a default."""
    try:
        sig = demangle.signature(sym)
    except Exception:
        return "undecided: demangle failed", None

    # rung-3 form: `void Timer::GetTime() {` -- `this` is implicit, so it is a method
    qualified = sig[:sig.find("(")]
    if re.search(re.escape(qualified) + r"\s*\(", text):
        return "instance", "C++ method form"

    # rung-2 form: the mangled name is spelled directly, so `this` is an explicit arg
    m = re.search(re.escape(sym) + r"\s*\(([^)]*)\)\s*\{", text, re.S)
    if not m:
        return "undecided: definition not found", None
    args = m.group(1).strip()
    c_n = 0 if args in ("", "void") else n_params("(" + args + ")")
    m_n = n_params(sig)
    if c_n == m_n + 1:
        return "instance", f"explicit this ({c_n} = {m_n}+1)"
    if c_n == m_n:
        return "static", f"r0 is argument 1 ({c_n} == {m_n})"
    return f"undecided: arity {c_n} vs {m_n}", None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=".")
    ap.add_argument("-o", "--out", default="build/static_symbols.json")
    ap.add_argument("--report", action="store_true")
    args = ap.parse_args()
    root = pathlib.Path(args.root).resolve()

    srcs = {p.stem: p for p in (root / "src").glob("*.c*")}
    verdicts, detail, r = {}, {}, collections.Counter()
    for st in (root / "config" / "arm9").rglob("symbols.txt"):
        for line in st.read_text(errors="replace").splitlines():
            m = SYM.match(line)
            if not m or not m.group(1).startswith("_ZN"):
                continue
            sym = m.group(1)
            r["symbols_seen"] += 1
            p = srcs.get(sym)
            if p is None:
                r["no_src_file"] += 1
                continue
            kind, why = classify(sym, p.read_text(errors="replace"))
            r[kind.split(":")[0] if kind.startswith("undecided") else kind] += 1
            if kind.startswith("undecided"):
                r[f"undecided_{kind.split(': ')[1].split(' ')[0]}"] += 1
            verdicts[sym] = kind if not kind.startswith("undecided") else "undecided"
            if why:
                detail[sym] = why

    statics = sorted(s for s, k in verdicts.items() if k == "static")
    out = {
        "meta": {
            "pass": "static_symbols",
            "note": "r0 is NOT `this` in these; evidence_rom must not attribute "
                    "their accesses to the class named in the symbol.",
            "recall": dict(sorted(r.items())),
        },
        "static": statics,
        "instance": sorted(s for s, k in verdicts.items() if k == "instance"),
        "undecided": sorted(s for s, k in verdicts.items() if k == "undecided"),
    }
    dest = root / args.out
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_text(json.dumps(out, indent=1), encoding="utf-8")

    tot = max(1, r["instance"] + r["static"])
    print(f"symbols seen                 : {r['symbols_seen']}")
    print(f"  no src file (undecidable)  : {r['no_src_file']}")
    print(f"  instance methods           : {r['instance']}")
    print(f"  STATIC / namespace         : {r['static']}  ({100*r['static']/tot:.1f}%)")
    print(f"  undecided                  : {len(out['undecided'])}")
    print(f"\nwrote {args.out}")
    print("UNDECIDED IS NOT INSTANCE. A symbol this pass could not classify is left "
          "out of both lists; a consumer that treats it as an instance method is "
          "assuming the thing this pass exists to establish.")


if __name__ == "__main__":
    main()
