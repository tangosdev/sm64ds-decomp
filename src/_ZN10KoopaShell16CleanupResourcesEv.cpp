//cpp
// @symbol _ZN10KoopaShell16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Releases the shared model file this shell claimed in InitResources, picked
 * by the same mModelIndex, so the claim and the release stay paired.
 */
#include "KoopaShell.h"
#include "SharedFilePtr.h"

extern "C" {
extern int data_ov102_0214d70c[];
}

int KoopaShell::CleanupResources()
{
  unsigned char i = mModelIndex;
  ((SharedFilePtr *)((void*)data_ov102_0214d70c[i]))->Release();
  return 1;
}
