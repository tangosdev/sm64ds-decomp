//cpp
// @symbol _ZN6Rabbit16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Seven releases, straight-line, no loop -- the ROM writes them out one after
 * another and so does this. They are not in address order, which is why the
 * sequence is reproduced literally rather than tidied into a table walk.
 *
 * The first, data_ov085_021305d8, is the handle RabbitKey releases too.
 */
#include "Rabbit.h"
#include "SharedFilePtr.h"

extern char data_ov085_021305d8;
extern char data_ov085_021305b8;
extern char data_ov085_021305d0;
extern char data_ov085_021305b0;
extern char data_ov085_021305c8;
extern char data_ov085_021305c0;
extern char data_ov085_021305e0;

int Rabbit::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_021305d8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305b8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305d0))->Release();
    ((SharedFilePtr *)(&data_ov085_021305b0))->Release();
    ((SharedFilePtr *)(&data_ov085_021305c8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305c0))->Release();
    ((SharedFilePtr *)(&data_ov085_021305e0))->Release();
    return 1;
}
