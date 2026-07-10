//cpp
/* func_ov006_020e3078 at 0x020e3078 (ov006)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
typedef unsigned char u8;
typedef unsigned short u16;

struct C;
typedef void (C::*PMF0)();
typedef void (C::*PMF1)(int);

extern "C" {
extern PMF0 data_ov006_021418b0[];
extern PMF1 data_ov006_02141910[];
void func_ov006_020e1680(char* c);
}

extern "C" void func_ov006_020e3078(char* c)
{
    if (*(u16*)(c + 0x4ee2) != 0) {
        u16* q = (u16*)((long long)(int)(c + 0x4ee2) & 0xFFFFFFFFFFFFFFFFLL);
        *q = *q - 1;
        return;
    }
    if (*(u8*)(c + 0xc4) == 0) {
        *(u8*)(c + 0xc3) = 1;
        *(u8*)(c + 0xc4) = 1;
        *(u16*)(c + 0xc0) = 0;
    }
    if (*(u8*)(c + 0x4ee9) != 0) {
        u8* q = (u8*)((long long)(int)(c + 0x4ee9) & 0xFFFFFFFFFFFFFFFFLL);
        *q = *q - 1;
    }
    (((C*)c)->*data_ov006_021418b0[*(u8*)(c + 0x4ee4)])();

    {
        int count = 0;
        int i = 0;
        char* p = c;
        for (; i < 5; i++, p += 0x2c) {
            if (*(u8*)(p + 0x4689) != 0) {
                *(int*)(p + 0x466c) = *(int*)(p + 0x4660);
                *(int*)(p + 0x4670) = *(int*)(p + 0x4664);
                if (*(u16*)(p + 0x4680) != 0) {
                    u16* q = (u16*)((long long)(int)(p + 0x4680) & 0xFFFFFFFFFFFFFFFFLL);
                    *q = *q - 1;
                }
                (((C*)c)->*data_ov006_02141910[*(u8*)(p + 0x4688)])(i);
                if (*(u8*)(p + 0x4688) != 2) count++;
            }
        }
        if (count != 0) return;
    }
    *(int*)(c + 0x4eac) = 2;
    func_ov006_020e1680(c);
    *(u16*)(c + 0x4ee2) = 0xc0;
}
