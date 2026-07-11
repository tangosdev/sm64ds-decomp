void func_ov006_02106910(char* base, int idx)
{
    int old = ((int*)(base + 0x4d54))[idx];
    ((int*)(base + 0x4d54))[idx] = old + 0x10000;
    if ((*(int*)(base + idx * 4 + 0x4d54) >> 12) >= 0xd0) {
        *(unsigned char*)(base + idx + 0x4efa) = 0;
        *(unsigned char*)(base + idx + 0x4f66) = 0;
    }

    {
        unsigned char* flag = (unsigned char*)(base + 0x4f8a + idx);
        if (*flag != 0) return;
        {
        int r = idx;
        int q = 0;
        int div = *(int*)(base + 0x4cbc);
        if (idx >= div) {
            do {
                r -= div;
                q++;
            } while (r >= div);
        }
        if (q == div - 1) {
            *flag = 1;
            return;
        }
        {
            int nxt = (q + 1) * div + r;
            if (((*(int*)(base + nxt * 4 + 0x4d54) - *(int*)(base + (unsigned int)idx * 4 + 0x4d54)) >> 12) > 0) return;
            *flag = 1;
            *(unsigned char*)(base + nxt + 0x4efa) = 3;
        }
        }
    }
}
