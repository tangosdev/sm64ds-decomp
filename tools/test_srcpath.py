"""What srcpath has to guarantee, especially the subdirectory case that does not exist
in the tree yet -- that is the whole point of the module, so it is the part most worth
pinning before anything moves."""
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import srcpath as SP  # noqa: E402


class SrcPath(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        repo = pathlib.Path(self.tmp.name)
        (repo / "src").mkdir()
        self._saved = (SP.REPO, SP.SRC)
        SP.REPO, SP.SRC = repo, repo / "src"
        SP.invalidate()

    def tearDown(self):
        SP.REPO, SP.SRC = self._saved
        SP.invalidate()
        self.tmp.cleanup()

    def write(self, rel, text="int f(void) { return 0; }\n"):
        p = SP.SRC / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text)
        SP.invalidate()
        return p

    # --- today's flat tree -------------------------------------------------
    def test_flat_c(self):
        p = self.write("func_0205a61c.c")
        self.assertEqual(SP.path_for("func_0205a61c"), p)

    def test_flat_cpp(self):
        p = self.write("_ZN6Player4InitEv.cpp")
        self.assertEqual(SP.path_for("_ZN6Player4InitEv"), p)

    def test_missing_is_none(self):
        self.assertIsNone(SP.path_for("nope"))

    def test_c_wins_over_cpp(self):
        c = self.write("both.c")
        self.write("both.cpp")
        self.assertEqual(SP.path_for("both"), c)
        self.assertEqual([p.suffix for p in SP.paths_for("both")], [".c", ".cpp"])

    # --- the layout this exists for ----------------------------------------
    def test_finds_symbol_in_subdirectory(self):
        p = self.write("actors/ov063_boo/Boo_Behavior.c")
        self.assertEqual(SP.path_for("Boo_Behavior"), p)

    def test_subdirectory_survives_a_deep_nest(self):
        p = self.write("arm9/nitro/os/OS_SleepThread.c")
        self.assertEqual(SP.path_for("OS_SleepThread"), p)

    def test_direct_hit_wins_over_scan(self):
        """A flat file shadows a nested one of the same name -- no scan, no ambiguity."""
        flat = self.write("dup.c")
        self.write("sub/dup.c")
        self.assertEqual(SP.path_for("dup"), flat)

    # --- writers ------------------------------------------------------------
    def test_new_path_defaults_to_src_root(self):
        self.assertEqual(SP.new_path_for("brand_new", "c"), SP.SRC / "brand_new.c")
        self.assertEqual(SP.new_path_for("brand_new", ".cpp"), SP.SRC / "brand_new.cpp")

    def test_new_path_keeps_an_existing_file_where_it_lives(self):
        self.write("actors/ov063_boo/Boo_Render.c")
        self.assertEqual(SP.new_path_for("Boo_Render", "cpp"),
                         SP.SRC / "actors/ov063_boo/Boo_Render.cpp")

    def test_new_path_rejects_a_non_source_extension(self):
        with self.assertRaises(ValueError):
            SP.new_path_for("x", "h")

    # --- symbol parsing -----------------------------------------------------
    def test_module_of_reads_the_symbol_not_the_config(self):
        self.assertEqual(SP.module_of("func_ov063_021160c4"), "ov063")
        self.assertEqual(SP.module_of("__sinit_ov002_02100560"), "ov002")
        self.assertEqual(SP.module_of("func_0205c410"), "arm9")
        self.assertEqual(SP.module_of("__sinit_02073a24"), "arm9")
        self.assertIsNone(SP.module_of("_ZN3Boo6RenderEv"))
        self.assertIsNone(SP.module_of("AngleDiff"))

    def test_class_of_takes_the_outer_component(self):
        self.assertEqual(SP.class_of("_ZN3Boo6RenderEv"), "Boo")
        self.assertEqual(SP.class_of("_ZN15FaderBrightness8SetToEndEv"), "FaderBrightness")
        self.assertEqual(SP.class_of("Boo_Spawn"), "Boo")
        # const method
        self.assertEqual(SP.class_of("_ZNK5Actor7GetPosEv"), "Actor")

    def test_class_of_rejects_a_nested_namespaces_inner_name(self):
        """Sound::Player must not be filed under the unrelated actor Player."""
        self.assertEqual(SP.class_of("_ZN5Sound6Player19SetPlayableSeqCountEii"), "Sound")

    def test_class_of_rejects_free_functions_and_special_symbols(self):
        """_Z<len><name> is a free function -- treating its name as a class invents a
        directory per function (29 of them in the tree)."""
        self.assertIsNone(SP.class_of("_Z14ApproachLinearRiii"))
        self.assertIsNone(SP.class_of("_ZTV5Model"))
        self.assertIsNone(SP.class_of("_ZTI5Model"))
        self.assertIsNone(SP.class_of("_ZThn80_N9Animation7AdvanceEv"))
        self.assertIsNone(SP.class_of("func_0205c410"))

    # --- placement: follows migration, never leads it -----------------------
    def test_unnamed_goes_to_its_module_bucket_once_that_exists(self):
        self.write("unnamed/ov063/func_ov063_021160c4.c")
        self.assertEqual(SP.new_path_for("func_ov063_02116190", "c"),
                         SP.SRC / "unnamed/ov063/func_ov063_02116190.c")

    def test_unnamed_stays_flat_until_its_module_is_migrated(self):
        self.write("unnamed/ov063/func_ov063_021160c4.c")
        self.assertEqual(SP.new_path_for("func_0205c410", "c"), SP.SRC / "func_0205c410.c")

    def test_unnamed_ignores_a_hand_placed_subsystem_directory(self):
        """src/engine/message/ holds two arm9 func_* by deliberate choice. That must not
        drag every future arm9 function in after them."""
        self.write("engine/message/func_0201cb2c.c")
        self.assertEqual(SP.new_path_for("func_02012345", "c"), SP.SRC / "func_02012345.c")

    def test_a_class_follows_its_siblings(self):
        self.write("actors/Boo/Boo_Spawn.cpp")
        self.write("actors/Boo/_ZN3BooD1Ev.c")
        self.assertEqual(SP.new_path_for("_ZN3Boo6RenderEv", "cpp"),
                         SP.SRC / "actors/Boo/_ZN3Boo6RenderEv.cpp")

    def test_a_spawn_follows_its_class(self):
        self.write("actors/MadPiano/_ZN8MadPianoD1Ev.c")
        self.assertEqual(SP.new_path_for("MadPiano_Spawn", "c"),
                         SP.SRC / "actors/MadPiano/MadPiano_Spawn.c")

    def test_flat_siblings_do_not_outvote_a_subdirectory(self):
        """A partly-migrated class keeps going to the subdirectory it has; the root is the
        unmigrated default, not a rival choice."""
        self.write("_ZN3BooD0Ev.c")
        self.write("_ZN3BooD1Ev.c")
        self.write("actors/Boo/Boo_Spawn.cpp")
        self.assertEqual(SP.new_path_for("_ZN3Boo6RenderEv", "cpp"),
                         SP.SRC / "actors/Boo/_ZN3Boo6RenderEv.cpp")

    def test_two_subdirectories_are_left_alone(self):
        self.write("actors/Boo/Boo_Spawn.cpp")
        self.write("engine/ghosts/_ZN3BooD1Ev.c")
        self.assertEqual(SP.new_path_for("_ZN3Boo6RenderEv", "cpp"),
                         SP.SRC / "_ZN3Boo6RenderEv.cpp")

    def test_an_unknown_class_stays_flat(self):
        self.write("actors/Boo/Boo_Spawn.cpp")
        self.assertEqual(SP.new_path_for("_ZN6Player4InitEv", "cpp"),
                         SP.SRC / "_ZN6Player4InitEv.cpp")

    def test_free_functions_and_plain_names_stay_flat(self):
        self.write("actors/Boo/Boo_Spawn.cpp")
        self.assertEqual(SP.new_path_for("_Z14ApproachLinearRiii", "c"),
                         SP.SRC / "_Z14ApproachLinearRiii.c")
        self.assertEqual(SP.new_path_for("AngleDiff", "c"), SP.SRC / "AngleDiff.c")

    # --- renaming: the commonest way a file's correct home changes -----------
    def test_rename_in_place_is_still_the_norm(self):
        p = self.write("func_0205c410.c")
        self.assertEqual(SP.rename_target(p, "_ZN6Player4InitEv"),
                         SP.SRC / "_ZN6Player4InitEv.c")

    def test_rename_out_of_an_unnamed_bucket_when_the_name_gains_a_class(self):
        """The defect this exists for: a named file left in src/unnamed/ov006/ misfiles
        itself AND splits its class across two directories, which disables placement for
        every other method of that class."""
        p = self.write("unnamed/ov006/func_ov006_02100000.c")
        self.assertEqual(SP.rename_target(p, "_ZN3Boo6RenderEv"),
                         SP.SRC / "_ZN3Boo6RenderEv.c")

    def test_rename_into_the_classs_existing_home(self):
        self.write("actors/Boo/Boo_Spawn.cpp")
        p = self.write("unnamed/ov006/func_ov006_02100000.c")
        self.assertEqual(SP.rename_target(p, "_ZN3Boo6RenderEv"),
                         SP.SRC / "actors/Boo/_ZN3Boo6RenderEv.c")

    def test_rename_stays_in_its_bucket_when_still_address_named(self):
        p = self.write("unnamed/ov006/func_ov006_02100000.c")
        self.assertEqual(SP.rename_target(p, "func_ov006_02100004"),
                         SP.SRC / "unnamed/ov006/func_ov006_02100004.c")

    def test_rename_leaves_a_bucket_that_belongs_to_another_module(self):
        p = self.write("unnamed/ov006/func_ov006_02100000.c")
        self.assertEqual(SP.rename_target(p, "func_ov002_02100000"),
                         SP.SRC / "func_ov002_02100000.c")

    def test_rename_does_not_disturb_a_hand_placed_subsystem_directory(self):
        """engine/message/ is a human's choice; srcpath has no standing to second-guess
        it, so a rename there stays put."""
        p = self.write("engine/message/func_0201cb2c.c")
        self.assertEqual(SP.rename_target(p, "func_0201cb30"),
                         SP.SRC / "engine/message/func_0201cb30.c")

    def test_rename_keeps_a_multi_suffix_extension(self):
        p = self.write("unnamed/ov006/func_ov006_02100000.cpp")
        self.assertEqual(SP.rename_target(p, "_ZN3Boo6RenderEv").suffix, ".cpp")

    def test_placement_never_overrides_where_a_file_already_is(self):
        """Migration decides; placement only follows. A file that exists is not relocated
        even when its cohort has since moved elsewhere."""
        self.write("actors/Boo/Boo_Spawn.cpp")
        self.write("_ZN3Boo6RenderEv.c")
        self.assertEqual(SP.new_path_for("_ZN3Boo6RenderEv", "cpp"),
                         SP.SRC / "_ZN3Boo6RenderEv.cpp")

    # --- bulk ---------------------------------------------------------------
    def test_index_and_iter_cover_nested_files(self):
        self.write("flat.c")
        self.write("courses/ov022_lethal_lava_land/FallBlockLll.cpp")
        self.assertEqual(
            SP.build_index(),
            {"flat": "src/flat.c",
             "FallBlockLll": "src/courses/ov022_lethal_lava_land/FallBlockLll.cpp"})
        self.assertEqual(len(list(SP.iter_sources())), 2)

    def test_index_agrees_with_path_for_on_a_dual_extension_symbol(self):
        """Directory order must not decide; .c wins in both, or the index lies."""
        self.write("both.cpp")
        self.write("both.c")
        self.assertEqual(SP.build_index()["both"], "src/both.c")
        self.assertEqual(SP.path_for("both"), SP.SRC / "both.c")

    def test_non_source_files_are_ignored(self):
        self.write("notes.txt", "x")
        self.write("header.h", "x")
        self.assertEqual(list(SP.iter_sources()), [])
        self.assertIsNone(SP.path_for("header"))


if __name__ == "__main__":
    unittest.main()
