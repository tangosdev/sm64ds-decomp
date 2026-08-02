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


if __name__ == "__main__":
    unittest.main()
