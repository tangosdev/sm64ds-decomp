"""Read/write the TU manifest, which is a DIRECTORY of one file per entry.

`config/tu_manifest.d/` holds `_meta.json` (the schema version and the preamble
that used to sit at the top of the old single file) plus one file per translation
unit, laid out `<module>/<stem>.json` -- the same per-module shape as
`config/arm9/overlays/<module>/`. In memory it is still the single dict every
caller already expects -- `{"schema_version", "about", "entries": [...]}` -- so
this module is the only place that knows the on-disk shape.

`build/tu_map.json` currently finds 521 candidate TUs, and that is a lower bound:
modules it flags `under_segmented` (`main` at 118 functions per TU, `ov007` at
137) will cut into many more once they have class labels to cut on. Budget for
roughly 700-1000 entries, which is why they are not all in one directory.

WHY A DIRECTORY. It used to be one `config/tu_manifest.json` with an `entries`
list, and every TU PR appended one object to it. That made the manifest a
serialisation point: any two TU PRs conflicted, and every open one went DIRTY
the moment another landed. The conflict was never semantic -- the entries are
keyed by `id` and no two PRs touch the same one -- but JSON cannot use
.gitattributes' `merge=union` the way the JSONL ledgers next to it do, and
GitHub does not run the union driver server-side regardless. Splitting by id
makes the common case (a PR adds one TU) touch a file nothing else touches, so
those PRs stop conflicting with each other and can land in any order.

A single `.json` path still works wherever a root is accepted, which is what
`tubuild.py --manifest` points at for a scratch copy.
"""
from __future__ import annotations

import json
import pathlib
import re

REPO = pathlib.Path(__file__).resolve().parent.parent
DEFAULT_ROOT = REPO / "config" / "tu_manifest.d"
META_NAME = "_meta.json"

DEFAULT_ABOUT = ("Recovered translation-unit membership and licensed ranges. See "
                 "notes/translation-unit-reconstruction-plan.md section 6 for the entry "
                 "schema and section 13 for the invariants this file is meant to carry. "
                 "Nothing here is enrolled: no entry in this manifest affects the ROM "
                 "build, and config/**/delinks.txt remains the sole authority over what "
                 "is compiled.")

_UNSAFE = re.compile(r"[^A-Za-z0-9._+-]")


def slug(tu_id):
    """Relative path for an entry id: `ov002/CameraTag` -> `ov002/CameraTag.json`.

    One directory per module, mirroring `config/arm9/overlays/<module>/`. An id
    with no `/` lands at the top level. The tree is expected to reach roughly
    700-1000 entries once modules like `main` are properly segmented, so a flat
    directory would be unbrowsable; this keeps the biggest one in the low
    hundreds.

    Not required to be reversible -- every entry file carries its own `id`, and
    that field, never the filename, is the identity. `save` asserts the mapping
    is injective so two ids can never land on one file.
    """
    parts = [p for p in tu_id.split("/") if p not in ("", ".", "..")]
    if not parts:
        raise ValueError(f"entry id {tu_id!r} has no usable path")
    safe = [_UNSAFE.sub("_", p) for p in parts]
    # Anything below <module>/<stem> is flattened, so an id can never escape its
    # module directory or nest arbitrarily deep.
    if len(safe) > 2:
        safe = [safe[0], "-".join(safe[1:])]
    return "/".join(safe) + ".json"


def is_single_file(root):
    return pathlib.Path(root).suffix == ".json"


def exists(root=None):
    root = pathlib.Path(root or DEFAULT_ROOT)
    return root.is_file() if is_single_file(root) else root.is_dir()


def empty():
    return {"schema_version": 1, "about": DEFAULT_ABOUT, "entries": []}


def load(root=None):
    root = pathlib.Path(root or DEFAULT_ROOT)

    if is_single_file(root):
        if not root.is_file():
            return empty()
        return json.loads(root.read_text(encoding="utf-8"))

    if not root.is_dir():
        return empty()

    meta_path = root / META_NAME
    data = (json.loads(meta_path.read_text(encoding="utf-8"))
            if meta_path.is_file() else {"schema_version": 1, "about": DEFAULT_ABOUT})
    data.pop("entries", None)

    entries = []
    for path in sorted(root.rglob("*.json")):
        if path.name == META_NAME:
            continue
        entry = json.loads(path.read_text(encoding="utf-8"))
        if "id" not in entry:
            raise ValueError(f"{path} has no `id`")
        entries.append(entry)

    seen = {}
    for e in entries:
        if e["id"] in seen:
            raise ValueError(f"duplicate entry id {e['id']!r} in {root}")
        seen[e["id"]] = True

    # Sorted by id so the in-memory order does not depend on directory listing
    # order, which differs between filesystems.
    data["entries"] = sorted(entries, key=lambda e: e["id"])
    return data


def _dump(path, obj):
    path.write_text(json.dumps(obj, indent=2, ensure_ascii=False) + "\n",
                    encoding="utf-8", newline="\n")


def save(data, root=None):
    root = pathlib.Path(root or DEFAULT_ROOT)

    if is_single_file(root):
        _dump(root, data)
        return

    root.mkdir(parents=True, exist_ok=True)
    entries = data.get("entries", [])

    wanted = {}
    for entry in entries:
        name = slug(entry["id"])
        if name in wanted:
            raise ValueError(f"entry ids {wanted[name]!r} and {entry['id']!r} "
                             f"both map to {name}")
        wanted[name] = entry["id"]

    meta = {k: v for k, v in data.items() if k != "entries"}
    meta.setdefault("schema_version", 1)
    meta.setdefault("about", DEFAULT_ABOUT)
    _dump(root / META_NAME, meta)

    for entry in entries:
        path = root / slug(entry["id"])
        path.parent.mkdir(parents=True, exist_ok=True)
        _dump(path, entry)

    # An entry that was renamed or dropped must not linger as a stale file that
    # the next load would silently read back in.
    for path in sorted(root.rglob("*.json")):
        if path.name == META_NAME:
            continue
        if path.relative_to(root).as_posix() not in wanted:
            path.unlink()
    # ...nor as an empty module directory.
    for d in sorted(root.rglob("*"), reverse=True):
        if d.is_dir() and not any(d.iterdir()):
            d.rmdir()


def entry_path(tu_id, root=None):
    return pathlib.Path(root or DEFAULT_ROOT) / slug(tu_id)


def describe(root=None):
    """Short human-facing path, for messages that used to name the single file."""
    root = pathlib.Path(root or DEFAULT_ROOT)
    try:
        return root.relative_to(REPO).as_posix()
    except ValueError:
        return root.as_posix()


# Explicit compiler-local TEXT correspondence. These readers do no I/O and do
# not establish retail byte correctness; callers retain their normal byte gates.
_RENAME_NONTEXT_FIELDS = ("rodata", "init", "ctor", "data", "bss", "exception", "exceptix")
_OWNED_FUNCTION_KEYS = {"module", "address", "size", "function_ordinal", "raw_type", "raw_binding"}


def _rename_integer(value):
    if isinstance(value, bool):
        raise ValueError("boolean is not an integer fact")
    if isinstance(value, int):
        return value
    if isinstance(value, str):
        return int(value, 0)
    raise ValueError("integer fact required")


def function_rename_registry_rows(text, module):
    """All named homes, retaining duplicates and non-functions as refusals."""
    rows = []
    for line in text.splitlines():
        match = re.match(r"^(\S+)\s+kind:(\S+)\s+addr:(0x[0-9a-fA-F]+)\b", line)
        if not match:
            continue
        name, kind, address = match.groups()
        size = re.search(r"(?:^|,)size=(0x[0-9a-fA-F]+|[0-9]+)(?:,|\))", kind)
        rows.append({"symbol": name, "module": module, "address": int(address, 0),
                     "size": int(size.group(1), 0) if size else None,
                     "type": "STT_FUNC" if kind.startswith("function(") else "NON_FUNCTION"})
    return rows


def _owned_function_rename_errors(entry, row, symbols, sections, homes, mapping):
    old, new = row["from"], row["to"]
    fact = row.get("owned_function")
    if not isinstance(fact, dict) or set(fact) != _OWNED_FUNCTION_KEYS:
        return ["owned_function requires the exact module/address/size/ordinal/type/binding facts"]
    try:
        address = _rename_integer(fact["address"])
        size = _rename_integer(fact["size"])
        ordinal = _rename_integer(fact["function_ordinal"])
        module = fact["module"]
        if not isinstance(module, str) or not re.fullmatch(r"arm9|itcm|dtcm|ov[0-9]{3}", module):
            raise ValueError("canonical module required")
        if address < 0 or size <= 0 or ordinal < 0 or address + size > 0x100000000:
            raise ValueError("invalid function extent/ordinal")
        if module != entry.get("module") or fact["raw_type"] != "STT_FUNC" or fact["raw_binding"] != "STB_LOCAL":
            raise ValueError("owned local STT_FUNC in the entry's exact module required")
        functions = entry.get("functions", [])
        matches = [f for f in functions if f.get("symbol") == new]
        if len(matches) != 1:
            raise ValueError("canonical function must occur exactly once")
        target = matches[0]
        if old in {f.get("symbol") for f in functions}:
            raise ValueError("raw spelling is another configured canonical function")
        if not isinstance(row.get("evidence"), str) or not row["evidence"].strip():
            raise ValueError("owned function requires explicit nonempty evidence text")
        if (_rename_integer(target.get("address")), _rename_integer(target.get("size")),
                _rename_integer(target.get("ordinal"))) != (address, size, ordinal):
            raise ValueError("canonical function address/size/ordinal differs")
        if sum(_rename_integer(f.get("ordinal")) == ordinal for f in functions) != 1:
            raise ValueError("canonical function ordinal is not unique")
        for other in functions:
            if other is target:
                continue
            lo, length = _rename_integer(other.get("address")), _rename_integer(other.get("size"))
            if length <= 0 or max(address, lo) < min(address + size, lo + length):
                raise ValueError("configured function extents overlap or are invalid")
        text = [s for s in entry.get("sections", []) if s.get("name") == ".text"]
        if len(text) != 1 or not (_rename_integer(text[0]["start"]) <= address and
                                 address + size <= _rename_integer(text[0]["end"])):
            raise ValueError("full function extent is not in the unique configured TEXT span")
        expected = {"symbol": new, "module": module, "address": address, "size": size, "type": "STT_FUNC"}
        if homes.get(new) != [expected]:
            raise ValueError("canonical symbol requires one exact module/home/full-size function registry row")
        # Function correspondences may not form chains/cycles. Existing non-text
        # simultaneous @ overlaps are deliberately left to the original OI rules.
        if old == new or new in mapping or any(k != old and v == old for k, v in mapping.items()):
            raise ValueError("owned function correspondence overlaps another input/output")
        if any(s["name"] == new for s in symbols):
            raise ValueError("canonical function spelling already exists in input")
        emitted = [s for s in symbols if s["name"] == old]
        if len(emitted) != 1:
            raise ValueError("emitted function must occur exactly once")
        symbol = emitted[0]
        if symbol["type"] != "STT_FUNC" or symbol["binding"] != "STB_LOCAL" or symbol["size"] != size:
            raise ValueError("emitted function type/binding/full size differs")
        section = sections.get(symbol["section"])
        if section is None or section["type"] != "SHT_PROGBITS" or section["flags"] & 6 != 6:
            raise ValueError("emitted function must be in allocated executable PROGBITS")
        if symbol["value"] < 0 or symbol["value"] + size > section["size"]:
            raise ValueError("emitted function exceeds its physical section")
        for field in ("compiler_only_output", "externalized_output", "undefined_symbol_aliases"):
            for policy in entry.get(field, []) or []:
                if any(policy.get(k) in (old, new) for k in ("symbol", "canonical_symbol", "from", "to")):
                    raise ValueError("owned function conflicts with drop/externalization/import policy")
        return []
    except (ValueError, TypeError, KeyError, AttributeError) as exc:
        return [str(exc)]


def defined_symbol_rename_plan(entry, symbols=(), sections=None, homes=None):
    """One shared TB/RA admission rule; no rename, I/O, alias or home creation."""
    rows = entry.get("defined_symbol_renames", [])
    if rows is None:
        rows = []
    if not isinstance(rows, list):
        return {}, [], ["defined_symbol_renames must be a list"]
    owned = {r["symbol"] for field in _RENAME_NONTEXT_FIELDS
             for r in (entry.get(field, []) if isinstance(entry.get(field, []), list) else [])
             if isinstance(r, dict) and r.get("symbol")}
    mapping, requested, reasons = {}, [], []
    for ordinal, row in enumerate(rows):
        label = f"defined_symbol_renames[{ordinal}]"
        if not isinstance(row, dict):
            reasons.append(f"{label} is not an object")
            continue
        missing = [key for key in ("from", "to", "evidence") if not row.get(key)]
        if missing:
            reasons.append(f"{label} is missing {missing}")
            continue
        old, new = str(row["from"]), str(row["to"])
        if old in mapping:
            reasons.append(f"duplicate defined symbol rename for {old}")
            continue
        if "owned_function" not in row and new not in owned:
            reasons.append(f"{label} renames to {new}, which the manifest does not own")
            continue
        if "owned_function" in row and (not isinstance(row["from"], str) or not isinstance(row["to"], str) or new in owned):
            reasons.append(f"{label} requires an unambiguous function-only target")
            continue
        mapping[old] = new
        requested.append(dict(row))
    if len(set(mapping.values())) != len(mapping):
        reasons.append("canonical symbol aliases must be unique")
    for row in requested:
        if "owned_function" in row:
            reasons.extend(f"owned function {row['from']}: {reason}" for reason in
                           _owned_function_rename_errors(entry, row, symbols, sections or {}, homes or {}, mapping))
    return mapping, requested, reasons


def defined_function_preimage(entry, mapping):
    """Exact input population for RA, from a frozen simultaneous name map."""
    functions = {f["symbol"] for f in entry.get("functions", []) if isinstance(f, dict) and f.get("symbol")}
    inverse = {new: old for old, new in mapping.items()}
    return frozenset(inverse.get(name, name) for name in functions)


if __name__ == "__main__":
    import argparse
    import sys

    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--root", default=None)
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("list", help="print entry ids")
    exp = sub.add_parser("export", help="print the combined manifest as one JSON document")
    exp.add_argument("--out", default=None)
    mig = sub.add_parser("migrate", help="split a single-file manifest into --root")
    mig.add_argument("source")
    args = ap.parse_args()

    if args.cmd == "list":
        for e in load(args.root)["entries"]:
            print(e["id"])
    elif args.cmd == "export":
        text = json.dumps(load(args.root), indent=2, ensure_ascii=False) + "\n"
        if args.out:
            pathlib.Path(args.out).write_text(text, encoding="utf-8", newline="\n")
        else:
            sys.stdout.write(text)
    elif args.cmd == "migrate":
        src = pathlib.Path(args.source)
        data = json.loads(src.read_text(encoding="utf-8"))
        save(data, args.root)
        print(f"{src} -> {describe(args.root)}: {len(data.get('entries', []))} entries")
