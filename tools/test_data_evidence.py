"""Evidence packets must not turn ambiguous identity or metadata into proof."""
import json
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import data_evidence as DE


class DataEvidenceTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.repo = pathlib.Path(self.temp.name)
        self.write("config/arm9/symbols.txt", """consumer kind:function(arm,size=0x20) addr:0x00000800
target kind:data(any) addr:0x00001000
target_alias kind:data(any) addr:0x00001000
next_data kind:data(any) addr:0x00001004
counter kind:bss addr:0x00002000
""")
        self.write("config/arm9/delinks.txt", """    .text start:0x00000800 end:0x00000900 kind:code align:4
    .data start:0x00001000 end:0x00001010 kind:data align:4
    .bss start:0x00002000 end:0x00002010 kind:bss align:4
src/consumer.c:
    complete
    .text start:0x00000800 end:0x00000820
    .data start:0x00001000 end:0x00001004
""")
        self.write("config/arm9/relocs.txt", """from:0x00000804 kind:load to:0x00001002 module:main
from:0x00001000 kind:load to:0x00001000 module:overlay(1)
""")
        self.write("config/arm9/overlays/ov001/symbols.txt", """other kind:data(any) addr:0x00001000
last kind:data(any) addr:0x00001008
""")
        self.write("config/arm9/overlays/ov001/delinks.txt",
                   "    .data start:0x00001000 end:0x00001010 kind:data align:4\n")
        self.write("config/arm9/overlays/ov001/relocs.txt",
                   "from:0x00001004 kind:load to:0x00001000 module:overlay(1)\n")
        self.write("src/consumer.c", "extern int target;\nint use(void) { return target_alias; }\nint target_extra;\n")
        self.write("include/data.h", "extern int target;\n")
        self.write("config/tu_manifest.d/arm9/consumer.json", json.dumps({
            "id": "arm9/consumer", "module": "arm9", "source": "src/consumer.c",
            "status": "promoted", "data": [{"symbol": "target", "size": "0x4",
                                              "evidence": "historical claim"}],
            "verification": {"result": "PASS"}}))

    def write(self, rel, text):
        path = self.repo / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
        return path

    def report(self, symbol="target", **kwargs):
        kwargs.setdefault("read_rom", False)
        with mock.patch.object(DE, "repository_state", return_value={"head": "fixture", "dirty": True}):
            return DE.build_report(self.repo, [symbol], **kwargs)

    def test_aliases_preserved_but_spacing_is_not_an_object_size(self):
        record = self.report()["objects"][0]
        self.assertEqual(record["aliases"], ["target", "target_alias"])
        self.assertIsNone(record["object_size"])
        self.assertIsNone(record["object_alignment"])
        self.assertEqual(record["window"]["size"], 4)
        self.assertEqual(record["window"]["basis"], "next-configured-boundary-upper-bound")

    def test_module_aware_relocations_keep_interior_offsets(self):
        refs = self.report()["objects"][0]["configured_references"]
        self.assertEqual(len(refs["incoming"]), 1)
        self.assertEqual(refs["incoming"][0]["target_offset"], 2)
        self.assertEqual(refs["incoming"][0]["source_context"]["symbols"], ["consumer"])
        self.assertEqual(refs["incoming"][0]["source_context"]["relation"], "inside-configured-function")
        self.assertEqual(refs["outgoing"][0]["target_module"], "ov001")
        self.assertEqual(refs["outgoing"][0]["target_context"]["symbols"], ["other"])

    def test_declared_owner_and_passing_manifest_do_not_become_verification(self):
        record = self.report()["objects"][0]
        self.assertEqual(record["declared_owners"][0]["source"], "src/consumer.c")
        self.assertTrue(record["declared_owners"][0]["complete"])
        self.assertEqual(record["manifest_claims"][0]["records"][0]["claim"]["evidence"], "historical claim")
        self.assertTrue(all(v == "not-run" for v in record["verification"].values()))
        self.assertTrue(all(v == "unreviewed" for v in record["interpretation"].values()))

    def test_text_references_match_alias_tokens_not_longer_names(self):
        report = self.report()
        hits = report["objects"][0]["source_text_references"]
        self.assertEqual(len(hits), 3)
        self.assertFalse(any("target_extra" in hit["text"] for hit in hits))
        self.assertIn("src/consumer.c", report["input_sha256"])
        self.assertIn("config/arm9/relocs.txt", report["input_sha256"])

    def test_last_symbol_window_stops_at_section_end(self):
        record = self.report("next_data")["objects"][0]
        self.assertEqual(record["window"]["end"], "0x00001010")
        self.assertEqual(record["window"]["size"], 12)

    def test_user_size_remains_an_observation_not_a_type_claim(self):
        record = self.report(size=3)["objects"][0]
        self.assertEqual(record["window"]["size"], 3)
        self.assertEqual(record["window"]["basis"], "user-requested-window")
        self.assertIsNone(record["object_size"])
        with self.assertRaisesRegex(ValueError, "inside"):
            self.report(size=17)
        with self.assertRaisesRegex(ValueError, "nonempty"):
            self.report(size=0)

    def test_ambiguous_names_require_module_even_at_the_same_address(self):
        self.write("config/arm9/overlays/ov001/symbols.txt",
                   "target kind:data(any) addr:0x00001000\n")
        with self.assertRaisesRegex(ValueError, "select --module"):
            self.report()
        record = self.report(module="overlay(1)")["objects"][0]
        self.assertEqual(record["id"], "ov001:0x00001000")

    def test_unknown_and_function_names_fail_explicitly(self):
        with self.assertRaisesRegex(ValueError, "found 0"):
            self.report("absent")
        with self.assertRaisesRegex(ValueError, "function symbols"):
            self.report("consumer")

    def test_bss_does_not_read_bytes_or_claim_a_match(self):
        record = self.report("counter", read_rom=True, registry=[])["objects"][0]
        self.assertEqual(record["rom"]["status"], "zero-fill-storage")
        self.assertNotIn("window_sha256", record["rom"])
        self.assertEqual(record["verification"]["storage_layout"], "not-run")

    def test_rom_observation_hashes_the_complete_window_including_tail(self):
        path = self.repo / "fixture.bin"
        raw = bytes(range(16))
        path.write_bytes(raw)
        registry = [{"name": "main", "base": 0x1000, "bin": path}]
        record = self.report(size=7, read_rom=True, registry=registry)["objects"][0]
        self.assertEqual(record["rom"]["module_sha256"], DE.digest(raw))
        self.assertEqual(record["rom"]["window_sha256"], DE.digest(raw[:7]))
        self.assertEqual(record["rom"]["observed_bytes"], 7)
        self.assertEqual(record["rom"]["preview_hex"], raw[:7].hex(" "))
        self.assertEqual(record["rom"]["preview_u32_le"], [0x03020100])
        self.assertEqual(record["verification"]["compiled_bytes"], "not-run")

    def test_missing_and_short_images_are_not_success(self):
        self.assertEqual(self.report(read_rom=True, registry=[])["objects"][0]["rom"]["status"], "unavailable")
        path = self.repo / "short.bin"
        path.write_bytes(b"x")
        record = self.report(read_rom=True, registry=[{"name": "main", "base": 0x1000, "bin": path}])["objects"][0]
        self.assertEqual(record["rom"]["status"], "window-outside-image")

    def test_missing_relocation_input_is_visible(self):
        (self.repo / "config/arm9/relocs.txt").unlink()
        refs = self.report()["objects"][0]["configured_references"]
        self.assertEqual(refs["missing_files"], ["config/arm9/relocs.txt"])

    def test_malformed_and_duplicate_relocations_fail_instead_of_disappearing(self):
        for text, message in (
            ("from:0xBADG kind:load to:0x00001002 module:main\n", "malformed relocation"),
            ("from:0x00000804 kind:load to:0x00001000 module:main\n" * 2, "duplicate relocation"),
        ):
            with self.subTest(message=message):
                self.write("config/arm9/relocs.txt", text)
                with self.assertRaisesRegex(ValueError, message):
                    self.report()

    def test_searched_files_without_hits_are_part_of_input_identity(self):
        self.write("src/unrelated.c", "int unrelated;\n")
        before = self.report()
        self.write("src/unrelated.c", "int renamed;\n")
        after = self.report()
        self.assertNotEqual(before["input_sha256"]["src/unrelated.c"],
                            after["input_sha256"]["src/unrelated.c"])
        self.assertIn("relocs.py", after["tool_sha256"])

    def test_linker_constants_remain_visible_without_invented_destinations(self):
        self.write("config/arm9/relocs.txt", "from:0x00000804 kind:link_time_const(ARM9_CTOR_START)\n")
        report = self.report()
        self.assertEqual(report["reference_inventory"]["link_time_constants"][0]["constant"], "ARM9_CTOR_START")
        self.assertEqual(report["reference_inventory"]["link_time_constants"][0]["target"], "not-resolved")
        self.assertEqual(report["objects"][0]["configured_references"]["incoming"], [])

    def test_a_window_reports_all_intersecting_owners(self):
        dl = self.repo / "config/arm9/delinks.txt"
        dl.write_text(dl.read_text() + "src/second.c:\n    complete\n"
                      "    .data start:0x00001004 end:0x00001008\n")
        owners = self.report(size=8)["objects"][0]["declared_owners"]
        self.assertEqual([o["source"] for o in owners], ["src/consumer.c", "src/second.c"])
        self.assertEqual([o["covers_window"] for o in owners], [False, False])
        self.assertEqual([o["contains_address"] for o in owners], [True, False])
        self.assertEqual(owners[1]["overlap_start"], "0x00001004")

    def test_batch_size_and_empty_selection_are_rejected(self):
        with self.assertRaisesRegex(ValueError, "exactly one"):
            DE.build_report(self.repo, ["target", "counter"], size=4, read_rom=False)
        with self.assertRaisesRegex(ValueError, "at least one"):
            DE.build_report(self.repo, [], read_rom=False)


if __name__ == "__main__":
    unittest.main()
