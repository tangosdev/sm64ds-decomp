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

The false-pass paths reported on PR #2471 get one class each at the bottom of this file.
They are `--changed` cases, so they run on a git repo past the tool's scan floors (2,100
sources, 6,300-odd declarations) rather than a two-file toy, which the tool refuses to
report a pass on at all -- and each one is written so it FAILS on the code as it was
before its fix. The first review reported four:

  * a shared typedef changed, invalidating a consumer the diff never touched
  * a `config/**/symbols.txt` row changed with no source path in the diff
  * a rename whose definition changed signature in the same commit
  * an `extern int target(void)` against an actual `int target = 0` definition

and the re-review four more:

  * `int prefix = 0, target = 0;` -- everything after the first `=` was thrown away
  * `int target;` -- an uninitialised external object was dropped from both lists
  * `const int t = 0;` in C++ and `namespace P { int t = 0; }` wrongly claiming the
    GLOBAL name `t`, which fabricates a finding rather than hiding one
  * a deleted `include/types.h`, which `_paths_of` discarded with every other `D` row

Seventeen assertions across those last four classes FAIL on the parser as it was. The
controls sitting beside them -- an `extern` object, a default argument, a pointer to
const, `extern "C"` inside a namespace, an `@symbol`-marked namespace definition, a
deleted SOURCE staying out of scope -- pass in BOTH states, which is what says the fixes
narrowed the right thing instead of switching a side off.

`BigTreeHarnessTests.test_a_wrong_return_type_fails_both_modes` is the overall control:
it fails both modes before every one of these fixes and after them, so a green run is
not a harness that stopped looking.

Self-running: `python tools/test_check_decl_agreement.py`, or via unittest/pytest.
Needs no ROM and no compiler.
"""
import io
import json
import pathlib
import re
import shutil
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


class OpaquePointerTests(unittest.TestCase):
    """`void *` declines to answer; it does not contradict another pointer.

    Half this tree is still unpromoted C shards that reconstruct an object as an
    opaque `char buf[0x50]` and have no type to name, so they spell every object
    pointer `void *`. Promoting ONE caller to the real C++ type flips the
    plurality, and without this the gate reds every shard left behind -- nine
    files for `dBgCh_Gnd`'s constructor alone, not one of them touched by the PR
    that flipped it. The gate would be measuring the promotion campaign's
    progress rather than a defect.
    """

    def test_void_star_against_a_named_pointer_is_silent(self):
        def tree(t):
            t.write("src/Init.cpp",
                    "void Init(dBgCh_Gnd *self)\n{\n    (void)self;\n}\n")
            t.write("src/shard.c", "extern void Init(void *self);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Init"), [])

    def test_it_is_silent_in_both_directions(self):
        def tree(t):
            t.write("src/Init.cpp", "void Init(void *self)\n{\n    (void)self;\n}\n")
            t.write("src/shard.c", "extern void Init(dBgCh_Gnd *self);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Init"), [])

    def test_two_DIFFERENT_named_pointees_still_contradict(self):
        """The silence is about `void *` only. Two real claims still disagree."""
        def tree(t):
            t.write("src/Init.cpp",
                    "void Init(dBgCh_Gnd *self)\n{\n    (void)self;\n}\n")
            t.write("src/shard.cpp", "extern void Init(dActor_c *self);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Init"),
                         [("param", "src/shard.cpp",
                           "#1 dActor_c *", "#1 dBgCh_Gnd *")])

    def test_void_star_against_a_NON_pointer_still_contradicts(self):
        """`int` is a real claim, so one of the two sides is genuinely wrong."""
        def tree(t):
            t.write("src/Init.cpp", "void Init(void *self)\n{\n    (void)self;\n}\n")
            t.write("src/shard.c", "extern void Init(int self);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Init"),
                         [("param", "src/shard.c", "#1 int", "#1 void *")])

    def test_a_void_star_RETURN_is_not_covered(self):
        """Only parameters. A return type is the callee's own statement, not a
        placeholder the shard was forced into by having no type to name."""
        def tree(t):
            t.write("src/Get.cpp", "dBgCh_Gnd *Get(void)\n{\n    return 0;\n}\n")
            t.write("src/shard.c", "extern void *Get(void);\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Get"),
                         [("return", "src/shard.c", "void *", "dBgCh_Gnd *")])


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


class AdoptedSymbolMarkTests(unittest.TestCase):
    """A delinked shard's `@symbol` line names its definition even from the top.

    The mark scan only honours a mark sitting between the previous statement and
    the declarator. A converted shard that declares a helper above its definition
    -- which the converted ones routinely do -- pushed its mark out of window, and
    the definition was then discarded as unnameable. 1180 of this tree's real C++
    definitions were invisible to the gate that way, so `_reference()` fell back to
    a plurality vote among the unconverted shards the conversion campaign exists to
    retire.
    """

    CONVERTED = '''// @symbol _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c
#include "types.h"

extern "C" void func_020374d4(void *ray, const Vector3 *pos);

void dBgCh_Gnd::SetObjAndPos(const Vector3 &vec_, void *actor_)
{
    func_020374d4(this, &vec_);
}
'''

    def test_a_helper_declaration_does_not_cost_the_definition_its_name(self):
        def tree(t):
            t.write("src/_ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c.cpp",
                    self.CONVERTED)
        _findings, _d, defs, _files = build(tree)
        self.assertIn("_ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c",
                      {d.symbol for d in defs})

    def test_the_definition_outranks_a_contradicting_plurality(self):
        """Twenty shards spelling a pointer do not outvote the reference that the
        ROM's own mangled name (`RK7Vector3`) spells."""
        sym = "_ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c"
        def tree(t):
            t.write("src/%s.cpp" % sym, self.CONVERTED)
            for i in range(20):
                t.write("src/shard%d.c" % i,
                        '#include "types.h"\n'
                        "extern void %s(void *self, Vector3 *pos, void *actor);\n"
                        % sym)
        findings, _d, _f, _files = build(tree)
        basis = {f["basis"] for f in findings if f["symbol"] == sym}
        self.assertEqual(basis, {"definition"})
        self.assertEqual({f["want"] for f in findings if f["symbol"] == sym},
                         {"#2 const Vector3 &"})

    def test_two_marks_in_one_file_adopt_nothing(self):
        """Two marks say nothing about which belongs to which definition."""
        def tree(t):
            t.write("src/pair.cpp",
                    '// @symbol _ZN1A1fEv\n'
                    '// @symbol _ZN1A1gEv\n'
                    '#include "types.h"\n'
                    'extern "C" void helper(void);\n'
                    'void A::f(void)\n{\n    helper();\n}\n')
        _findings, _d, defs, _files = build(tree)
        self.assertNotIn("_ZN1A1fEv", {d.symbol for d in defs})
        self.assertNotIn("_ZN1A1gEv", {d.symbol for d in defs})

    def test_two_unnamed_definitions_adopt_nothing(self):
        def tree(t):
            t.write("src/two.cpp",
                    '// @symbol _ZN1A1fEv\n'
                    '#include "types.h"\n'
                    'extern "C" void helper(void);\n'
                    'void A::f(void)\n{\n    helper();\n}\n'
                    'void A::g(void)\n{\n    helper();\n}\n')
        _findings, _d, defs, _files = build(tree)
        self.assertNotIn("_ZN1A1fEv", {d.symbol for d in defs})

    def test_a_mark_already_claimed_is_not_handed_out_twice(self):
        """The in-window mark named one definition; the orphan does not take it."""
        def tree(t):
            t.write("src/claimed.cpp",
                    '#include "types.h"\n'
                    'extern "C" void helper(void);\n'
                    '// @symbol _ZN1A1fEv\n'
                    'void A::f(void)\n{\n    helper();\n}\n')
        _findings, _d, defs, _files = build(tree)
        owners = [d for d in defs if d.symbol == "_ZN1A1fEv"]
        self.assertEqual(len(owners), 1)

    def test_an_unmarked_member_definition_still_claims_nothing(self):
        """No `@symbol` anywhere: the linker name is genuinely unrecoverable."""
        def tree(t):
            t.write("src/bare.cpp",
                    '#include "types.h"\n'
                    'void A::f(void)\n{\n}\n')
        _findings, _d, defs, _files = build(tree)
        self.assertEqual([d for d in defs if "::" in (d.symbol or "")], [])


class EastConstTests(unittest.TestCase):
    """`Vector3 const &` and `const Vector3 &` spell one type.

    The gate compared the two as text, so a converted definition writing the type
    one way contradicted a caller writing it the other. That false positive only
    became load-bearing once the adoption pass above let definitions be seen.
    """

    def test_east_const_and_west_const_are_one_type(self):
        def tree(t):
            t.write("src/def.cpp",
                    '#include "types.h"\n'
                    'extern "C" void Take(const Vector3 &v)\n{\n    (void)v;\n}\n')
            t.write("src/caller.cpp",
                    '#include "types.h"\n'
                    'extern "C" void Take(Vector3 const &v);\n')
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "Take"), [])

    def test_a_pointer_level_const_is_still_its_own_type(self):
        """`T * const *` is a pointer to a const pointer; it is not `const T **`."""
        def tree(t):
            t.write("src/def.cpp",
                    '#include "types.h"\n'
                    'extern "C" void Take(Vector3 * const *v)\n{\n    (void)v;\n}\n')
            t.write("src/caller.cpp",
                    '#include "types.h"\n'
                    'extern "C" void Take(const Vector3 **v);\n')
        findings, _d, _f, _files = build(tree)
        self.assertEqual(len(kinds(findings, "Take")), 1)

    def test_const_still_contradicts_non_const(self):
        def tree(t):
            t.write("src/def.cpp",
                    '#include "types.h"\n'
                    'extern "C" void Take(const Vector3 *v)\n{\n    (void)v;\n}\n')
            t.write("src/caller.cpp",
                    '#include "types.h"\n'
                    'extern "C" void Take(Vector3 *v);\n')
        findings, _d, _f, _files = build(tree)
        self.assertEqual(len(kinds(findings, "Take")), 1)


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

    def test_a_trailing_gnu_attribute_is_not_part_of_the_declaration(self):
        def tree(t):
            t.write("src/f.c", "void f(int a)\n{\n    (void)a;\n}\n")
            t.write("src/caller.c",
                    "extern void f(int a) __attribute__((long_call, target(\"arm\")));\n")
        findings, decls, _f, _files = build(tree)
        self.assertEqual([d.spelling() for d in decls if d.symbol == "f"],
                         ["void (int)"])
        self.assertEqual(kinds(findings, "f"), [])

    def test_a_redundantly_parenthesised_name_is_the_same_declaration(self):
        def tree(t):
            t.write("src/g.c", "char *g(int a)\n{\n    (void)a; return 0;\n}\n")
            t.write("src/caller.c", "extern char* (g)(int);\n")
        findings, decls, _f, _files = build(tree)
        self.assertEqual([d.spelling() for d in decls if d.symbol == "g"],
                         ["char * (int)"])
        self.assertEqual(kinds(findings, "g"), [])

    def test_a_pointer_to_member_array_is_counted_unparsed_not_guessed_at(self):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            t.write("src/x.cpp",
                    "extern int (Cls::*table[])(int);\n")
            _files, decls, _defs, unparsed = CDA.collect(t.root)
        self.assertEqual([d.symbol for d in decls], [])
        self.assertEqual(unparsed, 1)

    def test_a_function_pointer_parameter_name_is_not_part_of_its_type(self):
        def tree(t):
            t.write("src/vec.c",
                    "void vec(void (*ctor)(void *))\n{\n    (void)ctor;\n}\n")
            t.write("src/caller.c", "extern void vec(void (*)(void *));\n")
        findings, _d, _f, _files = build(tree)
        self.assertEqual(kinds(findings, "vec"), [])

    def test_a_default_argument_is_not_an_initialiser(self):
        """Only an `=` at depth zero defines anything; one in a parameter list does not."""
        def tree(t):
            t.write("src/darg.cpp",
                    "//cpp\nvoid darg(int a, int b) { (void)a; (void)b; }\n")
            t.write("src/darg_caller.cpp",
                    "//cpp\nextern void darg(int a, int b = 0);\n")
        findings, decls, _f, _files = build(tree)
        self.assertEqual([d.symbol for d in decls
                          if d.file == "src/darg_caller.cpp"], ["darg"])
        self.assertEqual(kinds(findings, "darg"), [])

    def test_a_static_assertion_is_not_a_declaration(self):
        """It carries a `==`, and this tree writes hundreds of them."""
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            t.write("include/sizes.h",
                    "struct Thing { int a; };\n"
                    'static_assert(sizeof(struct Thing) == 4, "Thing");\n')
            _files, decls, defs, unparsed = CDA.collect(t.root)
        self.assertEqual([d.symbol for d in decls], [])
        self.assertEqual([d.symbol for d in defs], [])
        self.assertEqual(unparsed, 0)


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


# ------------------------------------- the four false-pass paths reported on #2471

BIG_FILES = 2100


class BigRepo(object):
    """A git repo past the tool's scan floors: 2,100 sources, 6,300 declarations.

    The four cases below are `--changed` cases, and `--changed` means nothing on a
    two-file toy: the tool exits 2 with SCAN TOO SMALL below MIN_FILES/MIN_DECLS/
    MIN_SYMBOLS on purpose, so a small fixture would prove only that the floor works.
    This tree clears 2,000 files and 6,000 declarations, which is what the review that
    found these four paths held its own fixtures to.

    The filler sources disagree about nothing, so every finding a test sees is one that
    test wrote. Built once for the module and `reset()` between tests, because building
    it is the expensive part.
    """

    _shared = None

    @classmethod
    def shared(cls):
        if cls._shared is None:
            cls._shared = cls()
        return cls._shared

    @classmethod
    def dispose(cls):
        if cls._shared is not None:
            shutil.rmtree(str(cls._shared.root), ignore_errors=True)
            cls._shared = None

    def __init__(self):
        self.root = pathlib.Path(tempfile.mkdtemp(prefix="decl-agreement-big-"))
        self._build()

    # ------------------------------------------------------------------ plumbing

    def git(self, *args):
        return subprocess.run(["git", *args], cwd=str(self.root),
                              capture_output=True, text=True)

    def write(self, rel, text):
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")
        return p

    def reset(self):
        self.git("reset", "-q", "--hard", self.base)
        self.git("clean", "-qfd")

    def run_main(self, argv):
        """(exit code, stdout) for one `check_decl_agreement.py` run over this tree."""
        old_repo, old_baseline = CDA.REPO, CDA.BASELINE
        CDA.REPO = self.root
        CDA.BASELINE = self.root / "config" / "decl-agreement-baseline.json"
        buf = io.StringIO()
        old_stdout = sys.stdout
        sys.stdout = buf
        try:
            rc = CDA.main(argv)
        finally:
            sys.stdout = old_stdout
            CDA.REPO, CDA.BASELINE = old_repo, old_baseline
        return rc, buf.getvalue()

    def scope_size(self, out):
        """The `N file(s) in scope` number the --changed line prints."""
        m = re.search(r"(\d+) file\(s\) in scope", out)
        return int(m.group(1)) if m else None

    # ---------------------------------------------------------------- the tree

    def _build(self):
        self.git("init", "-q", "-b", "main")
        self.git("config", "user.email", "t@example.invalid")
        self.git("config", "user.name", "t")
        self.write("include/types.h", TYPES_H + "typedef u32 tdef_handle;\n")
        for i in range(BIG_FILES):
            self.write("src/filler_%04d.c" % i,
                       "".join("extern void filler_%04d_%s(int n);\n" % (i, s)
                               for s in ("a", "b", "c"))
                       + "void filler_%04d_body(int n) { (void)n; }\n" % i)
        # Path 1 (:1340), the shared typedef: an unchanged consumer spelling the alias
        # against an unchanged definition spelling what the alias means today.
        self.write("src/tdef_def.c", "void tdef_target(unsigned int h) { (void)h; }\n")
        self.write("src/tdef_user.c", "extern void tdef_target(tdef_handle h);\n")
        # Path 2 (:1276), the linkage input: a bare extern in a C++ TU of a name
        # `symbols.txt` does not record yet.
        self.write("src/cfg_user.cpp", "//cpp\nextern void cfg_target(void);\n")
        # Path 3 (:1162), the rename: a definition big enough for git to match it
        # across a rename, and an unchanged consumer of it.
        self.write("src/ren_old.c", _ren("void"))
        self.write("src/ren_user.c", "extern void ren_target(int a);\n")
        # Path 4 (:854), the initialised data definition.
        self.write("src/data_def.c", "int data_target = 0;\n")
        # Round two, item 1 (:1050): a declarator list. Only the FIRST declarator
        # survived splitting the whole statement at its first `=`.
        self.write("src/multi_def.c", "int multi_prefix = 0, multi_target = 0;\n")
        # The same loss through the walker: everything after a `{ ... }` initialiser
        # block used to be eaten with the braces.
        self.write("src/braced_def.c",
                   "int braced_prefix[] = { 1, 2 }, braced_target = 0;\n")
        # Round two, item 1 (:1088): an uninitialised external object. C's tentative
        # definition still defines the linker symbol.
        self.write("src/bare_def.c", "int bare_target;\n")
        # Round two, item 1, ownership: a declaration of a name NOTHING in the tree
        # defines. It agrees with itself and is silent -- unless an internal-linkage
        # or namespace-scoped object wrongly claims the global name, which turns it
        # into a false POSITIVE rather than a false pass.
        self.write("src/own_user.c", "extern void own_target(int a);\n")
        # An unrelated non-source file, so a deletion-only diff is still a diff and
        # `--changed` reaches the scan instead of erroring on an empty one.
        self.write("README.md", "fixture\n")
        # The positive control: a definition whose declaration a test will contradict.
        self.write("src/ctl_def.c", "void ctl_target(void) { }\n")
        rows = "".join("cfg_other_%d kind:function(arm,size=0x4) addr:0x0200%04x\n"
                       % (i, i) for i in range(4))
        self.write("config/arm9/symbols.txt", rows)
        self.git("add", "-A")
        self.git("commit", "-qm", "base")
        self.base = self.git("rev-parse", "HEAD").stdout.strip()


REN_BODY = """\
%s ren_target(int a)
{
    int i;
    int total;

    total = 0;
    for (i = 0; i < a; i++) {
        total += i;
        if (total > 100) {
            total -= 50;
        }
    }
    if (a < 0) {
        total = 0;
    }
    RETURN
}
"""


def _ren(ret):
    return (REN_BODY % ret).replace(
        "RETURN", "return total;" if ret != "void" else "(void)total;")


def tearDownModule():
    BigRepo.dispose()


class BigTreeHarnessTests(unittest.TestCase):
    """The fixture the four cases run on has to be a real scan, and quiet."""

    def test_the_fixture_tree_clears_the_scan_floors(self):
        repo = BigRepo.shared()
        repo.reset()
        files, decls, _defs, _u = CDA.collect(repo.root)
        self.assertGreaterEqual(len(files), 2000)
        self.assertGreaterEqual(len(decls), 6000)
        self.assertGreaterEqual(len({d.symbol for d in decls}), CDA.MIN_SYMBOLS)

    def test_the_unmutated_tree_is_clean_in_both_modes(self):
        repo = BigRepo.shared()
        repo.reset()
        rc_full, out_full = repo.run_main([])
        self.assertEqual(rc_full, 0, out_full)
        repo.write("src/filler_0000.c",
                   (repo.root / "src" / "filler_0000.c").read_text() + "\n")
        rc_changed, out_changed = repo.run_main(["--changed", "HEAD"])
        self.assertEqual(rc_changed, 0, out_changed)

    def test_a_wrong_return_type_fails_both_modes(self):
        """The control. It fails before these four fixes and after them."""
        repo = BigRepo.shared()
        repo.reset()
        repo.write("src/ctl_user.c", "extern int ctl_target(void);\n")
        rc_full, out_full = repo.run_main([])
        self.assertEqual(rc_full, 1, out_full)
        self.assertIn("src/ctl_user.c", out_full)
        rc_changed, out_changed = repo.run_main(["--changed", "HEAD", "--list"])
        self.assertEqual(rc_changed, 1, out_changed)
        self.assertIn("src/ctl_user.c", out_changed)


class SharedTypeScopeTests(unittest.TestCase):
    """Path 1, `:1340`: changing a shared typedef invalidates unchanged consumers.

    The consumer `tdef_user.c` declares `void tdef_target(tdef_handle)` and the
    definition file `tdef_def.c` defines `void tdef_target(unsigned int)`. They agree
    while `tdef_handle` is `u32`. Retyping the alias in the fixture's own types header
    -- and touching nothing else -- makes the consumer wrong, and the consumer is not
    in the diff.
    """

    def _mutate(self, repo):
        text = (repo.root / "include" / "types.h").read_text()
        repo.write("include/types.h",
                   text.replace("typedef u32 tdef_handle;",
                                "typedef u16 tdef_handle;"))

    def test_the_full_scan_sees_the_invalidated_consumer(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/tdef_user.c", out)

    def test_the_changed_scan_folds_in_the_consumer_of_the_typedef(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--changed", "HEAD", "--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/tdef_user.c", out)

    def test_the_chain_is_followed_through_a_second_header(self):
        """Transitively: an alias OF the changed alias puts its consumers in scope too.

        The comparison only resolves `include/types.h` and a file's own typedefs, so a
        second header's alias is compared by name. Scope must not depend on that: a
        declaration spelling `tdef_handle3` resolves through `tdef_handle` whether or
        not this tool can spell out what it means.
        """
        repo = BigRepo.shared()
        repo.reset()
        repo.write("include/handles.h",
                   "typedef tdef_handle tdef_handle2;\n"
                   "typedef tdef_handle2 tdef_handle3;\n")
        repo.write("src/tchain_user.c", "extern void tchain_target(tdef_handle3 h);\n")
        _files, decls, defs, _u = CDA.collect(repo.root)
        closure = CDA.expand_type_seeds({"tdef_handle"},
                                        CDA.typedef_graph(repo.root))
        self.assertIn("tdef_handle2", closure)
        self.assertIn("tdef_handle3", closure)
        self.assertNotIn("u8", closure)
        through, _syms = CDA.type_scope({"tdef_handle"}, decls, defs, repo.root)
        self.assertIn("src/tchain_user.c", through)
        self.assertNotIn("src/filler_0000.c", through)

    def test_a_changed_macro_used_as_a_type_reaches_its_consumer(self):
        """A macro is how a C file spells a type it has no typedef for."""
        repo = BigRepo.shared()
        repo.reset()
        repo.write("include/macros.h", "#define TDEF_MACRO_T unsigned int\n")
        repo.write("src/tmac_user.c", "extern void tmac_target(TDEF_MACRO_T h);\n")
        seeds = CDA.changed_type_names("HEAD", ["include/macros.h"], repo.root)
        self.assertIn("TDEF_MACRO_T", seeds)
        _files, decls, defs, _u = CDA.collect(repo.root)
        through, _syms = CDA.type_scope(seeds, decls, defs, repo.root)
        self.assertIn("src/tmac_user.c", through)
        self.assertNotIn("src/filler_0000.c", through)

    def test_a_struct_tag_is_a_seed_and_a_deleted_typedef_still_is(self):
        repo = BigRepo.shared()
        repo.reset()
        repo.write("include/tagged.h",
                   "struct TdefTag { int a; };\ntypedef struct TdefTag TdefAlias;\n")
        repo.git("add", "-A")
        repo.git("commit", "-qm", "add a tagged type")
        repo.write("include/tagged.h", "struct TdefTag { int a; int b; };\n")
        seeds = CDA.changed_type_names("HEAD", ["include/tagged.h"], repo.root)
        self.assertIn("TdefTag", seeds)
        self.assertIn("TdefAlias", seeds)

    def test_the_typedef_fold_does_not_drag_in_the_whole_tree(self):
        """Wide enough to catch the consumer, narrow enough to still be a PR scope."""
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        _rc, out = repo.run_main(["--changed", "HEAD"])
        size = repo.scope_size(out)
        self.assertIsNotNone(size, out)
        self.assertLess(size, 50, out)


class ConfigInputScopeTests(unittest.TestCase):
    """Path 2, `:1276`: a `config/**/symbols.txt` row is an input to the gate.

    Recording `cfg_target` unmangled makes the bare `extern` in the fixture's C++ TU
    `cfg_user.cpp` a linkage disagreement. No source path changed at all, and the PR
    scan used to exit 0 on that alone.
    """

    def _mutate(self, repo):
        path = repo.root / "config" / "arm9" / "symbols.txt"
        repo.write("config/arm9/symbols.txt",
                   path.read_text()
                   + "cfg_target kind:function(arm,size=0x4) addr:0x02001234\n")

    def test_the_full_scan_sees_the_new_linkage_disagreement(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/cfg_user.cpp", out)

    def test_the_changed_scan_does_not_exit_clean_on_a_config_only_diff(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--changed", "HEAD", "--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/cfg_user.cpp", out)

    def test_an_edited_row_counts_as_much_as_a_new_one(self):
        """A row whose kind changed is a changed input, not an unchanged file."""
        repo = BigRepo.shared()
        repo.reset()
        path = repo.root / "config" / "arm9" / "symbols.txt"
        repo.write("config/arm9/symbols.txt",
                   path.read_text().replace("cfg_other_0 kind:function",
                                            "cfg_other_0 kind:data"))
        names, paths = CDA.changed_config_symbols(
            "HEAD", ["config/arm9/symbols.txt"], repo.root)
        self.assertEqual(names, {"cfg_other_0"})
        self.assertEqual(paths, ["config/arm9/symbols.txt"])

    def test_an_untouched_config_still_ends_the_scan_early(self):
        """The early exit is right when nothing the gate reads changed."""
        repo = BigRepo.shared()
        repo.reset()
        repo.write("notes/unrelated.md", "text\n")
        rc, out = repo.run_main(["--changed", "HEAD"])
        self.assertEqual(rc, 0, out)
        self.assertIn("nothing for this gate to check", out)


class RenamedDefinitionScopeTests(unittest.TestCase):
    """Path 3, `:1162`: `--diff-filter=AM` drops a Git-detected rename.

    The fixture's `ren_old.c` becomes `ren_new.c` and its return type changes in the
    same commit. Git reports one `R` row, which is neither `A` nor `M`, so the renamed
    definition never reached the scope and its consumer was never re-checked.
    """

    def _mutate(self, repo):
        repo.git("mv", "src/ren_old.c", "src/ren_new.c")
        repo.write("src/ren_new.c", _ren("int"))
        # A real pull request touches more than the renamed file; without this the
        # diff would be empty and the tool would fail for that reason instead.
        repo.write("src/filler_0000.c",
                   (repo.root / "src" / "filler_0000.c").read_text()
                   + "/* touched */\n")
        repo.git("add", "-A")
        repo.git("commit", "-qm", "rename with a changed signature")

    def test_git_really_reports_this_as_a_rename(self):
        """If git stopped detecting it, the case below would pass for a wrong reason."""
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        out = repo.git("diff", "--name-status", "-M", "HEAD~1", "HEAD").stdout
        self.assertRegex(out, r"R\d*\tsrc/ren_old\.c\tsrc/ren_new\.c")

    def test_the_full_scan_rejects_the_renamed_signature(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/ren_user.c", out)

    def test_the_changed_scan_takes_both_sides_of_the_rename(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--changed", "HEAD~1", "--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/ren_user.c", out)

    def test_the_old_side_is_read_out_of_the_base_commit(self):
        """The path that is gone still says which symbols it used to define."""
        repo = BigRepo.shared()
        repo.reset()
        repo.git("mv", "src/ren_old.c", "src/ren_new.c")
        repo.write("src/ren_new.c", _ren("int").replace("ren_target", "ren_moved"))
        repo.git("add", "-A")
        repo.git("commit", "-qm", "rename the file and the symbol with it")
        touched, defined, _total, err = CDA.changed_scope("HEAD~1", repo.root)
        self.assertIsNone(err)
        self.assertIn("src/ren_old.c", touched)
        self.assertIn("src/ren_new.c", touched)
        self.assertIn("ren_target", defined)
        self.assertIn("ren_moved", defined)


class DataDefinitionTests(unittest.TestCase):
    """Path 4, `:854`: an initialised data definition is a definition.

    The fixture's `data_def.c` defines `int data_target = 0`. A file declaring
    `extern int data_target(void)` is claiming the linker's symbol is a function. It
    is not, and both modes used to pass because the definition was thrown away before
    the comparison ran.
    """

    def _mutate(self, repo):
        repo.write("src/data_user.c", "extern int data_target(void);\n")

    def test_the_full_scan_reports_the_kind_disagreement(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/data_user.c", out)
        self.assertIn("kind", out)

    def test_the_changed_scan_reports_it_too(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--changed", "HEAD", "--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/data_user.c", out)

    def test_the_definition_carries_the_declared_type_not_a_function_shape(self):
        repo = BigRepo.shared()
        repo.reset()
        _files, _decls, defs, _u = CDA.collect(repo.root)
        found = [d for d in defs if d.symbol == "data_target"]
        self.assertEqual(len(found), 1)
        self.assertFalse(found[0].is_function)
        self.assertEqual(found[0].ret, "int")

    def test_a_static_initialised_object_is_still_not_a_definition(self):
        """`static` is internal linkage: it is not the symbol anyone declares."""
        repo = BigRepo.shared()
        repo.reset()
        repo.write("src/data_static.c", "static int data_static_target = 0;\n")
        _files, _decls, defs, _u = CDA.collect(repo.root)
        self.assertEqual([d for d in defs if d.symbol == "data_static_target"], [])


# ---------------------------- the four defects reported on the second review of #2471

class LaterDeclaratorTests(unittest.TestCase):
    """Item 1, `:1050`: a declarator list is not one declarator.

    `int multi_prefix = 0, multi_target = 0;` defines two data symbols. The parser
    split the WHOLE statement at its first `=` before looking at the declarators, so
    it recorded `multi_prefix` and never saw `multi_target` -- and a file declaring
    `extern int multi_target(void);` contradicted a definition the tool had thrown
    away, in both the full scan and `--changed`.

    Each of these FAILS on the parser as it was and passes after it.
    """

    def _mutate(self, repo):
        repo.write("src/multi_user.c", "extern int multi_target(void);\n")

    def test_the_full_scan_reports_the_second_declarator(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/multi_user.c", out)
        self.assertIn("kind", out)

    def test_the_changed_scan_reports_it_too(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--changed", "HEAD", "--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/multi_user.c", out)

    def test_every_declarator_in_the_list_is_a_definition(self):
        repo = BigRepo.shared()
        repo.reset()
        _files, _decls, defs, _u = CDA.collect(repo.root)
        got = {d.symbol: d for d in defs
               if d.symbol in ("multi_prefix", "multi_target")}
        self.assertEqual(sorted(got), ["multi_prefix", "multi_target"])
        self.assertEqual(got["multi_target"].ret, "int")
        self.assertFalse(got["multi_target"].is_function)

    def test_a_declarator_after_a_brace_initialiser_survives_the_block(self):
        """`int a[] = { 1, 2 }, target = 0;` -- the walker used to eat `target`."""
        repo = BigRepo.shared()
        repo.reset()
        _files, _decls, defs, _u = CDA.collect(repo.root)
        got = sorted(d.symbol for d in defs
                     if d.symbol in ("braced_prefix", "braced_target"))
        self.assertEqual(got, ["braced_prefix", "braced_target"])

    def test_a_default_argument_is_still_not_an_initialiser(self):
        """The control for this fix: `= 0` inside the parentheses claims nothing."""
        decls, defs, _u = CDA.parse_file(
            "src/x.cpp", "//cpp\nextern void dflt(int a = 0);\n", {})
        self.assertEqual([d.symbol for d in defs], [])
        self.assertEqual([(d.symbol, d.is_function) for d in decls],
                         [("dflt", True)])


class UninitialisedDefinitionTests(unittest.TestCase):
    """Item 1, `:1088`: an uninitialised external object is still a definition.

    `int bare_target;` at file scope is C's tentative definition and C++'s definition
    outright: it defines the linker symbol. The parser dropped it -- it reached
    neither list -- so `extern int bare_target(void);` passed both modes exactly as
    the initialised case used to.

    The first three FAIL on the parser as it was.
    """

    def _mutate(self, repo):
        repo.write("src/bare_user.c", "extern int bare_target(void);\n")

    def test_the_full_scan_reports_the_kind_disagreement(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/bare_user.c", out)
        self.assertIn("kind", out)

    def test_the_changed_scan_reports_it_too(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--changed", "HEAD", "--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/bare_user.c", out)

    def test_the_definition_carries_the_declared_type(self):
        repo = BigRepo.shared()
        repo.reset()
        _files, _decls, defs, _u = CDA.collect(repo.root)
        found = [d for d in defs if d.symbol == "bare_target"]
        self.assertEqual(len(found), 1)
        self.assertFalse(found[0].is_function)
        self.assertEqual(found[0].ret, "int")

    def test_an_extern_object_with_no_initialiser_is_still_a_DECLARATION(self):
        """The control. `extern int t;` declares; only the keyword-free form defines."""
        decls, defs, _u = CDA.parse_file("src/x.c", "extern int t;\n", {})
        self.assertEqual([d.symbol for d in defs], [])
        self.assertEqual([d.symbol for d in decls], ["t"])

    def test_a_bare_function_declaration_still_defines_nothing(self):
        """A prototype has no body. Only OBJECTS define without a keyword."""
        decls, defs, _u = CDA.parse_file("src/x.c", "void f(int a);\n", {})
        self.assertEqual([d.symbol for d in defs], [])
        self.assertEqual([d.symbol for d in decls], [])

    def test_a_static_uninitialised_object_is_not_a_definition(self):
        decls, defs, _u = CDA.parse_file("src/x.c", "static int t;\n", {})
        self.assertEqual([d.symbol for d in defs], [])
        self.assertEqual([d.symbol for d in decls], [])


class DefinitionOwnershipTests(unittest.TestCase):
    """Item 1, ownership: internal-linkage and mangled-scope objects are not the global.

    `const int target = 0;` in a C++ translation unit and
    `namespace Private { int target = 0; }` both used to be recorded as a global
    definition named `target`. Neither is: the first has internal linkage, the second
    is `_ZN7Private6targetE`. Claiming them is worse than missing them -- it makes the
    gate report a FALSE finding against an honest declaration elsewhere.

    Every `assertEqual(..., [])` here FAILS on the parser as it was; the four controls
    around them pass in both states, which is what says the rule did not just switch
    the definition side off.
    """

    def _defs(self, rel, code):
        _d, defs, _u = CDA.parse_file(rel, code, {})
        return sorted(d.symbol for d in defs)

    # ------------------------------------------------------------- internal linkage

    def test_a_const_object_in_a_cxx_TU_is_not_the_global_symbol(self):
        self.assertEqual(self._defs("src/x.cpp", "//cpp\nconst int target = 0;\n"), [])

    def test_an_extern_const_object_in_a_cxx_TU_still_is(self):
        """`extern` restores external linkage. The control for the rule above."""
        self.assertEqual(
            self._defs("src/x.cpp", "//cpp\nextern const int target = 0;\n"),
            ["target"])

    def test_a_const_object_in_a_C_TU_still_is(self):
        """C has no such rule: file-scope `const` is external. Control."""
        self.assertEqual(self._defs("src/x.c", "const int target = 0;\n"), ["target"])

    def test_a_pointer_TO_const_is_not_a_const_object(self):
        """`const char *t` is a mutable pointer. Control: this tree has ~1,300."""
        self.assertEqual(self._defs("src/x.cpp", '//cpp\nconst char *target = 0;\n'),
                         ["target"])

    def test_a_const_pointer_IS_a_const_object(self):
        self.assertEqual(
            self._defs("src/x.cpp", "//cpp\nchar *const target = 0;\n"), [])

    # ---------------------------------------------------------------- mangled scope

    def test_a_namespace_scoped_object_is_not_the_global_symbol(self):
        self.assertEqual(
            self._defs("src/x.cpp", "//cpp\nnamespace P { int target = 0; }\n"), [])

    def test_a_namespace_scoped_function_is_not_the_global_symbol_either(self):
        self.assertEqual(
            self._defs("src/x.cpp", "//cpp\nnamespace P { void target(void) { } }\n"),
            [])

    def test_extern_c_INSIDE_a_namespace_still_defines_the_flat_name(self):
        """Language linkage beats scope, and this tree writes exactly this shape."""
        self.assertEqual(
            self._defs("src/x.cpp",
                       '//cpp\nnamespace P { extern "C" int target(void) { } }\n'),
            ["target"])

    def test_an_extern_c_BLOCK_inside_a_namespace_does_too(self):
        self.assertEqual(
            self._defs("src/x.cpp",
                       '//cpp\nnamespace P { extern "C" { int target = 0; } }\n'),
            ["target"])

    def test_a_marked_namespace_definition_keeps_its_at_symbol_name(self):
        """`// @symbol` is how this tree spells a name it cannot recover. Control."""
        self.assertEqual(
            self._defs("src/x.cpp",
                       "//cpp\nnamespace P {\n// @symbol _ZN1P6targetEv\n"
                       "void target(void) { }\n}\n"),
            ["_ZN1P6targetEv"])

    # -------------------------------------------------- and what that buys, end to end

    def test_a_namespace_object_does_not_fabricate_a_finding_against_a_real_extern(self):
        """The false POSITIVE the ownership bug makes. Fails before the fix, rc 1."""
        repo = BigRepo.shared()
        repo.reset()
        repo.write("src/own_ns.cpp", "//cpp\nnamespace Private { int own_target = 0; }\n")
        rc, out = repo.run_main(["--list"])
        self.assertEqual(rc, 0, out)
        self.assertNotIn("own_target", out)

    def test_a_const_object_does_not_either(self):
        repo = BigRepo.shared()
        repo.reset()
        repo.write("src/own_const.cpp", "//cpp\nconst int own_target = 0;\n")
        rc, out = repo.run_main(["--list"])
        self.assertEqual(rc, 0, out)
        self.assertNotIn("own_target", out)


class DeletedTypeInputTests(unittest.TestCase):
    """Item 2, `:1407`: `_paths_of` threw away every `D` row.

    Deleting `include/types.h` retypes `extern void tdef_target(tdef_handle);` in a
    file the diff never touched: `tdef_handle` stops resolving to `unsigned int` and
    the declaration stops agreeing with `void tdef_target(unsigned int)`. The full
    scan says so. `--changed` had no source path left in the diff at all, so it
    printed "nothing for this gate to check" and exited 0.

    `check_references.py` does not cover this. Every name here still resolves; what
    changed is what the surviving declarations MEAN.
    """

    def _mutate(self, repo):
        (repo.root / "include" / "types.h").unlink()
        repo.write("README.md", "fixture, touched\n")

    def test_the_full_scan_rejects_the_orphaned_consumer(self):
        """The premise. True before the fix and after it -- the full scan never
        looked at git."""
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/tdef_user.c", out)

    def test_the_changed_scan_does_not_exit_clean_on_a_deleted_type_header(self):
        """FAILS before the fix: rc 0, "nothing for this gate to check"."""
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        rc, out = repo.run_main(["--changed", "HEAD", "--list"])
        self.assertEqual(rc, 1, out)
        self.assertIn("src/tdef_user.c", out)

    def test_the_deleted_header_is_in_the_work_list(self):
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        total, err = CDA.changed_paths("HEAD", repo.root)
        self.assertIsNone(err)
        self.assertIn("include/types.h", total)

    def test_its_identities_are_read_out_of_the_base_blob(self):
        """The header is gone from disk; the typedef it used to supply is not."""
        repo = BigRepo.shared()
        repo.reset()
        self._mutate(repo)
        touched, _defined, _total, err = CDA.changed_scope("HEAD", repo.root)
        self.assertIsNone(err)
        self.assertIn("include/types.h", touched)
        seeds = CDA.changed_type_names("HEAD", touched, repo.root)
        self.assertIn("tdef_handle", seeds)

    def test_a_deleted_symbols_file_still_names_the_rows_it_removed(self):
        """The configuration half of the same rule."""
        repo = BigRepo.shared()
        repo.reset()
        (repo.root / "config" / "arm9" / "symbols.txt").unlink()
        repo.write("README.md", "fixture, touched\n")
        total, err = CDA.changed_paths("HEAD", repo.root)
        self.assertIsNone(err)
        self.assertIn("config/arm9/symbols.txt", total)
        names, paths = CDA.changed_config_symbols("HEAD", total, repo.root)
        self.assertEqual(paths, ["config/arm9/symbols.txt"])
        self.assertIn("cfg_other_0", names)

    def test_a_deleted_SOURCE_is_still_out_of_scope(self):
        """The carve-out that stays. A vanished definition is a name that no longer
        resolves, which is `check_references.py`'s question. Control: unchanged."""
        repo = BigRepo.shared()
        repo.reset()
        (repo.root / "src" / "filler_0001.c").unlink()
        repo.write("README.md", "fixture, touched\n")
        total, err = CDA.changed_paths("HEAD", repo.root)
        self.assertIsNone(err)
        self.assertNotIn("src/filler_0001.c", total)


class BomTests(unittest.TestCase):
    """A UTF-8 BOM is not part of the first token.

    `\\ufefftypedef unsigned short u16;` does not match `^typedef\\b`, so the typedef
    that opens `src/func_ov065_021183c8.c` -- the one file in this tree with a BOM --
    reached the declarator parser.

    HONEST LABEL: unlike the four classes above, these two pass on the parser as it was.
    The BOM was latent there because an uninitialised declarator was dropped before
    anything looked at it; the moment `int target;` became a definition it turned into a
    definition of a symbol called `u16`, which is how it was found. They guard a
    regression this branch introduced and then fixed, not one the review reported.
    """

    def test_a_typedef_behind_a_BOM_is_still_a_typedef(self):
        decls, defs, _u = CDA.parse_file(
            "src/x.c", "﻿typedef unsigned short u16;\n", {})
        self.assertEqual([d.symbol for d in defs], [])
        self.assertEqual([d.symbol for d in decls], [])

    def test_the_one_real_file_with_a_BOM_defines_no_type_name(self):
        rel = "src/func_ov065_021183c8.c"
        path = REPO / rel
        if not path.exists():
            self.skipTest("%s is not in this tree" % rel)
        self.assertEqual(path.read_bytes()[:3], b"\xef\xbb\xbf")
        _d, defs, _u = CDA.parse_file(
            rel, path.read_text(encoding="utf-8", errors="replace"),
            CDA.scalar_typedefs(REPO))
        self.assertEqual([d.symbol for d in defs if d.symbol in ("u8", "u16")], [])


if __name__ == "__main__":
    unittest.main(verbosity=2)
