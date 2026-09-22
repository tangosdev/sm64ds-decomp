#!/usr/bin/env python3
"""Census the calls in src/ that pass FEWER arguments than the callee really takes.

THE BUG CLASS. A function whose ROM body reaches its callee with its own incoming
registers untouched --

    func_02018e3c:
        push {r4, lr}
        bl   func_0205d644          ; r0 and r1 are still the caller's
        movs r4, r0

-- can be decompiled as `int func_02018e3c(void) { int r = func_0205d644(); ... }`
and be byte-exact, because on ARM the arguments ride through the registers and the
compiler emits no argument setup either way. The C is wrong all the same: it names
no parameters and forwards none, so every host that passes arguments on the stack
(the PC port's cdecl) loses them. Three of these were found by hand -- func_0205d23c
(a null archive lookup that crashed the port's seven-window session),
func_02018e3c/func_0205d644, and the SharedFilePtr::Construct chain -- and each was
byte-identical once the parameters were spelled and forwarded. Nothing in the byte
gate, the relocation gate or the link gate can see the difference, so it needs a
reader of its own.

WHAT THIS FINDS, AND WHAT IT DOES NOT. This is a CENSUS, not a verdict. A row here
says only "this call site passes fewer arguments than the callee's definition
declares". That is the static half of the shape; the other half -- that the caller
reaches the `bl` with those registers still holding its own arguments -- is only
settled by the disassembly at the call site, and the tool deliberately does not
guess at it. Read the ROM before changing anything.

WHY NOT sigaudit.py, WHICH ALSO COMPARES ARITIES. sigaudit audits near-miss DRAFTS
in nearmiss/db.jsonl against matched src/, to stop a wrong signature burying an
otherwise-correct candidate before it is landed. This runs over the LANDED tree in
the opposite direction, looking for the signature that already matched and is still
wrong, and it counts all callees rather than only `func_`-named ones -- the
SharedFilePtr::Construct chain is reached through a mangled name and sigaudit's
CALL_RE cannot see it. Its split_args/paren_span/nargs helpers are imported here
rather than copied.

    python tools/check_call_arity.py                  # the census
    python tools/check_call_arity.py --summary        # plus what was not decidable
    python tools/check_call_arity.py --json out.json
    python tools/check_call_arity.py --callee func_02017e48

`f()` IS NOT `f(void)`. In C an empty parameter list means UNSPECIFIED, not zero, so
a call reached through one cannot be scored against it. Those declarations are
counted and reported (--summary) rather than flagged: they are the blind spot, and
saying how large it is is part of the census.

GROUND TRUTH IS THE DEFINITION. The callee's own src/ file is what the ROM runs, so
its parameter list is what decides a row. Declarations elsewhere are collected too,
because a callee with no definition in src/ (an SDK entry point, a hatch) still has
evidence, but a row resting on one is marked DECL and is weaker: this tree lets each
file spell its own externs, so declarations of the same symbol routinely disagree, so
those rows are hidden unless --decl asks for them.

For a C++ method definition (Class::Method) the ABI arity is the declared count PLUS
ONE, for `this` -- unless the class declares the method `static`, in which case there
is no `this` and the count stands. Both include/**/*.h and the class bodies written
inside src/**.cpp are read for that marker; --summary reports how many pairs it found
and how many definitions took the +1.

Exit status is 0 for a clean census and 1 only with --fail-on-hits, which is what a
future CI wiring would use once the tree's existing rows are triaged; nothing calls
it that way yet.
"""
import argparse
import collections
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import asm_policy  # noqa: E402
import srcpath as SP  # noqa: E402
from sigaudit import paren_span, nargs  # noqa: E402
from check_decl_return_types import parse_nested  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent

# Any `name(` at all: the callee vocabulary is the set of symbols src/ defines, so the
# scan looks up every identifier it finds rather than building one regex per symbol.
IDENT_CALL = re.compile(r"\b([A-Za-z_]\w*)\s*\(")
# `<something> Class::Method(` -- a C++ method definition, whose ABI arity gains `this`.
METHOD_DEF = re.compile(r"\b(\w+)\s*::\s*(~?\w+)\s*\(")
# Statement keywords that are followed by a parenthesis and are not calls.
KEYWORDS = {"if", "while", "for", "switch", "return", "sizeof", "do", "else", "case",
            "goto", "catch", "typedef", "defined", "asm", "__asm", "static_cast",
            "reinterpret_cast", "const_cast", "dynamic_cast", "throw", "new", "delete"}
# A parameter list contains type words; an argument list normally does not. Used only to
# drop declarations that survive the brace-depth test (a local `extern` inside a body).
TYPE_KEYWORDS = ("char ", "int ", "void", "unsigned ", "signed ", "short ", "long ",
                 "struct ", "float ", "double ", "const ", "u8 ", "u16 ", "u32 ", "u64 ",
                 "s8 ", "s16 ", "s32 ", "s64 ", "bool ", "class ")

BLOCK_COMMENT = re.compile(r"/\*.*?\*/", re.S)
LINE_COMMENT = re.compile(r"//[^\n]*")
DQ_STRING = re.compile(r'"(?:\\.|[^"\\\n])*"')
SQ_STRING = re.compile(r"'(?:\\.|[^'\\\n])*'")
# A whole preprocessor line, continuations included. Blanked before anything else,
# because a function-like macro is a definition to this reader and a lie: `#define
# H(a, b)` in the minigame headers made every H(x) call read as a one-argument call
# to a two-parameter function, and ATI/H/F0/F4/inline_fn alone were 2,354 rows of
# noise on the first run. The cost is that a call written INSIDE a macro body is not
# counted; its expansion sites are invisible to a text scan in any case.
PREPROC = re.compile(r"^[ \t]*#(?:[^\n\\]|\\.)*", re.M)


def blank(text):
    """Blank comments and string/char literals, preserving every newline and the length.

    Length has to survive: brace depth is looked up by character offset, so a
    substitution that shortens the text moves every offset after it. Newlines have to
    survive so a reported line number is the real one.
    """
    def keep_lines(m):
        s = m.group(0)
        return "\n" * s.count("\n") + " " * (len(s) - s.count("\n"))

    for rx in (BLOCK_COMMENT, LINE_COMMENT, PREPROC, DQ_STRING, SQ_STRING):
        text = rx.sub(keep_lines, text)
    return text


STRUCT_KW = re.compile(r"\b(?:struct|class|union|enum)\b")
CLASS_OPEN = re.compile(r"^(?:struct|class)\s+(\w+)\b[^;{]*\{", re.M)
# `static void *LoadFile(SharedFilePtr &ptr);` inside a class body. The sigil is glued
# to the name in this tree, so the separator is whitespace OR nothing after `*`/`&`.
STATIC_METHOD = re.compile(r"^\s*static\s+([^;(){}]+?)(?:\s+|(?<=[*&])\s*)(~?\w+)\s*\(", re.M)


def statics_in_text(text):
    """(class, method) pairs one already-blanked text declares `static`."""
    out = set()
    for m in CLASS_OPEN.finditer(text):
        cls, i, depth = m.group(1), m.end(), 1
        while i < len(text) and depth:
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
            i += 1
        for s in STATIC_METHOD.finditer(text[m.end():i]):
            out.add((cls, s.group(2)))
    return out


def static_methods(root=None):
    """(class, method) pairs declared `static`, so `this` is NOT an argument.

    Without this the +1-for-`this` rule is wrong for every static member, and it is
    wrong loudly: Model::LoadFile, Animation::LoadFile, dActor_c::FindWithID and
    OAM::Render alone put 800 rows into the first census, every one of them a
    correctly-arity'd call that the tool had mis-scored.

    include/ is not the whole answer. A recovered class whose only member is the one
    method that file defines is declared in the .cpp itself -- `class Sound { public:
    static void StopLoadedMusic_Layer1(unsigned int arg); };` -- so src/ is read for
    class bodies too, by build(). This reads the headers; build() unions in the rest.
    """
    root = root or REPO
    out = set()
    inc = root / "include"
    if not inc.is_dir():
        return out
    for path in sorted(inc.rglob("*.h")):
        try:
            out |= statics_in_text(blank(path.read_text(encoding="utf-8", errors="replace")))
        except OSError:
            continue
    return out


def brace_kinds(text):
    """What kind of body each character offset sits in: 'func', 'type', 'data' or ''.

    Depth alone is not enough. A method declared inside a class body (`void
    LoadFile();`) is at depth 1 and reads as a zero-argument call to whatever free
    function shares the name; a declaration inside `extern "C" { ... }` is at depth 1
    and is not a call either. Both were in the first run's output. So each open brace
    is classified from the text between it and the previous statement boundary:

        void f(int a) {          -> the snippet has a ')': a function body
        struct P {               -> a struct/class/union/enum body
        static int t[] = {       -> an initialiser
        extern "C" {  /  else {  -> neither; inherits the enclosing kind

    A call counts only inside 'func'. A name(...) followed by '{' anywhere else is a
    definition, which is what makes the `extern "C"` blocks in the .cpp files readable.
    """
    kinds = [""] * (len(text) + 1)
    stack = []
    cur = ""
    boundary = 0
    for i, ch in enumerate(text):
        kinds[i] = cur
        if ch in ";}":
            boundary = i + 1
        if ch == "{":
            snippet = text[boundary:i]
            if ")" in snippet:
                kind = "func"
            elif STRUCT_KW.search(snippet):
                kind = "type"
            elif "=" in snippet:
                kind = "data"
            else:
                kind = cur
            stack.append(cur)
            cur = kind
            boundary = i + 1
        elif ch == "}":
            cur = stack.pop() if stack else ""
            boundary = i + 1
    kinds[len(text)] = cur
    return kinds


STORAGE = re.compile(r"\b(?:extern|static|typedef)\b")


def _looks_like_declaration(text, start, inner):
    """Is this `name(...)` inside a function body a declaration rather than a call?

    Two signals. A parameter list carries type words where an argument list normally
    does not -- and a K&R prototype has NO parameter list at all, which is why the
    type test is not enough on its own: `extern int func_ov002_020f26e0();` written
    INSIDE a body (this tree does that a lot) read as a zero-argument call and put
    three files into the census that were calling the function correctly one line
    later. So the statement in front of the name is checked for a storage class too.
    """
    if any(k in inner for k in TYPE_KEYWORDS):
        return True
    j = max(text.rfind(";", 0, start), text.rfind("{", 0, start), text.rfind("}", 0, start))
    return bool(STORAGE.search(text[j + 1:start]))


def _is_member_call(text, i):
    """True when the identifier at offset i is reached through `.`, `->`, `::` or `~`.

    `ptr.LoadFile()` is a method on a class, not the free function src/LoadFile.c
    defines, and scoring one against the other produced 40 rows of nonsense.
    """
    j = i - 1
    while j >= 0 and text[j] in " \t\n":
        j -= 1
    if j < 0:
        return False
    if text[j] in ".~":
        return True
    return j >= 1 and text[j - 1:j + 1] in ("->", "::")


def scan_file(text):
    """Return (definitions, calls) for one source's ALREADY-BLANKED text.

    definitions: (symbol, param_count_or_None, kind, start, after) outside a function,
                 kind 'def' for a body and 'decl' for a prototype.
    calls:       (callee, argcount, lineno) inside a function body.
    """
    kinds = brace_kinds(text)

    defs, calls = [], []
    for m in IDENT_CALL.finditer(text):
        name = m.group(1)
        if name in KEYWORDS:
            continue
        inner, after = paren_span(text, m.end() - 1)
        if inner is None:
            continue
        if kinds[m.start()] != "func":
            if kinds[m.start()] == "data":
                continue                       # inside an initialiser, not a signature
            tail = text[after:after + 200].lstrip()
            kind = "def" if tail.startswith("{") else "decl"
            count = None if inner.strip() == "" else nargs(inner)
            defs.append((name, count, kind, m.start(), after))
        else:
            if _looks_like_declaration(text, m.start(), inner) or _is_member_call(text, m.start()):
                continue          # a local `extern`, or a call on an object
            calls.append((name, nargs(inner), text.count("\n", 0, m.start()) + 1))
    return defs, calls


def build(paths, statics=None):
    """Walk src/ once. Returns the definition table, the declaration table and the calls."""
    statics = static_methods() if statics is None else set(statics)
    seed = len(statics)
    definitions = {}                                # symbol -> (count_or_None, path)
    declarations = collections.defaultdict(set)     # symbol -> {count_or_None}
    calls = []                                      # (caller, callee, argc, path, line, draft)
    methods_counted = 0
    # Two passes over one read. A `static` marker can sit in the very file being scored
    # (see static_methods), so every class body has to be collected before any
    # definition is scored -- and reading src/ twice is the one thing worth avoiding.
    cache = []
    for path in paths:
        try:
            text = path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        blanked = blank(text)
        cache.append((path, blanked, asm_policy.has_draft_banner(text)))
        statics |= statics_in_text(blanked)
    for path, blanked, is_draft in cache:
        stem = path.stem
        defs, file_calls = scan_file(blanked)
        for name, count, kind, start, after in defs:
            if kind == "decl":
                declarations[name].add(count)
                continue
            if name == stem and not is_draft:
                definitions[stem] = (count, path)
            elif stem.startswith("_Z") and not is_draft:
                # `SharedFilePtr &SharedFilePtr::Construct(unsigned int fileID) {` --
                # the file's mangled stem is the symbol, and the ABI takes `this` first.
                mm = METHOD_DEF.search(blanked[max(0, start - 120):after])
                if mm and mm.group(2) == name:
                    nested = parse_nested(stem)
                    this_arg = 0 if (nested and nested in statics) else 1
                    definitions[stem] = ((count or 0) + this_arg, path)
                    methods_counted += this_arg
        for callee, argc, lineno in file_calls:
            calls.append((stem, callee, argc, path, lineno, is_draft))
    return definitions, declarations, calls, methods_counted, (seed, len(statics))


def _rel(path):
    """Repo-relative where that means something, absolute otherwise (a test tree)."""
    try:
        return str(path.relative_to(REPO)).replace("\\", "/")
    except ValueError:
        return str(path).replace("\\", "/")


def census(definitions, declarations, calls, callee_filter=None):
    """Rows where a call passes fewer arguments than the callee's evidence declares."""
    rows = []
    for caller, callee, argc, path, lineno, is_draft in calls:
        if callee_filter and callee != callee_filter:
            continue
        if caller == callee:
            continue                       # a recursive call is scored by its own header
        if callee in definitions:
            want, where = definitions[callee]
            if want is None:
                continue
            source, evidence = "DEF", where.name
        else:
            counts = {c for c in declarations.get(callee, ()) if c is not None}
            if not counts:
                continue
            want, source = max(counts), "DECL"
            evidence = "%d declaration(s)" % len(counts)
        if argc >= want:
            continue
        rows.append({"caller": caller, "callee": callee, "passed": argc,
                     "declared": want, "source": source, "evidence": evidence,
                     "file": _rel(path),
                     "line": lineno, "draft": is_draft})
    rows.sort(key=lambda r: (r["callee"], r["caller"], r["line"]))
    return rows


def unknown_arity(definitions, declarations):
    """Symbols whose only evidence is a K&R empty list -- the blind spot, sized."""
    out = set()
    for name, counts in declarations.items():
        if name in definitions:
            continue
        if counts == {None}:
            out.add(name)
    for name, (count, _path) in definitions.items():
        if count is None:
            out.add(name)
    return out


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--json", help="write the census as JSON")
    ap.add_argument("--summary", action="store_true", help="also report what was not decidable")
    ap.add_argument("--callee", help="restrict the census to one callee")
    ap.add_argument("--include-drafts", action="store_true",
                    help="keep rows in NONMATCHING files (they do not reproduce the ROM)")
    ap.add_argument("--decl", action="store_true",
                    help="also show rows whose only evidence is another file's extern")
    ap.add_argument("--fail-on-hits", action="store_true", help="exit 1 if the census is non-empty")
    args = ap.parse_args(argv)

    paths = sorted(SP.iter_sources())
    statics = static_methods()
    definitions, declarations, calls, methods, static_counts = build(paths, statics)
    everything = census(definitions, declarations, calls, args.callee)
    rows = everything if args.include_drafts else [r for r in everything if not r["draft"]]
    if not args.decl:
        rows = [r for r in rows if r["source"] == "DEF"]

    print("=== calls passing fewer arguments than the callee declares (%d) ===" % len(rows))
    print("%-34s %-34s %6s %8s  %s" % ("caller", "callee", "passed", "declared", "evidence"))
    for r in rows:
        print("%-34s %-34s %6d %8d  %s %s" % (r["caller"], r["callee"], r["passed"],
                                              r["declared"], r["source"], r["evidence"]))
    if args.summary:
        unknown = unknown_arity(definitions, declarations)
        print()
        print("sources scanned            %d" % len(paths))
        print("definitions with a count   %d"
              % sum(1 for c, _ in definitions.values() if c is not None))
        print("  of them C++ methods      %d  (declared params + 1 for `this`)" % methods)
        print("  static methods, no this  %d pairs (%d from include/, %d more from src/)"
              % (static_counts[1], static_counts[0], static_counts[1] - static_counts[0]))
        print("call sites read            %d" % len(calls))
        print("K&R `()` unknown arity     %d symbols  (not scorable, not flagged)" % len(unknown))
        print("distinct callees flagged   %d" % len({r["callee"] for r in rows}))
        print("rows resting on a DECL     %d  (weaker evidence, shown with --decl)"
              % sum(1 for r in everything if r["source"] == "DECL"))
        print("rows in NONMATCHING files  %d  (excluded unless --include-drafts)"
              % sum(1 for r in everything if r["draft"]))
    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(rows, indent=1), encoding="utf-8")
    return 1 if (args.fail_on_hits and rows) else 0


if __name__ == "__main__":
    sys.exit(main())
