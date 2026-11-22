# Data tracking

`tools/data_tiers.py` is the data equivalent of `tools/tiers.py`. It reports
how much global storage has a source claim, and how much of that the full
build has proved against the cartridge. It does not replace the byte gates:

| Tool | Question it answers |
| --- | --- |
| `tools/data_inventory.py` | What configured storage exists, and which delinks claim covers it? Research backlog, not proof. |
| `tools/romdata_check.py` / `rombuild.py` | Does a compiled source object emit this data symbol byte-for-byte? |
| `tools/source_coverage.py` | Did this change stop building any source-owned byte? |
| `tools/validate_merge.py` | Did the full build lose a verified data symbol or gain a differing one? |
| `tools/data_tiers.py` | What is the per-address status, and what do the progress bars say? |

## The bar

The README has the function lines and one data line. Data is every configured
storage address: `.data`, `.rodata`, and `.bss`. A few configured ranges
are outside the progress total. A complete claim counts, whichever section
it is in.
There is no BSS line.

The address is the item. Aliases, renames and TU moves do not create a second
data item. The key is `module:0xaddress`.

An address is decompiled or it is not. Decompiled means one complete
`delinks.txt` claim covers it. That is the README count.

The text report can also say what a full build did to the bytes:

- **VERIFIED** — every byte of the symbol was compared and matched.
- **DIFFERS** — at least one compared byte disagreed.

A compare that stopped short does not get a status. The address stays
decompiled if the claim is complete, and not decompiled if it is not.

BSS has no ROM payload. A claimed BSS address still counts on the DATA line.

## Commands

```sh
# The DATA line. Verified and differing appear when a build report exists.
python tools/data_tiers.py

# Same report with the full build's verified and differing verdicts.
python tools/data_tiers.py --rom-report build/rombuild-report.json

# Per-address rows and a summary report for another tool.
python tools/data_tiers.py --json build/data-tracking.json \
  --items-json build/data-items.json

# Rewrite the DATA line in README.md. It does not need a ROM build.
python tools/data_tiers.py --write-readme
```

The inventory is read from `data_inventory.py` when `--inventory` is omitted.
`--rom-report` accepts either the `rombuild.py` report (`romData`) or
`rombuild.py --data-json`'s per-record list. The default is
`build/rombuild-report.json`. If that file is absent the verification tiers
are unavailable rather than guessed.

The DATA line is complete claims over every storage address. It needs no ROM.
Verified and differing counts are printed only when `--rom-report` points at
a full build.

## How to add data

1. Add the source definition and the complete `delinks.txt` claim.
2. Add the decision evidence to the TU manifest if the range needs more than
   one section or an intact-object proof (see `AGENTS.md`).
3. Run the full build with a report:
   `python tools/rombuild.py -j16 --no-rom --report-json build/rombuild-report.json`
4. Check the row:
   `python tools/data_tiers.py --rom-report build/rombuild-report.json`
5. The DATA line does not need a hand edit. A push to main that changes
   `src/`, `config/`, or `tools/data_tiers.py` runs
   `python tools/data_tiers.py --write-readme` in `.github/workflows/update-chaos-data.yml`
   and commits the line with the other progress files. The same command
   rewrites it locally.

Byte proof stays with `romdata_check` / `validate_merge` / `source_coverage`.
A claim can land before it is verified. The progress tool does not make a
claim true.
