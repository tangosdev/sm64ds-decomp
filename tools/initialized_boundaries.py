"""Explicit initialized linker fill and native vtable storage identity proofs."""
import io
import hashlib
import json
import pathlib
import re
import struct
import relocs as RL

import data_section_routes as DSR
import linker_alignment as LA
from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection
from elftools.common.exceptions import ELFError


def integer(value):
    if isinstance(value, bool) or not isinstance(value, (int, str)):
        raise ValueError("integer required")
    return int(value, 0) if isinstance(value, str) else value


def terminal_policy(claim):
    """Validate the explicit policy; absence never licenses padding."""
    policy = claim.get("terminal_linker_fill")
    if "terminal_linker_fill" not in claim:
        return None
    if (
        DSR.initialized_kind(claim.get("name")) is None
        or claim.get("module_section", claim["name"]) != claim["name"]
    ):
        raise ValueError("terminal fill requires initialized same-name section")
    if (
        not isinstance(policy, dict)
        or set(policy) != {"alignment", "value"}
        or type(policy["alignment"]) is not int
        or policy["alignment"] != 4
        or type(policy["value"]) is not int
        or policy["value"] != 0
    ):
        raise ValueError("terminal fill requires exact alignment4/value0 policy")
    start, end = integer(claim["start"]), integer(claim["end"])
    if not 0 <= start < end <= 0x100000000 or start % 4 or end % 4:
        raise ValueError("terminal fill requires aligned bounded claim")
    return dict(alignment=4, value=0)


def terminal_extent(claim, emitted_end):
    if terminal_policy(claim) is None:
        raise ValueError("no explicit terminal fill policy")
    start, end = integer(claim["start"]), integer(claim["end"])
    amount = end - emitted_end
    if (
        not start < emitted_end < end
        or not 1 <= amount <= 3
        or (emitted_end + 3) & ~3 != end
    ):
        raise ValueError("terminal fill must be1..3 bytes to next ALIGN4")
    return amount


def terminal_placement(claim, module_section):
    """Which input ends the fill: the section-end marker, or the next provider.

    "section-end": the claim owns the whole logical module section, so the fill is
    the section's own terminal ALIGN(4). "interior": the claim starts inside the
    section and ends before its end, so the fill is the ALIGNALL(4) padding that
    the linker inserts before the next input section. That successor must then be
    proven to land exactly at the claim end (verify_terminal_lcf, successor_start).
    A claim that reaches the section end without owning its start is neither.
    """
    if terminal_policy(claim) is None:
        raise ValueError("no explicit terminal fill policy")
    start, end = integer(claim["start"]), integer(claim["end"])
    if module_section.get("name") != claim["name"]:
        raise ValueError("claim must lie in its own logical module section")
    first, last = integer(module_section["start"]), integer(module_section["end"])
    if (first, last) == (start, end):
        return "section-end"
    if first <= start and end < last:
        return "interior"
    raise ValueError(
        "claim must own whole logical module section or end inside it"
    )


def license_terminal_fill(
    claim, module_section, emitted_end, *, configured_symbols, relocations, retail_fill
):
    """Input rows must be complete module-qualified config censuses, not samples.

    Configured symbol rows have address/size; relocation rows have site/size and
    resolved target. The caller owns parsing and exact module-set validation.
    An interior license is only half the proof: the actual link must also show
    the next input section lands at the claim end (link_plan does this).
    """
    placement = terminal_placement(claim, module_section)
    start, end = integer(claim["start"]), integer(claim["end"])
    emitted_end = integer(emitted_end)
    amount = terminal_extent(claim, emitted_end)
    for row in configured_symbols:
        address, size = integer(row["address"]), integer(row.get("size", 0))
        if row.get("unknown_initialized_extent") and start <= address < end:
            # DSD may infer storage through the next configured address or the
            # logical section end. An unsized initialized row is not a zero-size
            # label. Do not license bytes that this possible extent occupies.
            following = [
                integer(other["address"])
                for other in configured_symbols
                if address < integer(other["address"]) <= end
            ]
            size = min(following, default=end) - address
        if (
            size < 0
            or emitted_end <= address < end
            or (address < end and address + size > emitted_end)
        ):
            raise ValueError("configured symbol occupies terminal fill")
    for row in relocations:
        site, size, target = (
            integer(row["site"]),
            integer(row["size"]),
            integer(row["target"]),
        )
        if (
            size <= 0
            or (site < end and site + size > emitted_end)
            or emitted_end <= target < end
        ):
            raise ValueError("relocation occupies or targets terminal fill")
    if retail_fill != bytes(amount):
        raise ValueError("retail terminal fill is not exact zero bytes")
    return dict(
        section=claim["name"],
        start=start,
        emittedEnd=emitted_end,
        end=end,
        emittedBytes=emitted_end - start,
        linkedBytes=end - start,
        linkerFillBytes=amount,
        value=0,
        placement=placement,
    )


def verify_terminal_lcf(path, *, module, section, provider, placement="section-end"):
    """Conservative exact generated-selector grammar; no alternate script dialect.

    "section-end": the provider is the section's sole selector, followed by the
    terminal ALIGN(4) and end marker. "interior": the provider is selected once,
    between the section markers, and the very next statement is another literal
    selector of the same section; the caller must then prove from that object's
    actual section headers that it lands at the claim end (successor_start).
    """
    LA.verify_lcf(path)
    text = pathlib.Path(path).read_text(encoding="utf-8")
    text = re.sub(r"/\*.*?\*/|//[^\n]*", "", text, flags=re.S)
    if not re.fullmatch(r"(?:OV\d{3}|ARM9|ITCM|DTCM)", module):
        raise ValueError("invalid output module")
    if DSR.initialized_kind(section) is None:
        raise ValueError("unsupported initialized section")
    if not re.fullmatch(r"[A-Za-z0-9_+.-]+\.o", provider):
        raise ValueError("invalid provider basename")
    if re.search(r"\bFILL\b", text):
        raise ValueError("explicit FILL changes native zero-fill policy")
    blocks = list(
        re.finditer(r"(?m)^\s*(\.[\w]+)\s*:\s*\{([^{}]*)\}\s*>\s*(\w+)[ \t]*$", text)
    )
    matching = [b for b in blocks if b[3] == module]
    if len(matching) != 1:
        raise ValueError("missing unique output module or unexpected output suffix")
    body = matching[0][2]
    marker = module + "_" + section[1:].upper()
    begin, end = marker + "_START", marker + "_END"
    if (
        len(re.findall(r"\b" + re.escape(begin) + r"\s*=", text)) != 1
        or len(re.findall(r"\b" + re.escape(end) + r"\s*=", text)) != 1
    ):
        raise ValueError("missing unique section markers")
    selector = re.escape(provider) + r"\s*\(\s*" + re.escape(section) + r"\s*\)"
    if len(re.findall(selector, text)) != 1:
        raise ValueError("provider selector must occur exactly once")
    # A different object or wildcard selecting this section outside its markers
    # would still contribute bytes. Count the entire LCF, not only this provider.
    # DSD emits one literal input-section name per object selector. Do not
    # interpret wildcard/list syntax optimistically: an earlier selector could
    # consume this object's section before the licensed START marker.
    all_selectors = []
    builtins = {"ALIGN", "ALIGNALL", "AFTER", "WRITEW", "ADDR", "SIZEOF"}
    for call in re.finditer(r"([^\s(){};]+)\s*\(", text):
        if call[1] in builtins:
            continue
        literal = re.match(r"\s*(\.[A-Za-z0-9_.]+)\s*\)", text[call.end() :])
        if not re.fullmatch(r"[A-Za-z0-9_@+.-]+\.o", call[1]) or literal is None:
            raise ValueError("unsupported LCF input-section selector grammar")
        if literal[1] == section:
            all_selectors.append(call[1] + "(" + literal[1] + ")")
    if placement == "interior":
        return _interior_successor(
            text, matching[0], begin, end, module, section, provider, all_selectors
        )
    if placement != "section-end":
        raise ValueError("unknown terminal fill placement")
    if len(all_selectors) != 1 or re.fullmatch(selector, all_selectors[0]) is None:
        raise ValueError("logical section must have exactly one selector in LCF")
    pattern = (
        r"\.\s*=\s*ALIGN\(\s*4\s*\)\s*;\s*"
        + re.escape(begin)
        + r"\s*=\s*\.\s*;\s*"
        + selector
        + r"\s*\.\s*=\s*ALIGN\(\s*4\s*\)\s*;\s*"
        + re.escape(end)
        + r"\s*=\s*\.\s*;"
    )
    if re.search(pattern, body) is None:
        raise ValueError(
            "section must select sole provider then terminal ALIGN4/end marker"
        )
    return dict(module=module, section=section, provider=provider, alignment=4, fill=0)


_OBJECT_NAME = r"[A-Za-z0-9_@+.-]+\.o"


def _interior_successor(text, block, begin, end, module, section, provider, selectors):
    """The literal selector that directly follows an interior provider."""
    if selectors.count(provider + "(" + section + ")") != 1:
        raise ValueError("provider selector must occur exactly once")
    body = block[2]
    own = list(
        re.finditer(
            r"(?<![A-Za-z0-9_@+.-])"
            + re.escape(provider)
            + r"\s*\(\s*"
            + re.escape(section)
            + r"\s*\)",
            body,
        )
    )
    first = re.search(r"\b" + re.escape(begin) + r"\s*=\s*\.\s*;", body)
    last = re.search(r"\b" + re.escape(end) + r"\s*=\s*\.\s*;", body)
    if (
        len(own) != 1
        or first is None
        or last is None
        or not first.end() <= own[0].start() < own[0].end() <= last.start()
    ):
        raise ValueError("interior provider must be selected between its section markers")
    # Nothing but whitespace may separate the two selectors: an assignment, ALIGN,
    # WRITEW or symbol definition there would move or explain the fill instead.
    successor = re.match(
        r"\s*(" + _OBJECT_NAME + r")\s*\(\s*" + re.escape(section) + r"\s*\)",
        body[own[0].end() : last.start()],
    )
    if successor is None or successor[1] == provider:
        raise ValueError(
            "interior provider must be followed directly by another provider"
        )
    return dict(
        module=module,
        section=section,
        provider=provider,
        alignment=4,
        fill=0,
        placement="interior",
        successor=successor[1],
    )


def successor_start(raw, section, cursor):
    """Where the linker places a successor object's first bytes of ``section``.

    One literal selector places every same-named input section of the object in
    header order, each at ALIGNALL(4) or its own larger sh_addralign. Empty
    sections still align the cursor. An object that contributes no bytes cannot
    prove what lands at the claim end, so it is refused.
    """
    try:
        rows = [
            (sec["sh_addralign"], sec["sh_size"])
            for sec in ELFFile(io.BytesIO(raw)).iter_sections()
            if sec.name == section
        ]
    except (ELFError, IndexError, KeyError, struct.error) as exc:
        raise ValueError("malformed successor object") from exc
    for alignment, size in rows:
        step = LA.effective_alignment(alignment)
        cursor = (cursor + step - 1) // step * step
        if size:
            return cursor
    raise ValueError("successor object contributes no bytes to the section")


def json_digest(value):
    return hashlib.sha256(
        json.dumps(value, sort_keys=True, separators=(",", ":")).encode()
    ).hexdigest()


def prepared_receipt(raw, entry, claims, owned_report):
    """Caller supplies the just-measured TB.verify_owned_sections report.

    This is internal pipeline evidence, not a manifest field or user-provided
    receipt. Integration must produce it only after successful preparation.
    """
    if owned_report.get("ok") is not True:
        raise ValueError("successful actual prepared-owned proof required")
    return dict(
        objectSha256=hashlib.sha256(raw).hexdigest(),
        manifestSha256=json_digest(entry),
        claimsSha256=json_digest(claims),
        owned=owned_report,
    )


def checked_layout(raw, section, start):
    import tubuild as TB

    try:
        return TB.section_contribution(raw, section, start)
    except (ELFError, IndexError, KeyError, struct.error) as exc:
        raise ValueError("malformed prepared-object section layout") from exc


def layout_extent_matches(layout, claim, proof):
    physical = len(layout["bytes"])
    expected = integer(claim["end"]) - integer(claim["start"])
    if physical == expected:
        return True
    if terminal_policy(claim) is None:
        return False
    terminal_extent(claim, integer(claim["start"]) + physical)
    rows = [
        row
        for row in proof["owned"].get("rows", [])
        if row.get("section") == claim["name"]
    ]
    return (
        len(rows) == 1
        and rows[0].get("sizeOk") is True
        and rows[0].get("bytesOk") is True
        and rows[0].get("emittedBytes") == physical
        and rows[0].get("linkedBytes") == expected
    )


def prefix_storage_license(
    raw_objects,
    entry,
    claims,
    homes,
    *,
    vtable,
    alias,
    preparation_proofs,
    configured_extents,
):
    """Initialized-only identity proof from actual prepared native storage.

    Returns a zero-storage LCF identity, NOT an8-byte compiler object. No baseline
    symbol is needed. Real integration must pass the actual link object population
    and use already-validated claims/policies, then prove final module placement.
    homes must census EVERY configured module/name; configured_extents must census
    EVERY configured row for the owner module (all kinds, zero-size labels too).
    raw_objects must be the COMPLETE actual link object population. Partial caller
    populations are not sufficient evidence and must never be production inputs.
    """
    candidates = [row for row in entry.get("data", []) if row.get("symbol") == vtable]
    if len(candidates) != 1 or not vtable.startswith("_ZTV"):
        raise ValueError("one explicit vtable storage policy required")
    row = candidates[0]
    storage, public, size, bias = (
        integer(row[key])
        for key in ("emitted_storage_address", "address", "size", "address_point_bias")
    )
    if bias != 8 or storage + 8 != public or size <= 8 or size % 4:
        raise ValueError("exact8-byte native vtable prefix required")
    module = entry["module"]
    if homes.get(vtable) != [(module, public)] or homes.get(alias) != [
        (module, storage)
    ]:
        raise ValueError("unique same-module public/prefix configured homes required")
    seen = {}
    for configured in configured_extents:
        if configured["module"] != module:
            continue
        name = configured["symbol"]
        address, extent = integer(configured["address"]), integer(
            configured.get("size", 0)
        )
        if extent < 0:
            raise ValueError("invalid configured extent")
        if name in (vtable, alias):
            if not configured.get("initialized"):
                raise ValueError(
                    "configured public/prefix identity must be initialized data"
                )
            expected_address = public if name == vtable else storage
            expected_size = size - bias if name == vtable else bias
            if address != expected_address or extent not in (0, expected_size):
                raise ValueError(
                    "configured public/prefix extent conflicts with storage"
                )
            seen[name] = seen.get(name, 0) + 1
        elif storage <= address < storage + size or (
            extent and address < storage + size and address + extent > storage
        ):
            raise ValueError("another configured symbol occupies full vtable storage")
    if seen != {vtable: 1, alias: 1}:
        raise ValueError("complete unique configured public/prefix rows required")
    rtti = "_ZTI" + vtable[4:]
    rtti_rows = [r for r in entry.get("data", []) if r.get("symbol") == rtti]
    if len(rtti_rows) != 1 or homes.get(rtti) != [
        (module, integer(rtti_rows[0]["address"]))
    ]:
        raise ValueError("unique owned same-class RTTI required")
    definitions = {}
    for path, raw in raw_objects:
        elf = ELFFile(io.BytesIO(raw))
        if (
            elf.elfclass != 32
            or not elf.little_endian
            or elf["e_machine"] != "EM_ARM"
            or elf["e_type"] != "ET_REL"
        ):
            raise ValueError("prepared32-bit little-endian ARM object required")
        symtab = elf.get_section_by_name(".symtab")
        tables = [
            section
            for section in elf.iter_sections()
            if section["sh_type"] == "SHT_SYMTAB"
        ]
        if symtab is None or len(tables) != 1 or tables[0].name != ".symtab":
            raise ValueError("one canonical symbol table required")
        for symbol in symtab.iter_symbols():
            index = symbol["st_shndx"]
            if isinstance(index, int) and not 0 <= index < elf.num_sections():
                raise ValueError("invalid symbol section index")
            if (
                symbol.name
                and symbol["st_shndx"] != "SHN_UNDEF"
                and symbol["st_info"]["bind"] != "STB_LOCAL"
            ):
                definitions.setdefault(symbol.name, []).append((path, elf, symbol))
    if (
        alias in definitions
        or len(definitions.get(vtable, [])) != 1
        or len(definitions.get(rtti, [])) != 1
    ):
        raise ValueError(
            "prefix must be absent; unique actual vtable and RTTI providers required"
        )
    path, elf, symbol = definitions[vtable][0]
    source = pathlib.PurePosixPath(
        entry.get("promoted_source", entry["source"])
    ).with_suffix(".o")
    if not pathlib.Path(path).resolve().as_posix().endswith("/" + str(source)):
        raise ValueError("actual source provider mismatch")
    raw = next(data for candidate, data in raw_objects if candidate == path)
    proof = preparation_proofs.get(str(path), {})
    if (
        proof.get("objectSha256") != hashlib.sha256(raw).hexdigest()
        or proof.get("manifestSha256") != json_digest(entry)
        or proof.get("claimsSha256") != json_digest(claims)
        or proof.get("owned", {}).get("ok") is not True
    ):
        raise ValueError("stale or missing successful prepared-object proof")
    if not isinstance(symbol["st_shndx"], int):
        raise ValueError("defined section-relative vtable required")
    section = elf.get_section(symbol["st_shndx"])
    value = integer(symbol["st_value"])
    offset = value - bias
    if (
        symbol["st_info"]["type"] != "STT_OBJECT"
        or symbol["st_info"]["bind"] != "STB_GLOBAL"
        or symbol["st_other"]["visibility"] != "STV_DEFAULT"
        or symbol["st_size"] != size - bias
        or value != 8
        or offset != 0
        or section["sh_type"] != "SHT_PROGBITS"
        or DSR.initialized_kind(section.name) is None
        or size != section["sh_size"]
    ):
        raise ValueError(
            "prepared public symbol does not prove full native initialized storage"
        )
    # This narrow rule supports a dedicated compiler VT section only. Local
    # labels and OBJECT aliases are substantive too; do not ignore them merely
    # because they do not appear in the global link inventory.
    for occupant in elf.get_section_by_name(".symtab").iter_symbols():
        if occupant["st_shndx"] != symbol["st_shndx"]:
            continue
        if occupant.name == vtable and occupant.entry == symbol.entry:
            continue
        section_marker = (
            occupant["st_info"]["type"] == "STT_SECTION"
            and occupant["st_info"]["bind"] == "STB_LOCAL"
            and occupant["st_value"] == 0
            and occupant["st_size"] == 0
            and occupant.name in ("", section.name)
        )
        mapping_marker = (
            re.fullmatch(r"\$[adt](?:\.[0-9]+)?", occupant.name or "") is not None
            and occupant["st_info"]["type"] == "STT_NOTYPE"
            and occupant["st_info"]["bind"] == "STB_LOCAL"
            and occupant["st_size"] == 0
            and 0 <= occupant["st_value"] < size
        )
        if not section_marker and not mapping_marker:
            raise ValueError("another ELF symbol occupies dedicated vtable section")
    covering = [
        c
        for c in claims
        if c["name"] == section.name
        and integer(c["start"]) <= storage
        and storage + size <= integer(c["end"])
    ]
    if len(covering) != 1:
        raise ValueError("full source-owned vtable storage claim required")
    layout, error = checked_layout(raw, section.name, integer(covering[0]["start"]))
    placed = (layout or {}).get("symbols", {}).get(vtable)
    if (
        error
        or not placed
        or placed["address"] != public
        or placed["size"] != size - bias
        or not layout_extent_matches(layout, covering[0], proof)
    ):
        raise ValueError(
            "actual prepared section layout does not match configured public placement"
        )
    rpath, relf, rsymbol = definitions[rtti][0]
    if not isinstance(rsymbol["st_shndx"], int):
        raise ValueError("defined section-relative RTTI required")
    rsection = relf.get_section(rsymbol["st_shndx"])
    raddr = integer(rtti_rows[0]["address"])
    rsize = integer(rtti_rows[0]["size"])
    rclaims = [
        c
        for c in claims
        if c["name"] == rsection.name
        and integer(c["start"]) <= raddr
        and raddr + rsize <= integer(c["end"])
    ]
    if (
        rpath != path
        or rsymbol["st_info"]["type"] != "STT_OBJECT"
        or rsymbol["st_size"] != rsize
        or rsymbol["st_info"]["bind"] not in ("STB_GLOBAL", "STB_LOPROC")
        or rsymbol["st_info"]["bind"] != rtti_rows[0].get("binding")
        or rsymbol["st_other"]["visibility"] != "STV_DEFAULT"
        or rsection["sh_type"] != "SHT_PROGBITS"
        or len(rclaims) != 1
    ):
        raise ValueError("RTTI storage is not owned by same complete provider")
    rlayout, error = checked_layout(raw, rsection.name, integer(rclaims[0]["start"]))
    rplaced = (rlayout or {}).get("symbols", {}).get(rtti)
    if (
        error
        or not rplaced
        or rplaced["address"] != raddr
        or rplaced["size"] != rsize
        or not layout_extent_matches(rlayout, rclaims[0], proof)
    ):
        raise ValueError(
            "actual prepared RTTI layout differs from configured placement"
        )
    prefix = section.data()[offset : offset + 8]
    if prefix != bytes(8):
        raise ValueError("raw prefix words must be zero before relocation")
    relocs = []
    for relsec in elf.iter_sections():
        if (
            isinstance(relsec, RelocationSection)
            and relsec["sh_info"] == symbol["st_shndx"]
        ):
            if (
                relsec["sh_type"] != "SHT_RELA"
                or relsec["sh_entsize"] != 12
                or relsec["sh_size"] % 12
                or not 0 < relsec["sh_link"] < elf.num_sections()
            ):
                raise ValueError("native vtable prefix requires canonical RELA entries")
            table = elf.get_section(relsec["sh_link"])
            if table.name != ".symtab" or table["sh_type"] != "SHT_SYMTAB":
                raise ValueError("vtable relocation symbol table mismatch")
            for rel in relsec.iter_relocations():
                if rel["r_offset"] < offset + 8 and rel["r_offset"] + 4 > offset:
                    if not 0 < rel["r_info_sym"] < table.num_symbols():
                        raise ValueError("invalid prefix relocation symbol index")
                    addend = (
                        rel["r_addend"]
                        if rel.is_RELA()
                        else struct.unpack_from("<I", section.data(), rel["r_offset"])[
                            0
                        ]
                    )
                    relocs.append(
                        (
                            rel["r_offset"] - offset,
                            rel["r_info_type"],
                            table.get_symbol(rel["r_info_sym"]).name,
                            addend,
                        )
                    )
    if relocs != [(4, 2, rtti, 0)]:
        raise ValueError("prefix needs exact ABS32 same-class RTTI at+4")
    return dict(
        alias=alias,
        module=module,
        address=storage,
        identitySize=0,
        licensedStoragePrefixBytes=8,
        fullStorageBytes=size,
        publicBytes=size - bias,
        provider=str(path),
        publicSymbol=vtable,
        rttiSymbol=rtti,
        representation="LCF identity only; not an8-byte emitted compiler object",
    )


def config_population(config_root):
    """Complete configured identity/relocation census for a particular link tree."""
    import tubuild as TB

    root = pathlib.Path(config_root)
    homes, extents, relocations = {}, [], []
    symbol_pattern = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:0x([0-9a-fA-F]+)(?:\s|$)")
    for path in sorted(root.rglob("symbols.txt")):
        module = (
            "arm9" if path.parent == root else RL.normalize_module(path.parent.name)
        )
        for line in path.read_text(encoding="utf-8").splitlines():
            match = symbol_pattern.match(line)
            if not match:
                if line.strip() and not line.lstrip().startswith(("#", "//")):
                    raise ValueError(f"unsupported configured symbol row in {path}")
                continue
            name, kind, address = match[1], match[2], int(match[3], 16)
            homes.setdefault(name, []).append((module, address))
            sized = re.search(r"\bsize=(0x[0-9a-fA-F]+|[0-9]+)", kind)
            array = re.fullmatch(
                r"data\((byte|short|word)(?:\[(0x[0-9a-fA-F]+|[0-9]+)\])?\)", kind
            )
            size = (
                int(sized[1], 0)
                if sized
                else (int(array[2], 0) if array[2] else 1)
                * {"byte": 1, "short": 2, "word": 4}[array[1]]
                if array
                else 0
            )
            extents.append(
                dict(
                    symbol=name,
                    module=module,
                    address=address,
                    size=size,
                    initialized=kind.startswith("data("),
                    unknown_initialized_extent=(
                        kind.startswith("data(") and sized is None and array is None
                    ),
                )
            )
    pattern = re.compile(
        r"^from:0x([0-9a-fA-F]+) kind:(\S+) to:0x([0-9a-fA-F]+) module:(\S+)(?:\s|$)"
    )
    for path in sorted(root.rglob("relocs.txt")):
        module = (
            "arm9" if path.parent == root else RL.normalize_module(path.parent.name)
        )
        for line in path.read_text(encoding="utf-8").splitlines():
            match = pattern.match(line)
            if not match:
                constant = re.fullmatch(
                    r"from:0x([0-9a-fA-F]+) kind:link_time_const\([A-Za-z0-9_]+\)", line
                )
                if constant:
                    relocations.append(
                        dict(
                            module=module,
                            site=int(constant[1], 16),
                            size=4,
                            target=-8,
                            targetModules=set(),
                            kind="link_time_const",
                            targetModule="none",
                        )
                    )
                    continue
                if line.strip() and not line.lstrip().startswith(("#", "//")):
                    raise ValueError(f"unsupported configured relocation row in {path}")
                continue
            relocations.append(
                dict(
                    module=module,
                    site=int(match[1], 16),
                    size=4,
                    target=int(match[3], 16),
                    targetModules=TB._relocation_module_set(match[4]),
                    kind=match[2],
                    targetModule=match[4],
                )
            )
    return homes, extents, relocations


def finalize_layout(entry, claim, layout, config_root, target_reader, population=None):
    """Append only licensed linker fill to the comparison view, never the ELF."""
    if terminal_policy(claim) is None:
        return layout, None
    import tubuild as TB

    module = RL.normalize_module(entry["module"])
    header, _ = TB.parse_delinks_file(
        TB.module_dir_in(config_root, module) / "delinks.txt"
    )
    sections = [
        dict(name=name, start=start, end=end)
        for name, start, end in TB.EN.sections(header)
        if name == claim["name"]
    ]
    if len(sections) != 1:
        raise ValueError("terminal fill requires one logical module section")
    _, symbols, relocations = population or config_population(config_root)
    selected = [row for row in symbols if row["module"] == module]
    refs = [
        dict(
            site=row["site"] if row["module"] == module else -8,
            size=row["size"],
            target=row["target"]
            if module in row["targetModules"] or "none" in row["targetModules"]
            else -8,
        )
        for row in relocations
    ]
    start, end = integer(claim["start"]), integer(claim["end"])
    emitted_end = start + len(layout["bytes"])
    amount = terminal_extent(claim, emitted_end)
    fill = target_reader(module, emitted_end, amount)
    evidence = license_terminal_fill(
        claim,
        sections[0],
        emitted_end,
        configured_symbols=selected,
        relocations=refs,
        retail_fill=fill,
    )
    return dict(layout, bytes=layout["bytes"] + bytes(amount)), evidence


def prefix_policies(entry):
    policies = entry.get("initialized_prefix_aliases", [])
    if not isinstance(policies, list):
        raise ValueError("initialized_prefix_aliases must be a list")
    seen = set()
    for row in policies:
        if (
            not isinstance(row, dict)
            or set(row) != {"symbol", "vtable"}
            or not all(
                isinstance(row[key], str)
                and re.fullmatch(r"[A-Za-z_$][A-Za-z0-9_$]*", row[key])
                for key in ("symbol", "vtable")
            )
            or not row["vtable"].startswith("_ZTV")
            or row["symbol"] in seen
        ):
            raise ValueError("invalid or duplicate initialized prefix identity policy")
        seen.add(row["symbol"])
    return policies


def link_plan(entries, objects_txt, lcf_path, config_root):
    """Reprove opt-in boundary policies using the actual complete link population.

    Called by both scratch and normal links after object preparation and before
    LCF alias insertion. No saved manifest receipt substitutes for this proof.
    """
    import tubuild as TB

    selected = []
    for entry in entries:
        claims, errors = TB.manifest_section_claims(entry)
        if errors:
            raise ValueError("; ".join(errors))
        prefixes = prefix_policies(entry)
        terminals = [claim for claim in claims if terminal_policy(claim) is not None]
        if terminals or prefixes:
            if entry.get("production_mode") != "intact-object":
                raise ValueError(
                    "initialized boundary policies require an intact object"
                )
            selected.append((entry, claims, prefixes, terminals))
    result = dict(prefixes=[], terminalFill=[])
    if not selected:
        return result
    raw_objects = []
    for line in pathlib.Path(objects_txt).read_text(encoding="utf-8").splitlines():
        if line.strip():
            path = pathlib.Path(line.strip().strip('"')).resolve()
            raw_objects.append((path, path.read_bytes()))
    population = config_population(config_root)
    homes, extents, configured_relocations = population
    config_relocs = {}
    for row in configured_relocations:
        config_relocs.setdefault(row["module"], {})[row["site"]] = (
            row["kind"],
            row["target"],
            row["targetModule"],
        )
    for entry, claims, prefixes, terminals in selected:
        source = entry.get("promoted_source") or entry["source"]
        suffix = "/" + str(pathlib.PurePosixPath(source).with_suffix(".o"))
        providers = [
            (path, raw) for path, raw in raw_objects if path.as_posix().endswith(suffix)
        ]
        if len(providers) != 1:
            raise ValueError(f"{source}: expected one actual prepared provider")
        path, raw = providers[0]
        errors = TB.validate_enrolled_intact_tu_entry(
            TB.module_dir_in(config_root, entry["module"]) / "delinks.txt",
            source,
            claims,
        )
        if errors:
            raise ValueError("; ".join(errors))
        owned = TB.verify_owned_sections(
            raw,
            entry,
            claims,
            public_address_points=True,
            normalized_undefined_vtables=True,
            boundary_config_root=config_root,
            config_relocs=config_relocs,
            symbol_homes=homes,
        )
        proof = prepared_receipt(raw, entry, claims, owned)
        for claim in terminals:
            layout, error = checked_layout(raw, claim["name"], claim["start"])
            if error:
                raise ValueError(error)
            _, fill = finalize_layout(
                entry, claim, layout, config_root, TB.BP.target_bytes, population
            )
            lcf = verify_terminal_lcf(
                lcf_path,
                module=entry["module"].upper(),
                section=claim["name"],
                provider=path.name,
                placement=fill["placement"],
            )
            if fill["placement"] == "interior":
                # The LCF names the successor by basename; it must be one actual
                # link input, and its own headers must put it at the claim end.
                successors = [
                    successor_raw
                    for successor_path, successor_raw in raw_objects
                    if successor_path.name == lcf["successor"]
                ]
                if len(successors) != 1:
                    raise ValueError(
                        f"{source}: interior fill successor {lcf['successor']} "
                        "is not exactly one link input"
                    )
                placed = successor_start(
                    successors[0], claim["name"], fill["emittedEnd"]
                )
                if placed != fill["end"]:
                    raise ValueError(
                        f"{source}: interior fill successor {lcf['successor']} "
                        f"links at 0x{placed:08x}, claim ends at 0x{fill['end']:08x}"
                    )
            result["terminalFill"].append(
                dict(fill, lcf=lcf, source=source, objectSha256=proof["objectSha256"])
            )
        for policy in prefixes:
            result["prefixes"].append(
                prefix_storage_license(
                    raw_objects,
                    entry,
                    claims,
                    homes,
                    vtable=policy["vtable"],
                    alias=policy["symbol"],
                    preparation_proofs={str(path): proof},
                    configured_extents=extents,
                )
            )
    aliases = [row["alias"] for row in result["prefixes"]]
    if len(set(aliases)) != len(aliases):
        raise ValueError("duplicate initialized prefix identity across providers")
    return result
