//cpp
struct SharedFilePtr { void Release(); };
struct ModelAnim { ~ModelAnim(); };
extern SharedFilePtr data_ov006_02140450, data_ov006_02140460, data_ov006_02140468,
    data_ov006_02140458, data_ov006_02140438, data_ov006_02140440, data_ov006_02140448;
extern int data_ov006_02140430, data_ov006_0214040c, data_ov006_02140424,
    data_ov006_02140408, data_ov006_0214042c;
extern int data_ov006_0213b0cc;
extern int data_ov006_0213afd8;

extern "C" void* func_ov006_020c893c(int* c)
{
    *c = (int)&data_ov006_0213b0cc;
    data_ov006_02140450.Release();
    data_ov006_02140460.Release();
    data_ov006_02140468.Release();
    data_ov006_02140458.Release();
    data_ov006_02140438.Release();
    data_ov006_02140440.Release();
    data_ov006_02140448.Release();
    data_ov006_02140430 = 0;
    data_ov006_0214040c = 0;
    data_ov006_02140424 = 0;
    data_ov006_02140408 = 0;
    data_ov006_0214042c = 0;
    ((ModelAnim*)((char*)c+0x4c))->~ModelAnim();
    *c = (int)&data_ov006_0213afd8;
    return c;
}
