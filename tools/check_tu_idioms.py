"""Count the gap between a merged TU's bytes and the goal it is merging toward.

WHY THIS EXISTS
---------------
The byte gate asks one question of a src_tu/ translation unit: do these bytes
match the ROM? It cannot ask the other question -- the one the reconstruction
is actually for, stated verbatim in the minigame conversion goal: "convert the
minigame code to period accurate idiomatic, readable, c++ code the original
EAD team would have written." A TU can be byte-identical and still be nothing
but a shadow of the class shape: the two banked pilot 2 TUs are roughly 80%
extern "C" free functions doing `*(int *)(c + 0x51b8)` arithmetic over char*
params -- byte-true, and the opposite of idiomatic -- and every existing gate
passed them green (notes/tu-reconstruction-pilot2-report.md sec 6.1, gap 1).

The failure that made this file necessary was smaller and sharper. During
pilot 2 a TU shipped with a comment claiming a function was "the deleting
destructor, vtable slot 17" when tools/rtti_vtables.py proved it was slot 18
-- a mode-dispatched event handler. The byte gate went green: a comment
compiles to nothing. tools/check_src_tu.py went green: every symbol the
comment names still resolves. NOTHING WENT RED. Only outside-perspective
review caught it.

So this tool scores the goal axis the byte gate provably does not cover: how
much of each TU is still shadow (extern "C" free functions, raw-offset member
access), how much is named members, and whether what the manifest CLAIMS about
a TU's idioms matches what the tree MEASURES. It makes the silent cases
impossible: a manifest claim the code does not back, and a nonzero shadow
census nobody ever had to write a sentence about.

CHECKS
------
1. shadow census -- per TU, three counts over its source file:
     memberDefs  definitions inside a class body or with a `Class::name`
                 head (the idiomatic shape),
     externC     definitions with C linkage: an extern "C" prefix, a body
                 inside an `extern "C" { }` block, a name previously declared
                 extern "C" (a plain file-scope definition inherits the C
                 linkage -- check 3's symbol groups prove this matters), or a
                 `.c` source file, where every non-static function has C
                 linkage by language;
     free        file-scope definitions that are neither (genuine C++ free
                 functions);
   and rawOffsets: hex pointer arithmetic on a member base -- `c + 0x51b8`,
   folded chains `c + 0x5000 + 0x1b8` (summed, so the census reads the offset
   the code means), `p[0x2c]`, `) + 0x51ca` -- counted INSIDE function bodies
   only, so `u8 pad_4f38[0x1a];` declarations and `extern int data_x[8];`
   arrays never count. A large number here is a BASELINE, not a failure: the
   census is the conversion worklist, and the two pilot TUs are expected to
   carry large nonzero counts.
2. named-member coverage -- per TU whose class has a header
   (include/<ClassName>.h, name taken from the source stem), the fraction of
   member accesses spelled through header member names (`->name`, `.name`,
   bare `name` inside member functions) vs raw offsets, and the raw offsets
   still in the code as a worklist (each tagged with the header member at
   that offset when one exists). A TU whose class has no header reports
   `no-header`, honestly, rather than a zero that would read as "converted".
3. cross-TU extern-signature consistency -- every extern declaration across
   src_tu/ (and promoted TU sources) grouped by symbol name; TUs that declare
   the same symbol with different parameter spellings are reported together.
   This is live right now: _Z14ApproachLinearRiii is `int&` in one TU and
   `int *` in another, RandomIntInternal is `int*` vs `void*` -- both
   byte-true, because C linkage does not mangle parameter types. The family
   decision landed 2026-08-30 (notes/minigame-family-decisions.md,
   ADJUDICATION D3): the ratified spellings live in CANONICAL_SIGNATURES
   below, and the mangled-name identifiers that decision RETIRED as
   declaration names live in RATIFIED_TRUE_NAMES beside it. Census mode (no
   ids) stays report-only. GATED mode fails a TU that (a) declares a
   canonical symbol any way but its ratified (return, params) spelling, or
   (b) declares a retired mangled-name identifier at all -- the ratified
   form for that family is the true-name declaration, so the mangled name
   is wrong regardless of spelling, and regardless of whether any split
   exists to surface it.
4. manifest goal field -- an entry may carry an optional `goal` block:
     "goal": {"idiomCensus": {"externC": 14, "rawOffsets": 12},
              "justification": "the sentence that says why this is okay"}
   (notes/translation-unit-reconstruction-plan.md sec 6; optional and
   additive, schema_version unchanged). In gated mode (TU ids given, or
   --strict) the census is checked against it:
     - census disagrees with the claimed numbers -> FAIL, stale claim (the
       vtable-slot-17 class of error: prose the code does not back);
     - nonzero census and no justification -> FAIL, unjustified shadow;
     - nonzero census with a matching claim and a justification -> PASS,
       with the justification echoed so the reviewer reads it next to the
       numbers;
     - genuinely zero census -> PASS on the numbers alone.
   This makes "zero-or-justified" the promotion prerequisite: --strict
   cannot pass a TU that is still shadow and says nothing about it.

WHAT IT DOES NOT CHECK
----------------------
Offset CORRECTNESS. Whether `c + 0x51b8` names the field the ROM touches is
the byte gate's question and this tool never answers it; the two axes can
disagree in both directions -- a byte-true TU can be 100% shadow, and a
beautifully idiomatic TU can still miss bytes on one wrong constant. Counts
are also deliberately narrow: decimal indexing (`p[0]` writing a vptr), plain
dereferences without hex (`*(int*)p = (int)&_ZTV...`), and `- 0x` arithmetic
are not counted; named access is type-blind, so `((Obj *)x)->unk_51b8` counts
as named when unk_51b8 is a header member even though the object is reached
through a shadow type (the `x + 0x..` inside the cast is still counted raw).
Check 3 groups by the declared name, so the same symbol spelled
namespace-qualified in one TU (`GXS::LoadOBJPltt`) and mangled in another
(`_ZN3GXS11LoadOBJPlttEPKvjj`) is not grouped, and function-pointer parameter
names stay in the comparison key. All of these are undercounts or loose
joins in a REPORT-ONLY direction; none of them can turn red into green.

AN EMPTY CHECK IS NOT A PASS
----------------------------
A gate that finds nothing to do and reports success is indistinguishable
from a gate that is working (see tools/check_src_tu.py). This one FAILS when
the manifest has no entries, when no entry has a censusable source file, when
the whole tree yields zero function definitions, and -- in gated mode -- when
an id names no manifest entry, an entry's source file is missing, or a gated
TU parses to zero functions.

Usage:
  python tools/check_tu_idioms.py                     # census of every entry
  python tools/check_tu_idioms.py ov006/dScMgBSC_c+dScMgBSC_c   # census + gate it
  python tools/check_tu_idioms.py --strict            # gate the whole tree
  python tools/check_tu_idioms.py --json [out.json]   # machine-readable report
  python tools/check_tu_idioms.py --manifest PATH     # a scratch copy
  python tools/check_tu_idioms.py --root PATH         # a scratch tree

Exit 0 in census mode (unless the work list is empty), 1 in gated mode when
any gated TU fails, and 1 always when there is nothing to check.
"""
from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import tu_manifest  # noqa: E402  (sibling module, house pattern)
from check_src_tu import line_of, strip_comments_and_strings  # noqa: E402

DEFAULT_MANIFEST = REPO / "config" / "tu_manifest.d"

# Ratified canonical spellings for check 3, keyed by declared symbol name and
# valued by the (return, params) key this tool computes -- the return type is
# part of the key because C linkage discards it too, and a TU that spells
# `void RandomIntInternal(int*)` against a ratified `int` is exactly the
# drift this table exists to catch. From the adjudicated family decision D3
# (notes/minigame-family-decisions.md, 2026-08-30): RandomIntInternal is
# extern "C" with the definition's signature (src/RandomIntInternal.c
# returns int, takes int*). The ApproachLinear family is NOT here: its
# ratified form is the true-name OVERLOADED declaration (two spellings share
# one symbol name per pair), which a name-keyed table cannot express -- those
# live in RATIFIED_TRUE_NAMES below as retired mangled identifiers.
CANONICAL_SIGNATURES: dict = {
    "RandomIntInternal": ("int", "int*"),
}

# Mangled-name identifiers D3 RETIRED as declaration names, mapped to the
# ratified true-name form that replaces them. mwccarm mangles that form to
# the exact ROM symbol, so declaring the mangled identifier itself -- in any
# linkage, any spelling -- is the shape the decision replaced; and in C++
# linkage it is the measured phantom double-mangling hazard
# (`_Z22_Z14ApproachLinearRiiiPiii`), invisible to text-verify and fatal at
# link. Not a signature table: ANY declaration of these names fails a gated
# TU, which is why a symbol with no split still gets caught here when the
# signature report would never show it.
RATIFIED_TRUE_NAMES: dict = {
    "_Z14ApproachLinearRiii": "int ApproachLinear(int&, int, int)",
    "_Z14ApproachLinearRsss": "int ApproachLinear(s16&, s16, s16)",
    "_Z15ApproachLinear2Riii": "int ApproachLinear2(int&, int, int)",
    "_Z15ApproachLinear2Rsss": "int ApproachLinear2(s16&, s16, s16)",
}

HEX_RE = re.compile(r"0[xX][0-9a-fA-F]+")
# `extern "C"` before stripping: strip_comments_and_strings blanks `"C"` like
# any string literal, and the linkage classification turns on exactly that
# token. Swap it for a word first (BOTH quotes -- leaving the closing one
# behind would start an unterminated string literal); display() swaps it
# back for the report.
C_LANG = "C_LINKAGE"
EXTERN_C_SOURCE_RE = re.compile(r'extern(\s*)"C(\+\+)?"')
EXTERN_C_RE = re.compile(rf'extern\s*{C_LANG}(\+\+)?')
LINKAGE_TAIL_RE = re.compile(rf'extern\s*{C_LANG}(\+\+)?\s*$')


def prepare(raw: str) -> str:
    """Strip comments and strings, keeping `extern "C"` recognizable."""
    prepared = EXTERN_C_SOURCE_RE.sub(
        lambda m: f"extern{m.group(1)}{C_LANG}{m.group(2) or ''}", raw)
    return _drop_preprocessor(strip_comments_and_strings(prepared))


def _drop_preprocessor(stripped: str) -> str:
    """Blank preprocessor lines (#include/#define/#pragma/...).

    A directive between a declaration's `;` and a definition's `{` glues
    itself into the head -- FlyGuy's `#define M(p) (p)` turns the next
    function head into `#define ...\nint func(char*)`, which classifies as
    SKIP and leaves every statement in that body looking like file scope.
    A macro's parentheses can also unbalance the paren counter. Directives
    carry no braces in this tree; backslash continuations go with them.
    Newlines are kept so line numbers still match the source.
    """
    lines = stripped.split("\n")
    cont = False
    out = []
    for line in lines:
        if cont or line.lstrip().startswith("#"):
            cont = line.rstrip().endswith("\\")
            out.append("")
        else:
            cont = False
            out.append(line)
    return "\n".join(out)


def display(text: str) -> str:
    return text.replace(f"{C_LANG}++", '"C++"').replace(C_LANG, '"C"')


# Raw member access: hex pointer arithmetic off an identifier (or a compound
# expression ending in `)`), with any number of chained `+ 0x..` folded into
# one summed offset, and hex array indexing. A bare `0x4000008` MMIO literal
# never matches (no `+`, no base identifier), and decimal constants never
# match -- by design: the census wants member offsets, not every literal.
CHAIN_RE = re.compile(r"([A-Za-z_]\w*|\))((?:\s*\+\s*0[xX][0-9a-fA-F]+\s*)+)")
INDEX_RE = re.compile(r"([A-Za-z_]\w*)\s*\[\s*(0[xX][0-9a-fA-F]+)\s*\]")

ARROW_RE = re.compile(r"->\s*([A-Za-z_]\w*)")
DOT_RE = re.compile(r"\.\s*([A-Za-z_]\w*)")
BARE_RE = re.compile(r"(?<!->)(?<![.:])\b([A-Za-z_]\w*)\b")

# Frame kinds for the brace scanner. `skip` covers everything that is not a
# linkage block, a class/namespace body, or a function definition: control
# flow, enum bodies, initializer lists. Misfiling one of those as FUNC would
# inflate the census, so the classifier starts from the FIRST WORD of a head,
# which is the one token that cannot lie in this tree's sources.
KIND_LINKAGE, KIND_CLASS, KIND_NAMESPACE, KIND_FUNC, KIND_SKIP = (
    "linkage", "class", "namespace", "func", "skip")
CONTROL_WORDS = {"if", "else", "for", "while", "switch", "do", "catch",
                 "case", "default", "try"}
CLASS_HEAD_RE = re.compile(r"(?:typedef\s+)*(?:struct|class|union)\b")
NAMESPACE_HEAD_RE = re.compile(r"namespace\b")

TYPE_WORDS = {"void", "char", "short", "int", "long", "float", "double",
              "signed", "unsigned", "bool", "const", "volatile", "struct",
              "class", "union", "enum", "u8", "s8", "u16", "s16", "u32", "s32",
              "u64", "s64"}


def classify_head(head: str, enclosing: tuple) -> str:
    """Classify the text leading into a `{` so only definitions are counted.

    The text is already stripped of comments and strings; it runs from the
    previous `;`, `{` or `}` (ignoring semicolons inside parentheses, so a
    `for (i = 0; i < n; i++) {` head stays one segment) up to the brace.
    A brace inside a function body is never a function definition -- C has no
    nested functions -- which is what keeps a `label: if (...) {` body (BSC
    spells one `do1:`) out of the census.
    """
    head = head.strip()
    if not head or head.endswith("=") or head.startswith("#"):
        return KIND_SKIP
    if KIND_FUNC in enclosing:
        return KIND_SKIP  # C has no nested functions; a body brace is flow
    first = re.match(r"[A-Za-z_]\w*", head)
    if first and first.group(0) in CONTROL_WORDS:
        return KIND_SKIP
    if LINKAGE_TAIL_RE.search(head):
        return KIND_LINKAGE
    if NAMESPACE_HEAD_RE.match(head):
        return KIND_NAMESPACE
    if re.search(r"\benum\b", head):
        return KIND_SKIP
    if CLASS_HEAD_RE.match(head):
        return KIND_CLASS
    # The only remaining head that can precede a function body: a parameter
    # list closed just before the brace. Control flow was already caught by
    # its first word above.
    if head.endswith(")"):
        return KIND_FUNC
    return KIND_SKIP


def scan_frames(stripped: str):
    """One pass over the stripped text.

    Returns (frames, segments). `frames` is every balanced brace region with
    its kind, its enclosing kinds (bottom to top), its head text and span --
    the function bodies the census scans. `segments` is every semicolon-
    terminated file-scope declaration candidate: text, position, enclosing
    kinds. Segments inside class and function bodies are returned too but
    carry their enclosing kinds so callers can drop them.
    """
    frames, segments = [], []
    stack = []  # [kind, start, head]
    seg_start = 0
    paren = 0
    for i, ch in enumerate(stripped):
        if ch == "(":
            paren += 1
        elif ch == ")":
            paren = max(0, paren - 1)
        elif ch == "{":
            head = stripped[seg_start:i]
            kind = classify_head(head, tuple(s[0] for s in stack))
            frames_open = tuple(s[0] for s in stack)
            stack.append([kind, i, head])
            segments.append((head, seg_start, frames_open + (kind,), True))
            seg_start = i + 1
            paren = 0
        elif ch == "}":
            if stack:
                kind, start, head = stack.pop()
                frames.append({"kind": kind, "start": start, "end": i,
                               "head": head,
                               "enclosing": tuple(s[0] for s in stack)})
            seg_start = i + 1
            paren = 0
        elif ch == ";" and paren == 0:
            enclosing = tuple(s[0] for s in stack)
            segments.append((stripped[seg_start:i], seg_start, enclosing, False))
            seg_start = i + 1
    return frames, segments


def split_def_head(head: str):
    """Split a definition head into (namepart, params, name).

    `head` ends with `)`. The parameter list is found by matching the final
    paren backwards, so nested parens in a parameter list stay intact; the
    name is the identifier immediately before that open paren.
    """
    depth = 0
    for j in range(len(head) - 1, -1, -1):
        if head[j] == ")":
            depth += 1
        elif head[j] == "(":
            depth -= 1
            if depth == 0:
                params = head[j + 1:len(head) - 1]
                namepart = head[:j].rstrip()
                m = re.search(r"([A-Za-z_]\w*)\s*$", namepart)
                return namepart, params, (m.group(1) if m else "?")
    return head, "", "?"


def normalize_params(params: str) -> str:
    """Fold a parameter list to a spelling that ignores parameter NAMES.

    `int *x` and `int *t` fold to the same key; `int& v` and `int *x` do not.
    Parameter names are the one difference that never reaches the linker
    under C linkage -- which is exactly why TUs drifted apart.
    """
    out = []
    depth = 0
    cur = []
    parts = []
    for ch in params:
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    parts.append("".join(cur))
    for p in parts:
        p = p.strip()
        if not p:
            continue
        m = re.match(r"^(.*?)([A-Za-z_]\w*)\s*$", p)
        if m and m.group(1).strip():
            # `int *x` -> `int *`; `int& v` -> `int&`. Keep `unsigned int`
            # whole: the trailing word there is a type word, not a name.
            if m.group(2) not in TYPE_WORDS:
                p = m.group(1).rstrip()
        out.append(re.sub(r"\s+", "", p))
    return ",".join(out)


class Decl:
    __slots__ = ("name", "params", "ret", "linkage", "text", "line", "is_data")

    def __init__(self, name, params, ret, linkage, text, line, is_data):
        self.name, self.params, self.ret = name, params, ret
        self.linkage, self.text, self.line = linkage, text, line
        self.is_data = is_data

    @property
    def key(self):
        """Normalized spelling, for grouping TUs that disagree."""
        if self.is_data:
            return re.sub(r"\s+", "", self.ret) + ("[]" if "[" in self.text else "")
        return (re.sub(r"\s+", "", self.ret), self.params)


def _has_top_level_char(text: str, ch: str) -> bool:
    """True if `ch` appears in `text` at parenthesis depth 0."""
    depth = 0
    for c in text:
        if c in "(<[":
            depth += 1
        elif c in ")>]":
            depth -= 1
        elif c == ch and depth == 0:
            return True
    return False


def collect_declarations(stripped: str, segments, path_is_c: bool):
    """Every extern declaration in the file, plus its linkage.

    Scope: file scope and namespace/linkage blocks -- never inside a class
    (member declarations) or a function body (statements), never a typedef,
    never a `static`. Inside an `extern "C" { }` block every declaration is
    C-linked whether or not it spells `extern`, so bare prototypes there count
    too; at file scope a bare prototype still counts (it has external
    linkage), with C++ linkage unless it spells extern "C" or the file is C.
    """
    decls = []
    for text, pos, enclosing, is_brace_head in segments:
        if is_brace_head:
            continue  # a `{` head is a definition/open block, not a declaration
        if KIND_CLASS in enclosing or KIND_FUNC in enclosing:
            continue
        text = text.strip()
        if not text or text.startswith("#") or text.startswith("typedef"):
            continue
        # A top-level `=` makes it a definition with an initializer
        # (`int i = GetGameLanguage();` -- the sinit shape several TUs carry),
        # not a declaration of the called name.
        if _has_top_level_char(text, "="):
            continue
        if re.search(r"\bstatic\b", text):
            continue
        is_extern = bool(re.search(r"\bextern\b", text))
        looks_proto = text.endswith(")") and "(" in text
        in_linkage = KIND_LINKAGE in enclosing
        if not (is_extern or looks_proto):
            continue
        linkage = "cpp"
        if EXTERN_C_RE.search(text) or in_linkage or path_is_c:
            linkage = "c"
        # Name: the identifier right before the first top-level `(`, or the
        # last identifier for data declarations (`extern int data_x[8];`).
        i = text.find("(")
        if i < 0:
            idents = re.findall(r"[A-Za-z_]\w*", text)
            if not idents or "extern" not in text:
                continue
            name, ret, is_data = idents[-1], text, True
            ret = re.sub(rf"{EXTERN_C_RE.pattern}", "", ret)
            ret = re.sub(r"\bextern\b", "", ret)
        else:
            m = re.match(r"^(.*?)([A-Za-z_]\w*)\s*\($", text[:i + 1], re.S)
            if not m or m.group(2) in TYPE_WORDS:
                # `void (*dtor)(void)` -- a function-pointer declaration, not
                # a named symbol declaration; there is nothing to group.
                continue
            name, is_data = m.group(2), False
            # Parameter text: the balanced span after that paren.
            depth, end = 1, i + 1
            while end < len(text) and depth:
                if text[end] == "(":
                    depth += 1
                elif text[end] == ")":
                    depth -= 1
                end += 1
            params = normalize_params(text[i + 1:end - 1] if end > i + 1 else "")
            ret = re.sub(EXTERN_C_RE.pattern, "", text[:m.start(2)])
            ret = re.sub(r"\bextern\b", "", ret)
        decls.append(Decl(name, params if not is_data else "",
                          ret, linkage, text,
                          line_of(stripped, pos), is_data))
    return decls


def census_functions(stripped: str, frames, extern_c_names, path_is_c):
    """Classify every function definition in the file.

    Returns a list of dicts: name, kind (member/externC/free), body span, and
    the raw-offset sites found inside the body.
    """
    funcs = []
    for fr in frames:
        if fr["kind"] != KIND_FUNC:
            continue
        head = fr["head"]
        if head.strip().startswith("#"):
            continue
        namepart, params, name = split_def_head(head.strip())
        is_member = ("::" in namepart
                     or KIND_CLASS in fr["enclosing"])
        is_static = bool(re.search(r"\bstatic\b", head))
        is_extern_c = (EXTERN_C_RE.search(head) is not None
                       or KIND_LINKAGE in fr["enclosing"]
                       or (path_is_c and not is_static)
                       or (not is_member and name in extern_c_names))
        if is_member:
            kind = "member"
        elif is_extern_c:
            kind = "externC"
        else:
            kind = "free"
        sites = scan_raw_offsets(stripped, fr)
        funcs.append({"name": name, "kind": kind, "line": line_of(stripped, fr["start"]),
                      "sites": sites})
    return funcs


def scan_raw_offsets(stripped: str, frame):
    """Hex pointer arithmetic inside one function body.

    Only the body span is scanned, which is what keeps member declarations
    (`u8 pad_4f38[0x1a];`) and extern arrays (`extern int data_x[8];`) out of
    the count: they live outside function bodies. Chains fold, so
    `c + 0x5000 + 0x1b8` reports 0x51b8 -- the offset the code MEANS.
    """
    body = stripped[frame["start"] + 1:frame["end"]]
    base_line = line_of(stripped, frame["start"] + 1)
    sites = []
    for m in CHAIN_RE.finditer(body):
        base = m.group(1)
        # `s * 0x32000 + 0x800` (the fixed-point rounding idiom, ~30 sites in
        # this tree) matches at the `x32000` tail of the hex literal: the
        # identifier group cannot START on a digit, so the regex engine retries
        # one character in. A match that begins mid-token is not member
        # arithmetic, so skip it when the previous character is still part of
        # the same token. A `)` base is exempt: `(char *)c + 0x51ca` is always
        # a real compound expression, and the character before the `)` is
        # whatever that expression ended in.
        if base != ")" and m.start() > 0 and (
                body[m.start() - 1].isalnum() or body[m.start() - 1] == "_"):
            continue
        offset = sum(int(h, 16) for h in HEX_RE.findall(m.group(2)))
        sites.append((offset, base_line + body.count("\n", 0, m.start())))
    for m in INDEX_RE.finditer(body):
        offset = int(m.group(2), 16)
        sites.append((offset, base_line + body.count("\n", 0, m.start())))
    return sites


# ---------------------------------------------------------------------------
# Header member maps (check 2)
# ---------------------------------------------------------------------------

_MEMBER_NAME_RE = re.compile(r"([A-Za-z_]\w*)\s*(?:\[[^\]]*\])?\s*;")
_METHOD_NAME_RE = re.compile(r"([A-Za-z_]\w*)\s*\(")
_OFFSET_COMMENT_RE = re.compile(r"/\*.*?(?:offset\s+)?(0[xX][0-9a-fA-F]+)", re.S)
_NAME_ENCODED_RE = re.compile(r"\b(?:unk|pad)_([0-9a-fA-F]+)\b")
_INCLUDE_RE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*"([^"]+)"', re.M)


def header_member_map(root: pathlib.Path, class_name: str, _cache={}):
    """Member names (and offsets) reachable from include/<class_name>.h.

    The map follows the header's whole `#include` chain, because a TU spells
    base-class members too (`self->mHudScore` lives in dScMgBase_c.h, three
    levels up from dScMgBSC_c.h). Members carry their offset either in a
    trailing `/* 0xNN */` comment (this tree's convention -- also spelled
    `/* offset 0xNN */` in dScMgSingle3DBase_c.h) or in an unk_/pad_ name.
    Method declarations join the map without an offset: they are legal named
    access by arrow/dot but never counted as bare-name hits, which is what
    keeps a bare local identifier named like a method from inflating
    coverage.
    """
    if class_name in _cache:
        return _cache[class_name]
    by_name, by_offset = {}, {}
    seen = set()
    queue = [root / "include" / (class_name + ".h")]
    while queue:
        path = queue.pop()
        key = str(path)
        if key in seen:
            continue
        seen.add(key)
        if not path.is_file():
            continue
        raw = path.read_text(encoding="utf-8", errors="replace")
        stripped = prepare(raw)
        queue.extend(path.resolve().parent / inc
                     for inc in _INCLUDE_RE.findall(raw))
        queue.extend(root / "include" / inc
                     for inc in _INCLUDE_RE.findall(raw))
        # A light class-body tracker: member declarations are lines inside a
        # struct/class body with a `;` and no braces (enum bodies, inline
        # definitions) and no `=` (default arguments). Parens make it a
        # METHOD declaration -- named access by arrow/dot, but never a
        # bare-name hit, so it joins the map without an offset.
        depth = 0
        for lineno, (raw_line, line) in enumerate(zip(raw.splitlines(),
                                                      stripped.splitlines()), 1):
            opens = line.count("{")
            closes = line.count("}")
            if depth > 0 and ";" in line and "{" not in line \
                    and "}" not in line and "=" not in line:
                if "(" in line:
                    mm = _METHOD_NAME_RE.search(line)
                    if mm:
                        by_name.setdefault(mm.group(1), None)
                    depth += opens - closes
                    continue
                m = _MEMBER_NAME_RE.search(line)
                if m:
                    name = m.group(1)
                    om = _OFFSET_COMMENT_RE.search(raw_line)
                    if not om:
                        om = _NAME_ENCODED_RE.search(name)
                        offset = int(om.group(1), 16) if om else None
                    else:
                        offset = int(om.group(1), 16)
                    by_name.setdefault(name, offset)
                    if offset is not None:
                        by_offset.setdefault(offset, name)
            depth += opens - closes
    _cache[class_name] = (by_name, by_offset)
    return by_name, by_offset


def find_class_header(root: pathlib.Path, source_stem: str):
    """The TU's class header: first `+`-separated stem segment with one."""
    for cand in dict.fromkeys(source_stem.split("+")):
        if (root / "include" / (cand + ".h")).is_file():
            return cand
    return None


# ---------------------------------------------------------------------------
# Per-TU census
# ---------------------------------------------------------------------------

def census_tu(root: pathlib.Path, entry: dict):
    """Census one manifest entry. Returns the per-TU record; never raises."""
    rec = {"id": entry.get("id", "?"),
           "source": entry.get("source", ""),
           "missing": False}
    path = root / entry["source"]
    if not path.is_file():
        rec.update({"missing": True, "functions": 0, "memberDefs": 0,
                    "externC": 0, "freeDefs": 0, "rawOffsets": 0,
                    "rawSites": [], "coverage": {"header": None},
                    "funcList": []})
        return rec
    raw = path.read_text(encoding="utf-8", errors="replace")
    stripped = prepare(raw)
    frames, segments = scan_frames(stripped)
    path_is_c = path.suffix == ".c"
    decls = collect_declarations(stripped, segments, path_is_c)
    extern_c_names = {d.name for d in decls if d.linkage == "c"}
    funcs = census_functions(stripped, frames, extern_c_names, path_is_c)

    sites = []
    for f in funcs:
        sites.extend(f["sites"])
    rec["functions"] = len(funcs)
    rec["memberDefs"] = sum(1 for f in funcs if f["kind"] == "member")
    rec["externC"] = sum(1 for f in funcs if f["kind"] == "externC")
    rec["freeDefs"] = sum(1 for f in funcs if f["kind"] == "free")
    rec["rawOffsets"] = len(sites)
    rec["rawSites"] = sorted(
        ({"offset": hex(off), "line": line} for off, line in sites),
        key=lambda s: (int(s["offset"], 16), s["line"]))

    stem = pathlib.PurePosixPath(entry["source"]).stem
    header = find_class_header(root, stem)
    coverage = {"header": header}
    if header:
        by_name, by_offset = header_member_map(root, header)
        named = 0
        for fr in frames:
            if fr["kind"] != KIND_FUNC:
                continue
            body = stripped[fr["start"] + 1:fr["end"]]
            for m in ARROW_RE.finditer(body):
                if m.group(1) in by_name:
                    named += 1
            for m in DOT_RE.finditer(body):
                if m.group(1) in by_name:
                    named += 1
            for m in BARE_RE.finditer(body):
                n = m.group(1)
                # A call, not a field access; and only offset-bearing field
                # names count bare, so a method name used as a bare
                # identifier cannot inflate coverage.
                if n not in by_name or by_name[n] is None:
                    continue
                if re.match(r"\s*\(", body[m.end():]):
                    continue
                # `-> name` / `. name` / `:: name` with a space were already
                # counted above; the lookbehind cannot see past the space.
                if body[:m.start()].rstrip().endswith(("->", ".", "::")):
                    continue
                named += 1
        coverage["named"] = named
        coverage["raw"] = rec["rawOffsets"]
        denom = named + rec["rawOffsets"]
        coverage["fraction"] = (round(named / denom, 3) if denom else None)
        worklist = {}
        for off, _line in sites:
            worklist.setdefault(off, 0)
            worklist[off] += 1
        coverage["worklist"] = [
            {"offset": hex(off), "member": by_offset.get(off), "count": n}
            for off, n in sorted(worklist.items())]
    rec["coverage"] = coverage
    rec["funcList"] = [{"name": f["name"], "kind": f["kind"], "line": f["line"]}
                       for f in funcs]
    rec["_decls"] = decls
    return rec


# ---------------------------------------------------------------------------
# Check 3: cross-TU extern-signature consistency
# ---------------------------------------------------------------------------

def signature_groups(census):
    """Group every collected declaration by symbol name across TUs.

    Variants are keyed by the NORMALIZED spelling only -- two TUs writing
    `extern  void` and `extern void` (or `int a, int b` and `int x, int y`)
    are the same spelling and must not be reported as a split.
    """
    groups = {}
    for rec in census:
        for d in rec.get("_decls", []):
            g = groups.setdefault(d.name, {})
            if d.key not in g:
                g[d.key] = {"spelling": display(d.text), "tus": []}
            g[d.key]["tus"].append(rec["id"])
    return groups


def report_signature_groups(groups):
    """Report only symbols declared with more than one spelling."""
    out = []
    for name in sorted(groups):
        variants = groups[name]
        if len(variants) < 2:
            continue
        canonical = CANONICAL_SIGNATURES.get(name)
        entry = {"name": name, "canonical": canonical,
                 "matchesCanonical": None, "variants": []}
        for key in sorted(variants, key=str):
            v = variants[key]
            entry["variants"].append({
                "key": list(key) if isinstance(key, tuple) else key,
                "spelling": v["spelling"],
                "tus": sorted(set(v["tus"]))})
        if canonical is not None:
            # Full key: the return type is part of the ratified spelling, and
            # a tuple-valued CANONICAL_SIGNATURES entry never matches the old
            # params-only compare -- which is exactly the drift it now catches.
            entry["matchesCanonical"] = any(
                isinstance(k, list) and k == list(canonical)
                for k in (v["key"] for v in entry["variants"]))
        out.append(entry)
    return out


def gate_signatures(rec):
    """Ratified-spelling violations for one TU (adjudicated D3). Two shapes:

      - a declaration of a mangled-name identifier D3 retired: any spelling,
        any linkage. The ratified form is the true-name declaration, so the
        mangled name is wrong regardless of how it is spelled -- and a
        retired name with no split never reaches the signature groups, so
        this gate is the only place it gets caught;
      - a declaration of a canonical symbol whose (return, params) key is
        not the ratified one.

    Data declarations are skipped: their key is a type string, not a
    signature. Returns a list of failure strings."""
    out = []
    for d in rec.get("_decls", []):
        if d.is_data:
            continue
        true_name = RATIFIED_TRUE_NAMES.get(d.name)
        if true_name is not None:
            out.append(f"declares retired mangled-name identifier {d.name} "
                       f"(line {d.line}); ratified form: {true_name}")
            continue
        canonical = CANONICAL_SIGNATURES.get(d.name)
        if canonical is not None and d.key != canonical:
            params = canonical[1].replace(",", ", ")
            out.append(f"{d.name} declared as {display(d.text)} (line {d.line}); "
                       f"ratified spelling: {canonical[0]} {d.name}({params})")
    return out


def retired_declarations(census):
    """{retired name: sorted TU ids} over the censused tree, for the report.

    Retired-name declarations live outside the split machinery: a single-TU
    declaration produces no signature group at all, so report mode has to
    surface them here or they would be invisible until someone gates."""
    out = {}
    for rec in census:
        for d in rec.get("_decls", []):
            if d.name in RATIFIED_TRUE_NAMES:
                out.setdefault(d.name, []).append(rec["id"])
    return {k: sorted(set(v)) for k, v in out.items()}


# ---------------------------------------------------------------------------
# Check 4: the manifest goal field
# ---------------------------------------------------------------------------

def gate_goal(entry, census_rec):
    """Apply the zero-or-justified rule to one entry. Returns a verdict dict."""
    goal = entry.get("goal")
    if not isinstance(goal, dict):
        goal = {}
    claimed = goal.get("idiomCensus")
    justification = goal.get("justification") or ""
    verdict = {"present": bool(goal), "justification": justification.strip() or None,
              "claimed": claimed, "verdict": "pass", "reason": ""}
    nonzero = census_rec["externC"] > 0 or census_rec["rawOffsets"] > 0
    if claimed is not None:
        if not isinstance(claimed, dict):
            verdict.update(verdict="fail",
                           reason="goal.idiomCensus is not an object")
            return verdict
        for field, actual in (("externC", census_rec["externC"]),
                              ("rawOffsets", census_rec["rawOffsets"])):
            want = claimed.get(field)
            if want is None:
                verdict.update(
                    verdict="fail",
                    reason=(f"goal.idiomCensus has no '{field}' number "
                            f"(expected {actual})"))
                return verdict
            if not isinstance(want, int) or isinstance(want, bool):
                verdict.update(verdict="fail",
                               reason=f"goal.idiomCensus.{field} is not an integer")
                return verdict
            if want != actual:
                verdict.update(
                    verdict="fail",
                    reason=(f"stale claim: manifest goal.idiomCensus.{field} "
                            f"says {want}, the code measures {actual}"))
                return verdict
    if nonzero and not verdict["justification"]:
        verdict["reason"] = (f"unjustified: census is nonzero "
                             f"(externC={census_rec['externC']}, "
                             f"rawOffsets={census_rec['rawOffsets']}) and the "
                             f"entry carries no goal.justification")
        verdict["verdict"] = "fail"
    elif verdict["verdict"] == "pass" and nonzero:
        verdict["reason"] = "zero-or-justified satisfied by goal.justification"
    elif verdict["verdict"] == "pass":
        verdict["reason"] = "genuinely zero census"
    return verdict


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def build_parser():
    ap = argparse.ArgumentParser(
        description="Census shadow idioms in merged translation units "
                    "and gate them against the manifest's goal field.")
    ap.add_argument("tu_ids", nargs="*",
                    help="TU ids to census and gate (default: all, report only)")
    ap.add_argument("--strict", action="store_true",
                    help="gate: fail on unjustified nonzero census (with no "
                         "ids, gate every entry)")
    ap.add_argument("--manifest", default=None,
                    help="manifest path: the directory, or a single .json "
                         "scratch copy (default config/tu_manifest.d)")
    ap.add_argument("--root", default=None,
                    help="tree root to census (default: this repository)")
    ap.add_argument("--json", nargs="?", const="-", default=None, metavar="PATH",
                    help="machine-readable report (to stdout with --json, or a file)")
    return ap


def main(argv=None):
    args = build_parser().parse_args(argv)
    root = pathlib.Path(args.root).resolve() if args.root else REPO
    # A --root scratch tree needs its own manifest unless told otherwise.
    manifest_root = (pathlib.Path(args.manifest) if args.manifest
                     else root / "config" / "tu_manifest.d")
    manifest = tu_manifest.load(manifest_root)
    entries = manifest.get("entries", [])
    by_id = {e["id"]: e for e in entries}

    gated = bool(args.tu_ids) or args.strict
    gate_ids = list(dict.fromkeys(args.tu_ids))
    if args.strict and not gate_ids:
        gate_ids = [e["id"] for e in entries]

    failures = []
    if not entries:
        failures.append(f"no manifest entries at {tu_manifest.describe(manifest_root)} "
                        "-- an empty check is not a pass")

    census = []
    for entry in entries:
        census.append(census_tu(root, entry))

    # Focus the run when ids are given: the census table shows those TUs, the
    # signature section shows splits they participate in. The full tree is
    # still censused -- check 3 is a comparison ACROSS TUs by construction.
    focus = set(gate_ids) if gate_ids else None
    shown = [r for r in census if focus is None or r["id"] in focus]
    for tu_id in gate_ids:
        if tu_id not in by_id:
            failures.append(f"unknown TU id {tu_id!r}: not in the manifest")

    total_functions = sum(r["functions"] for r in census)
    checked_sources = sum(1 for r in census if not r["missing"])
    if entries and checked_sources == 0:
        failures.append("no manifest entry has a censusable source file")
    if checked_sources and total_functions == 0:
        failures.append("census found no function definitions in the whole tree")

    # Goal gate (check 4).
    for tu_id in gate_ids:
        entry = by_id.get(tu_id)
        if entry is None:
            continue
        rec = next((r for r in census if r["id"] == tu_id), None)
        if rec is None:
            continue
        if rec["missing"]:
            failures.append(f"{tu_id}: source {rec['source']} does not exist")
            continue
        if rec["functions"] == 0:
            failures.append(f"{tu_id}: no function definitions parsed from "
                            f"{rec['source']} -- either the file is empty or the "
                            f"scanner is pointed wrong")
            continue
        verdict = gate_goal(entry, rec)
        rec["goal"] = verdict
        if verdict["verdict"] != "pass":
            failures.append(f"{tu_id}: {verdict['reason']}")
        # Check 3's gated half: the ratified D3 spellings. Runs after the
        # goal verdict so a TU can fail for both reasons in one run.
        for msg in gate_signatures(rec):
            failures.append(f"{tu_id}: {msg}")

    # Check 3, cross-TU by construction.
    groups = signature_groups(census)
    sig_report = report_signature_groups(groups)
    if focus is not None:
        sig_report = [g for g in sig_report
                      if any(tu in focus for v in g["variants"] for tu in v["tus"])]

    report = {
        "schemaVersion": 1,
        "ok": not failures,
        "gated": gated,
        "checked": len(census),
        "failed": len(failures),
        "failures": failures,
        "census": [{k: v for k, v in r.items() if k != "_decls"} for r in census],
        "shown": [r["id"] for r in shown],
        "signatures": sig_report,
        "canonicalRatified": sorted(CANONICAL_SIGNATURES),
        "retiredNames": sorted(RATIFIED_TRUE_NAMES),
        "retiredDeclarations": retired_declarations(census),
    }

    print_report(report, census, shown, sig_report, gated)

    if args.json and args.json != "-":
        out = pathlib.Path(args.json)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8",
                       newline="\n")
    elif args.json == "-":
        print(json.dumps(report, indent=2))

    return 1 if failures else 0


def print_report(report, census, shown, sig_report, gated):
    have_header = sum(1 for r in shown if r["coverage"]["header"])
    w = max((len(r["id"]) for r in shown), default=0)
    print(f"tu idioms census: {len(shown)} of {len(census)} entries "
          f"({have_header} with a class header)"
          + (", GATED" if gated else ", report only"))
    for r in shown:
        cov = r["coverage"]
        if r["missing"]:
            print(f"{r['id']:<{w}}  MISSING SOURCE {r['source']}")
            continue
        cover = ""
        if cov["header"]:
            frac = cov["fraction"]
            pct = f"{frac * 100:.0f}%" if frac is not None else "n/a"
            cover = (f"header={cov['header']}.h named={cov['named']} "
                    f"coverage={pct}")
        else:
            cover = "no-header"
        print(f"{r['id']:<{w}}  funcs={r['functions']:<3} member={r['memberDefs']:<3} "
              f"externC={r['externC']:<3} free={r['freeDefs']:<3} "
              f"raw={r['rawOffsets']:<4} {cover}")
    for r in shown:
        goal = r.get("goal")
        if gated and goal is not None:
            mark = "PASS" if goal["verdict"] == "pass" else "FAIL"
            print(f"goal {mark} {r['id']}: {goal['reason']}")
            if goal["justification"]:
                print(f"      justification: {goal['justification']}")
    rat = report.get("retiredDeclarations") or {}
    if rat:
        print(f"\nretired mangled-name declarations ({len(rat)} symbol(s), "
              f"adjudicated D3; FAIL when gated):")
        for name in sorted(rat):
            print(f"  {name}  ratified form: {RATIFIED_TRUE_NAMES[name]}")
            print(f"      {', '.join(rat[name])}")
    if sig_report:
        state = ("ratified spellings enforced when gated"
                 if CANONICAL_SIGNATURES or RATIFIED_TRUE_NAMES else
                 "WARN ONLY -- ratification pending")
        print(f"\ncross-TU extern signature splits ({len(sig_report)} symbol(s), "
              f"{state})")
        for g in sig_report:
            print(f"  {g['name']}  declared {len(g['variants'])} way(s):")
            for v in g["variants"]:
                tus = ", ".join(v["tus"])
                mark = ""
                if (g.get("canonical") is not None
                        and v["key"] == list(g["canonical"])):
                    mark = "  <- RATIFIED"
                print(f"    [{','.join(v['key']) if isinstance(v['key'], list) else v['key']}]"
                      f" {v['spelling']}{mark}")
                print(f"      {tus}")
    else:
        print("\ncross-TU extern signature splits: none")
    if report["failures"]:
        print(f"\nFAIL ({len(report['failures'])}):")
        for f in report["failures"]:
            print(f"  {f}")
    else:
        print("\nOK")


if __name__ == "__main__":
    sys.exit(main())
