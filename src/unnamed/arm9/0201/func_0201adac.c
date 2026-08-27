#include "types.h"
/* func_0201adac - calls Message::AddChar twice with font-encoded char and char+1.
 * Reads index from data_0209d6f4 bytes [3] and [4], looks up in data_0208ee6c.
 * Attempt 3: correct types - S* (not S**), AddChar(int), u16 idx, reversed load order.
 */
extern void _ZN7Message7AddCharEc(int c);

struct MsgTextChar {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 indexHi;  /* offset 3 */
    u8 indexLo;  /* offset 4 */
};
extern struct MsgTextChar* data_0209d6f4;
extern u8 data_0208ee6c[];

void func_0201adac(void) {
    struct MsgTextChar* state = data_0209d6f4;
    u16 idx = state->indexLo | (state->indexHi << 8);
    _ZN7Message7AddCharEc(data_0208ee6c[idx]);
    _ZN7Message7AddCharEc((data_0208ee6c[idx] + 1) & 0xff);
}