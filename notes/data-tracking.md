# Data tracking

`tools/data_tiers.py` is the data equivalent of `tools/tiers.py` and
`function-authors.json`. It does not replace the existing data tools:

| Tool | Question it answers |
| --- | --- |
| `tools/data_inventory.py` | What configured storage exists, and which delinks claim covers it? Research backlog, not proof. |
| `tools/romdata_check.py` / `rombuild.py` | Does a compiled source object emit this data symbol byte-for-byte? |
| `tools/source_coverage.py` | Did this change stop building any source-owned byte? |
| `tools/validate_merge.py` | Did the full build lose a verified data symbol or gain a differing one? |
| `tools/data_tiers.py` | What is the per-address status, credit and progress picture? |

## Tiers

The address is the item. Aliases, renames and TU moves do not create a second
data item; the key is `module:0xaddress`, the same spelling used by
`function-authors.json`.

- **CLAIMED** — exactly one complete `delinks.txt` claim covers the address.
  This is declared source ownership, not a byte proof.
- **VERIFIED** — the full build's `romData` report compared an emitted symbol
  at that address and every compared byte matched.
- **PARTIAL** — it was compared, but the emitted extent or a blind relocation
  leaves some bytes unproven.
- **DIFFERS** — at least one emitted copy disagreed with the cartridge.
- **UNCLAIMED** — no complete claim covers the address; the cartridge supplies
  the bytes.
- **BSS_CLAIMED / BSS_UNCLAIMED** — storage has no ROM payload, so it is
  reported separately instead of being mixed into initialized data.

`NAMED` is a quality count: an address with a non-`data_`/`reg_` name. It is
independent of the byte tiers. `CREDITED` counts claimed addresses with a row
in `data-authors.json`.

## Commands

```sh
# Human-readable report. Without --rom-report the verification tiers are
# explicitly unavailable; claimed/named/BSS/credit still work.
python tools/data_tiers.py

# Same report with the full build's verified/partial/differ verdicts.
python tools/data_tiers.py --rom-report build/rombuild-report.json

# Per-address rows and a summary report for another tool.
python tools/data_tiers.py --json build/data-tracking.json \
  --items-json build/data-items.json

# Rewrite the DATA block in README.md.
python tools/data_tiers.py --rom-report build/rombuild-report.json --write-readme

# Credit every complete claim owned by a source file.
python tools/data_tiers.py --credit-source src/data/arm9/data_02075230.c \
  --author lunavyqo

# CI: malformed/stale credit plus new claims that have no author row.
python tools/data_tiers.py --check
python tools/data_tiers.py --check --base origin/main   # PR comparison
```

The inventory is read from `data_inventory.py` when `--inventory` is omitted.
`--rom-report` accepts either the `rombuild.py` report (`romData`) or
`rombuild.py --data-json`'s per-record list. The default is
`build/rombuild-report.json`; if that file is absent the verification tiers are
reported as unavailable rather than guessed.

## Credit

`data-authors.json` is the only data credit record. One row per claimed data
address, keyed by module and address. Historical rows were seeded from the git
blame of the source definition and carry `basis: "git-blame"`; they can be
corrected with evidence in a normal PR. New data work adds its own row.

A new claim needs a credit row. The existing backlog is named in
`config/data-tracking-baseline.json`; the baseline permits its old uncredited
claims but does not permit a **new** claim to land without an author. That is
why `data_tiers.py --check --base origin/main` is a PR gate while the older
inventory remains honest about its uncredited rows. The rollout baseline also
contains the claims of the data batch that was already in flight when the
tracker landed, so merging that batch does not turn the new gate red; its rows
can be credited separately when their authorship is settled.

Moving a data definition to another source file does not change its key, so it
does not need a credit edit. A rename of an alias is also not a new address;
`name` in the credit row is descriptive.

## How to add data

1. Add the source definition and the complete `delinks.txt` claim.
2. Add the decision evidence to the TU manifest if the range needs more than
   one section or an intact-object proof (see `AGENTS.md`).
3. Credit the address:
   `python tools/data_tiers.py --credit-source <path> --author <name>`.
4. Run the full build with a report and check the row:
   `python tools/rombuild.py -j16 --no-rom --report-json build/rombuild-report.json`
   then `python tools/data_tiers.py --rom-report build/rombuild-report.json --check`.
5. Regenerate the README DATA block with the same report if the landed row
   changes the public progress picture.

The credit check is intentionally separate from the byte check. A source claim
can be credited before it is verified; verification is established by
`romdata_check`/`validate_merge`, never by the author row.
