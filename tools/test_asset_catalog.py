import pathlib
import sys
import tempfile
import unittest
from types import SimpleNamespace

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import asset_catalog as AC  # noqa: E402


class AssetCatalogTests(unittest.TestCase):
    def test_constant_keeps_path_and_extension(self):
        self.assertEqual(
            AC.constant_base("data/enemy/piano/piano.bmd"),
            "FILE_ID_DATA_ENEMY_PIANO_PIANO_BMD",
        )

    def test_sanitizer_collisions_get_stable_id_suffixes(self):
        assets = [AC.Asset(7, "a-b.bin", 1), AC.Asset(9, "a_b.bin", 2)]
        self.assertEqual(AC.constants_for(assets), {
            7: "FILE_ID_A_B_BIN_ID_0007",
            9: "FILE_ID_A_B_BIN_ID_0009",
        })

    def test_runtime_handle_table_is_separate_from_nitrofs_ids(self):
        base = 0x1000
        data = bytearray(0x80)
        table = 0x1010
        paths = ((0x1040, b"data/a.bmd\0"), (0x1050, b"data/b.kcl\0"))
        for index, (pointer, payload) in enumerate(paths):
            data[table - base + index * 4:table - base + index * 4 + 4] = \
                pointer.to_bytes(4, "little")
            data[pointer - base:pointer - base + len(payload)] = payload
        overlay = SimpleNamespace(ramAddress=base, data=data)
        handles = AC.handles_from_overlay(overlay, [
            AC.Asset(0x123, "data/a.bmd", 10),
            AC.Asset(0x456, "data/b.kcl", 20),
        ], table_address=table, count=2)
        self.assertEqual([(h.handle, h.file_id) for h in handles],
                         [(0, 0x123), (1, 0x456)])

    def test_reference_scan_resolves_only_literal_direct_ids(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            (root / "sample.c").write_text(
                "void f(void) {\n"
                "  LoadFile(0x25c);\n"
                "  LoadFile(variable);\n"
                "  LoadCompressedFileAt(0x9807, dst);\n"
                "}\n",
                encoding="utf-8",
            )
            old_repo = AC.REPO
            try:
                AC.REPO = root
                rows = AC.scan_references(root, [
                    AC.AssetHandle(0x25C, 0x123, "data/enemy/piano/piano.bmd", 100),
                ])
            finally:
                AC.REPO = old_repo
        self.assertEqual(len(rows), 2)
        self.assertEqual(rows[0]["status"], "runtime-handle")
        self.assertEqual(rows[0]["path"], "data/enemy/piano/piano.bmd")
        self.assertEqual(rows[1]["status"], "encoded-or-unresolved")

    def test_candidates_include_global_field_and_actor_layout_evidence(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            src = root / "src"
            init = src / "unnamed" / "ov063" / "__sinit_ov063_test.c"
            actor = src / "actors" / "MadPiano" / "InitResources.cpp"
            init.parent.mkdir(parents=True)
            actor.parent.mkdir(parents=True)
            init.write_text(
                "void init(void) {\n"
                "  func_02017acc(data_ov063_0211ef80, 0x40a);\n"
                "  func_02017acc(t + 0x20, 0x40c);\n"
                "}\n",
                encoding="utf-8",
            )
            actor.write_text(
                "extern int data_ov063_0211ef80[];\n"
                "void use(void) { Model_Load(data_ov063_0211ef80); }\n",
                encoding="utf-8",
            )
            handles = [
                AC.AssetHandle(0x40A, 0x25C, "data/enemy/piano/piano.bmd", 100),
                AC.AssetHandle(0x40C, 0x25E,
                               "data/enemy/piano/piano_attack.bca", 80),
            ]
            old_repo = AC.REPO
            try:
                AC.REPO = root
                rows = AC.scan_references(src, handles)
                candidates = AC.build_rename_candidates(rows, src)
                layouts = AC.build_layout_candidates(rows, candidates)
            finally:
                AC.REPO = old_repo

        global_row = next(row for row in candidates
                          if row["candidate_kind"] == "global")
        self.assertEqual(global_row["current_name"], "data_ov063_0211ef80")
        self.assertEqual(global_row["suggested_name"], "gPianoModelFile")
        self.assertEqual(global_row["confidence"], "high")
        self.assertEqual(global_row["payload_type"], "BMD_File")
        self.assertIn("src/actors/MadPiano/InitResources.cpp",
                      global_row["consumer_sources"])

        field_row = next(row for row in candidates
                         if row["candidate_kind"] == "field")
        self.assertEqual(field_row["current_name"], "t+0x20")
        self.assertEqual(field_row["suggested_name"], "mPianoAttackAnimationFile")
        self.assertEqual(field_row["confidence"], "medium")

        self.assertEqual(len(layouts), 1)
        self.assertEqual(layouts[0]["confidence"], "high")
        self.assertEqual(
            layouts[0]["suggested_path"],
            "src/actors/MadPiano/__sinit_ov063_test.c",
        )

    def test_candidate_blocks_one_owner_with_multiple_assets(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            src = root / "src"
            src.mkdir()
            (src / "sample.c").write_text(
                "void f(void) {\n"
                "  func_02017acc(data_ov047_02112658, 1);\n"
                "  func_02017b4c(data_ov047_02112658, 2);\n"
                "}\n",
                encoding="utf-8",
            )
            old_repo = AC.REPO
            try:
                AC.REPO = root
                rows = AC.scan_references(src, [
                    AC.AssetHandle(1, 10, "data/stage/a.bmd", 10),
                    AC.AssetHandle(2, 11, "data/stage/b.kcl", 20),
                ])
                candidates = AC.build_rename_candidates(rows, src)
            finally:
                AC.REPO = old_repo

        self.assertEqual(len(candidates), 1)
        self.assertEqual(candidates[0]["confidence"], "blocked")
        self.assertEqual(candidates[0]["suggested_name"], "")
        self.assertIn("multiple asset paths", candidates[0]["blocker"])

    def test_mangled_resource_method_supplies_medium_confidence_layout_owner(self):
        self.assertEqual(
            AC.resource_owner_from_source(
                "src/_ZN14QuestionSwitch13InitResourcesEv.cpp"
            ),
            ("QuestionSwitch", "mangled-resource-method"),
        )
        self.assertIsNone(
            AC.resource_owner_from_source("src/_ZN7Message11DisplayTextEt.cpp")
        )

    @staticmethod
    def _fake_rom(directory, fnt=(0x100, 0x10), fat=(0x120, 0x20), size=0x200,
                  ovt9=(0x140, 0x20), ovt7=(0, 0)):
        """A blob shaped like a cartridge as far as the NitroFS words go.

        Only the header offsets matter here: 0x40 fnt_offset, 0x44 fnt_size,
        0x48 fat_offset, 0x4c fat_size, and the overlay half at 0x50 arm9 and
        0x58 arm7. The spans are filled with distinct byte patterns so a copy
        that reads the wrong offset cannot pass. The arm7 pair defaults to
        0 + 0, which is what SM64DS's own cartridge carries.
        """
        import struct
        blob = bytearray(size)
        struct.pack_into("<IIII", blob, AC.ROM_HEADER_FNT,
                         fnt[0], fnt[1], fat[0], fat[1])
        struct.pack_into("<IIII", blob, AC.ROM_HEADER_OVT,
                         ovt9[0], ovt9[1], ovt7[0], ovt7[1])
        for off, length, first in ((fnt[0], fnt[1], 0x10),
                                   (fat[0], fat[1], 0x80),
                                   (ovt9[0], ovt9[1], 0x40)):
            # only fill what fits; a slice assignment that ran off the end
            # would GROW the bytearray and quietly repair the very truncation
            # the refusal test is about
            fits = max(0, min(length, size - off))
            blob[off:off + fits] = bytes(range(first, first + fits))
        path = pathlib.Path(directory) / "fake.nds"
        path.write_bytes(bytes(blob))
        return path

    def test_nitrofs_tables_are_copied_verbatim_at_the_header_offsets(self):
        with tempfile.TemporaryDirectory() as tmp:
            rom = self._fake_rom(tmp)
            out = pathlib.Path(tmp) / "assets"
            meta = AC.write_nitrofs_tables(out, rom)
            self.assertEqual(meta, {"fnt_offset": 0x100, "fnt_size": 0x10,
                                    "fat_offset": 0x120, "fat_size": 0x20,
                                    "ovt9_offset": 0x140, "ovt9_size": 0x20,
                                    "ovt7_offset": 0, "ovt7_size": 0})
            self.assertEqual((out / "nitrofs_fnt.bin").read_bytes(),
                             bytes(range(0x10, 0x20)))
            self.assertEqual((out / "nitrofs_fat.bin").read_bytes(),
                             bytes(range(0x80, 0xa0)))
            self.assertEqual((out / "nitrofs_ovt9.bin").read_bytes(),
                             bytes(range(0x40, 0x60)))
            # An empty arm7 pair writes no file at all -- an empty blob would
            # read as a table that exists and happens to be zero long.
            self.assertFalse((out / "nitrofs_ovt7.bin").exists())
            rows = (out / "nitrofs.tsv").read_text(encoding="utf-8").splitlines()
            self.assertEqual(rows[0], "key\tvalue")
            self.assertIn("fat_offset\t288", rows)
            self.assertIn("ovt9_offset\t320", rows)
            self.assertIn("ovt7_size\t0", rows)

    def test_nitrofs_span_outside_the_image_is_refused(self):
        # A span that runs off the end is a wrong-version or truncated ROM.
        # Copying it short would produce a name table the walker reads past.
        with tempfile.TemporaryDirectory() as tmp:
            rom = self._fake_rom(tmp, fat=(0x1f0, 0x40))
            with self.assertRaises(ValueError) as caught:
                AC.write_nitrofs_tables(pathlib.Path(tmp) / "assets", rom)
            self.assertIn("fat", str(caught.exception))

    def test_overlay_table_span_outside_the_image_is_refused(self):
        # A zero pair is legal (that processor has no overlays); a non-empty
        # pair that runs off the end is a truncated or wrong-version ROM, and
        # a short copy would hand the ROM's own overlay reader a record it
        # would then copy code from.
        with tempfile.TemporaryDirectory() as tmp:
            rom = self._fake_rom(tmp, ovt9=(0x1f0, 0x40))
            with self.assertRaises(ValueError) as caught:
                AC.write_nitrofs_tables(pathlib.Path(tmp) / "assets", rom)
            self.assertIn("arm9 overlay table", str(caught.exception))

    def test_overlay_table_size_must_be_whole_records(self):
        # 32 bytes per OverlayInfo. A size that is not a multiple of it means
        # the header was read at the wrong offset, and every record after the
        # first would be shifted.
        with tempfile.TemporaryDirectory() as tmp:
            rom = self._fake_rom(tmp, ovt9=(0x140, 0x21))
            with self.assertRaises(ValueError) as caught:
                AC.write_nitrofs_tables(pathlib.Path(tmp) / "assets", rom)
            self.assertIn("32-byte OverlayInfo", str(caught.exception))


if __name__ == "__main__":
    unittest.main()
