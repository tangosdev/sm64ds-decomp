//cpp
// @symbol func_ov065_021177e4
/* NOT daYurei_Mucho_c's, and not a vtable slot. This file used to carry the
   coined name `Snufit_Kill` and the attribution "daYurei_Mucho_c::Kill --
   recovered from vtable slot identity". Three ROM reads refute both halves, so
   the promotion that retired the coined class name `Snufit` retired this
   spelling with it rather than transplanting it onto the ROM RTTI name:

     * 0x021177e4 appears in NO vtable in this overlay. Reading the 33 words at
       0x0211cb9c (_ZTV15daYurei_Mucho_c storage) and at 0x0211cc98
       (_ZTV5Swoop / _ZTV12daBasabasa_c storage) out of
       extracted/overlays/overlay_0065.bin, neither contains it.
     * The address sits inside the NEIGHBOUR's text run, between
       _ZN5SwoopD0Ev (0x02116fe8) and _ZN5Swoop16CleanupResourcesEv
       (0x02117aa4), whose factory is daBasabasa_c_classInit at 0x02117ee8.
       daYurei_Mucho_c's run ends at 0x02116f98.
     * The body types `t` against a layout daYurei_Mucho_c does not have: it
       calls ModelAnim::SetAnim on `t + 0x364`, where daYurei_Mucho_c holds a
       ShadowModel (its ModelAnim is at 0x300), and it reads
       data_ov065_0211d6a0, outside daYurei_Mucho_c's bss band
       0x0211d600..0x0211d690.

   Naming this function therefore belongs to the daBasabasa_c/Swoop owner, on
   that class's evidence. It keeps its address-derived symbol until then. */
struct Vector3 {
    int x, y, z;
    Vector3(int a, int b, int c) : x(a), y(b), z(c) {}
    ~Vector3() {}
};
typedef struct { int a, b; } P2;

extern "C" {
extern P2 data_ov065_0211d6a0;
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, int file, int frame, int speed, unsigned int flags);
extern void func_02012694(int id, void *pos);
extern int func_ov065_021177e4(int *t);
}

int func_ov065_021177e4(int *t)
{
    t[0x27] = -0x1000;
    t[0x28] = -0xa000;
    *((char *)t + 0x43c) = 0;
    t[0xf0] = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)t + 0x364, data_ov065_0211d6a0.b, 0, 0x1000, 0);
    Vector3 vec(t[0x17], t[0x18] + 0x64000, t[0x19]);
    *(short *)((char *)t + 0x100) = 0x19;
    if (t[0x18] == 0x898000)
        *(short *)((char *)t + 0x100) = 0x32;
    func_02012694(0xf9, (char *)t + 0x74);
    return 1;
}
