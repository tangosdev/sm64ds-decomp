"""A pragmatic Itanium C++ demangler, scoped to what the matching pipeline needs:
the class/method of a symbol and a best-effort argument list. The game's symbols
(e.g. _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_) ENCODE the class and every argument
type -- that is free context for the LLM tier and a free grouping key for
subsystem batching.

Handles:
  - Nested names (_ZN...E), free functions (_Z<len>...)
  - Constructors (C1/C2/C3), destructors (D0/D1/D2)
  - Non-virtual thunks (_ZThn<off>_...), virtual thunks (_ZTv<voff>_n<off>_...)
  - Operator overloads (new, delete, +, -, ==, etc.)
  - Templates with primary type extraction (Fix12<int> not just T)
  - Substitution back-references (S_, S0_, S1_, St)
  - Pointer/reference types, grouped const/volatile qualifiers, function pointers

API:
    demangle("_ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_") ->
        {"qualified": "Actor::SetRanges", "class": "Actor", "method": "SetRanges",
         "args": ["Fix12<int>", "Fix12<int>", "Fix12<int>", "Fix12<int>"],
         "nargs": 4, "ctor": False, "dtor": False,
         "thunk": False, "thunk_offset": None, "variant": None}
    Returns None for names that are not _Z-mangled (plain func_xxxx / C names).
"""
import re

BUILTIN = {
    "v": "void", "b": "bool", "c": "char", "a": "signed char", "h": "unsigned char",
    "s": "short", "t": "unsigned short", "i": "int", "j": "unsigned int",
    "l": "long", "m": "unsigned long", "x": "long long", "y": "unsigned long long",
    "f": "float", "d": "double", "w": "wchar_t",
}

# Itanium ABI operator codes
OPERATORS = {
    "nw": "operator new", "na": "operator new[]",
    "dl": "operator delete", "da": "operator delete[]",
    "ps": "operator+", "ng": "operator-",      # unary
    "ad": "operator&", "de": "operator*",       # unary
    "co": "operator~", "pl": "operator+", "mi": "operator-",
    "ml": "operator*", "dv": "operator/", "rm": "operator%",
    "an": "operator&", "or": "operator|", "eo": "operator^",
    "aS": "operator=", "pL": "operator+=", "mI": "operator-=",
    "mL": "operator*=", "dV": "operator/=", "rM": "operator%=",
    "aN": "operator&=", "oR": "operator|=", "eO": "operator^=",
    "ls": "operator<<", "rs": "operator>>",
    "lS": "operator<<=", "rS": "operator>>=",
    "eq": "operator==", "ne": "operator!=",
    "lt": "operator<", "gt": "operator>",
    "le": "operator<=", "ge": "operator>=",
    "nt": "operator!", "aa": "operator&&", "oo": "operator||",
    "pp": "operator++", "mm": "operator--",
    "cm": "operator,", "pm": "operator->*", "pt": "operator->",
    "cl": "operator()", "ix": "operator[]",
    "qu": "operator?",
    "cv": "operator <cast>",  # conversion operator, type follows
}


def _read_len_name(s, i):
    """Read a length-prefixed name: <digits><name>."""
    m = re.match(r"\d+", s[i:])
    if not m:
        return None, i
    n = int(m.group())
    i += len(m.group())
    return s[i:i + n], i + n


def _read_template_args(s, i, subs):
    """Read template arguments I...E and return a list of type strings.

    s[i] must be 'I'. Returns (arg_list, next_i).
    """
    assert s[i] == "I"
    i += 1  # skip 'I'
    args = []
    while i < len(s) and s[i] != "E":
        t, i = _read_type(s, i, subs)
        if t is not None:
            args.append(t)
        else:
            break
    if i < len(s) and s[i] == "E":
        i += 1  # skip closing 'E'
    return args, i



class _FunctionType(str):
    """Keep declarator structure while retaining the public string API."""

    def __new__(cls, result, args, declarator="", qualifiers="", linkage=""):
        value = cls._declare(result, args, declarator, qualifiers, linkage, "")
        obj = super().__new__(cls, value)
        obj.result, obj.args = result, tuple(args)
        obj.declarator, obj.qualifiers, obj.linkage = declarator, qualifiers, linkage
        return obj

    @staticmethod
    def _declare(result, args, declarator, qualifiers, linkage, name):
        name = declarator + name
        if declarator:
            name = "(" + name.rstrip() + ")"
        signature = name + "(" + (", ".join(args) if args else "void") + ")" + qualifiers
        if isinstance(result, _FunctionType):
            return result.declare(signature)
        return (linkage + str(result) + " " + signature).strip()

    def declare(self, name):
        return self._declare(self.result, self.args, self.declarator,
                             self.qualifiers, self.linkage, name)

    def indirect(self, token):
        return _FunctionType(self.result, self.args, self.declarator + token,
                             self.qualifiers, self.linkage)

    def qualify(self, suffix):
        if self.declarator:
            return _FunctionType(self.result, self.args, self.declarator + suffix + " ",
                                 self.qualifiers, self.linkage)
        return _FunctionType(self.result, self.args, "", self.qualifiers + suffix,
                             self.linkage)


def _read_function_type(s, i, subs):
    """Read F [Y] return-type parameter-types E without leaking inner arguments."""
    j = i + 1
    linkage = ''
    if j < len(s) and s[j] == "Y":
        linkage, j = 'extern "C" ', j + 1
    types = []
    while j < len(s) and s[j] != "E":
        ty, end = _read_type(s, j, subs)
        if end <= j:
            return "T", len(s)
        types.append(ty)
        j = end
    if j >= len(s) or len(types) < 2:
        return "T", min(j + 1, len(s))
    result, args = types[0], types[1:]
    # A single printable declarator cannot express distinct language linkages
    # for an outer function and the callback it returns. Keep that combination
    # unresolved instead of dropping the outer Y and aliasing two types.
    if linkage and isinstance(result, _FunctionType):
        return "T", j + 1
    if (any(t is None or re.search(r"\bT\b", t) for t in types)
            or ("void" in args and args != ["void"])
            or (isinstance(result, _FunctionType) and not result.declarator)):
        return "T", j + 1
    ty = _FunctionType(result, [] if args == ["void"] else args, linkage=linkage)
    subs.append(ty)
    return ty, j + 1


def _read_type(s, i, subs):
    """Return (type_string, next_i). Best-effort; unknown -> 'T'."""
    if i >= len(s):
        return None, i
    c = s[i]
    if c in BUILTIN:
        ty = BUILTIN[c]
        # Per Itanium ABI: builtin types are NOT substitution candidates
        return ty, i + 1
    if c in "KVr":                                  # one order-insensitive qualifier group
        j = i
        while j < len(s) and s[j] in "KVr":
            j += 1
        group = s[i:j]
        previous_subs = len(subs)
        inner, end = _read_type(s, j, subs)
        if not re.fullmatch(r"r?V?K?", group) or inner is None:
            return "T", end
        suffix = "".join({"K": " const", "V": " volatile", "r": " restrict"}[q]
                         for q in reversed(group))
        if isinstance(inner, _FunctionType):
            ty = inner.qualify(suffix)
            # A function's CV qualifiers are indivisible for substitution.
            if (not inner.declarator and len(subs) > previous_subs
                    and subs[-1] is inner):
                subs.pop()
        else:
            ty = inner + suffix
        subs.append(ty)
        return ty, end
    if c in "PRO":                                  # pointer / reference wrappers
        inner, j = _read_type(s, i + 1, subs)
        token = {"P": "*", "R": "&", "O": "&&"}[c]
        ty = inner.indirect(token) if isinstance(inner, _FunctionType) else (inner or "T") + " " + token
        subs.append(ty)
        return ty, j
    if c == "F":
        return _read_function_type(s, i, subs)
    if c == "S":                                     # substitution
        return _read_substitution(s, i, subs)
    if c.isdigit():                                  # length-prefixed name (a class type)
        name, j = _read_len_name(s, i)
        if j < len(s) and s[j] == "I":               # class<template-args>
            subs.append(name)                         # bare name is a sub
            targs, j = _read_template_args(s, j, subs)
            ty = f"{name}<{', '.join(targs)}>"
            subs.append(ty)                           # template instance is also a sub
        else:
            ty = name or "T"
            subs.append(ty)
        return ty, j
    if c == "N":                                     # nested type name N..E
        j = i + 1
        parts, valid = [], True
        previous_subs = len(subs)
        while j < len(s) and s[j] != "E":
            if s[j].isdigit():
                name, j = _read_len_name(s, j)
                if not name or j > len(s):
                    valid = False
                parts.append(name or "T")
                prefix = "::".join(parts)
                if j < len(s) and s[j] == "I":
                    subs.append(prefix)              # nested template prefix
                    targs, j = _read_template_args(s, j, subs)
                    parts[-1] = f"{parts[-1]}<{', '.join(targs)}>"
                # Every newly spelled prefix is a candidate, not just the
                # final type. N1A1BE records A before A::B; S_ can name A.
                subs.append("::".join(parts))
            elif s[j] == "S":
                sub_type, j = _read_substitution(s, j, subs)
                parts.append(sub_type)
                # A substituted prefix already has its own entry.
            else:
                # Skipping an unsupported component is not type resolution.
                valid = False
                j += 1
        ty = "::".join(parts)
        if not valid or not parts or j >= len(s) or re.search(r"\bT\b", ty):
            del subs[previous_subs:]
            return "T", min(j + 1, len(s))
        return ty, j + 1  # skip E
    return "T", i + 1                                # give up on this code, keep going


def _read_substitution(s, i, subs):
    """Read a substitution reference S_, S0_, S1_, St, etc."""
    if i + 1 < len(s) and s[i + 1] == "t":
        # St = std::
        subs.append("std")
        return "std", i + 2
    m = re.match(r"S([A-Za-z0-9]*)_", s[i:])
    if m:
        seq = m.group(1)
        idx = 0
        if seq == "":
            idx = 0  # S_ = first substitution
        else:
            idx = int(seq, 36) + 1
        advance = len(m.group())
        if idx < len(subs):
            return subs[idx], i + advance
        return "T", i + advance
    return "T", i + 2  # fallback skip


def _parse_thunk_prefix(s):
    """Parse thunk prefixes. Returns (thunk_offset, virtual_offset, rest) or None."""
    m = re.match(r"_ZThn(\d+)_(.+)", s)
    if m:
        return -int(m.group(1)), None, "_Z" + m.group(2)

    m = re.match(r"_ZTv(\d+)_n(\d+)_(.+)", s)
    if m:
        return -int(m.group(2)), int(m.group(1)), "_Z" + m.group(3)

    return None


def demangle(sym):
    """Demangle an Itanium ABI mangled name.

    Returns a dict with keys:
        qualified  - "Class::Method" or "Method"
        class      - class name or None
        method     - method name
        args       - list of argument type strings
        nargs      - len(args)
        ctor       - True if constructor
        dtor       - True if destructor
        variant    - "C1"/"C2"/"C3"/"D0"/"D1"/"D2" or None
        thunk      - True if this is a thunk
        thunk_offset - this-adjustment offset (negative int) or None
        thunk_virtual_offset - virtual offset or None
    Returns None for names that are not _Z-mangled.
    """
    if not sym.startswith("_Z"):
        return None

    thunk = False
    thunk_offset = None
    thunk_voffset = None
    if sym.startswith("_ZT"):
        parsed = _parse_thunk_prefix(sym)
        if parsed:
            thunk_offset, thunk_voffset, inner_sym = parsed
            thunk = True
            sym = inner_sym  # demangle the inner symbol

    s = sym
    nested = s.startswith("_ZN")
    body = s[3:] if nested else s[2:]
    parts = []
    variant = None
    subs = []  # substitution stack (Itanium ABI: only qualified-name prefixes + types)
    i = 0

    while i < len(body):
        c = body[i]
        if c == "E" and nested:
            i += 1
            break
        if c.isdigit():
            name, i = _read_len_name(body, i)
            parts.append(name)
            if nested:
                subs.append(name)
            if i < len(body) and body[i] == "I":     # template on this component
                targs, i = _read_template_args(body, i, subs)
                parts[-1] = f"{name}<{', '.join(targs)}>"
                if nested:
                    subs[-1] = parts[-1]
        elif body[i:i + 2] in ("C1", "C2", "C3"):
            variant = body[i:i + 2]
            parts.append("ctor")
            i += 2
        elif body[i:i + 2] in ("D0", "D1", "D2"):
            variant = body[i:i + 2]
            parts.append("dtor")
            i += 2
        elif not nested and body[i:i + 2] in OPERATORS:
            parts.append(OPERATORS[body[i:i + 2]])
            i += 2
        elif nested and body[i:i + 2] in OPERATORS:
            parts.append(OPERATORS[body[i:i + 2]])
            i += 2
        elif nested and body[i:i + 2] == "cv":
            i += 2
            conv_type, i = _read_type(body, i, subs)
            parts.append(f"operator {conv_type}")
        elif nested and body[i] == "S":
            sub_type, i = _read_substitution(body, i, subs)
            parts.append(sub_type)
        elif not nested:
            break
        else:
            i += 1

    if not parts:
        return None

    if nested and len(parts) >= 2 and parts[-1] not in ("ctor", "dtor"):
        if subs and subs[-1] == parts[-1]:
            subs.pop()

    method = parts[-1]
    klass = parts[-2] if len(parts) >= 2 else None
    ctor = method == "ctor"
    dtor = method == "dtor"
    if (ctor or dtor) and klass:
        base_klass = klass.split("<")[0] if "<" in klass else klass
        method = ("~" if dtor else "") + base_klass

    args = []
    while i < len(body):
        t, ni = _read_type(body, i, subs)
        if ni <= i:
            break
        if t == "void" and not args:                 # f(void) == no args
            i = ni
            break
        args.append(str(t))
        i = ni

    qualified = "::".join(p for p in parts if p not in ("ctor", "dtor")) or method
    if (ctor or dtor):
        qualified = (klass + "::" + method) if klass else method

    result = {
        "qualified": qualified, "class": klass, "method": method,
        "args": args, "nargs": len(args), "ctor": ctor, "dtor": dtor,
        "variant": variant,
        "thunk": thunk, "thunk_offset": thunk_offset,
        "thunk_virtual_offset": thunk_voffset,
    }
    return result


def signature(sym):
    """One-line 'Class::Method(t1, t2, ...)' or None."""
    d = demangle(sym)
    if not d:
        return None
    prefix = ""
    if d["thunk"]:
        off = d["thunk_offset"]
        if d["thunk_virtual_offset"] is not None:
            prefix = f"[virtual thunk({off})] "
        else:
            prefix = f"[thunk({off})] "
    variant_tag = ""
    if d["variant"]:
        tags = {"C1": "complete", "C2": "base", "C3": "allocating",
                "D0": "deleting", "D1": "complete", "D2": "base"}
        variant_tag = f" [{tags.get(d['variant'], d['variant'])}]"
    return f"{prefix}{d['qualified']}({', '.join(d['args'])}){variant_tag}"


if __name__ == "__main__":
    import sys
    for a in sys.argv[1:]:
        print(f"{a}\n  {demangle(a)}\n  {signature(a)}")
