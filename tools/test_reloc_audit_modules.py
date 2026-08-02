"""The strict-reloc gate must refuse a module ID it does not recognise.

Regression test for a silent false pass: check_destinations looked its module up
with `config_relocs.get(normalize_module(mod), {})`, so a misspelled module
(`arm9/itcm` for `itcm` -- the config *path* rather than the module ID) produced an
empty map, classified every relocation against cfg=None, returned no WRONG-DEST
rows, and let match.py --strict-relocs report a verified match. A deliberately
wrong callee passed that way.

These tests need no compiler and no ROM: the module check runs before the object
is parsed, which is also why an empty object is a valid argument here.
"""
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import reloc_audit as RA  # noqa: E402


def _check(module):
    """check_destinations with everything but the module stubbed out."""
    return RA.check_destinations(b"", "some_symbol", 0x01ffd920, 0x5c, module,
                                 name_index={}, config_relocs={}, sym_index={})


def test_known_modules_covers_the_real_ones():
    known = RA.known_modules()
    assert "arm9" in known
    assert "itcm" in known
    assert "dtcm" in known
    assert any(m.startswith("ov") for m in known)


def test_path_style_module_is_rejected():
    """The exact spelling that caused the false pass."""
    rows, reason = _check("arm9/itcm")
    assert rows is None
    assert "unknown module" in reason
    assert "arm9/itcm" in reason


def test_other_unknown_spellings_are_rejected():
    for bad in ("arm9/overlays/ov002", "bogus", "overlay2", ""):
        rows, reason = _check(bad)
        assert rows is None, f"{bad!r} should be rejected"
        assert "unknown module" in reason


def test_known_modules_get_past_the_module_check():
    """A real module ID must NOT be rejected for being unknown. It then reaches the
    object parse, which on our empty object either raises or returns some other
    reason -- both prove the module check let it through."""
    for good in ("itcm", "arm9", "main", "ov002", "overlay(2)"):
        try:
            rows, reason = _check(good)
        except Exception:
            continue        # got past the module check and into the ELF parse
        assert rows is None, f"{good!r} unexpectedly parsed an empty object"
        assert "unknown module" not in reason, f"{good!r} was wrongly rejected: {reason}"


def test_gate_wrong_dests_signals_cannot_check_with_none():
    """The contract the three call sites depend on: None means "did not check",
    [] means "checked, all destinations agree". They are not interchangeable, and
    None is falsy -- which is how the false pass got into all three."""
    assert RA.gate_wrong_dests(b"", "some_symbol", 0x01ffd920, 0x5c, "arm9/itcm") is None


def test_no_caller_treats_a_none_gate_result_as_clean():
    """Guard against the defect reappearing: every `gate_wrong_dests` call site must
    test `is None` before the plain truthiness check."""
    import re
    root = pathlib.Path(__file__).resolve().parent
    for path in sorted(root.glob("*.py")):
        if path.name.startswith("test_") or path.name == "reloc_audit.py":
            continue
        text = path.read_text(encoding="utf-8", errors="ignore")
        if "gate_wrong_dests" not in text:
            continue
        assert re.search(r"\bbad\s+is\s+None\b", text), (
            f"{path.name} calls gate_wrong_dests but never checks `bad is None`; "
            f"a None result would be treated as 'no wrong destinations'")
