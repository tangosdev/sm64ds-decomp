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




# Initialized linker boundaries share the same owned-byte verdict path.
import initialized_boundaries as P
import tempfile


class TerminalTests(unittest.TestCase):
    def setUp(self):
        self.claim = dict(
            name=".data.kuriMeta0",
            start=0x2122E30,
            end=0x2122E4C,
            terminal_linker_fill=dict(alignment=4, value=0),
        )
        self.module = dict(name=".data.kuriMeta0", start=0x2122E30, end=0x2122E4C)
        self.kw = dict(
            configured_symbols=[
                dict(address=0x2122E30, size=12),
                dict(address=0x2122E3C, size=15),
            ],
            relocations=[dict(site=0x2122E34, size=4, target=0x2122E3C)],
            retail_fill=b"\0",
        )

    def license(self, **kw):
        return P.license_terminal_fill(
            self.claim, self.module, kw.pop("emitted_end", 0x2122E4B), **(self.kw | kw)
        )

    def test_native27_linked28(self):
        row = self.license()
        self.assertEqual(
            (row["emittedBytes"], row["linkedBytes"], row["linkerFillBytes"]),
            (27, 28, 1),
        )

    def test_baseline_binds_boundary_and_alias_helpers(self):
        names = {path.name for path in TB.BASELINE_CONTROL_TOOLS}
        self.assertTrue(
            {"initialized_boundaries.py", "lcf_interior.py", "linker_alignment.py"}
            <= names
        )

    def test_absent_policy(self):
        del self.claim["terminal_linker_fill"]
        with self.assertRaises(ValueError):
            self.license()

    def test_policy_shape(self):
        for policy in (
            True,
            {},
            dict(alignment=8, value=0),
            dict(alignment=4, value=1),
            dict(alignment=4, value=False),
            dict(alignment=4, value=0, extra=0),
        ):
            with self.subTest(policy=policy), self.assertRaises(ValueError):
                self.claim["terminal_linker_fill"] = policy
                self.license()

    def test_bss_text_wrong_route(self):
        for name in (".bss", ".text", ".data;INJECT"):
            with self.subTest(name=name), self.assertRaises(ValueError):
                self.claim["name"] = name
                self.license()

    def test_partial_module_owner(self):
        # A claim that starts before its section, or reaches the section end
        # without owning the section start, is neither placement.
        for key, delta in (("start", 4), ("end", -4), ("start", -4)):
            with self.subTest(key=key, delta=delta), self.assertRaises(ValueError):
                self.module = dict(
                    name=".data.kuriMeta0", start=0x2122E30, end=0x2122E4C
                )
                self.module[key] += delta
                self.license()

    def test_section_end_placement(self):
        self.assertEqual(self.license()["placement"], "section-end")

    def test_interior_placement(self):
        # Ends inside a larger section: the fill is the ALIGNALL(4) padding before
        # the next input, which link_plan must still place at the claim end.
        for start in (0x2122E30, 0x2122E20):
            with self.subTest(start=start):
                self.module = dict(name=".data.kuriMeta0", start=start, end=0x2122F00)
                row = self.license()
                self.assertEqual(
                    (row["placement"], row["linkerFillBytes"]), ("interior", 1)
                )

    def test_interior_keeps_every_fill_refusal(self):
        self.module = dict(name=".data.kuriMeta0", start=0x2122E20, end=0x2122F00)
        refusals = (
            dict(retail_fill=b"\xff"),
            dict(retail_fill=b"\0\0"),
            dict(configured_symbols=[dict(address=0x2122E4B, size=0)]),
            dict(configured_symbols=[dict(address=0x2122E3C, size=16)]),
            dict(configured_symbols=[
                dict(address=0x2122E3C, size=0, unknown_initialized_extent=True),
                dict(address=0x2122E50, size=4)]),
            dict(relocations=[dict(site=0x2122E48, size=4, target=0)]),
            dict(relocations=[dict(site=0, size=4, target=0x2122E4B)]),
        )
        for kw in refusals:
            with self.subTest(kw=kw), self.assertRaises(ValueError):
                self.license(**kw)

    def test_interior_successor_rows_at_claim_end_are_not_fill(self):
        self.module = dict(name=".data.kuriMeta0", start=0x2122E20, end=0x2122F00)
        row = self.license(
            configured_symbols=self.kw["configured_symbols"]
            + [dict(address=0x2122E4C, size=4)],
            relocations=[dict(site=0x2122E4C, size=4, target=0x2122E4C)],
        )
        self.assertEqual(row["placement"], "interior")

    def test_interior_section_name_must_match(self):
        self.module = dict(name=".data", start=0x2122E20, end=0x2122F00)
        with self.assertRaises(ValueError):
            self.license()

    def test_extent(self):
        for end in (0x2122E4C, 0x2122E48, 0x2122E4D):
            with self.subTest(end=end), self.assertRaises(ValueError):
                self.license(emitted_end=end)

    def test_nonzero_or_wrong_retail_size(self):
        for body in (b"\xff", b"", b"\0\0"):
            with self.subTest(body=body), self.assertRaises(ValueError):
                self.license(retail_fill=body)

    def test_configured_alias_in_fill(self):
        with self.assertRaises(ValueError):
            self.license(configured_symbols=[dict(address=0x2122E4B, size=0)])

    def test_configured_object_crossing_fill(self):
        with self.assertRaises(ValueError):
            self.license(configured_symbols=[dict(address=0x2122E3C, size=16)])

    def test_unsized_final_initialized_object_cannot_be_zero_storage(self):
        rows = [dict(address=0x2122E30, size=0, unknown_initialized_extent=True),
                dict(address=0x2122E3C, size=0, unknown_initialized_extent=True)]
        with self.assertRaisesRegex(ValueError, "configured symbol occupies"):
            self.license(configured_symbols=rows)

    def test_unsized_rtti_bounded_by_explicit_name_is_valid(self):
        rows = [dict(address=0x2122E30, size=0, unknown_initialized_extent=True),
                dict(address=0x2122E3C, size=15)]
        self.assertEqual(self.license(configured_symbols=rows)["linkerFillBytes"], 1)

    def test_census_distinguishes_unknown_from_explicit_zero(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            (root / "symbols.txt").write_text(
                "unsized kind:data(any) addr:0x02000000\n"
                "zero kind:data(byte[0]) addr:0x02000004\n"
                "fixed kind:data(byte[15]) addr:0x02000008\n")
            _, rows, _ = P.config_population(root)
            self.assertEqual([row["unknown_initialized_extent"] for row in rows],
                             [True, False, False])

    def test_relocation_in_or_to_fill(self):
        for row in (
            dict(site=0x2122E48, size=4, target=0),
            dict(site=0, size=4, target=0x2122E4B),
        ):
            with self.subTest(row=row), self.assertRaises(ValueError):
                self.license(relocations=[row])

    def test_allthree_lengths(self):
        for size in (1, 2, 3):
            row = self.license(
                emitted_end=self.module["end"] - size,
                retail_fill=bytes(size),
                configured_symbols=[],
                relocations=[],
            )
            self.assertEqual(row["linkerFillBytes"], size)

    def test_owned_verifier_keeps_emitted_and_linked_sizes_distinct(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            module = root / "overlays/ov999"
            module.mkdir(parents=True)
            (module / "symbols.txt").write_text(
                "head kind:data(any) addr:0x02122e30\n"
                "name kind:data(byte[15]) addr:0x02122e3c\n"
            )
            (module / "relocs.txt").write_text("")
            (module / "delinks.txt").write_text(
                "    .data.kuriMeta0 start:0x02122e30 end:0x02122e4c kind:data align:4\n"
            )
            entry = dict(
                module="ov999",
                functions=[],
                data=[
                    dict(symbol="head", address=0x2122E30, size=12),
                    dict(symbol="name", address=0x2122E3C, size=15),
                ],
            )
            entry["sections"] = [
                dict(name=".text", start=0x1000, end=0x1004),
                dict(self.claim, input_section=".data"),
            ]
            claims, errors = TB.manifest_section_claims(entry)
            self.assertEqual(errors, [])
            layout = dict(
                bytes=bytes(27),
                relocs=[],
                inputSections=[1, 2],
                symbols={
                    name: dict(
                        address=address, size=size, bind="STB_GLOBAL", type="STT_OBJECT"
                    )
                    for name, address, size in [
                        ("head", 0x2122E30, 12),
                        ("name", 0x2122E3C, 15),
                    ]
                },
            )
            with mock.patch.object(
                TB, "section_contribution", return_value=(layout, None)
            ):
                result = TB.verify_owned_sections(
                    b"",
                    entry,
                    claims,
                    name_index={},
                    config_relocs={},
                    sym_index={},
                    symbol_homes={},
                    bss_boundaries=set(),
                    boundary_config_root=root,
                    target_reader=lambda module, start, size: bytes(size),
                )
            self.assertTrue(result["ok"], result["errors"])
            self.assertEqual(
                (result["rows"][0]["emittedBytes"], result["rows"][0]["linkedBytes"]),
                (27, 28),
            )
            self.assertEqual(len(layout["bytes"]), 27)


class LcfTests(unittest.TestCase):
    def text(self, body=None):
        body = body or (
            ". = ALIGN(4); OV074_DATA.KURIMETA0_START = .;\n"
            " d_a_kuri_king.o(.data.kuriMeta0)\n"
            " . = ALIGN(4); OV074_DATA.KURIMETA0_END = .;"
        )
        return (
            "MEMORY {\n OV074 : ORIGIN = 0x2122e30 > ov074.bin\n}\nSECTIONS {\n.ov074 : {\n ALIGNALL(4);\n"
            + body
            + "\n} > OV074\n}\n"
        )

    def verify(self, text):
        with tempfile.TemporaryDirectory() as d:
            p = pathlib.Path(d) / "test.lcf"
            p.write_text(text)
            return P.verify_terminal_lcf(
                p, module="OV074", section=".data.kuriMeta0", provider="d_a_kuri_king.o"
            )

    def test_actual_generated_shape(self):
        self.assertEqual(self.verify(self.text())["fill"], 0)

    def test_wrong_align(self):
        with self.assertRaises(ValueError):
            self.verify(self.text().replace("ALIGN(4)", "ALIGN(8)"))

    def test_missing_terminal_align(self):
        with self.assertRaises(ValueError):
            self.verify(
                self.text().replace(
                    ". = ALIGN(4); OV074_DATA.KURIMETA0_END", "OV074_DATA.KURIMETA0_END"
                )
            )

    def test_intervening_provider(self):
        with self.assertRaises(ValueError):
            self.verify(
                self.text().replace(
                    " . = ALIGN(4); OV074_DATA.KURIMETA0_END",
                    " gap.o(.data.kuriMeta0)\n . = ALIGN(4); OV074_DATA.KURIMETA0_END",
                )
            )

    def test_nonzero_fill(self):
        with self.assertRaises(ValueError):
            self.verify(self.text().replace("ALIGNALL(4);", "ALIGNALL(4); FILL(255);"))

    def test_output_fill_suffix(self):
        with self.assertRaises(ValueError):
            self.verify(self.text().replace("} > OV074", "} > OV074 = 255"))

    def test_wrong_output(self):
        with self.assertRaises(ValueError):
            self.verify(self.text().replace("OV074", "OV071"))

    def test_duplicate_provider(self):
        with self.assertRaises(ValueError):
            self.verify(
                self.text().replace(
                    "ALIGNALL(4);", "ALIGNALL(4); d_a_kuri_king.o(.data.kuriMeta0)"
                )
            )

    def test_other_provider_outside_markers(self):
        with self.assertRaises(ValueError):
            self.verify(
                self.text().replace(
                    "ALIGNALL(4);", "ALIGNALL(4); other.o(.data.kuriMeta0)"
                )
            )

    def test_wildcard_outside_markers(self):
        with self.assertRaises(ValueError):
            self.verify(
                self.text().replace("ALIGNALL(4);", "ALIGNALL(4); *(.data.kuriMeta0)")
            )

    def test_section_patterns_and_lists_refuse_outside_markers(self):
        for selector in (
            "*(.data*)",
            "other.o(.data*)",
            "other.o(.data?)",
            "other.o(.data[.]kuriMeta0)",
            "other.o(.text .data.kuriMeta0)",
            "other.o(.text,.data.kuriMeta0)",
            "KEEP(other.o(.data.kuriMeta0))",
        ):
            with self.subTest(selector=selector), self.assertRaises(ValueError):
                self.verify(
                    self.text().replace("ALIGNALL(4);", "ALIGNALL(4); " + selector)
                )

    INTERIOR = (
        ". = ALIGN(4); OV074_DATA.KURIMETA0_START = .;\n"
        " head.o(.data.kuriMeta0)\n"
        " d_a_kuri_king.o(.data.kuriMeta0)\n"
        " _dsd_gap@ov074_3.o(.data.kuriMeta0)\n"
        " . = ALIGN(4); OV074_DATA.KURIMETA0_END = .;"
    )

    def interior(self, text):
        with tempfile.TemporaryDirectory() as d:
            p = pathlib.Path(d) / "test.lcf"
            p.write_text(text)
            return P.verify_terminal_lcf(
                p,
                module="OV074",
                section=".data.kuriMeta0",
                provider="d_a_kuri_king.o",
                placement="interior",
            )

    def test_interior_names_its_direct_successor(self):
        row = self.interior(self.text(self.INTERIOR))
        self.assertEqual(
            (row["placement"], row["successor"], row["fill"]),
            ("interior", "_dsd_gap@ov074_3.o", 0),
        )

    def test_section_end_mode_refuses_interior_shape(self):
        with self.assertRaises(ValueError):
            self.verify(self.text(self.INTERIOR))

    def test_interior_mode_refuses_last_provider(self):
        # No successor: the section's terminal ALIGN, not ALIGNALL, decides the fill.
        with self.assertRaises(ValueError):
            self.interior(self.text())

    def test_interior_refuses_anything_between_provider_and_successor(self):
        for between in (
            ". = ALIGN(8);",
            ". = ALIGN(4);",
            ". = . + 2;",
            "WRITEW(0);",
            "label = .;",
        ):
            with self.subTest(between=between), self.assertRaises(ValueError):
                self.interior(
                    self.text(
                        self.INTERIOR.replace(
                            " _dsd_gap@ov074_3.o", " " + between + "\n _dsd_gap@ov074_3.o"
                        )
                    )
                )

    def test_interior_refusals(self):
        bad = (
            # provider selected twice
            self.INTERIOR.replace(" head.o", " d_a_kuri_king.o(.data.kuriMeta0)\n head.o"),
            # provider before the START marker
            self.INTERIOR.replace(
                ". = ALIGN(4); OV074_DATA.KURIMETA0_START = .;\n head.o(.data.kuriMeta0)\n"
                " d_a_kuri_king.o(.data.kuriMeta0)\n",
                "d_a_kuri_king.o(.data.kuriMeta0)\n"
                ". = ALIGN(4); OV074_DATA.KURIMETA0_START = .;\n head.o(.data.kuriMeta0)\n",
            ),
            # successor selects a different section
            self.INTERIOR.replace("_dsd_gap@ov074_3.o(.data.kuriMeta0)",
                                  "_dsd_gap@ov074_3.o(.data)"),
            # a name that merely ends with the provider's name is not the provider
            self.INTERIOR.replace(" d_a_kuri_king.o", " xd_a_kuri_king.o"),
            # wildcard selector anywhere
            self.INTERIOR.replace(" head.o(.data.kuriMeta0)", " *(.data.kuriMeta0)"),
        )
        for body in bad:
            with self.subTest(body=body), self.assertRaises(ValueError):
                self.interior(self.text(body))
        for text in (
            self.text(self.INTERIOR).replace("ALIGNALL(4);", "ALIGNALL(4); FILL(255);"),
            self.text(self.INTERIOR).replace("} > OV074", "} > OV074 = 255"),
        ):
            with self.subTest(text=text), self.assertRaises(ValueError):
                self.interior(text)

    def test_missing_or_duplicate_marker(self):
        for text in (
            self.text().replace("KURIMETA0_END", "OTHER_END"),
            self.text().replace(
                "ALIGNALL(4);", "ALIGNALL(4); OV074_DATA.KURIMETA0_END = .;"
            ),
        ):
            with self.subTest(text=text), self.assertRaises(ValueError):
                self.verify(text)


def _sections_elf(rows):
    """Minimal ELF32 relocatable: one PROGBITS section per (name, align, size)."""
    import struct

    names = b"\0.shstrtab\0" + b"".join(name.encode() + b"\0" for name, _a, _s in rows)
    offsets, cursor = [], 11
    for name, _a, _s in rows:
        offsets.append(cursor)
        cursor += len(name) + 1
    data = b"".join(bytes(size) for _n, _a, size in rows)
    shoff = 52 + len(data) + len(names)
    headers = [bytes(40)]
    at = 52
    for (name, align, size), offset in zip(rows, offsets):
        headers.append(struct.pack("<10I", offset, 1, 3, 0, at, size, 0, 0, align, 0))
        at += size
    headers.append(struct.pack("<10I", 1, 3, 0, 0, at, len(names), 0, 0, 1, 0))
    ident = b"\x7fELF\x01\x01\x01" + bytes(9)
    header = ident + struct.pack(
        "<HHIIIIIHHHHHH", 1, 40, 1, 0, 0, shoff, 0, 52, 0, 0, 40, len(headers),
        len(headers) - 1)
    return header + data + names + b"".join(headers)


class SuccessorTests(unittest.TestCase):
    def test_alignall_places_a_byte_aligned_gap_at_the_next_word(self):
        raw = _sections_elf([(".data", 1, 0x3B7C)])
        self.assertEqual(P.successor_start(raw, ".data", 0x208E4AE), 0x208E4B0)

    def test_larger_own_alignment_moves_the_successor(self):
        raw = _sections_elf([(".data", 32, 8)])
        self.assertEqual(P.successor_start(raw, ".data", 0x208E4AE), 0x208E4C0)

    def test_empty_leading_sections_still_align(self):
        raw = _sections_elf([(".text", 32, 4), (".data", 8, 0), (".data", 4, 4)])
        self.assertEqual(P.successor_start(raw, ".data", 0x208E4AE), 0x208E4B0)
        raw = _sections_elf([(".data", 16, 0), (".data", 4, 4)])
        self.assertEqual(P.successor_start(raw, ".data", 0x208E4AE), 0x208E4B0)
        raw = _sections_elf([(".data", 32, 0), (".data", 4, 4)])
        self.assertEqual(P.successor_start(raw, ".data", 0x208E4AE), 0x208E4C0)

    def test_no_contribution_or_malformed_refuses(self):
        for raw in (
            _sections_elf([(".text", 4, 4)]),
            _sections_elf([(".data", 4, 0)]),
            b"not an elf",
        ):
            with self.subTest(raw=raw[:8]), self.assertRaises(ValueError):
                P.successor_start(raw, ".data", 0x208E4AE)


if __name__ == "__main__":
    unittest.main()
