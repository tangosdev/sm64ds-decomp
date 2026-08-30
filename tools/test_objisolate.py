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

    def _plain_two_function_tu(self, extra=""):
        # mwcc emits function sections in reverse source order.  Defining Second
        # first therefore makes the object order match the ROM-order license below.
        obj = self.build(
            "struct Pair { void First(); void Second(); };\n"
            "void Pair::Second() {}\n"
            "void Pair::First() {}\n" + extra)
        return obj, ["_ZN4Pair5FirstEv", "_ZN4Pair6SecondEv"]

    def test_multi_symbol_isolation_preserves_one_exact_text_only_object(self):
        obj, names = self._plain_two_function_tu()
        raw = obj.read_bytes()
        out, plan = OI.derive_many(raw, names)
        self.assertIsNone(plan["error"])
        self.assertEqual(plan["keepSymbols"], names)
        self.assertEqual(len(plan["keep"]), 2)
        self.assertEqual(out, raw)
        OI.isolate_many(obj, names)
        self.assertEqual(obj.read_bytes(), raw)

    def test_multi_symbol_isolation_refuses_a_missing_member(self):
        obj, names = self._plain_two_function_tu()
        out, plan = OI.derive_many(
            obj.read_bytes(), [names[0], "_ZN4Pair7MissingEv"])
        self.assertIsNone(out)
        self.assertIn("0 defined symbols", plan["error"])

    def test_multi_symbol_isolation_refuses_ambiguous_definitions(self):
        import io
        import struct
        from elftools.elf.elffile import ELFFile

        obj, names = self._plain_two_function_tu()
        raw = bytearray(obj.read_bytes())
        elf = ELFFile(io.BytesIO(bytes(raw)))
        symtab = elf.get_section_by_name(".symtab")
        symbols = list(symtab.iter_symbols())
        first_index, first = next((i, s) for i, s in enumerate(symbols)
                                  if s.name == names[0]
                                  and s["st_shndx"] != "SHN_UNDEF")
        second_index, _second = next((i, s) for i, s in enumerate(symbols)
                                     if s.name == names[1]
                                     and s["st_shndx"] != "SHN_UNDEF")
        self.assertNotEqual(first_index, second_index)
        endian = "<" if elf.little_endian else ">"
        struct.pack_into(endian + "I", raw,
                         symtab.header["sh_offset"] + second_index * 16,
                         first["st_name"])

        out, plan = OI.derive_many(bytes(raw), names)
        self.assertIsNone(out)
        self.assertIn("2 defined symbols", plan["error"])

    def test_multi_symbol_isolation_refuses_unlicensed_helper_content(self):
        obj, names = self._plain_two_function_tu(
            'extern "C" int helper(int x) { return x + 1; }\n')
        out, plan = OI.derive_many(obj.read_bytes(), names)
        self.assertIsNone(out)
        self.assertIn("unlicensed content", plan["error"])
        self.assertIn("helper", plan["error"])

    def test_multi_symbol_isolation_refuses_unlicensed_data(self):
        obj, names = self._plain_two_function_tu(
            'extern "C" int owned_data = 1;\n')
        out, plan = OI.derive_many(obj.read_bytes(), names)
        self.assertIsNone(out)
        self.assertIn("unlicensed content", plan["error"])
        self.assertIn("owned_data", plan["error"])

    def test_multi_symbol_isolation_refuses_wrong_emission_order(self):
        # mwcc emits these function sections in reverse source order.  This spelling
        # therefore disagrees with the requested ROM order instead of silently
        # accepting an object that would swap the two contributions.
        obj = self.build(
            "struct Pair { void First(); void Second(); };\n"
            "void Pair::First() {}\n"
            "void Pair::Second() {}\n")
        names = ["_ZN4Pair5FirstEv", "_ZN4Pair6SecondEv"]
        out, plan = OI.derive_many(obj.read_bytes(), names)
        self.assertIsNone(out)
        self.assertIn("not emitted in ROM order", plan["error"])

    def test_multi_symbol_isolation_does_not_replace_singleton_semantics(self):
        obj, names = self._plain_two_function_tu()
        out, plan = OI.derive_many(obj.read_bytes(), [names[0]])
        self.assertIsNone(out)
        self.assertIn("at least two", plan["error"])
        singular, singular_plan = OI.derive(obj.read_bytes(), names[0])
        self.assertIsNone(singular_plan["error"])
        self.assertIsNotNone(singular)

    def test_duplicate_deadstrip_demands_the_cartridge_body(self):
        """`expect` is what makes discarding a symbol WITH a ROM home sound.

        The vague-linkage case -- types.h's empty ~Vector3, re-emitted by every TU
        that destroys one -- is not compiler-only: the cartridge has a copy and an
        enrolled source owns it. Licensing that here is only safe while this object's
        copy is provably the same body, so a wrong body must refuse rather than warn.
        """
        # An INLINE destructor is what makes the variant vague-linkage, which is
        # exactly the shape this path exists for: every TU that destroys the type
        # emits its own copy.
        obj = self.build("struct V { int v; ~V(){} };\n"
                         "void g(){ V v; v.v = 1; }\n")
        raw = obj.read_bytes()
        real = self._section_bytes(raw, "_ZN1VD1Ev")

        out, plan = OI.derive_deadstrip(raw, ["_ZN1VD1Ev"], {"_ZN1VD1Ev": real})
        self.assertIsNone(plan["error"])
        self.assertIsNotNone(out)

        _out, plan = OI.derive_deadstrip(raw, ["_ZN1VD1Ev"],
                                         {"_ZN1VD1Ev": bytes(len(real))})
        self.assertIn("non-relocated offset", plan["error"])

        # A length mismatch is a different body outright, not a masked word.
        _out, plan = OI.derive_deadstrip(raw, ["_ZN1VD1Ev"],
                                         {"_ZN1VD1Ev": real + bytes(4)})
        self.assertIn("not the cartridge's body", plan["error"])

        _out, plan = OI.derive_deadstrip(raw, ["_ZN1VD1Ev"], {"_ZN1VD2Ev": real})
        self.assertIn("was not requested", plan["error"])

    KEY_FUNCTION_CLASS = ("struct B { virtual ~B(); virtual int f(); }; "
                          "B::~B(){} int B::f(){ return 1; }")

    def test_deadstrip_externalises_a_vtable_the_survivors_reference(self):
        """A promoted TU keeps the vtable the ROM owns instead of losing it.

        An out-of-line destructor is the key function, so mwcc emits `_ZTV` in this
        object -- and the cartridge already has that vtable, at an address no promoted
        source claims.  Discarding the definition while the destructor goes on storing
        the vptr means the reference has to survive as an IMPORT, and the addend has to
        lose mwcc's preamble skip: `_ZTV<C>` here addresses the vtable object, in
        symbols.txt it addresses the slots.
        """
        raw = self.build(self.KEY_FUNCTION_CLASS).read_bytes()
        before = self._reloc_addends(raw, "_ZTV1B")
        self.assertTrue(any(a >= OI.VTABLE_PREAMBLE for a in before), before)

        out, plan = OI.derive_deadstrip(raw, ["_ZTV1B"])
        self.assertIsNone(plan["error"])
        self.assertEqual(plan["externalise"], ["_ZTV1B"])
        self.assertEqual(plan["dead"], [])
        self.assertTrue(plan["rebase"], "the vptr store's section must be rebased")

        after = self._reloc_addends(out, "_ZTV1B")
        self.assertTrue(after, "the reference must survive as an import")
        self.assertTrue(all(a < OI.VTABLE_PREAMBLE for a in after), after)
        self.assertEqual(sorted(a + OI.VTABLE_PREAMBLE for a in after), sorted(before))

    def test_deadstrip_accepts_a_vptr_store_already_in_rom_convention(self):
        """A source that declares the ROM symbol itself needs no preamble correction.

        `extern int _ZTV10dBgActor_c[];` names symbols.txt's slot array, so the store
        relocates with addend 0.  There is nothing to rebase, and refusing it would
        reject every promoted TU that inherits a base's vtable rather than owning one.
        """
        obj = self.build("extern int _ZTVBase[]; struct Q { virtual ~Q(); }; "
                         "Q::~Q(){ *(int**)this = _ZTVBase; } "
                         "int helper(){ return 7; }")
        raw = obj.read_bytes()
        self.assertEqual(set(self._reloc_addends(raw, "_ZTVBase")), {0})

        _out, plan = OI.derive_deadstrip(raw, ["_ZTV1Q"])
        self.assertIsNone(plan["error"])
        self.assertEqual(plan["externalise"], ["_ZTV1Q"])
        self.assertEqual(set(self._reloc_addends(_out, "_ZTVBase")), {0},
                         "an addend-0 reference is left exactly as it was")

    def test_deadstrip_refuses_a_vptr_store_between_the_two_conventions(self):
        """Neither spelling, so neither answer is safe; refuse rather than guess."""
        raw = self.build(self.KEY_FUNCTION_CLASS).read_bytes()
        raw = self._retarget_addend(raw, "_ZTV1B", 4)
        _out, plan = OI.derive_deadstrip(raw, ["_ZTV1B"])
        self.assertIn("unexpected reloc", plan["error"])

    def test_deadstrip_refuses_a_non_rtti_data_import(self):
        """Only the RTTI trio may be imported; anything else is an unsurveyed shape."""
        obj = self.build("int g_table[4] = {1,2,3,4}; int read(){ return g_table[2]; }")
        _out, plan = OI.derive_deadstrip(obj.read_bytes(), ["g_table"])
        self.assertIn("only _ZTV/_ZTI/_ZTS may be imported this way", plan["error"])

    def test_duplicate_body_evidence_is_function_only(self):
        """A data object's words are addends here and addresses in the cartridge."""
        raw = self.build(self.KEY_FUNCTION_CLASS).read_bytes()
        _out, plan = OI.derive_deadstrip(
            raw, ["_ZTV1B"], {"_ZTV1B": self._section_bytes(raw, "_ZTV1B")})
        self.assertIn("function-only", plan["error"])

    def _retarget_addend(self, raw, target, value):
        """A copy of `raw` with every code relocation against `target` given `value`."""
        import io, struct
        from elftools.elf.elffile import ELFFile
        from elftools.elf.relocation import RelocationSection
        out = bytearray(raw)
        elf = ELFFile(io.BytesIO(raw))
        secs = list(elf.iter_sections())
        symtab = elf.get_section_by_name(".symtab")
        endian = "<" if elf.little_endian else ">"
        for sec in secs:
            if not isinstance(sec, RelocationSection) or not sec.is_RELA():
                continue
            if not (secs[sec.header["sh_info"]].header["sh_flags"] & OI.SHF_EXECINSTR):
                continue
            for i, r in enumerate(sec.iter_relocations()):
                if symtab.get_symbol(r["r_info_sym"]).name == target:
                    struct.pack_into(endian + "i", out,
                                     sec.header["sh_offset"] + i * 12 + 8, value)
        return bytes(out)

    def _reloc_addends(self, raw, target):
        """Addends of every relocation against `target` from a surviving code section."""
        import io
        from elftools.elf.elffile import ELFFile
        from elftools.elf.relocation import RelocationSection
        elf = ELFFile(io.BytesIO(raw))
        secs = list(elf.iter_sections())
        symtab = elf.get_section_by_name(".symtab")
        out = []
        for sec in secs:
            if not isinstance(sec, RelocationSection):
                continue
            source = secs[sec.header["sh_info"]]
            if not (source.header["sh_flags"] & OI.SHF_EXECINSTR):
                continue
            if not source.header["sh_size"]:
                continue
            for r in sec.iter_relocations():
                if symtab.get_symbol(r["r_info_sym"]).name == target:
                    out.append(r["r_addend"])
        return out

    def _section_bytes(self, raw, name):
        import io
        from elftools.elf.elffile import ELFFile
        elf = ELFFile(io.BytesIO(raw))
        secs = list(elf.iter_sections())
        sym = next(s for s in elf.get_section_by_name(".symtab").iter_symbols()
                   if s.name == name and isinstance(s["st_shndx"], int))
        return bytes(secs[sym["st_shndx"]].data())

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

    def test_section_partition_keeps_only_declared_data_and_exact_deferred_text(self):
        """A partition is an exact inventory, not generic linker garbage collection."""
        import io
        from elftools.elf.elffile import ELFFile

        obj = self.build('extern "C" int owned_data = 7;\n'
                         'extern "C" int first() { return owned_data; }\n')
        raw = obj.read_bytes()
        source_elf = ELFFile(io.BytesIO(raw))
        first = next(s for s in source_elf.get_section_by_name(".symtab").iter_symbols()
                     if s.name == "first" and s["st_shndx"] != "SHN_UNDEF")
        out, plan = OI.derive_section_partition(
            raw, [".data"], ["owned_data"],
            [{"symbol": "first", "section": ".text", "size": first["st_size"]}])
        self.assertIsNone(plan["error"])
        self.assertEqual(obj.read_bytes(), raw, "partitioning must be pure")
        elf = ELFFile(io.BytesIO(out))
        live = [s.name for s in elf.iter_sections()
                if s.header["sh_type"] in OI.CONTENT and s.header["sh_size"]
                and not any(s.name.startswith(prefix) for prefix in OI.IGNORE)]
        self.assertEqual(live, [".data"])
        symbols = {s.name: s for s in elf.get_section_by_name(".symtab").iter_symbols()}
        self.assertNotEqual(symbols["owned_data"]["st_shndx"], "SHN_UNDEF")
        self.assertEqual(symbols["first"]["st_shndx"], "SHN_UNDEF")

    def test_section_partition_refuses_unexpected_helper_or_global(self):
        """Source drift cannot become green by silently shrinking the owned surface."""
        import io
        from elftools.elf.elffile import ELFFile

        obj = self.build('extern "C" int owned_data = 7;\n'
                         'extern "C" int surprise_data = 9;\n'
                         'extern "C" int first() { return owned_data; }\n'
                         'extern "C" int surprise_helper() { return 3; }\n')
        raw = obj.read_bytes()
        elf = ELFFile(io.BytesIO(raw))
        symtab = elf.get_section_by_name(".symtab")
        first = next(s for s in symtab.iter_symbols() if s.name == "first")
        out, plan = OI.derive_section_partition(
            raw, [".data"], ["owned_data"],
            [{"symbol": "first", "section": ".text", "size": first["st_size"]}])
        self.assertIsNone(out)
        self.assertTrue("unlicensed symbol" in plan["error"]
                        or "foreign content" in plan["error"], plan["error"])

    def test_section_partition_preserves_retained_vtable_relocation_addends(self):
        """Data partitioning must not apply function isolation's _ZTV addend rewrite."""
        import io
        from elftools.elf.elffile import ELFFile
        from elftools.elf.relocation import RelocationSection

        raw = self.build("struct P { virtual ~P(); }; P::~P(){}\n").read_bytes()
        elf = ELFFile(io.BytesIO(raw))
        symtab = elf.get_section_by_name(".symtab")
        symbols = list(symtab.iter_symbols())
        licensed = [s.name for s in symbols if s.name and s["st_info"]["type"] == "STT_OBJECT"
                    and isinstance(s["st_shndx"], int)
                    and elf.get_section(s["st_shndx"]).name == ".data"]
        deferred = [{"symbol": s.name, "section": ".text", "size": s["st_size"]}
                    for s in symbols if s.name and s["st_info"]["type"] == "STT_FUNC"
                    and isinstance(s["st_shndx"], int)
                    and elf.get_section(s["st_shndx"]).name == ".text"
                    and s["st_size"] > 0]

        def abi_addends(blob):
            parsed = ELFFile(io.BytesIO(blob))
            table = parsed.get_section_by_name(".symtab")
            return sorted(r["r_addend"] for sec in parsed.iter_sections()
                          if isinstance(sec, RelocationSection)
                          and parsed.get_section(sec.header["sh_info"]).name == ".data"
                          for r in sec.iter_relocations()
                          if table.get_symbol(r["r_info_sym"]).name.startswith("_ZTVN3abi"))

        before = abi_addends(raw)
        self.assertTrue(before)
        out, plan = OI.derive_section_partition(raw, [".data"], licensed, deferred)
        self.assertIsNone(plan["error"])
        self.assertEqual(abi_addends(out), before)

    def test_rebias_vtable_requires_one_exact_dedicated_global_object(self):
        """Only a whole dedicated _ZTV storage object may move to its public point."""
        import io
        import struct
        from elftools.elf.elffile import ELFFile
        from elftools.elf.relocation import RelocationSection

        whole = self.build("struct P { virtual ~P(); }; P::~P(){}\n").read_bytes()
        whole_elf = ELFFile(io.BytesIO(whole))
        whole_symbols = list(whole_elf.get_section_by_name(".symtab").iter_symbols())
        licensed = [s.name for s in whole_symbols
                    if s.name and s["st_info"]["type"] == "STT_OBJECT"
                    and isinstance(s["st_shndx"], int)
                    and whole_elf.get_section(s["st_shndx"]).name == ".data"]
        deferred = [{"symbol": s.name, "section": ".text", "size": s["st_size"]}
                    for s in whole_symbols
                    if s.name and s["st_info"]["type"] == "STT_FUNC"
                    and isinstance(s["st_shndx"], int)
                    and whole_elf.get_section(s["st_shndx"]).name == ".text"
                    and s["st_size"] > 0]
        raw, partition = OI.derive_section_partition(
            whole, [".data"], licensed, deferred)
        self.assertIsNone(partition["error"])
        elf = ELFFile(io.BytesIO(raw))
        symtab = elf.get_section_by_name(".symtab")
        symbols = list(symtab.iter_symbols())
        index, vtable = next((i, s) for i, s in enumerate(symbols) if s.name == "_ZTV1P")
        section = elf.get_section(vtable["st_shndx"])
        donor_name = "_ZN1PD2Ev"
        donor_index, donor = next((i, s) for i, s in enumerate(symbols)
                                  if s.name == donor_name)
        self.assertEqual(donor["st_shndx"], "SHN_UNDEF")
        policy = {"_ZTV1P": {"bias": 8, "size": vtable["st_size"],
                              "section": section.name,
                              "storageAlias": {"symbol": "data_2000", "size": 8,
                                               "donor": donor_name}}}
        out, report = OI.rebias_object_symbols(raw, policy)
        self.assertIsNone(report["error"])
        rebased = ELFFile(io.BytesIO(out))
        got = next(s for s in rebased.get_section_by_name(".symtab").iter_symbols()
                   if s.name == "_ZTV1P")
        self.assertEqual(got["st_value"], 8)
        self.assertEqual(got["st_size"], vtable["st_size"] - 8)
        alias = next(s for s in rebased.get_section_by_name(".symtab").iter_symbols()
                     if s.name == "data_2000")
        self.assertEqual((alias["st_value"], alias["st_size"], alias["st_shndx"]),
                         (0, 8, got["st_shndx"]))
        self.assertEqual((alias["st_info"]["bind"], alias["st_info"]["type"]),
                         ("STB_GLOBAL", "STT_OBJECT"))
        self.assertFalse(any(s.name == donor_name for s in
                             rebased.get_section_by_name(".symtab").iter_symbols()))
        self.assertEqual(section.data(), rebased.get_section(got["st_shndx"]).data())

        missing_donor = {"_ZTV1P": {**policy["_ZTV1P"],
                                     "storageAlias": {"symbol": "data_2000", "size": 8,
                                                      "donor": "missing"}}}
        refused, why = OI.rebias_object_symbols(raw, missing_donor)
        self.assertIsNone(refused)
        self.assertIn("has 0 symbol-table slots", why["error"])

        long_alias = {"_ZTV1P": {**policy["_ZTV1P"],
                                  "storageAlias": {"symbol": "data_name_that_is_too_long",
                                                   "size": 8, "donor": donor_name}}}
        refused, why = OI.rebias_object_symbols(raw, long_alias)
        self.assertIsNone(refused)
        self.assertIn("does not fit donor", why["error"])

        # A second symbol may point at a longer name beginning immediately before
        # the donor while the donor itself points at that name's suffix.  Checking
        # only offsets inside the donor span misses this overlap.
        string_table = elf.get_section(symtab.header["sh_link"])
        donor_string = string_table.header["sh_offset"] + donor["st_name"]
        suffix_other_index = next(i for i, sym in enumerate(symbols)
                                  if i not in (index, donor_index) and sym.name)
        suffix_endian = "<" if elf.little_endian else ">"
        suffix_shared = bytearray(raw)
        suffix_shared[donor_string - 1] = ord("X")
        struct.pack_into(suffix_endian + "I", suffix_shared,
                         symtab.header["sh_offset"] + suffix_other_index * 16,
                         donor["st_name"] - 1)
        refused, why = OI.rebias_object_symbols(bytes(suffix_shared), policy)
        self.assertIsNone(refused)
        self.assertIn("string-table boundary", why["error"])

        live_donor = {"_ZTV1P": {**policy["_ZTV1P"],
                                  "storageAlias": {"symbol": "data_2000", "size": 8,
                                                   "donor": "_ZTI1P"}}}
        refused, why = OI.rebias_object_symbols(raw, live_donor)
        self.assertIsNone(refused)
        self.assertIn("not an exact deadstripped", why["error"])

        endian = "<" if elf.little_endian else ">"
        entry = symtab.header["sh_offset"] + index * 16
        bad = bytearray(raw)
        struct.pack_into(endian + "I", bad, entry + 4, 4)
        refused, why = OI.rebias_object_symbols(bytes(bad), policy)
        self.assertIsNone(refused)
        self.assertIn("does not exactly cover", why["error"])

        bad = bytearray(raw)
        bad[entry + 12] = 0x01  # STB_LOCAL/STT_OBJECT
        refused, why = OI.rebias_object_symbols(bytes(bad), policy)
        self.assertIsNone(refused)
        self.assertIn("STB_GLOBAL/STT_OBJECT", why["error"])

        refused, why = OI.rebias_object_symbols(
            raw, {"_ZTVMissing": {"bias": 8, "size": 16, "section": ".data"}})
        self.assertIsNone(refused)
        self.assertIn("0 defined symbols", why["error"])

        refused, why = OI.rebias_object_symbols(
            raw, {"_ZTV1P": {"bias": vtable["st_size"],
                              "size": vtable["st_size"], "section": section.name}})
        self.assertIsNone(refused)
        self.assertIn("must be positive", why["error"])

        other_index, _other = next((i, s) for i, s in enumerate(symbols)
                                   if i != index and s.name == "_ZTI1P"
                                   and s["st_shndx"] != "SHN_UNDEF")
        bad = bytearray(raw)
        struct.pack_into(endian + "I", bad,
                         symtab.header["sh_offset"] + other_index * 16,
                         vtable["st_name"])
        refused, why = OI.rebias_object_symbols(bytes(bad), policy)
        self.assertIsNone(refused)
        self.assertIn("2 defined symbols", why["error"])

        bad = bytearray(raw)
        patched = False
        for relsec in elf.iter_sections():
            if not isinstance(relsec, RelocationSection) \
                    or relsec.header["sh_info"] != vtable["st_shndx"]:
                continue
            reloc = next(iter(relsec.iter_relocations()), None)
            if reloc is None:
                continue
            r_info = (index << 8) | reloc["r_info_type"]
            struct.pack_into(endian + "I", bad, relsec.header["sh_offset"] + 4, r_info)
            patched = True
            break
        self.assertTrue(patched)
        refused, why = OI.rebias_object_symbols(bytes(bad), policy)
        self.assertIsNone(refused)
        self.assertIn("smaller than bias", why["error"])

        bad = bytearray(raw)
        patched = False
        for relsec in elf.iter_sections():
            if not isinstance(relsec, RelocationSection) \
                    or relsec.header["sh_info"] != vtable["st_shndx"]:
                continue
            reloc = next(iter(relsec.iter_relocations()), None)
            if reloc is None:
                continue
            r_info = (donor_index << 8) | reloc["r_info_type"]
            struct.pack_into(endian + "I", bad, relsec.header["sh_offset"] + 4, r_info)
            patched = True
            break
        self.assertTrue(patched)
        refused, why = OI.rebias_object_symbols(bytes(bad), policy)
        self.assertIsNone(refused)
        self.assertIn("still referenced", why["error"])

    def test_rebias_vtable_preserves_live_reference_targets(self):
        """Whole-object vptr stores keep their target while _ZTV moves by eight."""
        import io
        from elftools.elf.elffile import ELFFile
        from elftools.elf.relocation import RelocationSection

        raw = self.build("struct P { virtual ~P(); virtual int f(); }; "
                         "P::~P(){} int P::f(){ return 1; }\n").read_bytes()

        def inspect(blob):
            parsed = ELFFile(io.BytesIO(blob))
            table = parsed.get_section_by_name(".symtab")
            symbols = list(table.iter_symbols())
            vtable = next(s for s in symbols if s.name == "_ZTV1P"
                          and s["st_shndx"] != "SHN_UNDEF")
            content = {i: sec.data() for i, sec in enumerate(parsed.iter_sections())
                       if sec.header["sh_type"] in OI.CONTENT and sec.header["sh_size"]}
            references = []
            for sec in parsed.iter_sections():
                if not isinstance(sec, RelocationSection):
                    continue
                source = parsed.get_section(sec.header["sh_info"])
                for reloc in sec.iter_relocations():
                    if table.get_symbol(reloc["r_info_sym"]).name != "_ZTV1P":
                        continue
                    references.append({
                        "section": source.name, "offset": reloc["r_offset"],
                        "type": reloc["r_info_type"], "addend": reloc["r_addend"],
                        "resolved": vtable["st_value"] + reloc["r_addend"],
                    })
            return vtable, content, references

        before_vtable, before_content, before_refs = inspect(raw)
        self.assertTrue(before_refs)
        self.assertTrue(all(row["type"] == OI.R_ARM_ABS32 and row["addend"] >= 8
                            for row in before_refs))
        policy = {"_ZTV1P": {"bias": 8, "size": before_vtable["st_size"],
                              "section": ".data"}}
        out, report = OI.rebias_object_symbols(raw, policy)
        self.assertIsNone(report["error"])
        after_vtable, after_content, after_refs = inspect(out)
        self.assertEqual(after_vtable["st_value"], before_vtable["st_value"] + 8)
        self.assertEqual(after_vtable["st_size"], before_vtable["st_size"] - 8)
        self.assertEqual(after_content, before_content)
        self.assertEqual([(r["section"], r["offset"], r["type"], r["resolved"])
                          for r in after_refs],
                         [(r["section"], r["offset"], r["type"], r["resolved"])
                          for r in before_refs])
        self.assertEqual([r["addend"] for r in after_refs],
                         [r["addend"] - 8 for r in before_refs])
        self.assertEqual(len(report["relocations"]), len(before_refs))

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

    def test_corrects_ctor_only_mi_secondary_vptr(self):
        """A constructor-only TU of an MI class stores its secondary vptr UNDEF.

        The vtable's key function is the destructor, so an MI class's
        constructor-only TU references `_ZTV1M` without defining it -- twice:
        addend 8 for the primary store, and 24 (8 preamble + 0x10 into the
        secondary block) for the secondary. The secondary used to be refused
        on the UNDEF path while no enrolled instance verified the arithmetic;
        ModelAnim's externalised 44 - 8 = +0x24 thunk landing is that
        verification, and dBgCh_Lin's constructor is the first enrolled
        function to walk this exact path."""
        obj = self.build("struct B1 { int p[4]; virtual ~B1(){} };\n"
                         "struct B2 { int q[4]; virtual ~B2(){} };\n"
                         "struct M : B1, B2 { M(); virtual ~M(); };\n"
                         "M::M(){}\n")
        self.assertIsNone(OI.plan(obj.read_bytes(), "_ZN1MC1Ev")["error"])
        before = sorted(set(self._vtable_addends(obj, "_ZN1MC1Ev")))
        self.assertEqual(before, [8, 24])
        OI.isolate(obj, "_ZN1MC1Ev")
        after = sorted(set(self._vtable_addends(obj, "_ZN1MC1Ev")))
        self.assertEqual(after, [0, 16])

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
