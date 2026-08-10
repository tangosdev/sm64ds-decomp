// GATE 32: the faces that CANNOT be built against include/.
//
// hal/bob_enemy_bridges.cpp writes a C-linkage face for every method whose
// class is in include/. Four of gate 32's are not: the defining TU declares its
// own LOCAL SHADOW CLASS for a method the shared header never got, so the only
// way to emit the decorated symbol is to declare the same shadow again -- and
// that cannot be done in a TU which has already included the real header,
// because the class definitions would collide.
//
// So this file includes NOTHING. MSVC's decoration of a member function does
// not depend on the rest of its class, so a shadow carrying only the methods
// being reached is enough.
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
struct Vector3_16_local { unsigned short x, y, z; };

/* CapEnemy::GetCapState (arm9 0x02005fa0) and CapEnemy::UpdateCapPos
   (0x020062b8). include/CapEnemy.h declares neither.
     * GetCapState's own TU declares the identical shadow, so its face is a
       plain forward from the Itanium C name the ov084 callers use.
     * UpdateCapPos is a NAME COLLISION rather than a missing declaration: its
       definition spells the second argument Vector3_16 and func_ov084_0212a580
       spells it Vector3_16_local. One struct on the ROM, two decorated names to
       MSVC. Declaring both overloads here defines the caller's and forwards it
       to the definition's. */
struct CapEnemy {
    int GetCapState();
    void UpdateCapPos(const Vector3 &pos, const Vector3_16 &rot);
    void UpdateCapPos(const Vector3 &pos, const Vector3_16_local &rot);
};

/* WithMeshClsn::Unk_0203589c (arm9 0x0203589c), reached from
   func_ov084_02129168, and Actor::UntrackInDeathTable (0x0200fd1c), reached
   from func_ov084_021298d0. Both are shadow methods in their own TUs. */
struct WithMeshClsn { void Unk_0203589c(); };
/* Actor::UntrackInDeathTable (arm9 0x0200fd1c), plus the two landing-dust
   wrappers at 0x0200fb4c / 0x0200fb84. include/Actor.h declares only the ...At
   forms that take a position; these take the actor's own. All three have
   C-named definitions and shadow-method callers. */
struct Actor {
    void UntrackInDeathTable();
    void HugeLandingDust(bool b);
    void LandingDust(bool b);
};

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" int _ZN8CapEnemy11GetCapStateEv(void *self)
{ return ((CapEnemy *)self)->CapEnemy::GetCapState(); }
#else
extern "C" int _ZN8CapEnemy11GetCapStateEv(void *self);  /* Linux: real symbol from src/_ZN8CapEnemy11GetCapStateEv */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN12WithMeshClsn12Unk_0203589cEv(void *self)
{ ((WithMeshClsn *)self)->WithMeshClsn::Unk_0203589c(); }
#else
extern "C" void _ZN12WithMeshClsn12Unk_0203589cEv(void *self);  /* Linux: real symbol from src/_ZN12WithMeshClsn12Unk_0203589cEv */
#endif /* _WIN32 */

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN5Actor19UntrackInDeathTableEv(void *self)
{ ((Actor *)self)->Actor::UntrackInDeathTable(); }
#else
extern "C" void _ZN5Actor19UntrackInDeathTableEv(void *self);  /* Linux: real symbol from src/_ZN5Actor19UntrackInDeathTableEv */
#endif /* _WIN32 */

void CapEnemy::UpdateCapPos(const Vector3 &pos, const Vector3_16_local &rot)
{ UpdateCapPos(pos, *(const Vector3_16 *)&rot); }

/* Camera::SetLookAt and Camera::SetPos (arm9 0x0200d7c0 and 0x0200d7ac).
   include/Camera.h has neither; both definitions are C-named free functions in
   their own .c TUs, and func_ov084_0212ccb4 -- the buddy's cannon cutscene --
   calls them as methods on a shadow of its own. */
struct Camera {
    void SetLookAt(const Vector3 &v);
    void SetPos(const Vector3 &v);
};

/* ModelAnim::SetAnim (arm9 0x02016ecc). include/ModelAnim.h DOES declare it,
   and says out loud that the definition stays a mangled free function rather
   than a method, so the caller's `?SetAnim@ModelAnim@@QAEXPAUBCA_File@@HHI@Z`
   has no definition either way. func_ov084_0212c92c is the caller. */
struct BCA_File;
struct ModelAnim {
    void SetAnim(BCA_File *animFile, int flags, int speed, unsigned startFrame);
};

extern "C" {
void _ZN6Camera9SetLookAtERK7Vector3(void *self, const void *v);
void _ZN6Camera6SetPosERK7Vector3(void *self, const void *v);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int flags,
                                                 int speed, unsigned start);
}

void Camera::SetLookAt(const Vector3 &v)
{ _ZN6Camera9SetLookAtERK7Vector3(this, &v); }
void Camera::SetPos(const Vector3 &v)
{ _ZN6Camera6SetPosERK7Vector3(this, &v); }
void ModelAnim::SetAnim(BCA_File *f, int flags, int speed, unsigned start)
{ _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(this, f, flags, speed, start); }

extern "C" {
void _ZN5Actor15HugeLandingDustEb(void *self, int b);
void _ZN5Actor11LandingDustEb(void *self, int b);
}
void Actor::HugeLandingDust(bool b) { _ZN5Actor15HugeLandingDustEb(this, b); }
void Actor::LandingDust(bool b) { _ZN5Actor11LandingDustEb(this, b); }

/* ApproachLinear, a plain free function whose definition already carries the
   Itanium C name (hal/method_faces.cpp bridges the reference form). ov084's
   caller declares it without extern "C", so MSVC decorates it. */
extern "C" void _Z14ApproachLinearRsss(short *x, short target, short step);
void ApproachLinear(short &x, short target, short step)
{ _Z14ApproachLinearRsss(&x, target, step); }

/* ---- ov014's four ---------------------------------------------------------
   src/func_ov014_02112ea8.cpp -- the CHAIN_CHOMP_FENCE breaking apart --
   declares its own shadow for each of these, and it names the actor class
   `ActorS`, so MSVC decorates PoofDustAt against that name rather than against
   Actor. Every definition is an Itanium C-named free function already in the
   build; what follows is the decorated reference pointing at it.

   MeshColliderBase::IsEnabled is not here. It is defined inline in that TU's
   own shadow, so MSVC emits the load rather than a call. */
struct MeshColliderBase { void Disable(); };
struct ActorS { void PoofDustAt(const Vector3 &v); };
/* Sound has no header at all -- every TU that reaches it declares its own
   two-line shadow -- so the king's boss music pair lands here rather than in
   hal/bob_enemy_header_faces.cpp. Both definitions are real static methods on
   a shadow of the same name, and his own states name them by the Itanium C
   name, so these are C faces onto them rather than alternatenames. */
struct Sound {
    static void PlayBank3(unsigned id, const Vector3 &v);
    static void LoadAndSetMusic_Layer3(unsigned musicId);
    static void Func_02048eb4();
};
namespace Particle {
struct System { static void *NewSimple(unsigned t, int x, int y, int z); };
}

extern "C" {
void _ZN16MeshColliderBase7DisableEv(void *self);
void _ZN5Actor10PoofDustAtERK7Vector3(void *self, const void *v);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned id, const void *v);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned t, int x, int y,
                                                     int z);
}

void MeshColliderBase::Disable() { _ZN16MeshColliderBase7DisableEv(this); }
void ActorS::PoofDustAt(const Vector3 &v)
{ _ZN5Actor10PoofDustAtERK7Vector3(this, &v); }
void Sound::PlayBank3(unsigned id, const Vector3 &v)
{ _ZN5Sound9PlayBank3EjRK7Vector3(id, &v); }
void *Particle::System::NewSimple(unsigned t, int x, int y, int z)
{ return _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(t, x, y, z); }

#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned musicId)
{ Sound::LoadAndSetMusic_Layer3(musicId); }
#else
extern "C" void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned musicId);  /* Linux: real symbol from src/_ZN5Sound22LoadAndSetMusic_Layer3Ej */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
extern "C" void _ZN5Sound13Func_02048eb4Ev(void)
{ Sound::Func_02048eb4(); }
#else
extern "C" void _ZN5Sound13Func_02048eb4Ev(void);  /* Linux: real symbol from src/_ZN5Sound13Func_02048eb4Ev */
#endif /* _WIN32 */
