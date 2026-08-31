#!/usr/bin/env python3
"""Type the object-extent `u8` markers in the 241 bannered headers.

Scope: the `marker: object, extent unknown` bucket of `tools/gen_header.py --report`
(180 fields at the time of writing).  Each is a lone `u8 name; /* 0xNN */` followed by
padding, where the `u8` is the generator's "an object of unknown type starts here"
marker -- NOT a width claim (notes/archive/plan-scalar-markers.md 1; misreading it as one
produced 229 false positives earlier in this programme).  Spans of 4 bytes or less
were already retyped as scalars; these are the ones with room for a real object.

TWO INDEPENDENT LINES OF EVIDENCE, deliberately kept apart
----------------------------------------------------------

(a) SIZE MATCH.  The placeholder name suggests a class (`mModelAnim` -> ModelAnim).
    Does `include/<That>.h` exist and is its asserted size equal to the marker's span?
    A NAME IS A HYPOTHESIS, NEVER EVIDENCE.  Size agreement alone is reported as
    `size-match only` and is never promoted to a decision.

(b) CALLEE TYPE.  The decisive one.  Somewhere the field's ADDRESS is taken and handed
    to something whose mangled name names the class:

      _ZN9ModelAnimD1Ev((char *)t + 0xd4)               EXACT   member destructor
      struct Amp : Actor { ModelAnim m0; /* 0xd4 */ }   EXACT   migrated file's layout
      _ZN9ModelBase7SetFileE...(this + 0xd4)            BOUND   a base of the real type
      _ZN5Enemy26...R12WithMeshClsnR9ModelAnimj(...)    BOUND   a typed reference param
      ((ModelBase *)((char *)&mModel))->SetFile(...)    BOUND   a cast

    Only the first two are EXACT.  A member subobject's own destructor is the class the
    compiler was told the member has, and the call is a relocation the byte gate checks.
    Everything else is a LOWER BOUND: `ModelBase::SetFile` on `this+0xd4` proves the
    object there IS-A ModelBase, and a `ModelAnim &` parameter binds a BlendModelAnim
    just as happily -- so neither names the declared type.

Two derived findings fall out of the same member map and are reported separately:

  * a marker that lands INSIDE an already-typed member is not an unknown object at all,
    it is the header naming an interior word of a member it already has;
  * a marker whose address goes to an UNNAMED function (`_ZN5dBgPiC2Ev`) stays
    undecided, but two markers handing their offsets to the SAME unnamed function have
    the same type -- three undecidables collapse to one.

Sources, both of them:
  * today's `src/` -- every enrolled file here reproduces the ROM byte for byte, so a
    mangled callee spelled in it is a relocation target the gate checks, not a guess;
  * the pre-migration tree at `5ddf7d2d~1`, read through `git cat-file --batch` with no
    checkout (same mechanism as tools/evidence_history.py).  That tree still spells
    member accesses as raw offsets and its D0/D1/D2 files are literally a table of
    member class -> offset.

TWO TRAPS THIS TOOL IS BUILT AROUND
-----------------------------------
1. `_ZN6Player9DropActorEv(*(void **)((char *)&unk_5e4))` does NOT say the object at
   0x5e4 is a Player.  It says the WORD there is a `Player *`.  An offset only counts
   when the whole argument IS the address -- `arg_address_offsets` enforces that, and
   a dereferenced one is recorded separately as pointer evidence, which is its own
   (useful) finding: a "large object" marker that is really a 4-byte pointer.
2. `_ZN5Actor15FindWithActorIDEjPS_(id, x)` passes `x` as the SECOND argument, so the
   callee's class says nothing about it.  Parameter types are decoded from the mangled
   name, and the argument count versus the parameter count is what decides whether
   argument 0 is a `this` at all.

Recall is reported at every stage: a pass that silently matches nothing prints a clean,
confident, wrong answer (notes/archive/plan-gen-header.md 2).

    python build/marker_types.py                       # report to stdout
    python build/marker_types.py --json build/marker_types.json
    python build/marker_types.py --cls Amp --verbose    # one class, all evidence
    python build/marker_types.py --no-history           # today's src only

Read-only.  Writes nothing except its optional --json output.
"""

from __future__ import annotations

import argparse
import collections
import json
import os
import pathlib
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(ROOT, "tools"))

import evidence_history as EH        # noqa: E402
import gen_header as GH              # noqa: E402

HIST = "5ddf7d2d~1"
REPORT = os.path.join(ROOT, "build", "gen_header_report.json")
MARKER_BUCKET = "marker: object, extent unknown"

# include/G2x.h describes hardware registers, not an object (issue #1148).
BOGUS_HEADERS = {"G2x"}


# ===========================================================================
# 1. mangled names: parameter types, and static-vs-member
# ===========================================================================

_BUILTIN = {
    "v": "void", "w": "wchar_t", "b": "bool", "c": "char", "a": "signed char",
    "h": "unsigned char", "s": "short", "t": "unsigned short", "i": "int",
    "j": "unsigned int", "l": "long", "m": "unsigned long", "x": "long long",
    "y": "unsigned long long", "n": "__int128", "o": "unsigned __int128",
    "f": "float", "d": "double", "e": "long double", "g": "__float128", "z": "...",
}


class MangleError(Exception):
    pass


class Demangler:
    """Just enough Itanium demangling to answer two questions.

    How many parameters does this symbol take, and what class (if any) does parameter
    k name?  Substitutions are tracked because they change the parameter COUNT --
    `_ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_` is four parameters, not one, and getting
    that wrong would silently shift every argument's type by three places.
    """

    def __init__(self, s):
        self.s = s
        self.i = 0
        self.subs = []

    def peek(self):
        return self.s[self.i] if self.i < len(self.s) else ""

    def take(self, n=1):
        r = self.s[self.i:self.i + n]
        self.i += n
        return r

    def num(self):
        j = self.i
        while j < len(self.s) and self.s[j].isdigit():
            j += 1
        if j == self.i:
            raise MangleError("expected digits at %d" % self.i)
        n = int(self.s[self.i:j])
        self.i = j
        return n

    def source_name(self):
        n = self.num()
        if self.i + n > len(self.s):
            raise MangleError("short source-name")
        return self.take(n)

    def sub_ref(self):
        # S_ / S<seq>_ / St / Ss / Si / So / Sd
        self.take()                       # 'S'
        c = self.peek()
        if c in "tsiod":
            self.take()
            return {"t": "std", "s": "std::string", "i": "std::istream",
                    "o": "std::ostream", "d": "std::iostream"}[c]
        if c == "_":
            self.take()
            idx = 0
        else:
            j = self.i
            while j < len(self.s) and self.s[j] not in "_":
                j += 1
            if j >= len(self.s):
                raise MangleError("unterminated substitution")
            body = self.s[self.i:j]
            self.i = j + 1
            idx = int(body, 36) + 1
        if idx >= len(self.subs):
            raise MangleError("substitution %d out of range" % idx)
        return self.subs[idx]

    def add(self, name):
        if name not in self.subs:
            self.subs.append(name)
        return name

    def template_args(self):
        self.take()                       # 'I'
        args = []
        while self.peek() and self.peek() != "E":
            args.append(self.type())
        if self.take() != "E":
            raise MangleError("unterminated template-args")
        return args

    def nested(self, add_final=True):
        self.take()                       # 'N'
        while self.peek() in ("K", "V", "r"):
            self.take()
        parts = []
        while self.peek() and self.peek() != "E":
            c = self.peek()
            if c.isdigit():
                parts.append(self.source_name())
            elif c == "S":
                parts.append(self.sub_ref())
            elif c == "I":
                parts[-1] = "%s<%s>" % (parts[-1], ", ".join(self.template_args()))
            elif c in "CD":
                parts.append(self.take(2))
            else:
                raise MangleError("nested component %r" % c)
            if self.peek() != "E":
                self.add("::".join(parts))
        if self.take() != "E":
            raise MangleError("unterminated nested-name")
        full = "::".join(parts)
        # A FUNCTION's complete name is not a substitution candidate; a TYPE's is.
        # Adding it anyway shifts every later S<n>_ by one, which silently retypes
        # arguments -- the exact failure mode this pass exists to avoid.
        return self.add(full) if add_final else full

    def type(self):
        c = self.peek()
        if not c:
            raise MangleError("type ran off the end")
        if c in "PROCG":
            self.take()
            inner = self.type()
            return self.add(inner + {"P": " *", "R": " &", "O": " &&",
                                     "C": " complex", "G": " imaginary"}[c])
        if c in "KVr":
            self.take()
            inner = self.type()
            return self.add("const " + inner if c == "K" else inner)
        if c == "A":
            self.take()
            dim = ""
            while self.peek() and self.peek() != "_":
                dim += self.take()
            self.take()
            return self.add("%s [%s]" % (self.type(), dim))
        if c == "M":
            self.take()
            cls = self.type()
            ty = self.type()
            return self.add("%s %s::*" % (ty, cls))
        if c == "F":
            self.take()
            args = []
            while self.peek() and self.peek() != "E":
                args.append(self.type())
            if self.take() != "E":
                raise MangleError("unterminated function type")
            return self.add("%s (%s)" % (args[0], ", ".join(args[1:])))
        if c == "N":
            n = self.nested()
            if self.peek() == "I":
                return self.add("%s<%s>" % (n, ", ".join(self.template_args())))
            return n
        if c == "S":
            n = self.sub_ref()
            if self.peek() == "I":
                return self.add("%s<%s>" % (n, ", ".join(self.template_args())))
            return n
        if c == "T":
            self.take()
            while self.peek() and self.peek() != "_":
                self.take()
            self.take()
            return self.add("T")
        if c.isdigit():
            n = self.source_name()
            if self.peek() == "I":
                self.add(n)
                return self.add("%s<%s>" % (n, ", ".join(self.template_args())))
            return self.add(n)
        if c in _BUILTIN:
            self.take()
            return _BUILTIN[c]
        raise MangleError("unhandled type code %r" % c)


_PARAM_CACHE = {}


def mangled_params(sym):
    """(params, ok).  `params` is the declared parameter type list, `this` excluded.

    `_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj`
        -> ['WithMeshClsn &', 'ModelAnim &', 'unsigned int']
    """
    if sym in _PARAM_CACHE:
        return _PARAM_CACHE[sym]
    res = (None, False)
    try:
        if not sym.startswith("_Z"):
            raise MangleError("not a _Z symbol")
        d = Demangler(sym[2:])
        if d.peek() == "N":
            d.nested(add_final=False)
        elif d.peek().isdigit():
            d.source_name()
        else:
            raise MangleError("unsupported name form")
        if d.peek() == "I":                     # function template
            d.template_args()
            d.type()                            # return type is mangled for templates
        params = []
        while d.peek():
            params.append(d.type())
        if params == ["void"]:
            params = []
        res = (params, True)
    except (MangleError, ValueError, IndexError):
        res = (None, False)
    _PARAM_CACHE[sym] = res
    return res


def base_class_of(sym):
    cls, meth, _why = EH.class_of_symbol(sym)
    return cls, meth


def is_structor(cls, meth):
    if not cls or not meth:
        return False
    last = cls.split("__")[-1]
    return meth == "~" + last or meth == last


def class_in(type_str, known):
    """'ModelAnim &' / 'Actor *' -> the class name, when it names a class we know of.

    Restricted to `known` -- the classes the tree actually has a header or an asserted
    size for.  Without that guard a mis-parsed substitution such as `Fix12` (the
    template, not `Fix12<int>`) reads as a class and manufactures a member type.
    Template-ids and arrays are refused outright.
    """
    if not type_str:
        return None
    t = type_str.strip()
    while t.endswith("&") or t.endswith("*"):
        t = t[:-1].strip()
    if t.startswith("const "):
        t = t[6:].strip()
    if "<" in t or "[" in t or "(" in t:
        return None
    if not re.fullmatch(r"[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*", t):
        return None
    if t in set(_BUILTIN.values()):
        return None
    name = t.replace("::", "__")
    return name if name in known else None


# ===========================================================================
# 2. sizes and the class hierarchy of the candidate types
# ===========================================================================

SIZE_ASSERT = re.compile(
    r"typedef\s+char\s+\w+_size_must_be_(0x[0-9a-fA-F]+)\s*\[\s*sizeof\s*\(\s*"
    r"(?:struct\s+)?(?P<cls>[A-Za-z_]\w*)\s*\)\s*==")
CPP_STRUCT = re.compile(r"^struct\s+(?P<cls>[A-Za-z_]\w*)\s*:\s*(?P<bases>[^{]+)\{",
                        re.M)


def header_facts(root):
    """(asserted sizes, base-class map) from the hand-reconstructed headers.

    `typedef char Model_size_must_be_0x50[sizeof(Model) == 0x50 ? 1 : -1];` is a
    compile-time assertion the build enforces in every TU that includes it, so it is
    the strongest size statement in the tree.  22 headers carry one.
    """
    sizes, bases = {}, {}
    inc = os.path.join(root, "include")
    for fn in sorted(os.listdir(inc)):
        if not fn.endswith(".h"):
            continue
        txt = pathlib.Path(inc, fn).read_text(encoding="utf-8", errors="replace")
        for m in SIZE_ASSERT.finditer(txt):
            line = txt[:m.start()].count("\n") + 1
            sizes.setdefault(m.group("cls"),
                             (int(m.group(1), 16), "include/%s:%d" % (fn, line)))
        for m in CPP_STRUCT.finditer(txt):
            bs = [re.sub(r"\b(public|private|protected|virtual)\b", " ", b).strip()
                  for b in m.group("bases").split(",")]
            bases.setdefault(m.group("cls"), [b for b in bs if b])
    return sizes, bases


def derived_map(root, marker_classes):
    """{class -> the marker classes whose storage its methods also address}.

    Single inheritance places every base at offset 0, so `daKrb_c`'s `this + 0xe0` is
    the SAME storage as `Enemy`'s `unk_0e0`.  A base class whose own methods never
    touch a field gives a per-class scan nothing to see -- the hierarchy pass records
    that blindness for widths (plan section 3, pass 2) and it applies here identically.
    """
    path = os.path.join(root, "build", "evidence_hierarchy.json")
    if not os.path.exists(path):
        return {}, ["evidence_hierarchy.json absent -- no derived-class reach"]
    h = json.load(open(path, encoding="utf-8"))["hierarchy"]
    out = collections.defaultdict(set)
    for cls, info in h.items():
        for anc in list(info.get("chain") or []) + [info.get("base")]:
            if anc in marker_classes and anc != cls:
                out[cls].add(anc)
    return out, []


def isa(cls, base, bases):
    """Does `cls` derive from (or equal) `base`, per the reconstructed headers?"""
    seen, stack = set(), [cls]
    while stack:
        c = stack.pop()
        if c == base:
            return True
        if c in seen:
            continue
        seen.add(c)
        stack.extend(bases.get(c, []))
    return False


# ===========================================================================
# 3. the corpus
# ===========================================================================

def today_files(root):
    src = os.path.join(root, "src")
    for dirpath, _dirs, names in os.walk(src):
        for n in sorted(names):
            if not n.endswith((".c", ".cpp")):
                continue
            p = os.path.join(dirpath, n)
            try:
                yield (os.path.relpath(p, root).replace("\\", "/"),
                       pathlib.Path(p).read_text(encoding="utf-8", errors="replace"))
            except OSError:
                continue


def history_files(root, commitish=HIST):
    for path, text in EH.cat_files(root, commitish,
                                   EH.list_tree(root, commitish, "src/")):
        if text is not None:
            yield path, text


# ===========================================================================
# 4. offset expressions -- and the dereference trap
# ===========================================================================

ID = r"[A-Za-z_][A-Za-z0-9_]*"
NUM = r"(?:0[xX][0-9a-fA-F]+|[0-9]+)"
_CAST = re.compile(r"\(\s*(?:struct\s+|class\s+|const\s+)*(?P<ty>[A-Za-z_][\w \t]*?)"
                   r"\s*(?P<st>\*+)\s*\)")

# A cast binds tighter than `+`, so `(char *)c + 0x59c` strides by 1 while
# `*(void **)(c + 0x59c)` strides by whatever `c` is -- the `(void **)` there applies
# to the SUM, not to the base.  Getting that backwards multiplies every offset by 4.
_CASTG = (r"(?:\(\s*(?:struct\s+|class\s+|const\s+)*(?P<cty{0}>[A-Za-z_][\w \t]*?)"
          r"\s*(?P<cst{0}>\*+)\s*\)\s*)?")
_FORMS = [
    ("base+off", re.compile(r"^&?\s*" + _CASTG.format(0) +
                            r"(?P<id>" + ID + r")\s*\+\s*(?P<n>" + NUM + r")$")),
    ("&base[i]", re.compile(r"^&\s*" + _CASTG.format(1) + r"(?P<id>" + ID +
                            r")\s*\[\s*(?P<n>" + NUM + r")\s*\]$")),
    ("&p->mem", re.compile(r"^&\s*(?P<id>" + ID + r")\s*->\s*(?P<mem>" + ID + r")$")),
    ("&mem", re.compile(r"^&\s*(?P<mem>" + ID + r")$")),
]

# `((char *)this)` -> `(char *)this`, so the cast can be seen to bind to the base
_REDUNDANT = re.compile(r"\(\s*(\(\s*[A-Za-z_][\w \t]*\*+\s*\)\s*[A-Za-z_]\w*)\s*\)")
# `(long long)`, `(int)` -- the width launderings this corpus is full of
_SCALAR_CAST = re.compile(r"\(\s*(?:signed\s+|unsigned\s+)*(?:long\s+long|long|int|"
                          r"short|char|u8|s8|u16|s16|u32|s32|u64|s64)\s*\)")


def _peel(txt):
    """Strip wrappers that do NOT bind to the base, and report a leading `*`.

    Deliberately LEAVES a cast that binds directly to the base identifier, because
    that one sets the pointer stride and the form regexes below read it.
    """
    t = txt
    while True:
        t = _SCALAR_CAST.sub(" ", t).strip()
        prev = t
        t = _REDUNDANT.sub(r"\1", t).strip()
        m = _CAST.match(t)
        if m:
            rest = t[m.end():].strip()
            wraps = rest.startswith("(") and EH._match_paren(rest, 0) == len(rest) - 1
            if wraps or rest.startswith("&"):
                t = rest              # applies to a whole group: not a stride cast
                continue
        if t.startswith("(") and EH._match_paren(t, 0) == len(t) - 1:
            t = t[1:-1].strip()
            continue
        if t == prev:
            break
    return t, t.startswith("*")


def arg_address_offsets(arg, bases, fieldmap, env):
    """(offsets, deref).  An offset counts ONLY when the whole argument IS the address.

    `_ZN6Player9DropActorEv(*(void **)((char *)&unk_5e4))` reports deref=True: the
    argument's value is the WORD stored there, so the callee types the POINTER, not
    the object.  Conflating the two turns a 4-byte pointer into a 100-byte member.
    """
    t, deref = _peel(arg)
    if deref:
        t, _ = _peel(t.lstrip("*"))
    out = []
    for how, rx in _FORMS:
        m = rx.match(t)
        if not m:
            continue
        g = m.groupdict()
        if how in ("&mem", "&p->mem"):
            if how == "&p->mem" and g.get("id") not in bases:
                break
            o = fieldmap.get(g["mem"])
            if o is not None:
                out.append((o, how))
            break
        if g["id"] not in bases:
            break
        sfx = "0" if how == "base+off" else "1"
        es = None
        if g.get("cty" + sfx):
            es = EH.elem_size(g["cty" + sfx], len(g["cst" + sfx]), env)
        if not es:
            es = bases[g["id"]]
        out.append((int(g["n"], 0) * es, how))
        break
    return out, deref


def split_args(txt):
    parts, depth, cur = [], 0, []
    for ch in txt:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append("".join(cur))
            cur = []
        else:
            cur.append(ch)
    if "".join(cur).strip():
        parts.append("".join(cur))
    return [p.strip() for p in parts]


def this_bases(code, stem, cls_last):
    """{identifier: element size} for locals that provably still hold `this`."""
    params, body, start = EH.find_definition(code, stem)
    if body is None and cls_last:
        params, body, start = EH.find_method_definition(code, cls_last, None)
    if body is None:
        return {}, None, 0
    # A real C++ method has no explicit parameter, but its body still writes
    # `char *c = (char *)this;` and spells every field off `c`.  Seeding `this` is
    # what lets find_aliases follow that; without the seed those files look empty.
    bases = {"this": ("char", 1)}
    if params:
        ps = EH.split_params(params)
        if ps:
            d = EH.parse_pointer_decl(ps[0])
            if d:
                bases[d[2]] = (d[0], d[1])
    env = EH.typedef_env(code)
    known = EH.find_aliases(body, bases)
    out = {nm: (EH.elem_size(ty, st, env) or 1) for nm, (ty, st) in known.items()}
    out.setdefault("this", 1)
    return out, body, start


# ===========================================================================
# 5. evidence extraction
# ===========================================================================

# `_ZN5dBgPiC2Ev(this + 0x10)` names no class, but two different classes handing
# their own offset to the SAME unnamed function have the same type there.  That turns
# a pair of undecidables into one undecidable, which is worth recording.
_CALL = re.compile(r"(?<![\w>.:])(?P<callee>_Z[A-Za-z0-9_]+|func_[A-Za-z0-9_]+)\s*\(")
_MCALL = re.compile(r"\(\s*(?:struct\s+|class\s+)?(?P<ty>[A-Za-z_]\w*)\s*\*\s*\)")


def _cite(origin, path, line):
    """Both trees use the same paths, so a bare `file:line` would be ambiguous."""
    return ("%s:%d" % (path, line) if origin == "today"
            else "%s:%s:%d" % (origin, path, line))


def scan_calls(body, bases, fieldmap, env, origin, path, base_line, known_types):
    out = []
    for m in _CALL.finditer(body):
        callee = m.group("callee")
        lp = m.end() - 1
        rp = EH._match_paren(body, lp)
        if rp < 0:
            continue
        anon = not callee.startswith("_Z")
        cls, meth = (None, None) if anon else base_class_of(callee)
        params, ok = (None, False) if anon else mangled_params(callee)
        args = split_args(body[lp + 1:rp])
        line = base_line + body[:m.start()].count("\n")
        # arg0 is `this` only when the call passes one more argument than the mangled
        # signature declares.  Otherwise it is a static/free function and arg0 is a
        # normal parameter.
        has_this = bool(cls) and ok and params is not None and len(args) == len(params) + 1
        arity_ok = ok and params is not None and len(args) in (len(params), len(params) + 1)
        for ai, arg in enumerate(args):
            offs, deref = arg_address_offsets(arg, bases, fieldmap, env)
            for off, how in offs:
                if off <= 0:
                    continue
                if anon:
                    if deref or ai != 0:
                        continue
                    out.append({"offset": off, "type": None, "kind": "anon",
                                "deref": False, "callee": callee, "argno": ai,
                                "how": how, "origin": origin,
                                "where": _cite(origin, path, line)})
                    continue
                if has_this and ai == 0:
                    ty, kind = cls, ("exact" if is_structor(cls, meth) else "bound")
                    # a member subobject's own structor names its DECLARED type
                elif not arity_ok and ai == 0 and cls:
                    # The file's hand-written extern disagrees with the mangled arity
                    # (this corpus adds spare `int` parameters), so `this` cannot be
                    # confirmed positionally.  Reported, never counted as decided.
                    ty, kind = cls, "arity-unclear"
                elif ok and params is not None:
                    pi = ai - 1 if has_this else ai
                    if pi < 0 or pi >= len(params):
                        continue
                    ty = class_in(params[pi], known_types)
                    if not ty:
                        continue
                    # A `ModelAnim &` parameter binds a ModelAnim OR anything derived
                    # from one, so this is a lower bound exactly like a base-class
                    # method call.  Calling it exact would type every BlendModelAnim
                    # member as a ModelAnim.
                    kind = "bound"
                else:
                    continue
                out.append({"offset": off, "type": ty,
                            "kind": ("pointer" if kind != "arity-unclear"
                                     else "pointer-unclear") if deref else kind,
                            "deref": deref, "callee": callee, "argno": ai,
                            "how": how, "origin": origin,
                            "where": _cite(origin, path, line)})
    return out


def scan_casts(body, bases, fieldmap, env, origin, path, base_line, known_types,
               local_structs):
    """`((ModelBase *)((char *)&mModel))->SetFile(...)` -- a cast is a lower bound.

    UNLESS the file declares that struct itself.  `src/_ZN5Stage11RenderModelEv.cpp`
    writes `struct ModelComponents { void *sub; Component *components; };` -- a local
    stand-in whose NAME a human chose and whose layout does not match the real class.
    Casting through it proves nothing about the type; it is recorded as `localname`
    and never decides anything.
    """
    out = []
    for m in _MCALL.finditer(body):
        ty = m.group("ty")
        if ty not in known_types:
            continue
        local = ty in local_structs
        tail = body[m.end():]
        # the cast's operand: a parenthesised expression right after it
        t = tail.lstrip()
        if not t.startswith("("):
            continue
        skip = len(tail) - len(t)
        rp = EH._match_paren(tail, skip)
        if rp < 0:
            continue
        offs, deref = arg_address_offsets(tail[skip:rp + 1], bases, fieldmap, env)
        line = base_line + body[:m.start()].count("\n")
        for off, how in offs:
            if off <= 0:
                continue
            out.append({"offset": off, "type": ty,
                        "kind": ("pointer" if deref else
                                 "localname" if local else "bound"),
                        "deref": deref, "callee": None, "argno": None,
                        "how": ("cast to a locally-declared struct " if local
                                else "cast ") + how,
                        "origin": origin, "where": _cite(origin, path, line)})
    return out


_SKIP_MEMBER_TY = set(EH._W1) | set(EH._W2) | set(EH._W4) | set(EH._W8) | {"void"}


def scan_local_layout(code, cls, origin, path):
    """A migrated file that declares its own layout IS a member-type table.

    A byte-matching migrated file declared, in effect,
    `struct Amp : Actor { ModelAnim m0; /* 0xd4 */ ... }`, so both the offsets
    and the class names in it are pinned by the ROM's own relocations.

    Named by class, not by path, on purpose: check_dead_references resolves any
    `a/b`-shaped token in prose as a live repo-rooted reference, and the file
    this came from is a per-symbol one -- those die on rename AND on TU
    promotion, which folds them into src/actors/<Class>.cpp.
    """
    texts = EH.find_struct_texts(code)
    if cls not in texts:
        return []
    env = EH.typedef_env(code)
    bases, bodytxt = texts[cls]
    off = 0
    for b in bases:
        sub = EH.struct_layout(b, texts, env)
        if not sub or not sub["complete"]:
            return []
        off += sub["size"]
    if not bases and re.search(r"\bvirtual\b", bodytxt):
        off = 4
    line0 = code[:code.index(bodytxt)].count("\n") + 1 if bodytxt in code else 0
    out = []
    for stmt in EH._split_top(bodytxt, ";"):
        s = re.sub(r"\b(?:public|private|protected)\s*:", " ", stmt).strip()
        if not s or EH._SKIP_STMT.match(s) or "(" in s:
            continue
        m = re.match(r"^(?P<ty>[A-Za-z_][\w \t:]*?)\s+(?P<st>[*&]*)\s*(?P<name>\w+)"
                     r"(?P<arr>(?:\s*\[[^\]]*\])*)\s*$", s)
        if not m:
            break
        ty = EH.normalize_type(m.group("ty"))
        stars = m.group("st").count("*")
        if stars:
            w, a = 4, 4
        else:
            w = EH.type_width(ty, 1, env)
            a = EH._align_of(w)
            if w is None:
                sub = EH.struct_layout(ty, texts, env)
                if not sub or not sub["complete"]:
                    break
                w, a = sub["size"], sub["align"]
        n = 1
        for dim in re.findall(r"\[([^\]]*)\]", m.group("arr") or ""):
            v = EH.const_expr(dim)
            if v is None:
                n = None
                break
            n *= v
        if n is None:
            break
        off = (off + a - 1) & ~(a - 1)
        if not stars and ty not in _SKIP_MEMBER_TY and n == 1 and off > 0:
            # The member TYPE is pinned only when the declaration forces the compiler
            # to emit a call to `_ZN<Type>D1Ev` -- that call is a relocation the byte
            # gate checks.  A member whose type declares no destructor contributes no
            # relocation, so its name is the author's choice and is marked weak.
            has_dtor = bool(re.search(r"~\s*" + re.escape(ty) + r"\s*\(",
                                      texts.get(ty, ("", ""))[1] or ""))
            out.append({"offset": off, "type": ty,
                        "kind": "exact" if has_dtor else "localname", "deref": False,
                        "callee": None, "argno": None,
                        "how": ("local struct member, destructor emitted" if has_dtor
                                else "local struct member, no destructor"),
                        "origin": origin, "where": _cite(origin, path, line0)})
        off += w * n
    return out


# ===========================================================================
# 6. main
# ===========================================================================

def name_hypothesis(field):
    """`mModelAnim` -> ModelAnim, `mModel2` -> Model.  A NAME, NOT EVIDENCE."""
    if field.startswith("unk_"):
        return None
    n = field
    if n.startswith("m") and len(n) > 1 and n[1].isupper():
        n = n[1:]
    n = re.sub(r"\d+$", "", n)
    return n or None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json")
    ap.add_argument("--cls")
    ap.add_argument("--verbose", action="store_true")
    ap.add_argument("--no-history", action="store_true")
    args = ap.parse_args()

    markers = json.load(open(REPORT, encoding="utf-8"))["findings"][MARKER_BUCKET]
    if args.cls:
        markers = [m for m in markers if m["cls"] == args.cls]

    headers = GH.load_headers(pathlib.Path(ROOT))
    sizes, cpp_bases = header_facts(ROOT)
    # Every class name the tree actually declares: header stems (recursively) plus
    # every struct/class body inside include/.  Vector3 lives in types.h, not in a
    # Vector3.h, and a stem-only universe drops it without saying so.
    known_types = set(sizes) | set(headers)
    for h in pathlib.Path(ROOT, "include").rglob("*.h"):
        known_types.add(h.stem)
        for m in re.finditer(r"\b(?:struct|class)\s+([A-Za-z_]\w*)\b",
                             h.read_text(encoding="utf-8", errors="replace")):
            known_types.add(m.group(1))

    want = {m["cls"] for m in markers}
    derived, derived_notes = derived_map(ROOT, want)
    rom_path = os.path.join(ROOT, "build", "evidence_rom.json")
    rom = (json.load(open(rom_path, encoding="utf-8"))["classes"]
           if os.path.exists(rom_path) else {})
    fieldmaps = {c: {f[3]: o for o, f in headers.get(c, {}).items()} for c in want}
    # what the header declares immediately after the marker -- explains a short span
    nextfield = {}
    for c in want:
        offs = sorted(headers.get(c, {}))
        for a, b in zip(offs, offs[1:]):
            nextfield[(c, a)] = (b, headers[c][b][3], headers[c][b][0])

    ev = collections.defaultdict(list)
    files_seen = collections.Counter()
    layout_sizes = collections.defaultdict(collections.Counter)
    stats = collections.Counter()

    def handle(path, text, origin):
        stem = os.path.basename(path).rsplit(".", 1)[0]
        cls, _meth, _why = EH.class_of_symbol(stem)
        if cls is None:
            stats["files_unattributable"] += 1
            return
        targets = ({cls} if cls in want else set()) | derived.get(cls, set())
        if not targets:
            # A free function (`func_ov070_02120724`) carries no class in its name, but
            # a migrated one includes exactly the header of the class it operates on.
            inc = set(re.findall(r'#include\s+"([A-Za-z_]\w*)\.h"', text)) & want
            if len(inc) == 1:
                targets = set(inc)
                stats["files_by_include"] += 1
        if not targets:
            return
        stats["files_attributed"] += 1
        for tgt in targets:
            files_seen[tgt] += 1
        code = EH.strip_noncode(text)
        env = EH.typedef_env(code)
        last = cls.split("__")[-1]
        bases, body, bstart = this_bases(code, stem, last)
        fm = fieldmaps.get(cls, {})
        got = []
        local_structs = set(EH.find_struct_texts(code))
        if body:
            bl = code[:bstart].count("\n") + 1
            got += scan_calls(body, bases, fm, env, origin, path, bl, known_types)
            got += scan_casts(body, bases, fm, env, origin, path, bl, known_types,
                              local_structs)
        got += scan_local_layout(code, last, origin, path)
        for g in got:
            for tgt in targets:
                g2 = dict(g)
                if tgt != cls:
                    g2["via"] = cls          # seen through a derived class's method
                ev[(tgt, g["offset"])].append(g2)
        if got:
            stats["files_with_evidence"] += 1
        lay = [g for g in got if g["how"] == "local struct member"]
        for a, b in zip(lay, lay[1:]):
            layout_sizes[a["type"]][b["offset"] - a["offset"]] += 1

    for path, text in today_files(ROOT):
        stats["files_today"] += 1
        handle(path, text, "today")
    if not args.no_history:
        for path, text in history_files(ROOT):
            stats["files_history"] += 1
            handle(path, text, HIST)

    # Every exactly-typed member of every marker class, at ANY offset -- not only at
    # the marker offsets.  A marker that lands INSIDE one of these is not an object of
    # unknown type at all; it is the header naming an interior word of a member it
    # already has, which is a layout bug rather than a typing question.
    members = collections.defaultdict(dict)
    anon_calls = collections.defaultdict(set)
    for (c, o), hits in ev.items():
        for h in hits:
            if h["kind"] == "exact" and h["type"]:
                members[c].setdefault(o, (h["type"], h["where"]))
            elif h["kind"] == "anon":
                anon_calls[(c, o)].add(h["callee"])

    def size_of(cls):
        if cls in sizes:
            return sizes[cls][0], sizes[cls][1]
        if cls in layout_sizes:
            n, k = layout_sizes[cls].most_common(1)[0]
            # UPPER BOUND ONLY.  A revision of `src/_ZN8PoleLiftD1Ev.cpp` padded
            # Model to 0x80 so the next member landed at 0x158; the real class is
            # 0x50 and the 0x30 between them is unknown space.  Never let this
            # override an assertion.
            return n, "inferred (UPPER BOUND) from %d local layout(s)" % k
        return None, None

    results = []
    for mk in markers:
        cls, off, span, field = mk["cls"], int(mk["offset"], 16), mk["span"], mk["field"]
        hits = ev.get((cls, off), [])
        exact = [h for h in hits if h["kind"] == "exact"]
        param = []
        bound = [h for h in hits if h["kind"] == "bound"]
        ptr = [h for h in hits if h["kind"] == "pointer"]
        weak = [h for h in hits if h["kind"] in ("arity-unclear", "pointer-unclear")]
        localn = [h for h in hits if h["kind"] == "localname"]

        hypo = name_hypothesis(field)
        hsize, hsrc = size_of(hypo) if hypo else (None, None)

        # does this marker land inside an already-typed member of the same class?
        inside = None
        for mo, (mty, mwhere) in sorted(members.get(cls, {}).items()):
            if mo >= off:
                continue
            msz = size_of(mty)[0]
            nxt = min([x for x in members[cls] if x > mo] or [None])
            limit = msz if msz is not None else (nxt - mo if nxt else None)
            if limit is not None and off < mo + limit:
                inside = {"member_offset": hex(mo), "member_type": mty,
                          "member_size": msz, "interior_by": off - mo,
                          "where": mwhere}
                break

        rec = {
            "cls": cls, "field": field, "offset": mk["offset"], "span": span,
            "where": mk["where"], "rom_or_history_widths": mk.get("observed"),
            "name_hypothesis": hypo, "hypothesis_size": hsize,
            "hypothesis_size_src": hsrc,
            "size_match": hsize is not None and span is not None and hsize == span,
            "next_declared": nextfield.get((cls, off)),
            "interior_to": inside,
            "anon_ctor": sorted(anon_calls.get((cls, off), ())),
            "files_scanned": files_seen.get(cls, 0),
            # pass 3's own view: how many times the ROM saw this offset's ADDRESS
            # taken.  It cannot name a type, but it separates "nothing is there" from
            # "something is there and no source names it".
            "rom_address_taken": (rom.get(cls, {}).get(mk["offset"], {})
                                  .get("address_only", 0)),
            "rom_functions": (rom.get(cls, {}).get(mk["offset"], {})
                              .get("functions", 0)),
            "counts": {k: len(v) for k, v in
                       (("exact", exact), ("bound", bound), ("pointer", ptr),
                        ("weak", weak), ("localname", localn)) if v},
            "evidence": hits,
        }

        strong = exact
        if cls in BOGUS_HEADERS:
            rec["verdict"] = "excluded (known-bogus header)"
        elif strong:
            t = collections.Counter(h["type"] for h in strong)
            rec["proposed"] = t.most_common(1)[0][0]
            rec["verdict"] = "confident" if len(t) == 1 else "confident (conflicting)"
            rec["decided_by"] = sorted({h["where"] for h in strong
                                        if h["type"] == rec["proposed"]})[:3]
        elif bound:
            t = collections.Counter(h["type"] for h in bound)
            # several bounds can be seen at once (ModelBase::SetFile and
            # ModelAnim::SetAnim on the same field); the MOST DERIVED one is the
            # informative bound, not the most frequent.
            cands = list(t)
            b = max(cands, key=lambda c: sum(1 for o in cands if isa(c, o, cpp_bases)))
            rec["bound_class"] = b
            rec["decided_by"] = sorted({h["where"] for h in bound
                                        if h["type"] == b})[:3]
            # (a) and (b) agreeing independently: the name's class derives from the
            # base the callee proves, AND the span equals that class's asserted size.
            if hypo and rec["size_match"] and isa(hypo, b, cpp_bases) and hypo != b:
                rec["proposed"] = hypo
                rec["verdict"] = "confident (base-bound + size)"
            elif hypo and isa(hypo, b, cpp_bases) and hypo != b:
                rec["proposed"] = hypo
                rec["verdict"] = "base-bound, size unconfirmed"
            else:
                rec["proposed"] = b
                rec["verdict"] = "base-bound only"
        elif ptr:
            t = collections.Counter(h["type"] for h in ptr)
            rec["proposed"] = t.most_common(1)[0][0] + " *"
            rec["verdict"] = "pointer, not an object"
            rec["decided_by"] = sorted({h["where"] for h in ptr})[:3]
        elif weak:
            tw = collections.Counter(h["type"] for h in weak)
            allptr = all(h["kind"] == "pointer-unclear" for h in weak)
            rec["proposed"] = tw.most_common(1)[0][0] + (" *" if allptr else "")
            rec["verdict"] = ("pointer, arity unconfirmed" if allptr
                              else "base-bound, arity unconfirmed")
            rec["decided_by"] = sorted({h["where"] for h in weak})[:3]
        elif localn:
            tl = collections.Counter(h["type"] for h in localn)
            rec["proposed"] = tl.most_common(1)[0][0]
            rec["verdict"] = "name-only (cast through a locally-declared struct)"
            rec["decided_by"] = sorted({h["where"] for h in localn})[:3]
        elif inside:
            rec["proposed"] = "(interior to %s @%s)" % (inside["member_type"],
                                                        inside["member_offset"])
            rec["verdict"] = "interior to a typed member (header layout bug)"
            rec["decided_by"] = [inside["where"]]
        elif rec["anon_ctor"]:
            rec["verdict"] = "undecided (unnamed callee: %s)" % rec["anon_ctor"][0]
            rec["decided_by"] = sorted({h["where"] for h in hits
                                        if h["kind"] == "anon"})[:3]
        elif rec["size_match"]:
            rec["proposed"] = hypo
            rec["verdict"] = "size-match only"
        else:
            rec["verdict"] = (
                "undecided (class has no source file)" if files_seen.get(cls, 0) == 0
                else "undecided (ROM sees the address taken, nothing names it)"
                if rec["rom_address_taken"]
                else "undecided (no evidence at this offset)")

        p = rec.get("proposed", "").replace(" *", "")
        psize, _ = size_of(p) if p else (None, None)
        rec["proposed_size"] = psize
        if psize is not None and span is not None:
            rec["span_vs_size"] = ("equal" if psize == span else
                                   "span SHORTER (header declares a field inside the object)"
                                   if span < psize else
                                   "span LONGER (unknown space follows the object)")
        else:
            rec["span_vs_size"] = None
        rec["name_agrees"] = (hypo is not None and rec.get("proposed") == hypo)
        results.append(rec)

    out = {"markers": len(markers), "stats": dict(stats),
           "sizes": {k: v[0] for k, v in sizes.items()},
           "layout_sizes": {k: dict(v) for k, v in layout_sizes.items()},
           "results": results}

    # The same class at the same offset across many headers: a shared shape whose
    # members must agree.  Listed so a disagreement is visible rather than averaged.
    same = collections.defaultdict(list)
    for r in results:
        if r.get("proposed") and r["verdict"].startswith("confident"):
            same[(int(r["offset"], 16), r["proposed"])].append(r["cls"])
    out["repeated_offset_type"] = {"%s@%#x" % (ty, off): sorted(cs)
                                   for (off, ty), cs in same.items() if len(cs) > 1}
    out["span_vs_size"] = dict(collections.Counter(
        r["span_vs_size"] for r in results if r.get("proposed")))
    out["derived_notes"] = derived_notes
    shared = collections.defaultdict(list)
    for r in results:
        for fn in r.get("anon_ctor") or ():
            shared[fn].append("%s.%s@%s" % (r["cls"], r["field"], r["offset"]))
    out["same_type_via_unnamed_callee"] = {k: v for k, v in shared.items()
                                           if len(v) > 1}
    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(out, indent=1), encoding="utf-8")

    print("corpus     : %d files today + %d at %s; %d attributed to one of the %d "
          "marker classes; %d yielded evidence"
          % (stats["files_today"], stats["files_history"], HIST,
             stats["files_attributed"], len(want), stats["files_with_evidence"]))
    noclass = sorted({r["cls"] for r in results if r["files_scanned"] == 0})
    print("recall     : %d markers; %d classes with 0 source files%s"
          % (len(markers), len(noclass), (" -> " + ", ".join(noclass)) if noclass else ""))
    print("size (a) vs callee (b): " + ", ".join(
        "%s=%s" % (k, v) for k, v in sorted(out["span_vs_size"].items(),
                                            key=lambda kv: str(kv[0]))))
    print()
    by = collections.Counter(r["verdict"] for r in results)
    for k, v in by.most_common():
        print("  %-46s %4d" % (k, v))
    print()
    order = {"confident": 0, "confident (base-bound + size)": 1,
             "confident (conflicting)": 2, "base-bound, size unconfirmed": 3,
             "base-bound only": 4, "pointer, not an object": 5, "size-match only": 6}
    for r in sorted(results, key=lambda r: (order.get(r["verdict"], 9), r["cls"],
                                            int(r["offset"], 16))):
        tag = {"equal": "size=", None: "size?"}.get(
            r["span_vs_size"], "size<" if r["span_vs_size"] and "SHORTER"
            in r["span_vs_size"] else "size>")
        print("%-30s %-6s span=%-5s %-24s %-5s %-46s %s"
              % (r["cls"] + "." + r["field"], r["offset"], r["span"],
                 r.get("proposed", "-"), tag if r.get("proposed") else "",
                 r["verdict"], (r.get("decided_by") or [""])[0]))
        if args.verbose:
            for h in r["evidence"]:
                print("        %-8s %-22s arg%-3s %-34s %s"
                      % (h["kind"], h["type"], h["argno"],
                         (h["callee"] or h["how"])[:34], h["where"]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
