# Mip Key source forms, PR2853

Producer input `e4d76233ccebc942180bcdeb4a19d34d2d65bdd6`, base
`4407c8471ed3299ea16d9cc899020e3232ac6ee9`; class `daObj_Mip_Key_c`, ov085
`0x0212cc88..0x0212d528` (15 functions, 2208 bytes). These are producer measurements,
not independent source acceptance. No shared ledger was edited.

## Reproduction

Compile each complete TU under `2004/b56` using `match.compile_c(path, '2004/b56',
build_pin.flags_for(path), include_dirs=[scratch_header_directory])`. The production
flags are `-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e
-gccext,on -msgstyle gcc -Cpp_exceptions off`. No extra optimization pragma or
compiler version was used. Isolate every manifest function with `objisolate.derive`,
then call `linkcheck.linkcheck(symbol, address, size, 'ov085', name_index,
obj=isolated, sym=symbol)`. A retained form requires `VERIFIED`, empty `diffs`, and
`blind: 0` for all 15 functions, not just wildcard comparison.

Local scripts, candidate sources, headers, full objects and structured results
are retained under the producer worktree's ignored `build/mip-triage/` directory;
runners are `build/mip-triage-run.py`, `build/mip-triage-combined.py`,
`build/mip-typed-helpers.py`, `build/mip-typed-final.py`, `build/mip-math-probes.py`
and `build/mip-global-probes.py`. The table below gives the actual source forms,
so the results can be reproduced without those temporary files.

## Successful forms

Each listed improvement and the combined source reproduces all 15 relocated functions.

- Replace the padded `MipKeyHost` with `int (daObj_Mip_Key_c::*)()` and a state
  containing `enter` and `execute`. Store `MipKeyState *mState` at 0x188 and
  `Player *mPlayer` at 0x18c. Invoke `(this->*mState->execute)()` directly.
- Replace the fabricated model vtable call with `mModel.Render(0)`; use
  `mAngleY += 0x500`, real Model/ShadowModel initialization, Player conversation
  methods, `ClosestPlayer`, `MarkForDestruction` and `UpdatePosWithOnlySpeed`.
- Use Camera::mFlags, Player::mStateFlags, the actual SharedFilePtr object and
  Message static methods. These compile without the old raw camera offset and
  mangled free-call spellings. Generic global placeholders elsewhere remain an
  explicit declaration-integration boundary, not proof these types are universal.
- Use `int ApproachLinear(int &, int, int)` and actual Vector3 math interfaces.
  Sound::PlaySub follows its matched definition: bool return, integer fifth
  argument. `func_02012790` follows its unsigned return and parameter.
- Carry vector: `Vector3 v; v = *(Vector3 *)s; v.y += 0xc8000;`
  followed by `ApproachLinear(self->mPosY, v.y, 0xa000)` reproduces 0x2b8.
  `Vector3 v = {s[0], s[1], s[2]}` also reproduces 0x2b8. Neither uses volatile.
  Replacing all three chase-local aggregates with Vector3 reproduces 0x144.
- Preserve the unsigned timer load with `(u16)c->mStateTimer != 0`; the plain
  signed member comparison emits `ldrsh` instead of retail `ldrh` at +0x20.
  This is an observed width/signedness constraint, not an extra volatile access.

## Failed bounded alternatives

| Site | Candidate | Result under 2004/b56 |
| --- | --- | --- |
| Carry update 0x0212cd80 | Only remove volatile from old aggregate |0x2a0, retail 0x2b8 |
| Carry update | Only change old volatile declaration to Vector3 |0x2c4 |
| Carry update | Copy initialize `Vector3 v = *(Vector3 *)s` |0x2b0 |
| Matrix helper 0x0212d2b8 | `thiz->mModel.mat4x3 = data_020a0e68` |0xd4, retail 0xbc |
| Matrix helper | `Fix12<int> radius={0x23000}; Fix12<int> height={0x12c000}; thiz->DropShadowRadHeight(thiz->mShadowModel, thiz->mModel.mat4x3, radius, height, 0xf)` |0xcc, retail 0xbc |

Passing integer literals directly to DropShadowRadHeight does not compile with
current Fix12: it has only a `val` field and no converting constructor. That failed
probe is not the evidence for retaining the scalar bridge; the valid aggregate
probe above is. No result establishes an absolute compiler impossibility.

## Provenance and retained work

RTTI at 0x02130194/0x021301a0 and the vtable address point 0x021301d8 identify this
class. Constructor/destructor bodies place Model 0x110 and ShadowModel 0x160 and
prove allocation size 0x1a0. Factory `classInit` is the existing lineage-supported
project alias; its exact original spelling is unknown. The historical filename
pilot is preserved with an immutable link to the actual original source.

The archived call census supports the 15-function run but cannot prove original
file-static linkage. Four address-named helpers occur as pointer-to-member pairs.
`__sinit_ov085_0212f9bc` copies two 8-byte records into each 16-byte state at 0x0213071c
and 0x0213072c. Its 0x0212fa28..0x0212fa3c literal pool is not executable initializer
code. Seven address-named C-linkage helper entry points remain reconstructed
boundaries; none is advertised as a recovered original file-static function.

MIPKEY-2853-04 remains open for genuine helper-method/symbol reconstruction and
MIPKEY-2853-05 for the flat Matrix4x3 copy and scalar DropShadowRadHeight bridge.
The next proposed owner is the coordinator. No follow-up issue has been posted,
no deferral accepted, and no source acceptance is asserted by this producer note.
The four typed global declaration identities require the active ledger owner's
coordination. MIPKEY-2853-01/02/03 are locally repaired; MIPKEY-2853-06's historical
citation is fixed but its declaration/current-base integration requirements remain.

The retained scalar shadow bridge was also checked against its actual definition:
void return, dActor_c/ShadowModel/Matrix4x3 pointers, scalar radius/depth, and u8
opacity. The public mangled method/header carries unsigned opacity; this caller
passes constant 0x0f in either form. Aligning the bridge to the definition and using
a typed RotationX matrix pointer preserves the entire compiled object identity.


## Final bounded reviewer pair

Independent reviewer `/root/review_mip_final` found two simpler forms at input
`1dfbfc4397e7118e2648e8750ce8ccdf091f7605`. The producer adopted both together:
`c->mAngleX += 0x1000` replaces the temporary s16 pointer, and the state setter
checks `c->mState->enter == 0` then calls `(c->*c->mState->enter)()` directly.
A fresh producer compile before and after preserves the complete object SHA256
`0fab978517bf4cdb1fdd83035d0a5fae7fdcdd0f0ef4b7196b8b2e0b54485f8d`.
All 15 functions also freshly verify with exact relocations, empty diffs and blind0.
No compiler metadata changed, so the full-ROM/object/data proof at 564695bfe carries
through this identity; a new full build was unnecessary. The scratch runner and
objects are retained under ignored `build/mip-triage/final-pair/` with proof.json.

Under MIPKEY-2853-04, the manifest now accurately describes the destructor as the
first declared non-inline virtual/key function. Other inherited overrides remain
virtual even where the class does not repeat the keyword. This corrects wording
only; the existing compiler-only vtable policy and addresses are unchanged.
