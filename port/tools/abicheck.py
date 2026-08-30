#!/usr/bin/env python3
"""abicheck -- catch thiscall/fastcall frame-contract breaks in the host port,
from the EMITTED CODE rather than from reading the C++.

THE DEFECT CLASS
    A hosted C++ virtual slot is entered as MSVC __thiscall: `this` in ecx,
    every further argument PUSHED by the caller, and the CALLEE pops them. The
    port fills its vtables with __fastcall thunks; __fastcall carries its first
    two parameters in ecx/edx and pops NOTHING, so a thunk is only
    thiscall-compatible if it declares enough parameters to put the pushed ones
    on the stack, which is what makes the compiler emit `ret <n>`.

    Slot takes N arguments past `this`  =>  the thunk must end in `ret 4*N`.
      bare `ret` where 4*N was wanted  -> UNDERPOP: the caller's pushed
          argument outlives the call, the caller pops one slot short and its
          own `ret` takes a saved register (usually the saved EBP, a stack
          address) as the return address. DEP refuses it: c0000005, access
          code 00000008, faulting address near ESP. Looks like a stack smash.
      `ret n` where 0 was wanted       -> OVERPOP: the callee pops what nobody
          pushed and the caller's frame runs high by n. Same fault, opposite
          sign.

WHAT THIS READS
    `dumpbin /disasm:nobytes` over the hal objects gives both halves without
    trusting any source parsing:
      * the slot, from the fill's own store
            mov dword ptr [__ZTV5Crate+54h],offset ?crate_pounded@@YIHPAX0@Z
        and 0x54/4 == 21;
      * the pop, from the thunk's terminating `ret`.

    THE FORMAT IS LOAD-BEARING. Plain `dumpbin /disasm` interleaves the encoded
    bytes between the address and the mnemonic, the store pattern never
    matches, and this reports "0 vtable slot fills" and exits 0 -- a green that
    checked nothing. --disasm-dir input is therefore checked for that shape,
    and a run that parses zero fills REFUSES rather than passing.

TWO EXEMPTIONS, both mechanical
    DECLINE   a thunk whose every path funnels into port_actor_slot_decline /
              abort / RaiseException never reaches its own `ret`: the SEH
              filter in port_dispatch_guarded returns EXCEPTION_EXECUTE_HANDLER
              and the unwinder restores esp from the frame. Its `ret` is dead
              code, so its pop size cannot matter.
    NON-ACTOR a table that is not the 31-slot Actor layout has a different slot
              map, so the Actor authority does not apply. Those are checked by
              CONSENSUS instead (below).

TWO INDEPENDENT VERDICTS
    AUTHORITY  slot -> argument count taken from include/ActorBase.h (0..17)
               and include/Actor.h (18..30).
    CONSENSUS  no header at all: for a given slot index across every Actor
               table, the pop size the majority of thunks emit is the expected
               one, and a thunk that disagrees with its peers is suspect. This
               is what catches a slot whose header authority is wrong or
               missing, and it is the check that keeps working for the
               non-Actor tables.

WHERE THE INPUT COMES FROM, AND WHY IT IS NOT A FIXED PATH
    The version of this tool recovered from branch port-abi-sweep read a FIXED
    directory, C:\\tmp\\sm64ds-abisweep\\_disasm, baked into the module. Two
    lanes running the suite at once would have written each other's
    disassembly and each measured the other's build -- the exact shape of "a
    real artifact from the wrong run" this repo has been bitten by before, and
    it would have read perfectly green. --disasm-dir is now required and
    port/tools/gen_disasm.cmd defaults it INSIDE the build directory, so the
    input travels with the build it describes.

    python port/tools/abicheck.py --disasm-dir <dir> [--json out.json]
    python port/tools/abicheck.py --selftest
"""

import argparse
import glob
import json
import os
import re
import sys
from collections import defaultdict, Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import msvc_undname as mu  # noqa: E402  (path set above)

# args past `this` for each Actor/ActorBase vtable slot.
# ActorBase.h declares 0..17, Actor.h declares 18..30.
ACTOR_SLOT_ARGS = {
    0: 0, 1: 0, 2: 1, 3: 0, 4: 0, 5: 1, 6: 0, 7: 0, 8: 1, 9: 0, 10: 0, 11: 1,
    12: 0, 13: 2, 14: 2, 15: 0, 16: 0, 17: 0, 18: 0, 19: 1, 20: 0, 21: 1,
    22: 1, 23: 1, 24: 1, 25: 1, 26: 1, 27: 1, 28: 1, 29: 0, 30: 1,
}
SLOT_NAME = {
    0: 'InitResources()', 1: 'BeforeInitResources()',
    2: 'AfterInitResources(u32)', 3: 'CleanupResources()',
    4: 'BeforeCleanupResources()', 5: 'AfterCleanupResources(u32)',
    6: 'Behavior()', 7: 'BeforeBehavior()', 8: 'AfterBehavior(u32)',
    9: 'Render()', 10: 'BeforeRender()', 11: 'AfterRender(u32)',
    12: 'OnPendingDestroy()', 13: 'Virtual34(u32,u32)',
    14: 'Virtual38(u32,u32)', 15: 'OnHeapCreated()', 16: '~D1', 17: '~D0',
    18: 'OnYoshiTryEat()', 19: 'OnTurnIntoEgg(Player&)', 20: 'Virtual50()',
    21: 'OnGroundPounded(Actor&)', 22: 'OnAttacked1(Actor&)',
    23: 'OnAttacked2(Actor&)', 24: 'OnKicked(Actor&)', 25: 'OnPushed(Actor&)',
    26: 'OnHitByCannonBlastedChar(Actor&)', 27: 'OnHitByMegaChar(Player&)',
    28: 'OnHitFromUnderneath(Actor&)', 29: 'OnAimedAtWithEgg()',
    30: 'OnAimedAtWithEggReturnVec()->Vector3',
    31: 'Kill()  [extension]', 32: 'AfterClsn(Actor&)  [extension]',
}

# args past `this` for EXTENSION slots -- the virtuals a derived class adds
# PAST the Actor tail at slot 30. ActorBase.h and Actor.h cannot supply these:
# they are per-class additions, so the header authority above has no key for
# them and every one read NO_AUTHORITY, which is a silent pass.
#
# KEYED ON (TABLE, SLOT), BECAUSE A SLOT NUMBER IS NOT A CONTRACT. The first
# version of this keyed on the slot alone and asserted slot 32 = 1 argument.
# THAT WAS WRONG AND IT BROKE A SECOND CLASS. Swept out of the linked binary --
# `dumpbin /disasm:nobytes walk_window.exe` for `call dword ptr [reg+000000
# 80h]` -- there are exactly THREE slot-32 dispatch sites and they disagree:
#
#   func_ov004_020b08f0 +0xb    mov ecx,esi / call [eax+80h]     pushes 0
#   func_ov002_020eff90 +0xb    push [ebp+10h] / call [eax+80h]  pushes 1
#   func_ov064_02116d1c +0x129  mov ecx,esi / call [eax+80h]     pushes 0
#
# An extension virtual is per-class by definition, so three classes' slot 32
# are three different methods and only ONE of them takes an argument. Widening
# Bully's thunk on the strength of the slot number made its callee pop four
# bytes func_ov064_02116d1c never pushed, and that function unwinds by
# `add esp,0Ch` and four explicit pops with no `mov esp,ebp`, so it cannot
# absorb an overpop -- the same wild execute as the bug, mirrored.
#
# So a row here binds ONE TABLE's slot to the site that dispatches it, and the
# site is named. A slot whose sites disagree and whose table is not listed
# stays EXT_UNJUDGED, which is the honest answer rather than a guess.
ACTOR_EXT_SLOT_AUTHORITY = {
    # PathLift / PathLiftActor_c, the two tables pl_fill_shared writes.
    # Dispatched ONLY by func_ov002_020eff90, the veneer PathLift's own base
    # init seats as the collider callback at MeshColliderBase+0x1c
    # (src/func_ov002_020efaf0.cpp:41), which pushes one word and does not
    # clean it. Report 7447e46c is that word going unpopped.
    ('_data_ov002_0210af70', 32): 1,
    ('_data_ov100_0214857c', 32): 1,
    # EYEROK (176, ov066), Platform-derived, slot 31 = Platform::Kill.
    # run rel0215 wave 2, lane cast-ov066. NOT keyed off the slot number: the
    # binary was swept for `call dword ptr [reg+7Ch]` the way this header's own
    # slot-32 paragraph prescribes, and slot 31 has FOURTEEN dispatch sites in
    # walk_window.exe -- 0046A22B, 005425F0, 005453C6, 005AF5DD, 005AF704,
    # 005BD4FB, 005D3ED1, 005D410F, 005D4371, 005D468E, 005D46CE, 005D481D,
    # 005D497C, 005DC24B -- and every single one is
    #     mov ecx,<reg> / mov eax,dword ptr [<reg>] / call dword ptr [eax+7Ch]
    # with NOTHING pushed. (The lone `push esi` in the 005DC24B window is that
    # function's own prologue save; `mov esi,dword ptr [ebp+8]` follows it.)
    # Slot 31 is UNANIMOUS where slot 32's three sites disagree, so ?ov66_kill's
    # __fastcall(void*, void*) / ret 0 shape is right.
    # ONLY THIS TABLE IS LISTED, deliberately. The same sweep would clear the
    # thirty-odd other slot-31 `*_kill` rows in abicheck_extslot_baseline.txt,
    # but retiring another lane's debt is that lane's call, not this one's, and
    # a blanket slot-31 rule is exactly the shortcut this header forbids.
    # Evidence: ...runs/rel0215/out/w2-ov066/slot31_dispatch_sweep.txt
    ('__ZTV6Eyerok', 31): 0,
}

# calls that do not come back: the thunk's own `ret` is unreachable
NORETURN_SINKS = ('port_actor_slot_decline', 'abort', 'RaiseException',
                  'exit', '_invalid_parameter', 'terminate')

FUNC = re.compile(r'^(\S+)(?:\s+\((.*)\))?:\s*$')
INSN = re.compile(r'^\s+([0-9A-F]+):\s+(\S+)\s*(.*?)\s*$')
VTSTORE = re.compile(
    r'dword ptr \[(?P<tab>[A-Za-z_?@$][\w?@$]*)(?:\+(?P<off>[0-9A-F]+)h)?\]\s*,\s*'
    r'offset (?P<sym>\S+)')
REGLOAD = re.compile(r'^(?P<reg>e[a-z]{2}),offset (?P<sym>\S+)$')
REGCOPY = re.compile(r'^(?P<reg>e[a-z]{2}),(?P<src>e[a-z]{2})$')
# `mov eax,dword ptr [ebp+8]` -- a register loaded from an INCOMING PARAMETER.
# A shared filler takes the table as an argument (`pl_fill_shared(void
# volatile **vt)`), so every slot it writes is stored through one of these and
# the table name is not in this function at all. Bound to the caller's pushed
# argument in bind_param_stores() below.
PARAMLOAD = re.compile(
    r'^(?P<reg>e[a-z]{2}),dword ptr \[ebp\+(?P<off>[0-9A-Fa-f]+)h?\]$')
# `push offset _data_ov002_0210af70` -- the table handed to such a filler.
PUSHOFF = re.compile(r'^offset (?P<sym>\S+)$')
PARAM_TAB = '#param'      # marker table name; never a real symbol
REGLEA = re.compile(r'^(?P<reg>e[a-z]{2}),\[(?P<src>e[a-z]{2})\+(?P<off>[0-9A-F]+h?)\]$')


def load(path):
    raw = open(path, 'rb').read()
    return raw.decode('utf-16-le' if raw[:2] == b'\xff\xfe' else 'utf-8',
                      'replace')


def parse(text, obj):
    """(functions, vtable stores) out of one object's disassembly."""
    funcs, stores, cur = {}, [], None
    pending_push = None     # the `push offset TAB` a `call` may consume
    regs = {}      # a fill usually does `mov eax,offset __ZTV5Crate` once and
    #              # then stores through eax, so the table name only survives
    #              # if we carry the register binding forward
    for line in text.splitlines():
        m = INSN.match(line)
        if m and cur is not None:
            op, args = m.group(2).lower(), m.group(3)
            if op == 'ret':
                a = args.strip().rstrip('hH')
                cur['rets'].add(int(a, 16) if a else 0)
            elif op == 'jmp':
                cur['jmps'].append(args.strip())
            elif op == 'call':
                cur['calls'].append(args.strip())
                # `push offset TAB` / `call FILLER` -- the edge that tells us
                # which table a shared filler was handed. Recorded per callee.
                if pending_push:
                    cur['fill_edges'].append((args.strip(), pending_push))
                pending_push = None
            elif op == 'push':
                pm = PUSHOFF.match(args.strip())
                pending_push = pm.group('sym') if pm else None
            elif op == 'mov':
                s = VTSTORE.search(args)
                if s:
                    tab = s.group('tab')
                    off = int(s.group('off') or '0', 16)
                    base = regs.get(tab)
                    if base:
                        tab, off = base[0], off + base[1]
                    stores.append((tab, off // 4, s.group('sym'),
                                   cur['sym'], obj))
                else:
                    rm = REGLOAD.match(args)
                    if rm:
                        regs[rm.group('reg')] = (rm.group('sym'), 0)
                    else:
                        pm = PARAMLOAD.match(args)
                        if pm:
                            regs[pm.group('reg')] = (
                                '%s+%s' % (PARAM_TAB, pm.group('off')), 0)
                        else:
                            rm = REGCOPY.match(args)
                            if rm and rm.group('src') in regs:
                                regs[rm.group('reg')] = regs[rm.group('src')]
                            elif rm:
                                regs.pop(rm.group('reg'), None)
            elif op == 'lea':
                rm = REGLEA.match(args)
                if rm and rm.group('src') in regs:
                    b = regs[rm.group('src')]
                    regs[rm.group('reg')] = (b[0], b[1] + int(
                        (rm.group('off') or '0').rstrip('hH') or '0', 16))
            cur['n'] += 1
            continue
        m = FUNC.match(line)
        if m and not line.startswith(' '):
            cur = dict(sym=m.group(1), sig=m.group(2) or '', rets=set(),
                       jmps=[], calls=[], n=0, obj=obj, fill_edges=[])
            pending_push = None
            funcs.setdefault(m.group(1), cur)
    return funcs, stores


def bind_param_stores(funcs, stores):
    """Resolve stores a SHARED FILLER made through its table parameter.

    THE HOLE THIS CLOSES. port/hal seats most of its Actor tables through one
    helper per overlay -- `pl_fill_shared(void volatile **vt)`,
    `ov63_fill_shared(...)` -- which writes twenty-odd slots through a pointer
    it was HANDED. In the emitted code that is

        ?pl_fill_shared@@YAXPCRAX@Z:
          mov eax,dword ptr [ebp+8]
          mov dword ptr [eax+4],offset ?pl_binit@@YIHPAX0@Z
          ...

    with no table name anywhere in the function, so the register binding never
    resolved and every one of those fills was dropped. That is not a corner:
    it is how the bug this checker was widened for got seated. pl_after_clsn
    had ZERO rows in abicheck -- measured, 2376 rows and not one of them --
    while the fix was being written on the strength of abicheck's output.

    The caller supplies the missing half, `push offset TAB` / `call FILLER`,
    recorded per function as a fill edge. A filler with several callers (six
    for ov63_fill_shared) fills every one of their tables, so every edge is
    emitted, which is what the source actually does.

    Only the FIRST parameter is bound, and only from `[ebp+8]`. A filler that
    takes the table second, or indexes it, is not attempted and its stores stay
    dropped rather than being guessed at."""
    # callee sym -> [table, ...], DEDUPED: two different functions often hand
    # the same filler the same table (a bring-up and a fill entry point both
    # do), and counting that twice inflates every number downstream.
    edges = defaultdict(list)
    for f in funcs.values():
        for callee, tab in f.get('fill_edges', ()):
            if is_vtable(tab) and tab not in edges[callee]:
                edges[callee].append(tab)
    if not edges:
        return stores, {}
    out, bound = [], Counter()
    for tab, slot, sym, where, obj in stores:
        if not tab.startswith(PARAM_TAB + '+8'):
            out.append((tab, slot, sym, where, obj))
            continue
        for real in edges.get(where, ()):
            out.append((real, slot, sym, where, obj))
            bound[where] += 1
    return out, bound


# ---- pop size of a symbol we have no disassembly for ------------------------
# A thunk that only tail-jumps hands the caller its TARGET's `ret`, and the
# target is usually a shadow-class method compiled into some src object we did
# not disassemble. Its MSVC decoration already carries the answer: the
# convention plus the declared parameter list. `undname` turns
#   ?AfterBehavior@ActorBase@@UAEXI@Z
# into
#   public: virtual void __thiscall ActorBase::AfterBehavior(unsigned int)
# and __thiscall pops every declared parameter, so that one is `ret 4`.
CONV_POP = {
    '__cdecl': lambda n: 0,
    '__stdcall': lambda n: 4 * n,
    '__thiscall': lambda n: 4 * n,
    '__fastcall': lambda n: 4 * max(0, n - 2),
}
WIDE = ('double', '__int64', 'long long')


def split_params(s):
    depth, cur, out = 0, '', []
    for ch in s:
        if ch in '(<':
            depth += 1
        elif ch in ')>':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur)
            cur = ''
        else:
            cur += ch
    out.append(cur)
    return [x.strip() for x in out if x.strip() and x.strip() != 'void']


def pop_from_signature(sig):
    """(pop, why) from a demangled signature, or (None, why)."""
    conv = next((c for c in CONV_POP if c in sig), None)
    if conv is None:
        return None, 'no convention in signature'
    i = sig.find('(')
    if i < 0:
        return None, 'no parameter list'
    j = sig.rfind(')')
    params = split_params(sig[i + 1:j])
    if any(w in p for p in params for w in WIDE):
        return None, 'wide parameter, size not 4'
    return CONV_POP[conv](len(params)), '%s/%d params' % (conv, len(params))


def pops(sym, funcs, extsig, depth=0):
    """Pop size the caller sees, following tail jumps. (set|None, how)"""
    f = funcs.get(sym)
    if f is None or depth > 6:
        sig = extsig.get(sym)
        if sig:
            p, why = pop_from_signature(sig)
            if p is not None:
                return {p}, 'decoration(' + why + ')'
            return None, 'undecidable:' + why
        return None, 'external'
    if f['rets']:
        return f['rets'], 'direct'
    for j in f['jmps']:
        r, how = pops(j, funcs, extsig, depth + 1)
        if r:
            return r, 'tailjmp->' + j
    return None, 'noret'


def declines(sym, funcs, depth=0):
    """Is this thunk itself a decline stub?

    Only a DIRECT call to a no-return sink counts. Recursing through callees
    would exempt every thunk that reaches a ROM body that can abort somewhere,
    which is nearly all of them -- and it silently swallowed crate_pounded, a
    known-live bug, when this was written with a depth of 4. The decline
    thunks inline ac_trap_report, so the sink call is in the thunk's own body.
    """
    f = funcs.get(sym)
    if f is None or depth > 4:
        return False
    if any(any(s in c for s in NORETURN_SINKS)
           for c in f['calls'] + f['jmps']):
        return True
    # A thunk whose only exit is a tail jump into a decline stub is itself one
    # (sub_actors' sa_trap13 sets a slot number and jumps to sa_trap, which
    # aborts). Following TAIL JUMPS is safe; following ordinary calls is not,
    # because almost every ROM body can reach an abort somewhere downstream.
    if not f['rets']:
        return any(declines(j, funcs, depth + 1) for j in f['jmps'])
    return False


def is_vtable(t):
    """Is this store destination a vtable at all?

    The store pattern also matches writes into ordinary static arrays, so keep
    only destinations that name a vtable: the Itanium _ZTV mangling the port
    uses for its host tables, an overlay data_ symbol, or an explicit VTable_.
    """
    return (t.startswith('__ZTV') or t.startswith('_ZTV')
            or t.startswith('_data_') or 'VTable' in t)


def analyse(funcs, stores, extsig):
    """Per-fill verdicts plus the consensus pass. Returns rows."""
    stores = [s for s in stores if is_vtable(s[0])]

    # classify tables: ACTOR31 iff anything is seated in the 18..30 range.
    # Slots 18..30 exist only on the Actor extension of ActorBase. Every other
    # host table in the port is shorter than that: Model 6, ModelAnim 7,
    # MeshCollider 13, MovingMeshCollider 16, ExpandingHeap 16, cylinder 4,
    # particle 2, and the dBase_c/dMeter_c/dMap_c HUD tables 18 (max slot 17).
    # Several Actor tables also carry a word at slot 31, so do not bound max.
    slotset = defaultdict(set)
    for tab, slot, sym, where, obj in stores:
        slotset[tab].add(slot)
    actor31 = {t for t, s in slotset.items() if any(18 <= x <= 30 for x in s)}

    rows = []
    for tab, slot, sym, where, obj in stores:
        f = funcs.get(sym, {})
        r, how = pops(sym, funcs, extsig)
        row = dict(table=tab, slot=slot, thunk=sym, sig=f.get('sig', ''),
                   obj=obj, fill=where, how=how,
                   pop=(sorted(r)[0] if r and len(r) == 1 else None),
                   pops=sorted(r) if r else None,
                   actor31=tab in actor31,
                   decline=declines(sym, funcs) if f else False,
                   slot_sig=SLOT_NAME.get(slot, '?'))
        need = ACTOR_SLOT_ARGS.get(slot) if row['actor31'] else None
        # Extension virtuals, the slots a derived class adds past the Actor
        # tail. The header authority has no key for them by construction, so
        # before this they fell straight through to NO_AUTHORITY and rested on
        # a consensus that cannot see a uniformly wrong slot.
        ext = need is None and row['actor31'] and slot > 30
        if ext:
            need = ACTOR_EXT_SLOT_AUTHORITY.get((tab, slot))
        row['ext_slot'] = ext
        row['want_pop'] = None if need is None else 4 * need
        if row['decline']:
            row['verdict'] = 'DECLINE'          # ret unreachable, cannot matter
        elif r is None:
            row['verdict'] = 'NORETURN'
        elif len(r) > 1:
            row['verdict'] = 'MIXED'
        elif need is None and ext:
            # An extension slot with no dispatch evidence. Loud on purpose:
            # NO_AUTHORITY reads as "a short non-Actor table", which is a fair
            # description of a Model or HUD table and a false one here.
            row['verdict'] = 'EXT_UNJUDGED'
        elif need is None:
            row['verdict'] = 'NO_AUTHORITY'     # non-Actor table: consensus
        else:
            row['verdict'] = ('OK' if row['pop'] == row['want_pop'] else
                              'UNDERPOP' if row['pop'] < row['want_pop'] else
                              'OVERPOP')
        rows.append(row)

    # consensus: what do a slot's peers emit, with no header at all
    peer = defaultdict(Counter)
    for r in rows:
        if r['pop'] is not None and not r['decline']:
            key = consensus_key(r)
            peer[key][r['pop']] += 1
    for r in rows:
        key = consensus_key(r)
        c = peer.get(key)
        if not c or r['pop'] is None or r['decline']:
            r['consensus'] = None
            continue
        top, n = c.most_common(1)[0]
        r['consensus'] = top
        r['consensus_n'] = n
        r['consensus_total'] = sum(c.values())
        r['consensus_odd'] = (r['pop'] != top and n > 1)
        # CONSENSUS CANNOT SEE A SLOT THAT IS WRONG EVERYWHERE. `consensus_odd`
        # asks whether a fill disagrees with its peers, so when every peer is
        # wrong in the same way nothing is odd and the slot reads clean twice
        # over -- which is precisely how slot 32 hid: three fills, all the
        # two-parameter shape, unanimous. Where the slot DOES have an
        # authority, say so when the whole peer group disagrees with it.
        r['consensus_vs_authority'] = (
            r.get('want_pop') is not None and top != r['want_pop'])
    return rows, actor31, slotset


def consensus_key(r):
    """Who counts as a fill's PEERS.

    Slots 0..30 are declared once, in ActorBase.h and Actor.h, so every
    Actor-layout table's slot 21 is the same method and they are all peers.
    AN EXTENSION SLOT IS NOT: a virtual a derived class adds is that class's
    own, so PathLift's slot 32 and Bully's slot 32 are different methods that
    happen to share an index. Pooling them makes the majority of one class the
    evidence against another -- measured here, it flagged the CORRECT
    pl_after_clsn as an outlier because Bully's two equally-correct bare rets
    outvoted it, on a 2-versus-2 split decided by tie-break. So extension slots
    are peers only within their own table."""
    return ((r['table'], r['slot']) if r['slot'] > 30
            else ('ACTOR31' if r['actor31'] else r['table'], r['slot']))


EXT_BASELINE = 'abicheck_extslot_baseline.txt'


def load_ext_baseline():
    """The frozen set of extension-slot fills with no per-table authority.

    Same contract as aritycheck's two baselines: DEBT, not clearance. The set
    may shrink without ceremony; a row that is not in it fails the run."""
    p = os.path.join(os.path.dirname(os.path.abspath(__file__)), EXT_BASELINE)
    keys = set()
    try:
        with open(p, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    keys.add(line)
    except OSError:
        pass
    return keys, p


def distinct(rows, pred):
    seen, out = set(), []
    for r in sorted(rows, key=lambda r: (r['thunk'], r['slot'])):
        if not pred(r):
            continue
        k = (r['thunk'], r['slot'])
        if k not in seen:
            seen.add(k)
            out.append(r)
    return out


# ---------------------------------------------------------------------------
# selftest: fixtures, so a parser that stops seeing a shape is loud
# ---------------------------------------------------------------------------
FIX_NOBYTES = """\
?crate_pounded@@YIHPAX0@Z:
  00000000: mov         eax,offset __ZTV5Crate
  00000005: ret
?good_pounded@@YIHPAX0@Z:
  00000000: ret         4
?hmc_trap27@@YIHPAX0@Z:
  00000000: call        _port_actor_slot_decline
  00000005: ret
?sa_trap13@@YIHPAX0@Z:
  00000000: jmp         ?sa_trap@@YIHPAX0@Z
?sa_trap@@YIHPAX0@Z:
  00000000: call        _RaiseException@16
  00000005: ret
?fwd_thunk@@YIHPAX0@Z:
  00000000: jmp         ?AfterBehavior@ActorBase@@UAEXI@Z
?fwd_thunk_wrong@@YIHPAX0@Z:
  00000000: jmp         ?OnHeapCreated@ActorBase@@UAEXXZ
?ext32_short@@YIHPAX0@Z:
  00000000: ret
?ext32_other@@YIHPAX0@Z:
  00000000: ret
?ext34_unknown@@YIHPAX0@Z:
  00000000: ret
_hal_fill_crate:
  00000000: mov         eax,offset __ZTV5Crate
  00000005: mov         dword ptr [eax+54h],offset ?crate_pounded@@YIHPAX0@Z
  0000000B: mov         dword ptr [eax+6Ch],offset ?good_pounded@@YIHPAX0@Z
  00000011: mov         dword ptr [eax+4Ch],offset ?hmc_trap27@@YIHPAX0@Z
  00000017: mov         dword ptr [eax+34h],offset ?sa_trap13@@YIHPAX0@Z
  0000001D: mov         dword ptr [eax+20h],offset ?fwd_thunk@@YIHPAX0@Z
  00000023: mov         dword ptr [eax+58h],offset ?fwd_thunk_wrong@@YIHPAX0@Z
  00000029: mov         dword ptr [eax+88h],offset ?ext34_unknown@@YIHPAX0@Z
  0000002F: ret
?pl_filler@@YAXPCRAX@Z:
  00000000: push        ebp
  00000001: mov         ebp,esp
  00000003: mov         eax,dword ptr [ebp+8]
  00000006: mov         dword ptr [eax+58h],offset ?good_pounded@@YIHPAX0@Z
  0000000D: mov         dword ptr [eax+80h],offset ?ext32_short@@YIHPAX0@Z
  00000014: pop         ebp
  00000015: ret
_hal_fill_pathlift:
  00000000: push        offset _data_ov002_0210af70
  00000005: call        ?pl_filler@@YAXPCRAX@Z
  0000000A: add         esp,4
  0000000D: ret
_hal_fill_crate2:
  00000000: mov         eax,offset __ZTV6Crate2
  00000005: mov         dword ptr [eax+58h],offset ?good_pounded@@YIHPAX0@Z
  0000000B: mov         dword ptr [eax+80h],offset ?ext32_other@@YIHPAX0@Z
  00000011: ret
"""

# The SAME fill, as plain `dumpbin /disasm` emits it: the encoded bytes sit
# between the address and the mnemonic. The store pattern must not match, and
# the run must be REFUSED rather than reported green. This is the false-green
# the wrapper was written against, pinned here at its source.
FIX_WITHBYTES = """\
_hal_fill_crate:
  00000000: B8 00 00 00 00     mov         eax,offset __ZTV5Crate
  00000005: C7 40 54 00 00 00  mov         dword ptr [eax+54h],offset ?crate_pounded@@YIHPAX0@Z
  0000000B: C3                 ret
"""


def selftest():
    bad = 0
    print('abicheck --selftest')

    funcs, stores = parse(FIX_NOBYTES, 'fixture')
    # The extsig map is what the demangler produces at run time. Both entries
    # are real undname output, verified by msvc_undname's own selftest.
    extsig = {'?AfterBehavior@ActorBase@@UAEXI@Z':
              'public: virtual void __thiscall ActorBase::AfterBehavior('
              'unsigned int)',
              '?OnHeapCreated@ActorBase@@UAEXXZ':
              'public: virtual void __thiscall ActorBase::OnHeapCreated(void)'}
    stores, _pb = bind_param_stores(funcs, stores)
    rows, actor31, slotset = analyse(funcs, stores, extsig)
    by = {(r['thunk'], r['slot']): r for r in rows}

    def want(thunk, slot, verdict, pop=None, note=''):
        nonlocal bad
        r = by.get((thunk, slot))
        got = r['verdict'] if r else 'NO ROW'
        gotpop = r['pop'] if r else None
        ok = got == verdict and (pop is None or gotpop == pop)
        bad += 0 if ok else 1
        print('    %-4s slot %-2s want %-12s got %-12s pop %-4s  %s'
              % ('ok' if ok else 'FAIL', slot, verdict, got, gotpop, note))

    print('\n  PARSE + VERDICT, %d fills off the nobytes fixture' % len(rows))
    # 0x54/4 == 21, OnGroundPounded(Actor&) wants ret 4; a bare ret is the
    # crate_pounded bug exactly as it shipped.
    want('?crate_pounded@@YIHPAX0@Z', 21, 'UNDERPOP', 0,
         'bare ret on a one-argument slot')
    # 0x6c/4 == 27, OnHitByMegaChar(Player&) wants ret 4 and gets it
    want('?good_pounded@@YIHPAX0@Z', 27, 'OK', 4, 'ret 4 on a one-arg slot')
    # 0x4c/4 == 19, OnTurnIntoEgg wants ret 4, but this one declines
    want('?hmc_trap27@@YIHPAX0@Z', 19, 'DECLINE', 0,
         'direct call to a no-return sink exempts the ret')
    # 0x34/4 == 13, Virtual34(u32,u32); tail-jumps into a decline stub
    want('?sa_trap13@@YIHPAX0@Z', 13, 'DECLINE', None,
         'tail jump into a decline stub is itself a decline')
    # 0x20/4 == 8, AfterBehavior(u32) wants ret 4; resolved through the
    # TARGET'S DECORATION because the thunk only tail-jumps
    want('?fwd_thunk@@YIHPAX0@Z', 8, 'OK', 4,
         'tail-jump pop resolved from the target decoration')
    # 0x58/4 == 22, OnAttacked1(Actor&) wants ret 4. This thunk tail-jumps to a
    # ZERO-argument method, so it hands the caller a bare ret. NOTHING IN THE
    # THUNK SAYS SO -- the verdict exists only because the target's decoration
    # was demangled. Until 2026-08-17 abicheck never had a demangler wired
    # (the recovered version read a precomputed file pair nothing generated),
    # so 638 of 2391 fills read NORETURN and this whole route was dead. This
    # is the RED half of that repair.
    want('?fwd_thunk_wrong@@YIHPAX0@Z', 22, 'UNDERPOP', 0,
         'wrong-arity tail-jump target, caught ONLY through the decoration')

    print('\n  A TABLE PASSED TO A SHARED FILLER IS STILL A TABLE')
    # THE MISS THAT MATTERED. pl_after_clsn -- the thunk in report 7447e46c --
    # is seated by pl_fill_shared through a pointer PARAMETER, so no register
    # in that function ever names a table and the fill was dropped entirely.
    # Measured on the real tree before this: 2376 rows and not one of them was
    # pl_after_clsn, pl_kill, pl_pounded or pl_atk1. The checker was reported
    # green over the exact seat the bug shipped from.
    ok = any(r['thunk'] == '?ext32_short@@YIHPAX0@Z'
             and r['table'] == '_data_ov002_0210af70' for r in rows)
    bad += 0 if ok else 1
    print('    %-4s a fill stored through [ebp+8] is bound to the table its '
          'caller pushed' % ('ok' if ok else 'FAIL'))

    print('\n  EXTENSION SLOTS PAST 30 (the slot-32 hole, report 7447e46c)')
    # Authority is keyed on (TABLE, slot). This table's slot 32 is dispatched
    # by func_ov002_020eff90, which pushes one word, so a bare ret underpops.
    want('?ext32_short@@YIHPAX0@Z', 32, 'UNDERPOP', 0,
         'bare ret where THIS TABLE\'s dispatch site pushes')
    # SAME SLOT, DIFFERENT TABLE, NO AUTHORITY. Keying on the slot number alone
    # would call this UNDERPOP too, which is exactly the wrong answer that
    # widened Bully's thunk into an overpop against a site that pushes nothing.
    want('?ext32_other@@YIHPAX0@Z', 32, 'EXT_UNJUDGED', 0,
         'same slot on a table with no authority stays unjudged')
    # 0x88/4 == 34: no authority anywhere, must not read NO_AUTHORITY, which
    # is the wording for a short non-Actor table.
    want('?ext34_unknown@@YIHPAX0@Z', 34, 'EXT_UNJUDGED', 0,
         'extension slot with no authority is unjudged, not passed')

    print('\n  A UNANIMOUSLY WRONG SLOT IS INVISIBLE TO CONSENSUS')
    # Two tables, both seating slot 32 with the same bare ret. Nothing is an
    # outlier, so consensus_odd is False for both -- which is how the real slot
    # 32 hid behind matching fills. The authority arm has to be what speaks,
    # and only for the table it actually has evidence for.
    r32 = [r for r in rows if r['slot'] == 32 and r['pop'] == 0]
    ok = len(r32) == 2 and not any(r.get('consensus_odd') for r in r32)
    bad += 0 if ok else 1
    print('    %-4s %d slot-32 fills agree with each other, 0 flagged as '
          'consensus outliers' % ('ok' if ok else 'FAIL', len(r32)))
    auth = [r for r in r32 if r['table'] == '_data_ov002_0210af70']
    ok = bool(auth) and all(r.get('consensus_vs_authority') for r in auth)
    bad += 0 if ok else 1
    print('    %-4s and the peer group is reported against the authority for '
          'the table that has one' % ('ok' if ok else 'FAIL'))

    print('\n  ACTOR31 classification')
    ok = '__ZTV5Crate' in actor31
    bad += 0 if ok else 1
    print('    %-4s a table with a slot in 18..30 is the Actor layout'
          % ('ok' if ok else 'FAIL'))

    print('\n  THE WRONG DISASSEMBLY FORMAT MUST NOT READ AS CLEAN')
    f2, s2 = parse(FIX_WITHBYTES, 'fixture')
    s2 = [s for s in s2 if is_vtable(s[0])]
    ok = len(s2) == 0
    bad += 0 if ok else 1
    print('    %-4s plain /disasm yields %d parseable stores (want 0, and the '
          'caller must refuse a zero-fill run)'
          % ('ok' if ok else 'FAIL', len(s2)))

    print('\n  POP FROM DECORATION')
    for sig, wantpop in (
            ('public: virtual void __thiscall A::f(unsigned int)', 4),
            ('void __cdecl f(int,int)', 0),
            ('int __fastcall f(void *,void *,int)', 4),
            ('public: void __thiscall A::f(double)', None)):
        got, why = pop_from_signature(sig)
        ok = got == wantpop
        bad += 0 if ok else 1
        print('    %-4s want %-4s got %-4s  %s'
              % ('ok' if ok else 'FAIL', wantpop, got, sig[:52]))

    print('\n%s' % ('SELFTEST PASSED' if not bad
                    else 'SELFTEST FAILED (%d)' % bad))
    return 1 if bad else 0


# ---------------------------------------------------------------------------
def main(argv):
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--disasm-dir', metavar='DIR',
                    help='directory of `dumpbin /disasm:nobytes` .txt output')
    ap.add_argument('--json', metavar='PATH')
    ap.add_argument('--selftest', action='store_true')
    args = ap.parse_args(argv[1:])

    if args.selftest:
        return selftest()
    if not args.disasm_dir:
        print('abicheck: --disasm-dir is required. Generate the input with\n'
              '  port/tools/gen_disasm.cmd <build-dir> <out-dir>\n'
              'which emits /disasm:nobytes and nothing else. There is no '
              'default path on purpose: a fixed one is shared between lanes '
              'and lets a run measure another tree\'s build.')
        return 2
    dis = os.path.abspath(args.disasm_dir)
    if not os.path.isdir(dis):
        print('abicheck: REFUSED -- %s is not a directory' % dis)
        return 2
    files = sorted(glob.glob(os.path.join(dis, '*.txt')))
    if not files:
        print('abicheck: REFUSED -- %s holds no .txt disassembly' % dis)
        return 2

    funcs, stores = {}, []
    for p in files:
        f, s = parse(load(p), os.path.basename(p).replace('.obj.txt', ''))
        for k, v in f.items():
            funcs.setdefault(k, v)
        stores.extend(s)

    # Resolve the shared fillers' parameter-borne stores. Done AFTER every
    # object is parsed, because the filler and the caller that names its table
    # need not be in the same object.
    stores, param_bound = bind_param_stores(funcs, stores)
    if param_bound:
        print('shared fillers: %d slot fills recovered through a table '
              'PARAMETER, from %d filler(s)'
              % (sum(param_bound.values()), len(param_bound)))
        for fn, n in sorted(param_bound.items(), key=lambda kv: -kv[1])[:6]:
            print('    %-44s %3d fills' % (fn[:44], n))

    # mangled name -> demangled signature, for the bodies a thunk tail-jumps
    # into that are defined outside the hal objects. Without these a
    # tail-jump-only thunk reads NORETURN, and NORETURN is UNCHECKED, not
    # passed -- it was 638 of 2391 fills on the first run of this tool.
    #
    # The recovered version read a precomputed _unres.txt / _unres_demangled
    # .txt pair out of the disasm directory. Nothing generated that pair, so
    # in practice extsig was always empty. It now demangles the targets itself
    # through port/tools/msvc_undname.py, and the file pair is kept only as an
    # override for a machine with no MSVC at all.
    wanted = set()
    for f in funcs.values():
        for j in f['jmps']:
            if j.startswith('?') and j not in funcs:
                wanted.add(j)
    for tab, slot, sym, where, obj in stores:
        if sym.startswith('?') and sym not in funcs:
            wanted.add(sym)

    extsig = {}
    npath = os.path.join(dis, '_unres.txt')
    dpath = os.path.join(dis, '_unres_demangled.txt')
    source = None
    try:
        names = open(npath).read().split('\n')
        demang = open(dpath, encoding='utf-8', errors='replace').read(
            ).splitlines()
        if len(demang) != len(names):
            print('abicheck: REFUSED -- the _unres pair in %s has %d demangled '
                  'lines for %d names; refusing to guess the alignment'
                  % (dis, len(demang), len(names)))
            return 2
        extsig = dict(zip(names, demang))
        source = 'the _unres pair in the disasm directory'
    except OSError:
        extsig = mu.demangle(sorted(wanted))
        if extsig:
            source = 'undname at %s' % mu.find_undname()
        else:
            source = None

    rows, actor31, slotset = analyse(funcs, stores, extsig)
    vt = [s for s in stores if is_vtable(s[0])]

    print('%d objects, %d functions, %d vtable slot fills'
          % (len(set(x[4] for x in vt)), len(funcs), len(vt)))
    print('%d Actor-layout tables, %d other tables\n'
          % (len(actor31), len(slotset) - len(actor31)))
    counts = Counter(r['verdict'] for r in rows)
    for k, v in sorted(counts.items()):
        print('  %-14s %d' % (k, v))

    # COVERAGE, printed because three of those verdicts are NOT passes.
    #
    #   NORETURN      the thunk's pop could not be resolved at all: it only
    #                 tail-jumps, and the target's decoration could not be
    #                 read. These fills are UNCHECKED, never passed.
    #   NO_AUTHORITY  a table that is not the 31-slot Actor layout, so the
    #                 header authority does not apply. Consensus still covers
    #                 it, but only where the slot has peers to disagree with.
    #   DECLINE       exempt by construction: the ret is unreachable.
    #
    # A checker that prints one number for "fills read" and lets the reader
    # assume all of them were judged is the vacuous-green shape one level up.
    judged = counts.get('OK', 0) + counts.get('UNDERPOP', 0) + \
        counts.get('OVERPOP', 0) + counts.get('MIXED', 0)
    print('\ndemangler: %s'
          % (source or 'NONE. Every tail-jump-only thunk reads NORETURN, '
                       'which is UNCHECKED and not a pass.'))
    if source:
        print('           %d of %d tail-jump targets resolved'
              % (len(extsig), len(wanted)))
    print('coverage: %d of %d fills got an AUTHORITY verdict (%.0f%%). '
          '%d NORETURN are UNCHECKED, not passed; %d NO_AUTHORITY rest on '
          'consensus alone; %d DECLINE are exempt.'
          % (judged, len(rows), 100.0 * judged / max(1, len(rows)),
             counts.get('NORETURN', 0),
             counts.get('NO_AUTHORITY', 0), counts.get('DECLINE', 0)))

    # A run that parsed nothing has not passed. This is the false green the
    # whole wrapper was written against; refuse it here, at the source.
    if not vt:
        print('\nabicheck: REFUSED -- 0 vtable slot fills parsed. The '
              'disassembly must be produced with `dumpbin /disasm:nobytes`; '
              'plain /disasm interleaves the encoded bytes, the store pattern '
              'never matches, and this run would otherwise read green while '
              'checking nothing.')
        return 2

    uniq = distinct(rows, lambda r: r['verdict'] in ('UNDERPOP', 'OVERPOP',
                                                     'MIXED'))
    nbad = len([r for r in rows if r['verdict'] in ('UNDERPOP', 'OVERPOP',
                                                    'MIXED')])
    print('\n=== %d distinct AUTHORITY mismatches (%d slot fills) ==='
          % (len(uniq), nbad))
    for r in uniq:
        print('%-9s slot %2d %-36s want ret %-3s got ret %-3s %s'
              % (r['verdict'], r['slot'], r['slot_sig'], r['want_pop'],
                 r['pop'], r['thunk']))
        print('           table %s  [%s]' % (r['table'], r['obj']))

    uodd = distinct(rows, lambda r: r.get('consensus_odd'))
    print('\n=== %d distinct CONSENSUS outliers (no header used) ==='
          % len(uodd))
    for r in uodd:
        flag = ('' if r['verdict'] in ('UNDERPOP', 'OVERPOP')
                else '  <-- AUTHORITY MISSED THIS')
        print('slot %2d %-30s %-34s got ret %-3s peers ret %s (%d/%d)%s'
              % (r['slot'], r['table'], r['thunk'], r['pop'], r['consensus'],
                 r['consensus_n'], r['consensus_total'], flag))

    # A whole peer group that disagrees with the slot's authority. This is the
    # arm consensus alone cannot have: it fires when NOTHING is an outlier
    # because everything is wrong together.
    uall = distinct(rows, lambda r: r.get('consensus_vs_authority'))
    print('\n=== %d distinct slots where the WHOLE peer group disagrees with '
          'the authority ===' % len(uall))
    for r in uall:
        print('slot %2d %-30s peers ret %-3s (%d/%d) but the dispatch site '
              'wants ret %s'
              % (r['slot'], r['table'], r['consensus'], r['consensus_n'],
                 r['consensus_total'], r['want_pop']))

    # Extension virtuals with no dispatch evidence. Not a pass: the header
    # authority stops at slot 30 by construction, so without this they are
    # invisible rather than judged.
    # EXTENSION slots with no per-table authority. LOUD BUT RATCHETED, not a
    # gate failure: the first cut of this counted them in the exit status, and
    # since the tree carries 35 of them abicheck could never exit 0 again --
    # which permanently fails abi_prove's "GREEN on this build" arm, whose
    # whole job is to show the checker passes when nothing is broken. A
    # checker that always fails proves as little as one that always passes.
    # So it ratchets, the way aritycheck's two baselines do: the frozen set may
    # shrink freely, and a NEW row is the only thing that fails.
    uext = distinct(rows, lambda r: r['verdict'] == 'EXT_UNJUDGED')
    live_ext = sorted({'%s slot %d %s' % (r['table'], r['slot'], r['thunk'])
                       for r in rows if r['verdict'] == 'EXT_UNJUDGED'})
    base_ext, base_path = load_ext_baseline()
    new_ext = [k for k in live_ext if k not in base_ext]
    gone_ext = [k for k in base_ext if k not in live_ext]
    print('\n=== %d distinct EXTENSION slots past 30 with no per-table '
          'authority (UNJUDGED, not passed) ===' % len(uext))
    for r in uext:
        print('slot %2d %-30s %-34s got ret %-3s  [%s]'
              % (r['slot'], r['table'], r['thunk'], r['pop'], r['obj']))
    print('  %d baselined, %d live, %d NEW, %d retired  [%s]'
          % (len(base_ext), len(live_ext), len(new_ext), len(gone_ext),
             os.path.basename(base_path)))
    if new_ext:
        print('  NEW, and this is the failure:')
        for k in new_ext:
            print('      %s' % k)
        print('  Read its dispatch site out of the emitted code and add a row '
              'to ACTOR_EXT_SLOT_AUTHORITY keyed on (table, slot), or baseline '
              'it with a ruling. Do NOT key on the slot number alone: slot 32 '
              'has three dispatch sites and only one of them pushes.')
    else:
        print('  EXTENSION RATCHET PASSED: no new unjudged extension slot. '
              'The baselined rows are UNCHECKED DEBT, not clearance.')

    if args.json:
        with open(args.json, 'w', encoding='utf-8') as f:
            json.dump(rows, f, indent=1)

    return 1 if uniq or uodd or uall or new_ext else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
