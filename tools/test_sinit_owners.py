import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import sinit_owners as SO  # noqa: E402


class SinitOwnersTests(unittest.TestCase):
    def test_polelift_manifest_is_curated_but_not_overclaimed_as_proven(self):
        evidence = SO.load_manifest_evidence()
        rows = evidence["__sinit_ov045_02112214"]
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0]["owner"], "ov045/PoleLift")
        self.assertEqual(rows[0]["kind"], "curated-out-of-scope")
        self.assertFalse(rows[0]["proven"])

    def test_unique_consumer_is_high_but_conflicting_type_is_ambiguous(self):
        self.assertEqual(
            SO.choose_classification(
                proven_owner="", curated_owner="", consumer_units={"ov001:2"},
                type_units=set(), order_unit="", asset_confidence="",
            ),
            ("high", "ov001:2"),
        )
        self.assertEqual(
            SO.choose_classification(
                proven_owner="", curated_owner="", consumer_units={"ov001:2"},
                type_units={"ov001:3"}, order_unit="", asset_confidence="",
            ),
            ("ambiguous", ""),
        )

    def test_under_segmented_consumer_cluster_is_only_medium(self):
        self.assertEqual(
            SO.choose_classification(
                proven_owner="", curated_owner="", consumer_units={"main:0"},
                type_units=set(), order_unit="", asset_confidence="",
                consumer_candidate_reliable=False,
            ),
            ("medium", "main:0"),
        )

    def test_consumer_and_order_disagreement_is_ambiguous(self):
        self.assertEqual(
            SO.choose_classification(
                proven_owner="", curated_owner="", consumer_units={"ov001:2"},
                type_units=set(), order_unit="ov001:3", asset_confidence="",
            ),
            ("ambiguous", ""),
        )

    def test_order_is_medium_and_never_proven(self):
        self.assertEqual(
            SO.choose_classification(
                proven_owner="", curated_owner="", consumer_units=set(),
                type_units=set(), order_unit="ov001:4", asset_confidence="",
            ),
            ("medium", "ov001:4"),
        )

    def test_source_consumer_scan_ignores_other_sinit_files(self):
        with tempfile.TemporaryDirectory() as tmp:
            repo = pathlib.Path(tmp)
            src = repo / "src"
            src.mkdir()
            (src / "__sinit_test.c").write_text(
                "extern int data_test; void __sinit_test(void) { use(data_test); }\n",
                encoding="utf-8",
            )
            (src / "Owner.c").write_text(
                "extern int data_test; int Owner(void) { return data_test; }\n",
                encoding="utf-8",
            )
            consumers = SO.build_global_consumer_index({"data_test"}, repo)
        self.assertEqual(consumers["data_test"], {"src/Owner.c"})

    def test_empty_initializer_cause_is_explicit(self):
        self.assertEqual(
            SO.initializer_cause(4, [], [], "void init(void) {}"),
            ["empty-stub"],
        )

    def test_ownership_globals_are_directional(self):
        text = (
            "Ctor(&data_object);\n"
            "register_global(data_object);\n"
            "data_copy.p0 = data_constant;\n"
        )
        self.assertEqual(
            SO.infer_ownership_globals(
                text, ["data_object", "data_copy", "data_constant"], ["Ctor"]
            ),
            ["data_copy", "data_object"],
        )


if __name__ == "__main__":
    unittest.main()
