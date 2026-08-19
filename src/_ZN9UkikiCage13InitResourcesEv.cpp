//cpp
// @symbol _ZN9UkikiCage13InitResourcesEv
/* recovered: shared common types, renamed to Class_Method
 *
 * UkikiCage::InitResources -- vtable slot 0, ov030 0x02111410. Attributed by
 * the vtable: _ZTV9UkikiCage (ov030 0x02115974) carries this address at slot
 * 0, where _ZTV10dBgActor_c carries fBase_c's generic 0x02043c80.
 *
 * Kept as a free function under the literal mangled name rather than a real
 * `UkikiCage::InitResources()` method: `self` arrives as a raw pointer and
 * the body still reads/writes by hand offset, unconverted -- the same shape
 * include/PushBlock.h's InitResources file documents. */
typedef int Fix12i;
typedef short s16;
struct SharedFilePtr { int x; }; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
struct V3 { Fix12i x, y, z; };
extern "C" {
struct BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, struct BMD_File *f, int a, int b);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int a, int b, void* vr, int e);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
struct KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(struct SharedFilePtr &f);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, struct KCL_File *k, struct Matrix4x3 &m, int fx, short s, struct CLPS_Block &c);
int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, struct V3*, void*, int, int);
extern struct SharedFilePtr data_ov030_02115c88;
extern struct SharedFilePtr data_ov030_02115c80;
extern struct CLPS_Block data_ov030_02114ee4;

int _ZN9UkikiCage13InitResourcesEv(char *self){
    struct BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov030_02115c88);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);
    *(int*)(self + 0x9c) = 0;
    *(int*)(self + 0xa0) = 0;
    *(int*)(self + 0x98) = 0;
    *(int*)(self + 0x80) = 0x1000;
    *(int*)(self + 0x84) = 0x1000;
    *(int*)(self + 0x88) = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(self + 0x320, self, 0x32000, 0x32000, 0, 0);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);
    {
        struct KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov030_02115c80);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x124, kcl, *(struct Matrix4x3 *)(self + 0x2ec), 0x199,
            *(s16 *)(self + 0x8e), data_ov030_02114ee4);
    }
    {
        struct V3 v;
        int y;
        v.x = *(int*)(self + 0x5c);
        y = *(int*)(self + 0x60);
        v.y = y;
        v.z = *(int*)(self + 0x64);
        v.y = y + 0x3c000;
        *(int*)(self + 0x4dc) = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0xb2, (unsigned char)(*(int*)(self + 8) & 0xf) | 0x50, &v, 0,
            *(signed char*)(self + 0xcc), -1);
    }
    return 1;
}
}
