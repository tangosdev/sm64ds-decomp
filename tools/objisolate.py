"""Reduce a compiled object to the one function the delink entry declares.

A C++ destructor cannot be compiled alone. `Coin::~Coin()` in a `.cpp` yields an
object carrying *six* content sections -- three `.text` (D0 0x58, D1 0x50, D2 0x50)
and five `.data` (the vtable plus the RTTI record and its name strings) -- because
under the Itanium ABI the class's vtable is emitted into the TU that defines its
key function, and mwcc emits every destructor variant alongside it.

`eligible.py` rejects that object, correctly: dsd's linker script selects an
object's code by section name (`Coin.o(.text)`), which matches ALL THREE `.text`
sections and would place D0 and D2 at D1's address, and the `.data` would duplicate
bytes the ROM's gap object already provides. 81 enrolled files sit at
"extra sections: .data" for exactly this reason.

The ROM's own layout says what to keep. For Coin, ov002 declares:

    _ZN4CoinD1Ev  size 0x50  addr 0x020b0f54     <- ours, 0x50, keep
    _ZN4CoinD0Ev  size 0x64  addr 0x020b0fa4     <- ours is 0x58; NOT ours
    _ZTV4Coin                addr 0x021087ec     <- carved out already; import it
    (no _ZN4CoinD2Ev anywhere)                   <- the ROM never had one

So: keep D1's `.text`, drop everything else, and rebind D1's reference to the
vtable from the object's own `.data` to the ROM's carved-out symbol. Surveyed over
all 81, that is the shape of 69 of them; 11 reference no local symbol at all.

HOW, AND WHY THIS WAY
---------------------
Header surgery in place, never a rewrite -- the same discipline as
`rombuild.retarget_text_section`, and for the same reason: this build exists to
compare bytes against the ROM, so the one thing it must not do is perturb them.
Dropping a section means zeroing its `sh_size`; the bytes stay in the file and the
linker contributes none of them. Nothing moves, so no relocation offset shifts and
no symbol index changes.

THE ONE DANGEROUS CASE, AND WHY IT DICTATES THE DESIGN
------------------------------------------------------
A dropped symbol must become SHN_UNDEF -- an IMPORT -- and never a definition at a
bogus address. dsd's gap objects import carved-out symbols *weakly*, so if this
object kept defining `_ZTV4Coin` in a now-empty `.data`, every user of Coin's
vtable in the ROM would bind to our address 0 instead of 0x021087ec. It would link
clean and produce a ROM that jumps through a null vtable. Zeroing a section without
also externalising its symbols is precisely the silent-corruption move, so the two
steps are one operation here and are not separable.

`_ZN4CoinD2Ev` is the awkward one: the ROM has no such symbol, so importing it
would be unresolvable. But it is also unreferenced once D0's section is gone --
verified per-file rather than assumed, via `dead_symbols`, and reported so a caller
can reject a file where it is not true.
"""
import io

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

CONTENT = ("SHT_PROGBITS", "SHT_NOBITS")
IGNORE = (".comment", ".debug", ".line", ".note")

SHN_UNDEF = 0
R_ARM_ABS32 = 2

# "There is nothing here to reduce" -- not a failure. An object whose enrolled symbol
# is not a defined function in it (an assembly stub, a data entry) simply has no
# section to keep, and the other gates judge it on their own terms.
#
# Callers must branch on this, so it is a value rather than a prose match. Both
# call sites used to compare against the message text -- one with `!=` on the whole
# formatted string, one with `.endswith` -- so adding a period here would have made
# eligible.py report a hard reason for every such file while rombuild kept passing
# them, with the two disagreeing about the same object.
NOT_A_FUNCTION = "not-a-defined-function"

# The two words of Itanium primary-vtable preamble: offset-to-top, then typeinfo.
#
# mwcc's own `_ZTV<C>` symbol addresses the START of the vtable object, so the store
# into `this->vptr` relocates against it with an addend of 8 to step over those two
# words. The ROM's symbols.txt uses the other convention -- `_ZTV<C>` IS the slot
# array, already past the preamble; this tree recorded that for `_ZTV5Actor`
# (0x0208e3a4) when reading it with a -2/-1 header shifted every slot by two.
#
# Rebinding the reloc to the ROM's symbol without adjusting therefore adds 8 twice.
# It links clean and writes a vptr one entry past the truth: measured as
# `Coin::~Coin +0x4c  rom 021087ec  built 021087f4` across 76 functions and 34
# modules. Nothing catches that except the byte compare, which is why the addend is
# corrected here and an unexpected one is a refusal rather than a guess.
VTABLE_PREAMBLE = 8


def _shdr_offset(elf, index):
    """File offset of section header `index`."""
    return elf["e_shoff"] + index * elf["e_shentsize"]


def plan(raw, keep_symbol):
    """What isolation would do, without doing it.

    Returns a dict:
      keep        section index the wanted function lives in, or None
      drop        section indices to zero
      externalise symbol names to turn into imports (referenced by the survivor)
      dead        symbol names dropped and referenced by nothing
      error       why isolation is impossible, or None
      kind        machine-readable tag for an error a caller must branch on;
                  only NOT_A_FUNCTION today, absent for every other error
    """
    elf = ELFFile(io.BytesIO(raw))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return {"error": "no .symtab"}

    keep = None
    for s in symtab.iter_symbols():
        if (s.name == keep_symbol and s["st_shndx"] != "SHN_UNDEF"
                and s["st_info"]["type"] == "STT_FUNC"):
            keep = s["st_shndx"]
    if not isinstance(keep, int):
        return {"error": f"{keep_symbol} is not a defined function here",
                "kind": NOT_A_FUNCTION}

    drop = [i for i, s in enumerate(secs)
            if s.header["sh_type"] in CONTENT and s.header["sh_size"]
            and i != keep and not any(s.name.startswith(p) for p in IGNORE)]
    dropset = set(drop)

    # Relocation sections that describe dropped sections go too: their targets no
    # longer exist, and leaving them would have the linker apply fixups into a
    # zero-length section.
    drop += [i for i, s in enumerate(secs)
             if isinstance(s, RelocationSection) and s.header["sh_size"]
             and s.header["sh_info"] in dropset]

    # What the SURVIVING section still points at decides import-vs-dead.
    referenced = set()
    for s in secs:
        if isinstance(s, RelocationSection) and s.header["sh_info"] == keep:
            for r in s.iter_relocations():
                sym = symtab.get_symbol(r["r_info_sym"])
                if sym.name:
                    referenced.add(sym.name)

    externalise, dead = [], []
    for s in symtab.iter_symbols():
        if not s.name or s.name == keep_symbol:
            continue
        if s["st_shndx"] in dropset:
            (externalise if s.name in referenced else dead).append(s.name)

    # EVERY RTTI-ish reference the kept function makes is checked, not only the ones
    # being externalised. Checking only the externalised set leaves a hole with a
    # demonstrated failing input:
    #
    #   A vtable symbol can be UNDEF in the object from the start, and then it is
    #   never a candidate for externalisation and never gets looked at. mwcc emits
    #   exactly that for a constructor-only TU -- the vtable's key function is the
    #   DESTRUCTOR, so a TU defining only `V::V()` references `_ZTV1V` without
    #   defining it -- and the implicit vptr store still carries addend 8. Same shape
    #   for a derived destructor over an inline base destructor: the object's own
    #   `_ZTV1D` gets corrected while the inlined `_ZTV1B` store does not.
    #
    #   Neither is hypothetical; both were reproduced under 2004/b56. Neither occurs
    #   in the 3,352 enrolled C++ candidates today, because their recovered sources
    #   spell the vptr store explicitly (`extern int _ZTV5Scene[]`, addend 0). Both
    #   arrive the moment a real-C++ constructor is enrolled, which is the direction
    #   this tree is moving.
    #
    # An UNDEF reference must therefore already be addend 0 -- the ROM symbol IS the
    # slot array, so symbol+0 is the whole address. A nonzero one is refused rather
    # than corrected: there is no enrolled instance to verify a correction against,
    # and fail-closed costs one function while fail-open corrupts a module.
    ext = set(externalise)
    for s in secs:
        if not isinstance(s, RelocationSection) or s.header["sh_info"] != keep:
            continue
        for r in s.iter_relocations():
            sym = symtab.get_symbol(r["r_info_sym"])
            addend = r["r_addend"] if s.is_RELA() else None
            shndx = sym["st_shndx"]

            # A reloc through a section symbol into a dropped section carries its
            # target in the addend alone, so externalising cannot preserve it and
            # nothing would flag the result. Zero instances enrolled; cheap to refuse.
            if not sym.name and shndx in dropset:
                return {"error": f"unnamed section-symbol reloc into dropped section "
                                 f"{secs[shndx].name} at 0x{r['r_offset']:x}"}

            if not sym.name.startswith(("_ZTV", "_ZTI", "_ZTS")):
                continue
            if not s.is_RELA():
                return {"error": f"{sym.name}: RTTI reloc is REL, not RELA"}

            if sym.name in ext:
                # MULTIPLE INHERITANCE puts a SECOND vptr store in the object,
                # pointing past the primary slots into the secondary sub-table --
                # addend 44 for ModelAnim, where the primary's is 8. The
                # correction is the same subtraction either way, because mwcc's
                # _ZTV addresses the vtable object's start while the ROM's IS the
                # slot array. The ROM names the destination independently and the
                # arithmetic lands on it exactly:
                #
                #   _ZTV9ModelAnim                   0x0208e980
                #   VTable_Animation_ModelAnimThunk  0x0208e9a4   = +0x24
                #   44 - VTABLE_PREAMBLE                          =  0x24
                #
                # Two symbols this tree recorded separately agreeing with the
                # subtraction is the enrolled instance the refusal wanted.
                ok_type = r["r_info_type"] == R_ARM_ABS32
                ok_addend = (addend >= VTABLE_PREAMBLE if sym.name.startswith("_ZTV")
                             else addend == 0)
                if not (ok_type and ok_addend):
                    return {"error": f"{sym.name}: unexpected reloc "
                                     f"type={r['r_info_type']} addend={addend}"}
            elif shndx == "SHN_UNDEF" or shndx == SHN_UNDEF:
                # The predicted case above, now real: an INLINE BASE DESTRUCTOR.
                # `Scene::~Scene()` inlines ~ActorDerived, so the object stores
                # _ZTV12ActorDerived without ever defining it -- UNDEF from the
                # start, addend 8, never a candidate for externalisation. The
                # correction is the same arithmetic the externalise path uses, and
                # it is verified the same way: rombuild byte-compares the linked
                # module, which is the only thing that caught the original 8-high
                # vptr bug. A different addend is still refused.
                if addend and not (sym.name.startswith("_ZTV")
                                   and addend == VTABLE_PREAMBLE):
                    return {"error": f"{sym.name}: undefined RTTI reference with "
                                     f"addend {addend}; the ROM symbol is already the "
                                     f"slot array, so this would land {addend} past it"}

    return {"keep": keep, "drop": sorted(set(drop)), "externalise": sorted(externalise),
            "dead": sorted(dead), "referenced": sorted(referenced), "error": None}


def referenced_undefined(raw, keep_symbol):
    """Undefined symbol names the kept function actually references.

    `eligible.py` rule 5 rejects an undefined symbol that config/**/symbols.txt does
    not define, because gap objects import weakly and an invented name would bind
    silently to 0. That hazard needs something to bind: after isolation an object
    carries dead imports (`_ZN4CoinD2Ev`, `_ZTVN3abi17__class_type_infoE`) that no
    surviving relocation names, and those cannot bind to anything because nothing
    looks them up. Rule 5 is therefore applied to this set rather than to every
    undefined symbol in the table."""
    elf = ELFFile(io.BytesIO(raw))
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return set()
    keep = None
    for s in symtab.iter_symbols():
        if (s.name == keep_symbol and s["st_shndx"] != "SHN_UNDEF"
                and s["st_info"]["type"] == "STT_FUNC"):
            keep = s["st_shndx"]
    out = set()
    for s in elf.iter_sections():
        if isinstance(s, RelocationSection) and s.header["sh_info"] == keep:
            for r in s.iter_relocations():
                sym = symtab.get_symbol(r["r_info_sym"])
                if sym.name and sym["st_shndx"] == "SHN_UNDEF":
                    out.add(sym.name)
    return out


def isolate(obj, keep_symbol):
    """Apply `plan` to the object file in place. Returns the plan.

    Idempotent: an object with nothing left to drop is rewritten to the same bytes,
    so a cached object processed by an earlier build is not corrupted by a later
    one."""
    raw = bytearray(obj.read_bytes())
    p = plan(bytes(raw), keep_symbol)
    if p.get("error"):
        return p
    if not p["drop"] and not p["externalise"]:
        return p

    elf = ELFFile(io.BytesIO(bytes(raw)))
    endian = "<" if elf.little_endian else ">"
    import struct

    # sh_size is at +0x14 in a 32-bit Elf32_Shdr.
    for i in p["drop"]:
        off = _shdr_offset(elf, i) + 0x14
        struct.pack_into(endian + "I", raw, off, 0)

    # Elf32_Sym is 16 bytes: name(4) value(4) size(4) info(1) other(1) shndx(2).
    symtab = elf.get_section_by_name(".symtab")
    base = symtab.header["sh_offset"]
    dropset = set(p["drop"])
    referenced = set(p["externalise"])

    def ext_or_visible(sym):
        """Names that must become imports: referenced by the survivor, or visible."""
        if sym.name in referenced:
            return {sym.name}
        return {sym.name} if sym["st_info"]["bind"] != "STB_LOCAL" else set()

    for idx, s in enumerate(symtab.iter_symbols()):
        if s.name == keep_symbol or s["st_shndx"] not in dropset:
            continue
        # EVERY symbol in a dropped section, whatever its binding -- not just
        # STB_GLOBAL/STB_WEAK. Restricting it to those two is what made mwldarm
        # reject `_ZN6Player18St_YoshiPower_MainEv.o` with "the sum of all symbol
        # sizes exceed section size": mwcc had emitted an inline `_ZN4PVecD1Ev`
        # under STB_LOPROC, a Metrowerks binding neither name matches, so its
        # st_size of 4 survived into a section that had just been zeroed. A symbol
        # claiming bytes its section no longer has is a malformed object, and the
        # linker is right to refuse it.
        ent = base + idx * 16
        struct.pack_into(endian + "I", raw, ent + 4, 0)          # st_value
        struct.pack_into(endian + "I", raw, ent + 8, 0)          # st_size
        # UNDEF unless it is a LOCAL that nothing references.
        #
        # Two separate hazards, and a local hits the second one even though it is
        # invisible to other objects:
        #
        #   left defined and externally visible -- it sits at offset 0 of an empty
        #   section, which the lcf still places at the kept function's address, so a
        #   weak gap-object import of that name binds there. `_ZN4CoinD0Ev` would
        #   resolve to Coin's D1.
        #
        #   left defined and REFERENCED -- the kept function's own relocation
        #   resolves to that same wrong address. `func_ov002_020bd664` is the case:
        #   a function-local static `table$8` and its guard `_ZGVtable$8`, both
        #   STB_LOCAL in .bss, both addressed by the function. Zeroing the section
        #   and keeping them defined pointed the loads at the function itself. The
        #   ROM supplies that .bss from the gap object, so this file simply cannot be
        #   isolated -- and made UNDEF it says so, because rule 5 then looks
        #   `table$8` up in symbols.txt, does not find it, and rejects. That is the
        #   existing rule doing the work rather than a new special case.
        #
        # An unreferenced local is neither: nothing outside can see it and nothing
        # inside asks for it, so zeroing its size is enough. Marking it undefined
        # would invent an import that no ROM symbol could satisfy.
        if s.name in ext_or_visible(s):
            struct.pack_into(endian + "H", raw, ent + 14, SHN_UNDEF)

    # Drop the preamble skip from every vtable reference, now that the symbol it
    # binds to already points at the slot array. Elf32_Rela is 12 bytes:
    # r_offset(4) r_info(4) r_addend(4). `plan` has already refused anything whose
    # type/addend is not the surveyed shape. Usually 8 -> 0; a multiple-
    # inheritance secondary vptr store subtracts the same 8 from a larger
    # addend and lands on the sub-table (44 -> 36 for ModelAnim).
    #
    # Two kinds reach here and both need the same correction. A vtable this object
    # DEFINED and is now externalising, and one it never defined at all -- the
    # inline-base-destructor case, where the store comes from a body inlined out of
    # a header, so the symbol is UNDEF from the start and is not in `externalise`.
    ext = set(p["externalise"])
    for s in elf.iter_sections():
        if not isinstance(s, RelocationSection) or s.header["sh_info"] != p["keep"]:
            continue
        if not s.is_RELA():
            continue
        roff = s.header["sh_offset"]
        for i, r in enumerate(s.iter_relocations()):
            sym = symtab.get_symbol(r["r_info_sym"])
            if not sym.name.startswith("_ZTV"):
                continue
            undef = sym["st_shndx"] in ("SHN_UNDEF", SHN_UNDEF)
            if (sym.name in ext or undef) and r["r_addend"] >= VTABLE_PREAMBLE:
                struct.pack_into(endian + "i", raw, roff + i * 12 + 8,
                                 r["r_addend"] - VTABLE_PREAMBLE)

    obj.write_bytes(bytes(raw))
    return p
