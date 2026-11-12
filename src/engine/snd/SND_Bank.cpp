//cpp
/* NitroSDK SND_bank.c: the links between banks and wave archives. A bank
 * can use up to four wave archives. Every bank that uses a wave archive
 * has a link to it, and the links are chained from the archive's topLink,
 * so the archive can find every bank that uses it. Each change is flushed
 * from the data cache, because the ARM7 driver reads the same memory.
 *
 * pret's pokediamond SND_bank.c defines SND_AssignWaveArc, SND_DestroyBank
 * and SND_DestroyWaveArc in this order, and mwccarm emits them in reverse.
 * This SDK is older than pret's: nothing here takes the sound mutex, and
 * the linker dropped the instrument and wave-data readers. */
#include "types.h"
#include "ds_width.h"
#include "nitro/os/cache.h"

extern "C" {

struct SNDWaveArc;

struct SNDWaveArcLink {
    SNDWaveArc * DS_P32 waveArc;  /* 0x00 */
    SNDWaveArcLink * DS_P32 next; /* 0x04 */
};

struct SNDWaveArc {
    char header[0x18];       /* 0x00 -- file header and data block header */
    SNDWaveArcLink * DS_P32 topLink; /* 0x18 */
    char reserved[0x1c];     /* 0x1c */
    u32 waveCount;           /* 0x38 */
};

struct SNDBankData {
    char header[0x18];             /* 0x00 -- file header and data block header */
    SNDWaveArcLink waveArcLink[4]; /* 0x18 */
    u32 instCount;                 /* 0x38 */
};

typedef char SNDWaveArc_size_must_be_0x3c[sizeof(SNDWaveArc) == 0x3c ? 1 : -1];
typedef char SNDBankData_size_must_be_0x3c[sizeof(SNDBankData) == 0x3c ? 1 : -1];

/* Point the bank's link at index to waveArc: unchain the link from the
 * archive it used before, then chain it at the head of waveArc's links. */
void SND_AssignWaveArc(SNDBankData *bank, int index, SNDWaveArc *waveArc) {
    SNDWaveArc *prev = bank->waveArcLink[index].waveArc;
    if (prev != 0) {
        if (waveArc == prev)
            return;
        if (&bank->waveArcLink[index] == prev->topLink) {
            prev->topLink = bank->waveArcLink[index].next;
            DC_StoreRange(bank->waveArcLink[index].waveArc, sizeof(SNDWaveArc));
        } else {
            SNDWaveArcLink *link;
            for (link = prev->topLink; link != 0; link = link->next) {
                if (&bank->waveArcLink[index] == link->next)
                    break;
            }
            link->next = bank->waveArcLink[index].next;
            DC_StoreRange(link, sizeof(SNDWaveArcLink));
        }
    }
    {
        SNDWaveArcLink *oldTop = waveArc->topLink;
        waveArc->topLink = &bank->waveArcLink[index];
        bank->waveArcLink[index].next = oldTop;
        bank->waveArcLink[index].waveArc = waveArc;
        DC_StoreRange(bank, sizeof(SNDBankData));
        DC_StoreRange(waveArc, sizeof(SNDWaveArc));
    }
}

/* Unchain each of the bank's links from the archive it points at. */
void SND_DestroyBank(SNDBankData *bank) {
    int i;
    for (i = 0; i < 4; i++) {
        SNDWaveArc *waveArc = bank->waveArcLink[i].waveArc;
        if (waveArc == 0)
            continue;
        if (&bank->waveArcLink[i] == waveArc->topLink) {
            waveArc->topLink = bank->waveArcLink[i].next;
            DC_StoreRange(waveArc, sizeof(SNDWaveArc));
        } else {
            SNDWaveArcLink *link;
            for (link = waveArc->topLink; link != 0; link = link->next) {
                if (&bank->waveArcLink[i] == link->next)
                    break;
            }
            link->next = bank->waveArcLink[i].next;
            DC_StoreRange(link, sizeof(SNDWaveArcLink));
        }
    }
}

/* Clear every link that still points at this wave archive. */
void SND_DestroyWaveArc(SNDWaveArc *waveArc) {
    SNDWaveArcLink *link = waveArc->topLink;
    while (link != 0) {
        SNDWaveArcLink *next = link->next;
        link->waveArc = 0;
        link->next = 0;
        DC_StoreRange(link, sizeof(SNDWaveArcLink));
        link = next;
    }
}
}
