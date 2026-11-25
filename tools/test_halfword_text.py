"""Halfword Thumb contributions stay on their ROM addresses."""
import io
import pathlib
import struct
import sys
import tempfile
import unittest

from elftools.elf.elffile import ELFFile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import halfword_text as HT

REPO = pathlib.Path(__file__).resolve().parent.parent

# The halfword rows known in ARM9. The linker support lands before their
# enrollment, so these rows are written into a temporary delinks.txt when a
# test needs the enrolled shape.
ARM9_HALFWORD_ROWS = (
    ("SVC_Div", 0x0200406a, 0x0200406e, (2, 2)),
    ("SVC_GetCRC16", 0x020041d2, 0x020041d6, (2, 2)),
    ("SVC_Halt", 0x020042b6, 0x020042ba, (2, 2)),
    ("SVC_CpuSet", 0x0200438a, 0x0200438e, (2, 2)),
    ("SVC_IsMmemExpanded", 0x020044ae, 0x020044b2, (2, 2)),
    ("SVC_CpuSetFast", 0x0200451e, 0x02004522, (2, 2)),
    ("SVC_UncompressRL8", 0x0200456e, 0x02004572, (2, 2)),
    ("SVC_UncompressHuffmanFromDevice", 0x020045e6, 0x020045ea, (2, 2)),
    ("SVC_UncompressLZ16FromDevice", 0x02004656, 0x0200465a, (2, 2)),
    ("SVC_WaitVBlankIntr", 0x020046c2, 0x020046c8, (2, 0)),
    ("SVC_WaitByLoop", 0x0200472a, 0x0200472e, (2, 2)),
    ("SVC_DivRem", 0x02004782, 0x02004788, (2, 0)),
    ("MTX_Scale43_", 0x020527e8, 0x020527fe, (0, 2)),
    ("MTX_RotX43_", 0x02052800, 0x0205281e, (0, 2)),
    ("MTX_RotY43_", 0x02052820, 0x0205283a, (0, 2)),
    ("MTX_RotZ43_", 0x0205283c, 0x02052856, (0, 2)),
)


def _write_arm9_halfword_shape(path):
    lines = ["    .text       start:0x02004000 end:0x0205285c kind:code align:32"]
    for stem, start, end, _edit in ARM9_HALFWORD_ROWS:
        lines.extend((
            "",
            f"src/{stem}.c:",
            "    complete",
            f"    .text start:0x{start:08x} end:0x{end:08x}",
        ))
    lines.extend((
        "",
        "src/after.c:",
        "    complete",
        "    .text start:0x02052858 end:0x0205285c",
    ))
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


class PlanTests(unittest.TestCase):
    def test_halfword_stub_between_word_gaps(self):
        pieces = [
            ("gap", 0x1000, 0x1006, None),
            ("src", 0x1006, 0x100a, "stub"),
            ("gap", 0x100a, 0x1010, None),
            ("src", 0x1010, 0x1014, "next"),
        ]
        infos = [(end - start, 4) for _kind, start, end, _stem in pieces]
        switches, patch, current = HT.plan_run(pieces, infos, 4)
        self.assertEqual(switches, [None, 2, None, 4])
        self.assertEqual(patch, [False, True, True, False])
        self.assertEqual(current, 4)

    def test_word_tail_uses_linker_padding_when_it_is_a_gap(self):
        # 0x16-byte function, two ROM bytes, next function on a word.
        pieces = [
            ("src", 0x200, 0x216, "scale"),
            ("gap", 0x216, 0x218, None),
            ("src", 0x218, 0x236, "rotx"),
        ]
        infos = [(end - start, 4) for _kind, start, end, _stem in pieces]
        switches, patch, current = HT.plan_run(pieces, infos, 4)
        self.assertEqual(switches, [None, 2, 4])
        self.assertEqual(patch, [False, True, False])
        self.assertEqual(current, 4)

    def test_size_mismatch_is_refused(self):
        pieces = [("src", 0x1000, 0x1004, "stub")]
        with self.assertRaises(HT.HalfwordLinkError):
            HT.plan_run(pieces, [(8, 4)], 4)

    def test_arm9_enrolled_thumb_places(self):
        with tempfile.TemporaryDirectory() as tmp:
            delinks = pathlib.Path(tmp) / "delinks.txt"
            _write_arm9_halfword_shape(delinks)
            pieces = HT.code_pieces(delinks)[".text"]
        infos = [(end - start, 4) for _kind, start, end, _stem in pieces]
        switches, patch, current = HT.plan_run(pieces, infos, 4)
        self.assertEqual(current, 4)
        patched = {stem for (_kind, _start, _end, stem), flag in zip(pieces, patch)
                   if flag and stem}
        self.assertTrue({
            "SVC_Div", "SVC_GetCRC16", "SVC_Halt", "SVC_CpuSet",
            "SVC_IsMmemExpanded", "SVC_CpuSetFast", "SVC_UncompressRL8",
            "SVC_UncompressHuffmanFromDevice", "SVC_UncompressLZ16FromDevice",
            "SVC_WaitVBlankIntr", "SVC_WaitByLoop", "SVC_DivRem",
        } <= patched)
        self.assertTrue({"MTX_Scale43_", "MTX_RotX43_", "MTX_RotY43_", "MTX_RotZ43_"}
                        .isdisjoint(patched))
        self.assertTrue(any(flag for flag in patch))
        self.assertIn(2, switches)


def _elf_text(size, align, name="sym", fill=0):
    """A relocatable ARM ELF with one ``.text`` and, for a function, one symbol."""
    text = bytes([fill]) * size
    if name.startswith("_dsd"):
        # A gap carries a symbol table so a function can be moved into it.
        strtab = b"\0"
        shstr = b"\0.text\0.symtab\0.strtab\0.shstrtab\0"
        text_off = 52
        sym_off = text_off + size
        str_off = sym_off + 32
        shstr_off = str_off + len(strtab)
        shoff = shstr_off + len(shstr)
        header = struct.pack(
            "<16sHHIIIIIHHHHHH",
            b"\x7fELF\x01\x01\x01" + b"\x00" * 9,
            1, 40, 1, 0, 0, shoff, 0, 52, 0, 0, 40, 5, 4)
        def sheader(sec_name, kind, flags, offset, sec_size, sec_align, link=0, info=0,
                    entsize=0):
            return struct.pack("<IIIIIIIIII", sec_name, kind, flags, 0, offset,
                               sec_size, link, info, sec_align, entsize)
        section_sym = struct.pack("<IIIBBH", 0, 0, 0, 3, 0, 1)
        return b"".join((
            header, text, b"\x00" * 16, section_sym, strtab, shstr,
            sheader(0, 0, 0, 0, 0, 0),
            sheader(1, 1, 6, text_off, size, align),
            sheader(7, 2, 0, sym_off, 32, 4, link=3, info=2, entsize=16),
            sheader(15, 3, 0, str_off, len(strtab), 1),
            sheader(23, 3, 0, shstr_off, len(shstr), 1),
        ))
    sym_name = pathlib.Path(name).stem.encode("ascii")
    strtab = b"\0" + sym_name + b"\0"
    shstr = b"\0.text\0.symtab\0.strtab\0.shstrtab\0"
    text_off = 52
    sym_off = text_off + size
    str_off = sym_off + 32
    shstr_off = str_off + len(strtab)
    shoff = shstr_off + len(shstr)
    header = struct.pack(
        "<16sHHIIIIIHHHHHH",
        b"\x7fELF\x01\x01\x01" + b"\x00" * 9,
        1, 40, 1, 0, 0, shoff, 0, 52, 0, 0, 40, 5, 4)
    def sheader(sec_name, kind, flags, offset, sec_size, sec_align, link=0, info=0,
                entsize=0):
        return struct.pack("<IIIIIIIIII", sec_name, kind, flags, 0, offset,
                           sec_size, link, info, sec_align, entsize)
    symbol = struct.pack("<IIIBBH", 1, 0, size, 0x12, 0, 1)
    body = b"".join((
        header,
        text,
        b"\x00" * 16,
        symbol,
        strtab,
        shstr,
        sheader(0, 0, 0, 0, 0, 0),
        sheader(1, 1, 6, text_off, size, align),
        sheader(7, 2, 0, sym_off, 32, 4, link=3, info=1, entsize=16),
        sheader(15, 3, 0, str_off, len(strtab), 1),
        sheader(23, 3, 0, shstr_off, len(shstr), 1),
    ))
    elf = ELFFile(io.BytesIO(body))
    sec = elf.get_section_by_name(".text")
    if sec.header["sh_size"] != size or sec.data() != text:
        raise AssertionError("synthetic ELF did not round-trip")
    return body


class ApplyTests(unittest.TestCase):
    def test_labels_stay_around_the_objects(self):
        # The assignments between the code and the exception table have to stay
        # there. Flushing the whole .text run at the next input slides them
        # above the objects and moves every overlay that follows ARM9.
        with tempfile.TemporaryDirectory() as tmp:
            root = pathlib.Path(tmp)
            config = root / "config"
            config.mkdir()
            (config / "delinks.txt").write_text(
                "    .text       start:0x02004000 end:0x02004010 kind:code align:32\n"
                "src/stub.c:\n"
                "    complete\n"
                "    .text start:0x02004006 end:0x0200400a\n",
                encoding="utf-8")
            objects = root / "obj"
            objects.mkdir()
            names = (
                ("_dsd_gap@main_0.o", 6, 4),
                ("stub.o", 4, 4),
                ("_dsd_gap@main_1.o", 6, 4),
            )
            fills = {"_dsd_gap@main_0.o": 0x11, "stub.o": 0x22, "_dsd_gap@main_1.o": 0x33}
            for name, size, align in names:
                (objects / name).write_bytes(_elf_text(size, align, name, fills[name]))
            # dsd/mwldarm are Windows tools even on the Linux validation worker.
            # Spell rows with backslashes so the pass cannot depend on pathlib's
            # host-specific separator rules.
            (root / "objects.txt").write_text(
                "\n".join(str(objects / name).replace("/", "\\")
                          for name, _size, _align in names) + "\n",
                encoding="utf-8")
            lcf = root / "arm9.lcf"
            original = (
                "    .arm9 : {\n"
                "        ALIGNALL(4);\n"
                "        . = ALIGN(32);\n"
                "        ARM9_TEXT_START = .;\n"
                "        _dsd_gap@main_0.o(.text)\n"
                "        stub.o(.text)\n"
                "        _dsd_gap@main_1.o(.text)\n"
                "        . = ALIGN(4);\n"
                "        ARM9_TEXT_END = .;\n"
                "        . = ALIGN(1);\n"
                "        ARM9_EXCEPTION_START = .;\n"
                "    } > ARM9\n"
            )
            lcf.write_bytes(original.encode("latin-1"))
            report = HT.apply(lcf, root / "objects.txt", config)
            rendered = lcf.read_text(encoding="latin-1")
            self.assertNotIn("stub.o(", rendered)
            self.assertLess(rendered.index("_dsd_gap@main_0.o(.text)"),
                            rendered.index("ARM9_TEXT_END = .;"))
            self.assertEqual(report["patchedObjects"], 1)
            with open(objects / "_dsd_gap@main_0.o", "rb") as handle:
                host = ELFFile(handle)
                self.assertEqual(host.get_section_by_name(".text").data(),
                                 b"\x11" * 6 + b"\x22" * 4 + b"\x33\x33")
                func = next(sym for sym in host.get_section_by_name(".symtab").iter_symbols()
                            if sym.name == "stub")
                self.assertEqual(func["st_value"], 6)
                self.assertEqual(func["st_size"], 4)
            with open(objects / "_dsd_gap@main_1.o", "rb") as handle:
                self.assertEqual(ELFFile(handle).get_section_by_name(".text").data(),
                                 b"\x33" * 4)

    def test_arm9_edges_land_on_words(self):
        with tempfile.TemporaryDirectory() as tmp:
            delinks = pathlib.Path(tmp) / "delinks.txt"
            _write_arm9_halfword_shape(delinks)
            pieces = HT.code_pieces(delinks)[".text"]
        edits = HT.absorption_edits(pieces)
        self.assertTrue(any(trim for row in edits for trim in row))
        for (kind, start, end, stem), (trim_start, trim_end) in zip(pieces, edits):
            if kind == "src":
                new_start, new_end = start - trim_start, end + trim_end
            else:
                new_start, new_end = start + trim_start, end - trim_end
            self.assertEqual((new_start % 4, new_end % 4), (0, 0))
        by_stem = {stem: edit for (_k, _s, _e, stem), edit in zip(pieces, edits)
                   if stem and any(edit)}
        expected = {stem: list(edit) for stem, _start, _end, edit in ARM9_HALFWORD_ROWS}
        self.assertEqual(by_stem, expected)

    def test_stock_arm9_matches_its_enrollment_state(self):
        pieces = HT.code_pieces(REPO / "config" / "arm9" / "delinks.txt")[".text"]
        edits = HT.absorption_edits(pieces)
        by_stem = {stem: edit for (_k, _s, _e, stem), edit in zip(pieces, edits) if stem}
        expected = {stem: list(edit) for stem, _start, _end, edit in ARM9_HALFWORD_ROWS}
        enrolled = sorted(stem for stem in expected if stem in by_stem)
        if not enrolled:
            self.assertFalse(any(trim for row in edits for trim in row))
            return
        for stem in enrolled:
            self.assertEqual(by_stem[stem], expected[stem])


if __name__ == "__main__":
    unittest.main()
