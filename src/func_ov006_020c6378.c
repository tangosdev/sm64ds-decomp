typedef short s16;
typedef unsigned short u16;

extern s16 SINE_TABLE[];
extern void func_ov006_020c49d8(void* this);

void func_ov006_020c6378(int this)
{
    s16 *ph = (s16*)(((int)this + 0xea) & 0xFFFFFFFFFFFFFFFF);
    int *pa = (int*)(((int)this + 0xa0) & 0xFFFFFFFFFFFFFFFF);
    *ph += 0x200;
    *(int*)(this + 0xa8) = 0;
    *(int*)(this + 0xac) = SINE_TABLE[(*(u16*)(this + 0xea) >> 4) * 2] >> 3;
    *pa += *(int*)(this + 0xac);
    func_ov006_020c49d8((void*)this);
}
