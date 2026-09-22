"""Tests for tools/check_tubuild_conflicts.py.

DELIBERATELY DEPENDENCY-FREE AND TOOLCHAIN-FREE. The thing under test reads two kinds
of text -- `notes` on a manifest entry and `/* TUBUILD CONFLICT */` comments in the
generated source -- and pairs them. It needs no compiler, no ROM, no pyelftools and no
capstone, and neither does this file: everything is a tmpdir and a string.

That is not an accident, it is the point. `tools/test_tubuild.py` cannot run on a
GitHub runner (it is pytest-style bare functions, so `python -m unittest` collects
none of it, and sixteen of its cases open with `if not _toolchain(): return`, which
reports PASS having asserted nothing). A checker whose tests only run on the build box
is a checker nobody notices breaking. These are unittest.TestCase methods with no
guards, so they run in CI -- in tubuild-conflicts.yml, beside the gate they
defend, which is where this tree keeps a gate's own tests -- and they fail
honestly when they fail.

The five correspondence states between a note and a marker:

    open note    + marker     -> agree, no problem
    open note    + no marker  -> STALE NOTE   (reconciled, but the note still claims open)
    RESOLVED     + no marker  -> agree, no problem
    RESOLVED     + marker     -> REOPENED     (the conflict came back)
    no note      + marker     -> UNRECORDED   (an unreviewed arbitrary choice)

plus the two arms that must fail closed rather than pass quietly:

    a conflict warning whose text the check cannot parse  -> UNPARSED NOTE
    a TUBUILD CONFLICT comment the check cannot parse     -> UNPARSED MARKER

Both exist because the pairing is only as good as its regexes: if tubuild's emission
changes and this checker silently stops recognising it, every real conflict becomes
invisible and the check goes green. An unreadable record is a failure.
"""

import contextlib
import io
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import check_tubuild_conflicts as C  # noqa: E402


# --- builders that spell tubuild's emission exactly -----------------------------------
#
# Anchored on tools/tubuild.py: the note at build_manifest_entry (`CONFLICT: {kind_label}
# {key!r} differs between ...`, prefixed `tubuild create warning: `) and the three
# markers in assemble_shadow_source. test_emission_shapes_still_match_tubuild below
# guards these against drift in the real file.

def note(kind, key_repr, first, second, resolved=False):
    return ("tubuild create warning%s: CONFLICT: %s %s differs between the legacy "
            "file that used to hold %s and the one that held %s; kept the first, the "
            "other is commented out for review"
            % (" (RESOLVED)" if resolved else "", kind, key_repr, first, second))


def macro_marker(key, fn, line="#define %s 2"):
    return ("/* TUBUILD CONFLICT -- alternate #define of %s, from the legacy file for "
            "%s, NOT applied: %s */" % (key, fn, line % key))


def extern_marker(key, fn):
    return ("/* TUBUILD CONFLICT -- alternate declaration of %s, from the legacy file "
            "for %s, NOT applied: extern int %s; */" % (key, fn, key))


def decl_marker(kind, dname, fn, body="struct C { int x; };"):
    return ("/* TUBUILD CONFLICT -- alternate body of %s '%s', from the legacy file "
            "for %s, NOT applied:\n%s\n*/" % (kind, dname, fn, body))


class _Tree(unittest.TestCase):
    """A throwaway repo with one source file, and check_entry run against it."""

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self._tmp.name)
        (self.repo / "src").mkdir()
        self.addCleanup(self._tmp.cleanup)
        # The symbol table is cached per repo path. Every test gets a fresh tmpdir so
        # they cannot collide, but a test that writes symbols.txt after a first lookup
        # would read a stale table -- clear it rather than depend on call order.
        C._SITES_CACHE.clear()

    def symbols(self, module, rows):
        """Write config/arm9/.../<module>/symbols.txt with (name, addr) rows."""
        d = self.repo / "config" / "arm9" / "overlays" / module
        d.mkdir(parents=True, exist_ok=True)
        (d / "symbols.txt").write_text(
            "".join("%s kind:function(arm,size=0x4) addr:0x%08x\n" % (n, a)
                    for n, a in rows), encoding="utf-8")

    def check(self, notes, source_text="int f(void) { return 0; }\n",
              write_source=True, source="src/T.cpp"):
        if write_source:
            (self.repo / source).write_text(source_text, encoding="utf-8")
        entry = {"id": "ovXXX/T", "source": source, "notes": list(notes)}
        return C.check_entry(entry, self.repo)

    def labels(self, problems):
        return sorted(label for label, _detail in problems)


class TestCorrespondence(_Tree):

    def test_open_note_with_matching_marker_agrees(self):
        problems, stats = self.check(
            [note("macro", "'FOO'", "func_a", "func_b")],
            macro_marker("FOO", "func_b"))
        self.assertEqual(problems, [])
        self.assertEqual(stats, (1, 1))

    def test_open_note_without_marker_is_stale(self):
        problems, stats = self.check([note("macro", "'FOO'", "func_a", "func_b")])
        self.assertEqual(self.labels(problems), ["STALE NOTE"])
        self.assertEqual(stats, (1, 0))
        self.assertIn("RESOLVED", problems[0][1])   # tells the reader the fix

    def test_resolved_note_without_marker_agrees(self):
        problems, stats = self.check(
            [note("macro", "'FOO'", "func_a", "func_b", resolved=True)])
        self.assertEqual(problems, [])
        self.assertEqual(stats, (0, 0))

    def test_resolved_note_with_marker_is_reopened(self):
        problems, _ = self.check(
            [note("macro", "'FOO'", "func_a", "func_b", resolved=True)],
            macro_marker("FOO", "func_b"))
        self.assertEqual(self.labels(problems), ["REOPENED"])

    def test_marker_without_note_is_unrecorded(self):
        problems, stats = self.check([], macro_marker("FOO", "func_b"))
        self.assertEqual(self.labels(problems), ["UNRECORDED"])
        self.assertEqual(stats, (0, 1))
        self.assertIn("unreviewed", problems[0][1])

    def test_clean_source_and_no_notes_is_clean(self):
        problems, stats = self.check([])
        self.assertEqual(problems, [])
        self.assertEqual(stats, (0, 0))


class TestKinds(_Tree):
    """All three conflict kinds must pair, including the tuple-keyed one."""

    def test_extern_declaration_pairs(self):
        problems, _ = self.check(
            [note("extern declaration", "'gFoo'", "func_a", "func_b")],
            extern_marker("gFoo", "func_b"))
        self.assertEqual(problems, [])

    def test_local_declaration_pairs_through_tuple_key(self):
        # The note side keys local declarations on a (kind, name) TUPLE repr, the
        # marker side spells them `struct 'C'`. Both must normalise to `struct C`.
        problems, _ = self.check(
            [note("local declaration", "('struct', 'C')", "func_a", "func_b")],
            decl_marker("struct", "C", "func_b"))
        self.assertEqual(problems, [])

    def test_all_three_kinds_together_pair(self):
        text = "\n".join([macro_marker("FOO", "func_b"),
                          extern_marker("gFoo", "func_b"),
                          decl_marker("struct", "C", "func_b")])
        problems, stats = self.check(
            [note("macro", "'FOO'", "func_a", "func_b"),
             note("extern declaration", "'gFoo'", "func_a", "func_b"),
             note("local declaration", "('struct', 'C')", "func_a", "func_b")],
            text)
        self.assertEqual(problems, [])
        self.assertEqual(stats, (3, 3))

    def test_same_key_from_a_different_legacy_function_does_not_pair(self):
        # The legacy function is part of the identity. A marker naming func_c is not
        # satisfied by a note about func_b, and pairing them would hide real drift.
        problems, _ = self.check(
            [note("macro", "'FOO'", "func_a", "func_b")],
            macro_marker("FOO", "func_c"))
        self.assertEqual(self.labels(problems), ["STALE NOTE", "UNRECORDED"])


class TestMultiset(_Tree):
    """Pairing is by multiset, not by set -- the counts have to survive."""

    def test_two_notes_one_marker_leaves_one_stale(self):
        n = note("macro", "'FOO'", "func_a", "func_b")
        problems, stats = self.check([n, n], macro_marker("FOO", "func_b"))
        self.assertEqual(self.labels(problems), ["STALE NOTE"])
        self.assertIn("once", problems[0][1])
        self.assertEqual(stats, (2, 1))

    def test_two_markers_one_note_leaves_one_unrecorded(self):
        m = macro_marker("FOO", "func_b")
        problems, stats = self.check([note("macro", "'FOO'", "func_a", "func_b")],
                                     m + "\n" + m)
        self.assertEqual(self.labels(problems), ["UNRECORDED"])
        self.assertIn("once", problems[0][1])
        self.assertEqual(stats, (1, 2))

    def test_three_markers_one_note_reports_the_count(self):
        m = macro_marker("FOO", "func_b")
        problems, _ = self.check([note("macro", "'FOO'", "func_a", "func_b")],
                                 "\n".join([m, m, m]))
        self.assertEqual(self.labels(problems), ["UNRECORDED"])
        self.assertIn("2 times", problems[0][1])

    def test_equal_counts_agree(self):
        n = note("macro", "'FOO'", "func_a", "func_b")
        m = macro_marker("FOO", "func_b")
        problems, stats = self.check([n, n], m + "\n" + m)
        self.assertEqual(problems, [])
        self.assertEqual(stats, (2, 2))


class TestFailsClosed(_Tree):
    """The two arms that must never pass quietly."""

    def test_unreadable_conflict_note_is_reported_not_dropped(self):
        problems, _ = self.check(
            ["tubuild create warning: CONFLICT: macro 'FOO' is different somehow"])
        self.assertEqual(self.labels(problems), ["UNPARSED NOTE"])

    def test_unreadable_marker_is_reported_not_dropped(self):
        problems, _ = self.check([], "/* TUBUILD CONFLICT -- something new here */\n")
        self.assertEqual(self.labels(problems), ["UNPARSED MARKER"])
        self.assertIn("emission has changed", problems[0][1])

    def test_unreadable_marker_is_counted_even_beside_a_readable_one(self):
        text = macro_marker("FOO", "func_b") + "\n/* TUBUILD CONFLICT -- novel */\n"
        problems, _ = self.check([note("macro", "'FOO'", "func_a", "func_b")], text)
        self.assertEqual(self.labels(problems), ["UNPARSED MARKER"])

    def test_notes_that_are_not_conflicts_are_ignored(self):
        # Only text that is recognisably a conflict warning may be flagged unparsed;
        # ordinary prose notes are not the checker's business.
        problems, stats = self.check(["promoted from src_tu on 2026-01-01",
                                      "see PR #1234 for the boundary rationale"])
        self.assertEqual(problems, [])
        self.assertEqual(stats, (0, 0))

    def test_missing_source_with_notes_is_a_problem(self):
        problems, stats = self.check([note("macro", "'FOO'", "func_a", "func_b")],
                                     write_source=False)
        self.assertEqual(self.labels(problems), ["MISSING SOURCE"])
        self.assertEqual(stats, (1, 0))

    def test_missing_source_without_notes_is_not(self):
        problems, stats = self.check([], write_source=False)
        self.assertEqual(problems, [])
        self.assertEqual(stats, (0, 0))


class TestHelpers(unittest.TestCase):

    def test_unrepr_unwraps_quoted_strings(self):
        self.assertEqual(C._unrepr("'FOO'"), "FOO")
        self.assertEqual(C._unrepr('  "FOO"  '), "FOO")

    def test_unrepr_flattens_tuples(self):
        self.assertEqual(C._unrepr("('struct', 'C')"), "struct C")

    def test_unrepr_falls_back_to_raw_text(self):
        # An unparsable key must still compare equal to itself, so both sides of the
        # pairing degrade together rather than one silently becoming something else.
        self.assertEqual(C._unrepr("  not a literal  "), "not a literal")

    def test_times_reads_naturally_at_one(self):
        self.assertEqual(C._times(1), "once")
        self.assertEqual(C._times(2), "2 times")

    def test_parse_markers_counts_unparsed(self):
        keys, unparsed = C.parse_markers(
            macro_marker("FOO", "func_b") + "\n/* TUBUILD CONFLICT -- ? */\n")
        self.assertEqual(keys, [("macro", "FOO", "func_b")])
        self.assertEqual(unparsed, 1)

    def test_parse_notes_marks_resolved(self):
        parsed = C.parse_notes({"notes": [
            note("macro", "'A'", "f", "g"),
            note("macro", "'B'", "f", "g", resolved=True)]})
        self.assertEqual([r for _k, r, _n in parsed], [False, True])


class TestMain(unittest.TestCase):
    """End-to-end through main(), including its exit codes."""

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self._tmp.name)
        (self.repo / "src").mkdir()
        self.md = self.repo / "config" / "tu_manifest.d" / "ov001"
        self.md.mkdir(parents=True)
        self.addCleanup(self._tmp.cleanup)

    def entry(self, notes, text):
        (self.repo / "src" / "T.cpp").write_text(text, encoding="utf-8")
        (self.md / "T.json").write_text(json.dumps(
            {"id": "ov001/T", "source": "src/T.cpp", "notes": notes}), encoding="utf-8")

    def run_main(self, *args):
        # Swallow the report. A test module that prints its fixtures' output into the
        # CI log is how a real failure goes unnoticed three months later.
        self.out = io.StringIO()
        with contextlib.redirect_stdout(self.out), contextlib.redirect_stderr(self.out):
            return C.main(list(args) + ["--repo", str(self.repo)])

    def test_clean_tree_exits_zero(self):
        self.entry([note("macro", "'FOO'", "f", "g")], macro_marker("FOO", "g"))
        self.assertEqual(self.run_main(), 0)

    def test_unreconciled_conflict_exits_one(self):
        self.entry([note("macro", "'FOO'", "f", "g")], "int x;\n")
        self.assertEqual(self.run_main(), 1)
        self.assertIn("STALE NOTE", self.out.getvalue())
        self.assertIn("ov001/T", self.out.getvalue())   # locatable id

    def test_clean_run_says_what_it_checked(self):
        # A silent pass and a pass that reports its input count are not the same
        # gate: the first cannot be told apart from one that stopped seeing its
        # inputs. 97 entries checked is the number that makes the green mean
        # something.
        self.entry([note("macro", "'FOO'", "f", "g")], macro_marker("FOO", "g"))
        self.assertEqual(self.run_main(), 0)
        self.assertIn("checked 1 manifest entries", self.out.getvalue())

    def test_unknown_entry_id_exits_two(self):
        # Fail loudly, not vacuously: a typo'd id must not report "0 problems".
        self.entry([], "int x;\n")
        self.assertEqual(self.run_main("ov001/nope"), 2)

    def test_selecting_a_known_id_still_checks_it(self):
        self.entry([note("macro", "'FOO'", "f", "g")], "int x;\n")
        self.assertEqual(self.run_main("ov001/T"), 1)

    def test_list_flag_does_not_change_the_verdict(self):
        self.entry([note("macro", "'FOO'", "f", "g")], macro_marker("FOO", "g"))
        self.assertEqual(self.run_main("--list"), 0)
        self.assertIn("ov001/T", self.out.getvalue())
        self.assertIn("ov001/T", self.out.getvalue())

    # The three emptiness arms, which are NOT one case. An earlier version of this
    # file asserted that no entries at all was a pass, reasoning that "every TU
    # carrying zero conflicts is the goal". That conflated two different empties.
    # Zero CONFLICT RECORDS across a manifest that loaded and sources that were read
    # really is the goal state, and must stay green or the gate gets switched off the
    # day the campaign succeeds. Zero ENTRIES, or entries whose sources are all
    # absent, means the check never reached its evidence -- a relocated manifest
    # directory, a partial checkout -- and reporting that as a pass is the vacuous
    # green this tree keeps writing gates to avoid.

    def test_no_entries_at_all_exits_two(self):
        self.assertEqual(self.run_main(), 2)
        self.assertIn("checked nothing", self.out.getvalue())

    def test_entries_whose_sources_are_all_absent_exits_two(self):
        # The manifest loads and names a source that is not there. Per entry this is
        # unremarkable (an entry with no notes and no file has nothing to check), so
        # nothing below the top level would notice the whole tree was missing.
        (self.md / "T.json").write_text(json.dumps(
            {"id": "ov001/T", "source": "src/T.cpp", "notes": []}), encoding="utf-8")
        self.assertEqual(self.run_main(), 2)
        self.assertIn("1 manifest entries, 0 of their sources present",
                      self.out.getvalue())

    def test_a_tree_with_no_conflicts_left_is_still_a_pass(self):
        # The goal state: real entries, real sources, and not one conflict record
        # anywhere. This is the arm that stops the fail-closed check above from
        # turning success into a red build.
        self.entry([], "int x;\n")
        self.assertEqual(self.run_main(), 0)
        self.assertIn("1 sources read", self.out.getvalue())


class TestEmissionDrift(unittest.TestCase):
    """The pairing is only as good as its regexes. If tubuild's wording changes and
    nobody updates this checker, every conflict silently stops being recognised --
    UNPARSED MARKER catches that at runtime, but only for trees that still have a
    marker. This catches it at review time instead."""

    def setUp(self):
        self.tubuild = (pathlib.Path(__file__).resolve().parent / "tubuild.py")
        if not self.tubuild.is_file():
            self.skipTest("tools/tubuild.py not present")
        self.text = self.tubuild.read_text(encoding="utf-8", errors="replace")

    def test_emission_shapes_still_match_tubuild(self):
        for fragment in [
                'CONFLICT: {kind_label} {key!r} differs between the legacy ',
                'f"tubuild create warning: {w}"',
                '/* TUBUILD CONFLICT -- alternate body of {kind} {dname!r}, from the ',
                '/* TUBUILD CONFLICT -- alternate #define of {key}, from the legacy ',
                '/* TUBUILD CONFLICT -- alternate declaration of {key}, from the ']:
            with self.subTest(fragment=fragment):
                self.assertIn(fragment, self.text,
                              "tubuild.py no longer emits this shape; "
                              "check_tubuild_conflicts.py must be updated with it")

    def test_checker_regexes_accept_the_documented_text(self):
        self.assertTrue(C.NOTE_RE.match(note("macro", "'FOO'", "f", "g")))
        self.assertTrue(C.MARK_MACRO.search(macro_marker("FOO", "g")))
        self.assertTrue(C.MARK_EXTERN.search(extern_marker("gFoo", "g")))
        self.assertTrue(C.MARK_DECL_BODY.search(decl_marker("struct", "C", "g")))



class TestRenameDesync(_Tree):
    """A note and a marker that are one record, spelled two ways across a rename.

    Renaming a symbol rewrites the legacy function name inside the source marker (it is
    comment text, and a tree-wide substitution reaches it) but not inside the manifest
    note, which is prose. Measured on main: three records in ov006/dScMgCard_c, where
    the note still says func_ov006_020db720 and the marker says
    _ZN11dScMgCard_c13OnTurnIntoEggEi -- the same address, renamed by PR #2087. Left
    unpaired those read as a lost record plus an unreviewed decision. They are neither.
    """

    OLD = "func_ov006_020db720"
    NEW = "_ZN11dScMgCard_c13OnTurnIntoEggEi"
    ADDR = 0x020db720

    def test_rename_pairs_note_and_marker_into_one_finding(self):
        self.symbols("ov006", [(self.NEW, self.ADDR)])
        problems, _stats = self.check(
            [note("extern declaration", "'K'", "fnA", self.OLD)],
            extern_marker("K", self.NEW))
        self.assertEqual(self.labels(problems), ["RENAME DESYNC"])

    def test_rename_pairing_requires_the_same_address(self):
        # Same kind, same key, but the two spellings are different functions. This must
        # NOT collapse -- that would silence a genuinely lost record with a genuinely
        # unreviewed one.
        self.symbols("ov006", [(self.NEW, 0x020dcafe)])
        problems, _stats = self.check(
            [note("extern declaration", "'K'", "fnA", self.OLD)],
            extern_marker("K", self.NEW))
        self.assertEqual(self.labels(problems), ["STALE NOTE", "UNRECORDED"])

    def test_rename_pairing_requires_the_same_module(self):
        self.symbols("ov019", [(self.NEW, self.ADDR)])
        problems, _stats = self.check(
            [note("extern declaration", "'K'", "fnA", self.OLD)],
            extern_marker("K", self.NEW))
        self.assertEqual(self.labels(problems), ["STALE NOTE", "UNRECORDED"])

    def test_no_symbol_table_reports_both_halves_rather_than_guessing(self):
        # With nothing to resolve the new spelling against, the check does not assume a
        # rename. Reporting two problems is the conservative answer, not the tidy one.
        problems, _stats = self.check(
            [note("extern declaration", "'K'", "fnA", self.OLD)],
            extern_marker("K", self.NEW))
        self.assertEqual(self.labels(problems), ["STALE NOTE", "UNRECORDED"])

    def test_a_rename_still_fails_the_check(self):
        # The whole point: this is a more accurate report, not an exemption.
        self.symbols("ov006", [(self.NEW, self.ADDR)])
        problems, _stats = self.check(
            [note("extern declaration", "'K'", "fnA", self.OLD)],
            extern_marker("K", self.NEW))
        self.assertTrue(problems)

    def test_detail_names_the_new_spelling_and_forbids_deleting_the_marker(self):
        self.symbols("ov006", [(self.NEW, self.ADDR)])
        problems, _stats = self.check(
            [note("extern declaration", "'K'", "fnA", self.OLD)],
            extern_marker("K", self.NEW))
        (_label, detail), = problems
        self.assertIn(self.NEW, detail)
        self.assertIn(self.OLD, detail)
        # Deleting the marker is exactly the wrong repair -- the conflict is still open.
        self.assertIn("do NOT delete the marker", detail)

    def test_pairing_is_a_multiset_leaving_the_surplus_note_stale(self):
        self.symbols("ov006", [(self.NEW, self.ADDR)])
        problems, _stats = self.check(
            [note("extern declaration", "'K'", "fnA", self.OLD),
             note("extern declaration", "'K'", "fnB", self.OLD)],
            extern_marker("K", self.NEW))
        self.assertEqual(self.labels(problems), ["RENAME DESYNC", "STALE NOTE"])

    def test_local_declaration_markers_pair_across_a_rename_too(self):
        self.symbols("ov006", [(self.NEW, self.ADDR)])
        problems, _stats = self.check(
            [note("local declaration", "('typedef', 's16')", "fnA", self.OLD)],
            decl_marker("typedef", "s16", self.NEW, "typedef short s16;"))
        self.assertEqual(self.labels(problems), ["RENAME DESYNC"])

    def test_arm9_auto_name_resolves_without_an_overlay_prefix(self):
        # func_XXXXXXXX with no ovNNN belongs to arm9. The directory name is the module,
        # so this pairs only against a symbols.txt under an arm9-named directory.
        d = self.repo / "config" / "arm9"
        d.mkdir(parents=True, exist_ok=True)
        (d / "symbols.txt").write_text(
            "Hud_RenderSprite kind:function(arm,size=0x4) addr:0x020733a8\n",
            encoding="utf-8")
        problems, _stats = self.check(
            [note("extern declaration", "'K'", "fnA", "func_020733a8")],
            extern_marker("K", "Hud_RenderSprite"))
        self.assertEqual(self.labels(problems), ["RENAME DESYNC"])

    def test_a_resolved_note_does_not_pair_as_a_rename(self):
        # RESOLVED + a marker under the new name is a REOPENED-shaped question, and the
        # rename pass must not eat it. It pairs only OPEN notes.
        self.symbols("ov006", [(self.NEW, self.ADDR)])
        problems, _stats = self.check(
            [note("extern declaration", "'K'", "fnA", self.OLD, resolved=True)],
            extern_marker("K", self.NEW))
        self.assertEqual(self.labels(problems), ["UNRECORDED"])


# Last in the file on purpose. A test class defined BELOW this block is
# invisible to `python tools/test_check_tubuild_conflicts.py` -- it is not yet
# defined when main() collects -- while `python -m unittest` still finds it. That
# split cost ten silently uncollected rename tests here, and it is exactly the
# kind of green that means nothing.
if __name__ == "__main__":
    unittest.main()
