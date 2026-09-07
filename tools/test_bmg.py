import contextlib
import io
import pathlib
import struct
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import bmg as BMG  # noqa: E402
import message_bank as MB  # noqa: E402


def synthetic_bank() -> bytes:
    messages = [
        MB.encode_message("HELLO\nWORLD"),
        MB.encode_message("Press <icon:A> to save"),
        MB.encode_message("NO MATCH HERE"),
    ]
    header = bytearray(b"GSEM1gmb" + b"\0" * 24)
    padding = b"\0" * 8
    inf_size = 0x10 + len(messages) * 8 + len(padding)
    inf = bytearray(b"1FNI" + struct.pack("<IHHI", inf_size, len(messages), 0x40, 0))
    entries = bytearray()
    text = bytearray(b"\xff")
    for index, message in enumerate(messages):
        entries += struct.pack("<I", len(text)) + bytes((index, 0, 2, 0))
        text += message
    dat = bytearray(b"1TAD" + b"\0" * 4)
    result = header + inf + entries + padding + dat + text
    result += b"\xff" * (-len(result) % 32)
    struct.pack_into("<I", result, 8, len(result) + 1)
    struct.pack_into("<I", result, 0x20 + inf_size + 4,
                     len(result) - (0x20 + inf_size) + 1)
    return bytes(result)


class BMGTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.bank = pathlib.Path(self.temp.name) / "msg_data_eng.bin"
        self.bank.write_bytes(synthetic_bank())

    def tearDown(self):
        self.temp.cleanup()

    def run_main(self, *args):
        stdout = io.StringIO()
        stderr = io.StringIO()
        with contextlib.redirect_stdout(stdout), contextlib.redirect_stderr(stderr):
            code = BMG.main([*args, "--file", str(self.bank)])
        return code, stdout.getvalue(), stderr.getvalue()

    def test_show_accepts_hex_id_and_prints_decoded_text(self):
        code, output, error = self.run_main("show", "0x1")
        self.assertEqual(code, 0)
        self.assertEqual(output, "Press <icon:A> to save\n")
        self.assertEqual(error, "")

    def test_show_reads_the_retail_lz77_wrapper(self):
        import ndspy.lz10

        self.bank.write_bytes(b"LZ77" + ndspy.lz10.compress(synthetic_bank()))
        code, output, error = self.run_main("show", "0")
        self.assertEqual(code, 0)
        self.assertEqual(output, "HELLO\nWORLD\n")
        self.assertEqual(error, "")

    def test_search_is_case_insensitive_and_single_line(self):
        code, output, error = self.run_main("search", "hello")
        self.assertEqual(code, 0)
        self.assertEqual(output, "0x0000: HELLO\\nWORLD\n")
        self.assertEqual(error, "")

    def test_regex_search_and_limit(self):
        code, output, _error = self.run_main(
            "search", "(save|match)", "--regex", "--limit", "1"
        )
        self.assertEqual(code, 0)
        self.assertEqual(output.count("0x"), 1)
        self.assertIn("save", output)

    def test_out_of_range_id_is_an_error(self):
        code, _output, error = self.run_main("show", "0xffff")
        self.assertEqual(code, 1)
        self.assertIn("out of range", error)


if __name__ == "__main__":
    unittest.main()
