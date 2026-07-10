/* func_ov006_020ff4ec at 0x020ff4ec
 *
 * Resets two 0x20-byte slot entries: flag=1, id=0xff, two counters=0.
 */
typedef unsigned char u8;

struct Slot {
    u8 active;   /* +0x00 (this+0x563b) */
    u8 count;    /* +0x01 */
    u8 id;       /* +0x02 */
    u8 timer;    /* +0x03 */
    char _pad[0x1c];
};

struct Obj {
    char _pad[0x563b];
    struct Slot slots[2];  /* 0x563b */
};

void func_ov006_020ff4ec(struct Obj *self)
{
    int i;

    for (i = 0; i < 2; i++) {
        self->slots[i].active = 1;
        self->slots[i].id = 0xff;
        self->slots[i].count = 0;
        self->slots[i].timer = 0;
    }
}
