//cpp
/* dMg3DEspAnimSet_c, span 0x020e777c..0x020e7b0c.
 * #pragma defer_codegen off: the nine functions are in ROM order.
 * dMg3DEspModel_c stays the next file (ResetTransform at 0x020e7b0c).
 */

#pragma defer_codegen off
#pragma opt_strength_reduction off

#include "common.h"
#include "dMg3DEspAnimSet_c.h"
#include "SharedFilePtr.h"

struct ModelComponents;
struct Obj {
    void* vt;
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4();
    virtual void m(void* arg);
};

extern "C" {
extern Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
int _Z15ApproachLinear2Rsss(short* p, short a, short b);
void _ZN9Animation7AdvanceEv(void* a);
int _ZN9Animation8FinishedEv(void* a);
int _ZN5Model8LoadFileER13SharedFilePtr(void *p);
int _ZN9Animation8LoadFileER13SharedFilePtr(void *p);
void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(int bmd, void *bma);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(char *t, int f, int a, int b);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *t, int f, int a, int b, unsigned u);
void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(char *t, void *f, int a, int b, unsigned u);
void _ZN5Model12SetPolygonIDEi(char *t, int id);
extern int data_ov006_02141e94;
extern int data_ov006_02141e6c;
extern int data_ov006_0213c7f4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN17dMg3DEspAnimSet_c6RenderEv, 0x020e777c, size 0x9c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMg3DEspAnimSet_c6RenderEv
void dMg3DEspAnimSet_c::Render()
{
    char *c = (char *)this;
    int i;
    char *r5;
    char *r4;
    void *zero;
    Matrix4x3_FromTranslation(&data_020a0e68, 0x8c000, 0x80000, 0x40000);
    r5 = c;
    r4 = c + 0x12c;
    i = 0;
    zero = (void*)i;
    do {
        if (*(int*)(c + i*4 + 0x168) != 0) {
            *(Matrix4x3*)(r5 + 0x1c) = data_020a0e68;
            ((MaterialChanger*)r4)->Update(*(ModelComponents*)(r5 + 8));
            ((Obj*)r5)->m(zero);
        }
        i++;
        r5 += 0x64;
        r4 += 0x14;
    } while (i < 3);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN17dMg3DEspAnimSet_c8BehaviorEv, 0x020e7818, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMg3DEspAnimSet_c8BehaviorEv
void dMg3DEspAnimSet_c::Behavior()
{
    char *self = (char *)this;
    int sb = 0;
    if (*(short*)(self + 0x178) != 0) {
        if (_Z15ApproachLinear2Rsss((short*)(self + 0x17a), 0, 1)) {
            *(short*)(self + 0x17a) = *(short*)(self + 0x17c);
            sb = 1;
            if (*(short*)(self + 0x178) > 0) {
                *(short*)(int)(self + 0x178) -= 1;
            }
        }
    }
    {
        int zero = 0;
        int i;
        char* a = self;
        char* b = self + 0x12c;
        char* cc = self;
        for (i = 0; i < 3; i++) {
            if (((int*)(self + 0x168))[i] != 0) {
                _ZN9Animation7AdvanceEv(a + 0x50);
                _ZN9Animation7AdvanceEv(b);
                if (_ZN9Animation8FinishedEv(a + 0x50)) {
                    ((int*)(self + 0x168))[i] = 0;
                }
            } else {
                if (sb == 1) {
                    ((int*)(self + 0x168))[i] = 1;
                    *(int*)(a + 0x58) = zero;
                    sb = zero;
                    *(int*)(cc + 0x134) = zero;
                }
            }
            a += 0x64;
            b += 0x14;
            cc += 0x14;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN17dMg3DEspAnimSet_c5ResetEv, 0x020e7910, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMg3DEspAnimSet_c5ResetEv
void dMg3DEspAnimSet_c::Reset()
{
    unk_178 = 0;
    unk_17c = 30;
    unk_17a = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020e792c, 0x020e792c, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e792c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e792c(char* c, short v){
  *(short*)(c+0x178)=v;
  *(short*)(c+0x17a)=0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_020e7940, 0x020e7940, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e7940
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e7940(char *p, short v)
{
    *(short *)(p + 0x17c) = v;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020e794c, 0x020e794c, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e794c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e794c(int *p, int v)
{
    p[93] = v;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN17dMg3DEspAnimSet_c13InitResourcesEv, 0x020e7954, size 0xfc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMg3DEspAnimSet_c13InitResourcesEv
void dMg3DEspAnimSet_c::InitResources()
{
    char *o = (char *)this;
    int bca;
    int m1 = -1;
    int z1 = 0;
    int z2 = 0;
    int bmd;
    int i;
    char *w;
    char *w2;
    *(int *)(o + 0x174) = 0x800;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov006_02141e94);
    bca = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e6c);
    _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(bmd, &data_ov006_0213c7f4);
    w = o;
    w2 = o + 0x12c;
    for (i = 0; i < 3; i++) {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(w, bmd, 1, m1);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(w, bca, 0x40000000, *(int *)(o + 0x174), z1);
        _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(w2, &data_ov006_0213c7f4, 0x40000000, *(int *)(o + 0x174), z2);
        _ZN5Model12SetPolygonIDEi(w, (i + 1) & 0xff);
        *(int *)(o + i * 4 + 0x168) = 0;
        w += 0x64;
        w2 += 0x14;
    }
    *(u16 *)(o + 0x178) = 0;
    *(u16 *)(o + 0x17a) = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN17dMg3DEspAnimSet_cD1Ev, 0x020e7a50, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMg3DEspAnimSet_cD1Ev
dMg3DEspAnimSet_c::~dMg3DEspAnimSet_c()
{
  ((SharedFilePtr *)(&data_ov006_02141e94))->Release();
  ((SharedFilePtr *)(&data_ov006_02141e6c))->Release();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN17dMg3DEspAnimSet_cC1Ev, 0x020e7aac, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN17dMg3DEspAnimSet_cC1Ev
dMg3DEspAnimSet_c::dMg3DEspAnimSet_c() {}
