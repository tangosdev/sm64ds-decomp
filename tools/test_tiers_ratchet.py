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


if __name__ == "__main__":
    unittest.main()
