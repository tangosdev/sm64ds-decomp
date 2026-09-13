"""Regression tests for tools/guard_size_assertions.py.

The tool rewrites 414 committed headers in one pass, so the ways it can be wrong
are the ways a bulk rewrite is always wrong: it changes a byte nobody asked it to
change, it is not idempotent so a second run doubles the guard, it flips this
tree's CRLF to LF under a one-line diff, or it guards an assertion that was
already about the host and quietly kills a live check.

Each of those has a test here. No compiler and no extracted ROM required.

    python tools/test_guard_size_assertions.py
"""
import pathlib
import subprocess
import sys
import tempfile
import unittest

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import guard_size_assertions as G       # noqa: E402

ONE_LINE = "typedef char Player_size_must_be_0x900[sizeof(Player) == 0x900 ? 1 : -1];"
TWO_LINE = ("typedef char Bird_size_must_be_0x184[\n"
            "    sizeof(Bird) == 0x184 ? 1 : -1];")
THREE_LINE = ("typedef char Matrix3x3_size_must_be_0x24[\n"
              "    sizeof(Matrix3x3) == 0x24 ? 1 : -1\n"
              "];")


def crlf(text):
    return text.replace("\n", "\r\n")


def wrapped(body):
    return "\n".join([G.GUARD_OPEN, G.GUARD_NOTE, body, G.GUARD_CLOSE])


class Rewrite(unittest.TestCase):

    def test_single_line_statement_is_wrapped_verbatim(self):
        src = "struct Player {};\n\n" + ONE_LINE + "\n"
        out, n = G.rewrite(src)
        self.assertEqual(n, 1)
        self.assertEqual(out, "struct Player {};\n\n" + wrapped(ONE_LINE) + "\n")

    def test_two_line_statement_keeps_its_own_line_break_and_indent(self):
        src = "x\n" + TWO_LINE + "\ny\n"
        out, n = G.rewrite(src)
        self.assertEqual(n, 1)
        self.assertEqual(out, "x\n" + wrapped(TWO_LINE) + "\ny\n")
        self.assertIn("    sizeof(Bird) == 0x184 ? 1 : -1];", out)

    def test_three_line_statement_is_one_statement_not_three(self):
        src = THREE_LINE + "\n"
        out, n = G.rewrite(src)
        self.assertEqual(n, 1)
        self.assertEqual(out.count(G.GUARD_OPEN), 1)
        self.assertEqual(out, wrapped(THREE_LINE) + "\n")

    def test_adjacent_statements_share_one_guard(self):
        body = ONE_LINE + "\n" + TWO_LINE
        out, n = G.rewrite(body + "\n")
        self.assertEqual(n, 2)
        self.assertEqual(out.count(G.GUARD_OPEN), 1)
        self.assertEqual(out, wrapped(body) + "\n")

    def test_statements_split_by_a_blank_line_get_their_own_guards(self):
        src = ONE_LINE + "\n\n" + TWO_LINE + "\n"
        out, n = G.rewrite(src)
        self.assertEqual(n, 2)
        self.assertEqual(out.count(G.GUARD_OPEN), 2)

    def test_nothing_but_the_guard_lines_is_added(self):
        """Every original line survives, in order, unedited: the output is the
        input with exactly three lines inserted and none removed or rewritten."""
        src = ("#ifndef BIRD_H\n#define BIRD_H\n\n/* a comment */\nstruct Bird {\n"
               "    u8 pad[4];   /* 0x000 */\n};\n\n" + TWO_LINE +
               "\n\n#endif /* BIRD_H */\n")
        out, _ = G.rewrite(src)
        original, produced = src.split("\n"), out.split("\n")
        self.assertEqual(len(produced), len(original) + 3)
        kept = [ln for ln in produced
                if ln not in (G.GUARD_OPEN, G.GUARD_NOTE, G.GUARD_CLOSE)]
        self.assertEqual(kept, original)

    def test_a_header_with_no_assertion_is_returned_unchanged(self):
        src = "#ifndef X_H\n#define X_H\nstruct X { int a; };\n#endif\n"
        out, n = G.rewrite(src)
        self.assertEqual(n, 0)
        self.assertIs(out, src)


class LineEndings(unittest.TestCase):
    """This tree is CRLF throughout. A rewrite that lands LF turns a one-line
    change into a whole-file change and hides the real diff."""

    def test_crlf_file_stays_crlf_including_the_inserted_lines(self):
        src = crlf("struct Player {};\n" + ONE_LINE + "\n")
        out, n = G.rewrite(src)
        self.assertEqual(n, 1)
        self.assertNotIn("\n", out.replace("\r\n", ""))
        self.assertIn("#ifndef " + G.GUARD + "\r\n", out)
        self.assertIn("#endif\r\n", out)
        self.assertEqual(out, crlf("struct Player {};\n" + wrapped(ONE_LINE) + "\n"))

    def test_lf_file_stays_lf(self):
        src = "struct Player {};\n" + ONE_LINE + "\n"
        out, _ = G.rewrite(src)
        self.assertNotIn("\r", out)

    def test_read_does_not_translate_line_endings(self):
        with tempfile.TemporaryDirectory() as td:
            p = pathlib.Path(td) / "a.h"
            p.write_bytes(crlf(ONE_LINE + "\n").encode())
            self.assertIn("\r\n", G.read(p))


class Idempotence(unittest.TestCase):

    def test_running_twice_changes_nothing(self):
        src = crlf("struct Player {};\n" + ONE_LINE + "\n" + TWO_LINE + "\n")
        once, n1 = G.rewrite(src)
        twice, n2 = G.rewrite(once)
        self.assertGreater(n1, 0)
        self.assertEqual(n2, 0)
        self.assertEqual(twice, once)

    def test_a_third_run_is_still_a_no_op(self):
        out = "x\n" + ONE_LINE + "\n"
        for _ in range(3):
            out, _ = G.rewrite(out)
        self.assertEqual(out.count(G.GUARD_OPEN), 1)


class ConditionalContext(unittest.TestCase):

    def test_an_assertion_already_guarded_by_hand_is_left_alone(self):
        src = "#ifndef " + G.GUARD + "\n" + ONE_LINE + "\n#endif\n"
        out, n = G.rewrite(src)
        self.assertEqual(n, 0)
        self.assertEqual(out, src)

    def test_a_host_only_assertion_is_left_asserting(self):
        """An assertion inside `#ifdef GUARD` is a claim about the HOST's layout.
        Wrapping it in `#ifndef GUARD` would make it unreachable, which reads in
        a diff exactly like guarding it and silently deletes the check."""
        src = "#ifdef " + G.GUARD + "\n" + ONE_LINE + "\n#endif\n"
        out, n = G.rewrite(src)
        self.assertEqual(n, 0)
        self.assertEqual(out, src)

    def test_the_else_of_a_host_branch_is_left_alone(self):
        """include/ArrowSignRight.h's shape: the flat host view first, the
        ROM-shaped declaration and its assertion in the `#else`."""
        src = ("#if defined(" + G.GUARD + ") && defined(_MSC_VER)\n"
               "struct Player { int flat; };\n"
               "#else\n"
               "struct Player : Base { int real; };\n"
               + ONE_LINE + "\n"
               "#endif\n")
        out, n = G.rewrite(src)
        self.assertEqual(n, 0)
        self.assertEqual(out, src)

    def test_an_or_condition_settles_nothing_so_the_assertion_is_guarded(self):
        """`#if defined(A) || defined(GUARD)` being true does not mean GUARD is
        defined. Reading it as a guard would leave a live assertion unwrapped."""
        src = "#if defined(SOMETHING) || defined(" + G.GUARD + ")\n" + ONE_LINE + "\n#endif\n"
        out, n = G.rewrite(src)
        self.assertEqual(n, 1)
        self.assertIn(wrapped(ONE_LINE), out)

    def test_an_unrelated_conditional_does_not_stop_the_guard(self):
        src = "#ifdef __cplusplus\n" + ONE_LINE + "\n#endif\n"
        out, n = G.rewrite(src)
        self.assertEqual(n, 1)
        self.assertIn(G.GUARD_OPEN, out)

    def test_nesting_is_popped_so_a_later_assertion_is_seen(self):
        src = ("#ifdef " + G.GUARD + "\nint a;\n#endif\n" + ONE_LINE + "\n")
        out, n = G.rewrite(src)
        self.assertEqual(n, 1)

    def test_an_unterminated_statement_raises_instead_of_mangling(self):
        with self.assertRaises(ValueError):
            G.rewrite("typedef char X_size_must_be_0x4[\n    sizeof(X) == 4 ? 1 : -1\n")


class Cli(unittest.TestCase):

    def _tree(self, td, text):
        root = pathlib.Path(td)
        (root / "include").mkdir()
        (root / "include" / "Player.h").write_bytes(text.encode())
        return root

    def _run(self, *args):
        return subprocess.run([sys.executable, str(TOOLS / "guard_size_assertions.py")] + list(args),
                              capture_output=True, text=True)

    def test_default_run_writes_nothing(self):
        with tempfile.TemporaryDirectory() as td:
            root = self._tree(td, crlf(ONE_LINE + "\n"))
            before = (root / "include" / "Player.h").read_bytes()
            r = self._run("--root", str(root))
            self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
            self.assertIn("would wrap 1 assertion(s) in 1 header(s)", r.stdout)
            self.assertEqual((root / "include" / "Player.h").read_bytes(), before)

    def test_write_then_check_is_green(self):
        with tempfile.TemporaryDirectory() as td:
            root = self._tree(td, crlf(ONE_LINE + "\n"))
            w = self._run("--root", str(root), "--write")
            self.assertEqual(w.returncode, 0, w.stdout + w.stderr)
            self.assertIn("wrapped 1 assertion(s)", w.stdout)
            after = (root / "include" / "Player.h").read_bytes()
            self.assertEqual(after, crlf(wrapped(ONE_LINE) + "\n").encode())
            c = self._run("--root", str(root), "--check")
            self.assertEqual(c.returncode, 0, c.stdout)
            self.assertIn("would wrap 0 assertion(s)", c.stdout)

    def test_check_fails_on_an_unguarded_tree(self):
        with tempfile.TemporaryDirectory() as td:
            root = self._tree(td, crlf(ONE_LINE + "\n"))
            r = self._run("--root", str(root), "--check")
            self.assertEqual(r.returncode, 1, r.stdout)
            self.assertIn("FAIL", r.stdout)

    def test_an_unparseable_header_is_named_not_just_traced(self):
        """A bare 'unterminated size assertion at line 3' over 500 headers is a
        traceback nobody can act on."""
        with tempfile.TemporaryDirectory() as td:
            root = self._tree(td, crlf("x\ntypedef char X_size_must_be_0x4[\n"
                                       "    sizeof(X) == 4 ? 1 : -1\n"))
            r = self._run("--root", str(root))
            self.assertEqual(r.returncode, 1, r.stdout + r.stderr)
            self.assertIn("Player.h", r.stdout)
            self.assertIn("unterminated size assertion", r.stdout)

    def test_this_repo_is_guarded(self):
        """The claim the committed headers make, asserted rather than trusted.
        It is the gate that catches a new header landing with a bare assertion."""
        r = self._run("--root", str(REPO), "--check")
        self.assertEqual(r.returncode, 0, r.stdout)
        self.assertIn("would wrap 0 assertion(s)", r.stdout)
        self.assertIn("0 run(s) inside a host-only conditional", r.stdout)


if __name__ == "__main__":
    unittest.main()
