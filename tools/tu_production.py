#!/usr/bin/env python3
"""Fail-closed production preparation for manifest-backed partitioned TUs.

The strict proof already lives in :mod:`tubuild`: compile one merged source,
derive its exact legacy text contributions, retain one licensed non-text object,
and compare the final module/ROM against a content-bound stock control.  This
module exposes the same object preparation to ``rombuild.py`` without teaching a
second implementation what compiler-only output, vague RTTI, vtable address
points, or storage aliases mean.

Only entries with a current ``partitioned-link-verified`` manifest record are
accepted.  The record is an admission prerequisite, not proof by itself: every
text contribution, non-text byte, relocation, symbol policy, and alias is
recomputed before an object override is returned.
"""

from __future__ import annotations

import hashlib
import json
import pathlib

import tubuild as TB


PRODUCTION_CONFIG = TB.REPO / "config" / "production-tus.json"


class ProductionTuError(RuntimeError):
    """A partitioned TU cannot safely enter the normal ROM build."""


def _raise(label, reasons):
    detail = "; ".join(str(reason) for reason in reasons if reason)
    raise ProductionTuError(f"{label}: {detail or 'refused without a reason'}")


def configured_ids(path=PRODUCTION_CONFIG):
    """Return all fail-closed TUs enabled in an ordinary stock build.

    ``partitioned_tus`` contributes exact compiler-owned text plus licensed
    non-text storage. ``derived_text_tus`` compiles one canonical TU and replaces
    only its existing per-function object contributions; it claims no data.
    """
    path = pathlib.Path(path)
    if not path.is_file():
        return []
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ProductionTuError(f"production TU config is unreadable: {exc}") from exc
    if data.get("schema_version") != 1:
        raise ProductionTuError(
            f"production TU config has unsupported schema_version "
            f"{data.get('schema_version')!r}")
    ids = []
    for key in ("partitioned_tus", "derived_text_tus"):
        rows = data.get(key, [])
        if not isinstance(rows, list):
            raise ProductionTuError(f"production TU config needs a {key} list")
        if any(not isinstance(row, str) or not row.strip() for row in rows):
            raise ProductionTuError("production TU ids must be non-empty strings")
        ids.extend(rows)
    if len(ids) != len(set(ids)):
        raise ProductionTuError("production TU config contains duplicate ids")
    return ids


def _strict_baseline():
    """Return the content-bound stock baseline or refuse stale/missing evidence."""
    report_path = TB.BASELINE_LINK / "linkcheck.json"
    linked_elf = TB.BASELINE_LINK / "final_link.o"
    if not report_path.is_file() or not linked_elf.is_file():
        raise ProductionTuError(
            "missing strict stock control; run `python tools/tubuild.py linkcheck "
            "--baseline --module <module> -j 16 --clean` first")
    try:
        report = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise ProductionTuError(f"stock control report is unreadable: {exc}") from exc
    if report.get("baseline") is not True \
            or (report.get("analysis") or {}).get("passed") is not True \
            or ((report.get("phases") or {}).get("checkModules") or {}).get("ok") is not True:
        raise ProductionTuError("stock control did not pass module fidelity")
    rom = report.get("rom") or {}
    if rom.get("matchesStockRom") is not True or not rom.get("sha256"):
        raise ProductionTuError("stock control did not prove a ROM identical to build/sm64ds.nds")
    _sha, error = TB.validate_partition_baseline_evidence(report, linked_elf)
    if error:
        raise ProductionTuError(f"stock control is stale: {error}")
    base_errors = (((report.get("phases") or {}).get("checkSymbols") or {})
                   .get("errors"))
    if not isinstance(base_errors, list):
        raise ProductionTuError("stock control has no baseline symbol-error inventory")
    return {
        "report": report,
        "reportPath": report_path,
        "linkedElf": linked_elf,
        "linkedElfSha256": hashlib.sha256(linked_elf.read_bytes()).hexdigest(),
        "symbolErrors": sorted(set(base_errors)),
        "romSha256": rom["sha256"],
    }


def _entry(data, tu_id):
    entry = TB.manifest_entry(data, tu_id)
    if entry is None:
        raise ProductionTuError(f"no manifest entry for {tu_id!r}")
    mode = entry.get("production_mode", "partitioned")
    expected = {
        "partitioned": ("partitioned_link", "partitioned-link-verified"),
        "derived-text": ("partial_isolation", "partial-link-verified"),
    }.get(mode)
    if expected is None:
        raise ProductionTuError(
            f"{tu_id}: unsupported production_mode {mode!r}")
    block, expected_state = expected
    state = (entry.get(block) or {}).get("state")
    if state != expected_state:
        raise ProductionTuError(
            f"{tu_id}: {block}.state is {state!r}, expected "
            f"{expected_state!r}")
    source = entry.get("source")
    if source != entry.get("promoted_source") or not isinstance(source, str) \
            or not source.startswith("src/") or not (TB.REPO / source).is_file():
        raise ProductionTuError(
            f"{tu_id}: production source must be the existing canonical "
            "promoted_source under src/")
    return entry


def _derived_text_output_audit(entry, obj_bytes):
    """Compare every deliberately discarded raw definition to manifest evidence.

    A derived-text TU contributes only its manifest functions, but the compiler may
    also instantiate an inherited RTTI record, vtable, or inline destructor variant.
    The partial link proves those outputs are not part of this TU's licensed ROM
    contribution.  Discarding them is safe only while their exact symbol/section,
    size, and binding remain the inventory already reviewed in the manifest.
    """
    inv = TB.elf_inventory(obj_bytes)
    funcs, objects, anonymous_sections = TB.unlicensed_inventory(entry, inv)
    section_by_index = {row["index"]: row["name"] for row in inv["sections"]}

    actual = set()
    for row in funcs:
        actual.add(("text", row["name"], int(row["size"]), row["bind"]))
    for row in objects:
        section = section_by_index.get(row["shndx"], "")
        actual.add((section.lstrip("."), row["name"], int(row["size"]), row["bind"]))
    for row in anonymous_sections:
        actual.add((row["name"].lstrip("."), f"<section:{row['name']}>",
                    int(row["size"]), None))

    observed = entry.get("unlicensed_output_observed") or {}
    expected = set()
    errors = []
    for category in ("text", "data", "bss", "init", "rodata", "ctor"):
        rows = observed.get(category, [])
        if not isinstance(rows, list):
            errors.append(f"unlicensed_output_observed.{category} is not a list")
            continue
        for row in rows:
            if not isinstance(row, dict) or not row.get("symbol"):
                errors.append(
                    f"unlicensed_output_observed.{category} has an invalid row")
                continue
            try:
                size = (int(row["size"], 0) if isinstance(row["size"], str)
                        else int(row["size"]))
            except (KeyError, TypeError, ValueError):
                errors.append(
                    f"discarded symbol {row.get('symbol')!r} has an invalid size")
                continue
            expected.add((category, row["symbol"], size, row.get("binding")))

    def describe(item):
        category, symbol, size, binding = item
        return f"{category}:{symbol} size=0x{size:x} binding={binding}"

    errors.extend(f"new discarded output: {describe(row)}"
                  for row in sorted(actual - expected))
    errors.extend(f"recorded discarded output disappeared or changed: {describe(row)}"
                  for row in sorted(expected - actual))
    return {
        "ok": not errors,
        "actual": [describe(row) for row in sorted(actual)],
        "expected": [describe(row) for row in sorted(expected)],
        "errors": errors,
    }


def _prepare_one(entry, config_root, work_root, jobs):
    claims, reasons = TB.manifest_section_claims(entry)
    if reasons:
        _raise(f"{entry['id']} manifest section claims", reasons)
    text = [claim for claim in claims if claim["name"] == ".text"]
    nontext = [claim for claim in claims if claim["name"] != ".text"]
    mode = entry.get("production_mode")
    if len(text) != 1:
        raise ProductionTuError(
            f"{entry['id']}: production TU requires exactly one .text claim")
    if mode == "partitioned" and not nontext:
        raise ProductionTuError(
            f"{entry['id']}: partitioned production requires at least one "
            "licensed non-text claim")
    if mode == "derived-text" and nontext:
        raise ProductionTuError(
            f"{entry['id']}: derived-text production cannot claim non-text output")
    span_start, span_end = text[0]["start"], text[0]["end"]
    legacy = [row["legacy_source"] for row in entry.get("functions", [])]
    if len(legacy) != len(set(legacy)):
        raise ProductionTuError(
            f"{entry['id']}: production partitioning requires one unique legacy "
            "object path per function")

    delinks = TB.module_dir_in(config_root, entry["module"]) / "delinks.txt"
    if not delinks.is_file():
        raise ProductionTuError(f"{entry['id']}: generated delinks file missing: {delinks}")
    if mode == "partitioned":
        replaced, reasons = TB.add_partitioned_tu_entry(
            delinks, span_start, span_end, entry["source"], legacy, claims)
        if reasons:
            _raise(f"{entry['id']} generated delinks partition", reasons)
        if replaced != sorted(path.replace("\\", "/") for path in legacy):
            raise ProductionTuError(
                f"{entry['id']}: generated delinks replaced {replaced}, expected "
                f"{sorted(legacy)}")
    else:
        _header, _entries, _inside, reasons = TB.span_entries(
            delinks, span_start, span_end, legacy)
        if reasons:
            _raise(f"{entry['id']} tracked text partition", reasons)

    raw, version, flags, _build_dir, _obj_path = TB._compile_tu(entry)
    discarded_output = (_derived_text_output_audit(entry, raw)
                        if mode == "derived-text" else None)
    if discarded_output is not None and not discarded_output["ok"]:
        _raise(f"{entry['id']} derived-text discarded output",
               discarded_output["errors"])
    derived = TB.derive_function_objects(raw, entry)
    comparison_root = work_root / "comparison" / TB.sanitize_id(entry["id"])
    production, errors = TB.production_objects(
        entry, comparison_root, jobs=max(1, jobs), cache=None)
    if errors:
        _raise(f"{entry['id']} production comparison compile", errors)
    rows = TB.partial_report(entry, len(raw), derived, production)
    bad = [(symbol, verdict.get("differences", []))
           for _ordinal, symbol, verdict in rows if not verdict.get("identical")]
    if bad:
        _raise(f"{entry['id']} text contribution equivalence",
               [f"{symbol}: {differences}" for symbol, differences in bad])

    if mode == "partitioned":
        post_policy, compiler_only, reasons = TB.apply_compiler_only_policy(raw, entry)
        if reasons:
            _raise(f"{entry['id']} compiler-only output", reasons)
        externalized_obj, externalized, reasons = \
            TB.apply_externalized_output_policy(post_policy, entry)
        if reasons:
            _raise(f"{entry['id']} exact RTTI externalization", reasons)
    else:
        # Derived-text owns no storage and no additional text. The complete raw
        # discarded surface was compared to unlicensed_output_observed above;
        # rewriting it with the stronger partitioned-data policies would apply a
        # second, unrelated ownership model and reject intentional D2/RTTI output.
        externalized_obj = raw
        compiler_only = {"mode": "discarded-output-audited",
                         "requested": [], "deadstripped": []}
        externalized = {"mode": "discarded-output-audited",
                        "requested": [], "externalized": []}

    if mode == "partitioned":
        owned_before = TB.verify_owned_sections(externalized_obj, entry, claims)
        if not owned_before.get("ok"):
            _raise(f"{entry['id']} licensed non-text contribution",
                   owned_before.get("errors", []))
        storage_obj, partition, reasons = \
            TB.derive_owned_nontext_object(externalized_obj, entry, claims)
        if reasons:
            _raise(f"{entry['id']} non-text partition", reasons)
        biases, reasons = TB.partition_vtable_rebiases(entry, claims)
        if reasons:
            _raise(f"{entry['id']} vtable address-point policy", reasons)
        storage_obj, rebias = TB.OI.rebias_object_symbols(storage_obj, biases)
        if storage_obj is None:
            _raise(f"{entry['id']} vtable address-point rewrite",
                   [rebias.get("error")])
        owned_after = TB.verify_owned_sections(
            storage_obj, entry, claims, public_address_points=True)
        if not owned_after.get("ok"):
            _raise(f"{entry['id']} reduced non-text contribution",
                   owned_after.get("errors", []))
    else:
        storage_obj = None
        biases = {}
        rebias = {"rebased": [], "aliases": [], "error": None}
        owned_before = {"ok": True, "claimed": 0, "rows": [], "errors": []}
        owned_after = dict(owned_before)
        partition = {
            "mode": "derived-text",
            "requestedSections": [],
            "licensedSymbols": [],
            "deferredOutputs": [
                {"symbol": row["symbol"], "section": ".text", "size": row["size"]}
                for row in entry.get("functions", [])
            ],
        }

    object_root = work_root / "objects" / TB.sanitize_id(entry["id"])
    overrides = {}
    for row in sorted(entry["functions"], key=lambda item: item["ordinal"]):
        symbol = row["symbol"]
        body, plan = derived[symbol]
        if body is None:
            raise ProductionTuError(
                f"{entry['id']}: objisolate refused {symbol}: {plan.get('error')}")
        path = object_root / pathlib.Path(row["legacy_source"]).with_suffix(".o")
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(body)
        overrides[row["legacy_source"].replace("\\", "/")] = path
    if storage_obj is not None:
        storage_path = object_root / pathlib.Path(entry["source"]).with_suffix(".o")
        storage_path.parent.mkdir(parents=True, exist_ok=True)
        storage_path.write_bytes(storage_obj)
        overrides[entry["source"].replace("\\", "/")] = storage_path

    return {
        "id": entry["id"],
        "mode": mode,
        "module": entry["module"],
        "source": entry["source"],
        "toolchain": version,
        "flags": flags,
        "textContributions": len(rows),
        "textEquivalent": len(rows),
        "nontextClaims": [dict(claim) for claim in nontext],
        "compilerOnly": compiler_only,
        "externalized": externalized,
        "discardedOutput": discarded_output,
        "partition": partition,
        "ownedBefore": owned_before,
        "ownedAfter": owned_after,
        "vtableRebias": rebias,
        "biases": biases,
        "overrides": overrides,
        "rawSha256": hashlib.sha256(raw).hexdigest(),
        "storageSha256": (hashlib.sha256(storage_obj).hexdigest()
                          if storage_obj is not None else None),
    }


def prepare(tu_ids, config_root, work_root, jobs=1):
    """Prepare exact object overrides and a generated partitioned delinks profile."""
    ids = [str(tu_id) for tu_id in tu_ids]
    if not ids:
        return {"entries": [], "overrides": {}, "baseline": None}
    if len(ids) != len(set(ids)):
        raise ProductionTuError("duplicate --partitioned-tu id")
    baseline = _strict_baseline()
    data = TB.load_manifest()
    config_root = pathlib.Path(config_root)
    work_root = pathlib.Path(work_root)
    work_root.mkdir(parents=True, exist_ok=True)
    prepared, overrides, claimed_legacy = [], {}, set()
    for tu_id in ids:
        entry = _entry(data, tu_id)
        legacy = {row["legacy_source"].replace("\\", "/")
                  for row in entry.get("functions", [])}
        overlap = sorted(legacy & claimed_legacy)
        if overlap:
            raise ProductionTuError(
                f"{tu_id}: legacy object(s) already claimed by another requested TU: "
                f"{overlap}")
        result = _prepare_one(entry, config_root, work_root, jobs)
        for rel, path in result.pop("overrides").items():
            if rel in overrides:
                raise ProductionTuError(f"duplicate prepared object override for {rel}")
            overrides[rel] = path
        claimed_legacy.update(legacy)
        prepared.append(result)
    return {
        "entries": prepared,
        "overrides": overrides,
        "baseline": {
            "report": baseline["reportPath"],
            "linkedElf": baseline["linkedElf"],
            "linkedElfSha256": baseline["linkedElfSha256"],
            "symbolErrors": baseline["symbolErrors"],
            "romSha256": baseline["romSha256"],
        },
    }


def verify_link(config_yaml, linked_elf, prepared):
    """Run the production-only module, symbol-delta, and storage-alias gates."""
    config_yaml = pathlib.Path(config_yaml)
    linked_elf = pathlib.Path(linked_elf)
    ok_modules, modules_out, _seconds = TB._run_dsd(
        [str(TB.RB.DSD), "check", "modules", "-c", str(config_yaml), "-f"],
        "dsd check modules")
    ok_symbols, symbols_out, _seconds = TB._run_dsd(
        [str(TB.RB.DSD), "check", "symbols", "-c", str(config_yaml),
         "-e", str(linked_elf), "-f", "-m", "12"],
        "dsd check symbols")
    symbol_errors = sorted({line.strip() for line in symbols_out.splitlines()
                            if "[ERROR]" in line})
    baseline_errors = set((prepared.get("baseline") or {}).get("symbolErrors") or [])
    new_errors = [line for line in symbol_errors if line not in baseline_errors]
    alias_rows, alias_errors = [], []
    for entry in prepared.get("entries", []):
        result = TB.verify_linked_storage_aliases(linked_elf, entry.get("biases", {}))
        alias_rows.extend(result.get("rows", []))
        alias_errors.extend(result.get("errors", []))
    return {
        "ok": bool(ok_modules and not new_errors and not alias_errors),
        "modulesOk": bool(ok_modules),
        "modulesOutput": modules_out[-4000:],
        "symbolsCommandOk": bool(ok_symbols),
        "symbolErrors": symbol_errors,
        "baselineSymbolErrors": sorted(baseline_errors),
        "newSymbolErrors": new_errors,
        "storageAliases": alias_rows,
        "storageAliasErrors": alias_errors,
    }


def report_view(prepared):
    """JSON-safe evidence for rombuild-report.json (object overrides stay internal)."""
    baseline = dict(prepared.get("baseline") or {})
    for key in ("report", "linkedElf"):
        if isinstance(baseline.get(key), pathlib.Path):
            baseline[key] = baseline[key].as_posix()
    entries = []
    for item in prepared.get("entries", []):
        clean = {key: value for key, value in item.items() if key != "biases"}
        entries.append(clean)
    return {"entries": entries, "baseline": baseline,
            "overrideCount": len(prepared.get("overrides", {}))}
