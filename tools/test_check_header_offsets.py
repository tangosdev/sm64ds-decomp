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

# Merge commits whose pull requests edited headers, and how many .h files each touched.
# Confirmed against `git diff --name-only --diff-filter=AM <sha>^...<sha> -- include/`.
REAL_HISTORY = {
    "1659": ("258fa9042903b378660000c13c31c1fc77ae717c", 33),
    "1665": ("558a1c26b1299f6e96e1b58a4c76e1bc682cf3a2", 15),
    "1666": ("957e60fc40ab89e6a8d240b5e97c2bfa8061d51c", 58),
    # #1667 is the src_tu gate: 31 files, none of them a header. The honest empty.
    "1667": ("3dc6c4df43928eb799faa4d7abe16e74198dbcd4", 0),
}


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


# ------------------------------------------------------------------------ real history

class RealHistoryTests(unittest.TestCase):
    """The fix must not have narrowed what the gate picks up. Aimed at merge commits
    whose pull requests really did edit headers, rather than at a fixture."""

    def _require(self, sha):
        r = subprocess.run(["git", "cat-file", "-e", f"{sha}^{{commit}}"],
                           cwd=str(REPO), capture_output=True, text=True)
        # Reported, never skipped: a self-skipping history test is indistinguishable
        # from one that passes, which is the failure mode this whole file is about.
        self.assertEqual(r.returncode, 0,
                         f"{sha} is not in this clone -- run `git fetch --unshallow` "
                         f"(a shallow clone cannot verify the resolution against history)")

    def test_the_headers_each_pull_request_edited_still_come_back(self):
        for pr, (sha, want) in sorted(REAL_HISTORY.items()):
            with self.subTest(pr=pr):
                self._require(sha)
                buckets, err = C.changed_paths(f"{sha}^", committed_only=True,
                                               repo=REPO, head=sha)
                self.assertIsNone(err)
                self.assertEqual(len(buckets["headers"]), want)
                self.assertEqual(buckets["dropped"], [])
                self.assertTrue(buckets["total"], "the diff itself must not be empty")

    def test_the_pull_request_that_touched_no_header_is_the_honest_empty(self):
        sha, _ = REAL_HISTORY["1667"]
        self._require(sha)
        buckets, err = C.changed_paths(f"{sha}^", committed_only=True, repo=REPO, head=sha)
        self.assertIsNone(err)
        self.assertEqual(buckets["include"], [])
        self.assertEqual(len(buckets["total"]), 31)


if __name__ == "__main__":
    unittest.main()


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
