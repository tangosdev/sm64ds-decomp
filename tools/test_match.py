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



class MemoizeCompiles(unittest.TestCase):
    """match.memoize_compiles, with the compiler itself stubbed out.

    pr_linkcheck turns this on so a TU owning hundreds of functions compiles once rather
    than once per symbol. What has to hold: off by default (other callers edit headers
    between compiles), a hit only for the same source bytes and compile inputs, the
    basename kept in the key, and a timeout never remembered as an answer.
    """

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self._tmp.cleanup)
        self.tmp = pathlib.Path(self._tmp.name)
        (self.tmp / "mw" / "v1").mkdir(parents=True)
        (self.tmp / "mw" / "v1" / "mwccarm.exe").write_bytes(b"")
        self.calls = []
        self.answer = b"OBJ"
        saved = (match.MW, match._compile_c_uncached, match._COMPILE_MEMO)
        self.addCleanup(self._restore, saved)
        match.MW = self.tmp / "mw"
        match._compile_c_uncached = self._fake

    def _restore(self, saved):
        match.MW, match._compile_c_uncached, match._COMPILE_MEMO = saved

    def _fake(self, cfile, exe, version, flags, search):
        self.calls.append(pathlib.Path(cfile).name)
        return self.answer

    def src(self, name, text, sub="a"):
        d = self.tmp / sub
        d.mkdir(exist_ok=True)
        p = d / name
        p.write_text(text, encoding="utf-8")
        return p

    def test_off_by_default(self):
        match.memoize_compiles(False)
        f = self.src("x.cpp", "int f;")
        match.compile_c(f, "v1", "-O4")
        match.compile_c(f, "v1", "-O4")
        self.assertEqual(len(self.calls), 2)

    def test_same_bytes_in_a_fresh_temp_copy_is_a_hit(self):
        # reloc_audit.winning_object writes a new temp copy per symbol; that must hit.
        match.memoize_compiles()
        a = match.compile_c(self.src("x.cpp", "int f;", "t1"), "v1", "-O4")
        b = match.compile_c(self.src("x.cpp", "int f;", "t2"), "v1", "-O4")
        self.assertEqual((a, b), (b"OBJ", b"OBJ"))
        self.assertEqual(len(self.calls), 1)

    def test_changed_bytes_name_version_or_flags_miss(self):
        match.memoize_compiles()
        match.compile_c(self.src("x.cpp", "int f;"), "v1", "-O4")
        match.compile_c(self.src("x.cpp", "int g;"), "v1", "-O4")
        match.compile_c(self.src("y.cpp", "int g;"), "v1", "-O4")  # __sinit_<name>
        match.compile_c(self.src("y.cpp", "int g;"), "v1", "-O2")
        self.assertEqual(len(self.calls), 4)

    def test_failure_is_remembered_but_timeout_is_not(self):
        match.memoize_compiles()
        f = self.src("x.cpp", "int f;")
        self.answer = None
        match.compile_c(f, "v1", "-O4")
        match.compile_c(f, "v1", "-O4")
        self.assertEqual(len(self.calls), 1)
        g = self.src("z.cpp", "int z;")
        self.answer = match._TIMED_OUT
        self.assertIsNone(match.compile_c(g, "v1", "-O4"))
        self.assertIsNone(match.compile_c(g, "v1", "-O4"))
        self.assertEqual(len(self.calls), 3)


if __name__ == "__main__":
    unittest.main()
