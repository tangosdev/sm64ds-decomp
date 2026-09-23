#!/usr/bin/env python3
"""Aggregate the SM64DS_NARROWRET probe files and name each call site's caller.

usage: python port/tools/narrowret_live.py <walk_window.map> <census.tsv> <out.tsv> <dir> [<dir> ...]

Every narrowret_<pid>.tsv under the given directories (one per game run; the
header line names the run's SM64DS_LEVEL / SM64DS_SCENE) is summed per
(function, return address). The return address is resolved through the map to
the function that made the call (the nearest public at or below it) and to its
object; the census tells whether that object's source declares the function
WIDE. A LIVE site is a caller that reads the result wide and had at least one
call whose raw register differed from the ARM-extended value.
"""
import bisect
import collections
import glob
import os
import re
import sys

mp, census, outp = sys.argv[1], sys.argv[2], sys.argv[3]
dirs = sys.argv[4:]

syms = []
for line in open(mp, encoding="utf-8", errors="replace"):
    m = re.match(r"\s*0001:[0-9a-f]{8}\s+(\S+)\s+([0-9a-f]{8})\s+f\s+(?:i\s+)?(\S+)\s*$", line)
    if m:
        syms.append((int(m.group(2), 16), m.group(1), m.group(3)))
syms.sort()
addrs = [s[0] for s in syms]


def caller_of(ra):
    i = bisect.bisect_right(addrs, ra) - 1
    if i < 0:
        return ("?", "?", 0)
    a, name, obj = syms[i]
    # several names can share an address (aliases, folded code): prefer a
    # decorated or ROM-looking one over a CRT one, keep them all for the record
    j = i
    names = []
    while j >= 0 and syms[j][0] == a:
        names.append(syms[j][1])
        j -= 1
    return (" | ".join(sorted(set(names))[:3]), obj, ra - a)


# census: (function, declarer object basename) -> wide use lines
wide = collections.defaultdict(list)
for line in open(census, encoding="utf-8", errors="replace").read().splitlines()[1:]:
    c = line.split("\t")
    if len(c) < 13 or c[9] != "WIDE":
        continue
    fn = c[0]
    df = c[6].rsplit(":", 1)[0]
    wide[(fn, os.path.basename(df).lower())].append("%s:%s %s" % (df, c[11], c[12][:90]))

FLAT_OF = {
    "dActor_c::HorzAngleToCPlayer": "dActor_c::HorzAngleToCPlayer",
}

agg = {}
runs = 0
for d in dirs:
    for f in glob.glob(os.path.join(d, "**", "narrowret_*.tsv"), recursive=True):
        runs += 1
        hdr = ""
        for line in open(f, encoding="utf-8", errors="replace"):
            if line.startswith("# narrowret"):
                hdr = line.strip()[len("# narrowret"):].strip()
                continue
            if line.startswith("#") or line.startswith("function\t"):
                continue
            c = line.rstrip("\n").split("\t")
            if len(c) < 6:
                continue
            fn, kind, ra, calls, rb, tb = c[0], c[1], int(c[2], 16), int(c[3]), int(c[4]), int(c[5])
            samp = c[6] if len(c) > 6 else ""
            k = (fn, ra)
            e = agg.setdefault(k, {"kind": kind, "calls": 0, "raw_bad": 0, "ret_bad": 0,
                                   "samples": [], "rows": set(), "badrows": set()})
            e["calls"] += calls
            e["raw_bad"] += rb
            e["ret_bad"] += tb
            row = " ".join(x.split("=")[1] if x.startswith(("SM64DS_LEVEL=", "SM64DS_SCENE="))
                           and False else x for x in hdr.split() if x.startswith(
                               ("SM64DS_LEVEL=", "SM64DS_SCENE=", "SM64DS_NARROWRET_TAG=")))
            e["rows"].add(row)
            if rb:
                e["badrows"].add(row)
            for s in samp.split():
                if len(e["samples"]) < 4 and s not in e["samples"]:
                    e["samples"].append(s)

out = ["function\tkind\tcaller\tcaller_object\tcall_offset\treturn_address\tcalls\traw_bad\t"
       "ret_bad\twide_declared_in_caller_source\tlive\tsamples(raw->ext)\trows_with_bad\t"
       "rows_seen\tcensus_use_lines"]
live_n = 0
for (fn, ra), e in sorted(agg.items(), key=lambda kv: (-kv[1]["raw_bad"], kv[0][0], kv[0][1])):
    cname, obj, off = caller_of(ra)
    objb = obj.split(":")[-1].lower()
    srcb = objb[:-4] if objb.endswith(".obj") else objb
    uses = wide.get((fn, srcb), [])
    if not uses and fn.startswith("_ZN") is False:
        uses = wide.get((fn, srcb.replace(".cpp", ".c")), [])
    wd = "yes" if uses else "no"
    live = "LIVE" if (uses and e["raw_bad"]) else ("bad-raw-narrow-reader?" if e["raw_bad"] else "-")
    if live == "LIVE":
        live_n += 1
    out.append("\t".join([fn, e["kind"], cname, obj, "+0x%x" % off, "%08x" % ra, str(e["calls"]),
                          str(e["raw_bad"]), str(e["ret_bad"]), wd, live, " ".join(e["samples"]),
                          str(len(e["badrows"])), str(len(e["rows"])),
                          " || ".join(uses[:3])]))
open(outp, "w", encoding="utf-8").write("\n".join(out) + "\n")
tot_calls = sum(e["calls"] for e in agg.values())
tot_bad = sum(e["raw_bad"] for e in agg.values())
tot_ret = sum(e["ret_bad"] for e in agg.values())
print("runs=%d sites=%d calls=%d raw_bad=%d ret_bad=%d live_sites=%d -> %s"
      % (runs, len(agg), tot_calls, tot_bad, tot_ret, live_n, outp))
