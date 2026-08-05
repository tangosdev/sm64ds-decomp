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
Every rewritten file is byte-verified against the ROM and reverted on failure.

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

import match as M              # noqa: E402
import modules as MOD          # noqa: E402
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
    """Locate `->method(...)` or `.method(...)`; (recv, args, span, op) or None."""
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
    """Locate `Qual::...::method(...)`; (args, span) or None.

    A qualified name preceded by a type token (`System* System::New(..)`, or the
    struct-local `static System* New(..)`) is a DECLARATION, not a call; touching
    it would redeclare the free symbol with the method's parameter list. Skip
    any occurrence whose preceding token is `*`, `&`, or an identifier other
    than a statement keyword."""
    for m in re.finditer(r"(?:[A-Za-z_]\w*\s*::\s*)+" + re.escape(method) + r"\s*\(",
                         text[start:]):
        s = start + m.start()
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
        return split_args(text[open_at + 1:i]), (s, i + 1)
    return None


def receiver_expr(recv, op):
    """The object-pointer argument for the free call.

    The receiver expression is kept VERBATIM (any pointer converts to the
    void* parameter implicitly) apart from one balanced outer paren layer:
    stripping casts is how `(ModelAnim*)((char*)c + 0x108)` once became
    `c + 0x108`, which is a different address. A `.` call's receiver is an
    object, so its address is taken instead."""
    expr = recv.strip()
    if expr.startswith("(") and expr.endswith(")"):
        depth = 0
        for i, ch in enumerate(expr):
            if ch == "(":
                depth += 1
            elif ch == ")":
                depth -= 1
                if depth == 0:
                    break
        if i == len(expr) - 1:
            expr = expr[1:-1].strip()
    return f"&({expr})" if op == "." else expr


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
    mods = {("arm9" if m["name"] == "main" else m["name"]): m for m in MOD.modules()}

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
        flags = M.DEFAULT_FLAGS
        if f.suffix == ".cpp" or text.startswith("//cpp"):
            flags = flags.replace("-lang c99", "-lang c++")

        obj = next((o for o in (M.compile_c(f, v, flags) for v in M.SWEEP) if o), None)
        if obj is None:
            return rel, "compile failed", 0
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
            if len(re.findall(_decl_re(method), text)) > 1:
                return rel, "method name declared in more than one local class", 0
            sig = method_decl(text, cls, method)
            if sig is None:
                return rel, "local declaration not found/parsed", 0
            ret, params, at, is_static, qual = sig
            todo.append((method, f"{qual}::{target}" if qual else target, is_static))
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

        # Pass 2: EVERY call site must go -- one left behind still references the
        # nonexistent mangled name and the file stays unresolvable.
        n = 0
        for method, target, is_static in todo:
            pos, replaced = 0, 0
            while True:
                if is_static:
                    hit = find_static_call(new, method, pos)
                    if hit is None:
                        break
                    cargs, (s, e) = hit
                    call = f"{target}({', '.join(cargs)})"
                else:
                    hit = find_call(new, method, pos)
                    if hit is None:
                        break
                    recv, cargs, (s, e), op = hit
                    call = f"{target}({', '.join([receiver_expr(recv, op)] + cargs)})"
                new = new[:s] + call + new[e:]
                pos = s + len(call)
                replaced += 1
            if replaced == 0:
                return rel, "call site not in a recognised shape", 0
            n += replaced

        if n == 0 or new == text:
            return rel, "call site not in a recognised shape", 0

        if not args.apply:
            return rel, "transformable (not compile-checked; run --apply)", n

        f.write_text(new, encoding="utf-8", newline="\n")
        tgt = (M.target_bytes(addr, size) if label == "arm9"
               else M.target_bytes(addr, size, mods[label]["bin"], mods[label]["base"]))
        compiled = False
        for v in M.SWEEP:
            o = M.compile_c(f, v, flags)
            if o is None:
                continue
            code, rr = M.extract_func(o, name)
            if code is None:
                continue
            compiled = True
            if M.compare(tgt, code, rr, verbose=False)[0]:
                return rel, "transformed", n
        f.write_bytes(orig_bytes)
        return rel, ("reverted (compiled but bytes differ)" if compiled
                     else "reverted (transformed source does not compile)"), 0

    if args.limit:
        jobs = jobs[:args.limit]
    counts = collections.Counter()
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, verdict, n in ex.map(one, jobs):
            counts[verdict] += 1
            print(f"  {verdict:50s} {rel}" + (f"  ({n} call(s))" if n else ""))
    print()
    for k, v in counts.most_common():
        print(f"  {v:5d}  {k}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
