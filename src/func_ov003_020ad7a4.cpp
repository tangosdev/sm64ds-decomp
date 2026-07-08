//cpp
extern "C" {
extern int SELECTED_LINE;
extern int LINE_SCROLL_POSITION;
extern int _ZN3OAM12objCountMainE;
extern char _ZN3OAM10bufferMainE[];
}
struct OamTmp { int x; };
namespace OAM { void EnableSubOAM(OamTmp*); }
extern "C" int func_ov003_020ad7a4(void)
{
    OamTmp tmp;
    OAM::EnableSubOAM(&tmp);
    {
        int idx = _ZN3OAM12objCountMainE;
        unsigned int v = ((SELECTED_LINE - LINE_SCROLL_POSITION + 2) << 3) & 0xff;
        char* p = _ZN3OAM10bufferMainE + (idx << 3);
        *(int*)p = v | 0x80000;
        *(short*)(p + 4) = 0;
    }
    return 1;
}
