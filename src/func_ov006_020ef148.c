#include "types.h"
// @symbol func_ov006_020ef148
// recovered name: dScMgJump2_c_Render
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* dScMgJump2_c::Render - recovered from vtable slot identity */
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern int GetGameLanguage(void);
extern void DrawOamSprite(void* a0, void* a1, int a2, void* a3);
extern void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);

extern int data_020a0e68;

struct M48 { int w[12]; };
struct S3 { int a, b, c; };

int func_ov006_020ef148(char* self)
{
    struct S3 local;
    void* p;
    int count;
    int a1v;
    int i;

    func_ov004_020b1a5c(data_ov006_02140308, 4);
    func_ov006_020eef58();
    func_ov006_020ef2b8();
    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0xffead000, -0xa0000);
    *(struct M48*)(self + 0x5a30) = *(struct M48*)&data_020a0e68;

    local = *(struct S3*)&data_ov006_0213ccb0;

    {
        int* obj = (int*)(self + 0x5a1c);
        p = (void*)obj[1];
        func_02045e44((void*)obj, 1, 0);
    }
    *(int*)((char*)p + 0x18) = *(int*)(self + 0x5a64);
    {
        void** vobj = (void**)(self + 0x5a14);
        (*(void(**)(void*, void*))((char*)*vobj + 0x14))((void*)vobj, &local);
    }

    func_ov006_020c70d0();
    func_ov006_020c425c();

    if (*(u16*)(self + 0x4664) == 1) {
        a1v = 0x6e;
        count = data_ov006_02140428;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void*)data_ov006_0213cccc[idx][1], (void*)a1v, 0xc, (void*)0);
            } else {
                RenderOamMainScreen(data_ov006_02134cf8, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    return 1;
}
