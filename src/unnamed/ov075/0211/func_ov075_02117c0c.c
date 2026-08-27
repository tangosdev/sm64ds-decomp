#include "types.h"
extern int data_ov075_0211d8e8;
extern int data_ov075_0211d930;
extern int data_ov075_0211d888;
extern int data_ov075_0211d810;

extern int func_0203da9c(void);
extern int TouchArea_Update(void* c, int id);
extern int func_ov075_02116d9c(int unused, int id);
extern void func_ov001_020ab41c(void* c);
extern int func_0203d9b4(void);
extern void func_ov075_0211a148(char* c, int a, int b);
extern void func_02030a78(void);
extern void func_02012790(int x);
extern void func_02030aa4(int arg);

void func_ov075_02117c0c(char* self)
{
    unsigned dok;
    int id;

    id = func_0203da9c();
    if (TouchArea_Update(self + 0x70, id) != 0 ||
        (func_ov075_02116d9c((int)self, id) != 0 && *(u8*)(self + 0x281) == 0)) {
        *(u8*)(self + 0xa4) = 0;
        func_ov001_020ab41c(self + 0x70);
        if (func_0203d9b4() == 0) goto DIFF_ELSE;
        dok = (*(u16*)(self + 0xc) == 6);
        if (!dok) goto DIFF_ELSE;
        func_ov075_0211a148(self, (int)&data_ov075_0211d8e8, 0x14);
        goto DIFF_DONE;
DIFF_ELSE:
        func_02030a78();
        func_ov075_0211a148(self, (int)&data_ov075_0211d930, 0x14);
DIFF_DONE:
        func_02012790(0x63);
        return;
    }
    if (TouchArea_Update(self + 0x94, id) == 0) {
        if (func_ov075_02116d9c((int)self, id) == 0)
            return;
        if (*(u8*)(self + 0x281) != 1)
            return;
    }
    *(u8*)(self + 0x80) = 0;
    func_ov001_020ab41c(self + 0x94);
    dok = (*(u16*)(self + 0xc) == 6);
    if (dok) {
        func_ov075_0211a148(self, (int)&data_ov075_0211d888, 0x14);
        func_02030aa4(3);
    } else {
        func_ov075_0211a148(self, (int)&data_ov075_0211d810, 0x14);
    }
    func_02012790(0x98);
}
