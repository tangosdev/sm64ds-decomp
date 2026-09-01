import csv
import json
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import profile_reconstruction as PR


class ProfileReconstructionUnitTests(unittest.TestCase):
    def test_symbol_preference_preserves_semantic_alias(self):
        names = ["data_ov002_0210947c", "WaterfallMist_SpawnInfo"]
        self.assertEqual(
            PR.best_symbol(names, "profile"), "WaterfallMist_SpawnInfo"
        )

    def test_boundary_confidence_is_evidence_symmetric(self):
        named = {"classes": ["A"]}
        unnamed = {"classes": []}
        self.assertEqual(PR.boundary_confidence(named, named), "high")
        self.assertEqual(PR.boundary_confidence(named, unnamed), "medium")
        self.assertEqual(PR.boundary_confidence(unnamed, named), "medium")
        self.assertEqual(PR.boundary_confidence(unnamed, unnamed), "low")

    def test_required_profiles_are_in_the_sample(self):
        ids = {actor_id for actor_id, _module, _reason in PR.PILOT_SELECTION}
        self.assertEqual(len(ids), 20)
        self.assertTrue({197, 229, 374, 375} <= ids)


class ProfileReconstructionOutputTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.payload = json.loads(PR.OUT_JSON.read_text(encoding="utf-8"))
        cls.rows = {row["profile_id"]: row for row in cls.payload["rows"]}
        cls.population = cls.payload["population_context"]
        cls.full_payload = json.loads(PR.OUT_FULL_JSON.read_text(encoding="utf-8"))
        cls.full_rows = cls.full_payload["rows"]
        with PR.OUT_RENAMES.open(encoding="utf-8", newline="") as fh:
            cls.renames = list(csv.DictReader(fh, delimiter="\t"))

    def test_output_shape_and_required_examples(self):
        self.assertEqual(len(self.rows), 20)
        for profile in ("WATERFALL", "OBJ_MIP_KEY", "MG_CURLING", "MG_CURLING_J"):
            self.assertIn(profile, self.rows)
        self.assertEqual(self.population["distinct_rom_profile_ids"], 391)
        self.assertEqual(self.population["rom_profile_id_collisions"], [])
        self.assertEqual(
            self.population["registry_entries_with_profile_index_match"], 391
        )
        self.assertEqual(
            self.population["registry_entries_without_profile_index_match"], []
        )

    def test_profile_proposals_derive_only_from_rom_id(self):
        for profile_id, row in self.rows.items():
            self.assertEqual(row["proposed_profile_name"], f"g_profile_{profile_id}")

    def test_waterfall_evidence_trace(self):
        row = self.rows["WATERFALL"]
        self.assertEqual(row["class_name"], "daObjWaterfall_c")
        self.assertEqual(row["alloc_size"], "0x000000dc")
        self.assertEqual(row["class_size"], "0x000000dc")
        self.assertEqual(row["installed_vtable"], "0x021094a0")
        self.assertEqual(row["factory_shape"], "new_plus_inlined_ctor")
        self.assertEqual(row["profile_index"], 197)
        self.assertTrue(row["profile_index_matches_actor_id"])
        self.assertEqual(row["execute_order"], 197)
        self.assertEqual(row["draw_order"], 21)
        self.assertEqual(row["group_flags"], "")
        self.assertEqual(row["actor_flags"], "0x00800003")
        self.assertEqual(row["clip_offset_y"], "0x00060000")
        self.assertEqual(row["clip_radius"], "0x00300000")
        self.assertEqual(row["clip_distance"], "0x02000000")
        self.assertEqual(row["far_distance"], "0x00000000")

    def test_scene_and_actor_profile_layouts_stay_distinct(self):
        self.assertEqual(self.rows["BOOT"]["descriptor_layout"], "base_profile_0x08")
        self.assertEqual(self.rows["MG_CURLING"]["descriptor_layout"], "base_profile_0x08")
        self.assertEqual(self.rows["WATERFALL"]["descriptor_layout"], "actor_profile_0x1c")

    def test_base_vtable_exception_does_not_coin_a_class_name(self):
        row = self.rows["STAR_CAMERA"]
        self.assertEqual(row["class_name"], "")
        self.assertEqual(row["factory_shape"], "uncertain")
        self.assertEqual(row["proposed_factory_name"], "")

    def test_true_overlay_multiplexing_is_not_recommended(self):
        row = self.rows["C1_TRAP"]
        self.assertEqual(row["registry_candidate_count"], 2)
        relevant = [r for r in self.renames if r["profile_id"] == "C1_TRAP"]
        self.assertTrue(relevant)
        self.assertTrue(all(r["apply_recommended"] == "no" for r in relevant))
        self.assertTrue(
            all("not_apply=overlay_multiplexed_registry_pointer" in r["evidence"] for r in relevant)
        )

    def test_false_mg_curling_j_overlay_profile_is_rejected(self):
        row = self.rows["MG_CURLING_J"]
        self.assertEqual(row["registry_candidate_count"], 1)
        self.assertEqual(row["overlay"], "ov006")
        self.assertEqual(row["class_name"], "dScMgCurling2_c")

    def test_duplicate_class_factories_are_not_globally_renamed(self):
        for profile in ("KURIBO", "TERESA", "BOSS_TERESA", "KINOKO_CREATE_TAG", "SHOOT_BOOK"):
            self.assertTrue(self.rows[profile]["proposed_factory_collision"])
            rename = next(
                r
                for r in self.renames
                if r["profile_id"] == profile and r["symbol_kind"] == "factory"
            )
            self.assertEqual(rename["apply_recommended"], "no")
            self.assertIn("not_apply=global_name_collision", rename["evidence"])

    def test_full_registry_export_preserves_overlay_candidates(self):
        self.assertEqual(self.full_payload["logical_registry_entries"], 391)
        self.assertEqual(self.full_payload["candidate_rows"], 401)
        self.assertEqual({row["actor_id"] for row in self.full_rows}, set(range(391)))
        ambiguous = {
            row["actor_id"]
            for row in self.full_rows
            if row["overlay_ambiguous"]
        }
        self.assertEqual(len(ambiguous), 10)
        for actor_id in ambiguous:
            rows = [row for row in self.full_rows if row["actor_id"] == actor_id]
            self.assertEqual(len(rows), 2)
            self.assertTrue(all(row["registry_candidate_count"] == 2 for row in rows))

    def test_full_registry_has_an_index_matching_candidate_for_every_id(self):
        matching = {
            row["actor_id"]
            for row in self.full_rows
            if row["profile_index_matches_actor_id"]
        }
        self.assertEqual(matching, set(range(391)))

    def test_first_source_wave_candidates_are_collision_free(self):
        expected = {
            "FM_BATTAN": "daObjFm_Battan_c",
            "KM3_KURUMAJIKU": "daObjKm3_Kurumajiku_c",
            "EYEKUN_BEAM": "daEyBm_c",
            "BIRIKYU": "daBrq_c",
            "BAR": "daBar_c",
        }
        by_profile = {
            row["profile_id"]: row
            for row in self.full_rows
            if row["profile_id"] in expected
        }
        self.assertEqual(set(by_profile), set(expected))
        for profile_id, class_name in expected.items():
            row = by_profile[profile_id]
            self.assertEqual(row["class_name"], class_name)
            self.assertEqual(row["registry_candidate_count"], 1)
            self.assertEqual(row["proposed_factory_collision"], "")
            self.assertEqual(row["factory_rename_recommended"], "yes")
            self.assertEqual(row["profile_rename_recommended"], "yes")


if __name__ == "__main__":
    unittest.main()
