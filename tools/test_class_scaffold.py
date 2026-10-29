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


def test_dry_run_plan_points_at_the_class_form_note(capsys):
    """The scaffold's plan output ends by sending the writer to notes/cpp-class-form.md,
    the measured destructor / vtable / struct-copy source forms (no inputs needed)."""
    import class_scaffold as CS
    plan = {"class": "daBar_c", "tree_name": "daBar_c", "module": "ov002",
            "record": {"kind": "class", "mangled": "7daBar_c", "addr": "0x0", "ztv": None,
                       "vtable": "0x0"},
            "bases": [], "size": None, "vtable": [], "base_slots": 0, "members": [],
            "lifecycle": "none", "warnings": [], "already_in_manifest": [],
            "destination": None, "destination_reason": "-", "existing_header": None,
            "files": []}
    CS.print_plan(plan)
    out = capsys.readouterr().out.rstrip().splitlines()
    assert out[-1].startswith("class form ")
    assert "notes/cpp-class-form.md" in out[-1]
    assert (REPO / "notes" / "cpp-class-form.md").is_file()


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
    for name, kw in (("daBar_c", {}), ("dExtAnm2Model_c", {"exclude_lifecycle": True}),
                     ("dCcAc_c", {})):
        plan, _entry = CS.build_plan(name, out=str(REPO / "build" / "scaffold" / "_test"), **kw)
        for path, text in plan["files"].items():
            if path.endswith(".json"):
                continue
            assert not re.search(r"\bunk_[0-9a-fA-F]+", text), path
            assert not re.search(r"\)\s*\n\{", text), path
        tu_text = next(t for p, t in plan["files"].items() if p.endswith(".cpp"))
        assert "notes/cpp-class-form.md" in tu_text
        # dExtAnm2Model_c::Func_020162C4 moved off the by-value Fix12 mangling
        # (#4311), so its scaffold carries the scalar symbol and no wall note;
        # dCcAc_c::Init still takes a Fix12<int> by value and must get one.
        if name == "dExtAnm2Model_c":
            assert "_ZN15dExtAnm2Model_c13Func_020162C4Ejiit" in tu_text
        if name == "dCcAc_c":
            assert "WALL 6az" in tu_text


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


def _tu_map_with(tmp_path, fingerprint):
    meta = {"under_segmented": []}
    if fingerprint is not None:
        meta["rtti_vtables_fingerprint"] = fingerprint
    path = tmp_path / "tu_map.json"
    path.write_text(json.dumps({"meta": meta, "modules": {"ov002": {"units": [
        {"start": "0x02001000", "end": "0x02001100"}]}}}), encoding="utf-8")
    return path


def test_tu_unit_refuses_a_map_built_from_other_rom_images(tmp_path, monkeypatch):
    import class_scaffold as CS
    import rom_fingerprint as RF
    monkeypatch.setattr(CS, "TU_MAP", _tu_map_with(
        tmp_path, {"scheme": RF.SCHEME, "sha256": "0" * 64}))
    monkeypatch.setattr(RF, "fingerprint",
                        lambda repo=None: {"scheme": RF.SCHEME, "sha256": "1" * 64})
    with pytest.raises(SystemExit, match="different ROM images"):
        CS.tu_unit("ov002", [0x02001010])


def test_tu_unit_accepts_a_matching_map_and_warns_on_an_old_one(tmp_path, monkeypatch, capsys):
    import class_scaffold as CS
    import rom_fingerprint as RF
    here = {"scheme": RF.SCHEME, "sha256": "1" * 64}
    monkeypatch.setattr(RF, "fingerprint", lambda repo=None: here)
    monkeypatch.setattr(CS, "TU_MAP", _tu_map_with(tmp_path, dict(here)))
    unit, _under = CS.tu_unit("ov002", [0x02001010])
    assert unit["start"] == "0x02001000"
    assert capsys.readouterr().err == ""
    RF._warned.clear()
    monkeypatch.setattr(CS, "TU_MAP", _tu_map_with(tmp_path, None))
    unit, _under = CS.tu_unit("ov002", [0x02001010])
    assert unit["start"] == "0x02001000"      # warned, not refused
    assert "no ROM fingerprint" in capsys.readouterr().err
