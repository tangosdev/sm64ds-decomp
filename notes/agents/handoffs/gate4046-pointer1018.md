# PR 4046 particle pointer correction

Task `gate4046-pointer1018` continues original candidate
`439c8aaf362765f94f963010b4bbf54601286ed3` on actual main
`39e85ae247da338885b6063c69bf7fef40e99454`. Producer session:
`codex-gate4046-pointer1018`; workflow:
`18af52b49b9fb5663d417bdfa86109b7b157aaa3`. Independent source review and
private validation must bind the final candidate and target before merge.

## Source correction

The original C creation helper stored pointers. PR 4046 replaced those slots with
integers to agree with other declarations, introducing a pointer-to-integer and
integer-to-pointer round trip. Declaration agreement did not justify that source
regression. All four declarations now name `Particle::System *` slots. Creation
still calls the real `Particle::Manager::AddSystem` method and accesses the real
`Particle::System::callbackFlags` field at offset 0x1c; assignment and retrieval
now remain pointer-valued throughout.

The getter returns `void *` with C linkage. Its three callers declare that same
opaque pointer return, preserving the C/C++ boundary. The event helper retains
the returned value in a pointer variable and passes it directly onward; the other
two callers retain their null checks and existing pointer conversion. The slot
initializer stores a null pointer with the existing loop shape. The manager's
opaque global interface and unrelated cleanup/allocation APIs remain unchanged.

This is seven source files: 020b7690, 020b76c4, 020b7708, 020b782c,
020be0dc, 020be3bc and 020beb80. No shared header, declaration baseline, symbol
address or enrollment change is needed for the correction. Original PR 4046's
four other helpers and all three enrollment/inventory blobs are identical to
439c8aaf. The user-owned 020c2f14 restoration source is untouched.

## Findings retained and corrected

- `OVTYPES-PARTICLE-DECL` (correctness, fixed): the prior finding remains relevant,
  but its old integer-slot rationale is superseded. All four slot declarations
  now agree on `Particle::System *[]`, and the getter plus all three consumers
  agree on `void *(int)`. No baseline modification hides a discrepancy.
- `OVTYPES-PARTICLE-POINTER-REGRESSION` (correctness, fixed): the creation helper's
  new integer slot interface lost the original pointer semantics. The coherent
  seven-file correction restores them while retaining actual Particle methods
  and member access. Byte proof alone was insufficient to catch this defect.
- Existing address names, opaque manager/allocator interfaces and other caller
  layouts remain partial reconstruction. This change does not claim original
  names, a complete class reconstruction or a promoted translation unit.

## Local verification

Compiler: pinned 2004/b56. Evidence is in the isolated producer worktree
`C:/tmp/sm64ds-gate4046-pointer1018/build/`.

- `pointer-linkcheck.json`: seven changed functions VERIFIED, zero differences,
  blind relocations, warnings or blockers. After the pinned formatter adjusted
  AddSystem statement wrapping, `pointer-formatted-linkcheck.json` independently
  rechecked that function: VERIFIED, zero blind relocations.
- `pointer-declarations.log`: full declaration ratchet against the main anchor,
  zero new disagreements and zero new local header redeclarations. There are
  1,471 existing scoped disagreements; these are not described as clean.
- `pointer-style.json`: all 11 changed source files, zero findings or deferrals.
- `pointer-port.log`: all 247 references resolve.
- `pointer-deadrefs.log`: no new dead references or broken Markdown links;
  existing baselined unresolved references remain distinct.
- `pointer-langmode.log`: language-mode ratchet PASS against the fetched local
  chaos-data baseline; no production baseline edit.

- `python tools/rombuild.py -j16 --report-json build/pointer-rombuild.json`:
  exit 0, built the 16 MiB ROM; 106/106 modules exact, 11,334/11,334 source
  functions reproduce, 348/348 owned data claims exact, and 246 owned BSS claims
  pass their object/symbol gates. The production build compiled all seven new
  source inputs and reused 2,460 unchanged content-cache objects.
- The stock-control pipeline still reports nine existing missing-symbol lines;
  final production comparison reports zero new symbol errors. The aggregate
  ROM-data scan retains one existing difference and its partial/unowned rows;
  these diagnostics are not mislabeled as clean owned-data validation.

Next: independent source review of the exact local commit, then update the
original PR branch and obtain private validation for its current target.
