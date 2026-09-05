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
3. **The base class.** The authoritative evidence is the `__si_class_type_info`
   record: its `+8` word names the direct base outright (for `dBgActor_c`,
   `_ZTI10dBgActor_c` points at `_ZTI8dActor_c`). Lead with that. Vtable
   *length* — 31 words = `dActor_c`, 32 = `dBgActor_c` — is corroboration for
   subclasses, not proof, so do not report it as the reason.
4. **Own overrides.** Ask the tooling rather than reading a header:

       python tools/rtti_extract.py            # writes build/rtti.json -- FIRST
       python tools/rtti_vtables.py --own <Class>

   `rtti_vtables.py` crashes with a bare `FileNotFoundError: build/rtti.json` if
   you skip the extract step. For each slot whose target lies inside this overlay: the
   slot index, the target address, and the inherited slot name. Slot names come
   from **the whole base chain**, not one header: `include/dActor_c.h` declares
   slots 18-30 and is the right source only for a direct `dActor_c` subclass. A
   `dEnemyBase_c` descendant has a 37-slot table and takes names from each header
   up the chain. Slot index is the method's identity — not its address.
5. **Where the RTTI actually lives.** `_ZTI`/`_ZTS` have vague linkage: the
   linker keeps one copy wherever it first landed, which can be a **different
   overlay from the `_ZTV`**. Record the module for each of the three symbols
   separately. `daOts_c`'s vtable is in ov064 while its `_ZTI`/`_ZTS` are in
   ov027. Resolve an ambiguous `dsd` module list by finding which overlay's
   `symbols.txt` names the address.
6. **Non-virtual methods.** Everything in the class's address range that no
   vtable slot points to. Find callers to distinguish a real method from a
   file-local helper.
7. **Size and fields.** The object size from the allocation call site, and the
   field offsets you can prove from load/store displacements. Mark every field
   you could not prove as `unproven` — do not invent a name for it.
8. **Which overlay(s)** the methods live in, and the module base address.

## Output

Write `notes/data/class-facts/<class>.json`:

    {
      "class": "da1up_c",
      "rtti_name": "7da1up_c",
      "overlay": "ov002",
      "module_base": "0x020ad660",
      "vtable": {"symbol": "_ZTV7da1up_c", "address": "0x021083c8", "slots": 31},
      "base_class": "dActor_c",
      "base_evidence": "_ZTI7da1up_c +8 -> _ZTI8dActor_c (__si_class_type_info)",
      "typeinfo": {"ZTI": "0x021138bc", "ZTI_module": "ov027",
                   "ZTS": "0x021138a4", "ZTS_module": "ov027"},
      "modules_touched": ["ov064", "ov027", "ov002", "arm9"],
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

The JSON is committed on `cpp/<Class>-tu` — the *same* branch the writing goes
on, because scout and writer are one claim and two branches for one handoff is
pure friction. Report: the base class and the `__si_class_type_info` record that
proves it, the override count, and what you could not prove.
