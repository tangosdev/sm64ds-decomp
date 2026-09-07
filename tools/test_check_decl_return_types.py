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
    """A throwaway include/ tree: `decl_common.h` plus named headers."""

    def __init__(self, decl, **headers):
        self.dir = tempfile.TemporaryDirectory()
        root = pathlib.Path(self.dir.name)
        (root / "include").mkdir()
        (root / "include" / "decl_common.h").write_text(decl, encoding="utf-8")
        for name, text in headers.items():
            (root / "include" / (name + ".h")).write_text(text, encoding="utf-8")
        self.root = root

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.dir.cleanup()

    def check(self):
        return C.check(self.root)


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


if __name__ == "__main__":
    unittest.main()
