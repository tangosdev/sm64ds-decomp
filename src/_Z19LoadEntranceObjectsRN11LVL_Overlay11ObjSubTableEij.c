enum Bool { false = 0, true = 1 };
typedef int Fix12i;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

typedef struct { Fix12i x, y, z; } Vector3;
typedef struct { s16 x, y, z; } Vector3_16;

struct Entry {
    u16 raw;        /* 0 */
    s16 x;          /* 2 */
    s16 y;          /* 4 */
    s16 z;          /* 6 */
    Vector3_16 rot; /* 8 */
    u16 param;      /* 0xe */
};

struct ObjSubTable {
    u8 pad0;
    u8 count;
    u8 pad2[2];
    struct Entry* entries;
};

extern u8 data_0209f21c;
extern u8 data_0209caa0[];
extern u8 data_0209f2d8;
extern u8 data_02092128[];
extern u16 data_ov002_0210cbf4[];
extern u8 data_0209fc5c[];
extern void* data_0209f394[];
extern u8 data_0209f250;
extern void* data_0209f5c0;
extern void* data_0209f318;
extern signed char data_ov002_0210cb5c[];

void func_0202b0e0(struct Entry* e, int count);
void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 flags, const Vector3* pos, const Vector3_16* rot, s32 area, s32 death);
void* _ZN12ActorDerived5SpawnEjP9ActorBaseii(u32 id, void* base, int a, int b);
void StartEntranceFaderWipe(void);

void _Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij(struct ObjSubTable* tbl, int p2, u32 p3)
{
    u32 sl;
    struct Entry* e = tbl->entries;
    func_0202b0e0(e, tbl->count);
    e += p3;
    sl = 0;

    int entranceId = 0xf;
    u8 i = 0;

    while (i < data_0209f21c) {
        {
            Vector3 pos;
            int vz = e->z << 12;
            int vy = e->y << 12;
            int vx = e->x << 12;
            pos.x = vx;
            pos.y = vy;
            pos.z = vz;

            u16 param = e->param;
            sl = (param >> 7) & 0xf;

            int f2 = data_0209caa0[0x41];
            int f1 = data_02092128[i];
            enum Bool cond = (enum Bool)(data_0209f2d8 == 1);
            if (cond != false) {
                f2 = 3;
                f1 = 3;
            }
            u32 flags = f2 | (f1 << 3) | (i << 6) | (sl << 8);

            void* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                data_ov002_0210cbf4[e->raw], flags, &pos, &e->rot,
                (signed char)(param & 7), -1);

            if (data_0209fc5c[i] == 0)
                data_0209f394[i] = 0;
            else
                data_0209f394[i] = a;

            if (i == data_0209f250)
                entranceId = (e->param >> 3) & 0xf;

            e++;
            i++;
        }
    }

    data_0209f318 = _ZN12ActorDerived5SpawnEjP9ActorBaseii(0x14c, data_0209f5c0, entranceId, 0);

    int t = data_0209f2d8;
    t = t == 2;
    if (t != false)
        return;

    if (sl >= 0x13)
        sl = 0;
    if (data_ov002_0210cb5c[sl] < 0)
        return;

    StartEntranceFaderWipe();
}
