import pathlib
import struct
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tu_object_diff as T


def inv(symbols=None, relocs=None, sections=None):
    """Build an inventory without needing a compiler or an ELF file."""
    return dict(symbols=dict(symbols or {}), relocs=dict(relocs or {}),
                sections=dict(sections or {}))


def sym(size=4, info=18, home=".text", body=b"\x00\x00\xa0\xe1"):
    return (size, info, home, body)


class DigestTests(unittest.TestCase):
    def test_identical_inventories_share_a_digest(self):
        a = inv({"f": sym()}, {".rel.text": [(0, 1)]}, {".text": (1, b"abcd", None)})
        b = inv({"f": sym()}, {".rel.text": [(0, 1)]}, {".text": (1, b"abcd", None)})
        self.assertEqual(T.digest(a), T.digest(b))

    def test_changed_instruction_bytes_change_the_digest(self):
        a = inv({"f": sym(body=b"\x00\x00\xa0\xe1")})
        b = inv({"f": sym(body=b"\x01\x00\xa0\xe1")})
        self.assertNotEqual(T.digest(a), T.digest(b))

    def test_relocation_entries_are_part_of_the_digest(self):
        a = inv(relocs={"f (.rela.text)": [(0, 1, "callee_a", 0)]})
        b = inv(relocs={"f (.rela.text)": [(0, 1, "callee_b", 0)]})
        self.assertNotEqual(T.digest(a), T.digest(b))

    def test_same_named_relocation_groups_do_not_collapse(self):
        # mwccarm emits one .rela.text per function; keying by section name
        # alone would hide every group but the last
        a = inv(relocs={"f (.rela.text)": [(0, 1, "x", 0)],
                        "g (.rela.text)": [(0, 1, "y", 0)]})
        b = inv(relocs={"f (.rela.text)": [(0, 1, "CHANGED", 0)],
                        "g (.rela.text)": [(0, 1, "y", 0)]})
        self.assertNotEqual(T.digest(a), T.digest(b))


class NumberedSymbolTests(unittest.TestCase):
    def test_recognises_the_four_shapes_mwccarm_emits(self):
        found = T.numbered_symbols(inv({
            "@1024": sym(),
            "__arraydtor$954": sym(),
            "sEnterTable$170": sym(),
            "_ZN6Player20Frame$2583Player_cppD1Ev": sym(),
            "_ZN6Player3FooEv": sym(),
            "data_0209f318": sym(),
        }))
        self.assertEqual(found, ["@1024", "_ZN6Player20Frame$2583Player_cppD1Ev",
                                 "__arraydtor$954", "sEnterTable$170"])

    def test_a_plain_at_sign_is_not_numbered(self):
        self.assertEqual(T.numbered_symbols(inv({"@notanumber": sym()})), [])


class CompareTests(unittest.TestCase):
    def test_no_findings_when_identical(self):
        a = inv({"f": sym()}, {"f (.rela.text)": [(0, 1, "g", 0)]},
                {"f (.text)": (1, b"abcd", None)})
        self.assertEqual(T.compare(a, dict(a, symbols=dict(a["symbols"]))), [])

    def test_a_new_numbered_symbol_is_reported_first(self):
        before = inv({"f": sym()})
        after = inv({"f": sym(body=b"\x99\x00\xa0\xe1"),
                     "_ZN6Player20Frame$2583Player_cppD1Ev": sym()})
        findings = T.compare(before, after)
        self.assertEqual(findings[0][0], "numbered-symbol")
        self.assertIn("Frame$2583Player_cpp", findings[0][1])
        # the ordinary byte change is still reported, just after it
        self.assertIn("symbol-changed", [k for k, _ in findings])

    def test_a_renumbered_symbol_reports_both_sides(self):
        findings = T.compare(inv({"@1024": sym()}), inv({"@1023": sym()}))
        kinds = [k for k, _ in findings]
        self.assertEqual(kinds[:2], ["numbered-symbol", "numbered-symbol"])
        joined = " ".join(t for _, t in findings)
        self.assertIn("@1023", joined)
        self.assertIn("@1024", joined)

    def test_symbol_size_change_is_described(self):
        findings = T.compare(inv({"f": sym(size=0x10, body=b"a" * 16)}),
                             inv({"f": sym(size=0x18, body=b"a" * 16)}))
        self.assertEqual(len(findings), 1)
        self.assertIn("size 0x10 -> 0x18", findings[0][1])

    def test_relocation_count_change_is_described(self):
        findings = T.compare(inv(relocs={"f (.rela.text)": [(0, 1, "g", 0)]}),
                             inv(relocs={"f (.rela.text)": [(0, 1, "g", 0), (4, 1, "h", 0)]}))
        self.assertEqual(findings[0][0], "reloc")
        self.assertIn("1 -> 2 entries", findings[0][1])

    def test_retargeted_relocation_is_reported_first_and_names_both_sides(self):
        # the blind spot match.compare has: same offset, different destination
        findings = T.compare(inv(relocs={"f (.rela.text)": [(0x20, 1, "right_callee", 0)]}),
                             inv(relocs={"f (.rela.text)": [(0x20, 1, "wrong_callee", 0)]}))
        self.assertEqual(findings[0][0], "reloc-destination")
        self.assertIn("right_callee -> wrong_callee", findings[0][1])
        self.assertIn("+0x20", findings[0][1])

    def test_a_destination_change_outranks_a_new_numbered_symbol(self):
        before = inv({"f": sym()}, {"f (.rela.text)": [(0, 1, "a", 0)]})
        after = inv({"f": sym(), "@99": sym()}, {"f (.rela.text)": [(0, 1, "b", 0)]})
        self.assertEqual([k for k, _ in T.compare(before, after)][:2],
                         ["reloc-destination", "numbered-symbol"])

    def test_addend_only_change_is_not_called_a_retarget(self):
        findings = T.compare(inv(relocs={"f (.rela.text)": [(0, 1, "g", 0)]}),
                             inv(relocs={"f (.rela.text)": [(0, 1, "g", 4)]}))
        self.assertEqual(findings[0][0], "reloc")
        self.assertIn("same destinations", findings[0][1])

    def test_nobits_section_size_change_is_described(self):
        findings = T.compare(inv(sections={".bss": (T.SHT_NOBITS, b"", 0x10)}),
                             inv(sections={".bss": (T.SHT_NOBITS, b"", 0x20)}))
        self.assertEqual(findings[0][0], "section")
        self.assertIn("nobits size 16 -> 32", findings[0][1])


class WordDiffTests(unittest.TestCase):
    def test_reports_the_offset_of_each_differing_word(self):
        before = inv({"f": sym(body=struct.pack("<III", 1, 2, 3))})
        after = inv({"f": sym(body=struct.pack("<III", 1, 9, 3))})
        len_a, len_b, rows = T.word_diff(before, after, "f")
        self.assertEqual((len_a, len_b), (12, 12))
        self.assertEqual(rows, [(4, 2, 9)])

    def test_raises_when_the_symbol_is_one_sided(self):
        with self.assertRaises(T.ObjectDiffError):
            T.word_diff(inv({"f": sym()}), inv({"g": sym()}), "f")


class ElfParsingTests(unittest.TestCase):
    def test_rejects_a_non_elf_blob(self):
        with self.assertRaises(T.ObjectDiffError):
            T.parse_sections(b"not an object at all")

    def test_rejects_a_truncated_header(self):
        with self.assertRaises(T.ObjectDiffError):
            T.parse_sections(b"\x7fELF" + b"\x00" * 8)

    def test_read_cstr_rejects_an_unterminated_string(self):
        with self.assertRaises(T.ObjectDiffError):
            T.read_cstr(b"abc", 0, 0)

    def test_read_cstr_reads_at_base_plus_offset(self):
        self.assertEqual(T.read_cstr(b"\0.text\0.data\0", 0, 1), ".text")


if __name__ == "__main__":
    unittest.main()
