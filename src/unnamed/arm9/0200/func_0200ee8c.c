typedef signed char s8;
typedef int s32;

extern s8 data_0209f224;
extern void* data_020876e4[];

extern s32 GetStarCameraSetting(s32 star);
extern void RunKuppaScript(void* script);

void func_0200ee8c(s32 arg0)
{
    s32 idx;
    if (arg0 < 0)
    {
        arg0 = GetStarCameraSetting((s32)data_0209f224);
    }
    RunKuppaScript(data_020876e4[arg0]);
}
