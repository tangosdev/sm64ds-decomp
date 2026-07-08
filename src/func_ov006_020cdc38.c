typedef unsigned short u16;
typedef signed int s32;

void func_ov006_020cdc38(void *arg0)
{
    *(u16 *)(((long long)(int)((char *)arg0 + 0x9a)) & 0xFFFFFFFFFFFFFFFFLL) += 0x100;
    if (*(u16 *)((char *)arg0 + 0x9a) & 0x8000) {
        *(s32 *)((char *)arg0 + 0x30) = 0xc00;
    } else {
        *(s32 *)((char *)arg0 + 0x30) = -0xc00;
    }
}
