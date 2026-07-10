/* func_ov007_020b7708 — lazily create particle system slot idx in the table
 * at data_ov007_02103450 via Particle::Manager::AddSystem(data_ov007_0210344c,
 * 0, pos), then set flag bit1 (+0x1c) on the new system.
 */

typedef unsigned int u32;

struct PtclSys {
    char _pad0[0x1c];
    u32 flags;            /* 0x1c */
};

extern struct PtclSys* data_ov007_02103450[];
extern void* data_ov007_0210344c;

extern struct PtclSys* _ZN8Particle7Manager9AddSystemEiR7Vector3(void* mgr, int id, void* pos);

void func_ov007_020b7708(int idx, void* pos)
{
    if (data_ov007_02103450[idx] == 0) {
        struct PtclSys* s;
        data_ov007_02103450[idx] = _ZN8Particle7Manager9AddSystemEiR7Vector3(data_ov007_0210344c, 0, pos);
        s = data_ov007_02103450[idx];
        *(u32*)(((long long)(int)((char*)s + 0x1c)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
    }
}
