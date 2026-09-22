import collections
import csv
import json
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import profile_reconstruction as PR
import tu_names as TN


class TranslationUnitFilenameTests(unittest.TestCase):
    def test_nsmbw_lineage_prefixes(self):
        self.assertEqual(TN.candidate_stem("dScBoot_c"), "d_s_boot")
        self.assertEqual(TN.candidate_stem("dScMgCurling2_c"), "d_s_mg_curling2")
        self.assertEqual(TN.candidate_stem("daObjWaterfall_c"), "d_a_obj_waterfall")
        self.assertEqual(TN.candidate_stem("daObj_Mip_Key_c"), "d_a_obj_mip_key")


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
        cls.full_by_profile = {
            row["profile_id"]: row
            for row in cls.full_rows
            if not row["overlay_resolution"].startswith("superseded_by_")
        }
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

    def test_true_overlay_multiplexing_is_resolved_without_erasing_it(self):
        """The multiplex is resolved by name correspondence, never hidden.

        The measured ambiguity must survive in the output: a reader has to be
        able to see that two overlays decode this pointer, and that the winner
        was chosen by a documented join rather than by the pointer alone.
        """
        row = self.rows["C1_TRAP"]
        self.assertEqual(row["registry_candidate_count"], 2)
        self.assertEqual(
            row["overlay_resolution"], "resolved_by_rtti_name_correspondence"
        )
        self.assertEqual(row["overlay"], "ov010")
        # A resolved multiplex stays B, not B+: the endpoints are ROM-proven but
        # the join between them is not a byte proof.
        self.assertEqual(row["profile_name_confidence"], "B")

        relevant = [r for r in self.renames if r["profile_id"] == "C1_TRAP"]
        self.assertTrue(relevant)
        profile = [r for r in relevant if r["symbol_kind"] == "profile"]
        self.assertTrue(profile)
        for r in profile:
            self.assertEqual(r["apply_recommended"], "yes")
            self.assertIn("overlay_multiplex_rtti_name_correspondence", r["evidence"])

    def test_shared_class_factories_are_disambiguated_by_profile_id(self):
        """N factories on one class get N names, and the collision stays on record."""
        seesaws = [
            row
            for row in self.full_rows
            if row["class_name"] == "daObjSeesaw_c"
            and not row["overlay_resolution"].startswith("superseded_by_")
        ]
        self.assertGreater(len(seesaws), 1)
        for row in seesaws:
            self.assertEqual(
                row["factory_name_resolution"], "disambiguated_by_profile_id"
            )
            self.assertEqual(
                row["proposed_factory_name"],
                f"daObjSeesaw_c_classInit_{row['profile_id']}",
            )
            # Resolving the collision must not erase the measurement of it.
            self.assertTrue(row["proposed_factory_collision"])
            self.assertEqual(row["factory_rename_recommended"], "yes")

    def test_proposed_factory_names_are_globally_unique(self):
        """The whole point of the suffix: one function, one name, tree-wide."""
        names = [
            row["proposed_factory_name"]
            for row in self.full_rows
            if row["factory_rename_recommended"] == "yes"
        ]
        self.assertTrue(names)
        duplicates = {n for n in names if names.count(n) > 1}
        self.assertEqual(duplicates, set())

    def test_unique_class_factory_keeps_the_bare_spelling(self):
        """A class reached by one registry entry is not given a pointless suffix."""
        bare = [
            row
            for row in self.full_rows
            if row["factory_name_resolution"] == "unique_class_factory"
        ]
        self.assertTrue(bare)
        for row in bare:
            self.assertEqual(
                row["proposed_factory_name"], f"{row['class_name']}_classInit"
            )
            self.assertFalse(row["proposed_factory_collision"])

    def test_superseded_row_is_never_disambiguated(self):
        """A phantom candidate must not claim a name the live row owns."""
        for row in self.full_rows:
            if row["overlay_resolution"].startswith("superseded_by_"):
                self.assertEqual(
                    row["factory_name_resolution"], "superseded_candidate"
                )
                self.assertNotEqual(row["factory_rename_recommended"], "yes")

    def test_disambiguation_refuses_a_non_bijective_suffix(self):
        """Two profile ids on one factory would give one function two names."""
        proposals = {"daX_c_classInit": {("ov001", 1), ("ov001", 2)}}
        addr_profiles = {"daX_c_classInit": {("ov001", 1): {"A", "B"}}}
        with self.assertRaises(SystemExit):
            PR.resolve_factory_name(
                "daX_c", "A", ("ov001", 1), proposals, addr_profiles
            )

    def test_multiplex_losing_candidate_is_marked_superseded(self):
        """Only one overlay may win a shared pointer, and the loser says so."""
        for actor_id in {row["actor_id"] for row in self.full_rows if row["overlay_ambiguous"]}:
            rows = [row for row in self.full_rows if row["actor_id"] == actor_id]
            self.assertEqual(len(rows), 2)
            resolutions = sorted(row["overlay_resolution"] for row in rows)
            winners = [
                row
                for row in rows
                if row["overlay_resolution"] == "resolved_by_rtti_name_correspondence"
            ]
            losers = [
                row
                for row in rows
                if row["overlay_resolution"].startswith("superseded_by_")
            ]
            self.assertEqual(len(winners), 1, f"actor {actor_id}: {resolutions}")
            self.assertEqual(len(losers), 1, f"actor {actor_id}: {resolutions}")
            # The loser names the overlay that beat it, and is never recommended.
            self.assertEqual(
                losers[0]["overlay_resolution"],
                f"superseded_by_{winners[0]['overlay']}",
            )
            self.assertNotEqual(losers[0]["profile_rename_recommended"], "yes")
            self.assertNotEqual(losers[0]["factory_rename_recommended"], "yes")

    def test_multiplex_resolution_only_covers_measured_ambiguity(self):
        """The table may not invent a resolution for an unambiguous actor."""
        import profile_reconstruction as PR

        ambiguous = {
            row["actor_id"] for row in self.full_rows if row["overlay_ambiguous"]
        }
        self.assertEqual(set(PR.MULTIPLEXED_RESOLUTION), ambiguous)
        for row in self.full_rows:
            if not row["overlay_ambiguous"]:
                self.assertEqual(
                    row["overlay_resolution"], "unique_registry_context"
                )

    def test_false_mg_curling_j_overlay_profile_is_rejected(self):
        row = self.rows["MG_CURLING_J"]
        self.assertEqual(row["registry_candidate_count"], 1)
        self.assertEqual(row["overlay"], "ov006")
        self.assertEqual(row["class_name"], "dScMgCurling2_c")
        self.assertEqual(row["class_filename_candidate"], "d_s_mg_curling2.cpp")

    def test_duplicate_class_factories_are_renamed_per_profile(self):
        """These five share a class with other entries and used to be unnameable.

        They are the reason the suffix exists: the bare <Class>_classInit would
        have collided on the symbol AND on the source filename.
        """
        for profile in ("KURIBO", "TERESA", "BOSS_TERESA", "KINOKO_CREATE_TAG", "SHOOT_BOOK"):
            row = self.rows[profile]
            self.assertTrue(row["proposed_factory_collision"])
            self.assertEqual(
                row["factory_name_resolution"], "disambiguated_by_profile_id"
            )
            self.assertTrue(row["proposed_factory_name"].endswith(f"_{profile}"))
            rename = next(
                r
                for r in self.renames
                if r["profile_id"] == profile and r["symbol_kind"] == "factory"
            )
            self.assertEqual(rename["apply_recommended"], "yes")
            self.assertIn(
                "classinit_disambiguated_by_rom_profile_id", rename["evidence"]
            )

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

    def test_factory_filename_is_unique_across_the_registry(self):
        """The target filename is the point of the column; a clash defeats it."""
        names = [
            row["factory_filename"]
            for row in self.full_rows
            if row["factory_filename"]
        ]
        self.assertTrue(names)
        duplicates = sorted(
            name for name, n in collections.Counter(names).items() if n > 1
        )
        self.assertEqual(duplicates, [])
        self.assertEqual(
            len(names),
            sum(
                1
                for row in self.full_rows
                if row["factory_rename_recommended"] == "yes"
            ),
        )

    def test_factory_filename_is_empty_without_a_recommended_rename(self):
        for row in self.full_rows:
            if row["factory_rename_recommended"] != "yes":
                self.assertEqual(row["factory_filename"], "")

    def test_factory_filename_keeps_the_current_extension(self):
        """A .c -> .cpp rename changes the language mode and costs the match."""
        for row in self.full_rows:
            if not row["factory_filename"]:
                continue
            self.assertEqual(
                pathlib.PurePosixPath(row["factory_filename"]).suffix,
                pathlib.PurePosixPath(row["current_factory_file"]).suffix,
            )

    def test_factory_filename_uses_the_class_stem_when_the_class_is_unshared(self):
        row = self.full_by_profile["FM_BATTAN"]
        self.assertEqual(row["class_filename_candidate"], "d_a_obj_fm_battan.cpp")
        self.assertEqual(row["factory_filename"], "d_a_obj_fm_battan.cpp")
        # BOOT's factory still lives in a .c, so its target keeps the .c: the
        # stem is reconstructed, the language mode is not up for reconstruction.
        boot = self.full_by_profile["BOOT"]
        self.assertEqual(boot["class_filename_candidate"], "d_s_boot.cpp")
        self.assertTrue(boot["current_factory_file"].endswith(".c"))
        self.assertEqual(boot["factory_filename"], "d_s_boot.c")

    def test_shared_class_filenames_are_disambiguated_by_profile_id(self):
        """daTrsTrap_c has four factories; one stem cannot name four files."""
        shared = {
            row["profile_id"]: row["factory_filename"]
            for row in self.full_rows
            if row["class_name"] == "daTrsTrap_c" and row["factory_filename"]
        }
        self.assertEqual(
            shared,
            {
                "TERESAPIT": "d_a_trs_trap_teresapit.c",
                "KAIDAN": "d_a_trs_trap_kaidan.c",
                "BOOKSHELF": "d_a_trs_trap_bookshelf.c",
                "MERRYGOROUND": "d_a_trs_trap_merrygoround.c",
            },
        )
        for row in self.full_rows:
            if row["class_name"] == "daTrsTrap_c":
                self.assertEqual(row["class_filename_candidate"], "d_a_trs_trap.cpp")


if __name__ == "__main__":
    unittest.main()
