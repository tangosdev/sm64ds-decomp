# Ask the Compiler

*What matching the ROM's largest unmatched function taught me — `dBgW_Kc::DetectClsn(dBgCh_SphCrr&)`, 1,778 instructions, 1493 → 36 mismatching words.*

---

## The short version, in plain English

Decompilation here means: write C++ that, when fed to the same 2004 compiler Nintendo used, produces *byte-for-byte the same machine code* as the cartridge. Not equivalent code — identical code. So the game is guessing how the original author spelled things.

For this function, everything the code *does* had been correct for a while. What was left was **where each variable lives on the stack** — the compiler assigns every local a numbered slot, and ours were four bytes off from the cartridge's, over and over.

I'd spent two prior sessions trying to work out which slot held which variable by staring at the disassembly. I got it wrong twice, in opposite directions, and once committed the wrong answer.

The unlock was embarrassingly simple: **instead of reverse-engineering where the compiler puts each variable, I asked it.** There's a debug flag that makes it just tell you, and — this is the part that makes it usable — turning it on doesn't change the generated code at all. So I can ask, get a map of every variable's stack slot by name, then throw the debug info away and ship the identical bytes.

That one change took the function from 125 wrong instructions to 36 in a single session.

The rest of this doc is the detail, plus four other things that cost me real time.

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

And it's fussy in a way that's itself informative. It fires on `c` (`&sphere.pos` — one `add` from the frame pointer). It does **not** fire on `f` (`this->kclFile` — a load) or on `rsc` (`radius << 4` — a load and a shift). Aimed at `rsc` I tried duplicate assignment, `= rsc`, `|= k0`, `+= k0`, `-= k0`, `^= k0`, `&= ~k0`, `*= k1`, at every statement site the variable reaches — 1,810 compiles — and nothing moved. **The lever needs a value that rematerialises without touching memory.**

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

The tell was a three-way gap — the gate said 39, my scorer said 36 — and I nearly explained it away. The fix was a scorer that compares operands verbatim and wildcards only what the linker fills in.

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

## 7. Stop at characterised walls, not at "didn't work"

36 mismatches remain, and I stopped. But "stopped" means something specific here — each remainder has a measured mechanism, not a shrug:

| # | What | Why it resists |
|---|---|---|
| 19 | `c` and `rsc` are exactly swapped: ROM has `c` in the chain at `0xc4` and `rsc` in the pool at `0x104`; we have the reverse | The second-definition lever needs a value that rematerialises without touching memory. `rsc` is a load and a shift. 1,991 compiles say so. |
| 15 | Prologue scheduling, indices 2–17 | Same 17 instructions, same registers, different order. The cartridge issues the centre loads y, x, z and never reads through the pointer the `add` just produced. 11 restructurings, all neutral or worse. |
| 1 | Index 1502, a commutative add | Flipping the source terms swaps the two *loads* instead, and costs three. |
| 1 | Index 1046 | Not an instruction — a literal-pool word that the disassembler renders as `andeq`. |

The difference matters for whoever picks this up. "Didn't work" invites re-derivation; "the lever needs a value that rematerialises without touching memory, and here is the 1,991-compile sweep that establishes `rsc` isn't one" tells them what would have to change for it to become reachable.

---

## The one-line takeaway

Every one of these is the same shape: **I was inferring something the toolchain would have told me, and I trusted a measurement I hadn't validated.** The frame map, the metric defect, the false comment, the untimed loop — four instances of guessing where asking was available. Ask first; it is nearly always cheaper than the inference, and it doesn't quietly return the wrong answer.
