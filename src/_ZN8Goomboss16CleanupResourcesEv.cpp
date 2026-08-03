//cpp
// @symbol _ZN8Goomboss16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Goomboss.h"
#include "SharedFilePtr.h"
extern "C" {
int func_ov074_0212229c(int* c);
void UnloadKeyModels(int i);
extern char data_ov002_0210da30;
extern char data_ov084_02130cf8;
extern char data_ov074_02123000;
extern char data_ov074_02123040;
}

int Goomboss::CleanupResources()
{
    int i;
    int v = ((int*)this)[2];
    if (v == 0x1111) {
        return func_ov074_0212229c(((int*)this));
    }
    ((SharedFilePtr *)(&data_ov002_0210da30))->Release();
    UnloadKeyModels(2);
    ((SharedFilePtr *)(&data_ov084_02130cf8))->Release();
    for (i = 0; i < 7; i++)
        ((SharedFilePtr *)(data_ov074_0212292c[i]))->Release();
    ((SharedFilePtr *)(&data_ov074_02123000))->Release();
    for (i = 0; i < 0xc; i++)
        ((SharedFilePtr *)(data_ov074_02122948[i]))->Release();
    ((SharedFilePtr *)(&data_ov074_02123040))->Release();
    return 1;
}
