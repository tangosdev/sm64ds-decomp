/* HOST COPY of src/_Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij.cpp
 * -- one more ARM argument ride-through. DORMANT on every level the port
 * mounts today; it is here so it stays that way.
 *
 * The matched TU ends
 *
 *     if (data_ov002_0210cb5c[sl] < 0)
 *         return;
 *     StartEntranceFaderWipe();          <- declared void(void)
 *
 * and src/engine/fader/StartEntranceFaderWipe.cpp defines that callee as
 * `void StartEntranceFaderWipe(int index)`: it computes &WIPES[index] and
 * installs it as the scene's fader. The ROM does pass the index. At
 * 0x020fe850, two instructions ahead of the bl:
 *
 *     ldr   r0, [pc, #0x50]     ; &data_ov002_0210cb5c
 *     cmp   sl, #0x13
 *     movhs sl, #0
 *     ldrsb r0, [r0, sl]        ; r0 = table[sl], the per-entrance wipe type
 *     cmp   r0, #0
 *     bxlt  lr                  ; the `< 0` return
 *     bl    #0x20298a0          ; StartEntranceFaderWipe, r0 untouched
 *
 * so the value the `< 0` test loads IS the argument. On a host cdecl call
 * the callee would instead read a stack slot nobody wrote and index WIPES
 * with it -- &WIPES[garbage] installed as data_0209f5bc, which HUD::Behavior
 * dispatches IsAtStart through every frame.
 *
 * WHAT THIS FILE DID NOT FIX. It was written for the level-2 door-entrance
 * fault of 2026-08-05, on the theory that the garbage index was installing a
 * bad fader. It was not: SM64DS_FADER_WATCH=1 says all four mounted levels
 * (1, 2, 6, 7) take the `< 0` return -- table[0] and table[10] are both -1 --
 * so this call never fires and the fader stays hal_wipes[0]. The real stomper
 * was an undersized host global, see hal/auto_bss.cpp. Kept anyway, on the
 * same footing as the other dormant ride-throughs in LEVELBOOT_HOST_SOURCES:
 * the register form is real, and the first entrance mounted with a
 * non-negative wipe type would land on it.
 *
 * Same body, argument spelled. LoadEntranceObjects is the only caller of
 * StartEntranceFaderWipe in the whole tree, so nothing else depends on the
 * register form. The rest of the function is byte-for-byte the matched
 * TU's logic, C++-ified only where C89 spellings do not compile here
 * (enum Bool -> int).
 */
#include <cstdio>
#include <cstdlib>

typedef int Fix12i;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
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

extern "C" {
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
void* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 flags, const Vector3* pos, const Vector3_16* rot, int area, int death);
void* _ZN7dBase_c5SpawnEjP7fBase_cii(u32 id, void* base, int a, int b);
void StartEntranceFaderWipe(int index);

/* PORT_HOST_ABI: ARM argument ride-through into StartEntranceFaderWipe,
 * which the matched TU declares void(void). See the header.
 *
 * RE-TESTED AND STILL HOLDING, run link100 wave 15, lane SEAT15E, against
 * src/ at 8ddff3187. The LINK15 census (out/LINK15/rows.tsv) put this row in
 * BATCH 4 as tract A with the T4 note "RIDE-THROUGH CLAIM STALE: every callee
 * the src declares now takes a parameter". THAT IS WRONG FOR THIS ROW, and it
 * is wrong on the one callee the banner is about. Read today, the matched TU
 * src/_Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij.cpp still says
 *
 *     :34    void StartEntranceFaderWipe(void);
 *     :100   StartEntranceFaderWipe();
 *
 * while src/engine/fader/StartEntranceFaderWipe.cpp:41 still defines
 * `extern "C" void StartEntranceFaderWipe(int index)`. The ROM still leaves
 * the wipe type the `< 0` test just loaded in r0 across the bl at 0x020fe850,
 * so the defect this file exists for is unchanged and a host cdecl call would
 * still index WIPES with an unwritten stack slot. LoadEntranceObjects is the
 * only caller of StartEntranceFaderWipe in the tree, so nothing else can make
 * the row true from the other side either.
 *
 * The row is therefore a STANDING host-ABI exception, not replacement work,
 * and this tag is the ruling. It raises no linkage count. The row retires when
 * the matched TU's own declaration carries the parameter -- a decomp-side
 * change under the byte gate, the same shape as the two ov007 seams this lane
 * did seat. */
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
            if (data_0209f2d8 == 1) {
                f2 = 3;
                f1 = 3;
            }
            u32 flags = f2 | (f1 << 3) | (i << 6) | (sl << 8);

            void* a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
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

    data_0209f318 = _ZN7dBase_c5SpawnEjP7fBase_cii(0x14c, data_0209f5c0, entranceId, 0);

    if (std::getenv("SM64DS_FADER_WATCH")) {
        u32 c = sl >= 0x13 ? 0 : sl;
        std::fprintf(stderr, "[fwatch] entrance: mode %d, slot %u, "
                     "wipe table[%u] = %d\n", (int)data_0209f2d8,
                     (unsigned)sl, (unsigned)c,
                     (int)data_ov002_0210cb5c[c]);
    }
    if (data_0209f2d8 == 2)
        return;

    if (sl >= 0x13)
        sl = 0;
    if (data_ov002_0210cb5c[sl] < 0)
        return;

    /* THE RIDE-THROUGH, SPELLED: the wipe type the ROM leaves in r0 is the
       table value the test above just read. SM64DS_FADER_WATCH=1 names it,
       because the whole reason this file exists is that one argument -- and
       an entrance whose table value is 0 looks exactly like the unspelled
       call on any host build where the stack slot happens to hold 0. */
    int wipe = data_ov002_0210cb5c[sl];
    if (std::getenv("SM64DS_FADER_WATCH"))
        std::fprintf(stderr, "[fwatch] entrance slot %u -> wipe %d\n",
                     (unsigned)sl, wipe);
    StartEntranceFaderWipe(wipe);
}
}  /* extern "C" */
