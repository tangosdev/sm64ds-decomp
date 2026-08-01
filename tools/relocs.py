"""Module-aware relocation and symbol resolution.

Relocation records identify both a destination address and a destination module.
Overlay modules reuse address ranges, so resolving by address alone can choose a
symbol from the wrong overlay.  The public resolver therefore uses a canonical
``(module, address)`` symbol index and helpers that consume the module field from
each relocation record.
"""
import argparse
import json
import pathlib
import re

REPO = pathlib.Path(__file__).resolve().parent.parent
CFG = REPO / "config" / "arm9"
RELOCS = CFG / "relocs.txt"
SYMS = CFG / "symbols.txt"
ITCM_RELOCS = CFG / "itcm" / "relocs.txt"
ITCM_SYMS = CFG / "itcm" / "symbols.txt"
DTCM_RELOCS = CFG / "dtcm" / "relocs.txt"
DTCM_SYMS = CFG / "dtcm" / "symbols.txt"
OVERLAYS = CFG / "overlays"

_REL_RE = re.compile(r"from:0x([0-9a-fA-F]+) kind:(\S+) to:0x([0-9a-fA-F]+) module:(\S+)")
_SYM_RE = re.compile(r"^(\S+)\s+kind:\S+\s+addr:0x([0-9a-fA-F]+)")


SymbolIndex = dict[tuple[str, int], str]


def normalize_module(module: str) -> str:
    """Canonical module IDs used by tooling and ledgers.

    Config relocation files use spellings such as ``main`` and ``overlay(2)``;
    tools and ledgers mostly use ``arm9`` and ``ov002``.  Normalize these in one
    place so symbol lookup can key on ``(module, addr)`` consistently.
    """
    m = str(module).strip()
    if m in {"main", "arm9"}:
        return "arm9"
    if m in {"itcm", "dtcm"}:
        return m
    overlay = re.fullmatch(r"overlay\((\d+)\)", m)
    if overlay:
        return f"ov{int(overlay.group(1)):03d}"
    overlay = re.fullmatch(r"ov(\d+)", m)
    if overlay:
        return f"ov{int(overlay.group(1)):03d}"
    return m


def iter_symbol_files(include_itcm_dtcm: bool = True):
    """Yield ``(module, symbols_path)`` for main, overlays, and optionally RAM."""
    yield "arm9", SYMS
    if include_itcm_dtcm:
        yield "itcm", ITCM_SYMS
        yield "dtcm", DTCM_SYMS
    if OVERLAYS.is_dir():
        for path in sorted(OVERLAYS.glob("ov*/symbols.txt")):
            yield normalize_module(path.parent.name), path


def iter_reloc_files(include_itcm_dtcm: bool = True):
    """Yield ``(module, relocs_path)`` for main, overlays, and optionally RAM."""
    yield "arm9", RELOCS
    if include_itcm_dtcm:
        yield "itcm", ITCM_RELOCS
        yield "dtcm", DTCM_RELOCS
    if OVERLAYS.is_dir():
        for path in sorted(OVERLAYS.glob("ov*/relocs.txt")):
            yield normalize_module(path.parent.name), path


def load_relocs_file(path):
    """{from_addr: (kind, to_addr, to_module)} for one relocs.txt."""
    d = {}
    for line in pathlib.Path(path).read_text(errors="ignore").splitlines():
        m = _REL_RE.match(line)
        if m:
            d[int(m.group(1), 16)] = (m.group(2), int(m.group(3), 16),
                                      normalize_module(m.group(4)))
    return d


def load_syms_file(path, module: str | None = None):
    """Load one symbols.txt file.

    Without ``module`` this keeps the historical flat ``{addr: name}`` behavior
    for main-only callers.  With ``module`` it returns ``{(module, addr): name}``.
    """
    d = {}
    key_module = normalize_module(module) if module is not None else None
    for line in pathlib.Path(path).read_text(errors="ignore").splitlines():
        m = _SYM_RE.match(line)
        if m:
            addr = int(m.group(2), 16)
            d[(key_module, addr) if key_module is not None else addr] = m.group(1)
    return d


def iter_syms_pairs(path, module: str | None = None):
    """Yield ``(name, (module, addr))`` for every symbols.txt line, without keying by
    address, so aliases (two names at one address -- a guessed vtable name and its
    RTTI-recovered real name) are both preserved instead of one overwriting the other."""
    key_module = normalize_module(module) if module is not None else None
    for line in pathlib.Path(path).read_text(errors="ignore").splitlines():
        m = _SYM_RE.match(line)
        if m:
            yield m.group(1), (key_module, int(m.group(2), 16))


def load_relocs():
    return load_relocs_file(RELOCS)


def load_syms():
    """Legacy main-only flat ``addr -> name`` symbol map."""
    return load_syms_file(SYMS)


def load_all_syms() -> SymbolIndex:
    """Merged ``(module, addr) -> name`` index for main, overlays, ITCM, and DTCM.

    This intentionally reads the checked-in config files directly instead of the
    extracted overlay manifest, so pure tooling/tests can run without a local ROM.
    """
    d: SymbolIndex = {}
    for module, path in iter_symbol_files(include_itcm_dtcm=True):
        d.update(load_syms_file(path, module))
    return d


def _fallback_name(addr: int) -> str:
    return f"func_{addr:08x}"


def name_of(module_or_addr, addr_or_syms, syms=None) -> str:
    """Symbol name for an address.

    Preferred usage is ``name_of(module, addr, symbol_index)`` with the module from
    a relocation record.  The old ``name_of(addr, flat_syms)`` form is retained for
    compatibility with main-only utilities.
    """
    if syms is None:
        addr = module_or_addr
        flat_syms = addr_or_syms
        return flat_syms.get(addr, _fallback_name(addr))

    module = normalize_module(module_or_addr)
    addr = addr_or_syms
    return syms.get((module, addr), syms.get(addr, _fallback_name(addr)))


def name_for_reloc(reloc, syms: SymbolIndex) -> str:
    """Symbol name for a ``(kind, to_addr, to_module)`` relocation tuple."""
    return name_of(reloc[2], reloc[1], syms)


def _flat_main_overlay_syms():
    """Recreate the old flat main+overlay lookup for audit comparisons."""
    flat = {}
    owner = {}
    for module, path in iter_symbol_files(include_itcm_dtcm=False):
        for addr, name in load_syms_file(path).items():
            flat[addr] = name
            owner[addr] = module
    return flat, owner


def audit_symbol_resolution(example_limit: int = 10):
    """Compare module-aware resolution with the historical flat lookup."""
    expected = load_all_syms()
    flat, flat_owner = _flat_main_overlay_syms()
    result = {
        "relocations": 0,
        "known_destinations": 0,
        "wrong_flat_substitutions": 0,
        "missing_from_flat": 0,
        "unresolved_destinations": 0,
        "examples": {"wrong": [], "missing": []},
    }
    for source_module, path in iter_reloc_files(include_itcm_dtcm=True):
        for from_addr, (kind, to_addr, to_module) in load_relocs_file(path).items():
            result["relocations"] += 1
            correct = expected.get((to_module, to_addr))
            if correct is None:
                result["unresolved_destinations"] += 1
                continue
            result["known_destinations"] += 1
            flat_name = flat.get(to_addr)
            if flat_name is None:
                result["missing_from_flat"] += 1
                if len(result["examples"]["missing"]) < example_limit:
                    result["examples"]["missing"].append({
                        "source_module": source_module,
                        "from": f"0x{from_addr:08x}",
                        "kind": kind,
                        "to_module": to_module,
                        "to": f"0x{to_addr:08x}",
                        "correct": correct,
                    })
            elif flat_name != correct:
                result["wrong_flat_substitutions"] += 1
                if len(result["examples"]["wrong"]) < example_limit:
                    result["examples"]["wrong"].append({
                        "source_module": source_module,
                        "from": f"0x{from_addr:08x}",
                        "kind": kind,
                        "to_module": to_module,
                        "to": f"0x{to_addr:08x}",
                        "correct": correct,
                        "flat_module": flat_owner[to_addr],
                        "flat": flat_name,
                    })
    return result


def main():
    ap = argparse.ArgumentParser(description="Module-aware relocation/symbol helpers")
    ap.add_argument("--audit", action="store_true",
                    help="report known destinations misresolved by the old flat lookup")
    ap.add_argument("--json", action="store_true", help="emit audit as JSON")
    args = ap.parse_args()

    if not args.audit:
        ap.print_help()
        return

    result = audit_symbol_resolution()
    if args.json:
        print(json.dumps(result, indent=2))
        return
    print("module-aware relocation symbol audit")
    for key in ("relocations", "known_destinations", "wrong_flat_substitutions",
                "missing_from_flat", "unresolved_destinations"):
        print(f"  {key}: {result[key]}")
    if result["examples"]["wrong"]:
        print("\nexamples: wrong flat substitution")
        for ex in result["examples"]["wrong"]:
            print(f"  {ex['source_module']} {ex['from']} -> {ex['to_module']}:{ex['to']} "
                  f"correct={ex['correct']} flat={ex['flat_module']}:{ex['flat']}")
    if result["examples"]["missing"]:
        print("\nexamples: missing from old flat lookup")
        for ex in result["examples"]["missing"]:
            print(f"  {ex['source_module']} {ex['from']} -> {ex['to_module']}:{ex['to']} "
                  f"correct={ex['correct']}")


if __name__ == "__main__":
    main()
