//cpp
extern "C" int data_ov006_0213ca1c;
extern "C" void _ZN5Sound12PlayBank2_2DEj(unsigned int);

#pragma optimize_for_size on

extern "C" void func_ov006_020eb558(int *thiz)
{
    int *g = &data_ov006_0213ca1c;
    int v0 = thiz[0];
    if (v0 == g[0] && (thiz[1] == g[1] || v0 == 0) && thiz[0x70/4] == 0) {
        short h = *(short*)((char*)thiz + 0x84);
        if (h < 0 && (short)(h + *(short*)((char*)thiz + 0x86)) > 0) {
            _ZN5Sound12PlayBank2_2DEj(0x1e8);
        } else if (h > 0 && (short)(h + *(short*)((char*)thiz + 0x86)) < 0) {
            _ZN5Sound12PlayBank2_2DEj(0x1e7);
        }
    }
    {
        short *p = (short*)(((int)thiz + 0x84) & 0xFFFFFFFFFFFFFFFFull);
        *p = *p + *(short*)((char*)thiz + 0x86);
    }
}
