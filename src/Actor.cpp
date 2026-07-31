//cpp
/*
 * Consolidated class implementation: Actor
 * Total methods: 81
 */

extern "C" {
    extern "C" int DeathTable_GetBit(int id);
    extern "C" int _ZN5Actor13DistToCPlayerEv(Actor *self);
    extern "C" int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(Actor *self, Fix12 threshold)
    extern "C" void *func_ov002_020e496c(Player *p);
    extern "C" void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
    extern "C" void DeathTable_ClearBit(int id);
    extern "C" void DeathTable_SetBit(int id);
    extern "C" void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *mF, short x, short y, short z);
    extern "C" void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
    extern "C" void Matrix4x3_FromTranslation(Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
    extern "C" void MulMat4x3Mat4x3(Matrix4x3 *m1, Matrix4x3 *m0, Matrix4x3 *mF);
    extern "C" void Vec3_Asr(Vector3 *vF, const Vector3 *v, int amount);
    extern "C" void Vec3_Lsl(Vector3 *vF, const Vector3 *v, int amount);
    extern "C" void _ZN5Actor13LandingDustAtER7Vector3b(Actor *self, Vector3 *pos, bool b)
    extern "C" void _ZN5Actor17HugeLandingDustAtER7Vector3b(Actor *self, Vector3 *pos, bool b)
    extern "C" void _ZN5Actor25OnAimedAtWithEggReturnVecEv(Vec *ret, Base *self)
    extern "C" void func_02010da4(int *p);
    extern "C" void* _ZN5ActorC1Ev(char* c) {
    extern "C" void* _ZN5ActorC2Ev(char* self)
    extern Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 actorID, u32 param1, const Vector3* pos, const Vector3_16* rot, s32 areaID, s32 deathTableID);
    extern ActorBase *_ZN12ActorDerived5SpawnEjP9ActorBaseii(u32 actorID, ActorBase *parent, u32 param1, s32 flags);
    extern ActorBase *func_02010e78(const struct Vector3 *pos, const struct Vector3_16 *rot, s8 areaID, s16 deathTableID);
    extern Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
    extern Vector3* func_02037dc4(void*);
    extern int *func_02043f4c(int *a, unsigned int j, int b);
    extern int IsStarCollectedInCurLevel(int i);
    extern int RandomIntInternal(int *seed);
    extern int SublevelToLevel(int i);
    extern int Vec3_Dist(const void* a, const void* b);
    extern int _ZN11RaycastLine10DetectClsnEv(RaycastLine*);
    extern int _ZN5Actor18GetBitInDeathTableEv(Actor* self);
    extern int _ZN9ActorBase12BeforeRenderEv(struct Actor* self);
    extern int _ZN9ActorBase19BeforeInitResourcesEv(Actor* self);
    extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
    extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);
    extern s32 _ZN9ActorBase22BeforeCleanupResourcesEv(struct ActorBase *self);
    extern struct Actor *Actor_ClosestPlayer(void);
    extern struct Actor *Actor_FindWithActorID(unsigned int actorID, struct Actor *searchStart);
    extern struct Actor *_ZN5Actor13ClosestPlayerEv(void);
    extern struct Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int id, unsigned int param, const struct Vector3 *pos, const struct Vector3_16 *r, int a, int b);
    extern struct Actor *func_ov002_020f0918(struct Actor *a, void *ps);
    extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
    extern struct Actor* func_02043f98(struct Actor**, unsigned int);
    extern void *_ZN5Actor13ClosestPlayerEv(void *self);
    extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 param, struct Vector3 *v, struct Vector3_16 *vr, s32 a, s32 b);
    extern void Actor_TeardownA(void *global, void *actorSub);   /* 0x0203b27c */
    extern void Actor_TeardownB(void *actorSub);                 /* 0x02044104 */
    extern void GiveCoins(s32 a, s32 b);
    extern void Memory_Deallocate(void *ptr, struct Heap *heap); /* 0x0203c1e8 */
    extern void SetStarMarker(int i, int v1, int v2);
    extern void SetStarMarker(s32 id, struct Actor *starMarker, u8 type); /* 0x0202a660 */
    extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(RaycastLine*, const Vector3*, const Vector3*, Actor*);
    extern void _ZN11RaycastLineC1Ev(RaycastLine*);
    extern void _ZN11RaycastLineD1Ev(RaycastLine*);
    extern void _ZN12ActorDerived18AfterInitResourcesEj(ActorDerived *self, u32 result);
    extern void _ZN5Actor10PoofDustAtERK7Vector3(struct Actor *thiz, const struct Vector3 *vec); /* 0x0200fdfc */
    extern void _ZN5Actor11UntrackStarERa(Actor* self, s8* starID);
    extern void _ZN5Actor13LandingDustAtER7Vector3b(struct Actor *thiz, struct Vector3 *vec, int doRaycast); /* 0x0200fac4 */
    extern void _ZN5Actor16TriplePoofDustAtERK7Vector3(struct Actor *thiz, const struct Vector3 *vec); /* 0x0200fd74 */
    extern void _ZN5Actor17HugeLandingDustAtER7Vector3b(struct Actor *thiz, struct Vector3 *vec, int doRaycast); /* 0x0200fb84 */
    extern void _ZN5Actor17TrackInDeathTableEv(Actor* self);
    extern void _ZN5Actor19DisappearPoofDustAtERK7Vector3(struct Actor *thiz, const struct Vector3 *vec); /* 0x0200fd04 */
    extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(struct Actor* self, struct CylinderClsn* clsn);
    extern void _ZN5Actor24KillAndTrackInDeathTableEv(void);
    extern void _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(struct Actor* self);
    extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, struct Vector3 *v);
    extern void _ZN6Player4HealEi(void *p, s32 amt);
    extern void _ZN9ActorBase11AfterRenderEj(void);
    extern void _ZN9ActorBase13AfterBehaviorEj(void);
    extern void _ZN9ActorBase18MarkForDestructionEv(Actor* self);
    extern void _ZN9ActorBase21AfterCleanupResourcesEj(void);
    extern void _ZN9ActorBaseD2Ev(int c);
    extern void _ZN9ActorBaseD2Ev(struct Actor *thiz);           /* 0x02043d48 */
    extern void func_0200d8c8(void*, int);
    extern void func_0203b27c(int a, int c);
    extern void func_02044104(int a);
}

extern ActorBase *gActorListHead; /* 0x0209f5c0 */
extern Fix12i gClosestPlayerDist; /* @ 0x0208e380 */
extern Matrix4x3 data_020a0e68;
extern int data_02075244[];
extern int data_0208e37c;
extern int data_0208e380;
extern int data_0208e3a4;
extern int data_0208e3a4[];
extern int data_0208e4b8;
extern int data_0208e4b8[];
extern int data_0209b468;
extern int data_0209b468[];
extern int data_0209e650[];
extern int data_0209f40c[];
extern int data_0209fc68;
extern int* data_020a4bb8;
extern s16 data_02082214[];
extern s16 data_0208e378;
extern s16* data_0209b45c;
extern s16* data_0209b460;
extern short data_0208e378;
extern signed char data_0209b44c;
extern signed char data_0209f2f8[];
extern struct Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
extern struct Actor *gClosestPlayer; /* @ 0x0209b458 */
extern struct Actor *gFarthestPlayer; /* @ 0x0209b450 */
extern struct Actor* data_0209b468;
extern struct Heap *Memory_gameHeapPtr;                      /* 0x020a0eac */
extern u16 data_02075230[];
extern u32 data_02075238[];
extern unsigned char data_0209f21c;
extern unsigned char data_0209f220[];
extern unsigned char data_0209f2c0[];
extern unsigned char data_0209f2d8;
extern void *Actor_TeardownGlobal;                           /* 0x0209b468 */
extern void *_ZTV12ActorDerived[];
extern void *_ZTV5Actor[];
extern void _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
extern void* data_0208e3a4;
extern void* data_0208e4b8;
extern void* data_0209b450;
extern void* data_0209b458;
extern void* data_0209b45c;
extern void* data_0209b460;
extern void* data_0209b468;
extern void* data_0209f318;
extern void* data_0209f394[];
extern void* data_020a4bb8;

struct Actor {
    // Class member declarations
    // Earthquake
    // FindWithID
    // PoofDustAt
    // SpawnCoins
    // AfterRender
    // LandingDust
    // OnAttacked1
    // OnAttacked2
    // SpawnNumber
    // UntrackStar
    // UpdateCarry
    // BeforeRender
    // ReflectAngle
    // AfterBehavior
    // ClosestPlayer
    // DistToCPlayer
    // LandingDustAt
    // OnTurnIntoEgg
    // OnYoshiTryEat
    // SmallPoofDust
    // SpawnFireball
    // SpawnSoundObj
    // BeforeBehavior
    // FarthestPlayer
    // GetSubtraction
    // TriplePoofDust
    // FindWithActorID
    // GivePlayerCoins
    // HugeLandingDust
    // IsPlayerInRange
    // OnGroundPounded
    // OnHitByMegaChar
    // JumpedOnByPlayer
    // OnAimedAtWithEgg
    // TriplePoofDustAt
    // DetectRaycastClsn
    // GetWaterHeightWDW
    // HugeLandingDustAt
    // TrackInDeathTable
    // AfterInitResources
    // ClosestWithActorID
    // DropShadowScaleXYZ
    // FindExplosionActor
    // GetBitInDeathTable
    // HorzAngleToCPlayer
    // HorzAngleToFPlayer
    // BeforeInitResources
    // DisappearPoofDustAt
    // DropShadowRadHeight
    // MakeVanishLuigiWork
    // OnHitFromUnderneath
    // UntrackAndSpawnStar
    // UntrackInDeathTable
    // AfterCleanupResources
    // BeforeCleanupResources
    // ClosestNonVanishPlayer
    // IsTooFarAwayFromPlayer
    // UpdatePosWithOnlySpeed
    // HorzAngleToCPlayerOrAng
    // BumpedUnderneathByPlayer
    // KillAndTrackInDeathTable
    // OnHitByCannonBlastedChar
    // OnAimedAtWithEggReturnVec
    // UpdatePosWithHorzSpeedAndAng
    // Next
    // Spawn
    // FindEgg
    // OnKicked
    // OnPushed
    // PoofDust
    // SetRanges
    // TrackStar
    // UpdatePos
    // Virtual50
    // Actor
    // ~Actor
};

// --- Actor::Earthquake ---
void Actor::Earthquake(void* v, int f) {
    func_0200d8c8(data_0209f318, f);

// --- Actor::FindWithID ---
struct Actor* Actor::FindWithID(unsigned int id){
  struct Actor* p = func_02043f98(&data_0209b468, id);
  if(p) return *(struct Actor**)((char*)p+8);
  return 0;

// --- Actor::PoofDustAt ---
void Actor::PoofDustAt(struct Actor *self, const struct Vector3 *pos)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x122, pos->x, pos->y, pos->z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x123, pos->x, pos->y, pos->z);

// --- Actor::SpawnCoins ---
void Actor::SpawnCoins(
    struct Actor *self, const struct Vector3 *pos, s32 count,
    s32 spread, s16 angle)
{
    struct Actor *actor;
    s32 ang;
    s32 prev;
    s32 i;
    u32 r;
    s16 ang0;
    struct Vector3_16 *rot;
    s32 deathId;
    u32 id;
    u32 param;
    ang0 = angle;
    rot = 0;
    id = 0x120;
    param = 2;
    deathId = -1;
    prev = 0xff;
    if (count > 1) {
        if (spread < 0x4000)
            spread = 0x4000;
    for (i = 0; i < count; i++) {
        actor = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            id, param, pos, rot,
            *(signed char *)((char *)self + 0xcc), deathId);
        if (actor != 0) {
            do {
                ang = (s32)(((u32)RandomIntInternal(data_0209e650) >> 16) << 27) >> 16;
            } while (ang == prev);
            r = (u32)RandomIntInternal(data_0209e650);
            r >>= 16;
            *(s16 *)((char *)actor + 0x92) = 0;
            spread = (u32)((u32)spread * ((r % 50) + 100)) / 100;
            prev = ang;
            *(s16 *)((char *)actor + 0x94) = (s16)(ang0 + ang);
            *(s16 *)((char *)actor + 0x96) = 0;
            *(s32 *)((char *)actor + 0x98) = spread;

// --- Actor::AfterRender ---
/* Actor::AfterRender @ 0x2010f6c (arm9) -- tail-call veneer to _ZN9ActorBase11AfterRenderEj (0x2043ac4).
 * ldr ip, [pc]; bx ip; .word 0x2043ac4
 */
void _ZN5Actor11AfterRenderEj(void) {
    _ZN9ActorBase11AfterRenderEj();

// --- Actor::LandingDust ---
void Actor::LandingDust(struct Actor *thiz, int doRaycast)
{
    struct Vector3 vec;
    vec.x = thiz->pos.x;
    vec.y = thiz->pos.y;
    vec.z = thiz->pos.z;
    _ZN5Actor17HugeLandingDustAtER7Vector3b(thiz, &vec, doRaycast);

// --- Actor::OnAttacked1 ---
void Actor::OnAttacked1(struct Actor* self, struct Actor* attacker)
{

// --- Actor::OnAttacked2 ---
void Actor::OnAttacked2(struct Actor* self, struct Actor* attacker)
{

// --- Actor::SpawnNumber ---
struct Actor *Actor::SpawnNumber(struct Actor *self, const struct Vector3 *pos, unsigned int v, int b, unsigned short t, void *ps)
{
    struct Actor *r;
    if (b) {
        r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x14a, (v & 0xf) | 0x10, pos, 0, *(signed char*)((char*)self+0xcc), -1);
    } else {
        r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x14a, v, pos, 0, *(signed char*)((char*)self+0xcc), -1);
    if (r == 0) return r;
    *(unsigned short*)((char*)r+0x14c) = t;
    return func_ov002_020f0918(r, ps);

// --- Actor::UntrackStar ---
void Actor::UntrackStar(struct Actor *thiz, s8 *starID)
{
    s8 id;
    id = *starID;
    if (id < 0)
        return;
    SetStarMarker((s32)id, (struct Actor *)0, 2);
    *starID = -1;

// --- Actor::UpdateCarry ---


// --- Actor::BeforeRender ---
int Actor::BeforeRender(struct Actor* this_)
{
    u32 flags;
    if (!_ZN9ActorBase12BeforeRenderEv(this_))
        return 0;
    flags = this_->flags;
    if (flags & 0x20)
        goto ret0;
    if (!(flags & 8))
        goto ret1;
    if (!(flags & 2))
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;

// --- Actor::ReflectAngle ---
s16 Actor::ReflectAngle(struct Actor *thiz, Fix12i normalX, Fix12i normalZ, s16 angToReflect)
{
    s16 normalAngle = _ZN4cstd5atan2E5Fix12IiES1_(normalX, normalZ);
    s16 delta = (s16)((angToReflect + 0x8000) - normalAngle);
    return (s16)(normalAngle - delta);

// --- Actor::AfterBehavior ---
/* Actor::AfterBehavior @ 0x2010fc8 (arm9) -- tail-call veneer to _ZN9ActorBase13AfterBehaviorEj (0x2043af8).
 * ldr ip, [pc]; bx ip; .word 0x2043af8
 */
void _ZN5Actor13AfterBehaviorEj(void) {
    _ZN9ActorBase13AfterBehaviorEj();

// --- Actor::ClosestPlayer ---
void* Actor::ClosestPlayer(void* c)
{
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = 0x80000000;
        int i;
        for (i = 0; i < data_0209f21c; i++) {
            void* p = data_0209f394[i];
            if (!p) continue;
            int d = Vec3_Dist((char*)c + 0x5c, (char*)p + 0x5c);
            if (d < data_0208e380) { data_0208e380 = d; data_0209b458 = p; }
            if (d > data_0208e37c) { data_0208e37c = d; data_0209b450 = p; }
    return data_0209b458;

// --- Actor::DistToCPlayer ---
Fix12i Actor::DistToCPlayer(struct Actor *self)
{
    _ZN5Actor13ClosestPlayerEv();
    return gClosestPlayerDist;

// --- Actor::LandingDustAt ---


// --- Actor::OnTurnIntoEgg ---
/* Actor::OnTurnIntoEgg @ 0x2010154 (arm9) -- tail-call veneer to _ZN5Actor24KillAndTrackInDeathTableEv (0x200f9b8).
 * ldr ip, [pc]; bx ip; .word 0x200f9b8
 */
void _ZN5Actor13OnTurnIntoEggER6Player(void) {
    _ZN5Actor24KillAndTrackInDeathTableEv();

// --- Actor::OnYoshiTryEat ---
OnYoshiEatReturnVal Actor::OnYoshiTryEat(struct Actor* self)
{
    return 0;

// --- Actor::SmallPoofDust ---
void Actor::SmallPoofDust(struct Actor *thiz)
{
    struct Vector3 vec;
    vec.x = thiz->pos.x;
    vec.y = thiz->pos.y;
    vec.z = thiz->pos.z;
    _ZN5Actor19DisappearPoofDustAtERK7Vector3(thiz, &vec);

// --- Actor::SpawnFireball ---
void Actor::SpawnFireball(
    Actor* self,
    const Vector3* pos,
    const Vector3_16* rot,
    Fix12i horzSpeed,
    Fix12i unk35c,
    u32 param1
) {
    Actor* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xfe, param1, pos, rot, (s32)self->areaID, -1);
    if (a != (Actor*)0) {
        a->horzSpeed = horzSpeed;
        a->unk35c = unk35c;

// --- Actor::SpawnSoundObj ---
void Actor::SpawnSoundObj(Actor* self, u32 soundObjParam) {
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0x167,
        soundObjParam,
        &self->pos,
        (const Vector3_16*)0,
        (s32)self->areaID,
        -1
    );

// --- Actor::BeforeBehavior ---
int Actor::BeforeBehavior(char* self)
{
    if (!_ZN9ActorBase14BeforeBehaviorEv(self))
        return 0;
    data_0209b458 = 0;
    signed char c = *(signed char*)(self + 0xcc);
    if (c >= 0 && !IsAreaShowing(c)) {
        *(u32*)((int)(self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x38;
        *(int*)(self + 0x74) = 0x7fffffff;
        *(int*)(self + 0x78) = 0x7fffffff;
        *(int*)(self + 0x7c) = 0x7fffffff;
        if (data_0209fc68 == 0 || (*(u32*)(self + 0xb0) & 0x10000))
            return 0;
    } else {
        if (*(int*)(self + 0xb8) != 0) {
            Vector3 tmp;
            tmp.x = *(int*)(self + 0x5c) >> 3;
            tmp.y = (*(int*)(self + 0x60) + *(int*)(self + 0xb4)) >> 3;
            tmp.z = *(int*)(self + 0x64) >> 3;
            MulVec3Mat4x3(&tmp, &data_0209b3ec, (Vector3*)(self + 0x74));
        if (*(u32*)(self + 0xb0) & 0x10003) {
            int r = _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(
                        &data_0209f43c,
                        (Vector3*)(self + 0x74),
                        *(int*)(self + 0xb8),
                        (unsigned char*)(self + 0xc4));
            int thresh = *(int*)(self + 0xbc);
            if (data_0209f274)
                thresh <<= 1;
            if (r > thresh) {
                *(u32*)((int)(self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x18;
                if (*(u32*)(self + 0xb0) & 0x10000)
                    return 0;
            } else {
                u32* p = (u32*)((int)(self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
                *p &= ~0x38;
                if (r > *(int*)(self + 0xc0))
                    *p |= 0x10;
        } else {
            *(u32*)((int)(self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x38;
    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) == 0) {
        u32 f = *(u32*)(self + 0xb0);
        if ((f & 9) != 9 && (data_0209b464 == 0 || (f & data_0209b464) != 0))
            goto do_copy;
        if (data_0209fc68 != 0)
            goto do_copy;
    return 0;
do_copy:
    *(int*)(self + 0x68) = *(int*)(self + 0x5c);
    *(int*)(self + 0x6c) = *(int*)(self + 0x60);
    *(int*)(self + 0x70) = *(int*)(self + 0x64);
    return 1;

// --- Actor::FarthestPlayer ---
struct Actor *Actor::FarthestPlayer(struct Actor *self)
{
    _ZN5Actor13ClosestPlayerEv();
    return gFarthestPlayer;

// --- Actor::GetSubtraction ---
int Actor::GetSubtraction(void*self,short a,short b){
int d=(short)(b-a);
if(d==-0x8000) d=-0x7fff;
if(d<0) d=(short)(-d);
return d;

// --- Actor::TriplePoofDust ---
void Actor::TriplePoofDust(struct Actor *thiz)
{
    struct Vector3 vec;
    vec.x = thiz->pos.x;
    vec.y = thiz->pos.y;
    vec.z = thiz->pos.z;
    _ZN5Actor16TriplePoofDustAtERK7Vector3(thiz, &vec);

// --- Actor::FindWithActorID ---
int Actor::FindWithActorID(unsigned int j, int p) {
  int *r;
  if (p) r = func_02043f4c(data_0209b468, j, p+0x50);
  else r = func_02043f4c(data_0209b468, j, 0);
  if (r) return r[2];
  return 0;

// --- Actor::GivePlayerCoins ---
void Actor::GivePlayerCoins(void *c, void *p, unsigned char h, u32 j)
{
  s32 amt;
  void *spawned;
  struct Vector3 vec;
  struct Vector3 *src;
  if (p == 0)
  {
    return;
  if (h == 0)
  {
    return;
  if (j >= 3)
  {
    return;
  if ((*((unsigned char *) (((char *) p) + 0x706))) != 0)
  {
    _ZN5Sound9PlayBank3EjRK7Vector3(0x12, (struct Vector3 *) (((char *) c) + 0x74));
  else
  {
    _ZN5Sound9PlayBank3EjRK7Vector3(0x11, (struct Vector3 *) (((char *) c) + 0x74));
  amt = h * data_02075238[j];
  GiveCoins(*((unsigned char *) (((char *) p) + 0x6d8)), amt);
  _ZN6Player4HealEi(p, amt << 8);
  src = (struct Vector3 *) (((int) p + 0x5c) & 0xFFFFFFFFFFFFFFFF);
  vec.x = src->x;
  vec.y = src->y;
  vec.z = src->z;
  vec.y = vec.y + 0x96000;
  spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(data_02075230[j], 8, &vec, (struct Vector3_16 *) 0, *((signed char *) (((char *) p) + 0xcc)), -1);
  if (spawned != 0)
  {
    *((void **) (((char *) spawned) + 0x39c)) = p;

// --- Actor::HugeLandingDust ---
void Actor::HugeLandingDust(struct Actor *thiz, int doRaycast)
{
    struct Vector3 vec;
    vec.x = thiz->pos.x;
    vec.y = thiz->pos.y;
    vec.z = thiz->pos.z;
    _ZN5Actor13LandingDustAtER7Vector3b(thiz, &vec, doRaycast);

// --- Actor::IsPlayerInRange ---
int Actor::IsPlayerInRange(struct Actor *self, Fix12i posX, Fix12i posY, Fix12i posZ, int maxDist) {
    struct Vector3 pos;
    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
    struct Actor *closest = Actor_ClosestPlayer();
    return Vec3_Dist(&pos, &closest->pos) < (maxDist << 12);

// --- Actor::IsPlayerInRange ---
int Actor::IsPlayerInRange(struct Actor *self, const struct Vector3 *pos, int maxDist) {
    struct Actor *closest = Actor_ClosestPlayer();
    return Vec3_Dist(pos, &closest->pos) < (maxDist << 12);

// --- Actor::IsPlayerInRange ---
int Actor::IsPlayerInRange(struct Actor *self, int maxDist) {
    struct Actor *closest = Actor_ClosestPlayer();
    return Vec3_Dist(&self->pos, &closest->pos) < (maxDist << 12);

// --- Actor::OnGroundPounded ---
void Actor::OnGroundPounded(struct Actor* self, struct Actor* pounder)
{

// --- Actor::OnHitByMegaChar ---
void Actor::OnHitByMegaChar(struct Actor* self, struct Player* player)
{

// --- Actor::JumpedOnByPlayer ---
int Actor::JumpedOnByPlayer(char *self, char *clsn, char *player)
{
    volatile s32 pad[4];
    s32 vel = *(s32 *)(player + 0xa8);
    (void)&pad;
    if (*(s32 *)(clsn + 0x18) & 6) {
        if (vel < 0 && *(u8 *)(player + 0x6de) != 0
            && *(s32 *)(self + 0x60) < *(s32 *)(player + 0x60) - 0xa000)
            return 1;
        return 0;
    } else {
        s32 c10 = *(s32 *)(clsn + 0x10);
        if (vel < 0 && c10 < 0 && c10 > vel)
            return 1;
        return 0;

// --- Actor::OnAimedAtWithEgg ---
Fix12i Actor::OnAimedAtWithEgg(struct Actor* self)
{
    return 81920; /* 0x14000 == 20.0 in 20.12 */

// --- Actor::TriplePoofDustAt ---
void Actor::TriplePoofDustAt(struct Actor *self, const struct Vector3 *pos)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x124, pos->x, pos->y, pos->z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x125, pos->x, pos->y, pos->z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x126, pos->x, pos->y, pos->z);

// --- Actor::DetectRaycastClsn ---
int Actor::DetectRaycastClsn(Actor *self, Vector3 *a, Vector3 *out, int doStore){
  RaycastLine rl;
  _ZN11RaycastLineC1Ev(&rl);
  _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&rl, a, (const Vector3*)out, 0);
  if(_ZN11RaycastLine10DetectClsnEv(&rl)){
    if(doStore){
      Vector3 *p = (Vector3*)(rl.data + 0x54);
      Vector3 *q = func_02037dc4(rl.data + 0x14);
      int x = p->x + q->x;
      int z = p->z + q->z;
      int y = p->y + q->y;
      out->x = x;
      out->y = y;
      out->z = z;
    _ZN11RaycastLineD1Ev(&rl);
    return 1;
  _ZN11RaycastLineD1Ev(&rl);
  return 0;

// --- Actor::GetWaterHeightWDW ---
int Actor::GetWaterHeightWDW(char *self) {
  int ret = *(int*)(self+0x60);
  if (data_0209f2f8[0] == 0x15 && *(signed char*)(self+0xcc) == 0) {
    unsigned int i = data_0209f2c0[0];
    if (i >= 3) i = 2;
    ret = data_02075244[i];
  return ret;

// --- Actor::HugeLandingDustAt ---


// --- Actor::TrackInDeathTable ---


// --- Actor::AfterInitResources ---
void Actor::AfterInitResources(char *self, u32 result)
{
    _ZN12ActorDerived18AfterInitResourcesEj((ActorDerived *)self, result);
    *(int *)(((long long)(int)(self + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x38;

// --- Actor::ClosestWithActorID ---
struct Actor *Actor::ClosestWithActorID(struct Actor *self, unsigned int actorID) {
    struct Actor *actor = 0;
    struct Actor *closest = 0;
    Fix12i closestDist = 0x7fffffff;
    for (;;) {
        actor = Actor_FindWithActorID(actorID, actor);
        if (!actor)
            break;
        if (actor != self) {
            Fix12i dist = Vec3_Dist(&self->pos, &actor->pos);
            if (dist < closestDist) {
                closestDist = dist;
                closest = actor;
    return closest;

// --- Actor::DropShadowScaleXYZ ---
void Actor::DropShadowScaleXYZ(
    struct Actor* this_,
    struct ShadowModel* shadow,
    struct Matrix4x3* matrix,
    Fix12i scaleX,
    Fix12i scaleY,
    Fix12i scaleZ,
    u8 opacity)
{
    if (this_->flags & 0x10)
        return;
    _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(shadow, matrix, scaleX, scaleY, scaleZ, opacity);

// --- Actor::FindExplosionActor ---
struct Actor* Actor::FindExplosionActor(struct Actor* self, struct CylinderClsn* clsn) {
    u32 id = clsn->otherObjID;
    u32 flags;
    if (id != 0) {
        flags = clsn->hitFlags & 0x4000;
        if (flags != 0)
            return _ZN5Actor10FindWithIDEj(id);
    return 0;

// --- Actor::GetBitInDeathTable ---


// --- Actor::HorzAngleToCPlayer ---
s16 Actor::HorzAngleToCPlayer(struct Actor *self)
{
    _ZN5Actor13ClosestPlayerEv();
    return Vec3_HorzAngle(&self->pos, &gClosestPlayer->pos);

// --- Actor::HorzAngleToFPlayer ---
s16 Actor::HorzAngleToFPlayer(struct Actor *self)
{
    _ZN5Actor13ClosestPlayerEv();
    return Vec3_HorzAngle(&self->pos, &gFarthestPlayer->pos);

// --- Actor::BeforeInitResources ---
int Actor::BeforeInitResources(Actor* self) {
    int spawnEvenIfKilledBefore;
    spawnEvenIfKilledBefore = (self->flags & 0x8000000) ? 1 : 0;
    if (spawnEvenIfKilledBefore != 0) {
        goto skip;
    if (_ZN5Actor18GetBitInDeathTableEv(self) == 0) {
        goto skip;
    _ZN9ActorBase18MarkForDestructionEv(self);
    return 1;
skip:;
    {
        int r = _ZN9ActorBase19BeforeInitResourcesEv(self);
        if (r == 0) return 0;
        return 1;

// --- Actor::DisappearPoofDustAt ---
void Actor::DisappearPoofDustAt(struct Actor *self, const struct Vector3 *pos)
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x127, pos->x, pos->y, pos->z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x128, pos->x, pos->y, pos->z);

// --- Actor::DropShadowRadHeight ---
void Actor::DropShadowRadHeight(
    struct Actor* this_,
    struct ShadowModel* shadow,
    struct Matrix4x3* matrix,
    Fix12i radius,
    Fix12i depth,
    u8 opacity)
{
    if (this_->flags & 0x10)
        return;
    _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(shadow, matrix, radius, depth, radius, opacity);

// --- Actor::MakeVanishLuigiWork ---
void Actor::MakeVanishLuigiWork(void *self, char *clsn)
{
    void *player;
    *(u32 *)(((long long)(int)(clsn + 0x18)) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;
    player = _ZN5Actor13ClosestPlayerEv(self);
    if (player == 0)
        return;
    if (*((u8 *)player + 0x6fb) != 0) {
        *(u32 *)(((unsigned long long)(u32)(clsn + 0x18)) & 0xFFFFFFFFFFFFFFFFULL) |= 2;

// --- Actor::OnHitFromUnderneath ---
void Actor::OnHitFromUnderneath(struct Actor* self, struct Actor* attacker)
{

// --- Actor::UntrackAndSpawnStar ---
Actor* Actor::UntrackAndSpawnStar(Actor* self, s8* trackStarID, u32 starID, const Vector3* spawnPos, u8 howToSpawnStar) {
    _ZN5Actor11UntrackStarERa(self, trackStarID);
    return _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0xb2,
        starID | (howToSpawnStar << 4),
        spawnPos,
        (const Vector3_16*)0,
        (s32)self->areaID,
        -1
    );

// --- Actor::UntrackInDeathTable ---


// --- Actor::AfterCleanupResources ---
/* Actor::AfterCleanupResources @ 0x2011214 (arm9) -- tail-call veneer to _ZN9ActorBase21AfterCleanupResourcesEj (0x2043b2c).
 * ldr ip, [pc]; bx ip; .word 0x2043b2c
 */
void _ZN5Actor21AfterCleanupResourcesEj(void) {
    _ZN9ActorBase21AfterCleanupResourcesEj();

// --- Actor::BeforeCleanupResources ---
s32 Actor::BeforeCleanupResources(struct Actor *self)
{
    s32 res = _ZN9ActorBase22BeforeCleanupResourcesEv((struct ActorBase*)self);
    if (res != 0)
        return 1;
    return 0;

// --- Actor::ClosestNonVanishPlayer ---
void* Actor::ClosestNonVanishPlayer(void* c)
{
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = 0x80000000;
        int i;
        for (i = 0; i < data_0209f21c; i++) {
            void* p = data_0209f394[i];
            if (!p) continue;
            if (*(unsigned char*)((char*)p + 0x6fb)) continue;
            int d = Vec3_Dist((char*)c + 0x5c, (char*)p + 0x5c);
            if (d < data_0208e380) { data_0208e380 = d; data_0209b458 = p; }
            if (d > data_0208e37c) { data_0208e37c = d; data_0209b450 = p; }
    return data_0209b458;

// --- Actor::IsTooFarAwayFromPlayer ---


// --- Actor::UpdatePosWithOnlySpeed ---


// --- Actor::HorzAngleToCPlayerOrAng ---
s16 Actor::HorzAngleToCPlayerOrAng(struct Actor *self)
{
    struct Actor *player;
    _ZN5Actor13ClosestPlayerEv();
    player = gClosestPlayer;
    if (player == 0)
        return self->ang.y;
    return Vec3_HorzAngle(&self->pos, &player->pos);

// --- Actor::BumpedUnderneathByPlayer ---
int Actor::BumpedUnderneathByPlayer(char*self,char*player){
if(*(unsigned char*)(player+0x6de)!=0 && *(int*)(player+0xa8)>0
   && *(int*)(player+0x60) < *(int*)(self+0x60))
  return 1;
return 0;

// --- Actor::KillAndTrackInDeathTable ---
void Actor::KillAndTrackInDeathTable(Actor* self) {
    _ZN5Actor17TrackInDeathTableEv(self);
    _ZN9ActorBase18MarkForDestructionEv(self);

// --- Actor::OnHitByCannonBlastedChar ---
void Actor::OnHitByCannonBlastedChar(struct Actor* self, struct Actor* attacker)
{

// --- Actor::OnAimedAtWithEggReturnVec ---


// --- Actor::UpdatePosWithHorzSpeedAndAng ---
void Actor::UpdatePosWithHorzSpeedAndAng(void *c){
    s32 v98 = *(s32*)((char*)c+0x98);
    if (v98 == 0) {
        s32 a8 = *(s32*)((char*)c+0xa8);
        s32 v9c = *(s32*)((char*)c+0x9c);
        s32 a0 = *(s32*)((char*)c+0xa0);
        s32 nx = a8 + v9c;
        if (nx >= a0) a0 = nx;
        *(s32*)((char*)c+0xa4) = 0;
        *(s32*)((char*)c+0xa8) = a0;
        *(s32*)((char*)c+0xac) = 0;
        return;
    } else {
        s32 a8;
        s32 v9c;
        s32 a0;
        s64 px;
        s32 j;
        s16 sinv;
        s32 nx;
        s16 cosv;
        s64 pz;
        j = ((s32)*(unsigned short*)((char*)c+0x94) >> 4) << 1;
        sinv = data_02082214[j + 1];
        a8 = *(s32*)((char*)c+0xa8);
        v9c = *(s32*)((char*)c+0x9c);
        a0 = *(s32*)((char*)c+0xa0);
        px = ((s64)v98 * sinv + 0x800) >> 12;
        nx = a8 + v9c;
        if (nx >= a0) a0 = nx;
        cosv = data_02082214[j];
        pz = ((s64)v98 * cosv + 0x800) >> 12;
        *(s32*)((char*)c+0xa4) = (s32)pz;
        *(s32*)((char*)c+0xa8) = a0;
        *(s32*)((char*)c+0xac) = (s32)px;
        return;

// --- Actor::Next ---
struct Actor* Actor::Next(struct Actor* c){
  struct Actor* p;
  if(c) p = *(struct Actor**)((char*)c+0x54);
  else  p = *(struct Actor**)&data_0209b468;
  if(p) return *(struct Actor**)((char*)p+8);
  return 0;

// --- Actor::Spawn ---
ActorBase *Actor::Spawn(
    u32 actorID, u32 param1,
    const struct Vector3 *pos, const struct Vector3_16 *rot,
    s8 areaID, s16 deathTableID)
{
    func_02010e78(pos, rot, areaID, deathTableID);
    return _ZN12ActorDerived5SpawnEjP9ActorBaseii(actorID, gActorListHead, param1, 2);

// --- Actor::FindEgg ---
struct Actor* Actor::FindEgg(struct Actor* self, struct CylinderClsn* clsn) {
    u32 id = clsn->otherObjID;
    u32 flags;
    if (id != 0) {
        flags = clsn->hitFlags & 0x2000;
        if (flags != 0)
            return _ZN5Actor10FindWithIDEj(id);
    return 0;

// --- Actor::OnKicked ---
void Actor::OnKicked(struct Actor* self, struct Actor* kicker)
{

// --- Actor::OnPushed ---
void Actor::OnPushed(struct Actor* self, struct Actor* pusher)
{

// --- Actor::PoofDust ---
void Actor::PoofDust(struct Actor *thiz)
{
    struct Vector3 vec;
    vec.x = thiz->pos.x;
    vec.y = thiz->pos.y;
    vec.z = thiz->pos.z;
    _ZN5Actor10PoofDustAtERK7Vector3(thiz, &vec);

// --- Actor::SetRanges ---
void Actor::SetRanges(char*self,int a,int b,int c,int d){
*(int*)(self+0xb4)=a;
*(int*)(self+0xb8)=b>>3;
*(int*)(self+0xbc)=c>>3;
*(int*)(self+0xc0)=d>>3;

// --- Actor::TrackStar ---
int Actor::TrackStar(struct Actor *self, unsigned int a, unsigned int b)
{
    int v = b;
    if (b == 2) {
        if (a != data_0209f220[0]) {
            if (SublevelToLevel(data_0209f2f8[0]) <= 0xe)
                return -1;
        if (IsStarCollectedInCurLevel(a)) v = 3;
    signed char i;
    for (i = 0; i < 0xc; i++) {
        if (data_0209f40c[i] == 0) {
            SetStarMarker(i, (int)self, v);
            return i;
    return -1;

// --- Actor::UpdatePos ---
void Actor::UpdatePos(struct Actor* this_, struct CylinderClsn* clsn)
{
    _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(this_);
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(this_, clsn);

// --- Actor::Virtual50 ---
bool Actor::Virtual50(struct Actor* self)
{
    return 1; /* VS_FAIL */

// --- Actor::Actor ---


// --- Actor::Actor ---


// --- Actor::~Actor ---
struct Actor *Actor::~Actor(struct Actor *thiz)
{
    thiz->vtable = (void *)_ZTV5Actor;
    Actor_TeardownA(&Actor_TeardownGlobal, (char *)thiz + 0x50);
    Actor_TeardownB((char *)thiz + 0x50);
    thiz->vtable = (void *)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(thiz);
    Memory_Deallocate(thiz, Memory_gameHeapPtr);
    return thiz;

// --- Actor::~Actor ---
int Actor::~Actor(int c) {
  *(int*)c = (int)data_0208e3a4;
  func_0203b27c((int)data_0209b468, c+0x50);
  func_02044104(c+0x50);
  *(int*)c = (int)data_0208e4b8;
  _ZN9ActorBaseD2Ev(c);
  return c;

// --- Actor::~Actor ---
int Actor::~Actor(int c) {
  *(int*)c = (int)data_0208e3a4;
  func_0203b27c((int)data_0209b468, c+0x50);
  func_02044104(c+0x50);
  *(int*)c = (int)data_0208e4b8;
  _ZN9ActorBaseD2Ev(c);
  return c;
