/* func_ov006_02102d6c at 0x02102d6c
 *
 * Resets entry i of a 0x40-stride array at this+0x4660: reverses and
 * quarters its velocity, clears counters, and tail-calls func_02012718
 * (sound: id 0x19c) with the entry's handle.
 */
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct Entry {
    u32 handle;    /* +0x00 (this+0x4660) */
    int a;         /* +0x04 = 0xa8000 */
    int b;         /* +0x08 = 0 */
    int vel;       /* +0x0c = -old/4 */
    char _pad10[0x26];
    u16 half;      /* +0x36 (this+0x4696) = 0 */
    char _pad38;
    u8 state;      /* +0x39 (this+0x4699) = 4 */
    char _pad3a[3];
    u8 flag;       /* +0x3d (this+0x469d) = 0 */
    char _pad3e[2];
};

struct Obj {
    char _pad[0x4660];
    struct Entry entries[1];  /* 0x4660 */
};

extern void func_02012718(int soundId, u32 handle);

void func_ov006_02102d6c(struct Obj *self, int i)
{
    self->entries[i].state = 4;
    self->entries[i].vel = -self->entries[i].vel >> 2;
    self->entries[i].b = 0;
    self->entries[i].half = 0;
    self->entries[i].flag = 0;
    self->entries[i].a = 0xa8000;
    func_02012718(0x19c, self->entries[i].handle);
}
