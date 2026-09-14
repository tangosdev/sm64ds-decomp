"""Regression tests for tools/check_header_offsets.py's WORK-LIST RESOLUTION.

The parser has been debugged into shape by a long series of "this reported a pass and
checked nothing" fixes, all written up in the module's own comments. This file covers
the layer above it: which files the gate decides to look at in the first place. That
layer had two holes, and a gate that looks at the wrong (empty) set of files reports a
pass exactly as convincingly as one that looks at the right set and finds nothing wrong.

Every test here is a PLANTED REGRESSION: `_resolve_the_old_way` below is the pre-fix
resolution, verbatim, and each case asserts that it says "pass" while the current code
says "fail". A test that only exercised the new code would not prove a hole was closed.
"""
import contextlib
import io
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
import unittest.mock

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import check_header_offsets as C  # noqa: E402

# A header the tool can actually parse: struct name == file stem, one commented field
# per declaration. `u32` is in the built-in SZ table, so this needs nothing from the
# real include/ tree.
GOOD = """\
struct Widget {
\tu32 first;   /* 0x000 */
\tu32 second;  /* 0x004 */
\tu16 third;   /* 0x008 */
};
"""
# `second` claims 0x008; two u32s put it at 0x004. This is the #1583 shape in
# miniature -- a field whose comment and whose computed offset have come apart.
BROKEN = """\
struct Widget {
\tu32 first;   /* 0x000 */
\tu32 second;  /* 0x008 */
\tu16 third;   /* 0x00c */
};
"""


def _resolve_the_old_way(base, repo):
    """The resolution as it stood before this change. Returns (headers, exit_code).

    Copied from the pre-fix `_resolve_paths`, which is what makes the assertions below
    proofs rather than assertions about the new code's opinion of itself:

        proc = subprocess.run(
            ["git", "diff", "--name-only", "--diff-filter=AM", f"{base}...HEAD",
             "--", "include/"], cwd=REPO, ...)
        argv = [p for p in proc.stdout.split() if p.endswith((".h", ".hpp"))]
        if not argv:
            print("no include/ header added or modified")
            sys.exit(0)                     # <-- both defects live on this line
    """
    proc = subprocess.run(
        ["git", "diff", "--name-only", "--diff-filter=AM", f"{base}...HEAD",
         "--", "include/"],
        cwd=str(repo), capture_output=True, text=True)
    if proc.returncode != 0:
        return [], 1
    heads = [p for p in proc.stdout.split() if p.endswith((".h", ".hpp"))]
    return heads, (0 if not heads else None)


class Repo:
    """A throwaway git repository. Real git, so the resolution is exercised for real."""

    def __enter__(self):
        self.dir = pathlib.Path(tempfile.mkdtemp(prefix="check_header_offsets_"))
        self.git("init", "-q", "-b", "main")
        self.git("config", "user.email", "gate@example.invalid")
        self.git("config", "user.name", "gate")
        self.git("config", "commit.gpgsign", "false")
        (self.dir / "include").mkdir()
        self.write("README", "base\n")
        self.write("include/Widget.h", GOOD)
        self.commit("base")
        self.base = self.rev("HEAD")
        return self

    def __exit__(self, *exc):
        shutil.rmtree(self.dir, ignore_errors=True)

    def git(self, *args):
        r = subprocess.run(["git", *args], cwd=str(self.dir),
                           capture_output=True, text=True)
        assert r.returncode == 0, f"git {' '.join(args)}: {r.stderr}"
        return r.stdout

    def rev(self, ref):
        return self.git("rev-parse", ref).strip()

    def write(self, rel, body):
        p = self.dir / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(body, encoding="utf-8")
        return self

    def commit(self, msg):
        self.git("add", "-A")
        self.git("commit", "-q", "-m", msg)
        return self

    def resolve(self, *argv):
        return C._resolve_paths(list(argv), self.dir)

    def run(self, *argv):
        return C.main(list(argv), self.dir)

    def old(self, base=None):
        return _resolve_the_old_way(base or self.base, self.dir)


# ------------------------------------------------------- defect 1: committed-only diff

class UncommittedChangesTests(unittest.TestCase):
    """`{base}...HEAD` is the commits. A header edited and not yet committed was not in
    it, so a local pre-commit run reported a pass over work it had never opened."""

    def test_an_uncommitted_broken_header_was_invisible_and_is_now_caught(self):
        """The header is already in the BASE, so no commit in `base...HEAD` names
        it and the old resolution had nothing to hand the walk. That is exactly the
        pre-commit run this gate is for, and it reported a pass over an unread file."""
        with Repo() as r:
            r.write("tools/x.py", "x = 1\n").commit("unrelated work")
            r.write("include/Widget.h", BROKEN)          # edited, NOT committed

            old_heads, old_code = r.old()
            self.assertEqual(old_heads, [], "precondition: the old resolution saw nothing")
            self.assertEqual(old_code, 0, "precondition: and called that a pass")

            paths, code = r.resolve("--changed", r.base)
            self.assertEqual(paths, ["include/Widget.h"])
            self.assertEqual(r.run("--changed", r.base), 1)

    def test_a_staged_but_uncommitted_broken_header_is_caught(self):
        with Repo() as r:
            r.write("tools/x.py", "x = 1\n").commit("unrelated work")
            r.write("include/Widget.h", BROKEN)
            r.git("add", "include/Widget.h")             # staged, still not committed
            self.assertEqual(r.old()[1], 0)
            self.assertEqual(r.run("--changed", r.base), 1)

    def test_a_brand_new_untracked_header_is_caught(self):
        """Neither diff sees an untracked file, and a brand-new header is the one
        whose offsets have never been checked by anything at all."""
        with Repo() as r:
            r.write("include/Fresh.h", BROKEN.replace("Widget", "Fresh"))
            self.assertEqual(r.old()[1], 0)
            paths, _ = r.resolve("--changed", r.base)
            self.assertEqual(paths, ["include/Fresh.h"])
            self.assertEqual(r.run("--changed", r.base), 1)

    def test_the_old_code_did_see_a_dirty_header_its_own_branch_committed(self):
        """Bounding the claim, so nobody widens it later: `base...HEAD` names that
        path and the walk reads the file from DISK, so the dirty bytes were checked.
        The hole is only for headers no commit in the range names -- which is every
        header on a branch that has not committed yet, and every untracked one."""
        with Repo() as r:
            r.write("include/Other.h", GOOD.replace("Widget", "Other"))
            r.commit("add another header")
            r.write("include/Other.h", BROKEN.replace("Widget", "Other"))
            self.assertEqual(r.old()[0], ["include/Other.h"])
            self.assertEqual(r.run("--changed", r.base), 1)

    def test_a_good_uncommitted_header_still_passes(self):
        """Not weakening the gate: folding the working tree in must not invent failures."""
        with Repo() as r:
            r.write("include/Widget.h", GOOD + "\n")   # touched, still correct
            self.assertEqual(r.run("--changed", r.base), 0)

    def test_committed_only_reproduces_ci_but_names_what_it_is_ignoring(self):
        with Repo() as r:
            r.write("tools/x.py", "x = 1\n").commit("a non-header commit")
            r.write("include/Widget.h", BROKEN)          # uncommitted
            paths, code = r.resolve("--changed", r.base, "--committed-only")
            self.assertIsNone(paths)
            self.assertEqual(code, 0, "the commits genuinely contain no header")
            # ...but the uncommitted one is reported, not silently dropped.
            buckets, err = C.changed_paths(r.base, committed_only=True, repo=r.dir)
            self.assertIsNone(err)
            self.assertEqual(buckets["worktree"], ["include/Widget.h"])

    def test_committed_only_outside_changed_is_refused(self):
        with Repo() as r:
            paths, code = r.resolve("--committed-only", "include/Widget.h")
            self.assertIsNone(paths)
            self.assertEqual(code, 1)


# ------------------------------------------------------- defect 2: the empty work list

class EmptyWorkListTests(unittest.TestCase):
    """The docstring already argued "an empty check is not a pass"; the guard only
    covered the zero-argv case, not the empty-diff case."""

    def test_a_diff_that_names_nothing_at_all_is_not_a_pass(self):
        """base == HEAD with a clean tree. On a pull request this cannot happen, so it
        is a base ref that resolved to the wrong commit -- an unfetched origin/main, a
        shallow clone with no merge base. The old code exited 0."""
        with Repo() as r:
            self.assertEqual(r.old()[1], 0, "precondition: the old code passed")
            paths, code = r.resolve("--changed", r.base)
            self.assertIsNone(paths)
            self.assertEqual(code, 1)

    def test_include_changed_but_the_extension_filter_ate_the_work_list(self):
        """The .h/.hpp filter exists to skip include/'s non-headers. When it discards
        EVERY changed include/ path it is no longer skipping noise, it is skipping the
        job -- and the old code could not tell the two apart."""
        with Repo() as r:
            r.write("include/Widget.inc", "u32 first;\n").commit("a non-.h header")
            self.assertEqual(r.old()[1], 0, "precondition: the old code passed")
            paths, code = r.resolve("--changed", r.base)
            self.assertIsNone(paths)
            self.assertEqual(code, 1)

    def test_a_change_that_genuinely_touches_no_header_passes_with_its_evidence(self):
        """The one honest empty, and the reason "empty always fails" would be wrong:
        most pull requests touch no header. What makes it not a hollow pass is that the
        count of files the diff DID contain is computed and printed."""
        with Repo() as r:
            r.write("tools/x.py", "x = 1\n")
            r.write("src/y.c", "int y;\n").commit("no headers")
            buckets, err = C.changed_paths(r.base, repo=r.dir)
            self.assertIsNone(err)
            self.assertEqual(len(buckets["total"]), 2)
            self.assertEqual(buckets["include"], [])
            paths, code = r.resolve("--changed", r.base)
            self.assertIsNone(paths)
            self.assertEqual(code, 0)

    def test_a_non_header_include_path_alongside_a_real_one_is_only_noise(self):
        with Repo() as r:
            r.write("include/Other.h", GOOD.replace("Widget", "Other"))
            r.write("include/notes.txt", "prose\n").commit("both")
            paths, code = r.resolve("--changed", r.base)
            self.assertEqual(paths, ["include/Other.h"])
            self.assertEqual(code, 0)

    def test_headers_deleted_since_the_commit_do_not_shrink_to_a_pass(self):
        with Repo() as r:
            r.write("include/Other.h", GOOD.replace("Widget", "Other")).commit("add")
            (r.dir / "include" / "Other.h").unlink()
            paths, code = r.resolve("--changed", r.base)
            self.assertIsNone(paths)
            self.assertEqual(code, 1)

    def test_no_arguments_is_still_refused(self):
        paths, code = C._resolve_paths([])
        self.assertIsNone(paths)
        self.assertEqual(code, 1)

    def test_an_unresolvable_base_is_an_error_not_a_pass(self):
        with Repo() as r:
            paths, code = r.resolve("--changed", "no/such/ref")
            self.assertIsNone(paths)
            self.assertEqual(code, 1)


# --------------------------------------------------------- the gate itself still works

class GateStillWorksTests(unittest.TestCase):
    def test_a_broken_header_named_explicitly_fails(self):
        with Repo() as r:
            r.write("include/Widget.h", BROKEN)
            self.assertEqual(r.run("include/Widget.h"), 1)

    def test_a_good_header_named_explicitly_passes(self):
        with Repo() as r:
            r.write("include/Widget.h", GOOD)
            self.assertEqual(r.run("include/Widget.h"), 0)

    def test_a_real_tree_header_still_parses(self):
        """Guards the repo-root resolution added here: a relative path from --changed
        must be read against the repository, not the process cwd.

        The header is `include/dActor_c.h` because the cartridge's own RTTI spells
        that class `8dActor_c` -- it is not a coined name awaiting a rename, so the
        path cannot go dead under check_dead_references. And rc == 0 alone would be
        satisfied by a header the parser SKIPPED, which is the failure mode this
        whole file exists to catch, so assert the gate did real work as well.
        """
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            rc = C.main(["include/dActor_c.h"])
        out = buf.getvalue()
        self.assertEqual(rc, 0, out)
        self.assertRegex(out, r"(\d+) commented fields, 0 mismatched")
        self.assertGreater(int(re.search(r"(\d+) commented fields", out).group(1)), 0,
                           "a header the parser checked zero fields in proves nothing")

    def test_the_cli_entry_point_still_returns_the_gates_verdict(self):
        r = subprocess.run([sys.executable, str(TOOLS / "check_header_offsets.py")],
                           capture_output=True, text=True, cwd=REPO)
        self.assertEqual(r.returncode, 1, r.stdout + r.stderr)
        self.assertIn("not a pass", r.stdout + r.stderr)


# ------------------------------------------ the shapes real pull requests have, rebuilt
#
# This section used to pin four 40-hex commit shas -- #1659, #1665, #1666 and #1667, all
# landed on 2026-08-21 -- and diff `<sha>^...<sha>` against THIS repository. It was the
# one part of this file that reached outside a tmpdir, on the argument that a fixture
# cannot show the resolution still works on history the tree really has.
#
# NOT a force-reset -- an identity SCRUB. At 07:50 EDT on 2026-09-13 the whole history
# was rewritten (see tools/repin_commit_ids.py's docstring): the trees are identical, but
# every commit got a new id, so all four pins stopped resolving. They survive in any clone
# that was already holding the loose objects -- which is every developer's, so the suite
# stayed green locally -- while `actions/checkout` fetches only reachable history and
# cannot see them at all. On a runner `_require` failed five times, and the `offsets`
# check went red on #2487, #2490, `match/f100-shapes` and `match/w4-exctab2`, none of
# which had touched a header. A local green a runner cannot reproduce is the exact shape
# this whole file exists to refuse. dc3dbbdfa re-pinned all four to their post-scrub ids
# and main is green on a runner again; what follows is about the NEXT rewrite, not that
# one.
#
# THIS IS NOT A RECURRENCE ARGUMENT, IT IS AN EXPOSURE ONE. Rewrites are rare: this
# clone's `git reflog show origin/main` covers 872 fetches over 31 days with exactly one
# forced-update, the 2026-09-13 scrub. The reason to stop pinning anyway is that the
# material that triggered it is still here. The scrub was over a personal Windows
# username in a hardcoded path -- tools/ovsweep.py:48, which now reads
# `C:/Users/tango/...` -- and origin/main @ 1224afd11 still spells THIRTY-ONE paths of
# that shape across six tracked files, sixteen under one username and fourteen under
# another:
#
#     git grep -ohE 'C:[\\/]{1,2}Users[\\/]{1,2}(andre|alexs|tango)' origin/main -- .
#
#     25  config/match_attempts.jsonl        1  notes/tu-cpp-census-2026-08.md
#      2  notes/real-cpp-migration-runbook.md    1  notes/agents/LAUNCH.md
#      1  notes/agents/references/pipeline-v1.md 1  tools/ovsweep.py
#
# Twenty-five of the thirty-one are recorded "srcPath" values in
# config/match_attempts.jsonl -- a generated log that keeps growing -- so the exposure is
# not a handful of stale notes, and it is not static. Mind the separator when re-checking:
# JSON escapes the path, so those 25 read `C:\\Users\\...` and a pattern matching a single
# separator misses every one of them. The maintainer designed for the repeat: repin's
# `--check` is documented as "the same predicate a pre-merge gate would use", dc3dbbdfa
# swapped a real corpus id in tools/test_repin_commit_ids.py for a synthetic
# `a1a1a1a1...` "so the test stays hermetic and idempotent under a future re-sweep", and
# the treadmill already needed a second pass 98 minutes later (b9ba3a59b). This file is
# the one repin names as more than stale prose, because a pin that no longer resolves
# fails CI outright rather than merely reading wrong. Applying the same synthetic-fixture
# treatment here finishes that job. The reconstruction below is built by real `git` in a
# tmpdir, so these tests assert the same things and depend on no repository history
# whatsoever.
#
# WHAT IS RECONSTRUCTED AND WHAT IS NOT. The counts are not invented. Each was measured
# with `git diff --name-status <sha>^...<sha>` while the objects were still readable, and
# the fixture reproduces the per-directory, per-extension file counts, the added/modified
# split, and the topology main actually has: one SQUASHED commit per pull request, single
# parent -- none of the four was a merge commit, which the old comment above them got
# wrong. What it does not reproduce is the CONTENT of those headers, which these tests
# never read: `changed_paths` resolves a work LIST, and the walk that opens the files is
# what every other class in this file covers.


def _numbered(pattern, count):
    return [pattern.format(i) for i in range(count)]


# Per pull request: the files it added and modified, and the two bucket sizes that are
# checked -- `headers` is len(buckets["headers"]), `total` is len(buckets["total"]).
PULL_REQUEST_SHAPES = {
    # 44 modified: 33 include/*.h + 11 src/*.cpp.
    "1659": {
        "subject": "Type 43 header placeholders from reference field offsets (#1659)",
        "added": [],
        "modified": (_numbered("include/Placeholder{:02d}.h", 33)
                     + _numbered("src/placeholder{:02d}.cpp", 11)),
        "headers": 33,
        "total": 44,
    },
    # 21 modified: 15 include/*.h + 6 src/*.cpp.
    "1665": {
        "subject": "Type the 15 short headers up to the size operator new proves (#1665)",
        "added": [],
        "modified": (_numbered("include/Short{:02d}.h", 15)
                     + _numbered("src/short{:02d}.cpp", 6)),
        "headers": 15,
        "total": 21,
    },
    # 66: 2 added tools/*.py, plus 64 modified -- 58 include/*.h and 6 src/*.cpp.
    "1666": {
        "subject": "Read member types out of the destructor relocations (#1666)",
        "added": ["tools/dtor_members.py", "tools/test_dtor_members.py"],
        "modified": (_numbered("include/Dtor{:02d}.h", 58)
                     + _numbered("src/dtor{:02d}.cpp", 6)),
        "headers": 58,
        "total": 66,
    },
    # The honest empty: 31 files, not one of them a header. 3 added (a workflow and two
    # tools) and 28 modified (1 src_tu/*.c, 26 src_tu/*.cpp, 1 tools/*.py).
    "1667": {
        "subject": "src_tu was stranded by a landed rename and no gate saw it (#1667)",
        "added": [".github/workflows/src-tu-refs.yml", "tools/check_src_tu.py",
                  "tools/test_check_src_tu.py"],
        "modified": (["src_tu/stranded.c"]
                     + _numbered("src_tu/tu{:02d}.cpp", 26)
                     + ["tools/tubuild.py"]),
        "headers": 0,
        "total": 31,
    },
}


class SquashedHistory(Repo):
    """A throwaway repository whose main branch advances one squashed commit per pull
    request, which is the shape this tree's own main has.

    All four are built into the SAME repository, in order, so every `<sha>^...<sha>` has
    commits on both sides of it, and Repo's own base header sits under all of them. A
    resolution that reached past the parent, or that quietly fell back to the working
    tree's HEAD when `head=` was passed, would pick up a neighbour's files and blow the
    exact counts below.
    """

    @staticmethod
    def _body(rel, tag):
        # Content is deliberately trivial: nothing here opens these files. `tag` differs
        # between the seed and the pull request so git records a modification.
        return f"/* {rel}: {tag} */\n"

    def pull_request(self, spec):
        """Land one pull request as a single squashed commit; return its sha."""
        seed = [p for p in spec["modified"] if not (self.dir / p).exists()]
        if seed:
            for p in seed:
                self.write(p, self._body(p, "before"))
            self.commit(f"seed the files {spec['subject']} modifies")
        for p in spec["added"]:
            self.write(p, self._body(p, "added"))
        for p in spec["modified"]:
            self.write(p, self._body(p, "after"))
        self.commit(spec["subject"])
        return self.rev("HEAD")


class PullRequestShapeTests(unittest.TestCase):
    """The fix must not have narrowed what the gate picks up. Aimed at the shapes whole
    pull requests really have -- dozens of files, most of them not headers -- rather than
    at the two- and three-file cases the classes above use."""

    @classmethod
    def setUpClass(cls):
        stack = contextlib.ExitStack()
        cls.addClassCleanup(stack.close)
        cls.repo = stack.enter_context(SquashedHistory())
        cls.shas = {pr: cls.repo.pull_request(spec)
                    for pr, spec in sorted(PULL_REQUEST_SHAPES.items())}

    def _buckets(self, pr):
        sha = self.shas[pr]
        buckets, err = C.changed_paths(f"{sha}^", committed_only=True,
                                       repo=self.repo.dir, head=sha)
        self.assertIsNone(err)
        return buckets

    def test_the_headers_each_pull_request_edited_still_come_back(self):
        for pr, spec in sorted(PULL_REQUEST_SHAPES.items()):
            with self.subTest(pr=pr):
                buckets = self._buckets(pr)
                self.assertEqual(len(buckets["headers"]), spec["headers"])
                self.assertEqual(buckets["dropped"], [])
                self.assertTrue(buckets["total"], "the diff itself must not be empty")
                self.assertEqual(len(buckets["total"]), spec["total"])

    def test_the_pull_request_that_touched_no_header_is_the_honest_empty(self):
        buckets = self._buckets("1667")
        self.assertEqual(buckets["include"], [])
        self.assertEqual(len(buckets["total"]), 31)

    def test_a_pull_request_picks_up_its_own_files_and_only_its_own(self):
        """Stronger than the counts, and the reason all four share one repository:
        Repo's base header and the other three pull requests' 130-odd files are all
        present in this history, and none of them may appear here."""
        spec = PULL_REQUEST_SHAPES["1659"]
        buckets = self._buckets("1659")
        self.assertEqual(buckets["total"], sorted(spec["added"] + spec["modified"]))
        self.assertEqual(buckets["headers"],
                         sorted(p for p in spec["modified"] if p.endswith(".h")))
        self.assertNotIn("include/Widget.h", buckets["total"])

    def test_pointing_head_at_history_ignores_a_dirty_working_tree(self):
        """`head=` is how a caller aims this at a commit other than the branch tip, and
        the working tree and untracked sources are meaningless there. The sha-pinned
        version of these tests relied on that silently -- it ran against a working
        checkout, which is routinely dirty -- and asserted it nowhere."""
        before = self._buckets("1665")
        self.repo.write("include/Placeholder00.h", "/* edited, never committed */\n")
        self.repo.write("include/Untracked.h", BROKEN.replace("Widget", "Untracked"))
        try:
            after = self._buckets("1665")
            self.assertEqual(after, before)
            self.assertEqual(after["worktree"], [])
            self.assertNotIn("include/Untracked.h", after["total"])
        finally:
            self.repo.git("checkout", "--", "include/Placeholder00.h")
            (self.repo.dir / "include" / "Untracked.h").unlink()

    def test_a_deleted_header_is_not_work_but_a_renamed_one_is(self):
        """Not a reconstruction -- none of the four deleted or renamed anything, so this
        is added coverage, and writing it is what found the `--no-renames` defect.

        `--diff-filter=AM` is what keeps a deletion out of the work list; there is no
        file left to open. A RENAME has to survive it, because a header that moved still
        has to have its offsets agree and its new path is the one anything later checks.
        changed_paths' docstring claimed it already did, "because `-M` is deliberately
        not passed" -- but `diff.renames` has defaulted to true since git 2.9, so git
        detected the move regardless, reported it as `R`, and the filter dropped it. A
        pull request that only moved headers resolved to an empty work list and took the
        honest-empty exit. The precondition below is that pre-fix command verbatim."""
        with Repo() as r:
            r.write("include/Gone.h", GOOD.replace("Widget", "Gone"))
            r.write("include/Before.h", GOOD.replace("Widget", "Before"))
            r.commit("two more headers")
            (r.dir / "include" / "Gone.h").unlink()
            r.git("mv", "include/Before.h", "include/After.h")
            r.commit("retire one header and move another")
            sha = r.rev("HEAD")

            before_the_fix = r.git("diff", "--name-only", "--diff-filter=AM",
                                   f"{sha}^...{sha}").split()
            self.assertEqual(before_the_fix, [],
                             "precondition: git's own rename detection emptied the list")

            buckets, err = C.changed_paths(f"{sha}^", committed_only=True,
                                           repo=r.dir, head=sha)
            self.assertIsNone(err)
            self.assertEqual(buckets["total"], ["include/After.h"])
            self.assertEqual(buckets["headers"], ["include/After.h"])


# ------------------------------------------------- defect 3: the root-vptr bail-out
#
# The pre-fix code refused to model any ROOT struct the moment its body reached a
# method declaration, on the grounds that a polymorphic root carries an implicit vptr
# that no declaration mentions. The offset IS derivable, and the tree writes the model
# down by hand -- verbatim, in include/Animation.h and include/dCc_c.h:
#
#   /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
#
# The bail-out also fired for structs with no `virtual` anywhere, because its third
# alternative matches ANY member-function declaration. Eighteen headers were skipped
# whole: no summary, no mismatch, no non-zero exit. Nine of them check clean now; the
# other nine are enumerated in config/header-offset-known-issues.txt with the parser
# limit that stops each one.

# The predicate of the deleted arm, VERBATIM, so the cases below stay planted
# regressions rather than tests of the new code alone.
_OLD_BAILOUT = re.compile(
    r"^\s*(virtual\b|~\w+\s*\([^;]*\)\s*;|[A-Za-z_][\w:<>, &*]*\([^;]*\)\s*(const)?\s*;)")


def _skipped_the_old_way(body):
    """True when the pre-fix code declared this ROOT struct unmodelled and skipped it."""
    started = False
    for line in body.splitlines():
        if not started:
            started = bool(re.match(r"^\s*struct \w+\s*\{", line))
            continue
        if _OLD_BAILOUT.match(line):
            return True
    return False


# A polymorphic root. Every offset counts the implicit vptr, so the first field is
# 0x004 -- which is what the eight headers in this shape already say.
POLY_GOOD = """\
struct Widget {
\tvirtual void Update();
\tu32 first;   /* 0x004 */
\tu32 second;  /* 0x008 */
};
"""
# The same class with its comments written as though there were no vptr.
POLY_BROKEN = POLY_GOOD.replace("0x004", "0x000").replace("0x008", "0x004")

# A flat view that declares the vptr as a REAL field; include/ArrowSignRight.h is the
# live instance. Its comments already count that field, so the walk must start at 0 --
# adding 4 unconditionally would report every field of every such header 4 bytes late.
FLAT_GOOD = """\
struct Widget {
\tvoid *vtable;  /* 0x000 */
\tu32 first;     /* 0x004 */
\tu32 second;    /* 0x008 */
\tvirtual void Update();
};
"""
FLAT_BROKEN = """\
struct Widget {
\tvoid *vtable;  /* 0x000 */
\tu32 first;     /* 0x008 */
\tu32 second;    /* 0x00c */
\tvirtual void Update();
};
"""

# No `virtual` anywhere. The bail-out's third alternative matches any member function,
# so a struct that merely DECLARES a method was skipped -- include/dMgState_c.h and
# include/dMg3DEspAnimSet_c.h are that shape, and neither is polymorphic.
NONPOLY_GOOD = """\
struct Widget {
\tu32 first;   /* 0x000 */
\tu32 second;  /* 0x004 */
\tvoid Reset();
};
"""
NONPOLY_BROKEN = NONPOLY_GOOD.replace("0x004", "0x008")


class RootVptrTests(unittest.TestCase):
    def test_every_fixture_below_was_skipped_whole_by_the_old_code(self):
        """The precondition for all of it. If these were not skipped there was no hole."""
        for name, body in (("POLY_GOOD", POLY_GOOD), ("POLY_BROKEN", POLY_BROKEN),
                           ("FLAT_GOOD", FLAT_GOOD), ("FLAT_BROKEN", FLAT_BROKEN),
                           ("NONPOLY_GOOD", NONPOLY_GOOD),
                           ("NONPOLY_BROKEN", NONPOLY_BROKEN)):
            self.assertTrue(_skipped_the_old_way(body),
                            f"{name}: precondition failed, the old code checked this")

    def test_a_polymorphic_root_is_modelled_and_a_wrong_offset_is_caught(self):
        with Repo() as r:
            r.write("include/Widget.h", POLY_GOOD)
            self.assertEqual(r.run("include/Widget.h"), 0)
        with Repo() as r:
            r.write("include/Widget.h", POLY_BROKEN)
            self.assertEqual(r.run("include/Widget.h"), 1,
                             "a root whose comments ignore its vptr must not pass")

    def test_an_explicitly_declared_vtable_field_is_not_double_counted(self):
        with Repo() as r:
            r.write("include/Widget.h", FLAT_GOOD)
            self.assertEqual(r.run("include/Widget.h"), 0,
                             "the vptr was counted twice for a header that spells it out")
        with Repo() as r:
            r.write("include/Widget.h", FLAT_BROKEN)
            self.assertEqual(r.run("include/Widget.h"), 1)

    def test_a_non_polymorphic_struct_gets_no_vptr_for_declaring_a_method(self):
        with Repo() as r:
            r.write("include/Widget.h", NONPOLY_GOOD)
            self.assertEqual(r.run("include/Widget.h"), 0)
        with Repo() as r:
            r.write("include/Widget.h", NONPOLY_BROKEN)
            self.assertEqual(r.run("include/Widget.h"), 1)

    def test_the_vptr_decision_is_scoped_to_this_struct_not_the_whole_file(self):
        """A twin header carries a flat C view of the same class below an `#else`, and
        that half routinely spells `void **vtable;` even where the C++ half above it
        does not -- include/Animation.h is exactly that shape. A file-wide search for
        either marker reads the wrong half and shifts every field of the right one.

        Not a planted regression, and it cannot be one: the old code skipped this
        header whole, so it "passed" here for the wrong reason. What this guards is the
        obvious WRONG way to write the new code -- grep the file for `virtual` and for
        a vtable field -- which would read 0 here and report `first` at 0x000."""
        twin = POLY_GOOD + """
#else
struct Widget {
\tvoid **vtable;  /* 0x000 */
\tu32 first;      /* 0x004 */
};
#endif
"""
        with Repo() as r:
            r.write("include/Widget.h", twin)
            self.assertEqual(r.run("include/Widget.h"), 0,
                             "the C++ half's implicit vptr was cancelled by the flat "
                             "half's vtable field")


class NestedTagShadowTests(unittest.TestCase):
    """CLASS_SIZES is keyed by the bare tag, so a class NESTED in the struct under test
    silently borrows the size of any unrelated top-level class sharing that tag.

    One instance in the tree, and unskipping fBase_c.h is what exposed it:
    `fBase_c::Manager` is SceneNode(0x14) + 2 * fLiNdBaPr_c(0x10) = 0x34, but
    `Manager_size_must_be_0x3c` in include/Particle__Manager.h describes an unrelated
    Particle::Manager. The checker took 0x3c and reported the two fields after it 8
    bytes late -- against a header whose own `fBase_c_size_must_be_0x50` assertion
    proves the comments right and the checker wrong.

    Refuse rather than guess: a wrong size shifts every later field, and each one still
    "matches" if the comments were written from the same wrong model. An honest
    UNPARSED is recoverable; a confident wrong number is not.
    """

    SHADOWED = """\
struct Widget {
\tstruct Manager {
\t\tu32 a;
\t\tu32 b;
\t};

\tu32 first;         /* 0x000 */
\tManager manager;   /* 0x004 */
\tu32 last;          /* 0x00c */
};
"""

    def test_a_nested_tag_does_not_borrow_an_unrelated_headers_size(self):
        with Repo() as r:
            r.write("include/Widget.h", self.SHADOWED)
            elsewhere = pathlib.Path(REPO) / "include" / "Elsewhere.h"
            with unittest.mock.patch.dict(C.SZ, {"Manager": 0x3c}), \
                    unittest.mock.patch.dict(C.CLASS_SIZE_SRC, {"Manager": elsewhere}):
                self.assertEqual(r.run("include/Widget.h"), 1,
                                 "an unrelated class's size was used silently")

    def test_a_tag_asserted_in_its_OWN_header_is_still_used(self):
        """Bounding the refusal. include/dScEntry_c.h nests `icon_c` and asserts
        `icon_c_size_must_be_0x24` nine lines below it; comparing the two paths
        unresolved -- CLASS_SIZE_SRC holds absolute rglob paths, `fp` arrives from
        argv -- made that look like a shadow and turned a header that checked nine
        fields clean into an UNPARSED failure."""
        with Repo() as r:
            r.write("include/Widget.h", self.SHADOWED)
            own = r.dir / "include" / "Widget.h"
            with unittest.mock.patch.dict(C.SZ, {"Manager": 8}), \
                    unittest.mock.patch.dict(C.CLASS_SIZE_SRC, {"Manager": own}):
                self.assertEqual(r.run("include/Widget.h"), 0,
                                 "a size asserted in this very header must still count")


# ------------------------------------- defect 4: nested tags that DO assert their size
#
# _shadowed_by_nested (above) made the gate refuse rather than guess whenever a class
# nested in the struct under test shared its tag with an unrelated top-level one. That
# is the right call when the size is genuinely unknown -- but for nine of the headers it
# refused, the size is not unknown at all. The tree had already invented an
# outer-qualified assertion name to dodge exactly this collision:
#
#   typedef char dMgPsOpt_TouchIcon_c_size_must_be_0x24[
#       sizeof(dMgPsOpt_c::TouchIcon_c) == 0x24 ? 1 : -1];
#
# The lookup only ever read the bare tag, so it never found it, and include/dMgPsOpt_c.h
# sat in config/header-offset-known-issues.txt reporting `struct spans 0x8`.
#
# The identifier is NOT a mechanical composition -- `dMgPsOpt_TouchIcon_c` drops the
# outer's `_c` -- so nothing derives the qualified name from it without guessing. The
# `sizeof(...)` operand IS the qualified name, by construction: the compiler resolves it
# or the header does not build. That is what CLASS_SIZES_QUALIFIED reads.
#
# Every case below is planted the same way: the fixture is run with the qualified table
# emptied, which is the pre-fix state exactly, and again with it populated.

# `Manager` is nested in the struct under test and asserted ONLY under its qualified
# name, so the bare-tag table cannot answer and the shadow check refuses. 8 bytes.
QUALIFIED_GOOD = """\
struct Widget {
\tstruct Manager {
\t\tu32 a;
\t\tu32 b;
\t};

\tu32 first;         /* 0x000 */
\tManager manager;   /* 0x004 */
\tu32 last;          /* 0x00c */
};
"""
# The same header with `last` written as though Manager were the 0x3c-byte top-level
# class -- i.e. what a comment author working from the wrong model would produce.
QUALIFIED_BROKEN = QUALIFIED_GOOD.replace("/* 0x00c */", "/* 0x040 */")


class QualifiedSizeAssertTests(unittest.TestCase):
    def test_a_qualified_assertion_is_read_where_the_bare_tag_cannot_answer(self):
        with Repo() as r:
            r.write("include/Widget.h", QUALIFIED_GOOD)
            # Pre-fix: no qualified table. The bare tag is unknown, the nested body
            # shadows, and the field goes UNPARSED -- which is a non-zero exit and,
            # worse, suppresses the field walk for the whole header.
            with unittest.mock.patch.dict(C.SZ, {}, clear=False), \
                    unittest.mock.patch.dict(C.CLASS_SIZES_QUALIFIED, {}, clear=True):
                self.assertEqual(r.run("include/Widget.h"), 1,
                                 "precondition: the old lookup could not size this")
            # With the assertion read out of its own sizeof() operand.
            with unittest.mock.patch.dict(C.CLASS_SIZES_QUALIFIED,
                                          {"Widget::Manager": 8}):
                self.assertEqual(r.run("include/Widget.h"), 0)

    def test_a_wrong_comment_is_still_caught_once_the_size_is_known(self):
        """The point of retiring a waiver is to start CHECKING the header, not to
        start passing it. A size the gate can resolve has to be able to fail."""
        with Repo() as r:
            r.write("include/Widget.h", QUALIFIED_BROKEN)
            with unittest.mock.patch.dict(C.CLASS_SIZES_QUALIFIED,
                                          {"Widget::Manager": 8}):
                self.assertEqual(r.run("include/Widget.h"), 1)

    def test_a_qualified_name_outranks_a_same_tagged_top_level_class(self):
        """This is C++ name lookup, not a heuristic. `Manager` written inside Widget
        names Widget::Manager whenever one exists, whatever unrelated top-level
        Manager the tree also declares -- so the qualified answer must WIN, not merely
        fill in where the bare table is silent. Getting this backwards reintroduces
        the fBase_c::Manager bug the shadow check was added to stop."""
        with Repo() as r:
            r.write("include/Widget.h", QUALIFIED_GOOD)
            elsewhere = pathlib.Path(REPO) / "include" / "Elsewhere.h"
            with unittest.mock.patch.dict(C.SZ, {"Manager": 0x3c}), \
                    unittest.mock.patch.dict(C.CLASS_SIZE_SRC, {"Manager": elsewhere}), \
                    unittest.mock.patch.dict(C.CLASS_SIZES_QUALIFIED,
                                             {"Widget::Manager": 8}):
                self.assertEqual(r.run("include/Widget.h"), 0,
                                 "the unrelated top-level size was preferred")

    def test_an_ambiguous_deeper_nesting_refuses_instead_of_picking_one(self):
        """The field walk does not track intermediate scopes, so `Outer::*::Inner` is
        matched by suffix. Two different classes can end in the same tag -- and two
        wrong guesses are not better than one. Refusing puts the header back on the
        shadow path, which reports UNPARSED rather than a confident wrong number."""
        with Repo() as r:
            r.write("include/Widget.h", QUALIFIED_GOOD)
            with unittest.mock.patch.dict(C.CLASS_SIZES_QUALIFIED,
                                          {"Widget::alpha_c::Manager": 8,
                                           "Widget::beta_c::Manager": 0x20},
                                          clear=True):
                self.assertEqual(r.run("include/Widget.h"), 1,
                                 "an ambiguous suffix match was resolved by guessing")
            # One candidate is not ambiguous, and the suffix form has to answer -- a
            # nested type declared in an intermediate scope is the reason it exists.
            with unittest.mock.patch.dict(C.CLASS_SIZES_QUALIFIED,
                                          {"Widget::alpha_c::Manager": 8}, clear=True):
                self.assertEqual(r.run("include/Widget.h"), 0)

    def test_the_two_sizes_in_every_real_assertion_agree(self):
        """The identifier carries the size as a `_0xN` suffix and the expression
        carries it again as the compared constant. This reads the first and keys it by
        the second's operand, which is only sound while the two never disagree. They
        do not, in any of the tree's assertions -- and if a future header makes them
        disagree, that header is the thing to fix, not this table."""
        disagreements = []
        for h in sorted((pathlib.Path(REPO) / "include").rglob("*.h")):
            for m in C._SIZE_ASSERT_FULL.finditer(h.read_text(errors="replace")):
                if int(m.group(2), 16) != int(m.group(4), 0):
                    disagreements.append(f"{h.name}: {m.group(1)} vs {m.group(4)}")
        self.assertEqual(disagreements, [])

    def test_the_waiver_this_change_retires_now_checks_clean(self):
        """include/dMgPsOpt_c.h is the live case: `TouchIcon_c mIcons[8];`, sized only
        by `sizeof(dMgPsOpt_c::TouchIcon_c) == 0x24`. It reported `struct spans 0x8`
        and was waived. The span it walks to now, 0x128, is independently asserted by
        the header's own dMgPsOpt_c_size_must_be_0x128 -- so the size being read here
        is corroborated by something other than the comments it is checking."""
        self.assertNotIn("include/dMgPsOpt_c.h", C._known_issues(),
                         "the waiver is back; this test guards its retirement")
        self.assertEqual(C.main(["include/dMgPsOpt_c.h"], REPO), 0)


# ---------------------------------- defect 5: Allman inline member-function bodies

ALLMAN_EMPTY = """\
struct Widget {
    virtual ~Widget()
    {
    }
    u32 first;   /* 0x004 */
    u32 second;  /* 0x008 */
};
"""

ALLMAN_NONEMPTY = """\
struct Widget {
    virtual ~Widget()
    {
        if (first) { first = 0; } // unmatched-looking }
        /* Braces in comments are not body braces: { } { */
        const char *brace = "}";
    }
    u32 first;   /* 0x004 */
    u32 second;  /* 0x008 */
    u16 third;   /* 0x00c */
};
"""

SAME_LINE_BODY = """\
struct Widget {
    virtual ~Widget() { /* } */ }
    u32 first;   /* 0x004 */
    u32 second;  /* 0x008 */
};
"""

DECLARED_DTOR = """\
struct Widget {
    virtual ~Widget();
    u32 first;   /* 0x004 */
    u32 second;  /* 0x008 */
};
"""


class InlineMethodBodyTests(unittest.TestCase):
    def _run(self, body):
        with Repo() as r:
            r.write("include/Widget.h", body)
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                rc = r.run("include/Widget.h")
            return rc, out.getvalue()

    def _assert_checked(self, body, count, span):
        rc, out = self._run(body)
        self.assertEqual(rc, 0, out)
        self.assertIn(
            f"{count} commented fields, 0 mismatched, 0 unparsed, struct spans {span}",
            out,
        )

    def test_an_empty_allman_destructor_does_not_end_the_struct(self):
        # Pre-fix: depth was computed from this declaration line alone.  It is
        # necessarily zero even though the body opens on the following line.
        signature = "    virtual ~Widget()"
        self.assertEqual(signature.count("{") - signature.count("}"), 0)
        self._assert_checked(ALLMAN_EMPTY, 2, "0xc")

    def test_a_nonempty_allman_body_ignores_comment_and_string_braces(self):
        self._assert_checked(ALLMAN_NONEMPTY, 3, "0xe")

    def test_a_one_line_body_keeps_working(self):
        self._assert_checked(SAME_LINE_BODY, 2, "0xc")

    def test_an_ordinary_destructor_declaration_does_not_consume_fields(self):
        self._assert_checked(DECLARED_DTOR, 2, "0xc")

    def test_a_missing_body_does_not_hide_the_following_field(self):
        malformed = ALLMAN_EMPTY.replace("    {\n    }\n", "")
        rc, out = self._run(malformed)
        self.assertEqual(rc, 1, out)
        self.assertIn("2 commented fields, 0 mismatched, 1 unparsed", out)

    def test_a_bad_field_after_an_allman_body_is_still_a_failure(self):
        broken = ALLMAN_EMPTY.replace("/* 0x008 */", "/* 0x00c */")
        rc, out = self._run(broken)
        self.assertEqual(rc, 1, out)
        self.assertIn("2 commented fields, 1 mismatched, 0 unparsed", out)


# ------------------------- allocation methods do not occupy or end instance fields

class AllocationMethodTests(unittest.TestCase):
    def _run(self, members):
        with tempfile.TemporaryDirectory(prefix="offset_opnew_") as tmp:
            header = pathlib.Path(tmp) / "Widget.h"
            header.write_text("struct Widget {\n" + members + "\n};\n",
                              encoding="utf-8")
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                rc = C.main([str(header)])
            return rc, out.getvalue()

    def _assert_checked(self, method):
        rc, out = self._run(
            "    u32 first; /* 0x000 */\n" + method +
            "\n    u32 second; /* 0x004 */\n    u16 third; /* 0x008 */")
        self.assertEqual(rc, 0, out)
        self.assertIn("3 commented fields, 0 mismatched, 0 unparsed, struct spans 0xa", out)

    def test_allocation_declarations_do_not_end_the_field_walk(self):
        for method in (
            "    static void *operator new(unsigned long size);",
            "    void* operator new(unsigned long);",
            "    static void *operator new[](unsigned long size);",
        ):
            with self.subTest(method=method):
                self._assert_checked(method)

    def test_inline_allocation_bodies_do_not_end_the_field_walk(self):
        for method in (
            "    static void *operator new(unsigned long size) { return Alloc(size); }",
            "    static void *operator new(unsigned long size) {\n"
            "        if (size) { return Alloc(size); }\n        return 0;\n    }",
            "    static void *operator new(unsigned long size)\n"
            "    {\n        return Alloc(size);\n    }",
        ):
            with self.subTest(method=method):
                self._assert_checked(method)

    def test_braces_in_comments_and_strings_do_not_consume_fields(self):
        self._assert_checked(
            "    static void *operator new(unsigned long size) { // }\n"
            "        /* { } { */\n"
            '        const char *text = "}";\n'
            "        return Alloc(size);\n    }")

    def test_an_allocator_before_fields_adds_no_vptr_or_storage(self):
        rc, out = self._run(
            "    static void *operator new(unsigned long size) {\n"
            "        return Alloc(size);\n    }\n"
            "    u32 first; /* 0x000 */\n    u8 tail; /* 0x004 */")
        self.assertEqual(rc, 0, out)
        self.assertIn("2 commented fields, 0 mismatched, 0 unparsed, struct spans 0x5", out)

    def test_a_wrong_offset_on_either_side_still_fails(self):
        for first, second in (("0x004", "0x004"), ("0x000", "0x008")):
            with self.subTest(first=first, second=second):
                rc, out = self._run(
                    f"    u32 first; /* {first} */\n"
                    "    static void *operator new(unsigned long size) { return Alloc(size); }\n"
                    f"    u32 second; /* {second} */")
                self.assertEqual(rc, 1, out)
                self.assertIn("2 commented fields, 1 mismatched, 0 unparsed", out)

    def test_unknown_and_malformed_fields_are_not_hidden(self):
        for field in ("UnknownType mystery;", "u32 mystery[UNKNOWN_BOUND];"):
            for before in (True, False):
                with self.subTest(field=field, before=before):
                    method = "    static void *operator new(unsigned long size) { return Alloc(size); }"
                    members = field + "\n" + method if before else method + "\n" + field
                    rc, out = self._run(members + "\n    u32 tail; /* 0x004 */")
                    self.assertEqual(rc, 1, out)
                    self.assertIn("UNPARSED", out)
                    self.assertIn(field, out)

    def test_a_missing_body_does_not_hide_the_following_field(self):
        rc, out = self._run(
            "    static void *operator new(unsigned long size)\n"
            "    u32 first; /* 0x000 */")
        self.assertEqual(rc, 1, out)
        self.assertIn("1 commented fields, 0 mismatched, 1 unparsed", out)
        self.assertIn("operator new", out)

    def test_an_incomplete_signature_is_not_an_allocation_method(self):
        rc, out = self._run(
            "    static void *operator new(unsigned long size;\n"
            "    u32 first; /* 0x000 */")
        self.assertEqual(rc, 1, out)
        self.assertIn("UNPARSED", out)
        self.assertIn("operator new", out)

    def test_a_similarly_named_pointer_is_still_a_field(self):
        rc, out = self._run(
            "    void *operator_new; /* 0x000 */\n"
            "    u32 tail; /* 0x004 */")
        self.assertEqual(rc, 0, out)
        self.assertIn("2 commented fields, 0 mismatched, 0 unparsed, struct spans 0x8", out)

    def test_the_neighboring_inline_accessor_also_preserves_fields(self):
        self._assert_checked(
            "    static void *operator new(unsigned long size) { return Alloc(size); }\n"
            "    const Vector3 &Pos() const {\n"
            "        return *reinterpret_cast<const Vector3 *>(&mPosX);\n    }")

    def test_a_named_allman_method_also_preserves_fields(self):
        self._assert_checked(
            "    int Value() const\n    {\n        return 0;\n    }")

    def test_unknown_and_malformed_fields_cannot_complete_a_missing_body(self):
        for field in ("UnknownType mystery;", "u32 mystery[UNKNOWN_BOUND];"):
            with self.subTest(field=field):
                rc, out = self._run(
                    "    static void *operator new(unsigned long size)\n" + field)
                self.assertEqual(rc, 1, out)
                self.assertIn("2 unparsed", out)
                self.assertIn("operator new", out)
                self.assertIn(field, out)

    def test_an_allman_allocator_can_split_the_semicolon_only(self):
        self._assert_checked("    static void *operator new(unsigned long size)\n    ;")

    def test_a_signature_without_body_at_end_of_file_is_not_a_pass(self):
        with tempfile.TemporaryDirectory(prefix="offset_opnew_") as tmp:
            header = pathlib.Path(tmp) / "Widget.h"
            header.write_text(
                "struct Widget {\n    static void *operator new(unsigned long size)\n",
                encoding="utf-8")
            out = io.StringIO()
            with contextlib.redirect_stdout(out):
                rc = C.main([str(header)])
            self.assertEqual(rc, 1, out.getvalue())
            self.assertIn("1 unparsed", out.getvalue())


# THIS BLOCK MUST BE THE LAST THING IN THE FILE, and it was not. It sat two thirds of
# the way up, immediately below the history tests, where four later classes had not been
# defined yet -- so `unittest.main()` collected what existed at that point and
# `sys.exit()`ed before the interpreter ever reached the rest. header-offsets.yml invokes
# this file as a SCRIPT, so what CI actually ran was 20 of the 53 tests: every case for
# the root vptr, the nested-tag shadow, the qualified size assertions, the Allman inline
# bodies and the allocation methods was defined but never collected, and the job printed
# `Ran 20 tests ... OK` over them. `python -m unittest tools.test_check_header_offsets`
# imports the module instead of running it, which is why running it that way locally
# found 53 and nobody noticed the split.
if __name__ == "__main__":
    unittest.main()
