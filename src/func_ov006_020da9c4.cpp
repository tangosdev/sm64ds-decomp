//cpp
// @symbol func_ov006_020da9c4
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgCard_c.h"
// @emits dScMgCard_c_Render
/* recovered: renamed to Class_Method */
/* dScMgCard_c::Render - recovered from vtable slot identity */
struct Node {
    virtual void m0();
    char pad[0x2c];
};

extern "C" void func_ov006_020c0aa8(void *c);
extern "C" int  RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern "C" void func_ov004_020b1bc8(char *a0, int a1, int a2, int a3);
extern "C" void func_ov004_020b1e34(void *a0, int a1, int a2, int a3);
extern "C" void func_ov006_020c1804(void *c);

extern int data_ov006_02134028;

extern "C" int dScMgCard_c_Render(char *c)
{
    struct dScMgCard_c *self = (struct dScMgCard_c *)(void *)c;
    int skip;
    short v;
    int i;
    int sb;

    func_ov006_020c0aa8(c + 0x4660);

    v = self->unk_5388;
    if (v > 3 && v < 0x11) {
        skip = -1;
        if (v > 0xe) {
            if (self->unk_5396 & 8) {
                if (func_ov006_020da5e8(c + 0x51a8, c + 0x5298) == 1) {
                    skip = 6 - func_ov006_020da4ac(c + 0x51a8, 0);
                }
            }
        }

        i = 0;
        sb = 0xc;
        for (; i < 6; i++) {
            if (i != skip)
                RenderOamMainScreen(data_ov006_02134010[i], 0x28, sb, -1, -1);
            sb += 0x10;
        }

        RenderOamMainScreen(data_ov006_02134028, 0x28, 0x38, -1, -1);
    }

    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);

    if (self->unk_5398 < self->unk_0b4)
        self->unk_5398 = self->unk_0b4;
    func_ov004_020adb1c(self->unk_5398);

    func_ov004_020b6430();

    func_ov006_020d9a14(c);

    {
        Node *bank = (Node *)(c + 0x51a8);
        int cnt = 4;
        char *cfgp = c + 0xc0;
        Node *node = &bank[4];
        do {
            unsigned char cfg = *(unsigned char *)(cfgp + 0x5000 + 0x1d2);
            if (self->unk_538e == cfg)
                goto chk1;
            if (self->unk_5390 != cfg)
                goto docall1;
        chk1:
            if (!(self->unk_5396 & 8))
                goto skip1;
        docall1:
            node->m0();
        skip1:
            cfgp -= 0x30;
            node--;
        } while (--cnt >= 0);
    }

    {
        Node *bank = (Node *)(c + 0x5298);
        int cnt = 4;
        char *cfgp = c + 0xc0;
        Node *node = &bank[4];
        do {
            unsigned char cfg = *(unsigned char *)(cfgp + 0x5000 + 0x2c2);
            if (self->unk_5392 == cfg)
                goto chk2;
            if (self->unk_5394 != cfg)
                goto docall2;
        chk2:
            if (!(self->unk_5396 & 8))
                goto skip2;
        docall2:
            node->m0();
        skip2:
            cfgp -= 0x30;
            node--;
        } while (--cnt >= 0);
    }

    func_ov006_020c1804(c + 0x4f38);
    return 1;
}
