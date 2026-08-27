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
                "src/game/actors/QuestionSwitch/_ZN14QuestionSwitch13InitResourcesEv.cpp"
            ),
            ("QuestionSwitch", "mangled-resource-method"),
        )
        self.assertIsNone(
            AC.resource_owner_from_source("src/ui/messages/Message/_ZN7Message11DisplayTextEt.cpp")
        )


if __name__ == "__main__":
    unittest.main()
