"""Build a local NitroFS asset catalog and find source references to it.

The ROM and generated outputs are intentionally local-only.  By default they land
under build/, which is gitignored:

    python tools/asset_catalog.py generate sm64.nds
    python tools/asset_catalog.py references

The generated C headers are deliberately not wired into matched source yet.  They
are stable naming aids and a review surface; adopting constants belongs in a
separately verified source/layout change.
"""
from __future__ import annotations

import argparse
import collections
import csv
import pathlib
import re
import sys
from dataclasses import dataclass


REPO = pathlib.Path(__file__).resolve().parent.parent
DEFAULT_MANIFEST = REPO / "build" / "assets" / "files.tsv"
DEFAULT_HANDLES = REPO / "build" / "assets" / "handles.tsv"
DEFAULT_FILE_HEADER = REPO / "build" / "generated" / "NitroFileId.h"
DEFAULT_HANDLE_HEADER = REPO / "build" / "generated" / "AssetHandle.h"
DEFAULT_REFERENCES = REPO / "build" / "assets" / "references.tsv"
DEFAULT_CANDIDATES = REPO / "build" / "assets" / "rename-candidates.tsv"
DEFAULT_LAYOUT_CANDIDATES = REPO / "build" / "assets" / "layout-candidates.tsv"

# These two facts are independently visible in the target image and its matched
# initializer, func_ov000_020aa420.  The path/pointer validation below turns a
# wrong-version ROM into a hard failure instead of a plausible-looking catalog.
OV000_HANDLE_TABLE_ADDRESS = 0x020BD4B8
OV000_HANDLE_COUNT = 0x80A


KIND_BY_SUFFIX = {
    ".bca": "animation",
    ".bmd": "model",
    ".btp": "texture-sequence",
    ".kcl": "collision",
    ".narc": "archive",
    ".sdat": "sound-archive",
    ".bin": "data",
}

PAYLOAD_TYPE_BY_SUFFIX = {
    ".bca": "BCA_File",
    ".bmd": "BMD_File",
    ".btp": "BTP_File",
    ".kcl": "KCL_File",
    ".narc": "NARC_File",
    ".sdat": "SDAT_File",
    ".bin": "binary data",
}

ANONYMOUS_DATA_RE = re.compile(r"data(?:_ov[0-9]+)?_[0-9a-fA-F]{8}")
FIELD_OWNER_RE = re.compile(
    r"(?P<base>[A-Za-z_][A-Za-z0-9_]*)\s*\+\s*"
    r"(?P<offset>0[xX][0-9a-fA-F]+|[0-9]+)"
)

REFERENCE_CALLS = {
    "LoadFile": 0,
    "LoadFileAt": 0,
    "LoadCompressedFileAt": 0,
    "_ZN5Model14LoadAndSetFileEtii": 1,
    "_ZN13SharedFilePtr9ConstructEj": 1,
    "SharedFilePtr_Construct_TexSeq": 1,
    # Matched constructor veneers.  Their recovered prototypes sometimes omit
    # the second argument, but retail forwards it to func_02017e0c.
    "func_020178cc": 1,
    "func_02017a24": 1,
    "func_02017ae4": 1,
    "func_02017acc": 1,
    "func_02017b4c": 1,
    "func_02017e48": 1,
    "func_02017e0c": 1,
}


@dataclass(frozen=True)
class Asset:
    file_id: int
    path: str
    size: int

    @property
    def suffix(self) -> str:
        return pathlib.PurePosixPath(self.path).suffix.lower()

    @property
    def kind(self) -> str:
        return KIND_BY_SUFFIX.get(self.suffix, "file")


@dataclass(frozen=True)
class AssetHandle:
    handle: int
    file_id: int
    path: str
    size: int

    @property
    def suffix(self) -> str:
        return pathlib.PurePosixPath(self.path).suffix.lower()

    @property
    def kind(self) -> str:
        return KIND_BY_SUFFIX.get(self.suffix, "file")


def constant_base(path: str, prefix: str = "FILE_ID") -> str:
    """Return a deterministic C identifier retaining the full path and suffix."""
    value = re.sub(r"[^A-Za-z0-9]+", "_", path).strip("_").upper()
    if not value or value[0].isdigit():
        value = "FILE_" + value
    return prefix + "_" + value


def constants_for(entries, value_attr: str = "file_id",
                  prefix: str = "FILE_ID") -> dict[int, str]:
    """Assign stable names, suffixing only actual sanitizer collisions."""
    bases = {}
    for entry in entries:
        bases.setdefault(constant_base(entry.path, prefix), []).append(entry)
    result = {}
    for base, group in bases.items():
        for entry in group:
            value = getattr(entry, value_attr)
            result[value] = (
                base if len(group) == 1 else f"{base}_ID_{value:04X}"
            )
    return result


def handles_from_overlay(overlay, assets: list[Asset], *,
                         table_address: int = OV000_HANDLE_TABLE_ADDRESS,
                         count: int = OV000_HANDLE_COUNT) -> list[AssetHandle]:
    """Read the game's handle -> path pointer table from decompressed overlay 0."""
    by_path = {asset.path: asset for asset in assets}
    table_offset = table_address - overlay.ramAddress
    table_size = count * 4
    if table_offset < 0 or table_offset + table_size > len(overlay.data):
        raise ValueError("overlay 0 does not contain the expected asset-handle table")

    handles = []
    for handle in range(count):
        entry_offset = table_offset + handle * 4
        pointer = int.from_bytes(overlay.data[entry_offset:entry_offset + 4], "little")
        string_offset = pointer - overlay.ramAddress
        if string_offset < 0 or string_offset >= len(overlay.data):
            raise ValueError(f"asset handle 0x{handle:04x} has an invalid path pointer")
        end = overlay.data.find(0, string_offset)
        if end < 0:
            raise ValueError(f"asset handle 0x{handle:04x} has an unterminated path")
        try:
            asset_path = bytes(overlay.data[string_offset:end]).decode("ascii")
        except UnicodeDecodeError as exc:
            raise ValueError(f"asset handle 0x{handle:04x} has a non-ASCII path") from exc
        asset = by_path.get(asset_path)
        if asset is None:
            raise ValueError(
                f"asset handle 0x{handle:04x} names a missing NitroFS file: {asset_path}"
            )
        handles.append(AssetHandle(handle, asset.file_id, asset.path, asset.size))
    return handles


def catalogs_from_rom(path: pathlib.Path) -> tuple[list[Asset], list[AssetHandle]]:
    try:
        import ndspy.rom
    except ImportError:
        sys.exit("ndspy not installed. Run: pip install ndspy")

    rom = ndspy.rom.NintendoDSRom.fromFile(str(path))
    assets = []
    for file_id, payload in enumerate(rom.files):
        name = rom.filenames.filenameOf(file_id)
        if name is not None:
            assets.append(Asset(file_id, name.replace("\\", "/"), len(payload)))
    try:
        overlay = rom.loadArm9Overlays()[0]
    except KeyError as exc:
        raise ValueError("ROM has no ARM9 overlay 0") from exc
    return assets, handles_from_overlay(overlay, assets)


def write_manifest(path: pathlib.Path, assets: list[Asset]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f, delimiter="\t", lineterminator="\n")
        writer.writerow(("file_id", "hex_id", "path", "kind", "size"))
        for asset in assets:
            writer.writerow(
                (asset.file_id, f"0x{asset.file_id:04x}", asset.path,
                 asset.kind, asset.size)
            )


def read_manifest(path: pathlib.Path) -> list[Asset]:
    with path.open(encoding="utf-8", newline="") as f:
        rows = csv.DictReader(f, delimiter="\t")
        return [Asset(int(row["file_id"]), row["path"], int(row["size"]))
                for row in rows]


def write_handles(path: pathlib.Path, handles: list[AssetHandle]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f, delimiter="\t", lineterminator="\n")
        writer.writerow(("handle", "hex_handle", "file_id", "hex_file_id",
                         "path", "kind", "size"))
        for entry in handles:
            writer.writerow((entry.handle, f"0x{entry.handle:04x}", entry.file_id,
                             f"0x{entry.file_id:04x}", entry.path, entry.kind,
                             entry.size))


def read_handles(path: pathlib.Path) -> list[AssetHandle]:
    with path.open(encoding="utf-8", newline="") as f:
        rows = csv.DictReader(f, delimiter="\t")
        return [AssetHandle(int(row["handle"]), int(row["file_id"]),
                            row["path"], int(row["size"])) for row in rows]


def write_header(path: pathlib.Path, entries, *, value_attr: str, prefix: str,
                 enum_name: str, description: str) -> None:
    names = constants_for(entries, value_attr, prefix)
    path.parent.mkdir(parents=True, exist_ok=True)
    guard = "SM64DS_GENERATED_" + re.sub(r"[^A-Za-z0-9]", "_", enum_name).upper() + "_H"
    lines = [
        "/* Generated by tools/asset_catalog.py from a user-supplied ROM.",
        f" * {description}",
        " * Local build artifact: do not commit this file or ROM-derived data.",
        " */",
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        f"enum {enum_name} {{",
    ]
    for entry in entries:
        value = getattr(entry, value_attr)
        lines.append(f"    {names[value]} = 0x{value:04x},")
    lines.extend(("};", "", "#endif", ""))
    path.write_text("\n".join(lines), encoding="utf-8")


def _balanced_calls(text: str):
    names = "|".join(re.escape(name) for name in REFERENCE_CALLS)
    pattern = re.compile(rf"\b({names})\s*\(")
    for match in pattern.finditer(text):
        depth = 1
        i = match.end()
        in_string = None
        escaped = False
        while i < len(text) and depth:
            char = text[i]
            if in_string:
                if escaped:
                    escaped = False
                elif char == "\\":
                    escaped = True
                elif char == in_string:
                    in_string = None
            elif char in "\"'":
                in_string = char
            elif char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
            i += 1
        if depth == 0:
            yield match.group(1), match.start(), text[match.end():i - 1]


def _split_args(args: str) -> list[str]:
    out = []
    start = 0
    depth = 0
    in_string = None
    escaped = False
    for i, char in enumerate(args):
        if in_string:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == in_string:
                in_string = None
        elif char in "\"'":
            in_string = char
        elif char in "([{":
            depth += 1
        elif char in ")]}" and depth:
            depth -= 1
        elif char == "," and depth == 0:
            out.append(args[start:i].strip())
            start = i + 1
    out.append(args[start:].strip())
    return out


def _integer_literal(value: str) -> int | None:
    value = value.strip()
    match = re.fullmatch(r"(?:\([^)]+\)\s*)?(0[xX][0-9a-fA-F]+|[0-9]+)[uUlL]*", value)
    return int(match.group(1), 0) if match else None


def suggested_owner_name(owner: str, asset) -> str:
    """Return a review candidate, never an automatic rename."""
    if not re.fullmatch(r"&?(?:data|ov\w+)_?[A-Za-z0-9_]*", owner.strip()):
        return ""
    stem = pathlib.PurePosixPath(asset.path).stem
    words = [word for word in re.split(r"[^A-Za-z0-9]+", stem) if word]
    camel = "".join(word[:1].upper() + word[1:] for word in words)
    suffix = {
        "model": "ModelFile",
        "animation": "AnimationFile",
        "collision": "CollisionFile",
        "texture-sequence": "TextureSequenceFile",
        "archive": "ArchiveFile",
        "sound-archive": "SoundArchiveFile",
        "data": "DataFile",
    }.get(asset.kind, "File")
    return "g" + camel + suffix


def suggested_field_name(asset) -> str:
    global_name = suggested_owner_name("data_00000000", asset)
    return "m" + global_name[1:] if global_name else ""


def anonymous_owner_symbol(owner: str) -> str:
    """Extract a bare anonymous data symbol from a simple owner expression."""
    matches = ANONYMOUS_DATA_RE.findall(owner)
    if len(matches) != 1:
        return ""
    remainder = owner.replace(matches[0], "")
    # Permit address-of and C-style casts, but no indexing or pointer arithmetic.
    remainder = re.sub(r"\([^()]+\)", "", remainder)
    return matches[0] if not remainder.strip(" \t&*") else ""


def field_owner(owner: str) -> tuple[str, int] | None:
    """Recognize a simple recovered object-plus-offset SharedFilePtr field."""
    value = owner.strip()
    while True:
        stripped = re.sub(r"^\([^()]+\)\s*", "", value)
        if stripped == value:
            break
        value = stripped
    match = FIELD_OWNER_RE.fullmatch(value)
    if not match:
        return None
    return match.group("base"), int(match.group("offset"), 0)


def scan_references(src_root: pathlib.Path,
                    handles: list[AssetHandle]) -> list[dict[str, str]]:
    by_id = {asset.handle: asset for asset in handles}
    rows = []
    for path in sorted((*src_root.rglob("*.c"), *src_root.rglob("*.cpp"))):
        text = path.read_text(encoding="utf-8", errors="replace")
        for callee, offset, raw_args in _balanced_calls(text):
            args = _split_args(raw_args)
            arg_index = REFERENCE_CALLS[callee]
            if arg_index >= len(args):
                continue
            raw_id = _integer_literal(args[arg_index])
            if raw_id is None:
                continue
            asset = by_id.get(raw_id)
            line = text.count("\n", 0, offset) + 1
            owner = args[0].strip() if arg_index > 0 and args else ""
            rows.append({
                "source": path.relative_to(REPO).as_posix(),
                "line": str(line),
                "callee": callee,
                "raw_id": f"0x{raw_id:04x}",
                "status": "runtime-handle" if asset else "encoded-or-unresolved",
                "path": asset.path if asset else "",
                "owner": owner,
                "suggested_owner": suggested_owner_name(owner, asset) if asset else "",
            })
    return rows


def write_references(path: pathlib.Path, rows: list[dict[str, str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fields = ("source", "line", "callee", "raw_id", "status", "path",
              "owner", "suggested_owner")
    with path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, delimiter="\t", fieldnames=fields,
                                lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def _source_files(src_root: pathlib.Path) -> list[pathlib.Path]:
    return sorted((*src_root.rglob("*.c"), *src_root.rglob("*.cpp")))


def symbol_consumers(src_root: pathlib.Path, symbols: set[str]) -> dict[str, set[str]]:
    """Find source consumers for candidate symbols in one linear source scan."""
    consumers = {symbol: set() for symbol in symbols}
    if not symbols:
        return consumers
    for path in _source_files(src_root):
        text = path.read_text(encoding="utf-8", errors="replace")
        found = set(ANONYMOUS_DATA_RE.findall(text)) & symbols
        if not found:
            continue
        source = path.relative_to(REPO).as_posix()
        for symbol in found:
            consumers[symbol].add(source)
    return consumers


def build_rename_candidates(rows: list[dict[str, str]], src_root: pathlib.Path) -> list[dict[str, str]]:
    """Aggregate references into review-only global and struct-field candidates."""
    resolved = [row for row in rows if row["status"] == "runtime-handle" and row["path"]]
    global_groups: dict[str, list[dict[str, str]]] = collections.defaultdict(list)
    field_rows = []
    for row in resolved:
        symbol = anonymous_owner_symbol(row["owner"])
        if symbol:
            global_groups[symbol].append(row)
        elif field_owner(row["owner"]):
            field_rows.append(row)

    consumers = symbol_consumers(src_root, set(global_groups))
    provisional = []
    for symbol, group in sorted(global_groups.items()):
        paths = sorted({row["path"] for row in group})
        handles = sorted({row["raw_id"] for row in group})
        sources = sorted({row["source"] for row in group})
        assets = [AssetHandle(int(row["raw_id"], 0), 0, row["path"], 0) for row in group]
        asset = assets[0]
        suggested = suggested_owner_name(symbol, asset) if len(paths) == 1 else ""
        blocker = "" if len(paths) == 1 else "owner maps to multiple asset paths"
        provisional.append({
            "candidate_kind": "global",
            "current_name": symbol,
            "suggested_name": suggested,
            "confidence": "high" if suggested else "blocked",
            "container_type": "SharedFilePtr",
            "payload_type": PAYLOAD_TYPE_BY_SUFFIX.get(asset.suffix, "unknown"),
            "asset_kind": asset.kind if len(paths) == 1 else "mixed",
            "asset_paths": "; ".join(paths),
            "runtime_handles": "; ".join(handles),
            "references": str(len(group)),
            "initializer_sources": "; ".join(sources),
            "consumer_sources": "; ".join(sorted(consumers[symbol])),
            "evidence": (
                "literal runtime handle resolves to one asset and initializes this owner"
                if len(paths) == 1 else
                "the same owner is initialized with different resolved assets"
            ),
            "blocker": blocker,
        })

    name_counts = collections.Counter(
        row["suggested_name"] for row in provisional if row["suggested_name"]
    )
    for row in provisional:
        if row["suggested_name"] and name_counts[row["suggested_name"]] > 1:
            row["confidence"] = "medium"
            row["blocker"] = (
                f"suggested name is shared by {name_counts[row['suggested_name']]} owners"
            )

    fields = []
    for row in sorted(field_rows, key=lambda item: (item["source"], int(item["line"]))):
        base, offset = field_owner(row["owner"])
        asset = AssetHandle(int(row["raw_id"], 0), 0, row["path"], 0)
        fields.append({
            "candidate_kind": "field",
            "current_name": f"{base}+0x{offset:x}",
            "suggested_name": suggested_field_name(asset),
            "confidence": "medium",
            "container_type": "SharedFilePtr",
            "payload_type": PAYLOAD_TYPE_BY_SUFFIX.get(asset.suffix, "unknown"),
            "asset_kind": asset.kind,
            "asset_paths": asset.path,
            "runtime_handles": row["raw_id"],
            "references": "1",
            "initializer_sources": f"{row['source']}:{row['line']}",
            "consumer_sources": "",
            "evidence": "object-plus-offset field is initialized from a resolved runtime handle",
            "blocker": "confirm the recovered base object's class and field layout",
        })
    return provisional + fields


def write_rename_candidates(path: pathlib.Path, rows: list[dict[str, str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fields = (
        "candidate_kind", "current_name", "suggested_name", "confidence",
        "container_type", "payload_type", "asset_kind", "asset_paths",
        "runtime_handles", "references", "initializer_sources",
        "consumer_sources", "evidence", "blocker",
    )
    with path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, delimiter="\t", fieldnames=fields,
                                lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def resource_owner_from_source(source: str) -> tuple[str, str] | None:
    """Return (owner, evidence kind) for a named resource consumer source."""
    actor_match = re.match(r"src/actors/([^/]+)/", source)
    if actor_match:
        return actor_match.group(1), "actor-directory"

    stem = pathlib.PurePosixPath(source).stem
    mangled = re.match(r"_ZN([0-9]+)", stem)
    if not mangled:
        return None
    length = int(mangled.group(1))
    start = mangled.end()
    owner = stem[start:start + length]
    remainder = stem[start + length:]
    if len(owner) != length:
        return None
    if not (remainder.startswith("13InitResourcesE") or
            remainder.startswith("16CleanupResourcesE")):
        return None
    return owner, "mangled-resource-method"


def build_layout_candidates(rows: list[dict[str, str]],
                            rename_rows: list[dict[str, str]]) -> list[dict[str, str]]:
    """Use resource-global consumers to suggest homes for static initializers."""
    consumers = {
        row["current_name"]: row["consumer_sources"].split("; ")
        for row in rename_rows if row["candidate_kind"] == "global"
    }
    by_source: dict[str, list[dict[str, str]]] = collections.defaultdict(list)
    for row in rows:
        if (row["status"] == "runtime-handle" and row["path"] and
                pathlib.PurePosixPath(row["source"]).name.startswith("__sinit_")):
            by_source[row["source"]].append(row)

    output = []
    for source, group in sorted(by_source.items()):
        if source.startswith("src/actors/"):
            continue
        owner_symbols = sorted(filter(None, {
            anonymous_owner_symbol(row["owner"]) for row in group
        }))
        consumer_sources = sorted({
            consumer
            for symbol in owner_symbols
            for consumer in consumers.get(symbol, [])
            if consumer != source
        })
        owner_evidence = [
            evidence for consumer in consumer_sources
            if (evidence := resource_owner_from_source(consumer))
        ]
        actors = sorted({owner for owner, _kind in owner_evidence})
        evidence_kinds = {kind for _owner, kind in owner_evidence}
        asset_dirs = sorted({
            str(pathlib.PurePosixPath(row["path"]).parent) for row in group
        })
        filename = pathlib.PurePosixPath(source).name
        suggested = f"src/actors/{actors[0]}/{filename}" if len(actors) == 1 else ""
        if len(actors) == 1:
            confidence = (
                "high" if "actor-directory" in evidence_kinds else "medium"
            )
            blocker = ""
            evidence = (
                "resource globals are consumed from one existing actor directory"
                if confidence == "high" else
                "resource globals are consumed by one mangled InitResources/CleanupResources class"
            )
        elif actors:
            confidence = "blocked"
            blocker = "resource globals are shared by multiple actor directories"
            evidence = "consumer locations disagree on one actor owner"
        else:
            confidence = "review"
            blocker = "no named actor consumer found"
            evidence = "asset directories identify a subsystem but not a C++ owner"
        output.append({
            "current_path": source,
            "suggested_path": suggested,
            "confidence": confidence,
            "actor": "; ".join(actors),
            "asset_directories": "; ".join(asset_dirs),
            "owners": "; ".join(owner_symbols),
            "consumer_sources": "; ".join(consumer_sources),
            "evidence": evidence,
            "blocker": blocker,
        })
    return output


def write_layout_candidates(path: pathlib.Path, rows: list[dict[str, str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fields = (
        "current_path", "suggested_path", "confidence", "actor",
        "asset_directories", "owners", "consumer_sources", "evidence", "blocker",
    )
    with path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, delimiter="\t", fieldnames=fields,
                                lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def cmd_generate(args) -> None:
    rom = pathlib.Path(args.rom)
    if not rom.is_file():
        sys.exit(f"ROM not found: {rom}")
    try:
        assets, handles = catalogs_from_rom(rom)
    except ValueError as exc:
        sys.exit(f"error: {exc}")
    write_manifest(args.manifest, assets)
    write_handles(args.handles, handles)
    write_header(args.file_header, assets, value_attr="file_id",
                 prefix="NITRO_FILE_ID", enum_name="NitroFileId",
                 description="Raw NitroFS IDs; these are not game-code asset handles.")
    write_header(args.handle_header, handles, value_attr="handle",
                 prefix="ASSET_HANDLE", enum_name="AssetHandle",
                 description="Game-code handles translated through overlay 0 at runtime.")
    print(f"Cataloged {len(assets):,} named NitroFS files and {len(handles):,} runtime handles")
    kinds = collections.Counter(asset.kind for asset in assets)
    print("  kinds    : " + ", ".join(
        f"{kind}={count:,}" for kind, count in sorted(kinds.items())
    ))
    print(f"  manifest: {args.manifest}")
    print(f"  handles : {args.handles}")
    print(f"  headers : {args.file_header}, {args.handle_header}")


def resolve_queries(queries: list[str], handles: list[AssetHandle],
                    references: list[dict[str, str]]) -> list[tuple[str, list]]:
    """Answer handle numbers, path fragments, and owner symbols from the catalog.

    An integer literal is always a runtime handle, never a NitroFS file ID; the
    two number spaces disagree and guessing between them invents asset names.
    """
    by_handle = {entry.handle: entry for entry in handles}
    results = []
    for query in queries:
        value = _integer_literal(query)
        if value is not None:
            entry = by_handle.get(value)
            results.append((query, [entry] if entry else []))
            continue
        needle = query.lower()
        matches = [entry for entry in handles if needle in entry.path.lower()]
        if not matches:
            owners = {row["raw_id"] for row in references
                      if needle in row["owner"].lower() and row["owner"]}
            matches = [by_handle[handle] for handle in
                       sorted(_integer_literal(raw) or -1 for raw in owners)
                       if handle in by_handle]
        results.append((query, matches))
    return results


def cmd_resolve(args) -> None:
    if not args.handles.is_file():
        sys.exit(f"Handle catalog not found: {args.handles}\nRun the generate command first.")
    handles = read_handles(args.handles)
    references = []
    if args.references.is_file():
        with args.references.open(encoding="utf-8", newline="") as f:
            references = list(csv.DictReader(f, delimiter="\t"))
    names = constants_for(handles, value_attr="handle", prefix="ASSET_HANDLE")
    by_handle_refs = collections.defaultdict(list)
    for row in references:
        if row["status"] == "runtime-handle":
            by_handle_refs[_integer_literal(row["raw_id"])].append(row)

    unresolved = 0
    for query, matches in resolve_queries(args.query, handles, references):
        if not matches:
            value = _integer_literal(query)
            if value is not None and value >= 0x8000:
                print(f"{query}: not a runtime handle; values >= 0x8000 bypass the "
                      f"overlay 0 table and stay encoded-or-unresolved")
            else:
                print(f"{query}: no match")
            unresolved += 1
            continue
        for entry in matches[:args.limit]:
            print(f"{entry.handle} (0x{entry.handle:04x})  {entry.path}")
            print(f"    kind={entry.kind} size={entry.size:,} "
                  f"nitro_file_id={entry.file_id} (0x{entry.file_id:04x})")
            print(f"    {names[entry.handle]}")
            for row in by_handle_refs.get(entry.handle, [])[:args.limit]:
                owner = row["owner"] or "-"
                suggested = row["suggested_owner"]
                label = f"{owner} -> {suggested}" if suggested else owner
                print(f"    used by {row['source']}:{row['line']} "
                      f"{row['callee']}  {label}")
        if len(matches) > args.limit:
            print(f"    ... {len(matches) - args.limit:,} more matches "
                  f"(raise --limit to see them)")
    if unresolved:
        sys.exit(1)


def cmd_references(args) -> None:
    if not args.handles.is_file():
        sys.exit(f"Handle catalog not found: {args.handles}\nRun the generate command first.")
    handles = read_handles(args.handles)
    rows = scan_references(args.src, handles)
    write_references(args.output, rows)
    candidates = build_rename_candidates(rows, args.src)
    write_rename_candidates(args.candidates, candidates)
    layouts = build_layout_candidates(rows, candidates)
    write_layout_candidates(args.layouts, layouts)
    direct = sum(row["status"] == "runtime-handle" for row in rows)
    high = sum(row["confidence"] == "high" for row in candidates)
    medium = sum(row["confidence"] == "medium" for row in candidates)
    blocked = sum(row["confidence"] == "blocked" for row in candidates)
    layout_high = sum(row["confidence"] == "high" for row in layouts)
    layout_medium = sum(row["confidence"] == "medium" for row in layouts)
    print(f"Found {len(rows):,} literal loader references ({direct:,} runtime handles)")
    print(f"  report: {args.output}")
    print(f"  names : {args.candidates} ({high:,} high, {medium:,} medium, {blocked:,} blocked)")
    print(f"  layout: {args.layouts} ({layout_high:,} high, {layout_medium:,} medium)")


def main(argv=None) -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)

    generate = commands.add_parser("generate", help="write the local manifest and C header")
    generate.add_argument("rom", help="user-supplied .nds ROM")
    generate.add_argument("--manifest", type=pathlib.Path, default=DEFAULT_MANIFEST)
    generate.add_argument("--handles", type=pathlib.Path, default=DEFAULT_HANDLES)
    generate.add_argument("--file-header", type=pathlib.Path, default=DEFAULT_FILE_HEADER)
    generate.add_argument("--handle-header", type=pathlib.Path, default=DEFAULT_HANDLE_HEADER)
    generate.set_defaults(func=cmd_generate)

    references = commands.add_parser("references", help="resolve literal asset IDs in src/")
    references.add_argument("--handles", type=pathlib.Path, default=DEFAULT_HANDLES)
    references.add_argument("--src", type=pathlib.Path, default=REPO / "src")
    references.add_argument("--output", type=pathlib.Path, default=DEFAULT_REFERENCES)
    references.add_argument("--candidates", type=pathlib.Path, default=DEFAULT_CANDIDATES)
    references.add_argument("--layouts", type=pathlib.Path,
                            default=DEFAULT_LAYOUT_CANDIDATES)
    references.set_defaults(func=cmd_references)

    resolve = commands.add_parser(
        "resolve",
        help="look up runtime handles by number, path fragment, or owner symbol")
    resolve.add_argument("query", nargs="+",
                         help="a handle literal (1570, 0x622), a path fragment "
                              "(kb1_ball), or an owner symbol (data_ov044_02111680)")
    resolve.add_argument("--handles", type=pathlib.Path, default=DEFAULT_HANDLES)
    resolve.add_argument("--references", type=pathlib.Path, default=DEFAULT_REFERENCES)
    resolve.add_argument("--limit", type=int, default=10,
                         help="maximum matches printed per query (default 10)")
    resolve.set_defaults(func=cmd_resolve)

    args = parser.parse_args(argv)
    args.func(args)


if __name__ == "__main__":
    main()
