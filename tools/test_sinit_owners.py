import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import sinit_owners as SO  # noqa: E402


class SinitOwnersTests(unittest.TestCase):
    def test_curated_init_is_not_overclaimed_as_proven(self):
        # A synthetic manifest, not a live one: the ov045 PoleLift manifest this
        # test used to read has since folded its initializer (#3661, landed in
        # #3703), the promotion that turns a curated row into a licensed one.
        # Even under a proof status, a curated row stays unproven.
        with tempfile.TemporaryDirectory() as tmp:
            repo = pathlib.Path(tmp)
            mdir = repo / "config" / "tu_manifest.d" / "ov045"
            mdir.mkdir(parents=True)
            (mdir / "daObjKm2_Ami_Bou_c.json").write_text(
                '{"id": "ov045/daObjKm2_Ami_Bou_c", "status": "promoted",'
                ' "functions": [{"symbol": "__sinit_d_a_obj_km2_ami_bou.cpp"}],'
                ' "tu_owned_but_out_of_scope": {"init": [{"symbol": "__sinit_ov045_02112214",'
                ' "evidence": "touches this TU\'s two .bss SharedFilePtr statics"}]}}',
                encoding="utf-8")
            evidence = SO.load_manifest_evidence(repo)
        rows = evidence["__sinit_ov045_02112214"]
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0]["owner"], "ov045/daObjKm2_Ami_Bou_c")
        self.assertEqual(rows[0]["kind"], "curated-out-of-scope")
        self.assertFalse(rows[0]["proven"])
        licensed = evidence["__sinit_d_a_obj_km2_ami_bou.cpp"]
        self.assertEqual([r["kind"] for r in licensed], ["licensed-function"])
        self.assertTrue(licensed[0]["proven"])

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

    def test_handle_class_definitions_are_owned_resource_handles(self):
        # The initializer of a TU that defines real-class handles constructs
        # each one; the old free-function veneer spelling still counts.
        text = (
            "TexSeqFile data_tex(0x8021);\n"
            "AnimFile data_anim(ASSET_SOME_BCA);\n"
            "func_02017acc(&data_model, 0x40a);\n"
            "extern ModelFile data_elsewhere;\n"
        )
        known = ["data_tex", "data_anim", "data_model", "data_elsewhere"]
        self.assertEqual(
            SO.infer_ownership_globals(text, known, []),
            ["data_anim", "data_model", "data_tex"],
        )
        self.assertEqual(SO.asset_handles(text), [0x40A, 0x8021])
        self.assertIn(
            "resource-handle-initialization",
            SO.initializer_cause(0x40, [], known, "ModelFile data_m(0x25c);"),
        )

    def test_an_assignment_does_not_run_across_lines(self):
        # A comment ending in `.` used to swallow the next definition, and a
        # read such as `x[i]) {` used to claim a later line's `=`.
        text = (
            "/* The table the actor framework reads from data_read[PLAYER]. */\n"
            "Profile data_profile = {0};\n"
            "if (mId == data_lookup[i]) {\n"
            "    local = 1;\n"
            "}\n"
        )
        known = ["data_read", "data_profile", "data_lookup"]
        self.assertEqual(SO.infer_ownership_globals(text, known, []), ["data_profile"])

    def test_definitions_with_constructor_arguments_are_owned(self):
        # A coined handle wrapper is constructed by the initializer just as a
        # real handle class is; a prototype is never a known global.
        text = (
            "BmbAnimFile gBmbWalkAnimFile(ASSET_BOMBHEI_WALK_BCA);\n"
            "static Fix<int> sScale(0x1000);\n"
            "void LoadThings(int id);\n"
            "extern BmbAnimFile gBmbOtherAnimFile;\n"
        )
        known = ["gBmbWalkAnimFile", "sScale", "gBmbOtherAnimFile"]
        self.assertEqual(
            SO.infer_ownership_globals(text, known, []),
            ["gBmbWalkAnimFile", "sScale"],
        )

    def test_consumer_coverage_does_not_count_unconsumed_globals_as_mapped(self):
        mapped, unconsumed = SO.consumer_coverage(
            {
                "data_mapped": ["src/Owner.cpp"],
                "data_unmapped": ["src/Unknown.cpp"],
                "data_unused": [],
            },
            {
                "data_mapped": ["ov001:2"],
                "data_unmapped": [],
                "data_unused": [],
            },
        )
        self.assertEqual(mapped, ["data_mapped"])
        self.assertEqual(unconsumed, ["data_unused"])
        ownership_globals = ["data_mapped", "data_unused"]
        self.assertEqual(
            SO.coverage_status(ownership_globals, ["data_mapped"]), "partial"
        )
        self.assertEqual(
            SO.coverage_status(ownership_globals, ownership_globals), "full"
        )
        self.assertEqual(SO.coverage_status(ownership_globals, []), "none")


if __name__ == "__main__":
    unittest.main()
