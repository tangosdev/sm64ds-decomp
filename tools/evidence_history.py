#!/usr/bin/env python3
"""Pass 1 of notes/plan-gen-header.md: recover per-class field evidence from history.

The 241 headers in include/ that carry

    /* AUTO-GENERATED from matched-function evidence by tools/gen_header.py */

were added wholesale by 5ddf7d2d (PR #866), which added zero tools/ files -- the
generator ran outside the repo and was never committed.  The tree at 5ddf7d2d~1
therefore still holds that generator's own *input*: source files that spell member
accesses as raw offsets, before migration replaced them with named members.

This tool reads that tree (via `git cat-file --batch`, never a checkout) and emits
per-class offset/width/signedness evidence to build/evidence_history.json.

Design note, and the whole reason --report exists
-------------------------------------------------
plan-gen-header.md section 2 records that the previous attempt at this measurement
matched exactly one access spelling, found 3 fields, and reported that as fact.
A tool that silently matches nothing produces a clean, confident, wrong answer.
So every number this tool prints is paired with its recall: how much was looked at,
how much was attributed, and why the rest was dropped.  "Found nothing" must be
visibly distinguishable from "looked for the wrong thing".

Usage
-----
    python tools/evidence_history.py --report
    python tools/evidence_history.py --report --class BrickBlock
    python tools/evidence_history.py --out build/evidence_history.json

Pure stdlib plus a `git` subprocess.  Read-only: writes only its output file.
"""

from __future__ import annotations

import argparse
import bisect
import collections
import json
import os
import re
import subprocess
import sys

DEFAULT_COMMITISH = "5ddf7d2d~1"
DEFAULT_OUT = os.path.join("build", "evidence_history.json")
BANNER = "AUTO-GENERATED from matched-function evidence"
PROVENANCE_CAP = 5


# ---------------------------------------------------------------------------
# git plumbing
# ---------------------------------------------------------------------------

def git(repo, *args):
    out = subprocess.run(
        ["git", "-C", repo] + list(args),
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True,
    )
    return out.stdout.decode("utf-8", "replace")


def list_tree(repo, commit, prefix="src/"):
    txt = git(repo, "ls-tree", "-r", "--name-only", commit, "--", prefix)
    return [ln for ln in txt.splitlines() if ln.strip()]


def cat_files(repo, commit, paths):
    """Stream many blobs through one `git cat-file --batch`.

    Yields (path, text).  Much faster than one `git show` per file (~2,500 files).
    """
    if not paths:
        return
    proc = subprocess.Popen(
        ["git", "-C", repo, "cat-file", "--batch"],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE,
    )
    req = "".join("%s:%s\n" % (commit, p) for p in paths).encode("utf-8")
    # Write on a thread so a large request cannot deadlock against the pipe buffer.
    import threading

    def _feed():
        try:
            proc.stdin.write(req)
            proc.stdin.close()
        except OSError:
            pass

    t = threading.Thread(target=_feed, daemon=True)
    t.start()

    out = proc.stdout
    for path in paths:
        header = out.readline()
        if not header:
            break
        parts = header.decode("utf-8", "replace").split()
        if len(parts) < 3 or parts[1] == "missing":
            yield path, None
            continue
        size = int(parts[2])
        blob = out.read(size)
        out.read(1)  # trailing newline
        yield path, blob.decode("utf-8", "replace")
    out.close()
    proc.wait()


# ---------------------------------------------------------------------------
# symbol -> class attribution
# ---------------------------------------------------------------------------

_CTOR_DTOR = re.compile(r"^[CD][0-9]")


def class_of_symbol(stem):
    """Map a mangled symbol to (class_path, method_name, why).

    Filenames are mangled symbols; `_ZN10BrickBlock8BehaviorEv` is length-prefixed
    `10` + `BrickBlock`.  Nested classes exist in this corpus and the committed
    headers spell them with `__` (include/ActorBase__SceneNode.h,
    include/Particle__Manager.h), so all components before the method name are kept.

    Returns (None, None, reason) when the symbol is not a class member.
    """
    if stem.startswith("_ZThn") or stem.startswith("_ZTv") or stem.startswith("_ZTh"):
        # Non-virtual/virtual thunk: `this` arrives already adjusted by a fixed
        # delta, so its offsets are relative to a base sub-object, not the class.
        return None, None, "file_thunk"
    m = re.match(r"^_ZN(K?)(V?)", stem)
    if not m:
        return None, None, "file_not_class_attributable"
    i = m.end()
    comps = []
    while i < len(stem) and stem[i].isdigit():
        j = i
        while j < len(stem) and stem[j].isdigit():
            j += 1
        n = int(stem[i:j])
        if n <= 0 or j + n > len(stem):
            return None, None, "file_not_class_attributable"
        comps.append(stem[j:j + n])
        i = j + n
    if not comps:
        return None, None, "file_not_class_attributable"
    rest = stem[i:]
    if rest.startswith("E"):
        # last component was the method name
        if len(comps) < 2:
            return None, None, "file_not_class_attributable"  # free function, no class
        return "__".join(comps[:-1]), comps[-1], None
    if _CTOR_DTOR.match(rest):
        meth = ("~" if rest[0] == "D" else "") + comps[-1]
        return "__".join(comps), meth, None
    if rest and rest[0].islower():
        # operator (nw, dl, pl, ...): every component is part of the class path
        return "__".join(comps), None, None
    if len(comps) >= 2:
        return "__".join(comps[:-1]), comps[-1], None
    return None, None, "file_not_class_attributable"


# ---------------------------------------------------------------------------
# C type widths
# ---------------------------------------------------------------------------

_DROP_QUALS = {"const", "volatile", "struct", "enum", "union", "register",
               "static", "extern", "inline", "__restrict", "restrict"}

_W1 = {"char", "signed char", "unsigned char", "u8", "s8", "uint8_t", "int8_t",
       "bool", "_Bool", "byte", "UInt8", "SInt8", "vu8", "vs8"}
_W2 = {"short", "short int", "signed short", "signed short int", "unsigned short",
       "unsigned short int", "u16", "s16", "uint16_t", "int16_t", "wchar_t",
       "fx16", "vu16", "vs16"}
_W4 = {"int", "signed", "signed int", "unsigned", "unsigned int", "long",
       "long int", "signed long", "signed long int", "unsigned long",
       "unsigned long int", "u32", "s32", "uint32_t", "int32_t", "float", "f32",
       "fx32", "Fix12i", "BOOL", "vu32", "vs32", "size_t", "ptrdiff_t"}
_W8 = {"long long", "long long int", "signed long long", "unsigned long long",
       "unsigned long long int", "double", "u64", "s64", "uint64_t", "int64_t",
       "f64", "fx64"}

_U_NAMES = {"u8", "u16", "u32", "u64", "uint8_t", "uint16_t", "uint32_t",
            "uint64_t", "size_t", "vu8", "vu16", "vu32", "UInt8"}
_S_NAMES = {"s8", "s16", "s32", "s64", "int8_t", "int16_t", "int32_t",
            "int64_t", "vs8", "vs16", "vs32", "SInt8"}


def normalize_type(txt):
    words = [w for w in re.split(r"\s+", txt.strip()) if w and w not in _DROP_QUALS]
    return " ".join(words)


_TYPEDEF_RE = re.compile(
    r"\btypedef\s+(?P<ty>[A-Za-z_][A-Za-z0-9_ \t]*?)\s*(?P<st>\**)\s*"
    r"(?P<name>[A-Za-z_]\w*)\s*;")


def typedef_env(code):
    """Per-file `typedef int Fix12;` map.

    Every file in this corpus carries its own typedef preamble (there was no shared
    types.h until the very commit this pass reads *behind*), so widths must be taken
    from the file's own spelling rather than from a table baked into this tool.
    """
    env = {}
    for m in _TYPEDEF_RE.finditer(code):
        name = m.group("name")
        ty = normalize_type(m.group("ty"))
        if not ty or ty == name:
            continue
        env.setdefault(name, (ty, len(m.group("st"))))
    return env


def resolve_type(txt, stars, env):
    t = normalize_type(txt)
    if env:
        for _ in range(8):
            if t not in env:
                break
            t2, st2 = env[t]
            if t2 == t:
                break
            t, stars = t2, stars + st2
    return t, stars


def type_width(txt, stars, env=None):
    """Byte width of `*(txt stars)ptr`.  None when the type is not a known scalar."""
    t, stars = resolve_type(txt, stars, env)
    if stars >= 2:
        return 4  # dereferencing a pointer-to-pointer yields a 4-byte pointer
    if t in _W1:
        return 1
    if t in _W2:
        return 2
    if t in _W4:
        return 4
    if t in _W8:
        return 8
    return None  # void, or a struct/typedef whose size we do not know


def elem_size(txt, stars, env=None):
    """Byte stride of pointer arithmetic on `txt stars name`."""
    t, stars = resolve_type(txt, stars, env)
    if stars >= 2:
        return 4
    if t == "void":
        return 1  # decompiled code uses the void*-arithmetic-is-bytes convention
    return type_width(t, stars)


def signedness(txt, stars, env=None):
    """'s', 'u' or None.  Only *explicit* spellings count.

    Bare `char`/`short`/`int` say nothing here: the plan's rule is that a signedness
    claim must be spelled, not inferred, because retyping s32<->u32 is codegen-
    affecting (runbook section 2) and a guess would manufacture the exact class of
    error this pass exists to catch.
    """
    t0 = normalize_type(txt)
    if stars >= 2:
        return None
    if t0 in _U_NAMES:
        return "u"
    if t0 in _S_NAMES:
        return "s"
    t, stars = resolve_type(txt, stars, env)
    if stars >= 2:
        return None
    if t in _U_NAMES:
        return "u"
    if t in _S_NAMES:
        return "s"
    words = t.split()
    if "unsigned" in words:
        return "u"
    if "signed" in words:
        return "s"
    return None


# ---------------------------------------------------------------------------
# comment / literal stripping (offset preserving, so line numbers stay true)
# ---------------------------------------------------------------------------

def strip_noncode(text):
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n and text[i + 1] == "/":
            while i < n and text[i] != "\n":
                out[i] = " "
                i += 1
        elif ch == "/" and i + 1 < n and text[i + 1] == "*":
            out[i] = out[i + 1] = " "
            i += 2
            while i < n and not (text[i] == "*" and i + 1 < n and text[i + 1] == "/"):
                if text[i] != "\n":
                    out[i] = " "
                i += 1
            if i < n:
                out[i] = " "
                if i + 1 < n:
                    out[i + 1] = " "
                i += 2
        elif ch in "\"'":
            quote = ch
            i += 1
            while i < n and text[i] != quote:
                if text[i] == "\\":
                    out[i] = " "
                    i += 1
                    if i < n and text[i] != "\n":
                        out[i] = " "
                        i += 1
                    continue
                if text[i] != "\n":
                    out[i] = " "
                i += 1
            if i < n:
                i += 1
        else:
            i += 1
    return "".join(out)


# ---------------------------------------------------------------------------
# locating the defining function and its first parameter
# ---------------------------------------------------------------------------

def _match_paren(text, i, open_ch="(", close_ch=")"):
    depth = 0
    while i < len(text):
        if text[i] == open_ch:
            depth += 1
        elif text[i] == close_ch:
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return -1


def _definition_at(code, name_end):
    lp = code.index("(", name_end - 1)
    rp = _match_paren(code, lp)
    if rp < 0:
        return None
    j = rp + 1
    while j < len(code) and (code[j].isspace() or code[j] == ":"):
        # skip a ctor-initialiser list introducer and whitespace
        if code[j] == ":":
            k = code.find("{", j)
            if k < 0:
                return None
            j = k
            break
        j += 1
    if j < len(code) and code[j] == "{":
        end = _match_paren(code, j, "{", "}")
        if end < 0:
            end = len(code) - 1
        return code[lp + 1:rp], code[j:end + 1], j
    return None


def find_definition(code, stem):
    """Return (params_text, body_text, body_start) for the file's own symbol."""
    for m in re.finditer(r"(?<![A-Za-z0-9_])" + re.escape(stem) + r"\s*\(", code):
        d = _definition_at(code, m.end())
        if d:
            return d
    return None, None, None


def find_method_definition(code, cls_last, method):
    """Find `Class::Method(...) { ... }` -- the already-real-C++ files.

    src/_ZN13ExpandingHeap9VAllocateEji.cpp is written as
    `void* ExpandingHeap::VAllocate(u32 size, int align)`, so there is no explicit
    `this` parameter at all and members are bare identifiers.
    """
    pat = (r"(?<![A-Za-z0-9_:])" + re.escape(cls_last) + r"\s*::\s*"
           r"(?P<meth>~?[A-Za-z_]\w*|operator[^\(]{1,12}?)\s*\(")
    best = None
    for m in re.finditer(pat, code):
        d = _definition_at(code, m.end())
        if not d:
            continue
        if method and m.group("meth").strip() == method:
            return d
        if best is None:
            best = d
    return best or (None, None, None)


def split_params(txt):
    parts, depth, cur = [], 0, []
    for ch in txt:
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    if "".join(cur).strip():
        parts.append("".join(cur))
    return [p.strip() for p in parts if p.strip()]


_DECL_RE = re.compile(
    r"^(?P<ty>[A-Za-z_][A-Za-z0-9_ \t]*?)\s*(?P<st>\*+)\s*(?:const\s+|volatile\s+)*"
    r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*$"
)


def parse_pointer_decl(txt):
    """`unsigned char *c` -> ('unsigned char', 1 star, 'c').  None if not a pointer."""
    t = txt.strip().rstrip(";").strip()
    m = _DECL_RE.match(t)
    if not m:
        return None
    ty = normalize_type(m.group("ty"))
    if not ty:
        return None
    return ty, len(m.group("st")), m.group("name")


# ---------------------------------------------------------------------------
# locally declared struct/class layouts
#
# A large slice of this corpus does not spell offsets at the use site at all.  It
# declares the layout once, at the top of the file, and then uses named members:
#
#     struct C { char pad[0x420]; PMF *pp; };      ... c->pp
#     struct FaderColor { void **vtable; };        ... thiz->vtable
#     class ExpandingHeap { u32 unk00; ... ExpandingHeapAllocator* allocator; };
#
# That is still per-file raw offset evidence -- the padding array *is* the offset --
# so it is recovered here.  Layout is computed with natural alignment; anything that
# cannot be sized (an unknown type, an unresolvable base class) truncates the layout
# at that member rather than guessing past it.
# ---------------------------------------------------------------------------

_SKIP_STMT = re.compile(
    r"^\s*(?:typedef|using|friend|template|static\s|enum\b|union\b|public:|private:|protected:)")
_FUNCPTR = re.compile(r"\(\s*\*+\s*(?P<name>[A-Za-z_]\w*)\s*\)\s*\(")


def _split_top(txt, seps=";"):
    parts, depth, cur = [], 0, []
    for ch in txt:
        if ch in "({[<":
            depth += 1
        elif ch in ")}]>":
            depth -= 1
        if ch in seps and depth == 0:
            parts.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    if "".join(cur).strip():
        parts.append("".join(cur))
    return parts


def _align_of(size):
    return 4 if size is None or size > 4 else size


_AST_OPS = None


def const_expr(txt):
    """Evaluate an integer constant array bound: `0x110 - 0xcd`, `8`, `7 * 4`.

    Padding in this corpus is routinely written as a subtraction between two
    offsets, so a plain int() would truncate every such struct at the first pad.
    """
    global _AST_OPS
    import ast
    if _AST_OPS is None:
        _AST_OPS = {
            ast.Add: lambda a, b: a + b, ast.Sub: lambda a, b: a - b,
            ast.Mult: lambda a, b: a * b, ast.Div: lambda a, b: a // b,
            ast.FloorDiv: lambda a, b: a // b, ast.Mod: lambda a, b: a % b,
            ast.LShift: lambda a, b: a << b, ast.RShift: lambda a, b: a >> b,
        }

    def ev(node):
        if isinstance(node, ast.Expression):
            return ev(node.body)
        if isinstance(node, ast.Constant) and isinstance(node.value, int):
            return node.value
        if isinstance(node, ast.BinOp) and type(node.op) in _AST_OPS:
            a, b = ev(node.left), ev(node.right)
            if a is None or b is None:
                return None
            return _AST_OPS[type(node.op)](a, b)
        if isinstance(node, ast.UnaryOp) and isinstance(node.op, ast.USub):
            v = ev(node.operand)
            return None if v is None else -v
        return None

    t = (txt or "").strip()
    if not t or not re.match(r"^[0-9a-fA-FxX\s+\-*/%()<>]+$", t):
        return None
    try:
        return ev(ast.parse(t, mode="eval"))
    except (SyntaxError, ValueError, TypeError):
        return None


def find_struct_texts(code):
    """name -> (base_list, body_text) for every struct/class with a body."""
    out = {}
    for m in re.finditer(r"\b(?:struct|class)\s+(?P<name>[A-Za-z_]\w*)\s*"
                         r"(?::(?P<bases>[^{;]*))?\{", code):
        open_brace = code.index("{", m.end() - 1)
        end = _match_paren(code, open_brace, "{", "}")
        if end < 0:
            continue
        bases = []
        if m.group("bases"):
            for b in m.group("bases").split(","):
                b = re.sub(r"\b(?:public|private|protected|virtual)\b", " ", b).strip()
                if b:
                    bases.append(b)
        out[m.group("name")] = (bases, code[open_brace + 1:end])
    return out


def struct_layout(name, texts, env=None, seen=None):
    """Compute {member -> (offset, width_or_None, sign_or_None)} + size/align/complete."""
    seen = seen or set()
    if name in seen or name not in texts:
        return None
    seen = seen | {name}
    bases, body = texts[name]

    off = 0
    align = 1
    members = {}
    complete = True

    for b in bases:
        sub = struct_layout(b, texts, env, seen)
        if sub is None or not sub["complete"]:
            return {"members": {}, "size": None, "align": 4, "complete": False}
        for mn, (mo, mw, ms) in sub["members"].items():
            members.setdefault(mn, (off + mo, mw, ms))
        align = max(align, sub["align"])
        off += sub["size"]

    if not bases and re.search(r"\bvirtual\b", body):
        off = 4  # implicit vptr occupies 0x00 (Itanium ABI, and how these were written)
        align = max(align, 4)

    for stmt in _split_top(body, ";"):
        s = stmt.strip()
        if not s:
            continue
        s = re.sub(r"\b(?:public|private|protected)\s*:", " ", s).strip()
        if not s or _SKIP_STMT.match(s):
            continue
        if re.match(r"^\s*(?:struct|class)\s+[A-Za-z_]\w*\s*\{", s):
            complete = False
            break
        fp = _FUNCPTR.search(s)
        if fp:
            off = (off + 3) & ~3
            members.setdefault(fp.group("name"), (off, 4, None))
            align = max(align, 4)
            off += 4
            continue
        if "(" in s:
            continue  # member function declaration: contributes no storage
        m = re.match(r"^(?P<ty>[A-Za-z_][A-Za-z0-9_ \t:]*)(?:\s+|(?=[*&]))"
                     r"(?P<decls>[*&\w\s,\[\]()+\-/]+)$", s)
        if not m:
            complete = False
            break
        ty = normalize_type(m.group("ty"))
        base_w = type_width(ty, 1, env)
        sub = None
        if base_w is None:
            rty, rst = resolve_type(ty, 0, env)
            sub = struct_layout(rty if rst == 0 else ty, texts, env, seen)
            if sub is not None and sub["complete"]:
                base_w = sub["size"]
        for d in m.group("decls").split(","):
            d = d.strip()
            dm = re.match(r"^(?P<st>[*&\s]*)(?P<name>[A-Za-z_]\w*)"
                          r"(?P<arr>(?:\s*\[[^\]]*\])*)\s*$", d)
            if not dm:
                complete = False
                break
            stars = dm.group("st").count("*")
            if stars:
                w, a, elemw = 4, 4, 4
            elif base_w is None:
                complete = False
                break
            else:
                w = base_w
                a = sub["align"] if sub is not None else _align_of(base_w)
                elemw = base_w
            count = 1
            arr_ok = True
            for dim in re.findall(r"\[([^\]]*)\]", dm.group("arr") or ""):
                v = const_expr(dim)
                if v is None or v < 0:
                    arr_ok = False
                    break
                count *= v
            if not arr_ok:
                complete = False
                break
            off = (off + a - 1) & ~(a - 1)
            align = max(align, a)
            # an array or a nested struct member proves the offset but has no
            # scalar width; record width only for a plain scalar/pointer member
            width = elemw if (count == 1 and elemw in (1, 2, 4, 8)) else None
            sg = None if stars else signedness(ty, 1, env)
            members.setdefault(dm.group("name"), (off, width, sg))
            off += w * count
        else:
            continue
        break

    size = (off + align - 1) & ~(align - 1) if complete else None
    return {"members": members, "size": size, "align": align, "complete": complete}


# ---------------------------------------------------------------------------
# `this` aliases
# ---------------------------------------------------------------------------

_ALIAS_RE = re.compile(
    r"(?:^|[;{}\n])\s*(?P<ty>[A-Za-z_][A-Za-z0-9_ \t]*?)\s*(?P<st>\*+)\s*"
    r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*=\s*"
    r"(?:\(\s*[A-Za-z_][A-Za-z0-9_ \t]*\*+\s*\)\s*)?"
    r"(?P<rhs>[A-Za-z_][A-Za-z0-9_]*)\s*;"
)


def find_aliases(body, bases):
    """Locals that provably still hold `this`, e.g. `unsigned char *c = (unsigned char *)thiz;`.

    Only single-assignment aliases qualify.  src/_ZN10ChainChomp13InitResourcesEv.cpp
    is the reason both halves of that rule matter: it aliases `thiz` to `c`
    (keep), but it also walks `unsigned char *dst = c;` forward with `dst = dst + 0xc`
    inside a loop (drop -- after one iteration `dst + 0x524` is no longer a field of
    this class, and recording it would invent seven fields that do not exist).
    """
    known = dict(bases)
    changed = True
    while changed:
        changed = False
        for m in _ALIAS_RE.finditer(body):
            name, rhs = m.group("name"), m.group("rhs")
            if name in known or rhs not in known:
                continue
            decl = parse_pointer_decl("%s %s %s" % (m.group("ty"), m.group("st"), name))
            if decl is None:
                continue
            ty, stars, _ = decl
            # reject if reassigned or stepped anywhere in the body
            reassign = re.compile(
                r"(?<![A-Za-z0-9_])" + re.escape(name) +
                r"\s*(?:\+\+|--|\+=|-=|=(?!=))"
            )
            hits = list(reassign.finditer(body))
            if len(hits) != 1 or not (m.start() <= hits[0].start() < m.end()):
                continue
            if re.search(r"(?:\+\+|--)\s*" + re.escape(name) + r"(?![A-Za-z0-9_])", body):
                continue
            known[name] = (ty, stars)
            changed = True
    return known


# ---------------------------------------------------------------------------
# the access scanner
# ---------------------------------------------------------------------------

ID = r"[A-Za-z_][A-Za-z0-9_]*"
NUM = r"(?:0[xX][0-9a-fA-F]+|[0-9]+)"


def _cast(sfx):
    return r"\(\s*(?P<ty%s>[A-Za-z_][A-Za-z0-9_ \t]*?)\s*(?P<st%s>\*+)\s*\)" % (sfx, sfx)


# Ordered.  Python's alternation tries branches left to right at each position, and
# finditer never overlaps, so the widest structural form at a position wins.
_FORMS = [
    # *(int *)(c + 0x80)      /  *(int *)((char *)t + 0x10)
    ("cast_deref_off",
     r"\*\s*" + _cast("a") + r"\s*\(\s*(?:" + _cast("b") + r"\s*)?"
     r"(?P<ida>" + ID + r")\s*(?P<sga>[+-])\s*(?P<na>" + NUM + r")\s*\)"),
    # *(int *)(0x10 + c)
    ("cast_deref_rev",
     r"\*\s*" + _cast("c") + r"\s*\(\s*(?P<nb>" + NUM + r")\s*\+\s*"
     r"(?:" + _cast("d") + r"\s*)?(?P<idb>" + ID + r")\s*\)"),
    # *(int *)((char *)c)[3]   -- cast, parenthesised base, index
    ("cast_deref_idx",
     r"\*\s*" + _cast("e") + r"\s*\(\s*(?:" + _cast("f") + r"\s*)?"
     r"(?P<idc>" + ID + r")\s*\)\s*\[\s*(?P<nc>" + NUM + r")\s*\]"),
    # *(int *)(c)   /   *(int *)c
    ("cast_deref_zero",
     r"\*\s*" + _cast("g") + r"\s*\(?\s*(?P<idd>" + ID + r")\s*\)?"
     r"(?![A-Za-z0-9_\[])"),
    # &c[3]  /  &((char *)c)[3]
    ("addr_of_index",
     r"&\s*\(?\s*(?:" + _cast("h") + r"\s*)?(?P<ide>" + ID + r")\s*\)?"
     r"\s*\[\s*(?P<ne>" + NUM + r")\s*\]"),
    # ((char *)this) + 0x5c
    ("paren_cast_addr",
     r"\(\s*" + _cast("i") + r"\s*(?P<idf>" + ID + r")\s*\)\s*\+\s*"
     r"(?P<nf>" + NUM + r")"),
    # ((char *)c)[3]
    ("paren_cast_idx",
     r"\(\s*" + _cast("j") + r"\s*(?P<idg>" + ID + r")\s*\)\s*\[\s*"
     r"(?P<ng>" + NUM + r")\s*\]"),
    # (char *)t + 0x10
    ("cast_addr",
     _cast("k") + r"\s*(?P<idh>" + ID + r")\s*\+\s*(?P<nh>" + NUM + r")"),
    # c[3]
    ("index",
     r"(?P<idi>" + ID + r")\s*\[\s*(?P<ni>" + NUM + r")\s*\]"),
    # c + 0x5c        (address-only: Vec3_Dist(c + 0x5c, ...))
    ("addr",
     r"(?P<idj>" + ID + r")\s*\+\s*(?P<nj>" + NUM + r")(?![A-Za-z0-9_])"),
    # self->unk_154   -- already migrated, NOT raw evidence
    ("named_member",
     r"(?P<idk>" + ID + r")\s*->\s*(?P<memk>" + ID + r")"),
]

_SCANNER = re.compile("|".join("(?:%s)" % pat for _, pat in _FORMS))
_FORM_KEYS = {
    "ida": "cast_deref_off", "idb": "cast_deref_rev", "idc": "cast_deref_idx",
    "idd": "cast_deref_zero", "ide": "addr_of_index", "idf": "paren_cast_addr",
    "idg": "paren_cast_idx", "idh": "cast_addr", "idi": "index", "idj": "addr",
    "idk": "named_member",
}


def classify(m):
    """Turn a scanner match into (form, base_id, cast_ty, cast_st, over_ty, over_st, num, sign)."""
    g = m.groupdict()
    for key, form in _FORM_KEYS.items():
        if g.get(key) is None:
            continue
        base = g[key]
        if form == "cast_deref_off":
            return form, base, g["tya"], g["sta"], g["tyb"], g["stb"], g["na"], g["sga"]
        if form == "cast_deref_rev":
            return form, base, g["tyc"], g["stc"], g["tyd"], g["std"], g["nb"], "+"
        if form == "cast_deref_idx":
            return form, base, g["tye"], g["ste"], g["tyf"], g["stf"], g["nc"], "+"
        if form == "cast_deref_zero":
            return form, base, g["tyg"], g["stg"], None, None, "0", "+"
        if form == "addr_of_index":
            return form, base, None, None, g["tyh"], g["sth"], g["ne"], "+"
        if form == "paren_cast_addr":
            return form, base, None, None, g["tyi"], g["sti"], g["nf"], "+"
        if form == "paren_cast_idx":
            return form, base, g["tyj"], g["stj"], g["tyj"], g["stj"], g["ng"], "+"
        if form == "cast_addr":
            return form, base, None, None, g["tyk"], g["stk"], g["nh"], "+"
        if form == "index":
            return form, base, None, None, None, None, g["ni"], "+"
        if form == "addr":
            return form, base, None, None, None, None, g["nj"], "+"
        if form == "named_member":
            return form, base, None, None, None, None, None, None
    return None, None, None, None, None, None, None, None


ADDRESS_ONLY_FORMS = {"addr_of_index", "paren_cast_addr", "cast_addr", "addr"}


_STMT_KEYWORDS = (r"(?:return|if|else|while|for|do|switch|case|goto|break|"
                  r"continue|sizeof|new|delete|typedef|extern)")


def rename_map(repo, commit):
    """Class renames applied to include/<Class>.h between `commit` and HEAD.

    Attribution here is by filename, and some classes were misnamed at the commit
    this pass reads.  Every `_ZN8MugenBgm*` file at 5ddf7d2d~1 is today's
    CutsceneObject (b220cf0b, "move CUTSCENE_OBJECT's files off the MugenBgm
    names"), and `_ZN19AmbientSoundEffects*` is today's MugenBgm (3c56d5c5).
    Without replaying those, this pass files one class's fields under another
    class's name -- silently, and in a form Pass 2 would then reconcile against the
    wrong base.  Renames are replayed oldest-first because they chain.

    Returns {name_at_commit: name_at_HEAD}.
    """
    txt = git(repo, "log", "--reverse", "--diff-filter=R", "-M", "--name-status",
              "--format=%x00", commit + "..HEAD", "--", "include/")
    current = {}
    for line in txt.splitlines():
        if not line.startswith("R"):
            continue
        parts = line.split("\t")
        if len(parts) != 3:
            continue
        old, new = parts[1], parts[2]
        if not (old.endswith(".h") and new.endswith(".h")):
            continue
        a = os.path.basename(old)[:-2]
        b = os.path.basename(new)[:-2]
        if a == b:
            continue
        hit = False
        for orig, cur in list(current.items()):
            if cur == a:
                current[orig] = b
                hit = True
        if not hit:
            current[a] = b
    return {k: v for k, v in current.items() if k != v}


def load_headers(repo, commit):
    """basename -> raw text, for every include/ file at the commit.

    Raw, not comment-stripped: strip_noncode() blanks string literals, which would
    erase the name in `#include "Timer.hpp"` and silently break the closure below.
    """
    out = {}
    paths = list_tree(repo, commit, "include/")
    for p, txt in cat_files(repo, commit, paths):
        if txt is not None:
            out[os.path.basename(p)] = txt
    return out


def include_closure(code, headers, depth=3):
    """Texts of the project headers a file quotes, transitively."""
    if not headers:
        return []
    seen, queue, out = set(), list(re.findall(r'#\s*include\s+"([^"]+)"', code)), []
    while queue and depth > 0:
        nxt = []
        for name in queue:
            base = os.path.basename(name)
            if base in seen or base not in headers:
                continue
            seen.add(base)
            txt = headers[base]
            out.append(txt)
            nxt.extend(re.findall(r'#\s*include\s+"([^"]+)"', txt))
        queue, depth = nxt, depth - 1
    return out


# ---------------------------------------------------------------------------
# per-file extraction
# ---------------------------------------------------------------------------

class Recall(object):
    def __init__(self):
        self.files_considered = 0
        self.files_class_attributable = 0
        self.files_parsed = 0
        self.accesses_seen = 0
        self.accesses_attributed = 0
        self.accesses_discarded_other_base = 0
        self.reasons = collections.Counter()
        self.by_form = collections.Counter()
        self.attributed_by_form = collections.Counter()
        self.renames = {}


def line_index(text):
    return [m.start() for m in re.finditer(r"\n", text)]


def line_of(nl_positions, pos):
    return bisect.bisect_right(nl_positions, pos) + 1


def extract_file(path, text, stem, cls, method, recall, headers=None):
    """Return {offset:int -> record} of evidence for the class this file defines."""
    code = strip_noncode(text)
    env = typedef_env(code)
    structs = find_struct_texts(code)
    # `#include "Timer.hpp"` -- a handful of files at this commit already keep their
    # layout in a header rather than inline, so pull those in too (name -> body maps
    # are position independent, so line numbers below stay tied to `code`).
    for inc_raw in include_closure(text, headers):
        inc = strip_noncode(inc_raw)
        for k, v in typedef_env(inc).items():
            env.setdefault(k, v)
        for k, v in find_struct_texts(inc).items():
            structs.setdefault(k, v)
    layout = None
    implicit_this = False

    params_txt, body, body_start = find_definition(code, stem)
    if body is None:
        # already-real-C++ spelling: `void* ExpandingHeap::VAllocate(u32, int) { ... }`
        cls_last = cls.split("__")[-1]
        params_txt, body, body_start = find_method_definition(code, cls_last, method)
        if body is None:
            recall.reasons["file_defining_function_not_found"] += 1
            return None
        implicit_this = True
        lay = struct_layout(cls_last, structs, env)
        if lay is None or not lay["members"]:
            recall.reasons["file_method_class_not_declared"] += 1
            return None
        layout = lay["members"]
        bases = {"this": ("void", 1)}
        recall.reasons["note_file_parsed_as_cpp_method"] += 1
    else:
        params = split_params(params_txt)
        if not params or normalize_type(params[0]) == "void":
            # e.g. src/_ZN5EnemyC2Ev.cpp -- a placeholder decompile, no `this` at all.
            recall.reasons["file_no_parameters"] += 1
            return None
        first = parse_pointer_decl(params[0])
        if first is None:
            # A non-pointer first parameter means this is not the implicit `this`:
            # almost always a static member function (_ZN5Actor5SpawnEjj... takes
            # `unsigned int` first).  Attributing its offsets would invent fields.
            recall.reasons["file_first_param_not_pointer"] += 1
            return None
        ty, stars, this_id = first
        bases = find_aliases(body, {this_id: (ty, stars)})
        if stars == 1:
            lay = struct_layout(ty, structs, env)
            if lay is not None and lay["members"]:
                layout = lay["members"]

    if layout:
        recall.reasons["note_file_layout_from_local_struct"] += 1
        # bare-identifier member uses are only safe when nothing local shadows them
        shadowed = set()
        for m in re.finditer(r"(?<![A-Za-z0-9_.>])(?P<n>[A-Za-z_]\w*)\s*(?:=[^=]|;|,|\))",
                             params_txt or ""):
            shadowed.add(m.group("n"))
        for m in _ALIAS_RE.finditer(body):
            shadowed.add(m.group("name"))
        for m in re.finditer(r"(?:^|[;{}\n])\s*(?!" + _STMT_KEYWORDS + r"\b)"
                             r"(?:[A-Za-z_][\w \t]*?)[\s*&]+"
                             r"(?P<n>[A-Za-z_]\w*)\s*(?:[=;,\[])", body):
            shadowed.add(m.group("n"))
    else:
        shadowed = set()

    nl = line_index(text)
    fields = {}

    def record(off, width, sign, pos):
        rec = fields.setdefault(off, {
            "widths": collections.Counter(),
            "signed": collections.Counter(),
            "address_only": 0,
            "provenance": [],
        })
        if width is None:
            rec["address_only"] += 1
        else:
            rec["widths"][width] += 1
            if sign:
                rec["signed"][sign] += 1
        if len(rec["provenance"]) < PROVENANCE_CAP:
            tag = "%s:%d" % (path, line_of(nl, body_start + pos))
            if tag not in rec["provenance"]:
                rec["provenance"].append(tag)

    # bare member identifiers inside a real C++ method body
    if layout and implicit_this:
        for m in re.finditer(r"(?<![A-Za-z0-9_.])(?<!->)(?P<n>[A-Za-z_]\w*)"
                             r"(?!\s*[\(:])", body):
            n = m.group("n")
            if n not in layout or n in shadowed:
                continue
            recall.accesses_seen += 1
            recall.by_form["cpp_bare_member"] += 1
            off, width, sg = layout[n]
            record(off, width, sg, m.start())
            recall.accesses_attributed += 1
            recall.attributed_by_form["cpp_bare_member"] += 1

    for m in _SCANNER.finditer(body):
        form, base, cty, cst, oty, ost, num, sign = classify(m)
        if form is None:
            continue
        recall.accesses_seen += 1
        recall.by_form[form] += 1
        if base not in bases:
            recall.accesses_discarded_other_base += 1
            recall.reasons["access_other_base"] += 1
            continue
        if form == "named_member":
            # `c->pp`.  Raw evidence only if the file declared the layout itself.
            mem = m.groupdict().get("memk")
            if layout and mem in layout:
                off, width, sg = layout[mem]
                record(off, width, sg, m.start())
                recall.accesses_attributed += 1
                recall.attributed_by_form["named_member"] += 1
            else:
                recall.reasons["access_named_member_unresolved"] += 1
            continue
        if sign == "-":
            recall.reasons["access_negative_offset"] += 1
            continue

        # stride for the `+ N` / `[N]` arithmetic
        if oty is not None:
            stride = elem_size(oty, len(ost), env)
        else:
            bty, bst = bases[base]
            stride = elem_size(bty, bst, env)
        if stride is None:
            recall.reasons["access_unknown_base_elem_size"] += 1
            continue

        n = int(num, 16) if num.lower().startswith("0x") else int(num)
        if form in ("cast_deref_off", "cast_deref_rev", "cast_deref_zero"):
            off = n * (1 if form == "cast_deref_zero" else stride)
        else:
            off = n * stride

        # width of the value actually touched
        width = None
        if cty is not None:
            width = type_width(cty, len(cst), env)
            if width is None:
                # *(Vector3 *)(c + 0x5c) -- proves a field is here, not how wide it is
                recall.reasons["note_access_width_unknown_kept_as_address_only"] += 1
        elif form == "index":
            bty, bst = bases[base]
            width = type_width(bty, bst, env)
        elif form == "paren_cast_idx":
            width = type_width(oty, len(ost), env)

        if form in ADDRESS_ONLY_FORMS:
            width = None  # an address, not a load: offset without width

        sg = None
        if width is not None:
            sg = signedness(cty, len(cst), env) if cty is not None else None
            if sg is None and cty is None and form in ("index", "paren_cast_idx"):
                sg = (signedness(oty, len(ost), env) if oty is not None
                      else signedness(bases[base][0], bases[base][1], env))
        record(off, width, sg, m.start())

        recall.accesses_attributed += 1
        recall.attributed_by_form[form] += 1
    return fields


# ---------------------------------------------------------------------------
# driver
# ---------------------------------------------------------------------------

def merge(dst, src):
    for off, rec in src.items():
        d = dst.setdefault(off, {
            "widths": collections.Counter(),
            "signed": collections.Counter(),
            "address_only": 0,
            "provenance": [],
        })
        d["widths"].update(rec["widths"])
        d["signed"].update(rec["signed"])
        d["address_only"] += rec["address_only"]
        for p in rec["provenance"]:
            if len(d["provenance"]) < PROVENANCE_CAP and p not in d["provenance"]:
                d["provenance"].append(p)


def build(repo, commitish, follow_renames=True):
    sha = git(repo, "rev-parse", commitish).strip()
    paths = list_tree(repo, sha, "src/")
    recall = Recall()
    recall.files_considered = len(paths)
    renames = rename_map(repo, sha) if follow_renames else {}
    recall.renames = renames

    targets = []
    for p in sorted(paths):
        stem = re.sub(r"\.(c|cpp|cc|cxx)$", "", os.path.basename(p))
        cls, meth, why = class_of_symbol(stem)
        if cls is None:
            recall.reasons[why] += 1
            continue
        if cls in renames:
            cls = renames[cls]
            recall.reasons["note_class_renamed_since_commit"] += 1
        targets.append((p, stem, cls, meth))
    recall.files_class_attributable = len(targets)

    classes = {}
    headers = load_headers(repo, sha)
    want = {p: (stem, cls, meth) for p, stem, cls, meth in targets}
    for path, text in cat_files(repo, sha, [p for p, _, _, _ in targets]):
        if text is None:
            recall.reasons["file_blob_missing"] += 1
            continue
        stem, cls, meth = want[path]
        fields = extract_file(path, text, stem, cls, meth, recall, headers)
        if fields is None:
            continue
        recall.files_parsed += 1
        if fields:
            merge(classes.setdefault(cls, {}), fields)
    return sha, classes, recall


def to_json(sha, classes, recall):
    out_classes = {}
    for cls in sorted(classes):
        offs = classes[cls]
        d = {}
        for off in sorted(offs):
            rec = offs[off]
            d["0x%x" % off] = {
                "widths": {str(w): rec["widths"][w] for w in sorted(rec["widths"])},
                "signed": {"s": rec["signed"].get("s", 0), "u": rec["signed"].get("u", 0)},
                "address_only": rec["address_only"],
                "provenance": rec["provenance"],
            }
        out_classes[cls] = d

    total_offsets = sum(len(v) for v in out_classes.values())
    return {
        "meta": {
            "pass": "history",
            "commit": sha,
            "recall": {
                "files_considered": recall.files_considered,
                "files_class_attributable": recall.files_class_attributable,
                "files_parsed": recall.files_parsed,
                "accesses_seen": recall.accesses_seen,
                "accesses_attributed": recall.accesses_attributed,
                "accesses_discarded_other_base": recall.accesses_discarded_other_base,
                "discard_reasons": dict(sorted(recall.reasons.items())),
            },
            "detail": {
                "classes_with_evidence": len(out_classes),
                "offsets_recovered": total_offsets,
                "accesses_seen_by_form": dict(sorted(recall.by_form.items())),
                "accesses_attributed_by_form": dict(sorted(recall.attributed_by_form.items())),
                "class_renames_followed": dict(sorted(recall.renames.items())),
            },
        },
        "classes": out_classes,
    }


def bannered_classes(repo):
    names = set()
    inc = os.path.join(repo, "include")
    if not os.path.isdir(inc):
        return names
    for fn in sorted(os.listdir(inc)):
        if not fn.endswith(".h"):
            continue
        try:
            with open(os.path.join(inc, fn), "r", encoding="utf-8", errors="replace") as fh:
                head = fh.read(4096)
        except OSError:
            continue
        if BANNER in head:
            names.add(fn[:-2])
    return names


def print_report(doc, repo):
    r = doc["meta"]["recall"]
    d = doc["meta"]["detail"]
    W = 78
    print("=" * W)
    print("RECALL -- evidence_history pass 1  (commit %s)" % doc["meta"]["commit"][:12])
    print("=" * W)
    print("  files considered (src/ at commit)   %8d" % r["files_considered"])
    print("  files class-attributable            %8d   %s" %
          (r["files_class_attributable"],
           _pct(r["files_class_attributable"], r["files_considered"])))
    print("  files parsed (this-base found)      %8d   %s" %
          (r["files_parsed"],
           _pct(r["files_parsed"], r["files_class_attributable"])))
    print("  " + "-" * (W - 4))
    print("  accesses seen (candidate sites)     %8d" % r["accesses_seen"])
    print("  accesses ATTRIBUTED to this-class   %8d   %s" %
          (r["accesses_attributed"], _pct(r["accesses_attributed"], r["accesses_seen"])))
    print("  accesses discarded, other base      %8d   %s" %
          (r["accesses_discarded_other_base"],
           _pct(r["accesses_discarded_other_base"], r["accesses_seen"])))
    print()
    print("  discard / skip reasons  (why something was NOT counted as evidence)")
    for k, v in sorted(r["discard_reasons"].items(), key=lambda kv: -kv[1]):
        if not k.startswith("note_"):
            print("      %-44s %8d" % (k, v))
    notes = [(k, v) for k, v in sorted(r["discard_reasons"].items()) if k.startswith("note_")]
    if notes:
        print()
        print("  notes  (informational, nothing was dropped)")
        for k, v in notes:
            print("      %-44s %8d" % (k, v))
    print()
    print("  access forms matched      seen  attributed")
    forms = set(d["accesses_seen_by_form"]) | set(d["accesses_attributed_by_form"])
    for f in sorted(forms, key=lambda f: -d["accesses_seen_by_form"].get(f, 0)):
        print("      %-22s %8d  %10d" %
              (f, d["accesses_seen_by_form"].get(f, 0),
               d["accesses_attributed_by_form"].get(f, 0)))
    print("=" * W)

    if r["accesses_attributed"] == 0:
        print("!! ZERO accesses attributed.  This is the plan section-2 failure mode:")
        print("!! the extractor matched nothing, which is NOT the same as there being")
        print("!! nothing to match.  Do not consume this file.")
        print("=" * W)
        return
    if r["files_parsed"] == 0 or r["accesses_attributed"] < r["files_parsed"]:
        print("!! WARNING: fewer attributed accesses than parsed files -- suspiciously")
        print("!! low.  Check the this-base identification before trusting this.")
        print("=" * W)

    banner = bannered_classes(repo)
    got = set(doc["classes"])
    ren = d.get("class_renames_followed") or {}
    if ren:
        print("  class renames replayed (name at commit -> name at HEAD):")
        for a, b in sorted(ren.items()):
            print("      %-30s -> %s" % (a, b))
    print("  classes with evidence               %8d" % d["classes_with_evidence"])
    print("  distinct offsets recovered          %8d" % d["offsets_recovered"])
    if banner:
        hit = banner & got
        print("  bannered headers in include/        %8d" % len(banner))
        print("  ...of those, with evidence          %8d   %s" %
              (len(hit), _pct(len(hit), len(banner))))
        miss = sorted(banner - got)
        if miss:
            print("  ...with NO evidence (%d): %s%s" %
                  (len(miss), ", ".join(miss[:12]), " ..." if len(miss) > 12 else ""))
        extra = sorted(got - banner)
        print("  classes with evidence but no bannered header: %d" % len(extra))
    print("=" * W)


def _pct(a, b):
    return "(%5.1f%%)" % (100.0 * a / b) if b else "(  n/a)"


def print_class(doc, name, repo):
    cls = doc["classes"].get(name)
    if cls is None:
        print("no evidence for class %r" % name)
        near = [c for c in doc["classes"] if name.lower() in c.lower()]
        if near:
            print("did you mean: %s" % ", ".join(sorted(near)[:10]))
        return
    print("=" * 78)
    print("EVIDENCE  class %s   (%d offsets)" % (name, len(cls)))
    print("=" * 78)
    print("  %-8s %-22s %-10s %-6s %s" % ("offset", "widths(count)", "signed", "addr", "provenance"))
    for off, rec in cls.items():
        w = ",".join("%s:%d" % (k, v) for k, v in rec["widths"].items()) or "-"
        s = "s%d/u%d" % (rec["signed"]["s"], rec["signed"]["u"])
        print("  %-8s %-22s %-10s %-6d %s" %
              (off, w, s, rec["address_only"], rec["provenance"][0] if rec["provenance"] else ""))
    hdr = os.path.join(repo, "include", name + ".h")
    if os.path.exists(hdr):
        print("-" * 78)
        print("committed header %s:" % hdr)
        with open(hdr, "r", encoding="utf-8", errors="replace") as fh:
            for ln in fh:
                print("  " + ln.rstrip())
    print("=" * 78)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--repo", default=os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    ap.add_argument("--commit", default=DEFAULT_COMMITISH,
                    help="commit-ish holding the pre-migration tree (default 5ddf7d2d~1)")
    ap.add_argument("--out", default=None, help="output JSON (default build/evidence_history.json)")
    ap.add_argument("--report", action="store_true", help="print the recall report")
    ap.add_argument("--class", dest="klass", default=None,
                    help="dump one class's evidence next to its committed header")
    ap.add_argument("--no-write", action="store_true", help="do not write the JSON")
    ap.add_argument("--no-follow-renames", action="store_true",
                    help="attribute by the class names the tree had at --commit, "
                         "without replaying later include/<Class>.h renames")
    args = ap.parse_args(argv)

    repo = os.path.abspath(args.repo)
    out = args.out or os.path.join(repo, DEFAULT_OUT)

    sha, classes, recall = build(repo, args.commit, not args.no_follow_renames)
    doc = to_json(sha, classes, recall)

    if not args.no_write:
        os.makedirs(os.path.dirname(os.path.abspath(out)), exist_ok=True)
        with open(out, "w", encoding="utf-8", newline="\n") as fh:
            json.dump(doc, fh, indent=2)
            fh.write("\n")

    if args.report or args.klass:
        if args.report:
            print_report(doc, repo)
        if args.klass:
            print_class(doc, args.klass, repo)
    else:
        print("wrote %s: %d classes, %d offsets, %d/%d accesses attributed "
              "(run --report for full recall)" %
              (out, doc["meta"]["detail"]["classes_with_evidence"],
               doc["meta"]["detail"]["offsets_recovered"],
               recall.accesses_attributed, recall.accesses_seen))
        if recall.accesses_attributed == 0:
            # No invocation of this tool may report a bare zero (plan section 6).
            print("!! ZERO accesses attributed -- matched nothing, which is not the "
                  "same as there being nothing to match.  Run --report.")
            return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
