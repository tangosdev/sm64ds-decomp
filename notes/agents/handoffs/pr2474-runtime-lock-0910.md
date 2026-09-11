# PR #2474: early filesystem lock initialization

The host filesystem initializer ran before the OS boot span seeded the lock-ID
words. A debugger on the failing level-0 process measured FS's stored ID as -3,
CARD's unlocked owner as -3, and kind 0. The first archive command called
`func_02060188(-3, 1)`, whose owner/kind check correctly reached `Crash()`.
This is finding **PORT-RUNTIME-01**.

The filesystem initializer now explicitly seeds the mapped lock words before
calling the real FS once-guard. The seed uses a function-local initializer so
its two stores run once per process, including concurrent seed calls. The later
normal boot call preserves the ID already allocated to FS. The allocator's
search, reserved IDs and exhaustion sentinel remain unchanged; this does not
introduce a new concurrent-allocation contract.

`python port/tools/test_os_lockid.py -v` compiles the actual allocator for x86
with the installed MSVC toolchain. Four tests pass: early allocation, preservation
through later boot, all 48 IDs followed by persistent -3 exhaustion, and repeated
concurrent seed calls. The fixture maps the actual shared-page address in its own
process. It does not replace the full host runtime check.

The full battery was run with both shared locks, the normal build step and
shipping-configuration checks enabled. The native build and all 20 smoke programs
passed. Levels 0, 1 and 2 passed; level 3 failed with access violation 0xc0000005
in `HalFaderWipe::AdvanceFade`. The battery stops there, so later levels, scenes,
shipping runtime, multiplayer and rewind are not accepted by this repair.
Evidence is in the producer tree's `build/runtime-lock-tests.log`,
`build/runtime-lock-full-battery.log`, `build/card-debug/result.json` and
`build/level3-debug/result.json`.

This continuation retains the prior PORT2474-01 and PORT2474-02 dispositions.
The original port source base is b99310a89af9c0e4737f81ebde00d5599dba7e31;
PR #2474 targets 31f3160466217cd906516d260dba0ac8f76aee26, which is not an
ancestor of its original 91f5259 head. No retargeting or main integration is
part of this change. The separate transcription, compiler-discovery and wireless
repairs retain their own independent queue acceptance.
