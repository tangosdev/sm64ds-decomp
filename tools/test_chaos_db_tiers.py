"""LINKED, class and TU columns retain their provenance checks."""
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import chaos_db_ci as CDB  # noqa: E402
import tiers  # noqa: E402


class ClassOf(unittest.TestCase):
    def test_a_method_has_its_class(self):
        self.assertEqual(CDB.class_of("_ZN10dMgPsOpt_c5CloseEv"), "dMgPsOpt_c")

    def test_a_destructor_has_its_class(self):
        self.assertEqual(CDB.class_of("_ZN8dActor_cD0Ev"), "dActor_c")

    def test_a_namespace_counts_as_the_owner(self):
        self.assertEqual(CDB.class_of("_ZN2GX13SetBankForTexEt"), "GX")

    def test_a_nested_owner_keeps_its_outer_scope(self):
        self.assertEqual(CDB.class_of("_ZN5Sound6Player21SetPlayableSeqCountEi"),
                         "Sound::Player")

    def test_a_thunk_names_the_class_it_adjusts(self):
        self.assertEqual(CDB.class_of("_ZThn80_N9ModelAnimD0Ev"), "ModelAnim")

    def test_placeholders_and_c_names_have_no_owner(self):
        for n in ("func_02011654", "func_ov006_020f15ac", "_dmul", "__aeabi_idiv", "memcpy"):
            self.assertIsNone(CDB.class_of(n), n)

    def test_a_free_mangled_function_has_no_owner(self):
        self.assertIsNone(CDB.class_of("_Z4Funcv"))

    def test_vague_linkage_data_has_no_owner(self):
        self.assertIsNone(CDB.class_of("_ZTV8dActor_c"))


class Scratch(unittest.TestCase):
    """A throwaway REPO for the readers that take their inputs from config/."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = pathlib.Path(self.tmp.name)
        self.addCleanup(self.tmp.cleanup)
        (self.root / "config").mkdir()
        saved = CDB.REPO
        CDB.REPO = self.root
        self.addCleanup(lambda: setattr(CDB, "REPO", saved))


class LinkedIsLiveRomLink(Scratch):
    """LINKED counts enrolled functions live from committed config, no stamp."""

    def authors(self, keys=("arm9:0x02000000", "arm9:0x02000010", "ov001:0x02100000")):
        (self.root / "function-authors.json").write_text(
            json.dumps({"functions": {k: {"author": "t", "name": "f"} for k in keys}}),
            encoding="utf-8")

    def entry(self, module_path, source, start, end, complete=True):
        d = self.root / "config" / module_path
        d.mkdir(parents=True, exist_ok=True)
        dl = d / "delinks.txt"
        with open(dl, "a", encoding="utf-8") as f:
            f.write(f"{source}:\n")
            if complete:
                f.write("    complete\n")
            f.write(f"    .text start:{start} end:{end}\n\n")

    def test_enrolled_functions_count_and_gaps_do_not(self):
        self.authors()
        self.entry("arm9", "src/a.c", "0x02000000", "0x02000014")
        k = tiers.linked(authors_path=self.root / "function-authors.json",
                         config_root=self.root / "config")
        # 0x02000000 and 0x02000010 are inside the range; 0x02100000 has no entry
        self.assertEqual((k["linked"], k["of"], k["pct"], k["unit"]),
                         (2, 3, 66.67, "functions"))

    def test_an_unenrolled_range_does_not_count(self):
        self.authors()
        self.entry("arm9", "src/a.c", "0x02000000", "0x02000014", complete=False)
        k = tiers.linked(authors_path=self.root / "function-authors.json",
                         config_root=self.root / "config")
        self.assertEqual((k["linked"], k["of"]), (0, 3))

    def test_no_authors_file_means_no_bar(self):
        k = tiers.linked(authors_path=self.root / "function-authors.json",
                         config_root=self.root / "config")
        self.assertIsNone(k)


class UnitIndex(Scratch):
    def entry(self, name, module, status, addrs):
        d = self.root / "config" / "tu_manifest.d" / module
        d.mkdir(parents=True, exist_ok=True)
        (d / f"{name}.json").write_text(json.dumps({
            "id": f"{module}/{name}", "module": module, "status": status,
            "functions": [{"symbol": f"s{a}", "address": hex(a), "size": "0x4"} for a in addrs],
        }), encoding="utf-8")

    def test_a_function_gets_its_units_id_and_state(self):
        self.entry("Actor", "arm9", "promoted", [0x2000000, 0x2000010])
        self.entry("Other", "ov001", "text-verified", [0x2100000])
        idx = CDB.unit_index()
        self.assertEqual(idx[("arm9", 0x2000000)], ("arm9/Actor", "promoted"))
        self.assertEqual(idx[("arm9", 0x2000010)], ("arm9/Actor", "promoted"))
        self.assertEqual(idx[("ov001", 0x2100000)], ("ov001/Other", "text-verified"))

    def test_the_same_address_in_another_module_is_another_function(self):
        self.entry("Actor", "arm9", "promoted", [0x2000000])
        self.assertNotIn(("ov001", 0x2000000), CDB.unit_index())

    def test_no_manifest_is_an_empty_index(self):
        self.assertEqual(CDB.unit_index(), {})

    def test_an_entry_missing_its_state_is_skipped_not_fatal(self):
        d = self.root / "config" / "tu_manifest.d" / "arm9"
        d.mkdir(parents=True)
        (d / "x.json").write_text(json.dumps({"id": "arm9/x", "module": "arm9",
                                              "functions": [{"address": "0x2000000"}]}),
                                  encoding="utf-8")
        self.assertEqual(CDB.unit_index(), {})


if __name__ == "__main__":
    unittest.main()
