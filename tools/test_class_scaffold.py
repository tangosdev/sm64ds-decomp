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
