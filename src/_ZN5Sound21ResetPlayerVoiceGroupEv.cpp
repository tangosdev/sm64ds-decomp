//cpp
// @symbol _ZN5Sound21ResetPlayerVoiceGroupEv
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
extern "C" {
extern int func_0203d974(void);
extern int func_020510a4(int a, int b);
extern int data_0209b498;
extern int data_0209b484;
extern unsigned char data_0209b478;
}

namespace Sound {

void ResetPlayerVoiceGroup(void)
{
    if (func_0203d974() == 0) {
        func_020510a4(data_0209b498, data_0209b484);
    }
    data_0209b478 = 0;
}

}
