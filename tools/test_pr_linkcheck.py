"""tools/pr_linkcheck.py's pre-supplied-object path: it now resolves a nested
entry point and a zero-size EABI alias record exactly as tools/linkcheck.py's
own corpus/--name path does, instead of losing the offset winning_object
returns.

WHY THIS EXISTS
---------------
LINKCHK2 (#2535) taught tools/linkcheck.py the two len-mismatch shapes the byte
gate could not verify -- a zero-size EABI alias record (a second name for a
differently-named, correctly-sized primary function at the same address, e.g.
_dmul aliasing func_01ff8708) and a nested entry point (a symbol whose ROM
range sits inside a containing symbol's compiled span, e.g. func_01ff98f4
inside func_01ff97d8.c's 0xb6c bytes) -- but its own PR text named one caller
left behind: tools/pr_linkcheck.py pre-supplies obj/sym straight into
LC.linkcheck rather than letting it call reloc_audit.winning_object itself, so
a nonzero nested-entry-point offset had nowhere to go, and the caller's own
first-loop winning_object call used the raw (unresolved) size for an alias
record, never the corrected one linkcheck() substitutes at its own top.

Measured directly against the real rows LINKCHK2's own PR text named (see
out/LINKCHK2/pr.txt and out/PRLINKNEST/pr.txt in the run directory). Before
this fix:

    python tools/pr_linkcheck.py --files src/func_01ff97d8.c
      NO-SYM src/func_01ff97d8.c  (6 slot(s))

Four of its six owned symbols -- every nested entry point with a nonzero own
size (func_01ff98f4, func_01ff99a4, func_01ff9d40, func_01ff9e2c) -- read
NO-SYM; only the container itself (off=0, the ordinary case) and the zero-size
alias _deq (which happened to self-heal through the `obj is None` fallback,
since its own first-loop winning_object call always failed on the raw size
and fell through to linkcheck()'s internal resolution) verified. After:

    ok      src/func_01ff97d8.c  (6 slot(s))

All six VERIFIED. A 34-file control sample of ordinary (non-alias,
non-nested) src files -- spanning arm9, itcm and four overlays -- is
byte-identical before and after this change.

THE FIX, two parts
-------------------
linkcheck.linkcheck() takes an explicit `off=0` parameter now instead of
hard-resetting it to 0 whenever `obj` is pre-supplied, so a caller that
already resolved a nested entry point's offset (via its own winning_object
call) can carry it through.

pr_linkcheck.check_file's first loop mirrors linkcheck()'s own resolution
order exactly: it corrects a zero-size alias record to its sized twin's real
length (bytegate.alias_target_size -- imported, not copied; the same helper
linkcheck() calls) BEFORE calling winning_object, then threads the offset
winning_object returns into LC.linkcheck via off=. Both parts matter together
for a symbol shaped like _deq -- an alias whose sized twin is itself a nested
entry point: the size fix is what makes winning_object see a resolvable
(nonzero) range at all, and the offset fix is what slices that range at the
right place once it does.

The tests below mock at the same winning_object/extract_func/
func_relocs_typed/rom_bytes boundary tools/test_linkcheck.py's
NestedOffsetRebaseUnit does, for the same reason stated there: a real compile
alone cannot prove the offset ARITHMETIC is correct rather than accidentally
vacuous (any sub-slice of an already-byte-perfect span trivially matches
wherever you cut it). These construct a synthetic containing symbol where the
right slice is known by hand and assert the exact window -- start, length,
and content -- handed to link_function, the function that actually applies
the relocations and does the byte compare, not just the final verdict; and,
for the two shapes off= exists to fix, call linkcheck() the way the pre-fix
pr_linkcheck.py always did (off hard-defaulted to 0, discarding whatever
winning_object actually returned) next to the way it does now, so the same
mocked fixture pins both the broken and the fixed behavior in one place.

    python -m unittest tools.test_pr_linkcheck -v
"""
import contextlib
import pathlib
import struct
import sys
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import linkcheck as LC        # noqa: E402
import reloc_audit as RA      # noqa: E402
import reverify_corpus as RV  # noqa: E402
import bytegate as BG         # noqa: E402
import pr_linkcheck as PL     # noqa: E402


def _spy(fn, calls):
    """Wrap `fn` so every call's args land in `calls`, then runs it for real."""
    def wrapped(*a, **kw):
        calls.append(a)
        return fn(*a, **kw)
    return wrapped


class NestedOffsetMustBeThreadedThroughLinkcheck(unittest.TestCase):
    """func_01ff98f4's own shape: NOT an alias (its config row carries its own
    real, nonzero size), but winning_object's own object is the CONTAINING
    symbol's (func_01ff97d8, 0xb6c bytes in the real tree) -- the offset is
    the only thing that says where inside it `name`'s own bytes start.

    A 0x54-byte containing symbol: 0x50 bytes of unrelated prefix (comfortably
    past link_function's 0x40 split-symbol-carrier overhang bound, so that
    branch is never in play here -- the real func_01ff97d8/func_01ff98f4 gap
    is 0xa80 bytes, the same shape), then the 4-byte nested function body
    itself -- one R_ARM_ABS32 pool slot, zero until the linker fills it.
    Addresses are chosen well outside every real module image so is_benign's
    real-ROM veneer/twin checks -- which read actual module bytes -- are
    mocked out rather than relied on to see nothing."""

    CONTAINER_CODE = b"\x11" * 0x50 + b"\x00\x00\x00\x00"
    NESTED_ADDR = 0x02000050
    RELOC_TARGET = 0xEE001000

    def _run(self, off, calls):
        target = struct.pack("<I", self.RELOC_TARGET)
        relocs = [{"off": 0x50, "type": LC.R_ARM_ABS32, "sym": "in_range",
                   "addr": self.RELOC_TARGET, "add": 0}]
        with contextlib.ExitStack() as stack:
            stack.enter_context(mock.patch.object(
                LC.M, "extract_func", return_value=(self.CONTAINER_CODE, set())))
            stack.enter_context(mock.patch.object(LC, "func_relocs_typed", return_value=relocs))
            stack.enter_context(mock.patch.object(RV, "rom_bytes", return_value=target))
            stack.enter_context(mock.patch.object(LC, "is_benign", return_value=False))
            stack.enter_context(mock.patch.object(LC, "is_interwork", return_value=False))
            stack.enter_context(mock.patch.object(
                LC, "link_function", side_effect=_spy(LC.link_function, calls)))
            return LC.linkcheck("func_01ff98f4", self.NESTED_ADDR, 4, "itcm", {},
                                obj=b"OBJ", sym="func_01ff97d8", off=off)

    def test_off_0_the_pre_fix_default_compares_the_wrong_window_and_fails(self):
        """Calling linkcheck() the way pre-fix pr_linkcheck.py always did --
        obj/sym pre-supplied, off left at its old hard-reset value of 0 --
        never even reaches a byte comparison: the container's own full,
        un-sliced length (0x54) can never equal the nested target's real
        length (4), so it returns NO-SYM (len-mismatch) before link_function
        is called at all. Proven, not asserted: link_function is spied and
        must see zero calls."""
        calls = []
        r = self._run(off=0, calls=calls)
        self.assertEqual(r["verdict"], "NO-SYM")
        self.assertEqual(r["reason"], "len-mismatch")
        self.assertEqual(calls, [])

    def test_off_threaded_through_compares_the_real_sliced_window_and_verifies(self):
        """Same object, same symbol, off=0x50 -- exactly what winning_object
        itself would have returned. Asserts the WINDOW handed to
        link_function, not just the verdict: it must be the 4 bytes at
        container offset 0x50:0x54, not the container's own first 4 bytes and
        not the whole 0x54-byte span."""
        calls = []
        r = self._run(off=0x50, calls=calls)
        self.assertEqual(r["verdict"], "VERIFIED")
        self.assertEqual(len(calls), 1)
        code_seen = calls[0][0]
        self.assertEqual(len(code_seen), 4)
        self.assertEqual(code_seen, self.CONTAINER_CODE[0x50:0x54])
        self.assertNotEqual(code_seen, self.CONTAINER_CODE[0:4])


class AliasOfANestedEntryPointMustBeThreadedThroughLinkcheck(unittest.TestCase):
    """_deq's own shape: a zero-size EABI alias record whose sized twin
    (func_01ff9d40 in the real tree) is ITSELF a nested entry point. Both
    fixes have to fire together: alias_target_size substitutes the real size
    (4, here) before the length checks run, and the offset has to reach the
    slice the same way the plain-nested case above needs it to."""

    CONTAINER_CODE = b"\x22" * 0x50 + b"\x00\x00\x00\x00"
    ALIAS_ADDR = 0x02000050
    RELOC_TARGET = 0xEE002000

    def _run(self, off, calls=None):
        target = struct.pack("<I", self.RELOC_TARGET)
        relocs = [{"off": 0x50, "type": LC.R_ARM_ABS32, "sym": "in_range",
                   "addr": self.RELOC_TARGET, "add": 0}]
        with contextlib.ExitStack() as stack:
            stack.enter_context(mock.patch.object(BG, "alias_target_size", return_value=4))
            stack.enter_context(mock.patch.object(
                LC.M, "extract_func", return_value=(self.CONTAINER_CODE, set())))
            stack.enter_context(mock.patch.object(LC, "func_relocs_typed", return_value=relocs))
            stack.enter_context(mock.patch.object(RV, "rom_bytes", return_value=target))
            stack.enter_context(mock.patch.object(LC, "is_benign", return_value=False))
            stack.enter_context(mock.patch.object(LC, "is_interwork", return_value=False))
            if calls is not None:
                stack.enter_context(mock.patch.object(
                    LC, "link_function", side_effect=_spy(LC.link_function, calls)))
            return LC.linkcheck("_deq", self.ALIAS_ADDR, 0, "itcm", {},
                                obj=b"OBJ", sym="func_01ff97d8", off=off)

    def test_off_0_compares_the_wrong_window_even_with_the_size_corrected(self):
        """The alias-size substitution alone is not enough: with off left at
        the pre-fix default of 0, the corrected (4-byte) target is compared
        against the container's own first 4 bytes -- the wrong window -- not
        the nested body at offset 0x50. Those first 4 bytes are 0x22222222,
        never equal to the RELOC_TARGET word the ROM target encodes, so this
        reads WRONG, not VERIFIED: a false negative on a genuinely correct
        source, exactly the class of bug a silently wrong slice produces."""
        r = self._run(off=0)
        self.assertNotEqual(r["verdict"], "VERIFIED")

    def test_off_threaded_through_compares_the_real_sliced_window_and_verifies(self):
        calls = []
        r = self._run(off=0x50, calls=calls)
        self.assertEqual(r["verdict"], "VERIFIED")
        self.assertEqual(len(calls), 1)
        code_seen = calls[0][0]
        self.assertEqual(len(code_seen), 4)
        self.assertEqual(code_seen, self.CONTAINER_CODE[0x50:0x54])


class CheckFileResolvesAndThreadsBeforeCallingLinkcheck(unittest.TestCase):
    """End-to-end through pr_linkcheck.check_file itself, not just
    linkcheck.linkcheck: proves check_file's own first loop -- the one that
    pre-supplies obj/sym -- applies the size correction and captures the
    offset from ITS OWN winning_object call, the way the two classes above
    prove linkcheck() uses them once handed in."""

    CONTAINER_CODE = b"\x33" * 0x50 + b"\x00\x00\x00\x00"
    ADDR = 0x02000050
    RELOC_TARGET = 0xEE003000

    def test_nested_owned_symbol_resolved_via_check_files_own_winning_object_call(self):
        idx = {"nested_child": [(self.ADDR, 4, "itcm")]}
        target = struct.pack("<I", self.RELOC_TARGET)
        relocs = [{"off": 0x50, "type": LC.R_ARM_ABS32, "sym": "in_range",
                   "addr": self.RELOC_TARGET, "add": 0}]
        calls = []
        with contextlib.ExitStack() as stack:
            stack.enter_context(mock.patch.object(PL.SP, "symbols_for",
                                                   return_value=["nested_child"]))
            wo = stack.enter_context(mock.patch.object(
                RA, "winning_object",
                return_value=(b"OBJ", "func_container", None, 0x50)))
            stack.enter_context(mock.patch.object(
                LC.M, "extract_func", return_value=(self.CONTAINER_CODE, set())))
            stack.enter_context(mock.patch.object(LC, "func_relocs_typed", return_value=relocs))
            stack.enter_context(mock.patch.object(RV, "rom_bytes", return_value=target))
            stack.enter_context(mock.patch.object(LC, "is_benign", return_value=False))
            stack.enter_context(mock.patch.object(LC, "is_interwork", return_value=False))
            stack.enter_context(mock.patch.object(
                LC, "link_function", side_effect=_spy(LC.link_function, calls)))
            rep = PL.check_file("fake/nested_child.c", idx, {})
        # winning_object was asked with the owned symbol's own nonzero size
        # (this row is not an alias, nothing to correct).
        self.assertEqual(wo.call_args.args[2], 4)
        self.assertEqual(rep["results"][0]["verdict"], "VERIFIED")
        self.assertEqual(calls[0][0], self.CONTAINER_CODE[0x50:0x54])

    def test_alias_child_gets_the_size_corrected_before_winning_object_is_called(self):
        """_deq's shape again, driven through check_file: idx (the config
        symbol index check_file resolves owned symbols against) carries the
        alias's raw recorded size, 0. check_file's first loop must correct it
        to the sized twin's real length (4, mocked via alias_target_size)
        BEFORE calling winning_object -- a fake winning_object that still
        sees a raw 0 returns len-mismatch, the pre-fix shape, which would
        read NO-SYM here rather than VERIFIED."""
        idx = {"alias_child": [(self.ADDR, 0, "itcm")]}
        target = struct.pack("<I", self.RELOC_TARGET)
        relocs = [{"off": 0x50, "type": LC.R_ARM_ABS32, "sym": "in_range",
                   "addr": self.RELOC_TARGET, "add": 0}]
        sizes_seen = []

        def fake_winning_object(name, addr, size, mod, *rest, **kwargs):
            sizes_seen.append(size)
            if size == 0:
                return None, None, "len-mismatch", 0
            return b"OBJ", "func_container", None, 0x50

        with contextlib.ExitStack() as stack:
            stack.enter_context(mock.patch.object(PL.SP, "symbols_for",
                                                   return_value=["alias_child"]))
            alt = stack.enter_context(mock.patch.object(
                BG, "alias_target_size", return_value=4))
            stack.enter_context(mock.patch.object(
                RA, "winning_object", side_effect=fake_winning_object))
            stack.enter_context(mock.patch.object(
                LC.M, "extract_func", return_value=(self.CONTAINER_CODE, set())))
            stack.enter_context(mock.patch.object(LC, "func_relocs_typed", return_value=relocs))
            stack.enter_context(mock.patch.object(RV, "rom_bytes", return_value=target))
            stack.enter_context(mock.patch.object(LC, "is_benign", return_value=False))
            stack.enter_context(mock.patch.object(LC, "is_interwork", return_value=False))
            rep = PL.check_file("fake/alias_child.c", idx, {})
        # Called from check_file's own first loop, and again inside
        # LC.linkcheck's unconditional top-of-function substitution (linkcheck
        # re-derives it independently -- the same "both call sites need it"
        # shape LINKCHK2's own AliasSizeSubstitutionUnit test pins) -- both
        # calls see the real address, never a stale or wrong one.
        self.assertEqual(alt.call_count, 2)
        for args, _kwargs in alt.call_args_list:
            self.assertEqual(args, ("itcm", self.ADDR))
        # winning_object itself is never asked with the raw, uncorrected 0 --
        # that is the len-mismatch dead end the pre-fix caller always hit.
        self.assertEqual(sizes_seen, [4])
        self.assertEqual(rep["results"][0]["verdict"], "VERIFIED")


if __name__ == "__main__":
    unittest.main()
