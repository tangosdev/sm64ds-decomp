typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Shared {
    char pad[0x1c];
    char *buf;
};

struct Csl {
    int f0;
    int f4;
    int f8;
    int fc;
};

int func_0206cf7c(int mask);
void func_0206cec0(void *c);
void func_0206ce20(void *s);
void func_0206cd9c(void *s);
void func_0206ce90(u32 flags);
void func_0206cb0c(void *c, int b);
void func_0206ca7c(void *c, int b);

#pragma optimize_for_size on

void func_0206cbc0(struct Csl *sl, u8 *sb, int count)
{
    struct Shared *sh = (struct Shared *)0x027fff60;
    void *base;
    void (*fn)(void *, int);
    int la[2];
    int lb[2];
    int v;

    base = sh->buf + 0x90;
    fn = 0;

    if (count == 0)
        return;
    if (func_0206cf7c(0x20000) == 0)
        return;
    if (func_0206cf7c(1) != 0) {
        fn = func_0206cb0c;
    } else if (func_0206cf7c(4) != 0) {
        fn = func_0206ca7c;
    }
    if (fn == 0)
        return;

    func_0206cec0(lb);
    func_0206ce20(la);

    if (sl != 0) {
        while (count-- != 0) {
            if ((u32)sl->fc < (u32)(sl->f4 - 1)) {
                if ((int)sb & 1)
                    v = (*(u16 *)(sb - 1) & 0xff00) >> 8;
                else
                    v = *(u16 *)sb & 0xff;
                v = (u16)v;
                fn(base, v & 0xff);
                sl->fc += 1;
            }
            {
                int t = sl->f8;
                sb += 1;
                sl->f8 = t + 1;
            }
        }
    }

    func_0206cd9c(la);
    func_0206ce90(lb[0]);
}
