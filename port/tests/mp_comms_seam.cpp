// MP1 STEP 5(c): THE SEAM'S ANSWERS, AND THE ROM'S FOUR-SLOT FAN-OUT.
// Run mg15, lane MP1.
//
// Two things this proves, both of them with the ROM's own code and not a
// transcription of it:
//
//   THE SEAM ANSWERS HONESTLY WITH NO PARTNERS. func_02040714 says idle,
//   func_02040704 says slot 0, func_020406b4 never completes a round, and
//   asking to become a parent or a child changes none of that. That is what a
//   DS with nobody in range does, and it is what makes the ROM's own timeout
//   (src/func_0203ea5c.c:487) drop the session back to solo instead of the
//   port inventing a fallback.
//
//   THE FOUR INPUT SLOTS CARRY FOUR COPIES OF THE LOCAL INPUT. SM64DS is
//   four-player all the way down in single player too: src/func_0203e0ac.c
//   cascades the local comms record into slots 3, 2, 1 and 0, and
//   src/func_0203bb60.c and src/func_0203bc7c.c fan those four records out
//   into TouchInfo[4] and PadData[4], which is where every stylus and button
//   read in the entire game comes from. All four TUs are the ROM's, linked.
//
// SILENT BY CONSTRUCTION: console only, no window, no BMP.

#include <cstdio>
#include <cstring>

#include "comms_seam.h"

// ---------------------------------------------------------------------------
// THE HOSTED RECORDS.
//
// The shipped binaries get these from hal/camera_bridges.cpp, which places
// them in ordered .dsstate$camcomm/$camrec sections at ROM spacing and asserts
// the offsets at bring-up (hal_camera_check_layout). The probe links neither
// that file nor its dependencies, so it reproduces the same split here -- SAME
// SECTION FAMILY AND SAME ORDER, because src/func_0203e0ac.c reaches slots 1,
// 2 and 3 through the interior symbols data_020a117c / data_020a11a0 /
// data_020a11c4 and a split that drifted would cascade into the wrong words.
// The layout is asserted below before anything is measured.
// ---------------------------------------------------------------------------
#define COMM(sec, name, size) \
    __pragma(section(sec, read, write))                          \
    extern "C" __declspec(allocate(sec)) __declspec(align(2))    \
    unsigned char name[size] = {0}

COMM(".dsstate$camcomm0000", data_020a1040, 4);
COMM(".dsstate$camcomm0001", data_020a1044, 2);
COMM(".dsstate$camcomm0002", data_020a1046, 2);
COMM(".dsstate$camcomm0003", data_020a1048, 4);
COMM(".dsstate$camcomm0004", data_020a104c, 2);
COMM(".dsstate$camcomm0005", data_020a104e, 2);
COMM(".dsstate$camcomm0006", data_020a1050, 2);
COMM(".dsstate$camcomm0007", data_020a1052, 0x12);

COMM(".dsstate$camrec0000", data_020a1154, 0xc);
COMM(".dsstate$camrec0001", data_020a1160, 2);
COMM(".dsstate$camrec0002", data_020a1162, 2);
COMM(".dsstate$camrec0003", data_020a1164, 2);
COMM(".dsstate$camrec0004", data_020a1166, 0x12);
COMM(".dsstate$camrec0005", data_020a1178, 4);
COMM(".dsstate$camrec0006", data_020a117c, 0x24);
COMM(".dsstate$camrec0007", data_020a11a0, 0x24);
COMM(".dsstate$camrec0008", data_020a11c4, 0x20);
#undef COMM

extern "C" {
// TouchInfo[4] (4-byte records) and PadData[4] (stride 4). In the shipped
// binaries hal/auto_bss.cpp hosts data_020a0de8 as four ROM-spaced slots and
// data_020a0e58 as int[8].
unsigned char data_020a0de8[16];
int data_020a0e58[8];
// The comms ROLE byte. hal/stage_slot0.cpp hosts it in the shipped binaries.
unsigned char data_020a0f04[4];

// The ROM's own code, linked as itself.
void func_0203e0ac(void);       // the solo cascade
void func_0203bb60(void);       // records -> TouchInfo[4]
void func_0203bc7c(void);       // records -> PadData[4]
int func_02040714(void);
int func_02040704(int);
int func_020406b4(const void *block, unsigned short *status);
const void *func_0204068c(unsigned short aid);
void func_020408b0(unsigned short mode);
void func_02040820(void);
void func_02040790(void);
void func_02040724(void);
void func_0203e20c(void);
}

static int g_fail;
#define CHECK(cond) do { if (!(cond)) { ++g_fail; \
    std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

// Write the local record the way src/func_0203df40.c does: the stylus quad at
// +4/+6/+8/+0xA, the key word at +0xE, and the live bit in the flag word.
static void set_local_input(unsigned x, unsigned y, unsigned touch, unsigned key)
{
    *(unsigned short *)(data_020a1040 + 0x04) = (unsigned short)x;
    *(unsigned short *)(data_020a1040 + 0x06) = (unsigned short)y;
    *(unsigned short *)(data_020a1040 + 0x08) = (unsigned short)touch;
    *(unsigned short *)(data_020a1040 + 0x0A) = 0;
    *(unsigned short *)(data_020a1040 + 0x0C) |= 0x8000;
    *(unsigned short *)(data_020a1040 + 0x0E) = (unsigned short)key;
}

int main() {
    using namespace port;
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::printf("== MP1 seam probe: the solo answers and the four-slot "
                "fan-out ==\n\n");

    // ---- 0. the record layout, before anything is measured ---------------
    std::printf("0 record layout (hal/camera_bridges.cpp's own assertion)\n");
    std::printf("   data_020a1164 - data_020a1154 = 0x%02x (want 0x10)\n",
                (unsigned)(data_020a1164 - data_020a1154));
    std::printf("   data_020a117c - data_020a1154 = 0x%02x (want 0x28)\n",
                (unsigned)(data_020a117c - data_020a1154));
    std::printf("   data_020a11c4 - data_020a1154 = 0x%02x (want 0x70)\n",
                (unsigned)(data_020a11c4 - data_020a1154));
    std::printf("   data_020a1050 - data_020a1040 = 0x%02x (want 0x10)\n",
                (unsigned)(data_020a1050 - data_020a1040));
    CHECK(data_020a1164 - data_020a1154 == 0x10);
    CHECK(data_020a117c - data_020a1154 == 0x28);
    CHECK(data_020a11c4 - data_020a1154 == 0x70);
    CHECK(data_020a1050 - data_020a1040 == 0x10);

    // ---- 1. the seam with no transport -----------------------------------
    std::printf("\n1 the seam with NO TRANSPORT (the shipped default)\n");
    CommsReadout r = comms_readout();
    std::printf("   transport=%s link=%d connected=%s slot=%d players=%d "
                "role=%d boot=0x%04x\n", r.transport, r.link_state,
                r.connected ? "yes" : "no", r.slot, r.players, r.role,
                (unsigned)r.boot_indicator);
    CHECK(comms_transport() == nullptr);
    CHECK(r.link_state == kCommsIdle);
    CHECK(!r.connected);
    CHECK(r.slot == 0);
    CHECK(r.players == 1);
    CHECK(r.role == kCommsRoleSolo);

    // Through the ROM's own entry points, which is what func_0203ea5c calls.
    func_020408b0(2);
    CHECK(func_02040714() == kCommsIdle);
    CHECK(func_02040704(0) == 0);
    unsigned char block[kCommsBlockBytes];
    std::memset(block, 0xAB, sizeof block);
    unsigned short status = 0xFFFF;
    CHECK(func_020406b4(block, &status) == 0);   // the round never completes
    CHECK(status == 0);
    CHECK(func_0204068c(0) == nullptr);
    CHECK(func_0204068c(1) == nullptr);

    // Asking for a role with no radio must not fake one.
    func_02040820();                            // become parent
    std::printf("   after become_parent: link=%d slot=%d\n",
                func_02040714(), func_02040704(0));
    CHECK(func_02040714() == kCommsIdle);
    func_02040790();                            // become child
    std::printf("   after become_child:  link=%d slot=%d\n",
                func_02040714(), func_02040704(0));
    CHECK(func_02040714() == kCommsIdle);
    func_02040724();                            // leave
    CHECK(func_02040714() == kCommsIdle);

    // ---- 2. the boot indicator is honest ---------------------------------
    std::printf("\n2 the boot indicator\n");
    std::printf("   comms_boot_indicator() = 0x%04x  "
                "(0 = cartridge boot, host-capable, no role claimed)\n",
                (unsigned)comms_boot_indicator());
    CHECK(comms_boot_indicator() == 0);
    CHECK(data_020a0f04[0] == kCommsRoleSolo);

    // ---- 3. role 3 is refused, not faked ---------------------------------
    std::printf("\n3 DS Download Play (role 3)\n");
    data_020a0f04[0] = kCommsRoleDownloadHost;
    func_0203e20c();
    std::printf("   role after func_0203e20c() = %d (want 0, solo)\n",
                data_020a0f04[0]);
    CHECK(data_020a0f04[0] == kCommsRoleSolo);

    // ---- 4. THE FOUR-SLOT FAN-OUT, all ROM code --------------------------
    //
    // src/func_0203e0ac.c cascades the local record into slots 3, 2, 1, 0 in
    // that order and within one call (each step reads the slot the previous
    // step just wrote), so ONE tick puts the local input in all four.
    std::printf("\n4 the four-slot fan-out, through the ROM's own code\n");

    struct Probe { unsigned x, y, touch, key; };
    const Probe cases[] = {
        {128,  96, 1, 0x0001},     // a stylus press and A
        {200, 150, 1, 0x0102},     // moved, B + a shoulder
        {255, 255, 0, 0x0000},     // released, nothing held
    };

    for (unsigned c = 0; c < sizeof cases / sizeof cases[0]; ++c) {
        const Probe &p = cases[c];
        set_local_input(p.x, p.y, p.touch, p.key);
        func_0203e0ac();           // the ROM's solo cascade  (slice_gate13)
        comms_fanout();            // func_0203bb60 + func_0203bc7c (slice_comms)

        std::printf("   local {x=%u y=%u touch=%u key=%04x} ->\n",
                    p.x, p.y, p.touch, p.key);
        for (int i = 0; i < kCommsMaxPlayers; ++i) {
            const unsigned char *rec = data_020a1154 + i * 0x24;
            const unsigned char *ti = data_020a0de8 + i * 4;
            const unsigned held = (unsigned)(data_020a0e58[i] & 0xFFFF);
            std::printf("     slot%d rec{x=%u y=%u touch=%u key=%04x} "
                        "TouchInfo{now=%u chg=%u x=%u y=%u} PadData{held=%04x}\n",
                        i, *(const unsigned short *)(rec + 4),
                        *(const unsigned short *)(rec + 6),
                        *(const unsigned short *)(rec + 8),
                        *(const unsigned short *)(rec + 0x0E),
                        ti[0], ti[1], ti[2], ti[3], held);

            // Four copies of the local input, which is the ROM's own answer
            // for a console with no partners.
            CHECK(*(const unsigned short *)(rec + 4) == p.x);
            CHECK(*(const unsigned short *)(rec + 6) == p.y);
            CHECK(*(const unsigned short *)(rec + 8) == p.touch);
            CHECK(*(const unsigned short *)(rec + 0x0E) == p.key);
            CHECK(ti[0] == (unsigned char)p.touch);
            CHECK(ti[2] == (unsigned char)p.x);
            CHECK(ti[3] == (unsigned char)p.y);
            CHECK(held == p.key);
        }
    }

    // ---- 5. the readout ---------------------------------------------------
    std::printf("\n5 comms_report, the instrument MP2 reads its verdict off\n");
    comms_report("solo");

    std::printf("\n%s  (%d failures)\n",
                g_fail ? "SEAM PROBE: FAILED" : "SEAM PROBE: OK", g_fail);
    return g_fail ? 1 : 0;
}
