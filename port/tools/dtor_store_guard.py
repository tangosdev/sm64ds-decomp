"""THE DESTRUCTOR VTABLE-STORE GUARD.

Run link100 wave 9b, lane DTORS-A.  This file is the AUTHORITY for the
PORT_HOST_ABI ruling that lets hal/dtor_forwarders_gen.cpp exist; tools/dtorfwd.py
is only its consumer.  Read the ruling in that generated file's banner, or
runs/link100/out/DTORS-A/ruling.md.

WHAT THE RULING PERMITS, in one paragraph.  192 rows on the port's link wall are
flat Itanium destructor names (`__ZN10daCamTag_cD1Ev`) whose owning src TU IS
compiled into walk_window and does not define them.  main's header declares the
destructor inline (`virtual ~daCamTag_c() {}`); mwccarm emits the body out of
line anyway because the class's vtable odr-uses it, and MSVC emits nothing,
because on the host the port's vtables are hand-written arrays and nothing
odr-uses the destructor.  A FORWARDER -- one generated TU that includes the real
header and defines the flat ROM name as a call to the destructor -- odr-uses it
and makes MSVC emit the body.  THE COST is that MSVC's destructor stores ITS OWN
vftable (`??_7Cls@@6B@`) into word 0 of the object before running the body, where
the cartridge's D1 stores `_ZTV<Cls>`.  MSVC folds the Itanium D1/D0 pair into
one scalar-deleting slot, so that table is NOT ROM-shaped: every virtual declared
after the destructor sits one slot early (memory note sm64ds-port-msvc-dtor-slot-shift).

THAT STORE IS INERT WHEN NOTHING DISPATCHES THROUGH THE OBJECT'S VPTR BETWEEN IT
AND THE BASE DESTRUCTOR CALL, which restores a ROM-shaped table as its own first
act.  That is a question about the cartridge, and this guard answers it from the
cartridge, per class, on every build:

  * disassemble the ROM body, skipping the literal pool, resolving pooled loads
    and branch targets through config/**/relocs.txt;
  * track the receiver symbolically (THIS, THIS+n, the vptr word, a word loaded
    through the vptr, a pooled _ZTV address, a pooled member offset);
  * the WINDOW opens at the first store of a pooled `_ZTV*` into word 0 of the
    receiver (or at offset 0 when the body makes no such store, because MSVC
    will store one there whether the cartridge did or not) and closes at the
    first `bl` to another class's D0/D1/D2 with the receiver UNCHANGED;
  * REFUSE the row if any virtual dispatch (`blx`/`bx`/`mov pc`/`ldr pc` through
    a word loaded from word 0 of the receiver) happens inside that window;
  * REFUSE the row if any call inside the window passes the receiver UNCHANGED to
    something that is not the base destructor, because such a callee can read
    word 0 through its own argument;
  * REFUSE the row if the window has no base destructor closing it, because then
    the wrong table is live when the body returns.

At the base of the ruling, measured once over all 192 rows: 0 refusals.  Every
window is closed by one of four base destructors (_ZN8dActor_cD2Ev 162,
_ZN11dScMgBase_cD2Ev 18, _ZN12dEnemyBase_cD2Ev 10, _ZN7fBase_cD2Ev 2), each of
which stores its own `_ZTV` into word 0 within its first five instructions, and
every call inside a window reaches a MEMBER subobject at `this+n`.  The evidence
is runs/link100/out/DTORS-A/dispatch_after_store.txt (the table),
streams_192.txt (every body printed) and callee_closure.txt (the callees walked
transitively: 14 of 15 have no indirect dispatch anywhere in their call closure,
and the one that does, __cxa_vec_cleanup, takes the element destructor as an
argument -- a pooled ROM function address at all 64 window call sites, never a
vtable word).

THE GUARD IS THE PART THAT KEEPS IT TRUE.  A class is admissible because of what
its ROM body does, and the ROM body is read at configure time, every build, from
the names hal/dtor_forwarders_gen.cpp actually defines.  A row that flips -- a
config record corrected, a span resized, a symbol re-keyed by the sync -- refuses
the build rather than shipping a wrong dispatch that no byte gate, no linker and
no linkage count can see.

    python dtor_store_guard.py <root>          the real check
    python dtor_store_guard.py --selftest      the fixture battery

Needs extracted/ (arm9_dec.bin and extracted/overlays), which every port tree
needs anyway, and capstone, which tools/facegen.py already requires in the same
guard wave.
"""
import os
import re
import sys

# ---------------------------------------------------------------------------
# the ROM images, per config module name


def _yaml_overlay_bases(path):
    bases, cur = {}, None
    for ln in open(path, encoding="utf-8", errors="replace"):
        m = re.match(r"\s*- id:\s*(\d+)", ln)
        if m:
            cur = int(m.group(1))
            continue
        m = re.match(r"\s*base_address:\s*(\d+)", ln)
        if m and cur is not None:
            bases[cur] = int(m.group(1))
            cur = None
    return bases


class Rom(object):
    """The cartridge, by config module name, with its symbol and reloc tables."""

    def __init__(self, root):
        self.root = os.path.abspath(root)
        self._img = {}
        self._facegen = None

    @property
    def facegen(self):
        if self._facegen is None:
            sys.path.insert(0, os.path.join(self.root, "port", "tools"))
            import facegen
            self._facegen = facegen
            self.by_addr = {}
            for name, ents in facegen.rom_index(self.root).items():
                for a, m in ents:
                    self.by_addr.setdefault(a, {}).setdefault(m, set()).add(
                        name)
        return self._facegen

    def image(self, mod):
        """(bytes, load address) for a config module name, or (None, reason)."""
        if mod in self._img:
            return self._img[mod]
        if mod == "arm9":
            p = os.path.join(self.root, "extracted", "arm9_dec.bin")
            # arm9.yaml's base_address, NOT 0x02000000.
            val = ((open(p, "rb").read(), 0x02004000) if os.path.exists(p)
                   else (None, "no extracted/arm9_dec.bin"))
        else:
            m = re.match(r"^arm9/overlays/ov(\d+)$", mod)
            if not m:
                val = (None, "no ROM image known for module %s" % mod)
            else:
                oid = int(m.group(1))
                y = os.path.join(self.root, "extracted", "dsd",
                                 "arm9_overlays", "overlays.yaml")
                p = os.path.join(self.root, "extracted", "overlays",
                                 "overlay_%04d.bin" % oid)
                bases = _yaml_overlay_bases(y) if os.path.exists(y) else {}
                if oid not in bases:
                    val = (None, "overlay %d has no base_address" % oid)
                elif not os.path.exists(p):
                    val = (None,
                           "no extracted/overlays/overlay_%04d.bin" % oid)
                else:
                    val = (open(p, "rb").read(), bases[oid])
        self._img[mod] = val
        return val

    def spell(self, addr, prefer=None):
        """The ROM's name for an address.

        OVERLAYS SHARE ADDRESSES.  ov000 and ov001 mount over the same memory,
        so 0x020aa6cc carries both `data_ov000_020aa6cc` and
        `func_ov001_020aa6cc`, and taking the first alphabetically reads a
        function as data.  The body's own module is the module every relocation
        checked here means, so a name from it wins.
        """
        self.facegen
        per_mod = self.by_addr.get(addr)
        if not per_mod:
            return "0x%08x" % addr
        if prefer and prefer in per_mod:
            return sorted(per_mod[prefer])[0]
        return sorted(n for names in per_mod.values() for n in names)[0]


# ---------------------------------------------------------------------------
# disassembly: pool skipped, pooled loads and branches resolved


def disasm(rom, mod, addr, size):
    """[(address, mnemonic, operands, resolved name or None)] or (None, why)."""
    from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
    md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
    md.detail = False
    img, base = rom.image(mod)
    if img is None:
        return None, base
    off = addr - base
    if off < 0 or off + size > len(img):
        return None, ("0x%08x+0x%x is outside %s's image (base 0x%08x, 0x%x "
                      "bytes)" % (addr, size, mod, base, len(img)))
    data = img[off:off + size]
    relocs = {}
    for frm, kind, to in rom.facegen.rom_relocs(rom.root).get(mod, ()):
        if addr <= frm < addr + size:
            relocs[frm] = (kind, to)
    pool = set()
    for ins in md.disasm(data, addr):
        m = re.match(r"^(\w+), \[pc, #(-?(?:0x)?[0-9a-fA-F]+)\]$", ins.op_str)
        if ins.mnemonic.startswith("ldr") and m:
            pool.add((ins.address + 8 + int(m.group(2), 0)) & 0xFFFFFFFC)
    out, at = [], addr
    while at < addr + size:
        if at in pool:
            w = int.from_bytes(data[at - addr:at - addr + 4], "little")
            named = (rom.spell(relocs[at][1], mod) if at in relocs
                     else rom.spell(w, mod))
            out.append((at, ".word", named, None))
            at += 4
            continue
        chunk = data[at - addr:at - addr + 4]
        got = list(md.disasm(chunk, at))
        if not got:
            out.append((at, ".word", "0x%08x"
                        % int.from_bytes(chunk, "little"), None))
            at += 4
            continue
        ins = got[0]
        pooled, text = None, ins.op_str
        m = re.match(r"^(\w+), \[pc, #(-?(?:0x)?[0-9a-fA-F]+)\]$", ins.op_str)
        if ins.mnemonic.startswith("ldr") and m:
            slot = (ins.address + 8 + int(m.group(2), 0)) & 0xFFFFFFFC
            if slot in relocs:
                pooled = rom.spell(relocs[slot][1], mod)
            else:
                w = 0
                if addr <= slot < addr + size:
                    w = int.from_bytes(data[slot - addr:slot - addr + 4],
                                       "little")
                pooled = rom.spell(w, mod)
            text = "%s, =%s" % (m.group(1), pooled)
        elif ins.mnemonic in ("b", "bl") and ins.op_str.startswith("#"):
            tgt = int(ins.op_str[1:], 0)
            pooled = (rom.spell(relocs[ins.address][1], mod)
                      if ins.address in relocs else rom.spell(tgt, mod))
            text = pooled
        out.append((ins.address, ins.mnemonic, text, pooled))
        at += 4
    return out, None


# ---------------------------------------------------------------------------
# the symbolic tracker

THIS = "THIS"     # the receiver, with a byte displacement
VPTR = "VPTR"     # a word loaded from word 0 of the receiver
SLOT = "SLOT"     # a word loaded through a VPTR register
ZTV = "ZTV"       # a pooled _ZTV* address
IMM = "IMM"       # a pooled plain integer (a member offset)
FN = "FN"         # a pooled ROM function address

_REG = r"(?:r\d+|sb|sl|fp|ip|lr|sp|pc)"


def analyse(lines, cls):
    """The measurement for one body: stores, vptr reads, dispatches, calls."""
    reg = {"r0": (THIS, 0)}
    stores, vptr_loads, dispatches, calls, vec = [], [], [], [], []
    base_at = None
    at0 = lines[0][0]

    for a, mn, ops, pooled in lines:
        off = a - at0
        if mn == ".word":
            continue
        m = re.match(r"^(%s), (%s)$" % (_REG, _REG), ops)
        if mn == "mov" and m:
            d, s = m.group(1), m.group(2)
            if d == "pc":
                v = reg.get(s)
                if v and v[0] in (SLOT, VPTR):
                    dispatches.append((off, "mov pc, %s" % s))
                continue
            reg[d] = reg.get(s)
            continue
        if mn == "mov" and re.match(r"^%s, #" % _REG, ops):
            reg[ops.split(",")[0]] = None
            continue
        m = re.match(r"^(%s), =(.+)$" % _REG, ops)
        if mn.startswith("ldr") and m and pooled is not None:
            d = m.group(1)
            if pooled.startswith("_ZTV"):
                reg[d] = (ZTV, pooled)
            elif re.match(r"^0x[0-9a-f]{8}$", pooled) \
                    and int(pooled, 16) < 0x01000000:
                reg[d] = (IMM, int(pooled, 16))
            elif re.match(r"^(_Z|func_)", pooled) or "Destructor" in pooled:
                reg[d] = (FN, pooled)
            else:
                reg[d] = None
            continue
        m = re.match(r"^(%s), \[(%s)(?:, #(-?(?:0x)?[0-9a-fA-F]+))?\]!?$"
                     % (_REG, _REG), ops)
        if mn.startswith("ldr") and m:
            d, b = m.group(1), m.group(2)
            n = int(m.group(3), 0) if m.group(3) else 0
            bv = reg.get(b)
            if bv and bv[0] == THIS and bv[1] + n == 0:
                reg[d] = (VPTR, 0)
                vptr_loads.append((off, "%s %s" % (mn, ops)))
                if d == "pc":
                    dispatches.append((off, "ldr pc, %s" % ops))
            elif bv and bv[0] == VPTR:
                reg[d] = (SLOT, n)
                if d == "pc":
                    dispatches.append((off, "ldr pc, %s" % ops))
            else:
                reg[d] = None
            continue
        if mn.startswith("str") and m:
            s, b = m.group(1), m.group(2)
            n = int(m.group(3), 0) if m.group(3) else 0
            bv, sv = reg.get(b), reg.get(s)
            if bv and bv[0] == THIS and bv[1] + n == 0 and sv \
                    and sv[0] == ZTV:
                stores.append((off, sv[1]))
            continue
        m = re.match(r"^(%s), (%s), (%s)$" % (_REG, _REG, _REG), ops)
        if mn in ("add", "sub") and m:
            d, b, o = m.group(1), m.group(2), m.group(3)
            bv, ov = reg.get(b), reg.get(o)
            if bv and bv[0] == THIS and ov and ov[0] == IMM:
                reg[d] = (THIS, bv[1] + (ov[1] if mn == "add" else -ov[1]))
            elif mn == "add" and ov and ov[0] == THIS and bv \
                    and bv[0] == IMM:
                reg[d] = (THIS, ov[1] + bv[1])
            else:
                reg[d] = None
            continue
        m = re.match(r"^(%s), (%s), #(-?(?:0x)?[0-9a-fA-F]+)$"
                     % (_REG, _REG), ops)
        if mn in ("add", "sub") and m:
            d, b = m.group(1), m.group(2)
            n = int(m.group(3), 0)
            bv = reg.get(b)
            reg[d] = ((THIS, bv[1] + (n if mn == "add" else -n))
                      if bv and bv[0] == THIS else None)
            continue
        if mn in ("blx", "bx") and re.match(r"^r\d+$", ops):
            v = reg.get(ops)
            if v and v[0] == SLOT:
                dispatches.append((off, "%s %s (vtable slot +0x%x)"
                                   % (mn, ops, v[1])))
            elif v and v[0] == VPTR:
                dispatches.append((off, "%s %s (the vptr word itself)"
                                   % (mn, ops)))
            else:
                dispatches.append((off, "%s %s (indirect, source not tracked)"
                                   % (mn, ops)))
            # an indirect call follows the same convention as a direct one
            reg = {k: v for k, v in reg.items()
                   if k in ("r4", "r5", "r6", "r7", "r8", "sb", "sl", "fp")}
            continue
        if mn == "bl" and pooled is not None:
            r0 = reg.get("r0")
            shape = ("this" if r0 and r0[0] == THIS and r0[1] == 0
                     else ("this+0x%x" % r0[1] if r0 and r0[0] == THIS
                           else "?"))
            calls.append((off, pooled, shape))
            if pooled == "__cxa_vec_cleanup":
                v = reg.get("r3")
                vec.append((off, "%s %s" % (v[0], v[1]) if v else "UNKNOWN"))
            if base_at is None and shape == "this" \
                    and re.match(r"^_ZN.*D[012]Ev$", pooled) \
                    and not _same_class(pooled, cls):
                base_at = off
            # the callee keeps the callee-saved registers and nothing else
            reg = {k: v for k, v in reg.items()
                   if k in ("r4", "r5", "r6", "r7", "r8", "sb", "sl", "fp")}
            continue
        m = re.match(r"^(%s)," % _REG, ops)
        if m and mn not in ("cmp", "cmn", "tst", "teq", "b", "push", "stm",
                            "stmdb", "str", "strb", "strh", "strd"):
            reg[m.group(1)] = None
    return {"stores": stores, "vptr_loads": vptr_loads,
            "dispatches": dispatches, "calls": calls, "vec_cleanup": vec,
            "base_at": base_at}


def _same_class(sym, cls):
    m = re.match(r"^_ZN(\d+)(.+)D[012]Ev$", sym)
    return bool(m) and m.group(2)[:int(m.group(1))] == cls


# ---------------------------------------------------------------------------
# the rule


def verdict(info):
    """("ok", note) when a forwarder is admissible, else ("refuse", reason)."""
    store_off = info["stores"][0][0] if info["stores"] else 0
    # with no base restore the window never closes, so the whole body is in it
    end = info["base_at"] if info["base_at"] is not None else 1 << 30
    hits = [(o, t) for o, t in info["dispatches"] if store_off <= o < end]
    if hits:
        return "refuse", ("virtual dispatch inside the window: "
                          + "; ".join("+0x%x %s" % (o, t) for o, t in hits))
    escaped = [(o, c) for o, c, s in info["calls"]
               if store_off <= o < end and s == "this"]
    if escaped:
        return "refuse", ("the receiver reaches %s unchanged inside the "
                          "window, which can read word 0 through its own "
                          "argument" % ", ".join("+0x%x %s" % (o, c)
                                                 for o, c in escaped))
    unknown = [(o, c) for o, c, s in info["calls"]
               if store_off <= o < end and s == "?"]
    if unknown:
        return "refuse", ("the receiver shape at %s could not be tracked, so "
                          "it is not known whether the outer object escapes"
                          % ", ".join("+0x%x %s" % (o, c)
                                      for o, c in unknown))
    if info["base_at"] is None:
        return "refuse", ("no base destructor is called with the receiver "
                          "unchanged, so nothing restores a ROM-shaped table "
                          "before the body returns")
    end = info["base_at"]
    return "ok", ("window +0x%x..+0x%x, %d call(s) in it, all to a member "
                  "subobject; base restore %s"
                  % (store_off, end, len([1 for o, _c, _s in info["calls"]
                                          if store_off <= o < end]),
                     [c for o, c, _s in info["calls"] if o == end][0]))


def check_name(rom, name):
    """("ok"/"refuse", reason) for one flat Itanium destructor name."""
    ident = name[1:] if name.startswith("__Z") else name
    m = re.match(r"^_ZN(\d+)(.+)D([012])Ev$", ident)
    if not m:
        return "refuse", "%s is not a flat Itanium destructor name" % name
    cls = m.group(2)[:int(m.group(1))]
    span, why = rom.facegen._span_of(rom.root, ident)
    if span is None:
        return "refuse", why
    addr, mod, size = span
    lines, why = disasm(rom, mod, addr, size)
    if lines is None:
        return "refuse", why
    v, note = verdict(analyse(lines, cls))
    return v, "%s 0x%08x %s: %s" % (ident, addr, mod, note)


# A DEFINITION, not a declaration: the line must not end in a semicolon.
# D2 as well as D0/D1 since run link100 wave 9c, lane DTORS2: a class whose
# destructor is defined OUT OF LINE by its own per-function structor TU can have
# its base-object destructor asked for by name too (ModelFamily_Dtors_HostCopy.c
# calls _ZN9ModelAnimD2Ev from _ZN10ModelAnim2D0Ev), and MSVC folds D1 and D2
# into one symbol for a class with no virtual base.  check_name has always
# accepted D2; only this reader did not, so a D2 forwarder would have gone
# UNGUARDED.
DEF_RE = re.compile(r'^extern "C" void (_ZN\w+D[012]Ev)\([^;\n]*\)[ \t]*$',
                    re.M)


def names_in(genfile):
    """The flat ROM names a generated forwarder file DEFINES."""
    if not os.path.exists(genfile):
        return []
    return DEF_RE.findall(open(genfile, encoding="utf-8").read())


def generated_files(root):
    """Every generated forwarder file in the tree, in a stable order.

    There is more than one because two of the headers involved cannot share a
    translation unit; tools/dtorfwd.py's SPLITS block carries the measurement.
    The guard reads them ALL, and reads what they DEFINE rather than an intent
    list, so neither a hand edit nor a new split file escapes it.
    """
    import glob
    pat = os.path.join(root, "port", "hal", "dtor_forwarders_gen*.cpp")
    return sorted(glob.glob(pat))


# ---------------------------------------------------------------------------
# the fixture battery


def _fixture(text):
    """[(addr, mnemonic, operands, pooled)] from a compact test spelling."""
    out = []
    lines = [ln.strip() for ln in text.strip().splitlines() if ln.strip()]
    for i, ln in enumerate(lines):
        parts = ln.split(None, 1)
        mn = parts[0]
        ops = parts[1] if len(parts) > 1 else ""
        pooled = None
        m = re.search(r"=(\S+)$", ops)
        if m:
            pooled = m.group(1)
        elif mn in ("b", "bl"):
            pooled = ops
        out.append((0x02000000 + 4 * i, mn, ops, pooled))
    return out


def selftest():
    fails = []

    def case(label, text, cls, want, needle=""):
        info = analyse(_fixture(text), cls)
        got, why = verdict(info)
        if got != want or (needle and needle not in why):
            fails.append("%s: wanted %s%s, got %s (%s)"
                         % (label, want, "/" + needle if needle else "",
                            got, why))

    # 1. the shape the ruling is about: store, base restore, nothing between.
    case("trivial daCamTag_c shape", """
        push {r4, lr}
        ldr r1, =_ZTV10daCamTag_c
        mov r4, r0
        str r1, [r4]
        bl _ZN8dActor_cD2Ev
        mov r0, r4
        pop {r4, lr}
        bx lr
    """, "daCamTag_c", "ok", "window +0xc..+0x10")

    # 2. members destroyed in the window, each at this+n: still admissible.
    case("member teardown in the window", """
        push {r4, lr}
        ldr r1, =0x000000d4
        mov r4, r0
        ldr ip, =_ZTV7daBar_c
        add r0, r4, r1
        str ip, [r4]
        bl _ZN5ModelD1Ev
        mov r0, r4
        bl _ZN8dActor_cD2Ev
        pop {r4, lr}
        bx lr
    """, "daBar_c", "ok", "1 call(s) in it")

    # 3. THE REFUSAL THE GUARD EXISTS FOR: a virtual call after the store.
    case("virtual dispatch after the store", """
        push {r4, lr}
        ldr r1, =_ZTV7daBar_c
        mov r4, r0
        str r1, [r4]
        ldr r2, [r4]
        ldr r3, [r2, #0x18]
        mov r0, r4
        blx r3
        mov r0, r4
        bl _ZN8dActor_cD2Ev
        pop {r4, lr}
        bx lr
    """, "daBar_c", "refuse", "virtual dispatch inside the window")

    # 4. the receiver escapes unchanged to something that is not a base dtor.
    case("receiver escapes in the window", """
        push {r4, lr}
        ldr r1, =_ZTV7daBar_c
        mov r4, r0
        str r1, [r4]
        mov r0, r4
        bl func_02012345
        mov r0, r4
        bl _ZN8dActor_cD2Ev
        pop {r4, lr}
        bx lr
    """, "daBar_c", "refuse", "func_02012345 unchanged")

    # 5. no base destructor at all: nothing restores a ROM-shaped table.
    case("no base restore", """
        push {r4, lr}
        ldr r1, =_ZTV7daBar_c
        mov r4, r0
        str r1, [r4]
        pop {r4, lr}
        bx lr
    """, "daBar_c", "refuse", "no base destructor")

    # 6. the class's OWN D2 does not close a window: the table it would store
    #    is the same non-ROM-shaped one.  The receiver reaching it unchanged is
    #    then an escape, and the row refuses rather than reading as closed.
    case("own D2 does not close the window", """
        push {r4, lr}
        ldr r1, =_ZTV7daBar_c
        mov r4, r0
        str r1, [r4]
        mov r0, r4
        bl _ZN7daBar_cD2Ev
        pop {r4, lr}
        bx lr
    """, "daBar_c", "refuse", "_ZN7daBar_cD2Ev unchanged")

    # 7. a body with NO vtable store is measured from offset 0, because MSVC
    #    stores one there whether the cartridge did or not.
    case("no ROM store, dispatch before the base call", """
        push {r4, lr}
        mov r4, r0
        ldr r2, [r4]
        ldr r3, [r2, #4]
        blx r3
        mov r0, r4
        bl _ZN8dActor_cD2Ev
        pop {r4, lr}
        bx lr
    """, "daBar_c", "refuse", "virtual dispatch inside the window")

    # 8. a dispatch AFTER the base restore is not this ruling's business: the
    #    object already carries the base's ROM table again.
    case("dispatch after the base restore", """
        push {r4, lr}
        ldr r1, =_ZTV7daBar_c
        mov r4, r0
        str r1, [r4]
        mov r0, r4
        bl _ZN8dActor_cD2Ev
        ldr r2, [r4]
        ldr r3, [r2, #4]
        blx r3
        pop {r4, lr}
        bx lr
    """, "daBar_c", "ok")

    # 9. an untracked receiver is a refusal, never an assumption.
    case("untracked receiver shape", """
        push {r4, lr}
        ldr r1, =_ZTV7daBar_c
        mov r4, r0
        str r1, [r4]
        ldr r0, [r4, #0x20]
        bl _ZN5ModelD1Ev
        mov r0, r4
        bl _ZN8dActor_cD2Ev
        pop {r4, lr}
        bx lr
    """, "daBar_c", "refuse", "could not be tracked")

    # 10. the name filter: only flat Itanium destructors are in scope.
    class _NoRom(object):
        root = "."
    got, why = check_name(_NoRom(), "?InitResources@daCamTag_c@@UAEHXZ")
    if got != "refuse" or "not a flat Itanium destructor name" not in why:
        fails.append("name filter: got %s (%s)" % (got, why))

    # 11. the generated-file reader takes definitions and not declarations.
    import tempfile
    fd, p = tempfile.mkstemp(suffix=".cpp")
    os.close(fd)
    open(p, "w", encoding="utf-8").write(
        'extern "C" void _ZN10daCamTag_cD1Ev(void *self);\n'
        'extern "C" void _ZN10daCamTag_cD1Ev(void *self)\n{ }\n'
        'extern "C" void _ZN10daCamTag_cD0Ev(void *self)\n{ }\n'
        '/* extern "C" void _ZN7daBar_cD1Ev(void *self) */\n')
    got = names_in(p)
    os.unlink(p)
    if got != ["_ZN10daCamTag_cD1Ev", "_ZN10daCamTag_cD0Ev"]:
        fails.append("names_in: got %r" % (got,))

    # 12. wave 9c: the reader takes a D2 definition too, so an out-of-line
    # premise row cannot be emitted past this guard unread.
    fd, p = tempfile.mkstemp(suffix=".cpp")
    os.close(fd)
    open(p, "w", encoding="utf-8").write(
        'extern "C" void _ZN9ModelAnimD2Ev(void *self)\n{ }\n'
        'extern "C" void _ZN9ModelAnimD3Ev(void *self)\n{ }\n')
    got = names_in(p)
    os.unlink(p)
    if got != ["_ZN9ModelAnimD2Ev"]:
        fails.append("names_in D2: got %r" % (got,))

    # 13. wave 9c: the ROM rule itself is asked of a D2 body exactly as it is of
    # a D1, and it is the SAME rule -- there is no second, weaker path for the
    # new premise.  ModelAnim's cartridge D2 stores _ZTV9ModelAnim, tears down a
    # member subobject at a displacement and closes on a base destructor.
    case("out-of-line premise, a D2 body", """
        push {r4, lr}
        ldr r2, =_ZTV9ModelAnim
        mov r4, r0
        str r2, [r4]
        add r0, r4, #0x50
        bl _ZN9AnimationD2Ev
        mov r0, r4
        bl _ZN5ModelD2Ev
        pop {r4, lr}
        bx lr
    """, "ModelAnim", "ok", "base restore")

    # 14. and a D2 body that dispatches through word 0 after the store is
    # refused on the same evidence as a D1 that does.
    case("out-of-line premise, a D2 that dispatches", """
        push {r4, lr}
        ldr r2, =_ZTV9ModelAnim
        mov r4, r0
        str r2, [r4]
        ldr r3, [r4]
        ldr r3, [r3, #0x18]
        blx r3
        mov r0, r4
        bl _ZN5ModelD2Ev
        pop {r4, lr}
        bx lr
    """, "ModelAnim", "refuse", "virtual dispatch inside the window")

    if fails:
        for f in fails:
            print("dtor_store_guard SELFTEST FAIL: %s" % f)
        return 1
    print("dtor_store_guard selftest OK -- 14 cases")
    return 0


def main():
    if "--selftest" in sys.argv:
        return selftest()
    root = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else ".")
    files = generated_files(root)
    names = []
    for f in files:
        names += names_in(f)
    if not names:
        print("dtor_store_guard OK -- hal/dtor_forwarders_gen*.cpp defines no "
              "forwarder (%d file(s) read)" % len(files))
        return 0
    dup = sorted(n for n in set(names) if names.count(n) > 1)
    if dup:
        print("dtor_store_guard REFUSES: %d name(s) are defined in more than "
              "one generated file, which is a duplicate symbol waiting to "
              "happen: %s" % (len(dup), ", ".join(dup[:10])))
        return 1
    rom = Rom(root)
    bad = []
    for n in names:
        v, why = check_name(rom, n)
        if v != "ok":
            bad.append((n, why))
    if bad:
        print("dtor_store_guard REFUSES %d of %d forwarder(s) over %d "
              "hal/dtor_forwarders_gen*.cpp file(s)."
              % (len(bad), len(names), len(files)))
        print("Each row below is a class whose ROM destructor body no longer "
              "satisfies the PORT_HOST_ABI ruling this file's header states, "
              "so MSVC's own vftable would be live on a dispatch. Drop the "
              "class from tools/dtorfwd.py's list and regenerate; do not "
              "weaken the rule.")
        for n, why in bad:
            print("  %s: %s" % (n, why))
        return 1
    print("dtor_store_guard OK -- %d forwarder(s) over %d file(s), every ROM "
          "destructor body still stores its vtable with no dispatch before the "
          "base restore" % (len(names), len(files)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
