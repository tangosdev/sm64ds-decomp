//cpp
struct Node { char pad[8]; unsigned short f8; };

struct O {
    char pad0[0x28];
    void** f28;      /* 0x28 */
    void** f2c;      /* 0x2c */
    char pad30[4];
    void*** f34;     /* 0x34 */
    Node** f38;      /* 0x38 */
    void** f3c;      /* 0x3c */
    void** f40;      /* 0x40 */
    void** f44;      /* 0x44 */
};

extern "C" {
extern void func_ov007_020c78dc(void* o);
extern void func_ov007_020c3d1c(void* o);
extern void func_ov007_020c8098(void* o);
extern void func_ov007_020c2bf8(void* o);
}

extern "C" void func_ov007_020c6d90(struct O* c, int idx)
{
    if (c->f38[idx] == 0) return;
    func_ov007_020c78dc(c->f3c[idx]);
    func_ov007_020c3d1c(c->f28[idx]);
    func_ov007_020c3d1c(c->f2c[idx]);
    {
        int i;
        for (i = 0; i < c->f38[idx]->f8; i++)
            func_ov007_020c8098(c->f34[idx][i]);
    }
    func_ov007_020c2bf8(c->f38[idx]);
    func_ov007_020c3d1c(c->f34[idx]);
    func_ov007_020c8098(c->f40[idx]);
    func_ov007_020c8098(c->f44[idx]);
    c->f38[idx] = 0;
}
