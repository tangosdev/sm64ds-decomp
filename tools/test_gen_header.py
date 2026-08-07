"""Regression tests for the gen_header evidence toolchain.

Every case here is a bug that actually shipped and was caught by review rather than
by any gate. That is the point: these tools produce *numbers about the tree*, and a
wrong number does not crash, does not fail a build, and reads exactly like a right
one. The failure mode is always the same shape -- something looked in the wrong
place, or did not look at all, and reported with full confidence.

  arm9 base       decoded every arm9 function 0x4000 bytes off; 2.1% of symbols
                  had a plausible prologue, against 64.8% at the right base
  nested classes  _ZN8Particle7Manager... filed under "Particle", contaminating it
                  and starving 17 nested headers
  ble vs bl       `startswith("bl")` treats a plain conditional branch as a call
  marker + pad    a pad separated from its marker by a comment was not seen, so a
                  marker read as a 1-byte width claim
  union confirm   a field counted as confirmed when ANY pass agreed, so passes that
                  disagreed with each other were silently resolved
  trailing marker given a 1-byte extent, contradicting load_headers' own docstring
  pointer parse   check_header_offsets could not parse `void **vtable`, skipped it
                  WITHOUT advancing the offset, and still printed 0 mismatched

No compiler and no extracted ROM required; everything here is static or uses a
temporary tree.
"""
import json
import pathlib
import re
import subprocess
import sys
import tempfile

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import evidence_rom as ER     # noqa: E402
import gen_header as GH       # noqa: E402


# --------------------------------------------------------------- evidence_rom

def test_arm9_base_matches_the_rest_of_the_tree():
    """arm9_dec.bin loads at 0x02004000. Three other tools already knew; this one
    did not, and a wrong base does not fail -- it decodes garbage confidently."""
    assert ER.ARM9_BASE == 0x02004000
    import modules
    assert ER.ARM9_BASE == modules.ARM9_BASE, "must agree with modules.py"


def test_prologue_guard_separates_right_base_from_wrong():
    """The guard exists because a wrong load address is otherwise invisible. If it
    cannot tell 65% from 2%, it is decoration."""
    assert ER.MIN_PROLOGUE_RATE > 0.05, "threshold must exclude the chance rate"
    assert ER.MIN_PROLOGUE_RATE < 0.60, "threshold must not exclude a correct base"


def test_nested_class_names_keep_every_component():
    cases = {
        "_ZN10BrickBlock8BehaviorEv": "BrickBlock",
        "_ZN10BrickBlockD0Ev": "BrickBlock",
        "_ZN8Particle7ManagerD1Ev": "Particle__Manager",
        "_ZN8Particle6System9NewSimpleEv": "Particle__System",
        "_ZN9ActorBase9SceneNodeD1Ev": "ActorBase__SceneNode",
        "_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii": "Actor",
        "func_ov002_020aea30": None,
    }
    for sym, want in cases.items():
        assert ER.class_of(sym) == want, f"{sym} -> {ER.class_of(sym)}, want {want}"


def test_calls_are_matched_by_instruction_id_not_string_prefix():
    """`ble`/`bls`/`blt` are plain branches. Matching them as `bl` clobbered r0-r3
    as though a call had happened, dropping live evidence."""
    src = (TOOLS / "evidence_rom.py").read_text(errors="replace")
    assert "ARM_INS_BL" in src and "ARM_INS_BLX" in src
    assert 'mnem.startswith("bl")' not in src, "string-prefix call matching is back"


def test_memory_ops_are_matched_by_instruction_id_not_string_prefix():
    """`strhs` is `str` with the HS condition, not `strh`. Capstone appends the
    condition code, so a prefix test files a 4-byte conditional store as a 2-byte one.

    Four such stores were the ENTIRE cross-pass-disagreement bucket -- ov064:0x02119974
    `strhi`, ov025:0x02112444 and 0x0211243c `strhs`, ov065:0x0211a7d0 `strhs`. This
    file documents the identical trap for branches and still committed it for memory.
    """
    src = (TOOLS / "evidence_rom.py").read_text(errors="replace")
    assert "MEMOPS" in src, "memory ops must be keyed on instruction id"
    for dead in ("MNEMONICS", 'mnem.startswith(m)', "in LOADS"):
        assert dead not in src, f"string-prefix memory matching is back: {dead}"
    # the collisions that motivated it, stated so the reason survives the code
    for cond, base in (("strhs", "strh"), ("strhi", "strh"),
                       ("ldrhs", "ldrh"), ("ldrhi", "ldrh")):
        assert cond.startswith(base), "premise of this test is wrong"


def test_stores_never_prove_signedness():
    """There is no `strsh`. A store writes the low bits whatever the source type, so
    treating one as evidence for `s16` would manufacture the exact codegen error the
    tool exists to find."""
    from capstone.arm import (ARM_INS_STRH, ARM_INS_STRB, ARM_INS_STR,
                              ARM_INS_LDRSH, ARM_INS_LDRSB, ARM_INS_LDR)
    for iid in (ARM_INS_STRH, ARM_INS_STRB, ARM_INS_STR):
        assert ER.MEMOPS[iid][1] is None, "a store must not carry signedness"
        assert ER.MEMOPS[iid][2] is False
    assert ER.MEMOPS[ARM_INS_LDRSH][1] == "s" and ER.MEMOPS[ARM_INS_LDRSB][1] == "s"
    assert ER.MEMOPS[ARM_INS_LDR][1] is None, "a word load says nothing about sign"


# ------------------------------------------------------------------ gen_header

def test_marker_pad_is_found_across_a_comment_block():
    """include/RaycastGround.h documents its own idiom over three lines, and the
    continuation lines start with prose -- not `*` -- so skipping comment-LOOKING
    prefixes is not enough."""
    lines = [
        "    u8  unk_010;            /* 0x010 - first byte of the ClsnResult",
        "                                       the hit is written into; kept as a",
        "                                       byte because the ctor spells it */",
        "    u8  pad_011[0x27];",
    ]
    assert GH.PAD_LINE.match(GH.next_meaningful(lines, 1, lines[0])), \
        "pad behind a comment block must still be found"


def test_trailing_marker_runs_to_the_end_of_the_struct():
    """load_headers' docstring says a trailing marker's object runs to the end.
    Giving it a 1-byte extent contradicted that and pushed interior offsets into
    'novel'."""
    with tempfile.TemporaryDirectory() as td:
        root = pathlib.Path(td)
        (root / "include").mkdir()
        (root / "include" / "T.h").write_text(
            "/* AUTO-GENERATED from matched-function evidence */\n"
            "struct T {\n"
            "    u8  pad_000[0x8];\n"
            "    u8  unk_008;            /* 0x008 */\n"
            "#ifdef __cplusplus\n"
            "#endif\n"
            "};\n")
        fields = GH.load_headers(root)["T"]
        _typ, _ptr, _arr, _nm, _where, placeholder, span = fields[0x8]
        assert placeholder, "a lone u8 before end-of-fields is the trailing marker form"
        assert span is None, "trailing marker extent is unknown, not 1"


def _tiny_tree(td, declared, rom_widths, history_widths):
    """A one-class tree with the two evidence passes disagreeing however we like."""
    root = pathlib.Path(td)
    (root / "include").mkdir()
    (root / "build").mkdir()
    (root / "include" / "T.h").write_text(
        "/* AUTO-GENERATED from matched-function evidence */\n"
        "struct T {\n"
        f"    {declared} unk_000;            /* 0x000 */\n"
        "};\n")
    for name, widths in (("rom", rom_widths), ("history", history_widths)):
        (root / "build" / f"evidence_{name}.json").write_text(json.dumps({
            "meta": {"pass": name, "recall": {}},
            "classes": {"T": {"0x0": {"widths": {str(w): 1 for w in widths},
                                      "signed": {"s": 0, "u": 0},
                                      "address_only": 0, "provenance": []}}}}))
    return root


def _buckets(root):
    subprocess.run([sys.executable, str(TOOLS / "gen_header.py"), "--root", str(root),
                    "-o", "build/r.json"], capture_output=True, text=True, check=True)
    return json.loads((root / "build" / "r.json").read_text())["buckets"]


def test_passes_that_disagree_are_not_confirmed_by_union():
    """Timer 0x0 was 'confirmed' as s32 because the ROM sees the two 4-byte halves of
    an s64 -- ARM has no 64-bit register, so it CANNOT see otherwise -- while history
    reads the real s64. Taking the union let the narrower pass win silently."""
    with tempfile.TemporaryDirectory() as td:
        b = _buckets(_tiny_tree(td, "s32", rom_widths=[4], history_widths=[8]))
    assert b.get("cross-pass width disagreement") == 1, b
    assert not b.get("confirmed"), "a disagreement must not be reported as confirmed"


def test_a_narrower_observation_is_not_a_disagreement():
    """A declared s64 can only be touched as two 4-byte halves -- ARM has no 64-bit
    register -- so 4-byte ROM accesses corroborate an 8-byte declaration rather than
    contradicting it. Timer and SphereClsn are exactly this shape."""
    with tempfile.TemporaryDirectory() as td:
        b = _buckets(_tiny_tree(td, "s64", rom_widths=[4], history_widths=[8]))
    assert b.get("confirmed") == 1, b
    assert not b.get("cross-pass width disagreement"), b


def test_a_wider_observation_is_still_a_disagreement():
    """Guards the guard: the rule above must not silence the real case. An access
    wider than the declaration reads past the field."""
    with tempfile.TemporaryDirectory() as td:
        b = _buckets(_tiny_tree(td, "u8", rom_widths=[1, 4], history_widths=[1, 4]))
    assert b.get("cross-pass width disagreement") == 1, b


def test_passes_that_agree_still_confirm():
    """Guards the guard: the test above must not pass by making everything a
    disagreement."""
    with tempfile.TemporaryDirectory() as td:
        b = _buckets(_tiny_tree(td, "s32", rom_widths=[4], history_widths=[4]))
    assert b.get("confirmed") == 1, b
    assert not b.get("cross-pass width disagreement"), b


def test_statics_are_classified_by_arity_not_by_mangling():
    """Nothing in the Itanium encoding distinguishes a static member from an instance
    one -- a method's `this` is implicit and never appears. So r0 in a static is just
    argument 1, and every access it makes would be filed against a class it may never
    touch. G2x is the extreme: its methods take a volatile u16* hardware register."""
    import static_symbols as SS
    assert SS.n_params("f()") == 0
    assert SS.n_params("f(int)") == 1
    assert SS.n_params("f(A<x,y>, int)") == 2, "commas inside <> are not separators"
    # rung-2 form: mangled name spelled directly, explicit this => instance
    inst = "int *_ZN10BrickBlockD0Ev(int *t)\n{\n"
    assert SS.classify("_ZN10BrickBlockD0Ev", inst)[0] == "instance"
    # same arity as the mangling => r0 is a real argument => static
    stat = "void _ZN5Stage14GraphCallback2EP12SceneRelated(void *a)\n{\n"
    assert SS.classify("_ZN5Stage14GraphCallback2EP12SceneRelated", stat)[0] == "static"
    # unparsable must be its own outcome, never a silent default to instance
    assert SS.classify("_ZN5Stage14GraphCallback2EP12SceneRelated", "")[0].startswith("undecided")


def test_history_and_statics_agree_on_arity():
    """Both passes must decide staticness the same way, or one filters a function
    the other keeps and the report contradicts itself. The pointer heuristic
    evidence_history had cannot do it: Stage::GraphCallback2(SceneRelated *) has a
    pointer first parameter and is still static."""
    import demangle
    import evidence_history as EH, static_symbols as SS
    for sym in ("_ZN5Stage14GraphCallback2EP12SceneRelated",
                "_ZN10BrickBlockD0Ev", "_ZN5Timer7GetTimeEv"):
        sig = demangle.signature(sym)
        assert sig, f"demangler regressed on {sym}"
        assert EH.mangled_param_count(sym) == SS.n_params(sig), sym
    assert EH.mangled_param_count("_ZN5Timer7GetTimeEv") == 0
    assert EH.mangled_param_count("_ZN5Stage14GraphCallback2EP12SceneRelated") == 1
    # an unreadable symbol must yield None, never 0 -- 0 would make every
    # zero-argument C definition look static and delete real evidence
    assert EH.mangled_param_count("not a mangled name") is None


# ------------------------------------------------------- check_header_offsets

def _check(tmp, text):
    p = pathlib.Path(tmp) / "H.h"
    p.write_text(text)
    r = subprocess.run([sys.executable, str(TOOLS / "check_header_offsets.py"), str(p)],
                       capture_output=True, text=True)
    return r.stdout


def test_pointer_declarations_parse_and_advance_the_offset():
    """`void **vtable` was skipped WITHOUT advancing the running offset, so every
    later field 'matched' four bytes out of place -- and it printed 0 mismatched."""
    with tempfile.TemporaryDirectory() as td:
        out = _check(td, "struct H {\n"
                         "    void **vtable;          /* 0x000 */\n"
                         "    u32 uniqueID;           /* 0x004 */\n"
                         "};\n")
    assert "0 mismatched" in out and "0 unparsed" in out, out


def test_an_unparsable_declaration_is_reported_not_skipped():
    """Silently skipping is what made the pointer bug invisible. An unknown type
    leaves the offset short, so it must be loud."""
    with tempfile.TemporaryDirectory() as td:
        out = _check(td, "struct H {\n"
                         "    WeirdType  thing;       /* 0x000 */\n"
                         "    u32 after;              /* 0x004 */\n"
                         "};\n")
    assert "UNPARSED" in out, out


def test_a_real_offset_error_is_caught():
    """The gate has to be able to fail, or 0-mismatched means nothing."""
    with tempfile.TemporaryDirectory() as td:
        out = _check(td, "struct H {\n"
                         "    u32 a;                  /* 0x000 */\n"
                         "    u32 b;                  /* 0x008 */\n"   # really 0x004
                         "};\n")
    assert "MISMATCH" in out, out


def test_an_elaborated_type_specifier_is_a_declaration():
    """`struct Vector3 v;` names the same type as `Vector3 v;`. These were the last
    unparsed declarations in include/, and one of them silences mismatch reporting
    for every field after it -- so a few of these hid the gate on 8 headers."""
    with tempfile.TemporaryDirectory() as td:
        out = _check(td, "struct H {\n"
                         "    u32 a;                  /* 0x000 */\n"
                         "    struct Vector3 v;       /* 0x004 */\n"
                         "    u32 b;                  /* 0x010 */\n"
                         "};\n")
    assert "0 unparsed" in out and "0 mismatched" in out, out


def test_class_sizes_are_found_in_include_subdirectories():
    """Matrix4x3 asserts its size in include/math/, so the non-recursive scan missed
    exactly the type Model.h and three others embed."""
    with tempfile.TemporaryDirectory() as td:
        out = _check(td, "struct H {\n"
                         "    struct Matrix4x3 m;     /* 0x000 */\n"
                         "    u32 after;              /* 0x030 */\n"
                         "};\n")
    assert "0 unparsed" in out and "0 mismatched" in out, out


def test_an_elaborated_unknown_tag_is_still_unparsed():
    """The keyword makes the shape parseable, not the size. Guessing a width here
    would put every later field at a wrong offset and still report 0 mismatched."""
    with tempfile.TemporaryDirectory() as td:
        out = _check(td, "struct H {\n"
                         "    u32 a;                  /* 0x000 */\n"
                         "    struct NoSuchType t;    /* 0x004 */\n"
                         "};\n")
    assert "UNPARSED" in out, out


def test_long_long_is_four_aligned_on_this_target():
    """mwccarm 2004/b56 -proc arm946e: `struct { char c; long long v; }` is 12 bytes,
    not 16. Self-aligning s64 to 8 invented padding and reported include/ClsnResult.h
    as broken when it is correct."""
    with tempfile.TemporaryDirectory() as td:
        out = _check(td, "struct H {\n"
                         "    u32 a;                  /* 0x000 */\n"
                         "    s64 v;                  /* 0x004 */\n"
                         "    u32 b;                  /* 0x00c */\n"
                         "};\n")
    assert "0 mismatched" in out and "0 unparsed" in out, out


if __name__ == "__main__":
    fails = 0
    for nm, fn in sorted(globals().items()):
        if nm.startswith("test_") and callable(fn):
            try:
                fn()
                print(f"  PASS  {nm}")
            except AssertionError as e:
                fails += 1
                print(f"  FAIL  {nm}: {e}")
            except Exception as e:                    # noqa: BLE001
                fails += 1
                print(f"  ERROR {nm}: {type(e).__name__}: {e}")
    print(f"\n{fails} failure(s)")
    sys.exit(1 if fails else 0)
