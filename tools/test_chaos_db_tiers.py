"""Optional linked, class and TU columns retain their provenance checks."""
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import chaos_db_ci as CDB  # noqa: E402
import tiers  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent


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

    def stamp(self, commit="abc1234"):
        (self.root / "config" / "port_linkage.json").write_text(json.dumps({
            "linkedTus": 2, "matchedTus": 3, "measuredAt": "2026-10-16",
            "branch": "port/x", "commit": commit}), encoding="utf-8")

    def listing(self, commit="abc1234", keys=("arm9:0x02000000", "ov001:0x02100000")):
        (self.root / "config" / "port_linked_functions.txt").write_text(
            "# header\n# commit: " + commit + "\n" + "\n".join(keys) + "\n", encoding="utf-8")


class PortedList(Scratch):
    def test_a_list_from_the_stamped_build_is_believed(self):
        self.stamp()
        self.listing()
        self.assertEqual(CDB.ported_functions(), {"arm9:0x02000000", "ov001:0x02100000"})

    def test_a_list_from_another_build_is_ignored(self):
        self.stamp("abc1234")
        self.listing("def5678")
        self.assertIsNone(CDB.ported_functions())

    def test_a_list_with_no_stamp_is_ignored(self):
        self.listing()
        self.assertIsNone(CDB.ported_functions())

    def test_a_stamp_with_no_provenance_does_not_vouch_for_a_list(self):
        (self.root / "config" / "port_linkage.json").write_text(
            json.dumps({"linkedTus": 2, "matchedTus": 3}), encoding="utf-8")
        self.listing()
        self.assertIsNone(CDB.ported_functions())

    def test_no_list_means_no_column(self):
        self.stamp()
        self.assertIsNone(CDB.ported_functions())


class PortedBarCountsFunctions(Scratch):
    """The PORTED bar counts functions, like MATCHED; units are detail."""

    def setUp(self):
        super().setUp()
        saved = (tiers.PORTED_STAMP, tiers.PORTED_FUNCTIONS)
        tiers.PORTED_STAMP = self.root / "config" / "port_linkage.json"
        tiers.PORTED_FUNCTIONS = self.root / "config" / "port_linked_functions.txt"
        self.addCleanup(lambda: (setattr(tiers, "PORTED_STAMP", saved[0]),
                                 setattr(tiers, "PORTED_FUNCTIONS", saved[1])))

    def listing(self, commit="abc1234", universe=5, keys=("arm9:0x02000000", "ov001:0x02100000")):
        lines = ["# header", "# commit: " + commit]
        if universe is not None:
            lines.append(f"# universe: {universe}")
        (self.root / "config" / "port_linked_functions.txt").write_text(
            "\n".join(lines + list(keys)) + "\n", encoding="utf-8")

    def test_a_list_makes_the_bar_per_function_and_keeps_the_unit_detail(self):
        self.stamp()
        self.listing()
        k = tiers.ported()
        self.assertEqual((k["unit"], k["ported"], k["of"], k["pct"]), ("functions", 2, 5, 40.0))
        self.assertEqual((k["portedTus"], k["matchedTus"]), (2, 3))

    def test_an_older_stamp_with_no_list_still_reads_as_units(self):
        self.stamp()
        k = tiers.ported()
        self.assertEqual((k["unit"], k["ported"], k["of"]), ("units", 2, 3))

    def test_a_list_from_another_build_leaves_the_unit_reading(self):
        self.stamp("abc1234")
        self.listing("def5678")
        self.assertEqual(tiers.ported()["unit"], "units")

    def test_a_list_with_no_universe_leaves_the_unit_reading(self):
        self.stamp()
        self.listing(universe=None)
        self.assertEqual(tiers.ported()["unit"], "units")

    def test_no_stamp_means_no_bar(self):
        self.listing()
        self.assertIsNone(tiers.ported())

    def test_the_published_tier_shares_the_function_denominator(self):
        self.stamp()
        self.listing()
        t = CDB.tier_stats(linked_n=7, total=9, ported_n=2, ported_total=7)
        self.assertEqual((t["linked"], t["linkedOf"], t["linkedPct"]), (7, 9, 77.78))
        self.assertEqual((t["ported"], t["portedOf"], t["portedPct"]), (2, 7, 28.57))
        self.assertEqual((t["portedUnits"], t["portedUnitsOf"]), (2, 3))
        self.assertFalse(any(k.startswith("converted") for k in t))

    def test_the_published_tier_falls_back_to_units_without_a_list(self):
        self.stamp()
        t = CDB.tier_stats(linked_n=7, total=9, ported_n=None, ported_total=7)
        self.assertEqual((t["linked"], t["linkedOf"]), (7, 9))
        self.assertEqual((t["ported"], t["portedOf"]), (2, 3))

    def test_the_readme_rows_say_functions_or_tus(self):
        self.stamp()
        self.listing()
        block = lambda: tiers.bar_block({"matched": None,
                                         "linked": {"linked": 7, "of": 9, "pct": 77.78},
                                         "ported": tiers.ported()})
        self.assertTrue(block().splitlines()[-2].endswith("2 / 5 functions"))
        self.assertTrue(block().splitlines()[-3].endswith("7 / 9 functions"))
        (self.root / "config" / "port_linked_functions.txt").unlink()
        block2 = tiers.bar_block({"matched": None,
                                  "linked": {"linked": 7, "of": 9, "pct": 77.78},
                                  "ported": tiers.ported()}).splitlines()
        self.assertTrue(block2[-2].endswith("2 / 3 matched TUs"), block2)


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


class CommittedListAgreesWithTheStamp(unittest.TestCase):
    """The tripwire for a hand-refreshed stamp. chaos_db_ci would only LOG that it ignored a
    list from another build, and a quiet missing column is how an Atlas goes grey for weeks."""

    def test_the_committed_files_describe_one_build(self):
        listing = REPO / "config" / "port_linked_functions.txt"
        if not listing.is_file():
            self.skipTest("no committed per-function list")
        stamp = tiers.ported()
        self.assertIsNotNone(stamp, "config/port_linkage.json has no usable stamp")
        header = {}
        for line in listing.read_text(encoding="utf-8").splitlines():
            if line.startswith("# ") and ": " in line:
                k, v = line[2:].split(": ", 1)
                header[k] = v
        self.assertEqual(header.get("commit"), stamp["commit"],
                         "config/port_linked_functions.txt and config/port_linkage.json come from "
                         "different port builds; re-run tools/port_linked_functions.py --stamp")
        self.assertEqual(header.get("branch"), stamp["branch"])
        self.assertEqual(int(header["linkedTus"]), stamp["portedTus"])
        self.assertEqual(int(header["matchedTus"]), stamp["matchedTus"])
        self.assertEqual(stamp["unit"], "functions", "the committed list must back the bar")
        self.assertEqual(stamp["ported"], int(header["functions"]))
        self.assertEqual(stamp["of"], int(header["universe"]))

    def test_every_key_is_a_module_and_an_address(self):
        listing = REPO / "config" / "port_linked_functions.txt"
        if not listing.is_file():
            self.skipTest("no committed per-function list")
        import re
        key = re.compile(r"^(arm9|itcm|ov\d{3}):0x[0-9a-f]{8}$")
        keys = [l for l in listing.read_text(encoding="utf-8").splitlines()
                if l and not l.startswith("#")]
        self.assertTrue(keys)
        self.assertEqual(len(keys), len(set(keys)), "duplicate keys")
        self.assertEqual(keys, sorted(keys), "the list is written sorted")
        bad = [k for k in keys if not key.match(k)]
        self.assertEqual(bad[:5], [])


if __name__ == "__main__":
    unittest.main()
