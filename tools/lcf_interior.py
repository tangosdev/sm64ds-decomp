"""Define, at link time, ROM functions that live inside another source's body.

A hand-asm source can cover several ROM functions in one asm block.
`src/unnamed/arm9/01ff/func_01ff97d8.c` is the ITCM soft-float routine at
0x01ff97d8..0x01ffa344: symbols.txt splits it into five functions (97d8, 98f4,
99a4, 9d40 `_deq`, 9e2c), but branches cross every boundary into interior
labels, so it is one asm block. mwccarm cannot export a second name from an asm
block (no `entry`, `.global` or `sym+off`), so that object defines only
`func_01ff97d8`.

A delinked caller never notices: dsd leaves a `module:none` call as raw bytes
and imports nothing. A compiled caller does. `bl func_01ff99a4` carries a
relocation against that name, and the link aborts `Undefined`, because the
range is source-built and no gap object defines the rows inside it any more.

`patch` closes that gap with absolute definitions: for every function row
of a fixed-address module (arm9, itcm, dtcm) that lies strictly inside a
`complete` delinks range and that no object handed to the link defines, it
adds `name = 0x<addr>;` to the linker command file, inside that module's
output section. Those modules never move, so the address in
symbols.txt is the address the linked code is at. Overlay code entry points are
left alone.

The output section matters to `dsd check symbols`, which reads the linked ELF
and looks for a row in its own module's section. An assignment at the top of
`SECTIONS` gets whatever section mwldarm happens to give it (`_deq` and
`func_01ff98f4` landed in OV007, OV060 and ARM9, never ITCM), so it links and
then fails the check. Inside `.itcm : {` the same assignment keeps its address
and lands in ITCM. A newly enrolled ITCM block takes a second route: the gap
object used to export every symbols.txt row at that address, and the asm block
exports one, so `stamp_linked_aliases` writes the other rows into that object
before the link, size 0 when symbols.txt says so, so the sum of symbol sizes
stays inside the section.

BSS interior labels, including overlay labels, retain configured identities even
without live imports when one actual source provider proves the complete NOBITS
object. The older imported-label path remains available. The LCF
defines the canonical label inside its enclosing output module. This preserves aliases
without adding storage, changing object bytes, or assuming an overlay base.

Initialized-data labels at an object's start or inside it are retained even without
live imports, because dsd validates configured identities. Each alias needs one actual PROGBITS OBJECT
definition from its complete source provider, a unique configured module home,
and a matching .data/.rodata range covering the entire emitted object.

It also adds the NitroSDK linker constants in LINKER_CONSTANTS. The SDK's own
LCF template assigns them and its OS init code takes their *address* as the
value (`(s32)SDK_IRQ_STACKSIZE`); dsd's generated command file does not, so a
compiled reader (func_02058308, func_02058df4) links `Undefined`.
"""
import pathlib
import re
import struct
import data_section_routes as DSR


def initialized_interior_rows(config_root, objects_txt, definitions):
    """Initialized-object aliases, including another name at the object's start."""
    return _source_storage_interior_rows(config_root, objects_txt, definitions)


def source_bss_interior_rows(config_root, objects_txt, definitions):
    return _source_storage_interior_rows(config_root, objects_txt, definitions, bss=True)


def _source_storage_interior_rows(config_root, objects_txt, definitions, bss=False):
    """Configured labels inside complete, actual source-defined storage objects.

    Unlike an unresolved import, an unreferenced configured interior label still
    needs an ELF identity for symbol validation. Only matching OBJECT storage
    in the same section, module and source provider can justify that identity.
    This adds no storage and does not infer an object's extent from the next label.
    """
    import io
    from elftools.elf.elffile import ELFFile

    root = pathlib.Path(config_root)
    storage_type = "SHT_NOBITS" if bss else "SHT_PROGBITS"
    homes, claims = {}, []
    symbol_row = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:0x([0-9a-fA-F]+)")
    section_row = re.compile(
        r"^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)")

    def supported_section(name):
        return name == ".bss" if bss else DSR.initialized_kind(name) is not None
    for symbols in sorted(root.rglob("symbols.txt")):
        module = symbols.parent.relative_to(root).as_posix()
        for line in symbols.read_text(encoding="utf-8").splitlines():
            match = symbol_row.match(line)
            if match:
                name, kind, address = match.groups()
                size = re.search(r"\bsize=(0x[0-9a-fA-F]+|[0-9]+)", kind)
                valid_kind = (re.fullmatch(r"bss(?:\(size=(?:0x[0-9a-fA-F]+|[0-9]+)\))?", kind)
                              is not None) if bss else kind.startswith("data(")
                homes.setdefault(name, []).append((
                    module, int(address, 16), valid_kind,
                    int(size[1], 0) if size else (None if bss else 0)))
        delinks = symbols.with_name("delinks.txt")
        if not delinks.is_file():
            continue
        source, complete, ranges = None, False, []

        def flush():
            if source and source.startswith("src/") and complete:
                claims.extend((module, source, *row) for row in ranges)

        for line in delinks.read_text(encoding="utf-8").splitlines():
            if line and not line[0].isspace() and line.rstrip().endswith(":"):
                flush()
                source, complete, ranges = line.strip()[:-1], False, []
            elif line.strip() == "complete":
                complete = True
            else:
                match = section_row.match(line)
                if match and supported_section(match[1]):
                    ranges.append((match[1], int(match[2], 16), int(match[3], 16)))
        flush()

    objects, definition_counts = {}, {}
    for line in pathlib.Path(objects_txt).read_text(encoding="utf-8").splitlines():
        path = pathlib.Path(line.strip().strip('"'))
        if not line.strip():
            continue
        raw = path.read_bytes()
        if raw[:4] != b"\x7fELF":
            continue
        elf = ELFFile(io.BytesIO(raw))
        if (elf.elfclass != 32 or not elf.little_endian
                or elf["e_type"] != "ET_REL" or elf["e_machine"] != "EM_ARM"):
            continue
        tables = [section for section in elf.iter_sections()
                  if section["sh_type"] == "SHT_SYMTAB"]
        for symbol in (symbol for table in tables for symbol in table.iter_symbols()):
            index = symbol["st_shndx"]
            if (symbol.name and index != "SHN_UNDEF"
                    and symbol["st_info"]["bind"] != "STB_LOCAL"):
                definition_counts[symbol.name] = definition_counts.get(symbol.name, 0) + 1
            if (not symbol.name or not isinstance(index, int) or index <= 0
                    or index >= elf.num_sections()
                    or symbol["st_info"]["bind"] == "STB_LOCAL"
                    or symbol["st_info"]["type"] != "STT_OBJECT"):
                continue
            section = elf.get_section(index)
            size, value = symbol["st_size"], symbol["st_value"]
            if (section["sh_type"] != storage_type
                    or not supported_section(section.name)
                    or not size or value + size > section["sh_size"]):
                continue
            objects.setdefault(symbol.name, []).append((path, section.name, size))

    owners = []
    for name, rows in objects.items():
        locations = homes.get(name, [])
        if (name not in definitions or len(rows) != 1 or len(locations) != 1
                or definition_counts.get(name) != 1):
            continue
        path, section, size = rows[0]
        module, address, is_data, declared_size = locations[0]
        if not is_data or (((bss and declared_size is not None) or declared_size)
                           and declared_size != size):
            continue
        matching = [(source, start, end) for mod, source, sec, start, end in claims
                    if mod == module and sec == section
                    and start <= address and address + size <= end
                    and path.resolve().as_posix().endswith("/" + str(
                        pathlib.PurePosixPath(source).with_suffix(".o")))]
        if len(matching) == 1:
            owners.append((name, module, address, size))
    out = []
    for name, locations in sorted(homes.items()):
        if name in definitions or len(locations) != 1:
            continue
        module, address, is_data, declared_size = locations[0]
        if not is_data:
            continue
        enclosing = [(owner, base) for owner, mod, base, size in owners
                     if mod == module and base <= address < base + size
                     and (not bss or base < address)
                     and address + (declared_size or 0) <= base + size]
        if len(enclosing) == 1:
            owner, base = enclosing[0]
            out.append((name, address, owner, address - base, module))
    return out

FIXED = ("arm9", "itcm", "dtcm")

# Value = what the ROM's literal pools hold where these are read: 0 at
# 0x02058458/0x02058e8c (no override, so the default system stack) and 0x600
# at 0x0205846c/0x02058e90.
LINKER_CONSTANTS = {
    "SDK_SYS_STACKSIZE": 0x0,
    "SDK_IRQ_STACKSIZE": 0x600,
}

_ROW = re.compile(
    r"^(\S+)\s+kind:(?:function|label)\([^)]*\).*?addr:0x([0-9a-fA-F]+)")
_TEXT = re.compile(r"^\s+\.text\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)")


def _module_dir(config_root, module):
    return config_root if module == "arm9" else config_root / module


def _complete_ranges(delinks_path, pattern):
    """Ranges selected from complete source entries, excluding module headers."""
    out, body, complete = [], [], False

    def flush():
        if complete:
            for ln in body:
                m = pattern.match(ln)
                if m:
                    out.append((int(m.group(1), 16), int(m.group(2), 16)))

    for ln in delinks_path.read_text(errors="ignore").splitlines():
        if ln and not ln[0].isspace() and ln.rstrip().endswith(":"):
            flush()
            body, complete = [], False
        elif ln.strip() == "complete":
            complete = True
        else:
            body.append(ln)
    flush()
    return out


def complete_text_ranges(delinks_path):
    """[(start, end)] of the .text section of every `complete` source entry."""
    return _complete_ranges(delinks_path, _TEXT)


def _elf_bss_definitions(data):
    """Global object extents in NOBITS sections; values stay section-relative."""
    if data[:6] != b"\x7fELF\x01\x01":
        return {}
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum = struct.unpack_from("<HH", data, 0x2e)
    secs = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize)
            for i in range(shnum)]
    out = {}
    for sec in secs:
        if sec[1] != 2:
            continue
        strings = secs[sec[6]]
        for off in range(sec[4], sec[4] + sec[5], 16):
            name, value, size, info, _other, shndx = struct.unpack_from(
                "<IIIBBH", data, off)
            if not name or not (info >> 4) or (info & 15) != 1 \
                    or not 0 < shndx < len(secs) or secs[shndx][1] != 8 \
                    or not size or value + size > secs[shndx][5]:
                continue
            start = strings[4] + name
            symbol = data[start:data.index(b"\0", start)].decode("latin-1")
            out.setdefault(symbol, []).append(size)
    return out


def bss_interior_rows(config_root, definitions, imported, bss_definitions):
    """Missing, uniquely homed labels inside actual source-defined BSS objects.

    Return (label, address, enclosing symbol, offset, module). The extent check
    ties each label to real source storage before an LCF definition is allowed.
    Both names must have exactly one configured home in the same module, the
    enclosing symbol must have one NOBITS definition, and its entire extent must
    lie in a complete BSS range. Ambiguity leaves the ordinary link checks red.
    """
    root = pathlib.Path(config_root)
    pattern = re.compile(r"^\s+\.bss\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)")
    row_pattern = re.compile(r"^(\S+)\s+kind:bss\s+addr:0x([0-9a-fA-F]+)(?:\s|$)")
    homes, ranges = {}, {}
    for syms in sorted(root.rglob("symbols.txt")):
        dl = syms.with_name("delinks.txt")
        module = syms.parent.relative_to(root).as_posix()
        ranges[module] = _complete_ranges(dl, pattern) if dl.is_file() else []
        for line in syms.read_text(errors="ignore").splitlines():
            # Count every kind's home: a same-named function/data in another
            # overlay is just as ambiguous as a second BSS row.
            match = re.match(r"^(\S+)\s+kind:.*?addr:0x([0-9a-fA-F]+)", line)
            if match:
                bss = row_pattern.match(line) is not None
                homes.setdefault(match[1], []).append((module, int(match[2], 16), bss))
    owners = []
    for name, sizes in bss_definitions.items():
        locations = homes.get(name, [])
        if name not in definitions or len(sizes) != 1 or len(locations) != 1:
            continue
        module, address, is_bss = locations[0]
        if is_bss and any(lo <= address and address + sizes[0] <= hi
                          for lo, hi in ranges[module]):
            owners.append((name, module, address, sizes[0]))
    out = []
    for name in sorted(imported - definitions):
        locations = homes.get(name, [])
        if len(locations) != 1:
            continue
        module, address, is_bss = locations[0]
        if not is_bss:
            continue
        enclosing = [(owner, base) for owner, mod, base, size in owners
                     if mod == module and base < address < base + size]
        if len(enclosing) == 1:
            owner, base = enclosing[0]
            out.append((name, address, owner, address - base, module))
    return out


def interior_rows(config_root):
    """[(name, addr)] for function and label rows strictly inside a complete range.

    Zero-size rows count: `_deq` is a size-0 alias row at 0x01ff9d40, the same
    address as func_01ff9d40, and a compiled caller of a `double ==` imports it
    by that name (func_0206f46c, func_020708b4). Skipping it left those links
    `Undefined : "_deq"`. Label rows count too: a secondary entry point inside
    a hand-asm function (_ZN3IRQ11SetIRQStateEb, 0x10 into func_0206da18) is a
    label, not a function, but callers import it by name all the same. A row at
    a range's own start is still skipped (strict `<`): that address is the
    source's own symbol, defined by its object."""
    return [(name, addr) for name, addr, _module in _interior_rows(config_root)]


def _interior_rows(config_root):
    config_root = pathlib.Path(config_root)
    rows = []
    for module in FIXED:
        d = _module_dir(config_root, module)
        syms, dl = d / "symbols.txt", d / "delinks.txt"
        if not (syms.is_file() and dl.is_file()):
            continue
        ranges = complete_text_ranges(dl)
        for ln in syms.read_text(errors="ignore").splitlines():
            m = _ROW.match(ln)
            if not m:
                continue
            addr = int(m.group(2), 16)
            if any(lo < addr < hi for lo, hi in ranges):
                rows.append((m.group(1), addr, module))
    return rows


def start_alias_rows(config_root):
    """[(name, addr)] for same-address pairs at a complete range's start.

    One asm block defines one spelling there, so a second row at the same
    address is the other spelling. `patch` defines whichever spelling the
    link imports but no object defines.
    """
    return [(name, addr) for name, addr, _module in _start_alias_rows(config_root)]


def _start_alias_rows(config_root):
    config_root = pathlib.Path(config_root)
    rows = []
    for module in FIXED:
        d = _module_dir(config_root, module)
        syms, dl = d / "symbols.txt", d / "delinks.txt"
        if not (syms.is_file() and dl.is_file()):
            continue
        ranges = complete_text_ranges(dl)
        starts = {lo for lo, _hi in ranges}
        by_addr: dict[int, list[tuple[str, int, str]]] = {}
        for ln in syms.read_text(errors="ignore").splitlines():
            m = _ROW.match(ln)
            if not m:
                continue
            addr = int(m.group(2), 16)
            if addr in starts:
                by_addr.setdefault(addr, []).append((m.group(1), addr, module))
        for addr in sorted(by_addr):
            if len(by_addr[addr]) >= 2:
                rows.extend(by_addr[addr])
    return rows


def _link_inventory(objects_txt):
    """(defined, imported, BSS extents): the non-local names the link's objects define, and the
    names they reference undefined."""
    defined, imported, bss = set(), set(), {}
    for line in pathlib.Path(objects_txt).read_text(errors="ignore").splitlines():
        path = line.strip().strip('"')
        if not path:
            continue
        try:
            data = pathlib.Path(path).read_bytes()
        except OSError:
            continue
        d, i = _elf_names(data)
        defined |= d
        imported |= i
        for name, sizes in _elf_bss_definitions(data).items():
            bss.setdefault(name, []).extend(sizes)
    return defined, imported, bss


def link_names(objects_txt):
    """(defined, imported) non-local symbols from the actual link inputs."""
    defined, imported, _bss = _link_inventory(objects_txt)
    return defined, imported


def _elf_names(data):
    """(defined, imported) names of one ELF object. Any binding but STB_LOCAL is a
    definition: mwcc marks vague-linkage ("multidef") definitions STB_LOPROC."""
    if data[:4] != b"\x7fELF" or data[4] != 1:
        return set(), set()
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum = struct.unpack_from("<HH", data, 0x2e)
    secs = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize)
            for i in range(shnum)]
    defined, imported = set(), set()
    for sec in secs:
        if sec[1] != 2:                     # SHT_SYMTAB
            continue
        strtab = secs[sec[6]]
        for off in range(sec[4], sec[4] + sec[5], 16):
            name, _value, _size, info, _other, shndx = struct.unpack_from(
                "<IIIBBH", data, off)
            if not name or (info >> 4) == 0:              # unnamed, or local
                continue
            start = strtab[4] + name
            sym = data[start:data.index(b"\0", start)].decode("latin-1")
            (imported if shndx == 0 else defined).add(sym)
    return defined, imported


# Object basenames whose asm block replaced a gap object that exported every
# symbols.txt row. The older complete ITCM block (func_01ff97d8.c) is not here:
# `patch` defines its interior rows inside `.itcm`, which satisfies dsd as well.
_ALIAS_OBJECTS = {
    "_dadd.o": "src/_dadd.c",
    "_dmul.o": "src/_dmul.c",
    "_ll_ufrom_d.o": "src/_ll_ufrom_d.c",
    "_dsub.o": "src/_dsub.c",
    "_fadd.o": "src/_fadd.c",
    "_ffix.o": "src/_ffix.c",
    "_ffltu.o": "src/_ffltu.c",
    "_fflt.o": "src/_fflt.c",
    "_fsub.o": "src/_fsub.c",
    "_ll_udiv.o": "src/_ll_udiv.c",
    "_ull_mod.o": "src/_ull_mod.c",
    "_ll_sdiv.o": "src/_ll_sdiv.c",
    "_s32_div_f.o": "src/_s32_div_f.c",
    "_u32_div_f.o": "src/_u32_div_f.c",
}

_NAMED = re.compile(
    r"^(\S+)\s+kind:(function|label)\(([^)]*)\).*?addr:0x([0-9a-fA-F]+)")


def _complete_text_range_for(delinks_path, source):
    """(lo, hi) of one complete .text entry, or None."""
    want = source.replace("\\", "/")
    current, complete = None, False
    found = None
    for ln in pathlib.Path(delinks_path).read_text(errors="ignore").splitlines():
        if ln and not ln[0].isspace() and ln.rstrip().endswith(":"):
            current = ln.strip()[:-1].replace("\\", "/")
            complete = False
            continue
        if current != want:
            continue
        if ln.strip() == "complete":
            complete = True
            continue
        match = _TEXT.match(ln)
        if match and complete:
            found = (int(match.group(1), 16), int(match.group(2), 16))
    return found


def _configured_rows(symbols_path, lo, hi):
    """[(name, addr, size, kind)] of function and label rows in [lo, hi)."""
    rows = []
    for ln in pathlib.Path(symbols_path).read_text(errors="ignore").splitlines():
        match = _NAMED.match(ln)
        if not match:
            continue
        addr = int(match.group(4), 16)
        if not lo <= addr < hi:
            continue
        size_match = re.search(r"size=0x([0-9a-fA-F]+)", match.group(3))
        size = int(size_match.group(1), 16) if size_match else 0
        rows.append((match.group(1), addr, size, match.group(2)))
    return rows


def _elf_sections(data):
    if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
        raise ValueError("alias stamp expects an ELF32 LE object")
    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2e)
    sections = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize)
                for i in range(shnum)]
    return shoff, shentsize, shstrndx, sections


def _section_name(data, sections, shstrndx, index):
    name_off = sections[shstrndx][4] + sections[index][0]
    end = data.index(b"\0", name_off)
    return data[name_off:end].decode("latin-1")


def _symbol_names(data, sym_off, sym_size, str_off):
    """[(name, value, size, info, shndx)] including the null symbol."""
    out = []
    for off in range(sym_off, sym_off + sym_size, 16):
        name, value, size, info, _other, shndx = struct.unpack_from("<IIIBBH", data, off)
        if name:
            start = str_off + name
            symbol = data[start:data.index(b"\0", start)].decode("latin-1")
        else:
            symbol = ""
        out.append((symbol, value, size, info, shndx))
    return out


def _stamp_elf(data, additions, shrink):
    """Return `data` with extra .text symbols. Section bytes are not rewritten.

    `additions` is [(name, value, size, kind)]. `shrink` is {name: size} applied
    to symbols the object already defines. Inserted bytes are a multiple of every
    later section's alignment, so those sections stay aligned.
    """
    if not additions and not shrink:
        return data
    shoff, shentsize, shstrndx, sections = _elf_sections(data)
    _phoff, _phentsize, phnum = struct.unpack_from("<IHH", data, 0x1c)
    if phnum:
        raise ValueError("alias stamp does not rewrite program headers")
    text_index = next(i for i, sec in enumerate(sections)
                      if sec[1] == 1 and _section_name(data, sections, shstrndx, i) == ".text")
    sym_index = next(i for i, sec in enumerate(sections) if sec[1] == 2)
    str_index = sections[sym_index][6]
    sym_off, sym_size = sections[sym_index][4], sections[sym_index][5]
    str_off, str_size = sections[str_index][4], sections[str_index][5]
    if str_off < sym_off + sym_size:
        raise ValueError("alias stamp expects the string table after the symbol table")
    text_size = sections[text_index][5]
    existing = _symbol_names(data, sym_off, sym_size, str_off)
    have = {name for name, _value, _size, _info, shndx in existing if name and shndx}
    additions = [row for row in additions if row[0] not in have]
    shrink = {name: size for name, size in shrink.items() if name in have}
    if not additions and not shrink:
        return data
    for name, value, size, _kind in additions:
        if value >= text_size or value + size > text_size:
            raise ValueError(f"{name} at +0x{value:x} size 0x{size:x} does not fit .text")
    by_name = {name: index for index, (name, *_rest) in enumerate(existing) if name}
    out = bytearray(data)
    for name, size in shrink.items():
        struct.pack_into("<I", out, sym_off + by_name[name] * 16 + 8, size)
    if not additions:
        return bytes(out)

    new_strings = b"".join(name.encode("latin-1") + b"\0" for name, *_rest in additions)
    delta_sym = 16 * len(additions)
    str_end = str_off + str_size
    sym_end = sym_off + sym_size
    moved_align = [sec[8] for sec in sections if sec[4] >= str_end and sec[8] > 1]
    if shoff >= str_end:
        moved_align.append(4)
    max_align = max([16, *moved_align])
    raw_shift = delta_sym + len(new_strings)
    pad = (max_align - (raw_shift % max_align)) % max_align
    info_for = {"function": (1 << 4) | 2, "label": 1 << 4}
    extra = bytearray()
    cursor = str_size
    for name, value, size, kind in additions:
        extra += struct.pack("<IIIBBH", cursor, value, size, info_for[kind], 0, text_index)
        cursor += len(name) + 1
    out[sym_end:sym_end] = extra
    out[str_end + delta_sym:str_end + delta_sym] = new_strings + (b"\0" * pad)
    shift = delta_sym + len(new_strings) + pad

    def move(offset):
        if offset >= str_end:
            return offset + shift
        if offset >= sym_end:
            return offset + delta_sym
        return offset

    struct.pack_into("<I", out, 0x20, move(shoff))
    new_shoff = move(shoff)
    for index, sec in enumerate(sections):
        name, typ, flags, addr, offset, size, link, info, align, entsize = sec
        offset = move(offset)
        if index == sym_index:
            size += delta_sym
        elif index == str_index:
            size += len(new_strings) + pad
        struct.pack_into("<IIIIIIIIII", out, new_shoff + index * shentsize,
                         name, typ, flags, addr, offset, size, link, info, align, entsize)
    return bytes(out)


def stamp_linked_aliases(objects_txt, config_root):
    """Give each newly enrolled ITCM asm object the symbols.txt rows it replaced.

    The gap object exported both spellings and the interior labels. The asm
    object exports one. dsd's symbol check wants the others as ELF symbols.
    """
    config_root = pathlib.Path(config_root)
    itcm = _module_dir(config_root, "itcm")
    delinks, symbols = itcm / "delinks.txt", itcm / "symbols.txt"
    if not delinks.is_file() or not symbols.is_file():
        return
    for line in pathlib.Path(objects_txt).read_text(errors="ignore").splitlines():
        path = pathlib.Path(line.strip().strip('"'))
        source = _ALIAS_OBJECTS.get(path.name)
        if source is None or not path.is_file():
            continue
        span = _complete_text_range_for(delinks, source)
        if span is None:
            raise ValueError(f"{source} is linked but has no complete itcm .text range")
        lo, hi = span
        rows = _configured_rows(symbols, lo, hi)
        data = path.read_bytes()
        _shoff, _shentsize, shstrndx, sections = _elf_sections(data)
        text_index = next(i for i, sec in enumerate(sections)
                          if sec[1] == 1 and _section_name(data, sections, shstrndx, i) == ".text")
        sym_index = next(i for i, sec in enumerate(sections) if sec[1] == 2)
        str_index = sections[sym_index][6]
        existing = _symbol_names(data, sections[sym_index][4], sections[sym_index][5],
                                 sections[str_index][4])
        have = {name: (value, size) for name, value, size, _info, shndx in existing
                if name and shndx}
        configured = {name: (addr - lo, size, kind) for name, addr, size, kind in rows}
        additions = [(name, value, size, kind)
                     for name, (value, size, kind) in configured.items() if name not in have]
        shrink = {}
        for name, (_value, size, _kind) in configured.items():
            if size or name not in have:
                continue
            old_value, old_size = have[name]
            if old_size and any(other_value == old_value and other_size
                                for other, (other_value, other_size, _k) in configured.items()
                                if other != name):
                shrink[name] = 0
        text = data[sections[text_index][4]:sections[text_index][4] + sections[text_index][5]]
        stamped = _stamp_elf(data, additions, shrink)
        stamped_text_off = _elf_sections(stamped)[3][text_index][4]
        stamped_text = stamped[stamped_text_off:stamped_text_off + len(text)]
        if stamped_text != text:
            raise ValueError(f"{path.name}: alias stamp changed .text bytes")
        if stamped != data:
            path.write_bytes(stamped)


def patch(lcf_path, objects_txt, config_root, *, initialized_prefixes=()):
    """Add the missing interior definitions and linker constants to `lcf_path`;
    return [(name, addr)].

    An interior code row is defined whenever no object defines it, imported or
    not: dsd checks every symbols.txt row, and an asm block cannot export it, so
    the LCF is the only place it can come from (the strict stock control links
    no compiled caller of func_01ff99a4). Start-alias spellings and linker
    constants still require an import. Source-proven storage interior labels also
    preserve configured identities checked by dsd, even without an import."""
    stamp_linked_aliases(objects_txt, config_root)
    defined, imported, bss = _link_inventory(objects_txt)
    aliases = bss_interior_rows(config_root, defined, imported, bss)
    aliases += initialized_interior_rows(config_root, objects_txt, defined)
    aliases += source_bss_interior_rows(config_root, objects_txt, defined)
    # Only the shared initialized-boundary link planner supplies these records,
    # after re-verifying the actual prepared objects and complete source claims.
    # They are zero-storage LCF identities, not additional compiler OBJECTs.
    for row in initialized_prefixes:
        if row["identitySize"] != 0 or row["alias"] in defined:
            raise ValueError("initialized prefix identity is already defined or has storage")
        aliases.append((row["alias"], row["address"], row["publicSymbol"],
                        -row["licensedStoragePrefixBytes"], row["module"]))
    aliases = sorted(set(aliases))
    if len({row[0] for row in aliases}) != len(aliases):
        raise ValueError("conflicting storage alias identities")
    interior = _interior_rows(config_root)
    # A linker constant is a value, not an address in a module: module None.
    gated = _start_alias_rows(config_root)
    gated += [(name, value, None) for name, value in LINKER_CONSTANTS.items()]
    missing = sorted({row for row in interior if row[0] not in defined}
                     | {row for row in gated
                        if row[0] in imported and row[0] not in defined},
                     key=lambda r: r[1])
    if not missing and not aliases:
        return []
    lcf_path = pathlib.Path(lcf_path)
    text = lcf_path.read_bytes().decode("latin-1")    # keep CRLF visible
    nl = "\r\n" if "\r\n" in text else "\n"
    constants = [(n, a) for n, a, module in missing if module is None]
    if constants:
        anchor = "SECTIONS {"
        at = text.index(anchor) + len(anchor)
        block = "".join(f"{nl}    {n} = 0x{a:08x};" for n, a in constants)
        text = text[:at] + block + text[at:]
    # Each insert goes first in its section, so reversed order reads ascending.
    for name, address, module in reversed(missing):
        if module is not None:
            text = _define_in_section(text, nl, module, name, address, "interior row")
    missing = [(n, a) for n, a, _module in missing]
    for name, address, _owner, _offset, module in aliases:
        section = pathlib.PurePosixPath(module).name if module != "." else "arm9"
        # mwldarm evaluates `object + offset` here before relocating the
        # object and would retain only the offset. A canonical value INSIDE
        # the output module preserves both the address and ELF section identity.
        text = _define_in_section(text, nl, section, name, address, "storage alias")
        missing.append((name, address))
    lcf_path.write_bytes(text.encode("latin-1"))
    return sorted(missing, key=lambda row: row[1])


def _define_in_section(text, nl, section, name, address, what):
    """`text` with `name = 0x<address>;` first inside the one `.<section>` block."""
    pattern = re.compile(r"^([ \t]*\." + re.escape(section) +
                         r"[ \t]*:[ \t]*\{)", re.MULTILINE)
    matches = list(pattern.finditer(text))
    if len(matches) != 1:
        raise ValueError(f"{what} {name} requires exactly one .{section} "
                         "output section")
    at = matches[0].end()
    return text[:at] + f"{nl}        {name} = 0x{address:08x};" + text[at:]
