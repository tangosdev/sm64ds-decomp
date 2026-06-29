# mwccarm DS matching idioms, mined from pret DS decomps

Mined 2026-06-29 from `pret/pokeplatinum` and `pret/pokediamond` (same compiler family:
CodeWarrior mwccarm, same platform: DS arm9 / ARM946E, both matched to ~100%). These are
the C-source idioms their contributors found by experiment for exactly the residuals that
floor us. Evidence = in-source `// ... to match` comments (193 in pokeplatinum, 46 in
pokediamond) plus `register` usages. No inline asm is used for matching in either repo.

## Register allocation and stack layout (attacks WALL #2: callee-saved set / register pressure)

This is the dominant category and the one most likely to move our register-pressure
divergences. mwccarm assigns registers and stack slots largely in source DECLARATION ORDER,
so the source controls the allocation:

1. **Declaration order is load-bearing.** Declare locals in the order the ROM allocates them.
   Reordering local declarations changes which value lands in r4 vs r5 vs a stack slot, which
   changes the pushed callee-saved set. Evidence: "must maintain this declaration order to
   match", "must declare in this order to match", "Need to be declared like this because of
   the decl order", "these 3 variables are shuffled on the stack, everything else matches".

2. **C89-style: declare ALL locals at the top of the block**, not at first use. Evidence:
   "must declare C89-style to match" (x8), "forward declaration required to match",
   "out-lined declaration required to match". Loop induction vars and temporaries especially:
   declare `int i;` at function top, not in the `for`.

3. **Reuse a variable instead of introducing a new one** to avoid an extra slot/register.
   Evidence: "couldn't get a match without reusing the variable", "speciesGBA is reused to
   store NDS species", "must reuse side var to match".

4. **Type choice changes regalloc.** Prefer plain `u32`/`int` locals over `enum` types for
   intermediate values: "purposefully left as u32, using HeapID causes fuckery with regalloc",
   "doesn't match if declared as enum Trap". (Distinct from the global `-enum int` flag; this
   is about the declared type of a *local*.)

5. **`register`-qualified locals**, including arrays, bias allocation:
   `register u32 chunk_starts[3];` (pokediamond filesystem.c). Use when a value must stay in a
   register across a region the compiler would otherwise spill.

## Control flow and expressions

6. **Keep ternaries as ternaries** — do not expand to if/else: "ternary must remain to match".
7. **Loop form matters** — sometimes a `while`/`do-while` is required where a `for` won't match:
   "can't do for loop - needed for matching". (Mirror the ROM's branch direction.)
8. **Keep computationally-dead lines the ROM still emits**: "this line has to stay to match",
   "unused, but must be kept to match".
9. **switch-case order must match the ROM jump table**: "the order of this switch statement
   must be maintained to match".

## Address materialization (WALL #1 lead — weak, only one idiom found)

10. **`void *` pointer arithmetic** is byte-granular and can force an address to be computed in
    a register rather than folded into a load offset: `return (u32)((void *)f(x) + value);`
    Evidence: "MUST be cast to void * to match", "UB: void * access", "leaving return as a
    void * to reflect this". This is the only materialization-adjacent idiom in either repo;
    neither repo documents a recipe for the zero-offset first-access `add rX,base,#OFF; ldr [rX]`
    case specifically, so wall #1 remains largely unsolved even here — but the void* cast is
    worth testing on our materialization near-misses.

## How to apply

For a register-pressure / stack-shuffle near-miss (the common wall-#2 shape): hold the logic
fixed and permute (a) the ORDER of top-of-block local declarations, (b) whether a temp is a
fresh local vs a reused one, (c) the declared TYPE of each local (u32 vs enum/narrow). These
are cheap source edits that change allocation without changing logic, exactly what the permuter
cannot reach. For an ordering near-miss, try the void* idiom and the ternary/loop-form rules.
