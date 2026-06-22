//cpp
typedef unsigned char u8;

extern "C" {

struct Obj;

}

struct Obj {
    virtual void m00();
    virtual void m04();
    virtual void m08();
    virtual void m0c();
    virtual void m10();
    virtual void m14();
    virtual void m18();
    virtual int  m1c();      /* vtable offset 0x1c */
    int pad[4];
    int field_14;            /* 0x14 */
    int field_18;            /* 0x18 */
    u8 pad1c[0xc];
    u8 field_28;             /* 0x28 */
};

extern "C" int func_0202ec9c(struct Obj *thiz, int arg1)
{
    if (arg1 == thiz->field_14) {
        thiz->field_28 = 0;
        return 0;
    }
    if (thiz->m1c()) {
        thiz->field_18 = arg1;
        thiz->field_28 = 1;
        return 1;
    } else {
        thiz->field_14 = arg1;
        thiz->field_28 = 0;
        return 0;
    }
}
