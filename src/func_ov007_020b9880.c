#pragma opt_strength_reduction off

typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;

extern void *func_ov007_020c3df4(int a, int b);
extern void *func_ov007_020c690c(int a0, int a1, int a2, int a3, void *a4, void *a5);
extern void *func_ov007_020c9388(int a);
extern void *func_ov007_020c908c(void *r5);
extern void *func_ov007_020c44e4(int a);
extern void func_ov007_020c43bc(void *a, int b);
extern void func_ov007_020c4388(char *r0, int r1);
extern void func_ov007_020bbe60(int idx);

extern char *data_ov007_02104ba0;
extern char *data_ov007_02104b9c;
extern char *data_ov007_0210342c;
extern int data_ov007_020d7b60;
extern int data_ov007_020d7b80;

void func_ov007_020b9880(int a0, int a1)
{
    int sp14, sp18, sp10, sp1c;
    int var_sl;
    void *temp_sb;
    void *temp_r4;
    void *temp_r5;

    if (data_ov007_02104ba0 != 0)
        return;

    data_ov007_02104ba0 = (char *)func_ov007_020c3df4(0, 0x4c);
    data_ov007_02104b9c = (char *)func_ov007_020c3df4(0, 0x14);

    *(void **)(data_ov007_02104ba0 + 0x0) = func_ov007_020c690c(0xc8, 0x1f4, 0x1f4,
        *(int *)(data_ov007_0210342c + 0x30), &data_ov007_020d7b60, &data_ov007_020d7b80);

    *(void **)(data_ov007_02104ba0 + 0x4) = func_ov007_020c9388(0);
    *(void **)(data_ov007_02104ba0 + 0x8) = func_ov007_020c9388(-1);
    *(void **)(data_ov007_02104ba0 + 0xc) = func_ov007_020c9388(0);
    *(void **)(data_ov007_02104ba0 + 0x10) = func_ov007_020c9388(0);

    *(int *)(data_ov007_02104ba0 + 0x1c) = 0;
    *(int *)(data_ov007_02104ba0 + 0x20) = 0;
    *(int *)(data_ov007_02104ba0 + 0x28) = 0;
    *(int *)(data_ov007_02104ba0 + 0x24) = 0;
    *(int *)(data_ov007_02104ba0 + 0x2c) = 0;
    *(int *)(data_ov007_02104ba0 + 0x38) = 0;

    *(void **)(data_ov007_02104ba0 + 0x44) = func_ov007_020c908c(data_ov007_02104ba0 + 0x3c);
    *(void **)(data_ov007_02104ba0 + 0x48) = func_ov007_020c908c(data_ov007_02104ba0 + 0x40);

    sp1c = 0;
    sp14 = 0x60000;
    sp18 = 0x100;
    sp10 = 1;

    for (var_sl = 0; var_sl < 2; var_sl++) {
        *(void **)(data_ov007_02104b9c + 0x4 + var_sl * 4) = func_ov007_020c44e4(sp10);
        temp_sb = *(void **)(data_ov007_02104b9c + 0x4 + var_sl * 4);

        if (var_sl == 0) {
            *(int *)(*(int **)((char *)temp_sb + 0x5c)) = *(int *)(data_ov007_0210342c + 0x84);
            *(int *)((char *)temp_sb + 0x10) = 0xfffcdf01;
            *(u8 *)((char *)temp_sb + 0x4e) = 0x18;
        } else {
            *(int *)(*(int **)((char *)temp_sb + 0x5c)) = *(int *)(data_ov007_0210342c + 0x88);
            *(int *)((char *)temp_sb + 0x10) = -0x32000;
            *(u8 *)((char *)temp_sb + 0x4e) = 0x17;
        }

        *(u16 *)((char *)temp_sb + 0x62) = 8;
        *(u16 *)((char *)temp_sb + 0x60) = *(u16 *)((char *)temp_sb + 0x62);
        *(int *)((char *)temp_sb + 0x4) = 0x80000;
        *(int *)((char *)temp_sb + 0x8) = sp14;
        *(s16 *)((char *)temp_sb + 0xc) = (s16)sp18;
        *(s16 *)((char *)temp_sb + 0xe) = (s16)sp18;
        *(int *)((char *)temp_sb + 0x50) = sp1c;

        func_ov007_020c43bc(temp_sb, *(int *)(data_ov007_0210342c + 0x34));
        func_ov007_020c4388((char *)temp_sb, *(int *)(data_ov007_0210342c + 0x34));
    }

    *(void **)(data_ov007_02104b9c + 0xc) = func_ov007_020c44e4(1);
    temp_r5 = *(void **)(data_ov007_02104b9c + 0xc);
    *(u16 *)((char *)temp_r5 + 0xc) = 0x48;
    *(u16 *)((char *)temp_r5 + 0xe) = 8;
    *(u16 *)((char *)temp_r5 + 0x4c) = 0x7f8a;
    *(u16 *)((char *)temp_r5 + 0x46) = *(u16 *)((char *)temp_r5 + 0x4c);
    *(u16 *)((char *)temp_r5 + 0x4a) = 0x6e02;
    *(u16 *)((char *)temp_r5 + 0x48) = *(u16 *)((char *)temp_r5 + 0x4a);
    *(int *)((char *)temp_r5 + 0x0) = *(int *)((char *)temp_r5 + 0x0) | 2;
    *(u8 *)((char *)temp_r5 + 0x4e) = 0x19;

    *(void **)(data_ov007_02104b9c + 0x10) = func_ov007_020c44e4(1);
    temp_r5 = *(void **)(data_ov007_02104b9c + 0x10);
    *(u16 *)((char *)temp_r5 + 0xc) = 0x48;
    *(u16 *)((char *)temp_r5 + 0xe) = 8;
    *(u16 *)((char *)temp_r5 + 0x44) = 0x294a;
    *(int *)((char *)temp_r5 + 0x50) = 0x5000;
    *(int *)((char *)temp_r5 + 0x0) = *(int *)((char *)temp_r5 + 0x0) | 1;
    *(u8 *)((char *)temp_r5 + 0x4e) = 0x1a;

    temp_r4 = *(void **)((char *)(*(void **)(data_ov007_0210342c + 0x128)) + 0x4);
    *(int *)((char *)(*(void **)(data_ov007_02104b9c + 0xc)) + 0x10) = *(int *)((char *)temp_r4 + 0x10) - 0x10;
    *(int *)((char *)(*(void **)(data_ov007_02104b9c + 0x10)) + 0x10) = *(int *)((char *)temp_r4 + 0x10) - 0xff;

    func_ov007_020c43bc(*(void **)(data_ov007_02104b9c + 0xc), *(int *)(data_ov007_0210342c + 0x34));
    func_ov007_020c43bc(*(void **)(data_ov007_02104b9c + 0x10), *(int *)(data_ov007_0210342c + 0x34));

    *(int *)(data_ov007_02104ba0 + 0x34) = 0;
    *(int *)((char *)(*(void **)(data_ov007_02104ba0 + 0x44)) + 0x18) = 0x800;
    *(int *)(data_ov007_02104ba0 + 0x3c) = *(int *)((char *)(*(void **)(data_ov007_02104ba0 + 0x44)) + 0x18);

    if (a0 != 0) {
        if (a0 == 1) {
            *(int *)(data_ov007_02104ba0 + 0x30) = 0;
            *(int *)(data_ov007_02104ba0 + 0x40) = 0;
            *(u16 *)((char *)(*(void **)(data_ov007_02104ba0 + 0x8)) + 0x2) = 0;
        }
    } else {
        *(int *)(data_ov007_02104ba0 + 0x30) = 1;
        *(int *)(data_ov007_02104ba0 + 0x40) = 0x1000;
        *(u16 *)((char *)(*(void **)(data_ov007_02104ba0 + 0x8)) + 0x2) = 2;
    }

    if (a0 != 1)
        return;

    func_ov007_020bbe60(a1);
}
