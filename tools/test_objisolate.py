"""What objisolate has to refuse.

The vtable-addend case is the one that matters. mwcc's `_ZTV<C>` addresses the start
of the vtable object, so a vptr store relocates against it with an addend of 8 to
step over offset-to-top and typeinfo; symbols.txt's `_ZTV<C>` IS the slot array.
Getting that wrong LINKS CLEANLY and writes a vptr one entry past the truth -- it
cost 76 functions across 34 modules before the byte compare caught it, and no gate
earlier than the byte compare can see it. If only one of these tests is ever kept,
keep the addend ones.

These compile real objects with the pinned mwccarm, because the whole subject is what
a specific compiler emits; a hand-built ELF fixture would test this file's idea of
mwcc rather than mwcc. They skip when the compiler is absent, so a checkout without
tools/mwccarm still runs the rest of the suite.
"""
import os
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import objisolate as OI  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent
MW = REPO / "tools" / "mwccarm"


def _compiler():
    try:
        from rombuild import VERSION, CFLAGS
    except Exception:
        return None
    exe = MW / VERSION / "mwccarm.exe"
    return (exe, CFLAGS) if exe.is_file() else None


@unittest.skipUnless(_compiler(), "mwccarm not present")
class Isolate(unittest.TestCase):
    def build(self, source):
        exe, cflags = _compiler()
        d = pathlib.Path(self.tmp.name)
        src, obj = d / "t.cpp", d / "t.o"
        src.write_text("//cpp\n" + source, encoding="utf-8")
        r = subprocess.run(
            [*os.environ.get("MWCCARM_LAUNCHER", "").split(), str(exe),
             *cflags.replace("-lang c99", "-lang c++").split(),
             "-i", str(REPO / "include"), "-c", str(src), "-o", str(obj)],
            capture_output=True, text=True, cwd=REPO,
            env=dict(os.environ, LM_LICENSE_FILE=str(MW / "license.dat")))
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)
        return obj

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()

    def tearDown(self):
        self.tmp.cleanup()

    def test_plain_destructor_is_isolable(self):
        """The case the whole pass exists for: D0/D1/D2 + vtable reduced to one."""
        obj = self.build("struct P { int p[4]; virtual ~P(); virtual void f(); };\n"
                         "P::~P(){}\n")
        plan = OI.plan(obj.read_bytes(), "_ZN1PD1Ev")
        self.assertIsNone(plan["error"])
        self.assertIn("_ZTV1P", plan["externalise"])
        self.assertIn("_ZN1PD0Ev", plan["dead"])

    def test_vtable_addend_is_corrected_to_zero(self):
        """8 -> 0, because the ROM symbol is already past the preamble."""
        from elftools.elf.elffile import ELFFile
        from elftools.elf.relocation import RelocationSection
        import io
        obj = self.build("struct P { int p[4]; virtual ~P(); virtual void f(); };\n"
                         "P::~P(){}\n")
        before = self._vtable_addends(obj, "_ZN1PD1Ev")
        self.assertEqual(before, [8], "mwcc no longer emits the preamble-skip addend; "
                                      "the -8 correction needs re-deriving")
        OI.isolate(obj, "_ZN1PD1Ev")
        self.assertEqual(self._vtable_addends(obj, "_ZN1PD1Ev"), [0])
        # ...and re-running must not subtract another 8.
        OI.isolate(obj, "_ZN1PD1Ev")
        self.assertEqual(self._vtable_addends(obj, "_ZN1PD1Ev"), [0])
        del ELFFile, RelocationSection, io

    def _vtable_addends(self, obj, keep):
        import io
        from elftools.elf.elffile import ELFFile
        from elftools.elf.relocation import RelocationSection
        elf = ELFFile(io.BytesIO(obj.read_bytes()))
        st = elf.get_section_by_name(".symtab")
        idx = [s["st_shndx"] for s in st.iter_symbols()
               if s.name == keep and s["st_shndx"] != "SHN_UNDEF"]
        out = []
        for s in elf.iter_sections():
            if isinstance(s, RelocationSection) and s.header["sh_info"] in idx:
                for r in s.iter_relocations():
                    if st.get_symbol(r["r_info_sym"]).name.startswith("_ZTV"):
                        out.append(r["r_addend"])
        return out

    def test_refuses_ctor_only_tu(self):
        """A constructor's TU references the vtable without defining it.

        The vtable's key function is the DESTRUCTOR, so a TU defining only `V::V()`
        leaves `_ZTV1V` UNDEF -- which means it is never a candidate for
        externalisation, and a guard that only inspects externalised symbols never
        looks at it. The addend is still 8. Refusing keeps this out of the pass list
        instead of letting it link and write the vptr one slot high."""
        obj = self.build("struct V { int p[4]; V(); virtual ~V(); virtual void f(); };\n"
                         "V::V(){}\n")
        self.assertIn("_ZTV1V", OI.plan(obj.read_bytes(), "_ZN1VC1Ev")["error"])

    def test_refuses_inlined_base_vtable_store(self):
        """A derived dtor over an INLINE base dtor stores the base's vptr too.

        The object's own `_ZTV1D` is in a dropped section and gets corrected; the
        inlined `_ZTV1B` store is UNDEF and would not have been."""
        obj = self.build("struct B { int p[4]; virtual ~B(){} virtual void f(); };\n"
                         "struct D : B { virtual ~D(); };\n"
                         "D::~D(){}\n")
        self.assertIn("_ZTV1B", OI.plan(obj.read_bytes(), "_ZN1DD1Ev")["error"])

    def test_local_static_is_reported_not_silently_dropped(self):
        """A function-local static cannot be isolated away.

        Its `.bss` comes from the ROM's gap object, and the symbol is STB_LOCAL so
        nothing outside can supply it. isolate must externalise it anyway, so that
        eligible.py rule 5 fails to find it in symbols.txt and rejects the file --
        rather than leaving it defined at offset 0 of an emptied section, which the
        lcf places at the kept function's own address."""
        obj = self.build("int f(int);\nint g(int n){ static int t[8]; return t[n&7]+f(n); }\n")
        plan = OI.plan(obj.read_bytes(), "_Z1gi")
        self.assertIsNone(plan["error"])
        self.assertTrue(plan["externalise"],
                        "the referenced local static must be externalised")


if __name__ == "__main__":
    unittest.main()
