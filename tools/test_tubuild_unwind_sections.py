"""Fail-closed coverage for the CodeWarrior unwind pair in the TU manifest schema.

WHY THIS MODULE EXISTS AND IS NOT PART OF ``tools/test_tubuild.py``.

The same reason ``tools/test_tubuild_owned_relocs.py`` and
``tools/test_tubuild_vtable_partitions.py`` exist, one boundary over.
``test_tubuild.py`` is pytest-style (bare ``def test_*`` functions and zero
``unittest.TestCase`` classes), so ``python -m unittest tools.test_tubuild``
prints "Ran 0 tests ... OK" and exits 0.  CI
(``.github/workflows/tool-tests.yml``) runs ``python -m unittest``, so the
sibling schema test that lives there, ``test_manifest_section_claims_are_explicit
_and_unambiguous``, has never executed on a runner.  Putting the unwind rows
beside it would have been decoration.  These are dict-driven, need neither
mwccarm nor ``extracted/``, and call only ``tubuild.*``.

WHAT THEY PIN.

A translation unit compiled with exceptions on emits two content sections next to
``.text``: one ``.exception`` frame record per function carrying a try, and one
12-byte ``.exceptix`` index entry naming that function and its frame.  ``arm9``
holds 636 bytes of ``.exceptix`` in retail, so these are ordinary retail ranges a
reconstructed TU has to be able to claim.

Until this module the schema had no name for either.  ``__destroy_arr``
(0x0207328c, .text 116 + .exception 20 + .exceptix 12) was refused three layers
deep: ``manifest_section_claims`` rejected both names outright, and with the name
list widened alone ``prepare_owned_nontext_section_order`` then called
``@ET@__destroy_arr`` and ``@EX@__destroy_arr`` unlicensed, because
``_SECTION_SYMBOL_FIELDS`` had no manifest field to declare them in.  Both halves
have to move together or the entry is refused with a worse message than before,
so both are asserted here.

The third copy of the name list is inline in ``rombuild.intact_tu_policies``
(tubuild imports rombuild, so the dependency cannot run the other way); that one
is pinned by ``test_rombuild.RomBuildEnrollment.test_intact_policy_admits
_exact_exception_and_exceptix_claims``.
"""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tubuild  # noqa: E402


class UnwindSectionClaims(unittest.TestCase):
    def test_manifest_claims_admit_exception_and_exceptix_ranges(self):
        """__destroy_arr's three real ranges, taken verbatim from the cartridge."""
        claims, reasons = tubuild.manifest_section_claims({"sections": [
            {"name": ".text", "start": "0x0207328c", "end": "0x02073300"},
            {"name": ".exception", "start": "0x0207372c", "end": "0x02073740"},
            {"name": ".exceptix", "start": "0x02073988", "end": "0x02073994"},
        ]})
        self.assertEqual(reasons, [])
        self.assertEqual(claims, [
            {"name": ".text", "module_section": ".text",
             "start": 0x0207328c, "end": 0x02073300},
            {"name": ".exception", "module_section": ".exception",
             "start": 0x0207372c, "end": 0x02073740},
            {"name": ".exceptix", "module_section": ".exceptix",
             "start": 0x02073988, "end": 0x02073994},
        ])

    def test_widening_the_names_did_not_widen_anything_else(self):
        """One contiguous contribution per section name still holds, and an
        invented unwind-looking name is still refused."""
        _claims, reasons = tubuild.manifest_section_claims({"sections": [
            {"name": ".text", "start": "0x1000", "end": "0x1010"},
            {"name": ".exception", "start": "0x2000", "end": "0x2010"},
            {"name": ".exception", "start": "0x2010", "end": "0x2020"},
            {"name": ".exceptable", "start": "0x3000", "end": "0x3010"},
        ]})
        self.assertTrue(any("duplicate .exception claim" in r for r in reasons),
                        reasons)
        self.assertTrue(any("unsupported name '.exceptable'" in r for r in reasons),
                        reasons)

    def test_unwind_symbols_have_a_manifest_field_to_be_licensed_in(self):
        """Without these two fields the retained sections define unlicensed
        symbols and prepare_owned_nontext_section_order refuses the whole entry."""
        self.assertEqual(tubuild._SECTION_SYMBOL_FIELDS[".exception"], "exception")
        self.assertEqual(tubuild._SECTION_SYMBOL_FIELDS[".exceptix"], "exceptix")
        rows = tubuild.manifest_owned_symbol_rows({
            "exception": [{"symbol": "@ET@__destroy_arr", "address": "0x0207372c"}],
            "exceptix": [{"symbol": "@EX@__destroy_arr", "address": "0x02073988"}],
        })
        self.assertEqual(rows, [
            (".exception", {"symbol": "@ET@__destroy_arr", "address": "0x0207372c"}),
            (".exceptix", {"symbol": "@EX@__destroy_arr", "address": "0x02073988"}),
        ])

    def test_a_section_with_no_owned_field_is_still_unlicensable(self):
        """The map is the licence, not the name list: a name the claims schema
        takes but the field map does not know licenses no symbols at all."""
        self.assertNotIn(".text", tubuild._SECTION_SYMBOL_FIELDS)
        self.assertEqual(tubuild.manifest_owned_symbol_rows(
            {"text": [{"symbol": "__destroy_arr", "address": "0x0207328c"}]}), [])


if __name__ == "__main__":
    unittest.main()
