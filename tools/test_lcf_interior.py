import pathlib
import struct
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import lcf_interior as LI


def _elf(defined=(), imported=(), local=()):
    """A minimal ELF32 relocatable carrying only a symbol table."""
    names = [*defined, *imported, *local]
    strtab = b"\0"
    offs = {}
    for n in names:
        offs[n] = len(strtab)
        strtab += n.encode() + b"\0"
    syms = b"\0" * 16
    for n in defined:
        syms += struct.pack("<IIIBBH", offs[n], 0, 4, (1 << 4) | 2, 0, 1)   # GLOBAL FUNC, shndx 1
    for n in imported:
        syms += struct.pack("<IIIBBH", offs[n], 0, 0, (1 << 4), 0, 0)       # GLOBAL, UNDEF
    for n in local:
        syms += struct.pack("<IIIBBH", offs[n], 0, 4, 2, 0, 1)              # LOCAL FUNC
    body_off = 52
    sym_off = body_off
    str_off = sym_off + len(syms)
    shoff = str_off + len(strtab)
    shdrs = b"\0" * 40
    shdrs += struct.pack("<IIIIIIIIII", 0, 2, 0, 0, sym_off, len(syms), 2, 1, 4, 16)
    shdrs += struct.pack("<IIIIIIIIII", 0, 3, 0, 0, str_off, len(strtab), 0, 0, 1, 0)
    hdr = b"\x7fELF\x01\x01\x01" + b"\0" * 9
    hdr += struct.pack("<HHIIIIIHHHHHH", 1, 40, 1, 0, 0, shoff, 0, 52, 0, 0, 40, 3, 0)
    return hdr + syms + strtab + shdrs


def _bss_elf(name="table", size=40, imported=()):
    """A NOBITS storage object plus imports, with no invented alias definition."""
    names = [name, *imported]
    strings = b"\0"
    offsets = {}
    for symbol in names:
        offsets[symbol] = len(strings)
        strings += symbol.encode() + b"\0"
    symbols = b"\0" * 16
    symbols += struct.pack("<IIIBBH", offsets[name], 0, size, 0x11, 0, 1)
    for symbol in imported:
        symbols += struct.pack("<IIIBBH", offsets[symbol], 0, 0, 0x10, 0, 0)
    symoff, stroff = 52, 52 + len(symbols)
    shoff = stroff + len(strings)
    headers = b"\0" * 40
    headers += struct.pack("<IIIIIIIIII", 0, 8, 3, 0, 52, size, 0, 0, 4, 0)
    headers += struct.pack("<IIIIIIIIII", 0, 2, 0, 0, symoff, len(symbols), 3, 1, 4, 16)
    headers += struct.pack("<IIIIIIIIII", 0, 3, 0, 0, stroff, len(strings), 0, 0, 1, 0)
    header = b"\x7fELF\x01\x01\x01" + b"\0" * 9
    header += struct.pack("<HHIIIIIHHHHHH", 1, 40, 1, 0, 0, shoff, 0, 52, 0, 0, 40, 4, 0)
    return header + symbols + strings + headers


SYMBOLS = (
    "func_01ff97d8 kind:function(arm,size=0x11c) addr:0x01ff97d8\n"
    "func_01ff98f4 kind:function(arm,size=0xb0) addr:0x01ff98f4\n"
    "func_01ff99a4 kind:function(arm,size=0x39c) addr:0x01ff99a4\n"
    "_deq kind:function(arm,size=0x0) addr:0x01ff9d40\n"
    "_deq_core kind:label(arm) addr:0x01ff9d50\n"
    "func_01ffa344 kind:function(arm,size=0xfc) addr:0x01ffa344\n"
)
DELINKS = (
    "    .text       start:0x01ff8000 end:0x01ffdf3c kind:code align:32\n"
    "\n"
    "src/unnamed/arm9/01ff/func_01ff97d8.c:\n"
    "    complete\n"
    "    .text start:0x01ff97d8 end:0x01ffa344\n"
    "\n"
    "src/unnamed/arm9/01ff/func_01ffa344.c:\n"
    "    .text start:0x01ffa344 end:0x01ffa440\n"
)
LCF = "MEMORY {\n}\nSECTIONS {\n    .text : {}\n}\n"


class LcfInteriorTests(unittest.TestCase):
    def setUp(self):
        self._td = tempfile.TemporaryDirectory()
        self.root = pathlib.Path(self._td.name)
        itcm = self.root / "config" / "itcm"
        itcm.mkdir(parents=True)
        (itcm / "symbols.txt").write_text(SYMBOLS)
        (itcm / "delinks.txt").write_text(DELINKS)

    def tearDown(self):
        self._td.cleanup()

    def _link(self, *objects):
        paths = []
        for i, data in enumerate(objects):
            p = self.root / f"o{i}.o"
            p.write_bytes(data)
            paths.append(f'"{p}"')
        objects_txt = self.root / "objects.txt"
        objects_txt.write_text("\n".join(paths) + "\n")
        lcf = self.root / "arm9.lcf"
        lcf.write_text(LCF)
        return lcf, objects_txt

    def test_only_complete_ranges_count(self):
        self.assertEqual(LI.complete_text_ranges(self.root / "config" / "itcm" / "delinks.txt"),
                         [(0x01ff97d8, 0x01ffa344)])

    def test_interior_rows_skip_the_range_start_but_keep_aliases_and_labels(self):
        self.assertEqual(LI.interior_rows(self.root / "config"),
                         [("func_01ff98f4", 0x01ff98f4), ("func_01ff99a4", 0x01ff99a4),
                          ("_deq", 0x01ff9d40), ("_deq_core", 0x01ff9d50)])

    def test_patch_defines_an_imported_interior_label(self):
        lcf, objects_txt = self._link(_elf(defined=["func_01ff97d8"]),
                                      _elf(defined=["func_0206cee0"], imported=["_deq_core"]))
        self.assertEqual(LI.patch(lcf, objects_txt, self.root / "config"),
                         [("_deq_core", 0x01ff9d50)])
        self.assertIn("    _deq_core = 0x01ff9d50;", lcf.read_text())

    def test_patch_defines_an_imported_zero_sized_alias(self):
        # func_0206f46c's `double ==` imports the runtime alias _deq, a size-0 row
        # inside func_01ff97d8's complete range; without a definition the link aborts.
        lcf, objects_txt = self._link(_elf(defined=["func_01ff97d8"]),
                                      _elf(defined=["func_0206f46c"], imported=["_deq"]))
        self.assertEqual(LI.patch(lcf, objects_txt, self.root / "config"),
                         [("_deq", 0x01ff9d40)])
        self.assertIn("    _deq = 0x01ff9d40;", lcf.read_text())

    def test_patch_defines_an_imported_sdk_linker_constant(self):
        # func_02058308 reads SDK_IRQ_STACKSIZE's address as its value (0x600).
        lcf, objects_txt = self._link(_elf(defined=["func_02058308"],
                                           imported=["SDK_IRQ_STACKSIZE"]))
        self.assertEqual(LI.patch(lcf, objects_txt, self.root / "config"),
                         [("SDK_IRQ_STACKSIZE", 0x600)])
        self.assertIn("    SDK_IRQ_STACKSIZE = 0x00000600;", lcf.read_text())

    def test_patch_leaves_a_linker_constant_nobody_imports(self):
        lcf, objects_txt = self._link(_elf(defined=["func_02058308"]))
        self.assertEqual(LI.patch(lcf, objects_txt, self.root / "config"), [])

    def test_patch_defines_only_imported_undefined_interior_rows(self):
        caller = _elf(defined=["func_ov002_020dc560"],
                      imported=["func_01ff99a4", "func_01ff98f4", "func_ov002_020dc000"])
        lcf, objects_txt = self._link(_elf(defined=["func_01ff97d8"]), caller)
        added = LI.patch(lcf, objects_txt, self.root / "config")
        self.assertEqual(added, [("func_01ff98f4", 0x01ff98f4), ("func_01ff99a4", 0x01ff99a4)])
        self.assertEqual(lcf.read_text(),
                         "MEMORY {\n}\nSECTIONS {\n"
                         "    func_01ff98f4 = 0x01ff98f4;\n"
                         "    func_01ff99a4 = 0x01ff99a4;\n"
                         "    .text : {}\n}\n")

    def test_patch_leaves_a_resolving_link_untouched(self):
        lcf, objects_txt = self._link(_elf(defined=["func_01ff98f4"]),
                                      _elf(imported=["func_01ff98f4"]))
        self.assertEqual(LI.patch(lcf, objects_txt, self.root / "config"), [])
        self.assertEqual(lcf.read_text(), LCF)

    def test_local_symbol_is_not_a_definition(self):
        lcf, objects_txt = self._link(_elf(local=["func_01ff98f4"]),
                                      _elf(imported=["func_01ff98f4"]))
        self.assertEqual(LI.patch(lcf, objects_txt, self.root / "config"),
                         [("func_01ff98f4", 0x01ff98f4)])

    def test_stb_loproc_counts_as_a_definition(self):
        data = bytearray(_elf(defined=["func_01ff98f4"]))
        info_at = 52 + 16 + 12                         # first real symbol's st_info
        data[info_at] = (13 << 4) | 2                  # STB_LOPROC, FUNC
        defined, _imported = LI._elf_names(bytes(data))
        self.assertIn("func_01ff98f4", defined)

    def test_crlf_lcf_keeps_its_line_endings(self):
        lcf, objects_txt = self._link(_elf(imported=["func_01ff98f4"]))
        lcf.write_bytes(LCF.replace("\n", "\r\n").encode())
        LI.patch(lcf, objects_txt, self.root / "config")
        self.assertIn(b"SECTIONS {\r\n    func_01ff98f4 = 0x01ff98f4;\r\n", lcf.read_bytes())


BSS_LCF = LCF.replace("    .text : {}", "    .text : {}\n    .ov006 : {\n    }")


class BssInteriorTests(unittest.TestCase):
    def _link(self, *objects):
        lcf, paths = LcfInteriorTests._link(self, *objects)
        lcf.write_text(BSS_LCF)
        return lcf, paths
    tearDown = LcfInteriorTests.tearDown

    def setUp(self):
        LcfInteriorTests.setUp(self)
        self.module = self.root / "config" / "overlays" / "ov006"
        self.module.mkdir(parents=True)
        self.symbols = self.module / "symbols.txt"
        self.delinks = self.module / "delinks.txt"
        self.symbols.write_text("table kind:bss addr:0x02142df8\n"
                                "inside kind:bss addr:0x02142e02\n")
        self.delinks.write_text("src/table.cpp:\n    complete\n"
                                "    .bss start:0x02142cc0 end:0x02142e88\n")

    def test_live_overlay_bss_alias_has_canonical_address_in_its_output_module(self):
        raw = _bss_elf(imported=["inside"])
        lcf, objects = self._link(raw)
        self.assertEqual(LI.patch(lcf, objects, self.root / "config"),
                         [("inside", 0x02142e02)])
        self.assertIn("    .ov006 : {\n        inside = 0x02142e02;", lcf.read_text())
        self.assertEqual((self.root / "o0.o").read_bytes(), raw)

    def test_bss_alias_requires_a_complete_source_owned_extent(self):
        lcf, objects = self._link(_bss_elf(imported=["inside"]))
        for text in ("    .bss start:0x02142cc0 end:0x02142e88\n",
                     "src/table.cpp:\n    .bss start:0x02142cc0 end:0x02142e88\n",
                     "src/table.cpp:\n    complete\n    .bss start:0x02142e00 end:0x02142e88\n"):
            with self.subTest(text=text):
                self.delinks.write_text(text)
                self.assertEqual(LI.patch(lcf, objects, self.root / "config"), [])
                self.assertEqual(lcf.read_text(), BSS_LCF)

    def test_bss_alias_requires_a_unique_owner_and_configured_home(self):
        for defined, size, extra in ((True, 8, ""), (False, 40, ""),
                                    (True, 40, "inside kind:bss addr:0x02142e04\n"),
                                    (True, 40, "table kind:data(any) addr:0x02142df8\n")):
            with self.subTest(defined=defined, size=size, extra=extra):
                self.symbols.write_text("table kind:bss addr:0x02142df8\n"
                                        "inside kind:bss addr:0x02142e02\n" + extra)
                obj = _bss_elf(size=size, imported=["inside"]) if defined else _elf(imported=["inside"])
                lcf, objects = self._link(obj)
                self.assertEqual(LI.patch(lcf, objects, self.root / "config"), [])

    def test_bss_alias_does_not_invent_a_label_at_or_past_the_owner_end(self):
        for address in (0x02142df8, 0x02142e20, 0x02142e24):
            with self.subTest(address=address):
                self.symbols.write_text("table kind:bss addr:0x02142df8\n"
                                        f"inside kind:bss addr:0x{address:08x}\n")
                lcf, objects = self._link(_bss_elf(imported=["inside"]))
                self.assertEqual(LI.patch(lcf, objects, self.root / "config"), [])

    def test_bss_alias_rejects_cross_overlay_name_ambiguity(self):
        other = self.root / "config" / "overlays" / "ov007"
        other.mkdir()
        (other / "symbols.txt").write_text("inside kind:bss addr:0x02142e02\n")
        lcf, objects = self._link(_bss_elf(imported=["inside"]))
        self.assertEqual(LI.patch(lcf, objects, self.root / "config"), [])

    def test_existing_alias_definition_and_duplicate_owners_are_not_replaced(self):
        for objects_in in ((_bss_elf(imported=["inside"]), _elf(defined=["inside"])),
                           (_bss_elf(imported=["inside"]), _bss_elf())):
            with self.subTest(count=len(objects_in)):
                lcf, objects = self._link(*objects_in)
                self.assertEqual(LI.patch(lcf, objects, self.root / "config"), [])


    def test_bss_alias_refuses_missing_or_ambiguous_output_module(self):
        for text in (LCF, BSS_LCF.replace("    .ov006 : {", "    .ov006 : { }\n    .ov006 : {")):
            with self.subTest(text=text):
                lcf, objects = self._link(_bss_elf(imported=["inside"]))
                lcf.write_text(text)
                before = lcf.read_bytes()
                with self.assertRaisesRegex(ValueError, "exactly one .ov006"):
                    LI.patch(lcf, objects, self.root / "config")
                self.assertEqual(lcf.read_bytes(), before)


if __name__ == "__main__":
    unittest.main()
