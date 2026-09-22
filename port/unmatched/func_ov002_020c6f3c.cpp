/* HOST COPY of src/actors/Player.cpp --
 * Player::St_LevelEnter_Main, with its step table read as the eight-byte
 * records it is. That src file is the provenance: its line 22 is the
 * dispatch site this file replaces, (c->*data_ov002_0211075c[...])().
 *
 * NAMING, and an earlier version of this header had it BACKWARDS in both
 * directions. Player::St_LevelEnter_Main is ov002 0x020c6f3c and config
 * names it _ZN6Player18St_LevelEnter_MainEv (ov002/symbols.txt, size
 * 0xa8). The name func_ov002_020c6f3c appears in NO symbols.txt and there
 * is no src/func_ov002_020c6f3c.cpp; it is this port's own C-linkage name
 * for the host copy below, which the earlier header cited as if it were
 * the source. The symbol that names another overlay's bytes at this
 * address is ov006's func_ov006_020c6f3c (size 0x34), a different
 * function.
 *
 * The body dispatches the entrance step through
 *
 *     (this->*data_ov002_0211075c[mStateStepSub])();
 *
 * a seven-entry array of mwcc pointer-to-member-functions that
 * __sinit_ov002_021019d0 copies out of relocated pairs at 0x0210a074 ..
 * 0x0210a48c. Each record is {function address, 0} and the records are eight
 * bytes apart. MSVC's pointer-to-member for a single-inheritance class is one
 * word, so the matched source compiled for the host would stride the array by
 * four and call every other half of a record. Reading the word at index * 8
 * and handing it to the state dispatcher keeps the ROM's own targets.
 *
 * The seven steps, in table order: 0x020c75f0, 0x020c7350, 0x020c71e0,
 * 0x020c7194, 0x020c72a4, 0x020c70ac, 0x020c6fe4.
 */
extern "C" {

typedef unsigned char u8;

extern unsigned char data_ov002_0211075c[];   /* 7 records of 8 bytes */
struct PortLevelEnterObj { char pad[0x118]; void *f118; };
extern PortLevelEnterObj *data_0209f318;
void Player_AdvanceAnims(void *c);
int hal_call_state_fn(void *self, unsigned ds_addr);

int func_ov002_020c6f3c(void *self)
{
    u8 *f = (u8 *)self;
    switch (f[0x6e3]) {
    case 8: case 9: case 11: case 12: case 16: case 18:
        if (data_0209f318)
            data_0209f318->f118 = self;
        break;
    default:
        break;
    }
    {
        unsigned step = f[0x6e5];
        unsigned fn = *(unsigned *)(data_ov002_0211075c + step * 8);
        if (fn)
            hal_call_state_fn(self, fn);
    }
    if (f[0x70c] == 0)
        Player_AdvanceAnims(self);
    return 1;
}

}
