//cpp
struct Thing { int x; };

struct Sub {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(Thing *t);
};

struct Obj {
    char pad80[0x80];
    Thing arg80;          /* +0x80 (passed by address) */
    char padb0[0xb0 - 0x84];
    unsigned int fb0;      /* +0xb0 */
    char pad30c[0x30c - 0xb4];
    Sub sub30c;            /* +0x30c */
    char pad370[0x370 - 0x310];
    Sub sub370;            /* +0x370 */
    char pad4a2[0x4a2 - 0x374];
    unsigned short b0 : 1;  /* +0x4a2 bit 0 */
    unsigned short b1 : 1;  /* bit 1 */
    unsigned short b2 : 1;  /* bit 2 */
};

extern "C" int _ZN9PowerStar6RenderEv(Obj *self) {
    int locked;
    if (self->arg80.x == 0) goto done;
    locked = (self->fb0 & 0x40000) != 0;
    if (locked) goto done;
    if (self->b1) goto callit;
done:
    return 1;
callit:
    if (!self->b2)
        self->sub30c.m5(&self->arg80);
    else
        self->sub370.m5(&self->arg80);
    return 1;
}
