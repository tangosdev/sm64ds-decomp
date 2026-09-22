"""The two CONVERTED-tier readings that were measurably wrong, pinned in both directions.

Neither of these is a regression test in the usual sense: each one records a
distinction the gate could not previously make, and for each the NEGATIVE half is the
half that matters. A `volatile` regex that stops catching match hacks is a worse defect
than the one it fixes, and a GONE-path resolver that makes TU promotions free would
delete the gate's whole reason to exist. So every class below asserts the thing that
must still be caught next to the thing that must now be let through.
"""
import json
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tiers  # noqa: E402
import tiers_ratchet as ratchet  # noqa: E402


def trick(text):
    """The no_codegen_trick criterion for a snippet: True when it scores a trick."""
    return not tiers.score_file("x.c", text)["no_codegen_trick"]


class MmioIsNotACodegenTrick(unittest.TestCase):
    """`volatile` reaching a Nintendo DS hardware register through a POINTER.

    There is no other way to write VRAM, the geometry engine or the IPC/DMA/divider
    registers, so this is correct code, not steering. All of these are shapes taken
    verbatim from src/.
    """

    def test_cast_to_volatile_pointer(self):
        self.assertFalse(trick("*(volatile unsigned int *)0x4000400 = 0x1a;\n"))

    def test_volatile_pointer_local(self):
        self.assertFalse(trick("volatile u16 *ime = (volatile u16 *)0x4000208;\n"))

    def test_volatile_struct_pointer(self):
        self.assertFalse(trick("volatile DMAChannelRegs *reg;\n"))

    def test_volatile_pointer_parameter(self):
        self.assertFalse(trick(
            "void G2x::SetBGyAffine(volatile unsigned short *p, Matrix2x2 *m) {}\n"))

    def test_volatile_void_pointer_return(self):
        self.assertFalse(trick("extern volatile void *GetBG1ScrPtr(void);\n"))

    def test_qualified_type_before_the_star(self):
        self.assertFalse(trick("volatile Nitro::Reg *r;\n"))

    def test_real_file_of_pure_mmio(self):
        p = ratchet.REPO / "src" / "_ZN8dScene_c22ResetHardwareRegistersEv.cpp"
        if not p.is_file():
            self.skipTest(f"{p} not in this tree")
        text = p.read_text(errors="replace")
        self.assertGreater(text.count("volatile"), 50)
        self.assertFalse(trick(text))


class MatchHacksAreStillCodegenTricks(unittest.TestCase):
    """A volatile OBJECT: the spill-forcing / CSE-defeating idiom, still caught."""

    def test_volatile_scalar_local(self):
        self.assertTrue(trick("    volatile int li;\n"))

    def test_volatile_scalar_local_with_initialiser(self):
        self.assertTrue(trick("    volatile s32 zero = 0;\n"))

    def test_volatile_aggregate_local(self):
        self.assertTrue(trick("    volatile Vector3 v;\n"))

    def test_volatile_array_local(self):
        self.assertTrue(trick("    volatile int dummy[4];\n"))

    def test_volatile_struct_field(self):
        self.assertTrue(trick("struct S { char pad[0x14]; volatile u32 f14; };\n"))

    def test_volatile_cast_round_trip(self):
        # notes: volatile-roundtrip-demotes-a-local
        self.assertTrue(trick("    rsc = (s32)(volatile s32)rsc;\n"))

    def test_volatile_cast_inside_an_expression(self):
        self.assertTrue(trick("    pos.x = px * (volatile int)m;\n"))

    def test_volatile_pointer_object_not_pointer_to_volatile(self):
        # The POINTER is volatile here, not the pointee: a spill hack, not MMIO.
        self.assertTrue(trick("    Node *volatile arr_a[4];\n"))

    def test_extern_volatile_ram_global(self):
        self.assertTrue(trick("extern volatile int data_0209e670;\n"))

    def test_the_other_two_tricks_are_untouched(self):
        self.assertTrue(trick("    addr = fillReg & 0xFFFFFFFFFFFFFFFF;\n"))
        self.assertTrue(trick("    __asm { nop }\n"))

    def test_a_comment_still_does_not_score(self):
        # _code_only masks comments; this is the existing behaviour, re-pinned
        # because the new regex must not reach through the mask either.
        self.assertFalse(trick("/* uses a volatile int scratch to force a spill */\n"))


class PromotedMoves(unittest.TestCase):
    """legacy_source -> promoted_source, read through tools/tu_manifest.py."""

    def _manifest(self, tmp, entries):
        root = pathlib.Path(tmp) / "manifest.json"
        root.write_text(json.dumps({"schema_version": 1, "entries": entries}),
                        encoding="utf-8")
        return root

    def test_only_promoted_entries_count(self):
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            root = self._manifest(tmp, [
                {"id": "ovA/X", "status": "promoted",
                 "promoted_source": "src/actors/X.cpp",
                 "functions": [{"legacy_source": "src/_ZN1X1aEv.cpp"},
                               {"legacy_source": "src/_ZN1X1bEv.cpp"}]},
                {"id": "ovA/Y", "status": "text-verified",
                 "promoted_source": "src/actors/Y.cpp",
                 "functions": [{"legacy_source": "src/_ZN1Y1aEv.cpp"}]},
            ])
            moves = ratchet.promoted_moves(root)
        self.assertEqual(moves, {
            "src/_ZN1X1aEv.cpp": ("ovA/X", "src/actors/X.cpp"),
            "src/_ZN1X1bEv.cpp": ("ovA/X", "src/actors/X.cpp"),
        })

    def test_the_live_manifest_resolves(self):
        for legacy, (tu_id, dest) in ratchet.promoted_moves().items():
            self.assertTrue(dest.startswith("src/"), f"{tu_id}: {dest}")
            self.assertNotEqual(legacy, dest)


class MoveIsNotADeletion(unittest.TestCase):
    """A GONE path reads as a MOVE, and only counts as a backslide when it dilutes."""

    MOVES = {"src/_ZN1X1aEv.cpp": ("ovA/X", "src/actors/X.cpp")}
    TRACKED = {"src/actors/X.cpp", "src/kept.c"}
    CLEAN = dict.fromkeys(tiers.CRITERIA, True)
    DIRTY = dict(CLEAN, no_mangled_refs=False)

    def test_a_real_deletion_still_reads_as_gone(self):
        msg = ratchet.why("src/_ZN1Z1aEv.cpp", {}, self.TRACKED, self.MOVES)
        self.assertIn("GONE", msg)
        self.assertNotIn("MOVED", msg)

    def test_a_move_names_the_absorbing_file(self):
        msg = ratchet.why("src/_ZN1X1aEv.cpp", {"src/actors/X.cpp": self.DIRTY},
                          self.TRACKED, self.MOVES)
        self.assertIn("MOVED", msg)
        self.assertIn("src/actors/X.cpp", msg)
        self.assertIn("ovA/X", msg)
        self.assertIn(tiers.CRITERION_LABEL["no_mangled_refs"], msg)

    def test_a_move_into_a_clean_file_says_nothing_was_lost(self):
        msg = ratchet.why("src/_ZN1X1aEv.cpp", {"src/actors/X.cpp": self.CLEAN},
                          self.TRACKED, self.MOVES)
        self.assertIn("MOVED", msg)
        self.assertIn("passes all five", msg)

    def test_a_move_to_an_untracked_file_is_a_deletion(self):
        msg = ratchet.why("src/_ZN1X1aEv.cpp", {}, {"src/kept.c"}, self.MOVES)
        self.assertIn("not tracked", msg)

    def test_clean_absorption_is_not_a_backslide(self):
        clean, backslid = ratchet.classify_missing(
            ["src/_ZN1X1aEv.cpp"], {"src/actors/X.cpp"}, self.TRACKED, self.MOVES)
        self.assertEqual((clean, backslid), (["src/_ZN1X1aEv.cpp"], []))

    def test_absorption_into_a_failing_file_STILL_fails(self):
        # The load-bearing half: promotions are not free. The absorbing file does not
        # pass all five, so the banked path really did lose its status.
        clean, backslid = ratchet.classify_missing(
            ["src/_ZN1X1aEv.cpp"], set(), self.TRACKED, self.MOVES)
        self.assertEqual((clean, backslid), ([], ["src/_ZN1X1aEv.cpp"]))

    def test_an_unrelated_deletion_is_never_absorbed(self):
        clean, backslid = ratchet.classify_missing(
            ["src/_ZN1Z1aEv.cpp"], {"src/actors/X.cpp"}, self.TRACKED, self.MOVES)
        self.assertEqual((clean, backslid), ([], ["src/_ZN1Z1aEv.cpp"]))

    def test_a_path_that_still_exists_is_judged_on_its_own_criteria(self):
        # Present in the tree and failing: the manifest must not launder it into a move.
        clean, backslid = ratchet.classify_missing(
            ["src/kept.c"], set(), self.TRACKED, self.MOVES)
        self.assertEqual((clean, backslid), ([], ["src/kept.c"]))


class BaselineIsInSync(unittest.TestCase):
    """`--check` must exit 0 on a clean tree, or every PR after this one goes red."""

    def test_check_passes(self):
        banked = ratchet.load_baseline(ratchet.BASELINE)
        self.assertIsNotNone(banked, "no usable config/converted-baseline.json")
        current, _ = ratchet.scan()
        _, backslid = ratchet.classify_missing(
            sorted(banked - current), current, set(ratchet.tracked_sources()),
            ratchet.promoted_moves())
        self.assertEqual(backslid, [])


class MatchedCaptionNamesTheAssemblySubset(unittest.TestCase):
    """MATCHED is billed as byte-exact C, and 113 of the functions under it are not C.

    They are real matches -- the original was assembly, so the asm block IS the
    recovered source -- but a bar that does not say so is a bar a reader can be
    surprised by. The caption is generated from the same database MATCHED is read from,
    never written into the README by hand, because it is a number.
    """

    BASE = {"converted": {"converted": 5, "functions": 10, "pct": 50.0},
            "linked": None}

    def matched(self, **kw):
        m = {"matched": 90, "functions": 100, "pct": 90.0,
             "matchedBytes": 900, "totalBytes": 1000, "bytePct": 90.0,
             "handAsm": 7, "handAsmBytes": 70}
        m.update(kw)
        return dict(self.BASE, matched=m)

    def test_the_caption_follows_the_matched_row(self):
        rows = tiers.bar_block(self.matched()).splitlines()
        self.assertTrue(rows[1].startswith("MATCHED"))
        self.assertIn("of which 7 are byte-exact assembly", rows[2])

    def test_the_caption_is_indented_so_it_is_not_mistaken_for_the_row(self):
        """write_readme preserves the last generated MATCHED row by finding the line
        that STARTS with "MATCHED". A caption flush to the margin would be picked up as
        the row itself and the real number would be lost on the next refresh."""
        caption = tiers.matched_caption(self.matched()["matched"])
        self.assertTrue(caption.startswith(" "))
        self.assertFalse(caption.strip().startswith("MATCHED"))

    def test_no_caption_when_the_database_predates_the_field(self):
        """An older chaos-db.json has no handAsmFunctions. Absent must read as "do not
        print the line", never as zero, or the bar would claim every match is C."""
        self.assertIsNone(tiers.matched_caption(
            {"matched": 90, "functions": 100, "pct": 90.0, "handAsm": None}))
        rows = tiers.bar_block(self.matched(handAsm=None)).splitlines()
        self.assertEqual(len(rows), 4, rows)

    def test_no_caption_when_the_subset_is_empty(self):
        self.assertIsNone(tiers.matched_caption(self.matched(handAsm=0)["matched"]))

    def test_the_report_names_the_subset_in_bytes_as_well(self):
        converted = {"converted": 5, "functions": 10, "pct": 50.0, "source_files": 8,
                     "criteria": {k: 5 for k in tiers.CRITERIA},
                     "distribution": {str(i): 2 for i in range(len(tiers.CRITERIA) + 1)},
                     "alt_core_two": 6, "alt_shared_header": 7}
        out = tiers.report(dict(self.matched(), converted=converted))
        self.assertIn("of which 7 functions (70 bytes) are byte-exact assembly", out)

    def test_a_preserved_matched_row_keeps_its_caption(self):
        """A worktree with no chaos-db.json cannot supply MATCHED, so write_readme
        carries the last generated row forward. Carrying the row without the caption
        would leave the block claiming more C than anything measured."""
        current = ("\n```\n"
                   "MATCHED    ####  99.6%   11,342 / 11,390 functions\n"
                   "           of which 113 are byte-exact assembly (hand-written "
                   "in the original, not C)\n"
                   "CONVERTED  ##    23.8%   2,707 / 11,357 functions\n"
                   "```\n")
        lines = current.splitlines()
        keep = []
        for i, line in enumerate(lines):
            if line.strip().startswith("MATCHED"):
                keep.append(line.strip())
                if i + 1 < len(lines) and lines[i + 1].strip().startswith("of which"):
                    keep.append(lines[i + 1].rstrip())
                break
        preserved = tiers.NEWLINE.join(keep)
        block = tiers.bar_block(dict(self.BASE, matched=None), preserved)
        self.assertIn("11,342 / 11,390", block)
        self.assertIn("of which 113 are byte-exact assembly", block)


if __name__ == "__main__":
    unittest.main()
