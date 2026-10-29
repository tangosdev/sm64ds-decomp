//cpp
#include "CP15.h"

extern "C" {

/* NitroSDK SND_DestroyWaveArc: unlink every SNDWaveArcLink that still points
 * at this wave archive, clearing each link and flushing it so the ARM7 sees
 * the change. Field names follow the SDK's SNDWaveArc/SNDWaveArcLink. */
typedef struct SNDWaveArcLink {
    void *waveArc;               /* 0x00 */
    struct SNDWaveArcLink *next; /* 0x04 */
} SNDWaveArcLink;

typedef struct SNDWaveArc {
    char pad_00[0x18];       /* fileHeader, blockHeader */
    SNDWaveArcLink *topLink; /* 0x18 */
} SNDWaveArc;

void func_0205b6b0(void *thiz)
{
    SNDWaveArcLink *link = ((SNDWaveArc *)thiz)->topLink;
    if (link == 0)
        return;
    do {
        SNDWaveArcLink *next = link->next;
        link->waveArc = 0;
        link->next = 0;
        CP15::FlushDataCache((unsigned int)link, 8);
        link = next;
    } while (link != 0);
}
}
