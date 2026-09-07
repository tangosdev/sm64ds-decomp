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
