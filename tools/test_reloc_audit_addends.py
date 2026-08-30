"""A data reloc's RELA addend is part of its destination.

Regression test for a false WRONG-DEST. `object_reloc_dests` resolved every
non-`_ZTV` relocation by symbol NAME alone, but config records the DESTINATION
the linked word points to, and mwccarm encodes any base+offset access (a struct
field, an array element, a strength-reduced pointer into a global) as the
symbol's base plus a nonzero RELA addend. Dropping the addend compared the base
against config's base+offset and reported WRONG-DEST on byte-exact code.
Observed live: func_ov075_02119dc4 relocates data_0209fc5c with addend 1,
config records 0x0209fc5d:arm9, linkcheck says VERIFIED -- and
match.py --strict-relocs called it NOT a match. linkcheck.func_relocs_typed had
already documented the rule for the linking side ("Reading only the base would
mis-link those slots and report a correct source as WRONG"); this pins the
destination-compare side.

Two addend shapes must NOT get the straight add and are pinned here too:
branch relocs, whose addend is PC-bias encoding (-8), not addressing; and
`_ZTV<C>` data relocs, whose addend carries mwcc's vtable preamble and keeps
the vt_form arithmetic object_reloc_dests documents.

No compiler and no ROM: the object is a hand-built ELF, same as
test_reloc_audit_sections.py.
"""
import io
import pathlib
import struct
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

from elftools.elf.elffile import ELFFile  # noqa: E402

import reloc_audit as RA  # noqa: E402

SHT_PROGBITS, SHT_SYMTAB, SHT_STRTAB, SHT_RELA = 1, 2, 3, 4
STT_FUNC, STB_GLOBAL = 2, 1
R_ARM_ABS32, R_ARM_CALL = 2, 28

FUNC = "func_ov075_02119dc4"
FUNC_ADDR = 0x02119DC4
DATA_BASE = 0x0209FC5C          # data_0209fc5c in symbols.txt
CFG_DEST = 0x0209FC5D           # config's recorded destination: base + addend 1
ZTV_SLOTS = 0x02100000          # symbols.txt _ZTV: already the slot array

NAME_INDEX = {"data_0209fc5c": ("arm9", DATA_BASE),
              "_ZTV5Dummy": ("arm9", ZTV_SLOTS)}


def _strtab(names):
    """(blob, {name: offset}) for an ELF string table."""
    blob, off = b"\0", {"": 0}
    for n in names:
        off[n] = len(blob)
        blob += n.encode() + b"\0"
    return blob, off


def _elf_with_addend_relocs():
    """An ET_REL object holding one function with three RELA relocations:

      +0x0  R_ARM_CALL   func_02001000   addend -8   (PC bias, not addressing)
      +0x8  R_ARM_ABS32  data_0209fc5c   addend  1   (the shape that misread)
      +0xc  R_ARM_ABS32  _ZTV5Dummy      addend  8   (vtable preamble)

    The +0x8 entry is func_ov075_02119dc4's exact shape: a pool word addressing
    one byte past a named global.
    """
    secnames = [".shstrtab", ".strtab", ".symtab", ".text", ".rela.text"]
    shstr, shoff = _strtab(secnames)
    symnames = ["data_0209fc5c", "func_02001000", "_ZTV5Dummy", FUNC]
    strtab, stroff = _strtab(symnames)

    # bl <ext>; bx lr; two literal-pool words
    text = struct.pack("<4I", 0xEBFFFFFE, 0xE12FFF1E, 0, 0)

    def sym(name_off, value, size, info, shndx):
        return struct.pack("<IIIBBH", name_off, value, size, info, 0, shndx)

    # symtab: null, data_0209fc5c(1), func_02001000(2), _ZTV5Dummy(3), FUNC(4)
    symtab = (sym(0, 0, 0, 0, 0)
              + sym(stroff["data_0209fc5c"], 0, 0, (STB_GLOBAL << 4), 0)
              + sym(stroff["func_02001000"], 0, 0, (STB_GLOBAL << 4), 0)
              + sym(stroff["_ZTV5Dummy"], 0, 0, (STB_GLOBAL << 4), 0)
              + sym(stroff[FUNC], 0, len(text), (STB_GLOBAL << 4) | STT_FUNC, 4))

    rela = (struct.pack("<IIi", 0x0, (2 << 8) | R_ARM_CALL, -8)
            + struct.pack("<IIi", 0x8, (1 << 8) | R_ARM_ABS32, 1)
            + struct.pack("<IIi", 0xC, (3 << 8) | R_ARM_ABS32, 8))

    blobs = [b"", shstr, strtab, symtab, text, rela]
    types = [0, SHT_STRTAB, SHT_STRTAB, SHT_SYMTAB, SHT_PROGBITS, SHT_RELA]
    names = ["", ".shstrtab", ".strtab", ".symtab", ".text", ".rela.text"]
    # sh_link: symtab -> strtab(2); rela -> symtab(3).  sh_info: rela -> its .text.
    links = [0, 0, 0, 2, 0, 3]
    infos = [0, 0, 0, 1, 0, 4]
    entsz = [0, 0, 0, 16, 0, 12]

    ehsize, shentsize, nsec = 52, 40, len(blobs)
    offsets, cur = [], ehsize
    for b in blobs:
        offsets.append(cur if b else 0)
        cur += len(b)
    shoff_start = cur

    eh = struct.pack(
        "<16sHHIIIIIHHHHHH",
        b"\x7fELF\x01\x01\x01" + b"\0" * 9,      # ELFCLASS32, ELFDATA2LSB, EV_CURRENT
        1,                                        # ET_REL
        40,                                       # EM_ARM
        1, 0, 0, shoff_start, 0,
        ehsize, 0, 0, shentsize, nsec, 1)         # e_shstrndx = 1

    out = bytearray(eh)
    for b in blobs:
        out += b
    for i in range(nsec):
        out += struct.pack("<10I", shoff[names[i]], types[i], 0, 0, offsets[i],
                           len(blobs[i]), links[i], infos[i], 4, entsz[i])
    return bytes(out)


def _dest_at(dests, off):
    return next(d for d in dests if d[0] == off)


def test_data_reloc_addend_lands_on_configs_destination():
    """The fix: base + addend, not base. Name-only resolution answered DATA_BASE
    here and the gate compared it against config's CFG_DEST."""
    dests, _ = RA.object_reloc_dests(_elf_with_addend_relocs(), FUNC, NAME_INDEX)
    assert _dest_at(dests, 0x8) == (0x8, "data_0209fc5c", "arm9", CFG_DEST), dests


def test_branch_addend_is_pc_bias_not_addressing():
    dests, _ = RA.object_reloc_dests(_elf_with_addend_relocs(), FUNC, NAME_INDEX)
    assert _dest_at(dests, 0x0) == (0x0, "func_02001000", None, 0x02001000), dests


def test_ztv_arithmetic_is_unchanged():
    """The special case the uniform add must not disturb: raw subtracts the
    preamble (addend 8 -> +0), isolated adds the addend as-is."""
    obj = _elf_with_addend_relocs()
    raw, _ = RA.object_reloc_dests(obj, FUNC, NAME_INDEX, vt_form="raw")
    assert _dest_at(raw, 0xC) == (0xC, "_ZTV5Dummy", "arm9", ZTV_SLOTS), raw
    iso, _ = RA.object_reloc_dests(obj, FUNC, NAME_INDEX, vt_form="isolated")
    assert _dest_at(iso, 0xC) == (0xC, "_ZTV5Dummy", "arm9", ZTV_SLOTS + 8), iso
    # The other two rows must not depend on vt_form at all.
    assert [d for d in raw if d[0] != 0xC] == [d for d in iso if d[0] != 0xC]


def test_the_object_really_carries_the_addend():
    """Guard against a vacuous pass: if the builder stopped emitting a nonzero
    addend (or config's destination stopped differing from the symbol's base),
    the test above would hold under the old name-only code too."""
    elf = ELFFile(io.BytesIO(_elf_with_addend_relocs()))
    rela = next(s for s in elf.iter_sections() if s.header["sh_type"] == "SHT_RELA")
    r = next(r for r in rela.iter_relocations() if r["r_offset"] == 0x8)
    assert r["r_addend"] == 1
    assert NAME_INDEX["data_0209fc5c"][1] + r["r_addend"] == CFG_DEST
    assert NAME_INDEX["data_0209fc5c"][1] != CFG_DEST


def test_strict_gate_verdict_is_ok_end_to_end():
    """Through check_destinations, the way match.py --strict-relocs consumes it:
    the symbol+addend row must classify OK against config's recorded destination,
    where it used to come back WRONG-DEST and veto a byte-exact match."""
    rows, missing = RA.check_destinations(
        _elf_with_addend_relocs(), FUNC, FUNC_ADDR, 0x10, "arm9",
        name_index=NAME_INDEX,
        config_relocs={"arm9": {FUNC_ADDR + 0x8: ("word", CFG_DEST, "arm9")}},
        sym_index={})
    assert rows is not None, missing
    row = next(r for r in rows if r["off"] == "+0x8")
    assert row["verdict"] == "OK", row
    assert missing == 0
