extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern void func_ov006_020f56f8(char* p);

void func_ov006_020f639c(char* c)
{
    if (*(unsigned short*)(c + 0x5300 + 0xe2)) {
        unsigned short* q = (unsigned short*)(((int)c + 0x53e2));
        *q = *q - 1;
        return;
    }
    if (*(unsigned char*)(c + 0x5000 + 0x405) >= *(unsigned short*)(c + 0x5300 + 0xea)) {
        *(int*)(c + 0x5000 + 0x3d4) = 4;
        *(unsigned short*)(c + 0x5300 + 0xe2) = 0;
        return;
    }
    {
        int count = 0;
        int i = 0;
        char* p = c;
        for (; i < 0x14; i++) {
            if (*(unsigned char*)(p + 0x5000 + 0x1bb) != 0) {
                if (*(unsigned char*)(p + 0x5000 + 0x1bc) == 2) {
                    *(unsigned char*)(p + 0x5000 + 0x1bc) = 3;
                    count++;
                }
            }
            p += 0x18;
        }
        if (count <= 2) {
            _ZN5Sound12PlayBank2_2DEj(0x145);
        } else {
            _ZN5Sound12PlayBank2_2DEj(0x146);
        }
    }
    *(int*)(c + 0x5000 + 0x3d8) = 3;
    func_ov006_020f56f8(c);
    *(unsigned short*)(c + 0x5300 + 0xe2) = 0x20;
}
