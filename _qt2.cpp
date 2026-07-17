//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" {
int test7a(u8 *buf, u32 len, u16 sum0)
{
    u16 sum = sum0;
    u32 j;
    for (j = 0; j < len; j++) {
        sum = (buf[j] & 0xFF) ^ (u16)(((sum << 1) & 0xFFFE) | ((sum >> 15) & 1));
    }
    return sum;
}

int test7b(char *buf, u32 len, u16 sum0)
{
    u16 sum = sum0;
    u32 j;
    for (j = 0; j < len; j++) {
        sum = ((u8)buf[j] & 0xFF) ^ (u16)(((sum << 1) & 0xFFFE) | ((sum >> 15) & 1));
    }
    return sum;
}
}
