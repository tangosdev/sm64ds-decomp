"""Translate the port's MSVC `/alternatename` linker pragmas into GCC weak
aliases for the Linux build.

WHY THIS EXISTS
On MSVC the port resolves a large class of "referenced-here, defined-under-a-
different-name" symbols with `#pragma comment(linker, "/alternatename:X=Y")`:
if X is left undefined at link, the linker resolves it to Y. GCC/ld has no
`/alternatename`; the equivalent is a weak alias emitted in an object
(`.set X, Y` / `__attribute__((weak, alias))`). But the referencing side on GCC
does NOT emit the same decorated name MSVC does -- it emits the platform's
Itanium mangling of whatever the slice TU declared. So the MSVC LHS is useless
on Linux; what we actually need is: for every symbol GCC/ld leaves UNDEFINED,
find the real definition it should resolve to and emit an alias.

HOW
The `/alternatename` RHS is the real definition name (a C-decorated `_name`, an
Itanium `__ZN...`, or another MSVC name for an alias chain). We:
  1. parse every pragma into (lhs_msvc, rhs) pairs;
  2. normalise each RHS to the Itanium/C symbol actually defined in the objects
     (strip the single MSVC leading `_`);
  3. for every symbol the GCC link leaves undefined, match it to a pragma whose
     RHS is a defined symbol representing the same entity -- keyed on the
     demangled (qualified-name, arg-count) so a register-equivalent parameter
     spelling (char vs int, void* vs T*) still matches -- and emit a weak alias
     `undefined -> rhs`.

The result is a single generated `.c` (a translation unit of GCC `asm(".set")`
directives, compiled only into the Linux targets). src/ and the byte-locked
tree are never touched, and MSVC never sees this file. Any undefined symbol we
could NOT map is printed so it can be handled explicitly (a genuine missing
host stub, not an alias gap).

Usage:
  python gcc_aliases.py --altnames alt.txt --undef undef_map.tsv \
        --defined defined.txt --out gcc_aliases.c
"""
import argparse
import re
import subprocess
import sys


def cxxfilt(names):
    """Demangle a list of symbols with c++filt (Itanium). Non-mangled names
    pass through unchanged."""
    out = subprocess.run(["c++filt"], input="\n".join(names),
                         capture_output=True, text=True).stdout
    return out.splitlines()


def parse_altnames(path):
    """Return list of (lhs, rhs_raw) from the pragma dump (one LHS=RHS/line)."""
    pairs = []
    for line in open(path):
        line = line.strip()
        if not line or "=" not in line:
            continue
        lhs, rhs = line.split("=", 1)
        pairs.append((lhs.strip(), rhs.strip()))
    return pairs


def rhs_to_symbol(rhs):
    """Normalise a pragma RHS to the symbol name as it appears in an object.
    MSVC decorates cdecl/data names with a single leading underscore; the
    Itanium `_ZN...` mangling therefore shows up as `__ZN...`. Strip exactly one
    leading underscore for those. A RHS that is itself an MSVC `?...@@` name is
    part of an alias chain and is skipped (we resolve to real defs only)."""
    if rhs.startswith("?"):
        return None                      # MSVC-mangled: alias chain, skip
    if rhs.startswith("__Z"):
        return rhs[1:]                   # __ZN... -> _ZN... (real Itanium)
    if rhs.startswith("_"):
        return rhs[1:]                   # _name -> name (C decoration)
    return rhs


def msvc_key(name):
    """Loose (qualified-name, arity) key for an MSVC-decorated name
    `?method@Class@Ns@@<cc><ret><args>@Z` (or `...@Z` / `@XZ` for no-arg).
    We only need the class::method path and a rough parameter arity to match
    the same entity's Itanium symbol. Returns None for a name we can't parse
    (data symbols `?x@@3...`, plain `_x`, etc.)."""
    if not name.startswith("?"):
        return None
    body = name[1:]
    at = body.find("@@")
    if at < 0:
        return None
    path = body[:at]                 # method@Class@Ns
    parts = path.split("@")
    if not parts:
        return None
    method = parts[0]
    qual = "::".join(reversed(parts))  # Ns::Class::method
    sig = body[at + 2:]
    # sig starts with access/call-conv codes then arg type codes ending @Z.
    # No-arg functions end in `XZ` (X = void arglist). Otherwise count top-level
    # argument type codes crudely: number of type tokens before the final @Z.
    # A precise MSVC type parse is unnecessary -- arity need only disambiguate
    # overloads, and register-equivalent params already share an arity.
    if sig.endswith("XZ"):
        arity = 0
    else:
        # crude: count 'U'/'V' (class), 'H'(int), 'I'(uint), 'PA'/'PB'(ptr),
        # 'AAU'/'ABU'(ref) occurrences is fragile; instead approximate arity by
        # the number of top-level '@'-free type groups. Fall back to -2 (any).
        arity = -2
    return (qual, method, arity)


def demangle_key(dem):
    """A loose key for matching: qualified name + parameter arity, ignoring the
    exact spelling of register-equivalent parameter types. `A::B::f(x, y)` ->
    ('A::B::f', 2). A data symbol (no parens) -> (name, -1)."""
    dem = dem.strip()
    i = dem.find("(")
    if i < 0:
        return (dem, -1)
    name = dem[:i].strip()
    args = dem[i + 1:dem.rfind(")")].strip()
    if args == "" or args == "void":
        arity = 0
    else:
        # top-level comma split (ignore commas nested in <> or ())
        depth = 0
        arity = 1
        for c in args:
            if c in "<(":
                depth += 1
            elif c in ">)":
                depth -= 1
            elif c == "," and depth == 0:
                arity += 1
    return (name, arity)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--altnames", required=True)
    ap.add_argument("--undef", required=True,
                    help="TSV: mangled<TAB>demangled, one undefined symbol/line")
    ap.add_argument("--defined", required=True,
                    help="newline list of defined symbols in the objects")
    ap.add_argument("--out", required=True)
    a = ap.parse_args()

    defined = set(l.strip() for l in open(a.defined) if l.strip())
    pairs = parse_altnames(a.altnames)

    # Build RHS index: demangle-key -> rhs_symbol, but only RHS that is DEFINED.
    rhs_syms = []
    rhs_for = []
    for lhs, rhs in pairs:
        sym = rhs_to_symbol(rhs)
        if sym and sym in defined:
            rhs_syms.append(sym)
            rhs_for.append((lhs, rhs, sym))
    rhs_dem = cxxfilt(rhs_syms)
    key_to_rhs = {}
    for (lhs, rhs, sym), dem in zip(rhs_for, rhs_dem):
        key_to_rhs.setdefault(demangle_key(dem), sym)
    # also allow exact-name (C symbols) direct hit
    name_to_rhs = {}
    for (lhs, rhs, sym), dem in zip(rhs_for, rhs_dem):
        name_to_rhs.setdefault(dem, sym)

    # DIRECT MODEL of /alternatename:X=Y -- "if X is undefined, resolve to Y".
    # On GCC the undefined symbol's name equals the LHS with MSVC decoration
    # stripped: `_name` -> `name`, `__ZN...`/`__ZTV...` -> `_ZN...`/`_ZTV...`.
    # An MSVC `?...@@` LHS can never be a GCC undefined name, so it is skipped.
    # The RHS may itself be another alternatename LHS (a chain, e.g. an alias to
    # an alias), so resolve transitively to a symbol that is actually defined.
    def lhs_norm(lhs):
        if lhs.startswith("?"):
            return None
        if lhs.startswith("__Z"):
            return lhs[1:]
        if lhs.startswith("_"):
            return lhs[1:]
        return lhs

    raw_map = {}            # normalized-LHS -> raw RHS (as written)
    for lhs, rhs in pairs:
        ln = lhs_norm(lhs)
        if ln:
            raw_map.setdefault(ln, rhs)

    # Index every DEFINED symbol by its demangled qualified name (ignoring the
    # parameter list), so an undefined `Class::method(regA)` can resolve to the
    # defined `Class::method(regB)` when they are the same ROM entity spelled
    # with register-equivalent parameter types. Ambiguous names (more than one
    # defined overload) are dropped -- we only auto-alias unambiguous ones.
    defined_list = sorted(defined)
    defined_dem = cxxfilt(defined_list)
    qual_to_def = {}
    qual_ambig = set()
    for sym, dem in zip(defined_list, defined_dem):
        q = dem.split("(", 1)[0].strip()
        if q == dem and "(" not in dem:
            continue                       # data symbol, skip qual index
        if q in qual_to_def and qual_to_def[q] != sym:
            qual_ambig.add(q)
        else:
            qual_to_def[q] = sym
    for q in qual_ambig:
        qual_to_def.pop(q, None)

    def msvc_qual(name):
        """Class::method path of an MSVC `?method@Class@Ns@@...` name (no args)."""
        if not name.startswith("?"):
            return None
        body = name[1:]
        at = body.find("@@")
        if at < 0:
            return None
        parts = body[:at].split("@")
        return "::".join(reversed(parts)) if parts else None

    def resolve_qual(rhs, seen=None):
        """Resolve an MSVC-mangled RHS to a defined Itanium symbol by matching
        its Class::method against the defined-symbol qualified-name index.
        Follows a `?..@@ = ?..@@` chain (e.g. StarMarkerFace -> StarMarker)."""
        if seen is None:
            seen = set()
        q = msvc_qual(rhs)
        if not q or q in seen:
            return None
        seen.add(q)
        if q in qual_to_def:
            return qual_to_def[q]
        # chase a further MSVC alias hop keyed by the same LHS spelling
        nxt = raw_map.get(rhs) or raw_map.get(rhs.lstrip("?"))
        # pragmas store LHS verbatim incl. the `?`, so look up by the raw name
        for lhs, r in pairs:
            if lhs == rhs:
                nn = rhs_to_symbol(r)
                if nn and nn in defined:
                    return nn
                if r.startswith("?"):
                    return resolve_qual(r, seen)
        return None

    def resolve(sym, seen=None):
        """Follow an alternatename chain to a DEFINED symbol, or None."""
        if seen is None:
            seen = set()
        if sym in seen:
            return None
        seen.add(sym)
        if sym in defined:
            return sym
        rhs = raw_map.get(sym)
        if rhs is None:
            return None
        nxt = rhs_to_symbol(rhs)     # normalize `_x`/`__Zx`; `?..@@` -> None
        if nxt is None:
            return None
        return resolve(nxt, seen)

    # The DOUBLE-MANGLE class: a slice TU declared a symbol whose NAME is itself
    # an Itanium/C symbol string (e.g. `_ZN9ModelBase7SetFileE...` or
    # `func_ov002_020b7f2c`) but did so WITHOUT extern "C" and with extra
    # register-carried `void*` params, so GCC mangled the whole thing again into
    # `_Z<NN><that-string><argmangle>`. The real definition is the inner string
    # verbatim. c++filt demangles `_Z<NN>foo...` to `foo(args)`, so the inner
    # symbol is the leading identifier of the demangled form. Recover it and
    # alias to it when it is a defined symbol.
    INNER = re.compile(r"^((?:_Z[A-Za-z0-9_]+)|(?:func_|data_|_ZN|_ZNK)[A-Za-z0-9_]*)")

    def inner_symbol(dem):
        # take everything up to the first '(' , then the leading identifier run
        head = dem.split("(", 1)[0].strip()
        m = INNER.match(head)
        if m and m.group(1) in defined:
            return m.group(1)
        # also try the whole head (covers `_ZN...` names with template E-suffix)
        if head in defined:
            return head
        return None

    aliases = []
    unmatched = []
    for line in open(a.undef):
        line = line.rstrip("\n")
        if "\t" not in line:
            continue
        mangled, dem = line.split("\t", 1)
        if mangled in defined:
            continue
        # 0) DIRECT alternatename: the undefined symbol IS a pragma LHS. Resolve
        #    its chain to a defined target. Highest confidence -- this is exactly
        #    what /alternatename encodes.
        tgt = None
        if mangled in raw_map:
            tgt = resolve(mangled)
        # 1) exact demangled name hit (C data/func)
        if not tgt:
            tgt = name_to_rhs.get(dem)
        # 2) loose key hit (method with register-equivalent args)
        if not tgt:
            tgt = key_to_rhs.get(demangle_key(dem))
        # 3) double-mangle: alias to the inner symbol name it re-wraps
        if not tgt:
            tgt = inner_symbol(dem)
        # 4) MSVC-RHS chain: the pragma for this symbol points at a `?..@@` RHS.
        #    Resolve that RHS's Class::method to a defined Itanium symbol.
        if not tgt and mangled in raw_map:
            rhs = raw_map[mangled]
            if rhs.startswith("?"):
                tgt = resolve_qual(rhs)
        #    ...also when the LHS itself is the MSVC form (undefined GCC name is
        #    Itanium, so look the pragma up by matching qualified name).
        if not tgt:
            uq = dem.split("(", 1)[0].strip()
            for lhs, rhs in pairs:
                if msvc_qual(lhs) == uq:
                    if rhs.startswith("?"):
                        tgt = resolve_qual(rhs)
                    else:
                        cand = rhs_to_symbol(rhs)
                        tgt = cand if cand in defined else None
                    if tgt:
                        break
        # 5) last resort: the undefined's own Class::method is unambiguously
        #    defined under a register-equivalent overload.
        if not tgt:
            uq = dem.split("(", 1)[0].strip()
            tgt = qual_to_def.get(uq)
        if tgt and tgt != mangled:
            aliases.append((mangled, tgt, dem))
        else:
            unmatched.append((mangled, dem))

    # OUTPUT: a linker-flags fragment, one `--defsym ALIAS=TARGET` per alias.
    # This is the exact GNU-ld equivalent of MSVC's /alternatename: it defines
    # the otherwise-undefined ALIAS to resolve to the address of the defined
    # TARGET at final link. (A standalone `.set` alias TU does NOT work -- when
    # the target is undefined in the alias TU, GNU as folds the alias away.
    # --defsym is resolved by ld against the whole link, which is what we need.)
    # Emitted as a newline-delimited list; CMake reads it into the link line.
    with open(a.out, "w") as f:
        for mangled, tgt, dem in sorted(set(aliases)):
            f.write("-Wl,--defsym,%s=%s\n" % (mangled, tgt))

    print("aliases emitted:", len(set(aliases)))
    print("unmatched undefined:", len(unmatched))
    for m, d in unmatched:
        print("  UNMATCHED", m, "==", d)


if __name__ == "__main__":
    main()
