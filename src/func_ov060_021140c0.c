typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct { s32 x, y, z; } Vector3;

extern int _ZNK9Animation12WillHitFrameEi(void* anim, int frame);
extern void _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
    void* self, const void* pos, const void* rot, int speed, int gravity, u32 flags);
extern int _ZN6Player9GetHealthEv(void* player);
extern int RandomIntInternal(int* seed);
extern void func_02012694(int id, void* pos);
extern int Bowser_IsAnimAtLastFrame(void* self);
extern void func_ov060_02111cc0(void* self, int a, int b);
extern int data_0209e650;
extern s16 data_02082214[];
extern int data_ov060_0211abe0[];

void func_ov060_021140c0(char* c)
{
    int new_var;
    if (*(u16*)((c + 0x300) + 0xfc) == 0) {
        void* player = *(void**)(c + 0x3a0);
        if (player == 0 || _ZN6Player9GetHealthEv(player) > 4) {
            s32 rnd = RandomIntInternal(&data_0209e650);
            u32 hi = ((u32)rnd) >> 16;
            u32 m = hi % 10u;
            c[0x428] = (char)((m % 3u) + 1);
        } else {
            c[0x428] = 3;
        }
    }
    if (*(int*)(c + 0x134) == data_ov060_0211abe0[1]) {
        if (_ZNK9Animation12WillHitFrameEi(c + 0x124, 5)) {
            Vector3 pos;
            u16 dir[3];
            u16 ax, ay, az;
            s16* tbl;
            s16 scale;
            int grav;
            /* `dp` must be a live pointer here, not a folded `&dir` at the call: the
               address has to be materialised before the two smlabb steps so it holds
               r2 across them, which is what pushes pos.x to r3 and `scale` to ip. */
            u16* dp;

            pos.x = *(int*)(c + 0x5c);
            tbl = data_02082214;
            pos.y = *(int*)(c + 0x60);
            scale = 0xe8;
            pos.z = *(int*)(c + 0x64);
            ax = *(u16*)(c + 0x8c);
            ay = *(u16*)(c + 0x8e);
            dir[1] = ay;
            dir[0] = ax;
            az = *(u16*)(c + 0x90);
            dir[2] = az;
            pos.x = (tbl[(dir[1] >> 4) * 2] * scale) + pos.x;
            new_var = pos.y + 0x58000;
            dp = dir;
            pos.z = (tbl[((dp[1] >> 4) * 2) + 1] * scale) + pos.z;
            /* Scheduling barrier, not dead code. Both operands are provably non-null,
               so this changes nothing at runtime, but the short-circuit `&&` splits the
               block and stops the scheduler hoisting the 0xa000 constant into the slot
               the ROM gives to `dp`. Without it this function is 11 words off. */
            if (dp != 0 && c != 0) {
            }
            pos.y = new_var;
            dir[0] = 0x1000;
            grav = 0xa000;
            _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
                c, &pos, dp, 0x1e000, grav, 0);
            func_02012694(0x122, c + 0x74);
        }
    }
    if (Bowser_IsAnimAtLastFrame(c) != 0) {
        if (*(int*)(c + 0x134) == data_ov060_0211abe0[1]) {
            unsigned char* p = (unsigned char*)(c + 0x423);
            *p = (*p) + 1;
            if (*(unsigned char*)(c + 0x423) >= *(unsigned char*)(c + 0x428)) {
                *(int*)(c + 0x40c) = 0;
            }
        } else {
            func_ov060_02111cc0(c, 0x14, 0);
        }
        *(int*)(c + 0x12c) = 0;
    }
}
