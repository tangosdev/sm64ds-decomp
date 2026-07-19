typedef unsigned char u8;

struct S6  { int v[6]; };
struct S7  { int v[7]; };
struct S21 { int v[21]; };

struct S72d90 {
    struct S6  p0;
    struct S7  p1;
    struct S21 p2;
};

extern unsigned char func_02072d90(struct S72d90 *c);
extern u8 *func_02071a50(u8 *p, int *out);
extern u8 *func_02071af8(u8 *p, int *out);
extern int func_02072fcc(int a, int b, int c);
extern int func_02071f88(void *a, void *list);
extern void func_02071f1c(char *a, void *b, char *c, int d);
extern void func_020731fc(void);
extern int func_020729f4(struct S72d90 *c);

struct Tmp {
    int val;
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
};

int func_02071d3c(int *self, struct S6 *src, int flag)
{
    struct Tmp tmp;
    struct S72d90 buf;
    int state;

    buf.p0.v[0] = src->v[0];
    buf.p0.v[1] = src->v[1];
    buf.p0.v[2] = src->v[2];
    buf.p0.v[3] = src->v[3];
    buf.p0.v[4] = src->v[4];
    buf.p0.v[5] = src->v[5];

    buf.p1.v[0] = self[0];
    buf.p1.v[1] = self[1];
    buf.p1.v[2] = self[2];
    buf.p1.v[3] = self[3];
    buf.p1.v[4] = self[4];
    buf.p1.v[5] = self[5];
    buf.p1.v[6] = self[6];

    buf.p2 = *(struct S21 *)((char *)self + 0x1c);

    state = func_02072d90(&buf);

    for (;;) {
        switch (state) {
        case 12:
        {
            u8 *cur = (u8 *)buf.p0.v[2];
            tmp.val = cur[1] | (cur[2] << 8) | (cur[3] << 16) | (cur[4] << 24);
            u8 *p = func_02071a50(cur + 5, &tmp.a);
            p = func_02071af8(p, &tmp.b);
            if (func_02072fcc(self[0], tmp.val, flag) == 0) {
                break;
            }
            goto L_return;
        }

        case 15:
        {
            u8 *cur = (u8 *)buf.p0.v[2];
            u8 *p = func_02071a50(cur + 1, &tmp.c);
            p = func_02071a50(p, &tmp.d);
            p = func_02071af8(p, &tmp.e);
            tmp.f = (int)p;
            if (func_02071f88((void *)self[0], &tmp.c) == 0) {
                func_02071f1c((char *)self, (void *)src, (char *)&tmp.c, buf.p0.v[2]);
            }
            break;
        }

        case 1:
        case 14:
        default:
            func_020731fc();
            goto L_return;

        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 13:
        case 16:
        case 17:
        case 18:
        case 19:
            break;
        }

        state = func_020729f4(&buf);
    }

L_return:
    return buf.p0.v[2];
}
