#!/usr/bin/env python3
"""aritycheck -- the third face of the class: the frame balances, the
arguments are still wrong.

C has no cross-translation-unit prototype checking for extern "C" symbols.
The linker matches on the NAME alone, so one TU may declare

    extern void _ZN9ActorBase18MarkForDestructionEv(void);   /* src/... */

while another DEFINES

    void _ZN9ActorBase18MarkForDestructionEv(void *self)     /* port/hal/... */
    { ((ActorBase *)self)->ActorBase::MarkForDestruction(); }

and nothing complains. The caller pushes nothing, the callee reads `self` off
[esp+4], and what is sitting there is THE CALLER'S OWN RETURN ADDRESS. Under
__cdecl every `ret` in that chain is the right size, so abicheck passes it and
aliascheck never sees it: no /alternatename is involved.

THE RULE: for every extern "C" symbol, every declaration's parameter count
must equal the definition's. A disagreement is a dropped or an invented
argument, and the DROPPED direction is the dangerous one -- the callee reads a
stack slot the caller never wrote.

    DROPS    declaration has FEWER parameters than the definition. The callee
             reads uninitialised stack. When the definition's first parameter
             is the receiver, this is the receiver-dropping class in source
             form rather than in linker-directive form.
    INVENTS  declaration has MORE. Under __cdecl the caller cleans up, so the
             extra push is wasted rather than fatal; it usually means the
             matched ROM body genuinely takes no receiver and a host thunk is
             passing one anyway.

THE FULL CENSUS DOES NOT GATE, AND THAT IS A MEASURED DECISION RATHER THAN
TIMIDITY. The RULE is sound and its named instances are real. The
IMPLEMENTATION is a regular-expression reader over C and C++ text, and on this
tree it reports about two thousand disagreements, of which an unknown but
large fraction are parser artefacts: K&R-style spellings, macro-expanded
declarations, C++ overloads that share an Itanium prefix, and method
definitions inside class bodies. A gate at that number would be switched off
within a day, and a gate with a two-thousand-row baseline is not a gate.

TWO SUBSETS DO GATE. THE FIRST IS THE ONE THAT KEPT RECURRING.

    THE RECEIVER SHAPE: a declaration with NO parameters at all, against a
    definition of an Itanium-mangled MEMBER name (_ZN...) that takes at least
    one. A member's receiver rides r0 on ARM like any other first argument, so
    a body transcribed from the ROM takes it as a parameter -- and a caller
    that spells the declaration `(void)` passes nothing and the callee reads
    the caller's return address as `this`.

    This is not a hypothetical subset. It is exactly what landed on cons twice
    on 2026-08-16, hours before this checker did:

      #1539  include/decl_FaderColor.h
             -extern void _ZN10FaderColor11AdvanceFadeEv(void);
             +extern void _ZN10FaderColor11AdvanceFadeEv(void*);
      #1543  src/ChainChomp_Spawn.cpp, ChiefChilly_Spawn.cpp, Wiggler_Spawn.c
             -void _ZN5EnemyC2Ev(void);          -> _ZN5EnemyC2Ev();
             +void _ZN5EnemyC2Ev(void*);         -> _ZN5EnemyC2Ev(c);

    Both were found by a person reading a fault. Both are in this subset, and
    both are ABSENT from it now, which is what makes it a usable ratchet. The
    live rows are frozen in aritycheck_receiver_baseline.txt; a row that is
    not in that file fails --gate-receiver. The list may only shrink.

    The frozen rows are DEBT, not clearance. Nobody has adjudicated them, and
    some will be parser artefacts. The ratchet's claim is narrow and true: the
    tree does not grow a NEW one without somebody seeing it.

    THE PLAIN-NAME SHAPE (--gate-plainfunc): the same dropped-argument class
    for the address-named C symbols, func_XXXXXXXX and func_ovNNN_XXXXXXXX.
    The receiver ratchet is scoped to _ZN member names, and that scoping let
    a shipped defect through: src/func_ov006_020d8cc4.cpp declared

        extern "C" int func_ov006_020d836c(void);

    and called it with nothing, while the definition takes the per-bomb
    receiver. The ROM leaves the receiver in r0 across the call (020d8ccc is
    `mov r5, r0` and the `bl` follows with r0 untouched), so the byte gate
    stayed green on ARM; on MSVC the caller pushed no argument, the callee
    read its receiver out of the caller's saved registers, and scene 370's
    Sort or 'Splode hung forever the first time a sorting bin filled. Fixed
    in ba2224f88 (run mg8, lane SBN, 2026-08-22); func_ov006_020d836c is not
    a _ZN name, so the receiver ratchet never looked at it.

    THE SUBSET IS THREE FILTERS DEEP, and each filter exists because the
    census without it is mostly noise. Measured on the mg8 tree, 2026-08-22:

      1069  DROPS rows for plain names (rides already out). Most are inert:
       387  after requiring the declaration to be a PROTOTYPE. In C,
            `extern int f();` with empty parentheses promises nothing --
            the calls in that TU pass whatever the transcriber wrote, so
            the declaration cannot drop anything. Empty parentheses in a
            .cpp file DO mean zero parameters and stay in.
       133  after requiring a CALL through the declaration. The __sinit
            destructor-registration TUs declare a body `(void)` and only
            ever take its ADDRESS -- 76 rows for func_02017ab4 alone --
            and an address-take reads no argument slots. A declaration in
            a HEADER is called through the TUs that #include it, so for
            header rows the call is looked for there instead.
       118  after requiring every parameter on BOTH sides to provably be
            one machine word: a pointer, a reference, or a named scalar
            type. Struct-by-value spellings (transcribers write `struct P
            pair` for what another TU spells as two ints), default
            arguments, variadic definitions, long long and double all make
            the COUNT diverge from the WORDS, so the other 15 rows are
            held out and PRINTED, like the RIDE and NS BINDING exclusions.
            Only for all-scalar signatures does "fewer parameters"
            literally mean "fewer words pushed" on both calling
            conventions at once.

    A plain name has exactly one definition in the tree and no C++
    overloads, so the artefacts that keep the full census non-gating (K&R
    spellings, shared Itanium prefixes, in-class definitions) do not apply.
    What survives all three filters is exactly: a call site that pushes
    fewer words than the callee reads. The live rows are frozen in
    aritycheck_plainfunc_baseline.txt; the Sort or 'Splode row is ABSENT
    from it, because it is fixed, which is the same proof-of-usefulness the
    receiver file carries.

    STATED BLIND SPOT: a call under an UNPROTOTYPED C declaration can still
    pass fewer words than the definition reads, and declaration-vs-
    definition comparison cannot see it. Counting arguments at every call
    site textually drowns in C++ default arguments and struct spellings
    (measured: 524 short-looking call sites tree-wide, most of them legal
    calls of defaulted definitions), so that census stays a pointer, not a
    verdict, until somebody reads push counts out of the emitted code.

WHAT WOULD MAKE THE FULL CENSUS GATE. Not a bigger regex. The honest route is
to read the arity out of the emitted code -- the callers' push counts, the way
abicheck reads emitted rets -- rather than out of the text. Until someone does
that, the two-thousand number is a pointer, not a verdict.

    python port/tools/aritycheck.py [repo-root]        census, always exit 0
    python port/tools/aritycheck.py --drops-only       the receiver subset
    python port/tools/aritycheck.py --gate-receiver    ratchet, exit 1 on new
    python port/tools/aritycheck.py --gate-plainfunc   ratchet, exit 1 on new
    python port/tools/aritycheck.py --selftest
    python port/tools/aritycheck.py --json out.json
"""

import argparse
import json
import os
import re
import sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import msvc_undname as mu  # noqa: E402  (path set above)
import alternatename_guard as ag  # noqa: E402
import tailjump_guard as tj  # noqa: E402

# TWO EXCLUSIONS, BOTH READ OFF ANOTHER CHECKER'S ARTEFACT RATHER THAN GUESSED
# HERE. Both were added by run link60 lane RF1, and both exist because closing
# the six live receiver defects of port/abi_checks.txt section 6 turned rows in
# this file into statements that are no longer true. A checker that keeps
# reporting a row after the reason for it is gone teaches people to ignore it,
# and this file's whole claim is that a NEW row means something.
#
# The imports are HARD, not guarded. If either sibling stops importing this
# checker refuses to run rather than quietly reverting to the wider census:
# the value of an exclusion is that somebody can see it fire.
#
#   RIDE       a declaration inside a frame that tailjump_guard ASSERTS is a
#              tail jump to that same callee cannot drop the argument. A jmp
#              reuses the caller's own cdecl frame, so the slot the callee
#              reads is the slot the caller wrote -- which is exactly what
#              reproduces the ROM's `bx ip` with r0 still live. The row stays
#              in the CENSUS, because the text really does disagree; it leaves
#              the RECEIVER subset, because no receiver is dropped.
#              Keyed on (declaring file, symbol) against the guard's own `tu`
#              and `callee`, so a frame displaced into a host copy stops
#              matching the guard's row AND stops matching this exclusion in
#              the same build.
#              ONLY THE HAND-DECLARED jump rows are consulted. The 22 derived
#              veneer rows need the extracted ROM, and this checker has to keep
#              running with no ROM and no build (run_checks calls it in the
#              no-build set), so they are deliberately left out and the rows
#              they would cover stay in the baseline.
#   NS BINDING a namespaced C++ declaration reaches the flat Itanium
#              definition ONLY through an /alternatename binding its MSVC
#              decoration to that flat name. `namespace Player { void
#              St_EndingFly_Main(); }` emits a ?St_EndingFly_Main@Player@@YA...
#              decoration and nothing else. With no directive pointing that at
#              __ZN6Player17St_EndingFly_MainEv, the callee is whatever defines
#              the MSVC name, and comparing arities against the flat body
#              compares two unrelated functions. Without this the checker
#              cannot tell a FIXED site from a broken one, because the
#              declaration text is identical either way -- which is precisely
#              the state lane RF1 left src/func_ov007_020b7764.cpp in: the
#              byte-locked declaration stays, the alias is gone, and the site
#              now reaches a face in hal/scene_boot.cpp that supplies the
#              receiver from the global the ROM reads it from.
#              IT IS COARSER THAN THE DECORATION IT READS, said out loud
#              because the test looks more precise than it is: it asks for `?`
#              and `@@YA` on one side and `__ZN` on the other, and does NOT
#              compare the return-type letter that follows YA. An
#              int-returning binding would be accepted as binding a
#              void-returning declaration of the same name. Not exploitable
#              here -- one namespaced declaration has a binding at all and it
#              matched -- and the failure direction is a row that APPEARS
#              rather than one that vanishes, which is the safe way round for
#              a ratchet. Tightening it means decoding the return type, which
#              is a demangler's job and rule P already owns one.

RIDES = frozenset((r['tu'], r['callee'])
                  for r in (tj.CLASS_A + tj.CLASS_C) if r['form'] == 'jump')


def ns_bindings(root):
    """{flat Itanium symbol} for every namespaced spelling an /alternatename
    binds to it.

    Read through alternatename_guard so the definition of "a linker input" is
    the build's own -- a directive QUOTED in a comment or a .txt is prose and
    binds nothing, the distinction commit ab554f5fd had to teach that reader
    after a comment failed a build.
    """
    port = os.path.join(root, 'port')
    if not os.path.isdir(port):
        return set()
    directives, _quoted = ag.collect_directives(port)
    out = set()
    for lhs, rhs, _rel, _line in directives:
        for a, b in ((lhs, rhs), (rhs, lhs)):
            # `a` is the MSVC free-function spelling, `b` the flat C name.
            # Exactly one leading underscore comes off: that is the 32-bit
            # C decoration, and linkage.py's banner records what stripping
            # more than one cost the last time somebody did it.
            if a.startswith('?') and '@@YA' in a and b.startswith('__ZN'):
                out.add(b[1:])
    return out


DEFAULT_ROOT = os.path.dirname(os.path.dirname(HERE))
DIRS = ('src', 'include', os.path.join('port', 'unmatched'),
        os.path.join('port', 'hal'))
RECEIVER_BASELINE = os.path.join(HERE, 'aritycheck_receiver_baseline.txt')
PLAINFUNC_BASELINE = os.path.join(HERE, 'aritycheck_plainfunc_baseline.txt')

KEYWORDS = {'if', 'for', 'while', 'switch', 'return', 'else', 'sizeof', 'do'}

# The return-type position must not be a statement keyword. Without this,
#     return func_X(a, b);
# and an `else` line followed by an indented call both parse as DECLARATIONS
# of func_X whose "parameters" are the call's arguments -- 501 phantom
# declarations and 33 phantom census rows on the mg8 tree, none of them in
# the receiver subset (measured before the fix landed: 166 receiver keys
# before and after, so the receiver baseline is untouched by this).
_NOT_KW = r'(?!(?:%s)\b)' % '|'.join(sorted(KEYWORDS))

# a definition: <ret> name(params) {
DEFN = re.compile(
    r'^[ \t]*(?:extern\s+"C"\s+)?(?:static\s+)?'
    r'(?:const\s+)?' + _NOT_KW + r'[A-Za-z_]\w*(?:\s*\*)*[\s*]+'
    r'(?P<name>_ZN\w+|func_\w+|[A-Za-z_]\w*)\s*\((?P<p>[^;{)]*)\)\s*\{', re.M)
# a declaration: <ret> name(params) ;
DECL = re.compile(
    r'^[ \t]*(?:extern\s+(?:"C"\s+)?)?'
    r'(?:const\s+)?' + _NOT_KW + r'[A-Za-z_]\w*(?:\s*\*)*[\s*]+'
    r'(?P<name>_ZN\w+|func_\w+)\s*\((?P<p>[^;{)]*)\)\s*;', re.M)

# the address-named C symbols the plain-name ratchet is scoped to
PLAIN = re.compile(r'^func_(?:ov\d+_)?[0-9a-f]{8}$')

# A declaration written in C++ rather than as a flat Itanium name:
#
#     namespace Player { void St_EndingFly_Main(); }
#
# emits the symbol _ZN6Player17St_EndingFly_MainEv, exactly like the flat
# spelling four sibling TUs use -- but nothing above matches it, so the site
# is invisible to this checker. That is not hypothetical: it is the SIXTH live
# receiver defect on cons (src/func_ov007_020b7764.cpp:9 calls
# Player::St_EndingFly_Main() while the definition takes a receiver), and lane
# ABR1's review found that neither aliascheck nor aritycheck could see it.
# aliascheck cannot because scene_boot.cpp:1126 spells that alias
# ?St_EndingFly_Main@Player@@YAXXZ, a free function on both sides, so the
# receiver AGREES and rule R is right to stay quiet.
#
# THE SCOPE HERE IS DELIBERATELY ONE SHAPE AND NOT A MANGLER. A single-level
# namespace, a function taking NO arguments, on one physical line. That
# mangles to _ZN<len><NS><len><Name>Ev and nothing else does. Templates,
# nested namespaces, overloads, arguments, and out-of-line class methods are
# NOT attempted -- Itanium argument encoding with its substitution rules is a
# project, and a half-written mangler produces confident wrong symbol names,
# which is worse than a blind spot somebody wrote down. The remaining blind
# spot is stated in port/abi_checks.txt section 7.
NS_DECL = re.compile(
    r'^[ \t]*namespace\s+(?P<ns>[A-Za-z_]\w*)\s*\{\s*'
    r'(?:const\s+)?[A-Za-z_]\w*(?:\s*\*)*[\s*]+'
    r'(?P<name>[A-Za-z_]\w*)\s*\(\s*(?:void\s*)?\)\s*;\s*\}', re.M)


def itanium_nullary(ns, name):
    """_ZN<len><ns><len><name>Ev -- the ONE shape NS_DECL admits."""
    return '_ZN%d%s%d%sEv' % (len(ns), ns, len(name), name)


def split_params(p):
    """The individual parameter texts of a parameter list."""
    p = p.strip()
    if p in ('', 'void'):
        return []
    depth, cur, out = 0, '', []
    for ch in p:
        if ch in '(<[':
            depth += 1
        elif ch in ')>]':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur)
            cur = ''
        else:
            cur += ch
    out.append(cur)
    return [x.strip() for x in out if x.strip()]


def nparams(p):
    return len(split_params(p))


def strip_comments(src):
    src = re.sub(r'/\*.*?\*/', '', src, flags=re.S)
    return re.sub(r'//[^\n]*', '', src)


def strip_strings(src):
    """String and char literals emptied, so a comma or parenthesis inside a
    literal cannot masquerade as an argument separator to the call reader.
    The literal "C" survives, because emptying it turns `extern "C"` into
    `extern ""`, at which point DECL no longer recognises the declaration in
    the stripped text and the declaration's own parenthesis reads as a
    call -- the selftest's never-called fixture caught exactly that."""
    src = re.sub(r'"(?:[^"\\\n]|\\.)*"',
                 lambda m: m.group(0) if m.group(0) == '"C"' else '""', src)
    return re.sub(r"'(?:[^'\\\n]|\\.)*'", "''", src)


# WHAT "PROVABLY ONE WORD" MEANS, for the plain-name ratchet. The census
# compares parameter COUNTS, but the defect is about WORDS: the slots a
# caller writes versus the slots a callee reads. For a pointer, a reference,
# or a named scalar type the two are the same on ARM AAPCS and x86 __cdecl
# alike. For everything else they can diverge -- a struct passed by value
# spans several words (transcribers legitimately spell `int a, int b` as one
# `struct Pair p`), long long and double span two, a defaulted C++ parameter
# need not be passed at all, and a variadic definition reads whatever the
# caller felt like. None of those spellings is a defect, so none of them may
# trip a ratchet; rows carrying one are HELD OUT and printed instead.
ONE_WORD_TYPES = frozenset((
    'int', 'char', 'short', 'long', 'bool', 'float', 'unsigned', 'signed',
    'size_t', 'u8', 's8', 'u16', 's16', 'u32', 's32', 'uint', 'BOOL',
    'int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t'))
_PARAM_SHAPE = re.compile(r'^[A-Za-z_][\w\s*&]*(?:\[\s*\d*\s*\])?$')


def words_provable(ptext):
    """True when EVERY parameter in this list provably occupies one word."""
    for param in split_params(ptext):
        if '=' in param or not _PARAM_SHAPE.match(param):
            return False        # a default argument, `...`, or a misparse
        if '*' in param or '&' in param or '[' in param:
            continue            # pointers, references, decayed arrays
        toks = param.replace('const', ' ').split()
        if toks.count('long') > 1 or 'double' in toks:
            return False        # two words
        if len(toks) > 1:
            toks = toks[:-1]    # drop the parameter's name
        if not toks or not all(t in ONE_WORD_TYPES for t in toks):
            return False        # a by-value type this reader cannot size
    return True


def calls_symbol(rel, texts, name, memo):
    """Does this TU CALL name -- not declare it, not define it, not merely
    take its address? A declaration whose symbol is only ever address-taken
    (the __sinit destructor-registration shape passes &func_020178b4 into
    the teardown chain) constrains no call, so its arity is inert.

    `memo` is scan-local on purpose: its keys are root-relative paths, and a
    cache that outlives one scan would bleed one tree's answers into the
    next tree's rows (the selftest scans several scratch trees in a row)."""
    spankey = ('#spans', rel)
    if spankey not in memo:
        s = strip_strings(texts[rel])
        spans = [(m.start(), m.end()) for m in DECL.finditer(s)]
        spans += [(m.start(), m.end()) for m in DEFN.finditer(s)]
        memo[spankey] = (s, spans)
    s, spans = memo[spankey]
    key = (rel, name)
    if key not in memo:
        hit = False
        for m in re.finditer(r'\b%s\s*\(' % re.escape(name), s):
            if not any(a <= m.start() < b for a, b in spans):
                hit = True
                break
        memo[key] = hit
    return memo[key]


def _includes(src, header_base):
    return re.search(r'#\s*include\s*[<"][^">]*\b%s[">]'
                     % re.escape(header_base), src) is not None


def scan(root):
    defs, decls = {}, defaultdict(list)
    bound = ns_bindings(root)
    ns_skipped = []
    texts = {}
    for d in DIRS:
        for dirpath, _, files in os.walk(os.path.join(root, d)):
            for fn in sorted(files):
                if not fn.endswith(('.c', '.cpp', '.h')):
                    continue
                p = os.path.join(dirpath, fn)
                try:
                    src = open(p, encoding='utf-8', errors='replace').read()
                except OSError:
                    continue
                rel = os.path.relpath(p, root).replace(os.sep, '/')
                src = strip_comments(src)
                texts[rel] = src
                for m in DEFN.finditer(src):
                    n = m.group('name')
                    if n in KEYWORDS:
                        continue
                    defs.setdefault(n, (nparams(m.group('p')), rel,
                                        src.count('\n', 0, m.start()) + 1,
                                        m.group('p').strip()))
                for m in DECL.finditer(src):
                    n = m.group('name')
                    if n in KEYWORDS:
                        continue
                    decls[n].append((nparams(m.group('p')), rel,
                                     src.count('\n', 0, m.start()) + 1,
                                     m.group('p').strip()))
                for m in NS_DECL.finditer(src):
                    n = itanium_nullary(m.group('ns'), m.group('name'))
                    line = src.count('\n', 0, m.start()) + 1
                    if n not in bound:
                        # Nothing binds this spelling to the flat Itanium
                        # body, so the flat body is not what it calls. See
                        # NS BINDING at the top of this file.
                        ns_skipped.append((n, rel, line))
                        continue
                    decls[n].append((0, rel, line, 'void'))
    rows = []
    for name, (dn, dfile, dline, dtext) in sorted(defs.items()):
        for cn, cfile, cline, ctext in decls.get(name, []):
            if cn == dn:
                continue
            rows.append(dict(sym=name, def_n=dn, def_file=dfile,
                             def_line=dline, def_text=dtext, decl_n=cn,
                             decl_file=cfile, decl_line=cline,
                             decl_text=ctext,
                             kind='DROPS' if cn < dn else 'INVENTS',
                             # a DECLARED TAIL-JUMP RIDE: the frame reuses its
                             # caller's argument frame, so the slot the callee
                             # reads was written. Census yes, ratchet no.
                             # See RIDE at the top of this file.
                             ride=((cfile, name) in RIDES),
                             # the receiver-dropping shape specifically: a
                             # declaration with NO parameters against a
                             # definition whose first one is the receiver
                             receiver=(cn == 0 and dn >= 1
                                       and name.startswith('_ZN')
                                       and (cfile, name) not in RIDES)))

    # THE PLAIN-NAME SUBSET, classified over the finished rows because it
    # needs what a single declaration match cannot see: whether the
    # declaring TU actually calls the symbol. See the header for the three
    # filters and the measured reason each exists.
    memo = {}
    for r in rows:
        r['plainfunc'] = False
        r['plainheld'] = False
        if r['kind'] != 'DROPS' or not PLAIN.match(r['sym']) or r['ride']:
            continue
        ext = r['decl_file'].rsplit('.', 1)[-1]
        # A PROTOTYPE constrains the calls in its TU; C's empty parentheses
        # do not (the calls still pass whatever the transcriber wrote).
        # In C++ an empty list IS a zero-parameter prototype.
        if not (r['decl_n'] >= 1 or r['decl_text'] == 'void'
                or (r['decl_text'] == '' and ext == 'cpp')):
            continue
        if ext == 'h':
            # a header calls nothing itself; its prototype constrains the
            # TUs that include it, so the call is looked for there
            base = r['decl_file'].rsplit('/', 1)[-1]
            called = any(
                rel2.rsplit('.', 1)[-1] in ('c', 'cpp')
                and r['sym'] in txt2 and _includes(txt2, base)
                and calls_symbol(rel2, texts, r['sym'], memo)
                for rel2, txt2 in texts.items())
        else:
            called = calls_symbol(r['decl_file'], texts, r['sym'], memo)
        if not called:
            continue
        if (words_provable(r['decl_text'])
                and words_provable(r['def_text'])):
            r['plainfunc'] = True
        else:
            r['plainheld'] = True
    return defs, decls, rows, ns_skipped


def receiver_key(r):
    """A ratchet key that survives edits above the row.

    Symbol plus the DECLARING file, never the line number: a row keyed by
    line moves every time somebody adds a comment, and a baseline that churns
    is a baseline nobody trusts. Two declarations of the same symbol in one
    file collapse to one key, which is the right resolution here -- fixing
    the file fixes both.
    """
    return '%s|%s' % (r['sym'], r['decl_file'])


def load_receiver_baseline(path=RECEIVER_BASELINE):
    rows = set()
    if os.path.isfile(path):
        with open(path, 'r', encoding='utf-8') as f:
            for ln in f:
                ln = ln.split('#', 1)[0].strip()
                if ln:
                    rows.add(ln)
    return rows


def selftest():
    bad = 0
    print('aritycheck --selftest')
    cases = [
        ('int,int', 2, 'two parameters'),
        ('void', 0, 'explicit void'),
        ('', 0, 'empty list'),
        ('void *self', 1, 'one pointer'),
        ('void *self, struct A (*cb)(int,int), int n', 3,
         'a function-pointer parameter is ONE parameter'),
        ('T<int,int> a, int b', 2, 'a template argument list is not a comma'),
    ]
    print('\n  PARAMETER COUNTING, %d fixtures' % len(cases))
    for text, want, note in cases:
        got = nparams(text)
        ok = got == want
        bad += 0 if ok else 1
        print('    %-4s want %-3s got %-3s  %s'
              % ('ok' if ok else 'FAIL', want, got, note))

    print('\n  DECL/DEFN RECOGNITION')
    sample = ('extern void _ZN9ActorBase18MarkForDestructionEv(void);\n'
              'void _ZN9ActorBase18MarkForDestructionEv(void *self)\n'
              '{ return; }\n'
              '/* void _ZN4Fake4CommentEv(void); */\n')
    s = strip_comments(sample)
    d = [m.group('name') for m in DECL.finditer(s)]
    f = [m.group('name') for m in DEFN.finditer(s)]
    ok = d == ['_ZN9ActorBase18MarkForDestructionEv'] and \
        f == ['_ZN9ActorBase18MarkForDestructionEv']
    bad += 0 if ok else 1
    print('    %-4s one declaration, one definition, comment ignored '
          '(decls=%s defns=%s)' % ('ok' if ok else 'FAIL', d, f))

    print('\n  NAMESPACED C++ DECLARATION (the sixth-defect blind spot)')
    ns_cases = [
        ('namespace Player { void St_EndingFly_Main(); }',
         ['_ZN6Player17St_EndingFly_MainEv'],
         'the real shape, src/func_ov007_020b7764.cpp:2'),
        ('namespace Player { void St_EndingFly_Main(void); }',
         ['_ZN6Player17St_EndingFly_MainEv'],
         'explicit void is the same symbol'),
        ('namespace A { int f(); }', ['_ZN1A1fEv'], 'short names'),
        # Everything below is OUT OF SCOPE ON PURPOSE. A half-written Itanium
        # mangler emits confident wrong symbol names, which is worse than a
        # blind spot somebody wrote down, so these must produce NOTHING.
        ('namespace Player { void f(int); }', [],
         'takes an argument: not attempted'),
        ('namespace A { namespace B { void f(); } }', [],
         'nested namespace: not attempted'),
        ('namespace Player { void f() { return; } }', [],
         'a definition, not a declaration'),
        ('struct Player { void f(); };', [], 'a class, not a namespace'),
    ]
    for text, want, note in ns_cases:
        got = [itanium_nullary(m.group('ns'), m.group('name'))
               for m in NS_DECL.finditer(text)]
        ok = got == want
        bad += 0 if ok else 1
        print('    %-4s %-34s %s' % ('ok' if ok else 'FAIL',
                                     (got or ['(nothing)'])[0], note))

    print('\n  NS BINDING -- a namespaced declaration only reaches the flat')
    print('               Itanium body through an /alternatename that says so')
    import shutil
    import tempfile
    PRAGMA = ('#pragma comment(linker, "/alternatename:'
              '?St_EndingFly_Main@Player@@YAXXZ='
              '__ZN6Player17St_EndingFly_MainEv")\n')
    SYM = '_ZN6Player17St_EndingFly_MainEv'
    ns_bind_cases = [
        ('port/hal/f.cpp', PRAGMA, True,
         'a real pragma in port/hal BINDS the spelling'),
        # The shape the tree actually carries: a deleted directive annotated
        # in the comment block that records the deletion. It is the text
        # without the #pragma, which is what alternatename_guard keys on --
        # a full #pragma line inside a /* */ IS still read as an input,
        # deliberately, because that reader does not parse comments.
        ('port/hal/f.cpp',
         '// (DELETED by lane RF1) /alternatename:'
         '?St_EndingFly_Main@Player@@YAXXZ='
         '__ZN6Player17St_EndingFly_MainEv\n', False,
         'the directive QUOTED in a comment, no pragma, binds nothing'),
        ('port/notes.txt', PRAGMA, False,
         'and quoted in a .txt binds nothing either'),
        ('port/hal/f.cpp', '', False,
         'no directive at all: the flat body is not the callee'),
    ]
    for rel, body, want, note in ns_bind_cases:
        tmp = tempfile.mkdtemp(prefix='aritycheck_nsbind_')
        try:
            path = os.path.join(tmp, rel.replace('/', os.sep))
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, 'w', encoding='utf-8') as f:
                f.write(body)
            got = SYM in ns_bindings(tmp)
        finally:
            shutil.rmtree(tmp, ignore_errors=True)
        ok = got == want
        bad += 0 if ok else 1
        print('    %-4s bound=%-5s %s' % ('ok' if ok else 'FAIL', got, note))

    print('\n  RIDE -- a declared tail-jump frame cannot drop the argument')
    ride_cases = [
        (('src/_ZN13HeapAllocator7DestroyEv.cpp',
          '_ZN13HeapAllocator6RemoveEv'), True,
         'the arm9 heap veneer this exclusion was added for'),
        (('src/func_0204ebb8.c', '_ZN13HeapAllocator6RemoveEv'), True,
         'its SolidHeap sibling'),
        (('src/func_ov007_020c05f8.c',
          '_ZN6Player17St_EndingFly_MainEv'), True,
         'a Class C row that predates this exclusion'),
        (('src/func_ov007_020add3c.c', 'func_ov007_020ae558'), False,
         'CLASS A is form=call: a REAL seam and NOT excluded'),
        (('src/_ZN13HeapAllocator7DestroyEv.cpp', 'func_0205ac5c'), False,
         'right frame, wrong callee: the pair is the key, not the file'),
    ]
    for key, want, note in ride_cases:
        got = key in RIDES
        ok = got == want
        bad += 0 if ok else 1
        print('    %-4s ride=%-5s %s' % ('ok' if ok else 'FAIL', got, note))

    print('\n  STATEMENTS ARE NOT DECLARATIONS (the else/return misparse)')
    stmt_cases = [
        ('return func_02aaaa04(a, b);', [],
         'a return-call is a call, not a declaration'),
        ('else\n    func_02aaaa04(a, b);', [],
         'an else line then an indented call is not a declaration'),
        ('extern int func_02aaaa04(void);', ['func_02aaaa04'],
         'a real declaration still is'),
    ]
    for text, want, note in stmt_cases:
        got = [m.group('name') for m in DECL.finditer(text)]
        ok = got == want
        bad += 0 if ok else 1
        print('    %-4s decls=%-18s %s'
              % ('ok' if ok else 'FAIL', got or ['(nothing)'], note))

    print('\n  ONE-WORD PROVABILITY (the plain-name ratchet\'s word test)')
    word_cases = [
        ('char *c', True, 'a pointer is one word'),
        ('int a, int b, int c', True, 'named scalars'),
        ('void', True, 'no parameters at all'),
        ('unsigned int size', True, 'a multi-token scalar'),
        ('int a, struct Pair p', False,
         'a struct by value cannot be sized here'),
        ('Pair p', False, 'nor an unknown type by value'),
        ('int a, int b = 7', False, 'a default need not be passed'),
        ('int w0, ...', False, 'a variadic definition reads anything'),
        ('long long x', False, 'two words'),
        ('double x', False, 'two words'),
    ]
    for text, want, note in word_cases:
        got = words_provable(text)
        ok = got == want
        bad += 0 if ok else 1
        print('    %-4s provable=%-5s (%s)  %s'
              % ('ok' if ok else 'FAIL', got, text, note))

    print('\n  PLAIN-NAME SUBSET -- a prototype, a call through it, and '
          'provable words')

    def plain_tree(files):
        tmp = tempfile.mkdtemp(prefix='aritycheck_plain_')
        try:
            for rel, body in files.items():
                path = os.path.join(tmp, rel.replace('/', os.sep))
                os.makedirs(os.path.dirname(path), exist_ok=True)
                with open(path, 'w', encoding='utf-8') as f:
                    f.write(body)
            _, _, prows, _ = scan(tmp)
            return {(r['sym'], r['decl_file']):
                    (r['plainfunc'], r['plainheld']) for r in prows}
        finally:
            shutil.rmtree(tmp, ignore_errors=True)

    DEF1 = 'int func_02aaaa04(char *c) { return 0; }\n'
    plain_cases = [
        ({'src/def.c': DEF1,
          'src/use.cpp': 'extern "C" int func_02aaaa04(void);\n'
                         'int use(void) { return func_02aaaa04(); }\n'},
         ('func_02aaaa04', 'src/use.cpp'), (True, False),
         "the Sort or 'Splode shape: (void) decl in a .cpp, called"),
        ({'src/def.c': DEF1,
          'src/use.cpp': 'extern "C" int func_02aaaa04(void);\n'},
         ('func_02aaaa04', 'src/use.cpp'), (False, False),
         'same declaration, never called: inert'),
        ({'src/def.c': DEF1,
          'src/use.c': 'extern int func_02aaaa04();\n'
                       'int use(void) { return func_02aaaa04(); }\n'},
         ('func_02aaaa04', 'src/use.c'), (False, False),
         'C empty parentheses are not a prototype'),
        ({'src/def.c': DEF1,
          'src/use.c': 'extern int func_02aaaa04(void);\n'
                       'int use(void) { return func_02aaaa04(); }\n'},
         ('func_02aaaa04', 'src/use.c'), (True, False),
         'explicit (void) in C is'),
        ({'src/def.c': DEF1,
          'src/use.c': 'extern void func_02aaaa04(void);\n'
                       'extern void func_020731dc();\n'
                       'void use(void) '
                       '{ func_020731dc(&func_02aaaa04); }\n'},
         ('func_02aaaa04', 'src/use.c'), (False, False),
         'address-taken only (the __sinit shape): inert'),
        ({'src/def4.c': 'int func_02bbbb08(int a, int b, int c, int d) '
                        '{ return a; }\n',
          'src/use.c': 'struct Pair { int a, b; };\n'
                       'extern int func_02bbbb08(int a, struct Pair p);\n'
                       'int use(struct Pair q) '
                       '{ return func_02bbbb08(1, q); }\n'},
         ('func_02bbbb08', 'src/use.c'), (False, True),
         'a struct-by-value spelling drops the COUNT, not the words: HELD'),
        ({'src/defd.cpp': 'extern "C" int func_02cccc0c(int a, int b = 7) '
                          '{ return a + b; }\n',
          'src/use.cpp': 'extern "C" int func_02cccc0c(int a);\n'
                         'int use(void) { return func_02cccc0c(3); }\n'},
         ('func_02cccc0c', 'src/use.cpp'), (False, True),
         'a defaulted definition makes short calls legal: HELD'),
        ({'src/def.c': DEF1,
          'include/decl_x.h': 'extern int func_02aaaa04(void);\n',
          'src/use.c': '#include "decl_x.h"\n'
                       'int use(void) { return func_02aaaa04(); }\n'},
         ('func_02aaaa04', 'include/decl_x.h'), (True, False),
         'a header prototype is called through the TU that includes it'),
        ({'src/def.c': DEF1,
          'include/decl_x.h': 'extern int func_02aaaa04(void);\n',
          'src/use.c': 'int use(void) { return 0; }\n'},
         ('func_02aaaa04', 'include/decl_x.h'), (False, False),
         'the same header with no calling includer: inert'),
    ]
    for files, key, want, note in plain_cases:
        got = plain_tree(files).get(key, ('(no row)', '(no row)'))
        ok = got == want
        bad += 0 if ok else 1
        print('    %-4s gate=%-5s held=%-5s  %s'
              % ('ok' if ok else 'FAIL', got[0], got[1], note))

    print('\n  DEMANGLER (shared, port/tools/msvc_undname.py)')
    rc = mu.selftest()
    if rc == 1:
        bad += 1

    print('\n%s' % ('SELFTEST PASSED' if not bad
                    else 'SELFTEST FAILED (%d)' % bad))
    return 1 if bad else 0


def main(argv):
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('root', nargs='?', default=DEFAULT_ROOT)
    ap.add_argument('--drops-only', action='store_true',
                    help='print the RECEIVER subset rather than everything')
    ap.add_argument('--gate-receiver', action='store_true',
                    help='exit 1 on a RECEIVER-shape row that is not in '
                         'aritycheck_receiver_baseline.txt')
    ap.add_argument('--gate-plainfunc', action='store_true',
                    help='exit 1 on a PLAIN-NAME row that is not in '
                         'aritycheck_plainfunc_baseline.txt')
    ap.add_argument('--write-receiver-baseline', action='store_true',
                    help='rewrite the baseline body from this tree. Only ever '
                         'correct when the rows have been looked at.')
    ap.add_argument('--write-plainfunc-baseline', action='store_true',
                    help='append this tree\'s plain-name rows to the '
                         'baseline. Only ever correct when the rows have '
                         'been looked at.')
    ap.add_argument('--selftest', action='store_true')
    ap.add_argument('--json', metavar='PATH')
    ap.add_argument('--limit', type=int, default=40,
                    help='rows to print (0 for all); the census counts are '
                             'always complete')
    args = ap.parse_args(argv[1:])

    if args.selftest:
        return selftest()

    root = os.path.abspath(args.root)
    defs, decls, rows, ns_skipped = scan(root)
    drops = [r for r in rows if r['kind'] == 'DROPS']
    invents = [r for r in rows if r['kind'] == 'INVENTS']
    recv = [r for r in rows if r['receiver']]
    rides = [r for r in rows if r['ride']]
    plain = [r for r in rows if r['plainfunc']]
    plainheld = [r for r in rows if r['plainheld']]

    if args.write_receiver_baseline:
        keys = sorted({receiver_key(r) for r in recv})
        with open(RECEIVER_BASELINE, 'a', encoding='utf-8') as f:
            for k in keys:
                f.write(k + '\n')
        print('appended %d receiver-shape keys to %s'
              % (len(keys), RECEIVER_BASELINE))
        return 0

    if args.write_plainfunc_baseline:
        keys = sorted({receiver_key(r) for r in plain})
        with open(PLAINFUNC_BASELINE, 'a', encoding='utf-8') as f:
            for k in keys:
                f.write(k + '\n')
        print('appended %d plain-name keys to %s'
              % (len(keys), PLAINFUNC_BASELINE))
        return 0

    print('aritycheck -- cross-TU arity disagreement census')
    print('  %d definitions, %d declarations of _ZN*/func_* symbols'
          % (len(defs), sum(len(v) for v in decls.values())))
    print('  %d disagreements: %d DROPS, %d INVENTS'
          % (len(rows), len(drops), len(invents)))
    print('  %d of the DROPS are the RECEIVER shape (declared with no '
          'parameters at all against an _ZN member definition that takes '
          'one)' % len(recv))
    # NEITHER EXCLUSION IS SILENT. An exclusion nobody can see is how a
    # checker stops checking without anybody noticing, which is stage 0 of
    # run_checks' whole reason for existing.
    print('  %d census row(s) held OUT of the receiver subset as a declared '
          'tail-jump RIDE (tailjump_guard asserts the frame jumps, so the '
          'caller\'s slot is the one the callee reads)' % len(rides))
    for r in sorted(rides, key=lambda x: (x['decl_file'], x['sym'])):
        print('      ride: %s|%s' % (r['sym'], r['decl_file']))
    print('  %d namespaced C++ declaration(s) read but NOT counted: no '
          '/alternatename binds the spelling to the flat Itanium body, so '
          'that body is not what they call' % len(ns_skipped))
    for sym, rel, line in sorted(ns_skipped):
        print('      unbound: %s  %s:%d' % (sym, rel, line))
    print('  %d of the DROPS are the PLAIN-NAME shape (a prototyped '
          'declaration of a plain func_* symbol, called through, short of '
          'the definition); %d more match that shape but are HELD OUT '
          'because a parameter is not provably one word (struct-by-value '
          'spellings, default arguments, variadic definitions)'
          % (len(plain), len(plainheld)))
    for r in sorted(plainheld, key=lambda x: receiver_key(x)):
        print('      held: %s' % receiver_key(r))
    print('  The full census is REPORT ONLY. See the header for why.')

    failed = 0
    if args.gate_receiver:
        base = load_receiver_baseline()
        keys = {receiver_key(r) for r in recv}
        new = sorted(keys - base)
        gone = sorted(base - keys)
        print('\n--- RECEIVER RATCHET (%s) ---'
              % os.path.basename(RECEIVER_BASELINE))
        print('  %d baselined, %d live, %d NEW, %d retired'
              % (len(base), len(keys), len(new), len(gone)))
        for k in gone:
            print('  retired (delete the row): %s' % k)
        if new:
            print('\n  NEW RECEIVER-SHAPE ROWS -- a declaration takes no '
                  'parameters where the definition takes a receiver:')
            for r in sorted(recv, key=lambda r: receiver_key(r)):
                if receiver_key(r) not in base:
                    print('    %s' % r['sym'])
                    print('        declared %d param(s) at %s:%d'
                          % (r['decl_n'], r['decl_file'], r['decl_line']))
                    print('        DEFINED  %d param(s) at %s:%d'
                          % (r['def_n'], r['def_file'], r['def_line']))
            print('\nARITYCHECK RECEIVER RATCHET FAILED: %d new row(s). A '
                  'member\'s receiver rides r0 on ARM like any other first '
                  'argument; declare it and pass it, the way #1539 and #1543 '
                  'did on 2026-08-16.' % len(new))
            failed += 1
        else:
            print('\nARITYCHECK RECEIVER RATCHET PASSED: no new '
                  'receiver-shape row. The %d baselined rows are '
                  'unadjudicated DEBT, not clearance.' % len(base))

    if args.gate_plainfunc:
        base = load_receiver_baseline(PLAINFUNC_BASELINE)
        keys = {receiver_key(r) for r in plain}
        new = sorted(keys - base)
        gone = sorted(base - keys)
        print('\n--- PLAIN-NAME RATCHET (%s) ---'
              % os.path.basename(PLAINFUNC_BASELINE))
        print('  %d baselined, %d live, %d NEW, %d retired'
              % (len(base), len(keys), len(new), len(gone)))
        for k in gone:
            print('  retired (delete the row): %s' % k)
        if new:
            print('\n  NEW PLAIN-NAME ROWS -- a prototyped declaration '
                  'drops argument words the definition reads, and the '
                  'declaring TU calls through it:')
            for r in sorted(plain, key=lambda r: receiver_key(r)):
                if receiver_key(r) not in base:
                    print('    %s' % r['sym'])
                    print('        declared (%s) at %s:%d'
                          % (r['decl_text'], r['decl_file'], r['decl_line']))
                    print('        DEFINED  (%s) at %s:%d'
                          % (r['def_text'], r['def_file'], r['def_line']))
            print('\nARITYCHECK PLAIN-NAME RATCHET FAILED: %d new row(s). '
                  'On ARM the dropped argument can ride a register that is '
                  'still live, so the byte gate stays green over the '
                  'defect; on the host the callee reads a stack slot '
                  'nobody wrote. Declare the full signature and pass it, '
                  'the way ba2224f88 did for the Sort or \'Splode bin-full '
                  'softlock.' % len(new))
            failed += 1
        else:
            print('\nARITYCHECK PLAIN-NAME RATCHET PASSED: no new '
                  'plain-name row. The %d baselined rows are unadjudicated '
                  'DEBT, not clearance.' % len(base))

    if failed:
        return 1

    show = (recv if args.drops_only else rows)
    lim = len(show) if args.limit == 0 else min(args.limit, len(show))
    if args.gate_receiver or args.gate_plainfunc:
        # A green ratchet has already said everything worth saying; dumping
        # the frozen rows behind it turns a one-line pass into 500 lines of
        # scroll nobody reads, and a gate nobody reads is not a gate.
        lim = 0
    if lim:
        print('\n--- %d of %d rows ---' % (lim, len(show)))
    for r in show[:lim]:
        print('  %-8s %s%s' % (r['kind'], r['sym'],
                               '   [RECEIVER SHAPE]' if r['receiver'] else ''))
        print('      declared %d param(s) at %s:%d'
              % (r['decl_n'], r['decl_file'], r['decl_line']))
        print('      DEFINED  %d param(s) at %s:%d'
              % (r['def_n'], r['def_file'], r['def_line']))

    if args.json:
        with open(args.json, 'w', encoding='utf-8') as f:
            json.dump(rows, f, indent=1)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
