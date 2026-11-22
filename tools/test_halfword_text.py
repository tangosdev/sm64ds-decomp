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
        pieces = HT.code_pieces(REPO / "config" / "arm9" / "delinks.txt")[".text"]
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


def _elf_text(size, align):
    """A relocatable ARM ELF whose only code section is ``.text``."""
    shstr = b"\0.text\0.shstrtab\0"
    text_off = 52
    shstr_off = text_off + size
    shoff = shstr_off + len(shstr)
    header = struct.pack(
        "<16sHHIIIIIHHHHHH",
        b"\x7fELF\x01\x01\x01" + b"\x00" * 9,
        1, 40, 1, 0, 0, shoff, 0, 52, 0, 0, 40, 3, 2)
    def sheader(name, kind, flags, offset, sec_size, sec_align):
        return struct.pack("<IIIIIIIIII", name, kind, flags, 0, offset,
                           sec_size, 0, 0, sec_align, 0)
    body = b"".join((
        header,
        b"\x00" * size,
        shstr,
        sheader(0, 0, 0, 0, 0, 0),
        sheader(1, 1, 6, text_off, size, align),
        sheader(7, 3, 0, shstr_off, len(shstr), 1),
    ))
    elf = ELFFile(io.BytesIO(body))
    sec = elf.get_section_by_name(".text")
    if sec.header["sh_size"] != size or sec.header["sh_addralign"] != align:
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
            for name, size, align in names:
                (objects / name).write_bytes(_elf_text(size, align))
            (root / "objects.txt").write_text(
                "\n".join(str(objects / name) for name, _size, _align in names) + "\n",
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
            rendered = lcf.read_bytes().decode("latin-1")
            self.assertEqual(rendered, (
                "    .arm9 : {\n"
                "        ALIGNALL(4);\n"
                "        . = ALIGN(32);\n"
                "        ARM9_TEXT_START = .;\n"
                "        _dsd_gap@main_0.o(.text)\n"
                "        ALIGNALL(2);\n"
                "        stub.o(.text)\n"
                "        _dsd_gap@main_1.o(.text)\n"
                "        ALIGNALL(4);\n"
                "        . = ALIGN(4);\n"
                "        ARM9_TEXT_END = .;\n"
                "        . = ALIGN(1);\n"
                "        ARM9_EXCEPTION_START = .;\n"
                "    } > ARM9\n"
            ))
            self.assertLess(
                rendered.index("stub.o(.text)"),
                rendered.index("ARM9_TEXT_END = .;"))
            self.assertEqual(report["patchedObjects"], 2)
            self.assertEqual(report["alignmentSwitches"], 2)
            again = HT.apply(lcf, root / "objects.txt", config)
            self.assertEqual(lcf.read_bytes().decode("latin-1"), rendered)
            self.assertEqual(again["patchedObjects"], 0)
            self.assertEqual(again["alignmentSwitches"], 2)


if __name__ == "__main__":
    unittest.main()
