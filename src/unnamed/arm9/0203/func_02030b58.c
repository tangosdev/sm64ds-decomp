#include "types.h"
extern int func_0203d9b4(void);
extern int func_0203d950(int i);
extern void func_02020214(unsigned int flags);
extern u8 func_0202027c(int bit);
extern int func_0203da9c(void);
extern void func_02030790(void);

extern u8 data_0209fc5c[];
extern u8 data_0209fc58;

/* Session state 5's per-frame handler (data_020927ec[5]). On the host
   (func_0203d9b4), post a slot mask of the players present on the link
   (func_0203d950) through func_02020214. A present slot whose bit the
   posted mask (func_0202027c) no longer has is dropped and gets its
   disconnect bit in data_0209fc58. If slot 0 is still present but this
   console's own slot (func_0203da9c) is not, flag all four and end the
   session (func_02030790). */
void func_02030b58(void) {
    if (func_0203d9b4() != 0) {
        u8 flags = 0;
        int j;
        for (j = 0; j < 4; j++) {
            if (func_0203d950(j) != 0)
                flags = (u8)(flags | (1 << j));
        }
        func_02020214(flags);
    }
    int i;
    for (i = 0; i < 4; i++) {
        if (data_0209fc5c[i] != 0) {
            if (func_0202027c(i) == 0) {
                data_0209fc58 = (u8)(data_0209fc58 | (1 << i));
                data_0209fc5c[i] = 0;
            }
        }
    }
    if (data_0209fc5c[0] == 0) return;
    if (data_0209fc5c[func_0203da9c()] != 0) return;
    data_0209fc58 |= 0xf;
    func_02030790();
}
