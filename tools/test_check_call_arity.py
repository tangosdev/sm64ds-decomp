"""Regression tests for tools/check_call_arity.py.

Two kinds of case, and the second kind is the one that matters:

  1. It finds the ride-through (a call that passes fewer arguments than the
     callee's own definition declares).
  2. It stays SILENT on the many shapes that only look like one.

The first run of this census over src/ produced 2,623 rows and 269 callees, and
every large group in it was the tool misreading the tree: function-like macros
(`#define H(a, b)`) read as two-parameter functions, method declarations inside
a class body read as zero-argument calls, `ptr.LoadFile()` scored against the
free function of the same name, K&R `extern int f();` written inside a function
body read as a zero-argument call, and -- the biggest -- static member functions
charged for a `this` they do not take. Each of those has a test below asserting
silence, because a census nobody trusts is worse than no census: the whole value
is that a row is worth reading the ROM for.
"""
import contextlib
import io
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import check_call_arity as A


class Tree:
    """A throwaway src/ tree, plus an optional include/ tree."""

    def __init__(self, sources, headers=None):
        self.dir = tempfile.TemporaryDirectory()
        root = pathlib.Path(self.dir.name)
        (root / "src").mkdir()
        for name, text in sources.items():
            (root / "src" / name).write_text(text, encoding="utf-8")
        (root / "include").mkdir()
        for name, text in (headers or {}).items():
            (root / "include" / name).write_text(text, encoding="utf-8")
        self.root = root

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.dir.cleanup()

    def rows(self, statics=None):
        paths = sorted((self.root / "src").iterdir())
        if statics is None:
            statics = A.static_methods(self.root)
        defs, decls, calls, _n, _c = A.build(paths, statics)
        return A.census(defs, decls, calls)


RIDE_CALLER = """\
extern int func_0205d644(void);
int func_02018e3c(void){
  return func_0205d644();
}
"""
RIDE_CALLEE = """\
int func_0205d644(void *out, const char *path){
  return path != 0 && out != 0;
}
"""


class TestFindsTheShape(unittest.TestCase):
    def test_zero_argument_call_to_a_two_parameter_definition(self):
        with Tree({"func_02018e3c.c": RIDE_CALLER,
                   "func_0205d644.c": RIDE_CALLEE}) as t:
            rows = t.rows()
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0]["caller"], "func_02018e3c")
        self.assertEqual(rows[0]["callee"], "func_0205d644")
        self.assertEqual((rows[0]["passed"], rows[0]["declared"]), (0, 2))
        self.assertEqual(rows[0]["source"], "DEF")

    def test_partial_call_is_found_too(self):
        with Tree({"a.c": "extern int b(int);\nint a(int x){ return b(x); }\n",
                   "b.c": "int b(int x, int y){ return x + y; }\n"}) as t:
            rows = t.rows()
        self.assertEqual([(r["passed"], r["declared"]) for r in rows], [(1, 2)])

    def test_the_line_number_is_the_real_one(self):
        src = "extern int b(void);\n\n\n\nint a(void){\n  return b();\n}\n"
        with Tree({"a.c": src, "b.c": "int b(int x){ return x; }\n"}) as t:
            rows = t.rows()
        self.assertEqual(rows[0]["line"], 6)

    def test_a_cpp_method_is_charged_for_this(self):
        with Tree({"_ZN5Thing3RunEi.cpp":
                   "//cpp\nvoid Thing::Run(int n)\n{\n  (void)n;\n}\n",
                   "caller.c": "extern int _ZN5Thing3RunEi();\n"
                               "void caller(void *self){ _ZN5Thing3RunEi(self); }\n"}) as t:
            rows = t.rows()
        self.assertEqual([(r["callee"], r["passed"], r["declared"]) for r in rows],
                         [("_ZN5Thing3RunEi", 1, 2)])


class TestStaysSilent(unittest.TestCase):
    def test_a_static_method_is_not_charged_for_this(self):
        """The 800-row failure: Model::LoadFile and friends take no `this`."""
        headers = {"Thing.h": "struct Thing {\n    static void Run(int n);\n};\n"}
        with Tree({"_ZN5Thing3RunEi.cpp": "//cpp\nvoid Thing::Run(int n)\n{\n  (void)n;\n}\n",
                   "caller.c": "extern int _ZN5Thing3RunEi(int);\n"
                               "void caller(void){ _ZN5Thing3RunEi(3); }\n"},
                  headers) as t:
            self.assertEqual(t.rows(), [])

    def test_a_static_marker_written_in_the_cpp_itself_counts(self):
        """class Sound { static void StopLoadedMusic_Layer1(u32); }; lives in src/."""
        with Tree({"_ZN5Sound4StopEj.cpp":
                   "//cpp\nclass Sound {\npublic:\n    static void Stop(unsigned int a);\n};\n"
                   "void Sound::Stop(unsigned int a) { (void)a; }\n",
                   "caller.c": "extern int _ZN5Sound4StopEj(unsigned int);\n"
                               "void caller(void){ _ZN5Sound4StopEj(1); }\n"}) as t:
            self.assertEqual(t.rows(), [])

    def test_a_function_like_macro_is_not_a_definition(self):
        """`#define H(a, b) ...` made every H(x) call a one-of-two row."""
        with Tree({"a.c": "#define H(a, b) ((a) + (b))\n"
                          "int a(int x){ return H(x, 1); }\n",
                   "H.c": "int H(int a, int b){ return a + b; }\n"}) as t:
            rows = t.rows()
        self.assertEqual(rows, [])

    def test_a_method_declared_in_a_class_body_is_not_a_call(self):
        with Tree({"a.cpp": "//cpp\nstruct P {\n    void Run();\n};\n"
                            "void a(P *p){ p->Run(); }\n",
                   "Run.c": "int Run(int x){ return x; }\n"}) as t:
            self.assertEqual(t.rows(), [])

    def test_a_member_call_is_not_the_free_function(self):
        with Tree({"a.cpp": "//cpp\nstruct P { void Run(); };\n"
                            "void a(P *p){ p->Run(); P q; q.Run(); }\n",
                   "Run.c": "int Run(int x){ return x; }\n"}) as t:
            self.assertEqual(t.rows(), [])

    def test_a_local_extern_prototype_is_not_a_zero_argument_call(self):
        """This tree writes `extern int f();` INSIDE bodies, then calls f(c)."""
        with Tree({"a.c": "int a(void *c){\n  extern int b();\n  return b(c);\n}\n",
                   "b.c": "int b(void *c){ return c != 0; }\n"}) as t:
            self.assertEqual(t.rows(), [])

    def test_passing_more_than_declared_is_not_this_tools_business(self):
        with Tree({"a.c": "extern int b(int, int);\nint a(void){ return b(1, 2); }\n",
                   "b.c": "int b(int x){ return x; }\n"}) as t:
            self.assertEqual(t.rows(), [])

    def test_a_kandr_definition_is_unknown_not_zero(self):
        """`f()` means unspecified. It cannot score a call, and it is counted."""
        with Tree({"a.c": "extern int b();\nint a(void){ return b(); }\n",
                   "b.c": "int b(){ return 1; }\n"}) as t:
            paths = sorted((t.root / "src").iterdir())
            defs, decls, calls, _n, _c = A.build(paths, set())
            self.assertEqual(A.census(defs, decls, calls), [])
            self.assertIn("b", A.unknown_arity(defs, decls))

    def test_a_nonmatching_file_is_marked_a_draft(self):
        with Tree({"func_02018e3c.c": "// NONMATCHING\n" + RIDE_CALLER,
                   "func_0205d644.c": RIDE_CALLEE}) as t:
            rows = t.rows()
        self.assertEqual([r["draft"] for r in rows], [True])

    def test_a_string_or_comment_cannot_make_a_row(self):
        src = ('/* func_0205d644(); in prose */\n'
               'extern int func_0205d644(void *, const char *);\n'
               'int a(void){ const char *s = "func_0205d644()"; return s != 0; }\n')
        with Tree({"a.c": src, "func_0205d644.c": RIDE_CALLEE}) as t:
            self.assertEqual(t.rows(), [])

    def test_a_recursive_call_is_not_scored_against_itself(self):
        with Tree({"a.c": "int a(int x, int y){ return x ? a(x - 1) : y; }\n"}) as t:
            self.assertEqual(t.rows(), [])


class TestPlumbing(unittest.TestCase):
    def test_blank_preserves_length_and_lines(self):
        src = 'int a; /* two\nlines */ char *s = "x{y";\n// tail\n'
        out = A.blank(src)
        self.assertEqual(len(out), len(src))
        self.assertEqual(out.count("\n"), src.count("\n"))
        self.assertNotIn("{", out)

    def test_brace_kinds_separates_bodies_from_types(self):
        src = "struct P {\n int a;\n};\nvoid f(int x) {\n g();\n}\n"
        kinds = A.brace_kinds(src)
        self.assertEqual(kinds[src.index("int a")], "type")
        self.assertEqual(kinds[src.index("g()")], "func")
        self.assertEqual(kinds[src.index("struct")], "")

    def test_brace_kinds_reads_an_extern_c_block_as_declarations(self):
        src = 'extern "C" {\nvoid f(int);\n}\n'
        self.assertNotEqual(A.brace_kinds(src)[src.index("void f")], "func")

    def test_main_prints_the_census_and_exits_zero(self):
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            rc = A.main(["--callee", "no_such_symbol_anywhere"])
        self.assertEqual(rc, 0)
        self.assertIn("calls passing fewer arguments", buf.getvalue())


if __name__ == "__main__":
    unittest.main()
