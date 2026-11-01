"""What pad_layout_audit counts as a pad member and as a header shadow."""
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import pad_layout_audit as P  # noqa: E402


def pads(text, path="src/x.cpp"):
    return [(p["name"], p["line"]) for s in P.parse(path, text) for p in s["pads"]]


def structs(text, path="src/x.cpp"):
    return {s["name"]: s for s in P.parse(path, text)}


class PadMembers(unittest.TestCase):
    def test_pad_named_arrays_count_and_real_names_do_not(self):
        text = ("struct A {\n"
                "    u8 pad_3ba[0x2];\n"
                "    char unk[4];\n"
                "    u8 _10[0x10];\n"
                "    u8 buffer[0x20];\n"
                "    u32 padCount;\n"
                "};\n")
        self.assertEqual(pads(text), [("pad_3ba", 2), ("unk", 3), ("_10", 4)])

    def test_wider_integer_pads_count_but_are_not_char(self):
        (s,) = P.parse("src/x.cpp", "struct A { u32 reserved[2]; };\n")
        self.assertEqual([(p["name"], p["char"]) for p in s["pads"]], [("reserved", False)])

    def test_comments_and_strings_are_not_parsed(self):
        text = ('/* struct B { u8 pad[4]; }; */\n'
                'const char *s = "struct C { u8 pad[4]; }";\n'
                '// struct D { u8 pad[4]; };\n')
        self.assertEqual(P.parse("src/x.cpp", text), [])

    def test_array_size_is_not_an_offset_comment(self):
        text = ("struct A {\n"
                "    u8 pad[0x2];\n"
                "    u8 gap[0x2]; /* 0x02 */\n"
                "    u8 pad_04[4];\n"
                "};\n")
        (s,) = P.parse("src/x.cpp", text)
        self.assertEqual([(p["offset_comment"], p["offset_named"]) for p in s["pads"]],
                         [(False, False), (True, False), (False, True)])

    def test_access_specifier_does_not_move_the_member_line(self):
        self.assertEqual(pads("class A {\npublic:\n    u8 pad[4];\n};\n"), [("pad", 3)])

    def test_enum_class_base_is_not_a_struct(self):
        self.assertEqual(P.parse("src/x.cpp", "enum class E : u8 { pad };\n"), [])

    def test_host_only_msvc_arm_is_not_read_in_src(self):
        text = ("#ifdef _MSC_VER\n"
                "struct Host { u8 pad[4]; };\n"
                "#else\n"
                "struct Rom { u8 pad[4]; };\n"
                "#endif\n")
        with tempfile.TemporaryDirectory() as d:
            root = pathlib.Path(d)
            (root / "src").mkdir()
            (root / "src/x.cpp").write_text(text)
            r = P.audit(root, ["src/x.cpp"])
        self.assertEqual([p["struct"] for p in r["_pads"]], ["Rom"])


class Scope(unittest.TestCase):
    def test_namespace_qualifies_and_extern_c_is_transparent(self):
        text = ("namespace Particle {\n"
                "struct System { u8 pad[4]; };\n"
                "}\n"
                'extern "C" {\n'
                "struct Plain { int x; };\n"
                "}\n")
        got = structs(text)
        self.assertTrue(got["Particle::System"]["file_scope"])
        self.assertTrue(got["Plain"]["file_scope"])

    def test_nested_and_function_local_structs_are_not_file_scope(self):
        text = ("struct Outer {\n"
                "    struct Inner { u8 pad[4]; } in;\n"
                "};\n"
                "void f() {\n"
                "    struct Local { u8 pad[4]; } l;\n"
                "}\n")
        got = structs(text)
        self.assertTrue(got["Outer"]["file_scope"])
        self.assertFalse(got["Inner"]["file_scope"])
        self.assertFalse(got["Local"]["file_scope"])

    def test_anonymous_typedef_takes_its_typedef_name(self):
        got = structs("typedef struct {\n    u8 pad[4];\n} Thing;\n")
        self.assertIn("Thing", got)
        self.assertTrue(got["Thing"]["file_scope"])


class Shadows(unittest.TestCase):
    def audit(self, files):
        with tempfile.TemporaryDirectory() as d:
            root = pathlib.Path(d)
            for path, text in files.items():
                (root / path).parent.mkdir(parents=True, exist_ok=True)
                (root / path).write_text(text)
            return P.audit(root, sorted(files))

    def test_local_copy_of_a_header_class_is_a_shadow(self):
        r = self.audit({
            "include/Foo.h": "struct Foo { int a; int b; };\n",
            "src/a.cpp": "struct Foo { u8 pad[4]; int b; };\n",
            "src/b.cpp": "struct Bar { u8 pad[4]; };\n",
        })
        self.assertEqual([(s["file"], s["name"], s["pads"]) for s in r["_shadows"]],
                         [("src/a.cpp", "Foo", 1)])
        self.assertEqual((r["header_shadows"], r["header_shadows_padded"]), (1, 1))

    def test_namespace_must_agree(self):
        r = self.audit({
            "include/P.h": "namespace Particle { struct System { int a; }; }\n",
            "src/a.cpp": "struct System { u8 pad[4]; };\n",
            "src/b.cpp": "namespace Particle { struct System { u8 pad[4]; }; }\n",
        })
        self.assertEqual([s["file"] for s in r["_shadows"]], ["src/b.cpp"])

    def test_nested_and_anonymous_structs_are_never_shadows(self):
        r = self.audit({
            "include/E.h": "struct Element { int a; };\ntypedef struct { int a; } *P;\n",
            "src/a.cpp": "struct Owner { struct Element { u8 pad[4]; } e; };\n",
            "src/b.cpp": "struct { u8 pad[4]; } g;\n",
        })
        self.assertEqual(r["_shadows"], [])

    def test_vendor_headers_name_shadows_but_their_pads_are_not_counted(self):
        r = self.audit({
            "include/nitro/os.h": "typedef struct OSThread { u8 reserved[8]; } OSThread;\n",
            "src/a.c": "typedef struct OSThread { u8 pad[8]; } OSThread;\n",
        })
        self.assertEqual(r["pad_members"], 1)
        self.assertEqual([s["name"] for s in r["_shadows"]], ["OSThread"])


if __name__ == "__main__":
    unittest.main()
