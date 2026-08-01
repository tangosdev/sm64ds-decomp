#!/usr/bin/env python3
"""Audit near-miss drafts for wrong function signatures, using matched src/ as ground truth.

A wrong arity costs argument-setup instructions, so it shows up as a small size mismatch or a
couple of divergent words -- and it is invisible to codegen-shape debugging, because the C looks
fine. It has buried at least three otherwise-correct drafts in this repo, so it is worth checking
BEFORE grinding shape.

Two independent checks, both pure static analysis (no compiles):

  A. The draft's OWN parameter count vs how already-matched code CALLS it.
  B. The draft's `extern` callee prototypes vs those callees' REAL definitions in matched src/.

Ground truth is only ever taken from files that byte-reproduce the ROM: files containing
`// NONMATCHING` are skipped, since their codegen (and often their signatures) are not authoritative.

Usage:
    python tools/sigaudit.py                      # audit every draft in nearmiss/db.jsonl
    python tools/sigaudit.py --name func_ov007_020c6550
    python tools/sigaudit.py --json out.json

Caveats worth knowing before you trust a hit:
  * `f()` in C means UNSPECIFIED parameters, not zero. Those are skipped, not flagged.
  * Check A treats the majority arg-count across call sites as truth; a function called through a
    mixture of prototypes can produce a false hit. Always eyeball the reported call sites.
  * A caller's translation unit can legitimately pass MORE arguments than the callee's definition
    uses -- a cross-TU prototype mismatch baked into the original build. Observed on
    func_ov006_020ec84c: the ROM does `mov r0,r6; mov r1,r5; bl ...` while that callee's matched
    definition takes one parameter, so the draft's 2-arg call was already correct. That is history,
    not a bug, and it is NOT statically detectable from src/ alone -- only the disassembly at the
    call site settles it. Expect roughly half of check-B hits to be this or a veneer.
  * Nothing here is auto-fixable. Correcting an arity means deciding WHAT to pass, which has to come
    from the disassembly at the call site. Read the ROM, do not guess.
"""
import argparse
import collections
import json
import os
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import srcpath as SP  # noqa: E402

CALL_RE = re.compile(r'\b(func_(?:ov\d+_)?[0-9a-f]{8})\s*\(')
TYPE_KEYWORDS = ('char', 'int', 'void', 'unsigned', 'signed', 'short', 'long', 'struct',
                 'float', 'const', 'u8', 'u16', 'u32', 's8', 's16', 's32', 'bool')


def strip_comments(s):
    s = re.sub(r'/\*.*?\*/', '', s, flags=re.S)
    return re.sub(r'//[^\n]*', '', s)


def split_args(s):
    """Split a parenthesised argument list on top-level commas."""
    out, depth, cur = [], 0, ''
    for ch in s:
        if ch in '([{':
            depth += 1
        elif ch in ')]}':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur.strip())
            cur = ''
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def paren_span(text, i):
    """text[i] must be '('. Returns (inner_text, index_after_close)."""
    depth = 0
    for j in range(i, len(text)):
        if text[j] == '(':
            depth += 1
        elif text[j] == ')':
            depth -= 1
            if depth == 0:
                return text[i + 1:j], j + 1
    return None, None


def nargs(inner):
    args = split_args(inner)
    if not args or (len(args) == 1 and args[0].replace(' ', '') == 'void'):
        return 0
    return len(args)


def _already_matched(repo, name):
    """True only if src/<name> exists AND byte-reproduces (i.e. is not a NONMATCHING hatch)."""
    path = SP.path_for(name)
    if path is None:
        return False
    try:
        head = path.read_text(encoding='utf-8', errors='ignore')[:400]
    except OSError:
        return False
    return 'NONMATCHING' not in head


def matched_sources(repo):
    """Yield (stem, text) for every src/ file that byte-reproduces the ROM."""
    for path in SP.iter_sources():
        try:
            text = path.read_text(encoding='utf-8', errors='ignore')
        except OSError:
            continue
        if 'NONMATCHING' in text[:400]:
            continue                      # not authoritative -- does not reproduce the ROM
        yield path.stem, strip_comments(text)


def build_indexes(repo):
    """definitions: name -> param count (from the DEFINITION), calls: name -> Counter(argcount)."""
    definitions, calls = {}, collections.defaultdict(collections.Counter)
    for stem, text in matched_sources(repo):
        for m in re.finditer(r'\b(%s)\s*\(' % re.escape(stem), text):
            inner, after = paren_span(text, m.end() - 1)
            if inner is None or '{' not in text[after:after + 3]:
                continue                  # a prototype, not the definition
            definitions[stem] = nargs(inner)
            break
        for m in CALL_RE.finditer(text):
            name = m.group(1)
            if name == stem:
                continue                  # skip the file's own definition/prototype
            inner, after = paren_span(text, m.end() - 1)
            if inner is None:
                continue
            if any(k in inner for k in TYPE_KEYWORDS):
                continue                  # looks like a prototype, not a call
            calls[name][nargs(inner)] += 1
    return definitions, calls


def _audit(repo, only=None):
    definitions, calls = build_indexes(repo)
    db = os.path.join(repo, 'nearmiss', 'db.jsonl')
    own_bad, callee_bad = [], []
    with open(db, 'rb') as fh:
        raw = fh.read()
    for line in raw.split(b'\n'):
        if not line.strip():
            continue
        d = json.loads(line.decode('utf-8', 'replace'))
        name, src = d.get('name') or '', d.get('c_source')
        if not src or (only and name != only):
            continue
        # Skip drafts whose function is already a REAL match -- auditing those re-solves solved work.
        # Crucially, a `// NONMATCHING` hatch in src/ does NOT count as solved: the function is still
        # shipped as hand-written asm, and its banked C draft is exactly what is needed to retire the
        # hatch. Two hatches (func_ov007_020c6550, func_ov006_020cb72c) were retired from such drafts
        # on 2026-07-30, and a naive "exists in src/" filter would have hidden both. As of that date
        # 61 of the 62 drafts with a src/ file are hatches; only 1 is genuinely stale.
        if _already_matched(repo, name):
            continue
        text = strip_comments(src)

        # --- A: the draft's own arity vs how matched code calls it ---
        for m in re.finditer(r'\b%s\s*\(' % re.escape(name), text):
            inner, after = paren_span(text, m.end() - 1)
            if inner is None or '{' not in text[after:after + 3]:
                continue
            observed = calls.get(name)
            if observed:
                majority = max(observed.items(), key=lambda kv: kv[1])[0]
                if majority != nargs(inner):
                    own_bad.append(dict(name=name, draft=nargs(inner), callers=majority,
                                        sites=dict(observed),
                                        divergences=d.get('divergences')))
            break

        # --- B: the draft's extern callee prototypes vs the real definitions ---
        for m in re.finditer(r'\bextern\b[^;{]*?\b(func_(?:ov\d+_)?[0-9a-f]{8})\s*\(', text):
            callee = m.group(1)
            inner, after = paren_span(text, m.end() - 1)
            if inner is None or callee not in definitions:
                continue
            if inner.strip() == '':
                continue                  # f() is UNSPECIFIED args in C, not zero -- not a bug
            if nargs(inner) == definitions[callee]:
                continue
            # A definition alone is NOT trustworthy ground truth. Tail-call veneers and thunks are
            # defined `(void)` because the body never names the argument, while the real calling
            # convention still passes it -- e.g. func_020383fc is defined `void func_020383fc(void)`
            # but is a veneer to a C++ method and every matched caller passes `this` in r0. Only
            # flag when the definition AND the matched call sites agree with each other and both
            # disagree with the draft.
            observed = calls.get(callee)
            if observed:
                majority = max(observed.items(), key=lambda kv: kv[1])[0]
                if majority != definitions[callee]:
                    continue              # definition and call sites disagree -> untrustworthy
                if majority == nargs(inner):
                    continue              # draft agrees with real usage
            callee_bad.append(dict(name=name, callee=callee, draft=nargs(inner),
                                   real=definitions[callee],
                                   call_sites=dict(observed) if observed else None,
                                   divergences=d.get('divergences')))
    return own_bad, callee_bad, len(definitions)


def audit(repo, only=None):
    """Audit a checkout while keeping srcpath's process-global root scoped."""
    saved = SP.set_root(pathlib.Path(repo).resolve())
    try:
        return _audit(repo, only)
    finally:
        SP.set_root(saved[0])


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--repo', default='.', help='decomp root (default: cwd)')
    ap.add_argument('--name', help='audit a single near-miss by symbol name')
    ap.add_argument('--json', help='write full findings to this path')
    args = ap.parse_args()

    own_bad, callee_bad, ndefs = audit(args.repo, args.name)

    def key(rec):
        dv = rec.get('divergences')
        return dv if isinstance(dv, int) else 10 ** 6

    print('indexed %d matched definitions\n' % ndefs)
    print('=== A: draft arity disagrees with matched call sites (%d) ===' % len(own_bad))
    for r in sorted(own_bad, key=key):
        print('  %-44s div=%-6s draft=%s callers=%s  sites=%s'
              % (r['name'], r['divergences'], r['draft'], r['callers'], r['sites']))
    print('\n=== B: extern callee prototype disagrees with real definition (%d) ===' % len(callee_bad))
    for r in sorted(callee_bad, key=key):
        print('  %-44s div=%-6s %s: draft=%s real=%s'
              % (r['name'], r['divergences'], r['callee'], r['draft'], r['real']))

    if args.json:
        with open(args.json, 'w', encoding='utf-8') as fh:
            json.dump({'own_arity': own_bad, 'callee_prototypes': callee_bad}, fh, indent=1)
        print('\nwrote %s' % args.json)
    return 1 if (own_bad or callee_bad) else 0


if __name__ == '__main__':
    sys.exit(main())
