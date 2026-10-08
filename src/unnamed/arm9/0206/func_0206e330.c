// @symbol func_0206e330
/* recovered: MSL memset/__fill_mem -- byte head, 32-byte unrolled block,
 * word tail, byte tail. The head and block loops share one counter, which is
 * what colours both into r3; opt_lifetimes off keeps the word counter separate. */
#pragma opt_lifetimes off
typedef unsigned int u32;
typedef unsigned char u8;

void func_0206e330(void *dst, int val, u32 n)
{
    u8 *p = (u8 *)dst;
    u32 c = (u32)val & 0xff;

    if (n >= 0x20) {
        u32 k, w;

        k = (u32)(-(int)p) & 3;
        if (k != 0) {
            n -= k;
            do {
                *p++ = (u8)c;
            } while (--k);
        }

        if (c != 0) {
            c |= (c << 24) | (c << 16) | (c << 8);
        }

        k = n >> 5;
        if (k != 0) {
            do {
                ((u32 *)p)[0] = c;
                ((u32 *)p)[1] = c;
                ((u32 *)p)[2] = c;
                ((u32 *)p)[3] = c;
                ((u32 *)p)[4] = c;
                ((u32 *)p)[5] = c;
                ((u32 *)p)[6] = c;
                ((u32 *)p)[7] = c;
                p += 0x20;
            } while (--k);
        }

        w = (n & 0x1f) >> 2;
        if (w != 0) {
            do {
                *(u32 *)p = c;
                p += 4;
            } while (--w);
        }

        n &= 3;
    }

    if (n != 0) {
        do {
            *p++ = (u8)c;
        } while (--n);
    }
}
