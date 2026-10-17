# Arrow Sign native static initialization

Producer: `codex-native-inits-three1015`; task `native-inits-three1015`.
Tested predecessor: Snowman Breath `388ef26b456bb3d2540cb37d77b2ae8072d34811`
(PR #3887), on main `e209dacfaaa95cb5490637ff053d62182689bebe`.

The production `src/game/actors/d_a_obj_yajirusi.cpp` defines four ordinary
C++ file-scope resource handles, in model-L/model-R/collision-L/collision-R
construction order. The compiler emits the original 188-byte initializer
and constructor-table word; the handwritten C shard is retired.
The local YajirusiModelFile/YajirusiCollisionFile type names are inferred ABI
views of the existing eight-byte resource families. Their ordinary C++
constructor/destructor imports resolve through exact manifest aliases to the
original four arm9 functions; no helper implementation or shared header changes.

Owned ov098 ranges (half-open): .text 0x02137be0..0x02138040;
.init 0x0213bf9c..0x0213c058; .ctor 0x0213c33c..0x0213c340;
.bss 0x0213c800..0x0213c850 (four eight-byte handles and four 12-byte
registration nodes). Compiler-local labels @676/@677/@678/@679 map to
fleet-unique @931/@932/@933/@937 at the original registration addresses.
The existing exact nontext-section ordering policy puts each complete emitted
BSS object at its ROM address; no storage, object bytes or relocation records
are patched.

Early proof: pinned 2004/b56; all 12 original function bodies unchanged;
the three nontext ranges, object sizes/placement and all 19 relocations verify.
The three file-table column symbols, class methods, RTTI/vtable ownership,
Fix12 ABI bridges and remaining layout/interface work stay under their existing
partial-reconstruction policies. Attribution remains at the original addresses.
Final proof: SCRATCH-DATA-VERIFIED, all four owned ranges and all 19 nontext
relocations verified, all 106 modules exact, and the full 16 MiB ROM identical
to a fresh strict stock control (SHA-256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`).
The raw symbol audit retains nine pre-existing errors, zero new. Default
production passes: 11,334/11,334 source functions, 334/334 owned data claims,
241 BSS claims/30,268 NOBITS bytes, all 106 modules exact. Raw per-function
check: 12 VERIFIED and initializer BLIND-10 on compiler C++ imports, zero
blocking; the exact owned-section/full-ROM alias proof resolves those ten sites.
Source style, declaration agreement, conversion ratchet, rename ledger, dead
references and port references pass. Focused metadata: 6 VERIFIED, 9 PARTIAL,
zero DIFFERS. The global inventory retains one unrelated existing difference.
Eight inherited RTTI records retain their exact externalization checks; only
the duplicate generic policy rows were removed.
Reports: build/yajirusi-final-fullrom.txt, build/yajirusi-production.json,
build/yajirusi-production.txt, build/yajirusi-prepush.txt and
build/yajirusi-exact-rtti.json (all ignored). Independent acceptance and
private CI validation remain pending.

The PC actor slice imports the whole source. Its ROM-backed smoke currently
fails in unchanged port/hal/actor_vtables.cpp on the pre-existing missing
ArrowSignRight.h include (also missing at the pinned main). Keep the new NDS
resource-family globals behind SM64DS_PLATFORM_PC so the host retains its
existing resource setup and does not gain unresolved NDS-only constructors.
The complete raw pinned NDS object is byte-identical before/after this guard.
This is a host seam, not a compiler matching workaround; the ROM initializer
continues to be emitted by ordinary C++ object construction.

Host controls: compiling this TU alone produces the same four diagnostics as
the untouched main source with the same headers (missing inherited update
helpers and a pre-existing non-void return issue). PC preprocessing confirms
that all four NDS-only global constructions are excluded. Neither the wider
ROM-backed actor smoke nor the standalone host compile is reported as green.
The raw NDS object before/after the guard has SHA-256
`9eafd694b11becf2646cddb35a15902030f45f6e7cbadc3d946a86b8b2157047`.
