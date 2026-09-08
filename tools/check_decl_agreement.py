"""Do this tree's `extern` declarations of a symbol agree with its definition?

    python tools/check_decl_agreement.py                 # gate: fail on NEW disagreements
    python tools/check_decl_agreement.py --check         # the same thing, spelled out
    python tools/check_decl_agreement.py --list          # print every disagreement, banked or not
    python tools/check_decl_agreement.py --inventory     # the headline plus the top offenders
    python tools/check_decl_agreement.py --symbol <name> # everything known about one symbol
    python tools/check_decl_agreement.py --update        # re-bank config/decl-agreement-baseline.json
    python tools/check_decl_agreement.py --changed <base>   # only what this branch can have broken

WHY THIS EXISTS
---------------
`mwccarm` does not compare one translation unit's `extern` against another's.
A symbol can be declared `int f(void*, int, int, void*, void*)` in one file,
`void* f(void*, int, int, void*, void*)` in a second, and defined
`void f(void*, unsigned, unsigned, ctor_t, dtor_t)` in a third. Per-TU compilation
and relocated byte checks can still pass: the compiler uses each local declaration,
the link resolves by name, and `check_references.py` checks that the name exists.
Those checks do not establish agreement between the reconstructed interfaces.

Measured on this tree the day the gate was written, `__cxa_vec_ctor` carried 52 extern
declarations: 47 returning `void`, 3 returning `int` and 2 returning `void *`, against a
definition that returns `void`. Both declarations and definitions are reconstructed
source, and either can be wrong. ROM instructions, call-site behavior and RTTI constrain
the reconstruction; project symbol spellings record additional hypotheses. None makes
every original C++ type uniquely recoverable. This gate checks cross-TU consistency,
not original-source identity. A mismatch can affect code generation or the calling
convention, including when these sources are reused in the host port; the effect depends
on the particular types and ABI. Byte proof and source review remain necessary.

WHAT IT COMPARES
----------------
Identity is the SYMBOL NAME as the linker sees it: the identifier a declaration spells, and
for a definition either the `// @symbol <name>` line above it (the tree's convention in a
merged translation unit) or the flat identifier the definition itself declares. That is why
a hand-spelled `_ZN9ModelAnim7AdvanceEv` extern and the file that defines it line up here
even though neither side is C++ the compiler would recognise as the same entity.

For every symbol with at least one `extern` declaration, this compares each declaration
against the reference spelling:

  return    the return type
  arity     the number of parameters
  param     each parameter's type, positionally
  kind      declared as data where the reference is a function, or the reverse
  linkage   a symbol `config/**/symbols.txt` records UNMANGLED, declared in a C++
            translation unit without `extern "C"`
  mangled   a `_Z`-spelled declaration whose parameter count matches neither the demangled
            argument count nor that count plus one (the tree's flat-`this` convention)

The reference is the DEFINITION when the tree contains one, and the PLURALITY declaration
when it does not (many of these symbols live in a dsd gap object and have no source). Every
finding says which basis it used: a definition-backed disagreement compares against a
concrete implementation spelling, while a plurality-backed one uses the most common
declaration. Neither basis proves which spelling correctly reconstructs the ROM.

AN OUT-OF-LINE MEMBER IS COMPARED FLAT. `int daBmb_c::Behavior(void)` is one parameter to
the linker, not none: the `this` pointer is the first. This tree declares such symbols flat
and writes that `this` out by hand, in roughly equal thirds as `void *`, `char *` and
`<Class> *`, so the implicit parameter is compared as a wildcard that any pointer satisfies
and nothing else does. Without that, every correct flat extern of a member would be billed
for an arity disagreement -- 1,451 of them, across the 1,216 member definitions the tree
holds. It still catches the case that matters, a `this` declared `int`.

TYPEDEF ALIASES ARE NOT DISAGREEMENTS. `include/types.h` is parsed for its scalar typedefs
and they are resolved transitively before comparison, so `u32` and `unsigned int` and
`unsigned` are one type, `Fix12i` and `s32` and `signed int` and `int` are one type, and a
file that spells a parameter `u16` is not billed for a file that spells it `unsigned short`.
Elaborated specifiers collapse too (`struct Vector3` is `Vector3`), a top-level `const` on a
by-value parameter is dropped because it is not part of the type, and an array parameter
decays to a pointer as C says it does. An array DATA declaration does not decay: `extern int
v[]` and `extern int v` are different objects and the difference is the point.

RATCHET, NOT A SWEEP
--------------------
`config/decl-agreement-baseline.json` banks the disagreements that exist today. They are
matching work with byte proof attached, not text edits: changing a declaration can change
instruction selection at every call site, so nothing here may be "fixed" without a rebuild.
The gate fails only on a disagreement that is NOT banked, which is exactly the shape a new
`extern` copied from the wrong sibling makes.

Baseline entries are keyed on (symbol, declaring file, "<kind>:<what differs>") and carry
no line number and no full declared spelling. A line churns on every edit above the
declaration; the full spelling would re-report a banked return-type disagreement the moment
an unrelated parameter in the same declaration changed. A genuinely new disagreement in an
already-banked file is still caught, because the thing that differs is part of the key.

IT CANNOT PASS BY DOING NOTHING
-------------------------------
This repo has shipped a gate that reported a pass after a reformat took its match count to
zero (`check_header_offsets`, see `.github/workflows/dead-references.yml`). So this tool
counts the files it opened, the declarations it parsed and the symbols it resolved, and
exits non-zero with SCAN TOO SMALL below a floor rather than announcing a clean tree. The
floors sit far below today's numbers; they trip when the parser breaks, not when source
changes.
"""
import argparse
import json
import os
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
BASELINE = REPO / "config" / "decl-agreement-baseline.json"

# Floors. A scan smaller than this means the parser broke, not that the tree got tidy.
MIN_FILES = 2000
MIN_DECLS = 5000
MIN_SYMBOLS = 500

SOURCE_SUFFIXES = (".c", ".cc", ".cpp")
HEADER_SUFFIXES = (".h", ".hpp")
SCAN_SUFFIXES = SOURCE_SUFFIXES + HEADER_SUFFIXES
CPP_SUFFIXES = (".cc", ".cpp", ".hpp")

IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
SYMBOL_MARK = re.compile(r"@symbol\s+([A-Za-z_][A-Za-z0-9_]*)")
SYMBOLS_ROW = re.compile(r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s+kind:(\w+)")

# Keywords that can never be a declarator's NAME. Anything else that trails a
# multi-token type is the identifier being declared.
TYPE_KEYWORDS = {
    "void", "char", "short", "int", "long", "float", "double", "signed",
    "unsigned", "bool", "wchar_t", "const", "volatile", "struct", "union",
    "enum", "class", "auto", "register", "static", "inline", "extern",
    "typename", "restrict", "__restrict", "_Bool",
}
# Specifiers that lead a declaration and are not part of the type. `asm` is here
# because mwccarm spells a hand-written primitive `asm void func_02057f54(void)`,
# and the `asm` says how the body was written, not what the function returns.
LEADING_SPECIFIERS = {"extern", "static", "inline", "__inline", "register",
                      "constexpr", "explicit", "virtual", "friend", "asm",
                      "_asm", "__asm"}
TAG_KEYWORDS = {"struct", "union", "enum", "class"}

# `int f()` in C declares a function with an UNSPECIFIED parameter list, not one
# with none. This tree carries 800 such declarations across 305 files; treating them
# as a claim of zero parameters would fabricate an arity disagreement against each.
UNSPECIFIED = None

SKIP_DIRS = {".git", "build", "extracted", "__pycache__", ".mypy_cache",
             ".pytest_cache", "node_modules", ".venv", "venv", "port", "src_tu",
             "config_tu"}

# Canonical spellings for the integer zoo. `char` is deliberately absent: plain
# `char` is a third type distinct from both `signed char` and `unsigned char`, and
# collapsing it would hide a real signedness disagreement.
INT_CANON = {
    "unsigned": "unsigned int",
    "signed": "int",
    "signed int": "int",
    "short int": "short",
    "signed short": "short",
    "signed short int": "short",
    "unsigned short int": "unsigned short",
    "long int": "long",
    "signed long": "long",
    "signed long int": "long",
    "unsigned long int": "unsigned long",
    "long long int": "long long",
    "signed long long": "long long",
    "signed long long int": "long long",
    "unsigned long long int": "unsigned long long",
    "signed char": "signed char",
}


# ------------------------------------------------------------------ scrubbing

def scrub(text):
    """(code, marks). Comments become spaces; string and char literals become spaces.

    Line structure is preserved exactly, so an index into `code` maps to the same line
    it does in `text`. `marks` is [(index, symbol)] for every `// @symbol <name>`
    comment, which is how a merged translation unit says which definition is which.

    A lexer rather than a regex because this tree writes URLs, `//` inside string
    literals and path-shaped constants constantly, and a comment regex eats them.
    """
    out = []
    marks = []
    i = 0
    size = len(text)
    while i < size:
        ch = text[i]
        if text.startswith("//", i):
            end = text.find("\n", i)
            if end < 0:
                end = size
            body = text[i:end]
            m = SYMBOL_MARK.search(body)
            if m:
                marks.append((i, m.group(1)))
            out.append(" " * (end - i))
            i = end
            continue
        if text.startswith("/*", i):
            end = text.find("*/", i + 2)
            end = size if end < 0 else end + 2
            body = text[i:end]
            m = SYMBOL_MARK.search(body)
            if m:
                marks.append((i, m.group(1)))
            out.append("".join(c if c == "\n" else " " for c in body))
            i = end
            continue
        if ch in ('"', "'"):
            quote = ch
            start = i
            i += 1
            while i < size:
                if text[i] == "\\":
                    i += 2
                    continue
                if text[i] == quote:
                    i += 1
                    break
                if text[i] == "\n":
                    break
                i += 1
            body = text[start:i]
            # `extern "C"` is the one string literal whose CONTENT is syntax, so it
            # survives; every other literal becomes blanks.
            if body in ('"C"', '"C++"'):
                out.append(body)
            else:
                out.append("".join(c if c == "\n" else " " for c in body))
            continue
        if ch == "#":
            # A preprocessor line is not a declaration and its continuations are not
            # either. Blank the whole logical line.
            start = i
            while i < size:
                end = text.find("\n", i)
                if end < 0:
                    end = size
                if text[end - 1:end] == "\\" or text[max(start, end - 2):end] == "\\\r":
                    i = end + 1
                    continue
                i = end
                break
            out.append("".join(c if c == "\n" else " " for c in text[start:i]))
            continue
        out.append(ch)
        i += 1
    return "".join(out), marks


def line_index(text):
    """Sorted list of newline positions, for turning an index into a 1-based line."""
    return [m.start() for m in re.finditer("\n", text)]


def line_of(newlines, idx):
    lo, hi = 0, len(newlines)
    while lo < hi:
        mid = (lo + hi) // 2
        if newlines[mid] < idx:
            lo = mid + 1
        else:
            hi = mid
    return lo + 1


# ---------------------------------------------------------------- type algebra

def scalar_typedefs(root=REPO):
    """The scalar aliases `include/types.h` declares, resolved transitively.

    Only single-line `typedef <tokens> <name>;` forms with no `*`, `(` or `[` are
    taken: those are the ones the tree treats as spellings of the same type. A
    struct typedef is deliberately not folded, because `Vector3` and its tag are
    already collapsed by the elaborated-specifier rule below.
    """
    path = root / "include" / "types.h"
    if not path.exists():
        return {}
    raw = {}
    text, _marks = scrub(path.read_text(encoding="utf-8", errors="replace"))
    for stmt in text.split(";"):
        stmt = " ".join(stmt.split())
        if not stmt.startswith("typedef "):
            continue
        body = stmt[len("typedef "):]
        if any(c in body for c in "*([{)]}"):
            continue
        toks = body.split()
        if len(toks) < 2:
            continue
        name, base = toks[-1], " ".join(toks[:-1])
        if not IDENT.fullmatch(name) or name in TYPE_KEYWORDS:
            continue
        raw[name] = base

    resolved = {}

    def resolve(name, seen):
        if name in resolved:
            return resolved[name]
        base = raw[name]
        toks = base.split()
        out = []
        for t in toks:
            if t in raw and t not in seen:
                out.append(resolve(t, seen | {t}))
            else:
                out.append(t)
        value = " ".join(out)
        resolved[name] = value
        return value

    for name in raw:
        resolve(name, {name})
    return resolved


def _canon_scalar(tokens):
    joined = " ".join(tokens)
    return INT_CANON.get(joined, joined).split()


def normalise_type(text, aliases, decay_arrays):
    """A canonical spelling of a type, or "" when there is nothing left to say.

    `decay_arrays` is True for a parameter (C says `int v[]` IS `int *`) and False for
    a data declaration (`extern int v[]` and `extern int v` are different objects).
    """
    text = text.strip()
    if not text:
        return ""
    # An alias for a function-pointer type expands to a spelling with parentheses
    # in it, which the token walk below would flatten into nonsense. Hand it back
    # whole instead.
    flat = " ".join(text.split())
    if flat in aliases and "(" in aliases[flat]:
        return aliases[flat]
    # Arrays. In a parameter they decay; in a declaration they are kept, but the
    # BOUND is dropped -- `extern int v[]` and `extern int v[8]` name the same object
    # and the tree writes both.
    arrays = 0
    while True:
        m = re.search(r"\[[^\[\]]*\]\s*$", text)
        if not m:
            break
        arrays += 1
        text = text[:m.start()].strip()
    toks = re.findall(r"[A-Za-z_][A-Za-z0-9_:]*|\*|&|<|>|,|::", text)
    # Elaborated specifiers name the same type as the bare tag.
    toks = [t for t in toks if t not in TAG_KEYWORDS]
    # A top-level const is not part of a by-value type. A const that guards a
    # pointee is: keep any `const` with a `*` or `&` after it.
    kept = []
    for i, t in enumerate(toks):
        if t == "const" and not any(x in ("*", "&") for x in toks[i + 1:]):
            continue
        kept.append(t)
    toks = kept
    # Alias resolution, transitive by construction (`aliases` is already resolved).
    expanded = []
    for t in toks:
        expanded.extend(aliases[t].split() if t in aliases else [t])
    ptr = [t for t in expanded if t in ("*", "&")]
    base = [t for t in expanded if t not in ("*", "&")]
    base = _canon_scalar(base)
    if arrays:
        if decay_arrays:
            ptr.append("*")
        else:
            ptr.append("[]" * arrays)
    out = " ".join(base)
    if ptr:
        out = (out + " " + " ".join(ptr)).strip()
    return " ".join(out.split())


def split_top(text, seps=(",",)):
    """Split on separators that are not inside (), [] or <>."""
    parts, depth, buf = [], 0, []
    for ch in text:
        if ch in "([<":
            depth += 1
        elif ch in ")]>":
            depth -= 1
        if ch in seps and depth <= 0:
            parts.append("".join(buf))
            buf = []
            continue
        buf.append(ch)
    parts.append("".join(buf))
    return parts


def parse_params(text, aliases, cxx):
    """(types, ok). `void` is no parameters; `...` is kept as a parameter of its own.

    An EMPTY list is `UNSPECIFIED` in C and no parameters in C++, which is the
    difference between `int f()` being a claim and being a shrug. `cxx` decides.
    """
    text = text.strip()
    if not text:
        return ((), True) if cxx else (UNSPECIFIED, True)
    if " ".join(text.split()) in ("void", "VOID"):
        return (), True
    types = []
    for raw in split_top(text):
        piece = raw.strip()
        if not piece:
            return (), False
        if piece == "...":
            types.append("...")
            continue
        # A default argument is not part of the type.
        piece = split_top(piece, seps=("=",))[0].strip()
        # A function-pointer parameter. The inner identifier is the PARAMETER NAME
        # and has to come out, or `void (*ctor)(void*)` and `void (*)(void*)` read
        # as two different types when they are one.
        if "(" in piece:
            fp = re.match(r"^(?P<pre>[^()]*)\(\s*(?P<stars>[*&]+)\s*"
                          r"([A-Za-z_][A-Za-z0-9_]*)?\s*(?P<arr>(\[[^\[\]]*\])*)\s*\)"
                          r"\s*\((?P<args>.*)\)\s*$", piece)
            if fp:
                inner, ok = parse_params(fp.group("args"), aliases, True)
                if ok:
                    types.append(" ".join(
                        ("%s (%s%s)(%s)" % (
                            normalise_type(fp.group("pre"), aliases, False),
                            fp.group("stars"), fp.group("arr"),
                            ", ".join(inner) if inner else "void")).split()))
                    continue
            types.append(" ".join(piece.split()))
            continue
        toks = piece.split()
        arrayed = bool(re.search(r"\[[^\[\]]*\]\s*$", piece))
        name_stripped = piece
        if len(toks) > 1:
            last = re.sub(r"\[[^\[\]]*\]\s*$", "", piece).strip()
            m = re.search(r"([A-Za-z_][A-Za-z0-9_]*)\s*$", last)
            if m:
                word = m.group(1)
                before = last[:m.start()].strip().split()
                if (word not in TYPE_KEYWORDS and before
                        and before[-1] not in TAG_KEYWORDS):
                    name_stripped = last[:m.start()]
                    if arrayed:
                        name_stripped += "[]"
        types.append(normalise_type(name_stripped, aliases, decay_arrays=True))
    return tuple(types), True


def parse_declarator(text, aliases, cxx=True):
    """(name, return_type, params, is_function) for one declarator, or None.

    `text` is a single declaration with its leading specifiers already stripped:
    `void *_ZN7fBase_cnwEj(unsigned int size)`, `SharedFilePtr data_ov102_0214e9c0`,
    `int _ZTV7daBmb_c[]`.
    """
    text = " ".join(text.split())
    if not text:
        return None
    # A single bare identifier is the tail of a `} Name;` aggregate, not a
    # declaration of anything.
    if IDENT.fullmatch(text):
        return None

    # `void (*handler)(int)` -- a pointer-to-function OBJECT, not a function. The
    # name sits inside the inner parentheses; the type is the whole spelling with
    # the name taken out, which is enough for two files to be compared.
    fp = re.match(r"^(?P<pre>.*?)\(\s*(?P<stars>[*&]+)\s*"
                  r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*(?P<arr>(\[[^\[\]]*\])*)\s*\)"
                  r"\s*\((?P<params>.*)\)\s*$", text)
    if fp and "(" not in fp.group("pre"):
        params, ok = parse_params(fp.group("params"), aliases, cxx)
        if not ok:
            return None
        shown = "void" if params == () else (
            "..." if params is UNSPECIFIED else ", ".join(params))
        typ = "%s (%s%s)(%s)" % (
            normalise_type(fp.group("pre"), aliases, decay_arrays=False),
            fp.group("stars"), fp.group("arr"), shown)
        return fp.group("name"), " ".join(typ.split()), None, False, False
    # Trailing cv-qualifiers, exception specifications and attributes. `__attribute__`
    # is stripped repeatedly because the tree writes `((long_call, target(...)))` and
    # the inner parentheses survive the string blanking as `target( )`.
    for _ in range(4):
        stripped = re.sub(r"\s*__attribute__\s*\(\(.*\)\)\s*$", "", text)
        stripped = re.sub(r"\)\s*(const|volatile|throw\s*\([^)]*\)|noexcept)\s*$",
                          ")", stripped).strip()
        if stripped == text:
            break
        text = stripped
    text = text.strip()

    if text.endswith(")"):
        depth = 0
        open_at = -1
        for i in range(len(text) - 1, -1, -1):
            ch = text[i]
            if ch == ")":
                depth += 1
            elif ch == "(":
                depth -= 1
                if depth == 0:
                    open_at = i
                    break
        if open_at <= 0:
            return None
        head = text[:open_at].strip()
        params_text = text[open_at + 1:-1]
        # `char *(func_ov007_020b78d0)(int)` -- a redundantly parenthesised name,
        # which is the same declaration as without the parentheses.
        paren_name = re.match(r"^(?P<pre>.*?)\(\s*(?P<name>[A-Za-z_][A-Za-z0-9_]*)"
                              r"\s*\)$", head)
        if paren_name and "(" not in paren_name.group("pre"):
            head = "%s %s" % (paren_name.group("pre").strip(),
                              paren_name.group("name"))
        # Anything else with parentheses left in the head declares a pointer to a
        # function or to a member. Out of scope, and reported as unparsed rather
        # than guessed at.
        if head.endswith(")") or "(" in head:
            return None
        m = re.search(r"([A-Za-z_][A-Za-z0-9_]*)\s*$", head)
        if not m:
            return None
        name = m.group(1)
        if name in TYPE_KEYWORDS:
            return None
        # `int daBmb_c::Behavior(void)`. The class qualifier belongs to the NAME, not
        # to the return type; leaving it in makes every such definition read as
        # returning `int daBmb_c::` and disagree with every flat declaration of it.
        lead = head[:m.start()].rstrip()
        member = lead.endswith("::")
        if member:
            lead = re.sub(r"[A-Za-z_][A-Za-z0-9_]*\s*::\s*$", "", lead)
        ret = normalise_type(lead, aliases, decay_arrays=False)
        if not ret:
            return None
        params, ok = parse_params(params_text, aliases, cxx)
        if not ok:
            return None
        return name, ret, params, True, member

    # Data. Strip an array bound before hunting for the identifier.
    stripped = re.sub(r"(\[[^\[\]]*\])+\s*$", "", text).strip()
    suffix = text[len(stripped):]
    m = re.search(r"([A-Za-z_][A-Za-z0-9_]*)\s*$", stripped)
    if not m:
        return None
    name = m.group(1)
    if name in TYPE_KEYWORDS:
        return None
    before = stripped[:m.start()].strip()
    if not before:
        return None
    typ = normalise_type(before + suffix, aliases, decay_arrays=False)
    if not typ:
        return None
    return name, typ, None, False, False


# ------------------------------------------------------------------- unit walk

def _brace_kind(head):
    """linkage | scope | aggregate | initializer | body.

    The `aggregate` verdict is what stops `typedef struct Vector3 { ... } Vector3;`
    from being read as a declaration of a symbol called `Vector3`: the block AND the
    declarator list after it belong to the type, not to any extern.
    """
    tail = " ".join(head.split())
    if re.search(r'extern\s+"C(\+\+)?"\s*$', tail):
        return "linkage"
    if (re.match(r"^(inline\s+)?namespace\b", tail)
            or re.search(r"\bnamespace\b[^;]*$", tail)):
        return "scope"
    if tail.endswith("="):
        return "initializer"
    if tail.endswith(","):
        return "aggregate"
    if re.search(r"\b(struct|union|enum|class)\b", tail) and not tail.endswith(")"):
        return "aggregate"
    return "body"


def top_level_units(code, default_linkage):
    """Yield (start, text, terminator, linkage, in_block, in_namespace) per statement.

    `in_block` says the statement sits inside an explicit `extern "C" { ... }`, where
    a declaration does not have to repeat the `extern` keyword to be one.

    Bodies and aggregate contents are skipped so fields are not mistaken for
    symbols. An initializer head is retained to identify its object definition.
    """
    i = 0
    n = len(code)
    start = 0
    paren = 0
    linkage = [default_linkage]
    kinds = []
    while i < n:
        block = "linkage" in kinds
        in_namespace = "scope" in kinds
        ch = code[i]
        if ch in "([":
            paren += 1
        elif ch in ")]":
            paren -= 1
        elif paren <= 0 and ch == ";":
            yield start, code[start:i], ";", linkage[-1], block, in_namespace
            start = i + 1
        elif paren <= 0 and ch == "{":
            head = code[start:i]
            kind = _brace_kind(head)
            if kind == "linkage":
                tail = " ".join(head.split())
                linkage.append("C" if tail.endswith('"C"') else "C++")
                kinds.append(kind)
                start = i + 1
            elif kind == "scope":
                linkage.append(linkage[-1])
                kinds.append(kind)
                start = i + 1
            else:
                if kind == "initializer":
                    yield start, head, "initializer", linkage[-1], block, in_namespace
                elif kind != "aggregate":
                    yield start, head, "{", linkage[-1], block, in_namespace
                depth = 0
                while i < n:
                    if code[i] == "{":
                        depth += 1
                    elif code[i] == "}":
                        depth -= 1
                        if depth == 0:
                            i += 1
                            break
                    i += 1
                if kind in ("aggregate", "initializer"):
                    # Consume the aggregate tail or the initializer terminator;
                    # neither belongs to the following statement.
                    tail_paren = 0
                    while i < n:
                        if code[i] in "([":
                            tail_paren += 1
                        elif code[i] in ")]":
                            tail_paren -= 1
                        elif code[i] == ";" and tail_paren <= 0:
                            i += 1
                            break
                        elif code[i] in "{}" and tail_paren <= 0:
                            break
                        i += 1
                start = i
                continue
        elif paren <= 0 and ch == "}":
            if kinds:
                kinds.pop()
                linkage.pop()
            start = i + 1
        i += 1


def _strip_specifiers(text):
    """(rest, saw_extern, saw_static, linkage_override)."""
    toks = text.split()
    saw_extern = saw_static = False
    override = None
    idx = 0
    while idx < len(toks):
        t = toks[idx]
        if t == "extern":
            saw_extern = True
            if idx + 1 < len(toks) and toks[idx + 1] in ('"C"', '"C++"'):
                override = "C" if toks[idx + 1] == '"C"' else "C++"
                idx += 1
            idx += 1
            continue
        if t in LEADING_SPECIFIERS:
            if t == "static":
                saw_static = True
            idx += 1
            continue
        break
    return " ".join(toks[idx:]), saw_extern, saw_static, override


# ------------------------------------------------------------------- collection

class Record(object):
    """One declaration or definition, reduced to what the linker would see.

    `is_member` marks an out-of-line member definition (`int daBmb_c::Behavior()`).
    The tree declares those flat, with the `this` pointer written out as an explicit
    first parameter, so a member's declared arity is one MORE than its definition's.
    Recording the fact instead of guessing is what keeps 3,000 correct flat externs
    out of the report.
    """

    __slots__ = ("symbol", "file", "line", "ret", "params", "is_function",
                 "linkage", "is_definition", "is_member")

    def __init__(self, symbol, file, line, ret, params, is_function, linkage,
                 is_definition, is_member=False):
        self.symbol = symbol
        self.file = file
        self.line = line
        self.ret = ret
        self.params = params
        self.is_function = is_function
        self.linkage = linkage
        self.is_definition = is_definition
        self.is_member = is_member

    def flat_params(self):
        """The parameter list a FLAT declaration of this symbol would carry.

        `None` when unspecified. A member gets its implicit `this` back, spelled as
        a wildcard: the tree writes it `void *`, `char *` and `<Class> *` in roughly
        equal thirds, and none of those three is more right than the others.
        """
        if self.params is UNSPECIFIED:
            return UNSPECIFIED
        if self.is_member:
            return ("<this>",) + tuple(self.params)
        return tuple(self.params)

    def spelling(self):
        if not self.is_function:
            return self.ret
        params = self.flat_params()
        if params is UNSPECIFIED:
            return "%s (unspecified)" % self.ret
        return "%s (%s)" % (self.ret, ", ".join(params) if params else "void")


def scan_targets(root=REPO):
    out = []
    for base, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        rel_base = pathlib.Path(base).relative_to(root).as_posix()
        if rel_base != "." and not rel_base.startswith(("src", "include")):
            continue
        for f in files:
            if not f.endswith(SCAN_SUFFIXES):
                continue
            rel = f if rel_base == "." else "%s/%s" % (rel_base, f)
            if rel.startswith(("src/", "include/")):
                out.append(rel)
    return sorted(out)


def local_typedefs(code, aliases):
    """A file's OWN scalar and function-pointer typedefs, resolved against `aliases`.

    Without this, `src/__cxa_vec_ctor.cpp` declaring its fourth parameter `ctor_t`
    and 52 other files declaring it `void *` produce a finding that reads
    "expected ctor_t", which tells a reader nothing about what actually differs.
    With it the same finding reads "expected void (*)(void *)", which is the fact.
    Local names never leave the file, so they cannot collide across the tree.
    """
    out = {}
    for stmt in split_top(code, seps=(";",)):
        flat = " ".join(stmt.split())
        if not flat.startswith("typedef "):
            continue
        body = flat[len("typedef "):]
        fp = re.match(r"^(?P<pre>[^()]*)\(\s*(?P<stars>[*&]+)\s*"
                      r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*\)\s*\((?P<args>.*)\)$",
                      body)
        if fp:
            inner, ok = parse_params(fp.group("args"), aliases, True)
            if ok and inner is not UNSPECIFIED:
                out[fp.group("name")] = " ".join(
                    ("%s (%s)(%s)" % (normalise_type(fp.group("pre"), aliases, False),
                                      fp.group("stars"),
                                      ", ".join(inner) if inner else "void")).split())
            continue
        if any(c in body for c in "()[]{}"):
            continue
        toks = body.split()
        if len(toks) < 2:
            continue
        name = toks[-1]
        if not IDENT.fullmatch(name) or name in TYPE_KEYWORDS:
            continue
        base = " ".join(toks[:-1])
        if base.rstrip("*& ") == "":
            continue
        out[name] = " ".join(
            (aliases.get(t, t) for t in base.split())).strip()
    return out


def parse_file(rel, text, aliases):
    """(declarations, definitions, unparsed_count) found in one file."""
    code, marks = scrub(text)
    newlines = line_index(text)
    default_linkage = "C" if rel.endswith(".c") else "C++"
    # `int f()` means zero parameters in C++ and nothing at all in C. A `.h` is
    # included from both here, so it gets C's reading -- the one that claims less.
    cxx = rel.endswith(CPP_SUFFIXES)
    if rel.endswith(HEADER_SUFFIXES) and not rel.endswith(".hpp"):
        # A .h in this tree is included from both, so its bare declarations are
        # judged by the includer; the linkage is genuinely unknown here rather
        # than C++, and the `linkage` check below only fires on C++ sources.
        default_linkage = "?"
    local = local_typedefs(code, aliases)
    if local:
        merged = dict(aliases)
        merged.update({k: v for k, v in local.items() if k not in aliases})
        aliases = merged
    decls, defs = [], []
    unparsed = 0
    for start, chunk, term, linkage, in_block, in_namespace in top_level_units(
            code, default_linkage):
        body = " ".join(chunk.split())
        if not body:
            continue
        if re.match(r"^(typedef|using|template|static_assert|_Static_assert)\b", body):
            continue
        rest, saw_extern, saw_static, override = _strip_specifiers(body)
        if override:
            linkage = override
        if not rest:
            continue
        if re.match(r"^(namespace|enum|class|struct|union)\b", rest) and "(" not in rest:
            continue
        # Where the declarator itself begins, not where the buffer does. The gap
        # between them is the blanked comment block, and the `@symbol` line lives
        # in it.
        decl_start = start + (len(chunk) - len(chunk.lstrip()))
        line = line_of(newlines, decl_start)

        if term == "{":
            # A definition. Its identity is the `@symbol` line above it when the file
            # carries one, and otherwise the flat identifier it declares. Only marks
            # between the PREVIOUS statement and this declarator count, so one
            # marked function in a file does not lend its name to the next.
            parsed = parse_declarator(rest, aliases, cxx)
            if parsed is None:
                continue
            name, ret, params, is_fn, member = parsed
            if not is_fn:
                continue
            marked = [s for idx, s in marks if start <= idx < decl_start]
            symbol = marked[-1] if marked else name
            if "::" in rest and not marked:
                # An out-of-line member with no `@symbol` line: the linker name is not
                # recoverable from the text, so claim nothing.
                continue
            defs.append(Record(symbol, rel, line, ret, params, True,
                               "C" if linkage == "C" else linkage, True, member))
            continue

        if saw_static:
            continue
        for piece in _declarator_pieces(rest):
            parts = split_top(piece, seps=("=",))
            has_init = len(parts) > 1
            declarator = parts[0].strip()
            parsed = parse_declarator(declarator, aliases, cxx)
            if parsed is None:
                if not IDENT.fullmatch(declarator):
                    unparsed += 1
                continue
            name, ret, params, is_fn, _member = parsed
            is_data_definition = not is_fn and (
                has_init or (not saw_extern and rel.endswith(SOURCE_SUFFIXES)))
            if is_data_definition:
                # C++ namespace-scope const objects have internal linkage unless
                # explicitly extern. Pointee const does not make the object const.
                names = list(re.finditer(r"\b" + re.escape(name) + r"\b", declarator))
                lead = declarator[:names[-1].start()] if names else declarator
                qualifiers = re.split(r"[*&]", lead)[-1]
                if cxx and not saw_extern and re.search(r"\bconst\b", qualifiers):
                    continue
                marked = [s for idx, s in marks if start <= idx < decl_start]
                if ("::" in declarator or in_namespace) and not marked:
                    continue  # A qualified object's linker name needs its marker.
                defs.append(Record(marked[-1] if marked else name, rel, line,
                                   ret, params, False, linkage, True))
            elif saw_extern or in_block or rel.endswith(HEADER_SUFFIXES):
                decls.append(Record(name, rel, line, ret, params, is_fn, linkage,
                                    False, False))
    return decls, defs, unparsed


def _declarator_pieces(rest):
    """`int a, b` is two declarators sharing a base type; `f(int, int)` is one."""
    parts = split_top(rest)
    if len(parts) == 1:
        return [rest]
    base = split_top(parts[0], seps=("=",))[0].strip()
    m = re.match(r"^(.*?)([A-Za-z_][A-Za-z0-9_]*\s*(\[[^\[\]]*\])*)$", base)
    if not m or not m.group(1).strip():
        return [rest]
    prefix = m.group(1).strip().rstrip("*&")
    out = [parts[0].strip()]
    for extra in parts[1:]:
        extra = extra.strip()
        if not extra or "(" in extra:
            return [rest]
        out.append("%s %s" % (prefix, extra))
    return out


def unmangled_symbols(root=REPO):
    """Names `config/**/symbols.txt` records WITHOUT Itanium mangling.

    These are the symbols the link resolves by their plain spelling, so a C++
    translation unit that declares one without `extern "C"` is making a claim the
    linker does not share.
    """
    names = set()
    config = root / "config"
    if not config.exists():
        return names
    for path in config.rglob("symbols.txt"):
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            m = SYMBOLS_ROW.match(line)
            if m and not m.group(1).startswith("_Z"):
                names.add(m.group(1))
    return names


def demangled_arity(symbol, root=REPO):
    """Argument count the Itanium name itself states, or None when it is not sure.

    None, not a guess, whenever `tools/demangle.py` emits its unresolved-type
    placeholder. That demangler does not model the `V` (volatile) qualifier, so
    `PVt` -- pointer to volatile unsigned short -- comes back as TWO arguments,
    `T *` and `unsigned short`. Six real parameters then read as seven and every
    correct declaration of `G2x::SetBGyAffine` is reported wrong. A gate that
    inherits another tool's defect is worse than one that says nothing, so the
    placeholder is treated as "unknown" and this check stands down.
    """
    if not symbol.startswith("_Z"):
        return None
    try:
        sys.path.insert(0, str(pathlib.Path(root) / "tools"))
        import demangle as _demangle
    except Exception:
        return None
    try:
        info = _demangle.demangle(symbol)
    except Exception:
        return None
    if not info:
        return None
    args = info.get("args") or []
    if any(re.fullmatch(r"T\s*[*&]*", a) for a in args):
        return None
    return info.get("nargs")


def collect(root=REPO, files=None):
    aliases = scalar_typedefs(root)
    targets = list(files) if files is not None else scan_targets(root)
    decls, defs = [], []
    unparsed = 0
    for rel in targets:
        path = root / rel
        if not path.exists():
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        d, f, u = parse_file(rel, text, aliases)
        decls.extend(d)
        defs.extend(f)
        unparsed += u
    return targets, decls, defs, unparsed


# ------------------------------------------------------------------ comparison

def _reference(symbol, decls, defs):
    """(record, basis). The definition when there is one, else the plurality vote."""
    own = [d for d in defs if d.symbol == symbol]
    if own:
        return own[0], "definition"
    tally = {}
    for d in decls:
        tally.setdefault(d.spelling(), []).append(d)
    best = max(tally.values(), key=lambda group: (len(group), -len(group[0].file)))
    if len(best) == len(decls):
        return best[0], "unanimous"
    return best[0], "plurality"


def disagreements(decls, defs, unmangled, root=REPO):
    """[(record dict)] for every declaration that contradicts its reference.

    `root` is where `tools/demangle.py` is looked up, so a test can point the whole
    comparison at a throwaway tree without reaching into this one.
    """
    by_symbol = {}
    for d in decls:
        by_symbol.setdefault(d.symbol, []).append(d)
    defs_by_symbol = {}
    for d in defs:
        defs_by_symbol.setdefault(d.symbol, []).append(d)

    out = []
    for symbol in sorted(by_symbol):
        group = by_symbol[symbol]
        ref, basis = _reference(symbol, group, defs_by_symbol.get(symbol, []))
        want_arity = demangled_arity(symbol, root)
        for d in group:
            if d is not ref and d.is_function != ref.is_function:
                out.append(_finding(symbol, "kind", d, ref, basis,
                                    "function" if d.is_function else "data",
                                    "function" if ref.is_function else "data"))
                continue
            if d.is_function and ref.is_function and d is not ref:
                if d.ret != ref.ret:
                    out.append(_finding(symbol, "return", d, ref, basis,
                                        d.ret, ref.ret))
                mine, theirs = d.flat_params(), ref.flat_params()
                # An unspecified list on either side claims nothing about arity, so
                # there is nothing to contradict.
                if mine is UNSPECIFIED or theirs is UNSPECIFIED:
                    pass
                elif len(mine) != len(theirs):
                    out.append(_finding(symbol, "arity", d, ref, basis,
                                        str(len(mine)), str(len(theirs))))
                else:
                    for i, (a, b) in enumerate(zip(mine, theirs)):
                        # `<this>` is the member's implicit first parameter. Any
                        # pointer spelling of it is right; a non-pointer is not.
                        if b == "<this>" or a == "<this>":
                            other = a if b == "<this>" else b
                            if other.endswith(("*", "&")):
                                continue
                            out.append(_finding(symbol, "param", d, ref, basis,
                                                "#%d %s" % (i + 1, a),
                                                "#%d a pointer (the implicit this)"
                                                % (i + 1,)))
                            continue
                        if a != b:
                            out.append(_finding(symbol, "param", d, ref, basis,
                                                "#%d %s" % (i + 1, a),
                                                "#%d %s" % (i + 1, b)))
            elif not d.is_function and not ref.is_function and d is not ref:
                if d.ret != ref.ret:
                    out.append(_finding(symbol, "return", d, ref, basis,
                                        d.ret, ref.ret))
            # FUNCTIONS ONLY. The Itanium ABI does not mangle a variable at global
            # scope, so `extern int BowserShutter_ClsnFile[];` in a .cpp emits the
            # plain name whether or not `extern "C"` is written, and billing those
            # 1,300-odd data declarations would drown the real finding.
            if (d.is_function and symbol in unmangled and d.linkage == "C++"
                    and d.file.endswith(CPP_SUFFIXES)):
                out.append(_finding(symbol, "linkage", d, ref, basis,
                                    "bare extern in a C++ TU", 'extern "C"'))
            if (want_arity is not None and d.is_function
                    and d.flat_params() is not UNSPECIFIED):
                if len(d.flat_params()) not in (want_arity, want_arity + 1):
                    out.append(_finding(symbol, "mangled", d, ref, basis,
                                        "%d parameter(s)" % len(d.flat_params()),
                                        "%d or %d (the name states %d)"
                                        % (want_arity, want_arity + 1, want_arity)))
    return out


def _finding(symbol, kind, decl, ref, basis, got, want):
    return {
        "symbol": symbol,
        "kind": kind,
        "file": decl.file,
        "line": decl.line,
        "decl": decl.spelling(),
        "got": got,
        "want": want,
        "basis": basis,
        "ref_file": ref.file,
        "ref_line": ref.line,
        "ref": ref.spelling(),
    }


def key_of(finding):
    """What the baseline remembers: (symbol, file, "<kind>:<what differs>").

    Deliberately NOT the whole declared spelling and NOT the line. A line number
    churns on every edit above the declaration, and keying on the full spelling
    would re-report a banked return-type disagreement the moment an unrelated
    parameter in the same declaration changed. `got` already names the one thing
    that differs, so this key allows removals, allows edits elsewhere in the same
    declaration, and still catches a genuinely new disagreement in a banked file.
    """
    return (finding["symbol"], finding["file"],
            "%s:%s" % (finding["kind"], finding["got"]))


BASELINE_COMMENT = (
    "Declaration/definition disagreements present when this gate was built. "
    "tools/check_decl_agreement.py fails on any disagreement NOT listed here. Keyed "
    "symbol -> declaring file -> \"<kind>:<what differs>\". These are NOT text edits: "
    "changing a declaration can change instruction selection at every call site, so "
    "each one is matching work that needs a rebuild and byte proof (tools/match.py) "
    "before it moves. Regenerate with --update only from a clean checkout, and read "
    "the diff: a new entry is usually an extern copied from the wrong sibling.")


# -------------------------------------------------------------------- baseline

def load_baseline(path=BASELINE):
    if not path.exists():
        return set()
    data = json.loads(path.read_text(encoding="utf-8"))
    known = set()
    for symbol, files in (data.get("known") or {}).items():
        for file, items in files.items():
            for item in items:
                known.add((symbol, file, item))
    return known


def write_baseline(findings, path=BASELINE):
    """Write the ratchet. JSON, but with the innermost list kept on one line.

    `json.dumps(..., indent=1)` puts every string of a 27,000-entry ratchet on its
    own line and produces a six-megabyte file that no reviewer can read a diff of.
    One line per (symbol, declaring file) is both smaller and the granularity a
    reviewer actually wants.
    """
    path.parent.mkdir(parents=True, exist_ok=True)
    grouped = {}
    total = 0
    for f in findings:
        symbol, file, item = key_of(f)
        bucket = grouped.setdefault(symbol, {}).setdefault(file, set())
        if item not in bucket:
            bucket.add(item)
            total += 1
    lines = [
        "{",
        ' "_comment": %s,' % json.dumps(BASELINE_COMMENT),
        ' "known": {',
    ]
    symbols = sorted(grouped)
    for si, symbol in enumerate(symbols):
        lines.append("  %s: {" % json.dumps(symbol))
        files = sorted(grouped[symbol])
        for fi, file in enumerate(files):
            items = json.dumps(sorted(grouped[symbol][file]),
                               separators=(", ", ": "))
            lines.append("   %s: %s%s" % (json.dumps(file), items,
                                          "" if fi == len(files) - 1 else ","))
        lines.append("  }%s" % ("" if si == len(symbols) - 1 else ","))
    lines.append(" }")
    lines.append("}")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    return total


# --------------------------------------------------------------------- changed

def _git_lines(args, repo=REPO):
    proc = subprocess.run(["git", *args], cwd=str(repo), capture_output=True,
                          text=True)
    if proc.returncode != 0:
        return None, "git %s failed: %s" % (" ".join(args), proc.stderr.strip())
    return [ln.strip() for ln in proc.stdout.splitlines() if ln.strip()], None


def changed_paths(base, repo=REPO, head="HEAD"):
    """Changed paths, including both sides of renames and deleted inputs.

    Three sources, unioned, for the reason `check_header_offsets.changed_paths` gives:
    a header edited and not yet committed is in none of the commits, and a
    pre-commit run that reports a pass over a file it never opened is worse than no
    gate at all.
    """
    commits, err = _git_lines(
        ["diff", "--name-only", "--no-renames", "--diff-filter=AMD",
         "%s...%s" % (base, head)], repo)
    if err:
        return None, err
    if head == "HEAD":
        dirty, err = _git_lines(
            ["diff", "--name-only", "--no-renames", "--diff-filter=AMD", "HEAD"], repo)
        if err:
            return None, err
        untracked, err = _git_lines(["ls-files", "--others", "--exclude-standard"],
                                    repo)
        if err:
            return None, err
    else:
        dirty = untracked = []
    return sorted(set(commits) | set(dirty) | set(untracked)), None


def needs_full_scan(paths, root=REPO):
    """Shared parser inputs and removed source can affect untouched declarations."""
    tools = {"tools/check_decl_agreement.py", "tools/demangle.py",
             "config/decl-agreement-baseline.json"}
    return any(p in tools or
               (p.startswith(("src/", "include/")) and p.endswith(HEADER_SUFFIXES)) or
               (p.startswith("config/") and p.endswith("/symbols.txt")) or
               (p.startswith(("src/", "include/")) and p.endswith(SCAN_SUFFIXES)
                and not (root / p).exists()) for p in paths)


def changed_scope(base, root=REPO, head="HEAD"):
    """(touched, defined_symbols, total_changed, error).

    `touched` includes removed paths as well as changed src/ and include/ sources.
    `defined_symbols` is every
    symbol those files DEFINE, and it is the half that matters: retyping or renaming
    a definition invalidates declarations in files the diff never touched, so a gate
    that looked only at the diff would call that clean. The caller widens the report
    to every file declaring one of these, using declarations it has already parsed.
    """
    total, err = changed_paths(base, root, head)
    if total is None:
        return None, None, None, err
    touched = sorted(p for p in total if p.endswith(SCAN_SUFFIXES)
                     and p.startswith(("src/", "include/")))
    if not touched:
        return [], set(), total, None
    aliases = scalar_typedefs(root)
    defined = set()
    for rel in touched:
        path = root / rel
        if not path.exists():
            continue
        _d, defs, _u = parse_file(
            rel, path.read_text(encoding="utf-8", errors="replace"), aliases)
        defined.update(d.symbol for d in defs)
    return touched, defined, total, None


# ------------------------------------------------------------------- reporting

def inventory(decls, defs, findings):
    symbols = {d.symbol for d in decls}
    counts = {}
    for f in findings:
        counts[f["symbol"]] = counts.get(f["symbol"], 0) + 1
    top = sorted(counts.items(), key=lambda kv: (-kv[1], kv[0]))
    kinds = {}
    for f in findings:
        kinds[f["kind"]] = kinds.get(f["kind"], 0) + 1
    return {
        "symbols": len(symbols),
        "declarations": len(decls),
        "definitions": len({d.symbol for d in defs}),
        "disagreeing_symbols": len(counts),
        "disagreements": len(findings),
        "by_kind": kinds,
        "top": top,
    }


def print_finding(f, indent="  "):
    print("%s%s: %s" % (indent, f["symbol"], f["kind"]))
    print("%s  %s:%d  declares  %s" % (indent, f["file"], f["line"], f["decl"]))
    print("%s  %s:%d  %s is  %s" % (indent, f["ref_file"], f["ref_line"],
                                    f["basis"], f["ref"]))
    print("%s  got %s, expected %s" % (indent, f["got"], f["want"]))


# ------------------------------------------------------------------------- cli

def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--check", action="store_true",
                    help="fail on disagreements not in the baseline (the default)")
    ap.add_argument("--list", action="store_true",
                    help="print every disagreement, baselined or not")
    ap.add_argument("--inventory", action="store_true",
                    help="print the headline counts and the top ten offenders")
    ap.add_argument("--symbol", metavar="NAME",
                    help="print every declaration and definition of one symbol")
    ap.add_argument("--update", action="store_true",
                    help="re-bank config/decl-agreement-baseline.json")
    ap.add_argument("--changed", metavar="BASE", nargs="?", const="origin/main",
                    help="scan only what this branch can have broken (default base "
                         "origin/main)")
    args = ap.parse_args(argv)

    touched = defined = scope = None
    if args.changed:
        if args.update:
            print("check_decl_agreement: --update re-banks the WHOLE tree; it cannot "
                  "be combined with --changed.", file=sys.stderr)
            return 2
        touched, defined, total, err = changed_scope(args.changed, REPO)
        if err:
            print("check_decl_agreement: %s" % err, file=sys.stderr)
            return 1
        if not total:
            print("check_decl_agreement: the diff against %s is EMPTY -- no file "
                  "changed anywhere in the tree." % args.changed,
                  file=sys.stderr)
            print("check_decl_agreement: that is a base ref that resolved to nothing, "
                  "not a change that touches no source. Check that %s exists and is "
                  "fetched (a shallow clone has no merge base)." % args.changed,
                  file=sys.stderr)
            return 1
        full_scan = needs_full_scan(total, REPO)
        if full_scan:
            print("check_decl_agreement: shared inputs or removed source changed; "
                  "checking the whole tree.")
            touched = None
        if not touched and not full_scan:
            print("check_decl_agreement: %d file(s) changed vs %s, 0 of them "
                  "a src/ or include/ source -- nothing for this gate to check."
                  % (len(total), args.changed))
            return 0

    files, decls, defs, unparsed = collect(REPO)

    if (len(files) < MIN_FILES or len(decls) < MIN_DECLS
            or len({d.symbol for d in decls}) < MIN_SYMBOLS):
        print("check_decl_agreement: SCAN TOO SMALL -- %d file(s), %d declaration(s), "
              "%d distinct symbol(s); expected at least %d, %d and %d."
              % (len(files), len(decls), len({d.symbol for d in decls}),
                 MIN_FILES, MIN_DECLS, MIN_SYMBOLS))
        print("  The parser is broken, or the tool is being run outside the repo.")
        print("  Refusing to report a pass on a scan this size.")
        return 2

    unmangled = unmangled_symbols(REPO)
    findings = disagreements(decls, defs, unmangled, REPO)

    if args.symbol:
        name = args.symbol
        for d in defs:
            if d.symbol == name:
                print("DEFINITION  %s:%d  %s  [%s linkage]"
                      % (d.file, d.line, d.spelling(), d.linkage))
        rows = {}
        for d in decls:
            if d.symbol == name:
                rows.setdefault((d.spelling(), d.linkage), []).append(d)
        for (spelling, linkage), group in sorted(rows.items(),
                                                 key=lambda kv: -len(kv[1])):
            print("%4d x  %s  [%s linkage]" % (len(group), spelling, linkage))
            for d in sorted(group, key=lambda r: (r.file, r.line))[:8]:
                print("          %s:%d" % (d.file, d.line))
            if len(group) > 8:
                print("          ... and %d more" % (len(group) - 8))
        mine = [f for f in findings if f["symbol"] == name]
        print("%d disagreement(s) for %s" % (len(mine), name))
        for f in mine:
            print_finding(f)
        return 0

    inv = inventory(decls, defs, findings)
    print("check_decl_agreement: %d file(s), %d extern declaration(s) of %d distinct "
          "symbol(s), %d of which the tree also defines; %d unparsed declarator(s)"
          % (len(files), inv["declarations"], inv["symbols"], inv["definitions"],
             unparsed))
    print("  %d disagreement(s) across %d symbol(s)   %s"
          % (inv["disagreements"], inv["disagreeing_symbols"],
             "  ".join("%s=%d" % kv for kv in sorted(inv["by_kind"].items()))))

    if args.inventory:
        print("\n  top offenders by disagreement count:")
        for symbol, count in inv["top"][:10]:
            print("    %5d  %s" % (count, symbol))

    if args.update:
        n = write_baseline(findings, BASELINE)
        print("  wrote %s (%d banked disagreement(s))"
              % (BASELINE.relative_to(REPO).as_posix(), n))
        return 0

    if touched is not None:
        scope = set(touched)
        if defined:
            scope.update(d.file for d in decls if d.symbol in defined)
        findings = [f for f in findings if f["file"] in scope]
        print("  --changed %s: %d source file(s) changed, defining %d symbol(s); "
              "%d file(s) in scope once every declaration of those is folded in; "
              "%d disagreement(s) among them"
              % (args.changed, len(touched), len(defined), len(scope),
                 len(findings)))

    if args.list:
        for f in sorted(findings, key=lambda x: (x["symbol"], x["file"], x["line"])):
            print_finding(f)

    known = load_baseline(BASELINE)
    new = [f for f in findings if key_of(f) not in known]
    if touched is None:
        healed = known - {key_of(f) for f in findings}
        if healed:
            print("  %d banked disagreement(s) are gone -- run --update to shrink the "
                  "baseline (not a failure)" % len(healed))

    if not new:
        print("  no new declaration disagreements")
        return 0

    print("\nFAIL: %d declaration(s) disagree with the selected reference:\n" % len(new))
    for f in sorted(new, key=lambda x: (x["symbol"], x["file"], x["line"])):
        print_finding(f)
    print("\nDeclarations and definitions are reconstructed interfaces; either can be wrong.")
    print("Compare the ROM, implementation and callers before reconciling them, then")
    print("REBUILD: signature changes can affect code generation or the calling convention.")
    print("Byte gates compile local declarations but do not compare cross-TU contracts.")
    print("Record deliberate byte-proved exceptions through the reviewed baseline process.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
