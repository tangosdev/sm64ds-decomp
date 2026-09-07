"""Tests for tools/fieldmap.py.

Two of these are regressions for bugs the tool actually had while it was being
used on the Player migration -- see test_address_take_keeps_parens and
test_signedness_difference_is_kept_visible.
"""
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import fieldmap  # noqa: E402

HEADER = """\
struct Thing {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x2];
    u16 mTimer;            /* 0x092 */
    u8  mFlagByte;            /* 0x094 */
};
"""

DERIVED = """\
struct Sub : Thing {
    s32 mOwn;            /* 0x100 */
};
"""


def _fields(tmp_path, text=HEADER, name="Thing"):
    inc = tmp_path / "include"
    inc.mkdir(exist_ok=True)
    (inc / f"{name}.h").write_text(text)
    return fieldmap.fields_for(name, repo=tmp_path)


def test_parses_commented_fields_and_skips_padding(tmp_path):
    fields, chain = _fields(tmp_path)
    assert chain == ["Thing"]
    assert fields[0x5c] == ("mPosX", "s32")
    assert fields[0x8e] == ("mAngleY", "s16")
    # padding is absence of knowledge, not a field
    assert all(not n.startswith("pad_") for n, _ in fields.values())


def test_derived_class_inherits_base_fields(tmp_path):
    inc = tmp_path / "include"
    inc.mkdir()
    (inc / "Thing.h").write_text(HEADER)
    (inc / "Sub.h").write_text(DERIVED)
    fields, chain = fieldmap.fields_for("Sub", repo=tmp_path)
    assert chain == ["Sub", "Thing"]
    assert fields[0x100] == ("mOwn", "s32")     # its own
    assert fields[0x5c] == ("mPosX", "s32")     # inherited


def test_matching_width_and_sign_becomes_a_plain_name(tmp_path):
    fields, _ = _fields(tmp_path)
    out, skipped = fieldmap.substitute("x = *(s32 *)(c + 0x5c);", fields, "c")
    assert out == "x = mPosX;"
    assert skipped == []


def test_signedness_difference_is_kept_visible(tmp_path):
    """An unsigned read of a signed field is an ldrh where the field's own type
    gives ldrsh. That is one instruction, and the tool must not silently pick
    either side -- it keeps the cast so a human decides whether the header is
    wrong or the cast is the semantics."""
    fields, _ = _fields(tmp_path)
    out, skipped = fieldmap.substitute("i = *(u16 *)(c + 0x8e);", fields, "c")
    assert out == "i = *(u16*)&mAngleY;"
    assert skipped == []


def test_width_mismatch_is_refused_and_reported(tmp_path):
    """Reading four bytes from a slot declared u8 means the header is probably
    wrong. Renaming it anyway would bury that."""
    fields, _ = _fields(tmp_path)
    src = "v = *(int *)(c + 0x94);"
    out, skipped = fieldmap.substitute(src, fields, "c")
    assert out == src
    assert any("width differs" in s for s in skipped)


def test_refused_deref_is_not_then_rewritten_as_an_address(tmp_path):
    """Regression. The two rules ran as separate passes, so a site the width
    check had just refused was picked up by the address-take rule and rewritten
    anyway -- `*(int *)(c + 0x94)` became `*(int *)(&mFlagByte)`, silently
    doing the thing the tool had declined to do."""
    fields, _ = _fields(tmp_path)
    src = "v = *(int *)(c + 0x94);"
    out, _ = fieldmap.substitute(src, fields, "c")
    assert out == src
    assert "&mFlagByte" not in out


def test_unnamed_offset_is_refused_and_reported(tmp_path):
    """The one place a mechanical substitution can silently land on the wrong
    field."""
    fields, _ = _fields(tmp_path)
    src = "v = *(s32 *)(c + 0x777);"
    out, skipped = fieldmap.substitute(src, fields, "c")
    assert out == src
    assert any("0x777" in s for s in skipped)


def test_address_take_keeps_parens(tmp_path):
    """Regression. The parentheses around `c + 0xNN` are not always a grouping:
    in `foo(c + 0x5c)` they are the call's argument list. Consuming them
    produced `foo&mPosX`, which still parsed far enough to give a confusing
    type error a hundred lines away."""
    fields, _ = _fields(tmp_path)
    out, _ = fieldmap.substitute("foo(c + 0x5c);", fields, "c")
    assert out == "foo(&mPosX);"
    assert "foo&" not in out


def test_report_separates_named_from_unnamed(tmp_path):
    fields, _ = _fields(tmp_path)
    named, unnamed = fieldmap.report(
        "a = *(s32*)(c + 0x5c); b = *(s32*)(c + 0x777);", fields, "c")
    assert named == [0x5c]
    assert unnamed == [0x777]


def test_receiver_name_is_respected(tmp_path):
    """Files use `c`, `self`, `thiz` and `this_`; a pass for one receiver must
    not silently claim to have handled another. (Player::InitResources has two
    in the same file.)"""
    fields, _ = _fields(tmp_path)
    src = "a = *(s32 *)(self + 0x5c); b = *(s32 *)(c + 0x5c);"
    out, _ = fieldmap.substitute(src, fields, "self")
    assert out == "a = mPosX; b = *(s32 *)(c + 0x5c);"
