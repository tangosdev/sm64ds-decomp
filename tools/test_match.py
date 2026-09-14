"""Flag resolution in tools/match.py.

`match.py` is the per-function scoring harness, so a flag it gets wrong does not fail --
it reports a near miss against bytes the build would never produce, and the operator goes
looking for a source-level cause that is not there. That is the third time this tree has
paid for a checker compiling with flags the build does not use (see
notes on tools/pr_linkcheck.py), so the resolution is a function with tests rather than a
few lines inside main().

Only `resolve_cpp_flags` is covered here: it is pure, needs no compiler and no ROM, and it
is the part that was silently wrong. Everything else in match.py needs mwccarm.
"""
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import match  # noqa: E402


class ResolveCppFlags(unittest.TestCase):
    C99 = "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e"
    CPP = "-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e"

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.tmp = pathlib.Path(self._tmp.name)
        self.addCleanup(self._tmp.cleanup)

    def write(self, name, text):
        p = self.tmp / name
        p.write_text(text, encoding="utf-8")
        return p

    def test_cpp_marker_flips_language_and_disables_exceptions(self):
        f = self.write("a.cpp", "//cpp" + chr(10) + "int f(void) { return 0; }" + chr(10))
        out = match.resolve_cpp_flags(f, self.C99)
        self.assertIn("-lang c++", out)
        self.assertNotIn("-lang c99", out)
        self.assertIn(match.CPP_EXCEPTIONS_FLAG, out)

    def test_hand_passed_lang_cpp_still_disables_exceptions(self):
        """The hole. `--flags "... -lang c++ ..."` is how a C++ candidate WITHOUT a //cpp
        marker is compiled, and it used to skip `-Cpp_exceptions off` entirely: the append
        lived inside the c99 -> c++ rewrite, which this path never enters. A source holding
        an object with a destructor then compiled WITH exception cleanup the ROM has not."""
        f = self.write("b.cpp", "struct S { ~S(); }; int f(void) { return 0; }" + chr(10))
        out = match.resolve_cpp_flags(f, self.CPP)
        self.assertIn(match.CPP_EXCEPTIONS_FLAG, out)

    def test_hand_passed_lang_cpp_on_a_marked_source_too(self):
        f = self.write("c.cpp", "//cpp" + chr(10) + "int f(void) { return 0; }" + chr(10))
        out = match.resolve_cpp_flags(f, self.CPP)
        self.assertIn(match.CPP_EXCEPTIONS_FLAG, out)
        self.assertEqual(out.count("-Cpp_exceptions"), 1)

    def test_explicit_exceptions_choice_is_left_alone(self):
        """A caller measuring the difference on purpose keeps their flag, either way."""
        f = self.write("d.cpp", "//cpp" + chr(10) + "int f(void) { return 0; }" + chr(10))
        for explicit in ("-Cpp_exceptions on", "-Cpp_exceptions off"):
            out = match.resolve_cpp_flags(f, self.C99 + " " + explicit)
            self.assertEqual(out.count("-Cpp_exceptions"), 1, out)
            self.assertIn(explicit, out)

    def test_plain_c_source_is_untouched(self):
        f = self.write("e.c", "int f(void) { return 0; }" + chr(10))
        self.assertEqual(match.resolve_cpp_flags(f, self.C99), self.C99)

    def test_a_marker_further_down_does_not_count(self):
        """fdiff/swarm read the FIRST bytes; a //cpp on line 2 is a comment, not a marker."""
        f = self.write("f.cpp", "int f(void);" + chr(10) + "//cpp" + chr(10))
        self.assertEqual(match.resolve_cpp_flags(f, self.C99), self.C99)

    def test_missing_candidate_defers_to_compile_c(self):
        """An unreadable path must not raise here -- compile_c reports it with a clearer
        message. It is also not C++, so the C flags come back unchanged."""
        missing = self.tmp / "nope.cpp"
        self.assertEqual(match.resolve_cpp_flags(missing, self.C99), self.C99)

    def test_default_flags_are_c99_and_carry_no_exceptions_flag(self):
        """Guards the premise of the two rewrites above. DEFAULT_FLAGS is pinned verbatim by
        nearmiss/eval_pin.json, so if it ever gains -lang c++ or -Cpp_exceptions this whole
        function needs rereading rather than a quiet test edit."""
        self.assertIn("-lang c99", match.DEFAULT_FLAGS)
        self.assertNotIn("-Cpp_exceptions", match.DEFAULT_FLAGS)


if __name__ == "__main__":
    unittest.main()
