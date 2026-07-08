typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

struct R {
    u16 field_0;   /* 0x0 */
    u16 field_2;   /* 0x2 */
    u16 field_4;   /* 0x4 */
};

extern u8 TOUCH_INPUT_ARR[];
extern struct R *func_0203dabc(int idx);

void func_0203bb60(void)
{
    u8 *p = TOUCH_INPUT_ARR;
    int i;
    for (i = 0; i < 4; i++) {
        struct R *r = func_0203dabc(i);
        p[1] = (u8)(r->field_4 ^ p[0]);
        p[0] = (u8)r->field_4;
        p[2] = (u8)r->field_0;
        p[3] = (u8)r->field_2;
        p += 4;
    }
}
