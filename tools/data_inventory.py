"""Inventory global-data reconstruction work without treating claims as proof.

    python tools/data_inventory.py --json build/data-inventory.json

One record is a module-qualified configured address, not necessarily one C++
object: aliases, interior labels and unknown extents remain explicit. Section
coverage includes unnamed storage. Work groups are research routing suggestions,
not historical TU assignments. This tool reads no ROM and changes no enrollment.
"""
import argparse
import bisect
import collections
import json
import pathlib
import re

import data_evidence as DE
import relocs


REPO = pathlib.Path(__file__).resolve().parent.parent
TOKEN = re.compile(r"(?<![A-Za-z0-9_$@])(?:[A-Za-z_$][A-Za-z0-9_$]*|@[0-9]+)(?![A-Za-z0-9_$@])")
COMPLEX_TOKEN = re.compile(r"[A-Za-z_.$@][A-Za-z0-9_.$@]*")
PLACEHOLDER = re.compile(r"(?:data|reg)_(?:ov[0-9]+_)?[0-9a-fA-F]+$")
STORAGE_KINDS = {"data", "rodata", "bss"}


def containing_claims(index, module, start, end):
    return [c for c in index.claims[module]
            if c["start"] < end and start < c["end"]]


def symbol_inventory(index):
    records = []
    for module, addresses in sorted(index.addresses.items()):
        for start, aliases in sorted(addresses.items()):
            data = [r for r in aliases if not r["kind"].startswith("function")]
            if not data:
                continue
            sections = [s for s in index.sections[module]
                        if s["start"] <= start < s["end"]]
            section = sections[0] if len(sections) == 1 else None
            category = "outside-or-ambiguous-section"
            if section:
                category = ("global-storage" if section["kind"] in STORAGE_KINDS
                            else "code-embedded-data" if any(
                                r["kind"].startswith("data") for r in data)
                            else "code-label")
            window = None
            owners = []
            if section:
                boundaries = index.boundaries[module]
                pos = bisect.bisect_right(boundaries, start)
                end = min(section["end"], boundaries[pos] if pos < len(boundaries)
                          else section["end"])
                window = {"start": DE.address(start), "end": DE.address(end),
                          "bytes": end - start,
                          "basis": "next-configured-boundary-observation-only"}
                for claim in containing_claims(index, module, start, end):
                    owners.append({**claim, "start": DE.address(claim["start"]),
                                   "end": DE.address(claim["end"]),
                                   "section_matches": claim["section"] == section["name"],
                                   "covers_window": claim["start"] <= start
                                   and end <= claim["end"]})
            complete = [c for c in owners if c["complete"] and c["section_matches"]]
            if not section:
                status = "unresolved-section"
            elif not complete:
                status = "no-complete-claim"
            elif len(complete) != 1:
                status = "conflicting-complete-claims"
            elif not complete[0]["covers_window"]:
                status = "partial-window-claim"
            else:
                status = "complete-window-claimed"
            sizes = sorted({r["configured_size"] for r in data
                            if r["configured_size"] is not None})
            names = sorted(r["symbol"] for r in aliases)
            records.append({
                "id": f"{module}:{DE.address(start)}", "module": module,
                "address": DE.address(start), "aliases": names,
                "symbol_rows": [{"name": r["symbol"], "kind": r["kind"],
                                 "configured_size": r["configured_size"],
                                 "file": r["file"], "line": r["line"]}
                                for r in aliases],
                "category": category, "section": section["name"] if section else None,
                "window": window, "object_size": None,
                "configured_size_claims": sizes,
                "conflicting_sizes": len(sizes) > 1,
                "mixed_function_alias": len(data) != len(aliases),
                "naming": "placeholder-only" if all(PLACEHOLDER.fullmatch(n) for n in names)
                          else "existing-nonplaceholder-unreviewed",
                "declared_ownership": status, "claims": owners,
                "incoming_relocations": [], "outgoing_relocations": [],
                "ambiguous_incoming_relocations": [],
                "source_text_mentions": [], "manifest_claims": [],
                "interpretation": {"name": "unreviewed", "type": "unreviewed",
                                   "historical_owner": "unreviewed"},
                "verification": "not-run",
            })
    return records


def attach_references(index, records):
    by_module = collections.defaultdict(dict)
    for row in records:
        by_module[row["module"]][int(row["address"], 16)] = row

    def locate(module, at):
        if at in by_module[module]:
            return by_module[module][at]
        boundaries = index.boundaries.get(module, [])
        pos = bisect.bisect_right(boundaries, at) - 1
        if pos < 0:
            return None
        row = by_module[module].get(boundaries[pos])
        if row and row["window"] and at < int(row["window"]["end"], 16):
            return row
        return None

    unresolved = []
    for reloc in index.relocations:
        module, site = reloc["source_module"], reloc["site"]
        source_sections = [s for s in index.sections[module] if s["start"] <= site < s["end"]]
        source_claims = [c for c in containing_claims(index, module, site, site + 1)
                         if len(source_sections) == 1 and c["section"] == source_sections[0]["name"]]
        item = {**reloc, "site": DE.address(site), "target": DE.address(reloc["target"]),
                "source_context": index.context(module, site),
                "source_claims": [{"source": c["source"], "complete": c["complete"],
                                   "section": c["section"]} for c in source_claims]}
        source = locate(module, site)
        if source:
            source["outgoing_relocations"].append(item)
        target_module = reloc["target_module"]
        multiple = re.fullmatch(r"overlays\(([0-9,]+)\)", target_module)
        if multiple:
            candidates = []
            for number in sorted(set(map(int, multiple[1].split(",")))):
                target = locate(f"ov{number:03d}", reloc["target"])
                if target:
                    candidates.append(target["id"])
                    target["ambiguous_incoming_relocations"].append(item)
            unresolved.append({**item, "candidate_windows": candidates,
                               "resolution": "ambiguous-overlay-set"})
        elif target_module in index.sections:
            target = locate(target_module, reloc["target"])
            if target:
                target["incoming_relocations"].append({**item,
                    "target_offset": reloc["target"] - int(target["address"], 16),
                    "relation": "exact-address" if reloc["target"] == int(target["address"], 16)
                                else "inside-observation-window-not-proven-object"})
            else:
                context = index.context(target_module, reloc["target"])
                at = index.addresses[target_module].get(reloc["target"], [])
                is_code = (context and context["relation"] == "inside-configured-function") or any(
                    r["kind"].startswith("function") for r in at)
                if not is_code:
                    unresolved.append({**item, "candidate_windows": [],
                                       "resolution": "no-configured-data-window"})
        else:
            unresolved.append({**item, "candidate_windows": [],
                               "resolution": "unresolved-target-module"})
    return unresolved


def attach_text_and_manifests(index, records):
    by_name = collections.defaultdict(list)
    by_identity = collections.defaultdict(list)
    for row in records:
        for name in row["aliases"]:
            by_name[name].append(row)
            by_identity[row["module"], name].append(row)
    for folder in ("src", "include"):
        for path in sorted((index.repo / folder).rglob("*")):
            if not path.is_file() or path.suffix not in (".c", ".cpp", ".h", ".hpp"):
                continue
            text = index.read(path)
            for line_number, line in enumerate(text.splitlines(), 1):
                tokens = set(TOKEN.findall(line)) | set(COMPLEX_TOKEN.findall(line))
                for name in sorted(tokens & by_name.keys()):
                    for row in by_name[name]:
                        row["source_text_mentions"].append({
                            "file": index.relative(path), "line": line_number,
                            "symbol": name, "module_ambiguous": len(by_name[name]) > 1,
                            "meaning": "text-only-may-be-comment-or-declaration"})
    for path in sorted((index.repo / "config" / "tu_manifest.d").rglob("*.json")):
        manifest = json.loads(index.read(path))
        module = relocs.normalize_module(manifest.get("module", ""))
        for field in ("data", "rodata", "bss", "ctor", "init", "exception", "exceptix"):
            for item in manifest.get(field, []):
                if not isinstance(item, dict):
                    continue
                for row in by_identity.get((module, item.get("symbol")), []):
                    row["manifest_claims"].append({
                        "file": index.relative(path), "id": manifest.get("id"),
                        "source": manifest.get("promoted_source") or manifest.get("source"),
                        "status": manifest.get("status"), "section": field,
                        "claim": item, "verification": "not-reverified"})


def section_coverage(index):
    """Partition storage by claim endpoints; include bytes with no named symbol."""
    result = []
    for module, sections in sorted(index.sections.items()):
        for section in sections:
            if section["kind"] not in STORAGE_KINDS:
                continue
            lo, hi = section["start"], section["end"]
            claims = [c for c in index.claims[module] if c["complete"] and c["section"] == section["name"]
                      and c["start"] < hi and lo < c["end"]]
            points = sorted({lo, hi, *(max(lo, c["start"]) for c in claims),
                             *(min(hi, c["end"]) for c in claims)})
            intervals, totals = [], collections.Counter()
            for start, end in zip(points, points[1:]):
                providers = [c for c in claims if c["start"] <= start and end <= c["end"]]
                status = ("no-complete-claim" if not providers else
                          "single-complete-claim" if len(providers) == 1 else
                          "conflicting-complete-claims")
                totals[status] += end - start
                intervals.append({"start": DE.address(start), "end": DE.address(end),
                                  "bytes": end - start, "status": status,
                                  "sources": sorted(c["source"] for c in providers)})
            result.append({"module": module, "section": section["name"],
                           "kind": section["kind"], "bytes": hi - lo,
                           "coverage_bytes": dict(totals), "intervals": intervals,
                           "verification": "declared-only-not-linked-proof"})
    return result


def work_groups(records):
    groups = {}
    for row in records:
        if row["category"] not in ("global-storage", "code-embedded-data"):
            key, basis, source = (row["module"], "special-address-review"), "special-address-review", None
        else:
            owners = [c["source"] for c in row["claims"] if c["complete"] and c["covers_window"]
                      and c["section_matches"]]
            users = sorted({c["source"] for ref in row["incoming_relocations"]
                            if ref["source_module"] == row["module"]
                            for c in ref["source_claims"] if c["complete"]})
            if row["declared_ownership"] == "complete-window-claimed":
                source, basis = owners[0], "declared-complete-owner"
            elif row["declared_ownership"] == "no-complete-claim" and len(users) == 1:
                source, basis = users[0], "single-resolved-source-candidate-not-owner"
            else:
                source = None
                basis = ("ownership-conflict" if row["declared_ownership"] in
                         ("conflicting-complete-claims", "partial-window-claim") else
                         "shared-users" if len(users) > 1 else "unassigned")
            key = row["module"], source or basis
        group = groups.setdefault(key, {"module": row["module"], "source": source,
                                       "members": [], "routing_basis": collections.Counter(),
                                       "ambiguous_target_sites": 0, "unassigned_referrer_sites": 0,
                                       "cross_module_referrer_sites": 0,
                                       "historical_owner": "unreviewed"})
        ambiguous = len(row["ambiguous_incoming_relocations"])
        unassigned = sum(len([c for c in ref["source_claims"] if c["complete"]]) != 1
                         for ref in row["incoming_relocations"])
        external = sum(ref["source_module"] != row["module"] for ref in row["incoming_relocations"])
        group["members"].append({"id": row["id"], "basis": basis,
                                 "ambiguous_target_sites": ambiguous,
                                 "unassigned_referrer_sites": unassigned,
                                 "cross_module_referrer_sites": external})
        group["ambiguous_target_sites"] += ambiguous
        group["unassigned_referrer_sites"] += unassigned
        group["cross_module_referrer_sites"] += external
        group["routing_basis"][basis] += 1
    return [groups[key] for key in sorted(groups)]


def build_report(repo):
    repo = pathlib.Path(repo).resolve()
    index = DE.EvidenceIndex(repo)
    records = symbol_inventory(index)
    unresolved = attach_references(index, records)
    attach_text_and_manifests(index, records)
    coverage = section_coverage(index)
    groups = work_groups(records)
    counts = {
        "configured_nonfunction_rows": sum(not r["kind"].startswith("function")
                                           for rows in index.symbols.values() for r in rows),
        "distinct_module_addresses": len(records),
        "addresses_with_aliases": sum(len(r["aliases"]) > 1 for r in records),
        "categories": dict(collections.Counter(r["category"] for r in records)),
        "ownership": dict(collections.Counter(r["declared_ownership"] for r in records)),
        "naming": dict(collections.Counter(r["naming"] for r in records)),
        "research_groups": len(groups),
        "unresolved_relocations": len(unresolved),
    }
    # The receipt can be checked after a run; no stale build report is imported.
    changed = [name for name, digest in index.inputs.items()
               if not (repo / name).is_file() or DE.digest((repo / name).read_bytes()) != digest]
    if changed:
        raise ValueError("inventory inputs changed during collection: " + ", ".join(changed[:8]))
    tools = pathlib.Path(__file__).resolve().parent
    return {
        "schema_version": 1, "repository": DE.repository_state(repo), "counts": counts,
        "purpose": "Research backlog, not recovered object count or source acceptance",
        "input_sha256": dict(sorted(index.inputs.items())),
        "tool_sha256": {name: DE.digest((tools / name).read_bytes()) for name in
                        ("data_inventory.py", "data_evidence.py", "relocs.py", "enroll.py")},
        "records": records, "section_coverage": coverage, "work_groups": groups,
        "reference_inventory": {"unresolved": unresolved,
                                "missing_files": index.missing_relocations,
                                "link_time_constants": index.link_constants},
        "limits": [
            "Configured addresses include aliases and interior labels, not a recovered object census.",
            "Windows and source text mentions do not recover sizes, types or original names.",
            "A work group routes research; one resolved source referrer does not establish historical TU ownership.",
            "Complete claims have not been checked against compiler objects or a fresh link.",
            "BSS coverage measures storage, not ROM payload or startup behavior.",
            "No ROM bytes are read; binary access decoding and runtime tracing remain separate.",
            "Hashes bind files read and rechecked, not an atomic filesystem snapshot; freeze inputs when collecting.",
        ],
    }


def summary(report):
    counts = report["counts"]
    lines = ["Global-data backlog (static claims; compiled verification NOT RUN)",
             f"Configured nonfunction rows: {counts['configured_nonfunction_rows']}",
             f"Distinct module/address records: {counts['distinct_module_addresses']}"]
    for label in ("categories", "ownership", "naming"):
        lines.append(label + ": " + ", ".join(f"{key}={value}" for key, value in sorted(counts[label].items())))
    for kind in ("initialized", "bss"):
        totals = collections.Counter()
        for section in report["section_coverage"]:
            if (section["kind"] == "bss") == (kind == "bss"):
                totals.update(section["coverage_bytes"])
        lines.append(kind + " storage bytes: " + ", ".join(f"{k}={v}" for k, v in sorted(totals.items())))
    lines.append(f"Research groups: {counts['research_groups']}; unresolved relocation sites: {counts['unresolved_relocations']}")
    return "\n".join(lines)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json", type=pathlib.Path, required=True)
    parser.add_argument("--summary-json", type=pathlib.Path,
                        help="write a compact census and work-group list without full per-address evidence")
    args = parser.parse_args(argv)
    try:
        report = build_report(REPO)
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        if args.summary_json:
            if args.summary_json.resolve() == args.json.resolve():
                raise ValueError("summary and full inventory must use different output paths")
            compact = {k: report[k] for k in ("schema_version", "repository", "counts", "purpose", "limits")}
            compact["work_groups"] = [{k: v for k, v in g.items() if k != "members"}
                                      | {"address_count": len(g["members"])} for g in report["work_groups"]]
            compact["section_coverage"] = [{k: v for k, v in s.items() if k != "intervals"}
                                           for s in report["section_coverage"]]
            args.summary_json.parent.mkdir(parents=True, exist_ok=True)
            args.summary_json.write_text(json.dumps(compact, indent=2) + "\n", encoding="utf-8")
    except (ValueError, OSError) as error:
        parser.exit(2, f"data inventory: {error}\n")
    print(summary(report))
    print(f"Full inventory: {args.json}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
