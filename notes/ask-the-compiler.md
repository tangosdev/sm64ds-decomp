# Ask the Compiler

*What matching the ROM's largest unmatched function taught me — `dBgW_Kc::DetectClsn(dBgCh_SphCrr&)`, 1,778 instructions, 1493 → 15 mismatching words.*

---

## The short version, in plain English

Decompilation here means: write C++ that, when fed to the same 2004 compiler Nintendo used, produces *byte-for-byte the same machine code* as the cartridge. Not equivalent code — identical code. So the game is guessing how the original author spelled things.

For this function, everything the code *does* had been correct for a while. What was left was **where each variable lives on the stack** — the compiler assigns every local a numbered slot, and ours were four bytes off from the cartridge's, over and over.

I'd spent two prior sessions trying to work out which slot held which variable by staring at the disassembly. I got it wrong twice, in opposite directions, and once committed the wrong answer.

The unlock was embarrassingly simple: **instead of reverse-engineering where the compiler puts each variable, I asked it.** There's a debug flag that makes it just tell you, and — this is the part that makes it usable — turning it on doesn't change the generated code at all. So I can ask, get a map of every variable's stack slot by name, then throw the debug info away and ship the identical bytes.

That one change took the function from 125 wrong instructions to 36 in a single session, and later work took it to 15 — every one of the function's 99 stack slots now matches the cartridge exactly.

The rest of this doc is the detail, plus eight other things that cost me real time.

---

## 1. Ask the compiler for the frame map

```
mwccarm ... -g          # adds .debug_info; codegen is IDENTICAL
```

DWARF's `DW_TAG_variable` → `DW_AT_location` → `DW_OP_fbreg <sleb128>` is the frame offset of every declared local, **by name**. That is the thing I had been trying to infer.

The byte-neutrality is what makes it a decomp tool rather than a curiosity. Verified across every metric we have: same instruction count, same registers, same slots, same encoded words. `-g` is a pure addition of a section.

**And it names the ROM's frame, not only ours.** This is the step that makes it powerful. Once a candidate has the same instruction *count* as the cartridge and the two streams are position-aligned, the ROM's slot referenced at position *k* holds whatever our slot at position *k* holds. Transfer our DWARF names through that permutation and you are reading the cartridge's frame layout, by variable name, for a binary that shipped in 2004 with no symbols.

That turned an opaque *"109 slot-only mismatches"* into **five named variables in the wrong place**. The search stopped being a blind declaration-order sweep and became "move this one thing."

Three gotchas, all of which bit:

- pyelftools needs `get_dwarf_info(relocate_dwarf_sections=False)` — mwccarm emits RELA relocs against `.debug_info` that its ARM handler rejects outright.
- `DW_AT_location` isn't always an expression block. When it's a loclist the value is an `int`, and `bytes(<int>)` in Python silently yields a zero-filled buffer — so a naive parser reads opcode 0 and drops the entry. My first pass lost half the variables that way, and I nearly concluded a variable had been optimised out.
- **Absence of a DWARF entry is not proof the variable left the frame.** A local can lose its `DW_AT_location` and keep its slot. Ten "wins" in one sweep were byte-identical.

---

## 2. The frame model that fell out of it

The tree's lore was *"declaration order IS the stack layout."* That's roughly half right. What the map actually shows:

```
low  [outgoing args][chain][spill pool][aggregates]  high
```

- **The chain** — declared locals the compiler kept in memory, in declaration order, contiguous. Exactly these get `DW_AT_location`. Its *length* sets where the spill pool begins, so one surplus chain word displaces **every** pool slot. On this function a single 4-byte surplus was worth 44 wrong references.
- **The spill pool** — compiler temporaries, and any local coalesced into one. Nothing here has a DWARF location, which means DWARF separates the two regions for free.
- **The aggregates** — arrays, structs, `Vector3` — go above both.

**The lever: a second definition moves a local out of the chain.** Writing `c = &sphere.pos;` a second time, in a different block, moves `c` from the chain to the pool. Worth +24 references. It must be a *different block* — on the next line it's a dead store, the compiler folds it, nothing moves.

And it's fussy about *how* you write it. It fires on `c` immediately; on `rsc` (`radius << 4`) it resisted roughly 500 compiles across eight mechanisms. For a long time I explained that as *"the lever needs a value that rematerialises without touching memory"*. That was a guess, and it was wrong — §9 has the measured answer, and the fix.

Callee-saved registers are declaration-ordered too: the six hottest locals get `r9, r8, r7, r6, r5, r4`, earliest-declared gets `r9`, and **the seventh loses** and takes a chain slot.

Those two rules look like they contradict each other when the ROM wants a spilled local *early* in the frame. They don't — **the chain is ordered over the locals that HAVE slots**, and the six register winners have none. So the loser can be declared after them and still land as the 34th slot. That single realisation put every named local on the cartridge's own offset.

---

## 3. My measuring stick was lying to me

This one is the most transferable, and the most embarrassing.

```python
def norm(ins):
    if ins.mnemonic.startswith("b"):
        return ins.mnemonic
    return ins.mnemonic + " " + PCREL.sub("[pc]", S.squash(ins.op_str))
```

`squash` normalises immediates. So `str r0, [sp, #0x114]` and `str r0, [sp, #0x120]` compare **equal**. Every scoring metric in the tree is built on that function — which means all of them are structurally blind to a stack-slot difference.

**That is the wrong metric for allocation work, which is exactly the situation that makes you reach for it.** It cost a real sweep: 24 permutations over four slots the byte gate reported as wrong, every one reading "no change," because the score literally could not distinguish them.

The tell was a three-way gap — the gate said 39, my scorer said 36 — and I nearly explained it away. (Both numbers were also inflated by uncounted relocations; see §7.) The fix was a scorer that compares operands verbatim and wildcards only what the linker fills in.

> **Rule:** when the remaining defect is register or stack allocation, cross-check the scorer against the byte gate's own count before believing a null result. A sweep that reports "all byte-identical" on a metric that cannot see your lever is not evidence of anything.

---

## 4. A comment I wrote that was measurably false

I landed this comment on the re-bind:

> *"the cartridge RELOADS `c` from its home here rather than reusing the pointer the `add` above just produced, and re-assigning it to the value it already holds is what re-issues that load."*

Plausible. Matches a real, documented mwccarm behaviour. Explains the +24. **And wrong** — delete the re-bind and the prologue emits exactly the same two reloads. The actual mechanism is the chain-shortening in §2.

The score went up, so I stopped checking. The lesson isn't "be more careful," it's structural: **a lever that works and a story about why it works are two separate claims, and only the first one got tested.** In a decomp repo the stories accumulate into lore that the next person searches by, so a wrong one costs more than a wrong patch. I now try to falsify the mechanism, not just bank the score.

---

## 5. Time the loop before you get clever

I had assumed a scoring iteration cost 30–60 seconds. It costs **~0.45 seconds warm.** I never measured it; the guess quietly shaped an entire session's strategy toward "think hard, test rarely."

At 0.45s the correct strategy inverts: sweep exhaustively. The 1,810-compile sweep in §2 is ~15 minutes of wall clock, and it converts "I have a hypothesis about `rsc`" into "`rsc` is not reachable by this lever at any site in the function" — a much stronger and more durable result.

Related trap: **dead levers are scoped to structure.** A lever measured byte-neutral across 19 variants, with positive controls, became worth +215 instructions after an unrelated structural change. Re-run the sweep after any restructuring; a "dead" result has a shelf life.

---

## 6. Matching pressure occasionally improves the source

Three of the last mismatches were commutative adds — the ROM writes `add Rd, <difference>, <radius term>` and we wrote the operands the other way round. Swapping the source terms is byte-identical, because the compiler canonicalises them.

What broke the canonicalisation was giving the difference a name:

```c
s32 dX = rawX - origin->x;
loX = (dX - (rad6 + 0x40)) >> 6;
hiX = (dX + (rad6 + 0x40)) >> 6;
```

Fixed three mismatches — and it's better code than the version that repeated the subtraction twice. That's worth noting because most matching work pushes the other way, toward contrived spellings you have to apologise for in a comment. Occasionally the cartridge's own shape is just the cleaner one, which is a small piece of evidence that you're converging on what the author actually wrote rather than on a coincidence.

---

## 7. Count the wildcards out before you quote a number

I reported "36 remaining" and it was wrong — not by a rounding error, but because I had never audited what the 36 *were*.

The raw word-by-word diff against the cartridge is 65. Thirty-four of those are **link-time relocations**: 33 `bl` targets, whose destination the linker fills in, plus one data word that holds a pointer. Our object file has zeros there because it hasn't been linked. They are wildcards, not defects.

The data word is the instructive one. An earlier version of my own notes described it as *"a literal-pool word, not an instruction"* and filed it under unfixable. It's a relocation — the source already references the right symbol. I had classified it once, written the classification down, and then trusted the note instead of the bytes.

So the honest count was 31, and after two more fixes it is 15. At the time I wrote this section, two defects remained and they were independent of each other:

| # | What | Status then |
|---|---|---|
| 16 | `c` and `rsc` occupy each other's stack slots: the cartridge chains `c` at `0xc4` and pools `rsc` at `0x104`; we had it the other way | ~500 compiles across eight mechanism families said `rsc` could not be made pool-resident — **§9 shows that conclusion was wrong** |
| 15 | Prologue instruction scheduling | Same 17 instructions, same registers, different order; still open |

One thing worth recording about the first. All 99 stack slots the two streams touch agreed on both address and reference count *except* those two, exactly exchanged — which is why it was a single defect and not sixteen. And I measured *why* the one lever I had works: re-binding `c` moves it out of the declaration chain only because **`c`'s pointee is `volatile`**, which stops the compiler folding the second assignment into the first. My previous explanation ("it needs a value that rematerialises without touching memory") was a guess that fit the data and was wrong — the same mistake as §4, caught faster.

That volatile observation turned out to be the thread to pull; §9 is where it goes.

## 8. When one expression carries two orderings, name the operands

The last fix, and the one I'd have found sooner with better habits.

One instruction differed by operand order — the ROM computes `unk_0ec + radius`, we computed `radius + unk_0ec`. The obvious move is to flip the source terms. I measured that flip in two separate sessions and rejected it both times, because it fixes nothing and *breaks three*: the two loads feeding the add swap places, and a nearby subtraction goes with them.

The reason is that a single expression was carrying two different orderings, and the compiler reads both off the same token order:

- which operand gets **loaded** first
- which operand is the add's **first source register**

Flip the tokens and you flip both. Give them names and they come apart:

```c
s32 ext = sphere.unk_0ec;      /* fixes the load order   */
s32 rad = sphere.radius;
if (da >= -(ext + rad) && da <= ext - rad && ...)   /* fixes the operand order */
```

Loads in the cartridge's order, add in the cartridge's order, nothing else moved.

This is the second instance of the same shape — §6's `s32 dX = rawX - origin->x;` took three commutative adds that no permutation of the original expression could reach. Two independent hits makes it a rule rather than a coincidence, and it retires a belief I'd been carrying: *"naming a subexpression is byte-neutral, so it can't be the lever."* It's byte-neutral when the expression is pinned by one thing. When it's pinned by two, naming is the only way to separate them.

---

## 9. A free statement can still change the allocator's mind

The slot swap in §7 was the big one: 16 of the remaining words, and I had declared it a characterised wall after ~500 compiles across eight families — second definitions, an *exhaustive* sweep of all 92 declaration sites, initializers, `register`, block scope, volatile and const sources, SROA, inlined helpers, every allocation pragma the compiler has. Nothing moved the variable out of the declaration chain.

The answer was one statement that compiles to nothing at all:

```c
rsc = sphere.radius << 4;
rsc = (s32)(volatile s32)rsc;   /* generates NO code */
```

Casting a value to a cv-qualified scalar type discards the qualifier, so the statement emits zero instructions — the function's length doesn't change by a single word. But it marks the value volatile inside the compiler's front end, and *that* demotes the local out of the declaration chain into the temp pool, landing on precisely the cartridge's slot.

The reason eight families of search missed it is the actual lesson. **The flavour of a free second definition picks where the variable lands:**

| written as | effect |
|---|---|
| `x = x + k0` (`k0` a variable holding 0) | folds and forgets — no demotion at all |
| `x = x * k1` (`k1` a variable holding 1) | demotes, but only to the slot right after the chain |
| `x = (T)(volatile T)x` | demotes into the temp pool proper |

I had been treating "give it a second definition" as one binary lever and testing whether it fired. It isn't binary — it's a lever with a *destination*, and I'd only ever tried the two flavours that land in the wrong place. A null result on a lever you've only spelled one way is not a null result on the lever.

Two structural facts fell out alongside it. The pool always begins at chain-end **plus eight** — one structural pad word — so any chained placement of the variable, including an apparently free four-byte alignment gap, shifts every pool slot. And an inline subexpression is not the same thing as a demoted local: spelled inline, the compiler recomputed the value instead of building the pool temp, and the code got longer. That last one is genuine evidence about the original source — whoever wrote this in 2004 declared a named variable there.

One caveat I want to keep attached to this: `(s32)(volatile s32)rsc` is a **matching hack, not a reconstruction**. No human wrote that. It proves the allocator state is reachable and it isolates the mechanism, but the spelling the original author used is still an open question. Those are different claims and it's worth not letting the byte count blur them — which is the §4 lesson again, in a happier register.

## 10. Coda: 15 → 0, from the other side of the fork

The prologue's fifteen fell in a parallel workstream the same day, and the shape of the fix is §9's lesson again from a different angle. ~350 statement-level rewrites of the head block were byte-neutral because the compiler canonicalises them to the same IR before allocation — but a **per-site cv-qualifier cast is its own CSE class**, so `((const Vector3 *)c)->x` never reaches that IR. Three casts, moved above the `rad6`/`origin` declarations, and the add result stops being forwarded into the first read; x takes its own reload, y and z share the second; indices 2..17 come out instruction-for-instruction the cartridge's. Crucially the casts want nothing from `c` itself — it stays chained — so the prologue lever and §9's slot fix compose instead of fighting, and the first compile of the two halves together read:

```
RESULT match=True mismatches=0/1778
```

The function is enrolled at `src/game/actors/dBgW_Kc/_ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr.cpp`. Two flagged untrue constructs ship in it (§9's round-trip, and the `const volatile` pointee) — finding the author's true spellings is open work, and this document's own §4 rule applies to any story anyone tells about them.

The measurement footnote that nearly hid the finish line: the byte gate that ships is the build-flag one (`-Cpp_exceptions off`). fdiff's default regime reads the matched file as 4/1778 — a literal-pool word and the zero-init store order are flag artifacts. Score with the build flags.

## The one-line takeaway

Every one of these is the same shape: **I was inferring something the toolchain would have told me, and I trusted a measurement I hadn't validated.** The frame map, the metric defect, the false comment, the untimed loop, the miscounted relocations, the twice-rejected flip, the one-flavour lever — seven instances of guessing where asking was available. Ask first; it is nearly always cheaper than the inference, and it doesn't quietly return the wrong answer.

And a corollary from §9, which is the one I'd most like to remember: **"I swept that exhaustively" is a claim about the axis you swept, not about the lever.** Twice now the wall came down not to a new idea but to the same idea spelled a different way.
