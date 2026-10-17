"""TU treemap accounting and enrollment need neither a ROM nor a compiler."""
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tu_treemap as TT  # noqa: E402


class TuTreemapTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.repo = pathlib.Path(self.temp.name)
        (self.repo / "src").mkdir()
        (self.repo / "src" / "Tree.cpp").write_text("//cpp\n", encoding="utf-8")
        (self.repo / "src_tu").mkdir()
        (self.repo / "src_tu" / "Tree.cpp").write_text("//cpp\n", encoding="utf-8")
        self.db = {"functions": [
            {"module": "arm9", "addr": 4096, "size": 16, "name": "One", "matched": True},
            {"module": "arm9", "addr": 4112, "size": 32, "name": "Two", "matched": False},
            {"module": "ov002", "addr": 4096, "size": 8, "name": "Three", "matched": True},
        ]}
        self.entry = {"id": "main/Tree", "module": "main", "source": "src_tu/Tree.cpp",
                      "promoted_source": "src/Tree.cpp", "status": "promoted", "functions": [
                          {"address": "0x1000"}, {"address": "0x1010"}]}
        self.owners = {("arm9", 4096): "src/Tree.cpp", ("arm9", 4112): "src/Tree.cpp"}

    def gather(self, entries=None, owners=None):
        return TT.gather(self.db, [self.entry] if entries is None else entries,
                         self.owners if owners is None else owners, self.repo)

    def test_bytes_partition_and_module_scoped_addresses(self):
        groups = self.gather()
        self.assertEqual(sum(g["bytes"] for g in groups), 56)
        self.assertEqual(sum(len(g["functions"]) for g in groups), 3)
        self.assertEqual(groups[0]["bytes"], 48)
        self.assertEqual(groups[0]["matchedBytes"], 16)
        self.assertEqual(groups[1]["module"], "ov002")
        self.assertEqual(groups[1]["state"], "unassigned")

    def test_promoted_status_does_not_prove_enrollment(self):
        group = self.gather(owners={})[0]
        self.assertEqual(group["state"], "reconstructed")
        self.assertEqual(group["source"], "src_tu/Tree.cpp")
        self.assertEqual(group["status"], "promoted")

    def test_all_declared_functions_must_have_same_complete_owner(self):
        self.assertEqual(self.gather()[0]["state"], "production")
        self.owners[("arm9", 4112)] = "src/Other.cpp"
        self.assertEqual(self.gather()[0]["state"], "reconstructed")

    def test_missing_source_or_function_prevents_production_label(self):
        (self.repo / "src" / "Tree.cpp").unlink()
        self.assertEqual(self.gather()[0]["state"], "reconstructed")
        (self.repo / "src" / "Tree.cpp").write_text("//cpp\n", encoding="utf-8")
        self.entry["functions"].append({"address": "0x1020"})
        group = self.gather()[0]
        self.assertEqual(group["state"], "reconstructed")
        self.assertEqual(group["missing"], 1)
        self.assertEqual(group["bytes"], 48)

    def test_duplicate_membership_and_duplicate_db_are_rejected(self):
        other = dict(self.entry, id="main/Other")
        with self.assertRaisesRegex(ValueError, "claimed by both"):
            self.gather(entries=[self.entry, other])
        self.db["functions"].append(self.db["functions"][0])
        with self.assertRaisesRegex(ValueError, "duplicate function"):
            self.gather()

    def test_no_manifests_retains_entire_universe(self):
        groups = self.gather(entries=[])
        self.assertEqual(sum(g["bytes"] for g in groups), 56)
        self.assertTrue(all(g["state"] == "unassigned" for g in groups))

    def test_unlisted_initializer_joins_its_complete_source_owner(self):
        self.db["functions"].append({"module": "arm9", "addr": 8192, "size": 24,
                                     "name": "__sinit_Tree.cpp", "matched": True})
        self.owners[("arm9", 8192)] = "src/Tree.cpp"
        groups = self.gather()
        tree = groups[0]
        self.assertEqual(tree["bytes"], 72)
        self.assertEqual(tree["ownerAssignedFunctions"], 1)
        self.assertEqual(tree["functions"][-1]["membership"], "source")
        self.assertEqual(tree["functions"][0]["membership"], "manifest")
        self.assertEqual(sum(len(g["functions"]) for g in groups), 4)
        self.assertEqual(groups[1]["functions"][0]["membership"], "unassigned")

    def test_source_ownership_does_not_expand_incomplete_or_ambiguous_tus(self):
        self.owners[("arm9", 8192)] = "src/Tree.cpp"
        self.db["functions"].append({"module": "arm9", "addr": 8192, "size": 24,
                                     "name": "__sinit_Tree.cpp", "matched": True})
        # A second TU using the same source makes additional membership ambiguous.
        other = dict(self.entry, id="main/Other", functions=[{"address": "0x1010"}])
        self.entry["functions"] = [{"address": "0x1000"}]
        groups = self.gather(entries=[self.entry, other])
        self.assertEqual(sum(g["ownerAssignedFunctions"] for g in groups), 0)
        self.assertTrue(any(f["addr"] == 8192 for g in groups if g["state"] == "unassigned"
                            for f in g["functions"]))
        # A partially reconstructed manifest sharing that source still makes
        # ownership ambiguous, even when only the first TU is fully enrolled.
        other["functions"] = [{"address": "0x1030"}]
        groups = self.gather(entries=[self.entry, other])
        self.assertEqual(sum(g["ownerAssignedFunctions"] for g in groups), 0)
        self.entry["functions"].append({"address": "0x1020"})
        groups = self.gather()
        self.assertEqual(groups[0]["state"], "reconstructed")
        self.assertEqual(groups[0]["ownerAssignedFunctions"], 0)

    def test_complete_range_join_excludes_placeholders_and_partial_bodies(self):
        cfg = self.repo / "config" / "arm9"
        cfg.mkdir(parents=True)
        symbols = cfg / "symbols.txt"
        symbols.touch()
        (cfg / "delinks.txt").write_text(
            "src/Tree.cpp:\n    .text start:0x1000 end:0x1010\n    complete\n"
            "src/Other.cpp:\n    .text start:0x1010 end:0x1030\n",
            encoding="utf-8")
        owners = TT.complete_owners(self.db["functions"], [(symbols, "arm9")])
        self.assertEqual(owners, {("arm9", 4096): "src/Tree.cpp"})
        (cfg / "delinks.txt").write_text(
            "src/Tree.cpp:\n    complete\n    .text start:0x1000 end:0x1018\n",
            encoding="utf-8")
        self.assertEqual(TT.complete_owners(self.db["functions"], [(symbols, "arm9")]), owners)

    def test_links_and_embedded_json_are_safe(self):
        self.assertIsNone(TT.source_link("../outside.cpp", self.repo))
        self.entry["id"] = 'main/Tree</script><script>alert("x")</script>'
        groups = self.gather()
        out = self.repo / "tu.html"
        TT.render(groups, out)
        html = out.read_text(encoding="utf-8")
        self.assertNotIn(self.entry["id"], html)
        payload = html.split('<script id="tu-data" type="application/json">')[1].split("</script>")[0]
        self.assertEqual(json.loads(payload)["groups"][0]["id"], self.entry["id"])
        self.assertIn("src/Tree.cpp", groups[0]["sourceUrl"])
        self.assertIn("config/tu_manifest.d/", groups[0]["manifestUrl"])

    def test_layout_preserves_all_positive_area_groups(self):
        groups = self.gather()
        regions, boxes = TT.layout(groups)
        self.assertEqual(len(regions), 2)
        self.assertEqual({b["index"] for b in boxes}, {0, 1})
        self.assertTrue(all(b["w"] > 0 and b["h"] > 0 for b in boxes))


if __name__ == "__main__":
    unittest.main()
