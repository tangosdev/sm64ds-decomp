typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

void func_ov006_020d6b88(char *this, int idx);
void func_ov006_020d6c90(char *this, int idx);
void func_ov006_020d6e8c(char *this, int idx);
void func_02012718(void *a, int b);
int func_020126e8(int a);
int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);

extern u8 data_020a0e40;
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

#define B ((char *)this + idx * 0x40 + 0x4000)

#pragma opt_common_subs off

void func_ov006_020d7f5c(char *this, int idx)
{
    if (*(u8 *)(this + 0x62f6) == 0xff) {
        *(u8 *)(B + 0x697) = 1;
        *(u16 *)((char *)this + 0x4692 + idx * 0x40) += 0x40;
        func_ov006_020d6b88(this, idx);
        func_ov006_020d6c90(this, idx);
    } else {
        int old_x = *(int *)(B + 0x660);
        int old_y = *(int *)(B + 0x664);
        int cx, cy;
        u8 was;
        int t = data_020a0e40;

        *(int *)(B + 0x660) = (data_020a0dea[t << 2] << 12) - *(int *)(B + 0x668);
        *(int *)(B + 0x664) = (data_020a0deb[t << 2] << 12) - *(int *)(B + 0x66c);
        cx = old_x >> 12;
        cy = old_y >> 12;

        func_ov006_020d6e8c(this, idx);

        {
            int t2 = data_020a0e40;
            *(int *)(B + 0x660) = (data_020a0dea[t2 << 2] << 12) - *(int *)(B + 0x668);
            *(int *)(B + 0x664) = (data_020a0deb[t2 << 2] << 12) - *(int *)(B + 0x66c);
        }

        was = *(u8 *)(B + 0x69e);
        if (*(u8 *)(B + 0x696) != 0) {
            if (cx > 0xc0 && cy > 0x40 && cy < 0x80) {
                *(u8 *)(B + 0x69e) = 1;
            } else {
                *(u8 *)(B + 0x69e) = 0;
            }
        } else {
            if (cx < 0x40 && cy > 0x40 && cy < 0x80) {
                *(u8 *)(B + 0x69e) = 1;
            } else {
                *(u8 *)(B + 0x69e) = 0;
            }
        }

        if (was == 0 && *(u8 *)(B + 0x69e) != 0) {
            func_02012718((void *)0x1e4, *(int *)(B + 0x660));
        }

        *(int *)(B + 0x688) = func_02012468(*(int *)(B + 0x688), 2, 0x1e5, 4, 0, 0,
                                            func_020126e8(*(int *)(B + 0x660)), 0);
        *(int *)(B + 0x67c) = *(int *)(B + 0x660);
        *(int *)(B + 0x680) = *(int *)(B + 0x664);
    }
}
