extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

#define F0(b,i)  (*(int*)  ((char*)(b) + 0x48c0 + (i)*0x24))
#define F4(b,i)  (*(int*)  ((char*)(b) + 0x48c4 + (i)*0x24))
#define F8(b,i)  (*(int*)  ((char*)(b) + 0x48c8 + (i)*0x24))
#define FC(b,i)  (*(int*)  ((char*)(b) + 0x48cc + (i)*0x24))
#define F14(b,i) (*(unsigned short*)((char*)(b) + 0x48d4 + (i)*0x24))
#define F14S(b,i) (*(short*)((char*)(b) + 0x48d4 + (i)*0x24))
#define F16(b,i) (*(unsigned short*)((char*)(b) + 0x48d6 + (i)*0x24))
#define F16S(b,i) (*(short*)((char*)(b) + 0x48d6 + (i)*0x24))
#define F1E(b,i) (*(unsigned char*)((char*)(b) + 0x48de + (i)*0x24))

void func_ov006_020e3f54(char *base, int idx)
{
    unsigned short v;

    F0(base, idx) = F0(base, idx) + F8(base, idx);
    F4(base, idx) = F4(base, idx) + FC(base, idx);

    if (F14(base, idx) != 0) {
        F14(base, idx) = F14(base, idx) - 1;
        if (F14S(base, idx) < 0)
            F14(base, idx) = 0;
        return;
    }

    if (F8(base, idx) > -0x300) {
        F8(base, idx) -= 8;
        if (F8(base, idx) <= -0x300)
            F8(base, idx) = 0x300;
    }

    v = F16(base, idx);
    if (v != 0) {
        F16(base, idx) = v - 1;
        if (F16S(base, idx) < 0)
            F16(base, idx) = 0;
        return;
    }

    F1E(base, idx) = 3;
    F16(base, idx) = (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf) + 0x20);
}
