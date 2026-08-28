"""summarize() counts ROM data symbols, not compiled object records.

The distinction is the whole reason this file exists. Vague linkage means one
cartridge symbol is emitted by every object that needs it, so a record count rises
and falls with how many FILES the tree is split into -- and TU reconstruction exists
to reduce that number. validate_merge ratchets on `verified`, so a metric that fell
whenever sources were merged would refuse the promotions on principle.
"""
import sys
import pathlib
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import romdata_check as RDC  # noqa: E402


def rec(symbol, verdict, src="src/a.cpp", size=4):
    return {"symbol": symbol, "verdict": verdict, "src": src, "bytes": size}


class SummarizeCountsSymbols(unittest.TestCase):
    def test_duplicate_vague_records_count_once(self):
        s = RDC.summarize([rec("_ZTI7fBase_c", RDC.VERIFIED, "src/a.cpp"),
                           rec("_ZTI7fBase_c", RDC.VERIFIED, "src/b.cpp"),
                           rec("_ZTI7fBase_c", RDC.VERIFIED, "src/c.cpp")])
        self.assertEqual(s["verified"], 1)
        self.assertEqual(s["verifiedBytes"], 4)
        self.assertEqual(s["totalRecords"], 3)
        self.assertEqual(s["verifiedRecords"], 3)

    def test_consolidating_sources_does_not_move_the_ratchet(self):
        """The measured shape of the first TU promotion, in miniature."""
        legacy = [rec("_ZTI7fBase_c", RDC.VERIFIED, f"src/f{i}.cpp") for i in range(8)]
        promoted = [rec("_ZTI7fBase_c", RDC.VERIFIED, "src/actors/TU.cpp")]
        self.assertEqual(RDC.summarize(legacy)["verified"],
                         RDC.summarize(promoted)["verified"])

    def test_best_verdict_wins_across_objects(self):
        s = RDC.summarize([rec("_ZTV4Foo", RDC.PARTIAL), rec("_ZTV4Foo", RDC.VERIFIED)])
        self.assertEqual((s["verified"], s["partial"]), (1, 0))

    def test_a_differing_copy_is_never_hidden_by_a_correct_sibling(self):
        """`differs` is deliberately worst-case, unlike every other bucket."""
        s = RDC.summarize([rec("_ZTV4Foo", RDC.VERIFIED, "src/a.cpp"),
                           rec("_ZTV4Foo", RDC.DIFFERS, "src/b.cpp")])
        self.assertEqual(s["differs"], 1)
        self.assertEqual(s["verified"], 0)
        self.assertEqual(len(s["differing"]), 1)

    def test_distinct_symbols_still_add_up(self):
        s = RDC.summarize([rec("_ZTI4Foo", RDC.VERIFIED), rec("_ZTS4Foo", RDC.PARTIAL),
                           rec("_ZTV4Bar", RDC.UNNAMED)])
        self.assertEqual((s["symbols"], s["verified"], s["partial"], s["unnamed"]),
                         (3, 1, 1, 1))

    def test_same_name_in_two_modules_is_two_symbols(self):
        """The same spelling in two overlays is two cartridge symbols, not one."""
        s = RDC.summarize([dict(rec("_ZTV4Foo", RDC.VERIFIED), module="ov006"),
                           dict(rec("_ZTV4Foo", RDC.DIFFERS), module="ov084")])
        self.assertEqual((s["symbols"], s["verified"], s["differs"]), (2, 1, 1))

    def test_unparsable_objects_never_dedupe_against_each_other(self):
        """check_object's `?` catch-all carries no symbol identity, only a source."""
        s = RDC.summarize([rec("?", RDC.UNNAMED, "src/a.cpp"),
                           rec("?", RDC.UNNAMED, "src/b.cpp")])
        self.assertEqual((s["symbols"], s["unnamed"]), (2, 2))

    def test_empty_input_is_all_zero(self):
        s = RDC.summarize([])
        self.assertEqual((s["symbols"], s["verified"], s["differs"], s["totalRecords"]),
                         (0, 0, 0, 0))


if __name__ == "__main__":
    unittest.main()
