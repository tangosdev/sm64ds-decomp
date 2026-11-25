"""The data tracker joins claims and build verdicts without inventing either."""
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import data_tiers as DT


def rec(address, section=".data", ownership="complete-window-claimed",
        naming="placeholder-only", aliases=None, size=8, category="global-storage",
        source=None):
    aliases = aliases or [f"data_{address + 0x20000000:08x}"]
    claims = []
    if source:
        claims.append({"source": source, "section": section,
                       "start": f"0x{address:08x}", "end": f"0x{address + size:08x}",
                       "complete": ownership == "complete-window-claimed",
                       "section_matches": True,
                       "covers_window": ownership == "complete-window-claimed"})
    return {
        "id": f"ov002:0x{address:08x}", "module": "ov002",
        "address": f"0x{address:08x}", "aliases": aliases,
        "category": category, "section": section,
        "window": {"start": f"0x{address:08x}", "end": f"0x{address + size:08x}",
                   "bytes": size},
        "object_size": None, "naming": naming,
        "declared_ownership": ownership, "claims": claims,
        "source_text_mentions": [], "manifest_claims": [],
        "verification": "not-run",
    }


def inventory(records):
    return {
        "records": records,
        "section_coverage": [
            {"module": "ov002", "section": ".data", "kind": "data",
             "bytes": 0x100, "coverage_bytes": {
                 "single-complete-claim": 0x60, "no-complete-claim": 0xa0}},
            {"module": "ov002", "section": ".bss", "kind": "bss",
             "bytes": 0x80, "coverage_bytes": {
                 "single-complete-claim": 0x30, "no-complete-claim": 0x50}},
        ],
        "repository": {"head": "test"},
    }


def verification(by_address, **extra):
    out = {
        "available": True, "by_address": by_address, "by_symbol": {},
        "symbols": len(by_address), "verified": 0,
        "differs": 0, "unnamed": 0, "verifiedBytes": 0,
        "sourceBuild": None,
    }
    out.update(extra)
    return out


class SummarizeTests(unittest.TestCase):
    def test_an_excluded_address_is_not_in_the_total(self):
        rows = [
            rec(0x02075f14),
            rec(0x02082158),
        ]
        rows[0]["module"] = "arm9"
        rows[0]["id"] = "arm9:0x02075f14"
        rows[1]["module"] = "arm9"
        rows[1]["id"] = "arm9:0x02082158"
        rows[1]["address"] = "0x02082158"
        report = DT.summarize(inventory(rows), verification({}))
        self.assertEqual(report["counts"]["items"], 1)
        self.assertEqual(report["items"][0]["address"], "0x02082158")

    def test_address_is_the_item_and_aliases_do_not_double_count(self):
        report = DT.summarize(
            inventory([rec(0x100, aliases=["data_1", "named_table"])]),
            verification({}))
        self.assertEqual(report["counts"]["items"], 1)
        self.assertEqual(report["items"][0]["aliases"], ["data_1", "named_table"])
        self.assertEqual(report["items"][0]["status"], "CLAIMED")

    def test_a_short_compare_stays_claimed_or_unclaimed(self):
        rows = [
            rec(0x100),
            rec(0x108),
            rec(0x110),
            rec(0x118, ownership="no-complete-claim"),
        ]
        ver = verification({
            "ov002:0x00000100": "VERIFIED",
            "ov002:0x00000108": "PARTIAL",
            "ov002:0x00000110": "DIFFERS",
        }, verified=1, differs=1, verifiedBytes=8)
        report = DT.summarize(inventory(rows), ver)
        self.assertEqual([r["status"] for r in report["items"]],
                         ["VERIFIED", "CLAIMED", "DIFFERS", "UNCLAIMED"])
        self.assertEqual(report["counts"]["claimed"], 3)
        self.assertNotIn("partial", report["counts"])

    def test_bss_is_storage_and_does_not_borrow_a_rom_verdict(self):
        rows = [rec(0x200, section=".bss"), rec(0x208, section=".bss",
                                                ownership="no-complete-claim")]
        ver = verification({"ov002:0x00000200": "VERIFIED"}, verified=1)
        report = DT.summarize(inventory(rows), ver)
        self.assertEqual([r["status"] for r in report["items"]],
                         ["BSS_CLAIMED", "BSS_UNCLAIMED"])
        self.assertEqual(report["counts"]["verified"], 0)
        self.assertEqual(report["bytes"]["sectionCoverage"]["bss:single-complete-claim"], 0x30)

    def test_readme_counts_bss_with_the_rest_of_the_data(self):
        rows = [rec(0x100), rec(0x200, section=".bss"),
                rec(0x208, section=".bss", ownership="no-complete-claim")]
        report = DT.summarize(inventory(rows), verification({}))
        self.assertEqual(report["counts"]["claimed"], 2)
        self.assertEqual(report["counts"]["items"], 3)
        block = DT.readme_block(report)
        data_lines = [line for line in block.splitlines() if line.startswith("DATA")]
        self.assertEqual(data_lines, [
            "DATA       " + DT.bar(2, 3) + "  66.7%   2 / 3"])
        self.assertNotIn("BSS", block)

    def test_named_is_independent_of_verification(self):
        rows = [rec(0x300, naming="existing-nonplaceholder-unreviewed"),
                rec(0x308)]
        report = DT.summarize(inventory(rows), verification({}))
        self.assertEqual(report["counts"]["named"], 1)
        self.assertNotIn("credited", report["counts"])
        self.assertNotIn("authors", report)

    def test_a_short_compare_does_not_change_a_verified_address(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "romdata.json"
            path.write_text(json.dumps([
                {"module": "ov002", "symbol": "data_a", "addr": 0x100,
                 "bytes": 8, "verdict": "VERIFIED"},
                {"module": "ov002", "symbol": "data_b", "addr": 0x100,
                 "bytes": 4, "verdict": "PARTIAL"},
            ]), encoding="utf-8")
            loaded = DT.load_verification(path)
        self.assertEqual(loaded["by_address"]["ov002:0x00000100"], "VERIFIED")

    def test_code_labels_are_not_data_items(self):
        report = DT.summarize(
            inventory([rec(0x400, category="code-label")]), verification({}))
        self.assertEqual(report["counts"]["items"], 0)


class ReportRenderingTests(unittest.TestCase):
    def test_readme_block_is_idempotent_and_preserves_neighbours(self):
        report = DT.summarize(
            inventory([rec(0x100, naming="existing-nonplaceholder-unreviewed")]),
            verification({"ov002:0x00000100": "VERIFIED"},
                         verified=1, verifiedBytes=8))
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "README.md"
            path.write_text("# Title\n\n<!-- tiers:end -->\n\nTail\n", encoding="utf-8")
            DT.write_readme(path, report)
            first = path.read_text(encoding="utf-8")
            DT.write_readme(path, report)
            second = path.read_text(encoding="utf-8")
        self.assertEqual(first, second)
        self.assertIn(DT.README_START, first)
        self.assertIn("DATA       ", first)
        self.assertNotIn("BSS", first)
        self.assertIn("Tail", first)
        self.assertEqual(first.count(DT.README_START), 1)

    def test_readme_does_not_wait_on_a_build_report(self):
        report = DT.summarize(inventory([rec(0x100)]), DT.load_verification(None))
        self.assertFalse(report["verification"]["available"])
        block = DT.readme_block(report)
        self.assertIn("1 / 1", block)
        self.assertNotIn("unavailable", block)

    def test_differs_wins_over_a_verified_sibling(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "romdata.json"
            path.write_text(json.dumps([
                {"module": "ov002", "symbol": "data_1", "addr": 0x100,
                 "bytes": 8, "verdict": "VERIFIED"},
                {"module": "ov002", "symbol": "data_1", "addr": 0x100,
                 "bytes": 8, "verdict": "DIFFERS"},
            ]), encoding="utf-8")
            ver = DT.load_verification(path)
        self.assertEqual(ver["by_address"]["ov002:0x00000100"], "DIFFERS")
        self.assertEqual(ver["differs"], 1)
        self.assertEqual(ver["verified"], 0)

    def test_report_normalizes_a_romdata_record_list(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "romdata.json"
            path.write_text(json.dumps([
                {"module": "ov002", "symbol": "data_1", "addr": 0x100,
                 "bytes": 8, "verdict": "VERIFIED"},
                {"module": "ov002", "symbol": "data_2", "addr": 0x108,
                 "bytes": 3, "verdict": "PARTIAL"},
                {"module": "ov002", "symbol": "data_3", "addr": 0x110,
                 "bytes": 5, "verdict": "DIFFERS"},
            ]), encoding="utf-8")
            ver = DT.load_verification(path)
        self.assertEqual(ver["by_address"]["ov002:0x00000100"], "VERIFIED")
        self.assertNotIn("ov002:0x00000108", ver["by_address"])
        self.assertEqual(ver["by_address"]["ov002:0x00000110"], "DIFFERS")
        self.assertEqual((ver["verified"], ver["differs"]), (1, 1))

    def test_report_normalizes_a_rombuild_romdata_report(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "report.json"
            path.write_text(json.dumps({"romData": {
                "symbols": 2, "verified": 1, "partial": 1, "differs": 0,
                "unnamed": 0, "verifiedBytes": 8, "partialBytes": 3,
                "verifiedSymbols": [{"module": "ov002", "symbol": "data_1",
                                     "addr": 0x100, "bytes": 8}],
                "differingSymbols": [],
            }}), encoding="utf-8")
            ver = DT.load_verification(path)
        self.assertEqual(ver["verified"], 1)
        self.assertEqual(ver["by_address"], {"ov002:0x00000100": "VERIFIED"})


if __name__ == "__main__":
    unittest.main()
