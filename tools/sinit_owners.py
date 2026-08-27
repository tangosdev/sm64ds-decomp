#!/usr/bin/env python3
"""Rank the likely original translation-unit owner of every ``__sinit_*``.

The linker separates an input object's ``.text``, ``.init``, ``.ctor``, data and
BSS contributions, so an initializer's address is not a useful proximity signal.
This report instead follows the initializer's relocations and source references:

    __sinit -> constructed/touched global -> consuming source -> candidate TU

The output is deliberately advisory.  It never moves a source or edits config.
``proven`` is reserved for an initializer already licensed by a link-verified or
promoted TU manifest; module-wide sinit order is only ``medium`` evidence.

Typical use::

    python tools/rtti_extract.py --check
    python tools/rtti_vtables.py
    python tools/tu_map.py
    python tools/sinit_owners.py
    python tools/sinit_owners.py --module ov045 --show all
"""

from __future__ import annotations

import argparse
import collections
import csv
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import relocs as R  # noqa: E402
import srcpath as SP  # noqa: E402


REPO = pathlib.Path(__file__).resolve().parent.parent
DEFAULT_TU_MAP = REPO / "build" / "tu_map.json"
DEFAULT_ASSET_LAYOUT = REPO / "build" / "assets" / "layout-candidates.tsv"
DEFAULT_JSON = REPO / "build" / "sinit-owners.json"
DEFAULT_TSV = REPO / "build" / "sinit-owners.tsv"

CLASSIFICATIONS = ("proven", "high", "medium", "ambiguous", "count-only")
IDENT_RE = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*\b")
SYMBOL_RE = re.compile(
    r"^(\S+)\s+kind:([^\s(]+)(?:\(([^)]*)\))?\s+addr:0x([0-9a-fA-F]+)"
)
SIZE_RE = re.compile(r"(?:^|,)size=0x([0-9a-fA-F]+)")
REGISTER_CALL_RE = re.compile(
    r"(?:func_020731dc|__register_global_object)\s*\(\s*&?([A-Za-z_]\w*)"
)
ASSET_CALL_RE = re.compile(
    r"(?:func_02017acc|func_02017b4c|_ZN13SharedFilePtr9ConstructEj)"
    r"\s*\([^,]+,\s*(0x[0-9a-fA-F]+|\d+)"
)
ASSIGN_LHS_RE = re.compile(
    r"\b([A-Za-z_]\w*)(?:\s*(?:\.|->|\[)[^=;]*)?\s*=(?!=)"
)
KNOWN_INIT_FIRST_ARG_RE = re.compile(
    r"(?:func_02017acc|func_02017b4c|_ZN13SharedFilePtr9ConstructEj)"
    r"\s*\(\s*&?([A-Za-z_]\w*)"
)


def canonical_module(module: str) -> str:
    """Use the TU map's spelling (``main`` rather than reloc's ``arm9``)."""
    normalized = R.normalize_module(module)
    return "main" if normalized == "arm9" else normalized


def repo_relative(path: pathlib.Path, repo: pathlib.Path = REPO) -> str:
    return path.resolve().relative_to(repo.resolve()).as_posix()


def parse_symbol_file(path: pathlib.Path, module: str) -> list[dict]:
    rows = []
    for line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        match = SYMBOL_RE.match(line)
        if not match:
            continue
        attrs = match.group(3) or ""
        size_match = SIZE_RE.search(attrs)
        rows.append({
            "name": match.group(1),
            "kind": match.group(2),
            "module": canonical_module(module),
            "addr": int(match.group(4), 16),
            "size": int(size_match.group(1), 16) if size_match else 0,
        })
    return rows


def load_config(repo: pathlib.Path = REPO):
    """Return module metadata and alias-preserving symbol indexes."""
    modules = {}
    by_home = collections.defaultdict(list)
    by_name = collections.defaultdict(list)
    for symbols_path, label in R.module_universe(repo=repo):
        module = canonical_module(label)
        rows = parse_symbol_file(symbols_path, module)
        modules[module] = {
            "symbols": symbols_path,
            "relocs": symbols_path.parent / "relocs.txt",
            "rows": rows,
        }
        for row in rows:
            by_home[(module, row["addr"])].append(row)
            by_name[row["name"]].append(row)
    return modules, by_home, by_name


def load_tu_map(path: pathlib.Path) -> tuple[dict, dict, dict]:
    """Return module records, ``(module,symbol)->unit`` and ``id->unit``."""
    if not path.is_file():
        raise SystemExit(
            f"missing TU map: {path}\n"
            "Generate it with: python tools/rtti_extract.py --check; "
            "python tools/rtti_vtables.py; python tools/tu_map.py"
        )
    raw = json.loads(path.read_text(encoding="utf-8"))
    modules = raw.get("modules", {})
    under_segmented = {
        canonical_module(row["module"])
        for row in raw.get("meta", {}).get("under_segmented", [])
    }
    for raw_module, module_row in modules.items():
        module_row["under_segmented"] = canonical_module(raw_module) in under_segmented
    by_symbol = {}
    by_id = {}
    for raw_module, module_row in modules.items():
        module = canonical_module(raw_module)
        for index, unit in enumerate(module_row.get("units", [])):
            classes = list(unit.get("classes") or [])
            label = "+".join(classes) or f"{unit['start']}..{unit['end']}"
            unit_id = f"{module}:{index}"
            record = {
                "id": unit_id,
                "module": module,
                "index": index,
                "label": label,
                "display": f"{module}/{label}",
                "classes": classes,
                "start": unit["start"],
                "end": unit["end"],
                "functions": list(unit.get("functions") or []),
            }
            by_id[unit_id] = record
            for symbol in record["functions"]:
                by_symbol[(module, symbol)] = record
    return modules, by_symbol, by_id


def load_manifest_evidence(repo: pathlib.Path = REPO) -> dict[str, list[dict]]:
    """Curated initializer ownership from TU manifests.

    ``tu_owned_but_out_of_scope.init`` is strong ownership research but is not
    link proof.  A manifest that actually licenses an ``.init`` range only earns
    ``proven`` once its status says the whole TU is link-verified or promoted.
    """
    output = collections.defaultdict(list)
    root = repo / "config" / "tu_manifest.d"
    if not root.is_dir():
        return output
    proof_statuses = {"link-verified", "ready", "promoted"}
    for path in root.rglob("*.json"):
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            continue
        owner = data.get("id") or path.stem
        status = data.get("status", "")
        for row in data.get("functions", []):
            symbol = row.get("symbol", "")
            if symbol.startswith("__sinit_"):
                output[symbol].append({
                    "owner": owner,
                    "kind": "licensed-function",
                    "proven": status in proof_statuses,
                    "manifest": repo_relative(path, repo),
                })
        owned = data.get("tu_owned_but_out_of_scope") or {}
        for row in owned.get("init", []):
            symbol = row.get("symbol", "")
            if symbol.startswith("__sinit_"):
                output[symbol].append({
                    "owner": owner,
                    "kind": "curated-out-of-scope",
                    "proven": False,
                    "manifest": repo_relative(path, repo),
                    "evidence": row.get("evidence", ""),
                })
    return output


def load_asset_layout(path: pathlib.Path) -> dict[str, dict]:
    if not path.is_file():
        return {}
    with path.open(encoding="utf-8", newline="") as handle:
        return {row["current_path"]: row for row in csv.DictReader(handle, delimiter="\t")}


def source_tokens(path: pathlib.Path | None) -> tuple[str, set[str]]:
    if path is None or not path.is_file():
        return "", set()
    text = path.read_text(encoding="utf-8", errors="ignore")
    return text, set(IDENT_RE.findall(text))


def choose_alias(rows: list[dict], tokens: set[str]) -> dict | None:
    if not rows:
        return None
    mentioned = [row for row in rows if row["name"] in tokens]
    candidates = mentioned or rows
    return sorted(candidates, key=lambda row: (row["name"].startswith("."), row["name"]))[0]


def is_constructor(symbol: str) -> bool:
    return symbol.startswith("_ZN") and bool(re.search(r"C[123]E", symbol))


def is_destructor(symbol: str) -> bool:
    return symbol.startswith("_ZN") and bool(re.search(r"D[012]E", symbol))


def initializer_cause(size: int, calls: list[str], globals_: list[str], text: str) -> list[str]:
    causes = []
    if size <= 4:
        causes.append("empty-stub")
    if any(is_constructor(call) for call in calls):
        causes.append("global-object-construction")
    if "func_020731dc" in calls or "__register_global_object" in calls:
        causes.append("destructor-registration")
    if globals_:
        causes.append("dynamic-global-data")
    if ASSET_CALL_RE.search(text):
        causes.append("resource-handle-initialization")
    return causes or ["unclassified-generated-init"]


def infer_ownership_globals(text: str, globals_: list[str],
                            constructors: list[str]) -> list[str]:
    """Globals this initializer demonstrably writes, constructs, or registers."""
    known = set(globals_)
    owners = set(REGISTER_CALL_RE.findall(text))
    owners.update(KNOWN_INIT_FIRST_ARG_RE.findall(text))
    owners.update(ASSIGN_LHS_RE.findall(text))
    for constructor in constructors:
        pattern = re.compile(
            re.escape(constructor) + r"\s*\(\s*&?([A-Za-z_]\w*)"
        )
        owners.update(pattern.findall(text))
    return sorted(owners & known)


def inventory_initializers(modules: dict, by_home: dict, by_name: dict,
                           repo: pathlib.Path = REPO,
                           selected_module: str | None = None) -> list[dict]:
    rows = []
    for module, meta in sorted(modules.items()):
        if selected_module and module != canonical_module(selected_module):
            continue
        sinits = sorted(
            (row for row in meta["rows"]
             if row["kind"] == "function" and row["name"].startswith("__sinit_")),
            key=lambda row: row["addr"],
        )
        relocations = (R.load_relocs_file(meta["relocs"])
                       if meta["relocs"].is_file() else {})
        for ordinal, sinit in enumerate(sinits):
            path = SP.path_for(sinit["name"])
            text, tokens = source_tokens(path)
            targets = []
            for from_addr, (kind, to_addr, target_module) in relocations.items():
                if not (sinit["addr"] <= from_addr < sinit["addr"] + max(sinit["size"], 4)):
                    continue
                home = (canonical_module(target_module), to_addr)
                target = choose_alias(by_home.get(home, []), tokens)
                targets.append({
                    "from": from_addr,
                    "kind": kind,
                    "module": home[0],
                    "addr": to_addr,
                    "name": target["name"] if target else f"0x{to_addr:08x}",
                    "symbol_kind": target["kind"] if target else "unknown",
                })

            calls = sorted({target["name"] for target in targets
                            if target["symbol_kind"] == "function"})
            globals_ = {target["name"] for target in targets
                        if target["symbol_kind"] != "function"
                        and not target["name"].startswith(".p__sinit_")}
            # Source tokens recover aliases which a relocation's exact address
            # cannot distinguish, and globals reached through an address addend.
            for token in tokens:
                candidates = by_name.get(token, [])
                if any(row["kind"] != "function" for row in candidates):
                    globals_.add(token)
            globals_list = sorted(globals_)
            constructors = sorted(call for call in calls if is_constructor(call))
            destructors = sorted(call for call in calls if is_destructor(call))
            registered = sorted(set(REGISTER_CALL_RE.findall(text)))
            assets = sorted({int(value, 0) for value in ASSET_CALL_RE.findall(text)})
            ownership_globals = infer_ownership_globals(text, globals_list, constructors)
            rows.append({
                "module": module,
                "ordinal": ordinal,
                "symbol": sinit["name"],
                "address": sinit["addr"],
                "size": sinit["size"],
                "source": repo_relative(path, repo) if path else "",
                "calls": calls,
                "constructors": constructors,
                "destructors": destructors,
                "globals": globals_list,
                "ownership_globals": ownership_globals,
                "registered_globals": registered,
                "asset_handles": assets,
                "causes": initializer_cause(sinit["size"], calls, globals_list, text),
                "relocations": targets,
            })
    return rows


def build_global_consumer_index(global_names: set[str], repo: pathlib.Path = REPO):
    """Map touched global names to non-initializer source files that mention them."""
    consumers = collections.defaultdict(set)
    if not global_names:
        return consumers
    for path in (repo / "src").rglob("*"):
        if not (path.is_file() and path.suffix in SP.SOURCE_SUFFIXES):
            continue
        if path.stem.startswith("__sinit_"):
            continue
        text = path.read_text(encoding="utf-8", errors="ignore")
        hits = set(IDENT_RE.findall(text)) & global_names
        if not hits:
            continue
        rel = repo_relative(path, repo)
        for name in hits:
            consumers[name].add(rel)
    return consumers


def source_unit(path: str, module: str, tu_by_symbol: dict,
                ownership: dict[str, list[str]]) -> set[str]:
    symbols = ownership.get(path) or [pathlib.PurePosixPath(path).stem]
    return {unit["id"] for symbol in symbols
            if (unit := tu_by_symbol.get((module, symbol))) is not None}


def ordered_candidate(row: dict, tu_modules: dict, tu_by_id: dict) -> dict | None:
    module_row = tu_modules.get(row["module"])
    if not module_row:
        return None
    units = module_row.get("units", [])
    if not (module_row.get("sinits") and
            module_row.get("sinits") == len(units) and
            module_row.get("ctor_entries") == module_row.get("sinits")):
        return None
    return tu_by_id.get(f"{row['module']}:{row['ordinal']}")


def choose_classification(*, proven_owner: str, curated_owner: str,
                          consumer_units: set[str], type_units: set[str],
                          order_unit: str, asset_confidence: str,
                          consumer_candidate_reliable: bool = True) -> tuple[str, str]:
    """Return ``(classification, candidate-id-or-label)`` from ranked evidence."""
    if proven_owner:
        return "proven", proven_owner
    if curated_owner:
        return "high", curated_owner
    if len(consumer_units) > 1:
        return "ambiguous", ""
    if len(consumer_units) == 1:
        candidate = next(iter(consumer_units))
        if type_units and candidate not in type_units:
            return "ambiguous", ""
        if order_unit and candidate != order_unit:
            return "ambiguous", ""
        return ("high" if consumer_candidate_reliable else "medium"), candidate
    if asset_confidence == "high":
        return "high", ""
    combined = set(type_units)
    if order_unit:
        combined.add(order_unit)
    if len(combined) == 1:
        return "medium", next(iter(combined))
    if len(combined) > 1:
        return "ambiguous", ""
    if asset_confidence == "medium":
        return "medium", ""
    return "count-only", ""


def consumer_coverage(global_sources: dict[str, list[str]],
                      consumer_units_by_global: dict[str, list[str]]) -> tuple[list[str], list[str]]:
    """Return ownership globals with mapped TU consumers and with no consumers at all."""
    mapped = sorted(
        name for name, units in consumer_units_by_global.items() if units
    )
    unconsumed = sorted(
        name for name, paths in global_sources.items() if not paths
    )
    return mapped, unconsumed


def coverage_status(ownership_globals: list[str], mapped_globals: list[str]) -> str:
    if not mapped_globals:
        return "none"
    if len(mapped_globals) == len(ownership_globals):
        return "full"
    return "partial"


def analyse(initializers: list[dict], tu_modules: dict, tu_by_symbol: dict,
            tu_by_id: dict, manifests: dict, assets: dict,
            repo: pathlib.Path = REPO) -> list[dict]:
    globals_all = {
        name for row in initializers
        for name in (row["ownership_globals"] or row["globals"])
    }
    consumers = build_global_consumer_index(globals_all, repo)
    ownership = SP.source_ownership_index()
    class_units = collections.defaultdict(set)
    for unit in tu_by_id.values():
        for cls in unit["classes"]:
            class_units[(unit["module"], cls)].add(unit["id"])

    output = []
    for row in initializers:
        module = row["module"]
        ownership_fallback = not row["ownership_globals"]
        ownership_globals = row["ownership_globals"] or row["globals"]
        global_sources = {
            name: sorted(consumers.get(name, set())) for name in ownership_globals
        }
        consumer_units = set()
        consumer_units_by_global = {}
        unmapped_consumer_sources = set()
        external_consumer_sources = set()
        for name, paths in global_sources.items():
            units_for_global = set()
            for path in paths:
                local_units = source_unit(path, module, tu_by_symbol, ownership)
                if local_units:
                    units_for_global.update(local_units)
                    consumer_units.update(local_units)
                    continue
                symbols = ownership.get(path) or [pathlib.PurePosixPath(path).stem]
                foreign = {
                    unit["id"] for (owner_module, symbol), unit in tu_by_symbol.items()
                    if owner_module != module and symbol in symbols
                }
                if foreign:
                    external_consumer_sources.add(path)
                else:
                    unmapped_consumer_sources.add(path)
            consumer_units_by_global[name] = sorted(units_for_global)

        mapped_ownership_globals, unconsumed_ownership_globals = consumer_coverage(
            global_sources, consumer_units_by_global
        )

        referenced_classes = sorted({
            cls for symbol in row["constructors"] + row["destructors"]
            if (cls := SP.class_of(symbol))
        })
        type_units = set()
        for cls in referenced_classes:
            type_units.update(class_units.get((module, cls), set()))

        order = ordered_candidate(row, tu_modules, tu_by_id)
        order_id = order["id"] if order else ""
        consumer_unit = (tu_by_id.get(next(iter(consumer_units)))
                         if len(consumer_units) == 1 else None)
        module_under_segmented = bool(
            tu_modules.get(module, {}).get("under_segmented")
        )
        consumer_candidate_reliable = bool(
            consumer_unit and consumer_unit["classes"] and not module_under_segmented
            and not ownership_fallback and not unmapped_consumer_sources
        )
        manifest_rows = manifests.get(row["symbol"], [])
        proven = next((item for item in manifest_rows if item.get("proven")), None)
        curated = next((item for item in manifest_rows
                        if item.get("kind") == "curated-out-of-scope"), None)
        asset = assets.get(row["source"], {})
        classification, candidate = choose_classification(
            proven_owner=proven["owner"] if proven else "",
            curated_owner=curated["owner"] if curated else "",
            consumer_units=consumer_units,
            type_units=type_units,
            order_unit=order_id,
            asset_confidence=asset.get("confidence", ""),
            consumer_candidate_reliable=consumer_candidate_reliable,
        )

        evidence = []
        blockers = []
        if proven:
            evidence.append(f"licensed by {proven['manifest']} ({proven['owner']})")
        if curated:
            detail = curated.get("evidence") or "explicit manifest ownership inventory"
            evidence.append(f"curated by {curated['manifest']}: {detail}")
        if consumer_units:
            evidence.append(
                f"{len(mapped_ownership_globals)} of {len(ownership_globals)} ownership "
                "global(s) map through consumers to "
                + ", ".join(sorted(consumer_units))
            )
        if unconsumed_ownership_globals:
            evidence.append(
                f"{len(unconsumed_ownership_globals)} ownership global(s) have no "
                "recovered source consumer"
            )
        if ownership_fallback and row["globals"]:
            evidence.append(
                "no directional write/construct/register target was recovered; "
                "consumer mapping falls back to all referenced globals"
            )
        if type_units:
            evidence.append(
                "constructor/destructor class maps to " + ", ".join(sorted(type_units))
            )
        if order:
            evidence.append(
                f"ordered hypothesis {order['id']} ({order['display']}): module has "
                "one sinit per candidate TU; this is corroboration, not proof"
            )
        if asset:
            evidence.append(
                f"asset layout {asset.get('confidence', 'review')}: "
                f"{asset.get('suggested_path') or asset.get('actor') or 'no unique path'}"
            )
        if len(consumer_units) > 1:
            blockers.append("touched-global consumers span multiple candidate TUs")
        if len(consumer_units) == 1 and not consumer_candidate_reliable:
            reasons = []
            if module_under_segmented:
                reasons.append("module TU map is under-segmented")
            if consumer_unit and not consumer_unit["classes"]:
                reasons.append("consumer cluster has no class label")
            if ownership_fallback:
                reasons.append("consumer edge is non-directional")
            if unmapped_consumer_sources:
                reasons.append("some same-module consumers are not mapped to a TU")
            blockers.append(
                "; ".join(reasons) + "; consumer ownership is only medium confidence"
            )
        if len(consumer_units) == 1 and type_units:
            only = next(iter(consumer_units))
            if only not in type_units:
                blockers.append("global-consumer and constructor-type evidence disagree")
        if (order_id and candidate and candidate != order_id
                and not candidate.startswith(module + "/")):
            blockers.append("candidate disagrees with the module-wide ordered hypothesis")
        if "empty-stub" in row["causes"] and classification == "count-only":
            blockers.append("empty initializer has no relocation fingerprint")
        if not consumer_units and not type_units and not curated and not proven:
            blockers.append("no touched global maps to a candidate TU")

        candidate_unit = tu_by_id.get(candidate)
        result = dict(row)
        result.update({
            "classification": classification,
            "candidate": candidate,
            "candidate_display": candidate_unit["display"] if candidate_unit else candidate,
            "consumer_units": sorted(consumer_units),
            "consumer_units_by_global": consumer_units_by_global,
            "mapped_ownership_globals": mapped_ownership_globals,
            "unconsumed_ownership_globals": unconsumed_ownership_globals,
            "consumer_coverage": coverage_status(
                ownership_globals, mapped_ownership_globals
            ),
            "type_units": sorted(type_units),
            "referenced_classes": referenced_classes,
            "order_candidate": order_id,
            "order_candidate_display": order["display"] if order else "",
            "global_consumers": global_sources,
            "unmapped_consumer_sources": sorted(unmapped_consumer_sources),
            "external_consumer_sources": sorted(external_consumer_sources),
            "asset_suggested_path": asset.get("suggested_path", ""),
            "asset_confidence": asset.get("confidence", ""),
            "evidence": evidence,
            "blockers": blockers,
        })
        output.append(result)
    return output


def json_ready(row: dict) -> dict:
    output = dict(row)
    output["address"] = f"0x{row['address']:08x}"
    output["size"] = f"0x{row['size']:x}"
    output["relocations"] = [dict(relocation) for relocation in row["relocations"]]
    for relocation in output["relocations"]:
        relocation["from"] = f"0x{relocation['from']:08x}"
        relocation["addr"] = f"0x{relocation['addr']:08x}"
    return output


def validate_rows(rows: list[dict]) -> None:
    """Fail loudly if the report drops or overstates an initializer."""
    errors = []
    keys = [(row["module"], row["symbol"]) for row in rows]
    duplicates = sorted(key for key, count in collections.Counter(keys).items()
                        if count != 1)
    if duplicates:
        errors.append(f"duplicate initializer keys: {duplicates[:5]}")
    by_module = collections.defaultdict(list)
    for row in rows:
        by_module[row["module"]].append(row["ordinal"])
    for module, ordinals in by_module.items():
        ordinals = sorted(ordinals)
        if ordinals != list(range(len(ordinals))):
            errors.append(f"{module}: non-contiguous initializer ordinals {ordinals[:8]}")
    for row in rows:
        if row["classification"] not in CLASSIFICATIONS:
            errors.append(f"{row['symbol']}: unknown classification {row['classification']}")
        if not row["source"]:
            errors.append(f"{row['symbol']}: no enrolled source")
        if row["classification"] == "proven" and not row["candidate"]:
            errors.append(f"{row['symbol']}: proven without an owner")
        if row["classification"] == "high" and not (
                row["candidate"] or row["asset_confidence"] == "high"):
            errors.append(f"{row['symbol']}: high without an owner signal")
    if errors:
        raise SystemExit("sinit_owners validation failed:\n  " + "\n  ".join(errors[:20]))


def write_json(path: pathlib.Path, rows: list[dict], tu_map: pathlib.Path) -> None:
    counts = collections.Counter(row["classification"] for row in rows)
    causes = collections.Counter(cause for row in rows for cause in row["causes"])
    high_coverage = collections.Counter(
        row["consumer_coverage"] for row in rows if row["classification"] == "high"
    )
    payload = {
        "meta": {
            "initializers": len(rows),
            "classifications": {name: counts.get(name, 0) for name in CLASSIFICATIONS},
            "high_consumer_coverage": {
                name: high_coverage.get(name, 0) for name in ("full", "partial", "none")
            },
            "causes": dict(sorted(causes.items())),
            "tu_map": repo_relative(tu_map) if tu_map.is_relative_to(REPO) else str(tu_map),
            "classification_contract": {
                "proven": "licensed by a link-verified, ready, or promoted TU manifest",
                "high": "curated manifest ownership or one non-conflicting consumer TU",
                "medium": "single type/order/asset hypothesis without consumer proof",
                "ambiguous": "independent ownership signals disagree or span multiple TUs",
                "count-only": "initializer count/order witness with no owner signal",
            },
        },
        "initializers": [json_ready(row) for row in rows],
    }
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def write_tsv(path: pathlib.Path, rows: list[dict]) -> None:
    fields = (
        "module", "ordinal", "symbol", "address", "size", "source",
        "classification", "candidate", "candidate_display", "order_candidate",
        "consumer_coverage",
        "causes", "globals", "constructors", "destructors", "registered_globals",
        "ownership_globals", "mapped_ownership_globals", "unconsumed_ownership_globals",
        "asset_handles", "consumer_units", "referenced_classes",
        "evidence", "blockers",
    )
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, delimiter="\t", fieldnames=fields,
                                lineterminator="\n")
        writer.writeheader()
        for row in rows:
            flat = {key: row.get(key, "") for key in fields}
            flat["address"] = f"0x{row['address']:08x}"
            flat["size"] = f"0x{row['size']:x}"
            for key in ("causes", "globals", "constructors", "destructors",
                        "registered_globals", "ownership_globals", "mapped_ownership_globals",
                        "unconsumed_ownership_globals", "asset_handles",
                        "consumer_units",
                        "referenced_classes", "evidence", "blockers"):
                flat[key] = "; ".join(str(value) for value in row.get(key, []))
            writer.writerow(flat)


def print_report(rows: list[dict], show: str, limit: int) -> None:
    counts = collections.Counter(row["classification"] for row in rows)
    causes = collections.Counter(cause for row in rows for cause in row["causes"])
    print(f"sinit_owners: {len(rows)} initializer(s)")
    print("  " + "  ".join(f"{name}={counts.get(name, 0)}" for name in CLASSIFICATIONS))
    high_coverage = collections.Counter(
        row["consumer_coverage"] for row in rows if row["classification"] == "high"
    )
    print("  high consumer coverage: "
          + "  ".join(f"{name}={high_coverage.get(name, 0)}"
                       for name in ("full", "partial", "none")))
    print("  causes: " + "  ".join(f"{name}={count}"
                                    for name, count in sorted(causes.items())))
    if show == "none":
        return
    selected = rows if show == "all" else [row for row in rows
                                             if row["classification"] == show]
    for row in selected[:limit]:
        candidate = row["candidate_display"] or row["order_candidate_display"] or "?"
        print(f"  {row['classification']:10} {row['module']:5} "
              f"{row['symbol']:32} -> {candidate}")
    if len(selected) > limit:
        print(f"  ... {len(selected) - limit} more {show} row(s)")


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--module", help="limit to main/arm9 or one ovNNN module")
    parser.add_argument("--tu-map", type=pathlib.Path, default=DEFAULT_TU_MAP)
    parser.add_argument("--asset-layout", type=pathlib.Path, default=DEFAULT_ASSET_LAYOUT)
    parser.add_argument("--json", type=pathlib.Path, default=DEFAULT_JSON)
    parser.add_argument("--tsv", type=pathlib.Path, default=DEFAULT_TSV)
    parser.add_argument("--no-write", action="store_true", help="print only")
    parser.add_argument("--show", choices=("none", "all") + CLASSIFICATIONS,
                        default="high", help="rows to print (default: high)")
    parser.add_argument("--limit", type=int, default=30)
    args = parser.parse_args(argv)

    tu_modules, tu_by_symbol, tu_by_id = load_tu_map(args.tu_map)
    modules, by_home, by_name = load_config()
    initializers = inventory_initializers(
        modules, by_home, by_name, selected_module=args.module
    )
    rows = analyse(
        initializers, tu_modules, tu_by_symbol, tu_by_id,
        load_manifest_evidence(), load_asset_layout(args.asset_layout),
    )
    validate_rows(rows)
    print_report(rows, args.show, max(args.limit, 0))
    if not args.no_write:
        write_json(args.json, rows, args.tu_map)
        write_tsv(args.tsv, rows)
        print(f"wrote {args.json}")
        print(f"wrote {args.tsv}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
