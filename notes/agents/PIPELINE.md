# Class reconstruction pipeline

Write the game's C++ classes back, one class at a time, with several agents
working at once and no human holding the thread in their head.

Every rule an agent must obey lives in this directory as plain Markdown. Nothing
here depends on a Claude skill, a Claude memory, or a particular model — a Codex
instance and a Claude instance read the same files and follow the same protocol.

## The shape

One class moves through five stages, plus a sixth that is not per-class. Each is a separate agent run with a fresh
context, reading the previous stage's written output rather than inheriting a
conversation.

| stage | role | consumes | produces |
|---|---|---|---|
| 1 | `scout` | the ROM | `notes/data/class-facts/<Class>.json` |
| 2 | `writer` | the facts file | one `src/actors/<Class>.cpp`, its manifest, and the bookkeeping below |
| 3 | `humanizer` | the written source | a revised source that reads like 2004 EAD C++ |
| 3b | `writer` again | the humanized source | as many members as byte-match allows turned into real `<Class>::` methods |
| 4 | `builder` | the revised source | green byte gates, then a PR |
| 5 | `reviewer` | the PR | merge, or a rejection with a named reason |
| - | `integrator` | several already-green PRs | one branch, one validator run |

The `integrator` is not a stage - it runs only when several proven class PRs are
open at once, and it exists purely because the validator is one box. See
`roles/integrator.md`. It re-derives nothing; it moves proven commits onto a
fresh base and proves that the composition changed nothing.

Stage 2 is wider than its one row suggests. Besides the source and manifest, a
promotion edits `delinks.txt`, `converted-baseline.json` (via `tiers_ratchet
--update`, never by hand), `converted-backslide-exceptions.jsonl`, any `port/`
slice manifest naming a deleted shard, and any prose naming one. Six non-source
files is typical.

The writer is **gathering, not authoring**: 387 of 429 classes already have a
real header, and the shards being folded together are existing matched code. Most
promotions touch no header at all.

## A promoted TU is not a reconstructed class

`status: promoted` proves the shards were folded into one TU that byte-matches. It
says **nothing** about whether the class got any methods, and until 2026-09-06 no
role file asked for any. The result, measured on `origin/main` from each manifest's
`functions[]` (mangled `_Z...` vs `func_*`):

| class | members | real methods |
|---|---|---|
| `dScMgCoin_c` | 33 | 0 |
| `dScMgTeresa_c` | 48 | 2 (D1/D0 only) |
| `dScMgPanel_c` | 71 | 6 |
| `dScMgCup_c` | 32 | 15 |
| `dScMgMemory2_c` | 52 | **51** |

Every one of those is `promoted`, and every queue and coverage metric counts them
equally. A TU of thirty `extern "C" func_ovNNN_*(char *self)` free functions in
`src/actors/<Class>.cpp` is a **merged file, not a reconstructed class** -- and the
goal is writing the classes back.

**The ruling: convert as far as byte-match allows.** `dScMgMemory2_c` is the proof
the route works end to end. Stage 3b owns it; it is the same `writer` role file and
may run as a separate pass on the same branch when stages 2 and 3 have already
pushed. A member that will not convert byte-neutrally **stays a free function** --
that is a result, not a failure. Report the count either way: "31/31 MATCH" hides
"1 of 31 is a method".

**Renaming a member is one edit, not two.** The new mangled name must reach
`symbols.txt` in the same commit: any pointer-to-member record in unowned `.data`
that still spells its target `func_*` links as `0x00000000`, because dsd resolves
those records by NAME. No byte gate at stage 2 or 3 catches it. And a class member
function **cannot sit inside an `extern "C" { }` region**, so the one file-scope
region goes after the last surviving `func_*` member.

## Coined-name classes are parked

A class whose name was coined rather than read from the cartridge's RTTI **is not
eligible for promotion**. Coined names block data verification against the ROM, and
a promotion under one banks a claim the cartridge cannot corroborate.

This parks 143 of the 201 unpromoted queue rows (2,341 shards) until the naming
question gets its own pass. Check the class's `identity_evidence` in its facts file
before claiming: `dScMgCurling2_c` is in scope because the ov006 bytes at
`0x0213c4c8` are literally `15dScMgCurling2_c`. **Do not run `class_rename.py`** to
get around this.

Stage 4 is the only stage that DECIDES whether the bytes are right, and it owns
`linkcheck` and `rombuild`. That is not the same as "stages 1-3 never run a byte
gate", which this file used to say and which is actively harmful at stage 3: a
humanizer's edits move bytes routinely, `tubuild.py verify` costs 2.6 seconds on
a 71-member TU, and running it after every batch is the difference between
knowing and bisecting. Stage 3 never changes semantics. Stage 5 is a different
agent from stage 4 — a producer's own green output is evidence, not review.

**Every merge to `main` invalidates the pinned base of every other open PR.** So
the cost of N separately-landed promotions is not N validator runs; it is N runs
plus N-1 rebases plus the ledger conflicts those rebases cause. Past two or
three open class PRs, the integrator is cheaper than the queue.

## The queue

**`notes/data/tu-promotion-queue.tsv` is the live queue.** Each row is a class
whose method bodies are still scattered across one-function shards, ranked
best-first by how many shards the promotion absorbs. It carries `shard_count`,
`promotion_route`, `blockers`, and a `sibling_oracle` naming an already-promoted
class in the same overlay to copy.

`notes/data/class-build-worklist.tsv` tracks a **different axis** — whether a
class has a real *header* — and 387 of 429 are already `DONE` there. It is the
right list for the 27-class header tail and the wrong one for this work:
`dBgActor_c` reads `DONE` in it while sitting at row 12 of the promotion queue
with 11 shards outstanding. `classqueue.py` prefers the promotion queue for
exactly this reason.

## Claiming work

A claim is a ref under `refs/claims/` pointing at a **parentless commit** unique
to the claiming agent. Two parentless commits are never ancestor-related, so a
second claimant's push is always a non-fast-forward and always rejected; the
claim is then read back from the remote to confirm our object actually landed.
No shared file is edited — a claims *file* is what produces merge cascades.

The parentless part is load-bearing. An earlier version pushed `HEAD`, which git
rejects only when the pusher is *behind* the holder. Agents branching off one
commit push the same SHA, git answers `Everything up-to-date` and exits 0, and
**every agent believes it won**. Do not simplify it back.

    python tools/classqueue.py next    --role writer
    python tools/classqueue.py claim   dActor_c --role writer --worktree C:/tmp/sm64ds-dactor

`claim` takes the **bare class name**. `ovNNN/dActor_c` is accepted and
normalized to the same ref — a class lives in one overlay, so the prefix adds
nothing. Before that normalization the two spellings built two different refs
and two agents could hold one class simultaneously; live claims existed in both
forms. Create the worktree first and claim once with `--worktree`: re-claiming
to attach the path afterwards is denied against yourself.

**Use forward slashes in `--worktree`, as the example above does — it is
required, not cosmetic.** A backslash path passed through a shell arrives with
its separators eaten and the claim records
`"worktree": "C:tmpsm64ds-memory2-build"`, which points at nothing and defeats
the whole reason the field exists.

    python tools/classqueue.py release dActor_c --role writer
    python tools/classqueue.py list

`claim` exits non-zero when the class is already held. That non-zero is the
lock. Do not retry it, do not force-push over it — take the next row.

Release your claim when your stage's output is committed and pushed, not when
you personally are done thinking.

**A released claim does not mean nobody has done the work.** The claim ref is
the only thing `classqueue.py` consults, so a class whose scout pushed a branch,
opened no PR and released its claim is offered again as unclaimed — and the next
agent rediscovers the whole thing. That happened to `dScMgCoin_c`. Before you
start, look:

    git branch -r --list 'origin/cpp/<Class>*'

If one exists, read it before you cut a new one.

## Launching an agent

Point any instance — Claude Code or Codex — at one role file and one class:

    Read notes/agents/PIPELINE.md and notes/agents/roles/<role>.md in
    C:\Users\andre\source\tangosdev\sm64ds-decomp, then execute that role for
    the next unclaimed class. Claim it first with tools/classqueue.py.

The role file carries the rest. An agent that finishes its class should claim
the next one rather than stopping.

## Rules that hold for every role

1. **Name your worktree and your ref in every claim about a file.** This tree
   routinely has 150+ sibling worktrees at different refs, and the main checkout
   is itself usually stale. `git show <ref>:<path>` carries the ref in the
   command; a bare file read does not.
2. **One worktree per agent**, created with the `decomp-worktree` recipe, torn
   down with `wt-remove.ps1`. Never `git worktree remove` — it deletes through
   the junctions and empties the real ROM dump, which cannot be re-downloaded.
3. **Never edit a shared ledger by hand.** `symbols/actor_renames.tsv` and
   `notes/data/c-cpp-classification.tsv` are append logs whose row *order* is
   load-bearing; a hand merge silently inverts rename chains.
4. **A near-miss never lands in `src/`.** Bank it in `nearmiss/db.jsonl` and
   restore the matched source.
5. **Report the outcome, not the effort.** If a gate failed, paste the failure.
6. **Force-pushes are blocked. Merge, do not rebuild — and do not rebase a
   branch that is already on the remote at all.** Every stage after the first
   inherits a pushed branch, so rebasing it makes your own push a
   non-fast-forward against the remote's copy of *the same commit*; repairing
   that costs an add/add conflict on the promoted `.cpp`, resolved correctly
   only by first proving the remote side byte-identical to what you started
   from. Merge `origin/main` in instead. And a branch rebuilt as a fast-forward
   from an older commit ships stale tree-derived work past review — which does
   not look stale, it looks confident.
