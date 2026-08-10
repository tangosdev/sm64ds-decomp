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
void _ZN13RaycastGroundC1Ev(void *self);
void _ZN13RaycastGroundD1Ev(void *self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self,
                                                        const void *v,
                                                        void *a);
void _ZN11RaycastLineC1Ev(void *self);
void _ZN11RaycastLineD1Ev(void *self);
void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self,
                                                          const void *a,
                                                          const void *b,
                                                          void *actor);
void _ZN11RaycastLine10GetClsnPosEv(void *res, void *self);
int _ZN6Player11ChangeStateERNS_5StateE(void *self, void *st);
int _ZN6Player7IsStateERNS_5StateE(void *self, void *st);
struct Actor *_ZN5Actor10FindWithIDEj(unsigned id);
int _ZNK10ClsnResult9GetClsnIDEv(const void *self);
int _ZNK12WithMeshClsn10IsOnGroundEv(const void *self);
unsigned char _ZN3OAM11GetObjWidthEii(int a, int b);
unsigned char _ZN3OAM12GetObjHeightEii(int a, int b);
int _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(void *attr, int *p,
                                                     void *m);
int _ZN8SaveData19IsCharacterUnlockedEj(unsigned ch);
int _ZN4cstd4fdivEii(int a, int b);
/* gate 18 */
void *_ZN5Actor13ClosestPlayerEv(void *self);
short _ZN5Actor18HorzAngleToCPlayerEv(void *self);
int _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *m, int rad, int h, unsigned f);
}

struct RaycastGround {
    RaycastGround();
    ~RaycastGround();
    void SetObjAndPos(const Vector3 &v, Actor *a);
};
#ifdef _WIN32 /* LINUX: each method-shadow's mangling IS the extern-C name it forwards to -> self-recurse on GCC. On Linux the callers bind to the real src/ bodies. */
RaycastGround::RaycastGround() { _ZN13RaycastGroundC1Ev(this); }
RaycastGround::~RaycastGround() { _ZN13RaycastGroundD1Ev(this); }
void RaycastGround::SetObjAndPos(const Vector3 &v, Actor *a)
{ _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(this, &v, a); }
#endif /* _WIN32 */

struct RaycastLine {
    RaycastLine();
    ~RaycastLine();
    int DetectClsn();
    void SetObjAndLine(const Vector3 &a, const Vector3 &b, Actor *actor);
    Vector3 GetClsnPos();
};
#ifdef _WIN32 /* LINUX: each method-shadow's mangling IS the extern-C name it forwards to -> self-recurse on GCC. On Linux the callers bind to the real src/ bodies. */
RaycastLine::RaycastLine() { _ZN11RaycastLineC1Ev(this); }
RaycastLine::~RaycastLine() { _ZN11RaycastLineD1Ev(this); }
void RaycastLine::SetObjAndLine(const Vector3 &a, const Vector3 &b,
                                Actor *actor)
{ _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(this, &a, &b, actor); }
Vector3 RaycastLine::GetClsnPos()
{
    Vector3 tmp;
    _ZN11RaycastLine10GetClsnPosEv(&tmp, this);
    return tmp;
}
#endif /* _WIN32 */

struct Player {
    struct State;
    int ChangeState(::State &s);
    void ChangeState(State &s);
    int IsState(::State &s);
    int IsState(State &s);
    void SetAnim(unsigned a, int b, int f, unsigned d);
};
int Player::ChangeState(::State &s)
{ return _ZN6Player11ChangeStateERNS_5StateE(this, &s); }
int Player::IsState(::State &s)
{ return _ZN6Player7IsStateERNS_5StateE(this, &s); }
#ifdef _WIN32 /* LINUX: the nested-State overloads mangle to exactly the extern-C
   names they forward to (_ZN6Player11ChangeStateERNS_5StateE /
   _ZN6Player7IsStateERNS_5StateE) -> self-recurse on GCC. The real src TUs
   (src/_ZN6Player11ChangeStateERNS_5StateE.cpp, src/_ZN6Player7IsStateERNS_5StateE.c)
   own those symbols; every nested-State caller binds straight to them. The
   ::State (global) overloads above mangle differently and stay real bridges. */
void Player::ChangeState(State &s)
{ _ZN6Player11ChangeStateERNS_5StateE(this, &s); }
int Player::IsState(State &s)
{ return _ZN6Player7IsStateERNS_5StateE(this, &s); }
#endif /* _WIN32 */
extern "C" int _ZN6Player7SetAnimEji5Fix12IiEj(void *, unsigned, int, int,
                                               unsigned);
void Player::SetAnim(unsigned a, int b, int f, unsigned d)
{ _ZN6Player7SetAnimEji5Fix12IiEj(this, a, b, f, d); }

struct ClsnResult { int GetClsnID() const; };
#ifdef _WIN32 /* LINUX: ClsnResult::GetClsnID() const mangles to
   _ZNK10ClsnResult9GetClsnIDEv, which the extern-C REAL BODY below also
   defines -> "symbol already defined" on GCC. On MSVC the two manglings
   differ so the method shadow forwards to the C body; on Linux they are ONE
   symbol, so the real body IS ClsnResult::GetClsnID and this shadow is a
   redundant duplicate. Guard it out; C++ callers bind to the real body. */
int ClsnResult::GetClsnID() const { return _ZNK10ClsnResult9GetClsnIDEv(this); }
#endif /* _WIN32 */

struct WithMeshClsn {
    int IsOnGround() const;
    int GetWallResult() const;
    int GetFloorResult() const;
};
int WithMeshClsn::IsOnGround() const
{ return _ZNK12WithMeshClsn10IsOnGroundEv(this); }

struct CylinderClsn;
struct ShadowModel;
struct Matrix4x3;
/* FindWithID is NOT here any more. src/_ZN5Actor10FindWithIDEj.cpp used to be
   a C free function that this file wrapped into a method; main rewrote it as
   the real static method against include/Actor.h, so defining it here is a
   second definition. The traffic now runs the other way and the C face its
   callers spell lives in hal/method_faces.cpp. */
struct Actor {
    void UpdatePosWithOnlySpeed(CylinderClsn *c);
    /* gate 18: ov085's TUs declare a local Actor shadow and call these as
       methods; all three are C-form definitions in src. */
    Matrix4x3 *UpdateCarry(Player &player, const Vector3 &vec);
    Player *ClosestPlayer();
    short HorzAngleToCPlayer();
    void DropShadowRadHeight(ShadowModel &sm, Matrix4x3 &m, int rad, int h,
                             unsigned f);
};
/* THE OTHER DIRECTION, in the same place for the same reason.
   src/_ZN5Actor11UpdateCarryER6PlayerRK7Vector3.cpp defines UpdateCarry as a
   method of its OWN local `class Actor` -- include/Actor.h does not declare
   it -- so the C name its ov085 caller uses has to be defined against a
   shadow too, and this is the only file that has one. Not an alias: the C
   form is __cdecl with `this` on the stack, the method is __thiscall with it
   in ecx. */
extern "C" Matrix4x3 *_ZN5Actor11UpdateCarryER6PlayerRK7Vector3(
    void *self, void *player, const void *vec)
{ return ((Actor *)self)->UpdateCarry(*(Player *)player,
                                      *(const Vector3 *)vec); }
Player *Actor::ClosestPlayer()
{ return (Player *)_ZN5Actor13ClosestPlayerEv(this); }
short Actor::HorzAngleToCPlayer()
{ return _ZN5Actor18HorzAngleToCPlayerEv(this); }
void Actor::DropShadowRadHeight(ShadowModel &sm, Matrix4x3 &m, int rad, int h,
                                unsigned f)
{ _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
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
unsigned char OAM::GetObjWidth(int a, int b)
{ return _ZN3OAM11GetObjWidthEii(a, b); }
unsigned char OAM::GetObjHeight(int a, int b)
{ return _ZN3OAM12GetObjHeightEii(a, b); }
int OAM::LoadAffineParams(OamAttr *attr, int *p, Matrix2x2 *m)
{ return _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(attr, p, m); }

struct SaveData { static int IsCharacterUnlocked(unsigned ch); };
int SaveData::IsCharacterUnlocked(unsigned ch)
{ return _ZN8SaveData19IsCharacterUnlockedEj(ch); }

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
/* PlaySub is a REAL adapter on every target: callers declare the 4th arg as a
   plain int, so `Sound::PlaySub(...,int,bool)` mangles to _ZN5Sound7PlaySubEjjjib
   -- a DIFFERENT symbol from the src body's _ZN5Sound7PlaySubEjjj5Fix12IiEb
   (Fix12<int>). It does not self-recurse on GCC and must stay to bridge the two
   manglings. */
int Sound::PlaySub(unsigned a, unsigned b, unsigned c, int d, bool e)
{ sd_consumer_init(); return _ZN5Sound7PlaySubEjjj5Fix12IiEb(a, b, c, d, e ? 1 : 0); }
#ifdef _WIN32
/* MSVC: the `Sound::` method mangling differs from the extern-C name, so these
   two wrappers really forward and get one free sd_consumer_init before the src
   body walks the SDAT root. */
void Sound::Play2D(unsigned a, unsigned b)
{ sd_consumer_init(); _ZN5Sound6Play2DEjj(a, b); }
void Sound::LoadAndSetMusic_Layer1(int a)
{ sd_consumer_init(); _ZN5Sound22LoadAndSetMusic_Layer1Ei(a); }
#endif /* _WIN32 -- on GCC `Sound::Play2D`/`LoadAndSetMusic_Layer1` ARE the
   _ZN5Sound...  symbols they forward to (same arg types), so the wrapper calls
   itself. On Linux the callers bind straight to the real src/ bodies;
   sd_consumer_init is already run during sound init (hal/sdat/consumer.cpp seats
   the SDAT root before any layer walks it) and is idempotent, so the per-call
   seat these two wrappers added is not needed. */

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

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN11RaycastLine10DetectClsnEv(void *self)
{ return ((RaycastLine *)self)->DetectClsn(); }
#else
extern "C" int _ZN11RaycastLine10DetectClsnEv(void *self);  /* Linux: real symbol from src/_ZN11RaycastLine10DetectClsnEv */
#endif /* _WIN32 */
extern "C" int _ZNK12WithMeshClsn13GetWallResultEv(const void *self)
{ return ((const WithMeshClsn *)self)->GetWallResult(); }
extern "C" int _ZNK12WithMeshClsn14GetFloorResultEv(const void *self)
{ return ((const WithMeshClsn *)self)->GetFloorResult(); }

/* gate 172: func_ov098_021390ec (Crate's state helper) declares a local
   `struct SurfaceInfo { void CopyNormalTo(Vector3 &) const; }` and calls it as
   a real method; the matched body is the C-form src
   (_ZNK11SurfaceInfo12CopyNormalToER7Vector3.c, slice_gate8.txt), so the
   method bridges to it -- an alias cannot cross the __thiscall/cdecl seam. */
struct SurfaceInfo { void CopyNormalTo(Vector3 &) const; };
extern "C" void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(const void *self,
                                                          Vector3 *v);
#ifdef _WIN32 /* LINUX: this method IS _ZNK11SurfaceInfo12CopyNormalToER7Vector3
   (its own mangling) -> self-recurse. The matched src TU owns the symbol on
   Linux; every method-form caller binds straight to it. */
void SurfaceInfo::CopyNormalTo(Vector3 &v) const
{ _ZNK11SurfaceInfo12CopyNormalToER7Vector3(this, &v); }
#endif /* _WIN32 */

struct SphereClsn { int DetectClsn(); };
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN10SphereClsn10DetectClsnEv(void *self)
{ return ((SphereClsn *)self)->DetectClsn(); }
#else
extern "C" int _ZN10SphereClsn10DetectClsnEv(void *self);  /* Linux: real symbol from src/_ZN10SphereClsn10DetectClsnEv */
#endif /* _WIN32 */

struct Message { void Update(); static void AddChar(char c); };
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN7Message6UpdateEv(void *self)
{ ((Message *)self)->Update(); }
#else
extern "C" void _ZN7Message6UpdateEv(void *self);  /* Linux: real symbol from src/_ZN7Message6UpdateEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN7Message7AddCharEc(char ch)
{ Message::AddChar(ch); }
#else
extern "C" void _ZN7Message7AddCharEc(char ch);  /* Linux: real symbol from src/_ZN7Message7AddCharEc */
#endif /* _WIN32 */


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

/* shadow-defined in their own TUs (struct CylinderClsn / struct Camera) */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self,
                                                                  void *cl)
{ ((Actor *)self)->UpdatePosWithOnlySpeed((CylinderClsn *)cl); }
#else
extern "C" void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self,
                                                                  void *cl);  /* Linux: real symbol from src/_ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn */
#endif /* _WIN32 */
struct Camera { void SetFlag_3(); };
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN6Camera9SetFlag_3Ev(void *self)
{ ((Camera *)self)->SetFlag_3(); }
#else
extern "C" void _ZN6Camera9SetFlag_3Ev(void *self);  /* Linux: real symbol from src/_ZN6Camera9SetFlag_3Ev */
#endif /* _WIN32 */

/* REAL BODY here, not a hop back to the method: the method shadow above
   already forwards to this C name, and forwarding back made a mutual
   tail-call -- /O2 turned it into a two-instruction jmp cycle that hung
   the first real-collision frame (WATCHDOG pinned EIP inside it).
   Field per src/_ZNK10ClsnResult9GetClsnIDEv.cpp: objID at +0x1c. */
extern "C" int _ZNK10ClsnResult9GetClsnIDEv(const void *self)
{ return (int)((const unsigned *)self)[7]; }
