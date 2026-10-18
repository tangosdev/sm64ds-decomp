# King Bob-omb resource handle storage

Task `data-bombking-handles1017`, issue [#4029](https://github.com/tangosdev/sm64ds-decomp/issues/4029). Producer `codex-bombking-data1017`. This commit is based on `e8dc126bcda1865cce7a4a4cb920fb2095deefb9`; no tooling or shared-header dependency is added. Independent review and PR publication follow the immutable queue offer; no merge is authorized.

## Source and ownership

The production actor source defines thirteen eight-byte POD handles in ov078 BSS `[0x02126ee0, 0x02126f48)`. Twelve use the existing local `BombkingAnimFile` type and one uses `BombkingModelFile`. Compile-time size guards require eight bytes. The layout is a u16 stored file ID, u8 reference count, one padding byte, and a typed loaded-file pointer at +4. Names follow the independently resolved runtime asset paths; they are reconstructed names.

The input runtime IDs 0x2bb..0x2c7 are **not** the values directly stored in `fileID`: `func_02017e0c` obtains the stored ID through `func_02018a24`, clears the count and pointer, and preserves the normal load/release behavior. The asset catalog maps the thirteen runtime inputs to `data/enemy/bombking/` animation/model paths; the experiment report records the exact mapping.

The separate, already enrolled `__sinit_ov078_02126660.c` still constructs the handles and registers their destruction. Its contradictory `extern void*` declarations are replaced by opaque declarations of the exact `BombkingAnimFile`/`BombkingModelFile` struct types defined in the C++ source. It only passes their addresses. No shared header declares these globals, and the initializer requires no payload fields. Their definitions create BSS without emitting an additional initializer.

The `complete` delink for the actor gains exactly the 104-byte BSS interval. Its existing 52-function text and 188-byte initialized metadata intervals retain their ownership. The separate initializer remains a separate production source; this is not a claim that the whole historical TU has been reconstructed. Existing profile, matrix, field and method improvements on current main are preserved.

## Candidate preservation and attribution

DD261 from PR #3937 is preserved as batch commit `2b3f5c857d046f8c7226da451fc59d3dc0af5b80`, authored by lunavyqo with Cursor attribution. Its commit message records original candidate `18a24c13747f`; that abbreviated original object/ref is unavailable here, so the full batch SHA is the reproducible source reference. The later PR3937 shared-header and method changes are not transplanted. Address-keyed function authorship is unchanged.

## Verification

Pinned 2004/b56 compilation emits thirteen global `SHT_NOBITS` symbols of eight bytes and no initializer sections. Comparing the entire set of named initialized symbols preserves all raw bytes and relocation destinations after the thirteen explicit identifier substitutions: 66 actor symbols including 54 emitted functions (52 licensed functions plus existing compiler-policy duplicates), and the complete separate initializer. The two base objects independently match root's hashes.

Focused prepush verifies 53/53 functions including the initializer, with zero warnings or blocking results. The declaration-agreement ratchet passes, source style reports zero findings/deferrals, and all 247 port references resolve. Formatter changes preserve preprocessing tokens. The old names have no remaining live source/header/config references. `check_references.py` initially had no eligibility report and did not establish a pass.

Fresh normal scratch linkage proves all three complete ranges and builds the 16MiB ROM with SHA256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`. Fresh default production passes 106/106 modules, 11,334/11,334 functions, 338/338 complete initialized-data claims and 246 BSS claims (31,320 bytes). The thirteen requested BSS objects are compiler-provider verified from the prepared actor object SHA256 `96bc82fd78567dc98c7fa31caef8787f21c0c240c072d899344b77046498c480`. Production also builds the full ROM with that exact SHA256. The separate initializer is 1,652 bytes at `[0x02126660,0x02126cd4)`; its final linked bytes independently equal retail. Nine strict symbol errors are identical to the same-worker control, with zero new errors; the strict symbol command is not globally clean. The optional broad ROM-data object scan still reports the unrelated `_ZTV10dCcAcPos_c` difference in two existing destructor objects; it does not invalidate the complete-claim/full-ROM result and is not represented as globally clean.

Ten unrelated manifest serialization changes from the scratch tool were restored only after proving JSON equality with the base, before production input binding. The first stale-control refusal and initial overbroad existing-vtable provider request are retained as discarded attempts; final runs use a fresh control and request only the thirteen new BSS objects. The final production report is tied to frozen source/config blobs subsequently committed; finalizing this document does not change compiler inputs. Private logs and reports live under `build/bombking-*`; the compact committed experiment report separates those gates from the object comparison. The externally supplied provenance tools come from PR4015 commit `b045eaba5fdc50e097a78be7f7ac55e0da308816`; source-worktree tools are resolved first, and the guarded wrapper binds the actual source/config/tool inputs and new link outputs.

## Remaining scope

Inherited BKG-06 and ASSET-NAMING-RECONSTRUCTION-01 remain deferred under their original issues/owners, as do unrelated inherited SNM-06 findings. BKG-RESOURCES-001's conflicting handle declarations and absent storage are addressed here; a shared-header layout is still not claimed. Native state constants, runtime state records, registration nodes, vectors and initializer consolidation remain follow-up work under #4029; the existing actor reconstruction debt remains under #2479. Keep the issues open. The queue carries every inherited finding, including historical fixed findings, rather than silently replacing the earlier review.
