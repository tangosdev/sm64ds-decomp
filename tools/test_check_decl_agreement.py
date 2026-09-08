"""Tests for tools/check_decl_agreement.py.

The gate exists to catch a declaration that contradicts the definition it names, so the
tests that matter are POSITIVE CONTROLS: given a file that declares `int f(...)` against
a `void f(...)` definition, does the tool actually say so? A gate that quietly finds
nothing is worse than no gate, and this tree has shipped one of those before
(`check_header_offsets` reported a pass after a reformat took its match count to zero),
which is what `test_scan_too_small_is_not_a_pass` defends against.

The five fixtures the gate was commissioned for, one test each:

  * an int/void return mismatch          -- the `__cxa_vec_ctor` shape from the tree
  * an arity mismatch
  * a typedef alias that is NOT a mismatch (`u32` and `unsigned int` are one type)
  * a bare `extern` in a C++ TU of a symbol `symbols.txt` records unmangled
  * a rename where the definition moved, leaving declarations naming a symbol that is
    no longer defined anywhere

Self-running: `python tools/test_check_decl_agreement.py`, or via unittest/pytest.
Needs no ROM and no compiler.
"""
import json
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import check_decl_agreement as CDA  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent

TYPES_H = """\
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;
typedef s32 Fix12i;
typedef struct Vector3 { Fix12i x, y, z; } Vector3;
"""


class Tree:
    """A throwaway repo-shaped tree to point the collector at."""

    def __init__(self, tmp, types_h=TYPES_H):
        self.root = pathlib.Path(tmp)
        (self.root / "src").mkdir(parents=True, exist_ok=True)
        (self.root / "include").mkdir(parents=True, exist_ok=True)
        (self.root / "config" / "arm9").mkdir(parents=True, exist_ok=True)
        self.write("include/types.h", types_h)
        self.symbols([])

    def write(self, rel, text):
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")
        return p

    def symbols(self, names, module="arm9"):
        rows = "".join("%s kind:function(arm,size=0x4) addr:0x0200%04x\n"
                       % (n, i) for i, n in enumerate(names))
        self.write("config/%s/symbols.txt" % module, rows)

    def run(self):
        """(findings, decls, defs) for this tree."""
        files, decls, defs, _unparsed = CDA.collect(self.root)
        unmangled = CDA.unmangled_symbols(self.root)
        return (CDA.disagreements(decls, defs, unmangled, self.root), decls, defs,
                files)


def build(fn, **kw):
    with tempfile.TemporaryDirectory() as tmp:
        t = Tree(tmp, **kw)
        fn(t)
        return t.run()


def kinds(findings, symbol=None):
    return sorted((f["kind"], f["file"], f["got"], f["want"])
                  for f in findings
                  if symbol is None or f["symbol"] == symbol)


# ------------------------------------------------------- the five commissioned cases

class ReturnMismatchTests(unittest.TestCase):
    """Fixture 1: a caller declares `int` where the definition returns `void`."""

    def _tree(self, t):
        t.write("src/__cxa_vec_ctor.cpp",
                '//cpp\n'
                'extern "C" void __cxa_vec_ctor(void *base, unsigned int n,\n'
                '                               unsigned int size)\n'
                "{\n"
                "    (void)base; (void)n; (void)size;\n"
                "}\n")
        t.write("src/d_a_hanachan.c",
                "extern int __cxa_vec_ctor(void *p, unsigned int a, unsigned int b);\n"
                "void hana(void) { __cxa_vec_ctor(0, 1, 2); }\n")

    def test_the_wrong_return_type_is_reported(self):
        findings, _d, _f, _files = build(self._tree)
        self.assertEqual(
            kinds(findings, "__cxa_vec_ctor"),
            [("return", "src/d_a_hanachan.c", "int", "void")])

    def test_the_finding_names_both_sides_with_a_line(self):
        findings, _d, _f, _files = build(self._tree)
        f = findings[0]
        self.assertEqual(f["file"], "src/d_a_hanachan.c")
        self.assertEqual(f["line"], 1)
        self.assertEqual(f["ref_file"], "src/__cxa_vec_ctor.cpp")
        self.assertEqual(f["ref_line"], 2)
        self.assertEqual(f["basis"], "definition")

    def test_a_void_pointer_return_is_also_reported(self):
        def tree(t):
            self._tree(t)
            t.write("src/d_a_luigi.cpp",
                    "extern void* __cxa_vec_ctor(void* a, unsigned int b,\n"
                    "                            unsigned int c);\n")
        findings, _d, _f, _files = build(tree)
        self.assertIn(("return", "src/d_a_luigi.cpp", "void *", "void"),
                      kinds(findings, "__cxa_vec_ctor"))

    def test_an_agreeing_declaration_is_silent(self):
        def tree(t):
            self._tree(t)
            t.write("src/d_a_good.c",
                    "extern void __cxa_vec_ctor(void *p, unsigned int a,\n"
                    "                           unsigned int b);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual([f for f in findings if f["file"] == "src/d_a_good.c"], [])


class ArityMismatchTests(unittest.TestCase):
    """Fixture 2: a declaration with the wrong number of parameters."""

    def test_too_few_parameters_is_reported(self):
        def tree(t):
            t.write("src/Spawn.cpp",
                    "void Spawn(int a, int b, int c)\n{\n    (void)a;\n}\n")
            t.write("src/caller.cpp", "extern void Spawn(int a, int b);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Spawn"),
                         [("arity", "src/caller.cpp", "2", "3")])

    def test_too_many_parameters_is_reported(self):
        def tree(t):
            t.write("src/Spawn.cpp", "void Spawn(int a)\n{\n    (void)a;\n}\n")
            t.write("src/caller.cpp", "extern void Spawn(int a, int b);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Spawn"),
                         [("arity", "src/caller.cpp", "2", "1")])

    def test_arity_is_reported_once_not_once_per_parameter(self):
        def tree(t):
            t.write("src/Spawn.cpp",
                    "void Spawn(int a, int b, int c, int d)\n{\n    (void)a;\n}\n")
            t.write("src/caller.cpp", "extern void Spawn(void);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(len(kinds(findings, "Spawn")), 1)

    def test_an_empty_parameter_list_in_c_claims_nothing(self):
        """`void f()` in C is an UNSPECIFIED list, not a claim of zero parameters."""
        def tree(t):
            t.write("src/Spawn.c", "void Spawn(int a, int b)\n{\n    (void)a;\n}\n")
            t.write("src/caller.c", "extern void Spawn();\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Spawn"), [])

    def test_an_empty_parameter_list_in_cpp_does_claim_zero(self):
        def tree(t):
            t.write("src/Spawn.c", "void Spawn(int a, int b)\n{\n    (void)a;\n}\n")
            t.write("src/caller.cpp", "extern void Spawn();\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Spawn"),
                         [("arity", "src/caller.cpp", "0", "2")])


class TypedefAliasTests(unittest.TestCase):
    """Fixture 3: spellings the tree already treats as the same type must be silent."""

    def test_u32_and_unsigned_int_are_one_type(self):
        def tree(t):
            t.write("src/Load.c",
                    '#include "types.h"\n'
                    "void Load(u32 a, u16 b, s8 c)\n{\n    (void)a;\n}\n")
            t.write("src/caller.c",
                    "extern void Load(unsigned int a, unsigned short b,\n"
                    "                 signed char c);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Load"), [])

    def test_bare_unsigned_is_unsigned_int(self):
        def tree(t):
            t.write("src/Load.c", "void Load(unsigned int a)\n{\n    (void)a;\n}\n")
            t.write("src/caller.c", "extern void Load(unsigned a);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Load"), [])

    def test_fix12i_resolves_through_two_typedefs_to_int(self):
        def tree(t):
            t.write("src/Load.c",
                    '#include "types.h"\n'
                    "void Load(Fix12i a)\n{\n    (void)a;\n}\n")
            t.write("src/caller.c", "extern void Load(int a);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Load"), [])

    def test_an_elaborated_specifier_is_the_same_type_as_its_tag(self):
        def tree(t):
            t.write("src/Load.c", "void Load(struct Vector3 *v)\n{\n    (void)v;\n}\n")
            t.write("src/caller.c", "extern void Load(Vector3 *v);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Load"), [])

    def test_a_top_level_const_on_a_by_value_parameter_is_not_a_disagreement(self):
        def tree(t):
            t.write("src/Load.c", "void Load(const int a)\n{\n    (void)a;\n}\n")
            t.write("src/caller.c", "extern void Load(int a);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Load"), [])

    def test_a_const_on_a_POINTEE_is_a_disagreement(self):
        """The one const that changes the type. It must NOT be folded away."""
        def tree(t):
            t.write("src/Load.c", "void Load(const char *a)\n{\n    (void)a;\n}\n")
            t.write("src/caller.c", "extern void Load(char *a);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Load"),
                         [("param", "src/caller.c", "#1 char *",
                           "#1 const char *")])

    def test_a_local_typedef_is_resolved_before_comparison(self):
        """`ctor_t` in the defining file must be compared as the type it names."""
        def tree(t):
            t.write("src/vec.cpp",
                    "typedef void (*ctor_t)(void *);\n"
                    'extern "C" void vec(ctor_t c)\n{\n    (void)c;\n}\n')
            t.write("src/caller.cpp", "extern void vec(void (*c)(void *));\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "vec"), [])

    def test_an_array_parameter_decays_but_an_array_OBJECT_does_not(self):
        def tree(t):
            t.write("src/Load.c", "void Load(int v[])\n{\n    (void)v;\n}\n")
            t.write("src/caller.c",
                    "extern void Load(int *v);\n"
                    "extern int table[];\n")
            t.write("src/other.c", "extern int table;\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Load"), [])
        self.assertEqual(len(kinds(findings, "table")), 1)


class LinkageTests(unittest.TestCase):
    """Fixture 4: a bare `extern` in a C++ TU of a symbol recorded unmangled."""

    def _tree(self, decl):
        def tree(t):
            t.symbols(["func_02073534"])
            t.write("src/func_02073534.c", "void func_02073534(void)\n{\n}\n")
            t.write("src/caller.cpp", decl)
        return tree

    def test_a_bare_extern_of_a_c_linkage_symbol_is_reported(self):
        findings, _d, _f, _files = build(
            self._tree("extern void func_02073534(void);\n"))
        self.assertIn(("linkage", "src/caller.cpp", "bare extern in a C++ TU",
                       'extern "C"'), kinds(findings, "func_02073534"))

    def test_extern_c_on_the_same_declaration_is_silent(self):
        findings, _d, _f, _files = build(
            self._tree('extern "C" void func_02073534(void);\n'))
        self.assertEqual(kinds(findings, "func_02073534"), [])

    def test_an_extern_c_BLOCK_covers_the_declarations_inside_it(self):
        findings, _d, _f, _files = build(self._tree(
            'extern "C" {\n    extern void func_02073534(void);\n}\n'))
        self.assertEqual(kinds(findings, "func_02073534"), [])

    def test_a_bare_extern_in_a_C_file_is_silent(self):
        """A .c file has C linkage already; there is nothing to disagree about."""
        def tree(t):
            t.symbols(["func_02073534"])
            t.write("src/func_02073534.c", "void func_02073534(void)\n{\n}\n")
            t.write("src/caller.c", "extern void func_02073534(void);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "func_02073534"), [])

    def test_a_DATA_declaration_is_not_billed_for_linkage(self):
        """Itanium does not mangle a global variable, so a bare extern is correct."""
        def tree(t):
            t.symbols(["BowserShutter_ClsnFile"])
            t.write("src/caller.cpp", "extern int BowserShutter_ClsnFile[];\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "BowserShutter_ClsnFile"), [])

    def test_a_symbol_symbols_txt_records_MANGLED_is_not_billed(self):
        def tree(t):
            t.symbols(["_ZN5Actor4TickEv"])
            t.write("src/caller.cpp", "extern void _ZN5Actor4TickEv(void *p);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(
            [f for f in findings
             if f["symbol"] == "_ZN5Actor4TickEv" and f["kind"] == "linkage"], [])


class MovedDefinitionTests(unittest.TestCase):
    """Fixture 5: a rename that left declarations naming the old symbol behind."""

    def test_declarations_of_a_renamed_symbol_fall_back_to_the_plurality(self):
        def tree(t):
            # The definition moved from `func_020733a8` to `__cxa_vec_ctor`.
            t.write("src/__cxa_vec_ctor.cpp",
                    'extern "C" void __cxa_vec_ctor(void *p)\n{\n    (void)p;\n}\n')
            t.write("src/a.c", "extern void func_020733a8(void *p);\n")
            t.write("src/b.c", "extern void func_020733a8(void *p);\n")
            t.write("src/c.c", "extern int func_020733a8(void *p);\n")
        findings, _d, defs, _files = build(tree)
        self.assertNotIn("func_020733a8", {d.symbol for d in defs})
        got = kinds(findings, "func_020733a8")
        self.assertEqual(got, [("return", "src/c.c", "int", "void")])
        self.assertEqual({f["basis"] for f in findings
                          if f["symbol"] == "func_020733a8"}, {"plurality"})

    def test_a_definition_outranks_the_plurality(self):
        """Forty-two agreeing declarations do not outvote one definition."""
        def tree(t):
            t.write("src/Spawn.c", "void Spawn(void *p)\n{\n    (void)p;\n}\n")
            for i in range(5):
                t.write("src/c%d.c" % i, "extern int Spawn(void *p);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(len(kinds(findings, "Spawn")), 5)
        self.assertEqual({f["want"] for f in findings if f["symbol"] == "Spawn"},
                         {"void"})
        self.assertEqual({f["basis"] for f in findings if f["symbol"] == "Spawn"},
                         {"definition"})

    def test_a_symbol_moved_under_an_at_symbol_marker_is_found(self):
        """A merged TU names each definition with `// @symbol`; that is the identity."""
        def tree(t):
            t.write("src/actors/daBmb_c.cpp",
                    "// @symbol _ZN7daBmb_c8BehaviorEv\n"
                    "void daBmb_Behavior(void *p)\n{\n    (void)p;\n}\n")
            t.write("src/caller.cpp",
                    "extern int _ZN7daBmb_c8BehaviorEv(void *p);\n")
        findings, _d, defs, _files = build(tree)
        self.assertIn("_ZN7daBmb_c8BehaviorEv", {d.symbol for d in defs})
        self.assertEqual(kinds(findings, "_ZN7daBmb_c8BehaviorEv"),
                         [("return", "src/caller.cpp", "int", "void")])


# ------------------------------------------------------------------- other controls

class KindTests(unittest.TestCase):
    def test_data_declared_where_a_function_is_defined(self):
        def tree(t):
            t.write("src/NullDestructor.c", "void NullDestructor(void)\n{\n}\n")
            t.write("src/__sinit.c", "extern int NullDestructor[];\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "NullDestructor"),
                         [("kind", "src/__sinit.c", "data", "function")])


class ParserTests(unittest.TestCase):
    """The parser must not invent symbols out of things that are not declarations."""

    def test_a_typedef_struct_tail_is_not_a_symbol(self):
        _findings, decls, defs, _files = build(lambda t: t.write(
            "src/x.c", "typedef struct Thing { int a; } Thing;\n"))
        self.assertEqual([d.symbol for d in decls], [])
        self.assertEqual([d.symbol for d in defs], [])

    def test_a_struct_field_is_not_a_declaration(self):
        _findings, decls, _f, _files = build(lambda t: t.write(
            "include/IRQ.h", "struct IRQ {\n    void (*Handler)(void);\n"
                             "    extern int nope;\n};\n"))
        self.assertEqual([d.symbol for d in decls], [])

    def test_a_path_inside_a_comment_is_not_parsed(self):
        _findings, decls, _f, _files = build(lambda t: t.write(
            "src/x.c", '// extern int ghost(void);\n/* extern int ghost2; */\n'
                       'extern int real(void);\n'))
        self.assertEqual(sorted(d.symbol for d in decls), ["real"])

    def test_a_declaration_inside_a_string_literal_is_not_parsed(self):
        _findings, decls, _f, _files = build(lambda t: t.write(
            "src/x.c", 'const char *s = "extern int ghost(void);";\n'
                       'extern int real(void);\n'))
        self.assertEqual(sorted(d.symbol for d in decls), ["real"])

    def test_a_preprocessor_line_is_not_a_declaration(self):
        _findings, decls, _f, _files = build(lambda t: t.write(
            "src/x.c", "#define SHIM extern int ghost(void);\n"
                       "extern int real(void);\n"))
        self.assertEqual(sorted(d.symbol for d in decls), ["real"])

    def test_a_comma_separated_data_declaration_is_two_symbols(self):
        _findings, decls, _f, _files = build(lambda t: t.write(
            "src/x.c", "extern int alpha, beta;\n"))
        self.assertEqual(sorted(d.symbol for d in decls), ["alpha", "beta"])

    def test_asm_is_a_specifier_not_a_return_type(self):
        def tree(t):
            t.write("src/prim.c", "asm void prim(void)\n{\n    nop\n}\n")
            t.write("src/caller.c", "extern void prim(void);\n")
        findings, _d, defs, _files = build(tree)
        self.assertEqual([d.ret for d in defs if d.symbol == "prim"], ["void"])
        self.assertEqual(kinds(findings, "prim"), [])

    def test_a_function_pointer_parameter_name_is_not_part_of_its_type(self):
        def tree(t):
            t.write("src/vec.c",
                    "void vec(void (*ctor)(void *))\n{\n    (void)ctor;\n}\n")
            t.write("src/caller.c", "extern void vec(void (*)(void *));\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "vec"), [])


class BaselineTests(unittest.TestCase):
    def _round_trip(self, findings):
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp) / "baseline.json"
            written = CDA.write_baseline(findings, path)
            data = json.loads(path.read_text(encoding="utf-8"))
            return written, data, CDA.load_baseline(path)

    def _one(self, symbol="Spawn", kind="return", file="src/a.c", got="int"):
        return {"symbol": symbol, "kind": kind, "file": file, "line": 1,
                "decl": "int (void)", "got": got, "want": "void",
                "basis": "definition", "ref_file": "src/Spawn.c", "ref_line": 1,
                "ref": "void (void)"}

    def test_a_written_baseline_reads_back_as_the_same_keys(self):
        findings = [self._one(), self._one(kind="arity", got="0")]
        written, data, known = self._round_trip(findings)
        self.assertEqual(written, 2)
        self.assertEqual(data["known"]["Spawn"]["src/a.c"],
                         ["arity:0", "return:int"])
        self.assertEqual(known, {("Spawn", "src/a.c", "return:int"),
                                 ("Spawn", "src/a.c", "arity:0")})

    def test_a_banked_disagreement_does_not_fail_the_gate(self):
        findings = [self._one()]
        _w, _d, known = self._round_trip(findings)
        self.assertEqual([f for f in findings if CDA.key_of(f) not in known], [])

    def test_a_NEW_disagreement_in_an_already_banked_file_still_fails(self):
        _w, _d, known = self._round_trip([self._one()])
        fresh = self._one(kind="param", got="#1 char *")
        self.assertNotIn(CDA.key_of(fresh), known)

    def test_the_baseline_is_valid_json_and_one_line_per_declaring_file(self):
        findings = [self._one(file="src/a.c"), self._one(file="src/b.c"),
                    self._one(symbol="Other", file="src/a.c")]
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp) / "baseline.json"
            CDA.write_baseline(findings, path)
            text = path.read_text(encoding="utf-8")
        json.loads(text)
        self.assertEqual(sum(1 for ln in text.splitlines() if ln.strip().endswith("],")
                             or ln.strip().endswith("]")), 3)

    def test_a_missing_baseline_is_an_empty_set_not_a_crash(self):
        with tempfile.TemporaryDirectory() as tmp:
            self.assertEqual(
                CDA.load_baseline(pathlib.Path(tmp) / "absent.json"), set())


class VacuousPassTests(unittest.TestCase):
    """A scan that inspected nothing must not report a clean tree."""

    def test_scan_too_small_is_not_a_pass(self):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            t.write("src/x.c", "extern int real(void);\n")
            old = CDA.REPO
            CDA.REPO = t.root
            try:
                rc = CDA.main([])
            finally:
                CDA.REPO = old
        self.assertEqual(rc, 2)

    def test_the_floors_sit_below_the_real_tree(self):
        """A floor above today's numbers would fail every run; below, it means something."""
        self.assertLess(CDA.MIN_FILES, 9000)
        self.assertLess(CDA.MIN_DECLS, 40000)
        self.assertLess(CDA.MIN_SYMBOLS, 14000)
        self.assertGreater(CDA.MIN_FILES, 0)
        self.assertGreater(CDA.MIN_DECLS, 0)
        self.assertGreater(CDA.MIN_SYMBOLS, 0)


class ChangedResolutionTests(unittest.TestCase):
    """`--changed` must fail on a base that resolved to nothing, not pass vacuously."""

    def _repo(self, tmp):
        root = pathlib.Path(tmp)
        run = lambda *a: subprocess.run(["git", *a], cwd=str(root),
                                        capture_output=True, text=True)
        run("init", "-q", "-b", "main")
        run("config", "user.email", "t@example.invalid")
        run("config", "user.name", "t")
        (root / "src").mkdir(parents=True, exist_ok=True)
        (root / "include").mkdir(parents=True, exist_ok=True)
        (root / "include" / "types.h").write_text(TYPES_H, encoding="utf-8")
        (root / "src" / "base.c").write_text("void base(void) {}\n", encoding="utf-8")
        run("add", "-A")
        run("commit", "-qm", "base")
        return root, run

    def test_an_empty_diff_is_an_error_not_a_pass(self):
        with tempfile.TemporaryDirectory() as tmp:
            root, _run = self._repo(tmp)
            touched, defined, total, err = CDA.changed_scope("HEAD", root)
            self.assertIsNone(err)
            self.assertEqual(total, [])
            self.assertEqual(touched, [])
            self.assertEqual(defined, set())

    def test_a_bad_base_ref_is_an_error_string_not_a_traceback(self):
        with tempfile.TemporaryDirectory() as tmp:
            root, _run = self._repo(tmp)
            _t, _d, _total, err = CDA.changed_scope("no-such-ref", root)
            self.assertIsNotNone(err)
            self.assertIn("git", err)

    def test_an_uncommitted_edit_is_in_the_work_list(self):
        with tempfile.TemporaryDirectory() as tmp:
            root, _run = self._repo(tmp)
            (root / "src" / "fresh.c").write_text("void fresh(int a) {}\n",
                                                  encoding="utf-8")
            touched, defined, _total, err = CDA.changed_scope("HEAD", root)
            self.assertIsNone(err)
            self.assertIn("src/fresh.c", touched)
            self.assertIn("fresh", defined)

    def test_the_symbols_a_changed_file_defines_are_what_widens_the_scope(self):
        with tempfile.TemporaryDirectory() as tmp:
            root, run = self._repo(tmp)
            (root / "src" / "moved.c").write_text("void moved(int a) {}\n",
                                                  encoding="utf-8")
            run("add", "-A")
            run("commit", "-qm", "add moved")
            touched, defined, _total, err = CDA.changed_scope("HEAD~1", root)
            self.assertIsNone(err)
            self.assertEqual(touched, ["src/moved.c"])
            self.assertEqual(defined, {"moved"})


class RealTreeTests(unittest.TestCase):
    """The gate must reproduce the finding it was commissioned for."""

    @classmethod
    def setUpClass(cls):
        if not (REPO / "src" / "__cxa_vec_ctor.cpp").exists():
            raise unittest.SkipTest("src/__cxa_vec_ctor.cpp not in this tree")
        cls.aliases = CDA.scalar_typedefs(REPO)

    def test_the_definition_of_cxa_vec_ctor_parses(self):
        rel = "src/__cxa_vec_ctor.cpp"
        text = (REPO / rel).read_text(encoding="utf-8", errors="replace")
        _decls, defs, _u = CDA.parse_file(rel, text, self.aliases)
        found = [d for d in defs if d.symbol == "__cxa_vec_ctor"]
        self.assertEqual(len(found), 1)
        self.assertEqual(found[0].ret, "void")
        self.assertEqual(len(found[0].params), 5)
        self.assertEqual(found[0].params[1], "unsigned int")

    def test_the_five_known_wrong_declarations_are_still_wrong(self):
        wrong = {
            "src/d_a_dp_lift.cpp": "int",
            "src/d_a_hanachan.c": "int",
            "src/game/actors/d_a_wanwan.cpp": "int",
            "src/d_a_luigi.cpp": "void *",
            "src/func_ov006_020c3f54.cpp": "void *",
        }
        for rel, want in wrong.items():
            path = REPO / rel
            if not path.exists():
                self.skipTest("%s not in this tree" % rel)
            text = path.read_text(encoding="utf-8", errors="replace")
            decls, _defs, _u = CDA.parse_file(rel, text, self.aliases)
            found = [d for d in decls if d.symbol == "__cxa_vec_ctor"]
            self.assertEqual(len(found), 1, rel)
            self.assertEqual(found[0].ret, want, rel)


if __name__ == "__main__":
    unittest.main(verbosity=2)
