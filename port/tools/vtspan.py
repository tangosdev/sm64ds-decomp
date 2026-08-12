"""vtspan -- size every hosted vtable against its true ROM span.

WHY THIS EXISTS
    The port fills its actor vtables at runtime into host arrays whose LENGTH
    is written by hand, and a host array shorter than the ROM table it stands
    in for is a live wild call: a dispatch of slot N past the end reads
    whatever the linker put next.  Slot 21 is OnGroundPounded, at byte offset
    0x54, and a [20] array is 0x50 bytes -- so a player ground-pounding an
    ordinary sign called the next symbol in the image.

HOW THE LENGTH IS DERIVED, AND WHY THE OBVIOUS WAY IS WRONG
    The obvious bound is the next symbol in the module's symbols.txt.  It is
    NOT reliable: where dsd could not tell a vtable's interior from the start
    of a new object it emitted an "ambiguous" data symbol INSIDE the table,
    and the next-symbol delta then truncates below the real length.
    data_ov018_02113a74 reads as 18 words that way and is 31; _ZTV9TowerStep
    reads as 15 and is 32.  A sweep that trusts the symbol bound calls both of
    those correct.

    So the length comes from the RELOC RUN: consecutive relocated words whose
    targets land in a code section.  That run does not stop on its own either,
    because the ROM packs a class's pointer-to-member pairs and the next
    class's vtable immediately behind, and those are relocated code pointers
    too.  Past slot 30 -- the end of Actor's own virtual list, which every
    Actor-derived table has -- two ROM-shaped terminators close it:

      * a dsd symbol STARTING at this word means a new object starts here;
        past slot 30 that bound is trustworthy, and it is what separates a
        class's own Kill from the PMF pairs behind it;
      * a word whose SUCCESSOR is Actor::BeforeInitResources is slot 0 of the
        next class's table.

    Both halves are needed.  Run alone overruns into the PMF table; symbol
    bound alone truncates.  The intersection is the ROM's answer.

THE SHAPES THIS PRINTS
    31 slots  a plain Actor subclass.  Slot 30 is
              Actor::OnAimedAtWithEggReturnVec (0x020100dc) and the table ends.
    32 slots  a Platform subclass.  Slot 31 is Platform::Kill (ov002
              0x020ee55c) or the class's own override of it.
    more      a deeper hierarchy of its own (Bully and BigBully are 37).

    "slot 30 is 0x020100dc and slot 31 is a real function" is the Platform
    signature; a table whose word past slot 30 is data is 31 slots and done.

USAGE
    python port/tools/vtspan.py <repo-root> --sweep
        every function-pointer table defined under port/, host size vs ROM
        span, with the undersized ones called out.  Exits 1 if any is short.
    python port/tools/vtspan.py <repo-root> --seats <dumpbin-disasm-dir>
        does the thunk seated in slot N run the body slot N is supposed to
        run?  Catches SAME-ARITY WRONG-BODY SEATS, which the sizing sweep and
        abicheck are both blind to.  See the --seats section below.  Exits 1
        if any seat is wrong, 2 if nothing could be checked.

        THE DISASSEMBLY MUST BE `dumpbin /disasm:nobytes`.  Plain /disasm puts
        the encoded bytes between the address and the mnemonic, nothing parses,
        and the run finds zero stores -- so a zero-checked run is a hard
        FAILURE here, not a pass.  Generate it per object:
            dumpbin /disasm:nobytes build/port/CMakeFiles/*/hal/*.obj
    python port/tools/vtspan.py <repo-root> <symbol> [<symbol>...]
        slot-by-slot dump of one table, each entry resolved to a name and
        cross-checked against the RAW ROM image word (never the dsd overlay
        export, which goes stale against config re-addressings).

    Symbols that carry no _ZTV name work too -- two of the seven ov015
    platform tables are data_ov015_02114360 and data_ov015_021147e8, so a
    _ZTV-only scan cannot see them.
"""
import collections
import pathlib
import re
import sys

CFG = "config/arm9"

# Actor's own virtual list, arm9, as every Actor-derived table carries it.
ACTOR_TAIL = {
    20: (0x0201014C, "Actor::Virtual50"),
    21: (0x02010148, "Actor::OnGroundPounded"),
    22: (0x02010144, "Actor::OnAttacked1"),
    23: (0x02010140, "Actor::OnAttacked2"),
    24: (0x0201013C, "Actor::OnKicked"),
    25: (0x02010138, "Actor::OnPushed"),
    26: (0x02010134, "Actor::OnHitByCannonBlastedChar"),
    27: (0x02010130, "Actor::OnHitByMegaChar"),
    28: (0x0201012C, "Actor::OnHitFromUnderneath"),
    29: (0x02010124, "Actor::OnAimedAtWithEgg"),
    30: (0x020100DC, "Actor::OnAimedAtWithEggReturnVec"),
}
PLATFORM_KILL = 0x020EE55C          # ov002, slot 31 of every Platform table
ACTOR_BINIT = 0x02011268            # Actor::BeforeInitResources, every [1]


def load_sections(path):
    out = []
    for ln in path.read_text().splitlines():
        m = re.match(r"\s+(\.\w+)\s+start:0x([0-9a-f]+) end:0x([0-9a-f]+) "
                     r"kind:(\w+)", ln)
        if m:
            out.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16),
                        m.group(4)))
    return out


def load_syms(path):
    by_addr, by_name = {}, {}
    for ln in path.read_text().splitlines():
        m = re.match(r"(\S+) kind:(\S+) addr:0x([0-9a-f]+)", ln)
        if m:
            name, addr = m.group(1), int(m.group(3), 16)
            by_addr.setdefault(addr, []).append(name)
            by_name[name] = addr
    return by_addr, by_name


def load_relocs(path):
    out = {}
    for ln in path.read_text().splitlines():
        m = re.match(r"from:0x([0-9a-f]+) kind:(\S+) to:0x([0-9a-f]+) "
                     r"module:(\S+)", ln)
        if m:
            out[int(m.group(1), 16)] = (m.group(2), int(m.group(3), 16),
                                        m.group(4))
    return out


class Rom:
    def __init__(self, root):
        self.root = pathlib.Path(root)
        self._img = {}
        self.mod = {}
        self._load("main", self.root / CFG)
        for d in sorted((self.root / CFG / "overlays").iterdir()):
            if d.is_dir() and re.fullmatch(r"ov\d+", d.name):
                self._load("overlay(%d)" % int(d.name[2:]), d)

    def _load(self, key, d):
        by_addr, by_name = load_syms(d / "symbols.txt")
        self.mod[key] = {
            "sections": load_sections(d / "delinks.txt"),
            "by_addr": by_addr,
            "by_name": by_name,
            "relocs": load_relocs(d / "relocs.txt"),
            "sorted": sorted(by_addr),
        }

    def modules_of(self, module):
        """dsd writes `main`, `overlay(2)` and, where it could not choose,
        `overlays(2,7)`. All three have to resolve, or a table with one
        ambiguous slot reads as if it ended there -- which is exactly how
        TowerStep's Platform::Kill went missing."""
        if module in self.mod:
            return [module]
        m = re.match(r"overlays\(([\d,\s]+)\)", module)
        if m:
            return [k for k in ("overlay(%d)" % int(x)
                                for x in m.group(1).split(",")) if k in self.mod]
        return []

    def is_code(self, addr, module):
        """True as soon as ANY candidate module puts addr in a code section.
        Taking the first candidate's word for it reads ov015's own Behavior as
        ov000 data and ends that table at slot 6."""
        seen = None
        for k in self.modules_of(module):
            for _, s, e, kind in self.mod[k]["sections"]:
                if s <= addr < e:
                    if kind == "code":
                        return True
                    seen = False
        return seen

    def name_of(self, addr, module):
        cands = self.modules_of(module)
        if not cands:
            return "?"
        if len(cands) > 1:
            cands.sort(key=lambda k: 0 if self.is_code(addr, k) else 1)
        for k in cands:
            names = self.mod[k]["by_addr"].get(addr)
            if names:
                return "/".join(names)
        m = self.mod[cands[0]]
        lo, hi = 0, len(m["sorted"])
        while lo < hi:
            mid = (lo + hi) // 2
            if m["sorted"][mid] <= addr:
                lo = mid + 1
            else:
                hi = mid
        if lo:
            base = m["sorted"][lo - 1]
            return "%s+0x%x" % ("/".join(m["by_addr"][base]), addr - base)
        return "?"

    def find(self, sym):
        for k, m in self.mod.items():
            if sym in m["by_name"]:
                yield k, m["by_name"][sym]

    def image(self, module):
        if module not in self._img:
            if module == "main":
                base = 0x02004000
                data = (self.root / "extracted/arm9_dec.bin").read_bytes()
            else:
                ovid = int(re.match(r"overlay\((\d+)\)", module).group(1))
                ytxt = (self.root /
                        "extracted/dsd/arm9_overlays/overlays.yaml").read_text()
                blk = re.search(rf"^  - id: {ovid}$\n(.*?)(?=^  - id: |\Z)",
                                ytxt, re.S | re.M)
                base = int(re.search(r"^    base_address: (\d+)$",
                                     blk.group(1), re.M).group(1))
                data = (self.root / ("extracted/overlays/overlay_%04d.bin"
                                     % ovid)).read_bytes()
            self._img[module] = (base, data)
        return self._img[module]

    def word(self, module, addr):
        base, data = self.image(module)
        off = addr - base
        if off < 0 or off + 4 > len(data):
            return None
        return int.from_bytes(data[off:off + 4], "little")

    def span(self, module, addr):
        """(ROM length in words, symbol-bound length in words)."""
        m = self.mod[module]
        rel = m["relocs"]
        n = 0
        while True:
            e = rel.get(addr + 4 * n)
            if not e or e[0] != "load" or not self.is_code(e[1], e[2]):
                break
            if n >= 31:
                # THE SYMBOL TERMINATOR HAS THE SAME DISEASE THE SYMBOL BOUND
                # HAS, and it took a second reviewer to find it: dsd sometimes
                # emits a spurious data symbol exactly AT slot 31, and the
                # terminator then under-reads a 32-slot Platform table as 31 --
                # precisely the truncation this file's header blames the bound
                # for. _ZTV11PyramidLift (ov025 0x021139d4) and
                # data_ov026_02113ba4 both read as 31 that way; neither is
                # hosted today, so it was latent, but the sweep would have
                # cleared a [31] host array for either the day it was.
                #
                # Discriminator: what the word actually IS. Platform::Kill at
                # index 31 means the table extends there, symbol or no symbol.
                # It cannot false-positive the other way -- _ZTV17BowserPuzzle
                # Piece stays 31 because its index-31 word is a class function
                # followed by a zero adjustment word, a PMF pair, not Kill.
                #
                # RESIDUAL GAP, stated rather than papered over: a class that
                # OVERRIDES Kill and also has a spurious symbol at 31 would
                # still under-read. None exists in the tree today (every
                # Kill-overriding table here has its next symbol at 32 or
                # later), and closing it properly needs the typeinfo chain --
                # ask whether the class descends from dBgActor_c -- rather than
                # one address compare.
                here = rel.get(addr + 4 * n)
                kill_here = here and here[1] == PLATFORM_KILL
                if (addr + 4 * n) in m["by_addr"] and not kill_here:
                    break
                nx = rel.get(addr + 4 * (n + 1))
                if nx and nx[1] == ACTOR_BINIT:
                    break
            n += 1
        nxt = next((a for a in m["sorted"] if a > addr), None)
        return n, ((nxt - addr) // 4 if nxt else None)

    def dump(self, module, addr, sym=""):
        m = self.mod[module]
        run, bound = self.span(module, addr)
        shape = {31: "plain Actor", 32: "Platform subclass"}.get(run, "own")
        print("=== %s  %s 0x%08x   ROM %d slots (%s), symbol bound %s"
              % (sym, module, addr, run, shape, bound))
        for i in range(run + 2):
            a = addr + 4 * i
            e = m["relocs"].get(a)
            raw = self.word(module, a)
            marks = []
            if a in m["by_addr"]:
                marks.append("<dsd symbol %s>" % "/".join(m["by_addr"][a]))
            if i == run:
                marks.append("<PAST THE END>")
            if not e:
                print("   [%2d] raw %08x   (no reloc)  %s"
                      % (i, raw or 0, " ".join(marks)))
                continue
            note = ""
            if i in ACTOR_TAIL and e[1] == ACTOR_TAIL[i][0]:
                note = "(shared %s)" % ACTOR_TAIL[i][1]
            elif i == 31 and e[1] == PLATFORM_KILL:
                note = "(shared Platform::Kill)"
            elif i in ACTOR_TAIL or i == 31:
                note = "OVERRIDE"
            bad = "" if raw == e[1] else "  IMAGE DISAGREES 0x%08x" % (raw or 0)
            print("   [%2d] %08x %-12s %-46s %-6s %-9s %s%s"
                  % (i, e[1], e[2], self.name_of(e[1], e[2]),
                     "code" if self.is_code(e[1], e[2]) else "DATA",
                     note, " ".join(marks), bad))
        print()


# ANY element type, ANY constant-expression length.
#
# The first version of this pattern wanted `void *` or `int` and a decimal
# literal, so it could not see
#
#     unsigned char data_ov079_02127fb8[31 * 4];
#
# which is BILL_BLASTER's table: 32 slots in the ROM, one short on the host,
# slot 31 a real override (ov079 0x02126e58) that nothing ever wrote. The sweep
# reported zero short tables with that array sitting in the tree, so "zero" was
# a statement about the pattern's field of view rather than about the tree.
#
# So: any type, and the length is EVALUATED rather than parsed -- 31 * 4, 0x20
# and 8 * SLOTS all resolve. A byte array divides back by its element size.
DEF = re.compile(
    r"^\s*(?:extern\s+\"C\"\s*\{?\s*)?(?:extern\s+)?"
    r"(?P<type>(?:unsigned\s+|signed\s+|const\s+|volatile\s+|struct\s+)*"
    r"[A-Za-z_]\w*(?:\s*\*)*)\s*"
    r"(?P<name>(?:_ZTV|data_)\w+)\s*\[(?P<len>[^\]]*)\]")

ELEM_BYTES = {"char": 1, "unsigned char": 1, "signed char": 1, "u8": 1,
              "short": 2, "unsigned short": 2, "u16": 2}


def decl_slots(type_text, len_text):
    """Slot count from a declaration, or None if the length is not constant."""
    try:
        n = int(eval(len_text, {"__builtins__": {}}, {}))
    except Exception:
        return None
    t = " ".join(type_text.split())
    if t.endswith("*"):
        return n                              # void *x[N] is N pointers
    # otherwise the declaration counts ELEMENTS, and a slot is four bytes:
    # int[18] is 18 slots, unsigned char[31 * 4] is 31.
    return n * ELEM_BYTES.get(t, 4) // 4


def sweep(rom, root):
    """Every function-pointer table port/ defines, host size vs ROM span."""
    found = {}
    for p in sorted((pathlib.Path(root) / "port").rglob("*.cpp")):
        for i, ln in enumerate(p.read_text(errors="replace").splitlines(), 1):
            # `extern int X[];` is a declaration and is rightly skipped.
            # `extern "C" { int X[20]; }` is a DEFINITION, and skipping that
            # hid _ZTV15IceSlideManager -- 20 host words against 31 in the ROM
            # -- for as long as this sweep has existed.
            if (re.match(r"\s*extern\s", ln) and '"C"' not in ln
                    and "=" not in ln):
                continue
            m = DEF.match(ln)
            if m:
                n = decl_slots(m.group("type"), m.group("len"))
                if n is not None:
                    found.setdefault(m.group("name"),
                                     (str(p.relative_to(root)), i, n))
    short = []
    print("%-34s %-5s %-5s %-6s %s"
          % ("array", "host", "ROM", "bound", "defined at"))
    for name, (where, line, host) in sorted(found.items()):
        for module, addr in rom.find(name):
            run, bound = rom.span(module, addr)
            if run < 4:
                continue                      # not a function-pointer table
            flag = ""
            if host < run:
                flag = "  <== SHORT BY %d" % (run - host)
                short.append((name, host, run, where))
            print("%-34s %-5d %-5d %-6s %s:%d%s"
                  % (name, host, run, bound, where, line, flag))
    print("\n%d table(s) shorter than the ROM span" % len(short))
    for name, host, run, where in short:
        print("  %-34s host %2d  ROM %2d   %s" % (name, host, run, where))
    return 1 if short else 0


# ============================================================================
# --seats: does the thunk in slot N run the body slot N is supposed to run?
# ============================================================================
#
# THE DEFECT CLASS THIS CATCHES, AND WHY NOTHING ELSE DOES
#     A fill can seat the right ARITY in the wrong SLOT. abicheck reads pop
#     sizes out of the emitted code, and a same-arity swap emits identical pop
#     sizes, so its report is byte-identical before and after the fix. The
#     sizing sweep above is blind to it too: the array is the right length and
#     every entry is a real function.
#
#     Real instances, more than one: a table seating Actor::Virtual50 (returns
#     1) in slot 29, where the ROM has Actor::OnAimedAtWithEgg (returns
#     0x14000, the egg auto-aim lock-on radius in 20.12 -- 20.0 units). Both
#     are `int f(Actor *)`, so the frame contract is fine and Yoshi's auto-aim
#     simply has no lock-on radius on that class.
#
# WHERE THE SLOT NUMBERS COME FROM
#     Not from a header and not from a comment. Every Actor-layout table in the
#     ROM votes: a table is Actor-layout when its word [1] is
#     Actor::BeforeInitResources, and across the 276 of them each shared arm9
#     body occupies exactly ONE index, unanimously. That agreement is the
#     authority.
#
#     It has to be, because the matched sources disagree with it. Every
#     src/_ZN5Actor* body from slot 18 up carries a header comment exactly one
#     slot LOW -- Virtual50 says 19, OnAimedAtWithEgg says 28 -- while the
#     bodies at slots 0..15 all say the right number. The boundary is the
#     destructor pair: those comments are in MSVC numbering, where Itanium's
#     D1/D0 fold into one deleting-destructor slot and everything past it
#     shifts down by one. They are not wrong in their own frame. They are wrong
#     for a ROM-shaped array, and seating a port vtable from them reproduces
#     the swap exactly.
#
# SCOPE, HONESTLY
#     This checks the SHARED arm9/ov002 bodies only -- the thirty functions
#     every Actor table draws on. A slot holding a class's own overlay body is
#     reported as unchecked rather than guessed at.
#
#     It is table-agnostic on purpose. Most port fills write through a
#     `void **vt` parameter, so the table base is in a register and one
#     object's disassembly does not say which class it belongs to. It does not
#     need to: a thunk that calls Actor::Virtual50 seated at slot 29 is a
#     defect whichever table it is in.
#
#     One inference NOT made here, because it is false: that a symbol reached
#     only by kind:load relocations is unreachable. Virtual dispatch IS a load
#     followed by an indirect call, so reloc kinds say nothing about
#     reachability in either direction.

FUNCHEAD = re.compile(r"^(\S+)(?:\s+\(.*\))?:\s*$")
INSN = re.compile(r"^\s+([0-9A-F]+):\s+(\S+)\s*(.*?)\s*$")
VTSTORE = re.compile(
    r"dword ptr \[(?P<tab>[A-Za-z_?@$][\w?@$]*|e[a-z]{2})"
    r"(?:\+(?P<off>[0-9A-F]+)h)?\]\s*,\s*offset (?P<sym>\S+)")


def canonical_slots(rom):
    """address -> slot index, for bodies every Actor-layout table agrees on."""
    votes = collections.defaultdict(collections.Counter)
    for mod, m in rom.mod.items():
        rel = m["relocs"]
        for addr, (kind, to, tmod) in rel.items():
            if kind != "load":
                continue
            nx = rel.get(addr + 4)
            if not (nx and nx[1] == ACTOR_BINIT):
                continue                    # addr is not a table's slot 0
            run, _ = rom.span(mod, addr)
            if run < 18:
                continue
            for i in range(run):
                e = rel.get(addr + 4 * i)
                if e:
                    votes[e[1]][i] += 1
    return {a: next(iter(c)) for a, c in votes.items()
            if len(c) == 1 and sum(c.values()) >= 5}


def itanium_face(name):
    """_ZN9ActorBase13AfterBehaviorEj -> ('ActorBase', 'AfterBehavior')."""
    m = re.match(r"_ZN(\d+)", name)
    if not m:
        return None
    i, n = m.end(1), int(m.group(1))
    cls, rest = name[i:i + n], name[i + n:]
    m = re.match(r"(\d+)", rest)
    if not m:
        return None
    k = int(m.group(1))
    return cls, rest[m.end():m.end() + k]


def load_disasm(d):
    """({function: calls}, [(table, slot, thunk, fill, obj)]) from dumpbin.

    REQUIRES `dumpbin /disasm:nobytes`. Plain /disasm interleaves the encoded
    bytes between the address and the mnemonic, so INSN captures the first
    opcode byte instead of the mnemonic, nothing parses as a call or a mov, and
    the check finds zero stores. seats() treats zero checked fills as a hard
    failure for exactly that reason: a silently-empty run of this tool is worse
    than no tool, because CI would read it as a pass."""
    funcs, stores, byteish = {}, [], 0
    for p in sorted(pathlib.Path(d).glob("*.txt")):
        raw = p.read_bytes()
        text = raw.decode("utf-16-le" if raw[:2] == b"\xff\xfe" else "utf-8",
                          "replace")
        cur = None
        for line in text.splitlines():
            m = INSN.match(line)
            if m and cur is not None:
                op, args = m.group(2).lower(), m.group(3)
                if re.fullmatch(r"[0-9a-f]{2}", op):
                    byteish += 1        # an encoded byte where a mnemonic goes
                if op in ("call", "jmp"):
                    cur["calls"].append(args.strip())
                elif op == "mov":
                    s = VTSTORE.search(args)
                    if s:
                        stores.append((s.group("tab"),
                                       int(s.group("off") or "0", 16) // 4,
                                       s.group("sym"), cur["sym"], p.name))
                continue
            m = FUNCHEAD.match(line)
            if m and not line.startswith(" "):
                cur = dict(sym=m.group(1), calls=[])
                funcs.setdefault(m.group(1), cur)
    return funcs, stores, byteish


def seats(rom, disasm):
    canon = canonical_slots(rom)
    by_cname, by_face = {}, {}
    for a, slot in canon.items():
        n = rom.name_of(a, "main")
        if not n.startswith("_ZN"):
            n = rom.name_of(a, "overlay(2)")
        if not n.startswith("_ZN"):
            continue
        by_cname["_" + n] = (slot, n)       # MSVC prefixes C symbols with _
        f = itanium_face(n)
        if f:
            by_face[f] = (slot, n)

    def resolve(callee):
        c = callee.split()[-1].strip()
        if c in by_cname:
            return by_cname[c]
        m = re.match(r"\?(\w+)@(\w+)@@", c)
        if m and (m.group(2), m.group(1)) in by_face:
            return by_face[(m.group(2), m.group(1))]
        return None

    funcs, stores, byteish = load_disasm(disasm)
    bad, checked, unchecked = [], 0, 0
    for tab, slot, thunk, fill, obj in stores:
        f = funcs.get(thunk)
        hits = {resolve(c) for c in f["calls"]} if f else set()
        hits = {h for h in hits if h}
        if len(hits) != 1:
            unchecked += 1                  # a class body, or a decline stub
            continue
        want, name = next(iter(hits))
        checked += 1
        if want != slot:
            bad.append((slot, want, name, thunk, fill, obj))
    print("%d slot fills seat a shared arm9 body and were checked; %d hold "
          "class bodies or decline stubs and were not." % (checked, unchecked))
    # A check that finds nothing to check has not passed, it has not run. This
    # tool's whole value is being a gate, and a gate that goes green on
    # malformed input is worse than no gate: CI reads it as coverage.
    if checked == 0:
        print("\nFAILED: nothing was checked, so nothing was verified.")
        if byteish:
            print("  %d instruction lines began with an encoded byte rather "
                  "than a mnemonic." % byteish)
            print("  This is plain `dumpbin /disasm` output. Regenerate it "
                  "with /disasm:nobytes.")
        else:
            print("  No vtable stores parsed out of %s -- wrong directory, or "
                  "the hal objects were not disassembled." % disasm)
        return 2
    if not bad:
        print("no wrong-body seats")
        return 0
    print("\n=== %d WRONG-BODY SEATS ===" % len(bad))
    for slot, want, name, thunk, fill, obj in sorted(bad):
        print("slot %-2d runs %s, whose ROM slot is %d" % (slot, name, want))
        print("        thunk %s, written by %s  [%s]" % (thunk, fill, obj))
    return 1


# ============================================================================
# --fills: does the fill WRITE every slot the ROM table has?
# ============================================================================
#
# WHY A THIRD CHECK
#     --sweep reads DECLARATIONS. --seats reads what a seated thunk CALLS.
#     Neither sees a table whose width lives in a loop bound and whose storage
#     is not declared here at all:
#
#         void **vt = (void **)data_ov002_0210a83c;   // the ov002 mount
#         for (int i = 0; i < 20; ++i)                // 31 slots in the ROM
#             vt[i] = (void *)ps_trap;
#
#     That is the PLAYER. Slots 20..30 kept the mounted ROM image's own DS
#     addresses, which on host resolve outside .text, and dispatching slot 25
#     on the Player exits -1073741819. There is no array declaration to widen
#     and no wrong body to catch -- the slot was simply never written.
#
#     Three of the tables found short this way carried a plain data_ name
#     rather than a _ZTV name (data_ov002_0210a83c, data_ov079_02127fb8,
#     data_ov015_021147e8). Naming, not just declaration shape, is the blind
#     spot; this check keys off the fill instead of the name.
#
# WHAT IT READS
#     The fills, from source. For each function it finds the table it binds
#     (`void **vt = (void **)NAME` or a direct `NAME[i] =`), the slots it
#     writes explicitly, the range any seed loop covers, and the slots any
#     shared helper it calls writes -- resolved transitively. Then it asks the
#     ROM how many slots the table has.
#
# ITS OWN FIELD OF VIEW, STATED RATHER THAN ASSUMED
#     This is a source scan, so a fill that writes through a shape it does not
#     model is invisible to it. That is exactly the failure that let the Player
#     sit unnoticed, so every fill it CANNOT resolve is listed by name at the
#     end instead of being silently dropped. An unresolved fill is a gap in the
#     check, not a pass.

FILLFN = re.compile(r"^(?:extern\s+\"C\"\s+)?(?:static\s+)?void\s+"
                    r"(\w+)\s*\(([^)]*)\)\s*$")
# ANY identifier, not just _ZTV/data_ prefixed: port_bp_shell_vtable and the
# ov013 clock tables bind through host names of their own, and a prefix filter
# here is the same naming blind spot that hid the Player's table from --sweep.
BIND = re.compile(r"void\s*\*\s*(?:volatile\s*)?\*\s*(\w+)\s*=\s*"
                  r"(?:\([^)]*\)\s*)?([A-Za-z_]\w*)\s*;")
WRITE = re.compile(r"\b(\w+)\s*\[\s*(\d+)\s*\]\s*=")
LOOP = re.compile(r"for\s*\(\s*(?:int\s+)?(\w+)\s*=\s*0\s*;\s*\1\s*<\s*"
                  r"([^;]+?)\s*;")
CALL = re.compile(r"\b(\w+)\s*\(\s*(?:\(void\s*\*\*\)\s*)?(\w+)\s*\)\s*;")


def _fn_bodies(text):
    """{name: body} for the brace-balanced top-level functions in a file."""
    out, lines = {}, text.splitlines()
    i = 0
    while i < len(lines):
        m = FILLFN.match(lines[i].rstrip())
        if m and i + 1 < len(lines) and lines[i + 1].startswith("{"):
            depth, j, buf = 0, i + 1, []
            while j < len(lines):
                depth += lines[j].count("{") - lines[j].count("}")
                buf.append(lines[j])
                j += 1
                if depth == 0:
                    break
            out[m.group(1)] = "\n".join(buf)
            i = j
            continue
        i += 1
    return out


ALIAS = re.compile(r'/alternatename:(\??[\w?@$]+)=(\??[\w?@$]+)')
# Some host tables carry a name the ROM never had -- port_bp_shell_vtable
# is invented outright, and _ZTV8dMeter_c is the config's name for a table
# whose ROM symbol is _ZTV3HUD. There is no mechanical route from those to
# an address, so the DECLARATION states it:
#     void *_ZTV8dMeter_c[18];   /* vtspan: _ZTV3HUD */
# Without it such a fill is unresolved, and an unresolved fill is a hole in
# this check rather than a pass -- which is how the Player's table stayed
# invisible. Annotate rather than skip.
ANNOT = re.compile(r'(\w+)\s*\[[^\]]*\]\s*;.*?vtspan:\s*(\w+)')
# `void **tabs[2] = { A, B }; ... void **vt = tabs[k];` -- one body
# filling several tables identically, which is how the Platform base
# table and its second name are seated.
MULTIBIND = re.compile(r"void\s*\*\*\s*(\w+)\s*\[\s*\d+\s*\]\s*=\s*\{([^}]*)\}")


def fills(rom, root):
    consts = {"HAL_PLAYER_SLOTS": 31}
    per_file, alias = {}, {}
    for p in sorted((pathlib.Path(root) / "port").rglob("*.cpp")):
        text = p.read_text(errors="replace")
        for m in re.finditer(r"#define\s+(\w+)\s+(\d+)", text):
            consts[m.group(1)] = int(m.group(2))
        # A host array often carries the ROM's name only through an alias
        # pragma -- _ZTV11daObjPile_c is really data_ov091_021352bc. Both
        # directions, because the pragma is written either way round.
        for m in ANNOT.finditer(text):
            alias.setdefault(m.group(1), set()).add(m.group(2))
        for m in ALIAS.finditer(text):
            a = re.sub(r"^_", "", m.group(1))
            b = re.sub(r"^_", "", m.group(2))
            alias.setdefault(a, set()).add(b)
            alias.setdefault(b, set()).add(a)
        per_file[p] = _fn_bodies(text)

    def resolve_table(name, seen=None):
        """The ROM symbol behind a table name, following alias pragmas."""
        seen = seen or set()
        if name in seen:
            return []
        seen.add(name)
        hits = list(rom.find(name))
        if hits:
            return hits
        for other in sorted(alias.get(name, ())):
            hits = resolve_table(other, seen)
            if hits:
                return hits
        return []

    def written(body, var, bodies, seen):
        """Slot indices a body writes through `var`, following helpers."""
        got = set()
        for m in WRITE.finditer(body):
            if m.group(1) == var:
                got.add(int(m.group(2)))
        for m in LOOP.finditer(body):
            tail = body[m.end():m.end() + 200]
            w = WRITE.search(tail)
            if not (w and w.group(1) == var):
                continue
            try:
                n = int(eval(m.group(2), {"__builtins__": {}}, dict(consts)))
            except Exception:
                continue
            got |= set(range(n))
        for m in CALL.finditer(body):
            fn, arg = m.group(1), m.group(2)
            if arg != var or fn in seen or fn not in bodies:
                continue
            seen.add(fn)
            inner = bodies[fn]
            im = re.search(r"\(\s*void\s*\*\*\s*(\w+)\s*\)", inner[:0])
            got |= written(inner, "vt", bodies, seen)
        return got

    rows, unresolved, helpers = [], [], set()
    # a shared helper is not a gap: it is reached with the bound variable from
    # a fill that IS resolved, and its writes are already folded in below.
    for p, bodies in per_file.items():
        for fn, body in bodies.items():
            b = BIND.search(body)
            if not b:
                continue
            for m in CALL.finditer(body):
                if m.group(2) == b.group(1) and m.group(1) in bodies:
                    helpers.add(m.group(1))
    for p, bodies in per_file.items():
        for fn, body in bodies.items():
            tables, var = [], None
            b = BIND.search(body)
            if b:
                var, tables = b.group(1), [b.group(2)]
            else:
                m = MULTIBIND.search(body)
                if m:
                    var = "vt"
                    tables = [x.strip() for x in m.group(2).split(",")
                              if x.strip()]
            if not tables:
                if (fn not in helpers
                        and re.search(r"\bvt\s*\[\s*\d+\s*\]\s*=", body)):
                    unresolved.append((str(p.relative_to(root)), fn,
                                       "table binding not modelled"))
                continue
            for table in tables:
                hits = resolve_table(table)
                if not hits:
                    unresolved.append((str(p.relative_to(root)), fn,
                                       "%s is a host-only name" % table))
                    continue
                module, addr = hits[0]
                span, _ = rom.span(module, addr)
                if span < 4:
                    continue
                got = written(body, var, bodies, set())
                missing = sorted(set(range(span)) - got)
                rows.append((table, fn, str(p.relative_to(root)), span,
                             missing))

    bad = [r for r in rows if r[4]]
    print("%d fills resolved to a ROM table" % len(rows))
    if bad:
        print("\n=== %d FILLS LEAVE A SLOT UNWRITTEN ===" % len(bad))
        for table, fn, where, span, missing in sorted(bad):
            miss = ",".join(str(x) for x in missing[:12])
            if len(missing) > 12:
                miss += ",..."
            print("%s (%s)\n        ROM has %d slots; never written: %s  [%s]"
                  % (table, fn, span, miss, where))
    else:
        print("every resolved fill writes every slot its ROM table has")
    if unresolved:
        print("\n%d fill(s) this scan could NOT resolve -- a gap in the check, "
              "not a pass:" % len(unresolved))
        for where, fn, why in sorted(unresolved):
            print("  %-38s %-34s %s" % (fn, why, where))
    return 1 if bad else 0


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    rom = Rom(argv[1])
    if argv[2] == "--sweep":
        return sweep(rom, argv[1])
    if argv[2] == "--fills":
        return fills(rom, argv[1])
    if argv[2] == "--seats":
        return seats(rom, argv[3])
    for want in argv[2:]:
        hits = list(rom.find(want))
        if not hits:
            print("%s: no such ROM symbol" % want)
            continue
        for module, addr in hits:
            rom.dump(module, addr, want)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
