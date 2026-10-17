#!/usr/bin/env python3
"""Render recovered TUs on Pages from committed manifests and progress data.

python tools/tu_treemap.py --from-db chaos-db.json --out docs/tu-treemap.html

Each function contributes its bytes exactly once. Manifest membership describes
reconstruction; only existing sources owning complete delinks ranges count as
production-enrolled. Functions without membership remain visible as Unassigned.
No ROM, compiler, or inferred build/tu_map.json is needed.
"""
from __future__ import annotations

import argparse
import bisect
import collections
import json
import pathlib
import sys
from urllib.parse import quote

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import relocs as RL  # noqa: E402
import rombuild_check as RBC  # noqa: E402
import treemap as TM  # noqa: E402
import tu_manifest as TUM  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent
GITHUB = "https://github.com/tangosdev/sm64ds-decomp/blob/main/"
COLORS = {"production": "#3fa75a", "reconstructed": "#578fce", "unassigned": "#4a4d52"}


def number(value):
    return int(value, 0) if isinstance(value, str) else int(value)


def module_label(label):
    return "arm9" if label == "main" else label


def source_link(path, repo):
    """Only link existing repository files; never let an input escape the repo."""
    if not path:
        return None
    resolved = (repo / path).resolve()
    if not resolved.is_relative_to(repo.resolve()) or not resolved.is_file():
        return None
    return GITHUB + quote(resolved.relative_to(repo.resolve()).as_posix(), safe="/")


def complete_owners(functions, modules):
    """Join canonical function addresses to the existing complete-range reader."""
    by_module = collections.defaultdict(list)
    for f in functions:
        by_module[f["module"]].append(f)
    owners = {}
    for symbols, label in modules:
        delinks = symbols.with_name("delinks.txt")
        if not delinks.is_file():
            continue
        rows = sorted(by_module[label], key=lambda f: f["addr"])
        addresses = [f["addr"] for f in rows]
        for path, start, end in RBC.complete_entries(delinks):
            if not path.startswith("src/"):
                continue
            i = bisect.bisect_left(addresses, start)
            while i < len(rows) and rows[i]["addr"] < end:
                f = rows[i]
                if f["addr"] + f["size"] <= end:
                    key = (label, f["addr"])
                    if key in owners and owners[key] != path:
                        raise ValueError(f"conflicting production owners for {key}")
                    owners[key] = path
                i += 1
    return owners


def gather(db, entries, owners, repo=REPO):
    """Partition the DB universe, preserving unmatched and unassigned functions."""
    functions = {}
    for f in db["functions"]:
        key = (f["module"], f["addr"])
        if key in functions:
            raise ValueError(f"duplicate function address: {key}")
        functions[key] = f
    claims, groups = {}, []
    for entry in sorted(entries, key=lambda e: e["id"]):
        module = module_label(entry["module"])
        keys = {(module, number(f["address"])) for f in entry.get("functions", [])}
        present = keys & functions.keys()
        for key in present:
            if key in claims:
                raise ValueError(f"{key} claimed by both {claims[key]} and {entry['id']}")
            claims[key] = entry["id"]
        rows = [functions[key] for key in sorted(present)]
        promoted = entry.get("promoted_source")
        enrolled = bool(keys) and keys == present and bool(source_link(promoted, repo)) and all(
            owners.get(key) == promoted for key in keys)
        source = promoted if enrolled else entry.get("source")
        groups.append({
            "id": entry["id"], "label": pathlib.PurePosixPath(source).name if source else entry["id"],
            "module": module, "source": source, "sourceUrl": source_link(source, repo),
            "manifestUrl": GITHUB + quote("config/tu_manifest.d/" + TUM.slug(entry["id"]), safe="/"),
            "status": entry.get("status", "unknown"), "confidence": entry.get("boundary_confidence", "unknown"),
            "state": "production" if enrolled else "reconstructed", "missing": len(keys - present),
            "functions": rows,
        })
    # Manifests may license only .text while the same compiled source also owns
    # .init functions. Attach those by complete production ownership, never by
    # proximity, a class label, or a manifest's promoted status alone.
    by_source = collections.defaultdict(list)
    promoted_sources = {entry["id"]: entry.get("promoted_source") for entry in entries}
    for group in groups:
        promoted = promoted_sources[group["id"]]
        if promoted:
            by_source[(group["module"], promoted)].append(group)
    for key, f in functions.items():
        if key in claims:
            continue
        candidates = by_source.get((key[0], owners.get(key)), [])
        if len(candidates) == 1 and candidates[0]["state"] == "production":
            group = candidates[0]
            group["functions"].append(dict(f, membership="source"))
            claims[key] = group["id"]
    unassigned = collections.defaultdict(list)
    for key, f in functions.items():
        if key not in claims:
            unassigned[f["module"]].append(f)
    for module, rows in sorted(unassigned.items()):
        groups.append({"id": module + "/unassigned", "label": "Unassigned", "module": module,
                       "source": None, "sourceUrl": None, "manifestUrl": None, "status": "no manifest",
                       "confidence": "unknown", "state": "unassigned", "missing": 0, "functions": rows})
    for group in groups:
        rows = group["functions"]
        group["bytes"] = sum(f["size"] for f in rows)
        group["matchedBytes"] = sum(f["size"] for f in rows if f["matched"])
        group["matchedFunctions"] = sum(bool(f["matched"]) for f in rows)
        group["ownerAssignedFunctions"] = sum(f.get("membership") == "source" for f in rows)
        group["functions"] = [{"name": f["name"], "addr": f["addr"], "size": f["size"],
                               "matched": bool(f["matched"]), "membership": f.get(
                                   "membership", "unassigned" if group["state"] == "unassigned" else "manifest")}
                              for f in sorted(rows, key=lambda f: f["addr"])]
    return groups


def layout(groups):
    """Use the same byte-sized squarified layout as the function treemap."""
    by_module = collections.defaultdict(list)
    for i, group in enumerate(groups):
        by_module[group["module"]].append((i, group))
    items = [{"value": sum(g["bytes"] for _, g in rows), "module": module, "groups": rows}
             for module, rows in sorted(by_module.items())]
    regions, boxes = [], []
    for item, x, y, w, h in TM.squarify(items, 0, 0, 1600, 1000):
        regions.append({"module": item["module"], "x": x, "y": y, "w": w, "h": h})
        padding = min(3, w / 10, h / 10)
        header = min(22, h / 5)
        tus = [{"value": g["bytes"], "index": i} for i, g in item["groups"]]
        for tu, tx, ty, tw, th in TM.squarify(tus, x + padding, y + header,
                                             w - padding * 2, h - header - padding):
            boxes.append({"index": tu["index"], "x": tx, "y": ty, "w": tw, "h": th})
    return regions, boxes


def render(groups, out):
    regions, boxes = layout(groups)
    payload = json.dumps({"groups": groups, "regions": regions, "boxes": boxes, "colors": COLORS},
                         separators=(",", ":"), ensure_ascii=True).replace("<", "\\u003c")
    template = pathlib.Path(__file__).with_name("tu_treemap.html").read_text(encoding="utf-8")
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(template.replace("__TU_DATA__", payload), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--from-db", type=pathlib.Path, required=True)
    parser.add_argument("--out", type=pathlib.Path, default=REPO / "docs" / "tu-treemap.html")
    args = parser.parse_args()
    db = json.loads(args.from_db.read_text(encoding="utf-8"))
    groups = gather(db, TUM.load()["entries"], complete_owners(db["functions"], RL.module_universe()))
    render(groups, args.out)
    print(f"wrote {args.out}: {sum(g['state'] != 'unassigned' for g in groups)} TUs; "
          f"{sum(g['bytes'] for g in groups):,} code bytes")


if __name__ == "__main__":
    main()
