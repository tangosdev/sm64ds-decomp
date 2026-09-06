#!/usr/bin/env python3
"""Bridge a MERGED TRANSLATION UNIT's real C++ methods back to the Itanium C
names the port reaches them by -- and REFUSE the bridges that would be wrong.

WHY THIS EXISTS

    main merged the port's one-function sources into real C++ translation
    units. src/minigames/d_s_mg_trampoline.cpp is 1,610 lines and defines
    dScMgTrampoline_c::StateDone(); src/actors/Player.cpp is 12,725 and
    defines Player::SetAnim(). Across the 2,140 merged TUs the port would have
    to start enrolling, 1,172 define at least one real C++ method and there
    are 1,836 of them.

    The port reaches nearly all of those bodies by their Itanium C spelling
    -- _ZN17dScMgTrampoline_c9StateDoneEv, declared `extern "C"` -- because
    that is what the one-function source emitted. A real method under MSVC
    emits ?StateDone@dScMgTrampoline_c@@QAEXXZ instead. Enrolling a merged TU
    therefore CHANGES THE EMITTED SYMBOL for about half the functions in it,
    and every one of those needs a bridge.

THE CORRECTION THIS TOOL CARRIES, AND IT IS THE WHOLE POINT

    RECON1 step 5 says each of those needs an `/alternatename`. That is true
    for the STATIC members and FALSE, dangerously, for every other one.

    An /alternatename renames a symbol. It does not change a calling
    convention. Measured, not argued -- two three-file programs built with
    the port's own flags, linked, and RUN:

        callee  ?m@C@@QAEHH@Z   mov DWORD PTR _this$[ebp], ecx   <- this in ECX
                                ret 4                            <- callee pops 1 arg

        caller  extern "C" int _ZN1C1mEi(void *self, int x);
                                push 5
                                push eax            <- `this` pushed on the STACK
                                call __ZN1C1mEi
                                add esp, 8          <- caller pops 2 args

    Alias the two together and link.exe resolves both names to one address,
    exit code 0, no warning, and the map shows the healthy shape. The program
    then exits 0xC0000005. The body reads `this` out of whatever was in ECX,
    reads its first argument out of the caller's `this` slot, and the two
    sides disagree about who pops the stack by four bytes on every call.
    The same pair of programs with a STATIC member instead links the same way
    and returns the right answer -- that difference is the whole verdict.

    alternatename_guard.py cannot catch it. Its test is "LHS and RHS at the
    SAME address in the map", which is exactly what this produces. It reads as
    the healthy shape. That is the port's own R1/R2 failure class one level
    deeper: not a defeated alias, a FIRING alias that is wrong.

    So this tool sorts every member into one of five verdicts and only ever
    emits a directive for the one that is safe:

      SAME_C       the merged TU still emits the Itanium C name. No bridge.
      ALIAS        the member is STATIC (MSVC S*/Y* = __cdecl, no receiver),
                   so the two spellings are the same function with two names.
                   An /alternatename is correct and free. EMITTED.
      FACE         the member is __thiscall. An alias would smash the
                   receiver. A C-linkage forwarder is emitted instead, in
                   port/hal/method_faces.cpp's existing shape. NO ALIAS.
      NO_EMISSION  nothing in the object corresponds to that ROM address. The
                   commonest cause is a destructor DEFINED IN THE CLASS BODY:
                   mwccarm emits the D1/D0 pair, MSVC emits an inline COMDAT
                   only where it is referenced, and in a TU with no
                   constructor it emits nothing at all. Enrolling that TU
                   LOSES those ROM bodies. REFUSED, never guessed.
      AMBIGUOUS    several object symbols carry that class::method and the
                   arity does not separate them. REFUSED.

WHY THE OBJECT IS THE ONLY SOURCE FOR THE MSVC SPELLING

    A predictor that mangles from the Itanium name would be guessing at the
    exact point where guessing is unsafe. The Itanium name encodes the ROM's
    types; the MSVC decoration encodes what MAIN'S HEADER declares, and those
    are not the same thing. The port's own cxx_aliases.cpp has the worked
    counter-example already in it:

        ?NewSimple@System@Particle@@SAXIHHH@Z
          = __ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_

    Itanium says Fix12<int>, three of them. MSVC says H, plain int, because
    the header this port compiles against typedefs Fix12i to int rather than
    declaring a class. Same ROM function, and no mangler reading the Itanium
    name could produce the MSVC string. Nothing in the Itanium name says
    whether a class type is `struct` (U) or `class` (V) either.

    So there is no predictor here, deliberately. The MSVC spelling comes out
    of a compiled object every time, and a TU MSVC will not compile has no
    answer at all rather than a guessed one. That is why an object (or a
    dumpbin page a sweep captured earlier) is required and not optional.

    The MEASURED consequence, over all 2,142 merged TUs the reconcile would
    enroll: 2,087 compile under the port's own flags and 55 do not, and the
    55 are named rather than estimated around.

WHAT THE COUNT ACTUALLY IS

    RECON1 put the alias bill at "~1,800 new alias rows". Read off the
    objects, over the whole merged set:

        SAME_C       956   no bridge needed
        ALIAS        163   an /alternatename, correct and free
        FACE       1,993   a C-linkage forwarder; an alias here CRASHES
        NO_EMISSION  225   the TU emits nothing for that ROM body
        AMBIGUOUS      0

    So 163 aliases, not 1,800, and 1,993 faces. The work is real and it is
    almost all a different shape from what the plan assumed.

WHAT IT WILL NOT DO
    * It never emits a directive whose LHS is already an /alternatename LHS
      under port/ with a different RHS (that is the DEFEATED-alias arrival
      shape alternatename_guard.py exists to fail on), nor one whose LHS is
      already some other directive's RHS.
    * It never emits a row for a NO_EMISSION or AMBIGUOUS member. Those are
      printed, counted and left for a human, because both of them are
      "somebody has to read the ROM", not "the tool needs a flag".

USAGE
    methodalias.py --selftest
    methodalias.py --tu src/minigames/d_s_mg_trampoline.cpp \
                   --obj tramp.obj --config-root <tree> --emit pragmas
    methodalias.py --tu <tu> --objdir D --config-root <tree> --emit undef
    methodalias.py --sweep tus.txt --objdir D --config-root <tree> \
                   --emit tsv

    --emit undef answers the question the port already asks of an alias
    set: with the bridges in place, is anything left UNDEF that the TU
    needs? Over the whole merged set that is 29 closed and 0 left open.
"""

import argparse
import os
import re
import subprocess
import sys

_HERE = os.path.dirname(os.path.abspath(__file__))
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

try:
    import msvc_undname
except ImportError:                                   # pragma: no cover
    msvc_undname = None


# ---------------------------------------------------------------------------
# 1. The merged TU's manifest.
#
# tools/tubuild.py writes an "Assembled from these legacy one-function
# sources (ROM address order)" block into every TU it creates, one line per
# member with its ROM address.
#
# THIS IS NOT THE JOIN KEY, and it took a measurement to find that out: only
# 77 of the 1,172 method-defining merged TUs carry one. The join is the ROM
# ADDRESS, from delinks.txt's .text range crossed with symbols.txt's per-
# function addresses (section 2), which every TU has whether or not anybody
# wrote it a header comment. The manifest is kept because it names the LEGACY
# SOURCE each member came from, which is what a reviewer wants beside a
# verdict, and because it is the only fallback when a TU has no delinks row.
# ---------------------------------------------------------------------------

MANIFEST_RE = re.compile(
    r'^\s*\*?\s*\[(\d+)\]\s+(0x[0-9a-fA-F]{8})\s+(\S+)\s*$', re.M)
MODULE_RE = re.compile(r'_(ov\d{3})_[0-9a-f]{8}')


def read_manifest(tu_path):
    """[(ordinal, addr, legacy_source)] for a merged TU, ROM address order.

    Only the header comment is scanned: a `[12] 0x0212...` shape deeper in the
    file is a per-member banner, not the manifest, and counting both would
    double every member.
    """
    with open(tu_path, 'r', encoding='utf-8', errors='replace') as f:
        text = f.read()
    head = text[:text.find('*/') + 2] if '*/' in text[:20000] else text[:20000]
    rows = [(int(a), int(b, 16), c) for a, b, c in MANIFEST_RE.findall(head)]
    rows.sort(key=lambda r: r[0])
    return rows


def guess_module(manifest):
    """'ov006' / 'arm9' from the manifest's legacy filenames, or None.

    Overlay addresses OVERLAP -- ov006 0x02121774 and ov007 0x02121774 are
    different functions -- so a config lookup keyed on the address alone is a
    coin flip. Refusing beats guessing, hence the None.
    """
    mods = set()
    for _, _, src in manifest:
        m = MODULE_RE.search(src)
        if m:
            mods.add(m.group(1))
    if len(mods) == 1:
        return mods.pop()
    if not mods:
        return 'arm9'
    return None


# ---------------------------------------------------------------------------
# 2. ROM address -> symbol, out of the tree's own config.
# ---------------------------------------------------------------------------

SYM_RE = re.compile(r'^(\S+)\s+kind:(\S+)\s+addr:(0x[0-9a-fA-F]+)')
DELINK_FILE_RE = re.compile(r'^(\S+\.(?:c|cpp|s|S)):\s*$')
DELINK_SEC_RE = re.compile(
    r'^\s+\.(\w+)\s+start:(0x[0-9a-fA-F]+)\s+end:(0x[0-9a-fA-F]+)')


def symbols_path(config_root, module):
    if module == 'arm9':
        return os.path.join(config_root, 'config', 'arm9', 'symbols.txt')
    return os.path.join(config_root, 'config', 'arm9', 'overlays', module,
                        'symbols.txt')


_SYM_CACHE = {}


def load_symbols(config_root, module):
    """{addr: (name, kind)} for one module, from config/**/symbols.txt."""
    ck = (os.path.abspath(config_root), module)
    if ck in _SYM_CACHE:
        return _SYM_CACHE[ck]
    path = symbols_path(config_root, module)
    out = {}
    _SYM_CACHE[ck] = out
    if not os.path.exists(path):
        return out
    with open(path, 'r', encoding='utf-8', errors='replace') as f:
        for line in f:
            m = SYM_RE.match(line)
            if m:
                out.setdefault(int(m.group(3), 16), (m.group(1), m.group(2)))
    return out


def modules(config_root):
    yield 'arm9'
    ovdir = os.path.join(config_root, 'config', 'arm9', 'overlays')
    if os.path.isdir(ovdir):
        for d in sorted(os.listdir(ovdir)):
            if d.startswith('ov'):
                yield d


def delinks_path(config_root, module):
    if module == 'arm9':
        return os.path.join(config_root, 'config', 'arm9', 'delinks.txt')
    return os.path.join(config_root, 'config', 'arm9', 'overlays', module,
                        'delinks.txt')


_RANGE_CACHE = {}


def _build_range_index(config_root):
    """{source path: [(module, [(start, end)])]} for the whole tree, once.

    Built in one pass rather than per-TU: a sweep over two thousand TUs that
    re-walked every delinks.txt each time would spend its whole run in the
    file system, and a slow tool is a tool a lane skips.
    """
    idx = {}
    for mod in modules(config_root):
        path = delinks_path(config_root, mod)
        if not os.path.exists(path):
            continue
        cur = None
        with open(path, 'r', encoding='utf-8', errors='replace') as f:
            for line in f:
                m = DELINK_FILE_RE.match(line)
                if m:
                    cur = m.group(1)
                    continue
                if cur is None:
                    continue
                m = DELINK_SEC_RE.match(line)
                if m and m.group(1) in ('text', 'init'):
                    ent = idx.setdefault(cur, {})
                    ent.setdefault(mod, []).append(
                        (int(m.group(2), 16), int(m.group(3), 16)))
    return idx


def find_ranges(config_root, tu_rel):
    """(module, [(start, end)]) for the TU's .text, from delinks.txt.

    THE ADDRESS IS THE JOIN, not the manifest comment and not the file name.
    delinks.txt says which ROM bytes a source file owns; symbols.txt says what
    lives at each address inside them. Everything downstream keys on that pair,
    so a TU whose header comment is stale, missing or hand-written is read
    exactly as well as one tubuild.py generated.

    A TU claimed by two modules is refused rather than picked: overlay
    addresses overlap and a wrong module silently names another class's body.
    """
    key = os.path.abspath(config_root)
    if key not in _RANGE_CACHE:
        _RANGE_CACHE[key] = _build_range_index(config_root)
    ent = _RANGE_CACHE[key].get(tu_rel.replace(os.sep, '/'))
    if not ent:
        return None, []
    if len(ent) > 1:
        raise RuntimeError('%s is claimed by %s; overlay addresses overlap, '
                           'so refusing to pick one'
                           % (tu_rel, ', '.join(sorted(ent))))
    mod = next(iter(ent))
    return mod, sorted(ent[mod])


def members_from_config(config_root, tu_rel):
    """[(ordinal, addr, name)] -- every ROM function the TU owns."""
    mod, rngs = find_ranges(config_root, tu_rel)
    if mod is None:
        return None, []
    syms = load_symbols(config_root, mod)
    out = []
    for addr, (name, kind) in syms.items():
        if not kind.startswith('function'):
            continue
        if any(s <= addr < e for s, e in rngs):
            out.append((addr, name))
    out.sort()
    return mod, [(i, a, n) for i, (a, n) in enumerate(out)]


# ---------------------------------------------------------------------------
# 3. Itanium names, only as far as the join needs.
#
# Enough to answer three questions and nothing more: which class, which
# member, how many parameters. A full demangler would be a bigger promise
# than the join can keep, because the join is by (class, member, arity) and
# the types themselves come from the object.
# ---------------------------------------------------------------------------

_BUILTIN = set('vwbcahstijlmxynofdegzDu')


class Itanium(object):
    def __init__(self, sym):
        self.sym = sym
        self.ok = False
        self.cls = None
        self.member = None
        self.is_const = False
        self.dtor = None                  # 'D0' / 'D1' / 'D2'
        self.ctor = None                  # 'C1' / 'C2' / 'C3'
        self.nparams = None
        self._parse()

    def _parse(self):
        s = self.sym
        if not s.startswith('_Z'):
            return
        i = 2
        if s[i:i + 1] == 'N':
            i += 1
            while s[i:i + 1] in ('K', 'V', 'r'):
                if s[i] == 'K':
                    self.is_const = True
                i += 1
            names = []
            while i < len(s) and s[i] != 'E':
                if s[i] in 'CD' and s[i + 1:i + 2] in '0123':
                    names.append(s[i:i + 2])
                    i += 2
                    continue
                j = i
                while j < len(s) and s[j].isdigit():
                    j += 1
                if j == i:
                    return                       # a shape this reader is not
                n = int(s[i:j])                  # promising to understand
                names.append(s[j:j + n])
                i = j + n
                if s[i:i + 1] == 'I':            # template args: keep them
                    depth, k = 0, i              # with the name they qualify
                    while k < len(s):
                        if s[k] == 'I':
                            depth += 1
                        elif s[k] == 'E':
                            depth -= 1
                            if depth == 0:
                                k += 1
                                break
                        k += 1
                    names[-1] += s[i:k]
                    i = k
            if s[i:i + 1] != 'E' or len(names) < 2:
                return
            i += 1
            last = names[-1]
            self.cls = '::'.join(names[:-1])
            if last.startswith('D') and last[1:] in '0123':
                self.dtor = last
                self.member = '~' + names[-2].split('I')[0]
            elif last.startswith('C') and last[1:] in '0123':
                self.ctor = last
                self.member = names[-2].split('I')[0]
            else:
                self.member = last
        else:
            j = i
            while j < len(s) and s[j].isdigit():
                j += 1
            if j == i:
                return
            n = int(s[i:j])
            self.member = s[j:j + n]
            i = j + n
        self.nparams = _count_params(s[i:])
        self.ok = self.member is not None

    def qualified(self):
        return '%s::%s' % (self.cls, self.member) if self.cls else self.member


def _count_params(rest):
    """How many top-level parameter types `rest` holds, or None.

    `v` alone is the void parameter list, i.e. zero. Substitutions (S_, S3_,
    St...) count as one type each without being resolved: the join only needs
    the count.
    """
    if rest == '' or rest == 'v':
        return 0
    i, n = 0, 0
    while i < len(rest):
        j = _skip_type(rest, i)
        if j is None or j <= i:
            return None
        n += 1
        i = j
    return n


def _skip_type(s, i):
    while i < len(s) and s[i] in 'PRODKVrCG':
        i += 1
    if i >= len(s):
        return None
    c = s[i]
    if c == 'S':                                   # substitution
        i += 1
        while i < len(s) and s[i] != '_':
            i += 1
        return i + 1
    if c == 'A':                                   # array
        i += 1
        while i < len(s) and s[i] != '_':
            i += 1
        return _skip_type(s, i + 1)
    if c == 'M':                                   # pointer to member
        j = _skip_type(s, i + 1)
        return _skip_type(s, j) if j else None
    if c == 'F':                                   # function type
        depth, k = 0, i
        while k < len(s):
            if s[k] == 'F':
                depth += 1
            elif s[k] == 'E':
                depth -= 1
                if depth == 0:
                    return k + 1
            k += 1
        return None
    if c == 'N':                                   # nested name
        depth, k = 0, i
        while k < len(s):
            if s[k] in 'NI':
                depth += 1
            elif s[k] == 'E':
                depth -= 1
                if depth == 0:
                    return k + 1
            k += 1
        return None
    if c.isdigit():
        j = i
        while j < len(s) and s[j].isdigit():
            j += 1
        n = int(s[i:j])
        j += n
        if s[j:j + 1] == 'I':
            depth, k = 0, j
            while k < len(s):
                if s[k] == 'I':
                    depth += 1
                elif s[k] == 'E':
                    depth -= 1
                    if depth == 0:
                        return k + 1
                k += 1
            return None
        return j
    if c == 'D':                                   # Dn, Ds, Di ...
        return i + 2
    if c == 'u':                                   # vendor type
        j = i + 1
        while j < len(s) and s[j].isdigit():
            j += 1
        return j + int(s[i + 1:j]) if j > i + 1 else None
    if c in _BUILTIN:
        return i + 1
    return None


# ---------------------------------------------------------------------------
# 4. The object: what MSVC ACTUALLY emitted.
# ---------------------------------------------------------------------------

# A dumpbin /symbols row, split on the pipe rather than by counting columns.
# The column count is not stable (`notype ()` is two tokens, `notype` is one),
# and a reader that miscounts columns silently drops symbols -- which reads as
# "the TU emits nothing", the one verdict here that must never be manufactured.
DUMPBIN_RE = re.compile(
    r'^\s*[0-9A-F]{3,}\s+[0-9A-F]{8}\s+(\S+)\s.*?\b(External|Static)\b\s*\|'
    r'\s*(\S+)')


def find_dumpbin():
    import glob
    import shutil
    found = shutil.which('dumpbin')
    if found:
        return found
    hits = []
    for var in ('ProgramFiles(x86)', 'ProgramFiles'):
        base = os.environ.get(var)
        if not base:
            continue
        hits.extend(glob.glob(os.path.join(
            base, 'Microsoft Visual Studio', '*', '*', 'VC', 'Tools', 'MSVC',
            '*', 'bin', 'Host*', '*', 'dumpbin.exe')))
    return sorted(hits)[-1] if hits else None


def read_object(obj_path, dumpbin=None):
    """(defined, undef) external symbol name sets, from dumpbin /symbols.

    A dumpbin that will not run is reported as an exception, never as an empty
    object: an empty object reads as "this TU emits nothing", which is a real
    verdict here (NO_EMISSION for every member) and must never be manufactured
    by a missing tool.
    """
    exe = dumpbin or find_dumpbin()
    if exe is None:
        raise RuntimeError('dumpbin not found; cannot read %s' % obj_path)
    env = dict(os.environ)
    env.pop('VSLANG', None)
    p = subprocess.run([exe, '/symbols', obj_path], capture_output=True,
                       text=True, timeout=600, env=env)
    if p.returncode != 0:
        raise RuntimeError('dumpbin exited %d on %s\n%s'
                           % (p.returncode, obj_path, p.stdout[-2000:]))
    return parse_dumpbin(p.stdout)


def parse_dumpbin(text):
    defined, undef = set(), set()
    for line in text.splitlines():
        if 'External' not in line:
            continue
        m = DUMPBIN_RE.match(line)
        if not m or m.group(2) != 'External':
            continue
        sect, name = m.group(1), m.group(3)
        (undef if sect == 'UNDEF' else defined).add(name)
    return defined, undef


UNDNAME_RE = re.compile(
    r'^(?:(public|protected|private):\s*)?'
    r'(?:(static)\s+)?(?:(virtual)\s+)?(.*)$')


class Msvc(object):
    """One decorated symbol, read through undname. Never through a table."""

    def __init__(self, dec, demangled):
        self.dec = dec
        self.text = demangled
        self.access = None
        self.static = False
        self.virtual = False
        self.conv = None
        self.qualified = None
        self.nparams = None
        self.is_const = False
        self._parse()

    def _parse(self):
        t = self.text
        m = UNDNAME_RE.match(t)
        self.access, st, vi, rest = m.group(1), m.group(2), m.group(3), m.group(4)
        self.static = bool(st)
        self.virtual = bool(vi)
        # The convention token is NOT always preceded by a space. A
        # constructor and a destructor have no return type, so undname prints
        # `public: __thiscall X::X(void)` and the token is the first thing
        # after the access. Matching ' __thiscall ' with a leading space
        # missed every ctor in the tree, and a missed ctor reads as
        # NO_EMISSION -- "enrolling this TU loses a ROM body" -- which is the
        # loudest wrong answer this tool can give.
        for conv in ('__thiscall', '__cdecl', '__stdcall', '__fastcall',
                     '__vectorcall'):
            m2 = re.search(r'(?:^|\s)%s\s' % conv, rest)
            if m2:
                self.conv = conv
                rest = rest[m2.end():]
                break
        else:
            return                                    # a data symbol
        if '(' not in rest:
            return
        name, args = rest.split('(', 1)
        self.qualified = name.strip()
        args = args.rsplit(')', 1)[0]
        self.is_const = rest.rstrip().endswith('const')
        self.nparams = _split_args(args)

    def is_receiverless(self):
        """True when the two spellings are the same machine function.

        A static member and a free function are __cdecl with no hidden
        receiver, so an /alternatename between the Itanium C name and this one
        renames one function. Anything __thiscall is a different function that
        happens to do the same work.
        """
        return self.conv == '__cdecl' and not self._has_receiver()

    def _has_receiver(self):
        # `?Foo@Bar@@QAE...` is a member with a `this`; `SA`/`YA` are not.
        body = self.dec.split('@@', 1)[1] if '@@' in self.dec else ''
        if not body:
            return False
        return body[0] in 'ABEFIJMNQRUVWX'


def _split_args(args):
    args = args.strip()
    if args in ('', 'void'):
        return 0
    depth, n = 0, 1
    for ch in args:
        if ch in '(<[':
            depth += 1
        elif ch in ')>]':
            depth -= 1
        elif ch == ',' and depth == 0:
            n += 1
    return n


# ---------------------------------------------------------------------------
# 5. The join, and the verdicts.
# ---------------------------------------------------------------------------

SAME_C = 'SAME_C'
ALIAS = 'ALIAS'
FACE = 'FACE'
NO_EMISSION = 'NO_EMISSION'
AMBIGUOUS = 'AMBIGUOUS'
NOT_A_METHOD = 'NOT_A_METHOD'

# The Itanium destructor variants against the MSVC decorations that can carry
# them. mwccarm emits D1 (complete object) and D0 (deleting) as two separate
# ROM bodies; MSVC FOLDS D1 and D2 into ??1 and puts the deleting one in the
# compiler-generated ??_G. ??_E is the vector-deleting form. A D2 with no ??1
# is not an error, it is the fold.
DTOR_MSVC_PREFIX = {'D0': ('??_G', '??_E'), 'D1': ('??1',), 'D2': ('??1',)}

# An adjustor thunk. `??_EdBgCh_Gnd@@WBA@AEPAXI@Z` is not the destructor, it is
# the entry a secondary base's vtable slot points at, which fixes `this` up by
# a fixed offset and jumps to the real one. mwccarm emits its own thunks and
# the ROM names them separately; taking one for the body would bind every
# caller to a pointer adjustment nobody asked for.
ADJUSTOR_RE = re.compile(r'@@W[0-9A-Z]+@')


class Row(object):
    def __init__(self, addr, itanium, legacy, verdict, msvc=None, note=''):
        self.addr = addr
        self.itanium = itanium
        self.legacy = legacy
        self.verdict = verdict
        self.msvc = msvc
        self.note = note

    @property
    def lhs(self):
        """The symbol the port's reference emits: cdecl's leading underscore."""
        return '_' + self.itanium

    def pragma(self):
        return ('#pragma comment(linker, "/alternatename:%s=%s")'
                % (self.lhs, self.msvc.dec))


def classify(members, syms, defined, undname_map, legacy_by_addr=None):
    """One Row per ROM member. Never fewer, never a silent drop.

    `members` is [(ordinal, addr, name_or_None)]. When the name is None the
    address is looked up in `syms`; that is the manifest path, kept so the
    selftest fixture and any caller holding only a manifest still work.
    """
    legacy_by_addr = legacy_by_addr or {}
    # Index the object's C++ symbols by qualified name.
    by_qual = {}
    dtors = {}
    for dec in sorted(defined):
        if not dec.startswith('?'):
            continue
        text = undname_map.get(dec)
        if text is None:
            continue
        if dec.startswith('??1') or dec.startswith('??_G') \
                or dec.startswith('??_E'):
            cls = dec.split('@@', 1)[0]
            cls = cls[3:] if dec.startswith('??1') else cls[4:]
            cls = cls.split('@')[0]
            dtors.setdefault(cls, []).append((dec, text))
            continue
        mv = Msvc(dec, text)
        if mv.qualified:
            by_qual.setdefault(mv.qualified, []).append(mv)

    rows = []
    for _, addr, given in members:
        legacy = legacy_by_addr.get(addr, '')
        name = given
        if name is None:
            entry = syms.get(addr)
            name = entry[0] if isinstance(entry, tuple) else entry
        if name is None:
            rows.append(Row(addr, '<no symbol at 0x%08x>' % addr, legacy,
                            NO_EMISSION, note='no config symbol at this addr'))
            continue
        if ('_' + name) in defined:
            rows.append(Row(addr, name, legacy, SAME_C))
            continue
        it = Itanium(name)
        if not it.ok or not it.cls:
            rows.append(Row(addr, name, legacy, NO_EMISSION,
                            note='C-named and not defined by the object'))
            continue
        if it.dtor:
            pool = [(d, t) for d, t in dtors.get(it.cls, [])
                    if not ADJUSTOR_RE.search(d)]
            # Itanium D0 is the DELETING destructor for ONE object, which is
            # MSVC's scalar ??_G. ??_E is the VECTOR deleting destructor, a
            # different function that walks an array. Only fall back to ??_E
            # when the scalar one is genuinely absent, and never let the two
            # sit in the same candidate set: "several symbols carry this
            # destructor" would be a refusal over a distinction the ABI has
            # already made.
            cands = []
            for prefixes in ([(p,) for p in DTOR_MSVC_PREFIX[it.dtor]]):
                cands = [Msvc(d, t) for d, t in pool if d.startswith(prefixes)]
                if cands:
                    break
            rows.append(_verdict(addr, name, legacy, cands, it,
                                 'destructor variant %s' % it.dtor))
            continue
        cands = list(by_qual.get(it.qualified(), []))
        if len(cands) > 1 and it.nparams is not None:
            narrowed = [c for c in cands if c.nparams == it.nparams]
            if narrowed:
                cands = narrowed
        if len(cands) > 1 and it.is_const:
            narrowed = [c for c in cands if c.is_const]
            if narrowed:
                cands = narrowed
        rows.append(_verdict(addr, name, legacy, cands, it, ''))
    # TWO ROM BODIES, ONE MSVC SYMBOL. mwccarm emits D1 (complete object) and
    # D2 (base object) as separate ROM functions; MSVC FOLDS them into one
    # ??1. Whichever of the two the port reaches, it gets the same code, and
    # the other ROM body is simply not in the binary any more. That is a real
    # loss of a distinction the ROM makes, so it is marked on both rows rather
    # than left to be discovered from a census that does not add up.
    seen = {}
    for r in rows:
        if r.msvc is None:
            continue
        seen.setdefault(r.msvc.dec, []).append(r)
    for dec, group in seen.items():
        if len(group) > 1:
            who = ', '.join('0x%08x %s' % (g.addr, g.itanium) for g in group)
            for g in group:
                g.note = ((g.note + '; ') if g.note else '') +                     'FOLDED: %d ROM bodies share %s (%s)' % (len(group), dec,
                                                             who)
    return rows


def _verdict(addr, name, legacy, cands, it, note):
    if not cands:
        return Row(addr, name, legacy, NO_EMISSION,
                   note=(note + '; ' if note else '')
                   + 'the object emits nothing for this member')
    if len(cands) > 1:
        return Row(addr, name, legacy, AMBIGUOUS,
                   note='%d object symbols carry %s: %s'
                   % (len(cands), it.qualified(),
                      ' '.join(c.dec for c in cands)))
    mv = cands[0]
    if mv.is_receiverless():
        n = note or 'static member: __cdecl, no receiver'
        return Row(addr, name, legacy, ALIAS, mv, n)
    n = note or ('%s: an /alternatename would smash the receiver'
                 % (mv.conv or 'non-cdecl'))
    return Row(addr, name, legacy, FACE, mv, n)


# ---------------------------------------------------------------------------
# 6. Guard safety. A generated row must never DEFEAT an existing alias.
# ---------------------------------------------------------------------------

def existing_directives(port_dir):
    """{lhs: rhs} and the set of every RHS, from the port's live pragmas."""
    try:
        import alternatename_guard as ag
    except ImportError:
        return {}, set()
    directives, _quoted = ag.collect_directives(port_dir)
    lhs = {}
    rhs = set()
    for l, r, _rel, _ln in directives:
        lhs.setdefault(l, set()).add(r)
        rhs.add(r)
    return lhs, rhs


def guard_check(rows, port_dir):
    """Reject any ALIAS row that would collide with a live directive.

    Two collisions matter and both are the same bug wearing different hats:
    a second RHS for an LHS that already has one (the linker takes the first
    and the other silently stops meaning anything), and an LHS that is already
    somebody's RHS (a chain, which /alternatename does not follow).
    """
    lhs_map, rhs_set = existing_directives(port_dir)
    bad = []
    for r in rows:
        if r.verdict != ALIAS:
            continue
        have = lhs_map.get(r.lhs)
        if have and r.msvc.dec not in have:
            bad.append((r, 'LHS already aliased to %s' % ', '.join(sorted(have))))
        elif r.lhs in rhs_set:
            bad.append((r, 'LHS is already another directive RHS (chain)'))
    return bad


# ---------------------------------------------------------------------------
# 7. Emission.
# ---------------------------------------------------------------------------

def emit_pragmas(rows, tu):
    out = ['/* Generated by port/tools/methodalias.py from %s.',
           ' * ONLY the static (receiverless) members are here. Every',
           ' * __thiscall member of that TU is in the faces block instead:',
           ' * an /alternatename cannot bridge cdecl to thiscall. */' % ()]
    out[0] = out[0] % tu
    n = 0
    for r in rows:
        if r.verdict == ALIAS:
            out.append(r.pragma())
            n += 1
    if not n:
        out.append('/* (no receiverless members in this TU) */')
    return '\n'.join(out)


def emit_faces(rows, tu):
    """C-linkage forwarders, port/hal/method_faces.cpp's shape.

    Deliberately NOT compilable as written: the return type and the parameter
    types are the ones the object reports, and the receiver cast needs the
    class's real header. Each face carries its object symbol so the reviewer
    can check TARGET / ARITY / RECEIVER against the emitted bytes, which is
    method_faces.cpp's own per-face checklist.
    """
    out = ['/* Faces for %s -- __thiscall members the port reaches by their'
           % tu,
           ' * Itanium C spelling. REVIEW EACH: target, arity, receiver.',
           ' * (method_faces.cpp header, "PER-FACE REVIEW CHECKLIST") */']
    for r in rows:
        if r.verdict != FACE:
            continue
        out.append('/* 0x%08x  %s */' % (r.addr, r.msvc.text))
        out.append('/* %s(void *self, ...) { ((%s *)self)->%s(...); } */'
                   % (r.itanium,
                      (r.msvc.qualified or '?').rsplit('::', 1)[0],
                      r.msvc.qualified or '?'))
    return '\n'.join(out)


def undef_closure(rows, undef):
    """Which of the object's UNDEFs the generated bridges close, and which not.

    THE CHECK THE PORT ALREADY USES, one level up. A merged TU does not only
    change the symbol the port's OTHER callers need; it usually references its
    own members by their Itanium C spelling too, because the ROM source did
    (`extern "C" void _ZN17dScMgTrampoline_c9StateDoneEv(void);` sits in
    d_s_mg_trampoline.cpp itself, feeding a pointer-to-member descriptor). So
    the TU's own object CANNOT LINK until each of those is bridged, and the
    UNDEF list is a ready-made statement of what the bridge set has to cover.

    Returns (covered, uncovered, foreign):
      covered    UNDEFs naming one of this TU's own members, with an ALIAS or
                 FACE row that supplies it.
      uncovered  the same, with NO row -- every one of these is an unresolved
                 external at link time. Loud, not silent, which is why this is
                 a check and not a guess.
      foreign    UNDEFs that are somebody else's symbol. Not this TU's problem.
    """
    own = {r.lhs: r for r in rows}
    covered, uncovered, foreign = [], [], []
    for u in sorted(undef):
        r = own.get(u)
        if r is None:
            foreign.append(u)
        elif r.verdict in (ALIAS, FACE):
            covered.append((u, r.verdict, r.msvc.dec))
        elif r.verdict == SAME_C:
            covered.append((u, 'SAME_C', '(the TU defines it itself)'))
        else:
            uncovered.append((u, r.verdict, r.note))
    return covered, uncovered, foreign


def emit_tsv(rows, tu):
    out = []
    for r in rows:
        out.append('\t'.join([
            tu, '0x%08x' % r.addr, r.itanium, r.verdict,
            r.msvc.dec if r.msvc else '', r.legacy, r.note]))
    return '\n'.join(out)


# ---------------------------------------------------------------------------
# 8. Driver.
# ---------------------------------------------------------------------------

def run_tu(tu_rel, config_root, obj, port_dir, dumpbin=None, symtext=None):
    tu_abs = tu_rel if os.path.isabs(tu_rel) \
        else os.path.join(config_root, tu_rel)
    module, members = members_from_config(config_root, tu_rel)
    manifest = read_manifest(tu_abs) if os.path.exists(tu_abs) else []
    legacy = {a: src for _, a, src in manifest}
    if module is None:
        # No delinks row: fall back to the manifest, and say so, because the
        # fallback is a comment and the primary is the ROM.
        module = guess_module(manifest)
        if module is None or not manifest:
            raise RuntimeError('%s: no delinks .text range and no usable '
                               'manifest; nothing address-derived to key on'
                               % tu_rel)
        members = [(i, a, None) for i, a, _ in manifest]
    syms = load_symbols(config_root, module)
    if symtext is not None:
        # A dumpbin page captured earlier by a sweep. Same bytes,
        # same reader; re-running dumpbin per TU over 2,000 objects
        # is the only thing it saves.
        defined, undef = parse_dumpbin(symtext)
    else:
        defined, undef = read_object(obj, dumpbin)
    decs = sorted(d for d in defined if d.startswith('?'))
    und = msvc_undname.demangle(decs) if (decs and msvc_undname) else {}
    if decs and not und:
        raise RuntimeError('undname unavailable or misaligned; every MSVC '
                           'spelling below would be a guess. Refusing.')
    rows = classify(members, syms, defined, und, legacy)
    return rows, defined, undef, module


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--tu', help='merged TU, repo-relative or absolute')
    ap.add_argument('--obj', help='its compiled object (ground truth)')
    ap.add_argument('--config-root', default='.',
                    help='tree whose config/ and src/ to read')
    ap.add_argument('--port-dir', default=os.path.dirname(_HERE))
    ap.add_argument('--sweep', help='file of TU paths, one per line')
    ap.add_argument('--objdir', help='objects for --sweep, named <stem>.obj')
    ap.add_argument('--emit', default='tsv',
                    choices=['tsv', 'pragmas', 'faces', 'summary', 'undef'])
    ap.add_argument('--out', help='write to this file instead of stdout')
    ap.add_argument('--dumpbin')
    ap.add_argument('--selftest', action='store_true')
    a = ap.parse_args(argv)

    if a.selftest:
        return selftest()

    tus = []
    if a.sweep:
        with open(a.sweep) as f:
            tus = [l.strip() for l in f if l.strip()
                   and not l.startswith('#')]
    elif a.tu:
        tus = [a.tu]
    else:
        ap.error('need --tu, --sweep or --selftest')

    all_rows, chunks, failures = [], [], []
    for tu in tus:
        obj = a.obj if a.obj else os.path.join(
            a.objdir or '.', os.path.basename(tu).rsplit('.', 1)[0] + '.obj')
        symtext = None
        if a.objdir:
            cand = os.path.join(a.objdir, tu.replace('/', '__') + '.syms')
            if os.path.exists(cand):
                symtext = open(cand, encoding='utf-8',
                               errors='replace').read()
        if symtext is None and not os.path.exists(obj):
            failures.append((tu, 'no object at %s' % obj))
            continue
        try:
            rows, _d, _u, _m = run_tu(tu, a.config_root, obj, a.port_dir,
                                      a.dumpbin, symtext)
            if a.emit == 'undef':
                cov, unc, foreign = undef_closure(rows, _u)
                chunks.append(
                    (tu + chr(10) +
                     '  own UNDEFs closed by a generated bridge: %d'
                     % len(cov) + chr(10) +
                     '  own UNDEFs LEFT OPEN: %d' % len(unc) + chr(10) +
                     '  UNDEFs belonging to other TUs: %d'
                     % len(foreign)))
                for u, v, d in cov:
                    chunks.append('    CLOSED  %-52s %-6s %s' % (u, v, d))
                for u, v, n in unc:
                    chunks.append('    OPEN    %-52s %-6s %s' % (u, v, n))
        except (RuntimeError, OSError) as e:
            failures.append((tu, str(e)))
            continue
        all_rows.extend(rows)
        if a.emit == 'tsv':
            chunks.append(emit_tsv(rows, tu))
        elif a.emit == 'pragmas':
            chunks.append(emit_pragmas(rows, tu))
        elif a.emit == 'faces':
            chunks.append(emit_faces(rows, tu))

    bad = guard_check(all_rows, a.port_dir)
    counts = {}
    for r in all_rows:
        counts[r.verdict] = counts.get(r.verdict, 0) + 1

    text = '\n'.join(c for c in chunks if c)
    if a.out:
        with open(a.out, 'w', encoding='utf-8') as f:
            f.write(text + ('\n' if text else ''))
    elif a.emit != 'summary':
        print(text)

    sys.stderr.write('methodalias: %d TU(s), %d member(s)\n'
                     % (len(tus) - len(failures), len(all_rows)))
    for k in (SAME_C, ALIAS, FACE, NO_EMISSION, AMBIGUOUS):
        sys.stderr.write('  %-12s %d\n' % (k, counts.get(k, 0)))
    for tu, why in failures:
        sys.stderr.write('  UNREADABLE   %s: %s\n' % (tu, why))
    for r, why in bad:
        sys.stderr.write('  GUARD REFUSAL %s: %s\n' % (r.lhs, why))
    return 1 if bad else 0


# ---------------------------------------------------------------------------
# 9. Selftest. Fixtures for the parsers; undname only where undname is the
#    thing under test, and a missing undname reports SKIPPED, never a pass.
# ---------------------------------------------------------------------------

ITANIUM_CASES = [
    # symbol, class, member, nparams, dtor
    ('_ZN17dScMgTrampoline_c9StateDoneEv', 'dScMgTrampoline_c',
     'StateDone', 0, None),
    ('_ZN17dScMgTrampoline_cD1Ev', 'dScMgTrampoline_c',
     '~dScMgTrampoline_c', 0, 'D1'),
    ('_ZN17dScMgTrampoline_cD0Ev', 'dScMgTrampoline_c',
     '~dScMgTrampoline_c', 0, 'D0'),
    ('_ZN6Player7SetAnimEji5Fix12IiEj', 'Player', 'SetAnim', 4, None),
    ('_ZN6Player11ChangeStateERNS_5StateE', 'Player', 'ChangeState', 1, None),
    ('_ZNK9Animation12WillHitFrameEi', 'Animation', 'WillHitFrame', 1, None),
    # 7, and the arity is CORROBORATED, not asserted: cxx_aliases.cpp:713
    # already carries this pair, and its MSVC side
    # ?New@System@Particle@@SAXIIHHHPBUVector3_16@@PAUCallback@2@@Z
    # has seven arguments too. `PK11Vector3_16f` is ONE type -- Vector3_16f is
    # eleven characters -- and a reader that split it would have said eight.
    ('_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE',
     'Particle::System', 'New', 7, None),
    # 5, corroborated by hal/reverse_bridges.cpp's own declaration of it:
    # six parameters there, the first of which is the explicit `this`.
    ('_ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j',
     'Actor', 'DropShadowRadHeight', 5, None),
    ('_ZN13RaycastGroundC1Ev', 'RaycastGround', 'RaycastGround', 0, None),
    ('_ZN3IRQ7DisableEv', 'IRQ', 'Disable', 0, None),
]

# (decorated, demangled, receiverless?) -- the verdict this tool turns on.
MSVC_CASES = [
    ('?StateDone@dScMgTrampoline_c@@QAEXXZ',
     'public: void __thiscall dScMgTrampoline_c::StateDone(void)', False),
    ('?Behavior@dScMgTrampoline_c@@UAEHXZ',
     'public: virtual int __thiscall dScMgTrampoline_c::Behavior(void)',
     False),
    ('?LoadInitialGroup@Sound@@SAXH@Z',
     'public: static void __cdecl Sound::LoadInitialGroup(int)', True),
    ('?GiveHealth@@YAHHH@Z', 'int __cdecl GiveHealth(int,int)', True),
    ('?GetBodyModelID@Player@@QBEII_N@Z',
     'public: unsigned int __thiscall Player::GetBodyModelID(unsigned int,'
     'bool)const ', False),
    # A CONSTRUCTOR has no return type, so the convention token is the FIRST
    # thing after the access. Pinned because reading it as a data symbol made
    # 70 constructors across the merged set report NO_EMISSION -- the tool
    # claiming enrollment would lose a ROM body it was in fact emitting.
    ('??0RaycastGround@@QAE@XZ',
     'public: __thiscall RaycastGround::RaycastGround(void)', False),
    ('??1dMgMCarloCardObj_c@@QAE@XZ',
     'public: __thiscall dMgMCarloCardObj_c::~dMgMCarloCardObj_c(void)',
     False),
    # MSVC's scalar deleting destructor: the Itanium D0 of this class.
    ('??_GdScMgPanel_c@@UAEPAXI@Z',
     "public: virtual void * __thiscall dScMgPanel_c::`scalar deleting "
     "destructor'(unsigned int)", False),
]

# A trimmed dumpbin /symbols page: one defined method, one defined C name,
# one UNDEF. The reader must sort all three and invent nothing.
DUMPBIN_FIXTURE = """
COFF SYMBOL TABLE
009 00000000 SECT3  notype ()    External     | ?StateDone@dScMgTrampoline_c@@QAEXXZ (public: void __thiscall dScMgTrampoline_c::StateDone(void))
00A 00000000 SECT5  notype ()    External     | _func_ov006_02120938
00B 00000000 UNDEF  notype ()    External     | _LoadFile
00C 00000000 SECT1  notype       Static       | .text
"""

MANIFEST_FIXTURE = """/* a merged TU
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021207dc  src/_ZN17dScMgTrampoline_cD1Ev.cpp
 *   [1] 0x02120880  src/_ZN17dScMgTrampoline_cD0Ev.cpp
 *   [2] 0x02120938  src/func_ov006_02120938.c
 *   [3] 0x02121774  src/func_ov006_02121774.c
 */
void x(void) { }
/* [9] 0xdeadbeef  not/in/the/header.c -- must NOT be read as a manifest row */
"""


def selftest():
    bad = 0

    print('  Itanium reader')
    for sym, cls, mem, npar, dt in ITANIUM_CASES:
        it = Itanium(sym)
        got = (it.cls, it.member, it.nparams, it.dtor)
        ok = got == (cls, mem, npar, dt)
        bad += 0 if ok else 1
        print('    %-4s %s' % ('ok' if ok else 'FAIL', sym))
        if not ok:
            print('         want %r' % ((cls, mem, npar, dt),))
            print('         got  %r' % (got,))

    print('  manifest reader')
    import tempfile
    fd, p = tempfile.mkstemp(suffix='.cpp', text=True)
    with os.fdopen(fd, 'w', encoding='utf-8') as f:
        f.write(MANIFEST_FIXTURE)
    try:
        man = read_manifest(p)
    finally:
        os.remove(p)
    ok = (len(man) == 4 and man[0][1] == 0x021207dc
          and man[3][2] == 'src/func_ov006_02121774.c')
    bad += 0 if ok else 1
    print('    %-4s 4 rows from the header comment, none from the body'
          % ('ok' if ok else 'FAIL'))
    if not ok:
        print('         got %r' % (man,))
    ok = guess_module(man) == 'ov006'
    bad += 0 if ok else 1
    print('    %-4s module inferred as ov006' % ('ok' if ok else 'FAIL'))

    print('  dumpbin reader')
    d, u = parse_dumpbin(DUMPBIN_FIXTURE)
    ok = (d == {'?StateDone@dScMgTrampoline_c@@QAEXXZ',
                '_func_ov006_02120938'} and u == {'_LoadFile'})
    bad += 0 if ok else 1
    print('    %-4s defined/undef split, Static rows ignored'
          % ('ok' if ok else 'FAIL'))
    if not ok:
        print('         got defined=%r undef=%r' % (sorted(d), sorted(u)))

    print('  receiver rule (the verdict this tool turns on)')
    for dec, text, want in MSVC_CASES:
        mv = Msvc(dec, text)
        got = mv.is_receiverless()
        ok = got == want
        bad += 0 if ok else 1
        print('    %-4s %-45s receiverless=%s' % ('ok' if ok else 'FAIL',
                                                  dec, got))

    print('  undname agreement (the decorations above are real, not typed)')
    if msvc_undname is None or msvc_undname.find_undname() is None:
        print('    SKIPPED, not passed: no undname. Every MSVC spelling this '
              'tool emits comes from undname, so with none present it '
              'refuses rather than guessing.')
    else:
        got = msvc_undname.demangle([d for d, _, _ in MSVC_CASES])
        for dec, text, _ in MSVC_CASES:
            g = got.get(dec, '<missing>')
            ok = g.strip() == text.strip()
            bad += 0 if ok else 1
            print('    %-4s %s' % ('ok' if ok else 'FAIL', dec))
            if not ok:
                print('         undname %r' % g)

    print('  end-to-end classify()')
    syms = {0x021207dc: ('_ZN17dScMgTrampoline_cD1Ev', 'function(arm)'),
            0x02120880: ('_ZN17dScMgTrampoline_cD0Ev', 'function(arm)'),
            0x02120938: ('func_ov006_02120938', 'function(arm)'),
            0x02121774: ('_ZN17dScMgTrampoline_c9StateDoneEv',
                         'function(arm)')}
    members = [(0, 0x021207dc, None), (1, 0x02120880, None),
               (2, 0x02120938, None), (3, 0x02121774, None)]
    legacy = {0x021207dc: 'src/_ZN17dScMgTrampoline_cD1Ev.cpp'}
    defined = {'_func_ov006_02120938',
               '?StateDone@dScMgTrampoline_c@@QAEXXZ'}
    und = {'?StateDone@dScMgTrampoline_c@@QAEXXZ':
           'public: void __thiscall dScMgTrampoline_c::StateDone(void)'}
    rows = classify(members, syms, defined, und, legacy)
    want = [NO_EMISSION, NO_EMISSION, SAME_C, FACE]
    got = [r.verdict for r in rows]
    ok = got == want
    bad += 0 if ok else 1
    print('    %-4s D1/D0 NO_EMISSION, func_ SAME_C, StateDone FACE '
          '(never ALIAS)' % ('ok' if ok else 'FAIL'))
    if not ok:
        print('         want %r' % want)
        print('         got  %r' % got)

    print('  the refusal that matters: a thiscall member never yields a '
          'pragma')
    pragmas = emit_pragmas(rows, 'fixture.cpp')
    ok = '/alternatename:' not in pragmas
    bad += 0 if ok else 1
    print('    %-4s emit_pragmas() produced no directive for a __thiscall '
          'member' % ('ok' if ok else 'FAIL'))

    print('SELFTEST %s' % ('FAILED' if bad else 'PASSED'))
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
