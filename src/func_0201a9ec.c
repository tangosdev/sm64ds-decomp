/* func_0201a9ec at 0x0201a9ec - thunk: func_0201a96c(self, 1) */

typedef unsigned char u8;

extern u8 func_0201a96c(void* self, u8 newVal);

u8 func_0201a9ec(void* self)
{
    return func_0201a96c(self, 1);
}
