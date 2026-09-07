"""Credit attribution, tested against real git history.

There was no coverage here before, which is a large part of why the bug this file was
written for survived: `match_finishers` carried a file's draft history across a rename but
not its finisher, so every moved file read as a fresh finish by whoever moved it. On the
real repository that had already re-pointed 94 files' credit away from the people who
matched them.

These build throwaway git repositories rather than mocking, because every one of the
failure modes is about what `git log -M` decides a commit did -- rename vs delete+add,
above or below the similarity threshold -- and a mock would just encode the assumption
under test.
"""
import json
import contextlib
import io
import pathlib
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import chaos_db_ci as CDB  # noqa: E402

DRAFT = "// NONMATCHING: not yet matched\nint f(void){return 0;}\n"
CLEAN = "int f(void){return 0;}\n"


class GitFixture(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        self.git("init", "-q", "-b", "main")
        self.git("config", "user.email", "setup@example.com")
        self.git("config", "user.name", "Setup")
        (self.repo / "src").mkdir()
        self._saved = CDB.REPO
        CDB.REPO = self.repo

    def tearDown(self):
        CDB.REPO = self._saved
        self.tmp.cleanup()

    def git(self, *args):
        return subprocess.run(["git", *args], cwd=self.repo, check=True,
                              capture_output=True, text=True).stdout

    def commit(self, who, msg):
        self.git("add", "-A")
        self.git("-c", f"user.email={who}@example.com", "-c", f"user.name={who}",
                 "commit", "-q", "--author", f"{who} <{who}@example.com>", "-m", msg)

    def write(self, rel, text):
        p = self.repo / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8")

    def move(self, old, new):
        (self.repo / new).parent.mkdir(parents=True, exist_ok=True)
        self.git("mv", old, new)


class Finishers(GitFixture):
    def test_finisher_is_the_person_who_removed_the_banner(self):
        self.write("src/f.c", DRAFT)
        self.commit("drafter", "draft")
        self.write("src/f.c", CLEAN)
        self.commit("matcher", "match it")
        self.assertEqual(CDB.match_finishers("HEAD"), {"src/f.c": "matcher"})

    def test_a_pure_move_does_not_steal_the_finish(self):
        """The bug. A relocation must not make the mover the finisher."""
        self.write("src/f.c", DRAFT)
        self.commit("drafter", "draft")
        self.write("src/f.c", CLEAN)
        self.commit("matcher", "match it")
        self.move("src/f.c", "src/unnamed/ov006/f.c")
        self.commit("mover", "layout: relocate")
        self.assertEqual(CDB.match_finishers("HEAD"),
                         {"src/unnamed/ov006/f.c": "matcher"})

    def test_the_old_path_does_not_linger_after_a_move(self):
        self.write("src/f.c", DRAFT)
        self.commit("drafter", "draft")
        self.write("src/f.c", CLEAN)
        self.commit("matcher", "match it")
        self.move("src/f.c", "src/unnamed/ov006/f.c")
        self.commit("mover", "layout: relocate")
        self.assertNotIn("src/f.c", CDB.match_finishers("HEAD"))

    def test_a_move_of_a_still_drafted_file_keeps_it_drafted(self):
        """Moving an unfinished draft must not finish it, and whoever removes the banner
        later still gets the credit."""
        self.write("src/f.c", DRAFT)
        self.commit("drafter", "draft")
        self.move("src/f.c", "src/unnamed/ov006/f.c")
        self.commit("mover", "layout: relocate")
        self.assertEqual(CDB.match_finishers("HEAD"), {})
        self.write("src/unnamed/ov006/f.c", CLEAN)
        self.commit("matcher", "match it")
        self.assertEqual(CDB.match_finishers("HEAD"),
                         {"src/unnamed/ov006/f.c": "matcher"})

    def test_extension_change_recorded_as_delete_plus_add_keeps_the_finish(self):
        self.write("src/f.c", DRAFT)
        self.commit("drafter", "draft")
        self.write("src/f.c", CLEAN)
        self.commit("matcher", "match it")
        (self.repo / "src" / "f.c").unlink()
        self.write("src/f.cpp", "//cpp\n" + CLEAN + "// rewritten enough to defeat -M\n" * 40)
        self.commit("promoter", "promote to cpp")
        self.assertEqual(CDB.match_finishers("HEAD").get("src/f.cpp"), "matcher")

    def test_a_later_touch_never_transfers_the_finish(self):
        self.write("src/f.c", DRAFT)
        self.commit("drafter", "draft")
        self.write("src/f.c", CLEAN)
        self.commit("matcher", "match it")
        self.write("src/f.c", CLEAN + "// tidy\n")
        self.commit("tidier", "tidy up")
        self.assertEqual(CDB.match_finishers("HEAD"), {"src/f.c": "matcher"})

    def test_a_file_that_was_never_drafted_has_no_finisher(self):
        self.write("src/f.c", CLEAN)
        self.commit("matcher", "match it")
        self.assertEqual(CDB.match_finishers("HEAD"), {})

    def test_a_short_clean_blob_is_not_contaminated_by_its_neighbour(self):
        """Blobs are classified from one concatenated `git cat-file --batch` response, so a
        fixed-size read past the end of a short blob lands in the NEXT blob's bytes. A clean
        file next to a drafted one was read as drafted and lost its finisher.

        The arrangement is deliberate, not incidental. Blobs are queried in sorted
        (commit-sha, path) order, and commit shas vary run to run -- so a draft-commit /
        clean-commit pair only puts a clean blob before a draft one about half the time,
        which is exactly the coin-flip that made this bug look like flaky tests. Putting
        BOTH in one commit pins the order to the paths: `a_clean` sorts before `b_draft`,
        the two are adjacent in the batch, and the read runs off the short clean blob into
        the banner every single time."""
        self.write("src/a_clean.c", DRAFT)
        self.write("src/b_draft.c", DRAFT)
        self.commit("drafter", "draft both")
        self.write("src/a_clean.c", CLEAN)              # finished, and only 23 bytes
        self.write("src/b_draft.c", DRAFT + "// still working\n")   # stays drafted
        self.commit("matcher", "match the first one")
        got = CDB.match_finishers("HEAD")
        self.assertEqual(got, {"src/a_clean.c": "matcher"},
                         "the short clean blob was read as drafted -- window bled into "
                         "the next blob")

    def test_classification_is_deterministic_across_repeated_calls(self):
        self.write("src/a_clean.c", DRAFT)
        self.write("src/b_draft.c", DRAFT)
        self.commit("drafter", "draft both")
        self.write("src/a_clean.c", CLEAN)
        self.write("src/b_draft.c", DRAFT + "// still working\n")
        self.commit("matcher", "match the first one")
        runs = [CDB.match_finishers("HEAD") for _ in range(3)]
        self.assertEqual(runs[0], runs[1])
        self.assertEqual(runs[1], runs[2])


class FirstMatchers(GitFixture):
    def test_a_move_carries_the_first_matcher(self):
        self.write("src/f.c", CLEAN)
        self.commit("matcher", "match it")
        self.move("src/f.c", "src/unnamed/ov006/f.c")
        self.commit("mover", "layout: relocate")
        self.assertEqual(CDB.first_matchers("HEAD"),
                         {"src/unnamed/ov006/f.c": "matcher"})

    def test_a_bulk_move_carries_credit_for_every_file(self):
        """Large relocations are exactly where git's rename cap used to bite."""
        for i in range(60):
            self.write(f"src/func_ov006_{i:08x}.c", f"int f{i}(void){{return {i};}}\n")
        self.commit("matcher", "match a batch")
        for i in range(60):
            self.move(f"src/func_ov006_{i:08x}.c",
                      f"src/unnamed/ov006/func_ov006_{i:08x}.c")
        self.commit("mover", "layout: relocate")
        got = CDB.first_matchers("HEAD")
        self.assertEqual(len(got), 60)
        self.assertEqual(set(got.values()), {"matcher"})


class Composite(GitFixture):
    """What the merge gate actually compares: overrides -> finishers -> first."""

    def resolve(self, rev="HEAD"):
        first = CDB.first_matchers(rev)
        fin = CDB.match_finishers(rev)
        return {p: (fin.get(p) or first.get(p)) for p in set(first) | set(fin)}

    def test_composite_survives_a_move_of_a_finished_draft(self):
        self.write("src/f.c", DRAFT)
        self.commit("drafter", "draft")
        self.write("src/f.c", CLEAN)
        self.commit("matcher", "match it")
        before = self.resolve()
        self.move("src/f.c", "src/unnamed/ov006/f.c")
        self.commit("mover", "layout: relocate")
        after = self.resolve()
        self.assertEqual(list(before.values()), ["matcher"])
        self.assertEqual(list(after.values()), ["matcher"])


if __name__ == "__main__":
    unittest.main()


class RenameReplay(GitFixture):
    """`prepush_attribution.project` must REPLAY renames in commit order.

    The bug these cover: the steps used to be composed into one {old: final} map and
    chased forward, which is right for a file that really moved A -> B -> C and wrong
    for a permutation, where step 2 only means what it means because step 1 already
    vacated the name.
    """

    def setUp(self):
        super().setUp()
        sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
        import prepush_attribution as PA
        self.PA = PA
        self._saved_pa = PA.REPO
        PA.REPO = self.repo

    def tearDown(self):
        self.PA.REPO = self._saved_pa
        super().tearDown()

    def steps(self):
        return self.PA.renames_between("main", "HEAD")

    def test_a_two_class_swap_keeps_every_credit(self):
        """A -> B while B -> C. Composed, this said A's credit landed on C."""
        self.write("src/b.c", CLEAN)
        self.write("src/a.c", CLEAN + "// a\n")
        self.commit("author", "start")
        self.git("branch", "base")
        self.move("src/b.c", "src/c.c")
        self.commit("renamer", "b -> c")
        self.move("src/a.c", "src/b.c")
        self.commit("renamer", "a -> b")

        steps = self.PA.renames_between("base", "HEAD")
        self.assertEqual(steps, [("b", "c"), ("a", "b")])

        before = {"a": ("src/a", "alice"), "b": ("src/b", "bob")}
        state, origin = self.PA.project(before, steps)
        # bob's work is now called c, alice's is now called b, and neither lost credit
        self.assertEqual(state["c"], ("src/b", "bob"))
        self.assertEqual(state["b"], ("src/a", "alice"))
        self.assertNotIn("a", state)
        self.assertEqual(origin["c"], "b")
        self.assertEqual(origin["b"], "a")

    def test_composing_would_have_been_wrong(self):
        """Pin the exact failure: forward-chasing sends a's credit to c."""
        steps = [("b", "c"), ("a", "b")]
        composed = {}
        for old, new in steps:
            composed[old] = new
        for start in list(composed):
            seen, cur = {start}, composed[start]
            while cur in composed and cur not in seen:
                seen.add(cur)
                cur = composed[cur]
            composed[start] = cur
        self.assertEqual(composed["a"], "c")          # the bug, preserved as a fact
        state, _ = self.PA.project({"a": ("src/a", "alice"), "b": ("src/b", "bob")}, steps)
        self.assertEqual(state["c"], ("src/b", "bob"))   # replay disagrees, and is right

    def test_a_genuine_chain_still_composes_correctly(self):
        """A -> B -> C in that order really is one file moving twice."""
        steps = [("a", "b"), ("b", "c")]
        state, origin = self.PA.project({"a": ("src/a", "alice")}, steps)
        self.assertEqual(state["c"], ("src/a", "alice"))
        self.assertEqual(origin["c"], "a")
        self.assertNotIn("a", state)
        self.assertNotIn("b", state)

    def test_a_rewrite_and_move_in_one_commit_is_still_a_loss(self):
        """The gate keeps its teeth: no R record, so no step, so the name is lost."""
        self.write("src/a.c", CLEAN)
        self.commit("author", "start")
        self.git("branch", "base2")
        (self.repo / "src/a.c").unlink()
        self.write("src/z.c", "int totally_different(void){return 42;}\n")
        self.commit("renamer", "rewrite and move at once")
        steps = self.PA.renames_between("base2", "HEAD")
        state, _ = self.PA.project({"a": ("src/a", "alice")}, steps)
        self.assertIn("a", state)          # never vacated -> reported lost
        self.assertNotIn("z", state)

    def test_member_overrides_preserve_credit_across_tu_consolidation(self):
        self.write("src/a.c", "int a(void){return 1;}\n")
        self.write("src/b.c", "int b(void){return 2;}\n")
        self.commit("alice", "match legacy members")
        self.git("branch", "pre_tu")

        (self.repo / "src/a.c").unlink()
        (self.repo / "src/b.c").unlink()
        self.write("src/Pair.cpp", "//cpp\nint a(){return 1;}\nint b(){return 2;}\n")
        self.write("attribution.json", json.dumps({"overrides": {
            "src/Pair.cpp#a": "alice",
            "src/Pair.cpp#b": "alice",
        }}))
        self.commit("promoter", "consolidate original TU")

        self.assertEqual(self.PA.member_overrides_at("HEAD"), {
            "a": ("src/Pair.cpp", "alice"),
            "b": ("src/Pair.cpp", "alice"),
        })
        out = io.StringIO()
        with mock.patch.object(sys, "argv", [
                "prepush_attribution.py", "--base", "pre_tu", "--head", "HEAD"]), \
                contextlib.redirect_stdout(out):
            self.assertEqual(self.PA.main(), 0)
        self.assertIn("2 consolidated with credit intact", out.getvalue())
