/* func_0202fc08 at 0x0202fc08 - a destructor for a Color-derived class.
 *   [this+0] = vtable (0x020926f0)
 *   if (this->unkF == 1) func_0202fb30(this);   (bl 0x0202fb30)
 *   bl 0x02017574 = Color::~Color (D1)
 * returns this.
 */

struct Obj {
    void **vtable;   /* 0x0 */
    char pad[0xf - 4];
    unsigned char unkF; /* 0xf */
};

extern void *data_020926f0[];

extern void func_0202fb30(struct Obj *thiz);    /* 0x0202fb30 */
extern void *_ZN5ColorD1Ev(struct Obj *thiz);   /* 0x02017574 */

struct Obj *func_0202fc08(struct Obj *thiz)
{
    thiz->vtable = (void **)data_020926f0;
    if (thiz->unkF == 1)
        func_0202fb30(thiz);
    _ZN5ColorD1Ev(thiz);
    return thiz;
}
