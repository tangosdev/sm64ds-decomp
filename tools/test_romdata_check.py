"""Data-symbol byte coverage, extent boundaries, and summary identities.

The distinction is the whole reason this file exists. Vague linkage means one
cartridge symbol is emitted by every object that needs it, so a record count rises
and falls with how many FILES the tree is split into -- and TU reconstruction exists
to reduce that number. validate_merge ratchets on `verified`, so a metric that fell
whenever sources were merged would refuse the promotions on principle.
"""
import sys
import pathlib
import unittest
from unittest import mock
import io
import struct
import tempfile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import romdata_check as RDC  # noqa: E402


def rec(symbol, verdict, src="src/a.cpp", size=4, addr=None):
    return {"symbol": symbol, "verdict": verdict, "src": src, "bytes": size,
            "addr": addr}


class SummarizeCountsSymbols(unittest.TestCase):
    def test_duplicate_vague_records_count_once(self):
        s = RDC.summarize([rec("_ZTI7fBase_c", RDC.VERIFIED, "src/a.cpp"),
                           rec("_ZTI7fBase_c", RDC.VERIFIED, "src/b.cpp"),
                           rec("_ZTI7fBase_c", RDC.VERIFIED, "src/c.cpp")])
        self.assertEqual(s["verified"], 1)
        self.assertEqual(s["verifiedBytes"], 4)
        self.assertEqual(s["totalRecords"], 3)
        self.assertEqual(s["verifiedRecords"], 3)
        self.assertEqual(s["verifiedSymbols"], [
            {"module": None, "symbol": "_ZTI7fBase_c", "addr": None, "bytes": 4}])

    def test_consolidating_sources_does_not_move_the_ratchet(self):
        """The measured shape of the first TU promotion, in miniature."""
        legacy = [rec("_ZTI7fBase_c", RDC.VERIFIED, f"src/f{i}.cpp") for i in range(8)]
        promoted = [rec("_ZTI7fBase_c", RDC.VERIFIED, "src/actors/TU.cpp")]
        self.assertEqual(RDC.summarize(legacy)["verified"],
                         RDC.summarize(promoted)["verified"])

    def test_best_verdict_wins_across_objects(self):
        s = RDC.summarize([rec("_ZTV4Foo", RDC.PARTIAL), rec("_ZTV4Foo", RDC.VERIFIED)])
        self.assertEqual((s["verified"], s["partial"]), (1, 0))

    def test_a_differing_copy_is_never_hidden_by_a_correct_sibling(self):
        """`differs` is deliberately worst-case, unlike every other bucket."""
        s = RDC.summarize([rec("_ZTV4Foo", RDC.VERIFIED, "src/a.cpp"),
                           rec("_ZTV4Foo", RDC.DIFFERS, "src/b.cpp")])
        self.assertEqual(s["differs"], 1)
        self.assertEqual(s["verified"], 0)
        self.assertEqual(len(s["differing"]), 1)
        self.assertEqual(s["differingSymbols"], [
            {"module": None, "symbol": "_ZTV4Foo", "addr": None, "bytes": 4}])

    def test_distinct_symbols_still_add_up(self):
        s = RDC.summarize([rec("_ZTI4Foo", RDC.VERIFIED), rec("_ZTS4Foo", RDC.PARTIAL),
                           rec("_ZTV4Bar", RDC.UNNAMED)])
        self.assertEqual((s["symbols"], s["verified"], s["partial"], s["unnamed"]),
                         (3, 1, 1, 1))

    def test_same_name_in_two_modules_is_two_symbols(self):
        """The same spelling in two overlays is two cartridge symbols, not one."""
        s = RDC.summarize([dict(rec("_ZTV4Foo", RDC.VERIFIED), module="ov006"),
                           dict(rec("_ZTV4Foo", RDC.DIFFERS), module="ov084")])
        self.assertEqual((s["symbols"], s["verified"], s["differs"]), (2, 1, 1))
        self.assertEqual(s["verifiedSymbols"], [
            {"module": "ov006", "symbol": "_ZTV4Foo", "addr": None, "bytes": 4}])

    def test_the_identity_rows_carry_the_rom_address_and_proven_bytes(self):
        """Without the address `validate_merge` cannot tell a rename from a deletion.

        `verify_data_symbol` has both on every record; dropping them here is what made
        the ROM-data ratchet name-only, and a name is the one part of a data symbol a
        source change is allowed to move.
        """
        s = RDC.summarize([
            dict(rec("_ZTV8daGmch_c", RDC.VERIFIED, size=0x84, addr=0x02128c04),
                 module="ov081"),
            dict(rec("_ZTV7daMky_c", RDC.DIFFERS, size=0x84, addr=0x02115bfc),
                 module="ov030")])
        self.assertEqual(s["verifiedSymbols"], [
            {"module": "ov081", "symbol": "_ZTV8daGmch_c",
             "addr": 0x02128c04, "bytes": 0x84}])
        self.assertEqual(s["differingSymbols"], [
            {"module": "ov030", "symbol": "_ZTV7daMky_c",
             "addr": 0x02115bfc, "bytes": 0x84}])

    def test_unparsable_objects_never_dedupe_against_each_other(self):
        """check_object's `?` catch-all carries no symbol identity, only a source."""
        s = RDC.summarize([rec("?", RDC.UNNAMED, "src/a.cpp"),
                           rec("?", RDC.UNNAMED, "src/b.cpp")])
        self.assertEqual((s["symbols"], s["unnamed"]), (2, 2))

    def test_empty_input_is_all_zero(self):
        s = RDC.summarize([])
        self.assertEqual((s["symbols"], s["verified"], s["differs"], s["totalRecords"]),
                         (0, 0, 0, 0))

import unittest.mock  # noqa: E402  (used by the data and preamble tests below)


class DataSymbolByteCoverage(unittest.TestCase):
    """Exercise verdicts with real relocation resolution and controlled ROM bytes."""

    def check(self, emitted, retail, *, extent, relocs=(), name="data_table"):
        elf = unittest.mock.Mock()
        elf.get_section.return_value.data.return_value = b"prefix" + emitted + b"suffix"
        with (unittest.mock.patch.object(RDC, "ELFFile", return_value=elf),
              unittest.mock.patch.object(RDC.RA, "resolve_candidate",
                                         return_value=("arm9", 0x200)),
              unittest.mock.patch.object(RDC, "rom_data_index",
                                         return_value={("arm9", name): (0x200, extent)}),
              unittest.mock.patch.object(RDC.LC, "func_relocs_typed",
                                         return_value=[dict(r) for r in relocs]),
              unittest.mock.patch.object(RDC.RV, "rom_bytes", return_value=retail)):
            return RDC.verify_data_symbol(b"object", name, 1, 6, len(emitted), names={})

    @staticmethod
    def relocation(offset, address=None, kind=RDC.LC.R_ARM_ABS32):
        return {"off": offset, "addr": address, "type": kind, "add": 0,
                "sym": "referenced_data"}

    def test_equal_short_objects_and_tails_are_fully_counted(self):
        for size in (1, 2, 3, 4, 5, 6, 7):
            with self.subTest(size=size):
                body = bytes(range(size))
                result = self.check(body, body, extent=size)
                self.assertEqual(result["verdict"], RDC.VERIFIED)
                self.assertEqual(result["bytes"], size)
                self.assertEqual(result["blindWords"], 0)

    def test_a_difference_in_any_tail_byte_is_detected(self):
        for size in (1, 2, 3, 5, 6, 7):
            for offset in range(size & ~3, size):
                with self.subTest(size=size, offset=offset):
                    retail = bytes(size)
                    body = bytearray(retail)
                    body[offset] = 1
                    result = self.check(body, retail, extent=size)
                    self.assertEqual(result["verdict"], RDC.DIFFERS)
                    self.assertEqual(result["bytes"], size)
                    self.assertEqual(result["differingBytes"], 1)

    def test_differing_bytes_counts_bytes_not_words(self):
        result = self.check(b"aXcdYfZ", b"abcdefg", extent=7)
        self.assertEqual(result["verdict"], RDC.DIFFERS)
        self.assertEqual(result["differingBytes"], 3)

    def test_blind_relocation_masks_the_entire_short_tail(self):
        for size in (5, 6, 7):
            with self.subTest(size=size):
                result = self.check(b"abcd" + b"X" * (size - 4),
                                    b"abcd" + b"Y" * (size - 4), extent=size,
                                    relocs=[self.relocation(4)])
                self.assertEqual(result["verdict"], RDC.PARTIAL)
                self.assertEqual(result["bytes"], 4)
                self.assertEqual(result["blindWords"], 1)

    def test_unsupported_relocation_also_masks_the_short_tail(self):
        result = self.check(b"abcdXXX", b"abcdYYY", extent=7,
                            relocs=[self.relocation(4, 0x1234, kind=255)])
        self.assertEqual(result["verdict"], RDC.PARTIAL)
        self.assertEqual(result["bytes"], 4)

    def test_a_blind_word_does_not_hide_a_later_tail_difference(self):
        result = self.check(b"XXXXabX", b"YYYYabc", extent=7,
                            relocs=[self.relocation(0)])
        self.assertEqual(result["verdict"], RDC.DIFFERS)
        self.assertEqual(result["bytes"], 3)
        self.assertEqual(result["differingBytes"], 1)

    def test_an_entirely_blind_short_object_is_not_verified(self):
        result = self.check(b"XXX", b"YYY", extent=3,
                            relocs=[self.relocation(0)])
        self.assertEqual(result["verdict"], RDC.PARTIAL)
        self.assertEqual(result["bytes"], 0)

    def test_equal_bytes_need_a_known_fully_covered_extent(self):
        for extent in (None, 8):
            with self.subTest(extent=extent):
                result = self.check(b"abcdefg", b"abcdefg", extent=extent)
                self.assertEqual(result["verdict"], RDC.PARTIAL)
                self.assertEqual(result["bytes"], 7)

    def test_resolved_pointer_and_tail_are_compared_together(self):
        result = self.check(b"\0\0\0\0abc", b"\x78\x56\x34\x12abc", extent=7,
                            relocs=[self.relocation(0, 0x12345678)])
        self.assertEqual(result["verdict"], RDC.VERIFIED)
        self.assertEqual(result["bytes"], 7)

    def test_vtable_preamble_mask_is_trimmed_before_comparing_tail(self):
        result = self.check(b"XXXXXXXXabcdX", b"abcde", extent=5,
                            name="_ZTV3Foo", relocs=[self.relocation(4)])
        self.assertEqual(result["verdict"], RDC.DIFFERS)
        self.assertEqual(result["bytes"], 5)
        self.assertEqual(result["blindWords"], 0)
        self.assertEqual(result["differingBytes"], 1)

    def test_vtable_slot_mask_is_rebased_after_the_preamble(self):
        result = self.check(b"XXXXXXXXXXXXabc", b"YYYYabc", extent=7,
                            name="_ZTV3Foo", relocs=[self.relocation(8)])
        self.assertEqual(result["verdict"], RDC.PARTIAL)
        self.assertEqual(result["bytes"], 3)
        self.assertEqual(result["blindWords"], 1)


class ExtentStopsAtTheVtablePreamble(unittest.TestCase):
    """`symbols.txt` points `_ZTV<C>` at the slot array, so the two words below it are
    the vtable object's own offset-to-top and typeinfo and belong to no symbol at all.
    Distance-to-the-next-symbol sizing charged them to whatever came before, which then
    scored PARTIAL by exactly eight bytes with nothing it could ever emit to close the
    gap. 522 of this ROM's 540 vtables are unowned by any source, so this was not rare.
    """
    ALWAYS = staticmethod(lambda addr: True)
    NEVER = staticmethod(lambda addr: False)

    def test_previous_symbol_is_not_charged_for_the_preamble(self):
        e = RDC._module_extents([(0x100, "data_x"), (0x120, "_ZTV3Foo")], self.ALWAYS)
        self.assertEqual(e["data_x"], 0x18)

    def test_without_the_fix_that_symbol_is_eight_bytes_short_forever(self):
        e = RDC._module_extents([(0x100, "data_x"), (0x120, "_ZTV3Foo")], self.NEVER)
        self.assertEqual(e["data_x"], 0x20)

    def test_a_vtable_does_not_pay_for_the_next_vtables_preamble(self):
        e = RDC._module_extents([(0x100, "_ZTV3Foo"), (0x190, "_ZTV3Bar")], self.ALWAYS)
        self.assertEqual(e["_ZTV3Foo"], 0x88)

    def test_a_named_preamble_word_keeps_only_the_bytes_it_owns(self):
        """Where config already names one of the two words, that symbol is a boundary
        too and simply gets the shorter extent. Seven symbols in this ROM sit exactly
        four bytes below a vtable and eight sit exactly at V-8; the blanket
        'subtract 8 from whatever precedes a vtable' formulation sized 23 of them <= 0,
        one of them -4, which is why this is a boundary set and not a subtraction."""
        e = RDC._module_extents(
            [(0x100, "data_x"), (0x11c, "data_typeinfo_word"), (0x120, "_ZTV3Foo")],
            self.ALWAYS)
        self.assertEqual(e["data_x"], 0x18)
        self.assertEqual(e["data_typeinfo_word"], 4)
        self.assertTrue(all(v > 0 for v in e.values()))

    def test_a_symbol_exactly_at_the_preamble_is_unchanged(self):
        e = RDC._module_extents(
            [(0x118, "data_preamble"), (0x120, "_ZTV3Foo")], self.ALWAYS)
        self.assertEqual(e["data_preamble"], 8)

    def test_the_last_symbol_still_has_no_extent(self):
        e = RDC._module_extents([(0x100, "data_x"), (0x120, "_ZTV3Foo")], self.ALWAYS)
        self.assertNotIn("_ZTV3Foo", e)


class PreambleIsGatedOnTheCartridge(unittest.TestCase):
    """9 of this ROM's 414 vtable addresses show something other than a preamble below
    them -- strings, code, a non-zero first word. The boundary is inserted only where
    the cartridge shows one, so an absent preamble leaves an extent too LONG (a PARTIAL
    that might have been VERIFIED) and never too short (a VERIFIED that should not be).
    """
    def _rv(self, words):
        import struct as _s

        class FakeRV:
            @staticmethod
            def mod_for(label):
                return {"base": 0x100}

            @staticmethod
            def rom_bytes(label, addr, size):
                return _s.pack("<II", *words)[:size]
        return FakeRV

    def check(self, words, typeinfo=frozenset()):
        with unittest.mock.patch.object(RDC, "RV", self._rv(words)):
            return RDC._vtable_preamble_at("ov006", 0x200, typeinfo)

    def test_zero_and_a_known_typeinfo_is_a_preamble(self):
        self.assertTrue(self.check((0, 0x1234), {0x1234}))

    def test_zero_and_null_typeinfo_is_a_preamble(self):
        """A vtable whose class carries no RTTI still gets the two words."""
        self.assertTrue(self.check((0, 0)))

    def test_a_nonzero_offset_to_top_is_not_a_preamble(self):
        self.assertFalse(self.check((0x2086f58, 0x1234), {0x1234}))

    def test_an_unrecognized_typeinfo_word_is_not_a_preamble(self):
        """`_ZTV8dActor_c` reads 'Play' 'Roo' below it -- the tail of a string."""
        self.assertFalse(self.check((0x79616c50, 0x6f6f5220)))

    def test_a_vtable_at_the_module_base_has_nothing_below_it(self):
        class FakeRV:
            @staticmethod
            def mod_for(label):
                return {"base": 0x200}

            @staticmethod
            def rom_bytes(label, addr, size):  # pragma: no cover - never reached
                raise AssertionError("read below the module base")
        with unittest.mock.patch.object(RDC, "RV", FakeRV):
            self.assertFalse(RDC._vtable_preamble_at("ov006", 0x200, frozenset()))

    def test_a_module_with_no_image_is_not_a_preamble(self):
        class FakeRV:
            @staticmethod
            def mod_for(label):
                return None
        with unittest.mock.patch.object(RDC, "RV", FakeRV):
            self.assertFalse(RDC._vtable_preamble_at("ov006", 0x200, frozenset()))

class RttiOffSecondaryPreamble(unittest.TestCase):
    """RTTI off zeroes a secondary preamble that still sits inside the primary _ZTV.

    The cartridge words are a negative offset-to-top and a _ZTI pointer, named at
    the following symbol's address point. Mask that pair. A zero slot stays a
    difference, and a preamble the object actually emitted is compared as-is.
    """

    def check(self, emitted, retail, index, *, extent=24, name="_ZTV3Foo"):
        elf = unittest.mock.Mock()
        elf.get_section.return_value.data.return_value = b"prefix" + emitted + b"suffix"
        with (unittest.mock.patch.object(RDC, "ELFFile", return_value=elf),
              unittest.mock.patch.object(RDC.RA, "resolve_candidate",
                                         return_value=("arm9", 0x200)),
              unittest.mock.patch.object(RDC, "rom_data_index", return_value=index),
              unittest.mock.patch.object(RDC.LC, "func_relocs_typed", return_value=[]),
              unittest.mock.patch.object(RDC.RV, "rom_bytes", return_value=retail)):
            return RDC.verify_data_symbol(b"object", name, 1, 6, len(emitted), names={})

    def test_rtti_off_zeros_are_masked_and_the_slots_still_count(self):
        slots = b"\x11\x22\x33\x44" * 4
        tail = b"\x55\x66\x77\x88" * 2
        emitted = b"\0" * 8 + slots + b"\0" * 8 + tail
        retail = slots + struct.pack("<II", 0xFFFFFFB0, 0x300) + tail
        index = {
            ("arm9", "_ZTV3Foo"): (0x200, 24),
            ("arm9", "VTable_Thunk"): (0x218, 8),
            ("arm9", "_ZTI3Foo"): (0x300, 12),
        }
        result = self.check(emitted, retail, index)
        self.assertEqual(result["verdict"], RDC.PARTIAL)
        self.assertEqual(result["note"], "RTTI-off secondary preamble masked")
        self.assertEqual(result["bytes"], len(retail) - 8)
        self.assertEqual(result["blindWords"], 1)

    def test_a_zero_where_the_cartridge_has_a_slot_still_differs(self):
        slots = b"\x11\x22\x33\x44" * 4
        emitted = b"\0" * 8 + slots + b"\0" * 8
        retail = slots + b"\x99\x88\x77\x66" * 2
        index = {
            ("arm9", "_ZTV3Foo"): (0x200, 24),
            ("arm9", "VTable_Thunk"): (0x218, 8),
            ("arm9", "_ZTI3Foo"): (0x300, 12),
        }
        result = self.check(emitted, retail, index, extent=16)
        self.assertEqual(result["verdict"], RDC.DIFFERS)
        self.assertNotIn("note", result)

    def test_an_emitted_preamble_is_compared_not_masked(self):
        slots = b"\x11\x22\x33\x44" * 4
        preamble = struct.pack("<II", 0xFFFFFFB0, 0x300)
        tail = b"\x55\x66\x77\x88" * 2
        body = slots + preamble + tail
        emitted = b"\0" * 8 + body
        index = {
            ("arm9", "_ZTV3Foo"): (0x200, len(body)),
            ("arm9", "VTable_Thunk"): (0x218, 8),
            ("arm9", "_ZTI3Foo"): (0x300, 12),
        }
        result = self.check(emitted, body, index, extent=len(body))
        self.assertEqual(result["verdict"], RDC.VERIFIED)
        self.assertNotIn("note", result)


class ObjectLocalDataRenames(unittest.TestCase):
    """Real ELF relocations must use this source's identity, not a global temp."""

    def setUp(self):
        from test_reloc_audit_renames import _elf_with_temp
        from elftools.elf.elffile import ELFFile
        raw = bytearray(_elf_with_temp())
        table = ELFFile(io.BytesIO(raw)).get_section_by_name('.symtab')
        # The first object is a native local literal; alpha is its pointer table.
        raw[table['sh_offset'] + 16 + 12] = 1
        raw[table['sh_offset'] + 32 + 12] = 0x11
        self.raw = bytes(raw)
        self.entry = dict(module='ov002', source='src/table.cpp', functions=[],
                          data=[dict(symbol='@999')],
                          defined_symbol_renames=[dict(
                              **{'from': '@653', 'to': '@999'}, evidence='exact literal')])
        self.names = {'alpha': ('ov002', 0x2000),
                      '@653': ('ov079', 0x9000), '@999': ('ov079', 0x9900)}
        self.registry = {('ov002', 'alpha'): (0x2000, 4),
                         ('ov002', '@999'): (0x3000, 4)}
        self.saved = RDC._source_manifests
        RDC._source_manifests = {'src/table.cpp': [self.entry]}
        self.tmp = tempfile.TemporaryDirectory()
        self.path = pathlib.Path(self.tmp.name) / 'table.o'
        self.path.write_bytes(self.raw)

    def tearDown(self):
        RDC._source_manifests = self.saved
        self.tmp.cleanup()

    def check(self, rel='src/table.cpp'):
        with mock.patch.object(RDC, 'rom_data_index', return_value=self.registry), \
                mock.patch.object(RDC.RV, 'rom_bytes', return_value=struct.pack('<I', 0x3000)):
            return RDC.check_object(self.path, rel, self.names)

    def test_local_literal_uses_validated_mapping_and_own_module(self):
        rows = self.check()
        self.assertEqual(len(rows), 1)  # native local literal is not a global claim
        self.assertEqual((rows[0]['symbol'], rows[0]['verdict'], rows[0]['bytes'],
                          rows[0]['blindWords']), ('alpha', RDC.VERIFIED, 4, 0))
        self.assertEqual(self.path.read_bytes(), self.raw)
        self.assertEqual(self.names['@653'], ('ov079', 0x9000))

    def test_other_source_does_not_receive_this_mapping(self):
        self.assertEqual(self.check('src/other.cpp')[0]['verdict'], RDC.DIFFERS)

    def test_missing_canonical_home_is_blind_not_other_overlay(self):
        del self.registry[('ov002', '@999')]
        row = self.check()[0]
        self.assertEqual((row['verdict'], row['bytes'], row['blindWords']),
                         (RDC.PARTIAL, 0, 1))

    def test_unowned_rename_is_explicit_refusal(self):
        self.entry['data'] = []
        rows = self.check()
        row = rows[0]
        self.assertEqual(row['verdict'], RDC.UNNAMED)
        self.assertIn('policy refused', row['note'])
        self.assertEqual(rows[1]['verdict'], RDC.DIFFERS)

    def test_missing_raw_definition_is_explicit_refusal(self):
        self.entry['defined_symbol_renames'][0]['from'] = '@652'
        row = self.check()[0]
        self.assertEqual(row['verdict'], RDC.UNNAMED)
        self.assertIn('rename refused', row['note'])

    def test_missing_function_population_is_explicit_refusal(self):
        self.entry['functions'] = [dict(symbol='not_this_TU')]
        self.assertIn('missing manifest function', self.check()[0]['note'])

    def test_two_source_policies_are_ambiguous(self):
        RDC._source_manifests['src/table.cpp'].append(dict(self.entry))
        self.assertIn('ambiguous', self.check()[0]['note'])

    def test_source_path_separators_do_not_change_identity(self):
        self.assertEqual(self.check('src\\table.cpp')[0]['verdict'], RDC.VERIFIED)

    def test_unmapped_local_does_not_borrow_global_home(self):
        # An unrelated licensed rename must not bless an unlicensed local temp.
        self.entry['data'] = [dict(symbol='beta')]
        self.entry['defined_symbol_renames'] = [dict(
            **{'from': 'alpha', 'to': 'beta'}, evidence='owned table')]
        self.names['beta'] = ('ov002', 0x2000)
        self.registry[('ov002', 'beta')] = (0x2000, 4)
        row = self.check()[0]
        self.assertEqual((row['verdict'], row['bytes'], row['blindWords']),
                         (RDC.PARTIAL, 0, 1))

    def _named_local_policy(self):
        self.entry['data'] = [dict(symbol='beta'),
                              dict(symbol='@653', address='0x3000', size='0x4')]
        self.entry['defined_symbol_renames'] = [dict(
            **{'from': 'alpha', 'to': 'beta'}, evidence='owned table')]
        self.registry[('ov002', 'beta')] = (0x2000, 4)
        self.registry[('ov002', '@653')] = (0x3000, 4)

    def test_owned_local_without_rename_uses_exact_module_home(self):
        self._named_local_policy()
        self.assertEqual(self.check()[0]['verdict'], RDC.VERIFIED)

    def test_owned_local_wrong_size_stays_blind(self):
        self._named_local_policy()
        self.entry['data'][1]['size'] = '0x8'
        self.assertEqual(self.check()[0]['blindWords'], 1)

    def test_owned_local_wrong_address_stays_blind(self):
        self._named_local_policy()
        self.entry['data'][1]['address'] = '0x9000'
        self.assertEqual(self.check()[0]['blindWords'], 1)

    def test_duplicate_owned_local_rows_stay_blind(self):
        self._named_local_policy()
        self.entry['data'].append(dict(self.entry['data'][1]))
        self.assertEqual(self.check()[0]['blindWords'], 1)


class UndefinedImportAliases(unittest.TestCase):
    """A manifest alias is the address romdata compares, not a second spelling."""

    def setUp(self):
        self.saved = RDC._alias_by_source
        self.names = {"_ZN5Model9DoSetFileEPcii": ("arm9", 0x02016bf8)}
        RDC._alias_by_source = {
            "src/game/actors/d_a_demo.cpp": [{
                "undefined_symbol_aliases": [{
                    "symbol": "_ZN17dExtSimpleModel_c9DoSetFileEPcii",
                    "canonical_symbol": "_ZN5Model9DoSetFileEPcii",
                    "canonical_module": "arm9",
                    "canonical_address": "0x02016bf8",
                }],
            }],
        }

    def tearDown(self):
        RDC._alias_by_source = self.saved

    def test_alias_resolves_to_the_canonical_address(self):
        merged = RDC._with_undefined_aliases("src/game/actors/d_a_demo.cpp", self.names)
        self.assertEqual(merged["_ZN17dExtSimpleModel_c9DoSetFileEPcii"],
                         ("arm9", 0x02016bf8))
        self.assertNotIn("_ZN17dExtSimpleModel_c9DoSetFileEPcii", self.names)

    def test_other_sources_are_unchanged(self):
        self.assertIs(RDC._with_undefined_aliases("src/other.cpp", self.names), self.names)

    def test_disagreement_with_symbols_txt_is_a_refusal(self):
        self.names["_ZN17dExtSimpleModel_c9DoSetFileEPcii"] = ("arm9", 0x1)
        with self.assertRaises(ValueError) as caught:
            RDC._with_undefined_aliases("src/game/actors/d_a_demo.cpp", self.names)
        self.assertIn("already resolves elsewhere", str(caught.exception))

    def test_a_canonical_that_misses_its_address_is_a_refusal(self):
        RDC._alias_by_source["src/game/actors/d_a_demo.cpp"][0][
            "undefined_symbol_aliases"][0]["canonical_address"] = "0x1"
        with self.assertRaises(ValueError) as caught:
            RDC._with_undefined_aliases("src/game/actors/d_a_demo.cpp", self.names)
        self.assertIn("does not resolve exactly", str(caught.exception))


class EmittedDataSymbols(unittest.TestCase):
    """Which defined objects are offered for comparison at all."""

    @staticmethod
    def symbol(name, shndx, bind="STB_GLOBAL"):
        return {"st_shndx": shndx, "st_value": 0, "st_size": 8,
                "st_info": {"type": "STT_OBJECT", "bind": bind}, "_name": name}

    def emitted(self, symbols, section_types, linked_global=()):
        syms = []
        for raw in symbols:
            s = unittest.mock.MagicMock()
            s.name = raw["_name"]
            s.__getitem__.side_effect = raw.__getitem__
            syms.append(s)
        elf = unittest.mock.Mock()
        elf.get_section_by_name.return_value.iter_symbols.return_value = syms
        elf.get_section.side_effect = lambda i: {"sh_type": section_types[i]}
        with unittest.mock.patch.object(RDC, "ELFFile", return_value=elf):
            return [name for name, *_ in RDC.emitted_data_symbols(b"object", linked_global)]

    def test_bss_objects_are_not_compared(self):
        # arm9's file bytes at .bss addresses are the autoload payloads, not zeros.
        self.assertEqual(
            self.emitted([self.symbol("data_table", 1), self.symbol("data_0209b468", 2)],
                         {1: "SHT_PROGBITS", 2: "SHT_NOBITS"}),
            ["data_table"])

    def test_locals_are_still_skipped(self):
        self.assertEqual(
            self.emitted([self.symbol("local_table", 1, bind="STB_LOCAL")],
                         {1: "SHT_PROGBITS"}),
            [])

    def test_a_local_linked_global_by_the_manifest_is_compared(self):
        # A compiler temp renamed to its ROM data name and bound STB_GLOBAL at link.
        self.assertEqual(
            self.emitted([self.symbol("data_ov009_021138fc", 1, bind="STB_LOCAL"),
                          self.symbol("local_table", 1, bind="STB_LOCAL")],
                         {1: "SHT_PROGBITS"}, {"data_ov009_021138fc"}),
            ["data_ov009_021138fc"])


if __name__ == "__main__":
    unittest.main()
