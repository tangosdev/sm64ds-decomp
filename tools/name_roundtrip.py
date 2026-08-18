#!/usr/bin/env python3
"""Round-trip every mangled symbol name through the pinned compiler.

## Why this exists

A retail cartridge has no symbol table, so every mangled name in
``config/**/symbols.txt`` is somebody's assertion (see
``notes/symbol-name-provenance.md``).  Roughly 1,349 of them are verbatim
imports from an ASM-hack project whose build links by address and therefore
could never have falsified a parameter type.  Half the tree's mangled names
have never been shown to a compiler at all.

The check here is mechanical and needs no judgement:

    symbols.txt name -> parse -> synthesise a declaration -> compile with the
    pin -> read the emitted symbol back -> compare to the original

A name the pinned toolchain **cannot emit** is wrong regardless of who wrote
it.  That is a strictly stronger statement than "a human disagrees with it".

The motivating case is ``_ZN7fBase_cnwEj`` (``fBase_c::operator
new(unsigned int)``).  Every installed mwccarm rejects ``operator
new(unsigned int)`` outright -- CodeWarrior's ``size_t`` is ``unsigned long``,
so it will only accept ``m``.  (It was spelled ``_ZN9ActorBasenwEj`` until the
class was renamed to its cartridge name in #1576; the rename carried the defect
forward under a new spelling, which is exactly what ``--check`` caught.)  ``j`` is the *GCC* spelling, inherited from
upstream projects built with a GCC ARM EABI toolchain.  Note carefully that
``Ej`` is perfectly correct for an ordinary ``u32`` parameter; the defect is
confined to the positions where the compiler validates the type against its
own ``size_t``.

## What this proves, and what it does NOT

PROVES: the name is spellable by the pinned compiler.  A ``REJECTED`` or
``MISMATCH`` verdict is a hard defect.

DOES NOT PROVE: that the types are *right*.  ``void f(unsigned int)`` and
``void f(int)`` both compile; only a byte-match of the callee body can choose
between them.  A ``ROUND-TRIPS`` verdict means "well-formed", not "correct".
Read ``notes/symbol-name-provenance.md`` before reading more into it.

Also note that Itanium mangling does not encode the return type (outside
templates) and does not distinguish a static member from an instance member,
so this check is blind to both by construction.

## Usage

    python tools/name_roundtrip.py                       # audit everything
    python tools/name_roundtrip.py --json out.json
    python tools/name_roundtrip.py --symbol _ZN7fBase_cnwEj --verbose
    python tools/name_roundtrip.py --check                # CI mode
    python tools/name_roundtrip.py --emit _ZN7fBase_cnwEj   # show the TU

``--check`` compares the current failure set against
``config/name-roundtrip-baseline.json`` and exits non-zero when a symbol that
used to round-trip stops doing so, or when a known-bad symbol is not in the
baseline.  Fixing a symbol is never a failure; it asks you to refresh the
baseline with ``--write-baseline``.
"""
from __future__ import annotations

import argparse
import collections
import concurrent.futures
import io
import json
import os
import pathlib
import re
import subprocess
import sys
import tempfile

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import match as M                                                     # noqa: E402
from mangle import defined_symbols                                    # noqa: E402
from rombuild import CFLAGS                                           # noqa: E402

# build_pin.flags_for() picks these for any //cpp source; a synthesised TU is
# by definition C++, so take the C++ branch unconditionally.
CPP_FLAGS = CFLAGS.replace("-lang c99", "-lang c++")
VERSION = M.CANONICAL

BASELINE = REPO / "config" / "name-roundtrip-baseline.json"

ROUND_TRIPS = "ROUND-TRIPS"
MISMATCH = "MISMATCH"
REJECTED = "REJECTED"
UNTESTABLE = "UNTESTABLE"


# --------------------------------------------------------------------------
# Itanium parser
#
# Deliberately strict.  Anything it does not fully understand raises Untestable
# rather than guessing, because a guess here manufactures a false MISMATCH and
# a false MISMATCH is worse than an honest gap.
# --------------------------------------------------------------------------

class Untestable(Exception):
    pass


BUILTIN = {
    "v": "void", "b": "bool", "c": "char", "a": "signed char",
    "h": "unsigned char", "s": "short", "t": "unsigned short",
    "i": "int", "j": "unsigned int", "l": "long", "m": "unsigned long",
    "x": "long long", "y": "unsigned long long", "f": "float",
    "d": "double", "e": "long double", "w": "wchar_t", "z": "...",
}

OPERATORS = {
    "nw": "operator new", "na": "operator new[]",
    "dl": "operator delete", "da": "operator delete[]",
    "ps": "operator+", "ng": "operator-", "ad": "operator&", "de": "operator*",
    "co": "operator~", "pl": "operator+", "mi": "operator-", "ml": "operator*",
    "dv": "operator/", "rm": "operator%", "an": "operator&", "or": "operator|",
    "eo": "operator^", "aS": "operator=", "pL": "operator+=", "mI": "operator-=",
    "mL": "operator*=", "dV": "operator/=", "rM": "operator%=", "aN": "operator&=",
    "oR": "operator|=", "eO": "operator^=", "ls": "operator<<", "rs": "operator>>",
    "lS": "operator<<=", "rS": "operator>>=", "eq": "operator==", "ne": "operator!=",
    "lt": "operator<", "gt": "operator>", "le": "operator<=", "ge": "operator>=",
    "nt": "operator!", "aa": "operator&&", "oo": "operator||", "pp": "operator++",
    "mm": "operator--", "cm": "operator,", "pm": "operator->*", "pt": "operator->",
    "cl": "operator()", "ix": "operator[]", "qu": "operator?",
}

# Operators whose arity the language fixes.  Used to decide how to declare
# them (member vs free, and how many parameters are legal).
UNARY_ONLY = {"operator~", "operator!", "operator++", "operator--"}


# ---- type model ----------------------------------------------------------

class Type:
    """A C++ type that can spell itself around a declarator."""
    is_builtin = False

    def decl(self, inner: str) -> str:
        raise NotImplementedError

    def spell(self) -> str:
        return self.decl("").strip()

    def names(self):
        """Every class-ish type name this type mentions, with completeness."""
        return []


class Builtin(Type):
    is_builtin = True

    def __init__(self, code):
        self.code = code
        self.text = BUILTIN[code]

    def decl(self, inner):
        return f"{self.text} {inner}".rstrip()


class Named(Type):
    """A class/struct/enum/union type, possibly nested: A::B::C."""

    def __init__(self, parts, template_args=None):
        self.parts = list(parts)          # ["LVL_Overlay", "ObjTable"]
        self.template_args = template_args  # list[Type] or None

    @property
    def text(self):
        base = "::".join(self.parts)
        if self.template_args is not None:
            args = ", ".join(a.spell() for a in self.template_args)
            return f"{base}<{args}>"
        return base

    def decl(self, inner):
        return f"{self.text} {inner}".rstrip()

    def names(self):
        out = [(tuple(self.parts), bool(self.template_args))]
        for a in (self.template_args or []):
            out.extend(a.names())
        return out


class Ptr(Type):
    def __init__(self, inner):
        self.inner = inner

    def decl(self, d):
        return self.inner.decl(f"*{d}")

    def names(self):
        return self.inner.names()


class Ref(Type):
    def __init__(self, inner, rvalue=False):
        self.inner = inner
        self.rvalue = rvalue

    def decl(self, d):
        return self.inner.decl(("&&" if self.rvalue else "&") + d)

    def names(self):
        return self.inner.names()


class CV(Type):
    def __init__(self, inner, quals):
        self.inner = inner
        self.quals = quals            # e.g. "const", "const volatile"

    def decl(self, d):
        # const applies to the pointee for `const T *`, so spell it as a
        # prefix on the underlying type.
        return f"{self.quals} {self.inner.decl(d)}"

    def names(self):
        return self.inner.names()


class FuncType(Type):
    def __init__(self, ret, params):
        self.ret = ret
        self.params = params

    def decl(self, d):
        args = ", ".join(p.spell() for p in self.params) or "void"
        if not d.startswith("*") and not d.startswith("&"):
            # A bare function type in a parameter position decays; keep it
            # legal by spelling it as the function type it is.
            return self.ret.decl(f"{d}({args})")
        return self.ret.decl(f"({d})({args})")

    def names(self):
        out = list(self.ret.names())
        for p in self.params:
            out.extend(p.names())
        return out


class MemberPtr(Type):
    def __init__(self, cls, target):
        self.cls = cls
        self.target = target

    def decl(self, d):
        inner = f"{self.cls.text}::*{d}"
        if isinstance(self.target, FuncType):
            args = ", ".join(p.spell() for p in self.target.params) or "void"
            return self.target.ret.decl(f"({inner})({args})")
        return self.target.decl(inner)

    def names(self):
        return list(self.cls.names()) + list(self.target.names())


class Array(Type):
    def __init__(self, n, inner):
        self.n = n
        self.inner = inner

    def decl(self, d):
        return self.inner.decl(f"{d}[{self.n if self.n is not None else ''}]")

    def names(self):
        return self.inner.names()


# ---- the parser ----------------------------------------------------------

class Parser:
    def __init__(self, s):
        self.s = s
        self.i = 0
        self.subs = []          # substitution table, in Itanium order

    # -- primitives --
    def peek(self, n=1):
        return self.s[self.i:self.i + n]

    def eof(self):
        return self.i >= len(self.s)

    def expect(self, ch):
        if self.peek() != ch:
            raise Untestable(f"expected {ch!r} at offset {self.i} in {self.s!r}")
        self.i += 1

    def source_name(self):
        m = re.match(r"(\d+)", self.s[self.i:])
        if not m:
            raise Untestable(f"expected length-prefixed name at {self.i}")
        n = int(m.group(1))
        self.i += len(m.group(1))
        name = self.s[self.i:self.i + n]
        if len(name) != n:
            raise Untestable("truncated source-name")
        self.i += n
        return name

    def add_sub(self, obj):
        self.subs.append(obj)
        return obj

    # -- types --
    def parse_type(self):
        c = self.peek()
        if c == "":
            raise Untestable("type expected, hit end of string")

        if c in BUILTIN and not (c in "S" ):
            # A digit or 'S' can never be a builtin key, so this is safe.
            self.i += 1
            return Builtin(c)          # builtins are NOT substitution candidates

        if c == "S":
            # Goes through parse_class_type so `St9type_info` -- a substitution
            # immediately followed by an unqualified name -- resolves.
            return self.parse_class_type()

        if c == "P":
            self.i += 1
            return self.add_sub(Ptr(self.parse_type()))
        if c == "R":
            self.i += 1
            return self.add_sub(Ref(self.parse_type()))
        if c == "O":
            self.i += 1
            return self.add_sub(Ref(self.parse_type(), rvalue=True))
        if c in "KVr":
            quals = []
            while self.peek() in "KVr":
                quals.append({"K": "const", "V": "volatile", "r": "restrict"}[self.peek()])
                self.i += 1
            # Itanium order is <restrict><volatile><const>; spell them plainly.
            inner = self.parse_type()
            return self.add_sub(CV(inner, " ".join(reversed(quals))))
        if c == "F":
            self.i += 1
            if self.peek() == "Y":
                raise Untestable("extern \"C\" function type (FY)")
            ret = self.parse_type()
            params = []
            while self.peek() not in ("E", ""):
                params.append(self.parse_type())
            self.expect("E")
            if len(params) == 1 and isinstance(params[0], Builtin) and params[0].code == "v":
                params = []
            return self.add_sub(FuncType(ret, params))
        if c == "M":
            self.i += 1
            cls = self.parse_type()
            target = self.parse_type()
            return self.add_sub(MemberPtr(cls, target))
        if c == "A":
            self.i += 1
            m = re.match(r"(\d*)_", self.s[self.i:])
            if not m:
                raise Untestable("array bound is an expression")
            self.i += len(m.group(0))
            n = int(m.group(1)) if m.group(1) else None
            return self.add_sub(Array(n, self.parse_type()))
        if c == "T":
            raise Untestable("template parameter (T_) -- needs the template context")
        if c == "U":
            raise Untestable("vendor extended type qualifier (U)")
        if c == "D":
            raise Untestable(f"C++11 extended builtin/decltype ({self.peek(2)})")
        if c == "C" or c == "G":
            raise Untestable("complex/imaginary type")
        if c == "N" or c.isdigit():
            return self.parse_class_type()
        if c == "Z":
            raise Untestable("local (Z) scope type")
        raise Untestable(f"unhandled type code {c!r} at offset {self.i} in {self.s!r}")

    def parse_template_args(self):
        self.expect("I")
        args = []
        while self.peek() not in ("E", ""):
            if self.peek() in "LXJ":
                raise Untestable("non-type / expression / pack template argument")
            args.append(self.parse_type())
        self.expect("E")
        if not args:
            raise Untestable("empty template argument list")
        return args

    def parse_class_type(self):
        """<class-enum-type> as a TYPE (adds the right substitutions)."""
        if self.peek() == "N":
            parts, targs, quals = self.parse_nested(as_type=True)
            if quals:
                raise Untestable("cv-qualified nested type name")
            return Named(parts, targs)
        if self.peek(2) == "St":
            # `St` is the ::std namespace abbreviation, and it is the ONLY
            # substitution that may be followed by an unqualified name.  A
            # back-reference such as S_ or S0_ is already a complete type, so
            # letting it absorb the next component silently eats a parameter.
            self.i += 2
            if not self.peek().isdigit():
                raise Untestable("bare St with no member name")
            node = Named(["std", self.source_name()])
            self.add_sub(node)
            return node
        if self.peek() == "S":
            return self.parse_substitution()
        name = self.source_name()
        node = Named([name])
        self.add_sub(node)
        if self.peek() == "I":
            targs = self.parse_template_args()
            inst = Named([name], targs)
            self.add_sub(inst)
            return inst
        return node

    def parse_nested(self, as_type):
        """Parse N [cv] <prefix...> <unqualified> E.

        Returns (parts, template_args_of_last, cv_quals).  Every strict prefix
        is registered as a substitution candidate; the full name is too when it
        is being used as a type.
        """
        self.expect("N")
        quals = []
        while self.peek() in "KVr":
            quals.append({"K": "const", "V": "volatile", "r": "restrict"}[self.peek()])
            self.i += 1
        if self.peek() in "RO":
            raise Untestable("ref-qualified member function (C++11)")

        parts = []
        last_targs = None
        while self.peek() not in ("E", ""):
            if self.peek(2) == "St":
                self.i += 2
                parts.append("std")
                continue
            if self.peek() == "S":
                sub = self.parse_substitution(prefix_context=True)
                if not isinstance(sub, Named):
                    raise Untestable("substitution in a name prefix is not a class name")
                parts.extend(sub.parts)
                if sub.template_args is not None:
                    last_targs = sub.template_args
                continue
            if self.peek().isdigit():
                parts.append(self.source_name())
            elif self.peek(2) in ("C1", "C2", "C3"):
                parts.append("@" + self.peek(2))
                self.i += 2
            elif self.peek(2) in ("D0", "D1", "D2"):
                parts.append("@" + self.peek(2))
                self.i += 2
            elif self.peek(2) == "cv":
                self.i += 2
                target = self.parse_type()
                parts.append("@cv:" + target.spell())
            elif self.peek(2) in OPERATORS:
                parts.append("@op:" + OPERATORS[self.peek(2)])
                self.i += 2
            elif self.peek() == "L":
                self.i += 1     # internal-linkage marker, no effect on the name
                continue
            elif self.peek() == "U":
                raise Untestable("unnamed/closure type in a nested name")
            elif self.peek() == "T":
                raise Untestable("template parameter in a nested name")
            else:
                raise Untestable(f"unhandled nested-name component {self.peek(2)!r}"
                                 f" at offset {self.i} in {self.s!r}")
            # A template-args list may follow any component.
            if self.peek() == "I":
                targs = self.parse_template_args()
                # register the un-instantiated prefix, then the instance
                self.add_sub(Named(list(parts)))
                inst = Named(list(parts), targs)
                if self.peek() != "E":
                    self.add_sub(inst)
                last_targs = targs
                continue
            if self.peek() != "E":
                self.add_sub(Named(list(parts)))
        self.expect("E")
        if as_type:
            self.add_sub(Named(list(parts), last_targs))
        return parts, last_targs, quals

    def parse_substitution(self, prefix_context=False):
        self.i += 1                                  # consume 'S'
        c = self.peek()
        if c in "tsiodab":
            raise Untestable(f"std library-type substitution abbreviation S{c}")
        m = re.match(r"([0-9A-Z]*)_", self.s[self.i:])
        if not m:
            raise Untestable(f"malformed substitution at offset {self.i}")
        seq = m.group(1)
        self.i += len(m.group(0))
        idx = 0 if seq == "" else int(seq, 36) + 1
        if idx >= len(self.subs):
            raise Untestable(f"substitution S{seq}_ out of range "
                             f"({idx} >= {len(self.subs)})")
        return self.subs[idx]


# ---- the symbol model ----------------------------------------------------

class Sym:
    def __init__(self, mangled):
        self.mangled = mangled
        self.kind = None          # "function" | "vtable" | "typeinfo" | "typeinfo-name"
        self.scope = []           # enclosing class/namespace components
        self.func = None          # unqualified function name (or @C1/@D0/@op:...)
        self.params = []          # list[Type]
        self.const = False
        self.type_name = None     # Named, for _ZTV/_ZTI/_ZTS
        self.template_args = None
        self.thunk_offset = None


SPECIAL = {"_ZTV": "vtable", "_ZTI": "typeinfo", "_ZTS": "typeinfo-name",
           "_ZTT": "VTT"}


def parse_symbol(mangled: str) -> Sym:
    if not mangled.startswith("_Z"):
        raise Untestable("not an Itanium mangled name")

    sym = Sym(mangled)

    # `_ZThn<offset>_` is followed by a bare <encoding>, with no second _Z.
    m = re.match(r"_ZThn(\d+)_(.+)$", mangled)
    if m:
        # A non-virtual thunk is emitted for an override reached through a
        # secondary base.  We do not know the real hierarchy, but we do not
        # need it: a synthetic first base of exactly <offset> bytes puts the
        # second base at the same this-adjustment, and the thunk's spelling
        # depends only on that number and on the target encoding.
        inner = parse_symbol("_Z" + m.group(2))
        if inner.kind != "function" or not inner.func.startswith("@D"):
            raise Untestable("thunk to something other than a destructor -- "
                             "the synthetic hierarchy only covers dtor thunks")
        if len(inner.scope) != 1:
            raise Untestable("thunk on a nested class -- not synthesised")
        sym.kind = "thunk"
        sym.thunk_offset = int(m.group(1))
        sym.scope = inner.scope
        sym.func = inner.func
        return sym
    if mangled.startswith("_ZTv") or mangled.startswith("_ZTc"):
        raise Untestable("virtual/covariant thunk -- needs the real class layout")
    if mangled.startswith("_ZTh"):
        raise Untestable("non-virtual thunk with a non-numeric offset")
    if mangled.startswith("_ZGV"):
        raise Untestable("guard variable")
    if mangled.startswith("_ZZ"):
        raise Untestable("function-local entity (_ZZ)")

    head = mangled[:4]
    if head in SPECIAL:
        if head == "_ZTT":
            raise Untestable("VTT -- needs virtual inheritance")
        p = Parser(mangled[4:])
        ty = p.parse_type()
        if not p.eof():
            raise Untestable(f"trailing text after special-name type: {p.s[p.i:]!r}")
        if not isinstance(ty, Named):
            raise Untestable("special name does not name a class type")
        sym.kind = SPECIAL[head]
        sym.type_name = ty
        return sym

    body = mangled[2:]
    p = Parser(body)
    p.i = 0
    if p.peek() == "N":
        parts, targs, quals = p.parse_nested(as_type=False)
        if "volatile" in quals:
            raise Untestable("volatile member function")
        sym.const = "const" in quals
        sym.scope = parts[:-1]
        sym.func = parts[-1]
        sym.template_args = targs if len(parts) == 1 or targs else None
    elif p.peek() == "L":
        raise Untestable("internal-linkage (_ZL) name")
    elif p.peek().isdigit():
        sym.scope = []
        sym.func = p.source_name()
        if p.peek() == "I":
            raise Untestable("function template specialisation")
    elif p.peek(2) in OPERATORS:
        sym.scope = []
        sym.func = "@op:" + OPERATORS[p.peek(2)]
        p.i += 2
    else:
        raise Untestable(f"unhandled encoding prefix {p.peek(2)!r}")

    if p.eof():
        # No bare-function-type: a static data member, or a variable in a
        # namespace.  Those mangle identically, so either synthesis proves the
        # spelling.
        if not sym.scope:
            raise Untestable("unscoped name with no function type -- a global "
                             "variable is not mangled at all")
        if sym.func.startswith("@"):
            raise Untestable(f"special name {sym.func} with no function type")
        sym.kind = "data"
        return sym

    sym.kind = "function"
    while not p.eof():
        sym.params.append(p.parse_type())
    if len(sym.params) == 1 and isinstance(sym.params[0], Builtin) \
            and sym.params[0].code == "v":
        sym.params = []
    if any(isinstance(t, Builtin) and t.code == "z" for t in sym.params[:-1]):
        raise Untestable("varargs marker in a non-final position")
    return sym


# --------------------------------------------------------------------------
# Source synthesis
# --------------------------------------------------------------------------

class ClassNode:
    def __init__(self, name):
        self.name = name
        self.children = {}
        self.complete = False      # needs a body (has members, children, or is by-value)
        self.members = []          # in-class declaration strings
        self.templated = False


class TU:
    """Accumulates the declarations one or more symbols need, then emits C++."""

    def __init__(self):
        self.roots = {}
        self.out_of_line = []
        self.template_names = set()

    def node(self, parts, complete=False):
        table = self.roots
        node = None
        for part in parts:
            if part not in table:
                table[part] = ClassNode(part)
            node = table[part]
            table = node.children
        if node is None:
            raise Untestable("empty qualified name")
        if complete:
            node.complete = True
        return node

    def need_type(self, ty: Type, by_value: bool):
        for parts, templated in ty.names():
            # Every enclosing component must be a complete type to be a scope.
            for depth in range(1, len(parts)):
                self.node(parts[:depth], complete=True)
            node = self.node(parts, complete=by_value)
            if templated:
                node.templated = True
                self.template_names.add(parts)

    # -- emission --
    def _emit_node(self, node, indent, lines, path):
        if not path and node.name == "std":
            # ::std must be a real namespace, because that is the only thing
            # the `St` substitution abbreviates.  Declaring `struct std`
            # instead would make the compiler spell it N3std...E.
            lines.append(f"{indent}namespace std {{")
            for child in node.children.values():
                self._emit_node(child, indent + "  ", lines, path + ["std"])
            for member in node.members:
                lines.append(f"{indent}  {member}")
            lines.append(f"{indent}}}")
            return
        if node.templated:
            # A class template used as a type: one type parameter is enough,
            # because the mangling only needs the template-id spelling.
            lines.append(f"{indent}template <class _T0> struct {node.name} "
                         "{ char _pad; };")
            return
        if not node.complete and not node.children and not node.members:
            lines.append(f"{indent}struct {node.name};")
            return
        lines.append(f"{indent}struct {node.name} {{")
        for child in node.children.values():
            self._emit_node(child, indent + "  ", lines, path + [node.name])
        for member in node.members:
            lines.append(f"{indent}  {member}")
        if not node.children and not node.members:
            lines.append(f"{indent}  char _pad;")
        lines.append(f"{indent}}};")

    def text(self):
        lines = ["//cpp -- synthesised by tools/name_roundtrip.py"]
        # Forward-declare every top-level name first.  A member function
        # signature routinely mentions a class that sorts after its own scope
        # (`MeshColliderBase::f(Actor *)`), and without this pass the compiler
        # reports "undefined identifier" -- a synthesiser artefact that reads
        # exactly like a real defect.
        for root in self.roots.values():
            if not root.templated and root.name != "std":
                lines.append(f"struct {root.name};")
        for root in self.roots.values():
            self._emit_node(root, "", lines, [])
        lines.extend(self.out_of_line)
        return "\n".join(lines) + "\n"


def _param_list(params):
    if not params:
        return "void"
    return ", ".join(
        ("..." if isinstance(p, Builtin) and p.code == "z" else p.spell())
        for p in params)


def synthesise(sym: Sym, tu: TU) -> None:
    """Add to `tu` whatever declarations make the pinned compiler emit `sym`."""
    if sym.kind in ("vtable", "typeinfo", "typeinfo-name"):
        parts = sym.type_name.parts
        if sym.type_name.template_args is not None:
            raise Untestable("special name on a template instantiation")
        for depth in range(1, len(parts)):
            tu.node(parts[:depth], complete=True)
        node = tu.node(parts, complete=True)
        # A polymorphic class with an out-of-line key function makes the
        # compiler emit the vtable (and, where it emits them at all, the
        # type_info records) in this TU.
        node.members.append("virtual void _key();")
        node.members.append(f"virtual ~{node.name}();")
        qual = "::".join(parts)
        tu.out_of_line.append(f"void {qual}::_key() {{}}")
        tu.out_of_line.append(f"{qual}::~{node.name}() {{}}")
        return

    if sym.kind == "data":
        for depth in range(1, len(sym.scope) + 1):
            tu.node(sym.scope[:depth], complete=True)
        node = tu.node(sym.scope, complete=True)
        node.members.append(f"static int {sym.func};")
        tu.out_of_line.append(f"int {'::'.join(sym.scope)}::{sym.func} = 1;")
        return

    if sym.kind == "thunk":
        base0 = f"_rtb0_{sym.thunk_offset}"
        cls = sym.scope[-1]
        pad = sym.thunk_offset - 4          # one vtable pointer, then filler
        if pad < 0 or sym.thunk_offset % 4:
            raise Untestable(f"thunk offset {sym.thunk_offset} is not a "
                             "reachable secondary-base offset")
        tu.out_of_line.insert(0, "")        # keep ordering readable
        pre = []
        pre.append(f"struct {base0} {{ virtual void _f0(); char _pad[{pad}]; }};")
        pre.append("struct _rtb1 { virtual ~_rtb1(); };")
        for depth in range(1, len(sym.scope)):
            tu.node(sym.scope[:depth], complete=True)
        qual = "::".join(sym.scope)
        pre.append(f"struct {cls} : {base0}, _rtb1 {{ virtual ~{cls}(); }};")
        pre.append(f"{qual}::~{cls}() {{}}")
        tu.out_of_line.extend(pre)
        return

    if sym.template_args is not None:
        raise Untestable("template specialisation")

    for p in sym.params:
        by_value = not isinstance(p, (Ptr, Ref))
        if isinstance(p, CV):
            by_value = not isinstance(p.inner, (Ptr, Ref))
        tu.need_type(p, by_value=by_value)

    args = _param_list(sym.params)
    func = sym.func
    const = " const" if sym.const else ""

    if not sym.scope:
        # A free function.  Operator functions at namespace scope have arity
        # rules; the synthesiser only handles the global allocation operators,
        # which are exactly the interesting ones.
        if func.startswith("@op:"):
            op = func[4:]
            if op in ("operator new", "operator new[]"):
                tu.out_of_line.append(f"void *{op}({args}) {{ return 0; }}")
                return
            if op in ("operator delete", "operator delete[]"):
                tu.out_of_line.append(f"void {op}({args}) {{}}")
                return
            raise Untestable(f"free operator function {op} -- arity/type rules "
                             "make a synthetic declaration unreliable")
        if func.startswith("@"):
            raise Untestable(f"unscoped special name {func}")
        tu.out_of_line.append(f"void {func}({args}) {{}}")
        return

    for depth in range(1, len(sym.scope) + 1):
        tu.node(sym.scope[:depth], complete=True)
    node = tu.node(sym.scope, complete=True)
    cls = node.name
    qual = "::".join(sym.scope)

    if func.startswith("@C"):
        node.members.append(f"{cls}({args});")
        tu.out_of_line.append(f"{qual}::{cls}({args}) {{}}")
        return
    if func.startswith("@D"):
        if sym.params:
            raise Untestable("destructor with parameters")
        # Declaring it virtual is what makes D0 exist at all; the compiler
        # emits the whole variant set and membership is what we test.
        node.members.append(f"virtual ~{cls}();")
        tu.out_of_line.append(f"{qual}::~{cls}() {{}}")
        return
    if func.startswith("@cv:"):
        target = func[4:]
        node.members.append(f"operator {target}(){const};")
        tu.out_of_line.append(f"{qual}::operator {target}(){const} "
                              "{ return *(" + target + " *)0; }")
        return
    if func.startswith("@op:"):
        op = func[4:]
        if op in ("operator new", "operator new[]"):
            node.members.append(f"void *{op}({args});")
            tu.out_of_line.append(f"void *{qual}::{op}({args}) {{ return 0; }}")
            return
        if op in ("operator delete", "operator delete[]"):
            node.members.append(f"void {op}({args});")
            tu.out_of_line.append(f"void {qual}::{op}({args}) {{}}")
            return
        # Ordinary operator overloads: the language fixes their arity, and a
        # member operator takes one fewer argument than the free form.
        node.members.append(f"void {op}({args}){const};")
        tu.out_of_line.append(f"void {qual}::{op}({args}){const} {{}}")
        return

    node.members.append(f"void {func}({args}){const};")
    tu.out_of_line.append(f"void {qual}::{func}({args}){const} {{}}")


# --------------------------------------------------------------------------
# Compiler oracle
# --------------------------------------------------------------------------

def compile_text(text: str, version: str = VERSION):
    """Compile a synthesised TU.  Returns (symbols|None, diagnostic)."""
    with tempfile.TemporaryDirectory() as td:
        src = pathlib.Path(td) / "rt.cpp"
        src.write_text(text, encoding="utf-8")
        buf = io.StringIO()
        exe = M.MW / version / "mwccarm.exe"
        if not exe.is_file():
            return None, f"no compiler installed at {version}"
        out_o = pathlib.Path(td) / "rt.o"
        env = dict(os.environ, LM_LICENSE_FILE=str(M.LICENSE))
        launcher = os.environ.get("MWCCARM_LAUNCHER", "").split()
        cmd = [*launcher, str(exe), *CPP_FLAGS.split(),
               "-i", str(M.INCLUDE.resolve()), "-c", str(src), "-o", str(out_o)]
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, env=env,
                               timeout=120)
        except subprocess.TimeoutExpired:
            return None, "compile timed out"
        if r.returncode != 0 or not out_o.is_file():
            detail = "\n".join(s for s in (r.stdout.strip(), r.stderr.strip()) if s)
            return None, detail
        del buf
        return defined_symbols(out_o.read_bytes()), ""


_NOISE = ("Environment variable 'MWCIncludes' not found",
          "Errors caused tool to abort.")


def _first_error(diag: str) -> str:
    """The informative line of an mwccarm failure.

    mwccarm prints a licence/include warning first and 'Errors caused tool to
    abort.' last; the middle line is the one that names the defect, and taking
    the first or last line hides it.
    """
    lines = [ln.strip() for ln in diag.splitlines() if ln.strip()]
    lines = [ln for ln in lines if not any(n in ln for n in _NOISE)]
    # Drop the temp-file path prefix so verdicts are stable across runs.
    cleaned = [re.sub(r"^\S*rt\.cpp:", "line ", ln) for ln in lines]
    return cleaned[0] if cleaned else "compile failed"


# --------------------------------------------------------------------------
# The audit
# --------------------------------------------------------------------------

def load_symbols(config_root: pathlib.Path):
    """Every mangled name in config/**/symbols.txt, with where it came from."""
    rows = {}
    for path in sorted(config_root.rglob("symbols.txt")):
        module = path.parent.name
        for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            fields = line.split()
            name = fields[0]
            if not name.startswith("_Z"):
                continue
            addr = None
            kind = None
            for f in fields[1:]:
                if f.startswith("addr:"):
                    addr = f[5:]
                elif f.startswith("kind:"):
                    kind = f[5:]
            rows.setdefault(name, {"name": name, "module": module,
                                   "addr": addr, "kind": kind,
                                   "path": str(path.relative_to(config_root.parent))})
    return rows


def audit_one(name: str, version: str = VERSION):
    """Verdict for one symbol, compiled on its own.  Slow but unambiguous."""
    try:
        sym = parse_symbol(name)
        tu = TU()
        synthesise(sym, tu)
    except Untestable as exc:
        return {"name": name, "verdict": UNTESTABLE, "reason": str(exc),
                "emitted": [], "source": ""}
    text = tu.text()
    syms, diag = compile_text(text, version)
    if syms is None:
        return {"name": name, "verdict": REJECTED, "reason": _first_error(diag),
                "emitted": [], "source": text, "diagnostic": diag}
    emitted = [s.name for s in syms if s.name.startswith("_Z")]
    if name in emitted:
        return {"name": name, "verdict": ROUND_TRIPS, "reason": "",
                "emitted": emitted, "source": text}
    return {"name": name, "verdict": MISMATCH,
            "reason": f"emitted {emitted}, claimed {name}",
            "emitted": emitted, "source": text}


def audit_batch(names, version=VERSION, batch=48):
    """Audit many symbols, batching compiles and bisecting on failure."""
    results = {}
    pending = []
    for name in names:
        try:
            sym = parse_symbol(name)
            tu = TU()
            synthesise(sym, tu)
        except Untestable as exc:
            results[name] = {"name": name, "verdict": UNTESTABLE,
                             "reason": str(exc), "emitted": []}
            continue
        pending.append((name, tu))

    def run(group):
        """Try one compile for the whole group; fall back to singles."""
        if not group:
            return
        merged = TU()
        try:
            for name, _ in group:
                synthesise(parse_symbol(name), merged)
        except Untestable:
            for name, _ in group:
                results[name] = audit_one(name, version)
            return
        syms, _diag = compile_text(merged.text(), version)
        if syms is not None:
            emitted = {s.name for s in syms}
            missing = [n for n, _ in group if n not in emitted]
            for name, _ in group:
                if name in emitted:
                    results[name] = {"name": name, "verdict": ROUND_TRIPS,
                                     "reason": "", "emitted": [name]}
            if not missing:
                return
            group = [(n, t) for n, t in group if n in missing]
        if len(group) == 1:
            results[group[0][0]] = audit_one(group[0][0], version)
            return
        half = len(group) // 2
        run(group[:half])
        run(group[half:])

    groups = [pending[i:i + batch] for i in range(0, len(pending), batch)]
    workers = min(8, (os.cpu_count() or 4))
    with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as pool:
        list(pool.map(run, groups))
    return results


# --------------------------------------------------------------------------
# CLI
# --------------------------------------------------------------------------

def rtti_triple_conflicts(info):
    """Classes whose _ZTV / _ZTI / _ZTS disagree about the class name.

    The round-trip cannot see this: `_ZTV5Enemy` and `_ZTI12dEnemyBase_c` are
    each perfectly well-formed on their own.  But one class has one name, so a
    compiler cannot emit that pair -- the defect only exists BETWEEN symbols.

    Two shapes are reported.  `duplicate` is two records of the same kind at one
    address, which is also the shape that makes the attribution gate say
    "lost N matched function(s)" because the last line wins.  `split` is a class
    whose vtable and typeinfo carry different names.
    """
    by_addr = collections.defaultdict(list)
    for name, meta in info.items():
        if name[:4] in ("_ZTV", "_ZTI", "_ZTS") and meta.get("addr"):
            by_addr[(meta["module"], meta["addr"])].append(name)

    duplicates = []
    for key, names in sorted(by_addr.items()):
        kinds = collections.defaultdict(list)
        for n in names:
            kinds[n[:4]].append(n)
        for head, group in kinds.items():
            if len(group) > 1:
                duplicates.append((key[0], key[1], head, sorted(group)))

    # A `_ZTS` suffix is the literal type-name string the cartridge stores, so
    # the typeinfo names are the trustworthy spelling of a class.  A vtable
    # naming a class that has no typeinfo under the same spelling is therefore
    # the vtable's problem: the two cannot both be right, and only one of them
    # was read out of the ROM.
    typed = {name[4:] for name in info if name[:4] in ("_ZTI", "_ZTS")}
    orphans = []
    for name, meta in sorted(info.items()):
        if name[:4] != "_ZTV":
            continue
        if name[4:] not in typed:
            orphans.append((meta.get("module"), meta.get("addr"), name))
    return duplicates, orphans


_VARIANT = re.compile(r"(C[123]|D[012])E")
_SUBSTITUTION = re.compile(r"S[0-9A-Z]*_")

_ALLOC_NAMES = {"operator new", "operator new[]",
                "operator delete", "operator delete[]"}


def _is_alloc_operator(name):
    """True when the symbol names one of the allocation operators.

    Decided by parsing, not by a substring test: `nwE` also occurs inside
    ordinary identifiers, and a lookbehind on letters gets `fBase_c` + `nw`
    exactly backwards.
    """
    try:
        sym = parse_symbol(name)
    except Untestable:
        return False
    return (sym.func or "").startswith("@op:") \
        and sym.func[4:] in _ALLOC_NAMES


def _variant_of(name):
    m = _VARIANT.search(name)
    return m.group(1) if m else None


def classify_cause(row):
    """A short, mechanical cause label for a failing symbol.

    Every label here is a statement about MANGLING, not about whether the
    types are semantically right.  See the module docstring.
    """
    name = row["name"]
    emitted = row.get("emitted") or []
    if row["verdict"] == REJECTED:
        if _is_alloc_operator(name):
            return ("GCC size_t spelling: an allocation operator's size "
                    "argument is 'j' (unsigned int).  CodeWarrior's size_t "
                    "is unsigned long, so it will only accept 'm'")
        return "compiler rejects the declaration"
    if row["verdict"] == MISMATCH:
        variant = _variant_of(name)
        if variant in ("C3",):
            return ("constructor variant C3 (the allocating constructor); "
                    "mwccarm emits C1 and C2 only, so no build can have "
                    "produced this name")
        claimed_subs = len(_SUBSTITUTION.findall(name))
        if any(len(_SUBSTITUTION.findall(e)) > claimed_subs for e in emitted):
            return ("Itanium substitution not applied: the name spells a type "
                    "out in full where the ABI requires an S_/S0_ back-"
                    "reference.  Hand-derived, not compiler-derived")
        if len(emitted) == 1:
            a, b = name, emitted[0]
            if len(a) == len(b) and sum(x != y for x, y in zip(a, b)) == 1:
                i = next(i for i, (x, y) in enumerate(zip(a, b)) if x != y)
                return f"one character differs at offset {i}: {a[i]!r} -> {b[i]!r}"
        return "different spelling"
    return "unknown"


def propose(row):
    """Candidate correct spellings, each of them compiler-derived.

    Returns (candidates, note).  More than one candidate means the fix is
    genuinely ambiguous from the mangling alone and needs other evidence --
    say so rather than picking.
    """
    name = row["name"]
    emitted = [e for e in (row.get("emitted") or []) if e.startswith("_Z")]

    if row["verdict"] == MISMATCH:
        want = _variant_of(name)
        if want:
            same = [e for e in emitted if _variant_of(e) == want]
            if len(same) == 1:
                return same, ""
            if want == "C3":
                return sorted(emitted), ("C3 is unproducible; the compiler "
                                         "offers C1 and C2 and the mangling "
                                         "alone cannot choose")
        if len(emitted) == 1:
            return emitted, ""
        return sorted(emitted), "several emitted names; none matches the variant"

    if row["verdict"] == REJECTED and _is_alloc_operator(name):
        # The only repair the ABI allows here is the size type, so try it and
        # let the compiler confirm rather than asserting the answer.
        seen = set()
        for cand in (re.sub(r"j$", "m", name), name.replace("Ej", "Em"),
                     re.sub(r"j", "m", name, count=1)):
            if cand == name or cand in seen:
                continue
            seen.add(cand)
            if audit_one(cand)["verdict"] == ROUND_TRIPS:
                return [cand], ""
    return [], "no mechanical repair found"


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--config", type=pathlib.Path, default=REPO / "config")
    ap.add_argument("--symbol", action="append", default=[],
                    help="audit only these symbols (repeatable)")
    ap.add_argument("--emit", metavar="SYMBOL",
                    help="print the synthesised TU for one symbol and stop")
    ap.add_argument("--version", default=VERSION)
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--batch", type=int, default=48)
    ap.add_argument("--json", type=pathlib.Path,
                    help="write the full per-symbol result set here")
    ap.add_argument("--verbose", action="store_true")
    ap.add_argument("--check", action="store_true",
                    help="CI mode: compare against the committed baseline")
    ap.add_argument("--write-baseline", action="store_true")
    ap.add_argument("--baseline", type=pathlib.Path, default=BASELINE)
    args = ap.parse_args(argv)

    if args.emit:
        try:
            tu = TU()
            synthesise(parse_symbol(args.emit), tu)
        except Untestable as exc:
            print(f"UNTESTABLE: {exc}")
            return 1
        print(tu.text())
        return 0

    info = load_symbols(args.config)
    names = args.symbol or sorted(info)
    if args.limit:
        names = names[:args.limit]

    if args.symbol:
        results = {n: audit_one(n, args.version) for n in names}
    else:
        results = audit_batch(names, args.version, args.batch)

    counts = collections.Counter(r["verdict"] for r in results.values())
    total = len(results)
    print(f"mangled symbols audited : {total}")
    for verdict in (ROUND_TRIPS, MISMATCH, REJECTED, UNTESTABLE):
        n = counts.get(verdict, 0)
        print(f"  {verdict:12} {n:6}  {100.0 * n / max(total, 1):5.1f}%")

    bad = {n: r for n, r in results.items()
           if r["verdict"] in (MISMATCH, REJECTED)}
    if bad:
        print(f"\n{len(bad)} symbol(s) the pinned compiler cannot produce:")
        for name in sorted(bad):
            row = bad[name]
            meta = info.get(name, {})
            print(f"  {name}")
            print(f"      module {meta.get('module', '?')} "
                  f"addr {meta.get('addr', '?')}")
            print(f"      {row['verdict']}: {row['reason'][:200]}")
            print(f"      cause: {classify_cause(row)}")
            cands, note = propose(row)
            if len(cands) == 1:
                print(f"      correct spelling (compiler-derived): {cands[0]}")
            elif cands:
                print(f"      AMBIGUOUS -- candidates: {', '.join(cands)}")
                print(f"      {note}")
            else:
                print(f"      no mechanical repair: {note}")
            row["proposed"] = cands
            row["cause"] = classify_cause(row)

    if not args.symbol:
        dups, orphans = rtti_triple_conflicts(info)
        print("\nRTTI record consistency (NOT a round-trip result -- these "
              "names are individually well-formed):")
        print(f"  addresses carrying two records of one kind : {len(dups)}")
        print(f"  _ZTV naming a class with no _ZTI/_ZTS      : {len(orphans)}")
        if args.verbose:
            for module, addr, head, group in dups:
                print(f"    {module} {addr} {head}: {' '.join(group)}")
            for module, addr, name in orphans:
                print(f"    {module} {addr} {name}")

    if args.verbose:
        why = collections.Counter(r["reason"] for r in results.values()
                                  if r["verdict"] == UNTESTABLE)
        if why:
            print("\nUNTESTABLE breakdown:")
            for reason, n in why.most_common():
                print(f"  {n:6}  {reason}")

    if args.json:
        payload = {}
        for name, row in results.items():
            entry = {k: v for k, v in row.items() if k != "source"}
            entry.update({k: v for k, v in info.get(name, {}).items()
                          if k != "name"})
            payload[name] = entry
        args.json.write_text(json.dumps(payload, indent=1, sort_keys=True),
                             encoding="utf-8")
        print(f"\nwrote {args.json}")

    if args.write_baseline:
        args.baseline.write_text(json.dumps({
            "_comment": "Symbols the pinned mwccarm cannot emit. Written by "
                        "tools/name_roundtrip.py --write-baseline. Every "
                        "'proposed' spelling below was produced by the "
                        "compiler, not hand-derived; more than one means the "
                        "mangling alone cannot choose.",
            "version": args.version,
            "flags": CPP_FLAGS,
            "failing": {n: {"verdict": r["verdict"], "reason": r["reason"],
                            "cause": r.get("cause", classify_cause(r)),
                            "module": info.get(n, {}).get("module"),
                            "addr": info.get(n, {}).get("addr"),
                            "proposed": r.get("proposed", propose(r)[0])}
                        for n, r in sorted(bad.items())},
        }, indent=1), encoding="utf-8")
        print(f"wrote baseline {args.baseline} ({len(bad)} failing)")
        return 0

    if args.check:
        if not args.baseline.is_file():
            print(f"\nFAIL: no baseline at {args.baseline}")
            return 1
        known = set(json.loads(args.baseline.read_text())["failing"])
        new = sorted(set(bad) - known)
        fixed = sorted(known - set(bad))
        if new:
            print(f"\nFAIL: {len(new)} symbol(s) newly unproducible by the pin:")
            for name in new:
                print(f"  {name}: {bad[name]['verdict']}: {bad[name]['reason'][:160]}")
            return 1
        if fixed:
            print(f"\nOK, and {len(fixed)} baseline symbol(s) now round-trip. "
                  f"Refresh with --write-baseline:")
            for name in fixed:
                print(f"  {name}")
        print("\nOK: no new unproducible symbols.")
        return 0

    return 0


if __name__ == "__main__":
    sys.exit(main())
