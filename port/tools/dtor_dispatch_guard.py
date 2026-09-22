"""dtor_dispatch_guard -- refuse a build in which the actor teardown has stopped
dispatching through ROM vtable slot 16.

WHY THIS EXISTS. Commit 9cbd99048 brought the two-arm destructor spelling onto the
port lineage. That change is right and closed forty measured wrong-slot dispatch
sites. It also, as a side effect nobody booked, made `this->~fBase_c()` in
src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp bind to a NON-VIRTUAL declaration, so
MSVC emitted a direct call to fBase_c's own destructor body and every actor's own
cleanup was skipped from then on.

NOTHING IN THE TREE REPORTED IT. The byte gate compares the ARM object, where the
spelling is right. The linker is happy: a direct call resolves. linkage.py counts
the TU either way. No guard looked at the call. And the boot got FURTHER, because
the call that used to fault was no longer being made, so the symptom read as a fix.
A silent skip that looks like an improvement is the worst shape a regression has,
and this file is the answer to it.

WHAT THE CARTRIDGE DOES, which is what is asserted (arm9 0x02043b2c, read from
extracted/arm9_dec.bin; the arm9 main module loads at 0x02004000, see
extracted/dsd/arm9/arm9.yaml base_address, NOT at 0x02000000):

    02043b78  mov r0, r4           ; this
    02043b7c  ldr r1, [r0]         ; the object's vtable
    02043b80  ldr r1, [r1, #0x40]  ; SLOT 16, the complete-object destructor
    02043b84  blx r1               ; dispatch
    02043b94  bl  #0x203c1e8       ; then Memory::Deallocate

    02043b40  ldr r0, [pc, ...] / add r1, r4, #0x14   ; manager.sceneNode
    02043b50  add r1, r4, #0x28                       ; manager.behaviorNode
    02043b58  ldr r0, [r4, #0x4c]                     ; heap
    02043b68  ldr r0, [r4, #0x48]                     ; lifecycleState

So the guard asserts two things about the built
?AfterCleanupResources@fBase_c@@UAEXI@Z:

  1. it dispatches vtable slot 16 exactly once, as `call dword ptr [reg + 0x40]`;
  2. it reads the four members at the cartridge's own offsets, which is what keeps
     port/unmatched/FBase_DtorDispatch_HostAbi.h's shadow honest -- a shadow whose
     layout drifted from include/fBase_c.h would still compile and still dispatch,
     and would pass check 1 alone.

Usage:
    python port/tools/dtor_dispatch_guard.py <root>
    python port/tools/dtor_dispatch_guard.py --selftest

Exit 0 = OK, 1 = REFUSED, 2 = could not read the build.
"""
import os
import re
import struct
import sys

import capstone
from capstone import x86

SYM = '?AfterCleanupResources@fBase_c@@UAEXI@Z'
SLOT16_DISP = 0x40
WANT_MEMBERS = {0x14: 'manager.sceneNode',
                0x28: 'manager.behaviorNode',
                0x48: 'lifecycleState',
                0x4c: 'heap'}

LINE = re.compile(r'^\s+([0-9a-f]{4}):([0-9a-f]{8})\s+(.+?)\s+([0-9a-f]{8})\s+(.*?)\s*$')

MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
MD.detail = True
MD.skipdata = True


class PE(object):
    def __init__(self, path):
        self.d = open(path, 'rb').read()
        e = struct.unpack_from('<I', self.d, 0x3c)[0]
        nsec = struct.unpack_from('<H', self.d, e + 6)[0]
        optsz = struct.unpack_from('<H', self.d, e + 20)[0]
        self.base = struct.unpack_from('<I', self.d, e + 24 + 28)[0]
        so = e + 24 + optsz
        self.secs = []
        for i in range(nsec):
            o = so + i * 40
            vsz, va, rsz, ptr = struct.unpack_from('<IIII', self.d, o + 8)
            self.secs.append((va, vsz, rsz, ptr))

    def read(self, rva, n):
        for va, vsz, rsz, ptr in self.secs:
            if va <= rva < va + max(vsz, rsz):
                d = rva - va
                if d >= rsz:
                    return b''
                return self.d[ptr + d:ptr + min(d + n, rsz)]
        return b''


def load_map(path, base):
    rows = []
    for ln in open(path, encoding='utf-8', errors='ignore'):
        m = LINE.match(ln.rstrip('\n'))
        if m:
            rows.append((int(m.group(4), 16) - base, m.group(3)))
    rows.sort()
    return rows


def analyse(code, rva):
    """Return (slot16 call count, set of member displacements read off the this
    register, the disassembly text)."""
    calls = 0
    members = set()
    text = []
    thisreg = None
    for ins in MD.disasm(code, rva):
        text.append('  %08x  %-10s %s' % (ins.address, ins.mnemonic, ins.op_str))
        if ins.mnemonic == 'mov' and len(ins.operands) == 2:
            dst, src = ins.operands
            if (dst.type == x86.X86_OP_REG and src.type == x86.X86_OP_REG
                    and MD.reg_name(src.reg) == 'ecx' and thisreg is None):
                thisreg = MD.reg_name(dst.reg)
        for op in ins.operands:
            if op.type != x86.X86_OP_MEM:
                continue
            m = op.mem
            if m.base == 0 or m.index != 0 or m.disp <= 0:
                continue
            reg = MD.reg_name(m.base)
            if ins.mnemonic == 'call':
                if m.disp == SLOT16_DISP:
                    calls += 1
                continue
            if reg in (thisreg, 'ecx'):
                members.add(m.disp)
    return calls, members, text


def check(exe, mappath, verbose=True):
    pe = PE(exe)
    rows = load_map(mappath, pe.base)
    hit = [i for i, (a, n) in enumerate(rows) if n == SYM]
    if not hit:
        print('dtor_dispatch_guard: CANNOT READ -- %s is not in %s' % (SYM, mappath))
        return 2
    i = hit[0]
    rva = rows[i][0]
    end = rows[i + 1][0] if i + 1 < len(rows) else rva + 0x200
    code = pe.read(rva, end - rva)
    if not code:
        print('dtor_dispatch_guard: CANNOT READ -- no bytes at rva 0x%x' % rva)
        return 2
    calls, members, text = analyse(code, rva)
    bad = []
    if calls != 1:
        bad.append('the teardown dispatches vtable slot 16 %d time(s), not once. The '
                   'cartridge does it exactly once at arm9 0x02043b84. Zero means every '
                   'actor\'s own cleanup is being SKIPPED and nothing else will say so.'
                   % calls)
    missing = sorted(set(WANT_MEMBERS) - members)
    if missing:
        bad.append('the teardown does not read %s at the cartridge\'s own offset(s): %s'
                   % (', '.join(WANT_MEMBERS[m] for m in missing),
                      ', '.join('+0x%02x' % m for m in missing)))
    if verbose:
        print('dtor_dispatch_guard: %s at rva 0x%x, %d byte(s)' % (SYM, rva, len(code)))
        print('  slot-16 dispatches        : %d (want 1)' % calls)
        print('  member offsets read       : %s'
              % ', '.join('+0x%02x' % m for m in sorted(members)))
        print('  cartridge offsets wanted  : %s'
              % ', '.join('+0x%02x %s' % (m, WANT_MEMBERS[m]) for m in sorted(WANT_MEMBERS)))
    if bad:
        for b in bad:
            print('dtor_dispatch_guard: REFUSED -- %s' % b)
        if verbose:
            print('\n'.join(text))
        return 1
    print('dtor_dispatch_guard: OK -- the actor teardown dispatches vtable slot 16 once '
          'and reads its members at the cartridge\'s offsets.')
    return 0


def selftest():
    """Prove the guard can say no: run its analysis over hand-assembled bytes for
    the broken shape (a direct call where the dispatch should be) and the correct
    one."""
    #   mov esi, ecx / mov eax,[esi] / mov ecx,esi / push 0 / call [eax+0x40]
    good = bytes.fromhex('8bf1') + bytes.fromhex('8b06') + bytes.fromhex('8bce') \
        + bytes.fromhex('6a00') + bytes.fromhex('ff5040') \
        + bytes.fromhex('8b4614') + bytes.fromhex('8b4628') \
        + bytes.fromhex('8b4648') + bytes.fromhex('8b464c') + b'\xc3'
    #   the same, but the dispatch replaced by a direct rel32 call
    bad = bytes.fromhex('8bf1') + bytes.fromhex('8bce') + bytes.fromhex('e800000000') \
        + bytes.fromhex('8b4614') + bytes.fromhex('8b4628') \
        + bytes.fromhex('8b4648') + bytes.fromhex('8b464c') + b'\xc3'
    gc, gm, _ = analyse(good, 0x1000)
    bc, bm, _ = analyse(bad, 0x1000)
    ok = (gc == 1 and set(WANT_MEMBERS) <= gm
          and bc == 0 and set(WANT_MEMBERS) <= bm)
    print('dtor_dispatch_guard: SELFTEST %s -- correct shape reads %d dispatch(es) and '
          '%d member(s); the devirtualised shape reads %d dispatch(es) and %d member(s), '
          'so the guard refuses the real defect and not merely a missing function.'
          % ('OK' if ok else 'FAIL', gc, len(gm & set(WANT_MEMBERS)),
             bc, len(bm & set(WANT_MEMBERS))))
    return 0 if ok else 1


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    rc = 0
    if '--selftest' in sys.argv or not args:
        rc = selftest()
        if not args:
            return rc
    root = args[0]
    exe = os.path.join(root, 'build', 'port', 'walk_window.exe')
    mp = os.path.join(root, 'build', 'port', 'walk_window.map')
    if not os.path.exists(exe) or not os.path.exists(mp):
        print('dtor_dispatch_guard: CANNOT READ -- no build at %s' % os.path.dirname(exe))
        return 2
    return check(exe, mp) or rc


sys.exit(main())
