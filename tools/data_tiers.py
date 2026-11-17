#!/usr/bin/env python3
"""Track global-data reconstruction the way function tiers track code.

The data inventory is the universe: every configured storage address, and
whether a complete delinks claim covers it. A claim is ownership, not proof.
The full build's ``romData`` report is the proof: which emitted data symbols
match the cartridge. This tool joins the two and reports one picture.

  CLAIMED    a complete delinks claim covers an initialized address
  VERIFIED   the full build compared an emitted symbol there byte-for-byte
  PARTIAL    the emitted symbol was compared, but coverage is short or blind
  DIFFERS    at least one emitted copy disagreed with the cartridge
  UNCLAIMED  no complete source claim covers the address
  BSS        storage only; its own bar, because it has no ROM payload

The address is the item, as a function address is the item in the function
tiers. Aliases, renames and TU moves do not create a second item. The key is
``module:0xaddress``.

Initialized claims and verified symbols are counted against initialized
storage. BSS is counted against BSS. Mixing the two makes a verified bar
that can never reach 100%, because BSS has nothing for the build to compare.

``source_coverage.py`` and ``validate_merge.py`` remain the byte ratchets.
This tool does not duplicate them and it does not record who wrote the source.

No ROM, compiler or linker is used here. ``--rom-report`` is the optional
``rombuild.py`` report; without it the byte-verification tiers are unavailable.
``--write-readme`` without a report refreshes the claim bars and leaves an
existing verified block in place.
"""
from __future__ import annotations

import argparse
import collections
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
DEFAULT_ROM_REPORT = REPO / "build" / "rombuild-report.json"
README = REPO / "README.md"
README_START = "<!-- data:start -->"
README_END = "<!-- data:end -->"

STORAGE_CATEGORIES = {"global-storage", "code-embedded-data"}
_VERIFIED_LINE = re.compile(r"^DATA (VERIFIED|PARTIAL|DIFFERS) ")


def canonical_id(module, address):
    """Module-qualified key. One address is one data item."""
    value = int(address, 0) if isinstance(address, str) else int(address)
    return f"{module}:0x{value:08x}"


def normalize_address(value):
    if isinstance(value, str):
        value = int(value, 0)
    return f"0x{int(value):08x}"


def is_bss(record):
    section = record.get("section") or ""
    return section == ".bss" or section.startswith(".bss.")


def is_named(record):
    """True when the address is not a data_/reg_ placeholder.

    The inventory calls this ``existing-nonplaceholder-unreviewed``: the
    spelling is not a placeholder, and nobody has accepted it as the original
    name. It is not a recovered-name count.
    """
    return record.get("naming") == "existing-nonplaceholder-unreviewed"


def _load_json(path):
    path = pathlib.Path(path)
    if not path.is_file():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, ValueError) as exc:
        raise ValueError(f"{path}: {exc}") from exc


def load_inventory(path=None, repo=REPO):
    """Read a generated inventory, or build the same report in this process."""
    if path:
        data = _load_json(path)
        if not isinstance(data, dict) or "records" not in data:
            raise ValueError(f"{path}: not a data_inventory report")
        return data
    if str(REPO / "tools") not in sys.path:
        sys.path.insert(0, str(REPO / "tools"))
    import data_inventory  # local tool; importing it reads no ROM

    return data_inventory.build_report(repo)


def _empty_verification(reason):
    return {"available": False, "reason": reason, "by_address": {},
            "symbols": 0, "verified": 0, "partial": 0, "differs": 0,
            "unnamed": 0, "verifiedBytes": 0, "partialBytes": 0}


def load_verification(path=None):
    """Normalize the full-build data verdicts into counts and address statuses."""
    path = pathlib.Path(path) if path else None
    data = _load_json(path) if path else None
    if data is None:
        return _empty_verification("no romData report")

    rom = data.get("romData", data) if isinstance(data, dict) else {}
    if isinstance(data, list):
        # The --data-json form: one record per emitted object/symbol. Dedupe a
        # symbol across duplicate vague-linkage copies, with DIFFERS winning.
        rank = {"UNNAMED": 0, "PARTIAL": 1, "VERIFIED": 2, "DIFFERS": 3}
        best = {}
        for row in data:
            if not isinstance(row, dict):
                continue
            verdict = str(row.get("verdict", "")).upper()
            if verdict not in rank:
                continue
            key = (row.get("module"), row.get("symbol"))
            current = best.get(key)
            if current is None or verdict == "DIFFERS" or (
                    current.get("verdict") != "DIFFERS"
                    and rank[verdict] > rank.get(current.get("verdict"), -1)):
                best[key] = {**row, "verdict": verdict}
        rows = list(best.values())
        counts = collections.Counter(r["verdict"] for r in rows)
        verified_bytes = sum(int(r.get("bytes", 0) or 0) for r in rows
                             if r["verdict"] == "VERIFIED")
        partial_bytes = sum(int(r.get("bytes", 0) or 0) for r in rows
                            if r["verdict"] == "PARTIAL")
        symbol_count = len(rows)
    else:
        if not isinstance(rom, dict) or "verified" not in rom:
            return _empty_verification("report has no romData verdicts")
        counts = collections.Counter({
            "VERIFIED": int(rom.get("verified", 0)),
            "PARTIAL": int(rom.get("partial", 0)),
            "DIFFERS": int(rom.get("differs", 0)),
            "UNNAMED": int(rom.get("unnamed", 0)),
        })
        verified_bytes = int(rom.get("verifiedBytes", 0))
        partial_bytes = int(rom.get("partialBytes", 0))
        rows = []
        for field, verdict in (("verifiedSymbols", "VERIFIED"),
                               ("partialSymbols", "PARTIAL"),
                               ("differingSymbols", "DIFFERS")):
            for row in rom.get(field, []) or []:
                if isinstance(row, dict):
                    rows.append({**row, "verdict": verdict})
        symbol_count = int(rom.get("symbols", len(rows)))

    by_address = {}
    by_symbol = {}
    for row in rows:
        verdict = row.get("verdict")
        if verdict not in ("VERIFIED", "PARTIAL", "DIFFERS"):
            continue
        module = row.get("module")
        if module not in ("arm9", "itcm", "dtcm") and not re.fullmatch(r"ov\d{3}", str(module)):
            continue
        addr = row.get("addr")
        symbol = row.get("symbol")
        if isinstance(addr, str):
            try:
                addr = int(addr, 0)
            except ValueError:
                addr = None
        if isinstance(addr, int):
            key = canonical_id(module, addr)
            old = by_address.get(key)
            # DIFFERS hides nothing. VERIFIED beats PARTIAL. A later PARTIAL
            # must not downgrade an exact comparison of the same address.
            if old is None or verdict == "DIFFERS" or (
                    old != "DIFFERS" and verdict == "VERIFIED"):
                by_address[key] = verdict
        if symbol:
            by_symbol.setdefault((module, symbol), verdict)

    return {
        "available": True,
        "by_address": by_address,
        "by_symbol": by_symbol,
        "symbols": symbol_count,
        "verified": counts["VERIFIED"],
        "partial": counts["PARTIAL"],
        "differs": counts["DIFFERS"],
        "unnamed": counts["UNNAMED"],
        "verifiedBytes": verified_bytes,
        "partialBytes": partial_bytes,
        "sourceBuild": (data.get("analysis", {}).get("sourceBuild")
                        if isinstance(data, dict) else None),
    }


def _record_rows(inventory, verification):
    """One row per configured storage address, with no double-counted aliases."""
    by_address = verification.get("by_address", {}) if verification else {}
    rows = []
    for record in inventory.get("records", []):
        if record.get("category") not in STORAGE_CATEGORIES:
            continue
        module = record.get("module")
        address = record.get("address")
        if not module or not address:
            continue
        key = canonical_id(module, address)
        bss = is_bss(record)
        complete = record.get("declared_ownership") == "complete-window-claimed"
        verdict = None if bss else by_address.get(key)
        if bss:
            status = "BSS_CLAIMED" if complete else "BSS_UNCLAIMED"
        elif verdict in ("VERIFIED", "PARTIAL", "DIFFERS"):
            status = verdict
        elif complete:
            status = "CLAIMED"
        else:
            status = "UNCLAIMED"
        rows.append({
            "id": key,
            "module": module,
            "address": normalize_address(address),
            "aliases": list(record.get("aliases") or []),
            "section": record.get("section"),
            "bss": bss,
            "claimed": complete,
            "claim_status": record.get("declared_ownership"),
            "status": status,
            "named": is_named(record),
            "source": next((c.get("source") for c in record.get("claims", [])
                            if c.get("complete") and c.get("covers_window")
                            and c.get("section_matches")), None),
            "bytes": int((record.get("window") or {}).get("bytes", 0) or 0),
        })
    return rows


def summarize(inventory, verification=None):
    """Build the stable, JSON-serializable tracking report."""
    verification = verification or load_verification(None)
    rows = _record_rows(inventory, verification)

    counts = collections.Counter()
    sizes = collections.Counter()
    modules = collections.defaultdict(lambda: {
        "items": 0, "claimed": 0, "initializedClaimed": 0, "verified": 0,
        "partial": 0, "differs": 0, "unclaimed": 0, "bssClaimed": 0,
        "bssUnclaimed": 0, "named": 0, "bytes": 0, "claimedBytes": 0,
        "verifiedBytes": 0,
    })
    for row in rows:
        status = row["status"]
        counts["items"] += 1
        counts["initialized" if not row["bss"] else "bss"] += 1
        sizes["windowBytes"] += row["bytes"]
        mod = modules[row["module"]]
        mod["items"] += 1
        mod["bytes"] += row["bytes"]
        if row["claimed"]:
            counts["claimed"] += 1
            sizes["claimedWindowBytes"] += row["bytes"]
            mod["claimed"] += 1
            mod["claimedBytes"] += row["bytes"]
            if not row["bss"]:
                counts["initializedClaimed"] += 1
                mod["initializedClaimed"] += 1
        if row["named"]:
            counts["named"] += 1
            mod["named"] += 1
        if status == "VERIFIED":
            counts["verified"] += 1
            mod["verified"] += 1
            mod["verifiedBytes"] += row["bytes"]
        elif status == "PARTIAL":
            counts["partial"] += 1
            mod["partial"] += 1
        elif status == "DIFFERS":
            counts["differs"] += 1
            mod["differs"] += 1
        elif status == "UNCLAIMED":
            counts["unclaimed"] += 1
            mod["unclaimed"] += 1
        elif status == "BSS_CLAIMED":
            counts["bssClaimed"] += 1
            mod["bssClaimed"] += 1
            sizes["bssClaimedWindowBytes"] += row["bytes"]
        elif status == "BSS_UNCLAIMED":
            counts["bssUnclaimed"] += 1
            mod["bssUnclaimed"] += 1

    # Exact section bytes come from the inventory's claim partition, not from the
    # per-address observation windows. A window is the next configured boundary,
    # not a recovered object size.
    section = collections.Counter()
    for item in inventory.get("section_coverage", []) or []:
        kind = "bss" if item.get("kind") == "bss" else "initialized"
        for status, value in (item.get("coverage_bytes") or {}).items():
            section[f"{kind}:{status}"] += int(value)
    section["initialized:total"] = sum(v for k, v in section.items()
                                       if k.startswith("initialized:") and k != "initialized:total")
    section["bss:total"] = sum(v for k, v in section.items()
                               if k.startswith("bss:") and k != "bss:total")

    source_build = verification.get("sourceBuild")
    return {
        "schemaVersion": 1,
        "purpose": "Data reconstruction progress. Inventory claims are not byte proof.",
        "repository": inventory.get("repository"),
        "counts": {
            "items": counts["items"],
            "initialized": counts["initialized"],
            "bss": counts["bss"],
            "claimed": counts["claimed"],
            "initializedClaimed": counts["initializedClaimed"],
            "verified": counts["verified"],
            "partial": counts["partial"],
            "differs": counts["differs"],
            "unclaimed": counts["unclaimed"],
            "bssClaimed": counts["bssClaimed"],
            "bssUnclaimed": counts["bssUnclaimed"],
            "named": counts["named"],
        },
        "bytes": {
            "window": sizes["windowBytes"],
            "claimedWindow": sizes["claimedWindowBytes"],
            "bssClaimedWindow": sizes["bssClaimedWindowBytes"],
            "sectionCoverage": dict(sorted(section.items())),
            "verified": int((verification or {}).get("verifiedBytes", 0)),
            "partial": int((verification or {}).get("partialBytes", 0)),
        },
        "verification": {
            "available": bool(verification.get("available")),
            "symbols": int(verification.get("symbols", 0)),
            "verified": int(verification.get("verified", 0)),
            "partial": int(verification.get("partial", 0)),
            "differs": int(verification.get("differs", 0)),
            "unnamed": int(verification.get("unnamed", 0)),
            "verifiedBytes": int(verification.get("verifiedBytes", 0)),
            "partialBytes": int(verification.get("partialBytes", 0)),
        },
        "sourceBuild": source_build,
        "modules": [{"module": key, **value} for key, value in sorted(modules.items())],
        "items": rows,
        "limits": [
            "A complete claim is declared ownership; it is not compiled proof.",
            "VERIFIED/PARTIAL/DIFFERS require the full build's romData report.",
            "CLAIMED and VERIFIED use the initialized universe. BSS is separate.",
            "Per-address window bytes are the next configured boundary upper bound, not object sizes.",
            "BSS is storage, not ROM payload; its byte count is NOBITS storage.",
            "NAMED means the spelling is not a data_/reg_ placeholder. It is not a reviewed name.",
        ],
    }


def _pct(done, total):
    return 100.0 * done / total if total else 0.0


def bar(done, total, width=30):
    filled = round(_pct(done, total) / 100.0 * width) if total else 0
    if done and total and filled == 0:
        filled = 1
    return "=" * filled + "-" * (width - filled)


def _count_line(label, done, total, suffix, extra=""):
    return (f"{label:<14} {bar(done, total)}  "
            f"{_pct(done, total):4.1f}%   "
            f"{done:,} / {total:,} {suffix}{extra}")


def readme_block(report, preserved_verified=None):
    c = report["counts"]
    v = report["verification"]
    initialized = c["initialized"]
    lines = [README_START, "```"]
    lines.append(_count_line("DATA CLAIMED", c["initializedClaimed"], initialized,
                             "initialized items"))
    if v["available"]:
        lines.append(_count_line(
            "DATA VERIFIED", c["verified"], initialized, "initialized items",
            f"   {v['verifiedBytes']:,} B exact"))
        if c["partial"] or v["partialBytes"]:
            lines.append(_count_line(
                "DATA PARTIAL", c["partial"], initialized, "initialized items",
                f"   {v['partialBytes']:,} B compared"))
        if c["differs"]:
            lines.append(_count_line(
                "DATA DIFFERS", c["differs"], initialized, "initialized items"))
    elif preserved_verified:
        lines.extend(preserved_verified)
    else:
        lines.append("DATA VERIFIED  (full build report unavailable)")
    lines.append(_count_line("DATA NAMED", c["named"], c["items"], "items"))
    bss_total = c["bssClaimed"] + c["bssUnclaimed"]
    bss_bytes = report["bytes"]["sectionCoverage"].get("bss:single-complete-claim", 0)
    lines.append(_count_line(
        "BSS CLAIMED", c["bssClaimed"], bss_total, "BSS items",
        f"   {bss_bytes:,} B"))
    lines.append("```")
    lines.append("")
    lines.append("Data progress: [notes/data-tracking.md](notes/data-tracking.md).")
    lines.append(README_END)
    return "\n".join(lines)


def _preserved_verification_lines(text):
    if README_START not in text or README_END not in text:
        return None
    block = text.split(README_START, 1)[1].split(README_END, 1)[0]
    kept = [line for line in block.splitlines() if _VERIFIED_LINE.match(line)]
    return kept or None


def write_readme(path, report):
    path = pathlib.Path(path)
    text = path.read_text(encoding="utf-8", errors="replace") if path.is_file() else ""
    preserved = None
    if not report["verification"].get("available"):
        preserved = _preserved_verification_lines(text)
    block = readme_block(report, preserved)
    if README_START in text and README_END in text:
        before, rest = text.split(README_START, 1)
        _old, after = rest.split(README_END, 1)
        new = before + block + after
    else:
        # Put the data block immediately after the function tiers block when it
        # exists; otherwise append it as its own section.
        marker = "<!-- tiers:end -->"
        if marker in text:
            before, after = text.split(marker, 1)
            new = before + marker + "\n\n" + block + after
        else:
            new = text.rstrip() + "\n\n## Data progress\n\n" + block + "\n"
    path.write_text(new, encoding="utf-8", newline="\n")
    return block


def _summary(report):
    c = report["counts"]
    v = report["verification"]
    initialized = c["initialized"]
    lines = [
        "Data tracking",
        f"  storage items: {c['items']:,} ({initialized:,} initialized, {c['bss']:,} BSS)",
        f"  claimed:       {c['initializedClaimed']:,} / {initialized:,} initialized "
        f"({_pct(c['initializedClaimed'], initialized):.1f}%)",
        f"  verified:      {c['verified']:,} / {initialized:,} initialized "
        f"({_pct(c['verified'], initialized):.1f}%)"
        + (f", {v['verifiedBytes']:,} B exact" if v["available"] else " (no romData report)"),
        f"  partial:       {c['partial']:,} / {initialized:,}"
        + (f", {v['partialBytes']:,} B compared" if v["available"] else ""),
        f"  differs:       {c['differs']:,}",
        f"  named:         {c['named']:,} / {c['items']:,} "
        f"({_pct(c['named'], c['items']):.1f}%)",
        f"  bss claimed:   {c['bssClaimed']:,} / {c['bss']:,} "
        f"({_pct(c['bssClaimed'], c['bss']):.1f}%)",
    ]
    section = report["bytes"]["sectionCoverage"]
    lines.append("  section bytes: "
                 + ", ".join(f"{k}={v:,}" for k, v in sorted(section.items())))
    return "\n".join(lines)


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--inventory", help="prebuilt data_inventory JSON (default: scan)")
    ap.add_argument("--rom-report", default=str(DEFAULT_ROM_REPORT),
                    help="rombuild report with romData, or a romdata_json list")
    ap.add_argument("--json", help="write the summary tracking report")
    ap.add_argument("--items-json", help="write every per-address tracking row")
    ap.add_argument("--write-readme", action="store_true")
    args = ap.parse_args(argv)

    try:
        inventory = load_inventory(args.inventory)
        verification = load_verification(args.rom_report)
        report = summarize(inventory, verification)
        if args.write_readme:
            write_readme(README, report)
            if verification.get("available"):
                print(f"updated {README} data block")
            else:
                print(f"updated {README} claim bars; verified lines kept "
                      f"(no romData report)")
        if args.json:
            path = pathlib.Path(args.json)
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8", newline="\n")
            print(f"report: {path}")
        if args.items_json:
            path = pathlib.Path(args.items_json)
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(json.dumps(report["items"], indent=2) + "\n",
                            encoding="utf-8", newline="\n")
            print(f"items: {path}")
        print(_summary(report))
        return 0
    except (OSError, ValueError) as exc:
        print(f"data tiers: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
