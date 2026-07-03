//cpp
extern "C" {
extern void func_ov002_020c0364(char* c, unsigned int arg);
extern int func_ov002_020c5244(void);
extern int func_ov002_020c6adc(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* state);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* anim, int frame);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern void func_ov002_020bcdf0(void* c);
extern int func_ov002_020d2fdc(void* c);
extern int func_ov002_020d22ec(char* c, int arg);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int d, unsigned int e);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int a, void* v);
extern int func_ov002_020d2da0(void* c);
extern void func_ov002_020cabe0(void* c);
extern void func_ov002_020d2f24(void* c);
extern void func_ov002_020d2e74(void* c);
extern void func_ov002_020bedd4(void* c);

extern int data_ov002_0211049c[];
extern void* data_0209f318;
extern unsigned char data_020a0e40;
extern unsigned char data_0209f49c[];
extern char data_0209f4a0[];
extern unsigned char data_0209f4ae[];
extern unsigned char data_0209f2d8;
extern int data_0209caa0[];
extern unsigned char data_0209f49e[];
extern int data_ov002_0211019c[];
extern int data_ov002_0211013c[];

int _ZN6Player12St_Wait_MainEv(char* c)
{
    if ((unsigned short)(*(unsigned short*)(c+0x6ce) & 1)) {
        func_ov002_020c0364(c, 3);
        return 1;
    }
    if (func_ov002_020c5244()) {
        return 1;
    }
    if (func_ov002_020c6adc(c)) {
        return 1;
    }
    if (*(int*)(c+0x68c) >= 0x1e000) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211049c);
        return 1;
    }

    {
    unsigned char st = *(unsigned char*)(c+0x6e3);
    void* r4 = data_0209f318;
    switch (st) {
    default:
        break;

    case 8: {
        if (!(*(unsigned short*)(data_0209f49c+data_020a0e40*0x18) & 0x800) &&
            _ZNK9Animation12WillHitFrameEi((char*)(*(void**)(c+(_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0)<<2)+0xdc))+0x50, 0)) {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2c, c+0x74);
        }
    }
    case 0: {
        unsigned char f2d8_0 = data_0209f2d8;
        int b0 = (f2d8_0 == 1);
        if (b0 == 0) {
            if (data_0209caa0[2] & 0x80) {
                int b1 = (f2d8_0 == 2);
                if (b1) goto blk0_34;
                goto blk0_35;
            }
            blk0_34:
            func_ov002_020bcdf0(c);
            goto blk0_36;
        }
        blk0_35:
        if (func_ov002_020d2fdc(c) == 0) {
            blk0_36:
            {
                int idx0 = data_020a0e40 * 0x18;
                short lvl0 = *(short*)(data_0209f4a0+idx0);
                if (lvl0 != 0) {
                    int thr0 = (*(unsigned char*)(data_0209f4ae+idx0)!=2) ? 0x471 : 0x555;
                    if (lvl0 < thr0) {
                        *(short*)(c+0x94) = *(short*)(c+0x6d2);
                        *(short*)(c+0x8e) = *(short*)(c+0x94);
                    }
                }
                {
                    int thr0b = (*(unsigned char*)(data_0209f4ae+idx0)!=2) ? 0x471 : 0x555;
                    func_ov002_020d22ec(c, thr0b);
                }
            }
        }
        break;
    }

    case 10: {
        if (*(unsigned char*)(c+0x6f9)!=0 || *(unsigned char*)(c+0x6fb)!=0) {
            *(unsigned char*)(c+0x6e3) = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
            *(unsigned short*)(c+0x6a4) = 0x384;
        }
        {
            int idx10 = data_020a0e40 * 0x18;
            short lvl10 = *(short*)(data_0209f4a0+idx10);
            if (lvl10 != 0) {
                int thr10 = (*(unsigned char*)(data_0209f4ae+idx10)!=2) ? 0x471 : 0x555;
                if (lvl10 < thr10) {
                    *(short*)(c+0x94) = *(short*)(c+0x6d2);
                    *(short*)(c+0x8e) = *(short*)(c+0x94);
                }
            }
            {
                int thr10b = (*(unsigned char*)(data_0209f4ae+idx10)!=2) ? 0x471 : 0x555;
                func_ov002_020d22ec(c, thr10b);
            }
        }
        break;
    }

    case 1: {
        if (_ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xc, 0, 0x1000, 0);
            *(unsigned char*)(c+0x6e3) = 2;
        }
        {
        unsigned int x1 = *(unsigned int*)(c+8);
        switch (x1) {
        default:
            break;
        case 0: {
            int idA = _ZNK6Player14GetBodyModelIDEjb(c, x1&0xff, 0);
            void* animA = *(void**)(c+(idA<<2)+0xdc);
            if (_ZNK9Animation12WillHitFrameEi((char*)animA+0x50, 0x5a)) {
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2d, c+0x74);
            } else if (_ZNK9Animation12WillHitFrameEi((char*)(*(void**)(c+(_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0)<<2)+0xdc))+0x50, 0x3a)
                    || _ZNK9Animation12WillHitFrameEi((char*)(*(void**)(c+(_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0)<<2)+0xdc))+0x50, 0x44)) {
                _ZN5Sound9PlayBank0EjRK7Vector3(4, c+0x74);
            }
            break;
        }
        case 1: {
            int idD = _ZNK6Player14GetBodyModelIDEjb(c, x1&0xff, 0);
            void* animD = *(void**)(c+(idD<<2)+0xdc);
            if (_ZNK9Animation12WillHitFrameEi((char*)animD+0x50, 0x6e)) {
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2d, c+0x74);
            } else {
                int idE = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8)&0xff, 0);
                void* animE = *(void**)(c+(idE<<2)+0xdc);
                if (_ZNK9Animation12WillHitFrameEi((char*)animE+0x50, 0x32)) {
                    _ZN5Sound9PlayBank0EjRK7Vector3(0xb1, c+0x74);
                }
            }
            break;
        }
        case 2: {
            int idF = _ZNK6Player14GetBodyModelIDEjb(c, x1&0xff, 0);
            void* animF = *(void**)(c+(idF<<2)+0xdc);
            if (_ZNK9Animation12WillHitFrameEi((char*)animF+0x50, 0x20)) {
                _ZN5Sound9PlayBank0EjRK7Vector3(2, c+0x74);
            } else if (_ZNK9Animation12WillHitFrameEi((char*)(*(void**)(c+(_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0)<<2)+0xdc))+0x50, 0x47)
                    || _ZNK9Animation12WillHitFrameEi((char*)(*(void**)(c+(_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0)<<2)+0xdc))+0x50, 0x60)) {
                _ZN5Sound9PlayBank0EjRK7Vector3(3, c+0x74);
            }
            break;
        }
        case 3: {
            int idI = _ZNK6Player14GetBodyModelIDEjb(c, x1&0xff, 0);
            void* animI = *(void**)(c+(idI<<2)+0xdc);
            if (_ZNK9Animation12WillHitFrameEi((char*)animI+0x50, 0xad)) {
                _ZN5Sound9PlayBank0EjRK7Vector3(0x50, c+0x74);
            }
            break;
        }
        }
        }
        if (func_ov002_020d22ec(c, 0)) {
            (*(unsigned int*)(((int)r4 + 0x154) & 0xFFFFFFFFFFFFFFFF)) &= ~0x2000;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2e, c+0x74);
        }
        break;
    }

    case 2: {
        if (*(unsigned char*)(c+0x723)==0 &&
            _ZNK9Animation12WillHitFrameEi((char*)(*(void**)(c+(_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0)<<2)+0xdc))+0x50, 1)) {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2a, c+0x74);
        }
        {
            unsigned char f2d8_2 = data_0209f2d8;
            int b2a = (f2d8_2 == 1);
            if (b2a == 0) {
                if ((data_0209caa0[2] & 0x80) == 0) break;
                int b2b = (f2d8_2 == 2);
                if (b2b != false) break;
            }
        }
        if (*(unsigned short*)(c+0x6a4) == 0) {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2e, c+0x74);
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa, 0x40000000, 0x1000, 0);
            *(unsigned char*)(c+0x6e3) = 3;
            *(unsigned char*)(c+0x721) = 1;
        }
        if (*(unsigned short*)(data_0209f49e+data_020a0e40*0x18) & 2) {
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211019c);
            (*(unsigned int*)(((int)r4 + 0x154) & 0xFFFFFFFFFFFFFFFF)) &= ~0x2000;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2e, c+0x74);
            return 1;
        }
        if (func_ov002_020d2da0(c)==1 || (unsigned short)(*(unsigned short*)(c+0x6ce)&4)!=0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xb, 0x40000000, 0x1000, 0);
            *(unsigned char*)(c+0x6e3) = 9;
        }
        break;
    }

    case 3: {
        if (_ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 9, 0, 0x1000, 0);
            *(unsigned char*)(c+0x6e3) = 4;
            *(int*)(c+0x56c) = 0;
            *(int*)(c+0x570) = 0;
            *(int*)(c+0x574) = 0;
            *(unsigned char*)(c+0x721) = 2;
            *(unsigned char*)(c+0x70c) = 1;
        }
        if ((unsigned short)(*(unsigned short*)(c+0x6ce)&4) != 0) {
            func_ov002_020cabe0(c);
            return 1;
        }
        break;
    }

    case 5: {
        if (_ZN6Player12FinishedAnimEv(c)) {
            *(unsigned char*)(c+0x6e3) = 6;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x96, 0, 0x1000, 0);
        }
        func_ov002_020d22ec(c, 0);
        break;
    }

    case 4: {
        if (*(unsigned char*)(c+0x6e6) == 0) {
            func_ov002_020d2f24(c);
            if (*(unsigned char*)(c+0x723)==0 &&
                _ZNK9Animation12WillHitFrameEi((char*)(*(void**)(c+(_ZNK6Player14GetBodyModelIDEjb(c,*(unsigned int*)(c+8)&0xff,0)<<2)+0xdc))+0x50, 0xd)) {
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x2b, c+0x74);
            }
        }
        {
            unsigned char f2d8_4 = data_0209f2d8;
            int b4a = (f2d8_4 == 1);
            if (b4a == 0) {
                if ((data_0209caa0[2] & 0x80) == 0) break;
                int b4b = (f2d8_4 == 2);
                if (b4b != false) break;
            }
            if (func_ov002_020d2da0(c)==1 || *(int*)(c+0x56c) >= 0x3000 || *(unsigned char*)(c+0x707)!=0 ||
                (unsigned short)(*(unsigned short*)(c+0x6ce)&4)) {
                func_ov002_020d2e74(c);
                *(unsigned char*)(c+0x721) = 0;
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 8, 0x40000000, 0x1000, 0);
                *(unsigned char*)(c+0x6e3) = 9;
            }
        }
        break;
    }

    case 6: {
        if (func_ov002_020d2da0(c) == 1) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x97, 0x40000000, 0x1000, 0);
            *(unsigned char*)(c+0x6e3) = 9;
        }
        break;
    }

    case 7: {
        if (_ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xf, 0, 0x1000, 0);
            *(unsigned char*)(c+0x6e3) = 8;
        }
        if ((unsigned short)(*(unsigned short*)(c+0x6ce)&0x800) == 0) {
            func_ov002_020d22ec(c, 0);
        }
        break;
    }

    case 9: {
        if (_ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        }
        break;
    }
    }
    }

    func_ov002_020bedd4(c);
    return 1;
}
}
