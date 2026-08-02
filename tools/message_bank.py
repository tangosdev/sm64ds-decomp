"""Extract and rebuild SM64DS message banks without losing unknown bytes.

The JSON export uses readable text plus explicit tokens:

* a literal newline is the game's 0xFD line break;
* ``<cmd:FE...>`` preserves a complete length-prefixed command;
* ``<icon:A>`` (and similar names) preserves a multi-byte button glyph;
* ``<glyph:AB>`` preserves a byte with no safe textual spelling.

Unchanged JSON rebuilds to the exact original compressed bytes.  After an edit,
the rebuilt bank uses valid deterministic LZ10 compression.
"""
from __future__ import annotations

import argparse
import base64
import hashlib
import json
import pathlib
import re
import struct
import sys


FORMAT = "sm64ds-message-bank-v1"
TOKEN_RE = re.compile(r"<(cmd|icon|glyph):([^>]+)>")

BASIC_CHARS = {
    **{i: str(i) for i in range(10)},
    **{0x0A + i: chr(ord("A") + i) for i in range(26)},
    0x24: "┌", 0x25: "┘", 0x26: "?", 0x27: "!", 0x28: "~",
    0x29: ",", 0x2A: "“", 0x2B: "”",
    **{0x2D + i: chr(ord("a") + i) for i in range(26)},
    0x47: "-", 0x48: ".", 0x49: "'", 0x4A: ":", 0x4B: ";",
    0x4C: "&", 0x4D: " ", 0x4E: "/",
}
ICONS = {
    bytes.fromhex("eeef"): "C",
    bytes.fromhex("f0"): "S",
    bytes.fromhex("f1"): "s",
    bytes.fromhex("f2f3"): "D-PAD",
    bytes.fromhex("f4f5"): "A",
    bytes.fromhex("f6f7"): "B",
    bytes.fromhex("f8f9"): "X",
    bytes.fromhex("fafb"): "Y",
}
ICON_BYTES = {name: payload for payload, name in ICONS.items()}


class MessageBankError(ValueError):
    pass


def _ndspy_lz10():
    try:
        import ndspy.lz10
    except ImportError:
        sys.exit("ndspy not installed. Run: pip install ndspy")
    return ndspy.lz10


def unpack_file(raw: bytes) -> tuple[bytes, bool]:
    if raw.startswith(b"LZ77"):
        return _ndspy_lz10().decompress(raw[4:]), True
    if raw.startswith(b"GSEM1gmb"):
        return raw, False
    raise MessageBankError("not an SM64DS LZ77 or GSEM1gmb message bank")


def _extended_char(code: int) -> str | None:
    if not 0x50 <= code <= 0xCF:
        return None
    try:
        return bytes((code + 0x30,)).decode("cp1252")
    except UnicodeDecodeError:
        return None


def decode_message(payload: bytes) -> str:
    if not payload or payload[-1] != 0xFF:
        raise MessageBankError("message does not end in 0xFF")
    out = []
    i = 0
    while i < len(payload) - 1:
        code = payload[i]
        if code == 0xFD:
            out.append("\n")
            i += 1
        elif code == 0xFE:
            if i + 1 >= len(payload):
                raise MessageBankError("truncated 0xFE command")
            length = payload[i + 1]
            if length < 2 or i + length > len(payload) - 1:
                raise MessageBankError(f"invalid 0xFE command length {length}")
            command = payload[i:i + length]
            out.append(f"<cmd:{command.hex().upper()}>")
            i += length
        else:
            icon = next(((raw, name) for raw, name in ICONS.items()
                         if payload.startswith(raw, i)), None)
            if icon:
                raw, name = icon
                out.append(f"<icon:{name}>")
                i += len(raw)
                continue
            char = BASIC_CHARS.get(code)
            if char is None:
                char = _extended_char(code)
            if char is None or char in BASIC_CHARS.values():
                # Basic spellings are canonical.  Duplicate extended spellings
                # stay explicit so decode -> encode cannot silently change bytes.
                if char is None or code not in BASIC_CHARS:
                    out.append(f"<glyph:{code:02X}>")
                else:
                    out.append(char)
            else:
                out.append(char)
            i += 1
    return "".join(out)


def _char_encoder() -> dict[str, int]:
    result = {char: code for code, char in BASIC_CHARS.items()}
    for code in range(0x50, 0xD0):
        char = _extended_char(code)
        if char is not None and char not in result:
            result[char] = code
    return result


CHAR_ENCODER = _char_encoder()


def encode_message(text: str) -> bytes:
    out = bytearray()
    i = 0
    while i < len(text):
        if text[i] == "\n":
            out.append(0xFD)
            i += 1
            continue
        if text[i] == "<":
            token = TOKEN_RE.match(text, i)
            if token is None:
                raise MessageBankError(f"invalid token at character {i}")
            kind, value = token.groups()
            if kind == "cmd":
                try:
                    raw = bytes.fromhex(value)
                except ValueError as exc:
                    raise MessageBankError(f"invalid command hex at character {i}") from exc
                if len(raw) < 2 or raw[0] != 0xFE or raw[1] != len(raw):
                    raise MessageBankError("command token must start FE and contain its byte length")
                out.extend(raw)
            elif kind == "icon":
                if value not in ICON_BYTES:
                    raise MessageBankError(f"unknown icon {value!r}")
                out.extend(ICON_BYTES[value])
            else:
                try:
                    raw = bytes.fromhex(value)
                except ValueError as exc:
                    raise MessageBankError(f"invalid glyph hex at character {i}") from exc
                if len(raw) != 1 or raw[0] in (0xFD, 0xFE, 0xFF):
                    raise MessageBankError("glyph token must contain one non-control byte")
                out.extend(raw)
            i = token.end()
            continue
        code = CHAR_ENCODER.get(text[i])
        if code is None:
            raise MessageBankError(
                f"character {text[i]!r} at {i} is not in the SM64DS western font"
            )
        out.append(code)
        i += 1
    out.append(0xFF)
    return bytes(out)


def _u16(data: bytes, offset: int) -> int:
    return struct.unpack_from("<H", data, offset)[0]


def _u32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def parse_bank(raw: bytes, source_name: str = "") -> dict:
    data, compressed = unpack_file(raw)
    if len(data) < 0x38 or data[:8] != b"GSEM1gmb":
        raise MessageBankError("invalid message bank header")
    inf_offset = 0x20
    if data[inf_offset:inf_offset + 4] != b"1FNI":
        raise MessageBankError("missing INF1 section")
    inf_size = _u32(data, inf_offset + 4)
    count = _u16(data, inf_offset + 8)
    entries_offset = inf_offset + 0x10
    entries_end = entries_offset + count * 8
    dat_offset = inf_offset + inf_size
    if entries_end > dat_offset or data[dat_offset:dat_offset + 4] != b"1TAD":
        raise MessageBankError("invalid INF1 size or missing DAT1 section")
    text_base = dat_offset + 8

    messages = []
    previous_offset = -1
    for index in range(count):
        entry_offset = entries_offset + index * 8
        string_offset = _u32(data, entry_offset)
        if string_offset <= previous_offset:
            raise MessageBankError("message offsets must be strictly increasing")
        previous_offset = string_offset
        start = text_base + string_offset
        cursor = start
        while cursor < len(data):
            code = data[cursor]
            if code == 0xFF:
                cursor += 1
                break
            if code == 0xFE:
                if cursor + 1 >= len(data) or data[cursor + 1] < 2:
                    raise MessageBankError(f"invalid command in message {index}")
                cursor += data[cursor + 1]
            else:
                cursor += 1
        else:
            raise MessageBankError(f"unterminated message {index}")
        if index + 1 < count:
            next_offset = _u32(data, entry_offset + 8)
            if cursor != text_base + next_offset:
                raise MessageBankError(f"gap or overlap after message {index}")
        payload = data[start:cursor]
        messages.append({
            "index": index,
            "info_hex": data[entry_offset + 4:entry_offset + 8].hex(),
            "text": decode_message(payload),
        })

    first_offset = _u32(data, entries_offset) if count else 0
    last_end = text_base + first_offset
    if messages:
        last_end = text_base + previous_offset + len(encode_message(messages[-1]["text"]))
    return {
        "format": FORMAT,
        "source_name": source_name,
        "compressed": compressed,
        "source_sha256": hashlib.sha256(raw).hexdigest(),
        "uncompressed_sha256": hashlib.sha256(data).hexdigest(),
        "file_header_hex": data[:0x20].hex(),
        "inf_header_hex": data[inf_offset:entries_offset].hex(),
        "inf_padding_hex": data[entries_end:dat_offset].hex(),
        "dat_header_hex": data[dat_offset:text_base].hex(),
        "text_prefix_hex": data[text_base:text_base + first_offset].hex(),
        "text_suffix_hex": data[last_end:].hex(),
        "messages": messages,
        # Kept last so opening the JSON shows readable messages, not a large
        # preservation blob.  This is what makes an unchanged compressed build
        # byte-identical even though generic LZ10 compressors choose other matches.
        "original_file_base64": base64.b64encode(raw).decode("ascii"),
    }


def build_bank(document: dict) -> bytes:
    if document.get("format") != FORMAT:
        raise MessageBankError(f"unsupported format {document.get('format')!r}")
    messages = document.get("messages")
    if not isinstance(messages, list):
        raise MessageBankError("messages must be a list")

    header = bytearray.fromhex(document["file_header_hex"])
    inf_header = bytearray.fromhex(document["inf_header_hex"])
    inf_padding = bytes.fromhex(document["inf_padding_hex"])
    dat_header = bytearray.fromhex(document["dat_header_hex"])
    prefix = bytes.fromhex(document["text_prefix_hex"])
    if len(header) != 0x20 or len(inf_header) != 0x10 or len(dat_header) != 8:
        raise MessageBankError("invalid preserved header length")
    if len(messages) != _u16(inf_header, 8):
        raise MessageBankError("changing the message count is not supported")

    entries = bytearray()
    text_data = bytearray(prefix)
    for expected_index, message in enumerate(messages):
        if message.get("index") != expected_index:
            raise MessageBankError("message indices must be contiguous and zero-based")
        info = bytes.fromhex(message["info_hex"])
        if len(info) != 4:
            raise MessageBankError(f"message {expected_index} info_hex must be four bytes")
        entries.extend(struct.pack("<I", len(text_data)))
        entries.extend(info)
        text_data.extend(encode_message(message["text"]))

    inf_size = 0x10 + len(entries) + len(inf_padding)
    struct.pack_into("<I", inf_header, 4, inf_size)
    struct.pack_into("<H", inf_header, 8, len(messages))
    dat_offset = len(header) + inf_size
    body_without_tail = header + inf_header + entries + inf_padding + dat_header + text_data
    suffix = bytes.fromhex(document["text_suffix_hex"])
    result = bytearray(body_without_tail + suffix)
    if len(result) % 32:
        result.extend(b"\xff" * (-len(result) % 32))

    # The retail western banks declare one byte beyond the decompressed payload.
    # Preserve that quirk, but derive it from the source rather than hard-coding it.
    original_data, _ = unpack_file(base64.b64decode(document["original_file_base64"]))
    declared_delta = _u32(header, 8) - len(original_data)
    struct.pack_into("<I", result, 8, len(result) + declared_delta)
    struct.pack_into("<I", result, dat_offset + 4,
                     len(result) - dat_offset + declared_delta)

    uncompressed = bytes(result)
    original = base64.b64decode(document["original_file_base64"])
    if hashlib.sha256(uncompressed).hexdigest() == document.get("uncompressed_sha256"):
        return original
    if document.get("compressed"):
        return b"LZ77" + _ndspy_lz10().compress(uncompressed)
    return uncompressed


def cmd_extract(args) -> None:
    raw = args.bank.read_bytes()
    document = parse_bank(raw, args.bank.name)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(document, ensure_ascii=False, indent=2) + "\n",
                           encoding="utf-8")
    print(f"Extracted {len(document['messages']):,} messages to {args.output}")


def cmd_build(args) -> None:
    document = json.loads(args.document.read_text(encoding="utf-8"))
    raw = build_bank(document)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(raw)
    print(f"Built {len(raw):,} bytes at {args.output}")


def cmd_roundtrip(args) -> None:
    raw = args.bank.read_bytes()
    rebuilt = build_bank(parse_bank(raw, args.bank.name))
    if rebuilt != raw:
        sys.exit("round-trip mismatch")
    print(f"EXACT: {args.bank} ({len(raw):,} bytes)")


def main(argv=None) -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)

    extract = commands.add_parser("extract", help="write a lossless editable JSON file")
    extract.add_argument("bank", type=pathlib.Path)
    extract.add_argument("--output", "-o", type=pathlib.Path, required=True)
    extract.set_defaults(func=cmd_extract)

    build = commands.add_parser("build", help="build a bank from extracted JSON")
    build.add_argument("document", type=pathlib.Path)
    build.add_argument("--output", "-o", type=pathlib.Path, required=True)
    build.set_defaults(func=cmd_build)

    roundtrip = commands.add_parser("roundtrip", help="prove exact unchanged rebuilding")
    roundtrip.add_argument("bank", type=pathlib.Path)
    roundtrip.set_defaults(func=cmd_roundtrip)

    args = parser.parse_args(argv)
    try:
        args.func(args)
    except (MessageBankError, KeyError, ValueError) as exc:
        sys.exit(f"error: {exc}")


if __name__ == "__main__":
    main()
