import contextlib
import io
import json
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tiers_ratchet as TR  # noqa: E402


class TranslationUnitIdentities(unittest.TestCase):
    def test_multi_function_source_is_banked_per_member(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            source = root / "src/actors/TU.cpp"
            source.parent.mkdir(parents=True)
            source.write_text(
                "//cpp\nint shared_helper;\nvoid First() {}\nvoid Second() {}\n",
                encoding="utf-8")
            ownership = {"src/actors/TU.cpp": ["First", "Second"]}
            with mock.patch.object(TR, "REPO", root):
                converted, scores = TR.scan(["src/actors/TU.cpp"], ownership)

            self.assertEqual(converted, {
                "src/actors/TU.cpp#First", "src/actors/TU.cpp#Second"})
            self.assertEqual(set(scores), converted)

    def test_multi_function_members_do_not_contaminate_each_other(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            source = root / "src/actors/TU.cpp"
            source.parent.mkdir(parents=True)
            source.write_text(
                "//cpp\n"
                "int unk_18;\n"
                "extern int _ZN3Bad3UseEv();\n"
                "// @symbol First\n"
                "void First() {}\n"
                "// @symbol Second\n"
                "int Second() { return unk_18 + _ZN3Bad3UseEv(); }\n",
                encoding="utf-8")
            ownership = {"src/actors/TU.cpp": ["First", "Second"]}
            with mock.patch.object(TR, "REPO", root):
                converted, scores = TR.scan(["src/actors/TU.cpp"], ownership)

            self.assertEqual(converted, {"src/actors/TU.cpp#First"})
            self.assertTrue(scores["src/actors/TU.cpp#First"]["no_unk_field"])
            self.assertTrue(scores["src/actors/TU.cpp#First"]["no_mangled_refs"])
            self.assertFalse(scores["src/actors/TU.cpp#Second"]["no_unk_field"])
            self.assertFalse(scores["src/actors/TU.cpp#Second"]["no_mangled_refs"])

    def test_inline_lifecycle_member_uses_its_header_definition(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            source = root / "src/actors/Thing.cpp"
            header = root / "include/Thing.h"
            source.parent.mkdir(parents=True)
            header.parent.mkdir(parents=True)
            header.write_text(
                "struct Thing { virtual ~Thing() {} };\n", encoding="utf-8")
            source.write_text(
                "//cpp\n"
                "#include \"Thing.h\"\n"
                "int unk_18;\n"
                "extern int _ZN3Bad3UseEv();\n"
                "// @symbol Other\n"
                "int Other() { return unk_18 + _ZN3Bad3UseEv(); }\n",
                encoding="utf-8")
            ownership = {
                "src/actors/Thing.cpp": ["_ZN5ThingD1Ev", "Other"]}
            with mock.patch.object(TR, "REPO", root):
                converted, scores = TR.scan(["src/actors/Thing.cpp"], ownership)

            identity = "src/actors/Thing.cpp#_ZN5ThingD1Ev"
            self.assertIn(identity, converted)
            self.assertTrue(scores[identity]["no_unk_field"])
            self.assertTrue(scores[identity]["no_mangled_refs"])

    def test_lifecycle_declaration_is_not_treated_as_its_definition(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            source = root / "src/actors/Thing.cpp"
            header = root / "include/Thing.h"
            source.parent.mkdir(parents=True)
            header.parent.mkdir(parents=True)
            header.write_text(
                "struct Thing { virtual ~Thing(); };\n", encoding="utf-8")
            source.write_text(
                "//cpp\n#include \"Thing.h\"\nint unk_18;\n",
                encoding="utf-8")
            ownership = {
                "src/actors/Thing.cpp": ["_ZN5ThingD1Ev", "Other"]}
            with mock.patch.object(TR, "REPO", root):
                converted, scores = TR.scan(["src/actors/Thing.cpp"], ownership)

            identity = "src/actors/Thing.cpp#_ZN5ThingD1Ev"
            self.assertNotIn(identity, converted)
            self.assertFalse(scores[identity]["no_unk_field"])

    def test_single_function_source_keeps_legacy_path_identity(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            source = root / "src/Only.cpp"
            source.parent.mkdir(parents=True)
            source.write_text("//cpp\nvoid Only() {}\n", encoding="utf-8")
            ownership = {"src/Only.cpp": ["Only"]}
            with mock.patch.object(TR, "REPO", root):
                converted, scores = TR.scan(["src/Only.cpp"], ownership)

            self.assertEqual(converted, {"src/Only.cpp"})
            self.assertEqual(set(scores), converted)

    def test_missing_promoted_member_is_named_not_reported_as_unreadable(self):
        why = TR.why("src/actors/TU.cpp#Missing", {}, {"src/actors/TU.cpp"})
        self.assertIn("no longer an enrolled member", why)

    def test_legacy_multi_function_path_upgrades_only_when_every_member_passes(self):
        rel = "src/actors/TU.cpp"
        ownership = {rel: ["First", "Second"]}
        current = {f"{rel}#First", f"{rel}#Second"}
        upgraded, backslid = TR.classify_missing(
            [rel], current, {rel}, {}, ownership)
        self.assertEqual((upgraded, backslid), ([rel], []))

        upgraded, backslid = TR.classify_missing(
            [rel], {f"{rel}#First"}, {rel}, {}, ownership)
        self.assertEqual((upgraded, backslid), ([], [rel]))

    def test_legacy_identity_upgrade_names_every_passing_member(self):
        rel = "src/actors/TU.cpp"
        ownership = {rel: ["First", "Second"]}
        clean = dict.fromkeys(TR.tiers.CRITERIA, True)
        scores = {f"{rel}#First": clean, f"{rel}#Second": clean}

        reason = TR.why(rel, scores, {rel}, ownership=ownership)

        self.assertIn("IDENTITY UPGRADE", reason)
        self.assertIn("2 independently passing member identities", reason)
        self.assertIn(f"{rel}#First", reason)
        self.assertIn(f"{rel}#Second", reason)

    def test_legacy_identity_upgrade_names_the_regressed_member(self):
        rel = "src/actors/TU.cpp"
        ownership = {rel: ["First", "Second"]}
        clean = dict.fromkeys(TR.tiers.CRITERIA, True)
        dirty = dict(clean, no_mangled_refs=False)
        scores = {f"{rel}#First": clean, f"{rel}#Second": dirty}

        reason = TR.why(rel, scores, {rel}, ownership=ownership)

        self.assertIn("IDENTITY UPGRADE INCOMPLETE", reason)
        self.assertIn(f"{rel}#Second", reason)
        self.assertIn(TR.tiers.CRITERION_LABEL["no_mangled_refs"], reason)
        self.assertNotIn("UNREADABLE", reason)

    def test_promoted_move_accepts_all_destination_member_identities(self):
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        moves = {legacy: ("ov001/TU", dest)}
        ownership = {dest: ["First", "Second"]}
        current = {f"{dest}#First", f"{dest}#Second"}

        moved, backslid = TR.classify_missing(
            [legacy], current, {dest}, moves, ownership)

        self.assertEqual((moved, backslid), ([legacy], []))

    def test_promoted_move_rejects_one_regressed_destination_member(self):
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        moves = {legacy: ("ov001/TU", dest)}
        ownership = {dest: ["First", "Second"]}

        moved, backslid = TR.classify_missing(
            [legacy], {f"{dest}#First"}, {dest}, moves, ownership)

        self.assertEqual((moved, backslid), ([], [legacy]))

    def test_promoted_move_names_the_regressed_destination_member(self):
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        moves = {legacy: ("ov001/TU", dest)}
        ownership = {dest: ["First", "Second"]}
        clean = dict.fromkeys(TR.tiers.CRITERIA, True)
        dirty = dict(clean, no_raw_offset=False)
        scores = {f"{dest}#First": clean, f"{dest}#Second": dirty}

        reason = TR.why(legacy, scores, {dest}, moves, ownership)

        self.assertIn("MOVED", reason)
        self.assertIn(f"{dest}#Second", reason)
        self.assertIn(TR.tiers.CRITERION_LABEL["no_raw_offset"], reason)


class OrphanedIdentities(unittest.TestCase):
    """A banked identity naming a file the tree no longer has is watching nothing.

    It cannot fail a criterion, because an untracked file is never scored, so the only
    way to notice it is to ask the question directly.
    """

    def test_a_banked_path_whose_file_is_gone_is_orphaned(self):
        banked = {"src/Kept.cpp", "src/Gone.cpp"}

        self.assertEqual(TR.orphaned_identities(banked, {"src/Kept.cpp"}),
                         ["src/Gone.cpp"])

    def test_a_member_identity_whose_file_exists_is_not_orphaned(self):
        """The split on `#` has to happen first.

        Testing the whole identity against the tracked set reports every promoted-TU
        member as an orphan -- 651 of 2701 banked identities when this was written.
        """
        rel = "src/actors/TU.cpp"
        banked = {f"{rel}#First", f"{rel}#Second"}

        self.assertEqual(TR.orphaned_identities(banked, {rel}), [])

    def test_a_member_identity_whose_file_is_gone_is_orphaned(self):
        rel = "src/actors/TU.cpp"
        banked = {f"{rel}#First", f"{rel}#Second"}

        self.assertEqual(TR.orphaned_identities(banked, {"src/Other.cpp"}),
                         [f"{rel}#First", f"{rel}#Second"])

    def test_a_bare_path_scored_per_member_is_not_orphaned(self):
        """`absorbed_clean` branch 1 is NOT an orphan and must not be reported as one.

        A multi-function source banked by its physical path before the per-member
        scorer existed appears in `banked - current`, because `scan()` now emits
        `path#symbol` for it. The file still exists; nothing is dangling.
        """
        rel = "src/actors/TU.cpp"
        ownership = {rel: ["First", "Second"]}
        current = {f"{rel}#First", f"{rel}#Second"}

        self.assertEqual(TR.orphaned_identities({rel}, {rel}), [])
        # and classify_missing still calls it a lossless ownership transition
        upgraded, backslid = TR.classify_missing(
            [rel], current, {rel}, {}, ownership)
        self.assertEqual((upgraded, backslid), ([rel], []))

    def test_a_promoted_move_left_unrewritten_is_an_orphan_not_a_transition(self):
        """The exact shape of the eight identities PR #2530 had to rewrite by hand.

        `classify_missing()` blesses this as `absorbed_clean` and lets --check return 0.
        The orphan question is asked first precisely so it does not.
        """
        legacy = "src/_ZN11RickshawBdwD1Ev.cpp"
        dest = "src/game/actors/d_a_obj_km1_kurumajiku.cpp"
        moves = {legacy: ("ov043/daObjKm1_Kurumajiku_c", dest)}
        ownership = {dest: ["First", "Second"]}
        current = {f"{dest}#First", f"{dest}#Second"}

        self.assertEqual(TR.orphaned_identities({legacy}, {dest}), [legacy])
        # classify_missing on its own would forgive it -- that is the defect
        upgraded, backslid = TR.classify_missing(
            [legacy], current, {dest}, moves, ownership)
        self.assertEqual((upgraded, backslid), ([legacy], []))

    def test_the_reason_names_the_destination_when_the_manifest_knows_it(self):
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        moves = {legacy: ("ov001/TU", dest)}

        reason = TR.orphan_reason(legacy, moves)

        self.assertIn("ORPHANED", reason)
        self.assertIn(dest, reason)
        self.assertIn("ov001/TU", reason)

    def test_the_reason_says_so_when_no_manifest_row_names_a_destination(self):
        reason = TR.orphan_reason("src/Legacy.cpp#Sym", {})

        self.assertIn("ORPHANED", reason)
        self.assertIn("src/Legacy.cpp", reason)
        self.assertIn("no promoted TU manifest claims it", reason)

    def test_the_remedy_is_a_rewrite_and_never_points_at_update(self):
        """--update would bank an orphan as a REMOVAL: a false backslide row and a
        falling `count`, for work that never happened."""
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            TR.report_orphans(["src/Legacy.cpp"],
                              {"src/Legacy.cpp": ("ov001/TU", "src/actors/TU.cpp")},
                              "config/converted-backslide-exceptions.jsonl")
        text = out.getvalue()

        self.assertIn("converted_baseline_update", text)
        self.assertIn("REWRITE", text)
        self.assertIn("Do NOT reach for --update here.", text)
        self.assertNotIn("tiers_ratchet.py --update", text)


class OrphanGateEndToEnd(unittest.TestCase):
    """--check must exit 1 on an orphan, and must not invent one on a clean tree."""

    def _run(self, banked, tree, ownership, argv):
        """Run main() over a synthetic tree, returning (exit code, stdout)."""
        td = tempfile.mkdtemp()
        root = pathlib.Path(td)
        for rel in tree:
            p = root / rel
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text("//cpp\nvoid First() {}\nvoid Second() {}\n",
                         encoding="utf-8")
        baseline = root / "baseline.json"
        baseline.write_text(
            json.dumps({"count": len(banked), "converted": sorted(banked)}) + "\n",
            encoding="utf-8")

        out = io.StringIO()
        with mock.patch.object(TR, "REPO", root), \
                mock.patch.object(TR, "tracked_sources", lambda: sorted(tree)), \
                mock.patch.object(TR, "promoted_moves", lambda root=None: {}), \
                mock.patch.object(TR.tiers.srcpath, "source_definition_index",
                                  lambda *a, **k: dict(ownership)), \
                mock.patch.object(
                    sys, "argv",
                    ["tiers_ratchet.py", *argv, "--baseline", str(baseline)]), \
                contextlib.redirect_stdout(out):
            code = TR.main()
        return code, out.getvalue()

    def test_check_fails_on_an_orphan(self):
        tree = ["src/Kept.cpp"]
        ownership = {"src/Kept.cpp": ["Kept"]}
        code, text = self._run({"src/Kept.cpp", "src/Gone.cpp"}, tree, ownership,
                               ["--check"])

        self.assertEqual(code, 1)
        self.assertIn("CONVERTED baseline orphan", text)
        self.assertIn("src/Gone.cpp", text)
        self.assertNotIn("CONVERTED ratchet PASS", text)

    def test_check_passes_when_every_banked_file_is_tracked(self):
        tree = ["src/Kept.cpp"]
        ownership = {"src/Kept.cpp": ["Kept"]}
        code, text = self._run({"src/Kept.cpp"}, tree, ownership, ["--check"])

        self.assertEqual(code, 0)
        self.assertIn("CONVERTED ratchet PASS", text)
        self.assertNotIn("orphan", text)

    def test_the_plain_report_names_orphans_without_an_exit_code(self):
        tree = ["src/Kept.cpp"]
        ownership = {"src/Kept.cpp": ["Kept"]}
        code, text = self._run({"src/Kept.cpp", "src/Gone.cpp"}, tree, ownership, [])

        self.assertEqual(code, 0)
        self.assertIn("ORPHANED", text)
        self.assertIn("src/Gone.cpp", text)
        self.assertIn("<- --check would fail", text)

    def test_an_orphan_is_not_also_counted_as_a_backslide(self):
        """It is removed from the backslide accounting, so the diagnostic is not
        two contradictory explanations of one entry."""
        tree = ["src/Kept.cpp"]
        ownership = {"src/Kept.cpp": ["Kept"]}
        code, text = self._run({"src/Kept.cpp", "src/Gone.cpp"}, tree, ownership,
                               ["--check"])

        self.assertEqual(code, 1)
        self.assertNotIn("CONVERTED backslide", text)


class BaselineIntegrity(unittest.TestCase):
    """A baseline that contradicts itself must stop the tool, not be absorbed by it."""

    def _write(self, body):
        td = tempfile.mkdtemp()
        path = pathlib.Path(td) / "converted-baseline.json"
        path.write_text(json.dumps(body, indent=2) + "\n", encoding="utf-8")
        return str(path)

    def test_duplicate_identity_is_refused(self):
        path = self._write({"count": 3, "converted": ["a.cpp", "b.cpp", "a.cpp"]})

        with self.assertRaises(TR.BaselineError) as caught:
            TR.load_baseline(path)

        self.assertIn("a.cpp", str(caught.exception))

    def test_count_disagreeing_with_the_array_is_refused(self):
        path = self._write({"count": 99, "converted": ["a.cpp", "b.cpp"]})

        with self.assertRaises(TR.BaselineError) as caught:
            TR.load_baseline(path)

        self.assertIn("99", str(caught.exception))

    def test_a_dropped_identity_hidden_by_a_duplicate_is_caught(self):
        """The shape this check exists for: `count` still matches, the set is smaller.

        Removing one identity and duplicating another keeps len(array) == count, so the
        count check alone would pass it. Only the distinctness check sees it.
        """
        path = self._write({"count": 3, "converted": ["a.cpp", "b.cpp", "b.cpp"]})

        with self.assertRaises(TR.BaselineError):
            TR.load_baseline(path)

    def test_consistent_baseline_still_loads(self):
        path = self._write({"count": 2, "converted": ["a.cpp", "b.cpp"]})

        self.assertEqual(TR.load_baseline(path), {"a.cpp", "b.cpp"})

    def test_absent_count_field_is_not_an_error(self):
        """`count` is metadata; only a PRESENT and WRONG one is evidence of an edit."""
        path = self._write({"converted": ["a.cpp", "b.cpp"]})

        self.assertEqual(TR.load_baseline(path), {"a.cpp", "b.cpp"})

    def test_missing_file_still_returns_None_rather_than_raising(self):
        """The absent case keeps its old contract -- main() prints its own guidance."""
        with tempfile.TemporaryDirectory() as td:
            self.assertIsNone(TR.load_baseline(str(pathlib.Path(td) / "nope.json")))

    def test_unparsable_file_still_returns_None_rather_than_raising(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "bad.json"
            path.write_text("{not json", encoding="utf-8")

            self.assertIsNone(TR.load_baseline(str(path)))

    def test_write_then_load_round_trips(self):
        """--update's own output must never trip the check it now has to pass."""
        with tempfile.TemporaryDirectory() as td:
            path = str(pathlib.Path(td) / "baseline.json")
            TR.write_baseline(path, {"b.cpp", "a.cpp", "a.cpp#Member"})

            self.assertEqual(TR.load_baseline(path),
                             {"a.cpp", "b.cpp", "a.cpp#Member"})

if __name__ == "__main__":
    unittest.main()
