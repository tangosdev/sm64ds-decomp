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

    def test_derive_is_isolate_without_the_write(self):
        """`derive` must be `isolate`'s reduction as a pure function of bytes.

        Translation-unit reconstruction reduces ONE compiled object N times, once per
        function it contributes (plan sec 9), which an in-place mutator cannot do: the
        first call consumes the input. That only works if the two agree exactly -- if
        `derive` drifted from `isolate`, the derived objects would stop being comparable
        to the ones the ROM build produces, which is the only evidence that makes the
        substitution safe."""
        obj = self.build("struct P { int p[4]; virtual ~P(); virtual void f(); };\n"
                         "P::~P(){}\n")
        raw = obj.read_bytes()
        out, plan = OI.derive(raw, "_ZN1PD1Ev")
        self.assertIsNone(plan["error"])
        self.assertEqual(obj.read_bytes(), raw, "derive must not touch the file")
        self.assertEqual(OI.derive(raw, "_ZN1PD1Ev")[0], out, "derive must be pure")
        OI.isolate(obj, "_ZN1PD1Ev")
        self.assertEqual(obj.read_bytes(), out, "isolate and derive must agree")
        # The reduction the TU case actually needs: the SAME untouched input reduced to
        # a different function, which is only expressible without the in-place write.
        other, plan2 = OI.derive(raw, "_ZN1PD0Ev")
        self.assertIsNone(plan2["error"])
        self.assertNotEqual(other, out)

    def test_exact_deadstrip_removes_unreferenced_compiler_only_d2(self):
        """A whole-TU scratch link may model the retail link's discarded D2.

        The operation is exact-symbol and pure: D2's section disappears, D0/D1 and
        the data emitted by the same real destructor remain, and the input object is
        untouched.  This is deliberately not ordinary one-function isolation.
        """
        import io
        from elftools.elf.elffile import ELFFile

        obj = self.build("struct P { int p[4]; virtual ~P(); virtual void f(); };\n"
                         "P::~P(){}\n")
        raw = obj.read_bytes()
        out, plan = OI.derive_deadstrip(raw, ["_ZN1PD2Ev"])
        self.assertIsNone(plan["error"])
        self.assertEqual(obj.read_bytes(), raw)
        self.assertIsNotNone(out)

        elf = ELFFile(io.BytesIO(out))
        syms = {s.name: s for s in elf.get_section_by_name(".symtab").iter_symbols()}
        self.assertEqual(syms["_ZN1PD2Ev"]["st_shndx"], "SHN_UNDEF")
        self.assertNotEqual(syms["_ZN1PD0Ev"]["st_shndx"], "SHN_UNDEF")
        self.assertNotEqual(syms["_ZN1PD1Ev"]["st_shndx"], "SHN_UNDEF")
        self.assertTrue(any(s.name == ".data" and s.header["sh_size"]
                            for s in elf.iter_sections()))

    def test_exact_deadstrip_refuses_a_surviving_reference(self):
        """Calling something is proof it is not dead compiler output."""
        obj = self.build('extern "C" int helper(int x) { return x + 1; }\n'
                         'extern "C" int kept(int x) { return helper(x); }\n')
        out, plan = OI.derive_deadstrip(obj.read_bytes(), ["helper"])
        self.assertIsNone(out)
        self.assertIn("surviving", plan["error"])
        self.assertIn("references compiler-only helper", plan["error"])

    def test_exact_vague_rtti_externalization_keeps_named_imports(self):
        """A dedicated inherited RTTI section becomes an import, not lost bytes."""
        import io
        from elftools.elf.elffile import ELFFile
        from elftools.elf.relocation import RelocationSection

        obj = self.build(
            "struct Base { virtual ~Base() {} };\n"
            "struct A : Base { virtual void a(); }; void A::a() {}\n")
        raw = obj.read_bytes()
        out, plan = OI.derive_externalized(raw, ["_ZTI4Base", "_ZTS4Base"])
        self.assertIsNone(plan["error"])
        self.assertEqual(obj.read_bytes(), raw, "externalization must be pure")
        self.assertEqual(plan["externalise"], ["_ZTI4Base", "_ZTS4Base"])

        elf = ELFFile(io.BytesIO(out))
        symtab = elf.get_section_by_name(".symtab")
        syms = {s.name: s for s in symtab.iter_symbols()}
        self.assertEqual(syms["_ZTI4Base"]["st_shndx"], "SHN_UNDEF")
        self.assertEqual(syms["_ZTS4Base"]["st_shndx"], "SHN_UNDEF")
        self.assertNotEqual(syms["_ZTI1A"]["st_shndx"], "SHN_UNDEF")
        referenced = set()
        for sec in elf.iter_sections():
            if isinstance(sec, RelocationSection) and sec.header["sh_size"]:
                referenced.update(symtab.get_symbol(r["r_info_sym"]).name
                                  for r in sec.iter_relocations())
        self.assertIn("_ZTI4Base", referenced,
                      "the surviving derived RTTI must still import its base RTTI")

    def test_externalized_vague_rtti_links_to_one_canonical_definition(self):
        """mwldarm resolves the surviving derived RTTI through the exact import."""
        import io
        from elftools.elf.elffile import ELFFile

        canonical = self.build(
            "struct Base { virtual ~Base() {} };\n"
            "struct A : Base { virtual void a(); }; void A::a() {}\n").read_bytes()
        consumer = self.build(
            "struct Base { virtual ~Base() {} };\n"
            "struct B : Base { virtual void b(); }; void B::b() {}\n").read_bytes()
        externalized, plan = OI.derive_externalized(
            consumer, ["_ZTI4Base", "_ZTS4Base"])
        self.assertIsNone(plan["error"])

        from rombuild import LD_VERSION, MW, launcher
        d = pathlib.Path(self.tmp.name)
        canonical_o, consumer_o, linked = d / "canonical.o", d / "consumer.o", d / "linked.o"
        canonical_o.write_bytes(canonical)
        consumer_o.write_bytes(externalized)
        objects = d / "objects.txt"
        objects.write_text(f"{canonical_o}\n{consumer_o}\n", encoding="utf-8")

        runtime_symbols = set()
        for raw in (canonical, externalized):
            elf = ELFFile(io.BytesIO(raw))
            symtab = elf.get_section_by_name(".symtab")
            runtime_symbols.update(s.name for s in symtab.iter_symbols()
                                   if s.name and s["st_shndx"] == "SHN_UNDEF"
                                   and s.name not in ("_ZTI4Base", "_ZTS4Base"))
        runtime_defs = "\n".join(
            f"  {name} = 0x02010000;" for name in sorted(runtime_symbols))
        lcf = d / "fixture.lcf"
        lcf.write_text(
            "MEMORY { TEST : ORIGIN = 0x02000000 > linked.bin }\n"
            f"SECTIONS {{\n{runtime_defs}\n.fixture : {{\n"
            "  canonical.o(.text) consumer.o(.text) "
            "canonical.o(.data) consumer.o(.data)\n"
            "} > TEST }\n", encoding="utf-8")
        cmd = [*launcher(), str(MW / LD_VERSION / "mwldarm.exe"),
               "-proc", "arm946e", "-nostdlib", "-interworking", "-nodead",
               "-m", "_ZN1A1aEv", f"@{objects}", str(lcf), "-o", str(linked)]
        result = subprocess.run(cmd, capture_output=True, text=True, cwd=d)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        linked_elf = ELFFile(io.BytesIO(linked.read_bytes()))
        names = [s.name for s in linked_elf.get_section_by_name(".symtab").iter_symbols()
                 if s["st_shndx"] != "SHN_UNDEF"]
        self.assertEqual(names.count("_ZTI4Base"), 1)
        self.assertEqual(names.count("_ZTS4Base"), 1)
        self.assertIn("_ZTI1B", names,
                      "the consumer's surviving RTTI must participate in the link")

    def test_vague_rtti_externalization_refuses_a_shared_section(self):
        """The exact name cannot license a neighbour in the same input section."""
        import io
        import struct
        from elftools.elf.elffile import ELFFile

        obj = self.build(
            "struct Base { virtual ~Base() {} };\n"
            "struct A : Base { virtual void a(); }; void A::a() {}\n")
        raw = bytearray(obj.read_bytes())
        elf = ELFFile(io.BytesIO(bytes(raw)))
        symtab = elf.get_section_by_name(".symtab")
        symbols = list(symtab.iter_symbols())
        zti_section = next(s["st_shndx"] for s in symbols if s.name == "_ZTI4Base")
        zts_index = next(i for i, s in enumerate(symbols) if s.name == "_ZTS4Base")
        endian = "<" if elf.little_endian else ">"
        struct.pack_into(endian + "H", raw,
                         symtab.header["sh_offset"] + zts_index * 16 + 14,
                         zti_section)
        out, plan = OI.derive_externalized(bytes(raw), ["_ZTI4Base"])
        self.assertIsNone(out)
        self.assertIn("also defines", plan["error"])

    def test_vague_rtti_externalization_refuses_an_unnamed_section_reference(self):
        """A section-symbol addend cannot be preserved by a named import."""
        import io
        import struct
        from elftools.elf.elffile import ELFFile
        from elftools.elf.relocation import RelocationSection

        obj = self.build(
            "struct Base { virtual ~Base() {} };\n"
            "struct A : Base { virtual void a(); }; void A::a() {}\n")
        raw = bytearray(obj.read_bytes())
        elf = ELFFile(io.BytesIO(bytes(raw)))
        secs = list(elf.iter_sections())
        symtab = elf.get_section_by_name(".symtab")
        symbols = list(symtab.iter_symbols())
        dropped = next(s["st_shndx"] for s in symbols if s.name == "_ZTI4Base")
        # mwcc does not emit STT_SECTION entries for every repeated .data section.
        # Repurpose another fixture RTTI symbol into the exact unnamed shape an ELF
        # producer could legally use, then point a surviving reloc through it.
        section_symbol = next(i for i, s in enumerate(symbols)
                              if s.name == "_ZTS4Base")
        patched = False
        endian = "<" if elf.little_endian else ">"
        symbol_entry = symtab.header["sh_offset"] + section_symbol * 16
        struct.pack_into(endian + "III", raw, symbol_entry, 0, 0, 0)
        raw[symbol_entry + 12] = 0x03  # STB_LOCAL/STT_SECTION
        struct.pack_into(endian + "H", raw, symbol_entry + 14, dropped)
        for relsec in secs:
            if not isinstance(relsec, RelocationSection) or relsec.header["sh_info"] == dropped:
                continue
            for i, reloc in enumerate(relsec.iter_relocations()):
                if symtab.get_symbol(reloc["r_info_sym"]).name != "_ZTI4Base":
                    continue
                r_info = (section_symbol << 8) | reloc["r_info_type"]
                struct.pack_into(endian + "I", raw,
                                 relsec.header["sh_offset"]
                                 + i * relsec.header["sh_entsize"] + 4,
                                 r_info)
                patched = True
                break
            if patched:
                break
        self.assertTrue(patched, "fixture must reference the inherited RTTI by name")
        out, plan = OI.derive_externalized(bytes(raw), ["_ZTI4Base"])
        self.assertIsNone(out)
        self.assertIn("unnamed section", plan["error"])

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

    def test_corrects_ctor_only_tu(self):
        """A constructor's TU references the vtable without defining it.

        The vtable's key function is the DESTRUCTOR, so a TU defining only `V::V()`
        leaves `_ZTV1V` UNDEF -- never a candidate for externalisation, so a guard
        that only inspects externalised symbols never looks at it. The addend is
        still 8, and it needs the same correction every other vtable store gets:
        the ROM's symbol IS the slot array. This was refused while no enrolled
        instance existed to verify the correction against; see the next test."""
        obj = self.build("struct V { int p[4]; V(); virtual ~V(); virtual void f(); };\n"
                         "V::V(){}\n")
        self.assertIsNone(OI.plan(obj.read_bytes(), "_ZN1VC1Ev")["error"])
        OI.isolate(obj, "_ZN1VC1Ev")
        self.assertEqual(sorted(set(self._vtable_addends(obj, "_ZN1VC1Ev"))), [0])

    def test_corrects_inlined_base_vtable_store(self):
        """A derived dtor over an INLINE base dtor stores the base's vptr too.

        The object's own `_ZTV1D` is in a dropped section and gets corrected; the
        inlined `_ZTV1B` store is UNDEF and used to be refused. Both are addend 8
        against a symbol the ROM defines as the slot array, so both drop to 0.

        This is the shape `Scene::~Scene()` has, and it is what the correction was
        verified on -- rombuild links the module and byte-compares it against the
        ROM, the only check that caught the original 8-high vptr bug."""
        obj = self.build("struct B { int p[4]; virtual ~B(){} virtual void f(); };\n"
                         "struct D : B { virtual ~D(); };\n"
                         "D::~D(){}\n")
        self.assertIsNone(OI.plan(obj.read_bytes(), "_ZN1DD1Ev")["error"])
        OI.isolate(obj, "_ZN1DD1Ev")
        self.assertEqual(sorted(set(self._vtable_addends(obj, "_ZN1DD1Ev"))), [0])
        # Idempotent: re-running must not subtract another 8.
        OI.isolate(obj, "_ZN1DD1Ev")
        self.assertEqual(sorted(set(self._vtable_addends(obj, "_ZN1DD1Ev"))), [0])

    def test_corrects_a_multiple_inheritance_secondary_vptr(self):
        """A second base means a second vptr store, further into the vtable.

        Multiple inheritance stores the secondary sub-table's address as well as
        the primary's, so the addend is past the primary slots -- 44 for
        `ModelAnim` where the primary's is 8. The correction is the same
        subtraction, because mwcc's `_ZTV` addresses the vtable object's start
        while the ROM's IS the slot array, and the ROM names the destination
        independently: `_ZTV9ModelAnim` 0x0208e980 and
        `VTable_Animation_ModelAnimThunk` 0x0208e9a4 differ by 0x24, which is
        exactly 44 - 8.

        This was refused while no enrolled instance existed to check the
        arithmetic against. Both stores now drop by the preamble."""
        obj = self.build("struct B1 { int p[4]; virtual ~B1(){} virtual void f(); };\n"
                         "struct B2 { int q[4]; virtual ~B2(){} virtual void g(); };\n"
                         "struct M : B1, B2 { virtual ~M(); };\n"
                         "M::~M(){}\n")
        self.assertIsNone(OI.plan(obj.read_bytes(), "_ZN1MD1Ev")["error"])
        before = sorted(set(self._vtable_addends(obj, "_ZN1MD1Ev")))
        OI.isolate(obj, "_ZN1MD1Ev")
        after = sorted(set(self._vtable_addends(obj, "_ZN1MD1Ev")))
        self.assertEqual(after, [a - 8 for a in before])
        self.assertIn(0, after)          # the primary landed on the slot array

    def test_still_refuses_a_vtable_addend_below_the_preamble(self):
        """Correctable means "past the preamble". An addend under it is not.

        Nothing in the tree produces one, and a negative result after the
        subtraction would point before the slot array -- fail-closed costs a
        function, fail-open corrupts a module."""
        obj = self.build("struct V { int p[4]; virtual ~V(); virtual void f(); };\n"
                         "V::~V(){}\n")
        raw = bytearray(obj.read_bytes())
        self.assertIsNone(OI.plan(bytes(raw), "_ZN1VD1Ev")["error"])

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
