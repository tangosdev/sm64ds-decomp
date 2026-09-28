#!/usr/bin/env python3
"""Convert Chaos Viewer's chaos-db.json into an objdiff progress report
(report.json, format version 2) for decomp.dev.

decomp.dev ingests objdiff-format reports from a GitHub Actions artifact named
"<version>-report". The workflow that uploaded it, report.yml, was disabled in
August and deleted on 2026-09-28; run this by hand to regenerate. This reads the same
committed-data chaos-db.json that powers the README bar and the treemap, so the
decomp.dev numbers always agree with the atlas. No ROM or extracted binaries are
needed: matched status and sizes come from committed src/ and config symbols.

Schema: objdiff.report (encounter/objdiff, proto3, REPORT_VERSION = 2), emitted
as canonical proto3 JSON (camelCase keys, 64-bit ints as strings), which is what
objdiff-cli itself writes and what decomp.dev's parser accepts.

Usage:
    python tools/decomp_report.py --db chaos-db.json --out report.json
"""
import argparse
import json
import pathlib

REPORT_VERSION = 2


def _pct(a, b):
    return round(a / b * 100.0, 4) if b else 0.0


def _measures(code, matched_code, funcs, matched_funcs, units, complete_units):
    # Every matched function here is byte-identical and ROM-verified, so
    # "complete" (linked) equals "matched". No data sections are tracked, so the
    # data measures are zero. fuzzy == matched because items are 100 or 0.
    p = _pct(matched_code, code)
    return {
        "fuzzyMatchPercent": p,
        "totalCode": str(code),
        "matchedCode": str(matched_code),
        "matchedCodePercent": p,
        "totalData": "0",
        "matchedData": "0",
        "matchedDataPercent": 0.0,
        "totalFunctions": funcs,
        "matchedFunctions": matched_funcs,
        "matchedFunctionsPercent": _pct(matched_funcs, funcs),
        "completeCode": str(matched_code),
        "completeCodePercent": p,
        "completeData": "0",
        "completeDataPercent": 0.0,
        "totalUnits": units,
        "completeUnits": complete_units,
    }


def _module_sort_key(label):
    # arm9 (main) first, then overlays ovNNN in numeric order
    if label == "arm9":
        return (0, 0)
    if label.startswith("ov"):
        try:
            return (1, int(label[2:]))
        except ValueError:
            pass
    return (2, 0)


def build_report(db):
    by_mod = {}
    for f in db["functions"]:
        by_mod.setdefault(f["module"], []).append(f)

    units = []
    tot_code = tot_matched_code = tot_funcs = tot_matched_funcs = complete_units = 0
    for label in sorted(by_mod, key=lambda m: (_module_sort_key(m), m)):
        fns = sorted(by_mod[label], key=lambda f: f["addr"])
        u_code = u_matched_code = u_matched = 0
        items = []
        for f in fns:
            size = int(f["size"])
            matched = bool(f["matched"])
            u_code += size
            if matched:
                u_matched_code += size
                u_matched += 1
            items.append({
                "name": f["name"],
                "size": str(size),
                "fuzzyMatchPercent": 100.0 if matched else 0.0,
                "address": str(int(f["addr"])),
            })
        u_complete = len(fns) > 0 and u_matched == len(fns)
        meta = {"moduleName": label, "complete": u_complete}
        if label.startswith("ov"):
            try:
                meta["moduleId"] = int(label[2:])
            except ValueError:
                pass
        units.append({
            "name": label,
            "measures": _measures(u_code, u_matched_code, len(fns), u_matched,
                                  1, 1 if u_complete else 0),
            "functions": items,
            "metadata": meta,
        })
        tot_code += u_code
        tot_matched_code += u_matched_code
        tot_funcs += len(fns)
        tot_matched_funcs += u_matched
        if u_complete:
            complete_units += 1

    return {
        "measures": _measures(tot_code, tot_matched_code, tot_funcs,
                              tot_matched_funcs, len(units), complete_units),
        "units": units,
        "version": REPORT_VERSION,
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--db", default="chaos-db.json")
    ap.add_argument("--out", default="report.json")
    args = ap.parse_args()
    db = json.loads(pathlib.Path(args.db).read_text(encoding="utf-8"))
    report = build_report(db)
    pathlib.Path(args.out).write_text(json.dumps(report), encoding="utf-8")
    m = report["measures"]
    print(f"wrote {args.out}: {len(report['units'])} units, "
          f"{m['matchedFunctions']}/{m['totalFunctions']} funcs "
          f"({m['matchedFunctionsPercent']}%), "
          f"{m['matchedCode']}/{m['totalCode']} bytes "
          f"({m['matchedCodePercent']}%)")


if __name__ == "__main__":
    main()
