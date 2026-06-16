//cpp
extern "C" {
extern int data_0209b2f4;
extern int data_0209b2f8;
extern int data_0209e664;
extern char data_0209e674[];
}
struct OamTmp { int x; };
namespace OAM { void EnableSubOAM(OamTmp*); }
extern "C" int func_ov003_020ad7a4(void)
{
    OamTmp tmp;
    OAM::EnableSubOAM(&tmp);
    {
        int idx = data_0209e664;
        unsigned int v = ((data_0209b2f4 - data_0209b2f8 + 2) << 3) & 0xff;
        char* p = data_0209e674 + (idx << 3);
        *(int*)p = v | 0x80000;
        *(short*)(p + 4) = 0;
    }
    return 1;
}
