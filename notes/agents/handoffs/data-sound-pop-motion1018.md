# Sound pop-motion arrays

This continuation reconstructs 21 three-entry constant arrays in the existing
Sound TU, covering ov006 `.rodata` [0x0212ee40,0x0212ef3c), exactly 252 bytes.
Twenty arrays use `int`; `sSoundPop06VX` retains its existing `u32` contract.
Existing descriptive names remain; original identifiers and declared bounds are
not established by the binary. Consumer kind values 0, 1 and 2 cover every entry.

Definitions replace imports in the existing C-linkage block. No shared header,
method body, initializer, symbol name or attribution changes. Standard manifest
ordering places whole emitted objects in retail order. The delink claims this
complete range; no adjacent bytes or guessed padding are enrolled.

Fresh pinned-compiler comparison preserves 112 existing definitions, 83 functions
and 302 relocation records. The prepared 252-byte contribution matches retail
with zero relocations. Strict verification passes all 83 functions with zero blind
words. The full scratch link reproduces all five enrolled ranges, all 106 modules
and the complete ROM (SHA-256 d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8).
Nine pre-existing symbol errors remain, with zero new errors; the raw symbol
check is not green. Normal-build provenance confirms all 21 actual compiled providers and all 252
bytes. Every bound input hash was rechecked. The normal build reproduces all
106 modules and the same complete ROM. Source style, declaration agreement,
manifest conflicts, port references, rename ledger and dead-reference gates pass.
The global raw data inventory retains one unrelated differing symbol; this is
not an all-symbol inventory acceptance claim.

The experiment record retains every inherited finding ID, using final accepted
reviews for BASE-PROV02 and SOUND2881-02 rather than earlier open snapshots.
Historical Cup/base findings stay scoped to their original work. Existing Sound
component/layout/lifecycle reconstruction remains partial. Broader arrays and 57
PMF descriptors remain deferred, including three no-argument Finish handlers.
[Issue4071](https://github.com/tangosdev/sm64ds-decomp/issues/4071) tracks this data
continuation; the old issue2492 references concern a now-closed Cup issue.

Producer: `codex-sound-pop-data1018`; queue: `data-sound-pop-motion1018`.
Base: `85b3e5c239f435ce63731c85c9c1b6cc7dac88f1`. Independent review and PR publication
follow complete local proof. No merge is authorized or performed.
