/* func_020313d8 - calls Message::AddChar twice with font-encoded char and char+1.
 * Reads index from data_0209fd0c bytes [3] and [4], looks up in data_02092810.
 * Attempt 3: correct types - S* (not S**), AddChar(int), u16 idx, reversed load order.
 */
typedef unsigned char u8;
typedef unsigned short u16;

extern void _ZN7Message7AddCharEc(int c);

struct MsgTextChar {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 indexHi;  /* offset 3 */
    u8 indexLo;  /* offset 4 */
};
extern struct MsgTextChar* data_0209fd0c;
extern u8 data_02092810[];

void func_020313d8(void) {
    struct MsgTextChar* state = data_0209fd0c;
    u16 idx = state->indexLo | (state->indexHi << 8);
    _ZN7Message7AddCharEc(data_02092810[idx]);
    _ZN7Message7AddCharEc((data_02092810[idx] + 1) & 0xff);
}
