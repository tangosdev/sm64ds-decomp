typedef int s32;

extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);

void func_ov006_02124dc0(void* arg0)
{
    char* c = (char*)arg0;

    *(s32*)(((int)c + 0x51bc)) =
        *(s32*)(((int)c + 0x51bc)) - 1;
    if (*(s32*)(c + 0x51bc) > 0)
        return;
    _ZN5Sound12PlayBank2_2DEj(0x148);
    *(s32*)(((int)c + 0x51b8)) =
        *(s32*)(((int)c + 0x51b8)) + 1;
}
