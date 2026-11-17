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

## Tiers

The address is the item. Aliases, renames and TU moves do not create a second
data item. The key is `module:0xaddress`.

Initialized data and BSS are separate universes. A BSS address has no ROM
payload, so it cannot be verified, and it is not part of the claimed or
verified denominator.

- **CLAIMED** — exactly one complete `delinks.txt` claim covers an initialized
  address. This is declared source ownership, not a byte proof.
- **VERIFIED** — the full build's `romData` report compared an emitted symbol
  at that address and every compared byte matched.
- **PARTIAL** — it was compared, but the emitted extent or a blind relocation
  leaves some bytes unproven. `romdata_check.summarize` lists these as
  `partialSymbols` so a short proof is not dropped.
- **DIFFERS** — at least one emitted copy disagreed with the cartridge. One
  differing copy wins over a correct sibling at the same address.
- **UNCLAIMED** — no complete claim covers the address; the cartridge supplies
  the bytes.
- **BSS_CLAIMED / BSS_UNCLAIMED** — storage, reported on its own bar.

`NAMED` counts addresses whose spelling is not a `data_` / `reg_` placeholder.
The inventory marks that state unreviewed. It is not a count of recovered names.

A verified address is still claimed when a complete window covers it. The
verified bar is the proved subset, the same way MATCHED and LINKED overlap
for functions.

## Commands

```sh
# Claim, named and BSS bars. Verification is reported as unavailable.
python tools/data_tiers.py

# Same report with the full build's verified, partial and differing verdicts.
python tools/data_tiers.py --rom-report build/rombuild-report.json

# Per-address rows and a summary report for another tool.
python tools/data_tiers.py --json build/data-tracking.json \
  --items-json build/data-items.json

# Rewrite the DATA block in README.md. With a rom report, the verified lines
# are rewritten. Without one, the claim bars update and the verified lines
# already in the README stay.
python tools/data_tiers.py --rom-report build/rombuild-report.json --write-readme
```

The inventory is read from `data_inventory.py` when `--inventory` is omitted.
`--rom-report` accepts either the `rombuild.py` report (`romData`) or
`rombuild.py --data-json`'s per-record list. The default is
`build/rombuild-report.json`. If that file is absent the verification tiers
are unavailable rather than guessed.

The function progress workflow refreshes the README on every push to `main`
because `chaos-db.json` is built from committed config. Verified data needs a
full ROM build, which that runner does not have, so the verified lines are
rewritten only when someone passes `--rom-report`. Claim, named and BSS bars
need no ROM.

## How to add data

1. Add the source definition and the complete `delinks.txt` claim.
2. Add the decision evidence to the TU manifest if the range needs more than
   one section or an intact-object proof (see `AGENTS.md`).
3. Run the full build with a report:
   `python tools/rombuild.py -j16 --no-rom --report-json build/rombuild-report.json`
4. Check the row:
   `python tools/data_tiers.py --rom-report build/rombuild-report.json`
5. Regenerate the README DATA block with the same report when the landed row
   changes the public progress picture.

Byte proof stays with `romdata_check` / `validate_merge` / `source_coverage`.
A claim can land before it is verified. The progress tool does not make a
claim true.
