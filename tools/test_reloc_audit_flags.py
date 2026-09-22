"""tools/reloc_audit.py: the audit compiles with the ROM BUILD's flags.

WHY THIS EXISTS
---------------
`winning_object` used to compile every candidate with the MATCH gate's flag set
(`match.DEFAULT_FLAGS` / `swarm.CPP_FLAGS`), while `rombuild.py` compiles with
`-Cpp_exceptions off`. The gate that predicts the link was therefore measuring a
build nobody ships, and it moved verdicts in BOTH directions -- measured 2026-09-13
on four open PRs:

  * Three false reds. `daObjKm1_Dorifu_c_classInit` (ov043 0x0211176c),
    `daObjKm3_Dorifu_c_classInit` (ov047 0x0211164c) and `daObjRc_Dorifu_c_classInit`
    (ov036 0x021120c8) each reproduce the cartridge exactly under the build's flags
    and miss it by the same five words under exceptions-on, where mwccarm schedules
    the second `__cxa_vec_ctor`'s stacked argument differently. All three read
    NO-REPRO on source the ROM build accepts.

  * One false green, the worse half. Exceptions-on makes mwccarm emit
    `.exceptix`/`.rela.exceptix` next to the function. `objisolate.isolate` returns
    early when its plan has nothing to drop, so on a single-function object those two
    extra sections are the only reason isolation runs at all -- and running it also
    applies the vtable-preamble correction that rewrites a `_ZTV<C>` reference's
    addend 8 down to 0. Compiled the way the build compiles it there is nothing to
    drop, isolation is a no-op, and the +8 reaches the linker. ov100 0x02147328
    link-checked VERIFIED while the ROM build stored 0x02148584 for a slot array at
    0x0214857c and reported the overlay MISMATCHING.

`config/tu_manifest.d/**` has recorded the build's flag string under
`verification.flags` all along, so the flags could have been read from there instead.
They are not, and `ManifestAgreement` below is why that choice is safe rather than
arbitrary: all 177 entries record exactly the two strings this function hands out, so
the two rules never disagree where both apply -- and only one of them reaches a file
with no manifest entry. The manifest's entries name 208 source paths, 146 of which
still exist; src/ holds 8,930. The file that produced the false green above is not one
of the 146.

    python -m unittest tools.test_reloc_audit_flags -v
"""
import glob
import json
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import build_pin as BP     # noqa: E402
import match as M          # noqa: E402
import reloc_audit as RA   # noqa: E402
import rombuild as RB      # noqa: E402
import swarm as S          # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent
MANIFEST = REPO / "config" / "tu_manifest.d"


class BuildFlagAttempts(unittest.TestCase):
    CPP = "//cpp\nint f(void) { return 0; }\n"
    C = "int f(void) { return 0; }\n"

    def test_a_marked_source_gets_the_builds_cpp_flags_only(self):
        self.assertEqual(RA.build_flag_attempts(self.CPP),
                         [(RB.CFLAGS.replace("-lang c99", "-lang c++"), ".cpp")])

    def test_an_unmarked_source_is_tried_c_then_cpp(self):
        """Committed sources do not all carry the marker, so the two-attempt shape
        stays -- what changes is that BOTH attempts now carry the build's flags."""
        attempts = RA.build_flag_attempts(self.C)
        self.assertEqual([suffix for _, suffix in attempts], [".c", ".cpp"])
        self.assertEqual(attempts[0][0], RB.CFLAGS)
        self.assertEqual(attempts[1][0], RB.CFLAGS.replace("-lang c99", "-lang c++"))

    def test_every_attempt_disables_cpp_exceptions(self):
        """The one flag the match gate's set omits and the build carries. Spelled out
        as its own assertion because it is the whole defect: an exceptions-on compile
        emits unwind sections the link never sees and schedules code the ROM does not
        contain."""
        for src in (self.C, self.CPP):
            for flags, _ in RA.build_flag_attempts(src):
                self.assertIn("-Cpp_exceptions off", flags)

    def test_the_match_gates_flag_sets_are_not_used(self):
        """Named directly, because this is what regressed: the match gate sweeps many
        compilers and asks whether a source is REAL. The link asks whether it will
        reproduce, and only the build's own flags answer that."""
        for src in (self.C, self.CPP):
            offered = [flags for flags, _ in RA.build_flag_attempts(src)]
            self.assertNotIn(M.DEFAULT_FLAGS, offered)
            self.assertNotIn(S.CPP_FLAGS, offered)

    def test_the_marker_decides_the_language_not_the_extension(self):
        """`build_pin.flags_for`'s rule, inherited here: `rombuild.compile_one` tests
        the `//cpp` first line and never the filename, and 53 committed `.c` files are
        compiled as C++ because of it."""
        self.assertEqual(RA.build_flag_attempts(self.CPP)[0][0],
                         BP.flags_for("anything.c", self.CPP))


class ManifestAgreement(unittest.TestCase):
    """Every recorded `verification.flags` in the TU manifest is one of the flag
    strings this function hands out.

    This is the measured argument for taking the flags from the build rather than
    from the manifest: where both rules apply they agree, entry for entry, so
    switching to the build's flags cannot contradict a promoted TU's recorded
    verification -- and unlike the manifest, the build's flags also answer for the
    thousands of sources no manifest entry names.

    Fails closed. An unreadable entry, a missing `verification.flags`, or an empty
    manifest directory is a failure here, not a silent pass on zero rows."""

    def _entries(self):
        out = []
        for path in sorted(glob.glob(str(MANIFEST / "**" / "*.json"), recursive=True)):
            if pathlib.PurePath(path).name == "_meta.json":
                continue
            with open(path, encoding="utf-8") as fh:
                out.append((path, json.load(fh)))
        return out

    def test_the_manifest_is_readable_and_not_empty(self):
        self.assertTrue(self._entries(), f"no manifest entries under {MANIFEST}")

    def test_every_recorded_flag_string_is_one_this_function_offers(self):
        offered = {flags for src in ("//cpp\n", "\n")
                   for flags, _ in RA.build_flag_attempts(src)}
        for path, entry in self._entries():
            verification = entry.get("verification")
            self.assertIsInstance(verification, dict,
                                  f"{path}: no verification block")
            flags = verification.get("flags")
            self.assertIsInstance(flags, str, f"{path}: no verification.flags")
            self.assertIn(flags, offered, f"{path}: verification.flags is not a flag "
                                          f"set the audit compiles with")


if __name__ == "__main__":
    unittest.main()
