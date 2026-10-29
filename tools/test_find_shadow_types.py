import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import find_shadow_types as F


class StripCommentsTests(unittest.TestCase):
    def test_removes_line_and_block_comments(self):
        # whitespace outside the comment is left exactly as it was
        self.assertEqual(F.strip_comments("a // b\nc"), "a \nc")
        self.assertEqual(F.strip_comments("a /* b */ c"), "a  c")

    def test_preserves_line_numbers_across_a_block_comment(self):
        text = "one\n/* two\nthree\n*/four\n"
        self.assertEqual(F.strip_comments(text).count("\n"), text.count("\n"))

    def test_does_not_strip_inside_a_string_literal(self):
        self.assertIn('"a // b"', F.strip_comments('x = "a // b";'))

    def test_unterminated_block_comment_does_not_hang(self):
        self.assertNotIn("struct", F.strip_comments("/* struct Vec3 { int x, y, z; };"))


class TwinTests(unittest.TestCase):
    def test_finds_a_comma_declared_triple(self):
        found = F.find_twins("struct Vec3i { int x, y, z; };")
        self.assertEqual(len(found), 1)
        self.assertEqual(found[0]["name"], "Vec3i")
        self.assertEqual(found[0]["stands_for"], "Vector3")

    def test_finds_a_separately_declared_triple(self):
        # the shape the first scan of Player.cpp missed, hiding 4 of 32
        found = F.find_twins("struct Vec3 {\n    int x;\n    int y;\n    int z;\n};")
        self.assertEqual(len(found), 1)
        self.assertEqual(found[0]["name"], "Vec3")

    def test_finds_a_typedef_without_a_tag(self):
        found = F.find_twins("typedef struct { s32 x, y, z; } Vec3b;")
        self.assertEqual(found[0]["name"], "Vec3b")

    def test_flags_a_twin_that_shadows_the_real_name(self):
        found = F.find_twins("struct Vector3 { int x, y, z; };")
        self.assertTrue(found[0]["shadows_real_name"])

    def test_s16_triple_is_offered_both_candidates(self):
        found = F.find_twins("struct Vec16 { s16 x, y, z; };")
        self.assertIn("Vector3_16", found[0]["stands_for"])
        self.assertIn("Vector3_16f", found[0]["stands_for"])

    def test_flags_a_volatile_twin_as_a_codegen_pin(self):
        found = F.find_twins("volatile struct { int x, y, z; } v;")
        self.assertTrue(found[0]["volatile"])

    def test_ignores_a_commented_out_twin(self):
        self.assertEqual(F.find_twins("/* struct Vec3 { int x, y, z; }; */"), [])

    def test_ignores_a_struct_with_a_method(self):
        self.assertEqual(F.find_twins("struct V { int x, y, z; void set(); };"), [])

    def test_ignores_wrong_member_names_and_counts(self):
        self.assertEqual(F.find_twins("struct A { int a, b, c; };"), [])
        self.assertEqual(F.find_twins("struct B { int x, y; };"), [])
        self.assertEqual(F.find_twins("struct C { int x, y, z, w; };"), [])

    def test_ignores_a_mixed_type_triple(self):
        self.assertEqual(F.find_twins("struct D { int x, y; short z; };"), [])

    def test_reports_the_definition_line(self):
        found = F.find_twins("\n\n\nstruct Vec3 { int x, y, z; };")
        self.assertEqual(found[0]["line"], 4)


class ScaffoldTests(unittest.TestCase):
    def test_finds_an_all_placeholder_scaffold(self):
        src = """struct StarDoor {
            virtual void v00(); virtual void v01(); virtual int GetType();
        };"""
        found = F.find_scaffolds(src)
        self.assertEqual(len(found), 1)
        self.assertEqual(found[0]["name"], "StarDoor")
        self.assertEqual(found[0]["placeholders"], 2)
        self.assertEqual(found[0]["named"], ["GetType"])
        self.assertEqual(found[0]["reaches_slot"], 2)

    def test_recognises_several_placeholder_spellings(self):
        src = ("struct S { virtual void m04(); virtual void Virtual10(); "
               "virtual void vfunc3(); virtual void unk12(); };")
        self.assertEqual(F.find_scaffolds(src)[0]["placeholders"], 4)

    def test_ignores_a_class_of_real_methods(self):
        src = "struct Model { virtual void UpdateVerts(); virtual void Render(); };"
        self.assertEqual(F.find_scaffolds(src), [])

    def test_threshold_is_respected(self):
        src = "struct S { virtual void v00(); virtual void Real(); };"
        self.assertEqual(F.find_scaffolds(src, threshold=2), [])
        self.assertEqual(len(F.find_scaffolds(src, threshold=1)), 1)

    def test_ignores_a_commented_out_scaffold(self):
        # the d_a_demo.cpp case: a TUBUILD CONFLICT block a grep counted as live
        src = """/* TUBUILD CONFLICT ... NOT applied:
        struct Obj { virtual void v00(); virtual void m04(); };
        */"""
        self.assertEqual(F.find_scaffolds(src), [])

    def test_handles_a_nested_brace_in_the_body(self):
        src = """struct S {
            virtual void v00();
            virtual void v01();
            struct Inner { int a; };
        };"""
        self.assertEqual(F.find_scaffolds(src)[0]["placeholders"], 2)


class ScanTests(unittest.TestCase):
    def test_scan_tags_each_finding_with_its_file(self):
        here = pathlib.Path(__file__).resolve().parent
        findings = F.scan([str(here / "find_shadow_types.py")])
        for f in findings:
            self.assertTrue(f["file"].endswith("find_shadow_types.py"))

    def test_selectors_restrict_the_kinds_reported(self):
        here = pathlib.Path(__file__).resolve().parent
        path = str(here / "test_find_shadow_types.py")
        only_twins = F.scan([path], want_scaffolds=False)
        self.assertTrue(all(f["kind"] == "pod-twin" for f in only_twins))


class FilesFromTests(unittest.TestCase):
    def test_reads_paths_from_a_file_and_skips_blank_lines(self):
        here = pathlib.Path(__file__).resolve().parent
        with tempfile.TemporaryDirectory() as td:
            listing = pathlib.Path(td) / "paths.txt"
            listing.write_text(str(here / "find_shadow_types.py") + "\n\n",
                               encoding="utf-8")
            self.assertEqual(F.main(["--files-from", str(listing), "--format", "json"]), 0)

    def test_errors_when_given_no_input_at_all(self):
        with self.assertRaises(SystemExit):
            F.main([])


if __name__ == "__main__":
    unittest.main()
