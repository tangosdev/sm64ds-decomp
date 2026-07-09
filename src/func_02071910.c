/* func_02071910 at 0x02071910, size 0x64
 * Matched byte-for-byte with mwccarm 1.2/sp2p3
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
typedef unsigned short u16;

#pragma opt_strength_reduction off
void* func_02071910(char* self)
{
    int off = (*(unsigned char*)(self + 0x6a) != 0) ? 0x10 : 0;
    int f60 = *(int*)(self + 0x60);
    char* lr = (char*)(*(int*)(self + 0x18) + f60 - off);
    int i;

    for (i = 15; i >= 0; i--) {
        u16 flags = *(u16*)(self + 0x68);
        if (flags & (1 << i)) {
            lr -= 4;
            *(int*)(self + 0x1c + i * 4) = *(int*)lr;
        }
    }

    *(int*)(self + 0x14) = *(int*)(self + 0x18) + f60;
    return *(void**)(self + 0x54);
}
