# Active claims

Claim a module or address range here before you start matching, so two people (or
two AI sessions) do not grind the same functions. The batch tools are range-scoped,
so one claimed range per worker keeps everyone on disjoint work by construction.

## First run / no API key

You do **not** need a live claims API key just to set up the repo, try the tools,
or run a first local batch. If you do not have a key from the Discord claims bot,
coordinate manually in this file:

1. Add a claim row for the module/range or batch you are about to try.
2. Commit/push it, or open a small PR if you cannot push directly.
3. Run your local work only inside that claimed range.

`tools/crackloop.py prep` will try the live API only when a key is configured. If no
key is present, it leaves the generated worklist intact and prints the Workflow
launch line; the manual row in this file is your coordination mechanism. The live
API is still preferred for serious concurrent AI batches because it can lock exact
function ranges without git races.

## How to claim

1. Pick a free module or address range that is not listed below.
2. Add a row to the table: the range, your name/handle, today's date, and status.
3. Commit it on its own (`Claim ov0XX`) and push, or open a small PR. Do this before
   you start matching.
4. When you are done or stop, change the status to `done` or remove your row.

Keep one active range per worker. If a range has not moved in a couple of weeks,
it is fair to take over: ping the claimant first.

## Claims

| Range | Who | Claimed | Status |
|---|---|---|---|
| _example: ov004 0x020b0000-0x020b8000_ | _handle_ | _2026-06-17_ | _example_ |
| AI-assisted crack sweep: smallest untried funcs (~0x100 band), spread across modules (this batch mostly ov006/ov007) | beansntoast (AI-assisted) | 2026-06-29 | in progress |
