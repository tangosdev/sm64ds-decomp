typedef int s32;

extern void* data_020890a0;

extern void _ZN5Sound16LoadInitialGroupEi(s32 group);
extern void RunKuppaScript(void* script);

void StartIntroCutscene(void)
{
    _ZN5Sound16LoadInitialGroupEi(0x25);
    RunKuppaScript(&data_020890a0);
}
