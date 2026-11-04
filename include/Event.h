#pragma once

#include "types.h"

/* Event bitfield helpers over the single event-flag word gEventFlags (0x0209f34c).
 * `bit` is an index into that word, not a mask.
 */
namespace Event {

s32 ClearBit(u32 bit);
void SetBit(u32 bit);
s32 GetBit(u32 bit);

}

/* Named indices into gEventFlags. */
enum EventBit {
    /* Set while a star (daStar_c) or a Bowser key (daObjKey_c, which spells it
       EVENT_KEY) is being collected, and cleared when the collection ends.
       dMeter_c::Render skips the coin, red-coin, silver-star and timer
       counters while it is set, and UpdateHealthMeter retracts the health
       meter. At runtime, setting it hides the coin and red-coin counters. */
    EVENT_COLLECTING_STAR = 0x1d
};

extern s32 gEventFlags;
