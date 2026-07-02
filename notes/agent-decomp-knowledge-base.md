# Agent Decompilation Knowledge Base

This note captures operating knowledge for LLM-assisted matching work in this repo.
It is not a substitute for byte verification. Its job is to make agents cheaper,
more repeatable, and less likely to lose useful context between functions.

External seed references:

- Chris Lewis, "Using Coding Agents to Decompile Nintendo 64 Games", 2025-11-04:
  https://blog.chrislewis.au/using-coding-agents-to-decompile-nintendo-64-games/
- Chris Lewis, "The Unexpected Effectiveness of One-Shot Decompilation with Claude",
  2025-11-27:
  https://blog.chrislewis.au/the-unexpected-effectiveness-of-one-shot-decompilation-with-claude/
- Chris Lewis, "The Long Tail of LLM-Assisted Decompilation", 2026-02-16:
  https://blog.chrislewis.au/the-long-tail-of-llm-assisted-decompilation/
- Chris Lewis, "Finding Jingle Town: Debugging an N64 Game without Symbols",
  2025-12-23:
  https://blog.chrislewis.au/finding-jingle-town-debugging-an-n64-game-without-symbols/
- Chris Lewis, "Snowboard Kids 2 is 100% Decompiled", 2026-05-17:
  https://blog.chrislewis.au/snowboard-kids-2-is-100-decompiled/
- Chris Lewis, "Snowboard Kids 2 is Recompiled", 2026-06-21:
  https://blog.chrislewis.au/snowboard-kids-2-is-recompiled/
- Macabeus, "Development Journey on Game Decompilation Using AI", 2025-05-20:
  https://gambiconf.substack.com/p/development-journey-on-game-decompilation
- Macabeus, "Building an AI-Powered VS Code Extension for Decompilation",
  2025-07-26:
  https://gambiconf.substack.com/p/development-journey-on-game-decompilation-808
- Macabeus, "Can LLMs Really Do Matching Decompilation? I Tested 60 Functions
  to Find Out", 2026-03-10:
  https://gambiconf.substack.com/p/can-llms-really-do-matching-decompilation
- Macabeus, "Development Journey on Game Decompilation Using AI" series link
  supplied by project contributor:
  https://gambiconf.substack.com/p/development-journey-on-game-decompilation-841

## Core Loop

For a single function, keep the loop narrow:

1. Read the annotated disassembly and resolved callees.
2. Write the smallest plausible C that explains the logic.
3. Verify with the matching oracle.
4. If it misses, compare instruction shape, form one concrete theory, and test one
   change.
5. Stop or mark difficult once attempts stop producing new information.

The important constraint is not "try harder"; it is "make each attempt explain the
previous miss." Avoid large rewrites that change control flow, local lifetimes, and
types at once, because the resulting byte diff is too noisy to learn from.

## Scheduling

Prefer functions with reusable context:

- Same mnemonic shape as an already matched function.
- Same resolved callee set as an already matched function.
- Small, straight-line functions with few branches.
- Functions in a fresh module or subsystem before its easy head is exhausted.
- Functions whose callees already have known signatures.

Use `tools/worklist.py` as the current scheduling and context source. Useful modes:

```sh
python tools/worklist.py --module ov002 --max 0x80 --easy --examples 2 --limit 20
python tools/worklist.py --spread --similar --max 0x120 --examples 2 --limit 50
python tools/worklist.py --module ov002 --addr 0x020b7f2c --pretty --examples 3
python tools/worklist.py --list-classes
```

The `--similar` and `--examples` modes are the local version of the "similar
functions first" lesson: matched functions are not just progress, they are retrieval
examples for future functions. Keep matched code clean enough to be a good example.

## Context Budget

Do not dump the whole project into an agent prompt. Provide:

- Module, function name, address, size, and target bytes.
- Annotated disassembly with resolved branches, callees, and literal pools.
- Known signatures from `tools/knowledge.py`.
- One to three matched examples when they share mnemonic shape or callee set.
- Relevant type definitions, macros, and project idioms when they are likely to
  explain source intent hidden by assembly.
- The relevant compiler/codegen note, usually from `notes/matching-style.md` or
  `notes/mwccarm-codegen.md`.

Make extra context available on demand through tools and notes. Unasked-for full
assembly listings, broad repo summaries, and speculative docs usually burn tokens
without improving the match.

Examples are part of the prompt budget, not free context. Start with a small cap
and measure. A useful example usually shares calls, mnemonic shape, type usage, or
nearby subsystem behavior with the target.

## Tooling Contract

Tools should be boring, explicit, and hard to misuse:

- Matching success means byte-identical output under the pinned mwccarm flags.
- Error output should say whether the failure is compile, match, relocation, or tool
  setup.
- Agents should use the repo's verifier instead of inventing alternate build paths.
- A failed attempt should leave enough artifact or summary to explain what was tried.

Current anchors:

- `tools/match.py`: compile and compare one candidate.
- `tools/fdiff.py`: inspect concise instruction-level differences.
- `tools/worklist.py`: emit self-contained unmatched function records.
- `tools/knowledge.py`: recover signatures from demangling and landed source.
- `tools/linkcheck.py`: stronger linked-byte relocation destination audit.
- `tools/permuter/`: last-mile shape search, not a substitute for logic.

When a mismatch is shown to an agent, prefer a bounded, function-scoped diff over
raw object or whole-module output. The diff should be formatted for diagnosis:
expected instruction, actual instruction, offset, relocation notes, and a short
source-location hint when available.

## Mechanical Fixups

Some AI mistakes are predictable and should be fixed by tools rather than prompt
pressure:

- Declarations in invalid places for the project's C dialect.
- Missing or inconsistent struct offset comments.
- Project-specific fixed-point notation or macro spelling.
- Formatting that fights local conventions.

Use AST-aware or parser-aware transformations where practical. Text rewrites are fine
for narrow, proven cases, but they should be tested against examples because C syntax
has many traps. Prompt rules are best reserved for semantic choices the tool cannot
make.

## Permuter Policy

Use permuters only after the candidate is logically correct and very close. They are
useful for register allocation, expression ordering, and local compiler-shape search,
but they can also introduce source that is less plausible and less useful as a future
example.

Do not treat a permuter improvement as proof that the source is better. Re-check for:

- Deleted calls or side effects.
- Strange variable reuse that hides the real structure.
- Control-flow artifacts that make related functions harder to match later.
- Local-optimum changes that increase percentage but move away from original intent.

## Long-Tail Strategy

As easy functions disappear, change the work rather than grinding the same queue:

- Rank by similarity to matched functions, not only by estimated difficulty.
- Clean up matched functions that are poor references, while preserving byte match.
- Add narrowly scoped notes for recurring idioms and verified codegen levers.
- Split work by module/range through the claims API before running batches.
- Keep hard residues in a log with the best current theory instead of re-discovering
  the same blocker.

Hard categories to expect:

- Large functions with many branches and live values.
- Graphics/GX/display-list-like code where macro intent is hidden by bit packing.
- Matrix, vector, trig, and fixed-point math where small algebra changes affect shape.
- mwccarm register allocation and base-materialization residue.

## Cleanup As Knowledge Capture

Matched code can be technically correct but still bad as a reference. Cleanup is useful
when it improves names, types, struct layout, or readable idioms without weakening the
match.

Record cleanup debt in `notes/readability-debt.md` when the clearer source would likely
break byte matching. Record reusable compiler-shape facts in `notes/matching-style.md`
or `notes/mwccarm-codegen.md`, not in one-off chat history.

## Runtime Validation

Static matching explains what instructions do. Runtime validation explains what the
game is doing with them.

Use debugger or emulator observation when static evidence leaves multiple plausible
meanings:

- Break on a suspected loader, state transition, actor method, or table consumer.
- Inspect inputs and global fields by address/offset when names are still missing.
- Change one menu selection, level, character, or save flag at a time and compare the
  observed values.
- Promote confirmed meanings into names, struct fields, or notes only after the
  observation is repeatable.

This matters most for subsystem documentation. A function can be byte-matched and
still have names like `unk7`; a breakpoint can turn that into "current level index" or
disprove the theory quickly.

## Guardrails

Agents must not improve apparent progress by changing the measurement:

- Do not edit ROM hashes, expected-byte data, generated outputs, or progress files
  unless the task explicitly requires it.
- Do not change symbols or config for naming-only research; write notes first.
- Do not bypass the verifier or report a match without running it.
- Do not overwrite unrelated dirty worktree changes.
- Lock active ranges through the live claims API before matching work.

Prompting alone is not enough for these rules. Prefer scripts, hooks, and tool errors
that make the wrong action fail loudly.

## Batch Driver Shape

A safe unattended batch needs:

- A parent scheduler that selects candidates, rather than asking the agent to browse.
- One candidate per agent run.
- A bounded attempt budget or a clear "no new information" stop rule.
- Append-only logs of candidate, timestamp, model, outcome, and failure reason.
- Duplicate-attempt detection so the agent does not resubmit the same C repeatedly.
- Timeouts split by failure mode: no first token, slow/stalled reasoning, and no
  submitted candidate.
- Graceful stop behavior that lets the current function finish.
- Commits or durable artifacts only after verification.

The repo already has most of the inputs for this shape in `tools/worklist.py`,
`tools/knowledge.py`, `tools/match.py`, `progress/`, and `nearmiss/`. The missing
piece is a first-class run log that records failed theories and successful patterns in
machine-readable form.

For automated runs, treat the LLM as one plugin in a pipeline:

1. Gather context.
2. Try deterministic templates and simple programmatic candidates.
3. Run the compiler and diff oracle.
4. Let the LLM iterate with bounded tool access.
5. Run a permuter only on close, logically plausible candidates.
6. Accept only verified matches, and log everything else.

The agent should not be able to edit project files, config, expected outputs, or
progress files directly during an unattended attempt. It should submit candidate C
through a controlled interface that compiles, diffs, and records the result.

## Evaluation

Do not tune prompts, model choices, or scheduling heuristics by vibes. Keep a small
benchmark set that spans easy, medium, and hard functions, then run changes against
the same set.

Track at least:

- Match rate.
- Attempt number where the match occurred.
- Compile-error rate.
- Duplicate-submission count.
- Timeout count by timeout type.
- Best divergence reached for failures.
- Whether templates, LLM, or permuter produced the final match.
- Post-match code quality issues found in review.

Low attempt counts matter. If a function has not improved after a few attempts, the
conditional chance of success is probably dropping; either escalate effort, change
context/examples, switch strategy, or stop and preserve the best failure record.

Separate "found a match" from "landable code." A second review/refinement phase may
be needed to replace unnecessary pointer arithmetic, restore project macros, recover
struct fields, and remove artifacts introduced only to satisfy the compiler.

## After Matching

100% function coverage is not the end state. It means every function has a C
implementation that can reproduce the original code. The knowledge work continues:

- Replace generated function names with evidence-backed names.
- Recover structs, enums, tables, asset references, and subsystem boundaries.
- Remove or quarantine any targeted inline-asm/register-coercion hacks where a clean
  source shape is later found.
- Split hardware-facing code from game logic so future portability work has a clear
  boundary.
- Use modding or targeted feature experiments as a way to force understanding of one
  slice of the game.

For this repo's stated PC-port roadmap, decompilation knowledge is useful even before
everything is matched: it identifies what is code vs data, which hardware interfaces
need replacement, and where generated/recompiled output needs special handling.

## Open KB Backlog

- Define a JSONL schema for agent attempts: function, context source, model, attempts,
  outcome, best divergence, theory, and next action.
- Add a script that summarizes failed attempts by cause: compile error, wrong callee,
  control-flow shape, register allocation, materialization, or unknown.
- Add duplicate-candidate hashing to any batch runner.
- Add timeout classification to batch logs.
- Extend `tools/worklist.py` similarity beyond exact mnemonic/callee matches, using a
  bounded edit distance over opcode sequences.
- Evaluate embeddings or locality-sensitive hashes over normalized assembly for
  example retrieval, but keep the current mnemonic/callee indexes as the cheap
  baseline.
- Add AST-aware fixups for C dialect issues and project-style boilerplate if they
  recur in agent output.
- Add domain notes for DS GX, fixed-point math, actor/state machines, save data, and
  particle/model animation idioms as those subsystems are worked.
- Promote durable discoveries from chat transcripts into `notes/` immediately after
  verification.
