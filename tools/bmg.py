#!/usr/bin/env python3
"""Show or search SM64DS's western BMG-shaped message banks.

Unlike stock BMG readers, this uses the repository's SM64DS-specific parser for
the byte-reversed GSEM1gmb/1FNI format, one-byte font, control commands, and
multi-byte controller glyphs.  The default bank is the extracted English ROM
asset; --file can point at any compatible bank.

Examples:
    python tools/bmg.py show 0x123
    python tools/bmg.py show 291 --language frn --meta
    python tools/bmg.py search "save" --language eng
    python tools/bmg.py search "<icon:A>" --file path/to/msg_data_eng.bin
"""
from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys

import message_bank as MB


REPO = pathlib.Path(__file__).resolve().parent.parent
MESSAGE_DIR = REPO / "extracted" / "dsd" / "files" / "data" / "message"
LANGUAGES = {
    "eng": "msg_data_eng.bin",
    "frn": "msg_data_frn.bin",
    "gmn": "msg_data_gmn.bin",
    "itl": "msg_data_itl.bin",
    "spn": "msg_data_spn.bin",
}


class BMGError(ValueError):
    pass


def message_id(value: str) -> int:
    try:
        result = int(value, 0)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid message ID: {value!r}") from exc
    if not 0 <= result <= 0xFFFF:
        raise argparse.ArgumentTypeError("message ID must be between 0 and 0xffff")
    return result


def nonnegative(value: str) -> int:
    try:
        result = int(value, 0)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(f"invalid count: {value!r}") from exc
    if result < 0:
        raise argparse.ArgumentTypeError("count must not be negative")
    return result


def resolve_bank(file: pathlib.Path | None, language: str) -> pathlib.Path:
    path = file if file is not None else MESSAGE_DIR / LANGUAGES[language]
    if not path.is_file():
        if file is None:
            raise BMGError(
                f"{path} not found; extract the ROM filesystem or pass --file"
            )
        raise BMGError(f"message bank not found: {path}")
    return path


def load_bank(path: pathlib.Path) -> dict:
    return MB.parse_bank(path.read_bytes(), path.name)


def message_at(document: dict, index: int) -> dict:
    messages = document.get("messages")
    if not isinstance(messages, list):
        raise BMGError("message document has no messages list")
    if index >= len(messages):
        raise BMGError(
            f"message ID 0x{index:04x} is out of range for {len(messages)} messages"
        )
    return messages[index]


def search_messages(document: dict, query: str, *, case_sensitive: bool = False,
                    regex: bool = False) -> list[dict]:
    if not query:
        raise BMGError("search query must not be empty")
    flags = 0 if case_sensitive else re.IGNORECASE
    pattern = re.compile(query if regex else re.escape(query), flags)
    return [message for message in document.get("messages", [])
            if pattern.search(message["text"])]


def add_bank_options(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--file", type=pathlib.Path,
                        help="bank path; overrides --language")
    parser.add_argument("--language", choices=sorted(LANGUAGES), default="eng",
                        help="extracted western bank (default: eng)")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    commands = parser.add_subparsers(dest="command", required=True)

    show = commands.add_parser("show", help="print one decoded message")
    show.add_argument("id", type=message_id, help="message ID in decimal or 0x hex")
    add_bank_options(show)
    show.add_argument("--meta", action="store_true",
                      help="show bank, index, and four-byte INF1 metadata")
    show.add_argument("--json", action="store_true", help="write structured output")

    search = commands.add_parser("search", help="find text in decoded messages")
    search.add_argument("query", help="literal text to find (or regex with --regex)")
    add_bank_options(search)
    search.add_argument("--case-sensitive", action="store_true")
    search.add_argument("--regex", action="store_true")
    search.add_argument("--limit", type=nonnegative, default=0,
                        help="maximum results; zero means unlimited")
    search.add_argument("--json", action="store_true", help="write structured output")
    return parser


def cmd_show(args) -> int:
    path = resolve_bank(args.file, args.language)
    message = message_at(load_bank(path), args.id)
    if args.json:
        print(json.dumps({
            "bank": str(path),
            "index": message["index"],
            "info_hex": message["info_hex"],
            "text": message["text"],
        }, ensure_ascii=False, indent=2))
    else:
        if args.meta:
            print(f"# {path.name}[0x{message['index']:04x}] info={message['info_hex']}")
        print(message["text"])
    return 0


def cmd_search(args) -> int:
    path = resolve_bank(args.file, args.language)
    matches = search_messages(load_bank(path), args.query,
                              case_sensitive=args.case_sensitive, regex=args.regex)
    if args.limit:
        matches = matches[:args.limit]
    if not matches:
        print(f"no messages matched {args.query!r} in {path.name}", file=sys.stderr)
        return 1
    if args.json:
        print(json.dumps({
            "bank": str(path),
            "query": args.query,
            "matches": matches,
        }, ensure_ascii=False, indent=2))
    else:
        for message in matches:
            text = message["text"].replace("\r", "\\r").replace("\n", "\\n")
            print(f"0x{message['index']:04x}: {text}")
    return 0


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return cmd_show(args) if args.command == "show" else cmd_search(args)
    except (BMGError, MB.MessageBankError, OSError, re.error) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
