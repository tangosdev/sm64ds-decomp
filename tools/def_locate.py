#!/usr/bin/env python3
"""Locate a function definition inside a multi-member source WITHOUT the
``// @symbol`` marker convention.

Two jobs:

- ``definitions(text)`` -- enumerate the top-level function definitions of a
  C/C++ source with their byte extents and parsed (qualified name, params).
- ``bind(path, text=None)`` -- resolve every owned linker symbol (function
  rows per ``srcpath.symbols_for``, plus data/bss rows resolved by address
  through ``srcpath.owner_at``'s delinks ranges) to the definition that emits
  it, by demangling the owned symbols and matching parsed signatures.
  Constructor/destructor variants (C1/C2, D0/D1/D2) fold onto their one
  shared definition; file-scope objects claim flat and static-member data
  symbols, and ``__sinit_<file>`` anchors the first static-initializer data
  block the way the marker convention did.

``member_fragment(text, symbol, bound)`` is one member, cut so it still
compiles to the symbol it was bound to. The cut runs from that definition
to the next one, and a ``namespace`` or ``extern "C"`` block that wraps
only this member is part of the cut. A block shared with a neighbour is
repeated around the member, so the slice does not keep the opening line
and drop the closing brace.

``python tools/def_locate.py <source>`` prints the file outline the
``// @symbol`` lines used to be: source order, the definition, the linker
symbol. That is the map. The comments are not.

The scanner is intentionally not a full C++ parser; it is a statement-level
walk over comment/string/preprocessor-blanked text, which is the same
strength the tree's other source tools run at. Definitions it cannot name
unambiguously are simply never bound -- the caller reports them, exactly the
posture ``_marked_member_fragment`` took for missing/duplicate markers.
"""
from __future__ import annotations

import bisect
import functools
import os
import re

import demangle as _dm


# --------------------------------------------------------------------- text

def _blank(text):
    """``text`` with comments, string/char literals and preprocessor lines
    replaced by spaces. Newlines are preserved so every offset still indexes
    the original string."""
    out = list(text)
    i, n = 0, len(out)

    def wipe(a, b):
        for k in range(a, b):
            if out[k] not in "\r\n":
                out[k] = " "

    bol = True
    while i < n:
        c = text[i]
        if bol and c in " \t\r":
            i += 1
            continue
        if bol and c == "#":
            j = i
            while j < n and out[j] != "\n":
                if out[j] == "\\" and j + 1 < n and out[j + 1] == "\n":
                    j += 1
                j += 1
            wipe(i, j)
            i = j
            bol = False
            continue
        bol = c == "\n"
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            if j == -1:
                j = n
            wipe(i, j)
            i = j
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            j = n if j == -1 else j + 2
            wipe(i, j)
            i = j
            continue
        if c in "\"'":
            # Keep the quote characters themselves: `extern "C" {` is
            # recognised by them.
            j = i + 1
            while j < n and text[j] != c:
                if text[j] == "\\":
                    j += 1
                if text[j] == "\n":          # unterminated literal: stop
                    break
                j += 1
            wipe(i + 1, j)
            i = j + 1
            continue
        i += 1
    return "".join(out)


def _line_offsets(text):
    offs = [0]
    for m in re.finditer("\n", text):
        offs.append(m.end())
    return offs


# --------------------------------------------------------------- definition

_IDENT = r"[A-Za-z_~][A-Za-z0-9_~]*"
_CONTAINER_RE = re.compile(
    rf'^\s*(?:extern\s+"[^"]*"|namespace\s+{_IDENT}|namespace)\s*$')
_EXTERN_C_KIND_RE = re.compile(r'extern\s+"\s*C\s*"')


def _split_head(head):
    """Return ``(declarator_text, is_function)`` for a statement head that ends
    at a top-level ``{``, or ``None`` when the head opens a non-function block
    (``struct X {``, ``= {``, ``else {`` ...)."""
    h = head.rstrip()
    if not h:
        return None
    # `... = {`, `[N] = {`, `{` after `)` of a call are not function bodies.
    if h[-1] == "=" or re.search(r'[=,]\s*$', h):
        return None
    # Strip ctor-initializer: `: member(x), member2(y)` -- a `:` at paren
    # depth 0 that is not `::`.
    depth = 0
    colon = -1
    for i, c in enumerate(h):
        if c in "(<[":
            depth += 1
        elif c in ")>]":
            depth = max(0, depth - 1)
        elif c == ":" and depth == 0:
            if i + 1 < len(h) and h[i + 1] == ":":
                continue
            if i > 0 and h[i - 1] == ":":
                continue
            colon = i
            break
    sig = h[:colon] if colon >= 0 else h
    sig = sig.rstrip()
    # Drop trailing cv-qualifiers / specifiers between `)` and the body,
    # including `throw()`/`throw(A, B)` exception specs (several may stack).
    _TRAIL = (r"(?:\bconst\b|\bvolatile\b|\boverride\b|\bfinal\b|\btry\b"
              r"|\bnoexcept\b|\bASM\b|\basm\b|\bthrow\s*\([^()]*\))\s*$")
    while True:
        new = re.sub(_TRAIL, "", sig).rstrip()
        if new == sig:
            break
        sig = new
    if not sig.endswith(")"):
        return None
    return sig


def _name_and_params(sig):
    """``(qualified, params_inside_parens)`` of a function declarator head, or
    ``(None, None)`` when no declarator name is found."""
    # The parameter group is the LAST depth-0 paren group of `sig` (ctor-init
    # was already cut; a function returning a function pointer has its own
    # parens earlier, which do not end at the close of `sig`).
    end = len(sig) - 1
    depth = 0
    start = -1
    for i in range(end, -1, -1):
        c = sig[i]
        if c == ")":
            depth += 1
        elif c == "(":
            depth -= 1
            if depth == 0:
                start = i
                break
    if start < 0:
        return None, None
    params = sig[start + 1:end]
    head = sig[:start].rstrip()
    # Trailing qualifiers before the params are impossible; the name is the
    # last identifier, allowing ~dtor / operator op / template args.
    m = re.search(r"((?:operator\s*(?:\(\)|\[\]|new|delete|[+\-*/%^&|~!=<>\[\],"
                  r"]+))|[A-Za-z_~][A-Za-z0-9_~]*)\s*(?:<[^<>]*>)?\s*$", head)
    if not m:
        return None, None
    name = m.group(1)
    rest = head[:m.start()].rstrip()
    # Walk back over `Owner::` qualifiers.
    owners = []
    while rest.endswith("::") or re.search(r'::\s*$', rest):
        rest2 = re.sub(r'::\s*$', '', rest).rstrip()
        q = re.search(r"([A-Za-z_][A-Za-z0-9_]*)\s*(?:<[^<>]*>)?\s*$", rest2)
        if not q:
            break
        owners.insert(0, q.group(1))
        rest = rest2[:q.start()].rstrip()
    qualified = "::".join(owners + [name]) if owners else name
    return qualified, params


def _norm_type(t, c_file, tmap=None):
    """Best-effort canonical spelling of one parameter type so a demangled
    argument and a source spelling compare equal."""
    t = re.sub(r"\s+", " ", t.strip())
    if not t or t == "...":
        return t
    # `Type name` / `Type *name` / `Type &name` -> drop a trailing identifier.
    t = re.sub(r"(?<=[\w>\]\)]) ([A-Za-z_]\w*)$", r" \1", t)
    t = re.sub(r"(?<=&)\s*[A-Za-z_]\w*$", "", t)
    t = re.sub(r"(?<=\*)\s*[A-Za-z_]\w*$", "", t)
    m = re.match(r"^(.*\S)\s+([A-Za-z_]\w*)(\s*\[[^\]]*\])?$", t)
    if m and m.group(2) not in ("const", "volatile", "long", "short",
                                "signed", "unsigned", "int", "char", "float",
                                "double", "bool", "struct", "class", "union",
                                "enum", "void"):
        t = m.group(1) + ("[]" if m.group(3) else "")
    t = re.sub(r"\s*\[\s*[^\]]*\]\s*$", "*", t)          # array param decays
    # `void (*fp)(int)` / `int (C::*op)(u32) const`: drop the declarator
    # name and normalize the arg list like any parameter list
    t = re.sub(r"\(\s*\*\s*" + _IDENT + r"\s*\)", "(*)", t)
    t = re.sub(r"\(\s*(\w[\w:]*)\s*::\s*\*\s*" + _IDENT + r"\s*\)",
               r"(\1::*)", t)
    m2 = re.search(r"\(([^()]*)\)\s*(const)?\s*$", t)
    if m2 and ("(*)" in t or "::*" in t):
        inner = ", ".join(_norm_params(m2.group(1), c_file, tmap))
        t = (t[:m2.start()] + "(" + inner + ")"
             + (" const" if m2.group(2) else ""))
    # `X const &` / `X volatile *` (demangled spelling) -> `const X &` /
    # `volatile X *` (source spelling); keep multiword bases together
    # (`unsigned short volatile *` -> `volatile unsigned short *`)
    for _ in range(2):
        t = re.sub(r"\b([A-Za-z_]\w*(?:\s+(?:unsigned|signed|long|short|int|char|float|double))*)\s+(const|volatile)(?=\s*[&*])",
                   r"\2 \1", t)
    # by-value top-level cv does not mangle
    if not re.search(r"[*&\[]", t):
        t = re.sub(r"\bconst\b\s*", "", t)
        t = re.sub(r"\bvolatile\b\s*", "", t)
    t = re.sub(r"\s*([*&])", r" \1", t)
    t = re.sub(r"\s+", " ", t).strip()
    typedef = {"u8": "unsigned char", "u16": "unsigned short",
               "u32": "unsigned int", "u64": "unsigned long long",
               "s8": "signed char", "s16": "short", "s32": "int",
               "s64": "long long", "f32": "float", "f64": "double",
               "size_t": "unsigned long", "Fix12i": "int",
               "vu8": "unsigned char", "vu16": "unsigned short",
               "vu32": "unsigned int", "vs8": "signed char",
               "vs16": "short", "vs32": "int"}
    # map the BASE type, preserving */&/const suffixes (`s8 &` -> `signed char &`)
    keywords = ("bool", "int", "char", "short", "long", "float", "double",
                "void", "signed", "unsigned", "const", "volatile")
    seen = set()
    for _ in range(3):                       # alias chains: A -> B -> int
        # the core type word is the last identifier before any */& tail:
        # `const s32 *` -> s32, `volatile u16 *` -> u16, `Heap *` -> Heap
        m = re.search(r"(\w[\w:<>]*)(?=[^()\w]*$)", t)
        if not m or m.group(1) in seen:
            break
        base = m.group(1)
        seen.add(base)
        if base in typedef:
            t = t[:m.start()] + typedef[base] + t[m.end():]
        elif tmap and base in tmap and base not in keywords:
            t = t[:m.start()] + tmap[base] + t[m.end():]
        else:
            break
    # `struct X`/`class X`/`enum X` tag words don't affect the mangled type
    t = re.sub(r"\b(?:struct|class|enum|union)\s+", "", t)
    t = re.sub(r"\s+", " ", t).strip()
    if c_file and t == "bool":
        t = "int"
    t = re.sub(r"^signed int$", "int", t)
    t = re.sub(r"^signed short$", "short", t)
    t = re.sub(r"^signed long long$", "long long", t)
    t = re.sub(r"^signed long$", "long", t)
    t = re.sub(r"^long int$", "long", t)
    t = re.sub(r"^long long int$", "long long", t)
    t = re.sub(r"^unsigned int$", "unsigned int", t)
    t = re.sub(r"^struct\s+", "", t)
    t = re.sub(r"^class\s+", "", t)
    t = re.sub(r"^enum\s+", "", t)
    return t


def _leaf_type(t):
    """``mHeap::Heap_t *`` -> ``Heap_t *``: a source may spell a nested class
    by its leaf name or its full path; compare on the leaf. Template
    arguments are kept (``Fix12<int>`` has no ``::`` to strip)."""
    t = re.sub(r"\b[A-Za-z_]\w*::", "", t)
    return re.sub(r"\(\s*\*\s*\)", "(*)", t)


def _norm_params(raw, c_file, tmap=None):
    parts, depth, cur = [], 0, ""
    for c in raw:
        if c in "(<[":
            depth += 1
        elif c in ")>]":
            depth = max(0, depth - 1)
        if c == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += c
    parts.append(cur)
    out = []
    for p in parts:
        p = p.split("=")[0]
        t = _norm_type(p, c_file, tmap)
        if t and t != "void":
            out.append(t)
    return tuple(out)


class Def:
    __slots__ = ("start", "end", "qualified", "params", "ns", "extern_c",
                 "flat_hint", "is_object", "has_init", "dead", "scopes")

    def __init__(self, start, end, qualified, params, ns, extern_c,
                 flat_hint=False, is_object=False, has_init=False,
                 dead=False, scopes=()):
        self.start = start          # byte offset, first char of the statement
        self.end = end              # byte offset just past the closing `}`
        self.qualified = qualified
        self.params = params        # raw text inside the parens
        self.ns = ns                # enclosing namespaces, outermost first
        self.extern_c = extern_c    # C linkage: extern "C" block/decl/line
        self.dead = dead            # inside an _MSC_VER branch: mwccarm
                                    # never sees it, so it emits no symbol
        # Enclosing ``namespace`` / ``extern "C"`` frames, outermost first.
        # Each is ``{kind, name, stmt, brace, close}``; ``close`` is filled
        # in when the frame's ``}`` is reached, so a Def built earlier still
        # sees it.
        self.scopes = scopes
        # `flat_hint`: an `extern "C" <name>(...);` declaration was seen at
        # top level earlier in this file, so an unmarked definition of <name>
        # still emits the flat symbol (C linkage is sticky per name).
        self.flat_hint = flat_hint
        self.is_object = is_object  # file-scope object definition
        self.has_init = has_init    # object: carries an initializer

    @property
    def full(self):
        return "::".join([*self.ns, self.qualified]) if self.ns else self.qualified


def _cpp_off_regions(text):
    """``#pragma cplusplus off`` region starts/ends: a sorted list of
    ``(offset, is_off)`` toggles. Code in an off region links flat."""
    return [(m.start(), m.group(1) == "off")
            for m in re.finditer(
                r"#\s*pragma\s+cplusplus\s+(off|on)\b", text)]


_MSC_DEFINED_RE = re.compile(r'defined\s*\(?\s*_MSC_VER')
_MSC_NEGATED_RE = re.compile(r'!\s*defined\s*\(?\s*_MSC_VER')


def _dead_regions(text):
    """Byte intervals mwccarm never sees: ``#if``-family branches that are
    only live with ``_MSC_VER`` defined (host/port code). Unknown conditions
    are treated as live -- under-claiming death is the safe side."""
    PP = re.compile(r'(?m)^[ \t]*#[ \t]*'
                    r'(ifdef|ifndef|if|elif|else|endif)\b([^\n]*)')

    def cond_eval(cond):
        """(arm_dead, else_dead) for one arm's condition text."""
        cond = cond.strip()
        if cond == "_MSC_VER":
            return True, False               # #ifdef _MSC_VER
        if _MSC_NEGATED_RE.search(cond) or _MSC_DEFINED_RE.search(cond):
            if _MSC_NEGATED_RE.search(cond):
                # `!defined(_MSC_VER)` arm is live under mwccarm; what
                # follows it (#else/later arms) needs the host compiler.
                return False, True
            if "||" in cond:
                return False, False          # `defined(_MSC_VER) || X` is X
            return True, False               # `defined(_MSC_VER) [&& X]`
        return False, False                  # unknown condition: live

    stack = []      # per open #if: [arm_dead, negated_msc_seen]
    spans = []
    cur = None      # offset where the dead interval opened
    for m in PP.finditer(text):
        kind, rest = m.group(1), m.group(2)
        if kind in ("if", "ifdef", "ifndef"):
            if kind == "ifdef":
                arm, els = cond_eval(rest)
            elif kind == "ifndef":
                arm, els = (False, True) if rest.strip() == "_MSC_VER" \
                    else (False, False)
            else:
                arm, els = cond_eval(rest)
            stack.append([arm, els])
        elif kind == "elif":
            if stack:
                arm, els = cond_eval(rest)
                stack[-1][0] = arm
                stack[-1][1] = stack[-1][1] or els
        elif kind == "else":
            if stack:
                stack[-1][0] = stack[-1][1]
        elif kind == "endif":
            if stack:
                stack.pop()
        dead = any(f[0] for f in stack)
        if dead and cur is None:
            cur = m.start()
        elif not dead and cur is not None:
            spans.append((cur, m.start()))
            cur = None
    if cur is not None:
        spans.append((cur, len(text)))
    return spans


def _dead_at(spans, pos):
    return any(a <= pos < b for a, b in spans)


def _cpp_is_off(toggles, pos):
    state = False
    for off, is_off in toggles:
        if off > pos:
            break
        state = is_off
    return state


def _flat_names_in(text):
    """Names declared inside ``extern "C"`` regions of ``text`` (headers
    included). A later definition of the same name keeps C linkage even
    with no marker on the definition itself."""
    blank = _blank(text)
    n = len(blank)
    flat = set()
    stack = []
    ctx = []
    stmt = None
    i = 0
    while i < n:
        c = blank[i]
        if c == "{":
            head_start = stmt if stmt is not None else i
            h = blank[head_start:i].strip()
            if _CONTAINER_RE.match(h):
                kind = ("extern_c"
                        if _EXTERN_C_KIND_RE.search(text[head_start:i])
                        else "ns")
                name_m = re.search(rf'namespace\s+({_IDENT})', h)
                ctx.append((kind, name_m.group(1) if name_m else None))
                stack.append(i)
                i += 1
                stmt = None
                continue
            depth, j = 1, i + 1
            while j < n and depth:
                if blank[j] == "{":
                    depth += 1
                elif blank[j] == "}":
                    depth -= 1
                j += 1
            i = j
            continue
        if c == "}":
            if stack:
                stack.pop()
            if ctx:
                ctx.pop()
            i += 1
            stmt = None
            continue
        if c == ";":
            if stmt is not None:
                if any(k == "extern_c" for k, _ in ctx) or \
                        _EXTERN_C_KIND_RE.match(text[stmt:i].strip()):
                    s = blank[stmt:i]
                    flat.update(re.findall(rf'\b({_IDENT})\s*\(', s))
                    flat.update(
                        re.findall(rf'\b({_IDENT})\s*(?=[,;=\[])', s))
                stmt = None
            i += 1
            continue
        if stmt is None and not c.isspace():
            stmt = i
        i += 1
    return flat


_INCLUDE_RE = re.compile(r'#\s*include\s+"([^"]+)"')


@functools.lru_cache(maxsize=None)
def _header_maps(fpath):
    """``(typedefs, flat_names, include_names)`` for one header file."""
    try:
        with open(fpath, encoding="utf-8", errors="replace") as f:
            text = f.read()
    except OSError:
        return {}, frozenset(), ()
    return (_typedef_map(text), frozenset(_flat_names_in(text)),
            tuple(_INCLUDE_RE.findall(text)))


_REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def _include_dirs(path):
    yield os.path.dirname(path)
    # the repo's include/ dir, found from this module's location -- callers
    # may pass cwd-relative paths from any working directory
    yield os.path.join(_REPO_ROOT, "include")
    p = path
    while True:
        np = os.path.dirname(p)
        cand = os.path.join(np, "include") if np else "include"
        if os.path.isdir(cand):
            yield cand
            return
        if not np or np == p:
            return
        p = np


def _include_maps(path, text):
    """Merged ``(typedefs, flat_names)`` over the file's own ``#include``
    closures (headers under ``include/`` or next to the file, recursive)."""
    tmap, flat = {}, set()
    seen = set()

    def visit(fpath, ftext, depth):
        nonlocal flat
        if depth > 3:
            return
        for name in _INCLUDE_RE.findall(ftext):
            for d in _include_dirs(fpath):
                cand = os.path.normpath(os.path.join(d, name))
                if cand in seen or not os.path.isfile(cand):
                    continue
                seen.add(cand)
                hm, hf, incs = _header_maps(cand)
                for k, v in hm.items():
                    tmap.setdefault(k, v)
                flat |= hf
                with open(cand, encoding="utf-8",
                          errors="replace") as f:
                    htext = f.read()
                visit(cand, htext, depth + 1)
                break
    visit(path, text, 0)
    return tmap, flat


def _typedef_map(text):
    """File-local ``typedef <type> Name;`` / ``using Name = <type>;``
    aliases. Simple spellings only -- the anonymous-struct form contains
    braces and is skipped on purpose."""
    blank = _blank(text)
    out = {}
    for m in re.finditer(r"typedef\s+([^;{}]+?)\s+(\w+)\s*"
                         r"(?:\[[^\]]*\])?\s*;", blank):
        out[m.group(2)] = m.group(1).strip()
    # `typedef int (C::*Name)(args) const;` -> Name = `int (C::*)(args) const`
    for m in re.finditer(
            r"typedef\s+(.+?)\s*\(\s*(\w[\w:]*)\s*::\s*\*\s*(\w+)\s*\)"
            r"\s*(\([^()]*\))\s*(const)?\s*;", blank):
        out[m.group(3)] = (f"{m.group(1).strip()} ({m.group(2)}::*)"
                           f"{m.group(4)}"
                           + (" const" if m.group(5) else ""))
    # `typedef void (*Name)(args);` -> Name = `void (*)(args)`
    for m in re.finditer(
            r"typedef\s+(.+?)\s*\(\s*\*\s*(\w+)\s*\)\s*(\([^()]*\))\s*;",
            blank):
        out.setdefault(m.group(2),
                       f"{m.group(1).strip()} (*){m.group(3)}")
    for m in re.finditer(r"\busing\s+(\w+)\s*=\s*([^;]+?)\s*;", blank):
        out.setdefault(m.group(1), m.group(2).strip())
    # normalize each value so substitutions compare equal to both the
    # demangled spelling and a param written with the alias; drop captures
    # that normalize to a bare tag word or back to the key itself
    norm = {}
    for k, v in out.items():
        nv = _norm_type(v, False)
        if nv and nv != k and nv not in ("struct", "class", "enum",
                                         "union", "void"):
            norm[k] = nv
    return norm


def definitions(text, extra_flat=frozenset()):
    """Top-level (and namespace/extern-"C"-scoped) function definitions of
    ``text`` as ``[Def]`` in source order. ``extra_flat`` carries names a
    header already declared under ``extern "C"`` (see ``_include_maps``)."""
    blank = _blank(text)
    n = len(blank)
    defs = []
    stack = []            # enclosing namespace / extern "C" frames
    ctx = []              # (kind, name) for namespace / extern-c scope
    flat_names = set(extra_flat)
    cpp_toggles = _cpp_off_regions(text)
    dead_spans = _dead_regions(text)
    stmt_start = None
    i = 0
    while i < n:
        c = blank[i]
        if c == "{":
            # What does this brace open? Look at the head of the statement.
            head_start = stmt_start if stmt_start is not None else i
            head = blank[head_start:i]
            h = head.strip()
            sig = _split_head(h)
            m = _CONTAINER_RE.match(h)
            if m:
                name_m = re.search(rf'namespace\s+({_IDENT})', h)
                # The linkage kind is read off the ORIGINAL text: `_blank`
                # wiped the string's contents, so `extern "C"` is `extern " "`.
                kind = ("extern_c"
                        if _EXTERN_C_KIND_RE.search(text[head_start:i])
                        else "ns")
                frame = {"kind": kind,
                         "name": name_m.group(1) if name_m else None,
                         "stmt": head_start, "brace": i, "close": None}
                ctx.append((kind, frame["name"]))
                stack.append(frame)
                i += 1
                stmt_start = None
                continue
            if sig is not None:
                # function definition -- consume the body to its close
                qualified, params = _name_and_params(sig)
                depth, j = 1, i + 1
                while j < n and depth:
                    if blank[j] == "{":
                        depth += 1
                    elif blank[j] == "}":
                        depth -= 1
                    j += 1
                if qualified:
                    ns = [nm for k, nm in ctx if k == "ns" and nm]
                    flat = (qualified.split("::")[-1] in flat_names
                            and not ns)
                    defs.append(Def(
                        head_start, j, qualified, params, tuple(ns),
                        any(k == "extern_c" for k, _ in ctx)
                        or _cpp_is_off(cpp_toggles, head_start)
                        or bool(_EXTERN_C_KIND_RE.match(
                            text[head_start:i].strip())), flat,
                        dead=_dead_at(dead_spans, head_start),
                        scopes=tuple(stack)))
                i = j
                stmt_start = None
                # A function body may be followed by a stray `;`; it is
                # swallowed by the normal statement loop.
                continue
            # A data/aggregate block inside the statement (struct body,
            # initializer, array): consume it, statement continues.
            depth, j = 1, i + 1
            while j < n and depth:
                if blank[j] == "{":
                    depth += 1
                elif blank[j] == "}":
                    depth -= 1
                j += 1
            i = j
            continue
        if c == "}":
            if stack:
                stack[-1]["close"] = i + 1
                stack.pop()
                if ctx:
                    ctx.pop()
            i += 1
            stmt_start = None
            continue
        if c == ";":
            if stmt_start is not None:
                stmt = blank[stmt_start:i]
                in_extern = (any(k == "extern_c" for k, _ in ctx)
                             or bool(_EXTERN_C_KIND_RE.match(
                                 text[stmt_start:i].strip())))
                if in_extern:
                    # `extern "C" <decl>;` or any declaration inside an
                    # `extern "C" { }` block -- harvest the DECLARED names;
                    # a later unmarked definition of the same name keeps
                    # C linkage.
                    flat_names.update(
                        re.findall(rf'\b({_IDENT})\s*\(', stmt))
                    flat_names.update(
                        re.findall(rf'\b({_IDENT})\s*(?=[,;=\[])', stmt))
                _record_object_defs(stmt, stmt_start, i + 1, ctx, defs,
                                    _dead_at(dead_spans, stmt_start),
                                    scopes=tuple(stack))
                stmt_start = None
            i += 1
            continue
        if stmt_start is None and not c.isspace():
            stmt_start = i
        i += 1
    return defs


_DECL_WORD_RE = re.compile(
    r'^\s*(?:typedef|using|static_assert|return|goto|struct|class|union|enum'
    r'|namespace|template|asm|__asm|friend|do|while|for|if|else|switch|case'
    r'|default|break|continue|delete)\b')
_SPEC_WORD_RE = re.compile(
    r'\b(?:extern|static|const|volatile|register|mutable|inline|constexpr'
    r'|virtual|friend)\b')
_SCALAR_TYPES = frozenset(
    "int char short long float double signed unsigned void bool wchar_t"
    " u8 s8 u16 s16 u32 s32 u64 s64 vu16 vs16 vu32 vs32 f32 fx16 fx32"
    " size_t ssize_t".split())


def _param_listish(inner):
    """True when a parenthesised list reads as parameter declarations, not
    argument expressions -- `int a, const V &v` vs `42, &g, a+b`. The truly
    ambiguous shapes (`Type x(a * b)`) resolve as declarations, matching
    the most-vexing-parse rule the compiler applies."""
    s = inner.strip()
    if s in ("", "void", "..."):
        return True
    items, depth, cur = [], 0, ""
    for ch in s:
        if ch in "([<":
            depth += 1
        elif ch in ")]>":
            depth = max(0, depth - 1)
        if ch == "," and depth == 0:
            items.append(cur)
            cur = ""
        else:
            cur += ch
    items.append(cur)
    for it in items:
        it = re.sub(r'=[^=]*$', "", it).strip()       # default-argument tail
        it = re.sub(r'\[[^\]]*\]', "", it).strip()   # array declarator dims
        if not it or it in ("...", "void"):
            continue
        if "(*" in it:                               # fn-pointer declarator
            continue
        # literal, call, cast or member access -> an argument expression;
        # `*`/`&` stay out: they are declarator operators as often as they
        # are multiplication/address-of, and ambiguity resolves as a decl.
        if re.search(r"(?<![\w])\d|\"|'|->|\.|\(|\)|[+\-|!~^%/?]", it):
            return False
        words = it.split()
        # a lone identifier is a value expression, not a parameter --
        # unless it spells a type name (scalar keyword, Capitalised class,
        # `_t` typedef). A SHOUTY `ASSET_FOO_BMD` word is a macro argument.
        if len(words) == 1:
            w = words[0].strip("*&")
            typeish = (w in _SCALAR_TYPES or w.endswith("_t")
                       or (w[:1].isupper() and not w.isupper()))
            if not typeish:
                return False
    return True


def _paren_inner(s, pos):
    """``s[pos]`` is ``(``; return the text inside the balanced parens."""
    depth, j = 1, pos + 1
    while j < len(s) and depth:
        if s[j] == "(":
            depth += 1
        elif s[j] == ")":
            depth -= 1
        j += 1
    return s[pos + 1:j - 1] if depth == 0 else None


def _declarator_names(head):
    """The object names a `Type a = v, *b, c[N];` statement declares -- the
    last identifier of each top-level comma part, read only up to its own
    initialiser so names inside `= {...}`/`(args)` never leak in."""
    parts, depth, cur = [], 0, ""
    for ch in head:
        if ch in "([{<":
            depth += 1
        elif ch in ")]}>":
            depth = max(0, depth - 1)
        if ch == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    parts.append(cur)
    names = []
    for p in parts:
        pm = re.search(r'\(\s*\*\s*(' + _IDENT + r')', p)   # `void (*cb)(..)`
        if pm:
            names.append(pm.group(1))
            continue
        ids = re.findall(rf'{_IDENT}(?::{_IDENT})*',
                         re.split(r'[=\(\{\[]', p, maxsplit=1)[0])
        if ids:
            names.append(ids[-1])
    return names


def _record_object_defs(stmt, start, end, ctx, defs, dead=False, scopes=()):
    """Append object ``Def`` records for a `;`-terminated statement that
    defines file-scope objects. ``stmt`` is the blanked statement text."""
    s = stmt.strip()
    if not s or _DECL_WORD_RE.match(s):
        return
    eq_init = bool(re.search(r'=(?!=)', s))
    brace_init = bool(re.search(r'\b' + _IDENT + r'\s*\{', s))
    # `Type x(args)` is an initialised object only when the parens carry
    # argument expressions; `Ret name(<params>)` is a function declaration
    # and defines nothing here.
    paren_init = False
    pm = re.search(r'\b(' + _IDENT + r'(?:::' + _IDENT + r')*)\s*\(', s)
    if pm:
        inner = _paren_inner(s, pm.end() - 1)
        if inner is not None and _param_listish(inner):
            return
        if inner is not None:
            paren_init = True
    has_init = eq_init or brace_init or paren_init
    # `extern Type x;` declares; only an initialised extern defines.
    if re.match(r'extern\b', s) and not has_init:
        return
    head = _SPEC_WORD_RE.sub("", s)
    names = _declarator_names(head)
    if not names and not has_init:
        return
    extern_c = any(k == "extern_c" for k, _ in ctx)
    ns = tuple(nm for k, nm in ctx if k == "ns" and nm)
    for nm in names:
        defs.append(Def(start, end, nm, "", ns, extern_c, False, True,
                        has_init, dead, scopes))


# ------------------------------------------------------------------ binding

def _expected_key(symbol):
    """The (qualified, params) a definition must show to emit ``symbol``, or
    ``(flat_name, None)`` for an unmangled symbol."""
    d = _dm.demangle(symbol)
    if d is None:
        return symbol, None
    if d.get("qualified") is None:
        return symbol, None
    return d["qualified"], tuple(_leaf_type(_norm_type(a, False))
                                 for a in d.get("args") or [])


def _comment_marks(text):
    """``[(offset, symbol)]`` of ``// @symbol`` lines -- transition-era
    tie-break input only. Line comments inside ``/* */`` or strings do not
    count, and the ``//`` must open the line (leading whitespace only)."""
    marks = []
    i, n = 0, len(text)
    in_block = in_str = None
    while i < n:
        c = text[i]
        if in_block:
            if c == "*" and i + 1 < n and text[i + 1] == "/":
                in_block, i = None, i + 2
            else:
                i += 1
            continue
        if in_str:
            if c == "\\":
                i += 2
                continue
            if c == in_str:
                in_str = None
            i += 1
            continue
        if c in "\"'":
            in_str = c
            i += 1
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "*":
            in_block, i = True, i + 2
            continue
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            j = n if j < 0 else j
            m = re.match(r"//\s*@symbol\s+(\S+)", text[i:j])
            line_start = text.rfind("\n", 0, i) + 1
            if m and not text[line_start:i].strip():
                marks.append((i, m.group(1)))
            i = j
            continue
        i += 1
    return marks


_DATA_ROW = re.compile(r"^(\S+)\s+kind:(\w+)[^\n]*?addr:(0x[0-9a-fA-F]+)")


@functools.lru_cache(maxsize=1)
def _data_ownership():
    """``{src rel path: [data symbols, ROM order]}`` for every NON-function
    symbols.txt row (``kind:data|bss|rodata|...``). ``srcpath.symbols_for``
    is function-kind only by design; data ownership is resolved by address
    through the same delinks ranges via ``srcpath.owner_at``."""
    import relocs
    import srcpath as SP
    out = {}
    for symbols_path, label in relocs.module_universe(repo=SP.REPO):
        rows = []
        for line in symbols_path.read_text(encoding="utf-8",
                                           errors="ignore").splitlines():
            m = _DATA_ROW.match(line.strip())
            if m and m.group(2) != "function":
                rows.append((int(m.group(3), 16), m.group(1)))
        rows.sort()
        for addr, name in rows:
            owner = SP.owner_at(label, addr)
            if owner is not None:
                rel = owner.relative_to(SP.REPO).as_posix()
                out.setdefault(rel, []).append(name)
    return out


def _all_owned(path):
    """Function + data symbols owned by ``path`` (delinks enrolment)."""
    import srcpath as SP
    owned = list(SP.symbols_for(path))
    owned += _data_ownership().get(str(path).replace("\\", "/"), [])
    return owned


def bind(path, text=None, owned=None):
    """``{symbol: Def}`` for every owned symbol of ``path`` that resolves.

    ``owned`` defaults to the file's enrolled symbols (symbols.txt function
    rows plus data/bss rows inside its delinks ranges). One definition can
    own several symbols -- ctor/dtor variants and deadstripped emissions
    alike."""
    if text is None:
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read()
    if owned is None:
        owned = _all_owned(path)
    c_file = str(path).endswith(".c")
    itmap, iflat = _include_maps(path, text)
    tmap = {**itmap, **_typedef_map(text)}
    defs = definitions(text, iflat)

    # expected key -> [symbols]
    groups = {}
    for sym in owned:
        groups.setdefault(_expected_key(sym), []).append(sym)

    bound = {}
    owned_set = set(owned)
    # host-only definitions (`#ifdef _MSC_VER` arms) emit no ROM symbol; a
    # live definition with the same name wins the binding, so dead ones are
    # only considered when no live definition claims the key.
    order = ([d for d in defs if not d.dead]
             + [d for d in defs if d.dead])
    # key -> [claiming defs]; resolved below -- several live definitions
    # claiming one key is ambiguous and binds nothing unless a literal
    # `// @symbol` mark (transition era) singles one out.
    claims = {}
    name_claims = {}
    for d in order:
        # a definition may literally spell the mangled symbol (hand-spelled
        # ABI walls: `int _ZN10Foo3barEv(Foo *self) {}`) -- bind it directly
        if d.full in owned_set:
            name_claims.setdefault(d.full, []).append(d)
    for d in order:
        if d.is_object:
            # an object defines a flat name (unmangled symbol) or a mangled
            # static-member name (`Class::x` -> `_ZN5Class1xE`)
            for k in ((d.full.split("::")[-1], None), (d.full, ())):
                if groups.get(k):
                    claims.setdefault(k, []).append(d)
                    break
            continue
        member = "::" in d.qualified
        if not member and (c_file or d.extern_c or d.flat_hint):
            # Emits the flat symbol -- extern "C" is flat even inside a
            # namespace, and in a .c file every top-level name is flat. A
            # `Class::member` under extern "C" still mangles (mwccarm accepts
            # the spelling and emits the member's mangled name regardless).
            key = (d.full.split("::")[-1], None)
        else:
            key = (d.full, tuple(_leaf_type(x)
                                 for x in _norm_params(d.params, c_file,
                                                       tmap)))
        if not groups.get(key) and key[1] is not None:
            # A partial qualifier is legal: `void StaffRoll::StartCredit`
            # still emits `daDemo_c::StaffRoll::StartCredit`'s symbol.
            for k in groups:
                if groups[k] and k[1] is not None and k[1] == key[1] \
                        and (k[0] == key[0] or k[0].endswith("::" + key[0])):
                    key = k
                    break
        if groups.get(key):
            claims.setdefault(key, []).append(d)

    marks = None
    def resolve(cands, want):
        live = [d for d in cands if not d.dead]
        cand = live or cands
        if len(cand) > 1:
            nonlocal marks
            if marks is None:
                marks = _comment_marks(text)
            idx = {id(d): i for i, d in enumerate(defs)}
            marked = []
            for d in cand:
                i = idx[id(d)]
                gap = defs[i - 1].end if i else 0
                if any(gap <= off < d.start and s in want
                       for off, s in marks):
                    marked.append(d)
            if len(marked) == 1:
                return marked[0]
            return None
        return cand[0] if cand else None

    for sym, cands in name_claims.items():
        d = resolve(cands, (sym,))
        if d is not None:
            bound[sym] = d
    for key, cands in claims.items():
        d = resolve(cands, groups[key])
        if d is not None:
            for s in groups[key]:
                bound[s] = d

    # `__sinit_<file>` is the compiler-emitted static-initializer function:
    # it has no function definition to mark, so the convention marks the
    # first static-initializer data block instead.
    for sym in owned:
        if sym in bound or not sym.startswith("__sinit"):
            continue
        objs = [d for d in order if d.is_object]
        # the sinit function exists to run constructor calls -- anchor it on
        # the first object whose initialiser can't be constant-folded (the
        # `Type x(args)`/`= f(..)` shapes carry a `(`); a `const int = 0x..`
        # needs no sinit at all.
        dyn = [d for d in objs
               if d.has_init and "(" in text[d.start:d.end]]
        first = (
            next((d for d in dyn if d.qualified in owned), None)
            or next(iter(dyn), None)
            or next((d for d in objs
                     if d.has_init and d.qualified in owned), None)
            or next((d for d in objs if d.has_init), None)
            or next(iter(objs), None))
        if first is not None:
            bound[sym] = first
    return bound, defs


def marks_for(path, text=None, owned=None):
    """Synthetic ``// @symbol`` marks for marker-era consumers:
    ``[(offset, symbol)]`` positioned just above each bound definition, in
    owned (ROM) order -- where real marker lines sat. Stacked ctor/dtor
    variants share a definition, so their marks stack with the last variant
    closest to the definition, exactly as the convention wrote them."""
    if text is None:
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read()
    if owned is None:
        owned = _all_owned(path)
    bound, _defs = bind(path, text, owned)
    per_def = {}
    for sym in owned:                       # owned order = marker order
        d = bound.get(sym)
        if d is not None:
            per_def.setdefault(d.start, []).append(sym)
    def _variant_rank(sym):
        # The complete-object variant (C1/D1) is the name a reader wants for
        # the definition itself; `marked[-1]` consumers claim the last mark.
        m = re.search(r'[CD]([0-3])E', sym)
        return 1 if m and sym[m.start():m.start() + 2] in ('C1', 'D1') else 0

    ends = {}
    for item in bound.values():
        if isinstance(item, Def):
            ends[item.start] = max(ends.get(item.start, 0), item.end)
    marks = []
    for start in sorted(per_def):
        syms = sorted(per_def[start], key=_variant_rank)
        # The decl window is the gap before the declarator. Stacking one
        # byte per variant walks backwards onto the previous function's
        # `}` when the gap is shorter than the stack, and that variant
        # then looks like a second, unattached destructor. Share the last
        # byte of the gap. C1/D1 is appended last; the sort below keeps
        # that order when the offsets tie.
        earlier = [end for begun, end in ends.items() if begun < start]
        prev_end = max(earlier) if earlier else 0
        if prev_end < start:
            anchor = start - 1
            for sym in syms:
                marks.append((anchor, sym))
        else:
            for j, sym in enumerate(syms):
                marks.append((max(0, start - len(syms) + j), sym))
    # owned-but-unbound symbols (emitted, no definition here -- the header-
    # inline destructor variants) were detached markers; park them past EOF
    # so they never fall into a definition's mark window.
    marks.extend((len(text) + i, sym)
                 for i, sym in enumerate(s for s in owned
                                         if s not in bound))
    # Offset only. A full tuple sort would reorder C1 behind C2 when the
    # variants share a byte, and marked[-1] would name the base variant.
    return sorted(marks, key=lambda mark: mark[0])


def _line_index(offs, offset):
    i = bisect.bisect_right(offs, offset) - 1
    return 0 if i < 0 else i


def _banner_start(text, blanked, offs, offset, floor):
    """Line start of ``offset``, walked up over the comment banner.

    Stops at real code, at a ``// @symbol`` line, and at ``floor`` so the
    banner cannot swallow the previous member."""
    line = _line_index(offs, offset)
    while line > 0:
        a, b = offs[line - 1], offs[line]
        if a < floor:
            break
        if blanked[a:b].strip():
            break
        if re.match(r"\s*//\s*@symbol\b", text[a:b]):
            break
        line -= 1
    return offs[line]


def _after_brace_line(text, brace):
    nl = text.find("\n", brace)
    return len(text) if nl < 0 else nl + 1


def _opener_text(text, scope):
    """The ``namespace`` / ``extern "C"`` lines, through the line of ``{``."""
    nl = text.find("\n", scope["brace"])
    end = len(text) if nl < 0 else nl + 1
    return text[scope["stmt"]:end]


def _scope_frames(d):
    return [s for s in getattr(d, "scopes", ())
            if s.get("kind") in ("ns", "extern_c") and s.get("close")]


def _private_anchor(d, prev_end, next_body):
    """Start of a block that closes before the next member, else the def."""
    anchor = d.start
    for scope in _scope_frames(d):
        if scope["stmt"] >= prev_end and scope["close"] <= next_body:
            anchor = min(anchor, scope["stmt"])
    return anchor


def _shared_interior(text, d, prev_end, next_body):
    """Offset just after a block's ``{`` when this is the first member of a
    block that also contains a later member. The opener line itself stays
    out of every slice and is repeated by ``_reclose``."""
    interior = None
    for scope in _scope_frames(d):
        if scope["stmt"] >= prev_end and scope["close"] > next_body:
            after = _after_brace_line(text, scope["brace"])
            interior = after if interior is None else min(interior, after)
    return interior


def _scope_cut(d, prev_end, offs):
    """Line a previous member must not cross: a block opener that belongs
    to this member (shared or private)."""
    cut = None
    for scope in _scope_frames(d):
        if prev_end <= scope["stmt"] < d.start:
            line = offs[_line_index(offs, scope["stmt"])]
            cut = line if cut is None else min(cut, line)
    return cut


def _reclose(text, frag, start, stop, scopes):
    """Put back every enclosing block the slice did not already carry whole.

    Innermost first. A closer that rode along without its opener is the
    old bug (``s32 fdiv() { ... }`` plus a stray ``}``); prepending the
    opener balances it. A block that spans the next member is repeated
    around this member only.
    """
    body = frag
    for scope in reversed(scopes):
        open_in = start <= scope["stmt"] < stop
        close_in = start < scope["close"] <= stop
        if open_in and close_in:
            continue
        opener = _opener_text(text, scope)
        if close_in and not open_in:
            body = opener + body
        elif open_in and not close_in:
            if not body.endswith("\n"):
                body += "\n"
            body += "}\n"
        else:
            if not body.endswith("\n"):
                body += "\n"
            body = opener + body + "}\n"
    return body


def member_fragment(text, symbol, bound, defs=None):
    """One member's source, or ``None`` when ``symbol`` is not bound.

    Bound definitions are the boundaries. An unbound helper between two
    of them stays with the member it sits under. A ``namespace`` or
    ``extern "C"`` block that wraps only this member is part of the slice.
    A block shared with another bound member is repeated around this one,
    so the slice keeps the linkage the definition was parsed with and does
    not end on a closing brace whose opener was left behind.
    """
    d = bound.get(symbol) if isinstance(bound, dict) else None
    if not isinstance(d, Def):
        return None
    uniq = {}
    for item in bound.values():
        if isinstance(item, Def):
            uniq[id(item)] = item
    members = sorted(uniq.values(), key=lambda m: (m.start, m.end))
    try:
        idx = next(i for i, m in enumerate(members) if m is d)
    except StopIteration:
        return None
    offs = _line_offsets(text)
    blanked = _blank(text)
    recs = []
    for i, m in enumerate(members):
        prev_end = members[i - 1].end if i else 0
        next_body = (members[i + 1].start if i + 1 < len(members)
                     else len(text) + 1)
        anchor = _private_anchor(m, prev_end, next_body)
        interior = _shared_interior(text, m, prev_end, next_body)
        start_at = anchor if interior is None else min(anchor, interior)
        start = _banner_start(text, blanked, offs, start_at, prev_end)
        recs.append((start, _scope_cut(m, prev_end, offs)))
    start = recs[idx][0]
    if idx + 1 < len(members):
        nstart, ncut = recs[idx + 1]
        stop = nstart if ncut is None else min(nstart, ncut)
    else:
        stop = len(text)
    if stop < start:
        stop = start
    frag = _reclose(text, text[start:stop], start, stop, _scope_frames(d))
    # ``#pragma cplusplus off`` gives C linkage with no brace block. The
    # slice does not carry a pragma that sits above every member, so say
    # the linkage here. A qualified name still mangles inside the block.
    if (d.extern_c and 'extern "C"' not in frag
            and "cplusplus" not in frag
            and not any(s["kind"] == "extern_c" for s in _scope_frames(d))):
        if not frag.endswith("\n"):
            frag += "\n"
        frag = 'extern "C" {\n' + frag + "}\n"
    return frag


def file_outline(path, text=None):
    """``[(line, name, symbol)]`` in source order.

    Line 0 is an owned symbol with no definition in this file (a header
    inline, or a symbol the binder could not place). Several symbols on
    one definition -- the constructor and destructor variants -- each get
    a row, which is the list the stacked marker comments used to be.
    """
    if text is None:
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read()
    bound, _defs = bind(path, text)
    offs = _line_offsets(text)
    rows, seen = [], set()
    for sym, item in bound.items():
        if not isinstance(item, Def):
            continue
        rows.append((_line_index(offs, item.start) + 1, item.full, sym))
        seen.add(sym)
    rows.sort()
    for sym in _all_owned(path):
        if sym not in seen:
            rows.append((0, "", sym))
    return rows


def main(argv=None):
    import sys
    args = list(sys.argv[1:] if argv is None else argv)
    if not args or args[0] in ("-h", "--help"):
        sys.stderr.write(
            "usage: python tools/def_locate.py <source> [<source> ...]\n"
            "\n"
            "Print the file outline: line, definition, linker symbol.\n"
            "This is the map the marker comments used to spell by hand.\n")
        return 2 if not args else 0
    for n, path in enumerate(args):
        if n:
            print()
        print(path)
        rows = file_outline(path)
        if not rows:
            print("  (nothing owned here)")
            continue
        width = max(len(name) for _line, name, _sym in rows)
        for line, name, sym in rows:
            where = f"{line:5d}" if line else "    -"
            print(f"  {where}  {name:<{width}}  {sym}")
    return 0


if __name__ == "__main__":
    import sys
    raise SystemExit(main())
