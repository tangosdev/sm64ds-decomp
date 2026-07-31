//cpp
// @symbol _ZN8MugenBgm13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MugenBgm.h"
typedef struct Obj Obj;
struct Obj {
    char pad0[8];
    unsigned int unk8;
    char pad_c[0x74];
    int unk80;
    int unk84;
    int unk88;
    char pad_8c[0x50];
    void *unkDC;
    void *unkE0;
};

extern void *_Znwj(int sz);

extern unsigned char data_0209f2d8;
extern char data_ov002_0211094c;
extern char data_ov085_0213074c;

int MugenBgm::InitResources()
{
    void *p;
    int t;

    if (((Obj *)this)->unk8 == 0x12) {
        p = _Znwj(0x84);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x64);
            _ZN9ModelAnimC2Ev(p);
            *(void **)p = &data_ov002_0210bcc4;
            *(void **)((char *)p + 0x50) = &data_ov002_0210bce8;
        }
        ((Obj *)this)->unkE0 = p;
        if (((Obj *)this)->unkE0 == 0) return 0;
        if (func_ov002_020f6618(((Obj *)this)->unkE0, &data_ov085_0213074c, 1, &data_ov002_0210b60c, 1, 1, &data_ov002_0210b608, -1) == 0) return 0;
    } else if (((Obj *)this)->unk8 == 0x13) {
        p = _Znwj(0x84);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x64);
            _ZN9ModelAnimC2Ev(p);
            *(void **)p = &data_ov002_0210bcc4;
            *(void **)((char *)p + 0x50) = &data_ov002_0210bce8;
        }
        ((Obj *)this)->unkE0 = p;
        if (((Obj *)this)->unkE0 == 0) return 0;
        t = (*(volatile unsigned char *)&data_0209f2d8 == 2);
        if (t == 0) {
            if (func_ov002_020f6618(((Obj *)this)->unkE0, &data_ov002_02110b98, 1, &data_ov002_0210b610, 1, 1, &data_ov002_0210b600, 0x16) == 0) return 0;
        } else {
            if (func_ov002_020f6618(((Obj *)this)->unkE0, &data_ov002_02110c18, 0xD, &data_ov002_0210bcf0, 1, 0xD, &data_ov002_0210bd24, 0x16) == 0) return 0;
        }
    } else if (((Obj *)this)->unk8 >= 0x14 && ((Obj *)this)->unk8 <= 0x16) {
        p = _Znwj(0x60);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x50);
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        ((Obj *)this)->unkDC = p;
        if (((Obj *)this)->unkDC == 0) return 0;
        if (func_ov002_020f6960(((Obj *)this)->unkDC, &data_ov002_02110b70, -1) == 0) return 0;
    } else if (((Obj *)this)->unk8 == 0x17) {
        p = _Znwj(0x60);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x50);
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        ((Obj *)this)->unkDC = p;
        if (((Obj *)this)->unkDC == 0) return 0;
        if (func_ov002_020f6960(((Obj *)this)->unkDC, &data_ov002_02110b50, 0x19) == 0) return 0;
    } else if (((Obj *)this)->unk8 == 0x18) {
        p = _Znwj(0x60);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x50);
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        ((Obj *)this)->unkDC = p;
        if (((Obj *)this)->unkDC == 0) return 0;
        if (func_ov002_020f6960(((Obj *)this)->unkDC, &data_ov002_0211094c, -1) == 0) return 0;
    } else if (((Obj *)this)->unk8 == 0x19) {
        p = _Znwj(0x60);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x50);
            _ZN5ModelC2Ev(p);
            *(void **)p = &data_ov002_0210bae4;
        }
        ((Obj *)this)->unkDC = p;
        if (((Obj *)this)->unkDC == 0) return 0;
        if (func_ov002_020f6960(((Obj *)this)->unkDC, &data_ov002_02110b78, 0x13) == 0) return 0;
    } else if (((Obj *)this)->unk8 >= 0x1A && ((Obj *)this)->unk8 <= 0x2D) {
        p = _Znwj(0x84);
        if (p) {
            func_ov002_020f6a50((char *)p + 0x64);
            _ZN9ModelAnimC2Ev(p);
            *(void **)p = &data_ov002_0210bcc4;
            *(void **)((char *)p + 0x50) = &data_ov002_0210bce8;
        }
        ((Obj *)this)->unkE0 = p;
        if (((Obj *)this)->unkE0 == 0) return 0;
        if (func_ov002_020f6618(((Obj *)this)->unkE0, &func_02113c20, 1, &data_ov002_0210b604, 0, 0, 0, -1) == 0) return 0;
    }
    ((Obj *)this)->unk80 = 0x1000;
    ((Obj *)this)->unk84 = 0x1000;
    ((Obj *)this)->unk88 = 0x1000;
    return 1;
}
