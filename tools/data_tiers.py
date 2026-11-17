#!/usr/bin/env python3
"""Track global-data reconstruction with the same surfaces as function tiers.

The data inventory is a backlog: it knows every configured storage address and
which source claim covers it, but a complete claim is not a compiled proof.  The
full build's ``romData`` report knows which emitted data symbols are byte-exact.
This tool joins those two facts, adds the credit record, and reports one
reproducible picture:

  CLAIMED    a complete delinks claim covers the address
  VERIFIED   the full build compared an emitted symbol there byte-for-byte
  PARTIAL    the emitted symbol was compared, but coverage is short or blind
  DIFFERS    at least one emitted copy disagreed with the cartridge
  UNCLAIMED  no complete source claim covers the address
  BSS        storage only; reported separately because it has no ROM payload

The address is the identity, as it is for ``function-authors.json``.  Aliases,
renames and TU moves do not create a second item.  Credit lives in
``data-authors.json`` and is keyed by module and address.  Existing historical
claims may be listed in ``config/data-tracking-baseline.json`` as uncredited;
a claim that is not in that baseline needs an author row.  That is the ratchet
for new data work, while ``source_coverage.py`` and ``validate_merge.py`` keep
the existing source-byte and emitted-byte ratchets.

No ROM, compiler or linker is used here.  ``--rom-report`` is the optional
``rombuild.py`` report; without it the byte-verification tiers are unavailable.
"""
from __future__ import annotations

import argparse
import collections
import json
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
DEFAULT_AUTHORS = REPO / "data-authors.json"
DEFAULT_BASELINE = REPO / "config" / "data-tracking-baseline.json"
DEFAULT_ROM_REPORT = REPO / "build" / "rombuild-report.json"
README = REPO / "README.md"
README_START = "<!-- data:start -->"
README_END = "<!-- data:end -->"

STORAGE_CATEGORIES = {"global-storage", "code-embedded-data"}
ID = re.compile(r"^(?P<module>arm9|itcm|dtcm|ov\d{3}):(?P<address>0x[0-9a-fA-F]{1,8})$")


def canonical_id(module, address):
    """The module-qualified key used by both tier rows and the credit record."""
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
    if str(REPO) not in sys.path:
        sys.path.insert(0, str(REPO / "tools"))
    import data_inventory  # local tool; importing it reads no ROM

    return data_inventory.build_report(repo)


def load_verification(path=None):
    """Normalize the full-build data verdicts into counts and address statuses."""
    path = pathlib.Path(path) if path else None
    data = _load_json(path) if path else None
    if data is None:
        return {"available": False, "reason": "no romData report", "by_address": {},
                "symbols": 0, "verified": 0, "partial": 0, "differs": 0,
                "unnamed": 0, "verifiedBytes": 0, "partialBytes": 0}

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
    else:
        if not isinstance(rom, dict) or "verified" not in rom:
            return {"available": False, "reason": "report has no romData verdicts",
                    "by_address": {}, "symbols": 0, "verified": 0, "partial": 0,
                    "differs": 0, "unnamed": 0, "verifiedBytes": 0, "partialBytes": 0}
        counts = collections.Counter({
            "VERIFIED": int(rom.get("verified", 0)),
            "PARTIAL": int(rom.get("partial", 0)),
            "DIFFERS": int(rom.get("differs", 0)),
            "UNNAMED": int(rom.get("unnamed", 0)),
        })
        verified_bytes = int(rom.get("verifiedBytes", 0))
        partial_bytes = int(rom.get("partialBytes", 0))
        rows = []
        for field in ("verifiedSymbols", "partialSymbols", "differingSymbols"):
            rows.extend(rom.get(field, []) or [])
        # Some older reports only list the exact and differing identities.
        rows = [{**r, "verdict": ("VERIFIED" if r in (rom.get("verifiedSymbols") or [])
                                 else "PARTIAL" if r in (rom.get("partialSymbols") or [])
                                 else "DIFFERS")}
                for r in rows]

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
            if old is None or verdict == "DIFFERS" or (
                    old != "DIFFERS" and verdict == "VERIFIED"):
                by_address[key] = verdict
        if symbol:
            by_symbol.setdefault((module, symbol), verdict)

    return {
        "available": True,
        "by_address": by_address,
        "by_symbol": by_symbol,
        "symbols": int(rom.get("symbols", len(rows) if isinstance(rom, dict) else 0))
        if isinstance(rom, dict) else len(rows),
        "verified": counts["VERIFIED"],
        "partial": counts["PARTIAL"],
        "differs": counts["DIFFERS"],
        "unnamed": counts["UNNAMED"],
        "verifiedBytes": verified_bytes,
        "partialBytes": partial_bytes,
        "sourceBuild": (data.get("analysis", {}).get("sourceBuild")
                        if isinstance(data, dict) else None),
    }


def load_authors(path=None):
    path = pathlib.Path(path) if path else DEFAULT_AUTHORS
    data = _load_json(path)
    if data is None:
        return {}, path
    rows = data.get("data", data) if isinstance(data, dict) else None
    if not isinstance(rows, dict):
        raise ValueError(f"{path}: expected a 'data' object keyed by module:address")
    return rows, path


def _record_rows(inventory, verification, authors):
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
        credited = key in authors and bool(authors[key].get("author"))
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
            "credited": credited,
            "author": authors.get(key, {}).get("author") if credited else None,
            "source": next((c.get("source") for c in record.get("claims", [])
                            if c.get("complete") and c.get("covers_window")
                            and c.get("section_matches")), None),
            "bytes": int((record.get("window") or {}).get("bytes", 0) or 0),
        })
    return rows


def summarize(inventory, verification=None, authors=None):
    """Build the stable, JSON-serializable tracking report."""
    verification = verification or load_verification(None)
    authors = authors or {}
    rows = _record_rows(inventory, verification, authors)

    counts = collections.Counter()
    sizes = collections.Counter()
    modules = collections.defaultdict(lambda: {
        "items": 0, "claimed": 0, "verified": 0, "partial": 0, "differs": 0,
        "unclaimed": 0, "bssClaimed": 0, "bssUnclaimed": 0, "named": 0,
        "credited": 0, "bytes": 0, "claimedBytes": 0, "verifiedBytes": 0,
    })
    author_counts = collections.Counter()
    author_bytes = collections.Counter()
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
        if row["named"]:
            counts["named"] += 1
            mod["named"] += 1
        if row["credited"]:
            counts["credited"] += 1
            mod["credited"] += 1
            author = row["author"]
            author_counts[author] += 1
            author_bytes[author] += row["bytes"]
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
        "purpose": "Data reconstruction progress and credit; inventory claims are not byte proof",
        "repository": inventory.get("repository"),
        "counts": {
            "items": counts["items"],
            "initialized": counts["initialized"],
            "bss": counts["bss"],
            "claimed": counts["claimed"],
            "verified": counts["verified"],
            "partial": counts["partial"],
            "differs": counts["differs"],
            "unclaimed": counts["unclaimed"],
            "bssClaimed": counts["bssClaimed"],
            "bssUnclaimed": counts["bssUnclaimed"],
            "named": counts["named"],
            "credited": counts["credited"],
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
        "authors": [{"author": key, "items": count, "windowBytes": author_bytes[key]}
                    for key, count in author_counts.most_common()],
        "items": rows,
        "limits": [
            "A complete claim is declared ownership; it is not compiled proof.",
            "VERIFIED/PARTIAL/DIFFERS require the full build's romData report.",
            "Per-address window bytes are the next configured boundary upper bound, not object sizes.",
            "BSS is storage, not ROM payload; its byte count is NOBITS storage.",
            "Credit is data-authors.json; an existing key is never rewritten by this tool.",
        ],
    }


def _pct(done, total):
    return 100.0 * done / total if total else 0.0


def bar(done, total, width=30):
    filled = round(_pct(done, total) / 100.0 * width) if total else 0
    if done and total and filled == 0:
        filled = 1
    return "=" * filled + "-" * (width - filled)


def readme_block(report, width=30):
    c = report["counts"]
    v = report["verification"]
    lines = [README_START, "```"]
    lines.append(f"DATA CLAIMED   {bar(c['claimed'], c['items'], width)}  "
                 f"{_pct(c['claimed'], c['items']):4.1f}%   "
                 f"{c['claimed']:,} / {c['items']:,} items")
    if v["available"]:
        lines.append(f"DATA VERIFIED  {bar(c['verified'], c['items'], width)}  "
                     f"{_pct(c['verified'], c['items']):4.1f}%   "
                     f"{c['verified']:,} / {c['items']:,} items   "
                     f"{v['verifiedBytes']:,} B exact")
        if c["partial"] or v["partialBytes"]:
            lines.append(f"DATA PARTIAL   {bar(c['partial'], c['items'], width)}  "
                         f"{_pct(c['partial'], c['items']):4.1f}%   "
                         f"{c['partial']:,} / {c['items']:,} items   "
                         f"{v['partialBytes']:,} B compared")
    else:
        lines.append("DATA VERIFIED  (full build report unavailable)")
    lines.append(f"DATA NAMED     {bar(c['named'], c['items'], width)}  "
                 f"{_pct(c['named'], c['items']):4.1f}%   "
                 f"{c['named']:,} / {c['items']:,} items")
    bss_total = c["bssClaimed"] + c["bssUnclaimed"]
    lines.append(f"BSS CLAIMED    {bar(c['bssClaimed'], bss_total, width)}  "
                 f"{_pct(c['bssClaimed'], bss_total):4.1f}%   "
                 f"{c['bssClaimed']:,} / {bss_total:,} BSS items   "
                 f"{report['bytes']['sectionCoverage'].get('bss:single-complete-claim', 0):,} B")
    lines.append(f"DATA CREDITED  {bar(c['credited'], c['claimed'], width)}  "
                 f"{_pct(c['credited'], c['claimed']):4.1f}%   "
                 f"{c['credited']:,} / {c['claimed']:,} claimed items")
    lines.append("```")
    lines.append("")
    lines.append("Data tiers, checks and credit: [notes/data-tracking.md](notes/data-tracking.md).")
    lines.append(README_END)
    return "\n".join(lines)


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


def validate_authors(authors, rows):
    """Return (errors, warnings) for the credit record against this inventory."""
    by_id = {row["id"]: row for row in rows}
    errors, warnings = [], []
    for key, row in authors.items():
        match = ID.fullmatch(str(key))
        if not match:
            errors.append(f"{key}: key must be module:0xaddress")
            continue
        canonical = canonical_id(match["module"], match["address"])
        if canonical != key:
            errors.append(f"{key}: expected canonical key {canonical}")
        if not isinstance(row, dict) or not isinstance(row.get("author"), str) \
                or not row["author"].strip():
            errors.append(f"{key}: missing nonempty author")
            continue
        if key not in by_id:
            errors.append(f"{key}: address is not a configured global-data record")
            continue
        if not by_id[key]["claimed"]:
            warnings.append(f"{key}: credit exists for an unclaimed data record")
    return errors, warnings


def load_baseline(path=None, base=None, repo=REPO, missing_ok=False):
    """Read the baseline file, or its blob at a git ref for PR comparisons."""
    path = pathlib.Path(path) if path else DEFAULT_BASELINE
    if base:
        rel = path.resolve().relative_to(pathlib.Path(repo).resolve()).as_posix()
        proc = subprocess.run(["git", "-C", str(repo), "show", f"{base}:{rel}"],
                              capture_output=True, text=True, encoding="utf-8")
        if proc.returncode != 0:
            if missing_ok:
                return None
            raise ValueError(f"baseline {rel} does not exist at {base}")
        try:
            return json.loads(proc.stdout)
        except ValueError as exc:
            raise ValueError(f"{base}:{rel}: {exc}") from exc
    data = _load_json(path)
    if data is None:
        return None
    return data


def check(report, authors, baseline=None):
    """Credit and schema checks; returns (errors, warnings)."""
    rows = report["items"]
    errors, warnings = validate_authors(authors, rows)
    claimed = {row["id"] for row in rows if row["claimed"]}
    if baseline is None:
        warnings.append("no data-tracking baseline: new uncredited claims are not gated")
        return errors, warnings
    known = set(baseline.get("uncredited", []) or [])
    new = sorted(claimed - known - set(authors))
    if new:
        errors.append(f"{len(new)} claimed data item(s) have no credit row: "
                      + ", ".join(new[:8]) + (", ..." if len(new) > 8 else ""))
    unknown = sorted(known - claimed)
    if unknown:
        warnings.append(f"{len(unknown)} baseline claim(s) are not present in this tree: "
                        + ", ".join(unknown[:8]) + (", ..." if len(unknown) > 8 else ""))
    return errors, warnings


def write_baseline(path, report, authors):
    claimed = {row["id"] for row in report["items"] if row["claimed"]}
    data = {
        "schemaVersion": 1,
        "purpose": "Existing uncredited data claims; new claims need data-authors.json",
        "uncredited": sorted(claimed - set(authors)),
    }
    path = pathlib.Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8", newline="\n")
    return data


def write_authors(path, authors):
    path = pathlib.Path(path)
    data = {
        "note": ("Who decompiled each data item. The key is the module and address; "
                 "moving source or renaming an alias does not change the key. This file "
                 "is the only data credit record."),
        "data": authors,
    }
    path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n",
                    encoding="utf-8", newline="\n")


def credit_source(report, authors, source, author):
    """Credit every complete claimed address owned by one source file."""
    source = source.replace("\\", "/")
    added = 0
    for row in report["items"]:
        if row["claimed"] and row["source"] == source and row["id"] not in authors:
            authors[row["id"]] = {
                "author": author,
                "name": row["aliases"][0] if row["aliases"] else None,
                "source": source,
            }
            added += 1
    return added


def _summary(report, authors_path):
    c = report["counts"]
    v = report["verification"]
    lines = [
        "Data tracking",
        f"  storage items: {c['items']:,} ({c['initialized']:,} initialized, {c['bss']:,} BSS)",
        f"  claimed:       {c['claimed']:,} ({_pct(c['claimed'], c['items']):.1f}%)",
        f"  verified:      {c['verified']:,} ({_pct(c['verified'], c['items']):.1f}%)"
        + (f", {v['verifiedBytes']:,} B exact" if v["available"] else " (no romData report)"),
        f"  partial:       {c['partial']:,}" + (f", {v['partialBytes']:,} B compared"
                                                 if v["available"] else ""),
        f"  differs:       {c['differs']:,}",
        f"  named:         {c['named']:,} ({_pct(c['named'], c['items']):.1f}%)",
        f"  credited:      {c['credited']:,} / {c['claimed']:,} claimed",
        f"  credit record: {authors_path}",
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
    ap.add_argument("--authors", default=str(DEFAULT_AUTHORS))
    ap.add_argument("--baseline", default=str(DEFAULT_BASELINE))
    ap.add_argument("--base", help="read the baseline from this git ref")
    ap.add_argument("--json", help="write the summary tracking report")
    ap.add_argument("--items-json", help="write every per-address tracking row")
    ap.add_argument("--write-readme", action="store_true")
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--update-baseline", action="store_true")
    ap.add_argument("--credit-source", help="credit every complete claim owned by this src path")
    ap.add_argument("--author", help="author name for --credit-source")
    args = ap.parse_args(argv)

    try:
        inventory = load_inventory(args.inventory)
        verification = load_verification(args.rom_report)
        authors, authors_path = load_authors(args.authors)
        report = summarize(inventory, verification, authors)
        if args.credit_source:
            if not args.author:
                raise ValueError("--credit-source needs --author")
            added = credit_source(report, authors, args.credit_source, args.author)
            write_authors(authors_path, authors)
            report = summarize(inventory, verification, authors)
            print(f"credited {added} data item(s) from {args.credit_source} to {args.author}")
        if args.update_baseline:
            baseline = write_baseline(args.baseline, report, authors)
            print(f"wrote {args.baseline}: {len(baseline['uncredited'])} uncredited claim(s)")
        if args.write_readme:
            if not verification.get("available"):
                raise ValueError("--write-readme needs a --rom-report with data verdicts")
            write_readme(README, report)
            print(f"updated {README} data block")
        if args.check:
            baseline = load_baseline(args.baseline, args.base, missing_ok=bool(args.base))
            if baseline is None and args.base:
                # First rollout: the baseline is introduced by this branch, so
                # the base ref cannot name it yet. The working-tree baseline is
                # the only usable one; later PRs read the committed base file.
                baseline = load_baseline(args.baseline)
                print(f"warning: no baseline at {args.base}; using the working-tree baseline",
                      file=sys.stderr)
            errors, warnings = check(report, authors, baseline)
            for warning in warnings:
                print(f"warning: {warning}", file=sys.stderr)
            for error in errors:
                print(f"error: {error}", file=sys.stderr)
            if errors:
                return 1
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
        print(_summary(report, authors_path))
        return 0
    except (OSError, ValueError) as exc:
        print(f"data tiers: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
