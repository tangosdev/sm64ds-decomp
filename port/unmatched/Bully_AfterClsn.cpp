/* HOST COPY of src/func_ov064_02117220.cpp -- BULLY's AfterClsn extension
 * virtual (daDonketu_c::AfterClsn, vtable SLOT 32 -- this comment said 35 and
 * that was wrong; hal/actor_classes_ov064.cpp fills this body at vt[32], which
 * is also where PathLift carries the same extension virtual, and the slot is
 * what its pop contract is read from), the coin pop on the
 * knocked-into-lava path plus the mole-style group census bump. The matched
 * .cpp cannot be compiled verbatim on the MSVC host: after declaring its
 * local `struct Actor` shadow with the two statics, it RE-declares both at
 * namespace scope (`Actor* Actor::Spawn(...);`), an out-of-class member
 * redeclaration mwccarm accepts and MSVC rejects (C2761).
 *
 * This is the port/unmatched/ host-copy pattern: the body is the matched
 * source's line for line, with only the two redundant redeclaration lines
 * dropped. The Actor::Spawn id 0x120 = 288 = COIN, hosted since gate 33;
 * both results are null-checked.
 */
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };
struct Actor {
    static Actor *Spawn(unsigned a, unsigned b, const Vector3 &pos,
                        const Vector3_16 *rot, int e, int f);
    static Actor *FindWithID(unsigned id);
};
extern "C" {
extern int RandomIntInternal(void *seed);
extern int data_0209e650;
extern int func_ov064_0211616c(void *thiz);
}

/* the shadow's static spellings onto the hosted C bodies; the Spawn one
   already exists twice (actor_faces_bob / bob_enemy_bridges), FindWithID's
   is new */
#pragma comment(linker, "/alternatename:?Spawn@Actor@@SAPAU1@IIABUVector3@@PBUVector3_16@@HH@Z=__ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii")
#pragma comment(linker, "/alternatename:?FindWithID@Actor@@SAPAU1@I@Z=__ZN5Actor10FindWithIDEj")

/* PORT_HOST_ABI: MSVC C2761 rejects the matched TU's out-of-class member
   redeclarations; body is the matched source's line for line. */
extern "C" void func_ov064_02117220(char *self)
{
    if (func_ov064_0211616c(self) == 0) return;
    int pz = *(int *)(self + 0x64);
    int py = *(int *)(self + 0x60) + 0x136000;
    int px = *(int *)(self + 0x5c);
    Vector3 pos;
    pos.x = px;
    pos.y = py;
    pos.z = pz;
    int r = RandomIntInternal(&data_0209e650);
    Vector3_16 rot;
    s16 ang = (s16)(*(s16 *)(self + 0x94) + 0x8000 + (((unsigned)r >> 8) & 0x3ff));
    rot.x = 0; rot.z = 0; rot.y = ang;
    Actor *a = Actor::Spawn(0x120, 2, pos, &rot, *(s8 *)(self + 0xcc), -1);
    if (a) {
        *(int *)((char *)a + 0x98) = 0xa000;
        *(int *)((char *)a + 0xa4) = 0;
        *(int *)((char *)a + 0xa8) = 0x50000;
        *(int *)((char *)a + 0xac) = 0;
    }
    Actor *f = Actor::FindWithID(*(unsigned *)(self + 0x3fc));
    if (f) {
        u8 *p = (u8 *)(((long long)(int)((char *)f + 0x3fe)));
        *p = (u8)(*p + 1);
    }
}
