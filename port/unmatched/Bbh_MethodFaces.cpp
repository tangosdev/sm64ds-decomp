/* MSVC-METHOD FACES for three arm9 bodies the BBH cast's matched .cpp TUs
 * call as real C++ methods -- each TU declares a minimal local class and
 * dispatches ?Method@Class@@... by MSVC name, while the matched body compiles
 * with C linkage. Each face here IS the MSVC method, defined against the same
 * minimal local declaration (this file includes NO class headers, so the
 * local shapes cannot collide with include/'s richer ones -- the reason these
 * do not sit in hal/actor_classes_ov063.cpp, which includes Actor.h), and it
 * forwards `this` into the C body. An /alternatename can never do this: the
 * targets are __thiscall (the method_faces.cpp law).
 *
 *   ?TrackStar@dActor_c@@QAEIII@Z            <- _ZN8dActor_c9TrackStarEjj (arm9
 *       0x0200ff94, matched .c on slice_w5a.txt); wanted by
 *       _ZN11daTrsIcon_c13InitResourcesEv.cpp (the boss star marker)
 *   ?JustHitGround@dBgCh_Actr@@QBE_NXZ  <- _ZNK12WithMeshClsn
 *       13JustHitGroundEv (arm9 0x0203571c, matched .c); wanted by
 *       _ZN11daTBasket_c8BehaviorEv.cpp (the cage's landing bounce). Const
 *       method, bool return -- the C body returns s32, normalized != 0.
 *   ?JumpIntoBooCage@Player@@QAEXAAUVector3@@@Z <- _ZN6Player
 *       15JumpIntoBooCageER7Vector3 (ov002, matched extern-C .cpp); wanted
 *       by the same Behavior (the player getting pulled into the cage).
 */

struct Vector3;
struct Vector3_16;

extern "C" {
int _ZN8dActor_c9TrackStarEjj(void *self, unsigned a, unsigned b);
int _ZNK10dBgCh_Actr13JustHitGroundEv(const void *self);
int _ZN6Player15JumpIntoBooCageER7Vector3(void *self, Vector3 *v);
}

struct Actor { unsigned TrackStar(unsigned a, unsigned b); };
unsigned Actor::TrackStar(unsigned a, unsigned b)
{ return (unsigned)_ZN8dActor_c9TrackStarEjj(this, a, b); }

struct WithMeshClsn { bool JustHitGround() const; };
bool WithMeshClsn::JustHitGround() const
{ return _ZNK10dBgCh_Actr13JustHitGroundEv(this) != 0; }

struct Player { void JumpIntoBooCage(Vector3 &v); };
void Player::JumpIntoBooCage(Vector3 &v)
{ _ZN6Player15JumpIntoBooCageER7Vector3(this, &v); }

/* ---- Coffin closure addendum: three return-type spellings ------------------
 * Coffin::InitResources (host mirror) declares int-returning SetFile pair
 * and a KCL_File*-returning MeshCollider::LoadFile; the linked MSVC bodies
 * spell void/char* returns, so those mangles do not exist. Same bridge, into
 * the matched C-linkage bodies (all three in the map).
 */
struct BMD_File;
struct KCL_File;
struct SharedFilePtr;
struct Matrix4x3;
struct CLPS_Block;
typedef int Fix12_;

extern "C" {
void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr *f);
int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *f, const Matrix4x3 *m, int s, short n, CLPS_Block *c);
}

/* ModelBase::SetFile's int-returning spelling is already bridged by
   hal/bob_enemy_bridges.cpp -- not repeated here (build 9's LNK2005). */

struct MeshCollider { static KCL_File *LoadFile(SharedFilePtr &f); };
KCL_File *MeshCollider::LoadFile(SharedFilePtr &f)
{ return (KCL_File *)_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&f); }

struct MovingMeshCollider {
    int SetFile(KCL_File *f, const Matrix4x3 &m, Fix12_ s, short n, CLPS_Block &c);
};
int MovingMeshCollider::SetFile(KCL_File *f, const Matrix4x3 &m, Fix12_ s,
                                short n, CLPS_Block &c)
{ return _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(this, f, &m, s, n, &c); }
