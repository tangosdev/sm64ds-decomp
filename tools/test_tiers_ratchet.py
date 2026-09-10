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


class OrphanDestinationSplit(unittest.TestCase):
    """The orphan classes want different commands, so the report must separate them.

    THE SPLIT IS ON THE IDENTITY A REWRITE CREATES, NOT ON THE DESTINATION FILE. Two
    earlier versions of this report keyed the reader-facing claim on the file, and a
    test in this class enforced the second of those mistakes. A multi-member
    destination is scored per member, so it can hold a target that passes and a sibling
    that fails; judged by the file, a clean rewrite is called a backslide. Measured on
    the real manifest: 620 of the 2,329 banked-path-to-destination pairs at a failing
    destination name a target that passes on its own, over 122 of the 129 failing
    destinations.

    What differs per class, measured rather than inferred:

      target passes,        --update exits 0, writes no exception row, and ABSORBS the
      file passes           entry. Rewrite onto the target; nothing regressed.
      target passes,        Rewrite; nothing regressed FOR THAT IDENTITY. --update
      file does not         still will not absorb it -- classify_missing() keys
                            absorption on the whole file -- so it exits 2 without a
                            --reason and writes a false REMOVED row with one.
      target fails          Rewrite, but it is a backslide the moment it exists.
                            --update behaves as in the row above.
      target unvouched      nothing scored the identity a rewrite would create. Do not
                            rewrite onto it.
      no destination        --update exits 2 without a --reason; with one it is the
                            correct command for a file that really was deleted.

    `UpdateBehaviourPin` below pins all of those against the real tool, so a future edit
    to this prose cannot quietly disagree with the code it describes.
    """

    LEGACY = "src/Legacy.cpp"
    DEST = "src/actors/TU.cpp"
    KNOWN = {LEGACY: ("ov001/TU", DEST)}
    SYMBOLS = {LEGACY: "Sym"}
    # Two enrolled members: `Sym` is what a rewrite of LEGACY creates, `Other` is the
    # sibling that lets the file's verdict differ from the target's. A single-member
    # fixture cannot express this class at all, which is why neither earlier round
    # caught it.
    OWN = {DEST: ["Sym", "Other"]}
    TARGET = f"{DEST}#Sym"
    SIBLING = f"{DEST}#Other"

    def _score(self, *failing):
        return {k: k not in failing for k in TR.tiers.CRITERIA}

    def _scores(self, target_fails=(), sibling_fails=()):
        return {self.TARGET: self._score(*target_fails),
                self.SIBLING: self._score(*sibling_fails)}

    def _report(self, orphans, moves, tracked=None, scores=None, ownership=None,
                symbols=None, banked=None):
        """`banked` defaults to the orphans themselves, which is what a real run holds.

        An orphan is by definition a banked identity, so a baseline that holds exactly
        the orphans is the smallest honest fixture. Tests that care what a rewrite does
        to `count` pass a larger one.
        """
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            TR.report_orphans(orphans, moves,
                              "config/converted-backslide-exceptions.jsonl",
                              set() if tracked is None else tracked,
                              {} if scores is None else scores,
                              {} if ownership is None else ownership,
                              self.SYMBOLS if symbols is None else symbols,
                              set(orphans) if banked is None else banked)
        return out.getvalue()

    def _clean_report(self, orphans=None):
        """Target passes, and so does every other member of its file."""
        return self._report(orphans or [self.LEGACY], self.KNOWN, {self.DEST},
                            self._scores(), self.OWN)

    def _target_only_report(self, orphans=None):
        """Target passes; a DIFFERENT member of the same file fails.

        This is the population VFY-2543-15 is about and the one no fixture covered.
        """
        return self._report(orphans or [self.LEGACY], self.KNOWN, {self.DEST},
                            self._scores(sibling_fails=(TR.tiers.CRITERIA[0],)),
                            self.OWN)

    def _backslide_report(self, orphans=None):
        """The target itself fails."""
        return self._report(orphans or [self.LEGACY], self.KNOWN, {self.DEST},
                            self._scores(target_fails=(TR.tiers.CRITERIA[0],)),
                            self.OWN)

    def _unvouched_report(self, orphans=None):
        """The destination is not a tracked source, so nothing scored anything."""
        return self._report(orphans or [self.LEGACY], self.KNOWN, set(), {}, {})

    def test_destination_is_keyed_on_the_file_part_not_the_whole_identity(self):
        """A member identity must resolve through its path, or the split misfires on
        every promoted-TU entry."""
        self.assertEqual(TR.orphan_destination("src/Legacy.cpp#Sym", self.KNOWN),
                         ("ov001/TU", self.DEST))
        self.assertIsNone(TR.orphan_destination("src/Other.cpp#Sym", self.KNOWN))

    def test_a_named_destination_is_not_assumed_to_pass(self):
        """VFY-2543-10: orphan_destination() answers "is there one", not "is it good".

        The whole defect was one function's answer being read as the other's.
        """
        self.assertEqual(
            TR.destination_state(self.DEST, {self.DEST}, self._scores(), self.OWN)[0],
            "converted")
        self.assertEqual(
            TR.destination_state(self.DEST, {self.DEST},
                                 self._scores(target_fails=(TR.tiers.CRITERIA[0],)),
                                 self.OWN)[0],
            "failing")
        self.assertEqual(
            TR.destination_state(self.DEST, set(), {}, {})[0], "unvouched")

    def test_a_destination_scored_per_member_is_judged_over_every_member(self):
        state, detail = TR.destination_state(
            self.DEST, {self.DEST},
            self._scores(sibling_fails=(TR.tiers.CRITERIA[1],)), self.OWN)

        self.assertEqual(state, "failing")
        self.assertIn(self.SIBLING, detail)

    def test_destination_state_refuses_to_answer_without_a_scan(self):
        """VFY-2543-17 (M-A): `scores=None` used to be this parameter's DEFAULT, and
        its arm returned a verdict -- "unvouched" -- reached with no evidence at all.
        Nothing exercised it, so flipping that arm to "converted" passed all 56 tests:
        the dangerous answer was the default answer. There is no arm now; a caller who
        omits the scan gets an error rather than an opinion."""
        for missing in ("tracked", "scores", "ownership"):
            args = {"tracked": {self.DEST}, "scores": self._scores(),
                    "ownership": self.OWN}
            args[missing] = None
            with self.assertRaises(ValueError):
                TR.destination_state(self.DEST, **args)
        with self.assertRaises(TypeError):
            TR.destination_state(self.DEST)

    def test_a_partly_scored_destination_is_not_called_converted(self):
        """VFY-2543-17 (M-B): deleting the partially-scored guard passed all 56 tests.

        An unscored member is not in `current`, so classify_missing() will not absorb
        the file -- calling it "converted" would predict an --update that does not
        happen, and would claim a file passes on the strength of the members that
        happened to be scored.
        """
        partial = {self.TARGET: self._score()}      # SIBLING never scored
        state, detail = TR.destination_state(self.DEST, {self.DEST}, partial, self.OWN)

        self.assertEqual(state, "unvouched")
        self.assertIn("not scored", detail)

    def test_target_state_answers_about_the_identity_not_the_file(self):
        """VFY-2543-15, at the smallest scale that can express it.

        Same file, same scan, two different questions -- and this is the case the two
        earlier rounds answered with the file's verdict.
        """
        scores = self._scores(sibling_fails=(TR.tiers.CRITERIA[0],))

        self.assertEqual(
            TR.destination_state(self.DEST, {self.DEST}, scores, self.OWN)[0],
            "failing")
        state, _detail, target = TR.target_state(
            self.LEGACY, self.DEST, {self.DEST}, scores, self.OWN, self.SYMBOLS)
        self.assertEqual(state, "converted")
        self.assertEqual(target, self.TARGET)

    def test_target_state_refuses_to_answer_without_a_scan(self):
        for missing in ("tracked", "scores", "ownership", "symbols"):
            args = {"tracked": {self.DEST}, "scores": self._scores(),
                    "ownership": self.OWN, "symbols": self.SYMBOLS}
            args[missing] = None
            with self.assertRaises(ValueError):
                TR.target_state(self.LEGACY, self.DEST, **args)

    def test_the_five_classes_are_reported_in_separate_sections(self):
        # LEGACY -> a passing target in a file that passes  (clean)
        # good   -> a passing target in a file that fails   (target_only)
        # bad    -> the member the scan fails               (backslide)
        # away   -> an untracked destination                (unvouched)
        # gone   -> no manifest row at all                  (unexplained)
        good, bad, away, gone = ("src/Good.cpp", "src/Bad.cpp", "src/Away.cpp",
                                 "src/Vanished.cpp")
        mixed = "src/actors/Mixed.cpp"
        moves = {self.LEGACY: ("ov001/TU", self.DEST),
                 good: ("ov002/TU", mixed),
                 bad: ("ov003/TU", mixed),
                 away: ("ov004/TU", "src/actors/Untracked.cpp")}
        ownership = dict(self.OWN, **{mixed: ["A", "B"]})
        scores = dict(self._scores(),
                      **{f"{mixed}#A": self._score(TR.tiers.CRITERIA[0]),
                         f"{mixed}#B": self._score()})
        symbols = {self.LEGACY: "Sym", good: "B", bad: "A", away: "A"}
        text = self._report([self.LEGACY, good, bad, away, gone],
                            moves, {self.DEST, mixed}, scores, ownership, symbols)

        heads = ["whose rewrite target PASSES, in a file that passes too",
                 "whose rewrite target PASSES, in a file that does NOT",
                 "whose rewrite target does NOT pass",
                 "whose rewrite target this tool cannot vouch for",
                 "with no TU promotion destination"]
        for head in heads:
            self.assertIn(head, text)
        positions = [text.index(h) for h in heads]
        self.assertEqual(positions, sorted(positions))

    def test_nothing_regressed_is_claimed_where_the_rewrite_target_passes(self):
        """VFY-2543-15, the invariant this class exists for.

        The version of this test that shipped in the previous round asserted
        `assertNotIn("Nothing regressed", failing_report)` -- keyed on the DESTINATION,
        so it enforced the false claim over the 620 pairs whose target passes anyway.
        It had to go, like the permissive one before it. What is true is keyed on the
        identity the block tells the reader to create.
        """
        self.assertIn("Nothing regressed", self._clean_report())
        self.assertIn("Nothing regressed FOR THESE IDENTITIES",
                      self._target_only_report())
        self.assertNotIn("Nothing regressed", self._backslide_report())
        self.assertNotIn("Nothing regressed", self._unvouched_report())
        self.assertNotIn("Nothing regressed", self._report(["src/Vanished.cpp"], {}))

    def test_a_passing_target_in_a_failing_file_is_not_called_a_backslide(self):
        """The other half of the same finding: the old text said "the rewritten
        identity is a backslide the moment it exists" over this class, which is false
        for every one of the 620."""
        text = self._target_only_report()

        self.assertNotIn("backslide the moment it exists", text)
        self.assertIn(f"rewrite onto {self.TARGET} -- it passes all five", text)

    def test_the_unit_mismatch_is_explained_where_it_appears(self):
        """One paragraph of this section keys on the identity and the next on the file,
        because classify_missing() does. A reader who notices that without an
        explanation will assume one half is wrong."""
        text = self._target_only_report()

        self.assertIn("KEYS ON THE FILE, NOT THE IDENTITY", text)
        self.assertIn("classify_missing() absorbs a moved path only", text)

    def test_both_failing_classes_state_the_rewrite_first_ordering(self):
        """VFY-2543-16: "bank it with a reason" sat four lines from "a permanent false
        statement in a log whose whole value is that it can be trusted". Measured: bank
        first and the row names the legacy path and is false; rewrite first and there
        is either no row at all (target passes) or a row naming the rewritten identity
        (target fails), which is true."""
        for text in (self._target_only_report(), self._backslide_report()):
            self.assertIn("REWRITE FIRST", text)

    def test_a_failing_target_names_the_criterion_it_fails(self):
        text = self._backslide_report()

        self.assertIn(TR.tiers.CRITERION_LABEL[TR.tiers.CRITERIA[0]], text)

    def test_a_failing_target_is_still_told_to_rewrite(self):
        """The rewrite is right in every state; only the reassurance is not."""
        text = self._backslide_report()

        self.assertIn("REWRITE these", text)
        self.assertIn(f"      rewrite onto {self.TARGET}", text)

    def test_neither_failing_class_is_handed_the_removal_command(self):
        """--update does not absorb either class, but re-banking is still not the
        repair: the row it writes says the legacy path was REMOVED and nothing was."""
        for text in (self._target_only_report(), self._backslide_report()):
            self.assertNotIn('python tools/tiers_ratchet.py --update --reason', text)
            self.assertIn("REMOVED", text)

    def test_an_unvouched_destination_is_not_rewritten_onto(self):
        text = self._unvouched_report()

        self.assertIn("cannot vouch for", text)
        self.assertIn("Do not rewrite onto an identity this tool cannot see", text)
        # the ROW must not name one; the prose two paragraphs down says not to
        self.assertNotIn("      rewrite onto", text)

    def test_a_carried_symbol_the_destination_does_not_define_is_unvouched(self):
        """VFY-2543-18: a `#symbol` carried on the banked identity is passed through
        without a manifest check. The only check that means anything is looking the
        resulting identity up in the scan -- a symbol the destination does not define
        is scored by nothing, so rewriting onto it would recreate the orphan."""
        text = self._report([f"{self.LEGACY}#Nowhere"], self.KNOWN, {self.DEST},
                            self._scores(), self.OWN)

        self.assertIn("cannot vouch for", text)
        self.assertIn(f"{self.DEST}#Nowhere", text)
        self.assertIn("does not define that symbol", text)

    def test_the_rewrite_target_follows_the_destination_member_count(self):
        """VFY-2543-13: tu_promote.py writes a bare dest for a single-function TU and
        `dest#symbol` otherwise, and scan() scores it the same way. 0 of the 136 real
        destinations take the first branch today, which is why a constant went
        unnoticed."""
        self.assertEqual(
            TR.rewrite_target(self.LEGACY, self.DEST, {self.DEST: ["Only"]},
                              self.SYMBOLS),
            self.DEST)
        self.assertEqual(
            TR.rewrite_target(self.LEGACY, self.DEST, self.OWN, self.SYMBOLS),
            self.TARGET)

    def test_an_empty_ownership_entry_takes_the_same_branch_scan_does(self):
        """VFY-2543-18: `len(members) <= 1` swallowed the empty case into the
        single-member one and happened to agree, guarded only by its one call site.
        scan() falls back to the path stem, so a destination with no ownership entry is
        scored under its BARE path and the rewrite target must be the bare path too."""
        self.assertEqual(TR.rewrite_target(self.LEGACY, self.DEST, {}, self.SYMBOLS),
                         self.DEST)
        self.assertEqual(
            TR.rewrite_target(self.LEGACY, self.DEST, {self.DEST: []}, self.SYMBOLS),
            self.DEST)

    def test_the_rewrite_target_carries_the_identitys_own_symbol_when_it_has_one(self):
        self.assertEqual(
            TR.rewrite_target(f"{self.LEGACY}#Other", self.DEST, self.OWN,
                              self.SYMBOLS),
            self.SIBLING)

    def test_an_unknown_symbol_is_not_invented(self):
        self.assertIsNone(
            TR.rewrite_target(self.LEGACY, self.DEST, self.OWN, {}))
        text = self._report([self.LEGACY], self.KNOWN, {self.DEST}, self._scores(),
                            self.OWN, {})

        self.assertIn("cannot vouch for", text)
        self.assertIn("no manifest row here enrolls", text)
        self.assertNotIn("      rewrite onto", text)

    def test_a_destination_unknown_orphan_gets_the_working_removal_command(self):
        """VFY-2543-03: base printed the command that works; forbidding it while
        naming the log it writes left the reader with a destination and no road."""
        text = self._report(["src/Vanished.cpp"], {})

        self.assertIn('python tools/tiers_ratchet.py --update --reason', text)
        self.assertIn("converted-backslide-exceptions.jsonl", text)
        self.assertIn("only caller of append_exceptions()", text)
        self.assertNotIn("Do NOT re-bank", text)

    def test_no_diagnostic_claims_a_falling_count_is_a_regression(self):
        """VFY-2543-02: `count` is metadata. load_baseline reads it back only to catch
        a hand-edit; nothing else in the tree reads it, so no gate can act on it."""
        for text in (self._clean_report(), self._target_only_report(),
                     self._backslide_report(), self._unvouched_report(),
                     self._report(["src/Vanished.cpp"], {})):
            self.assertNotIn("count` downward", text)
            self.assertNotIn("reports as a regression", text)

    def test_a_passing_destination_orphan_is_not_told_update_will_refuse(self):
        """VFY-2543-01: it does not refuse for this class -- it exits 0 and absorbs."""
        text = self._clean_report()

        self.assertNotIn("demand a", text)
        self.assertIn("does not refuse -- it ABSORBS", text)
        self.assertIn("Do NOT re-bank these with --update.", text)
        self.assertNotIn("--update --reason", text)

    def test_the_open_update_hole_is_acknowledged_not_asserted_away(self):
        """VFY-2543-06 is a real, unclosed hole: one --update launders every orphan of
        the passing-destination class and the result is green. The diagnostic must not
        imply otherwise."""
        self.assertIn("hole is still open", self._clean_report())

    def test_the_reason_names_the_destination_when_the_manifest_knows_it(self):
        reason = TR.orphan_reason(self.LEGACY, self.KNOWN)

        self.assertIn("ORPHANED", reason)
        self.assertIn(self.DEST, reason)
        self.assertIn("ov001/TU", reason)

    def test_the_reason_says_so_when_no_manifest_row_names_a_destination(self):
        reason = TR.orphan_reason("src/Legacy.cpp#Sym", {})

        self.assertIn("ORPHANED", reason)
        self.assertIn("src/Legacy.cpp", reason)
        self.assertIn("no promoted TU manifest claims it", reason)

    # ------------------------------------------------------------------ polish round

    @staticmethod
    def _flat(text):
        """One line, single-spaced, so an assertion survives a re-wrap.

        Every sentence below is wrapped by hand around interpolated numbers whose
        width depends on the data. Asserting on the wrapped form would make these
        tests fail when a figure gains a digit, which is the opposite of what they
        are for.
        """
        return " ".join(text.split())

    def _census_fixture(self):
        """Three pairs at one failing destination: two pass, one fails.

        Deliberately unlike the real tree, so a figure typed back into the report
        cannot pass. The report's own population is one orphan per failing class;
        the CENSUS is over `moves`, which is every promoted pair, orphan or not.
        """
        second, third = "src/Second.cpp", "src/Third.cpp"
        moves = dict(self.KNOWN, **{second: ("ov001/TU", self.DEST),
                                    third: ("ov001/TU", self.DEST)})
        symbols = dict(self.SYMBOLS, **{second: "Other", third: "Sym"})
        scores = self._scores(sibling_fails=(TR.tiers.CRITERIA[0],))
        return second, moves, symbols, scores

    def test_promotion_census_counts_the_pairs_and_the_destinations(self):
        """VFY-2543-20: the figures the report quotes have to come from somewhere."""
        _second, moves, symbols, scores = self._census_fixture()

        census = TR.promotion_census(moves, {self.DEST}, scores, self.OWN, symbols)

        self.assertEqual((census.pairs, census.destinations), (3, 1))
        self.assertEqual((census.dest_converted, census.dest_failing), (0, 1))
        self.assertEqual(census.at_failing, 3)
        self.assertEqual((census.target_passes, census.target_fails), (2, 1))
        self.assertEqual(census.failing_with_a_passing_target, 1)

        # A destination that passes is not in the pool the report divides up: the two
        # failing classes exist only under a failing destination.
        passing = TR.promotion_census(moves, {self.DEST}, self._scores(), self.OWN,
                                      symbols)
        self.assertEqual((passing.dest_converted, passing.at_failing), (1, 0))

    def test_the_printed_population_is_computed_from_this_run(self):
        """VFY-2543-20: 620 / 2,329 / 1,709 / 122 / 129 used to be typed in.

        Three mutants that falsified them left all 69 tests green, because nothing in
        the tool or the suite compared a printed figure to any data. A number a reader
        weighs a decision against has to be derived from the same scan the rest of the
        block is derived from, or it is a claim about a tree that no longer exists.
        """
        second, moves, symbols, scores = self._census_fixture()

        flat = self._flat(self._report([self.LEGACY, second], moves, {self.DEST},
                                       scores, self.OWN, symbols))

        self.assertIn("not quoted from a note: 2 of the 3 banked paths whose "
                      "destination fails are in this class, over 1 of the 1 failing "
                      "destinations", flat)
        self.assertIn("rather than quoted: 1 of the 3 banked paths whose destination "
                      "fails", flat)
        for stale in ("620", "2,329", "1,709", "122", "129"):
            self.assertNotIn(stale, flat)

    def test_the_count_claim_is_keyed_on_whether_the_target_is_already_banked(self):
        """VFY-2543-21: "leaving `count` unchanged" is wrong by one, and often.

        A rewrite onto a target that is not banked renames one element of the set.
        A rewrite onto one that IS banked drops the legacy key and inserts nothing --
        `tu_promote.py:converted_baseline_update` is literally `if target not in
        converted` -- so the set loses one and `count` falls. Measured on the real
        tree: 645 of the 2,359 promoted pairs name a target that is banked already,
        and for one of them the move is 2702 -> 2701. The block claimed one answer
        for both cases.
        """
        fresh = self._flat(self._clean_report())
        self.assertIn("0 of the 1 target(s) above are banked already", fresh)
        self.assertIn("moves count 1 -> 1", fresh)

        already = self._flat(self._report([self.LEGACY], self.KNOWN, {self.DEST},
                                          self._scores(), self.OWN,
                                          banked={self.LEGACY, self.TARGET}))
        self.assertIn("1 of the 1 target(s) above are banked already", already)
        self.assertIn("moves count 2 -> 1", already)

        # The retired sentence must not come back in either arm.
        for text in (fresh, already):
            self.assertNotIn("leaving `count` unchanged", text)

    def test_a_carried_symbol_the_manifest_disagrees_with_is_named(self):
        """VFY-2543-19: two sources answer "which symbol", and one won in silence.

        `rewrite_target()` prefers the symbol CARRIED on the banked identity;
        `promoted_symbols()` holds the manifest's answer for the same legacy path.
        Looking the result up in the scan cannot tell them apart -- it proves only
        that the destination defines the symbol it was handed -- so a rewrite onto the
        wrong member of the right file passed every check this tool had. Measured on
        the real tree: 0 of the 651 banked identities carrying a `#symbol` disagree
        with the manifest, so this is a state a hand rewrite introduces rather than one
        the tree is in, which is exactly why nothing here can be left to notice it by
        accident.
        """
        carried = f"{self.LEGACY}#Other"          # manifest says Sym, identity says Other

        state, detail, target = TR.target_state(
            carried, self.DEST, {self.DEST}, self._scores(), self.OWN, self.SYMBOLS)

        self.assertEqual((state, target), ("converted", self.SIBLING))
        self.assertIn(f"the manifest enrols {self.LEGACY} under Sym, not Other", detail)
        self.assertIn("cannot say which is right", detail)

        # Agreement is silent: a note on every row would be noise, not a signal.
        agreed = TR.target_state(f"{self.LEGACY}#Sym", self.DEST, {self.DEST},
                                 self._scores(), self.OWN, self.SYMBOLS)
        self.assertNotIn("the manifest enrols", agreed[1])

        # And it reaches the reader, not just the return value.
        printed = self._report([carried], self.KNOWN, {self.DEST}, self._scores(),
                               self.OWN)
        self.assertIn(f"the manifest enrols {self.LEGACY} under Sym, not Other",
                      printed)

        # The claim that made the silence look deliberate is retired for good.
        self.assertNotIn("only check that means anything", TR.rewrite_target.__doc__)


class SyntheticTree:
    """Build a throwaway src/ tree and a baseline, and run main() against them.

    A mixin, not a TestCase: `UpdateBehaviourPin` needs the same fixture but must not
    inherit and re-run every --check test along with it.
    """

    TWO_FN = "//cpp\nvoid First() {}\nvoid Second() {}\n"
    FAILING_TWO_FN = ("//cpp\nint unk_18;\n"
                      "void First() { unk_18 = 1; }\nvoid Second() {}\n")
    # One member fails and the other passes -- the shape VFY-2543-15 is about, and
    # the shape of the real src/actors/ActorDerived.cpp, where 1 of 5 members is still
    # named func_02013edc. FAILING_TWO_FN cannot express it: a file-scope `unk_18`
    # fails BOTH members, so every earlier fixture had the file and its members
    # agreeing, which is exactly the case that hides this defect.
    MIXED_TWO_FN = "//cpp\nvoid func_02013edc() {}\nvoid Second() {}\n"
    MIXED_OWN = ["func_02013edc", "Second"]

    def _tree(self, tree, banked, extra=None):
        td = tempfile.mkdtemp()
        root = pathlib.Path(td)
        for rel in tree:
            p = root / rel
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text((extra or {}).get(rel, "//cpp\nvoid First() {}\n"),
                         encoding="utf-8")
        baseline = root / "baseline.json"
        baseline.write_text(
            json.dumps({"count": len(banked), "converted": sorted(banked)}) + "\n",
            encoding="utf-8")
        return root, baseline

    def _patches(self, root, tree, ownership, moves, argv, symbols=None):
        # promoted_symbols is patched too: unpatched it would read the real manifest
        # from disk during a synthetic run, which is both slow and a hidden dependency
        # on repository data these fixtures are meant to be independent of.
        return (mock.patch.object(TR, "REPO", root),
                mock.patch.object(TR, "tracked_sources", lambda: sorted(tree)),
                mock.patch.object(TR, "promoted_moves",
                                  lambda root=None: dict(moves or {})),
                mock.patch.object(TR, "promoted_symbols",
                                  lambda root=None: dict(symbols or {})),
                mock.patch.object(TR.tiers.srcpath, "source_definition_index",
                                  lambda *a, **k: dict(ownership)),
                mock.patch.object(sys, "argv", argv))

    def _run(self, banked, tree, ownership, argv, moves=None, extra=None,
             symbols=None):
        """Run main() over a synthetic tree, returning (exit code, stdout)."""
        root, baseline = self._tree(tree, banked, extra)
        full = ["tiers_ratchet.py", *argv, "--baseline", str(baseline)]
        out = io.StringIO()
        with contextlib.ExitStack() as stack:
            for p in self._patches(root, tree, ownership, moves, full, symbols):
                stack.enter_context(p)
            stack.enter_context(contextlib.redirect_stdout(out))
            code = TR.main()
        return code, out.getvalue()


class OrphanGateEndToEnd(SyntheticTree, unittest.TestCase):
    """Drive main() over a synthetic tree and judge it by exit code and output.

    These replace an earlier set that asserted `orphaned_identities()` against literal
    sets. That restated the one-line predicate and would not have survived a rewrite
    that kept the same behaviour; what matters is whether the GATE fails.
    """

    def test_check_fails_on_an_orphan(self):
        code, text = self._run({"src/Kept.cpp", "src/Gone.cpp"}, ["src/Kept.cpp"],
                               {"src/Kept.cpp": ["Kept"]}, ["--check"])

        self.assertEqual(code, 1)
        self.assertIn("CONVERTED baseline orphan", text)
        self.assertIn("src/Gone.cpp", text)
        self.assertNotIn("CONVERTED ratchet PASS", text)

    def test_check_passes_when_every_banked_file_is_tracked(self):
        code, text = self._run({"src/Kept.cpp"}, ["src/Kept.cpp"],
                               {"src/Kept.cpp": ["Kept"]}, ["--check"])

        self.assertEqual(code, 0)
        self.assertIn("CONVERTED ratchet PASS", text)
        self.assertNotIn("orphan", text)

    def test_a_member_identity_whose_file_exists_does_not_fail_the_gate(self):
        """The `#`-split trap, as a gate outcome rather than a set comparison.

        Matching whole identities against the tracked set would report every
        promoted-TU member as an orphan -- 651 of 2701 entries when this was written.
        """
        rel = "src/actors/TU.cpp"
        code, text = self._run({f"{rel}#First", f"{rel}#Second"}, [rel],
                               {rel: ["First", "Second"]}, ["--check"],
                               extra={rel: self.TWO_FN})

        self.assertEqual(code, 0)
        self.assertIn("CONVERTED ratchet PASS", text)
        self.assertNotIn("ORPHANED", text)

    def test_a_bare_path_scored_per_member_stays_a_transition_and_passes(self):
        """`absorbed_clean` branch 1: the file STILL EXISTS, so it is not an orphan."""
        rel = "src/actors/TU.cpp"
        code, text = self._run({rel}, [rel], {rel: ["First", "Second"]}, ["--check"],
                               extra={rel: self.TWO_FN})

        self.assertEqual(code, 0)
        self.assertIn("CONVERTED ratchet PASS", text)
        self.assertIn("clean ownership transition", text)
        self.assertNotIn("ORPHANED", text)

    def test_a_promoted_move_left_unrewritten_fails_instead_of_being_forgiven(self):
        """The shape of the eight identities PR #2530 repaired by hand.

        classify_missing() sends this to absorbed_clean branch 2 and the gate used to
        print it under a PASS and return 0.
        """
        legacy = "src/_ZN11RickshawBdwD1Ev.cpp"
        dest = "src/game/actors/d_a_obj_km1_kurumajiku.cpp"
        code, text = self._run(
            {legacy}, [dest], {dest: ["First", "Second"]}, ["--check"],
            moves={legacy: ("ov043/daObjKm1_Kurumajiku_c", dest)},
            extra={dest: self.TWO_FN}, symbols={legacy: "First"})

        self.assertEqual(code, 1)
        self.assertIn("REWRITE these", text)
        self.assertIn(f"rewrite onto {dest}#First", text)
        self.assertNotIn("CONVERTED ratchet PASS", text)

    def test_the_plain_report_names_orphans_without_an_exit_code(self):
        code, text = self._run({"src/Kept.cpp", "src/Gone.cpp"}, ["src/Kept.cpp"],
                               {"src/Kept.cpp": ["Kept"]}, [])

        self.assertEqual(code, 0)
        self.assertIn("ORPHANED", text)
        self.assertIn("src/Gone.cpp", text)
        self.assertIn("<- --check would fail", text)
        # the same five-way split --check prints, through the same function
        self.assertIn("target passes ", text)
        self.assertIn("target passes, file", text)
        self.assertIn("target fails", text)
        self.assertIn("target unvouched", text)
        self.assertIn("investigate               1", text)

    def test_an_orphan_is_not_also_counted_as_a_backslide(self):
        """It is removed from the backslide accounting, so the diagnostic is not two
        contradictory explanations of one entry."""
        code, text = self._run({"src/Kept.cpp", "src/Gone.cpp"}, ["src/Kept.cpp"],
                               {"src/Kept.cpp": ["Kept"]}, ["--check"])

        self.assertEqual(code, 1)
        self.assertNotIn("CONVERTED backslide", text)

    def test_an_orphan_and_a_backslide_are_both_reported(self):
        """VFY-2543-08: the interleaved path. One defect must not mask the other."""
        code, text = self._run(
            {"src/Kept.cpp", "src/Bad.cpp", "src/Gone.cpp"},
            ["src/Bad.cpp", "src/Kept.cpp"],
            {"src/Kept.cpp": ["Kept"], "src/Bad.cpp": ["Bad"]}, ["--check"],
            extra={"src/Bad.cpp": "//cpp\nint unk_18;\nvoid Bad() { unk_18 = 1; }\n"})

        self.assertEqual(code, 1)
        self.assertIn("CONVERTED baseline orphan", text)
        self.assertIn("src/Gone.cpp", text)
        self.assertIn("CONVERTED backslide", text)
        self.assertIn("src/Bad.cpp", text)

    def test_an_orphan_run_still_lists_a_lossless_ownership_transition(self):
        """VFY-2543-07: the orphan-only exit used to drop what the PASS path lists."""
        rel = "src/actors/TU.cpp"
        code, text = self._run(
            {rel, "src/Gone.cpp"}, [rel], {rel: ["First", "Second"]}, ["--check"],
            extra={rel: self.TWO_FN})

        self.assertEqual(code, 1)
        self.assertIn("CONVERTED baseline orphan", text)
        self.assertIn("lossless ownership transition", text)
        self.assertIn("IDENTITY UPGRADE", text)

    def test_an_orphan_whose_rewrite_target_fails_is_not_told_nothing_regressed(self):
        """VFY-2543-10, as the run that exposed it: one --check contradicting itself.

        The report claimed "the readable code is still in the tree under the destination
        named above" and then, twelve lines later, printed a backslide against that same
        destination. Following the printed remedy banked a failing identity.
        """
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        code, text = self._run(
            {legacy, f"{dest}#First"}, [dest], {dest: ["First", "Second"]},
            ["--check"], moves={legacy: ("ov001/TU", dest)},
            extra={dest: self.FAILING_TWO_FN}, symbols={legacy: "First"})

        self.assertEqual(code, 1)
        self.assertIn("whose rewrite target does NOT pass", text)
        self.assertNotIn("Nothing regressed", text)
        # the two halves of the run now agree with each other
        self.assertIn("CONVERTED backslide", text)
        self.assertIn(f"{dest}#First", text)

    def test_an_orphan_whose_target_passes_in_a_failing_file_is_told_so(self):
        """VFY-2543-15 end to end, on the shape that produced the finding.

        The destination fails -- one of its two members is still named func_<addr> --
        but the identity this orphan's rewrite creates is the OTHER member, and it
        passes. Two earlier versions of this report told the reader that rewrite was a
        backslide. On the real tree this is 620 of the 2,329 pairs at a failing
        destination, over 122 of the 129 such destinations.
        """
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        code, text = self._run(
            {legacy}, [dest], {dest: self.MIXED_OWN}, ["--check"],
            moves={legacy: ("ov001/TU", dest)}, extra={dest: self.MIXED_TWO_FN},
            symbols={legacy: "Second"})

        self.assertEqual(code, 1)
        self.assertIn("whose rewrite target PASSES, in a file that does NOT", text)
        self.assertIn(f"rewrite onto {dest}#Second -- it passes all five", text)
        self.assertIn("Nothing regressed FOR THESE IDENTITIES", text)
        self.assertNotIn("backslide the moment it exists", text)
        # and the file's own verdict is still on the row, because --update keys on it
        self.assertIn(f"{dest}#func_02013edc", text)

    def test_rewriting_that_orphan_onto_its_target_makes_the_gate_pass(self):
        """The repair the section above prints, run end to end.

        Same tree, same failing destination; the only change is that the baseline
        names the target instead of the legacy path. Reproduced on the real tree too:
        src/_ZN7dBase_cD1Ev.cpp rewritten onto
        src/actors/ActorDerived.cpp#_ZN7dBase_cD1Ev takes --check from 1 to 0 with no
        exception row written at all.
        """
        dest = "src/actors/TU.cpp"
        code, text = self._run(
            {f"{dest}#Second"}, [dest], {dest: self.MIXED_OWN}, ["--check"],
            moves={"src/Legacy.cpp": ("ov001/TU", dest)},
            extra={dest: self.MIXED_TWO_FN}, symbols={"src/Legacy.cpp": "Second"})

        self.assertEqual(code, 0)
        self.assertIn("CONVERTED ratchet PASS", text)
        self.assertNotIn("CONVERTED baseline orphan", text)

    def test_a_failing_target_orphan_is_still_told_where_to_rewrite(self):
        """Naming the defect must not cost the reader the repair."""
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        _code, text = self._run(
            {legacy}, [dest], {dest: ["First", "Second"]}, ["--check"],
            moves={legacy: ("ov001/TU", dest)}, extra={dest: self.FAILING_TWO_FN},
            symbols={legacy: "First"})

        self.assertIn(f"rewrite onto {dest}#First", text)

    def test_a_backslide_on_a_promoted_destination_is_annotated_as_one(self):
        """VFY-2543-12: the old guard tested `rel in moves`, whose keys are LEGACY
        sources -- and a legacy source that is still tracked cannot reach the backslide
        list (0 of 2359 are tracked today) while one that is not is peeled off as an
        orphan. So it could only fire in a run with no MOVED line to annotate. The
        guard now tests what the paragraph is about: the failing file is a promotion
        DESTINATION."""
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        _code, text = self._run(
            {f"{dest}#First"}, [dest], {dest: ["First", "Second"]}, ["--check"],
            moves={legacy: ("ov001/TU", dest)}, extra={dest: self.FAILING_TWO_FN})

        self.assertIn("CONVERTED backslide", text)
        self.assertIn("Lines above name a promoted TU", text)

    def test_an_ordinary_backslide_is_not_annotated_as_a_promotion(self):
        code, text = self._run(
            {"src/Bad.cpp"}, ["src/Bad.cpp"], {"src/Bad.cpp": ["Bad"]}, ["--check"],
            extra={"src/Bad.cpp": "//cpp\nint unk_18;\nvoid Bad() { unk_18 = 1; }\n"})

        self.assertEqual(code, 1)
        self.assertIn("CONVERTED backslide", text)
        self.assertNotIn("Lines above name a promoted TU", text)


class UpdateBehaviourPin(SyntheticTree, unittest.TestCase):
    """Pin what --update MEASURABLY does to each orphan class.

    The diagnostics assert things about --update. An earlier version asserted the
    opposite of the truth for the destination-known class -- that it would refuse,
    demand a --reason and write an exception row -- and the suite carried the error
    instead of catching it, because nothing here ever ran --update. These tests do.

    The class also splits on the destination's STATE, which is not obvious from the
    code and was got wrong a second time: --update absorbs a moved path only when the
    destination passes. When it fails, the same entry is a removal, so the behaviour
    inverts -- refusal without a --reason, and a false removal row with one.

    THAT SPLIT IS ON THE FILE, WHILE THE REPORT'S REGRESSION CLAIM IS ON THE IDENTITY,
    and the last two tests here are the case where the two differ: a destination whose
    rewrite target passes while a sibling member fails. --update treats it exactly like
    any other failing destination, because classify_missing() requires EVERY enrolled
    member to be CONVERTED. Those two tests exist so the report's --update sentences
    cannot drift onto the identity along with the rest of the prose.
    """

    def _update(self, banked, tree, ownership, argv, moves=None, extra=None):
        root, baseline = self._tree(tree, banked, extra)
        exceptions = root / "exceptions.jsonl"
        full = ["tiers_ratchet.py", "--update", *argv, "--baseline", str(baseline),
                "--exceptions", str(exceptions)]
        out = io.StringIO()
        with contextlib.ExitStack() as stack:
            for p in self._patches(root, tree, ownership, moves, full):
                stack.enter_context(p)
            stack.enter_context(contextlib.redirect_stdout(out))
            code = TR.main()
        return code, out.getvalue(), exceptions, baseline

    def test_update_absorbs_a_destination_known_orphan_and_never_refuses(self):
        """Measured on the real pre-#2530 baseline too: exit 0, no exception row.

        This is why that class is told to rewrite rather than re-bank -- --update drops
        the dangling identity and the evidence of the defect with it.
        """
        legacy = "src/_ZN11RickshawBdwD1Ev.cpp"
        dest = "src/game/actors/d_a_obj_km1_kurumajiku.cpp"
        code, text, exceptions, _ = self._update(
            {legacy}, [dest], {dest: ["First", "Second"]}, [],
            moves={legacy: ("ov043/daObjKm1_Kurumajiku_c", dest)},
            extra={dest: self.TWO_FN})

        self.assertEqual(code, 0)
        self.assertFalse(exceptions.exists())
        self.assertIn("a move, not a removal", text)
        self.assertNotIn("REFUSING", text)

    def test_update_refuses_a_destination_unknown_orphan_without_a_reason(self):
        code, text, exceptions, _ = self._update(
            {"src/Kept.cpp", "src/Gone.cpp"}, ["src/Kept.cpp"],
            {"src/Kept.cpp": ["Kept"]}, [])

        self.assertEqual(code, 2)
        self.assertFalse(exceptions.exists())
        self.assertIn("REFUSING to bank 1 removal(s) without --reason", text)

    def test_update_with_a_reason_banks_the_removal_and_writes_the_row(self):
        """The command the destination-unknown diagnostic hands the reader must work."""
        code, text, exceptions, baseline = self._update(
            {"src/Kept.cpp", "src/Gone.cpp"}, ["src/Kept.cpp"],
            {"src/Kept.cpp": ["Kept"]}, ["--reason", "the code really left the tree"])

        self.assertEqual(code, 0)
        self.assertTrue(exceptions.exists())
        row = json.loads(exceptions.read_text(encoding="utf-8").strip())
        self.assertEqual(row["path"], "src/Gone.cpp")
        self.assertEqual(row["reason"], "the code really left the tree")
        self.assertNotIn("src/Gone.cpp", TR.load_baseline(str(baseline)))

    def test_update_refuses_an_orphan_whose_destination_fails(self):
        """The inverse of the case above, and the reason the diagnostic splits.

        classify_missing() absorbs a moved path only when the destination passes, so a
        failing destination sends the same entry to `removed`. Telling this class
        "--update exits 0 and absorbs" would have been wrong for 129 of the 136
        destinations the real manifest names.
        """
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        code, text, exceptions, _ = self._update(
            {legacy}, [dest], {dest: ["First", "Second"]}, [],
            moves={legacy: ("ov001/TU", dest)}, extra={dest: self.FAILING_TWO_FN})

        self.assertEqual(code, 2)
        self.assertFalse(exceptions.exists())
        self.assertIn("REFUSING to bank 1 removal(s) without --reason", text)

    def test_update_with_a_reason_writes_a_false_removal_row_for_that_orphan(self):
        """Why the failing-destination diagnostic forbids the removal command.

        The row records the LEGACY path as having left the CONVERTED set. It did not
        leave: it moved into `dest`, which the same manifest names. The exception log is
        the tool's permanent record of readability deliberately traded for byte-match,
        and a row like this one is a false entry in it.
        """
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        code, _text, exceptions, _baseline = self._update(
            {legacy}, [dest], {dest: ["First", "Second"]}, ["--reason", "probe"],
            moves={legacy: ("ov001/TU", dest)}, extra={dest: self.FAILING_TWO_FN})

        self.assertEqual(code, 0)
        row = json.loads(exceptions.read_text(encoding="utf-8").strip())
        self.assertEqual(row["path"], legacy)

    def test_update_still_refuses_when_only_the_rewrite_target_passes(self):
        """VFY-2543-15's asymmetry, pinned against the real tool.

        The report keys its regression claim on the identity and its --update claim on
        the FILE, and this is the case where those two differ: the target passes, the
        file does not, and --update behaves as it does for any failing destination
        because classify_missing() keys absorption on every enrolled member. If the
        report ever moved the --update sentence onto the identity as well, this goes
        red.
        """
        legacy = "src/Legacy.cpp"
        dest = "src/actors/TU.cpp"
        code, text, exceptions, _ = self._update(
            {legacy}, [dest], {dest: self.MIXED_OWN}, [],
            moves={legacy: ("ov001/TU", dest)}, extra={dest: self.MIXED_TWO_FN})

        self.assertEqual(code, 2)
        self.assertFalse(exceptions.exists())
        self.assertIn("REFUSING to bank 1 removal(s) without --reason", text)

    def test_update_with_a_reason_writes_the_false_row_for_that_class_too(self):
        code, _text, exceptions, _ = self._update(
            {"src/Legacy.cpp"}, ["src/actors/TU.cpp"],
            {"src/actors/TU.cpp": self.MIXED_OWN}, ["--reason", "probe"],
            moves={"src/Legacy.cpp": ("ov001/TU", "src/actors/TU.cpp")},
            extra={"src/actors/TU.cpp": self.MIXED_TWO_FN})

        self.assertEqual(code, 0)
        row = json.loads(exceptions.read_text(encoding="utf-8").strip())
        self.assertEqual(row["path"], "src/Legacy.cpp")


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
