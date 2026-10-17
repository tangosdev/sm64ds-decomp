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

if __name__ == "__main__":
    unittest.main()
