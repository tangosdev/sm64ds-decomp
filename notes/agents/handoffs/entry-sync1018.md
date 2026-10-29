# Entry player synchronization callbacks

This document describes this commit. The queue records its immutable SHA.

- Task: `entry-sync1018`, issue <https://github.com/tangosdev/sm64ds-decomp/issues/4084>.
- Producer: `codex-entry-sync-producer1018`, Codex; branch `cpp/entry-wait1018`.
- Source and workflow base: `3d76ff61c15e9e00d6d9ea317f4400aa1649764b`.
- Active independent-review policy: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Scope: three existing ov075 functions in `[0x021192bc, 0x02119640)`,
  their declarations/layout, enrollment, and rename bookkeeping. This is a
  partial class reconstruction, not a TU promotion or a completed Entry class.
- Next owner: independent verifier of this exact candidate, followed by the
  coordinator for publication. No merge is authorized.
- The user authorized the reviewed bounded bookkeeping exception by instructing
  us to continue and open the PR after reviewing the pending patch. It changes
  three author-name fields, appends three rename rows, moves two existing
  declaration-exception file keys, and adds one exact-symbol/count-two style
  exception for the inherited OAM bridge. The reservations of
  `issue2410-sound-playsub1015`, `pr-2471-source-review-fixes`, and
  `dactor-native-speed-style-continuation1016` remain intact. Their other entries
  and source files are untouched.

## Source recovery

| Address | Reconstructed method | Size |
| --- | --- | --- |
| `021192bc` | `dScEntry_c::UpdatePlayerSync()` | `0x190` |
| `0211944c` | `dScEntry_c::EnterPlayerSync()` | `0x98` |
| `021194e4` | `dScEntry_c::RenderWaitingText()` | `0x15c` |

The methods use the actual class header and named fields instead of a `char *`
object and offset arithmetic. `WaitingWork` is an inferred union view of four
shared work words, not a claim that the original source used this type or that
these words always contain waiting-state data. Selection, blinking, and results
steps reuse them (`02117590`, `02117fe4`, `021184b0`, `0211867c`). The general
`mStepWork` view keeps that reuse explicit. The wave phase remains signed: a
negative per-glyph phase and values beyond one revolution suppress bouncing.
The shared sound handle and byte-sized step phase retain their observed widths.

The existing Entry TU owns a different contiguous text interval. These three
methods remain individually enrolled at resolver-selected source paths. All
addresses and sizes are unchanged. The native Entry metadata, callback tables,
initializer and the rest of the class retain their existing ownership.

## Naming evidence

`__sinit_ov075_0211bb00` constructs step set `0211d918` from PMFs at `0211c878`,
`0211c760`, and `0211c780`. The ROM words identify the enter, update and render
functions above. The step installer copies them into the class's three member
callback slots and invokes the entry callback.

The enter routine loads encoded asset `0x8409`. Archive registry row
`0208ed08` covers `[0x8400,0x8427)`; its pathname pointer `0208ebd4` identifies
`/ARCHIVE/en1.narc`. Member 9 is
`data/2D_cad/d_2d_battle_framework_waiting_nsc.bin`. The English VS message bank
at `02092d3c` contains message `0x1d`, "One moment please...". These observations
identify the waiting UI; asset bytes remain private inputs, not source additions.

Gate `0203d974` tests download boot or a nonzero wireless station index. Update
waits for player-count agreement and two acknowledgement barriers, with a host
ready flag between them. `020308b4` calls `0203d854`/`0203d868`, which clear the
acknowledgement and set outgoing packet flag `0x1000`; `020308a8` forwards to
the acknowledgement poll at `0203d81c`. The unrelated cannon/course comment in
the legacy `020308b4` source is contradicted by those callees and was not used.
All three method names and the new field names are inferred, not preserved
Nintendo identifiers. Per-function authors remain unchanged.

## Compiler and interface boundaries

The renderer includes `OAM.h` for `RenderSub`; its fake local `OAM` class is gone.
The scalar linkage declaration for the Fix12-by-value Render overload remains
for the documented ABI reason in `OAM.h` and `notes/mwccarm-codegen.md` section
6az. Its return and leading parameter now agree with the existing definition.
The sine/cosine data uses the existing interleaved `s16` representation.

Production `2004/b56` experiments retain two source forms in the renderer:

- Replacing `(u16)(s16)(u16)phase` by `(u16)phase` removes four ARM shifts;
  the tested equivalent expression emits 332 rather than 348 bytes.
- Initializing `y = 96` and then adding the sine displacement instead of the
  explicit two-arm assignment emits 340 rather than 348 bytes.

The accepted body uses `bool waving` and direct constants. Removing the old
`long long` countdown-pointer cast, all six constant temporaries, and all scene
offset accesses preserves the 348-byte function. Reproduce these probes by
applying the stated single expression transformations and compiling with
`tools/linkcheck.py --c <probe> --name _ZN10dScEntry_c17RenderWaitingTextEv
--module ov075 --addr 0x021194e4 --size 0x15c` (the production pin is `2004/b56`).
Private scratch reports are in `build/entry-render-experiment/`.

The message argument, sound return/argument, step-set argument and array
declarations were aligned with existing definitions without changing bytes.
Two pre-existing ARM9 declaration discrepancies remain: `020308a8` is used for
its returned acknowledgement although the veneer source declares `void`, and
`020308b4`'s existing caller passes an `int` despite the callee's `s8` declaration.
Repairing those helpers and their other consumers is separate matching work.
Only the existing exceptions' file keys move with this caller; no additional
declaration mismatch is accepted.

## Proof and remaining work

The unchanged-base build reproduces 106/106 modules, all 11,334 enrolled
functions, and all 394 source-owned data claims. It also reproduces the existing
nine stock-control symbol diagnostics and the wider data inventory's one
difference and fifteen unnamed entries; these are not new failures or green
checks. The three reconstructed methods independently link to all 900 original
bytes with no blind relocations. Source review accepts the shared-state view,
method roles, and preservation of callback order and timeout behavior.

The header's affected set is exactly six sources: these three members and
`d_s_entry.cpp`, `d_s_entry_entry.cpp`, `d_s_entry_methods.cpp`. The 28 owned
functions pass the affected-source linked-byte check. Header layout remains
`0x288`, with all 19 annotated offsets accepted. All 247 port references resolve.
An independent production-object audit verifies all 184 relocation destinations
with zero missing, extra, blind or passenger entries, binding the six source
hashes and the header/config hashes in `build/entry-strict.json`. The result was
repeated after applying the pinned formatter. The declaration and style gates use only the bounded bookkeeping changes
described above; final results are recorded in the subsequent queue evidence.
Final committed-input full-ROM, strict-destination and independent review
results belong in the queue's subsequent evidence rather than a self-referential
SHA or a claim that public CI has already run.

The initializer, other state callbacks, remaining raw scene accesses and network
helper declarations are deferred under issue 4084 to their next scoped owner.
No broader class completion, original-file-boundary recovery, or new native
callback-table ownership is claimed.
