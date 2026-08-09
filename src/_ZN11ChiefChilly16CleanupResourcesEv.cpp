//cpp
// @symbol _ZN11ChiefChilly16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Releases NINE shared files and unloads the key models -- by far the largest
 * claim of any class migrated so far, and one of the nine (data_ov002_0210da30)
 * lives in ov002 rather than this overlay, borrowed from the shared pool and
 * still released here.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free.
 */
#include "ChiefChilly.h"
#include "SharedFilePtr.h"
extern "C" {
extern void UnloadKeyModels(int i);
extern void* data_ov073_02123280;
extern void* data_ov073_021232a0;
extern void* data_ov073_02123288;
extern void* data_ov073_021232a8;
extern void* data_ov073_02123290;
extern void* data_ov073_021232b0;
extern void* data_ov073_021232b8;
extern void* data_ov002_0210da30;
extern void* data_ov073_02123298;

}

int ChiefChilly::CleanupResources(){
  UnloadKeyModels(4);
  ((SharedFilePtr *)(&data_ov073_02123280))->Release();
  ((SharedFilePtr *)(&data_ov073_021232a0))->Release();
  ((SharedFilePtr *)(&data_ov073_02123288))->Release();
  ((SharedFilePtr *)(&data_ov073_021232a8))->Release();
  ((SharedFilePtr *)(&data_ov073_02123290))->Release();
  ((SharedFilePtr *)(&data_ov073_021232b0))->Release();
  ((SharedFilePtr *)(&data_ov073_021232b8))->Release();
  ((SharedFilePtr *)(&data_ov002_0210da30))->Release();
  ((SharedFilePtr *)(&data_ov073_02123298))->Release();
  return 1;
}
