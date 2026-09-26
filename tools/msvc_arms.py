#!/usr/bin/env python3
"""The text of a source file as mwccarm sees it: `_MSC_VER` arms dropped.

The PC port compiles some decomp translation units with MSVC, and a few of them carry
a host-only arm:

    #ifdef _MSC_VER
    extern "C" Model *_ZN5ModelD0Ev(Model *thiz) { ... }   // what MSVC builds
    #else
    Model::~Model() { ... }                                 // what the ROM is built from
    #endif

mwccarm never defines `_MSC_VER`, so the ROM is built from the `#else` side alone. The
scoreboards that read source as text (tools/tiers.py for CONVERTED, and
tools/langmode_audit.py for the language-mode counts) are questions about the decomp
source, so they read the file through `mwcc_side()`: the side mwccarm compiles, and
nothing the host build alone sees.

Only `_MSC_VER` is evaluated, and only in shapes whose answer does not depend on any
other macro:

    #ifdef _MSC_VER / #ifndef _MSC_VER
    #if _MSC_VER / #if defined(_MSC_VER) / #if defined _MSC_VER   (optionally parenthesised)
    #if !defined(_MSC_VER) / #if !defined _MSC_VER
    #if defined(A) && defined(_MSC_VER) && 1     (defined terms or numeric literals)

plus their `#elif` / `#else` / `#endif`. Any other condition that mentions `_MSC_VER`
(`#if _MSC_VER >= 1300`, one joined with `||`) is left as it is and every side of it is
read, which is how both tools read every conditional before this module existed;
`unresolved()` names those lines so a report can say so. Other macros are never
evaluated: `#if 0`, `SM64DS_PLATFORM_PC` and the rest read exactly as they did.

Dropped lines become empty lines, so line numbers stay what they are in the file.
Directives inside comments or literals are ignored. Files with continued lines or
multiline comments crossing a directive, or a local #define/#undef of _MSC_VER
are retained whole, conservatively. Bare macro terms in conjunctions stay unknown:
their expansions can introduce operators that change precedence.
Pure string work: stdlib only, no compiler, no ROM.
"""
import re

_DIRECTIVE = re.compile(r"^\s*#\s*(ifdef|ifndef|if|elif|else|endif)\b(.*)$")
_TRAILING_COMMENT = re.compile(r"\s*(//.*|/\*.*?\*/\s*)$")
_MSC = r"(?:defined\s*\(\s*_MSC_VER\s*\)|defined\s+_MSC_VER\b|_MSC_VER)"
_MSC_POS = re.compile(r"^" + _MSC + r"$")
_MSC_NEG = re.compile(r"^!\s*(?:defined\s*\(\s*_MSC_VER\s*\)|defined\s+_MSC_VER\b)$")
_TERM = re.compile(r"^!?\s*(?:defined\s*\(\s*[A-Za-z_]\w*\s*\)|"
                   r"defined\s+[A-Za-z_]\w*|_MSC_VER|[0-9]+)$")
_MSC_WRITE = re.compile(r"^\s*#\s*(?:define|undef)\s+_MSC_VER\b")
_NON_CODE = re.compile(r'/\*.*?(?:\*/|\Z)|//[^\n]*|"(?:\\.|[^"\\])*"|'
                       r"'(?:\\.|[^'\\])*'", re.S)


def _unparen(text):
    """Remove only balanced parentheses enclosing the entire expression."""
    text = text.strip()
    while text.startswith("(") and text.endswith(")"):
        depth = 0
        for i, char in enumerate(text):
            depth += (char == "(") - (char == ")")
            if depth == 0:
                break
        if i != len(text) - 1 or depth != 0:
            break
        text = text[1:-1].strip()
    return text


def _directive_view(text):
    """Mask comments/literals for recognition; retain the original output text.

    Continued lines and comments crossing a directive boundary need a full
    preprocessor. Keep those files whole rather than risk hiding compiled code or
    leaving an unmatched comment delimiter after blanking a directive.
    """
    crossing_lines = set()
    def mask(match):
        token = match.group()
        if "\n" in token:
            first = text.count("\n", 0, match.start())
            crossing_lines.update(range(first, first + token.count("\n") + 1))
        return "".join("\n" if c == "\n" else " " for c in token)
    lines = _NON_CODE.sub(mask, text).split("\n")
    unsupported = bool(re.search(r"\\\r?\n", text)) or any(
        _DIRECTIVE.match(lines[i]) for i in crossing_lines) or any(
        _MSC_WRITE.match(line) for line in lines)
    return lines, unsupported

TRUE, FALSE, UNKNOWN = "true", "false", "unknown"


def _condition(kind, arg):
    """TRUE/FALSE when mwccarm's answer for this condition is certain, else UNKNOWN."""
    a = _unparen(_TRAILING_COMMENT.sub("", arg))
    if kind == "ifdef":
        return FALSE if a == "_MSC_VER" else UNKNOWN
    if kind == "ifndef":
        return TRUE if a == "_MSC_VER" else UNKNOWN
    if _MSC_POS.match(a):
        return FALSE
    if _MSC_NEG.match(a):
        return TRUE
    if "&&" in a:
        # Only defined tests, numeric literals and the known _MSC_VER atom are
        # supported: other macros may expand operators. Splitting arbitrary C
        # expressions loses precedence: (defined(_MSC_VER) && X) == 0 and
        # defined(_MSC_VER) && X ? 0 : 1 are both true with _MSC_VER undefined.
        terms = [_unparen(term) for term in a.split("&&")]
        if all(_TERM.fullmatch(term) for term in terms) and any(
                _MSC_POS.fullmatch(term) for term in terms):
            return FALSE
    return UNKNOWN


def _mentions_msc(arg):
    return "_MSC_VER" in _TRAILING_COMMENT.sub("", arg)


class _Group:
    """One open #if group. `resolved` is False for a group mwccarm's answer is not known
    for; such a group is read in full, every side of it, exactly as before."""

    def __init__(self, resolved, keep=True, taken=False):
        self.resolved = resolved
        self.keep = keep        # the current side is compiled by mwccarm
        self.taken = taken      # an earlier side of this group was compiled


def _walk(text):
    """Yield (line, visible, unresolved_msc) for every line of `text`."""
    stack = []
    directive_lines, unsupported = _directive_view(text)
    for line, directive in zip(text.split("\n"), directive_lines):
        visible = all(g.keep for g in stack)
        m = _DIRECTIVE.match(directive)
        if unsupported:
            yield line, True, bool(m and _mentions_msc(m.group(2)))
            continue
        if not m:
            yield line, visible, False
            continue
        kind, arg = m.group(1), m.group(2)
        unresolved = False
        if kind in ("ifdef", "ifndef", "if"):
            c = _condition(kind, arg)
            if c == UNKNOWN:
                unresolved = _mentions_msc(arg)
                stack.append(_Group(resolved=False))
                yield line, visible, unresolved
            else:
                stack.append(_Group(resolved=True, keep=(c == TRUE), taken=(c == TRUE)))
                yield "", visible, False
            continue
        if not stack:           # unbalanced: leave the line alone
            yield line, visible, False
            continue
        g = stack[-1]
        if kind == "endif":
            stack.pop()
            yield ("" if g.resolved else line), all(x.keep for x in stack), False
            continue
        outer = all(x.keep for x in stack[:-1])
        if not g.resolved:
            # An `#elif defined(_MSC_VER)` under an unknown `#if` is not resolved either:
            # whether mwccarm reaches it depends on the other macro.
            yield line, outer, kind == "elif" and _mentions_msc(arg)
            continue
        if kind == "elif":
            c = _condition("if", arg)
            if g.taken:
                g.keep = False
            elif c == UNKNOWN:
                # Everything after here is an ordinary `#if <cond> ... #endif` to mwccarm.
                # Turn the group into an unresolved one and keep this directive, spelled
                # as the `#if` it now is, so every remaining side is read.
                g.resolved, g.keep = False, True
                start = directive.index("#")
                rewritten = line[:start] + re.sub(r"#\s*elif", "#if", line[start:], count=1)
                yield rewritten, outer, _mentions_msc(arg)
                continue
            else:
                g.keep = g.taken = (c == TRUE)
            yield "", outer, False
            continue
        # else
        g.keep = not g.taken
        g.taken = True
        yield "", outer, False


def mwcc_side(text):
    """`text` with every `_MSC_VER` side mwccarm does not compile blanked to empty lines.

    Idempotent, and a file with no `_MSC_VER` in it comes back unchanged.
    """
    if "_MSC_VER" not in text:
        return text
    return "\n".join(line if visible else "" for line, visible, _ in _walk(text))


def unresolved(text):
    """1-based line numbers of `_MSC_VER` conditions this module leaves to be read whole."""
    if "_MSC_VER" not in text:
        return []
    return [i for i, (_, _, u) in enumerate(_walk(text), 1) if u]
