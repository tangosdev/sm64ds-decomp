// NONMATCHING: base materialization / addressing (div=13). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;

extern u8 data_020a0e40;
extern u16 data_0209f49c[];
extern char _ZN6Player7ST_LANDE;

struct Player {
    char pad0[0x8e];
    s16 field_8e;            /* 0x8e */
    char pad2[0x94 - 0x90];
    s16 field_94;            /* 0x94 */
    char pad3[0xa0 - 0x96];
    int field_a0;            /* 0xa0 */
    char pad4[0x6de - 0xa4];
    u8 field_6de;            /* 0x6de */
};

extern void func_ov002_020e28d4(struct Player *thiz, int a, int b);
extern void func_ov002_020bedd4(struct Player *thiz);
extern void _ZN6Player11ChangeStateERNS_5StateE(struct Player *thiz, void *st);

int _ZN6Player12St_Spin_MainEv(struct Player *thiz)
{
    func_ov002_020e28d4(thiz, 0x1000, 0x1000);
    if (thiz->field_6de == 0) {
        thiz->field_8e = thiz->field_94;
        _ZN6Player11ChangeStateERNS_5StateE(thiz, &_ZN6Player7ST_LANDE);
    }
    if (*(u16*)((char*)data_0209f49c + (&data_020a0e40)[0] * 0x18) & 2) {
        thiz->field_a0 = -0x9000;
        thiz->field_8e += 0x2000;
    } else {
        thiz->field_a0 = -0xc000;
        thiz->field_8e += 0x1800;
    }
    func_ov002_020bedd4(thiz);
    return 1;
}
