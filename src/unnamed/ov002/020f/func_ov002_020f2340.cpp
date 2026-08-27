//cpp
struct OamAttr;
struct OAM { static void RenderSub(OamAttr *, int, int); };
extern OamAttr data_ov002_0210bddc;

extern "C" void func_ov002_020f2340(char *thiz)
{
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, *(unsigned short *)(*(char **)(thiz + 0xd4) + 8));
    OAM::RenderSub(&data_ov002_0210bddc, 0x80, *(unsigned short *)(*(char **)(thiz + 0xd4) + 0xa));
}
