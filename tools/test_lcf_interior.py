import pathlib
import shutil
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
LCF = "MEMORY {\n}\nSECTIONS {\n    .itcm : {\n    } > itcm\n}\n"


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
        self.assertIn(("_deq_core", 0x01ff9d50),
                      LI.patch(lcf, objects_txt, self.root / "config"))
        self.assertIn("        _deq_core = 0x01ff9d50;", lcf.read_text())

    def test_patch_defines_an_imported_zero_sized_alias(self):
        # func_0206f46c's `double ==` imports the runtime alias _deq, a size-0 row
        # inside func_01ff97d8's complete range; without a definition the link aborts.
        lcf, objects_txt = self._link(_elf(defined=["func_01ff97d8"]),
                                      _elf(defined=["func_0206f46c"], imported=["_deq"]))
        self.assertIn(("_deq", 0x01ff9d40), LI.patch(lcf, objects_txt, self.root / "config"))
        self.assertIn("        _deq = 0x01ff9d40;", lcf.read_text())

    def test_patch_defines_an_imported_sdk_linker_constant(self):
        # func_02058308 reads SDK_IRQ_STACKSIZE's address as its value (0x600).
        lcf, objects_txt = self._link(_elf(defined=["func_02058308"],
                                           imported=["SDK_IRQ_STACKSIZE"]))
        added = LI.patch(lcf, objects_txt, self.root / "config")
        self.assertIn(("SDK_IRQ_STACKSIZE", 0x600), added)
        self.assertNotIn("SDK_SYS_STACKSIZE", {name for name, _value in added})
        # A value, not an address in a module: it stays at the top of SECTIONS.
        self.assertIn("SECTIONS {\n    SDK_IRQ_STACKSIZE = 0x00000600;", lcf.read_text())

    def test_patch_leaves_a_linker_constant_nobody_imports(self):
        lcf, objects_txt = self._link(_elf(defined=["func_02058308"]))
        LI.patch(lcf, objects_txt, self.root / "config")
        self.assertNotIn("STACKSIZE", lcf.read_text())

    def test_patch_defines_every_undefined_interior_row_imported_or_not(self):
        # dsd checks every symbols.txt row, and the asm block cannot export one,
        # so an interior row nothing imports still needs its LCF definition: the
        # strict stock control links no compiled caller of func_01ff99a4.
        # The overlay row is not an interior row and stays undefined.
        caller = _elf(defined=["func_ov002_020dc560"],
                      imported=["func_01ff98f4", "func_ov002_020dc000"])
        lcf, objects_txt = self._link(_elf(defined=["func_01ff97d8", "_deq_core"]), caller)
        added = LI.patch(lcf, objects_txt, self.root / "config")
        self.assertEqual(added, [("func_01ff98f4", 0x01ff98f4), ("func_01ff99a4", 0x01ff99a4),
                                 ("_deq", 0x01ff9d40)])
        # Inside .itcm, not at the top of SECTIONS: there mwldarm gives the
        # assignment an arbitrary output section and `dsd check symbols` cannot
        # find the row in ITCM.
        self.assertEqual(lcf.read_text(),
                         "MEMORY {\n}\nSECTIONS {\n"
                         "    .itcm : {\n"
                         "        func_01ff98f4 = 0x01ff98f4;\n"
                         "        func_01ff99a4 = 0x01ff99a4;\n"
                         "        _deq = 0x01ff9d40;\n"
                         "    } > itcm\n}\n")

    def test_patch_refuses_an_lcf_without_the_module_section(self):
        lcf, objects_txt = self._link(_elf(imported=["func_01ff98f4"]))
        lcf.write_text("SECTIONS {\n    .text : {}\n}\n")
        with self.assertRaises(ValueError):
            LI.patch(lcf, objects_txt, self.root / "config")

    def test_patch_leaves_a_resolving_link_untouched(self):
        lcf, objects_txt = self._link(
            _elf(defined=["func_01ff98f4", "func_01ff99a4", "_deq", "_deq_core"]),
            _elf(imported=["func_01ff98f4"]))
        self.assertEqual(LI.patch(lcf, objects_txt, self.root / "config"), [])
        self.assertEqual(lcf.read_text(), LCF)

    def test_local_symbol_is_not_a_definition(self):
        lcf, objects_txt = self._link(_elf(local=["func_01ff98f4"]),
                                      _elf(imported=["func_01ff98f4"]))
        self.assertIn(("func_01ff98f4", 0x01ff98f4),
                      LI.patch(lcf, objects_txt, self.root / "config"))

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
        self.assertIn(b".itcm : {\r\n        func_01ff98f4 = 0x01ff98f4;\r\n", lcf.read_bytes())


BSS_LCF = LCF.replace("    } > itcm\n", "    } > itcm\n    .ov006 : {\n    }\n")


class BssInteriorTests(unittest.TestCase):
    def _link(self, *objects):
        lcf, paths = LcfInteriorTests._link(self, *objects)
        lcf.write_text(BSS_LCF)
        return lcf, paths
    tearDown = LcfInteriorTests.tearDown

    def setUp(self):
        LcfInteriorTests.setUp(self)
        # ITCM interior rows are defined unconditionally; keep them out of these.
        shutil.rmtree(self.root / "config" / "itcm")
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



def _data_elf(section_name=".rodata", size=16, value=0, payload_size=16):
    strings = b"\0block\0"
    section_strings = ("\0" + section_name + "\0.symtab\0.strtab\0.shstrtab\0").encode()
    symbols = b"\0" * 16 + struct.pack("<IIIBBH", 1, value, size, 0x11, 0, 1)
    payload = bytes(range(payload_size))
    data_offset = 52
    symbol_offset = data_offset + payload_size
    strings_offset = symbol_offset + len(symbols)
    section_strings_offset = strings_offset + len(strings)
    headers_offset = section_strings_offset + len(section_strings)
    headers = b"\0" * 40
    headers += struct.pack("<IIIIIIIIII", 1, 1, 2, 0, data_offset, payload_size, 0, 0, 4, 0)
    headers += struct.pack("<IIIIIIIIII", section_strings.index(b".symtab"), 2, 0, 0,
                           symbol_offset, len(symbols), 3, 1, 4, 16)
    headers += struct.pack("<IIIIIIIIII", section_strings.index(b".strtab"), 3, 0, 0,
                           strings_offset, len(strings), 0, 0, 1, 0)
    headers += struct.pack("<IIIIIIIIII", section_strings.index(b".shstrtab"), 3, 0, 0,
                           section_strings_offset, len(section_strings), 0, 0, 1, 0)
    header = b"\x7fELF\x01\x01\x01" + b"\0" * 9
    header += struct.pack("<HHIIIIIHHHHHH", 1, 40, 1, 0, 0, headers_offset, 0,
                          52, 0, 0, 40, 5, 4)
    return header + payload + symbols + strings + section_strings + headers


class InitializedInteriorTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = pathlib.Path(self.temp.name)
        self.config = self.root / "config"
        self.module = self.config / "overlays/ov047"
        self.module.mkdir(parents=True)
        self.symbols = self.module / "symbols.txt"
        self.symbols.write_text("block kind:data(any) addr:0x02111ab4\n"
                                "entry kind:data(any) addr:0x02111abc\n")
        self.delinks = self.module / "delinks.txt"
        self.delinks.write_text("    .rodata start:0x021119e0 end:0x021125e0 kind:rodata align:4\n"
                                "src/cart.cpp:\n    complete\n"
                                "    .rodata start:0x02111ab4 end:0x02111ac4\n")
        self.obj = self.root / "build/src/cart.o"
        self.obj.parent.mkdir(parents=True)
        self.obj.write_bytes(_data_elf())
        self.objects = self.root / "objects.txt"
        self.objects.write_text(str(self.obj) + "\n")
        self.lcf = self.root / "arm9.lcf"
        self.lcf.write_text("SECTIONS {\n    .ov047 : {\n    } > ov047\n}\n")

    def rows(self, defined=("block",)):
        return LI.initialized_interior_rows(self.config, self.objects, set(defined))

    def test_unimported_label_uses_complete_compiled_object_extent(self):
        self.assertEqual(self.rows(), [("entry", 0x02111abc, "block", 8, "overlays/ov047")])
        before = self.obj.read_bytes()
        self.assertEqual(LI.patch(self.lcf, self.objects, self.config), [("entry", 0x02111abc)])
        self.assertIn(".ov047 : {\n        entry = 0x02111abc;", self.lcf.read_text())
        self.assertEqual(self.obj.read_bytes(), before)

    def test_mutable_data_is_supported(self):
        self.obj.write_bytes(_data_elf(".data"))
        self.delinks.write_text(self.delinks.read_text().replace(".rodata", ".data"))
        self.assertEqual(len(self.rows()), 1)

    def test_named_initialized_slice_keeps_exact_provider_and_extent_guards(self):
        self.obj.write_bytes(_data_elf('.data.clps0'))
        self.delinks.write_text(self.delinks.read_text().replace('.rodata', '.data.clps0'))
        self.assertEqual(self.rows(), [('entry', 0x02111abc, 'block', 8, 'overlays/ov047')])
        original = self.delinks.read_text()
        self.delinks.write_text(original.replace('end:0x02111ac4', 'end:0x02111ac0'))
        self.assertEqual(self.rows(), [])
        self.delinks.write_text(original.replace('src/cart.cpp:', 'src/another.cpp:'))
        self.assertEqual(self.rows(), [])

    def test_unimported_start_alias_keeps_identity_without_storage(self):
        self.symbols.write_text(self.symbols.read_text().replace("0x02111abc", "0x02111ab4"))
        expected = [("entry", 0x02111ab4, "block", 0, "overlays/ov047")]
        for section in (".data", ".rodata"):
            with self.subTest(section=section):
                self.obj.write_bytes(_data_elf(section))
                self.delinks.write_text("src/cart.cpp:\n    complete\n"
                                        f"    {section} start:0x02111ab4 end:0x02111ac4\n")
                self.assertEqual(self.rows(), expected)
        before = self.obj.read_bytes()
        self.assertEqual(LI.patch(self.lcf, self.objects, self.config), [("entry", 0x02111ab4)])
        self.assertIn(".ov047 : {\n        entry = 0x02111ab4;", self.lcf.read_text())
        self.assertEqual(self.obj.read_bytes(), before)
        self.assertEqual(self.rows(("block", "entry")), [])

    def test_start_alias_declared_extent_must_fit_actual_object(self):
        for size, accepted in ((0, True), (4, True), (16, True), (17, False)):
            with self.subTest(size=size):
                self.symbols.write_text("block kind:data(any) addr:0x02111ab4\n"
                                        f"entry kind:data(size={size}) addr:0x02111ab4\n")
                self.assertEqual(bool(self.rows()), accepted)

    def test_start_alias_requires_complete_actual_source_extent(self):
        self.symbols.write_text(self.symbols.read_text().replace("0x02111abc", "0x02111ab4"))
        original = self.delinks.read_text()
        for changed in (original.replace("    complete\n", ""),
                        original.replace("end:0x02111ac4", "end:0x02111ac0"),
                        original.replace(".rodata", ".data"),
                        original.replace("src/cart.cpp", "src/other.cpp")):
            with self.subTest(delinks=changed):
                self.delinks.write_text(changed)
                self.assertEqual(self.rows(), [])
        self.delinks.write_text(original)
        self.obj.write_bytes(_data_elf(size=0))
        self.assertEqual(self.rows(), [])

    def test_start_alias_refuses_ambiguous_owner_or_module(self):
        original = "block kind:data(any) addr:0x02111ab4\nentry kind:data(any) addr:0x02111ab4\n"
        self.symbols.write_text(original)
        other_module = self.config / "overlays/ov048"
        other_module.mkdir(parents=True)
        for name in ("block", "entry"):
            with self.subTest(duplicate_home=name):
                (other_module / "symbols.txt").write_text(f"{name} kind:data(any) addr:0x02111ab4\n")
                self.assertEqual(self.rows(), [])
        (other_module / "symbols.txt").write_text("")
        self.objects.write_text((str(self.obj) + "\n") * 2)
        self.assertEqual(self.rows(), [])
        other = self.root / "other/build/src/cart.o"
        other.parent.mkdir(parents=True)
        other.write_bytes(self.obj.read_bytes().replace(b"block", b"other"))
        self.objects.write_text(str(self.obj) + "\n" + str(other) + "\n")
        self.symbols.write_text(original + "other kind:data(any) addr:0x02111ab4\n")
        self.assertEqual(self.rows(("block", "other")), [])

    def test_start_alias_cannot_cross_overlays_or_use_nondata_row(self):
        self.symbols.write_text("block kind:data(any) addr:0x02111ab4\n")
        other = self.config / "overlays/ov048"
        other.mkdir(parents=True)
        (other / "symbols.txt").write_text("entry kind:data(any) addr:0x02111ab4\n")
        self.assertEqual(self.rows(), [])
        (other / "symbols.txt").write_text("")
        for kind in ("bss(size=4)", "function(arm,size=4)"):
            with self.subTest(kind=kind):
                self.symbols.write_text("block kind:data(any) addr:0x02111ab4\n"
                                        f"entry kind:{kind} addr:0x02111ab4\n")
                self.assertEqual(self.rows(), [])

    def test_does_not_infer_extent_from_next_symbol(self):
        self.obj.write_bytes(_data_elf(size=8))
        self.assertEqual(self.rows(), [])
        self.obj.write_bytes(_data_elf(size=17))
        self.assertEqual(self.rows(), [])
        self.obj.write_bytes(_data_elf(value=4))
        self.assertEqual(self.rows(), [])

    def test_requires_complete_matching_section_and_whole_extent(self):
        original = self.delinks.read_text()
        for altered in (original.replace("    complete\n", ""),
                        original.replace("end:0x02111ac4", "end:0x02111ac0"),
                        original.replace("    .rodata start:0x02111ab4", "    .data start:0x02111ab4")):
            with self.subTest(delinks=altered):
                self.delinks.write_text(altered)
                self.assertEqual(self.rows(), [])

    def test_refuses_gap_or_wrong_source_provider(self):
        for name in ("_dsd_gap@ov047_0.o", "wrong.o"):
            with self.subTest(name=name):
                other = self.obj.with_name(name)
                other.write_bytes(self.obj.read_bytes())
                self.objects.write_text(str(other) + "\n")
                self.assertEqual(self.rows(), [])

    def test_refuses_ambiguous_module_homes_and_duplicate_definitions(self):
        other = self.config / "overlays/ov048"
        other.mkdir(parents=True)
        (other / "symbols.txt").write_text("entry kind:data(any) addr:0x02111abc\n")
        self.assertEqual(self.rows(), [])
        (other / "symbols.txt").write_text("block kind:data(any) addr:0x02111ab4\n")
        self.assertEqual(self.rows(), [])
        (other / "symbols.txt").write_text("")
        self.objects.write_text((str(self.obj) + "\n") * 2)
        self.assertEqual(self.rows(), [])

    def test_preserves_existing_definitions_and_refuses_nondata_rows(self):
        self.assertEqual(self.rows(("block", "entry")), [])
        self.symbols.write_text(self.symbols.read_text().replace(
            "entry kind:data(any)", "entry kind:function(arm,size=0x8)"))
        self.assertEqual(self.rows(), [])

    def test_same_numeric_address_in_another_overlay_is_not_an_alias(self):
        self.symbols.write_text("block kind:data(any) addr:0x02111ab4\n")
        other = self.config / "overlays/ov048"
        other.mkdir(parents=True)
        (other / "symbols.txt").write_text("entry kind:data(any) addr:0x02111abc\n")
        self.assertEqual(self.rows(), [])

    def test_duplicate_nonobject_definition_also_refuses_owner(self):
        other = self.obj.with_name("duplicate.o")
        other.write_bytes(_elf(defined=["block"]))
        self.objects.write_text(str(self.obj) + "\n" + str(other) + "\n")
        self.assertEqual(self.rows(), [])

    def test_rejects_conflicting_declared_object_extents(self):
        original = self.symbols.read_text()
        for altered in (original.replace("block kind:data(any)", "block kind:data(size=0x20)"),
                        original.replace("entry kind:data(any)", "entry kind:data(size=0xc)")):
            with self.subTest(symbols=altered):
                self.symbols.write_text(altered)
                self.assertEqual(self.rows(), [])

    def test_output_module_must_be_unique_and_failure_does_not_write(self):
        for value in ("SECTIONS {\n}\n",
                      "SECTIONS {\n.ov047 : { }\n.ov047 : { }\n}\n"):
            self.lcf.write_text(value)
            before = self.lcf.read_bytes()
            with self.assertRaisesRegex(ValueError, "exactly one .ov047"):
                LI.patch(self.lcf, self.objects, self.config)
            self.assertEqual(self.lcf.read_bytes(), before)


class SourceBssInteriorTests(unittest.TestCase):
    def setUp(self):
        InitializedInteriorTests.setUp(self)
        self.symbols.write_text("block kind:bss(size=16) addr:0x02111ab4\n"
                                "entry kind:bss(size=8) addr:0x02111abc\n")
        self.delinks.write_text(self.delinks.read_text().replace(".rodata", ".bss"))
        self.raw = bytearray(_data_elf(".bss"))
        self.shoff = struct.unpack_from("<I", self.raw, 0x20)[0]
        struct.pack_into("<I", self.raw, self.shoff + 40 + 4, 8)
        self.obj.write_bytes(self.raw)

    def rows(self):
        return LI.source_bss_interior_rows(self.config, self.objects, {"block"})

    def test_unimported_sized_field_retains_identity_without_storage(self):
        self.assertEqual(self.rows(), [("entry", 0x02111abc, "block", 8, "overlays/ov047")])
        self.assertEqual(LI.patch(self.lcf, self.objects, self.config), [("entry", 0x02111abc)])
        self.assertEqual(self.obj.read_bytes(), self.raw)
        self.assertIn("entry = 0x02111abc;", self.lcf.read_text())

    def test_unsized_interior_label_uses_actual_parent_extent(self):
        self.symbols.write_text(self.symbols.read_text().replace("entry kind:bss(size=8)",
                                                                "entry kind:bss"))
        self.assertEqual(len(self.rows()), 1)

    def test_zero_storage_interior_alias_is_preserved(self):
        self.symbols.write_text(self.symbols.read_text().replace("entry kind:bss(size=8)",
                                                                "entry kind:bss(size=0)"))
        self.assertEqual(len(self.rows()), 1)

    def test_start_alias_policy_is_not_extended_to_bss(self):
        self.symbols.write_text(self.symbols.read_text().replace("0x02111abc", "0x02111ab4"))
        self.assertEqual(self.rows(), [])

    def test_wrong_kind_zero_size_and_out_of_bounds_extents_fail(self):
        original = self.symbols.read_text()
        for altered in (original.replace("block kind:bss(size=16)", "block kind:bss(size=12)"),
                        original.replace("entry kind:bss(size=8)", "entry kind:bss(size=12)"),
                        original.replace("block kind:bss(size=16)", "block kind:bss(size=0)"),
                        original.replace("entry kind:bss(size=8)", "entry kind:data(any)"),
                        original.replace("entry kind:bss(size=8)", "entry kind:bss(size=bad)")):
            with self.subTest(symbols=altered):
                self.symbols.write_text(altered)
                self.assertEqual(self.rows(), [])

    def test_requires_nobits_arm_relocatable_object(self):
        for offset, fmt, value in ((self.shoff + 44, "<I", 1), (16, "<H", 2), (18, "<H", 3)):
            with self.subTest(offset=offset):
                changed = bytearray(self.raw)
                struct.pack_into(fmt, changed, offset, value)
                self.obj.write_bytes(changed)
                self.assertEqual(self.rows(), [])

    def test_requires_exact_complete_provider_and_extent(self):
        original = self.delinks.read_text()
        for altered in (original.replace("    complete\n", ""),
                        original.replace("src/cart.cpp", "src/other.cpp"),
                        original.replace("end:0x02111ac4", "end:0x02111ac0"),
                        original.replace(".bss", ".data")):
            with self.subTest(delinks=altered):
                self.delinks.write_text(altered)
                self.assertEqual(self.rows(), [])

    def test_duplicate_definition_or_cross_module_home_is_ambiguous(self):
        self.objects.write_text((str(self.obj) + "\n") * 2)
        self.assertEqual(self.rows(), [])
        self.objects.write_text(str(self.obj) + "\n")
        other = self.config / "overlays/ov048"
        other.mkdir(parents=True)
        for symbol, address in (("entry", "0x02111abc"), ("block", "0x02111ab4")):
            (other / "symbols.txt").write_text(f"{symbol} kind:bss addr:{address}\n")
            self.assertEqual(self.rows(), [])


def _asm_elf(text, defined):
    """ELF32 object: symbol table, string table, .text, then the section headers.

    `defined` is [(name, value, size)]. Names are global STT_FUNC in .text.
    """
    strings = b"\0"
    offsets = {}
    for name, _value, _size in defined:
        offsets[name] = len(strings)
        strings += name.encode() + b"\0"
    symbols = b"\0" * 16
    for name, value, size in defined:
        symbols += struct.pack("<IIIBBH", offsets[name], value, size, (1 << 4) | 2, 0, 4)
    shstr = b"\0.symtab\0.strtab\0.shstrtab\0.text\0"
    sym_off = 52
    str_off = sym_off + len(symbols)
    shstr_off = str_off + len(strings)
    text_off = (shstr_off + len(shstr) + 15) & ~15
    shoff = text_off + len(text)
    shdrs = b"\0" * 40
    shdrs += struct.pack("<IIIIIIIIII", 1, 2, 0, 0, sym_off, len(symbols), 2, 1, 4, 16)
    shdrs += struct.pack("<IIIIIIIIII", 9, 3, 0, 0, str_off, len(strings), 0, 0, 1, 0)
    shdrs += struct.pack("<IIIIIIIIII", 17, 3, 0, 0, shstr_off, len(shstr), 0, 0, 1, 0)
    shdrs += struct.pack("<IIIIIIIIII", 27, 1, 0, 0, text_off, len(text), 0, 0, 4, 0)
    hdr = b"\x7fELF\x01\x01\x01" + b"\0" * 9
    hdr += struct.pack("<HHIIIIIHHHHHH", 1, 40, 1, 0, 0, shoff, 0, 52, 0, 0, 40, 5, 3)
    pad = b"\0" * (text_off - shstr_off - len(shstr))
    return hdr + symbols + strings + shstr + pad + text + shdrs


class ItcmAliasStampTests(unittest.TestCase):
    def test_stamp_writes_the_other_spelling_into_the_object(self):
        root = pathlib.Path(tempfile.mkdtemp())
        self.addCleanup(lambda: __import__("shutil").rmtree(root, ignore_errors=True))
        itcm = root / "config" / "itcm"
        itcm.mkdir(parents=True)
        (itcm / "symbols.txt").write_text(
            "func_01ff8000 kind:function(arm,size=0x20) addr:0x01ff8000\n"
            "_dadd kind:function(arm,size=0x0) addr:0x01ff8000\n"
            "_dadd_core kind:label(arm) addr:0x01ff8010\n"
            "func_01ff97d8 kind:function(arm,size=0x11c) addr:0x01ff97d8\n"
            "_deq kind:function(arm,size=0x0) addr:0x01ff9d40\n")
        (itcm / "delinks.txt").write_text(
            "src/_dadd.c:\n"
            "    complete\n"
            "    .text start:0x01ff8000 end:0x01ff8020\n"
            "\n"
            "src/unnamed/arm9/01ff/func_01ff97d8.c:\n"
            "    complete\n"
            "    .text start:0x01ff97d8 end:0x01ffa344\n")
        text = bytes(range(0x20))
        obj = root / "_dadd.o"
        obj.write_bytes(_asm_elf(text, [("_dadd", 0, 0x20)]))
        untouched = root / "func_01ff97d8.o"
        untouched.write_bytes(_asm_elf(b"\x11" * 4, [("func_01ff97d8", 0, 4)]))
        before = untouched.read_bytes()
        listing = root / "objects.txt"
        listing.write_text(f"{obj}\n{untouched}\n")
        LI.stamp_linked_aliases(listing, root / "config")
        self.assertEqual(untouched.read_bytes(), before)
        stamped = obj.read_bytes()
        defined, imported = LI._elf_names(stamped)
        self.assertEqual(imported, set())
        self.assertEqual(defined, {"_dadd", "func_01ff8000", "_dadd_core"})
        _shoff, _entsize, shstrndx, sections = LI._elf_sections(stamped)
        text_index = next(i for i, sec in enumerate(sections)
                          if LI._section_name(stamped, sections, shstrndx, i) == ".text")
        self.assertEqual(stamped[sections[text_index][4]:][:0x20], text)
        names = LI._symbol_names(stamped, sections[1][4], sections[1][5], sections[2][4])
        by_name = {name: (value, size, info) for name, value, size, info, _shndx in names}
        self.assertEqual(by_name["_dadd"][:2], (0, 0))
        self.assertEqual(by_name["func_01ff8000"][:2], (0, 0x20))
        self.assertEqual(by_name["_dadd_core"][:2], (0x10, 0))
        self.assertEqual(by_name["_dadd_core"][2] & 0xF, 0)
        again = stamped
        obj.write_bytes(stamped)
        LI.stamp_linked_aliases(listing, root / "config")
        self.assertEqual(obj.read_bytes(), again)

        caller = root / "caller.o"
        caller.write_bytes(_elf(defined=["func_02060b64"], imported=["func_01ff8000"]))
        listing.write_text(f"{obj}\n{caller}\n")
        lcf = root / "arm9.lcf"
        lcf.write_text("SECTIONS {\n    .itcm : {}\n}\n")
        added = LI.patch(lcf, listing, root / "config")
        self.assertNotIn("func_01ff8000", {name for name, _addr in added})
        self.assertNotIn("func_01ff8000 =", lcf.read_text())


if __name__ == "__main__":
    unittest.main()
