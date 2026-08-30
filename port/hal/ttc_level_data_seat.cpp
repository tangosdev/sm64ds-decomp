/* run linkw wave 18 (lane w18): the TICK TOCK CLOCK level-data seat.
 *
 * THE JOB. Every Ttc class in ov065 reaches a CLPS collision block or an
 * animation descriptor that lives in whichever LEVEL overlay is loaded -- ov035
 * on level 27. Wave 17 seated the whole Ttc cluster without closing those reads
 * and measured what it costs: ids 108 (x8) and 114 (x5) ticked 300 frames
 * WITHOUT faulting, silently, on a garbage CLPS, and id 112 divided by zero in
 * Animation::Advance because its descriptor read zero frames. The quiet pair is
 * the worse half and is why that seat came back out.
 *
 * WHY THE MOUNT ALONE IS NOT THE ANSWER, WHICH IS THIS LANE'S CORRECTION.
 * The ranking this lane inherited said the remedy was "a per-symbol dual mount
 * alongside the whole mount -- ov022 already does exactly this". The dual mount
 * (port/ov035_syms.txt) is NECESSARY: it is what makes the ten CLPS blocks and
 * two descriptors exist as named host storage at all. It is NOT SUFFICIENT, and
 * tools/ovdata.py's own cross pass says why in as many words:
 *
 *     "Every level overlay is linked at the SAME base, because the DS only ever
 *      has one loaded ... Binding it to the ov009 copy would be right on the
 *      castle grounds and a walk over another level's bytes everywhere else,
 *      which is the class of bug this pass exists to remove. Resolving it
 *      correctly needs a seat that re-patches per loaded level; until there is
 *      one, raw is the honest answer and the sweep keeps it visible."
 *
 * MEASURED, not argued: each of the twelve target addresses lands inside
 * EIGHTEEN mounted overlay windows (ov009 ov010 ov012 ov013 ov014 ov015 ov016
 * ov018 ov019 ov020 ov021 ov022 ov025 ov035 ov045 ov052 ov056 ov060). The cross
 * pass drops a target covered by more than one window, so adding ov035 takes
 * that count from seventeen to eighteen and rebases nothing. `cross: ov065 9
 * pointers` before the mount and `cross: ov065 9` after it -- the same nine,
 * all into ov002 gaps. So the Ttc cluster's cost was re-ranked DOWNWARD on a
 * premise that does not hold; this file is the machinery that was missing.
 *
 * WHY A SEAT IS ALLOWED TO DO WHAT THE CROSS PASS REFUSES. The cross pass must
 * be conservative because it cannot know which level is loaded. A seat run FROM
 * THE LEVEL-27 MOUNT ROW knows exactly that, which is the disambiguation the
 * pass is missing. The Ttc classes exist only in Tick Tock Clock, so binding
 * their level-window reads to ov035's copy is right wherever they can run, and
 * on every other level these words keep their raw ROM value and nothing reads
 * them -- ov065's other residents (Snufit 236, Swoop 237, Dorrie 168,
 * DorrieCap 169) do not touch these four blocks.
 *
 * THE PRECEDENT is port_ov089_keymodels_fixup() in hal/actor_overlays.cpp,
 * which closes ov089's six LoadKeyModels cross pointers by hand for the same
 * reason and with the same ROM-value check. That seat is even named in the
 * cross pass's own HAND_SEATED table.
 *
 * ==== WHAT IS SEATED, AND WHAT IS NOT ======================================
 *
 * ov065 makes TWELVE reads into the level window. They split by WHERE the word
 * lives, and only one half is a seat's business:
 *
 *   EIGHT are words inside four ov065 DATA blocks, all four already in ov065's
 *   per-symbol mount. Those are this file's eight rewrites.
 *
 *   FOUR are literal-pool words inside ov065 FUNCTION bodies. The port does not
 *   mount ov065's .text, so there is no word to rewrite: those resolve at LINK
 *   time, because the matched TU names the ov035 symbol and the dual mount is
 *   what makes that name resolve. PAIRED EXPLICITLY, because listing the four
 *   bodies in address order beside the four names in address order gets three
 *   of the four pairings wrong -- the two orders are not the same order:
 *
 *     func_ov065_0211a358               +0xfc  -> 0x02112198   (id 110)
 *     func_ov065_0211b1d4               +0x140 -> 0x02112258   (id 113)
 *     TtcRotatingGear::InitResources    +0xe0  -> 0x021121b8   (ids 116/117)
 *     TtcMovingCubeA::InitResources     +0x138 -> 0x02112118   (id 118)
 *
 *   Each offset is the pool word's address minus the body's own, and each
 *   target is the word read out of extracted/overlays/overlay_0065.bin at that
 *   address. Nothing to do here, but they are the other half of why the mount
 *   is required.
 *
 * The four blocks and their eight words, read out of
 * extracted/overlays/overlay_0065.bin and cross-checked against
 * config/arm9/overlays/ov065/relocs.txt. Each block is 0x10 (or 0x8) and EVERY
 * word in it is a relocation, so the reloc run and the next-symbol landing
 * agree on the extent and the run terminates exactly at the boundary -- the one
 * place in this lane where both width routes told the same story:
 *
 *   data_ov065_0211cfd8  +0x0 -> 0x02112138  CLPS, ids 108/109
 *                        +0x4 -> ov065 SharedFilePtr (mount's own pass)
 *                        +0x8 -> ov065 SharedFilePtr (mount's own pass)
 *                        +0xc -> 0x02112178  CLPS, ids 108/109
 *   data_ov065_0211d16c  +0x0 -> 0x0211208c  animation descriptor, id 111
 *                        +0x4 -> 0x021120a4  animation descriptor, id 112
 *   data_ov065_0211d19c  +0x0 -> 0x021120d8  CLPS
 *                        +0xc -> 0x021121f8  CLPS
 *   data_ov065_0211d364  +0x0 -> 0x021120f8  CLPS, ids 114/115
 *                        +0xc -> 0x02112158  CLPS
 *
 * IDEMPOTENT BY VALUE, not by a done-guard. Level 27 can be entered more than
 * once, and a re-entry may or may not have reset .dsstate back to ROM values, so
 * a `static int done` would be wrong in one of the two cases. Instead each word
 * is accepted if it holds EITHER its ROM address or the host address this seat
 * would write; anything else is a third value nobody predicted and aborts. That
 * is strictly stronger than the done-guard the sibling-fill rule asks for.
 *
 * THE READBACK IS THE PROOF. Wave 17's finding was that a class ticking cleanly
 * proves nothing -- ids 108 and 114 ran 300 clean frames on garbage. So after
 * rewriting, this seat DEREFERENCES each seated pointer and checks the storage
 * it now names really is what the ROM says it is: the six CLPS blocks must read
 * the 'CLPS' magic (0x53504c43) and a sane {entrySize, count} header, and the
 * two animation descriptors must read a nonzero frame count -- which is exactly
 * the field whose zero divided by zero in Animation::Advance. A pointer that
 * lands on plausible-looking garbage fails here instead of ticking quietly.
 *
 * ===========================================================================
 * RUN REL0215 WAVE 3 (lane w3-a2): THE SEAT BECOMES PER LOADED LEVEL
 * ===========================================================================
 *
 * WHAT WAS MISSING, and it is one direction rather than the whole mechanism.
 * The pass above seats on level 27 and NEVER RELEASES. It hung off
 * port_mount_row_lvl27 alone, so once Tick Tock Clock had been entered the
 * eight words named ov035's copy for the rest of the process -- across a warp
 * to any other level and back. ovdata.py's cross_mode() asks for "a seat that
 * RE-PATCHES PER LOADED LEVEL"; a seat that patches on one level and is never
 * told about the others is half of that, and the half it is missing is the one
 * that makes the words honest everywhere else.
 *
 * WHAT THE RIGHT PER-LEVEL VALUE IS, MEASURED RATHER THAN ASSUMED. The obvious
 * generalisation -- resolve each target through the NOW-LOADED level's own
 * mount, PortLevelDesc::at(rom), the port_ovNN_at shape -- is WRONG HERE, and
 * it fails in TWO different ways depending on which level is loaded. Swept over
 * every overlay sharing the level window base 0x021111a0, for the target
 * 0x02112138: fifty-two share that base, ov035 owns the block, and of the OTHER
 * FIFTY-ONE --
 *
 *   28 CONTAIN the address, so at() hands back a host pointer quite happily,
 *      and ALL 28 read something that is not a CLPS block there (ov009 reads
 *      magic e28dd004, ov014 entrySize 0, and so on -- mostly ARM instructions,
 *      because at that offset most level overlays are still in .text). The
 *      seat's readback catches every one of them and aborts.
 *   23 DO NOT REACH IT at all: the address is past the end of their footprint,
 *      at() returns 0, and there is no host answer to seat.
 *
 * Level 33 is in the SECOND group and is worth naming because it is this
 * lane's own proof level: level 33 is ov041, whose window ends at 0x02111d40,
 * so 0x02112138 is 0x3f8 PAST it. at() returns 0 there and the readback is
 * never reached. (An earlier revision of this paragraph said level 33 was ov033
 * and that at() resolved into it -- ov033 is level 25, and the sentence was
 * wrong in both halves while the conclusion it supported was right.)
 *
 * ZERO of the fifty-one is a usable answer. These twelve addresses are not a
 * shared window slot that every level fills in with its own equivalent -- they
 * are Tick Tock Clock's own resource blocks, and no other level has anything at
 * those addresses that these classes could use.
 * Evidence: ...runs/rel0215/out/w3-a2/lvlwindow-sweep.txt (and lvlwindow.py,
 * which re-derives it from the config and the shipped images).
 *
 * So the per-loaded-level value is TWO-VALUED, not one-per-level:
 *
 *     loaded level 27  ->  ov035's copy, seated and readback-verified
 *     any other level  ->  the RAW ROM address, released
 *
 * and "raw is the honest answer" is ovdata.py's own words for the second row.
 *
 * WHY RELEASING IS SAFE, and it is a measurement rather than an argument. Lane
 * w3-a2 swept ALL FIFTY mounted levels (walk_window, 20 frames each) and
 * grepped each level's spawn census for ids 108..118. Exactly ONE level names
 * any of them: level 27, with 108 x8, 110 x4, 111 x3, 112 x1, 113 x11, 114 x5,
 * 118 x3. The other forty-nine name none. Evidence:
 * runs/rel0215/out/w3-a2/ttc-id-sweep.txt. Nothing off Tick Tock Clock reads
 * these words, which is the same fact the wave-18 note above states and now has
 * a number behind it.
 *
 * THE FOUR .text WORDS STAY BOUND TO ov035, AND THAT IS WHY THIS IS COHERENT.
 * The other four of ov065's twelve level-window reads are literal-pool words
 * inside compiled TUs; a compiled TU takes the block's ADDRESS, so that address
 * is fixed at LINK time and no per-level pass can move it. They resolve by name
 * onto data_ov035_02112118 / _02112198 / _021121b8 / _02112258. Making the
 * eight follow the loaded level while the four cannot would put the two halves
 * of one cluster on different storage on every level but 27. The two-valued
 * rule above keeps all twelve on ov035 exactly where the classes run and off it
 * everywhere else, so the cluster is never split.
 *
 * NO ONE-SHOT GUARD, WHICH IS THIS PASS'S ANSWER TO THE .dsstate QUESTION.
 * level_boot.cpp states the rule: "A ONE-SHOT GUARD BELONGS ON THE SAME SIDE OF
 * THE CAPTURED SECTION AS THE WORK IT GUARDS", because a guard on the far side
 * of a save-state restore and the bytes it describes can disagree. The eight
 * words ARE .dsstate -- ovdata.py routes every mounted byte into the captured
 * section -- so a host-side guard here would be exactly that defect, and a
 * bracketed one would only move the problem. This pass carries NO guard of any
 * kind: the value it must write is a pure function of the loaded level id, and
 * what it accepts before writing is the ROM address or the host address it
 * would itself write. There is no second description of the bytes, so there is
 * nothing for a restore to desynchronise, whichever way it moves them. It is
 * re-runnable by construction, allocates nothing on the host and registers
 * nothing with the host, which is the test level_boot.cpp applies to every pass
 * that sits inside the bracket.
 *
 * IT DEPENDS ON A MOUNT ORDERING NOTHING ASSERTS, and that is written here
 * because it is the one assumption this pass cannot check for itself. A level
 * change resolves the OUTGOING level's mount before mounting the INCOMING one
 * -- hal/level_change.cpp's port_level_capture_kcl looks up the outgoing
 * LVL_Overlay to save its KCL handle -- so this seat is called twice per
 * change and THE SECOND CALL IS THE ONE THAT DECIDES. Observed in both
 * directions in the traced run, on the stderr stream where the ordering is
 * reliable (runs/rel0215/out/w3-a2/mech-warp-27-33-27-SEATED.log):
 *
 *   606 [lvl] change: level 27 -> 33   1008 [lvl] change: level 33 -> 27
 *   615 [ttc-seat] level 27  SEATED    1017 [ttc-seat] level 33  RELEASED
 *   624 [ttc-seat] level 33  RELEASED  1026 [ttc-seat] level 27  SEATED
 *   627 [lvl] level 33 up             1031 [lvl] level 27 up
 *
 * Outgoing first, incoming second, both times. If that order were ever
 * reversed the words would end up describing the level the player just LEFT,
 * and nothing in the build would say so -- there is no assertion on it, here
 * or in level_change.cpp. A lane that changes the handoff's mount order owes
 * this seat a look.
 *
 * WHERE IT IS CALLED FROM. port_level_mount_at(), on BOTH of its paths -- the
 * patch path and the CACHE HIT path. The cache-hit path is load-bearing rather
 * than an optimisation: a session that goes 27 -> 33 -> 27 gets a cache hit on
 * the third mount, and the words at that moment hold whatever level 33 left
 * them holding. A seat that only ran when an overlay was freshly patched would
 * put Tick Tock Clock's second visit on released words.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

/* the four ov065 blocks, already in ov065's per-symbol mount */
extern unsigned char data_ov065_0211cfd8[];
extern unsigned char data_ov065_0211d16c[];
extern unsigned char data_ov065_0211d19c[];
extern unsigned char data_ov065_0211d364[];

/* the eight ov035 blocks they must name, from port/ov035_syms.txt */
extern unsigned char data_ov035_0211208c[];
extern unsigned char data_ov035_021120a4[];
extern unsigned char data_ov035_021120d8[];
extern unsigned char data_ov035_021120f8[];
extern unsigned char data_ov035_02112138[];
extern unsigned char data_ov035_02112158[];
extern unsigned char data_ov035_02112178[];
extern unsigned char data_ov035_021121f8[];

void port_ttc_level_data_seat(int level_id);

}  /* extern "C" */

namespace {

const unsigned CLPS_MAGIC = 0x53504c43u;   /* 'CLPS' */

enum Kind { KIND_CLPS, KIND_ANIM };

struct Seat {
    unsigned char *block;      /* the ov065 block holding the word      */
    const char *blockName;
    unsigned off;              /* byte offset of the word in that block */
    unsigned rom;              /* the DS address the ROM word carries   */
    unsigned char *host;       /* the ov035 storage it must name        */
    Kind kind;
    const char *what;
};

const Seat g_seats[] = {
    {data_ov065_0211cfd8, "data_ov065_0211cfd8", 0x0, 0x02112138, data_ov035_02112138, KIND_CLPS, "CLPS ids 108/109"},
    {data_ov065_0211cfd8, "data_ov065_0211cfd8", 0xc, 0x02112178, data_ov035_02112178, KIND_CLPS, "CLPS ids 108/109"},
    {data_ov065_0211d16c, "data_ov065_0211d16c", 0x0, 0x0211208c, data_ov035_0211208c, KIND_ANIM, "anim descriptor id 111"},
    {data_ov065_0211d16c, "data_ov065_0211d16c", 0x4, 0x021120a4, data_ov035_021120a4, KIND_ANIM, "anim descriptor id 112"},
    {data_ov065_0211d19c, "data_ov065_0211d19c", 0x0, 0x021120d8, data_ov035_021120d8, KIND_CLPS, "CLPS"},
    {data_ov065_0211d19c, "data_ov065_0211d19c", 0xc, 0x021121f8, data_ov035_021121f8, KIND_CLPS, "CLPS"},
    {data_ov065_0211d364, "data_ov065_0211d364", 0x0, 0x021120f8, data_ov035_021120f8, KIND_CLPS, "CLPS ids 114/115"},
    {data_ov065_0211d364, "data_ov065_0211d364", 0xc, 0x02112158, data_ov035_02112158, KIND_CLPS, "CLPS"},
};

const int SEAT_COUNT = (int)(sizeof(g_seats) / sizeof(g_seats[0]));

unsigned rd32(const unsigned char *p)
{
    return (unsigned)p[0] | ((unsigned)p[1] << 8) |
           ((unsigned)p[2] << 16) | ((unsigned)p[3] << 24);
}

/* Does the storage this pointer now names really hold what the ROM says?
   This is the half wave 17 could not do: a Ttc class ticking cleanly on a
   garbage CLPS produced 300 green frames and told nobody. */
bool readback_ok(const Seat &s, char *why, unsigned long whyLen)
{
    if (s.kind == KIND_CLPS) {
        unsigned magic = rd32(s.host);
        unsigned hdr = rd32(s.host + 4);
        unsigned esz = hdr & 0xffffu;
        unsigned cnt = (hdr >> 16) & 0xffffu;
        if (magic != CLPS_MAGIC) {
            std::snprintf(why, (size_t)whyLen,
                          "magic %08x, wanted 'CLPS' %08x", magic, CLPS_MAGIC);
            return false;
        }
        if (esz != 8 || cnt == 0 || cnt > 2) {
            std::snprintf(why, (size_t)whyLen,
                          "header entrySize=%u count=%u, every ov035 block is 8/1..2",
                          esz, cnt);
            return false;
        }
        std::snprintf(why, (size_t)whyLen, "'CLPS' entrySize=%u count=%u", esz, cnt);
        return true;
    }
    /* the animation descriptor: word[0] is the frame count Animation::Advance
       divides by, and reading it as zero is the c0000094 wave 17 measured. */
    unsigned frames = rd32(s.host);
    if (frames == 0) {
        std::snprintf(why, (size_t)whyLen, "frame count 0 -- the divide-by-zero shape");
        return false;
    }
    std::snprintf(why, (size_t)whyLen, "frames=%u", frames);
    return true;
}

void wr32(unsigned char *p, unsigned v)
{
    p[0] = (unsigned char)(v & 0xff);
    p[1] = (unsigned char)((v >> 8) & 0xff);
    p[2] = (unsigned char)((v >> 16) & 0xff);
    p[3] = (unsigned char)((v >> 24) & 0xff);
}

/* Tick Tock Clock. The only level whose object table names a Ttc id -- swept
   over all fifty mounted levels, see the header. */
const int TTC_LEVEL = 27;

/* SM64DS_TRACE_TTCSEAT=1: one line per word per mount, carrying the VALUE
   written. Quiet by default because this now runs on every level mount rather
   than only on Tick Tock Clock's, and forty-nine of the fifty have nothing to
   say. The summary line below is unconditional and names the level, so a log
   still shows which way each mount went. */
int trace_on(void)
{
    static int on = -1;
    if (on < 0)
        on = std::getenv("SM64DS_TRACE_TTCSEAT") != 0;
    return on;
}

}  /* namespace */

extern "C" void port_ttc_level_data_seat(int level_id)
{
    const bool ttc = (level_id == TTC_LEVEL);
    int wrote = 0, held = 0;
    for (int i = 0; i < SEAT_COUNT; ++i) {
        const Seat &s = g_seats[i];
        unsigned char *word = s.block + s.off;
        unsigned have = rd32(word);
        unsigned host = (unsigned)(size_t)s.host;
        /* THE TWO VALUES THIS WORD MAY EVER HOLD. Anything else is a third
           value nobody predicted -- another pass writing here, a mount that
           moved, a rolled-back image that did not roll back whole -- and it
           aborts rather than being overwritten quietly. This is the same
           abort-on-mismatch discipline port_jrb_staticrock_clps_seat and
           port_ov089_keymodels_fixup use, widened by one accepted value
           because this seat now writes in both directions. */
        if (have != s.rom && have != host) {
            std::fprintf(stderr,
                         "FATAL: Ttc level-data seat: %s+%#x holds %08x on level "
                         "%d; the ROM says %08x and this seat writes %08x or "
                         "%08x -- WRONG BYTES\n",
                         s.blockName, s.off, have, level_id, s.rom, s.rom, host);
            std::abort();
        }
        unsigned want = ttc ? host : s.rom;
        if (have != want) {
            wr32(word, want);
            ++wrote;
        } else {
            ++held;
        }
        /* THE READBACK IS THE PROOF, and it only has a subject when the word
           names host storage. Released, the word carries a raw DS address that
           this process deliberately does not provide -- there is nothing to
           dereference and dereferencing it is the wave-17 failure. */
        char why[96];
        if (ttc) {
            if (!readback_ok(s, why, sizeof why)) {
                std::fprintf(stderr,
                             "FATAL: Ttc level-data seat: %s+%#x now names %08x "
                             "for %s, but that storage reads %s\n",
                             s.blockName, s.off, host, s.what, why);
                std::abort();
            }
        } else {
            std::snprintf(why, sizeof why, "raw, level %d provides no such block",
                          level_id);
        }
        if (trace_on())
            std::fprintf(stderr, "  [ttc-seat] %s+%#-4x = %08x -> %s  (%s, %s)\n",
                         s.blockName, s.off, want, s.what, why,
                         have == want ? "held" : (ttc ? "seated" : "released"));
    }
    std::fprintf(stderr,
                 "  [ttc-seat] level %d: 8 ov065 level-window reads %s (%d "
                 "written, %d already right)%s\n",
                 level_id, ttc ? "SEATED onto ov035" : "RELEASED to raw",
                 wrote, held, ttc ? ", all readbacks verified" : "");
}
