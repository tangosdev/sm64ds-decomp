"""Place Thumb code that starts on a halfword, not a word.

mwldarm's ``ALIGNALL(4)`` rounds every input section up to a word. A Thumb
function whose ROM address is only halfword-aligned (the SVC stubs at the top
of ARM9) cannot be its own contribution under that rule: the linker inserts
two bytes and every later address in the module moves. The same thing happens
to the two-byte gap that follows a Thumb function whose size is 2 mod 4.

Those contributions stay whole sections, and the linker script is left exactly
as dsd wrote it. ``ALIGNALL(4)`` is a minimum this linker does not drop for one
input, so a 2-byte edge would be padded and every later address would move.
This pass runs after dsd has generated the script and after mwccarm has written
the objects. It moves the two neighboring gap bytes onto the function object,
so the object the linker sees starts and ends on a word. The function symbol
keeps the ROM address: a halfword start is two bytes into that object.

``tools/linker_alignment.py`` still checks the script dsd emitted. This edit
is applied after that check, immediately before ``mwldarm``.
"""
import io
import os
import pathlib
import re
import struct

from elftools.elf.elffile import ELFFile

import enroll


INPUT_RE = re.compile(r"^([ \t]*)(\S+)\((\.[^)]+)\)[ \t]*$")
ALIGNALL_RE = re.compile(r"^([ \t]*)ALIGNALL\((\d+)\)\s*;[ \t]*$")
SECTION_RE = re.compile(r"^([ \t]*)\.(\w+)[ \t]*:[ \t]*\{[ \t]*$")
SECTION_END_RE = re.compile(r"^[ \t]*\}[ \t]*>[ \t]*\w+[ \t]*$")
CODE_SECTIONS = {".text", ".init"}
RANGE_RE = re.compile(
    r"^[ \t]+(\.\S+)[ \t]+start:0x([0-9a-fA-F]+)[ \t]+end:0x([0-9a-fA-F]+)[ \t]*$")


class HalfwordLinkError(ValueError):
    """The halfword contributions cannot be proved to land on their ROM addresses."""


def _delinks_for(config_root, section):
    root = pathlib.Path(config_root)
    if section == "arm9":
        path = root / "delinks.txt"
    elif section in ("itcm", "dtcm"):
        path = root / section / "delinks.txt"
    elif re.fullmatch(r"ov\d+", section):
        path = root / "overlays" / section / "delinks.txt"
    else:
        return None
    return path if path.is_file() else None


def code_pieces(delinks_path):
    """``.text`` and ``.init`` contributions in link order, gaps included.

    Each row is ``(kind, start, end, stem)``. ``kind`` is ``"src"`` or
    ``"gap"``. ``stem`` is the source filename stem for a complete entry and
    ``None`` for a ROM-byte gap.
    """
    header, entries = enroll.read_delinks_full(delinks_path)
    bounds = enroll.sections(header)
    # Every file entry is its own linker input, complete or not. Only addresses
    # with no entry at all are gap objects. Counting complete entries alone
    # merges a demoted file into the neighbouring gap and the script no longer
    # matches.
    owned = []
    for rel, body in entries:
        stem = pathlib.PurePosixPath(rel).stem
        for line in body:
            match = RANGE_RE.match(line)
            if match and match.group(1) in CODE_SECTIONS:
                owned.append((match.group(1), int(match.group(2), 16),
                              int(match.group(3), 16), stem))
    out = {".text": [], ".init": []}
    for section in out:
        spans = [(lo, hi) for name, lo, hi in bounds if name == section]
        rows = sorted(
            (start, end, stem)
            for name, start, end, stem in owned if name == section)
        for lo, hi in spans:
            cursor = lo
            inside = [(start, end, stem) for start, end, stem in rows
                      if lo <= start and end <= hi]
            for start, end, stem in inside:
                if start < cursor:
                    raise HalfwordLinkError(
                        f"{delinks_path}: {section} ranges overlap at 0x{start:08x}")
                if start > cursor:
                    out[section].append(("gap", cursor, start, None))
                out[section].append(("src", start, end, stem))
                cursor = end
            if cursor < hi:
                out[section].append(("gap", cursor, hi, None))
            elif cursor > hi:
                raise HalfwordLinkError(
                    f"{delinks_path}: {section} ends at 0x{cursor:08x}, "
                    f"past the section end 0x{hi:08x}")
    return out


def _align_up(value, alignment):
    return (value + alignment - 1) & ~(alignment - 1)


def plan_step(piece, info, current, cursor):
    """One contribution. Returns ``(switch, patch, current, cursor)``.

    ``switch`` is 2 or 4 when ``ALIGNALL`` must change before this object, else
    ``None``. ``patch`` is true when the object's alignment has to drop to 2.
    ``cursor`` comes back as the contribution's end address. Pass ``cursor=None``
    for the first contribution of a run, before which the script may move the
    location counter on its own.
    """
    kind, start, end, stem = piece
    size, align = info
    if size != end - start:
        label = stem or "gap"
        raise HalfwordLinkError(
            f"{label} {kind} at 0x{start:08x} covers 0x{end - start:x} bytes "
            f"but its object contributes 0x{size:x}")
    if start % 2 != 0:
        raise HalfwordLinkError(
            f"code contribution at 0x{start:08x} is not halfword-aligned")
    need = 2 if start % 4 else 4
    if cursor is not None:
        eff = 2 if need == 2 else max(4, align)
        if _align_up(cursor, eff) != start:
            label = stem or "gap"
            raise HalfwordLinkError(
                f"{label} at 0x{start:08x} is not where alignment {eff} "
                f"places the cursor 0x{cursor:08x}")
    switch = need if need != current else None
    return switch, need == 2, need, end


def plan_run(pieces, infos, current):
    """How one consecutive run of code contributions has to be aligned.

    ``pieces`` and ``infos`` are parallel. ``infos`` rows are ``(size, align)``.
    Returns ``(switches, patch, current)`` where ``switches[i]`` is 2, 4, or
    ``None`` and ``patch[i]`` says the object's code section must be lowered to
    alignment 2.
    """
    switches = []
    patch = []
    cursor = None
    for piece, info in zip(pieces, infos):
        switch, do_patch, current, cursor = plan_step(piece, info, current, cursor)
        switches.append(switch)
        patch.append(do_patch)
    return switches, patch, current


def _section_info(path, section, start):
    """``(extent, first_align, header_indexes, ...)`` for ``section`` at ``start``.

    One section contributes its own size. Several word-aligned sections are
    laid out the way ``file.o(.text)`` does under ``ALIGNALL(4)``, and the
    extent includes the padding between them. A halfword address has to be a
    single section: the later alignment drop applies to every section of that
    name, and a second one would land on the wrong address.
    """
    raw = pathlib.Path(path).read_bytes()
    elf = ELFFile(io.BytesIO(raw))
    if elf.elfclass != 32:
        raise HalfwordLinkError(f"{path} is not a 32-bit object")
    found = []
    for index, sec in enumerate(elf.iter_sections()):
        if sec.name != section or not sec.header["sh_size"]:
            continue
        if sec.header["sh_type"] not in ("SHT_PROGBITS", "SHT_NOBITS"):
            continue
        found.append((index, sec.header["sh_size"], sec.header["sh_addralign"] or 1))
    if not found:
        raise HalfwordLinkError(f"{path} has no {section} bytes")
    halfword = start % 4 != 0
    if halfword and len(found) != 1:
        raise HalfwordLinkError(
            f"{path} has {len(found)} non-empty {section} sections at 0x{start:08x}; "
            "a halfword contribution has to be one section")
    if len(found) == 1:
        extent, align = found[0][1], found[0][2]
    else:
        cursor = start
        for index, size, align in found:
            placed = _align_up(cursor, max(4, align))
            if cursor == start and placed != start:
                raise HalfwordLinkError(
                    f"{path} {section} alignment {align} does not start at 0x{start:08x}")
            cursor = placed + size
        extent = cursor - start
        align = found[0][2]
    return (extent, align, [index for index, _size, _align in found],
            elf["e_shoff"], elf["e_shentsize"], elf.little_endian)


def _force_align(path, indexes, shoff, shentsize, little, align):
    if shentsize < 36:
        raise HalfwordLinkError(f"{path} has an unexpected section-header size")
    data = bytearray(pathlib.Path(path).read_bytes())
    endian = "<" if little else ">"
    changed = False
    for index in indexes:
        off = shoff + index * shentsize + 32
        current = struct.unpack_from(endian + "I", data, off)[0]
        if current <= align:
            continue
        struct.pack_into(endian + "I", data, off, align)
        changed = True
    if changed:
        pathlib.Path(path).write_bytes(data)
    return changed


def _path_name(text):
    """Basename of either host path spelling, independent of the Python host."""
    return pathlib.PurePosixPath(str(text).replace("\\", "/")).name


def _host_path(text):
    """Path of an object-list row on this host.

    The linker and dsd are Windows executables. On the Linux validation worker
    they run under Wine, so an object list can spell a Linux file as ``Z:\\home``
    or use backslash-separated relative rows even though Python itself is POSIX.
    Treat both spellings as paths; the Windows path remains native there.
    """
    raw = str(text).strip()
    if os.name != "nt":
        value = raw[4:] if raw.startswith("\\\\?\\") else raw
        drive = re.match(r"^([A-Za-z]):[\\/](.*)$", value)
        if drive and drive.group(1).upper() == "Z":
            return pathlib.Path("/" + drive.group(2).replace("\\", "/"))
        if "\\" in value:
            return pathlib.Path(value.replace("\\", "/"))
    return pathlib.Path(raw)


def _objects(objects_txt):
    rows = []
    for line in pathlib.Path(objects_txt).read_text(encoding="utf-8", errors="ignore").splitlines():
        line = line.strip()
        if line and not line.startswith("#"):
            rows.append(_host_path(line))
    return rows



def absorption_edits(pieces):
    """Bytes to move so every contribution starts and ends on a word.

    Each row is ``(trim_start, trim_end)``. A gap gives those bytes away. A
    function object receives them, in the same amounts. Both numbers are 0 or
    2. The linker script is not changed: the objects stay in order, and a gap
    that gives away all of its bytes is left empty.
    """
    edits = [[0, 0] for _piece in pieces]
    for index, (kind, start, end, stem) in enumerate(pieces):
        if kind != "src":
            continue
        if start % 4 == 2:
            if index == 0 or pieces[index - 1][0] != "gap" or pieces[index - 1][2] != start:
                raise HalfwordLinkError(
                    f"{stem} at 0x{start:08x} does not follow a ROM gap")
            if pieces[index - 1][2] - pieces[index - 1][1] < 2:
                raise HalfwordLinkError(
                    f"the gap before {stem} is shorter than 2 bytes")
            edits[index - 1][1] += 2
            edits[index][0] += 2
        if end % 4 == 2:
            if (index + 1 == len(pieces) or pieces[index + 1][0] != "gap"
                    or pieces[index + 1][1] != end):
                raise HalfwordLinkError(
                    f"{stem} ending at 0x{end:08x} is not followed by a ROM gap")
            if pieces[index + 1][2] - pieces[index + 1][1] < 2:
                raise HalfwordLinkError(
                    f"the gap after {stem} is shorter than 2 bytes")
            edits[index][1] += 2
            edits[index + 1][0] += 2
    for (kind, start, end, stem), (trim_start, trim_end) in zip(pieces, edits):
        size = end - start
        if trim_start + trim_end > size:
            label = stem or "gap"
            raise HalfwordLinkError(
                f"{label} at 0x{start:08x} cannot give away "
                f"{trim_start + trim_end} of its {size} bytes")
        if kind == "src":
            new_start = start - trim_start
            new_end = end + trim_end
        else:
            new_start = start + trim_start
            new_end = end - trim_end
        if new_start % 4 or new_end % 4:
            label = stem or "gap"
            raise HalfwordLinkError(
                f"{label} still spans 0x{new_start:08x}..0x{new_end:08x} "
                "after the gap bytes move")
    return edits


def _text_section(path, section):
    """The one non-empty ``section`` header, plus the file bytes."""
    path = pathlib.Path(path)
    data = bytearray(path.read_bytes())
    elf = ELFFile(io.BytesIO(data))
    if elf.elfclass != 32:
        raise HalfwordLinkError(f"{path} is not a 32-bit object")
    found = []
    for index, sec in enumerate(elf.iter_sections()):
        if sec.name != section or not sec.header["sh_size"]:
            continue
        if sec.header["sh_type"] != "SHT_PROGBITS":
            raise HalfwordLinkError(f"{path} {section} is not progbits")
        found.append(index)
    if len(found) != 1:
        raise HalfwordLinkError(
            f"{path} has {len(found)} non-empty {section} sections")
    index = found[0]
    header = elf.get_section(index).header
    return {
        "path": path,
        "data": data,
        "index": index,
        "offset": header["sh_offset"],
        "size": header["sh_size"],
        "shoff": elf["e_shoff"],
        "shentsize": elf["e_shentsize"],
        "little": elf.little_endian,
        "symtab": elf.get_section_by_name(".symtab"),
        "rela": elf.get_section_by_name(".rela" + section),
    }


def _pack_section(info, offset, size):
    if info["shentsize"] < 36:
        raise HalfwordLinkError(f"{info['path']} has an unexpected section-header size")
    endian = "<" if info["little"] else ">"
    base = info["shoff"] + info["index"] * info["shentsize"]
    struct.pack_into(endian + "II", info["data"], base + 16, offset, size)
    struct.pack_into(endian + "I", info["data"], base + 32, 4)


def _shift_symbols(info, delta, trim_start):
    """Move symbols that live in this section. ``delta`` is added to each value.

    A gap that loses its first bytes passes ``trim_start`` and a negative
    delta of the same size. A symbol inside those bytes is refused. The
    section symbol stays at 0.
    """
    symtab = info["symtab"]
    if symtab is None:
        return
    endian = "<" if info["little"] else ">"
    entry = symtab.header["sh_entsize"] or 16
    if entry < 16:
        raise HalfwordLinkError(f"{info['path']} has a short symbol entry")
    base = symtab.header["sh_offset"]
    for index, sym in enumerate(symtab.iter_symbols()):
        if sym["st_shndx"] != info["index"]:
            continue
        if sym["st_info"]["type"] == "STT_SECTION":
            continue
        value = sym["st_value"]
        if trim_start and value < trim_start:
            if value == 0 and sym["st_size"] == 0 and sym.name in ("", "$t", "$a", "$d"):
                continue
            raise HalfwordLinkError(
                f"{info['path']} symbol {sym.name or index} sits in the "
                f"{trim_start} bytes moved to the next object")
        if delta:
            struct.pack_into(endian + "I", info["data"], base + index * entry + 4,
                             value + delta)


def _shift_relocations(info, delta, trim_start):
    rela = info["rela"]
    if rela is None or not delta and not trim_start:
        return
    endian = "<" if info["little"] else ">"
    entry = rela.header["sh_entsize"] or 12
    if entry < 8:
        raise HalfwordLinkError(f"{info['path']} has a short relocation entry")
    base = rela.header["sh_offset"]
    for index, rel in enumerate(rela.iter_relocations()):
        offset = rel["r_offset"]
        if trim_start and offset < trim_start:
            raise HalfwordLinkError(
                f"{info['path']} has a relocation in the bytes moved off the gap")
        if delta:
            struct.pack_into(endian + "I", info["data"], base + index * entry, offset + delta)


def _apply_edit(info, piece, trim_start, trim_end, prefix, suffix):
    """Rewrite one section. ``prefix``/``suffix`` are bytes to add, or None.

    A gap passes the trim counts and no added bytes. A function passes the
    bytes taken from its neighboring gaps.
    """
    old = bytes(info["data"][info["offset"]:info["offset"] + info["size"]])
    if len(old) != info["size"]:
        raise HalfwordLinkError(f"{info['path']} {piece[0]} section is truncated")
    kind, start, end, stem = piece
    if info["size"] != end - start:
        label = stem or "gap"
        raise HalfwordLinkError(
            f"{label} at 0x{start:08x} covers 0x{end - start:x} bytes "
            f"but its object contributes 0x{info['size']:x}")
    if kind == "gap":
        if trim_start + trim_end > len(old):
            raise HalfwordLinkError(f"{info['path']} gap is too small to trim")
        _shift_symbols(info, -trim_start, trim_start)
        _shift_relocations(info, -trim_start, trim_start)
        _pack_section(info, info["offset"] + trim_start, len(old) - trim_start - trim_end)
        return old[:trim_start], old[len(old) - trim_end:] if trim_end else b""
    added_head = prefix or b""
    added_tail = suffix or b""
    if trim_start != len(added_head) or trim_end != len(added_tail):
        raise HalfwordLinkError(f"{stem} padding does not match the gap bytes")
    blob = added_head + old + added_tail
    new_off = len(info["data"])
    info["data"].extend(blob)
    _shift_symbols(info, len(added_head), 0)
    _shift_relocations(info, len(added_head), 0)
    _pack_section(info, new_off, len(blob))
    return None



def _normalize(data):
    """Lay section bytes out in header order so the linker's offset walk is boring."""
    elf = ELFFile(io.BytesIO(data))
    endian = "<" if elf.little_endian else ">"
    sections = list(elf.iter_sections())
    out = bytearray(data[:52])
    offsets = [0] * len(sections)
    for index, sec in enumerate(sections):
        if index == 0 or sec["sh_type"] == "SHT_NULL" or sec["sh_type"] == "SHT_NOBITS":
            continue
        align = sec["sh_addralign"] or 1
        while len(out) % align:
            out.append(0)
        offsets[index] = len(out)
        out.extend(data[sec["sh_offset"]:sec["sh_offset"] + sec["sh_size"]])
    while len(out) % 4:
        out.append(0)
    shoff = len(out)
    shentsize = elf["e_shentsize"]
    old_shoff = elf["e_shoff"]
    for index, sec in enumerate(sections):
        raw = bytearray(data[old_shoff + index * shentsize:
                             old_shoff + (index + 1) * shentsize])
        if offsets[index]:
            struct.pack_into(endian + "I", raw, 16, offsets[index])
        out.extend(raw)
    out[:52] = data[:52]
    struct.pack_into(endian + "I", out, 32, shoff)
    return bytes(out)


def _set_text(info, new_bytes):
    new_off = len(info["data"])
    info["data"].extend(new_bytes)
    _pack_section(info, new_off, len(new_bytes))
    info["offset"] = new_off
    info["size"] = len(new_bytes)


def _function_export(info):
    """``(name, size, thumb)`` of the one global function in this section."""
    name, size, thumb = None, None, False
    symtab = info["symtab"]
    if symtab is None:
        raise HalfwordLinkError(f"{info['path']} has no symbol table")
    for sym in symtab.iter_symbols():
        if sym["st_shndx"] != info["index"]:
            continue
        if sym.name == "$t":
            thumb = True
        elif (sym["st_info"]["type"] == "STT_FUNC"
              and sym["st_info"]["bind"] == "STB_GLOBAL"):
            name, size = sym.name, sym["st_size"]
    if not name:
        raise HalfwordLinkError(f"{info['path']} has no global function symbol")
    return name, size, thumb


def _bump_reloc_symbols(info, threshold, delta):
    """Keep relocations pointed at the same symbols after ``delta`` locals are inserted."""
    if not delta:
        return
    elf = ELFFile(io.BytesIO(bytes(info["data"])))
    endian = "<" if info["little"] else ">"
    for sec in elf.iter_sections():
        if sec["sh_type"] not in ("SHT_REL", "SHT_RELA"):
            continue
        entry = sec["sh_entsize"] or (8 if sec["sh_type"] == "SHT_REL" else 12)
        base = sec["sh_offset"]
        for index in range(sec["sh_size"] // entry):
            at = base + index * entry + 4
            word = struct.unpack_from(endian + "I", info["data"], at)[0]
            symbol = word >> 8
            if symbol >= threshold:
                struct.pack_into(endian + "I", info["data"], at,
                                 ((symbol + delta) << 8) | (word & 0xFF))


def _add_export(info, name, value, size, thumb):
    """Define ``name`` in this section at ``value``. A Thumb body also gets ``$t``."""
    symtab = info["symtab"]
    if symtab is None:
        raise HalfwordLinkError(f"{info['path']} has no symbol table")
    strtab = None
    elf = ELFFile(io.BytesIO(info["data"]))
    for sec in elf.iter_sections():
        if sec.header["sh_type"] == "SHT_STRTAB" and sec.name == ".strtab":
            strtab = sec
            break
    if strtab is None:
        raise HalfwordLinkError(f"{info['path']} has no string table")
    endian = "<" if info["little"] else ">"
    strings = bytearray(info["data"][strtab["sh_offset"]:
                                      strtab["sh_offset"] + strtab["sh_size"]])
    def add_name(text):
        if not text:
            return 0
        off = len(strings)
        strings.extend(text.encode("ascii") + b"\0")
        return off
    new_locals = []
    if thumb:
        new_locals.append(struct.pack(endian + "IIIBBH", add_name("$t"), value, 0, 0, 0,
                                      info["index"]))
    # Inserting locals in front of the globals renumbers every later symbol.
    # Relocations still name the old indexes, so move those with the symbols.
    _bump_reloc_symbols(info, symtab["sh_info"], len(new_locals))
    new_global = struct.pack(endian + "IIIBBH", add_name(name), value, size, 0x12, 0,
                             info["index"])
    old = []
    entry = symtab["sh_entsize"] or 16
    base = symtab["sh_offset"]
    raw = info["data"][base:base + symtab["sh_size"]]
    for index in range(0, len(raw), entry):
        old.append(bytes(raw[index:index + entry]))
    first_global = symtab["sh_info"]
    symbols = old[:first_global] + new_locals + old[first_global:] + [new_global]
    blob = b"".join(symbols)
    sym_off = len(info["data"])
    info["data"].extend(blob)
    str_off = len(info["data"])
    info["data"].extend(strings)
    shentsize = info["shentsize"]
    shoff = info["shoff"]
    # Section header index is the enumerate index elftools uses.
    for index, sec in enumerate(elf.iter_sections()):
        header = shoff + index * shentsize
        if sec.name == symtab.name and sec["sh_offset"] == symtab["sh_offset"]:
            struct.pack_into(endian + "II", info["data"], header + 16, sym_off, len(blob))
            struct.pack_into(endian + "I", info["data"], header + 28,
                             first_global + len(new_locals))
        elif sec.name == ".strtab" and sec["sh_offset"] == strtab["sh_offset"]:
            struct.pack_into(endian + "II", info["data"], header + 16, str_off, len(strings))


def _code_inputs(lcf_path):
    """``(section, kind, name)`` for every .text and .init input, in order."""
    text = pathlib.Path(lcf_path).read_bytes().decode("latin-1")
    rows = []
    pending = None
    for line in text.splitlines():
        sec = SECTION_RE.match(line)
        if sec:
            pending = sec.group(2)
            continue
        if pending is not None and SECTION_END_RE.match(line):
            pending = None
            continue
        inp = INPUT_RE.match(line) if pending is not None else None
        if inp and inp.group(3) in CODE_SECTIONS:
            rows.append((pending, inp.group(3), inp.group(2)))
    return rows


def _drop_inputs(lcf_path, objects_txt, dropped):
    """Remove absorbed inputs from the script and object list. Returns the script."""
    path = pathlib.Path(lcf_path)
    text = path.read_bytes().decode("latin-1")
    newline = "\r\n" if "\r\n" in text else "\n"
    kept = []
    for line in text.splitlines():
        inp = INPUT_RE.match(line)
        if inp and (inp.group(3), inp.group(2)) in dropped:
            continue
        kept.append(line)
    rendered = newline.join(kept) + newline
    if rendered != text:
        path.write_bytes(rendered.encode("latin-1"))
    referenced = set()
    for line in kept:
        inp = INPUT_RE.match(line)
        if inp:
            referenced.add(inp.group(2))
    obj_path = pathlib.Path(objects_txt)
    obj_text = obj_path.read_bytes().decode("latin-1")
    obj_nl = "\r\n" if "\r\n" in obj_text else "\n"
    obj_kept = []
    for line in obj_text.splitlines():
        name = _path_name(line.strip())
        if line.strip() and name not in referenced and any(
                kind_name[1] == name for kind_name in dropped):
            continue
        obj_kept.append(line)
    obj_rendered = obj_nl.join(obj_kept) + obj_nl
    if obj_rendered != obj_text:
        obj_path.write_bytes(obj_rendered.encode("latin-1"))
    return rendered


def apply(lcf_path, objects_txt, config_root):
    """Move a halfword function into the gap in front of it.

    The linker keeps every input section on a word. A two-byte stub therefore
    cannot be its own input: it is appended to the ROM gap that already ends at
    its address, and the stub's object is dropped from the script. The symbol
    keeps the ROM address because it points into that gap. A function that
    already starts on a word only absorbs the two-byte gap after it.
    """
    inputs = _code_inputs(lcf_path)
    if not inputs:
        return {"patchedObjects": 0, "alignmentSwitches": 0, "objects": []}
    by_name = {}
    for path in _objects(objects_txt):
        if path.name in by_name:
            raise HalfwordLinkError(f"{objects_txt} lists {path.name} twice")
        by_name[path.name] = path
    pieces_cache = {}

    def pieces_for(section):
        if section not in pieces_cache:
            path = _delinks_for(config_root, section)
            pieces_cache[section] = code_pieces(path) if path is not None else None
        return pieces_cache[section]

    grouped = {}
    for section, kind, name in inputs:
        grouped.setdefault(section, {".text": [], ".init": []})[kind].append(name)
    patched = []
    dropped = set()
    dirty = {}
    for section, by_kind in grouped.items():
        table = pieces_for(section)
        if table is None:
            raise HalfwordLinkError(f".{section} has code inputs but no delinks.txt")
        for kind, names in by_kind.items():
            pieces = table[kind]
            if len(names) != len(pieces):
                raise HalfwordLinkError(
                    f".{section} {kind} has {len(names)} linker inputs and "
                    f"{len(pieces)} delinks contributions")
            edits = absorption_edits(pieces)
            if not any(trim_start or trim_end for trim_start, trim_end in edits):
                continue
            section_kind = kind
            loaded = []
            for (trim_start, trim_end), name, (piece_kind, start, _end, stem) in zip(
                    edits, names, pieces):
                if piece_kind == "src" and pathlib.Path(name).stem != stem:
                    raise HalfwordLinkError(
                        f"{name} is not the object for {stem} at 0x{start:08x}")
                if piece_kind == "gap" and not pathlib.Path(name).name.startswith("_dsd_gap"):
                    raise HalfwordLinkError(
                        f"{name} occupies a ROM gap at 0x{start:08x}")
                if not trim_start and not trim_end:
                    loaded.append(None)
                    continue
                path = by_name.get(name)
                if path is None:
                    raise HalfwordLinkError(f"{name} is not listed in the object file")
                loaded.append(_text_section(path, kind))
            bodies = []
            starts = []
            for info, (_kind, start, end, _stem) in zip(loaded, pieces):
                if info is None:
                    bodies.append(None)
                    starts.append(start)
                    continue
                blob = bytes(info["data"][info["offset"]:info["offset"] + info["size"]])
                if len(blob) != end - start:
                    raise HalfwordLinkError(
                        f"{info['path']} contributes 0x{len(blob):x} bytes "
                        f"for 0x{start:08x}..0x{end:08x}")
                bodies.append(blob)
                starts.append(start)
            for index, (kind, start, end, stem) in enumerate(pieces):
                prefix, suffix = edits[index]
                if kind != "src" or not (prefix or suffix):
                    continue
                tail = b""
                if suffix:
                    nxt = bodies[index + 1]
                    if nxt is None or len(nxt) < suffix:
                        raise HalfwordLinkError(
                            f"the gap after {stem} cannot spare {suffix} bytes")
                    tail = nxt[:suffix]
                    bodies[index + 1] = nxt[suffix:]
                    starts[index + 1] += suffix
                    _shift_symbols(loaded[index + 1], -suffix, suffix)
                    _shift_relocations(loaded[index + 1], -suffix, suffix)
                if prefix:
                    host = index - 1
                    if starts[host] + len(bodies[host]) != start:
                        raise HalfwordLinkError(
                            f"{stem} at 0x{start:08x} does not sit at the end of the gap")
                    offset = len(bodies[host])
                    bodies[host] = bodies[host] + bodies[index] + tail
                    export_name, export_size, thumb = _function_export(loaded[index])
                    _add_export(loaded[host], export_name, offset, export_size, thumb)
                    _set_text(loaded[host], bodies[host])
                    dirty[loaded[host]["path"]] = loaded[host]
                    dropped.add((section_kind, names[index]))
                    patched.append(export_name)
                else:
                    bodies[index] = bodies[index] + tail
                    _set_text(loaded[index], bodies[index])
                    dirty[loaded[index]["path"]] = loaded[index]
                    patched.append(stem)
                if suffix and not bodies[index + 1]:
                    dropped.add((section_kind, names[index + 1]))
                elif suffix:
                    _set_text(loaded[index + 1], bodies[index + 1])
                    dirty[loaded[index + 1]["path"]] = loaded[index + 1]
    for info in dirty.values():
        info["path"].write_bytes(_normalize(info["data"]))
    if dropped:
        _drop_inputs(lcf_path, objects_txt, dropped)
    return {"patchedObjects": len(patched), "alignmentSwitches": 0,
            "objects": patched}
