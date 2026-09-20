# Handoff: pr2863-source-repair-0920

This commit is a bounded, locally verified producer repair for PR #2863.
Independent source acceptance remains pending. The produce stage stays running
until the integrator reviews the exact candidate; no source branch or queue output
has been published.

- Producer: `codex-spiny-producer-0920` (Codex).
- Input: `ae60f78461fd392862059377bb452143a4f3bd7c`.
- Source base: `9022c77b3834c5ee77c31b4fb4af587418137d6c`.
- Workflow pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Worktree: `C:/tmp/sm64ds-spiny2863-review0920`.
- Branch: `review/pr2863-repair0920`.
- Integrator and next coordination owner: `codex-integrator-0920`.
- Scope: existing Spiny shadow source, this handoff, and the
  [durable experiment record](../../experiments/pr2863-source-repair-0920.json).

## Repair

SP2863-01 is fixed locally: the namespace hiding an incompatible three-argument,
int-returning helper declaration is gone. InitResources uses the actual TU
definition's `void(void*, int)` contract. The timer assignment is unchanged.
The simple truthful call preserves the entire compiled object, so the removed
claim that a third argument is byte-required is unsupported by the current pin.

SP2863-02 is fixed locally: the fabricated `Obj::doit(int)` interface is gone.
The code calls the existing `Model` and `ModelAnim` `Render(0)` methods. Those real
slot-5 virtual calls preserve the entire object in isolation and in combination
with the helper correction.

Independent review of prior repair `3568a524fe60c5dfa2f3d4ff83451a1b3d11515a`
identified two additional blockers, preserved here as SP2863-05 and SP2863-06.

SP2863-05 is fixed locally across the first seven reported contract families:

- `Particle::System::NewSimple` bridges return `void*` at all three declarations.
- The short-reference `ApproachLinear` bridge returns `int`.
- `func_02010844` returns `s32`; assignment still performs the needed short store.
- `DetectRaycastClsn` uses the existing genuine member and reference interface.
- The scalar `Player::Hurt` bridges return `int` and use three `u8` flags.
- The scalar `ModelAnim::SetAnim` bridges take a `u16` start frame.
- The scalar `DropShadowRadHeight` bridge takes `u8` opacity.

These scalar bridges follow their actual current definitions. This does not
reconcile their shared aggregate/member declarations, reconstruct opaque object
views, or claim other inherited declarations are correct. No shared header is
changed. The explicit shadow-versus-definition audit now has no diagnostics for
these seven symbols. DropShadow's actual definition also received a fresh strict
relocation check, with empty differences and zero blind checks.

A second independent review at `fb82671cd8affb9faa26c6d5f33a9062b55429e3`
expanded SP2863-05 with two sound wrappers. Every local declaration of
`func_0201267c` and `func_02012694` now follows their actual definitions:
`void(unsigned int, const Vector3*)`. The ten declarations and eleven existing
coordinate views are corrected together. All IDs at these call sites are positive
constants. Fresh all34 strict checks and complete-object identity pass after this
additional repair; it remains part of the same correctness finding, not a deferred
follow-up. The shared headers and enrolled definitions are unchanged.

SP2863-06 is fixed locally: the false recovered `daJgm_c_Kill` / vtable claim is
replaced by the evidenced state3-entry PMF relationship. Relocations point from
`data_ov077_021278e8` to `func_ov077_021258dc`; the initializer copies that record
to `data_ov077_02127c28+0x30`, selected by the state helper's `state << 4` indexing.
No original private method name is invented. The unchanged enrolled legacy shard
still contains its old comment; this repair covers the owned shadow source.

The original author's cleanup and commit ancestry are retained. No functions or
symbols are moved or renamed, and no attribution, enrollment, header, manifest,
or shared ledger changes belong to this repair.

## Measured proof

The actual PR base, PR head, prior repair, and revised source compile to the identical
21,528-byte object under `2004/b56`, SHA256
`c052dd9b101ed0bc57abb9226bbe5dcc2389964d06bb60519c6adfbe3b61f659`.
Fresh producer verification of the applied source covers all 34 functions /
5,864 bytes: each returns `VERIFIED`, empty `diffs`, and `blind: 0` after object
isolation and module-aware relocation linking. Exact commands, flags, replay
source, per-function results, and unsuccessful alternatives are committed in the
experiment record. No workaround or compiler limitation is claimed.

`tubuild.py --manifest build/spiny2863-sound-repair/manifest.json verify ov077/Spiny`
returns **1**: the functions match, but nine emitted metadata symbols remain
unlicensed, so promotion is refused. Existing policies remove the Spiny D2 and
Vector3 D1; there are zero compiler-only policy errors. The D1/D0 ordinal pair
`[0, 1]` remains reversed. These outputs and ordering are unchanged from both
controls. The earlier stored count of eleven was stale; the fresh count is nine.

The prior repair passed reference checks and its committed Python replay. This
successor also passed the durable replay, reference check and git diff --check;
their local logs remain with its proof. The ordinary declaration ratchet excludes `src_tu`,
so the targeted audit above supplies the bounded contract check instead.

The source remains an unenrolled shadow. A normal full-ROM build would not
exercise this edit, so one was not used as its proof. Shared-header consumer
expansion is unnecessary because no header changed. Local full objects, logs,
scratch sources, and receipts remain under the worktree's ignored `build/`.

## Remaining scope and next action

Reconstruction is **partial**. SP2863-03 remains open: `mMatrix` is still a byte
member plus padding accessed through an M48 aggregate. SP2863-04 records the nine
unlicensed metadata outputs and remaining helper/lifecycle reconstruction.
Other inherited raw field views and call bridges were not accepted by these
bounded repairs. The new correctness/provenance findings were repaired; neither
is placed in a reconstruction follow-up. No follow-up issue has been posted, and this document does not turn that
remaining work into an accepted deferral.

The integrator should review this exact candidate, arrange an independently
claimed verifier, and establish the follow-up issue/owner before partial source
acceptance. The author of these repairs must not verify their own source under
another session identity. Promotion requires separately scoped ownership and
proof; this task neither performs nor authorizes it.
