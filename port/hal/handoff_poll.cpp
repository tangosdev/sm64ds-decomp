/* THE LAUNCHER HAND-OFF, GAME SIDE: see hal/handoff_poll.h for the contract (run after093, lane SEAMVS1).
 *
 * THE POLL. One call per game frame through port_sdat_frame_hook (hal/sdat/consumer.cpp: every loop, the boot title's, a crossing
 * title's and the level loop's), chained at static initialisation the way SEAMJOIN1's ip_frame chains at run time. Every 15th frame
 * of a live title it looks for <bundle>/handoff/request.json, and only when the request can be answered in place:
 *   - the title is the live scene (port_scene_live_id() == 1: the boot title or a title reached by a crossing) with no scene change
 *     pending (data_02092664 is the 0x187 sentinel);
 *   - no session is up: no hub, no VS arena, no DS session transport (the lockstep carrier), no coop, no online leave or join under way,
 *     no VS match end still tearing down;
 *   - the Joining plate is not up.
 * In a level, in a session or behind the plate the file is not touched, so the launcher's 5 s wait runs out and it tells the player to
 * go to the title first.
 *
 * THE PICKUP. request.json is renamed to request.taken (a second pickup is impossible), read, and deleted once read. "v" other than
 * 1, or a deadline_ms already past, is answered "expired". A file that does not read as the contract's JSON is refused. A kind with no
 * registered handler is refused. A refusal writes the response, prints "[handoff] refused: <why>; the launcher starts a fresh game"
 * and quits through the window's own WM_QUIT path; an atexit hook registered here, before main, ends that orderly exit with code 8. */
#include "handoff_poll.h"

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "comms_hub.h"
#include "vs_arena.h"

extern "C" {
extern void (*port_sdat_frame_hook)(void);   /* hal/sdat/consumer.cpp: one call per game frame */
int port_scene_live_id(void);                /* hal/scene_boot.cpp: 1 = SCENE_TITLE */
extern unsigned short data_02092664;         /* Scene::SetSceneToSpawn's pending id (0x187 = nothing) */
int port_join_plate_up(void);                /* hal/menu_join_plate.cpp */
int port_join_plate_holds_handoff(void);     /* hal/menu_join_plate.cpp (KNOWNNET1): up, and not just lowering */
int port_online_handoff_busy(const char **why);   /* hal/online_join.cpp: a leave or a join under way */
int port_vs_end_pending(void);               /* hal/star_flow.cpp: a VS match end still on its way to the title */
void port_vs_lobby_title_frame(void);        /* hal/scene_vs_menu.cpp: the VS lobby's mods, at the title */
#ifndef PORT_ROM_CLEAN
void _ZN8dScene_c14StartSceneFadeEjjt(unsigned id, unsigned param, unsigned short color);
extern int port_vs_force_nobody;             /* hal/vs_arena.cpp: the "seat has no body" drill (dev) */
int port_vs_matches_ended_in_place(void);    /* hal/star_flow.cpp */
extern int port_front_end_in_place;          /* hal/level_change.cpp (SEAMQUIT1): a title reached by an in-place quit */
extern char *data_ov007_0210342c;            /* dScDSMT_c's scene global (hal/title_entry.cpp reads it the same way) */
void func_ov007_020aec94(int choice);        /* src/func_ov007_020aec94.c: the title's own "the player chose X" */
void func_ov007_020b6134(int slot);          /* src/func_ov007_020b6134.cpp: the file select's A / B / C handler */
void port_touch_force_press(int x, int y, int frames);   /* hal/sub_screen.cpp */
extern unsigned char data_020a0f10[], data_0209f250[], data_0208a0e0, data_0209fc68[], data_0209f2d8;   /* the rows' state line */
#endif
}

namespace port {
struct CommsTransport;
const CommsTransport *comms_transport();     /* hal/comms_seam.h: the DS session carrier, null when none */
}

namespace {

/* ---- the registry ---------------------------------------------------------------------------------------------------------------- */
struct Reg { char kind[16]; PortHandoffFn fn; };
Reg g_reg[8];

/* ---- small helpers --------------------------------------------------------------------------------------------------------------- */
unsigned long long now_ms()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    const unsigned long long t = ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return t / 10000ull - 11644473600000ull;
}

std::string g_dir;            /* <bundle>/handoff, or the dev knob's folder */
bool g_dir_dev = false;

const std::string &folder()
{
    if (!g_dir.empty()) return g_dir;
#ifndef PORT_ROM_CLEAN
    /* SM64DS_HANDOFF_TEST=<folder>: the hand-off folder is that folder (dev only, compiled out of the kit). The same name carries
       JOINWIN2's die / hang successor drill (tests/walk_window.cpp); a value that names a folder (it holds a slash) is this one. */
    if (const char *t = std::getenv("SM64DS_HANDOFF_TEST"))
        if (std::strchr(t, '/') || std::strchr(t, '\\')) { g_dir = t; g_dir_dev = true; }
#endif
    if (g_dir.empty()) {
        char exe[MAX_PATH] = "";
        GetModuleFileNameA(0, exe, (DWORD)sizeof exe);
        std::string d = exe;
        const size_t cut = d.find_last_of("/\\");
        d = cut == std::string::npos ? std::string(".") : d.substr(0, cut);
        g_dir = d + "/handoff";
    }
    CreateDirectoryA(g_dir.c_str(), 0);   /* the contract: the game creates it if missing */
    return g_dir;
}

/* ---- a small JSON reader, enough for the contract's one flat object -------------------------------------------------------------- */
struct J {
    const char *p, *e;
    const char *err;
    void ws() { while (p < e && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) ++p; }
    bool lit(char c) { ws(); if (p < e && *p == c) { ++p; return true; } return false; }
    bool str(std::string *out)
    {
        ws();
        if (p >= e || *p != '"') { err = "a string was expected"; return false; }
        ++p;
        out->clear();
        while (p < e && *p != '"') {
            unsigned char c = (unsigned char)*p++;
            if (c < 0x20) { err = "a control character in a string"; return false; }
            if (c == '\\') {
                if (p >= e) break;
                const char x = *p++;
                switch (x) {
                case '"': c = '"'; break;
                case '\\': c = '\\'; break;
                case '/': c = '/'; break;
                case 'b': c = '\b'; break;
                case 'f': c = '\f'; break;
                case 'n': c = '\n'; break;
                case 'r': c = '\r'; break;
                case 't': c = '\t'; break;
                case 'u': {
                    if (e - p < 4) { err = "a short \\u escape"; return false; }
                    unsigned v = 0;
                    for (int k = 0; k < 4; ++k) {
                        const char h = *p++;
                        v = v * 16 + (h >= '0' && h <= '9' ? h - '0' : h >= 'a' && h <= 'f' ? h - 'a' + 10
                                      : h >= 'A' && h <= 'F' ? h - 'A' + 10 : 0x10000);
                    }
                    if (v > 0x7f) { err = "a \\u escape past ASCII"; return false; }
                    c = (unsigned char)v;
                    break;
                }
                default: err = "an unknown escape"; return false;
                }
            }
            if (c >= 0x80) { err = "a byte past ASCII in a string"; return false; }
            out->push_back((char)c);
            if (out->size() > 4096) { err = "a string longer than 4096 bytes"; return false; }
        }
        if (p >= e) { err = "an unterminated string"; return false; }
        ++p;
        return true;
    }
    bool num(long long *out)
    {
        ws();
        const char *s = p;
        if (p < e && *p == '-') ++p;
        while (p < e && *p >= '0' && *p <= '9') ++p;
        if (p == s || (p == s + 1 && *s == '-')) { err = "a number was expected"; return false; }
        if (p < e && (*p == '.' || *p == 'e' || *p == 'E')) { err = "a number with a fraction or an exponent"; return false; }
        *out = std::strtoll(std::string(s, p).c_str(), 0, 10);
        return true;
    }
};

struct Req {
    long long v = -1, deadline = -1;
    std::string nonce, kind;
    std::vector<std::string> set_name, set_value, clear;
    bool has_v = false, has_deadline = false, has_set = false, has_clear = false;
};

/* 0 = read; else why not */
const char *parse(const std::string &t, Req *r)
{
    static char buf[160];
    J j{t.data(), t.data() + t.size(), 0};
    if (t.size() >= 3 && (unsigned char)t[0] == 0xEF && (unsigned char)t[1] == 0xBB && (unsigned char)t[2] == 0xBF) j.p += 3;   /* a UTF-8 BOM */
    if (!j.lit('{')) return "the file is not a JSON object";
    if (!j.lit('}')) {
        for (;;) {
            std::string key;
            if (!j.str(&key)) return j.err;
            if (!j.lit(':')) return "a ':' was expected after a key";
            if (key == "v") { if (!j.num(&r->v)) return j.err; r->has_v = true; }
            else if (key == "deadline_ms") { if (!j.num(&r->deadline)) return j.err; r->has_deadline = true; }
            else if (key == "nonce") { if (!j.str(&r->nonce)) return j.err; }
            else if (key == "kind") { if (!j.str(&r->kind)) return j.err; }
            else if (key == "set") {
                if (!j.lit('{')) return "\"set\" is not an object";
                r->has_set = true;
                if (!j.lit('}')) {
                    for (;;) {
                        std::string n, v;
                        if (!j.str(&n)) return j.err;
                        if (!j.lit(':')) return "a ':' was expected in \"set\"";
                        if (!j.str(&v)) return "a \"set\" value is not a string";
                        r->set_name.push_back(n);
                        r->set_value.push_back(v);
                        if (j.lit(',')) continue;
                        if (j.lit('}')) break;
                        return "a ',' or '}' was expected in \"set\"";
                    }
                }
            } else if (key == "clear") {
                if (!j.lit('[')) return "\"clear\" is not an array";
                r->has_clear = true;
                if (!j.lit(']')) {
                    for (;;) {
                        std::string n;
                        if (!j.str(&n)) return "a \"clear\" entry is not a string";
                        r->clear.push_back(n);
                        if (j.lit(',')) continue;
                        if (j.lit(']')) break;
                        return "a ',' or ']' was expected in \"clear\"";
                    }
                }
            } else {
                std::snprintf(buf, sizeof buf, "an unknown key \"%.40s\"", key.c_str());
                return buf;
            }
            if (j.lit(',')) continue;
            if (j.lit('}')) break;
            return "a ',' or '}' was expected";
        }
    }
    j.ws();
    if (j.p != j.e) return "bytes after the object";
    if (!r->has_v) return "no \"v\"";
    if (!r->has_deadline) return "no \"deadline_ms\"";
    if (r->kind.empty()) return "no \"kind\"";
    if (r->nonce.empty() || r->nonce.size() > 64) return "no \"nonce\" (or one longer than 64)";
    for (char c : r->nonce)
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) return "the nonce is not hex";
    if (!r->has_set || !r->has_clear) return "no \"set\" or no \"clear\"";
    for (const std::string &n : r->set_name)
        if (n.compare(0, 7, "SM64DS_") || n.size() > 64 || n.find('=') != std::string::npos) return "a \"set\" name that is not SM64DS_<NAME>";
    for (const std::string &n : r->clear)
        if (n.compare(0, 7, "SM64DS_") || n.size() > 64 || n.find('=') != std::string::npos) return "a \"clear\" name that is not SM64DS_<NAME>";
    return 0;
}

/* ---- the answer ------------------------------------------------------------------------------------------------------------------ */
std::string esc(const std::string &s)
{
    std::string o;
    for (char c : s) {
        if (c == '"' || c == '\\') { o += '\\'; o += c; }
        else if ((unsigned char)c < 0x20 || (unsigned char)c > 0x7e) o += ' ';
        else o += c;
    }
    return o;
}

bool respond(const std::string &nonce, const char *result, const char *why)
{
    const std::string fin = folder() + "/response.json", tmp = fin + ".tmp";
    std::string body = "{\"v\":1,\"nonce\":\"" + esc(nonce) + "\",\"result\":\"" + result + "\"";
    if (why) body += ",\"why\":\"" + esc(why) + "\"";
    body += "}\n";
    FILE *f = std::fopen(tmp.c_str(), "wb");
    if (!f) return false;
    std::fwrite(body.data(), 1, body.size(), f);
    std::fclose(f);
    return MoveFileExA(tmp.c_str(), fin.c_str(), MOVEFILE_REPLACE_EXISTING) != 0;
}

/* ---- exit code 8 ----------------------------------------------------------------------------------------------------------------- */
volatile int g_exit8 = 0;

/* Registered before main (static initialisation), so it runs after every atexit hook main and the subsystems register: the orderly
   exit has written its logs and its census by then, and this only changes the code the process ends with. */
void exit8_at_exit()
{
    if (!g_exit8) return;
    std::fprintf(stderr, "[handoff] this process ends with exit code 8 (handoff refused: a fresh process takes the match)\n");
    std::fflush(stderr);
    std::fflush(stdout);
    ExitProcess(8);
}

void post_quit(int code)
{
    typedef void(WINAPI * Pqm)(int);
    HMODULE u = GetModuleHandleA("user32.dll");
    Pqm pqm = u ? (Pqm)GetProcAddress(u, "PostQuitMessage") : 0;
    if (pqm) pqm(code);
    else ExitProcess((UINT)code);
}

void quit8()
{
    g_exit8 = 1;
    post_quit(8);
}

void refuse(const std::string &nonce, const char *why)
{
    const bool ok = respond(nonce, "refused", why);
    std::fprintf(stderr, "[handoff] response \"refused\" written%s (nonce %s)\n", ok ? "" : " -- FAILED, the folder cannot be written",
                 nonce.empty() ? "-" : nonce.c_str());
    std::fprintf(stderr, "[handoff] refused: %s; the launcher starts a fresh game\n", why);
    std::fflush(stderr);
    quit8();
}

/* ---- the poll -------------------------------------------------------------------------------------------------------------------- */
void (*g_prev)(void) = 0;
unsigned g_frames = 0;        /* frames of a live title, counted over the whole process */
bool g_done = false;          /* a refusal is quitting this process: no second pickup */
int g_accepted = 0;           /* hand-offs this process has taken */

/* 0 = a request may be picked up now; else why not */
const char *busy()
{
    if (port_scene_live_id() != 1) return "the title is not the live scene";
    if (data_02092664 != 0x187) return "a scene change is pending";
    if (port_join_plate_holds_handoff()) return "the Joining plate is up";
    if (port::comms_hub_active() || port::comms_hub_state() != port::kHubOff) return "a hub is installed";
    if (port::vs_arena_active()) return "a VS arena is installed";
    if (port::comms_transport()) return "a DS session transport is installed";
    if (const char *c = std::getenv("SM64DS_COOP"))
        if (*c && std::strcmp(c, "0")) return "a coop session";
    const char *w = 0;
    if (port_online_handoff_busy(&w)) return w ? w : "an online join or leave is under way";
    if (port_vs_end_pending()) return "a VS match end is still on its way to the title";
    return 0;
}

void take()
{
    const std::string req = folder() + "/request.json", taken = folder() + "/request.taken";
    if (GetFileAttributesA(req.c_str()) == INVALID_FILE_ATTRIBUTES) return;
    if (!MoveFileExA(req.c_str(), taken.c_str(), MOVEFILE_REPLACE_EXISTING)) return;   /* the launcher is still renaming: next poll */
    std::string t;
    {
        FILE *f = std::fopen(taken.c_str(), "rb");
        if (f) {
            char buf[8192];
            size_t n;
            while ((n = std::fread(buf, 1, sizeof buf, f)) > 0 && t.size() < 65536) t.append(buf, n);
            std::fclose(f);
        }
    }
    DeleteFileA(taken.c_str());
    const unsigned long long now = now_ms();
    std::fprintf(stderr, "[handoff] request picked up at title frame %u (%u byte(s); renamed to request.taken, read, deleted; folder %s%s)\n",
                 g_frames, (unsigned)t.size(), folder().c_str(), g_dir_dev ? ", the dev folder" : "");
    std::fflush(stderr);
    Req r;
    if (const char *why = parse(t, &r)) {
        char buf[300];
        std::snprintf(buf, sizeof buf, "the request could not be read (%s)", why);
        g_done = true;
        refuse(r.nonce, buf);
        return;
    }
    if (r.v != 1 || r.deadline < (long long)now) {
        respond(r.nonce, "expired", 0);
        std::fprintf(stderr, "[handoff] expired: kind %s nonce %s (v %lld, deadline %lld, now %llu, %lld ms late); answered \"expired\", "
                     "the request is deleted, nothing changes\n", r.kind.c_str(), r.nonce.c_str(), r.v, r.deadline, now,
                     (long long)now - r.deadline);
        std::fflush(stderr);
        return;
    }
    PortHandoffFn fn = 0;
    for (const Reg &g : g_reg)
        if (g.fn && r.kind == g.kind) fn = g.fn;
    std::fprintf(stderr, "[handoff] kind %s nonce %s: %d name(s) to set, %d to clear, %lld ms before the deadline; handler %s\n",
                 r.kind.c_str(), r.nonce.c_str(), (int)r.set_name.size(), (int)r.clear.size(), r.deadline - (long long)now,
                 fn ? "registered" : "NONE");
    std::fflush(stderr);
    if (!fn) {
        char buf[200];
        std::snprintf(buf, sizeof buf, "no handler for kind \"%.32s\" in this build", r.kind.c_str());
        g_done = true;
        refuse(r.nonce, buf);
        return;
    }
    std::vector<const char *> sn, sv, cn;
    for (size_t i = 0; i < r.set_name.size(); ++i) { sn.push_back(r.set_name[i].c_str()); sv.push_back(r.set_value[i].c_str()); }
    for (const std::string &c : r.clear) cn.push_back(c.c_str());
    PortHandoffRequest q;
    q.kind = r.kind.c_str();
    q.nonce = r.nonce.c_str();
    q.nset = (int)sn.size();
    q.set_name = sn.empty() ? 0 : sn.data();
    q.set_value = sv.empty() ? 0 : sv.data();
    q.nclear = (int)cn.size();
    q.clear_name = cn.empty() ? 0 : cn.data();
    if (const char *why = fn(&q)) {
        g_done = true;
        refuse(r.nonce, why);
        return;
    }
    ++g_accepted;
    respond(r.nonce, "accepted", 0);
    std::fprintf(stderr, "[handoff] accepted: kind %s nonce %s; the running game takes the match (response written)\n", r.kind.c_str(),
                 r.nonce.c_str());
    std::fflush(stderr);
}

#ifndef PORT_ROM_CLEAN
/* SM64DS_VS_LOBBY_AT=<title frame>: on that frame of a live title the title asks for the VS lobby (scene 6) with the file select's
   own call, dScene_c::StartSceneFade(6, 0, 0x7fff) (src/game/stages/d_s_dsmt.cpp): the R6 rows' stand-in for the player's press on
   the VS button. SM64DS_VS_FORCE_NOBODY=1: the next in-place arena match takes a seat past its bodies (hal/vs_arena.cpp, the "seat has
   no body" row). Dev only, compiled out of the kit. */
int g_lobby_at = -2;
void dev_frame(unsigned title_frame)
{
    if (g_lobby_at == -2) {
        const char *e = std::getenv("SM64DS_VS_LOBBY_AT");
        g_lobby_at = e && *e ? std::atoi(e) : -1;
        const char *n = std::getenv("SM64DS_VS_FORCE_NOBODY");
        if (n && *n == '1') port_vs_force_nobody = 1;
    }
    /* SM64DS_TITLE_PICK_FILE=<0..2>: at a title reached by a VS match that ended in place, walk it the way hal/title_entry.cpp's
       port_title_skip_tick walks the boot title for SM64DS_BOOT_FILE (the touch on the title screen, the ROM's own menu choice 9, then
       the file's own button handler), so the R7 rows can pick a file after the match with no pad timing. The same after an accepted
       "adventure" hand-off that leaves the game at the title (the player's own file pick in a launcher-started online game), and at a
       title reached by an in-place quit (the reference rows: an offline quit's title, then the same file). */
    static int af = -2, af_step = 0;
    if (af == -2) {
        const char *e = std::getenv("SM64DS_TITLE_PICK_FILE");
        af = e && e[0] >= '0' && e[0] <= '2' && !e[1] ? e[0] - '0' : -1;
    }
    if (af >= 0 && af_step < 3 && (port_vs_matches_ended_in_place() > 0 || g_accepted || port_front_end_in_place) && data_ov007_0210342c) {
        char *g = data_ov007_0210342c;
        char *sp = *(char **)(g + 8);
        char *ep = *(char **)(g + 4);
        if (sp) {
            const int state = *(short *)sp, req = *(short *)(sp + 2), sf = *(int *)(sp + 0xc);
            if (af_step == 0 && state == 11 && sf >= 8) {
                af_step = 1;
                port_touch_force_press(128, 96, 2);
                std::fprintf(stderr, "[seamless] title frame %u SM64DS_TITLE_PICK_FILE: the title screen is touched\n", title_frame);
            } else if (af_step <= 1 && state == 0 && req == -1 && !*(int *)(g + 0x10) && *(char **)(g + 0x28) && sf >= 8 &&
                       !(ep && *(short *)ep == 5)) {
                af_step = 2;
                func_ov007_020aec94(9);
                std::fprintf(stderr, "[seamless] title frame %u SM64DS_TITLE_PICK_FILE: the menu's file select row chosen "
                             "(func_ov007_020aec94(9))\n", title_frame);
            } else if (state == 0 && req == -1 && ep && *(short *)ep == 5 && *(short *)(ep + 2) == -1 && sf >= 8) {
                af_step = 3;
                std::fprintf(stderr, "[seamless] title frame %u SM64DS_TITLE_PICK_FILE: before the press: data_020a0f10 %u data_0209f250 %u "
                             "data_0208a0e0 %u data_0209fc68 %u data_0209f2d8 %u\n", title_frame, (unsigned)data_020a0f10[0],
                             (unsigned)data_0209f250[0], (unsigned)data_0208a0e0, (unsigned)data_0209fc68[0], (unsigned)data_0209f2d8);
                func_ov007_020b6134(af);
                std::fprintf(stderr, "[seamless] title frame %u SM64DS_TITLE_PICK_FILE: file %c pressed through func_ov007_020b6134(%d)\n",
                             title_frame, 'A' + af, af);
            }
            std::fflush(stderr);
        }
    }
    if (g_lobby_at >= 0 && (int)title_frame == g_lobby_at && data_02092664 == 0x187) {
        std::fprintf(stderr, "[seamless] title frame %u SM64DS_VS_LOBBY_AT: the title asks for the VS lobby, StartSceneFade(6, 0, 0x7fff) "
                     "as the file select's VS button does\n", title_frame);
        std::fflush(stderr);
        _ZN8dScene_c14StartSceneFadeEjjt(6, 0, 0x7fff);
    }
}
#endif

void frame()
{
    if (g_prev) g_prev();
    if (port_scene_live_id() != 1) return;
    const unsigned f = g_frames++;
    port_vs_lobby_title_frame();
#ifndef PORT_ROM_CLEAN
    dev_frame(f);
#endif
    if (g_done || f % 15 != 14) return;
    if (busy()) return;
    take();
}

struct Seat {
    Seat()
    {
        g_prev = port_sdat_frame_hook;
        port_sdat_frame_hook = frame;
        std::atexit(exit8_at_exit);
    }
} g_seat;

}  // namespace

extern "C" int port_handoff_register(const char *kind, PortHandoffFn fn)
{
    if (!kind || !*kind || !fn || std::strlen(kind) >= sizeof g_reg[0].kind) return 0;
    for (Reg &g : g_reg)
        if (g.fn && !std::strcmp(g.kind, kind)) { g.fn = fn; return 1; }
    for (Reg &g : g_reg)
        if (!g.fn) {
            std::strcpy(g.kind, kind);
            g.fn = fn;
            return 1;
        }
    return 0;
}

extern "C" const char *port_handoff_value(const PortHandoffRequest *req, const char *name)
{
    if (!req || !name) return 0;
    const char *v = 0;
    for (int i = 0; i < req->nset; ++i)
        if (!std::strcmp(req->set_name[i], name)) v = req->set_value[i];   /* the last one wins, as a dictionary would */
    return v;
}

extern "C" int port_handoff_apply_env(const PortHandoffRequest *req)
{
    if (!req) return 0;
    for (int i = 0; i < req->nclear; ++i) {
        SetEnvironmentVariableA(req->clear_name[i], 0);
        _putenv_s(req->clear_name[i], "");
    }
    for (int i = 0; i < req->nset; ++i) {
        SetEnvironmentVariableA(req->set_name[i], req->set_value[i]);
        _putenv_s(req->set_name[i], req->set_value[i]);
    }
    std::fprintf(stderr, "[handoff] environment as a fresh process gets it: %d name(s) cleared and %d set, in both copies\n", req->nclear,
                 req->nset);
    std::fflush(stderr);
    return req->nclear + req->nset;
}

/* The window's own way out (WM_QUIT; the loops flush and main returns), for hal/ files that cannot reach tests/walk_window.cpp's W. */
extern "C" void port_window_quit(int code) { post_quit(code); }

extern "C" void port_handoff_unset_env(const PortHandoffRequest *req)
{
    if (!req) return;
    for (int i = 0; i < req->nset; ++i) {
        SetEnvironmentVariableA(req->set_name[i], 0);
        _putenv_s(req->set_name[i], "");
    }
}
