"""tools/linkcheck.py: the two len-mismatch shapes underneath the ITCM NO-SYM rows.

WHY THIS EXISTS
---------------
PGATE (#2527) and ITCMRES (#2533) closed the module-resolution gaps that sent every
ITCM symbol, then every consolidated-TU member, to NO-SYM before linkcheck.py even
ran. Nine rows kept reading NO-SYM after both landed, one level deeper, inside
linkcheck.py's own "len-mismatch" verdict -- and this module had no tests of its own
at all until now (only pgate.py's and prepush_linkcheck.py's thin subprocess wrappers
were covered).

Two distinct shapes, two fixes, both in this file:

  * _dmul, _ll_sdiv, _s32_div_f, _u32_div_f -- zero-size EABI alias records in
    config/arm9/itcm/symbols.txt, each a second name for a differently-named,
    correctly-sized primary function at the SAME address (_dmul aliases
    func_01ff8708). A zero-byte target can never be byte-compared against a nonzero
    compiled candidate; linkcheck() now substitutes the sized twin's real length
    (bytegate.alias_target_size) before comparing, and still reports the row under
    the requested (alias) name.
  * func_01ff98f4, func_01ff99a4, _deq, func_01ff9d40, func_01ff9e2c -- nested entry
    points: func_01ff97d8.c is a hand-asm block that packs six ROM functions into
    ONE compiled ELF symbol ("func_01ff97d8", 0xb6c bytes); the other five have no
    symbol of their own in the object at all. RA.winning_object now recognizes when
    a compiled symbol CONTAINS a requested range rather than equalling it, and
    reports the offset; linkcheck() slices the code and rebases+filters the
    relocations into that nested function's own window before comparing.

RealCompileFixtures compiles the REAL committed sources (src/_dmul.c,
src/func_01ff97d8.c) with the pinned mwccarm and skips if it is absent, the same
convention tools/test_objisolate.py and tools/test_rombuild.py use: the whole
subject is what a specific compiler emits, so a hand-built object would test this
file's idea of mwcc rather than mwcc.

The Unit classes below mock winning_object/extract_func/func_relocs_typed/rom_bytes
directly and need no toolchain at all. Real compiles alone cannot prove the REBASE
arithmetic is correct rather than accidentally vacuous -- any sub-slice of an
already-byte-perfect 0xb6c-byte span trivially matches the ROM regardless of where
you cut it, so a real-compile test cannot catch an off-by-one in the offset math or a
relocation that leaked in from outside the nested window. These construct a
synthetic containing symbol where the "right" answer is known by hand and check the
offset math directly, including a deliberately WRONG reloc destination.

    python -m unittest tools.test_linkcheck -v
"""
import pathlib
import sys
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import linkcheck as LC        # noqa: E402
import reloc_audit as RA      # noqa: E402
import reverify_corpus as RV  # noqa: E402
import bytegate as BG         # noqa: E402

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
class RealCompileFixtures(unittest.TestCase):
    """Real compiles of the two committed sources these fixes were written for."""

    def test_alias_zero_size_record_verifies_against_its_sized_twin(self):
        """src/_dmul.c compiles under the name "_dmul"; config records _dmul at
        0x01ff8708 size 0 and func_01ff8708 at the same address size 0x6f0. Asking
        linkcheck for "_dmul" with its own (zero) recorded size used to read NO-SYM
        with reason "len-mismatch" unconditionally -- 0x6f0 compiled bytes can never
        equal a 0-byte target. It must now resolve the sized twin's length itself and
        report VERIFIED, not require the caller to already know to pass 0x6f0."""
        name_index = RA.build_name_index()
        r = LC.linkcheck("_dmul", 0x01ff8708, 0, "itcm", name_index)
        self.assertEqual(r["verdict"], "VERIFIED")

    def test_nested_entry_point_extracted_from_the_containing_object(self):
        """src/func_01ff97d8.c compiles to ONE ELF symbol, "func_01ff97d8", 0xb6c
        bytes -- confirmed directly: the object this source produces defines no
        symbol named func_01ff98f4 at all. That symbol's own config row (0x01ff98f4,
        size 0xb0) sits 0x11c bytes inside func_01ff97d8's compiled span. Asking
        linkcheck for it directly must slice that span rather than reporting NO-SYM
        for a symbol the object never defines under that name."""
        name_index = RA.build_name_index()
        r = LC.linkcheck("func_01ff98f4", 0x01ff98f4, 0xb0, "itcm", name_index)
        self.assertEqual(r["verdict"], "VERIFIED")


class AliasSizeSubstitutionUnit(unittest.TestCase):
    """The zero-size substitution in isolation, without a compiler."""

    def test_a_genuine_zero_size_record_with_no_sized_twin_is_left_alone(self):
        """bytegate.alias_target_size returning None (no sized twin exists at this
        address) must not fabricate a size. winning_object should still be asked
        with size 0, and the pre-existing len-mismatch verdict is unaffected."""
        with mock.patch.object(BG, "alias_target_size", return_value=None) as alt, \
             mock.patch.object(LC.RA, "winning_object",
                               return_value=(None, None, "len-mismatch", 0)) as wo:
            r = LC.linkcheck("_true_orphan", 0x01ffffff, 0, "itcm", {})
        alt.assert_called_once_with("itcm", 0x01ffffff)
        self.assertEqual(wo.call_args.args[2], 0)  # size passed through unchanged
        self.assertEqual(r["verdict"], "NO-SYM")
        self.assertEqual(r["reason"], "len-mismatch")

    def test_a_sized_twin_is_substituted_before_winning_object_is_called(self):
        """The corrected size must reach BOTH winning_object (so its own candidate
        matching compares against the right length) and linkcheck's own target
        recomputation -- not just one of the two, which was the shape of the
        original bug (winning_object could privately succeed while linkcheck's
        separate re-derivation still compared against the stale zero length)."""
        with mock.patch.object(BG, "alias_target_size", return_value=0x6f0), \
             mock.patch.object(LC.RA, "winning_object") as wo, \
             mock.patch.object(LC.M, "extract_func", return_value=(b"\x00" * 0x6f0, set())), \
             mock.patch.object(LC, "func_relocs_typed", return_value=[]), \
             mock.patch.object(RV, "rom_bytes", return_value=b"\x00" * 0x6f0):
            wo.return_value = (b"OBJ", "_dmul", None, 0)
            r = LC.linkcheck("_dmul", 0x01ff8708, 0, "itcm", {})
        self.assertEqual(wo.call_args.args[2], 0x6f0)          # size winning_object saw
        self.assertEqual(r["verdict"], "VERIFIED")


class NestedOffsetRebaseUnit(unittest.TestCase):
    """The slice-and-rebase arithmetic linkcheck() applies when RA.winning_object
    reports a nonzero offset, fully mocked at the winning_object/extract_func/
    func_relocs_typed/rom_bytes boundary. A real compile alone cannot distinguish
    correct rebasing from an accident: any sub-slice of an already-byte-perfect
    0xb6c-byte span trivially matches the ROM wherever you cut it, so a bug that put
    the offset one word off, or leaked a neighbouring function's relocation into this
    one's window, would still read VERIFIED on the real func_01ff97d8.c fixture. This
    builds a container where the right slice and the right reloc set are known by
    construction, and checks both: a consistent relocation verifies, an inconsistent
    one is caught as WRONG rather than silently accepted."""

    # A 0x14-byte containing symbol: 0x10 bytes of unrelated prefix (the nested
    # function starts partway through, as func_01ff98f4 does inside func_01ff97d8),
    # then the 4-byte nested function body itself -- one R_ARM_ABS32 pool slot, zero
    # until the linker fills it. Addresses are chosen well outside every real module
    # image (0x02000000-ish arm9/overlay/itcm space) so is_benign's real-ROM veneer
    # and twin checks -- which read actual module bytes -- reliably see nothing and
    # never accidentally excuse a genuine mismatch.
    CONTAINER_CODE = b"\x11" * 0x10 + b"\x00\x00\x00\x00"
    NESTED_ADDR = 0x02000010
    RELOC_TARGET = 0xEE001000

    def _run(self, target_bytes, extra_relocs=()):
        relocs = [{"off": 0x10, "type": LC.R_ARM_ABS32, "sym": "in_range",
                   "addr": self.RELOC_TARGET, "add": 0}] + list(extra_relocs)
        with mock.patch.object(LC.RA, "winning_object",
                               return_value=(b"OBJ", "func_01ff97d8", None, 0x10)), \
             mock.patch.object(LC.M, "extract_func",
                               return_value=(self.CONTAINER_CODE, set())), \
             mock.patch.object(LC, "func_relocs_typed", return_value=relocs), \
             mock.patch.object(RV, "rom_bytes", return_value=target_bytes), \
             mock.patch.object(LC, "is_benign", return_value=False), \
             mock.patch.object(LC, "is_interwork", return_value=False):
            # is_benign / is_interwork read the real module images to excuse a
            # mismatch (veneers, twins). A mocked-arithmetic test must not touch
            # extracted/ (absent on the CI runner) and must not be excused.
            return LC.linkcheck("func_01ff98f4", self.NESTED_ADDR, 4, "itcm", {})

    def test_slices_code_and_rebases_the_reloc_into_the_nested_window(self):
        """The reloc at container-relative offset 0x10 must land at rebased offset 0
        inside the 4-byte nested window and get linked against the ROM word that
        encodes RELOC_TARGET -- proving both the code slice (0x10:0x14) and the
        reloc rebase (0x10 -> 0) land on the SAME 4 bytes."""
        import struct
        target = struct.pack("<I", self.RELOC_TARGET)
        r = self._run(target)
        self.assertEqual(r["verdict"], "VERIFIED")

    def test_a_wrong_destination_inside_the_nested_window_is_caught(self):
        """Same shape, but the ROM word encodes a DIFFERENT address than the
        candidate's relocation resolves to -- a false match, and rebasing must not
        wildcard it away just because it now sits at offset 0 of a slice."""
        import struct
        wrong_target = struct.pack("<I", 0xEE002000)
        r = self._run(wrong_target)
        self.assertEqual(r["verdict"], "WRONG")

    def test_a_reloc_outside_the_nested_window_is_excluded_not_misapplied(self):
        """A relocation belonging to a NEIGHBOURING nested function (container
        offset 0x4, well before this function's 0x10:0x14 window) must be dropped
        by the rebase filter, not rebased to a negative offset and misapplied to
        this function's own bytes."""
        import struct
        target = struct.pack("<I", self.RELOC_TARGET)
        neighbour = {"off": 0x4, "type": LC.R_ARM_ABS32, "sym": "neighbour",
                     "addr": 0xEE009999, "add": 0}
        r = self._run(target, extra_relocs=[neighbour])
        self.assertEqual(r["verdict"], "VERIFIED")


if __name__ == "__main__":
    unittest.main()
