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
