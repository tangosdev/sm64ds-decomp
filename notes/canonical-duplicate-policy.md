# Canonical duplicate functions

A promoted C++ TU can emit a vague-linkage function whose retail callback still
has a generic configured name. `compiler_only_output` may name that exact home:

```json
{
  "symbol": "_ZN7Vector3C1Ev",
  "disposition": "deadstrip-duplicate",
  "canonical_symbol": "func_0203d384",
  "canonical_module": "arm9",
  "canonical_address": "0x0203d384",
  "reason": "Original Vector3[7], stride-12 array construction callback."
}
```

The optional module/address pair must agree with the canonical symbol's unique
configured home. The canonical name must itself denote a sized function; a data
alias at the same address cannot borrow that function's extent. Neither spelling
nor the canonical range may belong to the TU. Full function and non-text storage
intervals are checked even when the manifest has no section rows. Non-text
addresses, sizes, and address-point metadata must form a valid positive extent. The emitted spelling must have no
configured home. Duplicate destinations, alias chains, overlapping policies and
an already-emitted canonical destination are refused.

Before any section removal, both scratch/intact preparation and ordinary ROM
builds require one complete, dedicated `STB_LOPROC` / `STT_FUNC` definition with
exactly the canonical extent and bytes. This first version rejects every emitted
or configured relocation within that body, and references to its section through
another symbol. Existing `objisolate` guards still apply. After proof, existing
section removal makes the symbol undefined; the existing exact undefined renamer
changes its spelling while preserving symbol indices and relocation addends.

Equal bytes do not establish historical identity. The reason and source review
must explain why this particular canonical function is the original callback.
For Moray, the original factory constructs seven stride-12 segment positions,
and the destructor and typed array identify `Vector3`. The native constructor
callback resolves to the original ARM9 destination `0x0203d384`. Its four-byte
empty body is `STB_LOPROC`, not `STB_WEAK`. No global symbol rename is required.
Policies without `canonical_symbol` retain their previous behavior.

## Producer handoff

Issue: [#3763](https://github.com/tangosdev/sm64ds-decomp/issues/3763).
This commit contains the tooling policy and tests; the source factory is a
separate candidate. Accepted task input was `731efc06d5d5e84ac33ff5f97880e9f90476158e`;
the producer restacked onto `d6f7cf11a9d03bd2c7adda0d137a0de81b35be43` before final
validation. Independent acceptance remains the verifier's responsibility.

Run `python -m pytest tools/test_canonical_duplicates.py tools/test_objisolate.py
 tools/test_rombuild.py tools/test_tu_production.py -q` (one command). With
`CANONICAL_DUPLICATE_FIXTURE` naming the private Moray directory containing
`daMoray_c.o` and `proof.json`, all 128 tests pass. The optional real fixture test
uses normal intact preparation and strict checks for all 21 Moray functions,
without address overrides, and rejects a corrupted constructor body. The
producer recompiled that source with the pinned compiler and current headers.

The broader `tools/test_tubuild.py` suite retains six failures on the exact
composed base: old PoleLift boundary, inventory, verification, promotion and
span expectations, plus RcCarpet's expected RTTI/resource section inversion.
The private base runner loads the immutable base versions of both tooling
modules and routes CLI subprocesses through those same versions; it reproduces
all six failures. These failures are retained, not changed or banked here.

The final frozen `python tools/rombuild.py -j16 --no-rom` exits zero: all 106
modules are exact, all 11,331 source functions reproduce, and all 331 owned data
claims match. The advisory emitted-data inventory retains one differing record;
it is separate from those ownership gates.

Exact validation results, fixture hashes, and private evidence locations are in
[the experiment record](experiments/canonical-duplicates1013.json). The verifier
should reproduce the positive preparation paths, rejection controls and current
ROM control before offering this tooling-only change for publication.
