/* port/hal/narrow_return.cpp -- the probe behind tools/hostgen.py's NARROW_RETURN
 * wrappers (run rel042, lane NARROWRET1).
 *
 * On the DS a function that returns s8/u8/s16/u16/bool extends the value into the
 * whole of r0 before it returns, so a caller that declares it wider is right on the
 * cartridge. MSVC leaves the upper bytes of EAX unspecified, so the same caller reads
 * junk on the PC. hostgen's NARROW_RETURN table puts a wrapper in front of every such
 * ROM body that has a wide declarer anywhere; each wrapper reports every call to
 * port_narrowret_hook when this file is linked (other targets get the null default
 * the generated TUs carry, and pay one load and one test).
 *
 * SM64DS_NARROWRET=1 turns the recorder on. Per call site (the function and the
 * caller's return address) it counts:
 *   calls      every call through the wrapper;
 *   raw_bad    calls whose raw register (what the body left in EAX, which is what a
 *              WIDE caller read before the cure) differs from the value extended the
 *              way the ARM epilogue extends it;
 *   ret_bad    calls whose returned value differs from the extended one (equal to
 *              raw_bad while the cure is off, 0 once it is on);
 * and keeps the first three raw/extended pairs of each bad site. At exit it writes
 * narrowret_<pid>.tsv into SM64DS_NARROWRET_DIR (else the working directory) and
 * one summary line to stderr. port/tools/narrowret_live.py maps the return
 * addresses through walk_window.map to the calling function.
 *
 * Key absent: the hook stays null, nothing is allocated, nothing is written. The
 * site table is allocated only when the key is set, so the image carries no BSS
 * for it and every section after .bss sits where it did before this file.
 * One game thread calls these bodies; the table is not locked. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

typedef void (*port_nr_hook_t)(int raw, int ret, int kind, const char *name,
                               void *ra);
extern "C" port_nr_hook_t port_narrowret_hook = 0;

/* ---- THE WIDTH TABLE (SM64DS_NARROWRET_WIDTH=1) ------------------------------
 * Each row calls a cured function through a WIDE declaration -- `extern "C" int`,
 * the shape every wide caller in src/ and port/ spells -- with inputs chosen so
 * the correct result is negative (signed) or has the top bit of its width set
 * (unsigned), and compares the full 32-bit register it gets back with the NARROW
 * reference: the same body read at its own width (the renamed body, or the member
 * through its real decoration), which MSVC extends itself. Equal is a PASS. A row
 * whose function cannot return such a value (a masked field, a bounded count) says
 * so and is run at its largest value. The table runs once, on the first call any
 * wrapper makes (the ROM image and the hosted globals are up by then), prints to
 * stderr and ends the run; every global a row writes is put back first. */
struct NrVec3 { int x, y, z; };
extern "C" {
int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
short _ZN4cstd5atan2E5Fix12IiES1___nr(int y, int x);
int GetPlayerFlagByte(int i, int p);
unsigned char GetPlayerFlagByte__nr(int i, int p);
extern unsigned char data_020a1166[];
int DecIfAbove0_Byte(unsigned char *p);
unsigned char DecIfAbove0_Byte__nr(unsigned char *p);
int DecIfAbove0_Short(unsigned short *p);
unsigned short DecIfAbove0_Short__nr(unsigned short *p);
int Vec3_HorzAngle(const NrVec3 *a, const NrVec3 *b);
short Vec3_HorzAngle__nr(const NrVec3 *a, const NrVec3 *b);
int Vec3_VertAngle(const NrVec3 *a, const NrVec3 *b);
short Vec3_VertAngle__nr(const NrVec3 *a, const NrVec3 *b);
int GetLevelPart(int idx);
signed char GetLevelPart__nr(int idx);
extern signed char data_02075264[];
int IsAreaShowing(int idx);
unsigned char IsAreaShowing__nr(int idx);
extern void *data_0209f314;
int func_020456a0(int a0, int a1, int shift, int a3, short *tbl);
unsigned short func_020456a0__nr(int a0, int a1, int shift, int a3, short *tbl);
int _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int nx, int nz, short ang);
short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s__nr(void *self, int nx, int nz, short ang);
int _ZN8SaveData13GetCoinRecordEj(unsigned course);
extern unsigned char data_0209cad2[];
int func_02020168(void);
unsigned char func_02020168__nr(void);
int func_02020364(int a0);
unsigned char func_02020364__nr(int a0);
int func_020201f0(int a0);
unsigned char func_020201f0__nr(int a0);
int IsPlayerWarping(int a0);
unsigned char IsPlayerWarping__nr(int a0);
int GetStarCameraSetting(int idx);
unsigned char GetStarCameraSetting__nr(int idx);
extern unsigned data_02092134;
int func_ov007_020c3be0(int t, int a, int b);
unsigned short func_ov007_020c3be0__nr(int t, int a, int b);
int _ZN3OAM11GetObjWidthEii(int shape, int size);
int _ZN3OAM12GetObjHeightEii(int shape, int size);
int CountStarsCollectedInLevelToDisplay(int course);
unsigned char CountStarsCollectedInLevelToDisplay__nr(int course);
int _ZN8SaveData26CountStarsCollectedInLevelEj(unsigned course);
int NumStars(void);
unsigned char NumStars__nr(void);
}
/* The members, through their real decorations: a shadow struct names the same
   symbol the definition TU emits (?GetCoinRecord@SaveData@@SAEI@Z and friends). */
struct SaveData {
    static unsigned char GetCoinRecord(unsigned course);
    static unsigned char CountStarsCollectedInLevel(unsigned course);
};
struct OAM {
    static unsigned char GetObjWidth(int shape, int size);
    static unsigned char GetObjHeight(int shape, int size);
};

namespace {

int g_wpass, g_wrows;
bool g_width_pending;

void wrow(const char *name, const char *kind, int wide, int ext, const char *want,
          bool demanded)
{
    bool ok = wide == ext;
    bool met = true;
    if (!strcmp(want, "negative")) met = ext < 0;
    else if (!strcmp(want, "top bit")) met = !strcmp(kind, "u8") ? (ext & 0x80) != 0
                                                                 : (ext & 0x8000) != 0;
    g_wrows++;
    if (ok && (met || !demanded)) g_wpass++;
    fprintf(stderr, "[nrwidth] %-44s %-4s wide=0x%08x ext=%-7d want=%-16s %s\n",
            name, kind, (unsigned)wide, ext, want,
            !ok ? "FAIL (wide read differs)" : (!met && demanded) ? "FAIL (input missed)"
                                                                  : "PASS");
}

void run_width(void)
{
    fprintf(stderr, "[nrwidth] each function read through a WIDE declaration against "
                    "its narrow reference\n");
    {   int w = _ZN4cstd5atan2E5Fix12IiES1_(-0x3000, 0x10000);
        int e = _ZN4cstd5atan2E5Fix12IiES1___nr(-0x3000, 0x10000);
        wrow("cstd::atan2(-0x3000, 0x10000)", "s16", w, e, "negative", true); }
    {   int i = 0, pp = 5, o = i * 0x24 + pp;
        unsigned char save = data_020a1166[o];
        data_020a1166[o] = 0x9c;
        int w = GetPlayerFlagByte(i, pp), e = GetPlayerFlagByte__nr(i, pp);
        wrow("GetPlayerFlagByte (byte = 0x9c)", "u8", w, e, "top bit", true);
        data_020a1166[o] = save; }
    {   unsigned char a = 0x81, b = 0x81;
        int w = DecIfAbove0_Byte(&a), e = DecIfAbove0_Byte__nr(&b);
        wrow("DecIfAbove0_Byte(0x81)", "u8", w, e, "top bit", true); }
    {   unsigned short a = 0x8001, b = 0x8001;
        int w = DecIfAbove0_Short(&a), e = DecIfAbove0_Short__nr(&b);
        wrow("DecIfAbove0_Short(0x8001)", "u16", w, e, "top bit", true); }
    {   NrVec3 p0 = {0, 0, 0}, p1 = {-0x40000, 0, 0x10000};
        int w = Vec3_HorzAngle(&p0, &p1), e = Vec3_HorzAngle__nr(&p0, &p1);
        wrow("Vec3_HorzAngle(dx < 0)", "s16", w, e, "negative", true); }
    {   NrVec3 p0 = {0, 0, 0}, p1 = {0x10000, -0x8000, 0};
        int w = Vec3_VertAngle(&p1, &p0), e = Vec3_VertAngle__nr(&p1, &p0);
        wrow("Vec3_VertAngle(dy < 0)", "s16", w, e, "negative", true); }
    {   signed char save = data_02075264[0];
        data_02075264[0] = -3;
        int w = GetLevelPart(0), e = GetLevelPart__nr(0);
        wrow("GetLevelPart (entry = -3)", "s8", w, e, "negative", true);
        data_02075264[0] = save; }
    {   unsigned char area[12 * 2] = {0};
        area[12 + 4] = 0xa5;
        void *save = data_0209f314;
        data_0209f314 = area;
        int w = IsAreaShowing(1), e = IsAreaShowing__nr(1);
        wrow("IsAreaShowing (flag = 0xa5)", "u8", w, e, "top bit", true);
        data_0209f314 = save; }
    {   short tbl[4] = {(short)0x8123, 0, 0, 0};
        int w = func_020456a0(0, 0, 0, 0, tbl), e = func_020456a0__nr(0, 0, 0, 0, tbl);
        wrow("func_020456a0 (a3 == 0, tbl[0] = 0x8123)", "u16", w, e, "top bit", true); }
    {   int w = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(0, 0x1000, 0x1000, 0x1234);
        int e = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s__nr(0, 0x1000, 0x1000, 0x1234);
        wrow("dActor_c::ReflectAngle(0x1000, 0x1000, 0x1234)", "s16", w, e, "negative", true); }
    {   unsigned char save = data_0209cad2[3];
        data_0209cad2[3] = 0x96;
        int w = _ZN8SaveData13GetCoinRecordEj(3), e = SaveData::GetCoinRecord(3);
        wrow("SaveData::GetCoinRecord (record = 0x96)", "u8", w, e, "top bit", true);
        data_0209cad2[3] = save; }
    {   unsigned char s0 = data_020a1166[2], s1 = data_020a1166[0];
        data_020a1166[2] = 0xff; data_020a1166[0] = 0xff;
        int w = func_02020168(), e = func_02020168__nr();
        wrow("func_02020168 (flag & 0x20)", "u8", w, e, "range 0..0x20", false);
        w = func_020201f0(0); e = func_020201f0__nr(0);
        wrow("func_020201f0 (flag & 0x10)", "u8", w, e, "range 0..0x10", false);
        w = func_02020364(0); e = func_02020364__nr(0);
        wrow("func_02020364 (flag & 8)", "u8", w, e, "range 0..8", false);
        w = IsPlayerWarping(0); e = IsPlayerWarping__nr(0);
        wrow("IsPlayerWarping ((flag & 0x70) >> 4)", "u8", w, e, "range 0..7", false);
        data_020a1166[2] = s0; data_020a1166[0] = s1; }
    {   unsigned save = data_02092134;
        data_02092134 = 0xffffffffu;
        int w = GetStarCameraSetting(2), e = GetStarCameraSetting__nr(2);
        wrow("GetStarCameraSetting (nibble 0xf)", "u8", w, e, "range 0..15", false);
        data_02092134 = save; }
    {   int w = func_ov007_020c3be0(0x800, 0x7fff, 0x7fff);
        int e = func_ov007_020c3be0__nr(0x800, 0x7fff, 0x7fff);
        wrow("func_ov007_020c3be0 (RGB555 blend)", "u16", w, e, "range 0..0x7fff", false); }
    {   int w = _ZN3OAM11GetObjWidthEii(0, 12), e = OAM::GetObjWidth(0, 12);
        wrow("OAM::GetObjWidth(0, 12)", "u8", w, e, "range 8..64", false);
        w = _ZN3OAM12GetObjHeightEii(0, 12); e = OAM::GetObjHeight(0, 12);
        wrow("OAM::GetObjHeight(0, 12)", "u8", w, e, "range 8..64", false); }
    {   int w = _ZN8SaveData26CountStarsCollectedInLevelEj(0);
        int e = SaveData::CountStarsCollectedInLevel(0);
        wrow("SaveData::CountStarsCollectedInLevel(0)", "u8", w, e, "range 0..8", false);
        w = CountStarsCollectedInLevelToDisplay(0); e = CountStarsCollectedInLevelToDisplay__nr(0);
        wrow("CountStarsCollectedInLevelToDisplay(0)", "u8", w, e, "range 0..8", false);
        w = NumStars(); e = NumStars__nr();
        wrow("NumStars()", "u8", w, e, "range 0..240", false); }
    fprintf(stderr, "[nrwidth] %d of %d PASS\n", g_wpass, g_wrows);
}

}  // namespace

namespace {

struct Site {
    const char *name;
    void *ra;
    int kind;
    unsigned calls, raw_bad, ret_bad;
    int nsamp;
    int raw[3], ext[3];
};

const int kSites = 8192;
Site *g_site;
int g_nsite;
unsigned g_calls, g_raw_bad, g_ret_bad, g_dropped;

int extend(int raw, int kind)
{
    switch (kind) {
    case 0: return (int)(signed char)raw;
    case 1: return (int)(unsigned char)raw;
    case 2: return (int)(short)raw;
    case 3: return (int)(unsigned short)raw;
    default: return (int)(unsigned char)raw;   /* bool: AL, 0 or 1 */
    }
}

const char *kind_name(int kind)
{
    static const char *n[] = {"s8", "u8", "s16", "u16", "bool"};
    return (kind >= 0 && kind < 5) ? n[kind] : "?";
}

void record(int raw, int ret, int kind, const char *name, void *ra)
{
    if (g_width_pending) {
        g_width_pending = false;
        run_width();
        fflush(stderr);
        exit(0);
    }
    if (!g_site) return;
    int ext = extend(raw, kind);
    unsigned h = ((unsigned)(size_t)ra * 2654435761u) ^ (unsigned)(size_t)name;
    Site *s = 0;
    for (int i = 0; i < kSites; i++) {
        Site *c = &g_site[(h + (unsigned)i) & (kSites - 1)];
        if (c->name == name && c->ra == ra) { s = c; break; }
        if (!c->name) {
            c->name = name; c->ra = ra; c->kind = kind;
            g_nsite++;
            s = c;
            break;
        }
    }
    g_calls++;
    if (raw != ext) g_raw_bad++;
    if (ret != ext) g_ret_bad++;
    if (!s) { g_dropped++; return; }
    s->calls++;
    if (raw != ext) {
        s->raw_bad++;
        if (s->nsamp < 3) {
            s->raw[s->nsamp] = raw;
            s->ext[s->nsamp] = ext;
            s->nsamp++;
        }
    }
    if (ret != ext) s->ret_bad++;
}

void dump(void)
{
    char path[1024];
    const char *dir = getenv("SM64DS_NARROWRET_DIR");
    if (dir && *dir)
        snprintf(path, sizeof path, "%s/narrowret_%d.tsv", dir, _getpid());
    else
        snprintf(path, sizeof path, "narrowret_%d.tsv", _getpid());
    FILE *f = fopen(path, "w");
    if (f) {
        const char *keys[] = {"SM64DS_LEVEL", "SM64DS_SCENE", "SM64DS_WARP_SEQ",
                              "SM64DS_WINDOW_SELFTEST", "SM64DS_SCENE_FRAMES",
                              "SM64DS_SELFTEST_IDLE", "SM64DS_NARROWRET_TAG"};
        fprintf(f, "# narrowret");
        for (unsigned k = 0; k < sizeof keys / sizeof keys[0]; k++) {
            const char *v = getenv(keys[k]);
            if (v) fprintf(f, " %s=%s", keys[k], v);
        }
        fprintf(f, "\n# calls=%u raw_bad=%u ret_bad=%u sites=%d dropped=%u\n",
                g_calls, g_raw_bad, g_ret_bad, g_nsite, g_dropped);
        fprintf(f, "function\tkind\treturn_address\tcalls\traw_bad\tret_bad\tsamples\n");
        for (int i = 0; g_site && i < kSites; i++) {
            const Site *s = &g_site[i];
            if (!s->name) continue;
            fprintf(f, "%s\t%s\t%08x\t%u\t%u\t%u\t", s->name, kind_name(s->kind),
                    (unsigned)(size_t)s->ra, s->calls, s->raw_bad, s->ret_bad);
            for (int j = 0; j < s->nsamp; j++)
                fprintf(f, "%s%08x->%d", j ? " " : "", (unsigned)s->raw[j], s->ext[j]);
            fprintf(f, "\n");
        }
        fclose(f);
    }
    fprintf(stderr, "[narrowret] calls=%u raw_bad=%u ret_bad=%u sites=%d -> %s\n",
            g_calls, g_raw_bad, g_ret_bad, g_nsite, f ? path : "(could not write)");
}

struct Arm {
    Arm()
    {
        const char *wv = getenv("SM64DS_NARROWRET_WIDTH");
        if (wv && *wv && *wv != '0') {
            g_width_pending = true;
            port_narrowret_hook = record;
        }
        const char *v = getenv("SM64DS_NARROWRET");
        if (v && *v && *v != '0') {
            g_site = (Site *)calloc(kSites, sizeof(Site));
            if (!g_site) return;
            port_narrowret_hook = record;
            atexit(dump);
        }
    }
} g_arm;

}  // namespace
