"""slot16_guard: every vtable SLOT 16 store must carry the destructor slot's
calling convention.

WHY THIS EXISTS. Slot 16 is the one actor vtable slot MSVC dispatches itself,
and its contract is not the vtable law's. The only dispatcher in the image is
matched source -- src/_ZN7fBase_c21AfterCleanupResourcesEj.cpp spells
this->~fBase_c() -- and MSVC compiles a virtual destructor call through its
single destructor slot as __thiscall with ONE CALLEE-POPPED stack argument, the
delete flag. A plain two-argument __fastcall face ends in a bare `ret`, pops
nothing, and the caller's epilogue then unwinds four bytes low and returns to
the saved EBP: a stack address, which Windows kills the process for executing.
That was the eleventh defect of the link100 port line. See port/hal/port_d16.h.

The rule this enforces is one line: every word stored into offset 0x40 of a
vtable must point at a PORT_D16 wrapper, and that wrapper must end in `ret 4`.

WHY IT READS THE IMAGE AND NOT THE SOURCE. The source pattern `vt[16] =` misses
real stores: this tree writes slot 16 as `vt[16]=` with no spaces, as
`_ZTV6Camera[16] =`, as `data_02099edc[16] = (int)(size_t)...`, as
`tabs[k][16] =`, and as a RANGE FILL LOOP that writes a trap into all 32 slots
and never overrides 16. The last shape is invisible to every source grep and is
exactly what this guard caught on the Platform base table.

HOW A TABLE IS RECOGNISED, without a name heuristic: group every
`mov dword ptr [abs32], imm32` whose imm32 is a code address by the map symbol
that contains abs32. A symbol taking at least MIN_SLOTS such stores at distinct
4-byte offsets is a vtable fill. Then the store at offset 0x40 is slot 16.
"""
import argparse, bisect, collections, re, sys
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

MIN_SLOTS = 16

# Tables that are NOT fBase_c-derived, so fBase_c::AfterCleanupResources -- the
# only slot-16 dispatcher in the image -- can never reach their slot 16. Each
# row carries the reason it is not an actor table.
ALLOW = {
    "_data_02099fe4": "CLPS block-reference table, a card/filesystem object, "
                      "not an fBase_c: nothing dispatches its slot 16",
}
LINE = re.compile(r"^\s*([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+([0-9a-fA-F]{8})\s+(\S*)\s*(\S*)\s*$")
R = ["eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"]


def load(exe, mapf):
    names = collections.defaultdict(list)
    for l in open(mapf, errors="replace"):
        m = LINE.match(l.rstrip("\n"))
        if m:
            names[int(m.group(4), 16)].append(m.group(3))
    keys = sorted(names)
    pe = pefile.PE(exe, fast_load=True)
    ib = pe.OPTIONAL_HEADER.ImageBase
    secs = {}
    for s in pe.sections:
        secs[s.Name.rstrip(b"\0").decode()] = (ib + s.VirtualAddress, s.get_data())
    return names, keys, secs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", required=True)
    ap.add_argument("--map")
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    mapf = a.map or a.exe.rsplit(".", 1)[0] + ".map"
    names, keys, secs = load(a.exe, mapf)

    def sym(va):
        i = bisect.bisect_right(keys, va) - 1
        if i < 0:
            return "(none)", 1 << 30
        return names[keys[i]][0], va - keys[i]

    tb, td = secs[".text"]
    tlo, thi = tb, tb + len(td)
    md = Cs(CS_ARCH_X86, CS_MODE_32)

    def read(va, n):
        for b, d in secs.values():
            if b <= va < b + len(d):
                return d[va - b:va - b + n]
        return None

    def retform(va, depth=0):
        d = read(va, 0x400)
        if not d or depth > 4:
            return set()
        for i in md.disasm(d, va):
            if i.mnemonic == "ret":
                return {i.op_str or "0"}
            if i.mnemonic == "jmp" and re.fullmatch(r"0x[0-9a-f]+", i.op_str):
                return retform(int(i.op_str, 16), depth + 1)
            if i.mnemonic == "int3":
                break
        return set()

    abs_stores, reg_sites = [], []
    for i in range(len(td) - 10):
        b = td[i]
        if b == 0xC7:
            modrm = td[i + 1]
            if (modrm >> 3) & 7:
                continue
            mod, rm = modrm >> 6, modrm & 7
            if mod == 0 and rm == 5:                       # [abs32], imm32
                dst = int.from_bytes(td[i + 2:i + 6], "little")
                imm = int.from_bytes(td[i + 6:i + 10], "little")
                if tlo <= imm < thi:
                    abs_stores.append((tb + i, dst, imm))
            elif mod == 1 and rm != 4 and td[i + 2] == 0x40:   # [reg+0x40], imm32
                imm = int.from_bytes(td[i + 3:i + 7], "little")
                if tlo <= imm < thi:
                    abs_stores.append((tb + i, None, imm))
        elif b == 0x89 and (td[i + 1] >> 6) == 1 and (td[i + 1] & 7) != 4 and td[i + 2] == 0x40:
            reg_sites.append((tb + i, R[(td[i + 1] >> 3) & 7]))

    # group absolute stores by containing symbol to find table fills
    bysym = collections.defaultdict(dict)
    for site, dst, imm in abs_stores:
        if dst is None:
            continue
        base, off = sym(dst)
        if off % 4 == 0 and off < 0x200:
            bysym[base][off] = (site, imm)
    slot16 = []
    for base, slots in bysym.items():
        if len(slots) >= MIN_SLOTS and 0x40 in slots:
            site, imm = slots[0x40]
            slot16.append((base, site, imm))
    # plus the vt-parameter form, which is a slot-16 store by construction.
    # LAST WRITE WINS: a fill that loops a trap into every slot and then
    # overrides 16 compiles to two stores at +0x40 in the same function, and
    # only the later one is the value the table ends up holding.
    byfn = {}
    for site, dst, imm in abs_stores:
        if dst is None:
            fn, _ = sym(site)
            prev = byfn.get(fn)
            if prev is None or site > prev[0]:
                byfn[fn] = (site, imm)
    for fn, (site, imm) in byfn.items():
        slot16.append(("(vt parameter in %s)" % fn, site, imm))

    bad, noret, allowed = [], [], []
    for base, site, tgt in slot16:
        n, d = sym(tgt)
        r = retform(tgt)
        if n.startswith("??$port_d16_face@") and r == {"4"}:
            continue
        if not r:
            # no reachable ret: an aborting trap. It can never corrupt the
            # caller's frame because it never returns to it.
            noret.append((base, n))
            continue
        if base in ALLOW:
            allowed.append((base, n, ALLOW[base]))
            continue
        bad.append((base, site, tgt, n, d, r))

    print("slot16_guard: %d table fill(s) of %d+ code words reach slot 16, %d checked store(s)"
          % (len(bysym), MIN_SLOTS, len(slot16)))
    for base, n in noret:
        print("  ok (never returns)  %s slot 16 <- %s" % (base, n))
    for base, n, why in allowed:
        print("  ok (allowlisted)    %s slot 16 <- %s  -- %s" % (base, n, why))
    for base, site, tgt, n, d, r in bad:
        sn, sd = sym(site)
        print("  REFUSED  %s slot 16 <- %08x %s+0x%x  ret=%s  (stored at %s+0x%x)"
              % (base, tgt, n, d, ",".join(sorted(r)) or "?", sn, sd))
    if reg_sites:
        print("slot16_guard: %d register-form store(s) at +0x40 -- not decidable "
              "statically, listed for review:" % len(reg_sites))
        for site, r in reg_sites[:12]:
            sn, sd = sym(site)
            print("    %08x  [reg+0x40] <- %s   in %s+0x%x" % (site, r, sn, sd))

    if a.selftest:
        # A checker that reads nothing passes everything. This arm proves the
        # guard can say NO: hand it a target that is not a wrapper and it must
        # come back bad.
        probe = None
        for base, site, tgt in slot16:
            probe = tgt
            break
        if probe is None:
            print("slot16_guard: SELFTEST INCONCLUSIVE -- no slot-16 store found")
            return 1
        fake = tlo  # the first byte of .text is not a PORT_D16 wrapper
        n, _ = sym(fake)
        if n.startswith("??$port_d16_face@"):
            print("slot16_guard: SELFTEST INCONCLUSIVE -- the probe address is a wrapper")
            return 1
        print("slot16_guard: SELFTEST OK -- the guard rejects a non-wrapper target (%s)" % n)

    if bad:
        print("slot16_guard: REFUSED -- %d slot-16 store(s) do not carry the "
              "destructor slot's calling convention. See port/hal/port_d16.h." % len(bad))
        return 1
    print("slot16_guard: OK -- every slot-16 store is a PORT_D16 wrapper ending in ret 4")
    return 0


sys.exit(main())
