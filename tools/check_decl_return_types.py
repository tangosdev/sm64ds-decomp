"""Confirm a symbol is declared with ONE return type, not two.

Two halves, because decl_common.h holds two kinds of row and they go wrong in
two different ways.

MANGLED ROWS (66 of them) are C++ methods declared twice, in files that never
see each other, and the check joins declaration to declaration. Exit status 1.

PLAIN C ROWS (1250 `func_<addr>` externs) have a DEFINITION in src/ carrying a
return type of its own, and the check joins the declaration to that. ADVISORY:
reported, exit status untouched. See THE PLAIN C HALF below for why the two
share almost no code.

    python tools/check_decl_return_types.py
    python tools/check_decl_return_types.py --summary      # what was NOT checked
    python tools/check_decl_return_types.py --no-plain-c   # mangled rows only

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

There is deliberately no --changed mode. The invariant is global to one file
plus every class header, so a whole-tree check is both cheap and the only honest
one: editing a class header can break agreement with a decl_common.h row that
the diff does not touch, and vice versa.

Exit status is 1 if any MANGLED symbol is declared with two different return
types, and 0 otherwise. A row that cannot be joined to a class virtual is
reported as UNJOINED, never counted as agreement -- see the accounting for why
most of them structurally cannot disagree.
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


# ---------------------------------------------------------------------------
# THE PLAIN C HALF
#
# 1250 of decl_common.h's rows are not mangled names at all. They are the
# unnamed `func_<addr>` externs, and every one of them has a DEFINITION in src/
# carrying a return type of its own:
#
#     include/decl_common.h    extern void func_0200c66c(void*, void*, int*, int*, int*);
#     src/func_0200c66c.c      int func_0200c66c(char *self, ...) { ... return res; }
#
# DECL_ROW above requires `_Z\w+`, so the join in check() cannot see a single
# one of them: a row that says `void` over a body that returns a value has been
# invisible to every gate in this tree. PR #2658 fixed one such row by hand
# (func_ov062_0211b51c) and the count this tool prints did not move, which is
# how the blind spot was noticed. Lane DECL2 then measured the rest by reading
# all 1250 rows: 64 disagreements. This is that audit wired in, so the next one
# is found by the tool and not by a person.
#
# The two halves deliberately share only strip_comments. The mangled half joins
# a DECLARATION to another DECLARATION, so its honest answer when it cannot join
# is UNJOINED. This half joins a declaration to a DEFINITION, so the definition
# is the authority and there is nothing to be ambiguous about -- but the mirror
# trap shows up instead: a spelling difference that is not a type difference.
# `u32` and `unsigned int` ARE one type here (include/types.h says so), and
# reporting those four pairs would bury the 52 rows that matter underneath them.
# That is why this half has its own canonicaliser and norm_type does not grow
# one: collapsing `bool` into `int` is correct for a C translation unit and
# wrong for the C++ rows check() compares.
#
# ADVISORY. Nothing in .github/workflows runs this tool at all today, and the
# rows it reports here are pre-existing on main, so the plain C findings are
# printed and the process exit status is left to the mangled half. Wiring it is
# a separate decision that belongs with whoever lands the last fix.

# Same shape as DECL_ROW: match the symbol first and take the type as whatever
# precedes it, because this file glues the sigil to the name --
# `extern char*func_02023178(int);` is 384 of them.
C_DECL_ROW = re.compile(r"^extern\s+(.+?)\s*(func_\w+)\s*\(", re.M)

# `typedef unsigned int u32;` in include/types.h. Rejects anything with a
# parenthesis or brace on the left (function-pointer and struct typedefs), so
# only scalar aliases are harvested.
TYPEDEF_ROW = re.compile(r"^typedef\s+([^;(){}*]+?)\s+(\w+)\s*;", re.M)

# The C base types, collapsed to one spelling each. `long` folds into `int`
# because this target is ARM32 EABI, where both are 32 bits and a return in r0
# is the same return -- the same reason `long long` stays separate (r0:r1).
# `char` is NOT folded into either signed or unsigned char: its signedness is a
# compiler flag rather than a spelling, so a `char` against an `s8` is a real
# question and stays reported.
BASE_CANON = {
    "int": "int", "signed": "int", "signed int": "int",
    "long": "int", "long int": "int", "signed long": "int",
    "signed long int": "int",
    "unsigned": "unsigned int", "unsigned int": "unsigned int",
    "unsigned long": "unsigned int", "unsigned long int": "unsigned int",
    "short": "short", "short int": "short", "signed short": "short",
    "signed short int": "short",
    "unsigned short": "unsigned short", "unsigned short int": "unsigned short",
    "long long": "long long", "signed long long": "long long",
    "long long int": "long long",
    "unsigned long long": "unsigned long long",
    "unsigned long long int": "unsigned long long",
    "signed char": "signed char", "unsigned char": "unsigned char",
    "char": "char", "void": "void", "float": "float", "double": "double",
}

# `bool` is harvested from nothing on purpose. include/types.h spells
# `typedef int bool;` under `#ifndef __cplusplus`, so in a .c translation unit
# the two ARE one type -- but a func_* body can live in a .cpp file, where bool
# is the real C++ type and mwccarm 2004/b56 widens it on return. The row does
# not say which kind of translation unit its caller is in, so a bool/int pair
# stays reported rather than being silently collapsed under the C spelling.
# This costs nothing today: zero of the 1250 rows are spelled bool.
ALIAS_EXCLUDE = {"bool"}


def scalar_aliases(root):
    """Read include/types.h and return alias -> canonical base type.

    Harvested rather than hard-coded so the table cannot drift away from the
    header the tree actually compiles against: `u32` is only `unsigned int`
    because types.h says so, and a future edit there moves this with it.
    Resolved transitively, so `Fix12i -> s32 -> signed int -> int`.

    An alias whose base does not land on a BASE_CANON spelling is DROPPED, not
    guessed: `typedef volatile unsigned int vu32;` keeps its volatile, and
    `typedef struct Vector3 {...} Vector3;` never matches TYPEDEF_ROW at all.
    """
    path = root / "include" / "types.h"
    if not path.is_file():
        return {}
    raw = {}
    for m in TYPEDEF_ROW.finditer(
            strip_comments(path.read_text(encoding="utf-8", errors="replace"))):
        base, name = " ".join(m.group(1).split()), m.group(2)
        if name in ALIAS_EXCLUDE:
            continue
        raw[name] = base
    out = {}
    for name in raw:
        seen, t = set(), name
        while t in raw and t not in seen:
            seen.add(t)
            t = raw[t]
        if t in BASE_CANON:
            out[name] = BASE_CANON[t]
    return out


def canon_c_type(t, aliases):
    """Canonical spelling of a C return type, for comparison only.

    Collapses what is spelling and keeps what is type:

      * `int *` / `int*`            -> `int*`        (norm_type's rule)
      * `u32` / `unsigned int`      -> `unsigned int` (include/types.h)
      * `struct Node*` / `Node*`    -> `Node*`       (elaborated vs typedef name)
      * `const char*` / `char*`     -> `char*`       (a const on a returned value
                                                      changes no instruction)
      * `volatile` is KEPT: it is not a spelling difference.
      * `int` and `unsigned int` stay apart. Same width, but the sign is the
        whole content of a row like func_02022cbc (`int` over a `u32` body), and
        it is what the caller's shift or compare is chosen from.
    """
    t = " ".join(t.replace("\t", " ").split())
    t = re.sub(r"\s*\*\s*", "*", t)
    t = t.replace("extern ", "")
    stars = ""
    while t.endswith("*"):
        stars += "*"
        t = t[:-1].strip()
    t = re.sub(r"\bconst\b", " ", t)
    t = re.sub(r"\b(?:struct|union|enum)\b", " ", t)
    t = " ".join(t.split())
    t = aliases.get(t, t)
    return BASE_CANON.get(t, t) + stars


DEF_ROW = re.compile(r"^[ \t]*([A-Za-z_][\w \t*&:<>,\"]*?)\s*(?<![\w])"
                     r"(func_\w+)\s*\(", re.M)


def _opens_a_body(text, paren):
    """True when the parameter list opening at `paren` is followed by `{`.

    This is what tells a DEFINITION from a DECLARATION, and it has to be this
    rather than "the line does not start with extern": 134 of these bodies are
    spelled

        extern "C" void func_01ffb07c(dBgW_Kc *self, const Vector3 *v)
        {

    in a .cpp file, because a C++ translation unit gives the symbol C linkage to
    keep the unmangled name. Reading `extern` as "declaration only" skipped every
    one of them and the check called them bodies it could not find.

    The parentheses are balanced by hand because a parameter list nests:
    `void func_x(void (*cb)(int))` has three closers and only the last ends it.
    """
    i, depth = paren, 0
    while i < len(text):
        if text[i] == "(":
            depth += 1
        elif text[i] == ")":
            depth -= 1
            if depth == 0:
                break
        i += 1
    if i >= len(text):
        return False
    j = i + 1
    while j < len(text) and text[j].isspace():
        j += 1
    return j < len(text) and text[j] == "{"


def _return_type_words(head):
    """Strip the storage and linkage noise off a captured return type.

    The linkage specifier goes as one unit rather than token by token: a bare
    `C` is dropped only when it is the quoted half of `extern "C"`, never when
    it is a return type named C, which this tree has --
    `extern "C" int func_0200ca50(C *self)`.

    `asm` is KEPT. An `asm void` body is a hand-written primitive whose real
    return convention lives in its instructions and not in its C return type,
    and a reader has to be told that rather than shown a comparison.
    """
    head = re.sub(r'^\s*extern\s*"[^"]*"', " ", head)
    words = head.split()
    while words and words[0] in ("extern", "static", "inline", "__inline"):
        words = words[1:]
    if not words or words[0] in ("return", "else", "case", "do"):
        return None
    return " ".join(words)


def definitions(text):
    """Map func_* name -> return type for every DEFINITION in one source file.

    Comments are stripped first for the same reason class_virtuals strips them:
    a `/* ... func_0200c66c(...) ... */` line above a definition is prose, and
    reading it would report the comment's words as a type.

    Indented definitions are read -- two files in src/ indent their whole
    contents by four spaces -- which is safe because a call site still cannot
    match: the type group must consume at least one character before the name,
    and every spelling that could supply one (`if (`, `switch (`, `x = `,
    `return `) either contains a character the group cannot hold or is followed
    by `;` rather than `{`.
    """
    text = strip_comments(text)
    out = {}
    for m in DEF_ROW.finditer(text):
        name = m.group(2)
        if name in out or not _opens_a_body(text, m.end() - 1):
            continue
        t = _return_type_words(m.group(1))
        if t:
            out[name] = t
    return out


def body_return_type(text, name):
    """The return type of `name`'s definition in one source file, or None.

    Shared with the audit that found these rows, so the gate and the audit
    cannot drift into disagreeing about what a body says.
    """
    return definitions(text).get(name)


def classify_c(dcanon, bcanon):
    """Name the KIND of disagreement, or None when the two agree.

    The classes are ordered by what they cost, which is how a fixing lane works
    the list: `void-vs-value` is the one that can change a caller's bytes (the
    caller was told r0 is dead), the pointer classes are host-port correctness,
    and `width-or-sign` is the sign of a value both sides already return.
    """
    if dcanon == bcanon:
        return None
    if (dcanon == "void") != (bcanon == "void"):
        return "void-vs-value"
    dp, bp = dcanon.endswith("*"), bcanon.endswith("*")
    if dp and bp:
        return "pointer-shape"
    if dp != bp:
        return "pointer-vs-scalar"
    return "width-or-sign"


C_CLASSES = ("two-headers", "void-vs-value", "pointer-shape",
             "pointer-vs-scalar", "width-or-sign", "asm-body")


def definition_index(root):
    """func_* name -> (source path, return type), for all of src/ at once.

    Indexed by what each file DEFINES rather than by what it is named, because
    405 of these rows have no src/<name>.c: they were folded into a consolidated
    translation unit that kept its own filename, so

        src/func_ov002_020d4748.cpp   also defines func_ov002_020bf5e0

    and a name-based lookup calls 290 of those bodies missing. One pass over
    src/ costs about 1.6 seconds on this tree and is the difference between
    comparing 842 rows and comparing 1132.

    The first definition wins, deterministically (paths are walked sorted). A
    duplicate definition of the same func_* name in two files would not link, so
    there is nothing to choose between.
    """
    out = {}
    src = root / "src"
    if not src.is_dir():
        return out
    for path in sorted(src.rglob("*")):
        if path.suffix not in (".c", ".cpp") or not path.is_file():
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        if "func_" not in text:
            continue
        for name, rtype in definitions(text).items():
            out.setdefault(name, (path, rtype))
    return out


# `extern "C" void func_ov006_020c3e70(char *t);` in a class header. The
# linkage specifier is optional because both spellings appear.
OTHER_HEADER_ROW = re.compile(
    r"^\s*extern\s+(?:\"C\"\s+)?(.+?)\s*(func_\w+)\s*\(", re.M)


def other_header_decls(root):
    """name -> [(header path, line, declared type)] outside decl_common.h.

    The third place a return type for one of these symbols can live, and the
    one that bites hardest: decl_common.h and a class header are BOTH included
    by the same translation unit, so a disagreement between them is not a
    silent wrong type, it is `illegal overloading` and the file does not
    compile. Five pairs disagree on this tree.

    Found the hard way. Correcting four decl_common.h rows from `void` to
    `void*` broke six sources that include both headers, and prepush_linkcheck
    reported the breakage as NO-SYM/len-mismatch -- a WARNING -- because a
    compile that produces no object produces no length to compare. A compile
    error that reads as a warning is the reason this check exists at all.
    """
    out = {}
    for path in sorted((root / "include").rglob("*.h")):
        if path.name == "decl_common.h":
            continue
        text = strip_comments(path.read_text(encoding="utf-8", errors="replace"))
        for m in OTHER_HEADER_ROW.finditer(text):
            out.setdefault(m.group(2), []).append(
                (path.relative_to(root).as_posix(),
                 text[:m.start()].count("\n") + 1, m.group(1).strip()))
    return out


SYMBOL_ROW = re.compile(r"^(\S+) kind:function\(", re.M)


def function_symbols(root):
    """Every function name the config knows about, across all modules.

    Only used to split the rows this check could not compare into two very
    different things. A row with no definition in src/ but a real symbol record
    is a function nobody has recovered yet; a row with neither is DEAD -- a name
    that was renamed out of the tree with its declaration left behind. There are
    112 of the second kind and 5 of the first, and reporting one number for both
    makes the check look 117 rows short of its reach when it is 5.
    """
    out = set()
    for f in sorted(root.glob("config/arm9/**/symbols.txt")):
        out.update(SYMBOL_ROW.findall(
            f.read_text(encoding="utf-8", errors="replace")))
    return out


def check_plain_c(root=REPO):
    """Return (rows, accounting) for the plain C func_* half.

    Each row is (symbol, declared type, body type, class, source path).
    """
    decl = strip_comments((root / "include" / "decl_common.h")
                          .read_text(encoding="utf-8", errors="replace"))
    aliases = scalar_aliases(root)
    index = definition_index(root)
    symbols = function_symbols(root)
    elsewhere = other_header_decls(root)
    rows = []
    acct = {"rows": 0, "no_definition": 0, "dead_row": 0, "unrecovered": 0,
            "typedef_equivalent": 0, "asm_body": 0, "compared": 0,
            "symbol_table": len(symbols), "also_in_a_class_header": 0}
    for m in C_DECL_ROW.finditer(decl):
        dtype, sym = m.group(1).strip(), m.group(2)
        acct["rows"] += 1
        # Checked before the body, and whether or not there IS a body: two
        # headers that disagree do not compile when one file includes both.
        for hpath, hline, htype in elsewhere.get(sym, []):
            acct["also_in_a_class_header"] += 1
            if canon_c_type(htype, aliases) != canon_c_type(dtype, aliases):
                rows.append((sym, dtype, htype, "two-headers",
                             f"{hpath}:{hline}"))
        hit = index.get(sym)
        if hit is None:
            # Either the row is stale -- src/engine/message/func_0201fe08.c is
            # the body, under the recovered name LoadMessageBankForLanguage --
            # or the function has no recovered source at all. Counted, not
            # guessed at, and split by whether the config still knows the name.
            acct["no_definition"] += 1
            if symbols:
                acct["unrecovered" if sym in symbols else "dead_row"] += 1
            continue
        path, btype = hit
        rel = path.relative_to(root).as_posix()
        is_asm = "asm" in btype.split()
        acct["asm_body"] += is_asm
        acct["compared"] += 1
        dc = canon_c_type(dtype, aliases)
        bc = canon_c_type(" ".join(w for w in btype.split() if w != "asm"),
                          aliases)
        kind = classify_c(dc, bc)
        if kind is None:
            if norm_type(dtype) != norm_type(btype):
                acct["typedef_equivalent"] += 1
            continue
        # An `asm` body's C return type is a declaration of convenience -- the
        # real one is in its instructions -- so a row over one is reported in
        # its own class instead of being handed to a lane as a type to correct.
        rows.append((sym, dtype, btype, "asm-body" if is_asm else kind, rel))
    return rows, acct


def report_plain_c(rows, acct, summary):
    """Print the advisory section. Returns nothing: it decides nothing."""
    print("\nPLAIN C ADVISORY: decl_common.h func_* rows vs their src/ definition")
    for kind in C_CLASSES:
        hits = [r for r in rows if r[3] == kind]
        if not hits:
            continue
        print(f"\n  {kind} ({len(hits)})")
        for sym, dtype, btype, kind, rel in hits:
            other = "header" if kind == "two-headers" else "body"
            print(f"    {sym:<26s} decl_common '{dtype}'  {other} '{btype}'  {rel}")
    if summary:
        print(f"\n  func_* rows in decl_common.h    {acct['rows']}")
        print(f"    no definition anywhere in src/   {acct['no_definition']}")
        if acct["symbol_table"]:
            print(f"      the name is still a symbol     {acct['unrecovered']}"
                  "  (no recovered body yet)")
            print(f"      the name is gone from config   {acct['dead_row']}"
                  "  (DEAD ROW: renamed away, declaration left behind)")
        print(f"    also declared in a class header  {acct['also_in_a_class_header']}")
        print(f"    COMPARED                         {acct['compared']}")
        print(f"      of those, hand-written asm     {acct['asm_body']}")
        print(f"      typedef-equivalent spellings   {acct['typedef_equivalent']}"
              "  (u32 vs unsigned int and friends,")
        print( "                                        NOT counted as disagreements)")
    print(f"{len(rows)} plain C row(s) disagree with their definition "
          "(ADVISORY, exit status unaffected)")


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

    # The plain C half prints BEFORE the mangled count, so that count stays the
    # last line of the run: pr.txt and the lane reports quote it as such.
    if "--no-plain-c" not in argv:
        crows, cacct = check_plain_c(root)
        report_plain_c(crows, cacct, "--summary" in argv or bool(crows))

    print(f"{len(bad)} symbol(s) declared with two different return types")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
