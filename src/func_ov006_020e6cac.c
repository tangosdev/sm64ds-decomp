// @symbol func_ov006_020e6cac
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgTrampoline2_c.h"
// recovered name: dScMgTrampoline2_c_OnAimedAtWithEggReturnVec
/* recovered: renamed to Class_Method */
/* dScMgTrampoline2_c::OnAimedAtWithEggReturnVec - recovered from vtable slot identity */
void _ZN2GX15SetBankForSubBGEt(unsigned int);
void _ZN2GX16SetBankForSubOBJEt(unsigned int);
void func_ov006_020e740c(void);
void func_ov004_020aeed8(void *this);
void func_ov006_020e7508(void);
void func_ov006_020e759c(void);
extern unsigned char data_0209e660;
extern unsigned char data_0209f5f8;

void func_ov006_020e6cac(char *this) {
    struct dScMgTrampoline2_c *self = (struct dScMgTrampoline2_c *)(void *)this;
    _ZN2GX15SetBankForSubBGEt(self->unk_0a0);
    _ZN2GX16SetBankForSubOBJEt(self->unk_4660);
    data_0209e660 = 1;
    func_ov006_020e740c();
    func_ov004_020aeed8(this);
    if (self->unk_4664 == 1) {
        func_ov006_020e7508();
    } else {
        func_ov006_020e759c();
        if (data_0209f5f8 == 0) data_0209f5f8 = 1;
    }
}
