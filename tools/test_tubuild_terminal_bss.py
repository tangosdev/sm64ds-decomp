"""Terminal BSS boundary regression tests; the core suite needs no ROM/compiler."""
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tubuild as TB


class TerminalBssBoundary(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.directory = pathlib.Path(self.temp.name)
        self.header = "    .bss start:0x0fe0 end:0x1020 kind:bss align:32\n"
        self.symbols = "terminal kind:bss(size=0x1c) addr:0x1000\n"
        self.body = ""
        self.claim = {"name": ".bss", "start": 0x1000, "end": 0x101c}
        self.entry = {"module": "ov999", "functions": [], "relocations": [],
                      "bss": [{"symbol": "terminal", "address": "0x1000", "size": "0x1c"}]}
        self.layout = {"bytes": bytes(0x1c), "relocs": [], "inputSections": [1],
                       "symbols": {"terminal": {"address": 0x1000, "size": 0x1c,
                                    "bind": "STB_GLOBAL", "type": "STT_OBJECT"}}}
        self.homes = {"terminal": [("ov999", 0x1000)]}

    def verify(self, raw=None):
        (self.directory / "delinks.txt").write_text(self.header + self.body, encoding="utf-8")
        (self.directory / "symbols.txt").write_text(self.symbols, encoding="utf-8")
        def module_directory(module):
            return self.directory if module == "ov999" else self.directory / "missing-module"
        with mock.patch.object(TB, "module_config_dir", side_effect=module_directory):
            if raw is None:
                with mock.patch.object(TB, "section_contribution", return_value=(self.layout, None)):
                    return self.call_verifier(b"")
            return self.call_verifier(raw)

    def call_verifier(self, raw):
        return TB.verify_owned_sections(raw, self.entry, [self.claim], name_index={},
                                        config_relocs={}, sym_index={}, symbol_homes=self.homes,
                                        target_reader=lambda *_args: None)

    def refused(self, message):
        result = self.verify()
        self.assertFalse(result["ok"], result)
        self.assertTrue(any(message in error for error in result["errors"]), result)
        return result

    def test_sized_final_object_leaves_four_unclaimed_bytes(self):
        result = self.verify()
        self.assertTrue(result["ok"], result)
        row = result["rows"][0]
        self.assertTrue(row["sizeOk"])
        self.assertEqual(row["end"], "0x0000101c")
        self.assertEqual(row["emittedBytes"], 28)
        self.assertEqual(row["terminalBssAnchor"], {
            "symbol": "terminal", "address": "0x1000", "size": "0x1c", "end": "0x101c",
            "moduleEnd": "0x1020", "alignment": 32, "unclaimedTailBytes": 4})

    def test_existing_symbol_start_anchor_does_not_need_new_rule(self):
        self.symbols = "terminal kind:bss addr:0x1000\nnext kind:bss addr:0x101c\n"
        self.homes["next"] = [("ov999", 0x101c)]
        result = self.verify()
        self.assertTrue(result["ok"], result)
        self.assertNotIn("terminalBssAnchor", result["rows"][0])

    def test_existing_module_end_anchor_does_not_need_size(self):
        self.header = self.header.replace("0x1020", "0x101c")
        self.symbols = self.symbols.replace("(size=0x1c)", "")
        result = self.verify()
        self.assertTrue(result["ok"], result)
        self.assertNotIn("terminalBssAnchor", result["rows"][0])

    def test_claiming_module_padding_still_fails_exact_contribution_size(self):
        self.claim["end"] = 0x1020
        self.refused(".bss emits 0x1c bytes, manifest claims 0x20")

    def test_unsized_object_cannot_borrow_its_candidate_size(self):
        self.symbols = self.symbols.replace("(size=0x1c)", "")
        self.refused("explicit configured BSS size")

    def test_invalid_or_conflicting_configured_sizes_fail_closed(self):
        for size in ("0", "-1", "bogus", "0x100000000", "0x1c,size=0x18"):
            with self.subTest(size=size):
                self.symbols = f"terminal kind:bss(size={size}) addr:0x1000\n"
                self.refused("terminal BSS anchor")

    def test_smaller_and_larger_configured_sizes_are_not_accepted(self):
        for size, message in (("0x18", "configured address/size"),
                              ("0x20", "crosses the proposed end")):
            with self.subTest(size=size):
                self.symbols = f"terminal kind:bss(size={size}) addr:0x1000\n"
                self.refused(message)

    def test_wrong_configured_kind_or_identity_fails(self):
        self.symbols = "terminal kind:data(word[7]) addr:0x1000\n"
        self.refused("explicit configured BSS size")
        self.symbols = "other kind:bss(size=0x1c) addr:0x1000\n"
        self.refused("one configured symbol row")

    def test_wrong_configured_address_fails(self):
        self.symbols = "terminal kind:bss(size=0x1c) addr:0x0ffc\n"
        self.refused("configured address/size")

    def test_other_module_cannot_supply_the_size(self):
        self.entry["module"] = "ov998"
        self.homes = {"terminal": [("ov998", 0x1000)]}
        self.refused("module delinks.txt and symbols.txt")

    def test_missing_or_invalid_section_alignment_fails(self):
        for replacement in ("", "align:0", "align:3", "align:0x18"):
            with self.subTest(alignment=replacement):
                self.header = "    .bss start:0x0fe0 end:0x1020 kind:bss " + replacement + "\n"
                self.refused("terminal BSS")

    def test_gap_of_one_alignment_or_more_is_not_padding_proof(self):
        self.header = self.header.replace("0x1020", "0x1040")
        self.refused("not solely the configured alignment tail")

    def test_end_must_round_to_the_actual_module_end(self):
        self.header = self.header.replace("0x1020", "0x1024")
        self.refused("not solely the configured alignment tail")

    def test_duplicate_container_is_ambiguous(self):
        self.header += self.header
        self.refused("one containing section")

    def test_second_container_with_missing_alignment_is_still_ambiguous(self):
        self.header += "    .bss start:0x0fe0 end:0x1020 kind:bss\n"
        self.refused("one containing section")

    def test_decimal_and_hexadecimal_config_values_are_supported(self):
        self.header = self.header.replace("align:32", "align:0x20")
        self.symbols = self.symbols.replace("size=0x1c", "size=28")
        result = self.verify()
        self.assertTrue(result["ok"], result)

    def test_symbol_in_tail_is_not_padding(self):
        self.symbols += "real_tail kind:bss addr:0x101e\n"
        self.refused("occupies the alleged BSS alignment tail")

    def test_sized_object_crossing_end_is_not_padding(self):
        self.symbols += "overlap kind:bss(size=0x24) addr:0x0ffc\n"
        self.refused("crosses the proposed end")

    def test_configured_interior_symbol_cannot_be_absorbed_by_terminal_array(self):
        self.symbols += "interior kind:bss addr:0x1018\n"
        self.refused("contains another configured symbol")

    def test_duplicate_symbol_row_or_alias_is_ambiguous(self):
        original = self.symbols
        self.symbols += original
        self.refused("one configured symbol row")
        self.symbols = original + "alias kind:bss addr:0x1000\n"
        self.refused("ambiguous configured aliases")

    def test_enrolled_tail_owner_is_not_padding(self):
        self.body = "src/tail.cpp:\n    complete\n    .bss start:0x101c end:0x1020\n"
        self.refused("owns the alleged BSS alignment tail")

    def test_manifest_size_still_has_to_agree(self):
        self.entry["bss"][0]["size"] = "0x18"
        result = self.refused("manifest says 0x18")
        self.assertTrue(any("manifest address/size" in e for e in result["errors"]))

    def test_missing_terminal_object_cannot_be_inferred(self):
        self.layout["symbols"]["terminal"]["size"] = 0x18
        self.refused("exactly one emitted final object")

    def test_end_proof_cannot_license_an_unanchored_start(self):
        self.claim["start"] = 0xffc
        self.layout["bytes"] = bytes(0x20)
        with mock.patch.object(TB, "_configured_terminal_bss_end") as fallback:
            self.refused("boundary anchors")
            fallback.assert_not_called()

    def test_wrong_symbol_home_is_still_rejected(self):
        self.homes = {"terminal": [("ov998", 0x1000)], "start": [("ov999", 0x1000)]}
        self.refused("no independent ov999 symbols.txt home")

    def test_unlicensed_emission_is_still_rejected(self):
        self.layout["symbols"]["extra"] = dict(self.layout["symbols"]["terminal"], size=4)
        self.refused("not licensed")

    def test_pinned_compiler_object_uses_the_same_boundary_path(self):
        if not (TB.M.MW / "2004/b56" / "mwccarm.exe").is_file():
            self.skipTest("requires the pinned mwccarm compiler")
        source = self.directory / "fixture.cpp"
        source.write_text('//cpp\nextern "C" int terminal[7];\nint terminal[7];\n', encoding="utf-8")
        flags = TB.RB.CFLAGS.replace("-lang c99", "-lang c++")
        raw = TB.M.compile_c(source, "2004/b56", flags)
        self.assertIsNotNone(raw, "pinned compiler failed")
        result = self.verify(raw)
        self.assertTrue(result["ok"], result)
        self.assertEqual(result["rows"][0]["emittedBytes"], 28)


if __name__ == "__main__":
    unittest.main()
