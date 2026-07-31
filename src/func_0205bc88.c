typedef unsigned short u16;
typedef unsigned char u8;

extern void FS_InitFile(int* s);
extern void func_0205c4e4(void* self, int value);
extern int func_0205c5e4(void* self, int x);
extern void CpuCopy8(const void* src, void* dst, unsigned int size);
extern const unsigned char data_0209a044[];

int func_0205bc88(char* c)
{
    char* g0;
    char tok[0x94];
    char it[0x44];
    char* m;
    register int curr;
    register int w;
    unsigned int i;
    unsigned int cnt;
    int b;

    g0 = *(char**)(c + 8);
    m = c + 0x2c;
    FS_InitFile((int*)it);
    *(char**)(it + 8) = *(char**)(c + 8);

    b = (*(int*)(c + 0xc) & 0x20) ? 1 : 0;
    if (b != 0) {
        curr = *(u16*)(c + 0x20);
        w = 0x10000;
    } else {
        w = *(int*)(c + 0x1c);
        if (*(u16*)(m + 8) != 0) {
            curr = *(u16*)(m + 0xa);
        } else {
            curr = 0x10000;
            i = 0;
            cnt = 0;
            do {
                func_0205c4e4(it, i);
                if (i == 0)
                    cnt = *(int*)(it + 0x28);
                *(void**)(it + 0x2c) = tok;
                *(int*)(it + 0x30) = 1;
                if (func_0205c5e4(it, 3) == 0) {
                    for (;;) {
                        if (*(int*)(tok + 0xc) == 0 &&
                            *(int*)(tok + 4) == w) {
                            curr = *(u16*)(it + 0x20);
                            break;
                        }
                        if (func_0205c5e4(it, 3) != 0)
                            break;
                    }
                }
                if (curr != 0x10000)
                    break;
                i++;
            } while (i < cnt);
        }
    }

    if (curr == 0x10000) {
        *(u16*)(m + 8) = 0;
        return 1;
    }

    if (*(u16*)(m + 8) == 0) {
        cnt = 0;
        if (*(unsigned int*)g0 <= 0xff)
            cnt += 1;
        else if (*(unsigned int*)g0 <= 0xff00)
            cnt += 2;
        else
            cnt += 3;
        cnt += 2;
        if (w != 0x10000)
            cnt += *(int*)(tok + 0x10);

        i = curr;
        if (curr != 0) {
            func_0205c4e4(it, curr);
            do {
                func_0205c4e4(it, *(int*)(it + 0x28));
                *(void**)(it + 0x2c) = tok;
                *(int*)(it + 0x30) = 1;
                if (func_0205c5e4(it, 3) == 0) {
                    for (;;) {
                        if (*(int*)(tok + 0xc) != 0 &&
                            *(u16*)(tok + 4) == i) {
                            cnt += *(int*)(tok + 0x10) + 1;
                            break;
                        }
                        if (func_0205c5e4(it, 3) != 0)
                            break;
                    }
                }
                i = *(u16*)(it + 0x20);
            } while (i != 0);
        }
        *(u16*)(m + 8) = (u16)(cnt + 1);
        *(u16*)(m + 0xa) = (u16)curr;
    }

    {
        char* buf = *(char**)(m);
        int count;

        if (buf == 0)
            return 0;
        count = *(u16*)(m + 8);
        if ((unsigned)*(int*)(m + 4) < (unsigned)count)
            return 1;

        {
            int pos;
            int nb;

            pos = 0;
            if (*(unsigned int*)g0 <= 0xff)
                nb = 1;
            else if (*(unsigned int*)g0 <= 0xff00)
                nb = 2;
            else
                nb = 3;

            CpuCopy8(g0, buf, nb);
            pos += nb;
            CpuCopy8(data_0209a044, buf + pos, 2);
        }

        func_0205c4e4(it, curr);
        if (w != 0x10000) {
            *(void**)(it + 0x2c) = tok;
            *(int*)(it + 0x30) = 0;
            if (func_0205c5e4(it, 3) == 0) {
                for (;;) {
                    if (*(int*)(tok + 0xc) == 0 &&
                        *(int*)(tok + 4) == w)
                        break;
                    if (func_0205c5e4(it, 3) != 0)
                        break;
                }
            }
            {
                int k = *(int*)(tok + 0x10) + 1;
                CpuCopy8(tok + 0x14, (buf + count) - k, k);
                count -= k;
            }
        } else {
            *(char*)(buf + count - 1) = 0;
            count -= 1;
        }

        if (curr != 0) {
            do {
                func_0205c4e4(it, *(int*)(it + 0x28));
                *(void**)(it + 0x2c) = tok;
                *(int*)(it + 0x30) = 0;
                *(char*)(buf + count - 1) = 0x2f;
                count--;
                if (func_0205c5e4(it, 3) == 0) {
                    for (;;) {
                        if (*(int*)(tok + 0xc) != 0 &&
                            *(u16*)(tok + 4) == curr) {
                            int ln = *(int*)(tok + 0x10);
                            CpuCopy8(tok + 0x14,
                                          (buf + count) - ln, ln);
                            count -= ln;
                            break;
                        }
                        if (func_0205c5e4(it, 3) != 0)
                            break;
                    }
                }
                curr = *(u16*)(it + 0x20);
            } while (curr != 0);
        }
        return 0;
    }
}
