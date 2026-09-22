"""Regression tests for tools/check_decl_return_types.py.

Two kinds of case here, and the second kind is the one that matters:

  1. It finds a real disagreement (the gate does its job).
  2. It does NOT fire on the many shapes that merely look like one.

A return-type gate that cannot tell `void *` from `void*`, or that resolves an
overload by guessing, fails on a correct tree. That kind of red is worse than no
gate at all: it trains everyone to route around the check. So most of what
follows plants a shape that is CORRECT and asserts silence.

The accounting is asserted too. This tool's honesty rests on reporting what it
could not join, and an early version counted all 274 destructors in include/ as
"virtual declarations not parsed" -- phantom blind spots on a tree whose real
figure is zero. test_destructors_are_handled_not_missed pins that.
"""
import contextlib
import io
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import check_decl_return_types as C


class Tree:
    """A throwaway include/ tree: `decl_common.h` plus named headers.

    `src` and `types_h` are for the plain C half: `src` is a map of file name
    (with its extension, since .c and .cpp are both real here) to contents, and
    `types_h` plants an include/types.h so the alias table is exercised as it is
    harvested rather than as a hard-coded list.
    """

    def __init__(self, decl, src=None, types_h=None, symbols=None, **headers):
        self.dir = tempfile.TemporaryDirectory()
        root = pathlib.Path(self.dir.name)
        (root / "include").mkdir()
        (root / "include" / "decl_common.h").write_text(decl, encoding="utf-8")
        for name, text in headers.items():
            (root / "include" / (name + ".h")).write_text(text, encoding="utf-8")
        if types_h is not None:
            (root / "include" / "types.h").write_text(types_h, encoding="utf-8")
        if src:
            (root / "src").mkdir()
            for name, text in src.items():
                (root / "src" / name).write_text(text, encoding="utf-8")
        for name, text in (symbols or {}).items():
            p = root / name
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text(text, encoding="utf-8")
        self.root = root

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.dir.cleanup()

    def check(self):
        return C.check(self.root)

    def check_c(self):
        return C.check_plain_c(self.root)

    def kinds(self):
        """symbol -> class, for the rows the plain C half reports."""
        return {r[0]: r[3] for r in self.check_c()[0]}


class FindsRealDisagreements(unittest.TestCase):
    def test_plain_disagreement_is_reported(self):
        with Tree("extern void _ZN3Foo3BarEv(void*);\n",
                  Foo="struct Foo {\n    virtual int Bar();\n};\n") as t:
            bad, acct = t.check()
            self.assertEqual(len(bad), 1)
            cls, meth, ctype, dtype, _sym = bad[0]
            self.assertEqual((cls, meth, ctype, dtype),
                             ("Foo", "Bar", "int", "void"))
            self.assertEqual(acct["joined"], 1)

    def test_bool_and_int_are_not_conflated(self):
        """The one disagreement in this family that can cost bytes.

        A bool return goes through a widening cast under mwccarm 2004/b56 that
        an int return does not, so normalising them together would hide the
        expensive case while still catching the cheap ones.
        """
        with Tree("extern int _ZN3Foo3BarEv(void*);\n",
                  Foo="struct Foo {\n    virtual bool Bar();\n};\n") as t:
            bad, _ = t.check()
            self.assertEqual(len(bad), 1)
            self.assertEqual(bad[0][2:4], ("bool", "int"))

    def test_disagreement_sets_exit_status(self):
        with Tree("extern void _ZN3Foo3BarEv(void*);\n",
                  Foo="struct Foo {\n    virtual int Bar();\n};\n") as t:
            buf = io.StringIO()
            with contextlib.redirect_stdout(buf):
                rc = C.main(["--root", str(t.root)])
            self.assertEqual(rc, 1)
            self.assertIn("DISAGREE Foo::Bar", buf.getvalue())
            # The accounting prints on failure too, unasked: a reader deciding
            # whether to believe a red run needs the join count as much as one
            # reading a green run.
            self.assertIn("JOINED and compared", buf.getvalue())

    def test_clean_tree_exits_zero(self):
        with Tree("extern int _ZN3Foo3BarEv(void*);\n",
                  Foo="struct Foo {\n    virtual int Bar();\n};\n") as t:
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(C.main(["--root", str(t.root)]), 0)

    def test_arity_mismatch_is_not_a_disagreement(self):
        """decl_common.h spells `this` as an explicit void* first parameter.

        Every row therefore has one more parameter than the class declaration.
        Only the return type is compared; a tool that compared signatures would
        fire on all 64 rows.
        """
        with Tree("extern int _ZN3Foo3BarEii(void*, int, int);\n",
                  Foo="struct Foo {\n    virtual int Bar(int a, int b);\n};\n") as t:
            bad, acct = t.check()
            self.assertEqual(bad, [])
            self.assertEqual(acct["joined"], 1)


class DoesNotFireOnCorrectShapes(unittest.TestCase):
    def test_pointer_spelling_is_normalised(self):
        with Tree("extern void*_ZN3Foo3BarEv(void*);\n",
                  Foo="struct Foo {\n    virtual void * Bar();\n};\n") as t:
            self.assertEqual(t.check()[0], [])

    def test_structors_are_skipped(self):
        """A constructor or destructor has no return type to disagree about."""
        decl = ("extern void _ZN3FooC1Ev(void*);\n"
                "extern void _ZN3FooD1Ev(void*);\n"
                "extern void _ZN3FooD2Ev(void*);\n")
        with Tree(decl, Foo="struct Foo {\n    virtual ~Foo();\n};\n") as t:
            bad, acct = t.check()
            self.assertEqual(bad, [])
            self.assertEqual(acct["joined"], 0)
            self.assertEqual(acct["no_return_type"], 3)

    def test_vtable_and_typeinfo_rows_are_skipped(self):
        decl = ("extern void *_ZTV3Foo(void);\n"
                "extern void *_ZTI3Foo(void);\n"
                "extern void *_ZTS3Foo(void);\n")
        with Tree(decl, Foo="struct Foo {\n    virtual int Bar();\n};\n") as t:
            bad, acct = t.check()
            self.assertEqual(bad, [])
            self.assertEqual(acct["no_return_type"], 3)

    def test_namespaced_free_function_is_not_a_class(self):
        """`Sound::Play2D` and friends are namespaced free functions.

        They have no vtable slot, so there is no second declaration site and
        nothing to disagree with. Counting them as checked would inflate the
        tool's apparent reach.
        """
        with Tree("extern int _ZN5Sound6Play2DEv(void*);\n") as t:
            bad, acct = t.check()
            self.assertEqual(bad, [])
            self.assertEqual(acct["not_a_class"], 1)
            self.assertEqual(acct["joined"], 0)

    def test_overloads_are_reported_ambiguous_not_failed(self):
        """Two virtuals share a name, so the join is not unique.

        Picking one would be a guess, and a wrong guess is a false failure on a
        correct tree. The row is counted as ambiguous and compared against
        nothing.
        """
        hdr = ("struct Foo {\n"
               "    virtual int Bar();\n"
               "    virtual void Bar(int a);\n"
               "};\n")
        with Tree("extern void _ZN3Foo3BarEv(void*);\n", Foo=hdr) as t:
            bad, acct = t.check()
            self.assertEqual(bad, [])
            self.assertEqual(acct["ambiguous"], 1)
            self.assertEqual(acct["joined"], 0)

    def test_non_virtual_method_is_not_joined(self):
        """A non-virtual method is reached by name, never through a vtable.

        It is still worth counting separately rather than silently: the row
        exists, and a reader should see that it was set aside on purpose.
        """
        with Tree("extern void _ZN3Foo3BarEv(void*);\n",
                  Foo="struct Foo {\n    virtual int Other();\n    int Bar();\n};\n") as t:
            bad, acct = t.check()
            self.assertEqual(bad, [])
            self.assertEqual(acct["not_virtual"], 1)

    def test_nested_braces_do_not_truncate_a_class_body(self):
        """A union or inline body inside the class must not end the scan early.

        With naive `\\{.*?\\}` matching, Bar falls outside the parsed body, the
        join fails, and the disagreement below goes unreported -- a false GREEN,
        which is the failure mode this whole file exists to prevent.
        """
        hdr = ("struct Foo {\n"
               "    union { int a; short b; } u;\n"
               "    virtual int Bar();\n"
               "};\n")
        with Tree("extern void _ZN3Foo3BarEv(void*);\n", Foo=hdr) as t:
            bad, acct = t.check()
            self.assertEqual(acct["joined"], 1)
            self.assertEqual(len(bad), 1)


class ReportsItsOwnReachHonestly(unittest.TestCase):
    def test_destructors_are_handled_not_missed(self):
        """Planted regression: destructors once inflated the unparsed count.

        They are recognised and dropped on purpose, so they belong in neither
        the compared set nor the blind spot count. Reporting them as unparsed
        claimed 274 blind spots on include/ where there are none.
        """
        hdr = ("struct Foo {\n"
               "    virtual ~Foo();\n"
               "    virtual ~Foo() {}\n"
               "    virtual int Bar();\n"
               "};\n")
        with Tree("extern int _ZN3Foo3BarEv(void*);\n", Foo=hdr) as t:
            _bad, acct = t.check()
            self.assertEqual(acct["unparsed_virtuals"], 0)

    def test_multiline_declaration_is_parsed(self):
        """A declaration split across lines IS read, because `\\s+` spans them.

        Asserted rather than assumed: I first documented the opposite, and a
        tool that silently skipped these would under-report while looking clean.
        """
        hdr = ("struct Foo {\n"
               "    virtual int\n"
               "    Bar();\n"
               "};\n")
        with Tree("extern void _ZN3Foo3BarEv(void*);\n", Foo=hdr) as t:
            bad, acct = t.check()
            self.assertEqual(acct["unparsed_virtuals"], 0)
            self.assertEqual(acct["joined"], 1)
            self.assertEqual(len(bad), 1)

    def test_prose_beginning_with_virtual_is_not_a_blind_spot(self):
        """Headers here carry heavy prose, and some lines start with `virtual`.

        Comments are stripped before anything is scanned, so such a line is
        neither read as a declaration nor scored as something the tool failed to
        read. Counting it would inflate the blind-spot number with text that was
        never a declaration.
        """
        hdr = ("struct Foo {\n"
               "    /* comment\n"
               "    virtual, in the sense that the slot is dispatched\n"
               "    */\n"
               "    virtual int Bar();\n"
               "};\n")
        with Tree("extern int _ZN3Foo3BarEv(void*);\n", Foo=hdr) as t:
            bad, acct = t.check()
            self.assertEqual(acct["unparsed_virtuals"], 0)
            self.assertEqual(acct["joined"], 1)
            self.assertEqual(bad, [])

    def test_comment_parenthesis_cannot_invent_a_method(self):
        """Planted regression, and the reason strip_comments exists.

        `virtual ~fBase_c();  /* slots 16 (D1), 17 (D0) */` was matched by
        taking the `(` from the comment, yielding a virtual named `16` with
        return type `~fBase_c(); /* slots`. It fired in 116 class bodies here and
        drove the blind-spot counter NEGATIVE, which is how it was noticed.
        """
        hdr = ("struct Foo {\n"
               "    virtual ~Foo();      /* slots 16 (D1), 17 (D0) */\n"
               "    virtual int Bar();\n"
               "};\n")
        with Tree("extern int _ZN3Foo3BarEv(void*);\n", Foo=hdr) as t:
            bad, acct = t.check()
            self.assertEqual(acct["unparsed_virtuals"], 0)
            self.assertEqual(bad, [])
            names = [n for _t, n in C.class_virtuals(t.root)[0]["Foo"]]
            self.assertEqual(names, ["Bar"])

    def test_destructor_cannot_borrow_parens_from_a_later_constructor(self):
        """Planted regression, and the second reason the counter went negative.

        `virtual ~dActor_c();` matched the virtual pattern by reading its return
        type as `~dActor_c();` and its method name as `dActor_c` -- the
        CONSTRUCTOR, declared lines below, whose `(` it borrowed. That both
        double-counted the destructor and injected a virtual named after the
        class into the map, where it could join a decl_common.h row and compare
        a garbage return type against it.
        """
        hdr = ("struct Foo {\n"
               "    virtual ~Foo();\n"
               "\n"
               "    Foo(int profile);\n"
               "    virtual int Bar();\n"
               "};\n")
        with Tree("extern int _ZN3Foo3BarEv(void*);\n", Foo=hdr) as t:
            bad, acct = t.check()
            self.assertEqual(acct["unparsed_virtuals"], 0)
            self.assertEqual(bad, [])
            names = [n for _t, n in C.class_virtuals(t.root)[0]["Foo"]]
            self.assertEqual(names, ["Bar"])

    def test_sigil_glued_to_the_method_name_is_still_read(self):
        """`virtual Vector3 &GetPos();` -- no space between the type and the name.

        Eight declarations in include/ are spelled this way (dCc_c and its three
        subclasses, dMgJump3DMario_c and its adapter). A separator of `\\s+` reads
        none of them, and they showed up as the tool's entire blind spot before
        the lookbehind arm was added.
        """
        hdr = ("struct Foo {\n"
               "    virtual Vector3 &GetPos();\n"
               "    virtual void *Unk_020c76d0() = 0;\n"
               "};\n")
        decl = ("extern void* _ZN3Foo6GetPosEv(void*);\n"
                "extern void* _ZN3Foo12Unk_020c76d0Ev(void*);\n")
        with Tree(decl, Foo=hdr) as t:
            bad, acct = t.check()
            self.assertEqual(acct["unparsed_virtuals"], 0)
            self.assertEqual(acct["joined"], 2)
            # The pointer row agrees; the reference row does not, and saying so
            # is the point -- `Vector3&` is not `void*`.
            self.assertEqual([b[1] for b in bad], ["GetPos"])

    def test_virtual_line_with_no_call_parens_is_counted_unparsed(self):
        """The counter must be able to be non-zero for an honest reason."""
        hdr = "struct Foo {\n    virtual int Bar;\n};\n"
        with Tree("extern int _ZN3Foo3BarEv(void*);\n", Foo=hdr) as t:
            _bad, acct = t.check()
            self.assertEqual(acct["unparsed_virtuals"], 1)
            self.assertEqual(acct["joined"], 0)


class ParsesMangledNamesBackward(unittest.TestCase):
    def test_recovers_class_and_method(self):
        self.assertEqual(C.parse_nested("_ZN11dScMgBase_c16OnAimedAtWithEggEv"),
                         ("dScMgBase_c", "OnAimedAtWithEgg"))

    def test_length_prefix_is_respected_not_guessed(self):
        """The prefix is a byte count, and names contain digits.

        `_ZN4dBgW9IsEnabledEv` must not be read as class `dBgW9`.
        """
        self.assertEqual(C.parse_nested("_ZN4dBgW9IsEnabledEv"),
                         ("dBgW", "IsEnabled"))

    def test_rejects_shapes_with_no_return_type(self):
        for sym in ("_ZTV11dScMgBase_c", "_ZN11dScMgBase_cC2Ev",
                    "_ZN11dScMgBase_cD2Ev", "_Z9SomeThingv"):
            self.assertIsNone(C.parse_nested(sym), sym)

    def test_rejects_a_truncated_name(self):
        """A length that runs off the end is malformed, not a class named ''."""
        self.assertIsNone(C.parse_nested("_ZN99Foo3BarEv"))


TYPES_H = (
    "typedef unsigned int   u32;\n"
    "typedef signed int     s32;\n"
    "typedef signed long long   s64;\n"
    "typedef volatile unsigned int vu32;\n"
    "typedef s32 Fix12i;\n"
    "#ifndef __cplusplus\n"
    "typedef int bool;\n"
    "#endif\n"
)


class PlainCRowsAgainstTheirBodies(unittest.TestCase):
    """The 1250 `func_<addr>` rows, which the mangled join cannot see at all."""

    def test_void_over_a_value_returning_body_is_reported(self):
        with Tree("extern void func_0200c66c(void*);\n",
                  src={"func_0200c66c.c": "int func_0200c66c(char *s)\n{\n"
                                          "    return 1;\n}\n"}) as t:
            rows, acct = t.check_c()
            self.assertEqual(len(rows), 1)
            sym, dtype, btype, kind, rel = rows[0]
            self.assertEqual((sym, dtype, btype, kind),
                             ("func_0200c66c", "void", "int", "void-vs-value"))
            self.assertEqual(rel, "src/func_0200c66c.c")
            self.assertEqual(acct["compared"], 1)

    def test_agreement_is_silent(self):
        with Tree("extern int func_0200c66c(void*);\n",
                  src={"func_0200c66c.c": "int func_0200c66c(char *s)\n{\n"
                                          "    return 1;\n}\n"}) as t:
            self.assertEqual(t.check_c()[0], [])

    def test_alias_harvested_from_types_h_is_not_a_disagreement(self):
        """`u32` and `unsigned int` are one type, and include/types.h says so.

        Harvested rather than hard-coded, so this pair is only equal here
        because the planted header makes it equal.
        """
        with Tree("extern unsigned int func_020228dc(void*);\n",
                  types_h=TYPES_H,
                  src={"func_020228dc.c": "u32 func_020228dc(char *s)\n{\n"
                                          "    return 0;\n}\n"}) as t:
            rows, acct = t.check_c()
            self.assertEqual(rows, [])
            self.assertEqual(acct["typedef_equivalent"], 1)

    def test_multi_word_base_type_is_canonicalised_whole(self):
        """Planted regression, and the reason the first audit over-reported.

        A token-by-token alias table maps `unsigned` to `unsigned int` and then
        leaves the following `int` alone, so `unsigned int` canonicalises to
        `unsigned int int` and stops matching `u32`. Four of the sixty-four rows
        that audit reported were this bug and not a disagreement; `s64` against
        `long long` was a fifth, by the same mechanism through `long`.
        """
        aliases = {"u32": "unsigned int", "s64": "long long"}
        self.assertEqual(C.canon_c_type("unsigned int", aliases), "unsigned int")
        self.assertEqual(C.canon_c_type("u32", aliases), "unsigned int")
        self.assertEqual(C.canon_c_type("long long", aliases), "long long")
        self.assertEqual(C.canon_c_type("s64", aliases), "long long")

    def test_alias_chain_is_resolved(self):
        """`Fix12i -> s32 -> signed int -> int`, in one hop from the table."""
        with Tree("extern int func_02010000(void*);\n",
                  types_h=TYPES_H,
                  src={"func_02010000.c": "Fix12i func_02010000(char *s)\n{\n"
                                          "    return 0;\n}\n"}) as t:
            self.assertEqual(t.check_c()[0], [])

    def test_sign_difference_is_reported(self):
        """Same width, and still a real row: it is what the caller shifts by."""
        with Tree("extern int func_02022cbc(void*);\n",
                  types_h=TYPES_H,
                  src={"func_02022cbc.c": "u32 func_02022cbc(char *s)\n{\n"
                                          "    return 0;\n}\n"}) as t:
            self.assertEqual(t.kinds(), {"func_02022cbc": "width-or-sign"})

    def test_bool_is_not_collapsed_into_int(self):
        """types.h spells `typedef int bool;` and this half still reports it.

        In a .c translation unit the two ARE one type. In a .cpp one, bool is
        the C++ type and mwccarm widens it on return, and the row does not say
        which kind of translation unit its caller is in. Zero rows in the tree
        are spelled bool, so keeping them apart costs nothing and keeps this
        half agreeing with the mangled one, which must keep them apart.
        """
        with Tree("extern bool func_02010000(void*);\n",
                  types_h=TYPES_H,
                  src={"func_02010000.c": "int func_02010000(char *s)\n{\n"
                                          "    return 0;\n}\n"}) as t:
            self.assertEqual(t.kinds(), {"func_02010000": "width-or-sign"})

    def test_pointer_spelling_is_normalised(self):
        with Tree("extern int*func_02010000(void*);\n",
                  src={"func_02010000.c": "int * func_02010000(char *s)\n{\n"
                                          "    return 0;\n}\n"}) as t:
            self.assertEqual(t.check_c()[0], [])

    def test_pointer_shape_is_its_own_class(self):
        with Tree("extern int*func_0203dabc(void*);\n",
                  types_h=TYPES_H,
                  src={"func_0203dabc.c": "u32 *func_0203dabc(char *s)\n{\n"
                                          "    return 0;\n}\n"}) as t:
            self.assertEqual(t.kinds(), {"func_0203dabc": "pointer-shape"})

    def test_pointer_against_a_scalar_is_its_own_class(self):
        with Tree("extern char*func_02023178(int);\n",
                  src={"func_02023178.c": "int func_02023178(int n)\n{\n"
                                          "    return n;\n}\n"}) as t:
            self.assertEqual(t.kinds(), {"func_02023178": "pointer-vs-scalar"})

    def test_void_pointer_against_void_is_a_value(self):
        """`void` and `void*` differ in whether r0 carries anything at all."""
        with Tree("extern void func_ov002_020f6a50(void*);\n",
                  src={"func_ov002_020f6a50.c":
                       "void *func_ov002_020f6a50(char *s)\n{\n"
                       "    return s;\n}\n"}) as t:
            self.assertEqual(t.kinds(),
                             {"func_ov002_020f6a50": "void-vs-value"})

    def test_const_on_a_returned_value_is_not_a_type_difference(self):
        with Tree("extern char*func_02010000(void*);\n",
                  src={"func_02010000.c":
                       "const char *func_02010000(char *s)\n{\n"
                       "    return s;\n}\n"}) as t:
            self.assertEqual(t.check_c()[0], [])

    def test_struct_elaborator_is_not_a_type_difference(self):
        with Tree("extern Node*func_02043f4c(void*);\n",
                  src={"func_02043f4c.c":
                       "struct Node* func_02043f4c(struct Node **pp)\n{\n"
                       "    return *pp;\n}\n"}) as t:
            self.assertEqual(t.check_c()[0], [])

    def test_volatile_is_kept(self):
        """Not a spelling: it changes what the compiler may do with the value."""
        with Tree("extern volatile int func_02010000(void*);\n",
                  src={"func_02010000.c": "int func_02010000(char *s)\n{\n"
                                          "    return 0;\n}\n"}) as t:
            self.assertEqual(t.kinds(), {"func_02010000": "width-or-sign"})


class ReadsTheDefinitionAndNotADeclaration(unittest.TestCase):
    def test_extern_c_definition_in_a_cpp_file_is_read(self):
        """134 bodies are spelled this way, and the first reader skipped them all.

        A C++ translation unit has to give the symbol C linkage to keep the
        unmangled name, so the definition opens with the same word a
        declaration does. What follows the parameter list is the difference.
        """
        body = ('extern "C" void func_01ffb07c(void *self)\n{\n}\n')
        with Tree("extern int func_01ffb07c(void*);\n",
                  src={"func_01ffb07c.cpp": body}) as t:
            rows, acct = t.check_c()
            self.assertEqual(acct["no_definition"], 0)
            self.assertEqual([(r[0], r[2]) for r in rows],
                             [("func_01ffb07c", "void")])

    def test_a_declaration_in_the_body_file_is_not_the_definition(self):
        """The file re-declares the symbol above its own body.

        Reading the declaration would compare decl_common.h's row against a copy
        of itself and report agreement no matter what the body says -- a false
        green, which is the failure this whole module exists to prevent.
        """
        body = ("extern void func_02010000(char *s);\n\n"
                "int func_02010000(char *s)\n{\n    return 1;\n}\n")
        with Tree("extern void func_02010000(void*);\n",
                  src={"func_02010000.c": body}) as t:
            self.assertEqual([(r[0], r[2]) for r in t.check_c()[0]],
                             [("func_02010000", "int")])

    def test_a_return_type_named_C_survives_the_linkage_strip(self):
        """`extern "C" int func_0200ca50(C *self)` -- the tree really has this.

        Stripping `extern`, `"`, `C`, `"` token by token eats a return type
        named C as well, so the specifier is removed as one quoted unit.
        """
        body = 'extern "C" C *func_0200ca50(C *self)\n{\n    return self;\n}\n'
        self.assertEqual(C.body_return_type(body, "func_0200ca50"), "C *")

    def test_call_sites_are_not_read_as_definitions(self):
        body = ("void func_0200aaaa(void)\n"
                "{\n"
                "    int x;\n"
                "    x = func_0200bbbb(1);\n"
                "    if (func_0200bbbb(2)) {\n"
                "        func_0200bbbb(3);\n"
                "    }\n"
                "}\n")
        decl = ("extern void func_0200aaaa(void);\n"
                "extern int func_0200bbbb(int);\n")
        with Tree(decl, src={"func_0200aaaa.c": body}) as t:
            rows, acct = t.check_c()
            self.assertEqual(rows, [])
            self.assertEqual(acct["compared"], 1)
            self.assertEqual(acct["no_definition"], 1)

    def test_an_indented_definition_is_read(self):
        """Two files in src/ indent their entire contents by four spaces."""
        body = ("    void func_0201cebc(short a0)\n"
                "    {\n"
                "        return;\n"
                "    }\n")
        with Tree("extern int func_0201cebc(short);\n",
                  src={"func_0201cebc.c": body}) as t:
            self.assertEqual(t.kinds(), {"func_0201cebc": "void-vs-value"})

    def test_nested_parentheses_in_the_parameter_list_are_balanced(self):
        """`void func_x(void (*cb)(int))` closes three times and ends once."""
        body = ("void func_02010000(void (*cb)(int), int n)\n{\n}\n")
        with Tree("extern int func_02010000(void*, int);\n",
                  src={"func_02010000.c": body}) as t:
            self.assertEqual(t.kinds(), {"func_02010000": "void-vs-value"})

    def test_a_comment_naming_the_function_is_not_a_definition(self):
        body = ("/* func_02010000(s) returns void on the failure path\n"
                "   and is called from two places */\n"
                "int func_02010000(char *s)\n{\n    return 1;\n}\n")
        self.assertEqual(C.body_return_type(body, "func_02010000"), "int")

    def test_a_body_folded_into_a_consolidated_tu_is_found(self):
        """405 rows have no src/<name>.c, and 290 of them are in a file like this.

        Indexing by filename calls those bodies missing; indexing by what each
        file DEFINES finds them, which is the whole reason the index is built
        from a scan rather than from a directory listing.
        """
        tu = ('extern "C" int func_ov002_020bf5e0(void)\n{\n    return 1;\n}\n'
              '\n'
              'extern "C" void func_ov002_020bf5e4(void)\n{\n}\n')
        decl = ("extern void func_ov002_020bf5e0(void);\n"
                "extern void func_ov002_020bf5e4(void);\n")
        with Tree(decl, src={"func_ov002_020d4748.cpp": tu}) as t:
            rows, acct = t.check_c()
            self.assertEqual(acct["compared"], 2)
            self.assertEqual([(r[0], r[4]) for r in rows],
                             [("func_ov002_020bf5e0",
                               "src/func_ov002_020d4748.cpp")])

    def test_a_row_with_no_body_anywhere_is_counted_not_guessed(self):
        """One row on main is this: the body was renamed out of the family.

        src/engine/message/func_0201fe08.c defines
        LoadMessageBankForLanguage, and decl_common.h still carries the old
        name. Counting it is how that shows up instead of being invisible.
        """
        with Tree("extern void func_0201fe08(void);\n",
                  src={"func_0201fe08.c":
                       "void LoadMessageBankForLanguage(void)\n{\n}\n"}) as t:
            rows, acct = t.check_c()
            self.assertEqual(rows, [])
            self.assertEqual(acct["no_definition"], 1)
            self.assertEqual(acct["compared"], 0)

    def test_an_asm_body_gets_its_own_class(self):
        """Its C return type is a convenience; the real one is in the opcodes.

        Reported so a reader knows the row exists, and kept out of the classes a
        fixing lane works, because "correct the declaration to match the body"
        is the wrong instruction when the body is hand-written assembly.
        """
        body = "asm void func_0205a588(void *p, int a, int b)\n{\n}\n"
        with Tree("extern void*func_0205a588(void*, int, int);\n",
                  src={"func_0205a588.c": body}) as t:
            rows, acct = t.check_c()
            self.assertEqual(t.kinds(), {"func_0205a588": "asm-body"})
            self.assertEqual(acct["asm_body"], 1)

    def test_an_asm_body_that_agrees_is_not_a_disagreement(self):
        """Four of the five asm rows agree once `asm` is set aside."""
        body = "asm void func_02052514(void)\n{\n}\n"
        with Tree("extern void func_02052514(void);\n",
                  src={"func_02052514.c": body}) as t:
            rows, acct = t.check_c()
            self.assertEqual(rows, [])
            self.assertEqual(acct["asm_body"], 1)

    def test_decl_row_sigil_glued_to_the_name_is_read(self):
        """`extern char*func_02023178(int);` -- 384 of decl_common.h is this."""
        decl = "extern char*func_02023178(int);\n"
        found = [(m.group(1), m.group(2)) for m in C.C_DECL_ROW.finditer(decl)]
        self.assertEqual(found, [("char*", "func_02023178")])


class ThePlainCHalfIsAdvisory(unittest.TestCase):
    def test_plain_c_rows_alone_exit_zero(self):
        """Nothing in .github/workflows runs this tool, and these rows are all
        pre-existing on main, so they are reported and decide nothing."""
        with Tree("extern void func_0200c66c(void*);\n",
                  src={"func_0200c66c.c": "int func_0200c66c(char *s)\n{\n"
                                          "    return 1;\n}\n"}) as t:
            buf = io.StringIO()
            with contextlib.redirect_stdout(buf):
                rc = C.main(["--root", str(t.root)])
            self.assertEqual(rc, 0)
            out = buf.getvalue()
            self.assertIn("PLAIN C ADVISORY", out)
            self.assertIn("1 plain C row(s) disagree", out)
            self.assertIn("0 symbol(s) declared with two different return types",
                          out)

    def test_the_mangled_count_stays_the_last_line(self):
        """pr.txt and the lane reports quote it as the last line of the run."""
        with Tree("extern void func_0200c66c(void*);\n"
                  "extern void _ZN3Foo3BarEv(void*);\n",
                  Foo="struct Foo {\n    virtual int Bar();\n};\n",
                  src={"func_0200c66c.c": "int func_0200c66c(char *s)\n{\n"
                                          "    return 1;\n}\n"}) as t:
            buf = io.StringIO()
            with contextlib.redirect_stdout(buf):
                rc = C.main(["--root", str(t.root)])
            self.assertEqual(rc, 1)
            lines = [l for l in buf.getvalue().splitlines() if l.strip()]
            self.assertEqual(lines[-1],
                             "1 symbol(s) declared with two different return types")

    def test_no_plain_c_restores_the_original_output(self):
        with Tree("extern void func_0200c66c(void*);\n",
                  src={"func_0200c66c.c": "int func_0200c66c(char *s)\n{\n"
                                          "    return 1;\n}\n"}) as t:
            buf = io.StringIO()
            with contextlib.redirect_stdout(buf):
                rc = C.main(["--root", str(t.root), "--no-plain-c"])
            self.assertEqual(rc, 0)
            self.assertNotIn("PLAIN C", buf.getvalue())

    def test_the_two_halves_do_not_read_each_other_s_rows(self):
        decl = ("extern void _ZN3Foo3BarEv(void*);\n"
                "extern void func_0200c66c(void*);\n")
        with Tree(decl, Foo="struct Foo {\n    virtual void Bar();\n};\n",
                  src={"func_0200c66c.c": "int func_0200c66c(char *s)\n{\n"
                                          "    return 1;\n}\n"}) as t:
            _bad, macct = t.check()
            _rows, cacct = t.check_c()
            self.assertEqual(macct["rows"], 1)   # only the _Z row
            self.assertEqual(cacct["rows"], 1)   # only the func_ row

    def test_a_class_header_that_redeclares_the_row_differently(self):
        """The disagreement that does not compile, rather than one that lies.

        decl_common.h and a class header are both included by the same
        translation unit, so two different return types for one extern "C" name
        is `illegal overloading` and the file does not build. Four such pairs
        were created by correcting decl_common.h alone, and prepush_linkcheck
        reported the breakage as NO-SYM -- a WARNING -- because a compile that
        produces no object produces no length to compare.
        """
        with Tree('extern void*func_ov006_020c3e70(char*);\n',
                  dScMgFlower_c='extern "C" void func_ov006_020c3e70(char *t);\n',
                  src={"func_ov006_020c3e70.c":
                       "void *func_ov006_020c3e70(char *t)\n{\n    return t;\n}\n"}) as t:
            rows, acct = t.check_c()
            self.assertEqual(acct["also_in_a_class_header"], 1)
            self.assertEqual([(r[0], r[3], r[4]) for r in rows],
                             [("func_ov006_020c3e70", "two-headers",
                               "include/dScMgFlower_c.h:1")])

    def test_a_class_header_that_agrees_is_silent(self):
        with Tree('extern void*func_ov006_020c3e70(char*);\n',
                  dScMgFlower_c='extern "C" void *func_ov006_020c3e70(char *t);\n',
                  src={"func_ov006_020c3e70.c":
                       "void *func_ov006_020c3e70(char *t)\n{\n    return t;\n}\n"}) as t:
            rows, acct = t.check_c()
            self.assertEqual(acct["also_in_a_class_header"], 1)
            self.assertEqual(rows, [])

    def test_a_class_header_is_checked_even_with_no_body(self):
        """Two headers disagree whether or not anyone has recovered the body."""
        with Tree("extern int func_02010000(void);\n",
                  SomeClass='extern "C" void func_02010000(void);\n') as t:
            rows, acct = t.check_c()
            self.assertEqual(acct["compared"], 0)
            self.assertEqual([r[3] for r in rows], ["two-headers"])

    def test_a_row_with_no_body_and_no_symbol_is_a_dead_row(self):
        """112 rows on main are this: a name renamed away, the row left behind.

        Split from the 5 that still name a real symbol, because one is a
        cleanup and the other is a function nobody has recovered yet. Reporting
        117 for both makes the check look 117 rows short of its reach when it
        is 5.
        """
        with Tree("extern void func_01ffb0fc(void);\n"
                  "extern void func_02010000(void);\n",
                  symbols={"config/arm9/symbols.txt":
                           "func_02010000 kind:function(arm,size=0x10)"
                           " addr:0x02010000\n"}) as t:
            _rows, acct = t.check_c()
            self.assertEqual(acct["no_definition"], 2)
            self.assertEqual(acct["dead_row"], 1)       # func_01ffb0fc
            self.assertEqual(acct["unrecovered"], 1)    # func_02010000

    def test_with_no_symbol_table_the_split_is_not_invented(self):
        """A tree with no config/ reports the total and claims nothing more."""
        with Tree("extern void func_01ffb0fc(void);\n") as t:
            _rows, acct = t.check_c()
            self.assertEqual(acct["no_definition"], 1)
            self.assertEqual(acct["symbol_table"], 0)
            self.assertEqual((acct["dead_row"], acct["unrecovered"]), (0, 0))

    def test_a_tree_with_no_src_is_not_an_error(self):
        """--root at a tree that has headers and no sources still runs."""
        with Tree("extern void func_0200c66c(void*);\n") as t:
            rows, acct = t.check_c()
            self.assertEqual(rows, [])
            self.assertEqual(acct["no_definition"], 1)


if __name__ == "__main__":
    unittest.main()
