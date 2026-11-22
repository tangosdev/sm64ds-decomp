"""Place Thumb code that starts on a halfword, not a word.

mwldarm's ``ALIGNALL(4)`` rounds every input section up to a word. A Thumb
function whose ROM address is only halfword-aligned (the SVC stubs at the top
of ARM9) cannot be its own contribution under that rule: the linker inserts
two bytes and every later address in the module moves. The same thing happens
to the two-byte gap that follows a Thumb function whose size is 2 mod 4.

Those contributions are still whole sections. This pass runs after dsd has
generated the linker script and after mwccarm has written the objects. It
lowers ``sh_addralign`` to 2 on each halfword contribution and switches the
script to ``ALIGNALL(2)`` for that one object, then back to ``ALIGNALL(4)``
before the next word-aligned contribution. Data, and every word-aligned
function, keeps the pinned ``ALIGNALL(4)`` minimum.

``tools/linker_alignment.py`` still checks the script dsd emitted. This edit
is applied after that check, immediately before ``mwldarm``.
"""
import io
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


def _objects(objects_txt):
    rows = []
    for line in pathlib.Path(objects_txt).read_text(encoding="utf-8", errors="ignore").splitlines():
        line = line.strip()
        if line and not line.startswith("#"):
            rows.append(pathlib.Path(line))
    return rows


ASSIGN_RE = re.compile(r"^[ \t]*\.\s*=")


def apply(lcf_path, objects_txt, config_root):
    """Rewrite ``lcf_path`` and the halfword objects it links. Return a report.

    Each object line is emitted where dsd wrote it. ``ALIGNALL`` for one
    contribution is inserted on the line above that contribution. Buffering a
    whole ``.text`` run and flushing it at the next input would slide
    ``ARM9_TEXT_END`` and the ``. = ALIGN`` assignments that sit between the
    code and the exception tables, and every later overlay origin would move.
    """
    lcf_path = pathlib.Path(lcf_path)
    text = lcf_path.read_bytes().decode("latin-1")
    newline = "\r\n" if "\r\n" in text else "\n"
    lines = text.splitlines()
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

    current = 4
    run_kind = None
    cursor = None
    piece_index = {".text": 0, ".init": 0}
    pending_section = None
    table = None
    block_indent = "        "
    new_lines = []
    patched = []
    switches = 0

    def restore_word(indent):
        """Back to a word before something that is not another code contribution."""
        nonlocal current, run_kind, cursor, switches
        if current != 4:
            new_lines.append(f"{indent}ALIGNALL(4);")
            current = 4
            switches += 1
        run_kind = None
        cursor = None

    def finish_section():
        if table is None:
            return
        for kind in CODE_SECTIONS:
            got = piece_index[kind]
            need = len(table[kind])
            if got != need:
                raise HalfwordLinkError(
                    f".{pending_section} {kind} has {got} linker inputs and "
                    f"{need} delinks contributions")

    def take_piece(kind, name, indent):
        nonlocal current, cursor, run_kind, switches
        if table is None:
            raise HalfwordLinkError(
                f".{pending_section} has {kind} inputs but no delinks.txt")
        rows = table[kind]
        index = piece_index[kind]
        if index >= len(rows):
            raise HalfwordLinkError(
                f".{pending_section} {kind} has {index + 1} linker inputs and "
                f"{len(rows)} delinks contributions")
        piece = rows[index]
        piece_index[kind] = index + 1
        piece_kind, start, _end, stem = piece
        if piece_kind == "src" and pathlib.Path(name).stem != stem:
            raise HalfwordLinkError(
                f"{name} is not the object for {stem} at 0x{start:08x}")
        if piece_kind == "gap" and not pathlib.Path(name).name.startswith("_dsd_gap"):
            raise HalfwordLinkError(
                f"{name} occupies a ROM gap at 0x{start:08x}")
        path = by_name.get(name)
        if path is None:
            raise HalfwordLinkError(f"{name} is not listed in the object file")
        size, align, indexes, shoff, shentsize, little = _section_info(
            path, kind, start)
        if run_kind != kind:
            # A .text run and an .init run are different address lists, and a
            # label or assignment may have moved the location counter between
            # them. The first object of a run is not checked against the
            # previous contribution.
            cursor = None
            run_kind = kind
        switch, do_patch, current, cursor = plan_step(
            piece, (size, align), current, cursor)
        if switch is not None:
            new_lines.append(f"{indent}ALIGNALL({switch});")
            switches += 1
        if do_patch and _force_align(path, indexes, shoff, shentsize, little, 2):
            patched.append(str(path))

    for line in lines:
        sec = SECTION_RE.match(line)
        if sec:
            if pending_section is not None:
                restore_word(block_indent)
                finish_section()
            new_lines.append(line)
            pending_section = sec.group(2)
            current = 4
            run_kind = None
            cursor = None
            piece_index = {".text": 0, ".init": 0}
            table = pieces_for(pending_section)
            continue
        if pending_section is not None and SECTION_END_RE.match(line):
            restore_word(block_indent)
            finish_section()
            pending_section = None
            table = None
            new_lines.append(line)
            continue
        if pending_section is not None and run_kind is not None and ALIGNALL_RE.match(line):
            # Inserted by a previous pass. This pass emits the switch again.
            continue
        inp = INPUT_RE.match(line) if pending_section is not None else None
        if inp:
            block_indent = inp.group(1)
            kind = inp.group(3)
            if kind in CODE_SECTIONS:
                take_piece(kind, inp.group(2), block_indent)
                new_lines.append(line)
                continue
            restore_word(block_indent)
            new_lines.append(line)
            continue
        if pending_section is not None and ASSIGN_RE.match(line):
            indent = re.match(r"^([ \t]*)", line).group(1)
            block_indent = indent or block_indent
            restore_word(block_indent)
            new_lines.append(line)
            continue
        matched = ALIGNALL_RE.match(line) if pending_section is not None else None
        if matched and run_kind is None:
            current = int(matched.group(2))
            block_indent = matched.group(1)
        new_lines.append(line)

    if pending_section is not None:
        restore_word(block_indent)
        finish_section()

    rendered = newline.join(new_lines) + newline
    if rendered != text:
        lcf_path.write_bytes(rendered.encode("latin-1"))
    return {"patchedObjects": len(patched), "alignmentSwitches": switches,
            "objects": patched}
