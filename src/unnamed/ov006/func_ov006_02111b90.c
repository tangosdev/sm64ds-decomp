extern void Vec2_Sub(int* o, int* a);
extern int Vec2_Len(int* v);
extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
extern void func_ov006_02115248(int a, int* p);

void func_ov006_02111b90(char* self, int a1) {
    int spd;

    if (*(int*)(self + 0x124) > 0) {
        *(int*)(self + 0x124) = 3;
        return;
    }
    {
        int v[2];
        Vec2_Sub(v, (int*)(self + 0x20));
        spd = Vec2_Len(v);
    }
    if (spd < 0x1000)
        return;
    switch (a1) {
    case 0x16f: {
        int vol = (spd << 4) >> 12;
        if (vol > 0x7f) vol = 0x7f;
        func_020126ac(a1, 5, vol, 0, func_020126e8(*(int*)(self + 8)));
        {
            int pos[2];
            pos[0] = *(int*)(self + 8);
            pos[1] = *(int*)(self + 0xc);
            func_ov006_02115248(*(int*)(self + 4), pos);
        }
        break;
    }
    case 0x171: {
        int vol = (((spd - 0x800) << 2) >> 12) + 0x32;
        if (vol > 0x7f) vol = 0x7f;
        func_020126ac(a1, 5, vol, 0, func_020126e8(*(int*)(self + 8)));
        break;
    }
    case 0x172: {
        int vol = (((spd - 0x800) << 2) >> 12) + 0x32;
        if (vol > 0x7f) vol = 0x7f;
        func_020126ac(a1, 5, vol, 0, func_020126e8(*(int*)(self + 8)));
        break;
    }
    case 0x1a3: {
        int vol = (((spd - 0x800) << 2) >> 12) + 0x32;
        if (vol > 0x7f) vol = 0x7f;
        func_020126ac(a1, 5, vol, 0, func_020126e8(*(int*)(self + 8)));
        break;
    }
    }
    *(int*)(self + 0x124) = 3;
}
