import base64
import pathlib
import struct
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import message_bank as MB  # noqa: E402


def synthetic_bank() -> bytes:
    messages = [
        MB.encode_message("HELLO\nWORLD"),
        MB.encode_message("Press <icon:A><cmd:FE05030002>"),
    ]
    header = bytearray(b"GSEM1gmb" + b"\0" * 24)
    inf_padding = b"\0" * 16
    inf_size = 0x10 + len(messages) * 8 + len(inf_padding)
    inf = bytearray(b"1FNI" + struct.pack("<IHHI", inf_size, len(messages), 0x40, 0))
    entries = bytearray()
    text = bytearray(b"\xff")
    for i, message in enumerate(messages):
        entries += struct.pack("<I", len(text)) + bytes((i, 0, 2, 0))
        text += message
    dat = bytearray(b"1TAD" + b"\0" * 4)
    result = header + inf + entries + inf_padding + dat + text
    result += b"\xff" * (-len(result) % 32)
    struct.pack_into("<I", result, 8, len(result) + 1)
    dat_offset = 0x20 + inf_size
    struct.pack_into("<I", result, dat_offset + 4, len(result) - dat_offset + 1)
    return bytes(result)


class MessageBankTests(unittest.TestCase):
    def test_text_codec_is_lossless_for_controls_icons_and_unknowns(self):
        raw = bytes.fromhex("114d2df4f5fdfe05030002d0ff")
        text = MB.decode_message(raw)
        self.assertEqual(text, "H a<icon:A>\n<cmd:FE05030002><glyph:D0>")
        self.assertEqual(MB.encode_message(text), raw)

    def test_uncompressed_bank_round_trips_exactly(self):
        raw = synthetic_bank()
        document = MB.parse_bank(raw, "synthetic.bin")
        self.assertEqual(MB.build_bank(document), raw)
        self.assertEqual(document["messages"][0]["text"], "HELLO\nWORLD")

    def test_edit_rebuilds_offsets_and_remains_parseable(self):
        raw = synthetic_bank()
        document = MB.parse_bank(raw)
        document["messages"][0]["text"] = "A MUCH LONGER LINE"
        rebuilt = MB.build_bank(document)
        reparsed = MB.parse_bank(rebuilt)
        self.assertEqual(reparsed["messages"][0]["text"], "A MUCH LONGER LINE")
        self.assertEqual(reparsed["messages"][1]["text"],
                         "Press <icon:A><cmd:FE05030002>")

    def test_bad_command_length_is_rejected(self):
        with self.assertRaises(MB.MessageBankError):
            MB.encode_message("<cmd:FE0701>")

    def test_message_count_change_is_rejected(self):
        document = MB.parse_bank(synthetic_bank())
        document["messages"].pop()
        with self.assertRaises(MB.MessageBankError):
            MB.build_bank(document)


if __name__ == "__main__":
    unittest.main()
