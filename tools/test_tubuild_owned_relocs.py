"""Fail-closed coverage for ``tubuild.verify_owned_sections``' relocation verdicts.

WHY THIS MODULE EXISTS AND IS NOT PART OF ``tools/test_tubuild.py``.

``test_tubuild.py`` is pytest-style: 46 bare ``def test_*`` functions and zero
``unittest.TestCase`` classes, so ``python -m unittest tools.test_tubuild`` prints
"Ran 0 tests ... OK" and exits 0.  CI (``.github/workflows/tool-tests.yml``) runs
``python -m unittest``, so nothing in that file has ever executed on a runner.
Worse, fifteen of its tests open with ``if not _toolchain(): return`` -- a bare
return, not a skip -- and every one of the existing owned-relocation assertions
sits behind one of those guards, because its fixtures compile a real object with
mwccarm.  Those tests are real, but they only run on a box that has the pinned
compiler, and a reviewer reading a green log cannot tell the difference between
"asserted" and "returned early".

The one seam that needs the compiler is ``section_contribution``, which is pure
ELF parsing: it turns an object's repeated input sections into
``{"bytes", "relocs", "symbols", "inputSections"}``.  Everything the verdict
ladder below decides is computed from that dict plus the manifest, the config
relocation map and the symbol name index -- all of which the caller injects.  So
these tests stub ``section_contribution`` with a layout of exactly the documented
shape and drive the real ``verify_owned_sections`` over it.  No mwccarm, no
``extracted/``, no ``config/``, no ``build/``: they assert on a bare runner.

WHAT THEY PIN.  PR #2059 added the ``normalized_undefined_vtables`` arm::

    bias = retained_vtable_biases.get(expected["symbol"])
    if bias is None and normalized_undefined_vtables \\
            and expected["symbol"].startswith("_ZTV") \\
            and expected_addend:
        bias = OI.VTABLE_PREAMBLE
    if bias is not None:
        expected_addend = (expected_addend - bias
                           if expected_addend >= bias else None)

``objisolate.rebias_object_symbols(..., normalize_undefined=True)`` rewrites an
undefined ``_ZTV`` import's addend from ``public + N`` to ``public + N - 8``,
because ``symbols.txt`` already names the slot array past the two-word ABI
preamble.  This code undoes that on the manifest side so the two are comparable.

The ``else None`` is the fail-closed half: a manifest addend BELOW the preamble
cannot have come from that rewrite, so there is no honest emitted addend to
compare against and the relocation must be rejected.  Nothing downstream re-checks
it -- ``None`` reaches ``rel["addend"] != expected_addend`` and must land on
``WRONG-ADDEND``.  A permissive value there (``0``, or the unbiased addend) would
let an object whose emitted addend disagrees with the cartridge verify clean, and
the existing suite does not notice: its wrong-addend case uses ``8 + 4 = 12``,
which is above the preamble and takes the arithmetic arm, never the ``else``.

The two ``WRONG-DEST`` clauses are pinned for the same reason: the candidate must
agree with the manifest AND the manifest must agree with ``config/**/relocs.txt``.
Each clause is asserted separately, because dropping either one alone still leaves
a check that looks present.
"""

import pathlib
import sys
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import tubuild as TB  # noqa: E402

MODULE = "ov999"
DATA_START = 0x02000000
DATA_END = 0x02000010
RELOC_SOURCE = DATA_START
OWNED_SYMBOL = "_ZTI5Thing"
IMPORTED_VTABLE = "_ZTVN10__cxxabiv117__class_type_infoE"
IMPORT_PUBLIC = 0x00006100
R_ARM_ABS32 = 2
CLAIMS = [{"name": ".data", "start": DATA_START, "end": DATA_END}]


def _layout(emitted_addend):
    """One ``.data`` contribution in the exact shape ``section_contribution`` returns.

    Sixteen bytes, one owned STT_OBJECT covering all of them, and one R_ARM_ABS32
    relocation at offset 0 against an UNDEFINED ``_ZTV`` import -- the shape
    ``rebias_object_symbols(normalize_undefined=True)`` produces.
    """
    return {
        "bytes": bytes(range(16)),
        "relocs": [{"offset": 0, "type": R_ARM_ABS32, "symbol": IMPORTED_VTABLE,
                    "addend": emitted_addend, "targetSection": "SHN_UNDEF"}],
        "symbols": {OWNED_SYMBOL: {"address": DATA_START, "size": 0x10,
                                   "bind": "STB_GLOBAL", "type": "STT_OBJECT",
                                   "sectionIndex": 1}},
        "inputSections": [1],
    }


def _entry(manifest_addend, target_address):
    return {
        "module": MODULE,
        "functions": [],
        "data": [{"symbol": OWNED_SYMBOL, "address": f"0x{DATA_START:08x}",
                  "size": "0x10"}],
        "relocations": [{"section": ".data", "source": f"0x{RELOC_SOURCE:08x}",
                         "type": "R_ARM_ABS32", "kind": "load",
                         "symbol": IMPORTED_VTABLE, "addend": manifest_addend,
                         "target_module": "arm9",
                         "target_address": f"0x{target_address:08x}"}],
    }


class NormalizedUndefinedVtableAddend(unittest.TestCase):
    """The ``normalized_undefined_vtables`` addend arm, both halves."""

    def verify(self, emitted_addend, manifest_addend, manifest_target,
               configured_target=None, import_address=IMPORT_PUBLIC):
        entry = _entry(manifest_addend, manifest_target)
        cfg = {MODULE: {RELOC_SOURCE: (
            "load",
            manifest_target if configured_target is None else configured_target,
            "arm9")}}
        layout = _layout(emitted_addend)
        with mock.patch.object(TB, "section_contribution",
                               lambda obj, name, start: (layout, None)):
            return TB.verify_owned_sections(
                b"", entry, CLAIMS,
                name_index={IMPORTED_VTABLE: ("arm9", import_address)},
                config_relocs=cfg, sym_index={},
                target_reader=lambda mod, start, size: layout["bytes"][:size],
                symbol_homes={}, bss_boundaries=set(),
                public_address_points=True, normalized_undefined_vtables=True)

    def row(self, result):
        return next(r for r in result["relocations"]
                    if int(r["source"], 0) == RELOC_SOURCE)

    def test_normalized_import_above_the_preamble_verifies(self):
        """Control: manifest 16 against emitted 8 is the arithmetic arm, and passes.

        Without this the fail-closed tests below could be passing for any reason
        at all -- a fixture that can never verify proves nothing about the branch.
        """
        result = self.verify(emitted_addend=8, manifest_addend=16,
                             manifest_target=IMPORT_PUBLIC + 8)
        self.assertTrue(result["ok"], result["errors"])
        row = self.row(result)
        self.assertEqual(row["verdict"], "OK")
        self.assertEqual(row["expectedAddend"], 16)
        self.assertEqual(row["expectedEmittedAddend"], 8)
        self.assertEqual(row["candidate"], f"0x{IMPORT_PUBLIC + 8:08x}")

    def test_manifest_addend_below_the_preamble_is_refused(self):
        """A manifest addend under 8 has no honest normalized form: reject it.

        The emitted addend here is 0 and every other fact -- destination, module,
        kind, type, symbol -- agrees, so ``else 0`` in place of ``else None``
        would call this relocation OK.  It must be WRONG-ADDEND.
        """
        result = self.verify(emitted_addend=0, manifest_addend=4,
                             manifest_target=IMPORT_PUBLIC)
        row = self.row(result)
        self.assertEqual(row["verdict"], "WRONG-ADDEND")
        self.assertIsNone(row["expectedEmittedAddend"])
        self.assertEqual(row["expectedAddend"], 4)
        self.assertFalse(result["ok"])
        self.assertTrue(any("WRONG-ADDEND" in reason for reason in result["errors"]),
                        result["errors"])

    def test_sub_preamble_addend_is_not_silently_left_unbiased(self):
        """The other permissive rewrite: passing the raw addend through.

        Emitted addend 4 matches the manifest's 4 exactly, and the destination is
        set so that everything downstream lines up.  ``else expected_addend``
        would make this OK; the fail-closed arm must still refuse it.
        """
        result = self.verify(emitted_addend=4, manifest_addend=4,
                             manifest_target=IMPORT_PUBLIC + 4)
        row = self.row(result)
        self.assertEqual(row["verdict"], "WRONG-ADDEND")
        self.assertIsNone(row["expectedEmittedAddend"])
        self.assertFalse(result["ok"])

    def test_addend_exactly_at_the_preamble_normalizes_to_zero(self):
        """The boundary itself takes the arithmetic arm, not the ``else``.

        ``expected_addend >= bias`` is inclusive: manifest 8 is the ordinary
        address-point reference and normalizes to 0.  A ``>`` there would push
        the single most common case into the refusal arm.
        """
        result = self.verify(emitted_addend=0, manifest_addend=8,
                             manifest_target=IMPORT_PUBLIC)
        self.assertTrue(result["ok"], result["errors"])
        row = self.row(result)
        self.assertEqual(row["verdict"], "OK")
        self.assertEqual(row["expectedEmittedAddend"], 0)


class OwnedRelocationDestination(unittest.TestCase):
    """Both halves of the ``WRONG-DEST`` conjunction, asserted separately."""

    verify = NormalizedUndefinedVtableAddend.verify
    row = NormalizedUndefinedVtableAddend.row

    def test_candidate_destination_must_match_the_manifest(self):
        """Clause one: where the relocation actually lands.

        The import resolves four bytes off, so the candidate is public+0xc while
        the manifest and config both say public+8.  Dropping
        ``cand_addr != expected["target_address"]`` makes this OK.
        """
        result = self.verify(emitted_addend=8, manifest_addend=16,
                             manifest_target=IMPORT_PUBLIC + 8,
                             import_address=IMPORT_PUBLIC + 4)
        row = self.row(result)
        self.assertEqual(row["verdict"], "WRONG-DEST")
        self.assertEqual(row["candidate"], f"0x{IMPORT_PUBLIC + 0xc:08x}")
        self.assertEqual(row["configured"], f"0x{IMPORT_PUBLIC + 8:08x}")
        self.assertFalse(result["ok"])

    def test_manifest_destination_must_match_the_configured_relocation(self):
        """Clause two: the manifest may not disagree with config/**/relocs.txt.

        Candidate and manifest agree at public+8; the configured destination is
        elsewhere.  Dropping ``expected["target_address"] != cfg[1]`` makes this
        OK -- and a manifest that can name its own destination is not a check.
        """
        result = self.verify(emitted_addend=8, manifest_addend=16,
                             manifest_target=IMPORT_PUBLIC + 8,
                             configured_target=IMPORT_PUBLIC + 0x100)
        row = self.row(result)
        self.assertEqual(row["verdict"], "WRONG-DEST")
        self.assertEqual(row["candidate"], f"0x{IMPORT_PUBLIC + 8:08x}")
        self.assertEqual(row["configured"], f"0x{IMPORT_PUBLIC + 0x100:08x}")
        self.assertFalse(result["ok"])


if __name__ == "__main__":
    unittest.main()
