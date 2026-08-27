//cpp
// @symbol _ZN14UnknownVsEntry16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "UnknownVsEntry.h"
#include "SharedFilePtr.h"
extern char data_ov075_0211d404[];
extern char data_ov075_0211d3c4[];
extern char data_ov075_0211d414[];
extern char data_ov075_0211d394[];
extern char data_ov075_0211d3cc[];
extern char data_ov075_0211d39c[];
extern char data_ov075_0211d3d4[];
extern char data_ov075_0211d3a4[];
extern char data_ov075_0211d3ec[];
extern char data_ov075_0211d384[];
extern char data_ov075_0211d424[];
extern char data_ov075_0211d42c[];
extern char data_ov075_0211d41c[];
extern char data_ov075_0211d3ac[];
extern char data_ov075_0211d3b4[];
extern char data_ov075_0211d3f4[];
extern char data_ov075_0211d38c[];
extern char data_ov075_0211d3dc[];
extern char data_ov075_0211d40c[];
extern char data_ov075_0211d3fc[];
extern char data_ov075_0211d3bc[];
extern char data_ov075_0211d3e4[];

int UnknownVsEntry::CleanupResources()
{
    CleanCommonModelDataArr();
    ((SharedFilePtr *)(data_ov075_0211d404))->Release();
    ((SharedFilePtr *)(data_ov075_0211d3c4))->Release();
    ((SharedFilePtr *)(data_ov075_0211d414))->Release();
    if (param1 != 1) {
        ((SharedFilePtr *)(data_ov075_0211d394))->Release();
        ((SharedFilePtr *)(data_ov075_0211d3cc))->Release();
        ((SharedFilePtr *)(data_ov075_0211d39c))->Release();
        ((SharedFilePtr *)(data_ov075_0211d3d4))->Release();
        ((SharedFilePtr *)(data_ov075_0211d3a4))->Release();
        ((SharedFilePtr *)(data_ov075_0211d3ec))->Release();
        ((SharedFilePtr *)(data_ov075_0211d384))->Release();
        ((SharedFilePtr *)(data_ov075_0211d424))->Release();
        ((SharedFilePtr *)(data_ov075_0211d42c))->Release();
        ((SharedFilePtr *)(data_ov075_0211d41c))->Release();
    } else {
        ((SharedFilePtr *)(data_ov075_0211d3ac))->Release();
        ((SharedFilePtr *)(data_ov075_0211d3b4))->Release();
        ((SharedFilePtr *)(data_ov075_0211d3f4))->Release();
        ((SharedFilePtr *)(data_ov075_0211d38c))->Release();
        ((SharedFilePtr *)(data_ov075_0211d3dc))->Release();
    }
    ((SharedFilePtr *)(data_ov075_0211d40c))->Release();
    ((SharedFilePtr *)(data_ov075_0211d3fc))->Release();
    if (param1 != 1) {
        ((SharedFilePtr *)(data_ov075_0211d3bc))->Release();
        ((SharedFilePtr *)(data_ov075_0211d3e4))->Release();
    }
    func_ov075_0211b3b8((char*)&unk_e80);
    return 1;
}
