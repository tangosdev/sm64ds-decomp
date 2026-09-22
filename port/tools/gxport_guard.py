#!/usr/bin/env python
"""Refuse the build when a ROM translation unit reaches a DS GEOMETRY COMMAND
PORT with a PLAIN store while being compiled plain.

THE FAULT THIS EXISTS FOR. ntr maps real memory across 0x04000000, so a store
to a geometry command register from a translation unit the port builds plain
LANDS -- the word is written, nothing faults, no call is skipped, the TU stays
byte-matched, the linker is happy and linkage.py counts the row. The geometry
engine is simply never told. Run mg15 lane TITLETEX found twelve of these in
ov007, run link100 lane MODESEL found a thirteenth, lane SAMECLASS eleven more,
and every one of them was found by a person reading a literal pool. The check
costs about a second and needs no build products beyond build.ninja.

WHAT COUNTS AS A COMMAND PORT, and why the window is this narrow. The 2D
registers (DISPCNT, the BGxCNT block, the window and blend registers, VRAMCNT)
are LATCHES: the port's software PPU reads them back out of the mapped window
every frame, so a plain store to one of those is correct and this guard must
not complain about it. 0x04000400..0x0400045f is the command FIFO and the
packed command ports, 0x04000440..0x040005ff the unpacked ones, and
0x04000600/0x04000604 GXSTAT and GXFIFO's status: writing any of those is an
INSTRUCTION to the geometry engine, and an instruction that never arrives is
thrown away. Those are the addresses here and nothing else.

    0x04000400 .. 0x0400064f

WHAT COUNTS AS A STORE. The detection is tools/hostgen.py's own MMIO_DEREF and
MMIO_BIND, deliberately, so "this file reaches a register" means exactly what
the fixing tool means by it -- a guard that disagreed with the tool that fixes
the fault would refuse rows hostgen cannot repair and pass rows it can. A file
is clean when the configured build compiles the GENERATED copy of it out of
build/<dir>/host-src rather than the raw one.

WHAT IT READS. The CONFIGURED build.ninja, not a source list and not a glob. A
translation unit that is not on a compile line cannot ship the fault and is not
reported; one that is on a compile line cannot be missed because a slice list
was read wrong. src/Initialise3dGraphics.cpp is on three slice lists, which is
exactly the shape a source-list walker gets wrong.

READ-ONLY TUs ARE NOT THE FAULT. src/func_02055464.c and src/func_02055490.c
read GXSTAT's stack-level field and write nothing. ntr::gxstat_publish keeps
the mapped window live for precisely those two readers (see the note above
ntr/gx.cpp's gxstat_publish), so a plain READ is served correctly. This guard
counts STORES, so those two pass without being named in a baseline -- there is
no list here to go stale.

A DELIBERATE PRE-FIX ARM IS NOT A FAILURE. The A/B switches that build these
TUs plain on purpose (SAMECLASS_GX_PLAIN, SAMECLASS_BOOT_PLAIN,
TITLEQUAD_GX_PLAIN and any other *_PLAIN option) are read out of CMakeCache.txt
and, when one of them is ON, the guard says so and stands down rather than
refusing the arm a lane configured on purpose.

    python port/tools/gxport_guard.py [--build-dir DIR] [--selftest]
"""
import argparse
import os
import re
import sys

BS = chr(92)
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import hostgen

LO, HI = 0x04000400, 0x0400064F

CMT = re.compile(r'/\*.*?\*/|//[^\n]*', re.S)
STR = re.compile(r'"(?:[^"\\\n]|\\.)*"')

# A file under one of these is the host's own I/O layer or a generated copy,
# and reaches the engine through NTR_MMIO by construction.
HOST_DIRS = ('/host-src/', '/port/hal/', '/port/ntr/', '/port/tests/',
             '/port/unmatched/', '/port/res/', '/port/tools/')


def strip(text):
    text = CMT.sub(lambda m: '\n' * m.group(0).count('\n'), text)
    return STR.sub('""', text)


def in_window(addr_text):
    return LO <= int(addr_text, 16) <= HI


# An object-like macro whose body is a register deref. src/func_ov075_0211a948.c
# and the two ov080 material setters spell every one of their registers this
# way, so a scanner that does not expand these sees the deref on the #define
# line -- where it is followed by a newline and looks like a read -- and never
# sees the store at all. Expanded first, both halves read correctly.
DEFINE_REG = re.compile(
    r'^[ \t]*#[ \t]*define[ \t]+(\w+)[ \t]+(\([ \t]*\*.*\))[ \t]*$', re.M)

# What has to follow a deref for it to be a STORE. `=` not part of `==`,
# `<=`, `>=`, `!=`, or any compound assignment.
STORE_AFTER = re.compile(r'\s*(?:\|=|&=|\^=|\+=|-=|\*=|/=|%=|<<=|>>=|=(?!=))')
INCDEC_AFTER = re.compile(r'\s*(?:\+\+|--)')


def expand_reg_macros(text):
    """Inline object-like macros whose body is a register deref, and drop the
    #define lines themselves so the body is counted where it is USED."""
    bodies = {}
    for m in DEFINE_REG.finditer(text):
        bodies[m.group(1)] = m.group(2)
    if not bodies:
        return text
    text = DEFINE_REG.sub(lambda m: '', text)
    for name, body in bodies.items():
        # The bodies are parenthesised so the macro is safe to use inside an
        # expression. That outer pair has to come off, or the deref the store
        # detector looks at is followed by a `)` where the `=` should be.
        if body.startswith('(') and body.endswith(')'):
            inner = body[1:-1]
            if inner.count('(') == inner.count(')'):
                body = inner
        text = re.sub(r'\b' + re.escape(name) + r'\b', body, text)
    return text


def command_ports(text):
    """The command-port addresses this text STORES to, as {addr: how}.

    A deref is `*(T *)0x40004xx =`; a bind is a pointer local bound once to
    such a literal and then written through. Both latch and neither arrives.
    A READ of a command port is not this fault and is not reported:
    ntr::gxstat_publish keeps the mapped window live for the game's two
    GXSTAT stack-level readers, which is why those two need no baseline here.
    """
    text = expand_reg_macros(text)
    hits = {}
    for m in hostgen.MMIO_BIND.finditer(text):
        if not in_window(m.group(3)):
            continue
        name = m.group(1)
        # The binding statement itself reads `*name = (T *)0xADDR;` once the
        # declarator is included, so it has to come out before the search or
        # every bind convicts itself.
        rest = text[:m.start()] + text[m.end():]
        rest = re.sub(r'(?:(?:volatile|const)\s+)*(?:' + hostgen.MMIO_TYPE
                      + r')\s*\*\s*' + re.escape(name) + r'\b', '', rest)
        store = re.search(r'(?:\*\s*' + re.escape(name) + r'\b|\b'
                          + re.escape(name) + r'\s*\[[^\]]*\])'
                          r'\s*(?:\|=|&=|\^=|\+=|-=|\*=|/=|%=|<<=|>>=|=(?!=))',
                          rest)
        if store:
            hits[int(m.group(3), 16)] = 'bind'
    for m in hostgen.MMIO_DEREF.finditer(text):
        if not in_window(m.group(3)):
            continue
        tail = text[m.end():]
        if STORE_AFTER.match(tail) or INCDEC_AFTER.match(tail):
            hits[int(m.group(3), 16)] = 'deref'
    return hits


def ninja_sources(ninja_path):
    """Every source on a compile edge of the configured build, once each."""
    seen, order = set(), []
    with open(ninja_path, encoding='utf-8', errors='replace') as f:
        for line in f:
            if not line.startswith('build ') or '.obj:' not in line:
                continue
            parts = line.split('.obj:', 1)[1].split()
            if len(parts) < 2:
                continue
            src = parts[1].replace('$:', ':').replace('$ ', ' ')
            src = src.replace(BS, '/')
            if src not in seen:
                seen.add(src)
                order.append(src)
    return order


def plain_arm_options(build_dir):
    """The *_PLAIN A/B switches this configure has turned ON."""
    cache = os.path.join(build_dir, 'CMakeCache.txt')
    on = []
    if not os.path.isfile(cache):
        return on
    for line in open(cache, encoding='utf-8', errors='replace'):
        m = re.match(r'([A-Z0-9_]*_PLAIN):BOOL=(\w+)', line.strip())
        if m and m.group(2).upper() in ('ON', '1', 'TRUE', 'YES'):
            on.append(m.group(1))
    return on


def scan(build_dir):
    """(offenders, checked) for a configured build directory.

    An offender is a source the build compiles RAW that stores to a command
    port. A source whose generated copy is on a compile line instead is clean,
    which is what routing through hostgen looks like from here.
    """
    ninja = os.path.join(build_dir, 'build.ninja')
    srcs = ninja_sources(ninja)
    lowered = set(s.lower() for s in srcs)

    offenders, checked = [], 0
    for src in srcs:
        low = src.lower()
        if any(d in low for d in HOST_DIRS):
            continue
        real = src if os.path.isfile(src) else os.path.join(build_dir, src)
        if not os.path.isfile(real):
            continue
        checked += 1
        hits = command_ports(strip(
            open(real, encoding='utf-8', errors='replace').read()))
        if not hits:
            continue
        # routed? the generated copy is what the build compiles instead
        stem = low.rsplit('.', 1)[0]
        tail = '/host-src/' + stem.split('/src/', 1)[-1] + '.cpp' \
            if '/src/' in stem else None
        if tail and any(s.endswith(tail) for s in lowered):
            continue
        offenders.append((real, hits))
    return offenders, checked


# ---- the fixtures ----------------------------------------------------------
#
# Every arm here has a break only that arm catches, and each is written as the
# tree has actually seen it. A guard whose fixtures only ever pass is a guard
# nobody has tested.
FIXTURES = [
    # (name, source text, expected command-port addresses found)
    ('plain deref, the TITLETEX shape',
     'void f(void){ *(volatile unsigned int *)0x4000444 = 0; }',
     {0x4000444}),
    ('vu32 typedef with a u suffix, the shape that missed the old pattern '
     'twice',
     'void f(void){ *(vu32 *)0x40004ccu = 0x7fff; }',
     {0x40004CC}),
    ('a macro-defined register, the ov075/ov080 material shape',
     '#define R (*(volatile u32 *)0x040004a4)\nvoid f(void){ R = 0x11f0088; }',
     {0x40004A4}),
    ('a pointer bound once and written through, the func_0204488c shape',
     'void f(int c){ volatile int *m = (volatile int *)0x400046c; *m = c; }',
     {0x400046C}),
    ('GXSTAT, which is a command port and not a latch',
     'void f(void){ *(volatile unsigned int *)0x4000600 = 0; }',
     {0x4000600}),
    ('a compound assignment, the func_020556d0 shape',
     'void f(void){ *(volatile u32 *)0x4000600 |= 0x8000; }',
     {0x4000600}),
    # the other direction: things that MUST NOT be reported
    ('src/func_02055464.c, which READS GXSTAT and writes nothing '
     '(ntr::gxstat_publish serves it)',
     'int f(int *out){ if (*(volatile unsigned int *)0x4000600 & 0x4000) '
     'return -1; *out = (*(volatile unsigned int *)0x4000600 & 0x2000) >> 13; '
     'return 0; }',
     set()),
    ('a busy-wait on GXSTAT, which is a read',
     'void f(void){ while (*(volatile unsigned int *)0x4000600 & 0x8000000) ; }',
     set()),
    ('a comparison, which is a read and not a store',
     'int f(void){ return *(volatile u32 *)0x4000600 == 0; }',
     set()),
    ('a pointer bound to a command port and only READ through',
     'int f(void){ volatile int *m = (volatile int *)0x400046c; return *m; }',
     set()),
    ('DISPCNT, a 2D latch the software PPU reads back',
     'void f(void){ *(volatile unsigned int *)0x4000000 = 0x11b08; }',
     set()),
    ('VRAMCNT, a 2D latch',
     'void f(void){ *(volatile unsigned char *)0x4000240 = 0x83; }',
     set()),
    ('POWCNT, a 2D latch',
     'void f(void){ *(volatile unsigned short *)0x4000304 = 0x820f; }',
     set()),
    ('an already-routed store',
     '#include "ntr/mmio.h"\nvoid f(void){ NTR_MMIO(u32, 0x4000444) = 0; }',
     set()),
    ('a command port named only in a comment',
     '/* 0x4000444 is MTX_PUSH */\nvoid f(void){ }',
     set()),
    ('a command port named only in a string',
     'void f(void){ log("0x4000444"); }',
     set()),
    ('a computed address, which resolves through the mapped latch window',
     'void f(int i){ *(volatile unsigned int *)(0x4000440 + i) = 0; }',
     set()),
]


def selftest():
    bad = 0
    for name, text, want in FIXTURES:
        got = set(command_ports(strip(text)))
        if got != want:
            bad += 1
            print('  FIXTURE FAILED: %s' % name)
            print('    wanted %s' % sorted('%08x' % a for a in want))
            print('    got    %s' % sorted('%08x' % a for a in got))
    if bad:
        print('gxport_guard: %d of %d fixtures failed' % (bad, len(FIXTURES)))
        return 1
    print('gxport_guard: selftest OK, %d fixtures' % len(FIXTURES))
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build-dir',
                    default=os.path.join(os.path.dirname(HERE), '..',
                                         'build', 'port'))
    ap.add_argument('--selftest', action='store_true')
    ap.add_argument('--no-standdown', action='store_true',
                    help='scan even a configure that has a *_PLAIN A/B switch '
                         'on. This is how the tree scan is tested against a '
                         'tree that is known to be dirty: point it at a '
                         'deliberately plain build and it must name every TU '
                         'that switch un-routed.')
    args = ap.parse_args()

    if args.selftest:
        return selftest()

    # The fixtures run before the tree does, every time, so weakening a rule
    # breaks the build instead of quietly disarming the check.
    if selftest():
        return 1

    build_dir = os.path.abspath(args.build_dir)
    ninja = os.path.join(build_dir, 'build.ninja')
    if not os.path.isfile(ninja):
        print('gxport_guard: no build.ninja in %s, nothing configured to '
              'check' % build_dir)
        return 0

    arms = plain_arm_options(build_dir)
    if arms and not args.no_standdown:
        print('gxport_guard: standing down, this configure has a deliberate '
              'pre-fix arm on: %s' % ', '.join(arms))
        return 0

    offenders, checked = scan(build_dir)
    if not offenders:
        print('  OK: none of the %d ROM sources this build compiles reaches a '
              'geometry command port with a plain store.' % checked)
        return 0

    print('gxport_guard: REFUSED. These translation units are compiled PLAIN '
          'and store to a DS geometry command port, so the word latches in '
          'mapped memory and the geometry engine is never told:')
    for real, hits in offenders:
        print('  %s' % real)
        print('      %s' % ' '.join('%08x(%s)' % (a, hits[a])
                                    for a in sorted(hits)))
    print('The fix is to add the source to PORT_HOSTGEN_TU in '
          'port/CMakeLists.txt, which routes its stores through NTR_MMIO '
          'without editing src/ and without costing the byte match.')
    return 1


if __name__ == '__main__':
    sys.exit(main())
