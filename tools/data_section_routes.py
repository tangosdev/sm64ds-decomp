"""Route complete native initialized input sections to explicit module slices.

Only section-header names change. The ordinary TU byte, extent, relocation and
provider audits remain responsible for proving the resulting complete claims.
Named slices do not license gaps or infer compiler padding.
"""
import io
import re
import struct


_NAMED = re.compile(r"^(\.data|\.rodata)\.[A-Za-z][A-Za-z0-9_]*$")
_FIELDS = {".data": "data", ".rodata": "rodata"}


def initialized_kind(name):
    if not isinstance(name, str):
        return None
    if name in _FIELDS:
        return name
    match = _NAMED.fullmatch(name)
    return match.group(1) if match else None


def _integer(value):
    if isinstance(value, bool) or not isinstance(value, (str, int)):
        raise ValueError("expected integer or hexadecimal string")
    return int(value, 0) if isinstance(value, str) else value


def routes(entry):
    """Return validated named-slice claims; ordinary claims are untouched."""
    result, errors, names = [], [], set()
    claims = entry.get("sections", [])
    if not isinstance(claims, list):
        return [], ["section claims must be a list"]
    for row in claims:
        if not isinstance(row, dict):
            continue
        name, origin = row.get("name"), row.get("input_section")
        named = isinstance(name, str) and _NAMED.fullmatch(name)
        if origin is None and not named:
            continue
        label = f"initialized section route {name!r}"
        if not named or origin != initialized_kind(name):
            errors.append(f"{label} requires a matching .data or .rodata input_section")
            continue
        if row.get("module_section", name) != name:
            errors.append(f"{label} must use its own named module section")
            continue
        try:
            start, end = _integer(row["start"]), _integer(row["end"])
            if not 0 <= start < end <= 0x100000000:
                raise ValueError("invalid 32-bit interval")
        except (KeyError, ValueError, TypeError) as exc:
            errors.append(f"{label}: {exc}")
            continue
        if name in names:
            errors.append(f"duplicate initialized section route {name}")
            continue
        names.add(name)
        result.append({"name": name, "input_section": origin,
                       "start": start, "end": end})
    for left, right in zip(sorted(result, key=lambda r: r["start"]),
                           sorted(result, key=lambda r: r["start"])[1:]):
        if left["end"] > right["start"]:
            errors.append(f"initialized routes {left['name']} and {right['name']} overlap")
    return result, errors


def _extent(row):
    start = _integer(row.get("emitted_storage_address",
                             row.get("address", row.get("rom_symbol_address"))))
    size = _integer(row["size"])
    if size <= 0 or not 0 <= start < start + size <= 0x100000000:
        raise ValueError("invalid object extent")
    return start, start + size


def owned_section(entry, section, row, routed=None):
    """Select a named slice only from the object's complete declared extent.

    ``routed`` is ``routes(entry)``, for callers asking about many rows of one entry.
    Recomputing it per row made those callers quadratic: ov007's 443 routes cost
    func_ov007_020bbe60.c two minutes of every ROM build.
    """
    declared, errors = routed if routed is not None else routes(entry)
    if errors or section not in _FIELDS:
        return section
    try:
        start, end = _extent(row)
    except (ValueError, TypeError, KeyError):
        return section
    homes = [r["name"] for r in declared if r["input_section"] == section
             and r["start"] <= start and end <= r["end"]]
    return homes[0] if len(homes) == 1 else section


def apply(raw, entry):
    """Rename complete, uniquely anchored PROGBITS sections without splitting.

    The manifest must license every non-section symbol occupying a routed input
    section, and those objects must cover its full extent. Their declared addresses
    must imply one common section base. A partial/ambiguous input section is refused,
    even when the requested subset happens to have the expected bytes.
    """
    declared, errors = routes(entry)
    report = {"routes": [], "stringTable": None}
    if errors:
        return None, report, errors
    if not declared:
        return raw, report, []
    from elftools.elf.elffile import ELFFile
    import objisolate as OI

    try:
        elf = ELFFile(io.BytesIO(raw))
        sections = list(elf.iter_sections())
        symtab = elf.get_section_by_name(".symtab")
        if symtab is None:
            raise ValueError("no .symtab")
        symbols = list(symtab.iter_symbols())
    except Exception as exc:
        return None, report, [f"initialized section routing cannot parse ELF: {exc}"]

    assignments = {}
    by_name = {}
    for symbol in symbols:
        if isinstance(symbol["st_shndx"], int):
            by_name.setdefault(symbol.name, []).append(symbol)
    for route in declared:
        target, origin = route["name"], route["input_section"]
        objects = entry.get(_FIELDS[origin], [])
        if not isinstance(objects, list):
            errors.append(f"{target} owned objects must be a list")
            continue
        rows = [r for r in objects if isinstance(r, dict)
                and owned_section(entry, origin, r, (declared, [])) == target]
        if not rows:
            errors.append(f"{target} has no complete manifest-owned objects")
            continue
        anchors = {}
        licensed = set()
        for row in rows:
            name = row.get("symbol")
            if not isinstance(name, str) or not name or name in licensed:
                errors.append(f"{target} has a missing or duplicate anchor symbol")
                continue
            definitions = by_name.get(name, [])
            if len(definitions) != 1:
                errors.append(f"{target} anchor {name!r} has {len(definitions)} definitions")
                continue
            symbol = definitions[0]
            index = symbol["st_shndx"]
            if not 0 < index < len(sections):
                errors.append(f"{target} anchor {name!r} has an invalid section index")
                continue
            section = sections[index]
            if (symbol["st_info"]["type"] != "STT_OBJECT"
                    or section["sh_type"] != "SHT_PROGBITS"
                    or not section["sh_flags"] & 2
                    or section["sh_flags"] & 4
                    or section.name not in (origin, target)):
                errors.append(f"{target} anchor {name!r} is not in its native initialized section")
                continue
            start, end = _extent(row)
            if symbol["st_size"] != end - start or symbol["st_size"] <= 0:
                errors.append(f"{target} anchor {name!r} size disagrees with native object")
                continue
            if index in assignments and assignments[index] != target:
                errors.append(f"native section[{index}] is split between initialized routes")
                continue
            assignments[index] = target
            anchors.setdefault(index, []).append((name, start, symbol))
            licensed.add(name)

        for index, objects in anchors.items():
            section = sections[index]
            occupants = [s for s in symbols if s["st_shndx"] == index
                         and s.name and not s.name.startswith("$")
                         and s["st_info"]["type"] != "STT_SECTION"]
            foreign = sorted(s.name for s in occupants if s.name not in licensed)
            if foreign:
                errors.append(f"{target} native section[{index}] has unlicensed occupants: {foreign}")
            bases = {address - s["st_value"] for _, address, s in objects}
            if len(bases) != 1:
                errors.append(f"{target} native section[{index}] anchors disagree on its address")
                continue
            base = next(iter(bases))
            if not route["start"] <= base < base + section["sh_size"] <= route["end"]:
                errors.append(f"{target} would claim only part of native section[{index}]")
            spans = sorted((s["st_value"], s["st_value"] + s["st_size"])
                           for _, _, s in objects)
            cursor = 0
            for start, end in spans:
                if start > cursor:
                    errors.append(f"{target} native section[{index}] has unowned interior bytes")
                cursor = max(cursor, end)
            if cursor != section["sh_size"]:
                errors.append(f"{target} native section[{index}] has unowned trailing bytes")
            report["routes"].append({"sectionIndex": index, "from": section.name,
                                     "to": target, "address": hex(base),
                                     "bytes": section["sh_size"],
                                     "symbols": sorted(name for name, _, _ in objects)})
        existing = {i for i, s in enumerate(sections)
                    if s.name == target and s["sh_size"]}
        if existing - set(anchors):
            errors.append(f"{target} already contains unanchored input sections")
    if errors:
        return None, report, errors

    changes = [(i, target) for i, target in sorted(assignments.items())
               if sections[i].name != target]
    if not changes:
        return raw, report, []
    grown, offsets, growth = OI._grow_string_table(
        raw, elf["e_shstrndx"], [name.encode("ascii") for _, name in changes])
    report["stringTable"] = growth
    if grown is None:
        return None, report, [growth.get("error", "section-name string-table growth failed")]
    rewritten = ELFFile(io.BytesIO(grown))
    out = bytearray(grown)
    endian = "<" if rewritten.little_endian else ">"
    for (index, _), offset in zip(changes, offsets):
        struct.pack_into(endian + "I", out, OI._shdr_offset(rewritten, index), offset)
    return bytes(out), report, []
