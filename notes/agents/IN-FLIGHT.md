# What is in flight, and where that is recorded

Written 2026-09-07 while winding the class pipeline down for a workflow upgrade.

This is a v1 cutover snapshot, not the active v2 queue authority. Follow
[PIPELINE.md](PIPELINE.md) and [CUTOVER.md](CUTOVER.md) for the replacement protocol.
Preserve the observations below, but remeasure the inventory before adoption.

This file exists because of a question with an uncomfortable answer: *is everyone's
state being managed somewhere?* The artifacts are safe. The coordination is not.
Read [The four stores](#the-four-stores) before you trust any single one of them.

## The four stores

| store | what it holds | lifetime |
|---|---|---|
| `refs/claims/<role>/<Class>` on origin | class, role, worktree, holder, host, UTC time, nonce | **deleted on release** |
| `origin/cpp/<Class>-tu` | the work product | durable, but nothing indexes it |
| `notes/data/class-facts/<Class>.json` | what the scout read off the cartridge | durable once it reaches `main` |
| `..\sm64ds-handoff-ledger.tsv` (outside the repo) | the Tango bridge conversation | never committed, machine-local |

The gap is between rows one and two. `classqueue.py` consults **only** the claim
ref, and `cmd_claim` deletes that ref on release — so a class whose agent finished,
pushed a branch and released is offered again as unclaimed, with the finished work
invisible. `prior_work()` mitigates it by printing matching branches at claim time,
and that warning is worth reading, but it is a substring match: claiming `BobOmb`
also lists `cpp/KingBobOmb-tu`, a different class in a different overlay.

**Nothing anywhere records which agent is on which class right now.** That lives in
one conversation's context and in `ListAgents`, and both die with the session.

## Re-derive it, do not trust this file's numbers

Every count below was true when written and starts rotting immediately. Three
commands rebuild the picture from scratch:

    git ls-remote origin 'refs/claims/*'

    git for-each-ref --format='%(refname:short)' 'refs/remotes/origin/cpp/*' |
      while read r; do
        git merge-base --is-ancestor "$r" origin/main || echo "$r"
      done

    gh pr list --state open --limit 100 --json number,headRefName

The second is the one that matters. As written it returned **57 unmerged `cpp/*`
branches with zero open PRs between them** — work that was done, pushed, and then
left where only a branch listing can find it.

## The backlog, by age

- **11 branches** from 2026-09-06/07, the run this file was written during.
- **32 branches** dated 2026-09-05, from an earlier session — the `-tu` / `-tu-build`
  pairs (`daOts_c`, `daPgDfdr_c` at 24 commits, `daDkk_c`, `daDgr_c`,
  `dScMgHanachan_c`, `dScMgMemory2_c`, `Player`, `Eyerok`, `Goomboss`,
  `KingBobOmb`, `Koopa`, and more). Some of this landed on `main` under a different
  branch name and some did not; **which is which has never been established.**
- **14 older branches**, including `cpp/promote-da-obj-lava` at 40 commits.

Two of the older ones carry inline-dtor flips that exist nowhere else. Audit before
deleting anything in this set.

## The 2026-09-07 wave, stopped mid-flight

Five classes were promoted under the ROM-name ruling and the run was stopped
deliberately, to upgrade the workflow. **Nothing is uncommitted and nothing is
local-only** -- every branch below is pushed.

| class | overlay | bytes | methods | where it stopped |
|---|---|---|---|---|
| `daGmch_c` | ov081 | 37/37 | **36/37** | PR #2395, CI green, validator red |
| `daMky_c` | ov030 | 44/44 | 20/44 | PR #2397 |
| `da1up_c` | ov002 | 36/36 | 9/36 | PR #2398 |
| `daMip_c` | ov085 | 32/32 | **32/32** | gated, **no PR** -- `cpp/Rabbit-tu` |
| `daBmb_c` | ov102 | 35/35 | 13/35 | gated, **no PR** -- `cpp/BobOmb-tu` |

184 shards folded into 5 TUs; 110 members became real C++ methods. The wall on
every unconverted member is **naming or scope, never codegen**.

### The blocker that stops all five

`validate_merge.rom_data_regressions` diffs raw `(module, symbol)` sets and has no
rename awareness, so retiring a coined `_ZTV<Coined>` reads as a lost symbol:

    ROM data verification lost 1 exact symbol(s): ov081:_ZTV8Moneybag

Nothing was lost -- `_ZTV8daGmch_c` verifies at the same module, address
`0x02128c04` and `0x84` bytes. **Every promotion under the ROM-name ruling trips
this**, so it is not one PR's problem.

Two workarounds are wrong and must not be taken: a `symbols.txt` alias for the
retired name fabricates a symbol identity to silence a gate, and consulting
`symbols/actor_renames.tsv` lets the PR under validation certify its own rename.
**Anchor on the ROM address instead** -- a lost symbol is not a loss when head
verifies a symbol at the same `(module, address)` with the same byte count, which
cannot be faked because the new name must actually byte-verify there. Match
one-to-one, and when either side's rows carry no address, report the loss exactly
as today.

`romdata_check.summarize()` currently emits `{"module", "symbol"}` only, so the
address has to be added there first. And the validator **restores all of `tools/`
from the base commit**, so this cannot ride along in a class PR -- it lands on
`main` first, then the five revalidate.

### One commit to revert before `cpp/BobOmb-tu` ships

`93d9a31cb` ("Raise the langmode extern_vtable ratchet by one for daBmb_c") banks
a rise that the `daGmch_c` run later proved is not byte-necessary. The `+1` is the
promotion moving a vptr declaration from `include/decl_common.h` into the folded
factory; putting it in the class header instead restored 165/168 with
**byte-identical objects** (`sha256 0c6eee50…`, confirmed against the pinned
mwccarm). `langmode_audit.py`'s own RATCHET block only permits booking a rise that
is renames giving `func_*` functions their evidenced C++ symbols, which this is
not. Do the same move on ov102 and drop the bank.

## What the upgrade should fix

1. **A released claim erases the only record that work happened.** Release should
   leave a tombstone — class, branch, tip SHA, stage reached — not nothing.
2. **`DENIED -- already claimed` does not say who holds it.** An agent that re-runs
   its own claim cannot tell its own lock from a rival's, and `PIPELINE.md` tells a
   denied claimant to take the next row. That is an abandonment bug waiting to fire.
   Print the holder record on denial.
3. **`prior_work()` matches on substring.** `BobOmb` collides with `KingBobOmb`.
   Match on the class name, or on the branch's manifest, not the prefix.
4. **Finishing a stage does not open a PR**, and no stage owns the transition. Every
   one of the 57 is a writer or scout that did its job and stopped one step short.
5. **Scout facts are the durable asset and they land last.** They cost the most
   tokens to produce, they are pure notes with no byte risk, and they sat on
   unmerged branches. They should land as soon as the scout finishes, before any
   writer runs.

## Rules that did hold

Worth keeping through the upgrade, because they were measured and they worked:

- **One agent per worktree, always.** The `build/` corruption is intra-worktree;
  sibling worktrees do not collide, and `build/objcache` is content-hashed and safe
  to share.
- **Concurrent classes in distinct overlays.** Five ran at once across ov030, ov081,
  ov102, ov002 and ov085 with no `symbols.txt` or manifest conflict.
- **Never `git worktree remove`** — it deletes through the junctions and empties the
  shared, non-redownloadable `extracted/` ROM dump. `wt-remove.ps1` only.
