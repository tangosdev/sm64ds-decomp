"""Independent negative controls for source contribution proof; no ROM required."""
import copy
import pathlib
import struct
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import data_provenance as DP  # noqa: E402


def symbol_elf(symbol="table", section=".data", size=7, address=0,
               nobits=False, alignment=1, linked=False):
    """A small real ARM ELF, including named sections and an OBJECT symbol."""
    strings = b"\0" + symbol.encode() + b"\0"
    section_strings = b"\0"
    offsets = {}
    for name in (section, ".symtab", ".strtab", ".shstrtab"):
        offsets[name] = len(section_strings)
        section_strings += name.encode() + b"\0"
    body = b"" if nobits else bytes(size)
    symbols = bytes(16) + struct.pack("<IIIBBH", 1, address, size, 0x11, 0, 1)
    body_offset = 52
    sym_offset = body_offset + len(body)
    str_offset = sym_offset + len(symbols)
    names_offset = str_offset + len(strings)
    headers_offset = names_offset + len(section_strings)
    headers = bytes(40)
    headers += struct.pack("<IIIIIIIIII", offsets[section], 8 if nobits else 1,
                           3, address if linked else 0, body_offset, size,
                           0, 0, alignment, 0)
    headers += struct.pack("<IIIIIIIIII", offsets[".symtab"], 2, 0, 0,
                           sym_offset, len(symbols), 3, 1, 4, 16)
    headers += struct.pack("<IIIIIIIIII", offsets[".strtab"], 3, 0, 0,
                           str_offset, len(strings), 0, 0, 1, 0)
    headers += struct.pack("<IIIIIIIIII", offsets[".shstrtab"], 3, 0, 0,
                           names_offset, len(section_strings), 0, 0, 1, 0)
    header = b"\x7fELF\x01\x01\x01" + bytes(9)
    header += struct.pack("<HHIIIIIHHHHHH", 2 if linked else 1, 40, 1,
                          0, 0, headers_offset, 0, 52, 0, 0, 40, 5, 4)
    return header + body + symbols + strings + section_strings + headers


def map_text(module="ov047", section=".data", address=0x02112408,
             size=7, provider="thing.o", name="table"):
    return ("# Unreferenced symbols:\n\n# Link map of Entry\n"
            "  1](table,object) found in global irrelevant.o\n\n"
            f"# .{module}\n"
            f"#>{address:08X}          {module.upper()}_DATA_START (linker command file)\n"
            f"  {address:08X} {size:08X} {section:<7} {name}\t({provider})\n")


def lcf_text(module="ov047", section=".data", provider="thing.o"):
    return ("SECTIONS {\n"
            f"    .{module} : {{\n"
            "        ALIGNALL(4);\n"
            f"        {provider}({section})\n"
            f"    }} > {module.upper()}\n"
            "}\n")


class LinkerArtifactParsing(unittest.TestCase):
    def test_map_preserves_module_when_addresses_collide(self):
        rows = DP.parse_map(map_text(module="ov046") + map_text(module="ov047"))
        self.assertEqual([(r["module"], r["address"]) for r in rows],
                         [("ov046", 0x02112408), ("ov047", 0x02112408)])

    def test_map_uses_memory_rows_not_closure_mentions(self):
        rows = DP.parse_map(map_text(name="Namespace::Table[7]"))
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0]["object"], "thing.o")
        self.assertEqual(rows[0]["size"], 7)
        self.assertEqual(rows[0]["display_name"], "Namespace::Table[7]")

    def test_selectors_keep_overlay_and_section_identity(self):
        rows = DP.parse_lcf_selectors(lcf_text() + lcf_text("ov048", ".bss"))
        self.assertEqual([(r["module"], r["section"], r["object"]) for r in rows],
                         [("ov047", ".data", "thing.o"),
                          ("ov048", ".bss", "thing.o")])

    def test_conflicting_lcf_output_module_is_rejected(self):
        with self.assertRaises(ValueError):
            DP.parse_lcf_selectors(lcf_text().replace("> OV047", "> OV048"))

    def test_malformed_memory_record_is_not_silently_ignored(self):
        with self.assertRaises(ValueError):
            DP.parse_map(map_text().replace("00000007", "NOTASIZE"))

    def test_exception_index_heading_preserves_arm9_until_memory_map(self):
        text = """# .arm9
# Exception index
  020737A8 0000027C         Exception Table Index\t()
  02075B04 00000010 .rodata constant\t(thing.o)
# Memory map:
  02075B14 00000004 .data   summary_entry\t(unrelated.o)
"""
        rows = DP.parse_map(text)
        self.assertEqual(len(rows), 1)
        self.assertEqual((rows[0]["module"], rows[0]["address"], rows[0]["object"]),
                         ("arm9", 0x02075B04, "thing.o"))


class ContributionProof(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.repo = pathlib.Path(self.temp.name)
        self.build = self.repo / "build"
        self.source = self.repo / "src" / "thing.cpp"
        self.source.parent.mkdir(parents=True)
        self.source.write_text("int table[2];\n", encoding="utf-8")
        self.obj = self.build / "src" / "thing.o"
        self.obj.parent.mkdir(parents=True)
        self.linked = self.build / "final_link.o"
        self.built = self.build / "ov047.bin"
        self.retail = self.repo / "retail.bin"
        self.body = b"abcdxyz"
        self.built.write_bytes(self.body)
        self.retail.write_bytes(self.body)
        self.request = {
            "module": "ov047", "symbol": "table", "address": 0x02112408,
            "size": 7, "section": ".data", "source": "src/thing.cpp",
            "repo": str(self.repo), "build_root": str(self.build),
            "expected_object": str(self.obj),
            "config_section": {"start": 0x02112408, "end": 0x0211240f,
                               "kind": "data", "alignment": 1},
        }
        self.objects = {"thing.o": [self.obj]}
        self.rows = DP.parse_map(map_text())
        self.selectors = DP.parse_lcf_selectors(lcf_text())
        self.image = {"base": 0x02112408, "built": self.built, "retail": self.retail}
        self.write_objects()

    def write_objects(self, *, size=7, section=".data", module="OV047",
                      address=0x02112408, nobits=False, alignment=1):
        self.obj.write_bytes(symbol_elf(size=size, section=section,
                                        nobits=nobits, alignment=alignment))
        self.linked.write_bytes(symbol_elf(size=size, section=module,
                                           address=address, linked=True))

    def verify(self):
        return DP.verify_record(self.request, self.rows, self.selectors,
                                self.objects, self.linked, self.image)

    def assert_failed(self, result):
        self.assertTrue(result["errors"], result)
        self.assertEqual(result["status"], "refused", result)

    def test_source_provider_with_exact_linked_bytes_passes(self):
        result = self.verify()
        self.assertFalse(result["errors"], result)
        self.assertEqual(result["status"], "source-bytes-verified", result)

    def test_identical_rom_bytes_from_gap_object_are_rejected(self):
        self.rows = DP.parse_map(map_text(provider="_dsd_gap@ov047_0.o"))
        self.assertEqual(self.built.read_bytes(), self.retail.read_bytes())
        self.assert_failed(self.verify())

    def test_same_address_and_size_in_another_overlay_do_not_count(self):
        self.rows = DP.parse_map(map_text(module="ov048"))
        self.assert_failed(self.verify())

    def test_wrong_overlay_in_linked_elf_is_rejected(self):
        self.write_objects(module="OV048")
        self.assert_failed(self.verify())

    def test_lcf_selector_from_another_overlay_does_not_count(self):
        self.selectors = DP.parse_lcf_selectors(lcf_text(module="ov048"))
        self.assert_failed(self.verify())

    def test_duplicate_input_basenames_are_ambiguous(self):
        other = self.build / "other" / "thing.o"
        other.parent.mkdir()
        other.write_bytes(self.obj.read_bytes())
        self.objects["thing.o"].append(other)
        self.assert_failed(self.verify())

    def test_source_named_object_from_delinks_is_still_rom_fallback(self):
        delinked = self.build / "delinks" / "src" / "thing.o"
        delinked.parent.mkdir(parents=True)
        delinked.write_bytes(self.obj.read_bytes())
        self.objects["thing.o"] = [delinked]
        self.assert_failed(self.verify())

    def test_overlap_from_another_provider_cannot_hide_behind_an_exact_row(self):
        other = dict(self.rows[0], object="_dsd_gap@ov047_0.o", size=1,
                     address=self.request["address"] + 6)
        self.rows.append(other)
        self.assert_failed(self.verify())

    def test_map_range_must_cover_the_whole_requested_object(self):
        self.rows = DP.parse_map(map_text(size=4))
        self.assert_failed(self.verify())

    def test_map_provider_must_start_at_the_requested_address(self):
        self.rows = DP.parse_map(map_text(address=0x02112409))
        self.assert_failed(self.verify())

    def test_compiled_symbol_size_cannot_be_a_matching_prefix(self):
        self.obj.write_bytes(symbol_elf(size=4))
        self.assert_failed(self.verify())

    def test_corrupt_prepared_or_final_elf_is_an_explicit_refusal(self):
        for path in (self.obj, self.linked):
            with self.subTest(path=path.name):
                original = path.read_bytes()
                try:
                    path.write_bytes(b"invalid ELF")
                    self.assert_failed(self.verify())
                finally:
                    path.write_bytes(original)

    def test_one_changed_trailing_byte_is_rejected(self):
        self.built.write_bytes(self.body[:-1] + b"!")
        self.assert_failed(self.verify())

    def test_a_changed_pointer_word_is_rejected(self):
        self.built.write_bytes(b"ABCD" + self.body[4:])
        self.assert_failed(self.verify())

    def test_module_image_base_must_agree_with_linked_elf(self):
        self.image["base"] -= 4
        self.built.write_bytes(bytes(4) + self.body)
        self.retail.write_bytes(bytes(4) + self.body)
        self.assert_failed(self.verify())

    def test_equal_truncated_images_cannot_pass(self):
        self.built.write_bytes(self.body[:4])
        self.retail.write_bytes(self.body[:4])
        self.assert_failed(self.verify())

    def test_config_section_cannot_end_before_requested_range(self):
        self.request["config_section"]["end"] -= 1
        self.assert_failed(self.verify())

    def bss(self):
        self.request.update(size=8, section=".bss")
        self.request["config_section"].update(end=0x02112410, kind="bss", alignment=4)
        self.rows = DP.parse_map(map_text(section=".bss", size=8))
        self.selectors = DP.parse_lcf_selectors(lcf_text(section=".bss"))
        self.write_objects(size=8, section=".bss", nobits=True, alignment=4)

    def test_bss_uses_storage_layout_without_inventing_rom_bytes(self):
        self.bss()
        self.built.unlink()
        self.retail.unlink()
        result = self.verify()
        self.assertFalse(result["errors"], result)
        self.assertEqual(result["status"], "source-storage-layout-verified", result)
        self.assertEqual(result["linked_bytes"]["status"], "not-applicable-no-ROM-payload")
        self.assertEqual(result["startup_initialization"], "not-run")

    def test_bss_request_smaller_than_compiled_storage_is_rejected(self):
        self.bss()
        self.request["size"] = 4
        self.rows = DP.parse_map(map_text(section=".bss", size=4))
        self.assert_failed(self.verify())

    def test_bss_label_does_not_turn_initialized_data_into_nobits(self):
        self.bss()
        self.obj.write_bytes(symbol_elf(size=8, section=".bss", nobits=False))
        self.assert_failed(self.verify())

    def test_bss_alignment_is_checked(self):
        self.bss()
        self.obj.write_bytes(symbol_elf(size=8, section=".bss", nobits=True, alignment=16))
        self.assert_failed(self.verify())


class SnapshotBinding(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.repo = pathlib.Path(self.temp.name)
        self.build = self.repo / "build"
        self.build.mkdir()
        self.obj = self.write_bytes("build/src/thing.o", symbol_elf())
        self.source = self.write_text("src/thing.cpp",
                                      '#include "thing.h"\nconst char table[7] = {};\n')
        self.header = self.write_text("include/thing.h", "extern const char table[7];\n")
        self.write_text("config/arm9/symbols.txt", "")
        self.write_text("config/arm9/delinks.txt", "")
        self.symbols = self.write_text("config/arm9/overlays/ov047/symbols.txt",
                                       "table kind:data(any) addr:0x02112408\n")
        self.relocs = self.write_text("config/arm9/overlays/ov047/relocs.txt", "")
        self.delinks = self.write_text("config/arm9/overlays/ov047/delinks.txt", """
    .data start:0x02112408 end:0x0211240f kind:data align:1
src/thing.cpp:
    complete
    .data start:0x02112408 end:0x0211240f
""")
        self.lcf = self.write_text("build/arm9.lcf", lcf_text())
        self.object_list = self.write_text("build/objects.txt", f'"{self.obj}"\n')
        self.retail = self.write_bytes("retail.bin", b"abcdxyz")
        self.built = self.build / "ov047.bin"
        self.linked = self.build / "final_link.o"
        self.map = self.build / "final_link.o.xMAP"
        self.request = {"module": "ov047", "symbol": "table", "address": 0x02112408,
                        "size": 7, "section": ".data", "source": "src/thing.cpp"}
        self.images = {"ov047": {"base": 0x02112408,
                                  "built": self.built, "retail": self.retail}}

    def write_bytes(self, name, content):
        path = self.repo / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(content)
        return path

    def write_text(self, name, content):
        return self.write_bytes(name, content.encode("utf-8"))

    def snapshot(self):
        return DP.snapshot_link_inputs(self.repo, self.build, [copy.deepcopy(self.request)],
                                        self.images)

    def linked_outputs(self):
        self.linked.write_bytes(symbol_elf(section="OV047", address=0x02112408, linked=True))
        self.map.write_text(map_text(), encoding="utf-8")
        self.built.write_bytes(self.retail.read_bytes())

    def test_fresh_link_outputs_and_unchanged_inputs_pass(self):
        snapshot = self.snapshot()
        self.linked_outputs()
        report = DP.audit_link(snapshot)
        self.assertTrue(report["ok"], report)
        self.assertEqual(len(report["objects"]), 1)
        self.assertTrue(report["input_sha256"])
        self.assertTrue(report["artifact_sha256"])

    def test_existing_link_outputs_are_not_accepted_as_a_fresh_run(self):
        for artifact in (self.linked, self.map, self.built):
            with self.subTest(artifact=artifact.name):
                artifact.write_bytes(b"stale output")
                with self.assertRaises(ValueError):
                    self.snapshot()
                artifact.unlink()

    def test_missing_new_outputs_cannot_pass(self):
        report = DP.audit_link(self.snapshot())
        self.assertFalse(report["ok"], report)

    def test_input_or_output_changed_during_verification_invalidates_receipt(self):
        verify = DP.verify_record
        for path in (self.obj, self.built):
            with self.subTest(path=path.name):
                snapshot = self.snapshot()
                self.linked_outputs()
                original = path.read_bytes()

                def mutate_after_observation(*args, **kwargs):
                    result = verify(*args, **kwargs)
                    path.write_bytes(original + b"changed during audit")
                    return result

                try:
                    with mock.patch.object(DP, "verify_record", side_effect=mutate_after_observation):
                        report = DP.audit_link(snapshot)
                    self.assertFalse(report["ok"], report)
                    self.assertTrue(any("changed during audit" in error
                                        for error in report["errors"]), report)
                    self.assertFalse(report["objects"][0]["errors"], report)
                finally:
                    path.write_bytes(original)
                    for artifact in (self.linked, self.map, self.built):
                        artifact.unlink()

    def test_mutated_link_input_cannot_reuse_an_equal_final_image(self):
        # Header/compiler freshness is bound before compilation by the build wrapper.
        for path in (self.source, self.obj, self.lcf,
                     self.object_list, self.symbols, self.delinks, self.relocs, self.retail):
            with self.subTest(path=path.relative_to(self.repo).as_posix()):
                snapshot = self.snapshot()
                old = path.read_bytes()
                self.linked_outputs()
                try:
                    path.write_bytes(old + b"\nmutation")
                    report = DP.audit_link(snapshot)
                    self.assertFalse(report["ok"], report)
                finally:
                    path.write_bytes(old)
                    for artifact in (self.linked, self.map, self.built):
                        artifact.unlink()

    def test_noncomplete_declared_owner_cannot_claim_compiled_provider(self):
        self.delinks.write_text(self.delinks.read_text().replace("    complete\n", ""),
                                encoding="utf-8")
        with self.assertRaises(ValueError):
            self.snapshot()

    def test_source_without_a_declared_range_is_rejected(self):
        self.request["source"] = "src/another.cpp"
        self.write_text("src/another.cpp", "const char table[7] = {};\n")
        with self.assertRaises(ValueError):
            self.snapshot()


if __name__ == "__main__":
    unittest.main()
