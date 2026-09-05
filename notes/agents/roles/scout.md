# Role: scout — establish what the ROM says about one class

You produce facts, not code. Another agent writes the class from your file, and
they cannot see the ROM the way you can, so anything you leave out becomes a
guess they make silently.

## Claim

    python tools/classqueue.py next  --role writer
    python tools/classqueue.py claim <class> --role writer --worktree <path>

(The scout claims the `writer` slot, because scouting and writing are one
handoff and splitting the claim lets a writer start on unfinished facts.)

## What to establish

Work from `extracted/`, not from `src/`. `src/` is what we have reconstructed so
far and may be wrong; the cartridge is not.

1. **Identity.** The RTTI name string in the overlay, and its address. The
   registry `class_name` is real RTTI and is trustworthy; a function name in
   `symbols.txt` may be a coined guess. If the two disagree, the RTTI wins.
2. **The vtable.** `_ZTV<mangled>` address. The symbol IS the address point —
   slot 0 sits at the symbol, and the two header words (offset-to-top, RTTI
   pointer) sit at −8 and −4. Dump every slot. The table ends where the next
   RTTI name string begins; trailing null slots are truncated by the linker, so
   a short table is not proof of a short class.
3. **The base class.** Vtable *length* identifies it: 31 words = `dActor_c`,
   32 = `dBgActor_c`. Confirm against which slots point outside this overlay
   (inherited, into arm9) versus inside it (this class's own overrides).
4. **Own overrides.** For each slot whose target lies inside this overlay: the
   slot index, the target address, and the inherited slot name from
   `include/dActor_c.h` (slots 18-30 are declared there in order). Slot index is
   the method's identity — not its address.
5. **Non-virtual methods.** Everything in the class's address range that no
   vtable slot points to. Find callers to distinguish a real method from a
   file-local helper.
6. **Size and fields.** The object size from the allocation call site, and the
   field offsets you can prove from load/store displacements. Mark every field
   you could not prove as `unproven` — do not invent a name for it.
7. **Which overlay(s)** the methods live in, and the module base address.

## Output

Write `notes/data/class-facts/<class>.json`:

    {
      "class": "da1up_c",
      "rtti_name": "7da1up_c",
      "overlay": "ov002",
      "module_base": "0x020ad660",
      "vtable": {"symbol": "_ZTV7da1up_c", "address": "0x021083c8", "slots": 31},
      "base_class": "dActor_c",
      "base_evidence": "31-word vtable; 22 of 31 slots point into arm9",
      "overrides": [
        {"slot": 18, "address": "0x020af3a0", "name": "OnYoshiTryEat",
         "body": "mov r0,#4; bx lr", "note": "returns 4"}
      ],
      "non_virtual": [],
      "size": null,
      "fields": [],
      "unproven": ["object size - no allocation site found"]
    }

Every address is a string in `0x` form. `unproven` is the important field: it is
the writer's list of things they must not fabricate.

## Done when

The JSON is committed and pushed on a branch named `facts/<class>`, and you have
reported: the base class and how you proved it, the override count, and what you
could not prove.
