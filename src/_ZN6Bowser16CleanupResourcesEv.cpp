//cpp
// @symbol _ZN6Bowser16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Bowser frees the whole fight. One single, then a 0x1c-entry table and a
 * six-entry table walked by index, then two more singles -- one of which
 * (data_ov089_02132c50) lives in ov089, not this overlay.
 *
 * The two loops are reproduced rather than unrolled: 0x1c and 6 are the counts
 * the ROM's own comparisons test against, and the tables are arrays of
 * POINTERS to handles, unlike the singles which are handles themselves.
 *
 * That is why his siblings release almost nothing -- BowserFire and BowserTail
 * hold no reference at all, and BowserShockwaves shares 0211b208 with him.
 */
#include "Bowser.h"
#include "SharedFilePtr.h"

extern "C" void func_02011cfc(void);
extern char data_ov060_0211ac78;
extern SharedFilePtr *data_ov060_021192dc[];
extern SharedFilePtr *data_ov060_0211927c[];
extern char data_ov060_0211b208;
extern char data_ov089_02132c50;

int Bowser::CleanupResources()
{
    int i;
    ((SharedFilePtr *)(&data_ov060_0211ac78))->Release();
    for (i = 0; i < 0x1c; i++)
        data_ov060_021192dc[i]->Release();
    for (i = 0; i < 6; i++)
        data_ov060_0211927c[i]->Release();
    ((SharedFilePtr *)(&data_ov060_0211b208))->Release();
    ((SharedFilePtr *)(&data_ov089_02132c50))->Release();
    func_02011cfc();
    return 1;
}
