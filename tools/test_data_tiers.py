"""The data tracker joins claims, build verdicts and credit without inventing any."""
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
        "symbols": len(by_address), "verified": 0, "partial": 0,
        "differs": 0, "unnamed": 0, "verifiedBytes": 0, "partialBytes": 0,
        "sourceBuild": None,
    }
    out.update(extra)
    return out


class SummarizeTests(unittest.TestCase):
    def test_address_is_the_item_and_aliases_do_not_double_count(self):
        report = DT.summarize(
            inventory([rec(0x100, aliases=["data_1", "named_table"])]),
            verification({}), {})
        self.assertEqual(report["counts"]["items"], 1)
        self.assertEqual(report["items"][0]["aliases"], ["data_1", "named_table"])
        self.assertEqual(report["items"][0]["status"], "CLAIMED")

    def test_verified_partial_and_differ_are_distinct_tiers(self):
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
        }, verified=1, partial=1, differs=1, verifiedBytes=8, partialBytes=3)
        report = DT.summarize(inventory(rows), ver, {})
        self.assertEqual([r["status"] for r in report["items"]],
                         ["VERIFIED", "PARTIAL", "DIFFERS", "UNCLAIMED"])
        self.assertEqual(report["counts"]["verified"], 1)
        self.assertEqual(report["counts"]["partial"], 1)
        self.assertEqual(report["counts"]["differs"], 1)
        self.assertEqual(report["counts"]["unclaimed"], 1)

    def test_bss_is_storage_and_does_not_borrow_a_rom_verdict(self):
        rows = [rec(0x200, section=".bss"), rec(0x208, section=".bss",
                                                ownership="no-complete-claim")]
        ver = verification({"ov002:0x00000200": "VERIFIED"}, verified=1)
        report = DT.summarize(inventory(rows), ver, {})
        self.assertEqual([r["status"] for r in report["items"]],
                         ["BSS_CLAIMED", "BSS_UNCLAIMED"])
        self.assertEqual(report["counts"]["verified"], 0)
        self.assertEqual(report["bytes"]["sectionCoverage"]["bss:single-complete-claim"], 0x30)

    def test_named_and_credit_are_independent_of_verification(self):
        rows = [rec(0x300, naming="existing-nonplaceholder-unreviewed"),
                rec(0x308)]
        authors = {"ov002:0x00000300": {"author": "lunavyqo"}}
        report = DT.summarize(inventory(rows), verification({}), authors)
        self.assertEqual(report["counts"]["named"], 1)
        self.assertEqual(report["counts"]["credited"], 1)
        self.assertEqual(report["authors"], [{"author": "lunavyqo", "items": 1,
                                              "windowBytes": 8}])
        self.assertEqual(report["items"][1]["author"], None)

    def test_code_labels_are_not_data_items(self):
        report = DT.summarize(
            inventory([rec(0x400, category="code-label")]), verification({}), {})
        self.assertEqual(report["counts"]["items"], 0)


class CreditTests(unittest.TestCase):
    def setUp(self):
        self.rows = DT.summarize(
            inventory([rec(0x100, source="src/a.c"), rec(0x108, source="src/b.c")]),
            verification({}), {})

    def test_credit_source_only_touches_the_named_source(self):
        authors = {}
        added = DT.credit_source(self.rows, authors, "src/a.c", "alice")
        self.assertEqual(added, 1)
        self.assertEqual(authors["ov002:0x00000100"]["author"], "alice")
        self.assertNotIn("ov002:0x00000108", authors)

    def test_new_claim_must_be_credited_but_baseline_rows_are_allowed(self):
        base = {"uncredited": ["ov002:0x00000100", "ov002:0x00000108"]}
        errors, _warnings = DT.check(self.rows, {}, base)
        self.assertEqual(errors, [])
        base = {"uncredited": ["ov002:0x00000100"]}
        errors, _warnings = DT.check(self.rows, {}, base)
        self.assertEqual(len(errors), 1)
        self.assertIn("ov002:0x00000108", errors[0])
        credited = {"ov002:0x00000108": {"author": "alice"}}
        errors, _warnings = DT.check(self.rows, credited, base)
        self.assertEqual(errors, [])

    def test_author_row_must_resolve_and_name_a_claim(self):
        errors, _warnings = DT.validate_authors(
            {"ov002:0x00000100": {"author": ""}}, self.rows["items"])
        self.assertTrue(errors)
        errors, _warnings = DT.validate_authors(
            {"ov002:0xdeadbeef": {"author": "alice"}}, self.rows["items"])
        self.assertTrue(errors)
        errors, _warnings = DT.validate_authors(
            {"ov002:0x00000100": {"author": "alice"}}, self.rows["items"])
        self.assertEqual(errors, [])


class ReportRenderingTests(unittest.TestCase):
    def test_readme_block_is_idempotent_and_preserves_neighbours(self):
        report = DT.summarize(
            inventory([rec(0x100, naming="existing-nonplaceholder-unreviewed")]),
            verification({"ov002:0x00000100": "VERIFIED"},
                         verified=1, verifiedBytes=8), {})
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "README.md"
            path.write_text("# Title\n\n<!-- tiers:end -->\n\nTail\n", encoding="utf-8")
            DT.write_readme(path, report)
            first = path.read_text(encoding="utf-8")
            DT.write_readme(path, report)
            second = path.read_text(encoding="utf-8")
        self.assertEqual(first, second)
        self.assertIn(DT.README_START, first)
        self.assertIn("DATA CLAIMED", first)
        self.assertIn("Tail", first)
        self.assertEqual(first.count(DT.README_START), 1)

    def test_verification_absent_is_explicit(self):
        report = DT.summarize(inventory([rec(0x100)]), DT.load_verification(None), {})
        self.assertFalse(report["verification"]["available"])
        self.assertIn("full build report unavailable", DT.readme_block(report))

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
        self.assertEqual(ver["by_address"]["ov002:0x00000108"], "PARTIAL")
        self.assertEqual(ver["by_address"]["ov002:0x00000110"], "DIFFERS")
        self.assertEqual((ver["verified"], ver["partial"], ver["differs"]), (1, 1, 1))

    def test_report_normalizes_a_rombuild_romdata_report(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "report.json"
            path.write_text(json.dumps({"romData": {
                "symbols": 2, "verified": 1, "partial": 1, "differs": 0,
                "unnamed": 0, "verifiedBytes": 8, "partialBytes": 3,
                "verifiedSymbols": [{"module": "ov002", "symbol": "data_1",
                                     "addr": 0x100, "bytes": 8}],
                "partialSymbols": [{"module": "ov002", "symbol": "data_2",
                                    "addr": 0x108, "bytes": 3}],
                "differingSymbols": [],
            }}), encoding="utf-8")
            ver = DT.load_verification(path)
        self.assertEqual(ver["verified"], 1)
        self.assertEqual(ver["by_address"]["ov002:0x00000100"], "VERIFIED")
        self.assertEqual(ver["by_address"]["ov002:0x00000108"], "PARTIAL")


if __name__ == "__main__":
    unittest.main()
