#!/usr/bin/env python3
"""Offset -> field-name map for a reconstructed class, and the mechanical
substitution that uses it.

Migrating a `char* c` function to a real C++ method means replacing
`*(u8 *)(c + 0x6e5)` with `mStateWork`. That is a rename, and renames are safe.
What is NOT safe is rewriting the function while you are at it: restructuring
evaluation order and cast widths at the same time as renaming produces a byte
mismatch with no signal about which change caused it. (Measured: one rewritten
function came back `999 words differ`, a size mismatch.)

So this tool only ever renames, and it REFUSES rather than guesses:

  *(T*)(c + 0xNN)   -> field          when T matches the field's width AND signedness
  *(T*)(c + 0xNN)   -> *(T*)&field    when the width matches but the signedness does not
  (c + 0xNN)        -> (&field)       address-takes, parentheses preserved
  anything else     -> left alone, and reported

The signedness rule is the load-bearing one. `*(u16*)` on an `s16` field is an
ldrh where the field's own type would give ldrsh, and that difference is one
instruction. Sometimes it means the header is wrong (Player's mJumpComboTimer
was s16 where every ROM read is unsigned); sometimes the header is right and
the cast is the semantics (mAngleY is genuinely signed, but indexing a sin/cos
table wants the angle to wrap). This tool cannot tell those apart -- it keeps
the distinction visible so a human can.

Usage:
    python tools/fieldmap.py Player                     # map summary
    python tools/fieldmap.py Player --report src/x.cpp  # what it can/cannot name
    python tools/fieldmap.py Player --apply  src/x.cpp --recv c
"""
import argparse
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent

# `s32 mPosX;            /* 0x05c */`  and  `u8 pad_0e1[0x7];`
DECL = re.compile(r"^\s*(\w[\w \*]*?)\s+(\w+)\s*(\[[^\]]*\])?\s*;\s*/\* (0x[0-9a-fA-F]+)")
# `struct Player : Actor {`
STRUCT = re.compile(r"^\s*struct\s+(\w+)\s*(?::\s*(?:public\s+)?(\w+)\s*)?\{", re.M)

WIDTH = {"u8": 1, "s8": 1, "unsigned char": 1, "signed char": 1, "char": 1,
         "u16": 2, "s16": 2, "unsigned short": 2, "short": 2,
         "u32": 4, "s32": 4, "unsigned int": 4, "int": 4, "unsigned": 4, "long": 4}
UNSIGNED = {"u8", "u16", "u32", "unsigned char", "unsigned short", "unsigned int", "unsigned"}


def _norm(t):
    return re.sub(r"\s+", " ", t.strip())


def parse_header(path):
    """{offset: (name, type)} for every commented field in `path`, and the base
    class name if the struct declares one. Padding is skipped: it is absence of
    knowledge, not a field."""
    text = pathlib.Path(path).read_text(errors="replace")
    m = STRUCT.search(text)
    base = m.group(2) if m else None
    out = {}
    for line in text.splitlines():
        d = DECL.match(line)
        if not d:
            continue
        typ, name, _arr, off = d.groups()
        if name.startswith("pad_"):
            continue
        out.setdefault(int(off, 16), (name, _norm(typ)))
    return out, base


def fields_for(cls, repo=REPO):
    """Merge the class's own fields over its bases'. A derived class declares
    everything it knows about itself, so anything it does NOT declare and a
    base does belongs to the base."""
    merged, seen = {}, set()
    chain, cur = [], cls
    while cur and cur not in seen:
        seen.add(cur)
        h = repo / "include" / f"{cur}.h"
        if not h.exists():
            break
        own, base = parse_header(h)
        chain.append((cur, own))
        cur = base
    for _name, own in reversed(chain):        # bases first, derived wins
        merged.update(own)
    return merged, [c for c, _ in chain]


def report(text, fields, recv):
    """Offsets the file uses off `recv`, and whether each can be named."""
    used = sorted({int(o, 16) for o in
                   re.findall(rf"\b{re.escape(recv)}\s*[+]\s*(0x[0-9a-fA-F]+)", text)})
    named, unnamed = [], []
    for off in used:
        (named if off in fields else unnamed).append(off)
    return named, unnamed


def substitute(text, fields, recv):
    """Rename offsets to field names. Returns (new_text, skipped) where
    `skipped` explains every site left alone."""
    skipped = []

    # ONE pass over both forms. Two passes would let the address-take rule take
    # a second bite at a site the deref rule had just refused on a width
    # mismatch, rewriting `*(int *)(c + 0x94)` to `*(int *)(&mFlagByte)` --
    # silently doing the thing it had declined to do.
    SITE = re.compile(
        rf"(?:\*\(([\w ]+?)\s*\*\)\s*)?"
        rf"\(\s*{re.escape(recv)}\s*[+]\s*(0x[0-9a-fA-F]+)\s*\)")

    def site(m):
        cast, off = m.group(1), int(m.group(2), 16)
        if off not in fields:
            kind = "" if cast else " (address-take)"
            skipped.append(f"unnamed offset 0x{off:x}{kind}")
            return m.group(0)
        name, ftyp = fields[off]
        if cast is None:
            # KEEP the parentheses. They are not always a grouping -- in
            # `foo(c+0x380)` they are the call's argument list, and dropping
            # them yields `foo&mMeshClsn`, which still parses far enough to
            # produce a confusing type error a hundred lines away.
            return f"(&{name})"
        cast = _norm(cast)
        cw, fw = WIDTH.get(cast), WIDTH.get(ftyp)
        if cw is None or fw is None or cw != fw:
            skipped.append(f"0x{off:x}: cast {cast!r} vs field {ftyp!r} -- width differs")
            return m.group(0)
        if (cast in UNSIGNED) == (ftyp in UNSIGNED):
            return name
        # Signedness differs: that is an ldrh/ldrsh difference and it is
        # semantic. Keep it visible rather than silently adopting either.
        return f"*({cast}*)&{name}"

    return SITE.sub(site, text), skipped


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("cls", help="class name, e.g. Player (reads include/<cls>.h)")
    ap.add_argument("--report", metavar="SRC", help="list the offsets SRC uses")
    ap.add_argument("--apply", metavar="SRC", help="rewrite SRC in place")
    ap.add_argument("--recv", default="c", help="receiver identifier in SRC (default: c)")
    a = ap.parse_args(argv)

    fields, chain = fields_for(a.cls)
    if not fields:
        print(f"no fields found for {a.cls} -- is include/{a.cls}.h present?")
        return 1

    if not a.report and not a.apply:
        print(f"{a.cls}: {len(fields)} named offsets via {' <- '.join(chain)}")
        return 0

    src = pathlib.Path(a.report or a.apply)
    text = src.read_text(errors="replace")

    if a.report:
        named, unnamed = report(text, fields, a.recv)
        print(f"{src}: {len(named) + len(unnamed)} distinct offsets off {a.recv!r}")
        for off in named:
            n, t = fields[off]
            print(f"  0x{off:<5x} {n:28s} {t}")
        for off in unnamed:
            print(f"  0x{off:<5x} *** UNNAMED ***")
        return 0

    new, skipped = substitute(text, fields, a.recv)
    src.write_text(new)
    for s in sorted(set(skipped)):
        print(f"  left alone -- {s}")
    left = re.findall(rf"\b{re.escape(a.recv)}\s*[+]\s*0x[0-9a-fA-F]+", new)
    print(f"{src}: rewritten, {len(left)} raw {a.recv}+offset site(s) remain")
    return 0


if __name__ == "__main__":
    sys.exit(main())
