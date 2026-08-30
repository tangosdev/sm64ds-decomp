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


class ProductionTuError(RuntimeError):
    """A partitioned TU cannot safely enter the normal ROM build."""


def prepare_intact_object(raw, entry):
    """Apply and reverify the exact policies for one production intact TU object."""
    claims, reasons = TB.manifest_section_claims(entry)
    if reasons:
        _raise(f"{entry.get('id', '<unknown>')} manifest section claims", reasons)
    text = [claim for claim in claims if claim["name"] == ".text"]
    nontext = [claim for claim in claims if claim["name"] != ".text"]
    if len(text) != 1 or not nontext:
        raise ProductionTuError(
            f"{entry.get('id', '<unknown>')}: intact production requires one .text "
            "claim and at least one non-text claim")

    post_policy, compiler_only, reasons = TB.apply_compiler_only_policy(raw, entry)
    if reasons:
        _raise(f"{entry['id']} compiler-only output", reasons)
    externalized_obj, externalized, reasons = \
        TB.apply_externalized_output_policy(post_policy, entry)
    if reasons:
        _raise(f"{entry['id']} exact RTTI externalization", reasons)

    owned_before = TB.verify_owned_sections(externalized_obj, entry, claims)
    if not owned_before.get("ok"):
        _raise(f"{entry['id']} licensed non-text contribution",
               owned_before.get("errors", []))
    biases, reasons = TB.partition_vtable_rebiases(entry, claims)
    if reasons:
        _raise(f"{entry['id']} vtable address-point policy", reasons)
    linked_obj, rebias = TB.OI.rebias_object_symbols(
        externalized_obj, biases, normalize_undefined=True)
    if linked_obj is None:
        _raise(f"{entry['id']} vtable address-point rewrite", [rebias.get("error")])
    owned_after = TB.verify_owned_sections(
        linked_obj, entry, claims, public_address_points=True)
    if not owned_after.get("ok"):
        _raise(f"{entry['id']} production non-text contribution",
               owned_after.get("errors", []))

    span_start, span_end = text[0]["start"], text[0]["end"]
    rows, extra, emitted, order_ok = TB.audit_tu_object(
        linked_obj, entry, span_start, span_end, TB.complete_ranges(TB.CFG_ARM9))
    audit_errors = TB.object_audit_refusals(rows, extra, order_ok)
    if audit_errors:
        _raise(f"{entry['id']} production object audit", audit_errors)
    return linked_obj, {
        "compilerOnly": compiler_only, "externalized": externalized,
        "ownedBefore": owned_before, "ownedAfter": owned_after,
        "vtableRebias": rebias,
        "objectAudit": {"rows": rows, "extraSections": extra,
                        "emitted": emitted, "orderOk": order_ok},
        "sha256": hashlib.sha256(linked_obj).hexdigest(),
    }


def _raise(label, reasons):
    detail = "; ".join(str(reason) for reason in reasons if reason)
    raise ProductionTuError(f"{label}: {detail or 'refused without a reason'}")


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
    state = (entry.get("partitioned_link") or {}).get("state")
    if state != "partitioned-link-verified":
        raise ProductionTuError(
            f"{tu_id}: partitioned_link.state is {state!r}, expected "
            "'partitioned-link-verified'")
    return entry


def _prepare_one(entry, config_root, work_root, jobs):
    claims, reasons = TB.manifest_section_claims(entry)
    if reasons:
        _raise(f"{entry['id']} manifest section claims", reasons)
    text = [claim for claim in claims if claim["name"] == ".text"]
    nontext = [claim for claim in claims if claim["name"] != ".text"]
    if len(text) != 1 or not nontext:
        raise ProductionTuError(
            f"{entry['id']}: production partitioning requires one .text claim and "
            "at least one non-text claim")
    span_start, span_end = text[0]["start"], text[0]["end"]
    legacy = [row["legacy_source"] for row in entry.get("functions", [])]
    if len(legacy) != len(set(legacy)):
        raise ProductionTuError(
            f"{entry['id']}: production partitioning requires one unique legacy "
            "object path per function")

    delinks = TB.module_dir_in(config_root, entry["module"]) / "delinks.txt"
    if not delinks.is_file():
        raise ProductionTuError(f"{entry['id']}: generated delinks file missing: {delinks}")
    replaced, reasons = TB.add_partitioned_tu_entry(
        delinks, span_start, span_end, entry["source"], legacy, claims)
    if reasons:
        _raise(f"{entry['id']} generated delinks partition", reasons)
    if replaced != sorted(path.replace("\\", "/") for path in legacy):
        raise ProductionTuError(
            f"{entry['id']}: generated delinks replaced {replaced}, expected "
            f"{sorted(legacy)}")

    raw, version, flags, _build_dir, _obj_path = TB._compile_tu(entry)
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

    post_policy, compiler_only, reasons = TB.apply_compiler_only_policy(raw, entry)
    if reasons:
        _raise(f"{entry['id']} compiler-only output", reasons)
    externalized_obj, externalized, reasons = \
        TB.apply_externalized_output_policy(post_policy, entry)
    if reasons:
        _raise(f"{entry['id']} exact RTTI externalization", reasons)

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
    storage_path = object_root / pathlib.Path(entry["source"]).with_suffix(".o")
    storage_path.parent.mkdir(parents=True, exist_ok=True)
    storage_path.write_bytes(storage_obj)
    overrides[entry["source"].replace("\\", "/")] = storage_path

    return {
        "id": entry["id"],
        "module": entry["module"],
        "source": entry["source"],
        "toolchain": version,
        "flags": flags,
        "textContributions": len(rows),
        "textEquivalent": len(rows),
        "nontextClaims": [dict(claim) for claim in nontext],
        "compilerOnly": compiler_only,
        "externalized": externalized,
        "partition": partition,
        "ownedBefore": owned_before,
        "ownedAfter": owned_after,
        "vtableRebias": rebias,
        "biases": biases,
        "overrides": overrides,
        "rawSha256": hashlib.sha256(raw).hexdigest(),
        "storageSha256": hashlib.sha256(storage_obj).hexdigest(),
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
