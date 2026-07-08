extern void* _Znwj(unsigned int);
extern void _ZN3Fog4InitEt5Fix12IiES1_(void* thiz, unsigned short a, int b, int d);
extern short data_ov002_02110af4;
extern void* ENDLESS_STAIRS_FOG_PTR;
extern signed char ENDLESS_STAIRS_FOG_AREA_ID;

int func_ov002_020f1eb4(char* c)
{
    void* p;
    *(int*)(c + 8) &= 0xf;
    if (*(int*)(c + 8) != 1) {
        data_ov002_02110af4 = *(short*)(c + 0x8e);
        p = _Znwj(0x28);
        ENDLESS_STAIRS_FOG_PTR = p;
        if (p != 0) {
            _ZN3Fog4InitEt5Fix12IiES1_(p, 0, 0x700, 0xd00);
        }
        ENDLESS_STAIRS_FOG_AREA_ID = *(signed char*)(c + 0xcc);
    }
    c[0xcc] = -1;
    *(short*)(c + 0x8e) = -*(short*)(c + 0x8e);
    return 1;
}
