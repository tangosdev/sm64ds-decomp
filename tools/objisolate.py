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
SHF_EXECINSTR = 0x4

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
                # start, addend 8, never a candidate for externalisation. A
                # constructor-only TU of an MI class stores its SECONDARY vptr
                # the same way: UNDEF _ZTV<C>, addend 24 = 8 preamble + 0x10
                # into the secondary block, which must land on the ROM's
                # separately named thunk table. That is exactly the
                # externalise case's arithmetic, whose enrolled instance
                # (ModelAnim: 44 - 8 = +0x24 = VTable_Animation_ModelAnimThunk)
                # is the verification the original refusal asked for, and
                # rombuild's module byte-compare still checks every landing.
                # Non-_ZTV RTTI addends stay refused, and so does any _ZTV
                # addend below the preamble: no surveyed shape produces one.
                if addend and not (sym.name.startswith("_ZTV")
                                   and addend >= VTABLE_PREAMBLE):
                    return {"error": f"{sym.name}: undefined RTTI reference with "
                                     f"addend {addend}; the ROM symbol is already the "
                                     f"slot array, so this would land {addend} past it"}

    return {"keep": keep, "drop": sorted(set(drop)), "externalise": sorted(externalise),
            "dead": sorted(dead), "referenced": sorted(referenced), "error": None}


def plan_many(raw, keep_symbols):
    """Validate an exact text-only multi-function object for production linking.

    This is deliberately not ``plan`` with a wider argument.  The one-function
    pipeline is allowed to discard compiler-emitted neighbours and data because one
    delinks entry licenses one function.  A consolidated source has one spanning
    delinks entry: its intact object is the linker contribution, so silently dropping
    an unexpected helper or data section would create a hole inside a range dsd no
    longer fills from the ROM.

    The accepted shape is intentionally narrow:

    * at least two distinct requested symbols, each defined exactly once as a function;
    * one dedicated, non-empty ``.text`` section per requested symbol;
    * those sections occur in the caller's ROM order;
    * no other non-debug content and no unlicensed symbol in a kept section; and
    * no undefined RTTI relocation that still needs the single-function ``-8`` rewrite.

    Success therefore needs no ELF surgery: the exact input object is preserved.  The
    returned plan uses the familiar shape so callers can report one isolation verdict
    without weakening or changing ``plan``/``derive``/``isolate`` for singletons.
    """
    requested = [str(name) for name in keep_symbols if str(name)]
    if len(requested) < 2:
        return {"error": "multi-symbol isolation needs at least two symbols"}
    if len(set(requested)) != len(requested):
        return {"error": "duplicate multi-symbol isolation request"}

    elf = ELFFile(io.BytesIO(bytes(raw)))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return {"error": "no .symtab"}
    syms = list(symtab.iter_symbols())

    by_name = {
        name: [s for s in syms if s.name == name
               and s["st_shndx"] not in ("SHN_UNDEF", SHN_UNDEF)]
        for name in requested
    }
    for name in requested:
        matches = by_name[name]
        if len(matches) != 1:
            return {"error": f"{name} has {len(matches)} defined symbols, expected one"}
        sym = matches[0]
        if sym["st_info"]["type"] != "STT_FUNC":
            return {"error": f"{name} is {sym['st_info']['type']}, expected STT_FUNC"}
        shndx = sym["st_shndx"]
        sec = secs[shndx] if isinstance(shndx, int) else None
        if sec is None or sec.name != ".text" \
                or sec.header["sh_type"] != "SHT_PROGBITS" \
                or not sec.header["sh_size"]:
            return {"error": f"{name} is not in a non-empty .text/SHT_PROGBITS section"}
        if sym["st_value"] != 0 or sym["st_size"] != sec.header["sh_size"]:
            return {"error": f"{name} does not exactly cover its .text section "
                             f"(value=0x{sym['st_value']:x}, symbol=0x{sym['st_size']:x}, "
                             f"section=0x{sec.header['sh_size']:x})"}

    keep = [by_name[name][0]["st_shndx"] for name in requested]
    if len(set(keep)) != len(keep):
        return {"error": "requested functions do not occupy distinct .text sections"}

    occupants = sorted({
        sym.name for sym in syms
        if sym.name and not sym.name.startswith("$")
        and sym["st_info"]["type"] != "STT_SECTION"
        and sym["st_shndx"] in set(keep)
    })
    foreign = sorted(set(occupants) - set(requested))
    if foreign:
        return {"error": f"kept function section(s) also define unlicensed symbol(s): "
                         f"{foreign}"}

    live = [
        (index, sec) for index, sec in enumerate(secs)
        if sec.header["sh_type"] in CONTENT and sec.header["sh_size"]
        and not any(sec.name.startswith(prefix) for prefix in IGNORE)
    ]
    extra = [(index, sec) for index, sec in live if index not in set(keep)]
    if extra:
        detail = []
        for index, sec in extra:
            names = sorted({
                sym.name for sym in syms
                if sym.name and not sym.name.startswith("$")
                and sym["st_info"]["type"] != "STT_SECTION"
                and sym["st_shndx"] == index
            })
            detail.append(f"section[{index}] {sec.name} size 0x{sec.header['sh_size']:x} "
                          f"defines {names}")
        return {"error": "unlicensed content in text-only multi-symbol object: "
                         + "; ".join(detail)}

    emitted = [name for _index, name in sorted(zip(keep, requested))]
    if emitted != requested:
        return {"error": "requested functions are not emitted in ROM order: "
                         f"requested {requested}, emitted {emitted}"}

    referenced = set()
    for relsec in secs:
        if not isinstance(relsec, RelocationSection) or relsec.header["sh_info"] not in keep:
            continue
        for reloc in relsec.iter_relocations():
            target = symtab.get_symbol(reloc["r_info_sym"])
            if target.name:
                referenced.add(target.name)
            if not target.name.startswith(("_ZTV", "_ZTI", "_ZTS")):
                continue
            if not relsec.is_RELA():
                return {"error": f"{target.name}: RTTI reloc is REL, not RELA"}
            if target["st_shndx"] in ("SHN_UNDEF", SHN_UNDEF) \
                    and reloc["r_addend"] != 0:
                return {"error": f"{target.name}: intact multi-symbol object has "
                                 f"undefined RTTI addend {reloc['r_addend']}; text-only "
                                 "production isolation cannot rewrite it"}

    return {"keep": keep, "keepSymbols": requested, "drop": [],
            "externalise": [], "dead": [], "referenced": sorted(referenced),
            "error": None}


def derive_many(raw, keep_symbols):
    """Pure, fail-closed validation of one exact text-only multi-function object."""
    p = plan_many(bytes(raw), keep_symbols)
    return (None, p) if p.get("error") else (bytes(raw), p)


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


def derive(raw, keep_symbol):
    """`isolate`, as a pure function of bytes: (derived_bytes, plan).

    Same reduction, same plan, no filesystem. `derived_bytes` is None exactly when
    the plan carries an error.

    WHY THIS EXISTS SEPARATELY FROM `isolate`
    -----------------------------------------
    `isolate` answers "reduce THIS object to the one function its delink entry
    declares", which is a 1:1 relationship between a source file and a function and
    is all the current build needs. Translation-unit reconstruction needs the 1:N
    one: compile the recovered multi-function `.cpp` ONCE, then produce one derived
    object per function it is licensed to contribute
    (notes/translation-unit-reconstruction-plan.md section 9, phase D). That is N
    reductions of the SAME input bytes, which an in-place mutator cannot express --
    each reduction has to start from the untouched object, and `isolate` has already
    consumed it by the second call.

    Nothing about the reduction itself changes: `plan` still decides what to keep,
    drop, externalise and re-addend, and `isolate` is now this function plus a
    write. A caller that wants the derived object on disk under a DIFFERENT name
    (the TU case: one source, N object paths) writes the bytes itself.
    """
    p = plan(bytes(raw), keep_symbol)
    if p.get("error"):
        return None, p
    if not p["drop"] and not p["externalise"]:
        return bytes(raw), p
    return _apply(raw, p, keep_symbol), p


def deadstrip_plan(raw, symbol_names, expect=None):
    """Plan removal of exact, explicitly named compiler-only output sections.

    This is intentionally narrower than a linker dead-strip pass.  TU reconstruction
    occasionally makes mwcc emit a destructor D2 or an inline/helper body which the
    retail link discarded, while this repository links with ``-nodead``.  A caller may
    model that historical discard only when all of these facts are mechanically true:

    * every requested symbol is a defined function or data object in its own
      content section;
    * every named, non-mapping symbol in each removed section is requested too;
    * no relocation from a surviving content section references a requested
      *function* or an unnamed section symbol for a removed section.  A requested
      *data object* may be referenced by name -- becoming an import is the point, and
      it is the only way a destructor can go on storing the vptr of a vtable the ROM
      owns -- but only for ``_ZTV``/``_ZTI``/``_ZTS``, whose relocation shapes are
      surveyed, and never through an unnamed section symbol.

    ``expect`` extends this to the *duplicate* case: a vague-linkage body the compiler
    re-emits in every TU that needs it, while the retail image keeps exactly one copy
    at a configured address.  Such a symbol is NOT compiler-only -- it has a ROM home --
    so discarding it is only sound when this object's copy is provably the same body.
    ``{symbol: bytes}`` demands exactly that: ``STB_LOPROC`` binding and a section whose
    content equals the cartridge's own bytes at the home address.  A caller that cannot
    produce those bytes must not use this path.  If nothing else defines the symbol the
    link fails loudly on an undefined reference, so this cannot silently drop a function
    that has a retail address.

    ``STT_OBJECT`` is accepted alongside ``STT_FUNC`` because a reconstructed TU that
    names its key function emits the class's ``_ZTV``/``_ZTI``/``_ZTS`` as well as its
    code, and an intact multi-symbol object refuses any surviving non-``.text`` content.
    Without this, promoting a TU meant deleting the out-of-line destructor -- and with
    it the only thing that makes the vtable comparable to the cartridge at all.  The
    section-level checks above are linkage-agnostic and carry over unchanged; what does
    NOT carry over is ``expect``.  A data object's words are relocation targets, so the
    bytes in this object are addends, not linked addresses, and comparing them against
    the cartridge would compare two different things.  Raw duplicate-body evidence is
    therefore refused for data, and the caller proves those bodies the way
    ``romdata_check`` does: by applying the relocations first.

    There are no patterns and no binding-based guesses.  The caller must name every
    symbol, and any ambiguity is an error.  The returned plan has the same shape as
    :func:`plan`, so :func:`_apply` remains the single implementation of the safe ELF
    header surgery (zero section sizes and externalise visible definitions).
    """
    wanted = {str(n) for n in symbol_names if str(n)}
    if not wanted:
        return {"error": "no compiler-only symbols were requested"}

    elf = ELFFile(io.BytesIO(bytes(raw)))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return {"error": "no .symtab"}
    syms = list(symtab.iter_symbols())

    defined = {s.name: s for s in syms
               if s.name and s["st_shndx"] not in ("SHN_UNDEF", "SHN_ABS")}
    missing = sorted(wanted - set(defined))
    if missing:
        return {"error": f"compiler-only symbol(s) not defined: {missing}"}

    bad_type = sorted(n for n in wanted
                      if defined[n]["st_info"]["type"] not in ("STT_FUNC", "STT_OBJECT"))
    if bad_type:
        return {"error": f"compiler-only deadstrip accepts functions and data objects "
                         f"only: {bad_type}"}

    drop_content = {defined[n]["st_shndx"] for n in wanted}
    if not all(isinstance(i, int) and secs[i].header["sh_type"] in CONTENT
               for i in drop_content):
        return {"error": "a compiler-only symbol is not in a content section"}

    for shndx in sorted(drop_content):
        occupants = {s.name for s in syms
                     if s.name and not s.name.startswith("$")
                     and s["st_shndx"] == shndx
                     and s["st_info"]["type"] != "STT_SECTION"}
        foreign = sorted(occupants - wanted)
        if foreign:
            return {"error": f"section[{shndx}] {secs[shndx].name} also defines "
                             f"non-policy symbol(s): {foreign}"}

    funcs = {n for n in wanted if defined[n]["st_info"]["type"] == "STT_FUNC"}
    objects = wanted - funcs

    # A discarded section may have relocations of its own; those disappear with it.
    # Only references from SURVIVING content are load-bearing and therefore blockers.
    #
    # `rebase` collects the surviving CODE sections whose vtable relocations still
    # carry mwcc's preamble skip.  mwcc's `_ZTV<C>` addresses the vtable object's
    # start, so a vptr store against a definition in this object reads `+8`; the ROM
    # symbol IS the slot array, so once that definition is externalised the same store
    # must read `+0`.  `plan` performs the subtraction for the one kept function and
    # `plan_many` refuses an object that still needs it -- this is the third producer,
    # and it hands `_apply` a set of sections rather than a single kept index.  Data
    # sections are deliberately excluded: a surviving `_ZTI` record's link to its ABI
    # vtable is verified ROM data, not an address-point convention to correct.
    rebase = set()
    for rel in secs:
        if not isinstance(rel, RelocationSection) or rel.header["sh_info"] in drop_content:
            continue
        source = secs[rel.header["sh_info"]]
        if source.header["sh_type"] not in CONTENT or not source.header["sh_size"]:
            continue
        executable = bool(source.header["sh_flags"] & SHF_EXECINSTR)
        for r in rel.iter_relocations():
            target = symtab.get_symbol(r["r_info_sym"])
            names_target = target.name in funcs
            section_target = (not target.name and target["st_shndx"] in drop_content)
            if names_target or section_target:
                label = target.name or f"section[{target['st_shndx']}]"
                return {"error": f"surviving section[{rel.header['sh_info']}] "
                                 f"{source.name} references compiler-only {label} at "
                                 f"0x{r['r_offset']:x}"}
            is_rtti = target.name.startswith(("_ZTV", "_ZTI", "_ZTS"))
            if target.name in objects and not is_rtti:
                return {"error": f"surviving section[{rel.header['sh_info']}] "
                                 f"{source.name} references compiler-only data "
                                 f"{target.name} at 0x{r['r_offset']:x}; only "
                                 f"_ZTV/_ZTI/_ZTS may be imported this way"}
            undef = target["st_shndx"] in ("SHN_UNDEF", SHN_UNDEF)
            if not is_rtti or not (target.name in objects or undef):
                continue
            if not rel.is_RELA():
                return {"error": f"{target.name}: RTTI reloc is REL, not RELA"}
            addend = r["r_addend"]
            if not executable:
                # Only a code section stores a vptr.  A surviving `_ZTI` record's own
                # `+8` link to the ABI type_info vtable is an ordinary external
                # reference that this plan does not disturb, so it is left alone; a
                # data reference to a definition being externalised HERE is outside
                # the surveyed shape and is refused rather than rewritten on a guess.
                if addend and target.name in objects:
                    return {"error": f"{target.name}: RTTI reference with addend "
                                     f"{addend} from surviving data section "
                                     f"{source.name}"}
                continue
            # Two spellings of a vptr store are legitimate and they differ by one
            # preamble.  mwcc's `_ZTV<C>` addresses the vtable OBJECT, so a store
            # against a definition in this object carries `+8` and must lose it once
            # the definition is externalised.  A source that declares the ROM symbol
            # itself (`extern int _ZTV10dBgActor_c[];`) is already using symbols.txt's
            # convention, where the symbol IS the slot array: addend 0, nothing to
            # correct, and `_apply` leaves it alone.  Anything strictly between the
            # two is a shape this survey has not seen, and it is not guessed at.
            shape = ((addend == 0 or addend >= VTABLE_PREAMBLE)
                     if target.name.startswith("_ZTV") else addend == 0)
            if r["r_info_type"] != R_ARM_ABS32 or not shape:
                return {"error": f"{target.name}: unexpected reloc "
                                 f"type={r['r_info_type']} addend={addend}"}
            if addend:
                rebase.add(rel.header["sh_info"])

    for name, want in sorted((expect or {}).items()):
        if name not in wanted:
            return {"error": f"{name}: duplicate-body evidence for a symbol that was "
                             f"not requested"}
        sym = defined[name]
        if sym["st_info"]["type"] != "STT_FUNC":
            return {"error": f"{name}: raw duplicate-body evidence is function-only; a "
                             f"data object's words are relocation addends here and "
                             f"linked addresses in the cartridge, so the two are not "
                             f"comparable before the link"}
        if sym["st_info"]["bind"] != "STB_LOPROC":
            return {"error": f"{name}: duplicate deadstrip needs vague linkage, got "
                             f"{sym['st_info']['bind']}"}
        got, want = bytes(secs[sym["st_shndx"]].data()), bytes(want)
        if len(got) != len(want):
            return {"error": f"{name}: this object's body ({len(got)} bytes) is not the "
                             f"cartridge's body at its configured home "
                             f"({len(want)} bytes)"}
        # A relocated word holds an addend here and a linked address in the cartridge,
        # so comparing it raw would report every branch and every vptr store as a
        # difference -- a 4-byte `bx lr` was the only body flat enough to survive it.
        # What this module can prove on its own is the instruction stream around them.
        # Resolving the relocations needs symbols.txt homes, which is a layer above:
        # see `rombuild._duplicate_body_reasons`, which links the body and compares the
        # words masked out here.
        relocated = set()
        for rel in secs:
            if isinstance(rel, RelocationSection) and rel.header["sh_info"] == sym["st_shndx"]:
                relocated.update(r["r_offset"] & ~3 for r in rel.iter_relocations())
        for off in range(0, len(got) & ~3, 4):
            if off in relocated:
                continue
            if got[off:off + 4] != want[off:off + 4]:
                return {"error": f"{name}: this object's body differs from the "
                                 f"cartridge's at its configured home, at "
                                 f"non-relocated offset 0x{off:x}"}

    drop = set(drop_content)
    drop.update(i for i, s in enumerate(secs)
                if isinstance(s, RelocationSection) and s.header["sh_size"]
                and s.header["sh_info"] in drop_content)
    return {"keep": None, "drop": sorted(drop), "externalise": sorted(objects),
            "dead": sorted(funcs), "referenced": [], "rebase": sorted(rebase),
            "error": None}


def derive_deadstrip(raw, symbol_names, expect=None):
    """Return an object with exact declared compiler-only sections discarded.

    ``None`` is returned instead of bytes when :func:`deadstrip_plan` refuses.  The
    input is never mutated.  This exists for scratch TU links; production one-function
    isolation continues to use :func:`derive` unchanged.
    """
    p = deadstrip_plan(raw, symbol_names, expect)
    if p.get("error"):
        return None, p
    return _apply(raw, p, None), p


def externalize_plan(raw, symbol_names):
    """Plan exact removal of dedicated vague RTTI definition sections.

    This is deliberately not a generic COMDAT/coalescing implementation.  The only
    accepted definitions are explicitly named ``_ZTI``/``_ZTS`` objects emitted as
    ``STB_LOPROC`` in a dedicated, non-empty ``.data`` section.  A caller must prove
    that another canonical definition is correct before applying this mechanical
    step; this function proves only that zeroing the selected sections and turning
    their symbols into imports cannot discard any neighbouring content.

    References by name from surviving sections are allowed -- importing the selected
    definition is the point.  A reference through an unnamed section symbol is not:
    after the section is emptied there is no symbol that can preserve its offset, so
    that shape is refused rather than guessed.
    """
    names = [str(name) for name in symbol_names if str(name)]
    if not names:
        return {"error": "no vague RTTI symbols were requested"}
    if len(names) != len(set(names)):
        return {"error": "duplicate vague RTTI symbol was requested"}
    wanted = set(names)

    elf = ELFFile(io.BytesIO(bytes(raw)))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return {"error": "no .symtab"}
    syms = list(symtab.iter_symbols())

    by_name = {name: [s for s in syms
                      if s.name == name
                      and s["st_shndx"] not in ("SHN_UNDEF", "SHN_ABS")]
               for name in wanted}
    missing = sorted(name for name, matches in by_name.items() if not matches)
    if missing:
        return {"error": f"vague RTTI symbol(s) not defined: {missing}"}
    ambiguous = sorted(name for name, matches in by_name.items() if len(matches) != 1)
    if ambiguous:
        return {"error": f"vague RTTI symbol(s) have multiple definitions: {ambiguous}"}

    drop_content = set()
    for name in sorted(wanted):
        sym = by_name[name][0]
        if not name.startswith(("_ZTI", "_ZTS")):
            return {"error": f"{name}: only exact _ZTI/_ZTS RTTI objects may be externalized"}
        if sym["st_info"]["bind"] != "STB_LOPROC" \
                or sym["st_info"]["type"] != "STT_OBJECT":
            return {"error": f"{name}: expected STB_LOPROC/STT_OBJECT, got "
                    f"{sym['st_info']['bind']}/{sym['st_info']['type']}"}
        shndx = sym["st_shndx"]
        if not isinstance(shndx, int) or shndx >= len(secs):
            return {"error": f"{name}: definition is not in an ordinary section"}
        sec = secs[shndx]
        if sec.name != ".data" or sec.header["sh_type"] != "SHT_PROGBITS" \
                or not sec.header["sh_size"]:
            return {"error": f"{name}: expected a non-empty .data/SHT_PROGBITS section"}
        if sym["st_value"] != 0 or sym["st_size"] != sec.header["sh_size"]:
            return {"error": f"{name}: definition does not exactly cover its section "
                    f"(value=0x{sym['st_value']:x}, symbol=0x{sym['st_size']:x}, "
                    f"section=0x{sec.header['sh_size']:x})"}
        occupants = sorted(s.name for s in syms
                           if s.name and not s.name.startswith("$")
                           and s["st_shndx"] == shndx
                           and s["st_info"]["type"] != "STT_SECTION")
        if occupants != [name]:
            return {"error": f"{name}: section[{shndx}] also defines "
                    f"{[n for n in occupants if n != name]}"}
        if shndx in drop_content:
            return {"error": f"{name}: policy symbols share section[{shndx}]"}
        drop_content.add(shndx)

    referenced = set()
    for rel in secs:
        if not isinstance(rel, RelocationSection) or rel.header["sh_info"] in drop_content:
            continue
        source = secs[rel.header["sh_info"]]
        if source.header["sh_type"] not in CONTENT or not source.header["sh_size"]:
            continue
        for r in rel.iter_relocations():
            target = symtab.get_symbol(r["r_info_sym"])
            if not target.name and target["st_shndx"] in drop_content:
                return {"error": f"surviving section[{rel.header['sh_info']}] "
                        f"{source.name} references unnamed section[{target['st_shndx']}] "
                        f"at 0x{r['r_offset']:x}"}
            if target.name in wanted:
                referenced.add(target.name)

    drop = set(drop_content)
    drop.update(i for i, sec in enumerate(secs)
                if isinstance(sec, RelocationSection) and sec.header["sh_size"]
                and sec.header["sh_info"] in drop_content)
    return {"keep": None, "drop": sorted(drop), "externalise": sorted(wanted),
            "dead": [], "referenced": sorted(referenced), "error": None}


def derive_externalized(raw, symbol_names):
    """Return an object importing exact dedicated vague RTTI definitions."""
    p = externalize_plan(raw, symbol_names)
    if p.get("error"):
        return None, p
    return _apply(raw, p, None), p


def section_partition_plan(raw, keep_section_names, licensed_symbols, deferred_outputs):
    """Plan a scratch object containing only explicitly licensed input sections.

    This is the non-text half of TU partitioned linking.  It is deliberately narrower
    than a general linker garbage collector: callers name exact ELF input-section
    names and every non-mapping symbol those surviving sections may define.  All
    other content is removed, while named references from a survivor into removed
    content become imports so separately-derived text objects can satisfy them.

    Repeated input sections are kept as a group because an LCF ``file.o(.data)``
    selector consumes every section with that name in compiler order.  Consequently
    one foreign definition in any same-named section is fatal; retaining it would
    silently widen the manifest's ownership claim.  A relocation through an unnamed
    section symbol into removed content is also fatal because no named import can
    preserve that address.
    """
    names = [str(name) for name in keep_section_names if str(name)]
    allowed = {str(name) for name in licensed_symbols if str(name)}
    if not names:
        return {"error": "no input section names were requested"}
    if len(names) != len(set(names)):
        return {"error": "duplicate input section name was requested"}

    elf = ELFFile(io.BytesIO(bytes(raw)))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return {"error": "no .symtab"}
    syms = list(symtab.iter_symbols())

    keep = {i for i, sec in enumerate(secs)
            if sec.name in set(names) and sec.header["sh_type"] in CONTENT
            and sec.header["sh_size"]}
    missing = sorted(name for name in names
                     if not any(secs[i].name == name for i in keep))
    if missing:
        return {"error": f"requested input section(s) are not emitted: {missing}"}

    occupants = sorted({sym.name for sym in syms
                        if sym.name and not sym.name.startswith("$")
                        and sym["st_info"]["type"] != "STT_SECTION"
                        and sym["st_shndx"] in keep})
    foreign = sorted(set(occupants) - allowed)
    missing_licensed = sorted(allowed - set(occupants))
    if foreign:
        return {"error": f"kept input section(s) also define unlicensed symbol(s): "
                          f"{foreign}"}
    if missing_licensed:
        return {"error": f"licensed symbol(s) are not defined in kept input sections: "
                          f"{missing_licensed}"}

    drop_content = {i for i, sec in enumerate(secs)
                    if sec.header["sh_type"] in CONTENT and sec.header["sh_size"]
                    and i not in keep
                    and not any(sec.name.startswith(p) for p in IGNORE)}

    # Every other live byte must be one exact, declared text contribution that the
    # caller has already proved contribution-equivalent.  Policies applied before
    # this helper have zero-sized compiler-only/externally-owned sections, so they do
    # not appear here.  This inventory prevents an unexpected helper/global or an
    # anonymous padding section from being silently discarded by the partition.
    deferred = []
    for index, row in enumerate(deferred_outputs or []):
        if not isinstance(row, dict) or not row.get("symbol") \
                or not row.get("section") or "size" not in row:
            return {"error": f"deferred_outputs[{index}] needs symbol/section/size"}
        try:
            size = int(row["size"], 0) if isinstance(row["size"], str) \
                else int(row["size"])
        except (TypeError, ValueError):
            return {"error": f"deferred_outputs[{index}] has invalid size"}
        if size <= 0:
            return {"error": f"deferred_outputs[{index}] has non-positive size"}
        deferred.append({"symbol": str(row["symbol"]),
                         "section": str(row["section"]), "size": size})
    deferred_by_name = {row["symbol"]: row for row in deferred}
    if len(deferred_by_name) != len(deferred):
        return {"error": "duplicate deferred-output symbol was requested"}

    seen_deferred = set()
    for shndx in sorted(drop_content):
        sec = secs[shndx]
        sec_occupants = [sym for sym in syms
                         if sym.name and not sym.name.startswith("$")
                         and sym["st_info"]["type"] != "STT_SECTION"
                         and sym["st_shndx"] == shndx]
        names_here = sorted(sym.name for sym in sec_occupants)
        if len(sec_occupants) != 1 or names_here[0] not in deferred_by_name:
            return {"error": f"foreign content section[{shndx}] {sec.name} size "
                    f"0x{sec.header['sh_size']:x} has occupants {names_here}; expected "
                    "one exact deferred output"}
        sym = sec_occupants[0]
        expected = deferred_by_name[sym.name]
        if sec.name != expected["section"] or sec.header["sh_type"] != "SHT_PROGBITS":
            return {"error": f"deferred {sym.name} is in {sec.name}/"
                    f"{sec.header['sh_type']}, expected "
                    f"{expected['section']}/SHT_PROGBITS"}
        if sym["st_info"]["type"] != "STT_FUNC" or sym["st_value"] != 0 \
                or sym["st_size"] != expected["size"] \
                or sec.header["sh_size"] != expected["size"]:
            return {"error": f"deferred {sym.name} does not exactly cover section"
                    f"[{shndx}] (type={sym['st_info']['type']}, value="
                    f"0x{sym['st_value']:x}, symbol=0x{sym['st_size']:x}, section="
                    f"0x{sec.header['sh_size']:x}, expected=0x{expected['size']:x})"}
        seen_deferred.add(sym.name)
    missing_deferred = sorted(set(deferred_by_name) - seen_deferred)
    if missing_deferred:
        return {"error": f"declared deferred output(s) are not exact live sections: "
                          f"{missing_deferred}"}

    drop = set(drop_content)
    drop.update(i for i, sec in enumerate(secs)
                if isinstance(sec, RelocationSection) and sec.header["sh_size"]
                and sec.header["sh_info"] in drop_content)

    referenced = set()
    for relsec in secs:
        if not isinstance(relsec, RelocationSection) \
                or relsec.header["sh_info"] not in keep:
            continue
        for reloc in relsec.iter_relocations():
            target = symtab.get_symbol(reloc["r_info_sym"])
            if not target.name and target["st_shndx"] in drop_content:
                return {"error": f"surviving section[{relsec.header['sh_info']}] "
                        f"{secs[relsec.header['sh_info']].name} references unnamed "
                        f"section[{target['st_shndx']}] at 0x{reloc['r_offset']:x}"}
            if target.name and target["st_shndx"] in drop_content:
                if target["st_info"]["bind"] == "STB_LOCAL":
                    return {"error": f"surviving section[{relsec.header['sh_info']}] "
                            f"{secs[relsec.header['sh_info']].name} references local "
                            f"symbol {target.name} in dropped section"
                            f"[{target['st_shndx']}]"}
                referenced.add(target.name)

    dead = sorted(sym.name for sym in syms
                  if sym.name and sym["st_shndx"] in drop_content
                  and sym.name not in referenced)
    return {"keep": None, "keeps": sorted(keep), "drop": sorted(drop),
            "externalise": sorted(referenced), "dead": dead,
            "referenced": sorted(referenced), "keptSectionNames": names,
            "licensedSymbols": sorted(allowed), "deferredOutputs": deferred,
            "error": None}


def derive_section_partition(raw, keep_section_names, licensed_symbols,
                             deferred_outputs):
    """Return a scratch object reduced to exact licensed input-section groups."""
    p = section_partition_plan(raw, keep_section_names, licensed_symbols,
                               deferred_outputs)
    if p.get("error"):
        return None, p
    return _apply(raw, p, None), p


def rebias_object_symbols(raw, symbol_policies):
    """Move exact retained object symbols from storage start to public address point.

    C++ vtable definitions are the motivating case.  mwcc's symbol covers the full
    object including its two-word ABI preamble, while this repository's configured
    ``_ZTV`` symbol names the slot-array address eight bytes later.  Function
    isolation already rewrites imports to that public convention; a separately linked
    data partition must therefore expose its strong definition at the same point.

    Content and relocations are deliberately untouched.  A policy may additionally
    split the preamble into an exact storage-alias symbol by reusing one explicitly
    deadstripped compiler-only symbol-table slot.  Reusing a slot keeps every ELF
    offset stable; it is allowed only when the old name is an unreferenced GLOBAL/FUNC
    import with enough exclusive string-table storage for the alias.
    """
    requested = {}
    for name, policy in dict(symbol_policies).items():
        if not isinstance(policy, dict):
            return None, {"rebased": [], "error": f"{name} rebias policy is not an object"}
        try:
            normalized = {
                "bias": int(policy["bias"]), "size": int(policy["size"]),
                "section": str(policy["section"]),
            }
            alias = policy.get("storageAlias")
            if alias is not None:
                normalized["storageAlias"] = {
                    "symbol": str(alias["symbol"]),
                    "donor": str(alias["donor"]),
                    "size": int(alias["size"]),
                }
            requested[str(name)] = normalized
        except (KeyError, TypeError, ValueError):
            return None, {"rebased": [], "aliases": [],
                          "error": f"{name} needs bias/size/section and valid optional "
                                   "storageAlias fields"}
    if not requested:
        return bytes(raw), {"rebased": [], "aliases": [], "error": None}
    bad_bias = sorted(name for name, policy in requested.items()
                      if policy["bias"] <= 0 or policy["bias"] >= policy["size"])
    if bad_bias:
        return None, {"rebased": [], "aliases": [],
                      "error": f"symbol rebias must be positive: {bad_bias}"}

    raw_out = bytearray(raw)
    elf = ELFFile(io.BytesIO(bytes(raw_out)))
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return None, {"rebased": [], "aliases": [], "error": "no .symtab"}
    syms = list(symtab.iter_symbols())
    protected_sections = {
        i: sec.data() for i, sec in enumerate(elf.iter_sections())
        if ((sec.header["sh_type"] in CONTENT and sec.header["sh_size"])
            or (isinstance(sec, RelocationSection) and sec.header["sh_size"]))
    }
    by_name = {name: [(i, sym) for i, sym in enumerate(syms)
                      if sym.name == name and sym["st_shndx"] not in
                      ("SHN_UNDEF", "SHN_ABS")]
               for name in requested}
    for name, matches in by_name.items():
        if len(matches) != 1:
            return None, {"rebased": [], "aliases": [],
                          "error": f"{name} has {len(matches)} defined "
                          "symbols, expected one"}
        _index, sym = matches[0]
        if not name.startswith("_ZTV"):
            return None, {"rebased": [], "aliases": [],
                          "error": f"{name} is not an exact _ZTV symbol"}
        if sym["st_info"]["bind"] != "STB_GLOBAL" \
                or sym["st_info"]["type"] != "STT_OBJECT":
            return None, {"rebased": [], "aliases": [], "error": f"{name} is "
                          f"{sym['st_info']['bind']}/{sym['st_info']['type']}, expected "
                          "STB_GLOBAL/STT_OBJECT"}
        shndx = sym["st_shndx"]
        sec = elf.get_section(shndx) if isinstance(shndx, int) else None
        if sec is None or sec.name != requested[name]["section"] \
                or sec.header["sh_type"] != "SHT_PROGBITS" or not sec.header["sh_size"]:
            return None, {"rebased": [], "aliases": [],
                          "error": f"{name} is not in retained "
                          f"{requested[name]['section']}/SHT_PROGBITS content"}
        if sym["st_value"] != 0 or sym["st_size"] != sec.header["sh_size"] \
                or sym["st_size"] != requested[name]["size"]:
            return None, {"rebased": [], "aliases": [],
                          "error": f"{name} does not exactly cover its "
                          f"preverified storage section (value=0x{sym['st_value']:x}, "
                          f"symbol=0x{sym['st_size']:x}, section="
                          f"0x{sec.header['sh_size']:x}, manifest="
                          f"0x{requested[name]['size']:x})"}

    # Rebiasing changes what a relocation to this definition means. No current owned
    # data needs such a self-reference, so refuse it rather than guessing whether its
    # addend used storage-start or public-address-point convention.
    for relsec in elf.iter_sections():
        if not isinstance(relsec, RelocationSection):
            continue
        source = elf.get_section(relsec.header["sh_info"])
        if source.header["sh_type"] not in CONTENT or not source.header["sh_size"]:
            continue
        for reloc in relsec.iter_relocations():
            target = symtab.get_symbol(reloc["r_info_sym"])
            if target.name in requested:
                return None, {"rebased": [], "aliases": [],
                              "error": f"surviving {source.name} "
                              f"relocation at 0x{reloc['r_offset']:x} targets rebased "
                              f"symbol {target.name}"}

    aliases = {}
    used_donors = set()
    existing_names = {sym.name for sym in syms if sym.name}
    for vtable_name, policy in requested.items():
        alias = policy.get("storageAlias")
        if alias is None:
            continue
        alias_name, donor_name = alias["symbol"], alias["donor"]
        if not alias_name or alias_name.startswith("_ZTV"):
            return None, {"rebased": [], "aliases": [],
                          "error": f"{vtable_name} storage alias has invalid name "
                                   f"{alias_name!r}"}
        if alias["size"] != policy["bias"]:
            return None, {"rebased": [], "aliases": [],
                          "error": f"{alias_name} size 0x{alias['size']:x} does not equal "
                                   f"{vtable_name} bias 0x{policy['bias']:x}"}
        if alias_name in existing_names:
            return None, {"rebased": [], "aliases": [],
                          "error": f"storage alias {alias_name} already exists"}
        if donor_name in used_donors:
            return None, {"rebased": [], "aliases": [],
                          "error": f"storage alias donor {donor_name} is reused"}
        donors = [(i, sym) for i, sym in enumerate(syms) if sym.name == donor_name]
        if len(donors) != 1:
            return None, {"rebased": [], "aliases": [],
                          "error": f"storage alias donor {donor_name} has {len(donors)} "
                                   "symbol-table slots, expected one"}
        donor_index, donor = donors[0]
        if donor["st_shndx"] not in ("SHN_UNDEF", SHN_UNDEF) \
                or donor["st_value"] != 0 or donor["st_size"] != 0 \
                or donor["st_info"]["bind"] != "STB_GLOBAL" \
                or donor["st_info"]["type"] != "STT_FUNC" \
                or donor["st_other"]["visibility"] != "STV_DEFAULT":
            return None, {"rebased": [], "aliases": [],
                          "error": f"storage alias donor {donor_name} is not an exact "
                                   "deadstripped GLOBAL/FUNC import"}
        for relsec in elf.iter_sections():
            if not isinstance(relsec, RelocationSection):
                continue
            source = elf.get_section(relsec.header["sh_info"])
            if source.header["sh_type"] not in CONTENT or not source.header["sh_size"]:
                continue
            if any(reloc["r_info_sym"] == donor_index
                   for reloc in relsec.iter_relocations()):
                return None, {"rebased": [], "aliases": [],
                              "error": f"storage alias donor {donor_name} is still "
                                       "referenced by surviving content"}
        donor_bytes = donor_name.encode("ascii", errors="strict")
        alias_bytes = alias_name.encode("ascii", errors="strict")
        if len(alias_bytes) > len(donor_bytes):
            return None, {"rebased": [], "aliases": [],
                          "error": f"storage alias {alias_name} does not fit donor "
                                   f"{donor_name}'s string-table slot"}
        donor_name_offset = donor["st_name"]
        overlapping = [(i, sym.name) for i, sym in enumerate(syms)
                       if i != donor_index and isinstance(sym["st_name"], int)
                       and donor_name_offset <= sym["st_name"] <=
                       donor_name_offset + len(donor_bytes)]
        if overlapping:
            return None, {"rebased": [], "aliases": [],
                          "error": f"storage alias donor {donor_name} has shared or "
                                   f"substring string-table users {overlapping}"}
        string_table = elf.get_section(symtab.header["sh_link"])
        string_offset = string_table.header["sh_offset"] + donor_name_offset
        if donor_name_offset == 0 or raw_out[string_offset - 1] != 0:
            return None, {"rebased": [], "aliases": [],
                          "error": f"storage alias donor {donor_name} does not start at "
                                   "an exclusive string-table boundary"}
        if bytes(raw_out[string_offset:string_offset + len(donor_bytes) + 1]) \
                != donor_bytes + b"\0":
            return None, {"rebased": [], "aliases": [],
                          "error": f"storage alias donor {donor_name} does not occupy "
                                   "the expected exclusive string-table bytes"}
        aliases[vtable_name] = {**alias, "index": donor_index,
                                "nameOffset": donor_name_offset,
                                "stringOffset": string_offset,
                                "donorBytes": donor_bytes, "aliasBytes": alias_bytes}
        used_donors.add(donor_name)

    mutable_symbol_indices = {index for index, _sym in
                              (by_name[name][0] for name in requested)}
    mutable_symbol_indices.update(alias["index"] for alias in aliases.values())
    protected_symbols = {
        i: (sym.name, sym["st_name"], sym["st_value"], sym["st_size"],
            sym["st_info"]["bind"], sym["st_info"]["type"],
            sym["st_other"]["visibility"], sym["st_shndx"])
        for i, sym in enumerate(syms) if i not in mutable_symbol_indices
    }

    import struct
    endian = "<" if elf.little_endian else ">"
    base = symtab.header["sh_offset"]
    rows, alias_rows = [], []
    for name in sorted(requested):
        index, sym = by_name[name][0]
        bias = requested[name]["bias"]
        alias = aliases.get(name)
        if alias is not None:
            string_offset = alias["stringOffset"]
            raw_out[string_offset:string_offset + len(alias["donorBytes"])] = \
                alias["aliasBytes"] + b"\0" * (len(alias["donorBytes"])
                                                 - len(alias["aliasBytes"]))
            donor_ent = base + alias["index"] * 16
            struct.pack_into(endian + "I", raw_out, donor_ent + 4, 0)
            struct.pack_into(endian + "I", raw_out, donor_ent + 8, alias["size"])
            raw_out[donor_ent + 12] = 0x11  # STB_GLOBAL/STT_OBJECT
            struct.pack_into(endian + "H", raw_out, donor_ent + 14,
                             sym["st_shndx"])
            alias_rows.append({"symbol": alias["symbol"], "donor": alias["donor"],
                               "value": 0, "size": alias["size"],
                               "section": requested[name]["section"]})
        ent = base + index * 16
        new_value = sym["st_value"] + bias
        new_size = sym["st_size"] - bias
        struct.pack_into(endian + "I", raw_out, ent + 4, new_value)
        struct.pack_into(endian + "I", raw_out, ent + 8, new_size)
        rows.append({"symbol": name, "bias": bias,
                     "oldValue": sym["st_value"], "newValue": new_value,
                     "oldSize": sym["st_size"], "newSize": new_size})
    out = bytes(raw_out)
    checked = ELFFile(io.BytesIO(out))
    checked_symtab = checked.get_section_by_name(".symtab")
    checked_symbols = list(checked_symtab.iter_symbols())
    checked_protected = {
        i: (sym.name, sym["st_name"], sym["st_value"], sym["st_size"],
            sym["st_info"]["bind"], sym["st_info"]["type"],
            sym["st_other"]["visibility"], sym["st_shndx"])
        for i, sym in enumerate(checked_symbols) if i not in mutable_symbol_indices
    }
    if checked_protected != protected_symbols:
        return None, {"rebased": rows, "aliases": alias_rows,
                      "error": "storage rewrite changed a non-policy symbol"}
    checked_sections = {
        i: sec.data() for i, sec in enumerate(checked.iter_sections())
        if i in protected_sections
    }
    if checked_sections != protected_sections:
        return None, {"rebased": rows, "aliases": alias_rows,
                      "error": "storage rewrite changed content or relocation bytes"}
    for name, alias in aliases.items():
        matches = [sym for sym in checked_symbols
                   if sym.name == alias["symbol"]]
        vtables = [sym for sym in checked_symbols if sym.name == name]
        if len(matches) != 1 or len(vtables) != 1:
            return None, {"rebased": rows, "aliases": alias_rows,
                          "error": f"post-rewrite alias/vtable uniqueness failed for "
                                   f"{alias['symbol']}/{name}"}
        got, vtable = matches[0], vtables[0]
        if got["st_info"]["bind"] != "STB_GLOBAL" \
                or got["st_info"]["type"] != "STT_OBJECT" \
                or got["st_value"] != 0 or got["st_size"] != alias["size"] \
                or got["st_shndx"] != vtable["st_shndx"] \
                or vtable["st_value"] != alias["size"]:
            return None, {"rebased": rows, "aliases": alias_rows,
                          "error": f"post-rewrite storage split is not exact for "
                                   f"{alias['symbol']}/{name}"}
    return out, {"rebased": rows, "aliases": alias_rows, "error": None}


def isolate(obj, keep_symbol):
    """Apply `plan` to the object file in place. Returns the plan.

    Idempotent: an object with nothing left to drop is rewritten to the same bytes,
    so a cached object processed by an earlier build is not corrupted by a later
    one."""
    raw = obj.read_bytes()
    out, p = derive(raw, keep_symbol)
    if p.get("error") or not (p["drop"] or p["externalise"]):
        return p
    obj.write_bytes(out)
    return p


def isolate_many(obj, keep_symbols):
    """Validate and preserve an exact text-only multi-function object in place."""
    raw = obj.read_bytes()
    out, p = derive_many(raw, keep_symbols)
    if out is not None and out != raw:
        obj.write_bytes(out)
    return p


def _apply(raw, p, keep_symbol):
    """The reduction itself: header surgery on a copy of `raw`, per plan `p`."""
    raw = bytearray(raw)
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
    # The -8 address-point rewrite is a legacy *function-isolation* rule.  A retained
    # data section may itself describe an ABI vtable object whose relocation addend 8
    # is part of the verified ROM data (for example an _ZTI record's ABI-vtable link).
    # Section partitioning therefore never opts into this rewrite: it must preserve
    # the already-verified data relocation stream byte-for-byte.
    # A plan may name the sections to correct outright (deadstrip_plan does, because
    # it keeps many and drops the definitions out from under them).  Absent that the
    # legacy rule stands: the single kept function's section, and nothing else.
    keeps = p.get("rebase")
    if keeps is None:
        keeps = {p["keep"]} if isinstance(p.get("keep"), int) else set()
    keeps = set(keeps)
    for s in elf.iter_sections():
        if not isinstance(s, RelocationSection) or s.header["sh_info"] not in keeps:
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

    return bytes(raw)
