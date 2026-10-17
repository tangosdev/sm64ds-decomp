"""A backlog census must preserve uncertainty and account for unnamed storage."""
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import data_inventory as DI


class DataInventoryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.repo = pathlib.Path(self.temp.name)
        self.write("config/arm9/symbols.txt", """caller kind:function(arm,size=0x20) addr:0x00000800
label kind:label(arm) addr:0x00000804
BuildInfo kind:data(any) addr:0x00000840
data_00001004 kind:data(any) addr:0x00001004
gData kind:data(any) addr:0x00001004
tail kind:data(any) addr:0x0000100c
counter kind:bss addr:0x00002000
outside kind:data(any) addr:0x00003000
""")
        self.write("config/arm9/delinks.txt", """    .text start:0x00000800 end:0x00000900 kind:code align:4
    .data start:0x00001000 end:0x00001010 kind:data align:4
    .bss start:0x00002000 end:0x00002010 kind:bss align:4
src/caller.cpp:
    complete
    .text start:0x00000800 end:0x00000820
    .data start:0x00001004 end:0x0000100c
""")
        self.write("config/arm9/relocs.txt", """from:0x00000808 kind:load to:0x00001004 module:main
from:0x0000080c kind:load to:0x0000100c module:main
from:0x00001004 kind:load to:0x00001004 module:overlay(1)
""")
        for number in (1, 2):
            root = f"config/arm9/overlays/ov{number:03d}"
            self.write(root + "/symbols.txt", "gData kind:data(any) addr:0x00001004\n")
            self.write(root + "/delinks.txt", "    .data start:0x00001000 end:0x00001010 kind:data align:4\n")
            self.write(root + "/relocs.txt", "")
        self.write("src/caller.cpp", "// gData is only a comment here.\nextern int gData;\n")
        self.write("include/globals.h", "extern int tail;\n")
        self.write("config/tu_manifest.d/arm9/caller.json", json.dumps({
            "module": "arm9", "id": "arm9/caller", "source": "src/caller.cpp",
            "status": "promoted", "data": [{"symbol": "gData", "size": "0x8"}],
            "verification": {"result": "PASS"}}))

    def write(self, path, text):
        p = self.repo / path
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8")
        return p

    def append(self, path, text):
        p = self.repo / path
        p.write_text(p.read_text() + text)

    def report(self):
        return DI.build_report(self.repo)

    def row(self, report, address="0x00001004", module="arm9"):
        return next(r for r in report["records"] if r["id"] == module + ":" + address)

    def test_aliases_share_record_but_overlay_identity_does_not(self):
        r = self.report()
        self.assertEqual(r["counts"]["configured_nonfunction_rows"], 9)
        self.assertEqual(r["counts"]["distinct_module_addresses"], 8)
        self.assertEqual(self.row(r)["aliases"], ["data_00001004", "gData"])
        self.assertEqual(self.row(r, module="ov001")["declared_ownership"], "no-complete-claim")

    def test_spacing_and_passing_manifest_are_not_type_or_build_proof(self):
        row = self.row(self.report())
        self.assertEqual(row["window"]["bytes"], 8)
        self.assertIsNone(row["object_size"])
        self.assertEqual(row["verification"], "not-run")
        self.assertEqual(row["manifest_claims"][0]["verification"], "not-reverified")
        self.assertTrue(all(v == "unreviewed" for v in row["interpretation"].values()))

    def test_unlabelled_prefix_and_tail_are_in_section_coverage(self):
        r = self.report()
        section = next(s for s in r["section_coverage"] if s["module"] == "arm9" and s["section"] == ".data")
        self.assertEqual(section["coverage_bytes"], {"no-complete-claim": 8, "single-complete-claim": 8})
        self.assertEqual(section["intervals"][0]["start"], "0x00001000")
        self.assertEqual(sum(i["bytes"] for i in section["intervals"]), section["bytes"])

    def test_labels_code_embedded_data_and_outside_constants_stay_separate(self):
        r = self.report()
        self.assertEqual(self.row(r, "0x00000804")["category"], "code-label")
        self.assertEqual(self.row(r, "0x00000840")["category"], "code-embedded-data")
        self.assertEqual(self.row(r, "0x00003000")["category"], "outside-or-ambiguous-section")
        self.assertIsNone(self.row(r, "0x00003000")["window"])

    def test_partial_and_competing_complete_claims_are_not_owners(self):
        self.append("config/arm9/delinks.txt", "src/other.cpp:\n    complete\n    .data start:0x00001008 end:0x00001010\n")
        r = self.report()
        self.assertEqual(self.row(r)["declared_ownership"], "conflicting-complete-claims")
        self.assertTrue(any(g["routing_basis"].get("ownership-conflict") for g in r["work_groups"]))
        p = self.repo / "config/arm9/delinks.txt"
        p.write_text(p.read_text().replace("    .data start:0x00001004 end:0x0000100c\n", ""))
        self.assertEqual(self.row(self.report())["declared_ownership"], "partial-window-claim")

    def test_wrong_section_claim_does_not_cover_storage(self):
        p = self.repo / "config/arm9/delinks.txt"
        p.write_text(p.read_text().replace("    .data start:0x00001004", "    .rodata start:0x00001004"))
        r = self.report()
        self.assertEqual(self.row(r)["declared_ownership"], "no-complete-claim")
        self.assertFalse(self.row(r)["claims"][0]["section_matches"])
        section = next(s for s in r["section_coverage"] if s["module"] == "arm9" and s["section"] == ".data")
        self.assertEqual(section["coverage_bytes"], {"no-complete-claim": 16})

    def test_ambiguous_overlays_are_not_resolved_incoming_edges(self):
        self.append("config/arm9/relocs.txt", "from:0x00000810 kind:load to:0x00001004 module:overlays(1,2)\n")
        r = self.report()
        one, two = self.row(r, module="ov001"), self.row(r, module="ov002")
        self.assertEqual(len(one["incoming_relocations"]), 1)
        self.assertEqual(len(two["incoming_relocations"]), 0)
        self.assertEqual(len(two["ambiguous_incoming_relocations"]), 1)
        self.assertEqual(len(r["reference_inventory"]["unresolved"][0]["candidate_windows"]), 2)

    def test_unlabelled_target_and_outside_symbol_references_are_preserved(self):
        self.append("config/arm9/relocs.txt", "from:0x00000810 kind:load to:0x00001002 module:main\nfrom:0x00000814 kind:load to:0x00003000 module:main\n")
        r = self.report()
        self.assertEqual(r["reference_inventory"]["unresolved"][0]["target"], "0x00001002")
        self.assertEqual(len(self.row(r, "0x00003000")["incoming_relocations"]), 1)

    def test_comments_and_externs_are_only_text_evidence(self):
        r = self.report()
        row = self.row(r, module="ov002")
        self.assertEqual(len(row["source_text_mentions"]), 2)
        self.assertTrue(all(h["module_ambiguous"] for h in row["source_text_mentions"]))
        self.assertEqual(row["declared_ownership"], "no-complete-claim")
        self.assertEqual(row["incoming_relocations"], [])

    def test_compiler_names_and_token_boundaries(self):
        self.append("config/arm9/symbols.txt", ".p__sinit_Foo.cpp kind:data(any) addr:0x00001000\n@12 kind:bss addr:0x00002008\n")
        self.write("src/caller.cpp", '// .p__sinit_Foo.cpp\n// @12wrong @123\n// @12\n')
        r = self.report()
        self.assertEqual(len(self.row(r, "0x00001000")["source_text_mentions"]), 1)
        self.assertEqual(len(self.row(r, "0x00002008")["source_text_mentions"]), 1)

    def test_single_resolved_source_routes_research_but_does_not_assign_ownership(self):
        self.append("config/arm9/relocs.txt", "from:0x00000850 kind:load to:0x0000100c module:main\n")
        r = self.report()
        group = next(g for g in r["work_groups"] if g["module"] == "arm9" and g["source"] == "src/caller.cpp")
        member = next(m for m in group["members"] if m["id"] == "arm9:0x0000100c")
        self.assertEqual(member["basis"], "single-resolved-source-candidate-not-owner")
        self.assertEqual(member["unassigned_referrer_sites"], 1)
        self.assertEqual(group["historical_owner"], "unreviewed")

    def test_bss_and_missing_relocation_inputs_are_visible_without_rom(self):
        (self.repo / "config/arm9/overlays/ov002/relocs.txt").unlink()
        r = self.report()
        self.assertEqual(self.row(r, "0x00002000")["section"], ".bss")
        self.assertIn("config/arm9/overlays/ov002/relocs.txt", r["reference_inventory"]["missing_files"])
        self.assertIn("config/arm9/symbols.txt", r["input_sha256"])

    def test_cross_module_consumer_is_not_a_tu_owner_candidate(self):
        r = self.report()
        group = next(g for g in r["work_groups"] if g["module"] == "ov001")
        self.assertIsNone(group["source"])
        self.assertEqual(group["cross_module_referrer_sites"], 1)
        self.assertEqual(group["members"][0]["basis"], "unassigned")


if __name__ == "__main__":
    unittest.main()
