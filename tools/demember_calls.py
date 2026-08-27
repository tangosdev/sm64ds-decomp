"""Turn a member call whose mangled name does not exist into an extern "C" free call.

## The shape this fixes

A file declares an ad-hoc local class so it can spell a method call:

    struct Actor { short ReflectAngle(int, int, short); };
    ...
    ((Actor*)self)->ReflectAngle(a, b, c);

The compiler mangles that declaration into `_ZN5Actor12ReflectAngleEiis`. The ROM's
symbol is `_ZN5Actor12ReflectAngleE5Fix12IiES1_s` -- the parameters are `Fix12<int>`,
not `int`, because whoever recovered the signature guessed scalars. So the reference
resolves to nothing and `eligible.py` will not enroll the file.

`resolve_placeholders.py` cannot repair this: the mangled name never appears in the
source, so there is no token to rewrite. It reports these as `not textual`.

## Why not just correct the declaration

Because `Fix12<int>` is a class, and mwccarm passes a by-value class differently from
an `int` -- at the *call site*, not only in the callee. Measured:

    take_i(h, 0x800)   ->  mov r1,#0x800
    take_f(h, v)       ->  ldr r1,[pc,#..] ; ldm r1,{r1}

So declaring the true signature changes the caller's bytes and breaks the match. See
notes/mwccarm-codegen.md 6az.

## What it does instead

The form the corpus already uses in over a thousand files, and the one 6az prescribes:
name the ROM symbol directly, keeping the LOCAL declaration's return and parameter
types verbatim -- they are what currently produce matching bytes (sub-word types are
not interchangeable with int) -- with a `void*` receiver prepended:

    extern "C" short _ZN5Actor12ReflectAngleE5Fix12IiES1_s(void *, int, int, short);
    ...
    _ZN5Actor12ReflectAngleE5Fix12IiES1_s(self, a, b, c);

Registers are registers: the ROM's `Fix12<int>` parameter arrives in r1 exactly as an
`int` would. The declaration deliberately misstates the types and says so in a comment,
because an unmarked lie gets read as recovered truth and propagated.

A file is only touched when EVERY missing symbol in it has reloc evidence naming a
real target -- a partial repair cannot enroll the file and would be diff churn.

Every rewritten file is verified against the ROM and reverted on failure, and that
check has to be more than a byte compare, because this tool changes WHICH SYMBOL a
call resolves to and `match.py` compares every relocated word as a wildcard. It is
therefore blind, by construction, to the only thing being edited. The verification
below reads the relocation's destination out of `config/**/relocs.txt`, and compiles
with the one mwccarm `tools/rombuild.py` will build the file with rather than a sweep
of all 25 installed ones. Both of those are load-bearing:

    src/unnamed/ov006/020c/func_ov006_020cb030.cpp passed the byte-only sweep check this tool shipped
    with, was enrolled, and the ROM build reported ov006 MISMATCHING --
    `module fidelity: 105/106`, `ROM-build analysis: FAIL`. The differing word was a
    `bl` at 0x020cb088 going to func_ov006_020c9024 where the ROM's own relocation
    records 0x020cb134. A wildcard cannot see that. See tools/build_pin.py.

Usage:
    python tools/demember_calls.py                 # report only
    python tools/demember_calls.py --apply
"""
import argparse
import collections
import concurrent.futures
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import build_pin as BP         # noqa: E402
import match as M              # noqa: E402
import eligible as E           # noqa: E402
import resolve_placeholders as RP   # noqa: E402
from enroll import candidates  # noqa: E402

def decode(sym):
    """_ZN[K]<len>Name...<len>method E...  ->  (Class, method), or None.

    The method is the LAST component and its class the second-to-last: a
    namespace-qualified `_ZN8Particle6System9NewSimpleE..` is (System, NewSimple),
    not (Particle, System). Constructors/destructors (C1/D1: no length-prefixed
    method component) come back None and are left for a different tool."""
    m = re.match(r"^_ZN(K?)(\d+)", sym)
    if not m:
        return None
    i = 3 + len(m.group(1))
    parts = []
    while i < len(sym) and sym[i].isdigit():
        d = re.match(r"\d+", sym[i:])
        n = int(d.group(0))
        i += len(d.group(0))
        parts.append(sym[i:i + n])
        i += n
    return (parts[-2], parts[-1]) if len(parts) >= 2 else None


# A method declaration inside a local struct: type tokens, the name, a paren group
# with no nested parens, optional const/pure-virtual, semicolon. The leading
# [;{}\n] boundary is what keeps this from matching a CALL, whose name is always
# preceded by `->` or `.`.
def _decl_re(method):
    return (r"(?:^|[;{}\n])\s*((?:[\w:<>,]+[\s*&]+)+)"
            + re.escape(method) + r"\s*\(([^()]*)\)\s*(?:const)?\s*(?:=\s*0\s*)?;")


def scopes_at(text, upto):
    """The stack of open-brace scopes enclosing offset `upto`.

    Each entry is ("ns", name) for a namespace, ("extern", None) for an
    extern "C"/"C++" block, ("cls", None) for a struct/class/union body, and
    ("other", None) for anything else (function bodies, initialisers)."""
    stack = []
    for m in re.finditer(r"[{}]", text[:upto]):
        if m.group(0) == "}":
            if stack:
                stack.pop()
            continue
        head = text[max(0, m.start() - 160):m.start()].strip()
        ns = re.search(r"namespace\s+([A-Za-z_]\w*)$", head)
        if ns:
            stack.append(("ns", ns.group(1)))
        elif re.search(r'extern\s*"C(?:\+\+)?"$', head):
            stack.append(("extern", None))
        elif re.search(r"\b(?:struct|class|union)\b[^{;()]*$", head):
            stack.append(("cls", None))
        else:
            stack.append(("other", None))
    return stack


def method_decl(text, cls, method):
    """(ret, params, insert_at, is_static, qual) from `cls`'s declaration.

    `insert_at` is the offset just past the struct's closing `};` line: every
    type the parameter list names is necessarily complete there, and every call
    site of the method necessarily comes after it. The declaration stays inside
    whatever namespace declares the struct -- its parameter types are only
    visible there, and extern "C" linkage makes the symbol flat regardless --
    so calls must be qualified with `qual` (empty when at global scope).
    `cls` may also be a NAMESPACE (`namespace cstd { int atan2(int, int); }`
    mangles the same way): then the declaration is a free function, treated as
    static (no receiver), inserted right after its own `;`, and `qual`
    includes `cls` itself.
    None if the declaration cannot be found, parsed, or is inside a scope an
    extern "C" declaration cannot live in (a class body, a function)."""
    m = re.search(r"\b(?:struct|class)\s+" + re.escape(cls) + r"\b[^{;]*\{", text)
    ns_form = m is None
    if ns_form:
        m = re.search(r"\bnamespace\s+" + re.escape(cls) + r"\s*\{", text)
        if not m:
            return None
    stack = scopes_at(text, m.start())
    if any(kind in ("cls", "other") for kind, _ in stack):
        return None
    quals = [name for kind, name in stack if kind == "ns"] + ([cls] if ns_form else [])
    qual = "::".join(quals)
    depth, i = 1, m.end()
    while i < len(text) and depth:
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
        i += 1
    dm = re.search(_decl_re(method), text[m.end():i])
    if not dm:
        return None
    if ns_form:
        at = m.end() + dm.end()  # just past the declaration's own `;`
        ret = re.sub(r"\b(?:virtual|inline|static)\b", " ", dm.group(1))
        return " ".join(ret.split()), " ".join(dm.group(2).split()), at, True, qual
    # Insert IMMEDIATELY after the struct's `};` -- seeking to end-of-line
    # would escape a namespace that closes on the same line, leaving the
    # declaration at global scope while the call is spelled qualified.
    at = i
    while at < len(text) and text[at] in " \t\r\n":
        at += 1
    if at >= len(text) or text[at] != ";":
        return None  # `struct X {...} name;` or EOF: nowhere safe to insert
    at += 1
    is_static = re.search(r"\bstatic\b", dm.group(1)) is not None
    ret = re.sub(r"\b(?:virtual|inline|static)\b", " ", dm.group(1))
    return " ".join(ret.split()), " ".join(dm.group(2).split()), at, is_static, qual


def split_args(s):
    """Top-level comma split, respecting nesting."""
    out, depth, cur = [], 0, ""
    for ch in s:
        if ch in "([": depth += 1
        elif ch in ")]": depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip()); cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def find_call(text, method, start=0):
    """Locate `->method(...)` or `.method(...)`; (recv, args, span, op) or None.

    Deliberately NOT class-aware: it finds the next call spelled with this NAME,
    whichever object it is called on. Attributing that call to a declaring class
    is `receiver_class`'s job and the caller's decision -- see the replacement
    pass, which will not rewrite a call it cannot attribute."""
    for m in re.finditer(r"(->|\.)" + re.escape(method) + r"\s*\(", text[start:]):
        op_at = start + m.start()
        open_at = start + m.end() - 1
        depth, i = 0, open_at
        while i < len(text):
            if text[i] == "(": depth += 1
            elif text[i] == ")":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        if i >= len(text):
            continue
        # Walk left over the receiver: either a parenthesised cast-expression or a
        # plain identifier chain. Anything else is left alone rather than guessed at.
        j = op_at
        if text[j - 1] == ")":
            depth, k = 0, j - 1
            while k >= 0:
                if text[k] == ")": depth += 1
                elif text[k] == "(":
                    depth -= 1
                    if depth == 0:
                        break
                k -= 1
            if k < 0:
                continue
            recv_start = k
        else:
            k = j
            while k > 0:
                if text[k - 1].isalnum() or text[k - 1] in "_[].":
                    k -= 1
                elif k >= 2 and text[k - 2:k] == "->":
                    k -= 2
                else:
                    break
            recv_start = k
        recv = text[recv_start:op_at].strip()
        if not recv:
            continue
        return recv, split_args(text[open_at + 1:i]), (recv_start, i + 1), m.group(1)
    return None


KEYWORD_BEFORE_CALL = {"return", "else", "case", "do"}


def find_static_call(text, method, start=0):
    """Locate `Qual::...::method(...)`; (owner, args, span) or None.

    `owner` is the LAST qualifier component -- the class or namespace the call
    names -- so the caller can check the call belongs to the declaration it is
    about to rewrite. `A::run()` and `B::run()` are different symbols.

    A qualified name preceded by a type token (`System* System::New(..)`, or the
    struct-local `static System* New(..)`) is a DECLARATION, not a call; touching
    it would redeclare the free symbol with the method's parameter list. Skip
    any occurrence whose preceding token is `*`, `&`, or an identifier other
    than a statement keyword."""
    for m in re.finditer(r"((?:[A-Za-z_]\w*\s*::\s*)+)" + re.escape(method) + r"\s*\(",
                         text[start:]):
        s = start + m.start()
        owner = re.findall(r"[A-Za-z_]\w*", m.group(1))[-1]
        open_at = start + m.end() - 1
        k = s
        while k > 0 and text[k - 1] in " \t\r\n":
            k -= 1
        if k > 0 and text[k - 1] in "*&":
            continue
        if k > 0 and (text[k - 1].isalnum() or text[k - 1] == "_"):
            w = k
            while w > 0 and (text[w - 1].isalnum() or text[w - 1] == "_"):
                w -= 1
            if text[w:k] not in KEYWORD_BEFORE_CALL:
                continue
        depth, i = 0, open_at
        while i < len(text):
            if text[i] == "(":
                depth += 1
            elif text[i] == ")":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        if i >= len(text):
            continue
        return owner, split_args(text[open_at + 1:i]), (s, i + 1)
    return None


def member_bodies(text, cls):
    """Spans of the body of every `cls::name(..) { .. }` definition in `text`.

    Only a definition whose `)` is followed directly by `{`. That excludes a
    declaration (`;`), a const member function (`this` would be `const cls*`,
    which does not convert to the `void*` receiver), and a constructor with an
    initialiser list -- none of which this tool has any business rewriting."""
    out = []
    for m in re.finditer(r"\b" + re.escape(cls) + r"\s*::\s*~?[A-Za-z_]\w*\s*\(", text):
        depth, i = 0, m.end() - 1
        while i < len(text):
            if text[i] == "(":
                depth += 1
            elif text[i] == ")":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        if i >= len(text):
            continue
        j = i + 1
        while j < len(text) and text[j] in " \t\r\n":
            j += 1
        if j >= len(text) or text[j] != "{":
            continue
        depth, k = 0, j
        while k < len(text):
            if text[k] == "{":
                depth += 1
            elif text[k] == "}":
                depth -= 1
                if depth == 0:
                    break
            k += 1
        out.append((j, min(k + 1, len(text))))
    return out


def find_implicit_call(text, method, cls, start=0):
    """Locate an unqualified `method(...)` inside a member function of `cls`.

    (args, span) or None. This is the shape the corpus spells when the file IS a
    member of the class -- `Player::St_Land_Init` calling its sibling `SetAnim(..)`
    with no receiver at all, which no amount of looking for `->` will find.

    Attribution here is the strongest of the three, and comes from the language
    rather than from a cast: inside `cls::f() { .. }` an unqualified `method` is
    found in class scope, which is searched before namespace scope, so it IS
    `this->method(..)` and a free function of the same name is hidden. That is
    also why nothing outside such a body is ever considered -- and why the
    declaration inside `struct cls { .. }` cannot be mistaken for a call."""
    bodies = member_bodies(text, cls)
    if not bodies:
        return None
    for m in re.finditer(r"(?<![\w.])" + re.escape(method) + r"\s*\(", text[start:]):
        s = start + m.start()
        if not any(a <= s < b for a, b in bodies):
            continue
        k = s
        while k > 0 and text[k - 1] in " \t\r\n":
            k -= 1
        # `->m(`, `.m(` and `Q::m(` belong to the other two finders; a preceding
        # `*`, `&` or identifier is a declaration, not a call.
        if k >= 2 and text[k - 2:k] in ("->", "::"):
            continue
        if k >= 1 and text[k - 1] in ".*&":
            continue
        if k >= 1 and (text[k - 1].isalnum() or text[k - 1] == "_"):
            w = k
            while w > 0 and (text[w - 1].isalnum() or text[w - 1] == "_"):
                w -= 1
            if text[w:k] not in KEYWORD_BEFORE_CALL:
                continue
        open_at = start + m.end() - 1
        depth, i = 0, open_at
        while i < len(text):
            if text[i] == "(":
                depth += 1
            elif text[i] == ")":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        if i >= len(text):
            continue
        return split_args(text[open_at + 1:i]), (s, i + 1)
    return None


def _unwrap(expr):
    """`expr` less ONE balanced outer paren layer, if it is wrapped in one.

    `(a) + (b)` starts and ends with a paren without being wrapped in one, so
    the depth has to come back to zero at the last character and nowhere
    earlier."""
    expr = expr.strip()
    if not (expr.startswith("(") and expr.endswith(")")):
        return expr
    depth = 0
    for i, ch in enumerate(expr):
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
            if depth == 0:
                break
    return expr[1:-1].strip() if i == len(expr) - 1 else expr


# The cast that says what a receiver IS: `((Actor*)self)->` names Actor. Only a
# pointer cast counts -- `->` on a non-pointer would not compile.
_RECV_CAST = re.compile(r"^\(\s*(?:const\s+|volatile\s+|struct\s+|class\s+)*"
                        r"([A-Za-z_]\w*(?:\s*::\s*[A-Za-z_]\w*)*)\s*\*+\s*\)")


def receiver_class(recv):
    """The class named by the receiver's outermost cast, or None if it says nothing.

    `((Actor*)self)` -> "Actor"; `((Actor*)((char*)c + 0x108))` -> "Actor" (the
    OUTERMOST cast is the receiver's type, the inner one is address arithmetic);
    a bare `self` or `obj->field` -> None, because the file does not say there
    what type it is."""
    m = _RECV_CAST.match(_unwrap(recv))
    return re.findall(r"[A-Za-z_]\w*", m.group(1))[-1] if m else None


def receiver_expr(recv, op):
    """The object-pointer argument for the free call.

    The receiver expression is kept VERBATIM (any pointer converts to the
    void* parameter implicitly) apart from one balanced outer paren layer:
    stripping casts is how `(ModelAnim*)((char*)c + 0x108)` once became
    `c + 0x108`, which is a different address. A `.` call's receiver is an
    object, so its address is taken instead."""
    expr = _unwrap(recv)
    return f"&({expr})" if op == "." else expr


def rewrite_calls(text, todo):
    """Point every call site of each `todo` method at its ROM symbol.

    `todo` is a list of `(method, target, is_static, cls, ambiguous)`. Returns
    `(new_text, n_rewritten, None)`, or `(text, 0, reason)` if the file must not
    be touched at all.

    Two obligations, and the second is the one with teeth:

    EVERY call site of `cls::method` must go. One left behind still references
    the mangled name the ROM does not define, so the file stays unresolvable and
    the edit is pure churn.

    NO call site of anyone else's method may go with it. A call is a relocation,
    and `match.compare` wildcards every relocated word, so redirecting the wrong
    call still byte-matches -- the gate downstream is blind, by construction, to
    this pass being wrong, and so this pass has to be right by construction.
    Concretely, in `func_ov022_02111bdc.cpp` both `ModelBase::SetFile` and
    `MovingMeshCollider::SetFile` are declared and called; only the latter is
    missing from the ROM, and the former is four lines earlier. Rewriting "the
    next call spelled SetFile" sends a working call to a different function.

    So a call is rewritten only where the source says which class it belongs to:
    a qualified call names it outright, an instance call names it in the
    receiver's cast, and an unqualified call inside `cls::f() { .. }` is named by
    the function it sits in. Where the name is declared only once in the file the
    receiver need not repeat it, there being nothing else it could mean; where it
    is declared more than once, a receiver that names nothing is not evidence and
    the whole file is left alone."""
    orig, n = text, 0
    for method, target, is_static, cls, ambiguous in todo:
        pos, replaced, unattributed = 0, 0, 0
        while True:
            if is_static:
                hit = find_static_call(text, method, pos)
                if hit is None:
                    break
                owner, cargs, (s, e) = hit
                if owner != cls:
                    pos = e              # `Other::method(..)` is a different symbol
                    continue
                call = f"{target}({', '.join(cargs)})"
            else:
                hit = find_call(text, method, pos)
                if hit is None:
                    break
                recv, cargs, (s, e), op = hit
                rc = receiver_class(recv)
                if rc is not None and rc != cls:
                    pos = e              # cast to something else: not this method
                    continue
                if rc is None and ambiguous:
                    unattributed += 1
                    pos = e
                    continue
                call = f"{target}({', '.join([receiver_expr(recv, op)] + cargs)})"
            text = text[:s] + call + text[e:]
            pos = s + len(call)
            replaced += 1
        if unattributed:
            return orig, 0, (f"ambiguous: {method} is declared more than once and "
                             f"{unattributed} call site(s) do not name a class")
        # ... and the same method called with no receiver at all, from inside a
        # member function of its own class. A separate sweep rather than a third
        # branch above: the two shapes are independent, and an explicit rewrite
        # leaves nothing an unqualified search could pick up by mistake.
        if not is_static:
            pos = 0
            while True:
                hit = find_implicit_call(text, method, cls, pos)
                if hit is None:
                    break
                cargs, (s, e) = hit
                call = f"{target}({', '.join(['this'] + cargs)})"
                text = text[:s] + call + text[e:]
                pos = s + len(call)
                replaced += 1
        if replaced == 0:
            return orig, 0, "call site not in a recognised shape"
        n += replaced
    return text, n, None


COMMENT = ('/* Signature deliberately copied from the local declaration above: the\n'
           '   ROM name carries by-value class parameters (e.g. Fix12<int>), which\n'
           '   mwccarm passes differently at the call site, so declaring the true\n'
           '   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */\n')


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("-j", "--jobs", type=int, default=10)
    ap.add_argument("--limit", type=int)
    args = ap.parse_args()

    relocs, syms = RP.load_relocs(), RP.load_symbols()
    info = {}
    for (d, name, rel, addr, size, sec) in candidates()[0]:
        info[rel.replace("\\", "/")] = (name, addr, size, d)

    strict = None
    try:
        import reloc_audit as RA
        import relocs as RL
        strict = (RA, RA.build_name_index(), RA.build_config_relocs(), RL.load_all_syms())
    except Exception as e:                                          # noqa: BLE001
        print(f"  (reloc-destination check unavailable: {e}; verification is byte-only "
              f"-- which is exactly the blind spot this tool has to cover, so treat "
              f"anything it reports as transformed as UNVERIFIED)")

    entries, _, _ = E.load_report()
    jobs = []
    for r in entries:
        if not (r.get("reason") or "").startswith("unresolvable"):
            continue
        rel = r["file"].replace("\\", "/")
        wanted = [s for s in (r.get("missing") or []) if decode(s)]
        if wanted and rel in info:
            jobs.append((rel, wanted, list(r.get("missing") or [])))

    print(f"files referencing a class method that resolves to nothing: {len(jobs)}")

    def one(job):
        rel, wanted, allmiss = job
        f = REPO / rel
        name, addr, size, d = info[rel]
        label = d.relative_to(REPO / "config").as_posix()
        label = "arm9" if label == "arm9" else label.split("/")[-1]
        orig_bytes = f.read_bytes()
        text = orig_bytes.decode("utf-8", "surrogateescape")

        # The build's compiler, not the first sweep member that happens to compile:
        # this object's relocation OFFSETS are added to the function's address to look
        # the destination up in relocs.txt, so taking them from a compiler that laid
        # the function out differently resolves the call against the wrong ROM word.
        version, why = BP.compiler_for(f, name)
        if version is None:
            return rel, f"unverifiable: {why}", 0
        obj = M.compile_c(f, version, BP.flags_for(f, text))
        if obj is None:
            return rel, f"compile failed under the build's compiler ({version})", 0
        rl = RP.relocs_for(obj, name)
        if rl is None:
            return rel, "function not in object", 0

        table = relocs.get(label, {})
        real, generic, ambiguous = {}, 0, 0
        for off, sym in rl:
            if sym not in wanted:
                continue
            entry = table.get(addr + off)
            if entry is None:
                continue
            to, spec = entry
            cands = RP.module_keys(spec)
            if not cands:
                continue
            nm, why = RP.resolve_in(syms, cands, to, label)
            if nm and not RP.GENERIC.match(nm):
                real[sym] = nm
            elif nm:
                generic += 1
            elif why and "names" in why:
                ambiguous += 1
        if not real:
            if ambiguous:
                return rel, "target ambiguous across overlays", 0
            if generic:
                return rel, "target unnamed in config", 0
            return rel, "no reloc evidence for the member call", 0
        # A repair that leaves any other missing symbol behind cannot enroll the
        # file; do not touch it at all.
        if set(allmiss) - set(real):
            return rel, "partial: other unresolvable references remain", 0

        # Pass 1: parse every declaration and record where its extern decl goes --
        # just past the declaring struct, where its parameter types are complete
        # and before every call site.
        todo, inserts = [], collections.defaultdict(list)
        for sym, target in real.items():
            cls, method = decode(sym)
            sig = method_decl(text, cls, method)
            if sig is None:
                return rel, "local declaration not found/parsed", 0
            ret, params, at, is_static, qual = sig
            # Is this NAME declared more than once in the file? If it is, a call
            # spelled `p->method(..)` does not say which declaration it means, and
            # the replacement pass below demands the receiver name a class. The
            # count is textual and so over-cautious by design: it can only ask for
            # more evidence, never less. (`method_decl` above is class-scoped, so
            # the DECLARATION lookup was never the ambiguous half.)
            ambiguous = len(re.findall(_decl_re(method), text)) > 1
            todo.append((method, f"{qual}::{target}" if qual else target,
                         is_static, cls, ambiguous))
            if target not in text:
                # A static method has no receiver; an instance method's becomes
                # a leading void*.
                first = "" if is_static else ("void *, " if params else "void *")
                inserts[at].append(f'extern "C" {ret} {target}({first}{params});')

        # Insert declarations first (descending, so earlier offsets stay valid);
        # the replacement pass rescans, so it needs no coordinate bookkeeping.
        new = text
        for at in sorted(inserts, reverse=True):
            blob = "\n" + COMMENT + "\n".join(inserts[at]) + "\n"
            if "\r\n" in text:
                blob = blob.replace("\n", "\r\n")
            new = new[:at] + blob + new[at:]

        # Pass 2 -- see rewrite_calls.
        new, n, why = rewrite_calls(new, todo)
        if why is not None:
            return rel, why, 0

        if n == 0 or new == text:
            return rel, "call site not in a recognised shape", 0

        if not args.apply:
            return rel, "transformable (not compile-checked; run --apply)", n

        f.write_text(new, encoding="utf-8", newline="\n")
        ok, why = BP.verify(f, name, addr, size, label, strict=strict, version=version)
        if ok:
            return rel, "transformed", n
        f.write_bytes(orig_bytes)
        return rel, f"reverted: {why}", 0

    if args.limit:
        jobs = jobs[:args.limit]
    counts = collections.Counter()
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, verdict, n in ex.map(one, jobs):
            # Bucket before the colon: the reasons carry per-file detail now.
            counts[verdict.split(":")[0]] += 1
            print(f"  {verdict:50s} {rel}" + (f"  ({n} call(s))" if n else ""))
    print()
    for k, v in counts.most_common():
        print(f"  {v:5d}  {k}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
