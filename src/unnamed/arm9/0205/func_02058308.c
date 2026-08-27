#include "types.h"
typedef struct {
    char pad[0x14];
    int slots[16];
} Mgr;

extern void func_00000000(void);
extern void func_00000600(void);

extern int data_020a6130;
extern Mgr data_020a6134;
extern char data_020a621c[];
extern char data_020a613c[];
extern void *data_020a612c;
extern char data_023c0000;
extern char data_020a6188[];
extern void func_02057e34(void);
extern char data_020a6378[];
extern u32 func_02057e48(u32 newVal);
extern void func_02058200(char *self, int a1, int a2, int end, int arg4, int arg5);

void func_02058308(void)
{
    int i;
    int ovr;
    int base;
    int end;

    if (data_020a6130 != 0) {
        return;
    }
    data_020a6130 = 1;

    for (i = 0; i < 16; i++) {
        data_020a6134.slots[i] = 0;
    }

    data_020a612c = data_020a613c;

    *(int *)(data_020a621c + 0x70) = 0x10;
    *(int *)(data_020a621c + 0x6c) = 0;
    *(int *)(data_020a621c + 0x64) = 1;
    *(int *)(data_020a621c + 0x68) = 0;
    *(int *)(data_020a621c + 0x74) = 0;

    *(char **)((char *)&data_020a6134 + 0x14) = data_020a621c;
    *(char **)((char *)&data_020a6134 + 0xc) = data_020a621c;
    *(char **)((char *)&data_020a6134 + 8) = data_020a621c;

    ovr = (s32)func_00000000;
    if (ovr == 0) {
        base = 0x23c0020 - ovr;
    } else {
        base = (u32)&data_023c0000 + 0x3fc0 - (s32)func_00000600 - ovr;
    }

    end = (u32)&data_023c0000 + 0x3fc0 - (s32)func_00000600;

    *(int *)(data_020a621c + 0x88) = end;
    *(int *)(data_020a621c + 0x84) = base;
    *(int *)(data_020a621c + 0x8c) = 0;
    *(int *)(end - 4) = (int)0xfddb597d;
    *(int *)(*(int *)(data_020a621c + 0x84)) = 0x7bf9dd5b;
    *(u16 *)(data_020a621c + 0x90) = 0;
    *(u16 *)((char *)&data_020a6134 + 2) = 0x10;
    *(u16 *)((char *)&data_020a6134 + 0) = 0;
    *(u16 *)((char *)&data_020a6134 + 4) = 0;
    *(char **)0x027fffa0 = (char *)&data_020a6134;

    func_02057e48(0);

    func_02058200(data_020a6188, (int)&func_02057e34, 0, (int)data_020a6378, 0xc8, 0x1f);

    *(int *)(data_020a6188 + 0x70) = 0x20;
    *(int *)(data_020a6188 + 0x64) = 1;
}
