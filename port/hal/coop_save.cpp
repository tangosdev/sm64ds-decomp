// COOP SAVE. Run mp1 wave 56, lane COOPSAVE1. Design and rules: coop_save.h,
// coop_save_wire.h, and ntr/backup.cpp section 6 (the fence).
//
// WHO WRITES WHAT. Only the HOST console writes the .coopsav, and it does it
// with the ROM's own SaveData::SaveFile (checksum, tag and mirror are the
// ROM's), with one exception: the GUEST's first record, which is seeded into
// its throwaway image before the title reads the files (write_guest_seed
// below), because no ROM save call can run before the ROM has a file open.
// That image is deleted at exit.
//
// THE HOST RECORD, in order of preference: the live working copy
// (data_0209caa0, once the game has opened slot 0), else slot 0 of the file on
// disk (checksum checked), else "blank" (a new file: the guest keeps an erased
// image and the ROM installs defaults, so the opening plays on every console).
//
// WHAT A RECEIVED RECORD CAN DO: OR bits from coop_mono_mask() into the live
// copy and nothing else (coop_save_wire.h). Honest limit: a guest can SET
// stars, doors, cannons and unlock bits on the host file (that is what sharing
// progress is); it cannot clear a bit or set a scalar, and a forged record
// from another seat of the room is the same thing.

#include "coop_save.h"
#include "coop_save_wire.h"
#include "coop_grant_guard.h"
#include "comms_hub.h"
#include "hub_presence.h"
#include "star_tally.h"
#include "ntr/backup.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

extern "C" {
extern signed char SUBLEVEL_LEVEL_TABLE[];   // MPSAFE1: level -> course (the same table party_stars uses)
extern unsigned char data_0209caa0[];
extern unsigned char data_020a4b40[8];     // the record tag (ntr/backup.cpp section 6)
int  _ZN8SaveData8SaveFileEjP12FileSaveData(unsigned fileID, void *data);
int  _ZN8SaveData15SaveCurrentFileEv(void);
void CollectStar(int course, int star);
void *_ZN8dActor_c4NextEPKS_(const void *prev);   // KEYDOOR10 proof knob: walk the live actors
extern void *data_0209f394[];                      // the players
extern int   data_0209f250;                        // my player index
extern signed char data_0209f2f8;                  // LEVEL_ID
// RABBIT1: the rabbit key's own save calls (src/engine/save/SaveData.cpp, src/unnamed/arm9/0201/func_02013638.c)
void func_02013868(int kind, int character);       // the key's carried entry: +0x0c bit and minigame unlock bit
int  func_02013638(int n);                          // the minigame menu's unlock test (MiniGm_IsUnlocked)
int  func_02013890(int kind, int character);        // the rabbit's "already caught" test (daMip_c::StateCaughtMain)
int  _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData(void *dest);
int  _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(void *data);
}

namespace port {
static_assert(kStarTallySub == kCoopSaveSubTally, "tally sub-type");
namespace {

const uint8_t kCoopTag[8] = { 'd', 's', ' ', 'c', 'o', 'o', 'p', '!' };
const uint8_t kMagic[4]   = { 0x38, 0x30, 0x30, 0x30 };   // "8000"

bool env_on(const char *n) {
    const char *e = std::getenv(n);
    return e && *e && *e != '0';
}

// ---- state (game thread only) ----------------------------------------------
int      g_coop = -1, g_host = -1;
bool     g_init = false, g_registered = false, g_external = false;
unsigned g_frame = 0;
char     g_sav[1024];
CoopSaveStats g_st = {};

uint8_t  g_pending[kCoopRecBytes];  bool g_pending_have = false;   // host: guest bits
uint8_t  g_hostrec[kCoopRecBytes];  bool g_host_have = false;      // guest: host bits
bool     g_host_blank = false, g_host_dirty = false;
bool     g_answer = false;           // host: a SAVE_REQ is waiting
uint8_t  g_last_sent[kCoopRecBytes]; bool g_last_have = false;
ULONGLONG g_last_send_ms = 0, g_last_req_ms = 0, g_json_ms = 0;
bool     g_json_dirty = false;
unsigned g_seq = 0;
bool     g_probe_done = false, g_forge_done = false, g_fetched = false;
// host: a save that did not reach the disk is retried until it does
bool     g_save_retry = false, g_atexit = false;
unsigned g_retry_frame = 0, g_fail_seen = 0, g_retry_tries = 0;
ULONGLONG g_retry_ms = 0;
constexpr unsigned kRetryFrames = 30;     // bounded backoff: every 30 frames
constexpr unsigned kRetryMs = 2000;       // and at least every 2 s tick

bool coop() { if (g_coop < 0) g_coop = env_on("SM64DS_COOP") ? 1 : 0; return g_coop == 1; }
bool host() { if (g_host < 0) g_host = env_on("SM64DS_COOP_HOST") ? 1 : 0; return g_host == 1; }

// ---- MPSAFE1: the grant guard (coop_grant_guard.h) and the visit memory it needs ----
CoopGrantGuard g_guard;
uint32_t g_visit_t[4][kGgCourses];
bool     g_visit_seen[4][kGgCourses];
unsigned g_epoch[4];  bool g_epoch_have[4];
bool     g_course_known[kGgCourses];  bool g_course_init = false;

int course_of_level(int level) {
    if (level < 0 || level >= 52) return -1;
    const int c = SUBLEVEL_LEVEL_TABLE[level];
    return (c >= 0 && c < kGgCourses) ? c : -1;
}

// What each room seat has been doing lately, from its SYNL presence: which course it was in.
void note_visits(uint32_t now) {
    if (!g_course_init) {
        g_course_init = true;
        for (int l = 0; l < 52; ++l) { const int c = course_of_level(l); if (c >= 0) g_course_known[c] = true; }
    }
    for (int s = 0; s < 4; ++s) {
        HubSeatInfo si;
        if (comms_hub_seat_info(s, &si) && si.present) {
            if (!g_epoch_have[s] || g_epoch[s] != si.join_epoch) {   // a seat that left and came back starts fresh
                std::memset(g_visit_t[s], 0, sizeof g_visit_t[s]);
                std::memset(g_visit_seen[s], 0, sizeof g_visit_seen[s]);
                g_guard.reset_seat(s);
                g_epoch[s] = si.join_epoch; g_epoch_have[s] = true;
            }
        }
        HubPresence p;
        if (hub_presence_get(s, &p, nullptr) && p.in_level()) {
            const int c = course_of_level(p.level);
            if (c >= 0) { g_visit_t[s][c] = now; g_visit_seen[s][c] = true; }
        }
    }
}

// Courses this seat was seen in within kGgVisitMs. A course no level maps to is never tested (fail open).
void visited_of(int seat, uint32_t now, uint8_t *out /* kGgCourses */) {
    for (int c = 0; c < kGgCourses; ++c) {
        out[c] = !g_course_known[c] ||
                 (seat >= 0 && seat < 4 && g_visit_seen[seat][c] &&
                  (uint32_t)(now - g_visit_t[seat][c]) <= (uint32_t)kGgVisitMs);
    }
}

bool live_valid() {
    return std::memcmp(data_0209caa0, kMagic, 4) == 0 && data_0209caa0[0x328] == 0;
}

// magic + the mergeable bits only: what goes on the wire
void mask_record(uint8_t *out, const uint8_t *rec) {
    const uint8_t *mk = coop_mono_mask();
    std::memset(out, 0, kCoopRecBytes);
    for (int i = 0; i < kCoopRecBytes; ++i) out[i] = (uint8_t)(rec[i] & mk[i]);
    std::memcpy(out, kMagic, 4);
}

// ---- files -----------------------------------------------------------------
bool ends_coopsav(const char *p) {
    size_t n = std::strlen(p);
    if (n < 8) return false;
    return _stricmp(p + n - 8, ".coopsav") == 0;
}

void mkdirs(const char *path) {
    char buf[1100];
    std::snprintf(buf, sizeof buf, "%s", path);
    for (char *p = buf + 1; *p; ++p) {
        if (*p == '\\' || *p == '/') {
            char c = *p; *p = '\0';
            CreateDirectoryA(buf, NULL);
            *p = c;
        }
    }
}

void iso_now(char *out, size_t cap) {
    std::time_t t = std::time(nullptr);
    std::tm tmv;
    gmtime_s(&tmv, &t);
    std::strftime(out, cap, "%Y-%m-%dT%H:%M:%SZ", &tmv);
}

void json_path(char *out, size_t cap) {
    std::snprintf(out, cap, "%s", g_sav);
    size_t n = std::strlen(out);
    if (n >= 8) std::snprintf(out + n - 8, cap - (n - 8), ".json");
}

// <name>.json: display name, created, last played, star count. No personal data.
void write_json(int stars) {
    char jp[1100];
    json_path(jp, sizeof jp);
    char created[40] = "";
    if (std::FILE *f = std::fopen(jp, "rb")) {
        char buf[2048];
        size_t n = std::fread(buf, 1, sizeof buf - 1, f);
        buf[n] = '\0';
        std::fclose(f);
        const char *k = std::strstr(buf, "\"created\":\"");
        if (k) {
            k += 11;
            size_t i = 0;
            while (k[i] && k[i] != '"' && i < sizeof created - 1) { created[i] = k[i]; ++i; }
            created[i] = '\0';
        }
        if (stars < 0) {
            const char *s = std::strstr(buf, "\"stars\":");
            if (s) stars = std::atoi(s + 8);
        }
    }
    char now[40];
    iso_now(now, sizeof now);
    if (!created[0]) std::snprintf(created, sizeof created, "%s", now);
    if (stars < 0) stars = 0;
    const char *nm = std::getenv("SM64DS_COOP_SAVE_NAME");
    char name[80];
    size_t j = 0;
    if (nm) for (size_t i = 0; nm[i] && j < 64; ++i) {
        unsigned char c = (unsigned char)nm[i];
        name[j++] = (c < 0x20 || c == '"' || c == '\\') ? '_' : (char)c;
    }
    name[j] = '\0';
    if (!name[0]) std::snprintf(name, sizeof name, "Coop file");
    char tmp[1200];
    std::snprintf(tmp, sizeof tmp, "%s.%lu.tmp", jp, (unsigned long)GetCurrentProcessId());
    if (std::FILE *f = std::fopen(tmp, "wb")) {
        std::fprintf(f, "{\"version\":1,\"name\":\"%s\",\"created\":\"%s\","
                        "\"last_played\":\"%s\",\"stars\":%d}\n",
                     name, created, now, stars);
        std::fclose(f);
        if (!MoveFileExA(tmp, jp, MOVEFILE_REPLACE_EXISTING)) std::remove(tmp);
    }
}

// ---- records ---------------------------------------------------------------
// The host current record: live, else the file slot 0 (checksum checked), else
// none (blank).
bool host_record(uint8_t *out68, bool *blank) {
    if (live_valid()) {
        std::memcpy(out68, data_0209caa0, kCoopRecBytes);
        *blank = false;
        return true;
    }
    for (int copy = 0; copy < 2; ++copy) {
        uint8_t b[10 + kCoopRecBytes];
        if (!ntr::backup::read(copy ? 0x1000u : 0u, b, sizeof b)) continue;
        if (std::memcmp(b + 2, kCoopTag, 8) != 0) continue;
        uint16_t crc = (uint16_t)(b[0] | (b[1] << 8));
        if (crc != coop_record_checksum(kCoopTag, b + 10, kCoopRecBytes)) continue;
        if (std::memcmp(b + 10, kMagic, 4) != 0) continue;
        std::memcpy(out68, b + 10, kCoopRecBytes);
        *blank = false;
        return true;
    }
    std::memset(out68, 0, kCoopRecBytes);
    *blank = true;
    return false;
}

bool send_ctl(int sub, const uint8_t *payload, int plen) {
    uint8_t dg[64 + 700];
    int n = hub_coop_encode(kHubTagCoopCtl, sub, ++g_seq, payload, plen, dg, sizeof dg);
    if (n <= 0) return false;
    return comms_hub_send(dg, n);
}

void send_rec(bool from_host, bool blank, const uint8_t *rec) {
    uint8_t pl[kCoopRecPayload];
    uint8_t m[kCoopRecBytes];
    std::memset(m, 0, sizeof m);
    if (!blank) mask_record(m, rec);
    coop_rec_encode(pl, from_host, blank, blank ? nullptr : m);
    if (send_ctl(kCoopSaveSubRec, pl, kCoopRecPayload)) {
        if (from_host) ++g_st.rec_sent_host; else ++g_st.rec_sent_guest;
    }
    g_last_send_ms = GetTickCount64();
}

void on_ctl(int seat, const unsigned char *d, int len, void *) {
    coop_save_handle(seat, d, len);
}

void exit_save();

void ensure_init() {
    if (g_init) return;
    g_init = true;
    if (!coop()) return;
    if (host()) {
        coop_save_prepare();
        if (!g_atexit) { g_atexit = true; std::atexit(exit_save); }
    }
    if (!g_external && !g_registered)
        g_registered = hub_on_kind(kHubCoopCtl, on_ctl, nullptr);
    std::fprintf(stderr, "[coopsave] init role=%s handler=%s file=%s\n",
                 host() ? "host" : "guest",
                 g_external ? "external" : (g_registered ? "own" : "NONE"),
                 host() ? g_sav : "(temp image)");
}

// Frame the seed record the ROM way (the guest first record only).
void write_guest_seed(const uint8_t *host68) {
    uint8_t rec[kCoopRecBytes];
    std::memset(rec, 0, sizeof rec);
    std::memcpy(rec, kMagic, 4);
    rec[0x41] = 3;                 // SetDefaultValues: character 3
    rec[0x08] |= 8;                // SetDefaultValues: flags2 bit 3
    coop_merge_record(rec, host68);
    uint8_t img[10 + kCoopRecBytes];
    uint16_t crc = coop_record_checksum(kCoopTag, rec, kCoopRecBytes);
    img[0] = (uint8_t)(crc & 0xFF);
    img[1] = (uint8_t)(crc >> 8);
    std::memcpy(img + 2, kCoopTag, 8);
    std::memcpy(img + 10, rec, kCoopRecBytes);
    ntr::backup::write(0u, img, sizeof img);
    ntr::backup::write(0x1000u, img, sizeof img);
}

bool retry_off() {
    static int off = -1;
    if (off < 0) { const char *e = std::getenv("SM64DS_COOPSAVE_NO_RETRY"); off = (e && *e == '1') ? 1 : 0; }
    return off == 1;
}

void mark_retry() {
    if (retry_off()) return;
    g_save_retry = true;
    g_retry_frame = g_frame + kRetryFrames;
    g_retry_ms = GetTickCount64();
}

void save_live() {
    int ok = _ZN8SaveData8SaveFileEjP12FileSaveData(0, data_0209caa0);
    ++g_st.saves_written;
    std::fprintf(stderr, "[coopsave] saved slot 0 through SaveFile ok=%d stars=%d\n",
                 ok, coop_record_stars(data_0209caa0));
    g_json_dirty = true;
    g_fail_seen = ntr::backup::stats().flush_failures;
    if (!ok) {
        mark_retry();
    } else if (g_save_retry) {
        g_save_retry = false;
        std::fprintf(stderr, "[coopsave] save retry landed after %u tries\n", g_retry_tries);
        g_retry_tries = 0;
    }
}

// A flush that failed anywhere (the ROM's own SaveCurrentFile included) leaves
// the disk behind the live record: arm the retry.
void watch_flush_failures() {
    const unsigned fs = ntr::backup::stats().flush_failures;
    if (fs != g_fail_seen) {
        g_fail_seen = fs;
        if (!g_save_retry) {
            std::fprintf(stderr, "[coopsave] a save did not reach the disk, will retry\n");
            mark_retry();
        }
    }
}

// Last synchronous attempt at exit (exit() runs atexit callbacks).
void exit_save() {
    if (!coop() || !host() || retry_off()) return;
    if (live_valid()) {
        watch_flush_failures();
        if (g_save_retry) {
            save_live();
            if (g_save_retry)
                std::fprintf(stderr, "[coopsave] EXIT: the coop save did NOT reach the disk "
                             "(%s); the last merged progress is lost\n", g_sav);
        }
    }
}

// ---- proof drivers ---------------------------------------------------------
// SM64DS_COOPSAVE_PROBE=star:<course>:<bit>@<frame>: the game own star choke
// point (CollectStar, then SaveCurrentFile) at a chosen frame, once the file
// is open. Stands in for walking into a star.
void run_probe() {
    if (g_probe_done) return;
    const char *e = std::getenv("SM64DS_COOPSAVE_PROBE");
    if (!e) { g_probe_done = true; return; }
    int c = 0, b = 0, f = 0;
    if (std::sscanf(e, "star:%d:%d@%d", &c, &b, &f) != 3 || c < 0 || c > 29 ||
        b < 0 || b > 7) { g_probe_done = true; return; }
    if ((int)g_frame < f || !live_valid()) return;
    g_probe_done = true;
    CollectStar(c, b);
    int ok = _ZN8SaveData15SaveCurrentFileEv();
    std::fprintf(stderr, "[coopsave] PROBE star course %d bit %d collected "
                 "(CollectStar + SaveCurrentFile ok=%d) stars=%d\n",
                 c, b, ok, coop_record_stars(data_0209caa0));
}

// SM64DS_COOPSAVE_FORGE=<mode>@<frame>: send ONE hand-made SAVE_REC / SAVE_REQ.
//   clear      guest record that is magic only (tries to clear every bit)
//   setall     guest record with every mergeable bit set (the honest limit)
//   hostclear  a "host" record that is magic only, sent by a guest
//   badlen     payload 40 bytes    badflags  reserved bytes non-zero
//   badreq     SAVE_REQ with a non-zero payload    badrole  from_host = 2
void run_forge() {
    if (g_forge_done) return;
    const char *e = std::getenv("SM64DS_COOPSAVE_FORGE");
    if (!e) { g_forge_done = true; return; }
    char mode[24] = ""; int f = 0;
    if (std::sscanf(e, "%23[a-z]@%d", mode, &f) != 2) { g_forge_done = true; return; }
    if ((int)g_frame < f || comms_hub_state() != kHubLive) return;
    g_forge_done = true;
    uint8_t pl[kCoopRecPayload];
    std::memset(pl, 0, sizeof pl);
    std::memcpy(pl + 4, kMagic, 4);
    int plen = kCoopRecPayload, sub = kCoopSaveSubRec;
    if (!std::strcmp(mode, "setall")) {
        const uint8_t *mk = coop_mono_mask();
        for (int i = 4; i < kCoopRecBytes; ++i) pl[4 + i] = mk[i];
    } else if (!std::strcmp(mode, "hostclear")) pl[0] = 1;
    else if (!std::strcmp(mode, "badlen")) plen = 40;
    else if (!std::strcmp(mode, "badflags")) pl[2] = 1;
    else if (!std::strcmp(mode, "badrole")) pl[0] = 2;
    else if (!std::strcmp(mode, "badreq")) { sub = kCoopSaveSubReq; plen = 4; pl[1] = 7; }
    else if (std::strcmp(mode, "clear")) return;
    bool ok = send_ctl(sub, pl, plen);
    std::fprintf(stderr, "[coopsave] FORGE %s sent=%d len=%d\n", mode, ok, plen);
}

// KEYDOOR10 proof knobs (inert unless named):
//   SM64DS_KEYDOOR_SAVE=<frame>          the host writes its live record to the coop file once at that frame (the
//                                        session-one "save" of a save / quit / reload row)
//   SM64DS_KEYDOOR_AT=<frame>:<variant>  from that frame, in the castle interior (level 2), list the doors (class 353)
//                                        once, then stand my player on the door of that variant (param1, the
//                                        door table index: 7 + key index for a key door) facing it for 20 frames
//                                        (three times, 300 frames apart: an unrelated message on screen holds the door),
//                                        the box daDoor_c::func_ov100_021452e4 tests; then log flags1, the door's
//                                        state record (+0x140, as a step from the closed state's record) and my
//                                        position every 30 frames for 900 frames.
bool g_kd_save_done = false, g_kd_listed = false;
unsigned g_kd_from = 0, g_kd_t = 0; int g_kd_var = -1; char *g_kd_door = nullptr; const void *g_kd_closed = nullptr;
void run_keydoor() {
    static bool read = false;
    if (!read) {
        read = true;
        const char *e = std::getenv("SM64DS_KEYDOOR_AT");
        if (!e || std::sscanf(e, "%u:%d", &g_kd_from, &g_kd_var) != 2) g_kd_var = -1;
        if (!std::getenv("SM64DS_KEYDOOR_SAVE")) g_kd_save_done = true;
    }
    if (!g_kd_save_done && host() && live_valid() && (int)g_frame >= std::atoi(std::getenv("SM64DS_KEYDOOR_SAVE"))) {
        g_kd_save_done = true;
        unsigned f1 = 0; std::memcpy(&f1, data_0209caa0 + 4, 4);
        std::fprintf(stderr, "[keydoor] host saves for the reload: flags1=%08x\n", f1);
        save_live();
    }
    if (g_kd_var < 0 || g_frame < g_kd_from || data_0209f2f8 != 2) return;
    const int me = data_0209f250;
    char *pl = (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
    if (!pl) return;
    if (!g_kd_listed) {
        g_kd_listed = true;
        unsigned f1 = 0; std::memcpy(&f1, data_0209caa0 + 4, 4);
        std::fprintf(stderr, "[keydoor] castle interior: my flags1=%08x (key 0 held %d, key 1 held %d, key 3 held %d, "
                     "door 0 unlocked %d, door 1 unlocked %d)\n", f1, (f1 >> 1) & 1, (f1 >> 2) & 1, (f1 >> 4) & 1,
                     (f1 >> 8) & 1, (f1 >> 9) & 1);
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (*(const unsigned short *)(a + 0x0c) != 353) continue;
            const int v = *(const int *)(a + 8);
            const int *q = (const int *)(a + 0x5c);
            std::fprintf(stderr, "[keydoor] door variant %d at %d,%d,%d angle %04x\n", v, q[0] >> 12, q[1] >> 12, q[2] >> 12,
                         (unsigned)(unsigned short)*(const short *)(a + 0x8e));
            if (v == g_kd_var && !g_kd_door) { g_kd_door = a; g_kd_closed = *(void *const *)(a + 0x140); }
        }
        if (!g_kd_door) { std::fprintf(stderr, "[keydoor] no door of variant %d here\n", g_kd_var); g_kd_var = -1; return; }
    }
    char *d = g_kd_door;
    ++g_kd_t;
    if (g_kd_t % 300 <= 20 && g_kd_t % 300 >= 1) {   // three tries (t 1, 301, 601): a message on screen holds the door
        for (int j = 0; j < 3; ++j) ((int *)(pl + 0x5c))[j] = ((const int *)(d + 0x5c))[j];
        *(short *)(pl + 0x8e) = (short)(*(const short *)(d + 0x8e) + 0x8000);
    }
    if (g_kd_t % 300 <= 3 || g_kd_t % 300 == 21 || g_kd_t % 30 == 0) {
        unsigned f1 = 0; std::memcpy(&f1, data_0209caa0 + 4, 4);
        const long step = ((const char *)*(void *const *)(d + 0x140) - (const char *)g_kd_closed) / 16;
        const int *q = (const int *)(pl + 0x5c);
        std::fprintf(stderr, "[keydoor] t=%u door variant %d state step %ld (0 closed, 1 shut-message, 3 key talk, 4/5 going "
                     "through) flags1=%08x me %d,%d,%d facing %04x, me in the door's frame %d,%d,%d (door facing %04x)\n", g_kd_t,
                     g_kd_var, step, f1, q[0] >> 12, q[1] >> 12, q[2] >> 12, (unsigned)(unsigned short)*(const short *)(pl + 0x8e),
                     ((const int *)(d + 0x80))[0] >> 12, ((const int *)(d + 0x80))[1] >> 12, ((const int *)(d + 0x80))[2] >> 12,
                     (unsigned)(unsigned short)*(const short *)(d + 0x8e));
    }
    if (g_kd_t >= 900) g_kd_var = -1;
}

// ---- RABBIT1 (run coopfix1): the rabbit keys ---------------------------------
// A caught rabbit's key is ONE ROM call, func_02013868(kind, character), made by the key's carried entry (d_a_obj_mip_key.cpp,
// func_ov085_0212d038). It sets bit (kind << 2 | character) in TWO words: the file record's +0x0c (what the rabbit asks to know it was
// caught, func_02013890, and what the room record carries) and the minigame record's unlock word at +0x48 of the live block
// (func_0201361c; the minigame menu reads ONLY this one: MiniGm_IsUnlocked -> func_02013638). The minigame record is not part of
// the coop record (+0x00..+0x43), so before this a friend's key reached everyone's +0x0c and nobody's minigame menu, the host saved
// the merged +0x0c with SaveFile only (never the minigame record), and the catcher's own unlock lived in a guest temp image that is
// deleted when the room ends. So, every frame of a coop room, once the file is open:
//   1. the live minigame unlock word takes every key bit of the room's +0x0c word (the ROM's own invariant: func_02013868 is the
//      only writer of +0x0c and it always sets both). The host then saves its minigame record into the .coopsav, so the room's
//      file holds the unlock as well as the key; a late joiner or a restarted host gets it from +0x0c on the first frame.
//   2. every key bit the room holds is ORed into the minigame record of this player's OWN single player save on disk
//      (ntr/backup.cpp solo_minigame_unlock), at once, so it survives the room ending, the window being closed or a crash.
// SM64DS_COOP_RABBIT_FIX=0 turns both off (the "before" of the proof rows).
// Proof knobs, off unless set:
//   SM64DS_COOP_RABBIT_GRANT=<frame>:<kind>  at that frame of the room, what the key pickup does for MY character: func_02013868
//                                            (kind, my character) and SaveData::SaveCurrentFile (the "Saving" box after the key)
//   SM64DS_COOP_RABBIT_LOG=<kind>            a [cooprabbit] line when the key or unlock word changes, and every 600 frames: both
//                                            words, the rabbit's "already caught" answer for that kind per character, the menu's test
constexpr uint32_t kKeyBits = 0x0FFFFFFFu;   // 7 kinds x 4 characters (coop_save_wire.h: +0x0c is 28 bits)
uint32_t  g_home_done = 0;                   // key bits already in this player's own save
ULONGLONG g_home_ms = 0;
uint32_t  g_rl_keys = 0xFFFFFFFFu, g_rl_mg = 0xFFFFFFFFu;
bool      g_grant_done = false;

bool rabbit_fix() {
    static int v = -1;
    if (v < 0) { const char *e = std::getenv("SM64DS_COOP_RABBIT_FIX"); v = (e && *e == '0') ? 0 : 1; }
    return v == 1;
}
uint32_t rd32le(const uint8_t *p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
int my_character() {
    const int me = data_0209f250;
    const char *pl = (me >= 0 && me < 16) ? (const char *)data_0209f394[me] : nullptr;
    return pl ? (int)(*(const unsigned *)(pl + 0x08) & 3) : -1;   // Player param1 = the character (d_a_mip.cpp reads pl->param1)
}

void rabbit_frame() {
    if (!live_valid()) return;
    const char *gr = std::getenv("SM64DS_COOP_RABBIT_GRANT");
    if (gr && !g_grant_done) {
        unsigned f = 0; int kind = 0;
        if (std::sscanf(gr, "%u:%d", &f, &kind) == 2 && g_frame >= f && kind >= 0 && kind < 7) {
            const int ch = my_character();
            if (ch >= 0) {
                g_grant_done = true;
                const int before = func_02013890(kind, ch) != 0;
                func_02013868(kind, ch);
                const int saved = _ZN8SaveData15SaveCurrentFileEv();
                std::fprintf(stderr, "[cooprabbit-knob] f=%u key caught: kind %d character %d (bit %d), had it before %d; "
                             "SaveCurrentFile ok=%d; +0x0c=%08x unlock=%08x\n", g_frame, kind, ch, kind * 4 + ch, before, saved,
                             rd32le(data_0209caa0 + 0x0c), rd32le(data_0209caa0 + 0x48));
            }
        }
    }
    const uint32_t keys = rd32le(data_0209caa0 + 0x0c) & kKeyBits;
    if (rabbit_fix()) {
        const uint32_t mg = rd32le(data_0209caa0 + 0x48);
        const uint32_t add = keys & ~mg;
        if (add) {
            const uint32_t now_mg = mg | add;
            std::memcpy(data_0209caa0 + 0x48, &now_mg, 4);
            int ok = -1;
            if (host()) ok = _ZN8SaveData13SaveMinigamesEP16MinigameSaveData(data_0209caa0 + 0x44);
            std::fprintf(stderr, "[cooprabbit] f=%u minigame unlock word took the room's key bits %08x: %08x -> %08x (%s)\n", g_frame,
                         add, mg, now_mg, host() ? (ok ? "host: minigame record saved to the room file" : "host: minigame save FAILED")
                                                   : "guest");
        }
        const ULONGLONG now = GetTickCount64();
        if ((keys & ~g_home_done) && (g_home_ms == 0 || now - g_home_ms >= 2000)) {
            g_home_ms = now;
            char msg[1400];
            const int r = ntr::backup::solo_minigame_unlock(keys, msg, sizeof msg);
            if (r >= 0) g_home_done |= keys;
            std::fprintf(stderr, "[cooprabbit] f=%u own save: %s: %s\n", g_frame,
                         r > 0 ? "key bits written" : r == 0 ? "already there" : "NOT written (retry in 2 s)", msg);
        }
    }
    if (const char *lg = std::getenv("SM64DS_COOP_RABBIT_LOG")) {
        const int kind = std::atoi(lg);
        const uint32_t mg = rd32le(data_0209caa0 + 0x48);
        if (keys != g_rl_keys || mg != g_rl_mg || (g_frame % 600) == 0) {
            g_rl_keys = keys; g_rl_mg = mg;
            std::fprintf(stderr, "[cooprabbit] f=%u me=char%d keys(+0x0c)=%08x unlock(+0x48)=%08x kind %d: caught-before "
                         "m%d l%d w%d y%d, menu-unlocked m%d l%d w%d y%d\n", g_frame, my_character(), keys, mg, kind,
                         func_02013890(kind, 0) != 0, func_02013890(kind, 1) != 0, func_02013890(kind, 2) != 0,
                         func_02013890(kind, 3) != 0, func_02013638(kind * 4 + 0) != 0, func_02013638(kind * 4 + 1) != 0,
                         func_02013638(kind * 4 + 2) != 0, func_02013638(kind * 4 + 3) != 0);
        }
    }
}

}  // namespace

// ---- public ----------------------------------------------------------------
bool coop_save_active() { return coop(); }

void coop_save_leave_flush() { exit_save(); }

bool coop_save_file_has_record(const char *path) {
    if (!path || !*path) return false;
    std::FILE *f = std::fopen(path, "rb");
    if (!f) return false;
    static uint8_t img[0x1000 + 10 + kCoopRecBytes];
    const size_t got = std::fread(img, 1, sizeof img, f);
    std::fclose(f);
    for (int copy = 0; copy < 2; ++copy) {
        const size_t off = copy ? 0x1000u : 0u;
        if (got < off + 10 + kCoopRecBytes) continue;
        const uint8_t *b = img + off;
        if (std::memcmp(b + 2, kCoopTag, 8) != 0) continue;
        if ((uint16_t)(b[0] | (b[1] << 8)) != coop_record_checksum(kCoopTag, b + 10, kCoopRecBytes)) continue;
        if (std::memcmp(b + 10, kMagic, 4) != 0) continue;
        return true;
    }
    return false;
}

void coop_save_session_reset() {
    g_coop = g_host = -1;
    g_init = false;
    g_registered = false;      // comms_hub_teardown forgot every hub_on_kind handler
    g_frame = 0;
    g_sav[0] = 0;
    g_st = CoopSaveStats{};
    g_pending_have = g_host_have = g_host_blank = g_host_dirty = g_answer = g_last_have = false;
    g_last_send_ms = g_last_req_ms = g_json_ms = 0;
    g_json_dirty = false;
    g_probe_done = g_forge_done = g_fetched = false;
    g_save_retry = false;      // g_atexit stays: exit_save is registered once per process and asks coop() itself
    g_retry_frame = g_fail_seen = g_retry_tries = 0;
    g_retry_ms = 0;
    g_guard = CoopGrantGuard{};
    std::memset(g_visit_t, 0, sizeof g_visit_t);
    std::memset(g_visit_seen, 0, sizeof g_visit_seen);
    std::memset(g_epoch, 0, sizeof g_epoch);
    std::memset(g_epoch_have, 0, sizeof g_epoch_have);
}

int coop_save_fetch_step(int timeout_ms) {
    static ULONGLONG t0 = 0;
    if (!coop() || host() || g_fetched) { t0 = 0; return 1; }
    ensure_init();
    const ULONGLONG now = GetTickCount64();
    if (!t0) t0 = now;
    if (g_host_have) {
        if (!g_host_blank) write_guest_seed(g_hostrec);
        ++g_st.fetch_ok;
        g_fetched = true;
        g_host_dirty = false;
        std::fprintf(stderr, "[coopsave] fetch ok in %llu ms (in place): %s, stars %d\n", (unsigned long long)(now - t0),
                     g_host_blank ? "new file (blank)" : "host record seeded into slot 0", g_host_blank ? 0 : coop_record_stars(g_hostrec));
        t0 = 0;
        return 1;
    }
    if ((long long)(now - t0) >= timeout_ms) {
        ++g_st.fetch_timeout;
        std::fprintf(stderr, "[coopsave] fetch TIMEOUT after %llu ms (budget %d, in place): the host did not answer\n",
                     (unsigned long long)(now - t0), timeout_ms);
        t0 = 0;
        return -1;
    }
    if (comms_hub_state() == kHubLive && now - g_last_req_ms >= 250) {
        uint8_t z[kCoopReqPayload] = {0, 0, 0, 0};
        if (send_ctl(kCoopSaveSubReq, z, sizeof z)) ++g_st.req_sent;
        g_last_req_ms = now;
    }
    return 0;
}
bool coop_save_is_host() { return coop() && host(); }
CoopSaveStats coop_save_stats() { return g_st; }
void coop_save_set_external_dispatch(bool on) { g_external = on; }

bool coop_save_prepare() {
    if (!coop() || !host()) return true;
    const char *p = std::getenv("SM64DS_COOP_SAVE");
    if (!p || !*p) p = std::getenv("SM64DS_SAVE_PATH");
    if (!p || !*p || std::strlen(p) + 1 >= sizeof g_sav || !ends_coopsav(p)) return false;
    std::snprintf(g_sav, sizeof g_sav, "%s", p);
    mkdirs(g_sav);
    // A new file is an erased image. CREATE_NEW: never touches an existing one.
    HANDLE h = CreateFileA(g_sav, GENERIC_WRITE, 0, NULL, CREATE_NEW,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    bool created = false;
    if (h != INVALID_HANDLE_VALUE) {
        static uint8_t blank[ntr::backup::kSize];
        std::memset(blank, ntr::backup::kErased, sizeof blank);
        DWORD w = 0;
        created = WriteFile(h, blank, sizeof blank, &w, NULL) && w == sizeof blank;
        CloseHandle(h);
        if (!created) DeleteFileA(g_sav);
    }
    char jp[1100];
    json_path(jp, sizeof jp);
    if (created || GetFileAttributesA(jp) == INVALID_FILE_ATTRIBUTES) write_json(0);
    std::fprintf(stderr, "[coopsave] host file %s (%s)\n", g_sav,
                 created ? "created, a new file" : "reopened");
    return GetFileAttributesA(g_sav) != INVALID_FILE_ATTRIBUTES;
}

// One line per drop, the first 24 only (a hostile peer must not fill a log).
static bool drop(unsigned *counter, int seat, const char *why) {
    static unsigned said = 0;
    ++*counter;
    if (said < 24) {
        ++said;
        std::fprintf(stderr, "[coopsave] DROP seat %d: %s\n", seat, why);
    }
    return true;
}

bool coop_save_handle(int seat, const unsigned char *d, int len) {
    if (!d || len < kHubCoopHeaderBytes) return false;
    const int sub = d[6];
    if (sub < kCoopSaveSubLo || sub > kCoopSaveSubHi) return false;
    if (!coop()) return true;
    if (!hub_coop_validate(kHubTagCoopCtl, d, len))
        return drop(&g_st.dropped_bad, seat, "bad SYNB head");
    const uint8_t *pl = d + kHubCoopHeaderBytes;
    const int plen = len - kHubCoopHeaderBytes;
    if (sub == kCoopSaveSubReq) {
        if (plen != kCoopReqPayload || (pl[0] | pl[1] | pl[2] | pl[3])) {
            return drop(&g_st.dropped_bad, seat, "malformed SAVE_REQ");
        }
        if (host()) { ++g_st.req_recv; g_answer = true; }
        return true;
    }
    if (sub == kCoopSaveSubRec) {
        bool blank = false;
        uint8_t rec[kCoopRecBytes];
        int r = coop_rec_decode(pl, plen, &blank, rec);
        if (r < 0) return drop(&g_st.dropped_bad, seat, "malformed SAVE_REC");
        if (!blank && std::memcmp(rec, kMagic, 4) != 0)
            return drop(&g_st.dropped_bad, seat, "SAVE_REC without the file magic");
        if ((r == 0) != host())
            return drop(&g_st.dropped_role, seat, "SAVE_REC for the other role");
        if (r == 0) {
            ++g_st.rec_recv_guest;
            if (!g_pending_have) std::memset(g_pending, 0, sizeof g_pending);
            {   // MPSAFE1: believe only what the sender could have earned (coop_grant_guard.h)
                uint8_t base[kCoopRecBytes], vis[kGgCourses];
                if (live_valid()) std::memcpy(base, data_0209caa0, sizeof base); else std::memset(base, 0, sizeof base);
                const uint8_t *mk = coop_mono_mask();
                for (int i = 0; i < kCoopRecBytes; ++i) base[i] |= (uint8_t)(g_pending[i] & mk[i]);
                const uint32_t now = (uint32_t)GetTickCount64();
                note_visits(now);
                visited_of(seat, now, vis);
                const GgVerdict gv = g_guard.admit(seat, base, rec, now, vis);
                if (gv != kGgOk) {
                    static char why[64];
                    std::snprintf(why, sizeof why, "SAVE_REC grants more than earned (%s)", gg_name(gv));
                    return drop(&g_st.dropped_grant, seat, why);
                }
            }
            int n = coop_merge_record(g_pending, rec);
            g_pending_have = true;
            std::fprintf(stderr, "[coopsave] host heard seat %d record, %d new bits "
                         "queued (stars in it %d)\n", seat, n, coop_record_stars(rec));
        } else {
            ++g_st.rec_recv_host;
            if (g_host_have && !blank) {   // MPSAFE1: after the first record, a "host" record is held to the allowance
                uint8_t base[kCoopRecBytes];
                std::memcpy(base, g_hostrec, sizeof base);
                if (live_valid()) {
                    const uint8_t *mk = coop_mono_mask();
                    for (int i = 0; i < kCoopRecBytes; ++i) base[i] |= (uint8_t)(data_0209caa0[i] & mk[i]);
                }
                const GgVerdict gv = g_guard.admit(seat, base, rec, (uint32_t)GetTickCount64(), nullptr);
                if (gv != kGgOk) {
                    static char why[64];
                    std::snprintf(why, sizeof why, "host SAVE_REC grants more than allowed (%s)", gg_name(gv));
                    return drop(&g_st.dropped_grant, seat, why);
                }
            }
            g_host_blank = blank;
            if (!g_host_have) std::memset(g_hostrec, 0, sizeof g_hostrec);
            if (!blank) coop_merge_record(g_hostrec, rec);   // only grows
            g_host_have = true;
            g_host_dirty = true;
        }
        return true;
    }
    if (sub == kStarTallySub) {          // SMALLS9: the per-player star tally (hal/star_tally.h)
        if (!star_tally_on_payload(host(), seat, pl, plen))
            return drop(&g_st.dropped_bad, seat, "malformed TALLY");
        return true;
    }
    return drop(&g_st.dropped_subtype, seat, "unknown sub-type in 0x20-0x2f");
}

bool coop_save_fetch(int timeout_ms) {
    if (!coop() || host()) { if (host()) coop_save_prepare(); return true; }
    if (g_fetched) return true;
    ensure_init();
    const ULONGLONG t0 = GetTickCount64();
    ULONGLONG last_req = 0;
    while (!g_host_have) {
        const ULONGLONG now = GetTickCount64();
        if ((long long)(now - t0) >= timeout_ms) {
            ++g_st.fetch_timeout;
            std::fprintf(stderr, "[coopsave] fetch TIMEOUT after %llu ms (budget %d): "
                         "the host did not answer\n",
                         (unsigned long long)(now - t0), timeout_ms);
            return false;
        }
        if (comms_hub_state() == kHubLive && now - last_req >= 250) {
            uint8_t z[kCoopReqPayload] = {0, 0, 0, 0};
            if (send_ctl(kCoopSaveSubReq, z, sizeof z)) ++g_st.req_sent;
            last_req = now;
        }
        comms_hub_pump();
        Sleep(10);
    }
    if (!g_host_blank) write_guest_seed(g_hostrec);
    ++g_st.fetch_ok;
    g_fetched = true;
    std::fprintf(stderr, "[coopsave] fetch ok in %llu ms: %s, stars %d\n",
                 (unsigned long long)(GetTickCount64() - t0),
                 g_host_blank ? "new file (blank)" : "host record seeded into slot 0",
                 g_host_blank ? 0 : coop_record_stars(g_hostrec));
    g_host_dirty = false;
    return true;
}

}  // namespace port

extern "C" void port_coop_save_frame(void) {
    using namespace port;
    // RABBIT1 proof knob, any session (solo too): SM64DS_RABBIT_READBACK=<frame>:<kind> does what the title's command 21 does
    // (src/game/stages/d_s_dsmt.cpp: SaveData::ReadMinigameData into the live record at +0x44, from the medium this process has
    // selected) and prints the unlock word it read and the minigame menu's own test (func_02013638) for that kind, per character.
    if (const char *rb = std::getenv("SM64DS_RABBIT_READBACK")) {
        static unsigned rbf = 0; static bool rb_done = false;
        unsigned f = 0; int kind = 0;
        ++rbf;
        if (!rb_done && std::sscanf(rb, "%u:%d", &f, &kind) == 2 && rbf >= f && kind >= 0 && kind < 7) {
            rb_done = true;
            uint32_t was, now_w, magic;
            std::memcpy(&was, data_0209caa0 + 0x48, 4);
            const int r = _ZN8SaveData16ReadMinigameDataEP16MinigameSaveData(data_0209caa0 + 0x44);
            std::memcpy(&now_w, data_0209caa0 + 0x48, 4);
            std::memcpy(&magic, data_0209caa0 + 0x44, 4);
            std::fprintf(stderr, "[rabbit-readback] f=%u medium %s (coop %d): ReadMinigameData=%d magic=%08x unlock word read=%08x "
                         "(live before %08x); kind %d menu-unlocked m%d l%d w%d y%d; file keys(+0x0c)=%08x
", rbf, ntr::backup::path(),
                         coop() ? 1 : 0, r, magic, now_w, was, kind, func_02013638(kind * 4) != 0,
                         func_02013638(kind * 4 + 1) != 0, func_02013638(kind * 4 + 2) != 0, func_02013638(kind * 4 + 3) != 0,
                         *(const uint32_t *)(data_0209caa0 + 0x0c));
        }
    }
    if (!coop()) return;
    ensure_init();
    ++g_frame;
    /* the ROM puts "ds mario" back at boot; a coop room keeps its own tag */
    if (std::memcmp(data_020a4b40, kCoopTag, 8) != 0) std::memcpy(data_020a4b40, kCoopTag, 8);
    star_tally_frame();
    note_visits((uint32_t)GetTickCount64());
    if ((g_frame % 600) == 0)       // MPSAFE1: the drop counters, so a hostile peer shows up in the log as numbers
        std::fprintf(stderr, "[coopsave] stat f=%u role=%s rec_guest=%u rec_host=%u merged=%u drop_bad=%u drop_role=%u "
                     "drop_sub=%u drop_grant=%u live_stars=%d\n", g_frame, host() ? "host" : "guest", g_st.rec_recv_guest,
                     g_st.rec_recv_host, g_st.merged_bits, g_st.dropped_bad, g_st.dropped_role, g_st.dropped_subtype,
                     g_st.dropped_grant, live_valid() ? coop_record_stars(data_0209caa0 + 0) : -1);
    run_probe();
    run_forge();
    run_keydoor();
    rabbit_frame();
    const ULONGLONG now = GetTickCount64();
    const bool live = comms_hub_state() == kHubLive;
    const bool others = live && comms_hub_seat_mask() != 0;

    if (live_valid()) {
        if (host() && g_pending_have) {
            int n = coop_merge_record(data_0209caa0, g_pending);
            g_pending_have = false;
            if (n) {
                g_st.merged_bits += (unsigned)n;
                std::fprintf(stderr, "[coopsave] host merged %d guest bits\n", n);
                save_live();
            }
        }
        if (!host() && g_host_dirty && !g_host_blank) {
            int n = coop_merge_record(data_0209caa0, g_hostrec);
            if (n) {
                g_st.merged_bits += (unsigned)n;
                std::fprintf(stderr, "[coopsave] guest took %d bits from the host record\n", n);
                save_live();
            }
        }
        g_host_dirty = false;
        if (host() && !retry_off()) {
            watch_flush_failures();
            if (g_save_retry && (g_frame >= g_retry_frame || GetTickCount64() - g_retry_ms >= kRetryMs)) {
                ++g_retry_tries;
                save_live();
            }
        }
        uint8_t sn[kCoopRecBytes];
        mask_record(sn, data_0209caa0);
        if (host()) {
            const bool changed = !g_last_have || std::memcmp(sn, g_last_sent, sizeof sn) != 0;
            if (others && (g_answer || changed || now - g_last_send_ms >= 2000)) {
                send_rec(true, false, data_0209caa0);
                std::memcpy(g_last_sent, sn, sizeof sn);
                g_last_have = true;
                g_answer = false;
            } else if (!others) {
                g_answer = false;
            }
        } else if (others) {
            const bool covered = g_host_have && coop_record_covers(g_hostrec, sn);
            if (!covered && (!g_last_have || std::memcmp(sn, g_last_sent, sizeof sn) != 0 ||
                             now - g_last_send_ms >= 1500)) {
                send_rec(false, false, data_0209caa0);
                std::memcpy(g_last_sent, sn, sizeof sn);
                g_last_have = true;
            }
        }
        if (host() && g_json_dirty && now - g_json_ms >= 1000) {
            write_json(coop_record_stars(data_0209caa0));
            g_json_dirty = false;
            g_json_ms = now;
        }
    } else if (host() && g_answer && others) {
        uint8_t rec[kCoopRecBytes];
        bool blank = true;
        host_record(rec, &blank);
        send_rec(true, blank, rec);
        g_answer = false;
    }
    if (live && others) {                // SMALLS9: my star tally to the host / the host table to all
        uint8_t tp[kStarTallyPayloadMax];
        const int tn = star_tally_take_outgoing(host(), tp);
        if (tn > 0) send_ctl(kStarTallySub, tp, tn);
    }
    if (!host() && live && !g_host_have && now - g_last_req_ms >= 250) {
        uint8_t z[kCoopReqPayload] = {0, 0, 0, 0};
        if (send_ctl(kCoopSaveSubReq, z, sizeof z)) ++g_st.req_sent;
        g_last_req_ms = now;
    }
}

// BOOT HOOK (one line in tests/walk_window.cpp's main, after the hub install).
// A host prepares its file; a guest fetches the host record when
// SM64DS_COOPSAVE_BOOT_FETCH=<ms> is set (the proof rows set it; COOPBOOT1 owns
// the real boot and calls coop_save_fetch itself, which is idempotent, and
// decides the exit). Inert outside a coop room.
extern "C" void port_coop_save_boot(void) {
    using namespace port;
    if (!coop()) return;
    ensure_init();
    const char *e = std::getenv("SM64DS_COOPSAVE_BOOT_FETCH");
    if (host() || !e || !*e) return;
    int ms = std::atoi(e);
    if (ms <= 0) ms = 3000;
    if (!coop_save_fetch(ms)) {
        std::fprintf(stderr, "[coopsave] the host did not answer for the save; "
                     "exiting with code 6\n");
        std::fflush(stderr);
        ExitProcess(6);
    }
}
