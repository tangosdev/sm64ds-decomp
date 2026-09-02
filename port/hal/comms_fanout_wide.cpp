// THE WIDE FAN-OUT. 0.3.2, the sixteen-player release.
//
// src/func_0203bb60.c and src/func_0203bc7c.c are steps 0x16 and 0x17 of the
// ROM's main loop: every frame they copy the per-slot comms records into the
// per-player TouchInfo and PadData arrays that the rest of the game reads its
// input from. Both loop `i < 4`, hard. With the conductor hosted wide
// (hal/comms_conductor_wide.cpp) the records for slots 4..15 arrive every
// round -- and then never left them: sixteen bodies in lockstep, twelve of
// them standing on their spawn with a pad that read zero on every peer. The
// independent review of the sixteen-player merge caught it by reading the
// per-slot pad words in the ladder logs.
//
// Same hosting shape as the conductor, because it is the same wall. The two
// ROM TUs stay in the link, byte for byte, under the per-source renames
// func_0203bb60_narrow / func_0203bc7c_narrow (port/CMakeLists.txt), and are
// what every session of four or fewer runs. The wide copies below are the
// same two functions with the bound at kPortMaxPlayers, chosen by the one
// number the carrier picks the wire from (port::comms_session_players()).
// No transport installed answers 0, which is narrow, which is solo:
// unchanged.
//
// STORAGE: TouchInfo (data_020a0de8, a placed band, sixteen entries since
// vs16), PadData (data_020a0e58, sixteen pairs since vs16), and the two
// previous-keys arrays data_020a0e48 / data_020a0e50 (sixteen halfwords each,
// widened with this file). The accessors func_0203dabc / func_0203dae4 index
// the record run, which is sixteen records since the conductor was hosted.

#include "comms_loopback.h"
#include "vs_width.h"

extern "C" {

typedef unsigned char u8;
typedef unsigned short u16;

struct R {
    u16 field_0;
    u16 field_2;
    u16 field_4;
};

typedef struct InputPair {
    u16 cur;
    u16 pressed;
} InputPair;

extern u8 data_020a0de8[];
extern struct R *func_0203dabc(int idx);

extern unsigned char data_020a0e44;
extern u16 data_020a0e48[kPortMaxPlayers];
extern u16 data_020a0e50[kPortMaxPlayers];
extern InputPair data_020a0e58[kPortMaxPlayers];
extern u16 func_0203dae4(int idx);

// The narrow arms: the ROM TUs themselves, under the build's rename.
void func_0203bb60_narrow(void);
void func_0203bc7c_narrow(void);

}  // extern "C"

namespace {

// src/func_0203bb60.c with `i < 4` at kPortMaxPlayers. Nothing else differs.
void touch_fanout_wide(void)
{
    u8 *p = data_020a0de8;
    int i;
    for (i = 0; i < kPortMaxPlayers; i++) {
        struct R *r = func_0203dabc(i);
        p[1] = (u8)(r->field_4 ^ p[0]);
        p[0] = (u8)r->field_4;
        p[2] = (u8)r->field_0;
        p[3] = (u8)r->field_2;
        p += 4;
    }
}

// src/func_0203bc7c.c with `i < 4` at kPortMaxPlayers. Nothing else differs.
void pad_fanout_wide(void)
{
    InputPair *p = data_020a0e58;
    int i;
    data_020a0e44 = 0;
    for (i = 0; i < kPortMaxPlayers; i++)
    {
        u16 keys = func_0203dae4(i);
        u16 old;
        int changed;
        if ((keys & 0x30) == 0x30)
            keys &= ~0x30;
        if ((keys & 0xc0) == 0xc0)
            keys &= ~0xc0;
        if (keys == 0x30c)
            data_020a0e44 = 1;
        old = data_020a0e50[i];
        changed = keys ^ old;
        p->pressed = keys & changed;
        data_020a0e48[i] = old & changed;
        data_020a0e50[i] = keys;
        p->cur = keys;
        p++;
    }
}

inline bool wide(void)
{
    return port::comms_session_players() > kPortNarrowPlayers;
}

}  // namespace

extern "C" void func_0203bb60(void)
{
    if (!wide()) { func_0203bb60_narrow(); return; }
    touch_fanout_wide();
}

extern "C" void func_0203bc7c(void)
{
    if (!wide()) { func_0203bc7c_narrow(); return; }
    pad_fanout_wide();
}
