extern void func_ov006_0211f454(unsigned char* base, int idx);
extern void func_ov006_0211f34c(unsigned char* base, int idx);
extern void func_ov006_0211e8a8(unsigned char* base, int idx);
extern short data_02082214[];

void func_ov006_0211f0d0(unsigned char* base, int idx)
{
    int off = idx * 0x24;
    unsigned short* cnt = (unsigned short*)((unsigned char*)(base + 0x466e) + off);
    if (*cnt != 0) {
        *cnt = *cnt - 1;
        if (*(short*)cnt <= 0) {
            *cnt = 0;
            *(unsigned char*)((unsigned char*)(base + 0x4678) + off) = 3;
            return;
        }
    }
    func_ov006_0211f454(base, idx);
    func_ov006_0211f34c(base, idx);
    {
        int v = *(unsigned short*)((unsigned char*)(base + 0x466c) + off);
        short look = data_02082214[(v >> 4) * 2 + 1];
        if (look >= 0)
            *(unsigned char*)((unsigned char*)(base + 0x467e) + off) = 1;
        else
            *(unsigned char*)((unsigned char*)(base + 0x467e) + off) = 0;
    }
    func_ov006_0211e8a8(base, idx);
}
