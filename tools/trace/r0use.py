"""r0use: does a function read r0 on entry, or overwrite it first?

  python tools/trace/r0use.py arm9 0x020092c4 [ovNNN 0x... ...]

A `this` sweep reads r0 at a function's entry and calls the function a method
of the class whose vptr r0 holds. That is evidence only when the function reads
r0: func_020092c4 starts `add r0, sp, #8`, so whatever r0 held on entry is the
caller's leftover, and its class says nothing about the function.

check(code, addr, thumb) walks the instructions from the entry along every path
(both sides of a conditional branch, into an in-function `b`), and reports the
first instruction that reads r0 before any unconditional write to it:

  base         r0 is the base of a load or store:  ldr r1, [r0, #8]
  read         r0 is any other source operand:      mov r4, r0 / cmp r0, #0 / push {r0}
  call         a bl/blx (or a tail `b` out of the function) with r0 untouched: r0 is
               passed on as the callee's first argument
  overwritten  every path writes r0 before reading it (r0 is not a parameter here)
  unused       no path reads or writes r0 before it returns

r0_used is True for base, read and call. A conditional write (moveq r0, #1)
leaves the other path's r0 live, so the walk goes on. The bytes come from the
extracted module images (symtab.Table.code), so the check is static: it needs
no emulator.
"""
import sys

try:
    from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM, CS_MODE_THUMB, CS_GRP_CALL, CS_GRP_BRANCH_RELATIVE
    from capstone import arm as csarm
except ImportError:        # pragma: no cover - capstone is in the tool requirements
    Cs = None

USED = ('base', 'read', 'call')
_MAX_STEPS = 400


def _dis(thumb):
    md = Cs(CS_ARCH_ARM, CS_MODE_THUMB if thumb else CS_MODE_ARM)
    md.detail = True
    return md


def _decode(md, code, addr, start):
    off = start - addr
    if off < 0 or off >= len(code):
        return None
    for insn in md.disasm(code[off:off + 4], start, 1):
        return insn
    return None


def _r0(insn):
    """(reads r0 as base, reads r0 otherwise, writes r0)."""
    R0 = csarm.ARM_REG_R0
    base = other = write = False
    for op in insn.operands:
        if op.type == csarm.ARM_OP_MEM:
            if op.mem.base == R0:
                base = True
            if op.mem.index == R0:
                other = True
    try:
        rd, wr = insn.regs_access()
    except Exception:          # pragma: no cover - older capstone
        rd, wr = insn.regs_read, insn.regs_write
    if R0 in rd and not base:
        other = True
    if R0 in wr:
        write = True
    m = insn.mnemonic
    # capstone lists a pop/ldm register list as written and push/stm as read; both are right
    if m.startswith(('push', 'stm')) and any(op.type == csarm.ARM_OP_REG and op.reg == R0 for op in insn.operands):
        other = True
    return base, other, write


def _cond(insn):
    return insn.cc not in (csarm.ARM_CC_AL, csarm.ARM_CC_INVALID)


def _branch_target(insn):
    for op in insn.operands:
        if op.type == csarm.ARM_OP_IMM:
            return op.imm
    return None


def _returns(insn):
    m, s = insn.mnemonic, insn.op_str
    if m.startswith('bx') and 'lr' in s:
        return True
    if (m.startswith('pop') or m.startswith('ldm')) and 'pc' in s:
        return True
    if m.startswith('mov') and s.startswith('pc,'):
        return True
    return False


def check(code, addr, thumb=False, size=None):
    """{'r0_used': bool, 'r0_first_use': 'kind@+off insn'} for the function whose
    bytes `code` start at `addr`. size bounds in-function branches (default len(code))."""
    if Cs is None:
        return {'r0_used': None, 'r0_first_use': 'no capstone'}
    if not code:
        return {'r0_used': None, 'r0_first_use': 'no bytes'}
    size = size or len(code)
    md = _dis(thumb)
    end = addr + size
    found = []            # (order, kind, off, text) of the first r0 read on each path
    wrote = []            # (order, off, text) of the first unconditional r0 write on a path
    seen = set()
    stack = [addr]
    order = 0
    steps = 0
    while stack and steps < _MAX_STEPS:
        pc = stack.pop()
        while steps < _MAX_STEPS:
            if pc in seen or not (addr <= pc < end):
                break
            seen.add(pc)
            steps += 1
            insn = _decode(md, code, addr, pc)
            if insn is None:
                break
            order += 1
            text = '%s %s' % (insn.mnemonic, insn.op_str)
            off = pc - addr
            is_call = insn.group(CS_GRP_CALL)              # bl, blx (any condition)
            base, other, write = _r0(insn)
            if base or other:
                found.append((order, 'base' if base else 'read', off, text))
                break
            if is_call:
                found.append((order, 'call', off, text))
                break
            if write and not _cond(insn):
                wrote.append((order, off, text))
                break
            if _returns(insn):
                if not _cond(insn):
                    break
                pc += insn.size
                continue
            if insn.group(CS_GRP_BRANCH_RELATIVE):
                tgt = _branch_target(insn)
                if tgt is None:
                    break
                if not (addr <= tgt < end):
                    if not _cond(insn):
                        found.append((order, 'call', off, text + ' (tail)'))
                        break
                elif _cond(insn):
                    stack.append(tgt)
                else:
                    pc = tgt
                    continue
            pc += insn.size
    if found:
        order, kind, off, text = min(found)
        return {'r0_used': True, 'r0_first_use': '%s@+0x%x %s' % (kind, off, text)}
    if wrote:
        order, off, text = min(wrote)
        return {'r0_used': False, 'r0_first_use': 'overwritten@+0x%x %s' % (off, text)}
    return {'r0_used': False, 'r0_first_use': 'unused'}


def check_symbol(tab, mod, addr):
    """check() on module `mod`'s function at addr, bytes from symtab's images."""
    fn = tab.function_at(mod, addr)
    if fn is None:
        return {'r0_used': None, 'r0_first_use': 'no symbol'}
    code = tab.code(mod, fn.addr, fn.size)
    if code is None:
        return {'r0_used': None, 'r0_first_use': 'no image'}
    return check(code, fn.addr, fn.thumb, fn.size)


def main(argv=None):
    import os
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import symtab
    argv = list(sys.argv[1:] if argv is None else argv)
    if len(argv) < 2 or len(argv) % 2:
        raise SystemExit(__doc__.split('\n\n')[1])
    tab = symtab.table()
    for i in range(0, len(argv), 2):
        r = check_symbol(tab, argv[i], int(argv[i + 1], 16))
        print('%s %s r0_used=%s %s' % (argv[i], argv[i + 1], r['r0_used'], r['r0_first_use']))


if __name__ == '__main__':
    main()
