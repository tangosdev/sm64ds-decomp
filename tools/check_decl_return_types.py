"""Confirm a mangled symbol is declared with ONE return type, not two.

Every recovered C++ method is declared twice, in files that never see each other:

    include/decl_common.h    extern void _ZN11dScMgBase_c16OnAimedAtWithEggEv(void*);
    include/dScMgBase_c.h    virtual int  OnAimedAtWithEgg();          /* slot 29 */

The compiler never compares them -- neither header includes the other, and a
caller reaching the symbol through one of them compiles cleanly no matter what
the other says. The linker binds on the mangled name alone and does not carry a
return type, so the disagreement survives every existing gate: the byte gates
pass because each translation unit is internally consistent, and
check_header_offsets only looks at field layout.

The failure this prevents is a caller that reads r0 from a callee which never
wrote it (or the reverse), reached through whichever of the two declarations
happened to be in scope. It is silent at compile time and at link time.

Measured on pr/2102 by hand before this tool existed: 4 disagreements. Run over
the same four refs, this reproduces that exactly --

    origin/main                 2   (joined 7)
    origin/cpp/minigame-slot28  2   (joined 7)
    origin/cpp/minigame-slot29  3   (joined 8)
    origin/cpp/minigame-slot30  4   (joined 9)

-- from a wider join than the hand census managed, which missed the eight
declarations that glue `*` or `&` to the method name.

Two of the four (dScMgBase_c::BeforeInitResources, ::AfterInitResources) predate
the slot work and are on main today, which is why this is NOT yet wired into CI
-- see the `--summary` accounting and notes in the PR that introduced it.

    python tools/check_decl_return_types.py
    python tools/check_decl_return_types.py --summary   # show what was NOT checked

There is deliberately no --changed mode. The invariant is global to one file
plus every class header, so a whole-tree check is both cheap and the only honest
one: editing a class header can break agreement with a decl_common.h row that
the diff does not touch, and vice versa.

Exit status is 1 if any symbol is declared with two different return types, and
0 otherwise. A row that cannot be joined to a class virtual is reported as
UNJOINED, never counted as agreement -- see summarize() for why most of them
structurally cannot disagree.
"""
import re, sys, pathlib

REPO = pathlib.Path(__file__).resolve().parent.parent

# `extern <type> _ZN...(` -- the type may end in `*` with no space before the
# symbol, as in `extern void*_ZN2G212GetBG1ScrPtrEv(void);`, so the symbol is
# matched first and the type is whatever precedes it.
DECL_ROW = re.compile(r"^extern\s+(.+?)\s*(_Z\w+)\s*\(", re.M)
# A class or struct body. Brace matching is done by hand, not by regex, because
# these bodies nest (unions, anonymous structs) and a non-greedy `\{.*?\}` stops
# at the first inner close.
CLASS_OPEN = re.compile(r"^(?:struct|class)\s+(\w+)\b[^;{]*\{", re.M)
# `virtual int  Foo();` -- return type and name. The whitespace spans newlines, so
# a declaration split across two lines is read correctly; that is measured, not
# assumed (test_multiline_declaration_is_parsed).
#
# The return type excludes `;` `(` `)` `{` `}` so a match cannot run past the end
# of one declaration. With a permissive `.+?` there,
#
#     virtual ~dActor_c();
#     ...
#     dActor_c(s16 profile);           <- the constructor, lines later
#
# matched as return type `~dActor_c();` and method name `dActor_c`, borrowing its
# `(` from the constructor. That both double-counted the destructor and injected
# a virtual named after the class into the map.
#
# The separator is whitespace OR nothing at all when the type ends in `*` or `&`,
# because this tree glues the sigil to the name:
#
#     virtual Vector3 &GetPos();
#     virtual void *Unk_020c76d0() = 0;
#
# Eight declarations here are spelled that way. A plain `\s+` separator reads
# none of them.
VIRTUAL = re.compile(r"^\s*virtual\s+([^;(){}]+?)(?:\s+|(?<=[*&])\s*)(~?\w+)\s*\(",
                     re.M)
# `virtual ~Foo();` does NOT match VIRTUAL -- there is only one token after
# `virtual`, and VIRTUAL needs two. Recognised separately so it is scored as
# handled-and-dropped rather than as a blind spot.
DESTRUCTOR = re.compile(r"^\s*virtual\s+~\w+\s*\(", re.M)
# Any line that opens with `virtual` at all, so the tool can report how many it
# failed to parse instead of quietly under-reaching.
VIRTUAL_ANY = re.compile(r"^\s*virtual\b", re.M)
COMMENT = re.compile(r"/\*.*?\*/|//[^\n]*", re.S)


def strip_comments(text):
    """Blank out comments, preserving newlines so line structure survives.

    Not cosmetic. The headers here carry heavy prose, and VIRTUAL's `\\s+` spans
    newlines, so a destructor followed by a comment containing a parenthesis --

        virtual ~fBase_c();                    /* slots 16 (D1), 17 (D0) */

    -- matches as return type `~fBase_c(); /* slots` and method name `16`,
    taking its `(` from the comment. That invents a virtual named `16`, and on
    this tree it did so in 116 class bodies. Comments are also the reason
    VIRTUAL_ANY over-counted: prose lines beginning with the word `virtual` are
    not declarations and must not be scored as blind spots either.
    """
    return COMMENT.sub(lambda m: "\n" * m.group(0).count("\n"), text)


def parse_nested(sym):
    """Recover (class, method) from `_ZN<len>Class<len>MethodE...`.

    Parsing backward out of the mangled name rather than forward from a header
    is what makes this complete. A forward matcher -- mangle each declared
    virtual and look it up -- has to guess parameter encodings to build the
    string, so it silently misses every method whose arity or parameter types it
    guesses wrong. The mangled name already carries the answer.

    Returns None for anything that cannot carry a return type: vtable and
    typeinfo data (`_ZTV`, `_ZTI`, `_ZTS`), constructors and destructors, and
    non-nested names.
    """
    if not sym.startswith("_ZN"):
        return None
    s, parts = sym[3:], []
    while s and s[0].isdigit():
        n = 0
        while s and s[0].isdigit():
            n = n * 10 + int(s[0])
            s = s[1:]
        if len(s) < n:
            return None
        parts.append(s[:n])
        s = s[n:]
    # A structor has no return type to disagree about: the component encoding
    # (C1/C2/C3, D0/D1/D2) sits where the method name would be.
    if s[:2] in ("C1", "C2", "C3", "D0", "D1", "D2"):
        return None
    if not s.startswith("E") or len(parts) < 2:
        return None
    return parts[-2], parts[-1]


def norm_type(t):
    """Collapse spelling differences that are not type differences.

    `void *` and `void*` are the same type; `bool` and `int` are NOT, and must
    stay distinct -- a bool return goes through a widening cast under
    mwccarm 2004/b56 that an int return does not, so conflating them here would
    hide the one disagreement in this family that can cost bytes.
    """
    t = " ".join(t.split())
    t = re.sub(r"\s*\*\s*", "*", t)
    return t.replace("extern ", "").strip()


def class_virtuals(root):
    """Map class name -> list of (return type, method name) for every header.

    Also returns the number of `virtual` lines that did not parse, so the caller
    can report the tool's own reach rather than implying it read everything.
    Destructors are handled, not missed: they are recognised by DESTRUCTOR and
    deliberately dropped because they have no return type, so they must not be
    added to the unparsed count -- doing so reported 274 phantom blind spots on
    a tree whose real figure is zero, which would make a reader distrust a green
    run. They need their own pattern because `virtual ~Foo();` has one token
    after `virtual` and VIRTUAL requires two.
    """
    out, unparsed = {}, 0
    for path in sorted((root / "include").rglob("*.h")):
        text = strip_comments(path.read_text(encoding="utf-8", errors="replace"))
        for m in CLASS_OPEN.finditer(text):
            cls, i, depth = m.group(1), m.end(), 1
            while i < len(text) and depth:
                if text[i] == "{":
                    depth += 1
                elif text[i] == "}":
                    depth -= 1
                i += 1
            body = text[m.end():i]
            handled = len(DESTRUCTOR.findall(body))
            for v in VIRTUAL.finditer(body):
                handled += 1
                if v.group(2).startswith("~"):
                    continue                     # e.g. `virtual inline ~Foo()`
                out.setdefault(cls, []).append((norm_type(v.group(1)), v.group(2)))
            unparsed += len(VIRTUAL_ANY.findall(body)) - handled
    return out, unparsed


def check(root=REPO):
    """Return (disagreements, accounting). Reads the tree, decides nothing."""
    decl = strip_comments((root / "include" / "decl_common.h")
                          .read_text(encoding="utf-8", errors="replace"))
    virt, unparsed = class_virtuals(root)
    known = set(virt)

    bad, acct = [], {"rows": 0, "no_return_type": 0, "not_a_class": 0,
                     "not_virtual": 0, "ambiguous": 0, "joined": 0,
                     "unparsed_virtuals": unparsed}
    for m in DECL_ROW.finditer(decl):
        dtype, sym = norm_type(m.group(1)), m.group(2)
        acct["rows"] += 1
        pair = parse_nested(sym)
        if pair is None:
            acct["no_return_type"] += 1
            continue
        cls, meth = pair
        if cls not in known:
            # Almost always an SDK namespace (GX, Sound, cstd): a namespaced
            # free function, which has no vtable slot and so cannot disagree.
            acct["not_a_class"] += 1
            continue
        cand = [v for v in virt[cls] if v[1] == meth]
        if not cand:
            acct["not_virtual"] += 1
            continue
        if len(cand) > 1:
            # Overloads share a name, so the join is not unique. Reported rather
            # than resolved: picking one would be a guess, and a wrong guess here
            # is a false failure on a correct tree.
            acct["ambiguous"] += 1
            continue
        acct["joined"] += 1
        if cand[0][0] != dtype:
            bad.append((cls, meth, cand[0][0], dtype, sym))
    return bad, acct


def main(argv):
    root = REPO
    if "--root" in argv:
        root = pathlib.Path(argv[argv.index("--root") + 1]).resolve()
    if not (root / "include" / "decl_common.h").is_file():
        # Reached by pointing --root at an extracted tree with a mistyped or
        # shell-mangled path. Saying so beats a traceback: this is a gate, and a
        # gate that crashes is indistinguishable from a gate that failed.
        print(f"no include/decl_common.h under {root}", file=sys.stderr)
        return 2
    bad, acct = check(root)

    for cls, meth, ctype, dtype, sym in bad:
        print(f"DISAGREE {cls}::{meth}: class header says '{ctype}', "
              f"decl_common.h says '{dtype}'  [{sym}]")

    if "--summary" in argv or bad:
        # Printed on failure too: the count of rows that could not be joined is
        # the honest bound on this check, and a reader deciding whether to trust
        # a green run needs it as much as one reading a red one.
        print(f"\n  decl_common.h rows              {acct['rows']}")
        print(f"    no return type (vtable/structor) {acct['no_return_type']}")
        print(f"    not a class in include/          {acct['not_a_class']}")
        print(f"    class method, not virtual        {acct['not_virtual']}")
        print(f"    ambiguous (overloads)            {acct['ambiguous']}")
        print(f"    JOINED and compared              {acct['joined']}")
        if acct["unparsed_virtuals"]:
            print(f"  virtual lines not parsed         {acct['unparsed_virtuals']}"
                  "  (not checked, and NOT counted as agreement)")

    print(f"{len(bad)} symbol(s) declared with two different return types")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
