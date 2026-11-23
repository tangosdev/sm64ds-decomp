#!/usr/bin/env python3
"""Track global-data reconstruction the way function tiers track code.

The data inventory is the universe: every configured storage address, and
whether a complete delinks claim covers it. A claim is ownership, not proof.
The full build's ``romData`` report is the proof: which emitted data symbols
match the cartridge. This tool joins the two and reports one picture.

  CLAIMED    a complete delinks claim covers the address. This is decompiled.
  UNCLAIMED  no complete claim covers the address
  VERIFIED   the full build compared the whole emitted symbol and it matched
  DIFFERS    at least one compared byte disagreed

The address is the item. Aliases, renames and TU moves do not create a second
one. The key is ``module:0xaddress``.

Decompiled or not is the complete claim. The public bar is one DATA line, and
BSS counts in it. A few configured ranges do not. A short or blind byte
compare is not a third state: the address stays claimed or unclaimed.
Verified and differing are byte results in the text report.

``source_coverage.py`` and ``validate_merge.py`` remain the byte ratchets.
This tool does not duplicate them.

No ROM, compiler or linker is used here. ``--rom-report`` is the optional
``rombuild.py`` report; without it the byte-verification lines are unavailable.
``--write-readme`` rewrites the single DATA line from the inventory.
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

# Half-open ranges left out of the progress total.
EXCLUDED_RANGES = (
    ("arm9", 0x02075f14, 0x02082158),
    ("arm9", 0x0208a178, 0x0208c178),
    ("arm9", 0x0209289c, 0x0209435c),
    ("ov007", 0x020d87b8, 0x02103260),
)


def canonical_id(module, address):
    """Module-qualified key. One address is one data item."""
    value = int(address, 0) if isinstance(address, str) else int(address)
    return f"{module}:0x{value:08x}"


def normalize_address(value):
    if isinstance(value, str):
        value = int(value, 0)
    return f"0x{int(value):08x}"


def excluded_address(module, address):
    """True when this address is outside the progress total."""
    value = int(address, 0) if isinstance(address, str) else int(address)
    return any(module == mod and start <= value < end
               for mod, start, end in EXCLUDED_RANGES)


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
            "symbols": 0, "verified": 0, "differs": 0,
            "unnamed": 0, "verifiedBytes": 0}


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
        rank = {"UNNAMED": 0, "VERIFIED": 1, "DIFFERS": 2}
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
        symbol_count = len(rows)
    else:
        if not isinstance(rom, dict) or "verified" not in rom:
            return _empty_verification("report has no romData verdicts")
        counts = collections.Counter({
            "VERIFIED": int(rom.get("verified", 0)),
            "DIFFERS": int(rom.get("differs", 0)),
            "UNNAMED": int(rom.get("unnamed", 0)),
        })
        verified_bytes = int(rom.get("verifiedBytes", 0))
        rows = []
        for field, verdict in (("verifiedSymbols", "VERIFIED"),
                               ("differingSymbols", "DIFFERS")):
            for row in rom.get(field, []) or []:
                if isinstance(row, dict):
                    rows.append({**row, "verdict": verdict})
        symbol_count = int(rom.get("symbols", len(rows)))

    by_address = {}
    by_symbol = {}
    for row in rows:
        verdict = row.get("verdict")
        # A short or blind compare is not a progress state. The address stays
        # claimed or unclaimed.
        if verdict not in ("VERIFIED", "DIFFERS"):
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
            # DIFFERS hides nothing. A later non-difference must not downgrade
            # an exact comparison of the same address.
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
        "differs": counts["DIFFERS"],
        "unnamed": counts["UNNAMED"],
        "verifiedBytes": verified_bytes,
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
        if excluded_address(module, address):
            continue
        key = canonical_id(module, address)
        bss = is_bss(record)
        complete = record.get("declared_ownership") == "complete-window-claimed"
        verdict = None if bss else by_address.get(key)
        if bss:
            status = "BSS_CLAIMED" if complete else "BSS_UNCLAIMED"
        elif verdict in ("VERIFIED", "DIFFERS"):
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
        "differs": 0, "unclaimed": 0, "bssClaimed": 0,
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
        },
        "verification": {
            "available": bool(verification.get("available")),
            "symbols": int(verification.get("symbols", 0)),
            "verified": int(verification.get("verified", 0)),
            "differs": int(verification.get("differs", 0)),
            "unnamed": int(verification.get("unnamed", 0)),
            "verifiedBytes": int(verification.get("verifiedBytes", 0)),
        },
        "sourceBuild": source_build,
        "modules": [{"module": key, **value} for key, value in sorted(modules.items())],
        "items": rows,
        "limits": [
            "A complete claim is declared ownership; it is not compiled proof.",
            "A short byte compare is not its own state. Decompiled means a complete claim.",
            "VERIFIED and DIFFERS require the full build's romData report.",
            "The DATA bar counts every storage address, BSS included.",
            "A few configured ranges are outside the progress total.",
            "Per-address window bytes are the next configured boundary upper bound, not object sizes.",
            "BSS has no ROM payload. A claim counts it as data; the build cannot byte-compare it.",
            "NAMED means the spelling is not a data_/reg_ placeholder. It is not a reviewed name.",
        ],
    }


def _pct(done, total):
    return 100.0 * done / total if total else 0.0


def bar(done, total, width=30):
    """Same glyphs as the function bars in tools/tiers.py."""
    filled = round(_pct(done, total) / 100.0 * width) if total else 0
    if done and total and filled == 0:
        filled = 1
    return "█" * filled + "░" * (width - filled)


def readme_block(report):
    """One DATA line. BSS is included; it is not its own bar."""
    c = report["counts"]
    done, total = c["claimed"], c["items"]
    line = (f"DATA       {bar(done, total)}  "
            f"{_pct(done, total):4.1f}%   {done:,} / {total:,}")
    return "\n".join([
        README_START,
        "```",
        line,
        "```",
        "",
        "Data progress: [notes/data-tracking.md](notes/data-tracking.md).",
        README_END,
    ])


def write_readme(path, report):
    path = pathlib.Path(path)
    text = path.read_text(encoding="utf-8", errors="replace") if path.is_file() else ""
    block = readme_block(report)
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
    lines = [
        "Data tracking",
        f"  data:          {c['claimed']:,} / {c['items']:,} "
        f"({_pct(c['claimed'], c['items']):.1f}%)",
        f"  verified:      {c['verified']:,}"
        + (f", {v['verifiedBytes']:,} B exact" if v["available"] else " (no romData report)"),
        f"  differs:       {c['differs']:,}",
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
            print(f"updated {README} data block")
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
