"""The strict-reloc gate must check the object the production link consumes, which for
a promoted TU with `defined_symbol_renames` is the RENAMED object.

mwcc numbers anonymous data (`@NNN`) per translation unit. When a fold makes a class
TU emit an `@NNN` another module already defines globally, the manifest licenses a
fleet-unique name and tu_production renames the definition before the link. The gate
used to resolve the compiled name instead: it looked `@653` up in symbols.txt, found
ov079's `@653`, and reported a correct ov002 `__sinit_d_path_lift_actor.cpp` WRONG.

No compiler and no ROM: the object is a hand-built ELF, one function `alpha` with
one ABS32 relocation to a defined data temp `@653`.
"""
import io
import pathlib
import struct
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import reloc_audit as RA  # noqa: E402
from elftools.elf.elffile import ELFFile  # noqa: E402

SHT_PROGBITS, SHT_SYMTAB, SHT_STRTAB, SHT_RELA = 1, 2, 3, 4
STT_OBJECT, STT_FUNC, STB_GLOBAL = 1, 2, 1
R_ARM_ABS32 = 2


def _strtab(names):
    blob, off = b"\0", {"": 0}
    for n in names:
        off[n] = len(blob)
        blob += n.encode() + b"\0"
    return blob, off


def _elf_with_temp(temp="@653"):
    """ET_REL: .text holds `alpha` (one word, relocated to `temp`); .data holds `temp`."""
    secnames = [".shstrtab", ".strtab", ".symtab", ".text", ".rela.text", ".data"]
    shstr, shoff = _strtab(secnames)
    strtab, stroff = _strtab([temp, "alpha"])

    def sym(name_off, value, size, info, shndx):
        return struct.pack("<IIIBBH", name_off, value, size, info, 0, shndx)

    text = struct.pack("<I", 0)
    data = struct.pack("<I", 0x12345678)
    symtab = (sym(0, 0, 0, 0, 0)
              + sym(stroff[temp], 0, 4, (STB_GLOBAL << 4) | STT_OBJECT, 6)
              + sym(stroff["alpha"], 0, 4, (STB_GLOBAL << 4) | STT_FUNC, 4))
    rela = struct.pack("<III", 0x0, (1 << 8) | R_ARM_ABS32, 0)

    blobs = [b"", shstr, strtab, symtab, text, rela, data]
    types = [0, SHT_STRTAB, SHT_STRTAB, SHT_SYMTAB, SHT_PROGBITS, SHT_RELA, SHT_PROGBITS]
    names = ["", ".shstrtab", ".strtab", ".symtab", ".text", ".rela.text", ".data"]
    links = [0, 0, 0, 2, 0, 3, 0]
    infos = [0, 0, 0, 1, 0, 4, 0]
    entsz = [0, 0, 0, 16, 0, 12, 0]

    ehsize, shentsize, nsec = 52, 40, len(blobs)
    offsets, cur = [], ehsize
    for b in blobs:
        offsets.append(cur if b else 0)
        cur += len(b)
    eh = struct.pack("<16sHHIIIIIHHHHHH", b"\x7fELF\x01\x01\x01" + b"\0" * 9,
                     1, 40, 1, 0, 0, cur, 0, ehsize, 0, 0, shentsize, nsec, 1)
    out = bytearray(eh)
    for b in blobs:
        out += b
    for i in range(nsec):
        out += struct.pack("<10I", shoff[names[i]], types[i], 0, 0, offsets[i],
                           len(blobs[i]), links[i], infos[i], 4, entsz[i])
    return bytes(out)


def _defined(obj):
    symtab = ELFFile(io.BytesIO(obj)).get_section_by_name(".symtab")
    return {s.name for s in symtab.iter_symbols() if s.name and s["st_shndx"] != "SHN_UNDEF"}


# `@653` is ANOTHER module's temp; `@999` is the name this TU's manifest licenses.
NAME_INDEX = {"@653": ("ov079", 0x02130000), "@999": ("ov002", 0x0210aef8)}


class ManifestRenames(unittest.TestCase):
    def setUp(self):
        self._saved = RA._RENAME_POLICIES

    def tearDown(self):
        RA._RENAME_POLICIES = self._saved

    def _policy(self, funcs, mapping):
        RA._RENAME_POLICIES = [(frozenset(funcs), dict(mapping))]

    def test_the_raw_object_resolves_to_the_other_modules_temp(self):
        """Without this, the test below could pass on an object with no hazard."""
        dests, _ = RA.object_reloc_dests(_elf_with_temp(), "alpha", NAME_INDEX)
        self.assertEqual(dests, [(0x0, "@653", "ov079", 0x02130000)])

    def test_a_licensed_rename_is_applied_before_resolving(self):
        self._policy({"alpha"}, {"@653": "@999"})
        obj = RA._with_manifest_renames(_elf_with_temp())
        self.assertIn("@999", _defined(obj))
        self.assertNotIn("@653", _defined(obj))
        dests, _ = RA.object_reloc_dests(obj, "alpha", NAME_INDEX)
        self.assertEqual(dests, [(0x0, "@999", "ov002", 0x0210aef8)])

    def test_a_policy_for_another_tu_is_not_applied(self):
        """A manifest renames only its own object: every function it lists must be here."""
        self._policy({"alpha", "beta"}, {"@653": "@999"})
        raw = _elf_with_temp()
        self.assertEqual(RA._with_manifest_renames(raw), raw)

    def test_a_policy_naming_an_undefined_temp_is_not_applied(self):
        self._policy({"alpha"}, {"@655": "@998"})
        raw = _elf_with_temp()
        self.assertEqual(RA._with_manifest_renames(raw), raw)

    def test_a_refused_rename_fails_closed(self):
        """A destination longer than the source cannot be renamed in place; the raw
        object is kept, so the gate keeps resolving to the other module and says WRONG."""
        self._policy({"alpha"}, {"@653": "@2162"})
        raw = _elf_with_temp()
        self.assertEqual(RA._with_manifest_renames(raw), raw)


if __name__ == "__main__":
    unittest.main()
