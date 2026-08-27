typedef struct { int w[2]; } SharedFilePtr;
typedef struct BMD_File BMD_File;

extern SharedFilePtr data_ov006_021405f8;
extern SharedFilePtr data_ov006_02140608;
extern SharedFilePtr data_ov006_021405d0;
extern SharedFilePtr data_ov006_02140628;
extern SharedFilePtr data_ov006_02140618;
extern SharedFilePtr data_ov006_02140638;
extern SharedFilePtr data_ov006_021405f0;
extern SharedFilePtr data_ov006_021405e8;
extern SharedFilePtr data_ov006_02140600;
extern SharedFilePtr data_ov006_021405e0;
extern SharedFilePtr data_ov006_02140610;
extern SharedFilePtr data_ov006_021405d8;
extern SharedFilePtr data_ov006_02140620;
extern SharedFilePtr data_ov006_02140630;

extern void* data_ov006_02140590;
extern void* data_ov006_02140560;
extern void* data_ov006_02140580;
extern void* data_ov006_021405c0;
extern void* data_ov006_0214054c;
extern void* data_ov006_02140564;
extern void* data_ov006_021405a0;
extern void* data_ov006_0214057c;
extern void* data_ov006_0214056c;
extern void* data_ov006_02140568;
extern void* data_ov006_0214059c;
extern void* data_ov006_02140540;
extern void* data_ov006_021405c4;

extern char* data_ov006_02141a40;

extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void func_ov006_020bfec0(char* p, void* q, short* s);
extern void func_02016a14(void* self, int a, int b);
extern void func_02016a04(void* self, int a);
extern void func_ov006_020ca3a8(char* c);

int func_ov006_020ca430(char* c)
{
    BMD_File* f;
    int ret;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov006_021405f8);
    data_ov006_02140590 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140608);
    data_ov006_02140560 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_021405d0);
    data_ov006_02140580 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140628);
    data_ov006_021405c0 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140618);
    data_ov006_0214054c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140638);
    data_ov006_02140564 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_021405f0);
    data_ov006_021405a0 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_021405e8);
    data_ov006_0214057c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140600);
    data_ov006_0214056c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_021405e0);
    data_ov006_02140568 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140610);
    data_ov006_0214059c = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_021405d8);
    data_ov006_02140540 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140620);
    data_ov006_021405c4 = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02140630);
    ret = _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x78, f, 1, -1);
    *(int*)(c + 0x60) = 0;
    if (data_ov006_02141a40 != 0)
        func_ov006_020bfec0(data_ov006_02141a40, c + 0x24, (short*)(c + 0x56));
    *(int*)(c + 0x48) = 0;
    func_02016a14(c + 0x78, 0x73ff, 0);
    func_02016a04(c + 0x78, 0x3e52);
    func_ov006_020ca3a8(c);
    return ret;
}
