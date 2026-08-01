"""Stock/mod build profiles must stay disposable and make mods explicit."""
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rombuild_profile as RP  # noqa: E402


class RomBuildProfile(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        self.config = self.repo / "config" / "arm9"
        self.config.mkdir(parents=True)
        (self.repo / "src").mkdir()
        (self.repo / "mods").mkdir()
        (self.repo / "extracted" / "dsd").mkdir(parents=True)
        (self.repo / "build" / "build").mkdir(parents=True)
        (self.config / "config.yaml").write_text(
            "rom_config: ../../extracted/dsd/config.yaml\n"
            "build_path: ../../build\n"
            "delinks_path: ../../build/delinks\n"
            "main_module:\n"
            "  object: ../../build/build/arm9.bin\n",
            encoding="utf-8")
        self.original_delinks = (
            "    .text start:0x02000000 end:0x02000008 kind:code\n\n"
            "mods/Example.c:\n"
            "    complete\n"
            "    .text start:0x02000000 end:0x02000004\n")
        (self.config / "delinks.txt").write_text(self.original_delinks, encoding="utf-8")
        (self.repo / "src" / "Example.c").write_text("int Example(void) { return 0; }\n")
        (self.repo / "mods" / "Example.c").write_text("int Example(void) { return 1; }\n")

    def tearDown(self):
        self.tmp.cleanup()

    def prepare(self, profile):
        return RP.prepare_profile(profile, self.repo, self.config, self.repo / "build")

    def test_stock_redirects_mod_to_verified_source_without_touching_config(self):
        result = self.prepare("stock")
        generated = result["configRoot"]
        text = (generated / "delinks.txt").read_text(encoding="utf-8")
        self.assertIn("src/Example.c:", text)
        self.assertNotIn("mods/Example.c:", text)
        self.assertEqual((self.config / "delinks.txt").read_text(encoding="utf-8"),
                         self.original_delinks)
        self.assertEqual(result["modReplacements"], [{
            "name": "Example", "modPath": "mods/Example.c", "srcPath": "src/Example.c"}])
        self.assertEqual(result["modGapFallbacks"], [])

    def test_generated_config_paths_still_resolve_to_original_targets(self):
        generated = self.prepare("stock")["configRoot"]
        values = {}
        for line in (generated / "config.yaml").read_text(encoding="utf-8").splitlines():
            stripped = line.strip()
            if ": " in stripped:
                key, value = stripped.split(": ", 1)
                values[key] = value
        self.assertEqual((generated / values["rom_config"]).resolve(),
                         self.repo / "extracted" / "dsd" / "config.yaml")
        self.assertEqual((generated / values["build_path"]).resolve(), self.repo / "build")
        self.assertEqual((generated / values["object"]).resolve(),
                         self.repo / "build" / "build" / "arm9.bin")

    def test_mods_profile_preserves_mod_entry(self):
        result = self.prepare("mods")
        text = (result["configRoot"] / "delinks.txt").read_text(encoding="utf-8")
        self.assertIn("mods/Example.c:", text)
        self.assertEqual(result["modReplacements"], [])

    def test_stock_uses_rom_gap_without_a_verified_source_counterpart(self):
        (self.repo / "src" / "Example.c").unlink()
        result = self.prepare("stock")
        text = (result["configRoot"] / "delinks.txt").read_text(encoding="utf-8")
        self.assertIn("mods/Example.c:", text)
        self.assertNotIn("complete", text)
        self.assertEqual(result["modReplacements"], [])
        self.assertEqual(result["modGapFallbacks"][0]["name"], "Example")

    def test_stock_uses_rom_gap_for_a_nonmatching_counterpart(self):
        (self.repo / "src" / "Example.c").write_text(
            "// NONMATCHING\nint Example(void) { return 0; }\n")
        result = self.prepare("stock")
        text = (result["configRoot"] / "delinks.txt").read_text(encoding="utf-8")
        self.assertNotIn("complete", text)
        self.assertEqual(result["modGapFallbacks"][0]["name"], "Example")

    def test_profile_rejects_config_path_escape(self):
        config = self.config / "config.yaml"
        config.write_text(config.read_text(encoding="utf-8").replace(
            "../../extracted/dsd/config.yaml", "../../../../outside/config.yaml"),
            encoding="utf-8")
        with self.assertRaisesRegex(RP.ProfileError, "unsafe rom_config"):
            self.prepare("stock")


if __name__ == "__main__":
    unittest.main()
