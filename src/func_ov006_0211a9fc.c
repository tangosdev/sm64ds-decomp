/* func_ov006_0211a9fc at 0x0211a9fc
 *
 * Activates entry i of a 0x24-stride array at this+0x51b8: sets flags,
 * clears counters, and loads its duration from data_ov006_0212ee70[type].
 */
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct Entry {
    u32 value;     /* +0x00 (this+0x51b8) */
    u32 counter;   /* +0x04 */
    char _pad8[8];
    u16 halfword;  /* +0x10 (this+0x51c8) */
    char _pad12[3];
    u8 flagA;      /* +0x15 (this+0x51cd) */
    char _pad16[3];
    u8 flagB;      /* +0x19 (this+0x51d1) */
    u8 type;       /* +0x1a (this+0x51d2) */
    char _pad1b[9];
};

struct Obj {
    char _pad[0x51b8];
    struct Entry entries[1];  /* 0x51b8 */
};

extern u32 data_ov006_0212ee70[];

void func_ov006_0211a9fc(struct Obj *self, int i)
{
    self->entries[i].flagA = 1;
    self->entries[i].counter = 0;
    self->entries[i].value = data_ov006_0212ee70[self->entries[i].type];
    self->entries[i].flagB = 1;
    self->entries[i].halfword = 0;
}
