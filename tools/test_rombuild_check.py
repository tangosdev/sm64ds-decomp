"""Structured ROM metrics keep stock mismatches and intentional mods distinct."""
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rombuild_check as RBC  # noqa: E402


class RomBuildCheck(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = pathlib.Path(self.tmp.name)
        self.config = self.root / "config" / "arm9"
        self.config.mkdir(parents=True)
        self.built = self.root / "build"
        self.retail = self.root / "retail"
        self.built.mkdir()
        (self.retail / "arm9").mkdir(parents=True)
        self.old_build, self.old_extracted = RBC.BUILD, RBC.EXTRACTED
        RBC.BUILD, RBC.EXTRACTED = self.built, self.retail
        (self.config / "symbols.txt").write_text(
            "Example kind:function(arm,size=0x4) addr:0x00001000\n"
            "Other kind:function(arm,size=0x4) addr:0x00001004\n",
            encoding="utf-8")
        self.write_delinks("src/Example.c")
        (self.retail / "arm9" / "arm9.bin").write_bytes(b"ABCDEFGH")
        (self.built / "arm9.bin").write_bytes(b"ABCDEFGH")

    def tearDown(self):
        RBC.BUILD, RBC.EXTRACTED = self.old_build, self.old_extracted
        self.tmp.cleanup()

    def write_delinks(self, rel, end=0x00001004):
        (self.config / "delinks.txt").write_text(
            "    .text start:0x00001000 end:0x00001008 kind:code\n\n"
            f"{rel}:\n"
            "    complete\n"
            f"    .text start:0x00001000 end:0x{end:08x}\n",
            encoding="utf-8")

    def test_stock_exact_reports_fidelity_and_source_coverage(self):
        report = RBC.analyze(self.config, "stock")
        self.assertTrue(report["passed"])
        self.assertEqual(report["moduleFidelity"]["percent"], 100.0)
        self.assertEqual(report["sourceBuild"]["sourceFunctions"], 1)
        self.assertEqual(report["sourceBuild"]["sourceBytes"], 4)
        self.assertEqual(report["sourceBuild"]["sourceBytesPercent"], 50.0)

    def test_shared_source_counts_every_owned_function(self):
        self.write_delinks("src/actors/Pair.cpp", end=0x00001008)
        report = RBC.analyze(self.config, "stock")
        self.assertTrue(report["passed"])
        self.assertEqual(report["sourceBuild"]["sourceFunctions"], 2)
        self.assertEqual(report["sourceBuild"]["reproducingFunctions"], 2)
        self.assertEqual(report["sourceBuild"]["sourceBytes"], 8)

    def test_intact_entry_counts_code_members_and_nontext_separately(self):
        (self.config / "delinks.txt").write_text(
            "    .text start:0x00001000 end:0x00001008 kind:code\n"
            "    .data start:0x00001008 end:0x0000100c kind:data\n\n"
            "src/actors/Pair.cpp:\n"
            "    complete\n"
            "    .text start:0x00001000 end:0x00001008\n"
            "    .data start:0x00001008 end:0x0000100c\n",
            encoding="utf-8")
        (self.retail / "arm9" / "arm9.bin").write_bytes(b"ABCDEFGHIJKL")
        (self.built / "arm9.bin").write_bytes(b"ABCDEFGHIJKL")

        text = (self.config / "delinks.txt").read_text(encoding="utf-8")
        self.assertEqual(RBC.complete_entries_text(text),
                         [("src/actors/Pair.cpp", 0x1000, 0x1008)])
        self.assertEqual(RBC.complete_entry_sections_text(text), [
            ("src/actors/Pair.cpp", ".text", 0x1000, 0x1008),
            ("src/actors/Pair.cpp", ".data", 0x1008, 0x100c),
        ])

        report = RBC.analyze(self.config, "stock")
        self.assertTrue(report["passed"])
        self.assertEqual(report["sourceBuild"]["sourceFunctions"], 2)
        self.assertEqual(report["sourceBuild"]["sourceBytes"], 8)
        self.assertEqual(report["moduleComposition"]["sourceDataBytes"], 4)
        self.assertEqual(report["moduleComposition"]["unownedDataBytes"], 0)

    def test_module_paths_accept_config_or_arm9_as_the_root(self):
        self.assertEqual(RBC.module_label(self.config, self.config), "arm9")
        self.assertEqual(RBC.module_label(self.config, self.config.parent), "arm9")

    def test_stock_rejects_any_module_difference(self):
        (self.built / "arm9.bin").write_bytes(b"XBCDEFGH")
        report = RBC.analyze(self.config, "stock")
        self.assertFalse(report["passed"])
        self.assertEqual(report["moduleFidelity"]["unexpectedDifferingBytes"], 1)
        self.assertEqual(report["sourceBuild"]["mismatchingFunctions"], 1)

    def test_mods_allows_only_differences_inside_explicit_mod_range(self):
        self.write_delinks("mods/Example.c")
        (self.built / "arm9.bin").write_bytes(b"XBCDEFGH")
        report = RBC.analyze(self.config, "mods")
        self.assertTrue(report["passed"])
        self.assertEqual(report["moduleFidelity"]["differingBytes"], 1)
        self.assertEqual(report["moduleFidelity"]["unexpectedDifferingBytes"], 0)
        self.assertEqual(report["sourceBuild"]["modBytes"], 4)

    def test_mods_rejects_difference_outside_mod_range(self):
        self.write_delinks("mods/Example.c")
        (self.built / "arm9.bin").write_bytes(b"ABCDXFGH")
        report = RBC.analyze(self.config, "mods")
        self.assertFalse(report["passed"])
        self.assertEqual(report["moduleFidelity"]["unexpectedDifferingBytes"], 1)


if __name__ == "__main__":
    unittest.main()
