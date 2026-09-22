// Reverse bridges: MSVC-method references -> C-named definitions.
//
// Slice .cpp TUs compiled against local shadows call these as real methods
// (?Name@Class@@QAE...); the definitions live under Itanium C names in
// their own files. Shadow classes here (NO game headers -- the owner name
// alone fixes the mangling) define each member forwarding to the C name.
// Sound/Scene statics that have no host backend yet are quiet stubs; the
// port grows real ones with the audio/fader gates.

#include <cstdio>

struct Vector3 { int x, y, z; };
struct Actor;
struct OamAttr;
struct Matrix2x2;
struct FaderBrightness;
struct Vector3_16;
struct State;

extern "C" {
void _ZN9dBgCh_GndC1Ev(void *self);
void _ZN9dBgCh_GndD1Ev(void *self);
void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self,
                                                        const void *v,
                                                        void *a);
void _ZN9dBgCh_LinC1Ev(void *self);
void _ZN9dBgCh_LinD1Ev(void *self);
void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void *self,
                                                          const void *a,
                                                          const void *b,
                                                          void *actor);
void _ZN9dBgCh_Lin10GetClsnPosEv(void *res, void *self);
int _ZN6Player11ChangeStateERNS_5StateE(void *self, void *st);
int _ZN6Player7IsStateERNS_5StateE(void *self, void *st);
struct Actor *_ZN8dActor_c10FindWithIDEj(unsigned id);
int _ZNK5dBgPi9GetClsnIDEv(const void *self);
int _ZNK10dBgCh_Actr10IsOnGroundEv(const void *self);
unsigned char _ZN3OAM11GetObjWidthEii(int a, int b);
unsigned char _ZN3OAM12GetObjHeightEii(int a, int b);
int _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(void *attr, int *p,
                                                     void *m);
int _ZN8SaveData19IsCharacterUnlockedEj(unsigned ch);
int _ZN4cstd4fdivEii(int a, int b);
/* gate 18 */
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
short _ZN8dActor_c18HorzAngleToCPlayerEv(void *self);
int _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *m, int rad, int h, unsigned f);
}

struct RaycastGround {
    RaycastGround();
    ~RaycastGround();
    void SetObjAndPos(const Vector3 &v, Actor *a);
};
RaycastGround::RaycastGround() { _ZN9dBgCh_GndC1Ev(this); }
RaycastGround::~RaycastGround() { _ZN9dBgCh_GndD1Ev(this); }
void RaycastGround::SetObjAndPos(const Vector3 &v, Actor *a)
{ _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(this, &v, a); }

struct RaycastLine {
    RaycastLine();
    ~RaycastLine();
    int DetectClsn();
    void SetObjAndLine(const Vector3 &a, const Vector3 &b, Actor *actor);
    Vector3 GetClsnPos();
};
RaycastLine::RaycastLine() { _ZN9dBgCh_LinC1Ev(this); }
RaycastLine::~RaycastLine() { _ZN9dBgCh_LinD1Ev(this); }
void RaycastLine::SetObjAndLine(const Vector3 &a, const Vector3 &b,
                                Actor *actor)
{ _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(this, &a, &b, actor); }
Vector3 RaycastLine::GetClsnPos()
{
    Vector3 tmp;
    _ZN9dBgCh_Lin10GetClsnPosEv(&tmp, this);
    return tmp;
}

struct Player {
    struct State;
    int ChangeState(::State &s);
    void ChangeState(State &s);
    int IsState(::State &s);
    int IsState(State &s);
    void SetAnim(unsigned a, int b, int f, unsigned d);
    void Hurt(const Vector3 &, unsigned, int, unsigned, unsigned, unsigned);
};
int Player::ChangeState(::State &s)
{ return _ZN6Player11ChangeStateERNS_5StateE(this, &s); }
void Player::ChangeState(State &s)
{ _ZN6Player11ChangeStateERNS_5StateE(this, &s); }
/* RETIRED at FACES4 (wave 9c) under the SHADOW RULE. THE GLOBAL-::State
   spelling only. src/actors/Player.cpp, the TU config says owns the ROM
   address, emits ?IsState@Player@@QAE_NAAUState@1@@Z -- returning bool, with
   the NESTED Player::State -- and this file spelled the same one function two
   more ways, returning int, so facegen saw three definitions of Player::IsState
   and refused to bind the flat name at all. Measured before removal: ZERO
   objects in walk_window's 8662 link inputs reference
   ?IsState@Player@@QAEHAAUState@@@Z, so it goes. The nested spelling below
   STAYS: src/func_ov002_020e2ea0.cpp really does ask for it, and a host bridge
   for a caller that spells a member its own way is legitimate -- what was never
   legitimate was letting it stand in for the definition.
int Player::IsState(::State &s)
{ return _ZN6Player7IsStateERNS_5StateE(this, &s); }                         */
int Player::IsState(State &s)
{ return _ZN6Player7IsStateERNS_5StateE(this, &s); }
extern "C" int _ZN6Player7SetAnimEji5Fix12IiEj(void *, unsigned, int, int,
                                               unsigned);
void Player::SetAnim(unsigned a, int b, int f, unsigned d)
{ _ZN6Player7SetAnimEji5Fix12IiEj(this, a, b, f, d); }

/* Player::Hurt: the receiver face for the intro knockback command
   (src/func_ov002_020bd250.cpp reaches it through a local shadow class, so
   MSVC mangles ?Hurt@Player@@QAEXABUVector3@@IHIII@Z, a thiscall member with
   `this` in ecx). The one real definition carries the Itanium C name with C
   linkage, self as its first stack argument. */
extern "C" int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self,
                                                       const void *src,
                                                       unsigned a, int b,
                                                       unsigned c, unsigned d,
                                                       unsigned e);
void Player::Hurt(const Vector3 &v, unsigned a, int b, unsigned c, unsigned d,
                  unsigned e)
{ _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(this, &v, a, b, c, d, e); }

/* ClsnResult::GetClsnID is the MATCHED body now (run linkw wave 3, w3-a).
   src/_ZNK5dBgPi9GetClsnIDEv.cpp declares it `u32`, and MSVC puts the
   return type in the decoration, so the matched symbol is
   ?GetClsnID@dBgPi@@QBEIXZ. The shadow here used to say `int`, which
   decorates ...QBEHXZ -- a different symbol entirely, which is exactly why
   w2-a found "the two never meet and no duplicate is raised". Spelling the
   shadow `unsigned` makes this declaration name the matched body, and the
   definition that used to sit on the next line is deleted so the matched TU
   supplies it. The TU was already compiled (slice_gate10.txt) and only ever
   stripped for want of a reference, so no slice line is needed. */
struct ClsnResult { unsigned GetClsnID() const; };

/* The `int`-returning spelling kept alive for whoever still emits it. Same
   __thiscall, no arguments, result in EAX either way, so this is a pure
   return-type face -- the construction hal/actor_faces_bob.cpp uses for
   ShadowModel::InitCylinder. It is an /alternatename rather than a second
   definition so it costs nothing when nothing references it. */
struct ClsnResultIntFace { int GetClsnID() const; };
int ClsnResultIntFace::GetClsnID() const
{ return (int)((const ClsnResult *)this)->ClsnResult::GetClsnID(); }
#pragma comment(linker, "/alternatename:?GetClsnID@dBgPi@@QBEHXZ=?GetClsnID@ClsnResultIntFace@@QBEHXZ")

struct WithMeshClsn {
    int IsOnGround() const;
    int GetWallResult() const;
    int GetFloorResult() const;
};
int WithMeshClsn::IsOnGround() const
{ return _ZNK10dBgCh_Actr10IsOnGroundEv(this); }

struct dCc_c;
struct ShadowModel;
struct Matrix4x3;
/* FindWithID is NOT here any more. src/_ZN8dActor_c10FindWithIDEj.cpp used to be
   a C free function that this file wrapped into a method; main rewrote it as
   the real static method against include/Actor.h, so defining it here is a
   second definition. The traffic now runs the other way and the C face its
   callers spell lives in hal/method_faces.cpp. */
struct Actor {
    void UpdatePosWithOnlySpeed(dCc_c *c);
    /* gate 18: ov085's TUs declare a local Actor shadow and call these as
       methods; all three are C-form definitions in src. */
    Matrix4x3 *UpdateCarry(Player &player, const Vector3 &vec);
    Player *ClosestPlayer();
    short HorzAngleToCPlayer();
    void DropShadowRadHeight(ShadowModel &sm, Matrix4x3 &m, int rad, int h,
                             unsigned f);
};
/* THE OTHER DIRECTION, in the same place for the same reason.
   src/_ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3.cpp defines UpdateCarry as a
   method of its OWN local `class Actor` -- include/Actor.h does not declare
   it -- so the C name its ov085 caller uses has to be defined against a
   shadow too, and this is the only file that has one. Not an alias: the C
   form is __cdecl with `this` on the stack, the method is __thiscall with it
   in ecx. */
extern "C" Matrix4x3 *_ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(
    void *self, void *player, const void *vec)
{ return ((Actor *)self)->UpdateCarry(*(Player *)player,
                                      *(const Vector3 *)vec); }
Player *Actor::ClosestPlayer()
{ return (Player *)_ZN8dActor_c13ClosestPlayerEv(this); }
short Actor::HorzAngleToCPlayer()
{ return _ZN8dActor_c18HorzAngleToCPlayerEv(this); }
void Actor::DropShadowRadHeight(ShadowModel &sm, Matrix4x3 &m, int rad, int h,
                                unsigned f)
{ _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
      this, &sm, &m, rad, h, f); }

template <typename T> struct Fix12 { T val; };
struct OAM {
    static unsigned char GetObjWidth(int a, int b);
    static unsigned char GetObjHeight(int a, int b);
    static int LoadAffineParams(OamAttr *attr, int *p, Matrix2x2 *m);
    static void Render(bool sub, OamAttr *attr, int x, int y, int pal,
                       int pri, Fix12<int> sx, Fix12<int> sy, int rot,
                       int mode);
};
/* RETIRED at ALIAS2 (wave 8, the main -> port sync): main's tree defines all
   four of these as real static members now, so src/ emits
   ?GetObjWidth@OAM@@SAEHH@Z, ?GetObjHeight@OAM@@SAEHH@Z,
   ?LoadAffineParams@OAM@@SAHPAUOamAttr@@PAHPAUMatrix2x2@@@Z and
   ?IsCharacterUnlocked@SaveData@@SAHI@Z itself, and these bridges were the
   second definition (LNK2005). Each is a static, so there is no receiver to
   bridge and the flat ROM name aliases straight onto the src symbol; lane
   ALIAS2's batch-2 rows in hal/cxx_aliases.cpp carry that direction.
unsigned char OAM::GetObjWidth(int a, int b)
{ return _ZN3OAM11GetObjWidthEii(a, b); }
unsigned char OAM::GetObjHeight(int a, int b)
{ return _ZN3OAM12GetObjHeightEii(a, b); }
int OAM::LoadAffineParams(OamAttr *attr, int *p, Matrix2x2 *m)
{ return _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(attr, p, m); }

struct SaveData { static int IsCharacterUnlocked(unsigned ch); };
int SaveData::IsCharacterUnlocked(unsigned ch)
{ return _ZN8SaveData19IsCharacterUnlockedEj(ch); }                          */

namespace cstd { int fdiv(int a, int b); }
int cstd::fdiv(int a, int b) { return _ZN4cstd4fdivEii(a, b); }

/* ---- sound statics ------------------------------------------------------
   These were stubbed while the SDAT root was null. The host now seats a real
   root and hosts the ARM7 (hal/sdat/), so they forward to the matched src
   the way every other reverse bridge here does: src/ defines the Itanium
   name as an extern "C" symbol, and this is the MSVC-mangled face the C++
   TUs call. StopLoadedMusic_Layer1 is the exception -- its src file spells
   the C++ method out directly, so it just joins the slice and needs no
   face here. */
extern "C" {
int  _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned, unsigned, unsigned, int, int);
void _ZN5Sound6Play2DEjj(unsigned, unsigned);
void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int);
}
struct Sound {
    static int PlaySub(unsigned, unsigned, unsigned, int, bool);
    static void Play2D(unsigned, unsigned);
    static void LoadAndSetMusic_Layer1(int);
};
/* Seat the SDAT root before any of these walk it. A harness without a frame
   loop never calls sdat_host_tick, and the table walkers read
   data_020a5bb8 + 0x84 unconditionally. Idempotent. */
void sd_consumer_init(void);
int Sound::PlaySub(unsigned a, unsigned b, unsigned c, int d, bool e)
{ sd_consumer_init(); return _ZN5Sound7PlaySubEjjj5Fix12IiEb(a, b, c, d, e ? 1 : 0); }
void Sound::Play2D(unsigned a, unsigned b)
{ sd_consumer_init(); _ZN5Sound6Play2DEjj(a, b); }
void Sound::LoadAndSetMusic_Layer1(int a)
{ sd_consumer_init(); _ZN5Sound22LoadAndSetMusic_Layer1Ei(a); }

struct Scene {
    static void SetAndStopColorFader();
    static void SetFaders(FaderBrightness *);
    static void StartSceneFade(unsigned, unsigned, unsigned short);
};
void Scene::SetAndStopColorFader() {}
void Scene::SetFaders(FaderBrightness *) {}
void Scene::StartSceneFade(unsigned, unsigned, unsigned short) {}

/* GATE 29 REMOVED FOUR NO-OPS FROM HERE.
   Particle::System::New, ::NewSimple, ::NewBigSplash and
   Particle::RunningSlidingDustAt were empty bodies, because the real src
   walks a manager the host never seated and St_Land's dust NewSimple
   faulted on the null system. All four are now the ROM's own matched
   bodies, carried by slice_gate29.txt over a real SysTracker; the
   MSVC-mangled C++ faces are aliased onto them in hal/cxx_aliases.cpp. */

/* FACEFIX 2026-09-14: THIS FACE USED TO CALL ANOTHER FACE. It spelled the
   receiver as this file's own RaycastLine shadow, whose DetectClsn is declared
   at line 64 and defined nowhere here, so hal/faces4_rows.cpp:131 supplied it --
   and that definition forwards straight back to this flat name. Face calls face,
   no exit. The REAL matched body is in the link the whole time, decorated
   ?DetectClsn@dBgCh_Lin@@QAE_NXZ at 004cd200 out of
   src/_ZN9dBgCh_Lin10DetectClsnEv.cpp.obj: class dBgCh_Lin, __thiscall, BOOL
   return, no parameters. Spelling the shadow with the ROM's own class name and
   the ROM's own return type lands on it. faces4_rows' row is left alone on
   purpose: with this face reaching real code, RaycastLine::DetectClsn becomes an
   ordinary terminating two-hop forwarder for whatever still spells it that way. */
struct dBgCh_Lin { bool DetectClsn(); };
extern "C" int _ZN9dBgCh_Lin10DetectClsnEv(void *self)
{ return ((dBgCh_Lin *)self)->dBgCh_Lin::DetectClsn() ? 1 : 0; }
extern "C" int _ZNK10dBgCh_Actr13GetWallResultEv(const void *self)
{ return ((const WithMeshClsn *)self)->GetWallResult(); }
extern "C" int _ZNK10dBgCh_Actr14GetFloorResultEv(const void *self)
{ return ((const WithMeshClsn *)self)->GetFloorResult(); }

/* gate 172: func_ov098_021390ec (Crate's state helper) declares a local
   `struct SurfaceInfo { void CopyNormalTo(Vector3 &) const; }` and calls it as
   a real method; the matched body is the C-form src
   (_ZNK11SurfaceInfo12CopyNormalToER7Vector3.c, slice_gate8.txt), so the
   method bridges to it -- an alias cannot cross the __thiscall/cdecl seam. */
/* RETIRED at ALIAS2 (wave 8, the main -> port sync): main's tree defines
   SurfaceInfo::CopyNormalTo as a real member now, so src/ emits
   ?CopyNormalTo@SurfaceInfo@@QBEXAAUVector3@@@Z itself and this bridge was the
   second definition (LNK2005).
struct SurfaceInfo { void CopyNormalTo(Vector3 &) const; };
extern "C" void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(const void *self,
                                                          Vector3 *v);
void SurfaceInfo::CopyNormalTo(Vector3 &v) const
{ _ZNK11SurfaceInfo12CopyNormalToER7Vector3(this, &v); }                     */

struct SphereClsn { int DetectClsn(); };
extern "C" int _ZN12dBgCh_SphCrr10DetectClsnEv(void *self)
{ return ((SphereClsn *)self)->DetectClsn(); }

struct Message { void Update(); static void AddChar(char c); };
extern "C" void _ZN7Message6UpdateEv(void *self)
{ ((Message *)self)->Update(); }
extern "C" void _ZN7Message7AddCharEc(char ch)
{ Message::AddChar(ch); }


/* THE SPRITE LIST HAS TO BE TERMINATED, and OAM::Render trusts that it is.
   Its walk advances OamAttr by OamAttr (8 bytes: u32 a01, u16 a2, u16 a3) and
   the ONLY thing that stops it is a3 == 0xffff. Hand it a table that has no
   terminator and it walks straight out of main RAM: on host that faults at
   exactly 0x02400000, the first address past the 4MB mapping, which is the
   crash that has been showing up as "rare, in OAM::Render, once the HUD and
   minimap draw". On hardware main RAM mirrors every 4MB, so the same runaway
   walk keeps reading and eventually finds a 0xffff by luck, which is how the
   ROM gets away with it and why this only bites the port.

   Checking for the terminator before the call turns that crash into one
   skipped sprite and, more usefully, prints the address of the table that is
   missing it -- which is the actual bug, some sprite table not being seated.
   The bound is generous: the biggest real list here is well under 256. */
/* RETIRED at the main -> port sync (lane SYNC7, wave 9, the fold merge). main's
   #2712 rewrote OAM::Render as matching C in
   src/_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii.c, so src now DEFINES this flat
   name itself and this forwarder is a second definition (LNK2005).

   THE 0xffff TERMINATOR GUARD GOES WITH IT, and that is a real loss, not a
   tidy-up: a sprite table seated without a terminator used to cost one skipped
   sprite and a printed address, and now costs an access violation at 0x02400000
   the way it did before the guard existed. It is recorded in out/SYNC7/bugs.md.
   Putting it back means a hostgen substitution on the src TU, which is lane
   HOSTGEN4's mechanism and not a second definition of the ROM's own name.
extern "C" void *_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int sub, void *attr, int x, int y, int pal, int pri,
    int sx, int sy, int rot, int mode)
{
    enum { OAM_MAX = 256 };
    const unsigned short *a3 = (const unsigned short *)attr + 3;
    int n = 0;
    if (!attr) return 0;
    for (; n < OAM_MAX; ++n, a3 += 4)
        if (*a3 == 0xffffu) break;
    if (n >= OAM_MAX) {
        static const void *said;
        if (said != attr) {
            said = attr;
            std::fprintf(stderr, "[oam] attr list at %p has no 0xffff "
                         "terminator in %d entries, sprite skipped\n",
                         attr, OAM_MAX);
        }
        return 0;
    }
    Fix12<int> fsx, fsy;
    fsx.val = sx; fsy.val = sy;
    OAM::Render(sub != 0, (OamAttr *)attr, x, y, pal, pri, fsx, fsy, rot,
                mode);
    return 0;
}
*/

/* FACEFIX 2026-09-14: THIS FACE USED TO CALL ANOTHER FACE, same shape as the
   dBgCh_Lin row above and with a much wider blast radius. It spelled the
   receiver as this file's `struct Actor` shadow, so it bound
   ?UpdatePosWithOnlySpeed@Actor@@QAEXPAUdCc_c@@@Z, which is defined by
   port/unmatched/KnockDownPlank_Behavior.cpp:94 (reached through the
   /alternatename in hal/cxx_aliases.cpp:3006) and forwards straight back here.
   Every flat caller of this name was therefore in an unbounded loop, and there
   are many: HootTheOwl and Klepto's state dispatch, Bullet, KingBobOmb.
   The REAL matched body is linked as ?UpdatePosWithOnlySpeed@dActor_c@@-
   QAEXPAUdCc_c@@@Z at 004cd5d0 out of
   src/_ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c.cpp.obj. Spelling the shadow
   with the ROM's own class name lands on it; KnockDownPlank's face stays and
   becomes an ordinary terminating two-hop forwarder for func_ov015_02111d98's
   own `struct Actor` spelling. */
struct dActor_c { void UpdatePosWithOnlySpeed(dCc_c *c); };
extern "C" void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *self,
                                                                  void *cl)
{ ((dActor_c *)self)->dActor_c::UpdatePosWithOnlySpeed((dCc_c *)cl); }
struct Camera { void SetFlag_3(); };
extern "C" void _ZN6Camera9SetFlag_3Ev(void *self)
{ ((Camera *)self)->SetFlag_3(); }

/* THE HOST BODY IS GONE (run linkw wave 3, w3-a). It read the objID at +0x1c
   itself; the matched src/_ZNK5dBgPi9GetClsnIDEv.cpp reads
   ((const u32 *)this)[7], the same word, and now owns the work. This is the
   reference edge that stops /OPT:REF discarding it: 22 of its 27 ROM callers
   are in the image and every one of them calls this C name.

   THE JMP-CYCLE HAZARD THIS COMMENT USED TO WARN ABOUT DOES NOT APPLY, and
   the reason is worth keeping. The old cycle was method -> C name -> method
   with BOTH ends being ...QBEHXZ, so /O2 folded it into a two-instruction jmp
   loop that hung the first real-collision frame. The method this forwards to
   now is ...QBEIXZ, the matched body, which reads the field and returns. One
   hop, and the far end does real work rather than hopping back. */
extern "C" int _ZNK5dBgPi9GetClsnIDEv(const void *self)
{ return (int)((const ClsnResult *)self)->ClsnResult::GetClsnID(); }
