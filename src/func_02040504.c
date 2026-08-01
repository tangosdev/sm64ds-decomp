#include "types.h"
extern void func_02040820(void);
extern void func_02040790(void);
extern int data_020a0f5c;
extern int data_020a0f94;

void func_02040504(u16 *o)
{
    if (o[1] == 0) {
        switch (data_020a0f5c) {
        case 0:
            data_020a0f94 = 0;
            break;
        case 3:
            data_020a0f94 = 0;
            func_02040820();
            break;
        case 4:
            data_020a0f94 = 0;
            func_02040790();
            break;
        }
    } else {
        data_020a0f94 = 1;
    }
}
