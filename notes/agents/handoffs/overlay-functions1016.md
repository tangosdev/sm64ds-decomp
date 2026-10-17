# Overlay helper reconstruction, first production slice

Candidate: this commit. Base: `67bf07bbb364e06fe50de726ee2b2e99c51d1696`. Producer/integration owner:
`codex-overlay-root1016`. No shared header or attribution ledger changes.
Independent acceptance is recorded separately. Tracking issue #3970 supersedes
the initial queue issue reference from older asset work, which is not the
authority for this overlay-wide scope.

## Production changes

`d_s_entry.cpp` now owns 15 functions in 02115ab8..021165b0, absorbing the
border-palette helper 02116128 and player-row helper 0211621c. Their old shards
are retired and complete delinks enrollment points to the consolidated input.
The canonical promotion planner refuses this already-promoted TU; this is an
extension of its existing source/manifest/enrollment, not a shadow promotion.

The gap originally contained **60** helpers, not 69. The original 82-function
run consists of 13 lower functions + 60 helpers + nine upper functions already
in d_s_entry_methods.cpp. This slice leaves 58 helpers. Every configured inbound
relocation to the 60 helpers originates in the scene run or its step-table data.
The next helper, 021165b0, is modified in PR #3947, so this slice ends before it.
That is a coordination boundary, not evidence for an original source-file edge.

The 18 helpers at 0211a948..0211b524 are a different family: dEntObj calls them
with its embedded 0xa8-byte object at +0xe80. Initializer 0211c51c constructs
its model handle and copies PMFs targeting 0211b260, 0211b1cc and 0211afb0.
Do not fold that family into dScEntry. The scene initializer 0211bb00 constructs
its graph callback and state tables; those typed globals still need recovery.

The entry fold exposed two existing caller declaration disagreements. GetBG1ScrPtr
now returns void*, and the narrow OAM bridge returns OamAttr* and accepts OamAttr*;
casts retain the measured body representation. No baseline entries were added.
The Fix12-by-value bridge remains explicitly documented by the existing compiler
constraint. The row helper's declaration ranks and arithmetic operand order stay
as previously matched; the inline player-index helper receives a descriptive name.

The three ov007 functions 020c94a0/020c94e4/020c951c now explain message lookup,
reset and section binding, with named section tags/count/stride fields. They
still have their current linker names and ABI. The INF1 stride is in bits and
the index is narrowed before multiplication; no fixed eight-byte stride is
assumed. Missing sections retain previous values; reset does not free memory.
The four shared words 02104c18..02104c28 have eleven configured references,
all in these three functions. This supports a coherent parser scope but does
not prove an original TU boundary, so those shards remain separate.

## Remaining reconstruction and overlap

The 525 ov007 functions are not one dScDSMT TU and not a proved SDK library.
They include bespoke file/menu UI, input, message readers, state machines and
graphics. ROM strings include file_menu.ncg and title_demo_file_menu.ncl.
Useful next scopes are the four-function 16-byte state controller
020c92d0..020c93b4, the five-function scene-table decoder 020c95fc..020c9a84,
and grid effects around 020ca5f0/020ca86c/020caeac. Zero initializer witnesses
and the existing under-segmentation notes prevent blind whole-overlay folding.

The message binder's caller 020b8548 supplies an unused third diagnostic-string
argument and overlaps PR #3937; lookup caller 020b8a78 overlaps #3947. Existing
integer/pointer declaration inconsistencies remain explicit. MessageBank.h has
overbroad historical comments about unread fields; no speculative shared-header
change is included. The next owner is the overlay producer, after current PRs
compose and exact interface/consumer proof is reserved.

PR #3961 adds four ov007 overlaps beyond the original audit: 020ae558,
020b5bf0, 020c5014 and 020c8498. Current-main composition must refresh these
heads; the present checks are pinned to the base above, not a future main.

Initial verification: all 15 entry functions MATCH, correct emission order,
objisolate and relocation-destination checks clean. All three ov007 functions
strictly VERIFIED with zero blind relocations. Historical combined-candidate proof is
recorded in overlay-snow-init1016.md; final publication excludes its Snowman fold. Source reconstruction remains partial.
