# Typed async-job callback interface

Task `entry-async-interface1021` continues accepted `entry-input1020` from current main `2c27a208676f327a93a60e764396a30f4627cac4`. Producer: `codex-entry-async-interface-producer1021`, assisted by `header_verify_b`. The requested deliverable is a PR; no merge is authorized.

## Contract and consumers

`AsyncJobCallback` is `void (*)(int argument)`. The scheduler already invokes that contract when its worker is occupied: it calls the callback synchronously with the supplied integer, then returns one. Otherwise it creates/resumes a worker and returns zero. The OS-thread constructor stores the supplied argument in saved r0 and initializes the entry PC from the callback address, independently confirming the asynchronous argument contract.

The common header declares the scheduler and two jobs. Boot and MB pass the real function pointers directly, replacing integer conversions and MB's false byte-array declaration of the Boot callback. The two jobs now declare the unused integer argument the scheduler supplies. Entry passes its static `LoadCourseResources(int)` callback without an integer conversion; `StartMatch()` and native null member-pointer assignment remain intact. All four scheduler calls retain their argument, priority, stack-base and stack-size values.

The scheduler uses descriptive argument/local names. A zero stack base still requests allocation; its address and size remain target-width integers. The single callback-to-integer conversion is confined to the unreconstructed OS-thread API, whose definition and other callers still use that address representation. The SDK API itself is not migrated by this task. Existing compiler directives remain unchanged.

## Scope and coordination

Five source files and the new `AsyncJob.h` carry the typed interface. The obsolete `extern void func_02034fbc(void);` line is removed from `decl_common.h` so it cannot contradict the corrected definition. That is the sole change to this shared header. Its existing `jump-contract-repair-0918` reservation remains untouched. The exact one-line cleanup was presented to the user before their instruction to open the prepared PR; the continuation treats that instruction as authorization for this bounded cleanup. No other reserved surface is edited.

No symbols, source paths, compiler pins, enrollment ranges, TU manifests, declaration baselines or attribution records change. Existing ownership and partial class recovery remain unchanged. The Entry class is not consolidated. Remaining callbacks, real construction/teardown and native callback-table initialization retain their predecessor findings.

## Proof

The live shared-header closure has 138 source consumers. The 136 unchanged consumers are compiled with both complete immutable-base and candidate include trees under pinned flags, and their complete ELF objects must be identical. Boot and MB are the two modified members of that closure; together with scheduler, Boot job and Entry loading, five changed sources receive fresh production-object proof. The complete affected union is 141 sources.

Focused proof covers twenty functions and 5,728 bytes. It checks linked bytes and every relocation destination, including the four overlay-ID constants (0, 1, 100 and 102) that the generic audit labels EXTRA; those words require direct candidate-versus-ROM decoding, not blind allowance. Production compiler-only handling, dead stripping and vtable isolation are preserved. No focused owner claims intact data, so that proof does not need or substitute a fresh global stock-control run.

The final immutable candidate also requires full production ROM comparison, official prepush across all affected consumers, declaration agreement, source style and path/reference checks. Independent verification binds its own objects and reports to the exact commit/base. Exact counts, hashes and terminal results reside in queue evidence. The unchanged stock-control symbol diagnostics and broader data inventory are distinct from passing ROM-module and changed-source checks.
