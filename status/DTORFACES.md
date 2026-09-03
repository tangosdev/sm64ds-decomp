# DTOR-FACES-CPP: the real-C++ destructor TUs of seated classes, linked from src

Lane 10 of `C:/tmp/link-scope/status/LINKSCOPE.md`. Branch `port/dtor-faces-cpp`
off 5600924cb. Nothing under src/ or include/ is touched; every change is in
port/ and this file.

## The wall, and the mechanism

Sixty-odd seated classes have a D1 in src/ that is the shadow-class C++ form
(`struct Actor { char pad[..]; virtual ~Actor(); }; struct Coin : Actor
{ CommonModel m0; ...; virtual ~Coin(); }; Coin::~Coin() {}`). mwcc turns that
into the byte-matched ROM body; MSVC turns it into `??1Coin@@UAE@XZ`, whose
synthesised member chain calls `??1CommonModel@@QAE@XZ`, `??1ShadowModel@@QAE@XZ`,
`??1MovingCylinderClsn@@QAE@XZ`, `??1WithMeshClsn@@QAE@XZ` and `??1Actor@@UAE@XZ`,
decorated __thiscall names nothing in the port defined (the bodies exist under
their ROM C names). Every fill so far answered with a transcription of the
ROM chain in a static thunk and left the matched TU out of the link.

The mechanism (`port/hal/dtor_faces_cpp.cpp`, `.h`, `port/slice_dtorfaces.txt`):

1. FACES. One real __thiscall definition per decorated shadow-class
   destructor, declared exactly as the src TUs declare it, forwarding `this`
   to the C body. Which body is read off the ROM relocations of all 67
   class-form D1 TUs: every base call is 0x020112c8 `_ZN5ActorD2Ev` (so the
   virtual-mangled `??1Actor@@UAE@XZ` forwards to D2, beside the seam's
   non-virtual `??1Actor@@QAE@XZ`); every member call is a plain `bl` to that
   member class's D1. Seven new faces here (Actor U; MovingCylinderClsn,
   MovingCylinderClsnWithPos, ShadowModel, WithMeshClsn, CommonModel,
   ExtendingMeshCollider Q); the other four the TUs ask for (Model, ModelAnim,
   TextureSequence, TextureTransformer) already exist in
   `unmatched/Mg3DEsp_Faces.cpp` and are linked in the same four targets.
   The set was measured, not predicted: all 64 candidate TUs compiled under
   the port's MSVC flags and their UNDEF `??1` symbols collected; the eleven
   names above are the whole list.
2. ADAPTERS. `hal_cppd1_<Class>`, the vtable-law __fastcall thunk whose body
   is the qualified call `((Class *)s)->Class::~Class()`, a direct call into
   the matched body. The fill's slot-16 line points at it; the transcribed
   static thunk is deleted.

Run-time difference, stated: the MSVC body stores its own `??_7Class@@6B@`
into the vptr where the ROM body stored the class table; nothing dispatches
in that window and Actor's D2 stores `_ZTV5Actor` as its first statement.
The member chain is the same calls at the same offsets.

## Scope check against the live tree

Per the coordinator's rule, every class was checked on THIS tree's own
walk_window.map (5600924cb, built in the worktree) before seating: none of
the 67 `??1<Class>@@UAE@XZ` symbols is in the map, and every slot-16 write
for the seated classes below was read off the fill source (a transcribed
thunk, a shared Actor-only thunk, or a host copy).

## Seated

Group 1, the proof: Coin (`hal/actor_classes_bob_world.cpp`, ROM 0x020b0f54,
ov002). Chain WithMeshClsn 0x1ac, MovingCylinderClsn 0x178, ShadowModel 0x150,
CommonModel 0x114 and 0xd8, Actor D2; MSVC's ??1Coin@@UAE@XZ disassembled
from the built object emits exactly those offsets. Linkage 9403 -> 9404.
Teardown selftest (walk_window, SM64DS_LEVEL=1 SM64DS_FAULTS_FATAL=1
SM64DS_WINDOW_SELFTEST=600 SM64DS_WARP_SEQ=6@100,7@200,8@300,15@400,1@500):
rc 0, 51 / 185 / 174 / 208 actors torn down on the four warps, no fault,
no declined teardown. vtspan --seats over dumpbin /disasm:nobytes of the
946 hal objects: 1689 checked, no wrong-body seats; --sweep rc 0.

## Skipped, with the wall

(filled below)

## Proofs

(filled below)
