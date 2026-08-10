extern int RandomIntInternal(int* seed);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* f, int b, int c, unsigned int d);
extern void func_ov006_020c802c(void* p);

extern int data_0209e650;
extern int data_ov006_0213b01c;
extern void* data_ov006_02140424;

void func_ov006_020c8680(char* self)
{
    unsigned int r;
    int r5;

    *(short*)(self + 0x32) -= 1;
    if (*(short*)(self + 0x32) == 0) {
        *(int*)(self + 0x18) = 0x100000;
        r = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        *(int*)(self + 0x14) = ((int)r - 0x800) * 0xc0;
        *(int*)(self + 0x24) = 0;
        r5 = data_ov006_0213b01c;
        r = ((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13;
        *(int*)(self + 0x20) = (int)(((long long)(((int)r - 0x800) << 1) * r5 + 0x800) >> 12);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x4c, data_ov006_02140424, 0x40000000, 0x800, 0);
        func_ov006_020c802c(self);
        return;
    }
    *(int*)(self + 0x18) = 0x100000;
    *(int*)(self + 0x24) = 0;
}
