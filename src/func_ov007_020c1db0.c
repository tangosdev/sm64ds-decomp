typedef unsigned short u16;
typedef short s16;

typedef struct Vec4 { u16 a, b, c, d; } Vec4;

typedef struct T {
    Vec4 v0;    /* 0x00 */
    Vec4 v8;    /* 0x08 */
    Vec4 v10;   /* 0x10 */
    Vec4 v18;   /* 0x18 */
    int f20;
    int f24;
    int f28;
    int f2c;
    u16 f30, f32, f34, f36;
    s16 f38, f3a, f3c, f3e;
    int f40;
} T;

extern void func_0205f040(void);
extern void func_0205efc0(void *c);

void func_ov007_020c1db0(T *a, Vec4 *b)
{
    a->v10 = a->v8;
    a->v8 = a->v0;
    if (b == 0) {
        u16 tmp[4];
        func_0205f040();
        func_0205efc0(tmp);
        a->v0.a = tmp[0];
        a->v0.b = tmp[1];
        a->v0.c = tmp[2];
        a->v0.d = tmp[3];
    } else {
        a->v0 = *b;
    }
    a->v8 = a->v0;

    if (*(volatile unsigned int *)&a->f2c & 1) {
        if (a->v8.c == 0)
            a->f2c &= ~1;
        else
            a->v8.c = 0;
    }

    if (a->v8.c != 0) {
        if (a->v8.d & 1) {
            if (a->v10.c != 0)
                a->v8.a = a->v10.a;
            else
                a->v8.c = 0;
        }
        if (a->v8.d & 2) {
            if (a->v10.c != 0)
                a->v8.b = a->v10.b;
            else
                a->v8.c = 0;
        }
    }

    if (a->v8.c != 0) {
        a->v18 = a->v8;
        if (a->v10.c == 0) {
            a->f30 = a->v8.a;
            a->f32 = a->v8.b;
            a->f20 = 0;
            a->f3a = 0;
            a->f38 = a->f3a;
            a->f40 = -1;
            a->f3e = 0;
            a->f3c = a->f3e;
        } else {
            *(int *)(((int)a + 0x20) & 0xFFFFFFFFFFFFFFFFLL) += 1;
            a->f38 = a->v8.a - a->f30;
            a->f3a = a->v8.b - a->f32;
            a->f3c = a->v8.a - a->v10.a;
            a->f3e = a->v8.b - a->v10.b;
        }
    } else {
        if (a->v10.c == 0) {
            *(int *)(((int)a + 0x24) & 0xFFFFFFFFFFFFFFFFLL) += 1;
        } else {
            a->f24 = 0;
            a->f34 = a->v8.a;
            a->f36 = a->v8.b;
        }
    }

    *(int *)(((int)a + 0x28) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    if (a->f28 < 0)
        a->f28 = 0;
}
