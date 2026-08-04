//cpp
struct SharedFilePtr { void Release(); };
struct ModelAnim { ~ModelAnim(); };
extern SharedFilePtr data_ov006_021405f8, data_ov006_02140608, data_ov006_021405d0,
    data_ov006_02140628, data_ov006_02140618, data_ov006_02140638, data_ov006_021405f0,
    data_ov006_021405e8, data_ov006_02140600, data_ov006_021405e0, data_ov006_02140610,
    data_ov006_021405d8, data_ov006_02140620;
extern int data_ov006_02140590, data_ov006_02140560, data_ov006_02140580,
    data_ov006_021405c0, data_ov006_0214054c, data_ov006_02140564, data_ov006_021405a0,
    data_ov006_0214057c, data_ov006_0214056c, data_ov006_02140568, data_ov006_0214059c,
    data_ov006_02140540;
extern int data_ov006_0213b2c4;
extern int data_ov006_0213b3c4;

extern "C" void* func_ov006_020ccfc8(int* c)
{
    *c = (int)&data_ov006_0213b2c4;
    data_ov006_021405f8.Release();
    data_ov006_02140608.Release();
    data_ov006_021405d0.Release();
    data_ov006_02140628.Release();
    data_ov006_02140618.Release();
    data_ov006_02140638.Release();
    data_ov006_021405f0.Release();
    data_ov006_021405e8.Release();
    data_ov006_02140600.Release();
    data_ov006_021405e0.Release();
    data_ov006_02140610.Release();
    data_ov006_021405d8.Release();
    data_ov006_02140620.Release();
    data_ov006_02140590 = 0;
    data_ov006_02140560 = 0;
    data_ov006_02140580 = 0;
    data_ov006_021405c0 = 0;
    data_ov006_0214054c = 0;
    data_ov006_02140564 = 0;
    data_ov006_021405a0 = 0;
    data_ov006_0214057c = 0;
    data_ov006_0214056c = 0;
    data_ov006_02140568 = 0;
    data_ov006_0214059c = 0;
    data_ov006_02140540 = 0;
    ((ModelAnim*)((char*)c+0x6c))->~ModelAnim();
    *c = (int)&data_ov006_0213b3c4;
    return c;
}
