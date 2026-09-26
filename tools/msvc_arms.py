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
    #if A && defined(_MSC_VER) && B     (false under mwccarm whatever A and B are)

plus their `#elif` / `#else` / `#endif`. Any other condition that mentions `_MSC_VER`
(`#if _MSC_VER >= 1300`, one joined with `||`) is left as it is and every side of it is
read, which is how both tools read every conditional before this module existed;
`unresolved()` names those lines so a report can say so. Other macros are never
evaluated: `#if 0`, `SM64DS_PLATFORM_PC` and the rest read exactly as they did.

Dropped lines become empty lines, so line numbers stay what they are in the file.
Pure string work: stdlib only, no compiler, no ROM.
"""
import re

_DIRECTIVE = re.compile(r"^\s*#\s*(ifdef|ifndef|if|elif|else|endif)\b(.*)$")
_TRAILING_COMMENT = re.compile(r"\s*(//.*|/\*.*?\*/\s*)$")
_MSC = r"(?:defined\s*\(\s*_MSC_VER\s*\)|defined\s+_MSC_VER\b|_MSC_VER)"
_MSC_POS = re.compile(r"^\(?\s*" + _MSC + r"\s*\)?$")
_MSC_NEG = re.compile(r"^!\s*\(?\s*" + r"(?:defined\s*\(\s*_MSC_VER\s*\)|defined\s+_MSC_VER\b)"
                      + r"\s*\)?$")
_MSC_TERM = re.compile(r"^\(?\s*(?:defined\s*\(\s*_MSC_VER\s*\)|defined\s+_MSC_VER\b)\s*\)?$")

TRUE, FALSE, UNKNOWN = "true", "false", "unknown"


def _condition(kind, arg):
    """TRUE/FALSE when mwccarm's answer for this condition is certain, else UNKNOWN."""
    a = _TRAILING_COMMENT.sub("", arg).strip()
    if kind == "ifdef":
        return FALSE if a == "_MSC_VER" else UNKNOWN
    if kind == "ifndef":
        return TRUE if a == "_MSC_VER" else UNKNOWN
    if _MSC_POS.match(a):
        return FALSE
    if _MSC_NEG.match(a):
        return TRUE
    if "||" not in a and "&&" in a:
        # A conjunction is false as soon as one term is: `defined(_MSC_VER)` is.
        if any(_MSC_TERM.match(t.strip()) for t in a.split("&&")):
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
    for line in text.split("\n"):
        visible = all(g.keep for g in stack)
        m = _DIRECTIVE.match(line)
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
                yield re.sub(r"#\s*elif", "#if", line, count=1), outer, _mentions_msc(arg)
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
