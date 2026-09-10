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

    All are orphans by the same predicate. What differs is what a reader should do, and
    what --update does if they do not -- measured, not inferred:

      destination passes    --update exits 0, writes no exception row, and ABSORBS the
                            entry. Rewrite onto it; nothing regressed.
      destination fails     classify_missing() absorbs a moved path only when the
                            destination passes, so this class reaches `removed`:
                            --update exits 2 without a --reason, and with one it records
                            the legacy path as REMOVED, which is false. Rewrite, but do
                            not call it a clean move.
      destination unvouched nothing scored it. Do not rewrite onto a file this tool
                            cannot see.
      no destination        --update exits 2 without a --reason; with one it is the
                            correct command for a file that really was deleted.

    `UpdateBehaviourPin` below pins all of those against the real tool, so a future edit
    to this prose cannot quietly disagree with the code it describes.
    """

    LEGACY = "src/Legacy.cpp"
    DEST = "src/actors/TU.cpp"
    KNOWN = {LEGACY: ("ov001/TU", DEST)}
    SYMBOLS = {LEGACY: "Sym"}

    def _score(self, *failing):
        return {k: k not in failing for k in TR.tiers.CRITERIA}

    def _passing_dest(self):
        return {self.DEST: self._score()}

    def _failing_dest(self):
        return {self.DEST: self._score(TR.tiers.CRITERIA[0])}

    def _report(self, orphans, moves, tracked=None, scores=None, ownership=None,
                symbols=None):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            TR.report_orphans(orphans, moves,
                              "config/converted-backslide-exceptions.jsonl",
                              tracked, scores, ownership,
                              self.SYMBOLS if symbols is None else symbols)
        return out.getvalue()

    def _passing_report(self, orphans=None):
        return self._report(orphans or [self.LEGACY], self.KNOWN, {self.DEST},
                            self._passing_dest())

    def _failing_report(self, orphans=None):
        return self._report(orphans or [self.LEGACY], self.KNOWN, {self.DEST},
                            self._failing_dest())

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
            TR.destination_state(self.DEST, {self.DEST}, self._passing_dest())[0],
            "converted")
        self.assertEqual(
            TR.destination_state(self.DEST, {self.DEST}, self._failing_dest())[0],
            "failing")
        self.assertEqual(TR.destination_state(self.DEST, set(), {})[0], "unvouched")

    def test_a_destination_scored_per_member_is_judged_over_every_member(self):
        ownership = {self.DEST: ["First", "Second"]}
        both = {f"{self.DEST}#First": self._score(),
                f"{self.DEST}#Second": self._score()}
        one_bad = dict(both,
                       **{f"{self.DEST}#Second": self._score(TR.tiers.CRITERIA[1])})

        self.assertEqual(
            TR.destination_state(self.DEST, {self.DEST}, both, ownership)[0],
            "converted")
        state, detail = TR.destination_state(self.DEST, {self.DEST}, one_bad, ownership)
        self.assertEqual(state, "failing")
        self.assertIn(f"{self.DEST}#Second", detail)

    def test_the_three_destination_states_are_reported_in_separate_sections(self):
        text = self._report(
            [self.LEGACY, "src/Other.cpp", "src/Vanished.cpp"],
            {self.LEGACY: ("ov001/TU", self.DEST),
             "src/Other.cpp": ("ov002/TU", "src/actors/Failing.cpp")},
            {self.DEST, "src/actors/Failing.cpp"},
            dict(self._passing_dest(),
                 **{"src/actors/Failing.cpp": self._score(TR.tiers.CRITERIA[0])}))

        self.assertIn("with a destination that PASSES today", text)
        self.assertIn("with a destination that does NOT pass today", text)
        self.assertIn("with no TU promotion destination", text)
        self.assertLess(text.index("PASSES today"), text.index("does NOT pass today"))
        self.assertLess(text.index("does NOT pass today"),
                        text.index("no TU promotion destination"))

    def test_nothing_regressed_is_claimed_only_where_the_destination_passes(self):
        """VFY-2543-10, the real invariant.

        The earlier version of this test asserted only that a destination was NAMED,
        which is the weaker claim the bug lived inside: of the 136 destinations the real
        manifest names, 7 pass and 129 do not, so "a destination exists" licenses
        nothing. A run that printed this sentence over a failing destination
        contradicted its own backslide section twelve lines later.
        """
        self.assertIn("Nothing regressed", self._passing_report())
        self.assertNotIn("Nothing regressed", self._failing_report())
        self.assertNotIn("Nothing regressed",
                         self._report([self.LEGACY], self.KNOWN, set(), {}))
        self.assertNotIn("Nothing regressed", self._report(["src/Vanished.cpp"], {}))

    def test_a_failing_destination_names_the_criterion_it_fails(self):
        text = self._failing_report()

        self.assertIn(TR.tiers.CRITERION_LABEL[TR.tiers.CRITERIA[0]], text)

    def test_a_failing_destination_is_still_told_to_rewrite(self):
        """The rewrite is right in every state; only the reassurance is not."""
        text = self._failing_report()

        self.assertIn("REWRITE these", text)
        self.assertIn(f"      rewrite onto {self.DEST}", text)

    def test_a_failing_destination_is_not_handed_the_removal_command(self):
        """--update does not absorb this class, but re-banking is still not the repair:
        the row it writes says the legacy path was REMOVED and nothing was."""
        text = self._failing_report()

        self.assertNotIn('python tools/tiers_ratchet.py --update --reason', text)
        self.assertIn("recording the legacy path as REMOVED", text)

    def test_an_unvouched_destination_is_not_rewritten_onto(self):
        text = self._report([self.LEGACY], self.KNOWN, set(), {})

        self.assertIn("cannot vouch for", text)
        self.assertIn("Do not rewrite", text)
        self.assertNotIn(f"rewrite onto {self.DEST}", text)

    def test_the_rewrite_target_follows_the_destination_member_count(self):
        """VFY-2543-13: tu_promote.py writes a bare dest for a single-function TU and
        `dest#symbol` otherwise, and scan() scores it the same way. 0 of the 136 real
        destinations take the first branch today, which is why a constant went
        unnoticed."""
        self.assertEqual(TR.rewrite_target(self.LEGACY, self.DEST, {}, self.SYMBOLS),
                         self.DEST)
        self.assertEqual(
            TR.rewrite_target(self.LEGACY, self.DEST,
                              {self.DEST: ["First", "Second"]}, self.SYMBOLS),
            f"{self.DEST}#Sym")

    def test_the_rewrite_target_carries_the_identitys_own_symbol_when_it_has_one(self):
        self.assertEqual(
            TR.rewrite_target(f"{self.LEGACY}#Other", self.DEST,
                              {self.DEST: ["First", "Second"]}, self.SYMBOLS),
            f"{self.DEST}#Other")

    def test_an_unknown_symbol_is_not_invented(self):
        self.assertIsNone(TR.rewrite_target(self.LEGACY, self.DEST,
                                            {self.DEST: ["First", "Second"]}, {}))
        text = self._report([self.LEGACY], self.KNOWN, {self.DEST},
                            {f"{self.DEST}#First": self._score(),
                             f"{self.DEST}#Second": self._score()},
                            {self.DEST: ["First", "Second"]}, {})
        self.assertIn("<the symbol the manifest enrolls it under>", text)

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
        for text in (self._passing_report(), self._failing_report(),
                     self._report([self.LEGACY], self.KNOWN, set(), {}),
                     self._report(["src/Vanished.cpp"], {})):
            self.assertNotIn("count` downward", text)
            self.assertNotIn("reports as a regression", text)

    def test_a_passing_destination_orphan_is_not_told_update_will_refuse(self):
        """VFY-2543-01: it does not refuse for this class -- it exits 0 and absorbs."""
        text = self._passing_report()

        self.assertNotIn("demand a", text)
        self.assertIn("does not refuse -- it ABSORBS", text)
        self.assertIn("Do NOT re-bank these with --update.", text)
        self.assertNotIn("--update --reason", text)

    def test_the_open_update_hole_is_acknowledged_not_asserted_away(self):
        """VFY-2543-06 is a real, unclosed hole: one --update launders every orphan of
        the passing-destination class and the result is green. The diagnostic must not
        imply otherwise."""
        self.assertIn("hole is still open", self._passing_report())

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


class SyntheticTree:
    """Build a throwaway src/ tree and a baseline, and run main() against them.

    A mixin, not a TestCase: `UpdateBehaviourPin` needs the same fixture but must not
    inherit and re-run every --check test along with it.
    """

    TWO_FN = "//cpp\nvoid First() {}\nvoid Second() {}\n"
    FAILING_TWO_FN = ("//cpp\nint unk_18;\n"
                      "void First() { unk_18 = 1; }\nvoid Second() {}\n")

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
            extra={dest: self.TWO_FN})

        self.assertEqual(code, 1)
        self.assertIn("REWRITE these", text)
        self.assertIn(dest, text)
        self.assertNotIn("CONVERTED ratchet PASS", text)

    def test_the_plain_report_names_orphans_without_an_exit_code(self):
        code, text = self._run({"src/Kept.cpp", "src/Gone.cpp"}, ["src/Kept.cpp"],
                               {"src/Kept.cpp": ["Kept"]}, [])

        self.assertEqual(code, 0)
        self.assertIn("ORPHANED", text)
        self.assertIn("src/Gone.cpp", text)
        self.assertIn("<- --check would fail", text)
        # the same four-way split --check prints, through the same function
        self.assertIn("dest passes", text)
        self.assertIn("dest fails", text)
        self.assertIn("dest unvouched", text)
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

    def test_an_orphan_whose_destination_fails_is_not_told_nothing_regressed(self):
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
        self.assertIn("does NOT pass today", text)
        self.assertNotIn("Nothing regressed", text)
        # the two halves of the run now agree with each other
        self.assertIn("CONVERTED backslide", text)
        self.assertIn(f"{dest}#First", text)

    def test_a_failing_destination_orphan_is_still_told_where_to_rewrite(self):
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
