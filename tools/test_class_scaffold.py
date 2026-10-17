"""Tests for tools/class_scaffold.py, tools/units.py and the UNIT-VERDICT line.

The scaffold test needs the extracted ROM and build/rtti.json (tools/rtti_extract.py);
without them it SKIPS and says why, it does not pass."""
import json
import pathlib
import sys

import pytest

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import tubuild as TB  # noqa: E402
import units as U  # noqa: E402


def test_unit_verdict_is_one_space_separated_line_with_a_fixed_prefix():
    line = TB.unit_verdict("tubuild.verify", "arm9/ModelAnim2", "text-verified",
                           bytes="4/4", reason="has a space", exit=0)
    assert "\n" not in line
    assert line.startswith("UNIT-VERDICT tool=tubuild.verify id=arm9/ModelAnim2 "
                           "result=text-verified ")
    fields = dict(part.split("=", 1) for part in line.split()[1:])
    assert fields["bytes"] == "4/4"
    assert fields["reason"] == "has_a_space"
    assert fields["exit"] == "0"


@pytest.mark.parametrize("statuses,want", [
    ([None, None], "none"),
    (["promoted", "promoted"], "promoted"),
    (["promoted", None], "partial"),
    (["shadow", "text-verified"], "text-verified"),
    (["link-verified", None], "link-verified"),
])
def test_state_folds_member_statuses(statuses, want):
    assert U.state_of(statuses) == want


def _have_inputs():
    return (REPO / "build" / "rtti.json").is_file() and \
        (REPO / "extracted" / "arm9_dec.bin").is_file()


def test_scaffold_reproduces_the_hand_made_dabar_manifest_shape():
    if not _have_inputs():
        pytest.skip("needs extracted/ and build/rtti.json (python tools/rtti_extract.py)")
    import class_scaffold as CS
    plan, entry = CS.build_plan("daBar_c", out=str(REPO / "build" / "scaffold" / "_test"))
    hand = json.loads((REPO / "config" / "tu_manifest.d" / "ov002" / "daBar_c.json")
                      .read_text(encoding="utf-8"))
    assert entry["promoted_source"] == hand["promoted_source"]
    assert entry["sections"] == hand["sections"]
    assert [(f["symbol"], f["address"], f["size"]) for f in entry["functions"]] == \
        [(f["symbol"], f["address"], f["size"]) for f in hand["functions"]]
    # The ROM vtable, not build/rtti_vtables.json: 31 slots, InitResources first.
    assert len(plan["vtable"]) == 31
    assert plan["vtable"][0]["symbol"] == "_ZN7daBar_c13InitResourcesEv"


def test_scalar_fix12_mangling_rewrites_only_the_simple_shape():
    import class_scaffold as CS
    assert CS.scalar_fix12_mangling("_ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt") == \
        "_ZN10ModelAnim213Func_020162C4Ejiit"
    # S1_ refers back to Fix12<int>: renumbering is mangle.py's job, not ours.
    assert CS.scalar_fix12_mangling("_ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_") is None
    assert CS.scalar_fix12_mangling("_ZN7daBar_c6RenderEv") is None


def test_scaffold_output_passes_the_source_style_patterns():
    """No unk_<hex> field, no Allman brace, a 6az note on a by-value Fix12 member
    (the three things source style flagged on the first scaffold-built TU, #4003)."""
    if not _have_inputs():
        pytest.skip("needs extracted/ and build/rtti.json (python tools/rtti_extract.py)")
    import re
    import class_scaffold as CS
    for name, kw in (("daBar_c", {}), ("dExtAnm2Model_c", {"exclude_lifecycle": True})):
        plan, _entry = CS.build_plan(name, out=str(REPO / "build" / "scaffold" / "_test"), **kw)
        for path, text in plan["files"].items():
            if path.endswith(".json"):
                continue
            assert not re.search(r"\bunk_[0-9a-fA-F]+", text), path
            assert not re.search(r"\)\s*\n\{", text), path
        if name == "dExtAnm2Model_c":
            tu = next(t for p, t in plan["files"].items() if p.endswith(".cpp"))
            assert "WALL 6az" in tu and "_ZN10ModelAnim213Func_020162C4Ejiit" in tu


@pytest.mark.parametrize("param", ["R5Fix12IiE", "RK5Fix12IiE", "P5Fix12IiE"])
def test_fix12_reference_and_pointer_parameters_do_not_hit_the_value_wall(param):
    import class_scaffold as CS
    symbol = "_ZN7daBar_c4TestE" + param
    assert CS.scalar_fix12_mangling(symbol) is None
    assert CS.fix12_wall_note(symbol) == []


def test_fix12_scalar_suggestion_never_retypes_a_reference_in_a_mixed_signature():
    import class_scaffold as CS
    symbol = "_ZN7daBar_c4TestER5Fix12IiE5Fix12IiE"
    assert CS.scalar_fix12_mangling(symbol) is None
    assert "Ask tools/mangle.py" in "\n".join(CS.fix12_wall_note(symbol))
