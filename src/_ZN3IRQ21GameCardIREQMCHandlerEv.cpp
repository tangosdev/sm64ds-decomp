//cpp
// @symbol _ZN3IRQ21GameCardIREQMCHandlerEv
//
// Migrated to a real namespace definition: the compiler mangles the name, it is
// no longer spelled by hand. See notes/plan-cpp-language-mode.md phase 1
// (layout-free SDK namespaces -- no this, no vtable, no header).
typedef int (*fp)(void);
extern "C" fp data_020a89a4;
extern "C" int func_020610fc(void);

namespace IRQ {

void GameCardIREQMCHandler()
{
    fp cb = data_020a89a4;
    int r = 1;
    *(volatile int*)0x020a89a0 = r;
    if (cb) r = cb();
    if (r == 0) return;
    func_020610fc();
}

}
