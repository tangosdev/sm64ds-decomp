/* LOBBY CLIENT (run after093, lane LOBBYNET1): the game talks to the lobby server itself. Contract: hal/lobby_client.h.
 *
 * THE WIRE IS THE LAUNCHER'S. Every request is the JSON POST the launcher's LobbyClient makes (https://tangos.dev/port/lobby/<verb>,
 * contract v6, port/tools/lobby/app/server.py), with the same game build id (version.json's GameVersion + "-" + the first eight hex
 * digits of GameGitTip, exactly LobbyWire.GameBuildId), so a room the game makes and a room the launcher makes are the same thing to the
 * server and to each other. No server change.
 *
 * NOTHING BLOCKS THE FRAME. Two worker threads do the network: the ACTION thread runs the player's verbs one at a time off a queue,
 * the POLL thread holds the room's long poll (wait 10 s) and folds every answer into the shared record. The frame thread only copies
 * that record into the snapshot lobby_state() hands out (lobby_tick) and, at the title, starts a match the poll has delivered.
 * WinHTTP is loaded by name (LoadLibraryA), never imported: port/tools/sdl_import_guard.py allows KERNEL32 alone.
 *
 * READY is the server's pre_ok (the `preflight` verb): it is per member, everyone sees it, and the server's VS start already refuses
 * while a playing member is not ready (member_not_ready). A coop start needs only the host on the server; the in-game rule (the
 * host's box becomes START once everyone is ready) is checked here before the verb is sent.
 *
 * STARTING. The server's plan is turned into the SAME environment the launcher builds (LobbyLaunch.BuildAdventureEnv for coop,
 * BuildChildEnv for VS: the same names set, the same AlwaysClear list and inherited prefixes cleared) and handed to the hand-off
 * handler of its kind in this process (hal/handoff_poll.cpp port_handoff_dispatch: coop -> hal/coop_boot.cpp, vs -> the arena in place).
 * When the handler refuses (a lockstep VS match, anything the in-place path cannot take) the environment is applied to this process and
 * the game starts itself again with it (CreateProcess of its own exe, the way the launcher starts a fresh game with those names) and
 * this process quits. The successor carries the room on in SM64DS_LOBBY_RESUME (url, room, token, cursor, launch number, match) so it
 * keeps the seat alive and reports the result; the name is cleared from its environment as soon as it is read.
 *
 * THE MATCH RESULT. The game still writes the "[vs] MATCH OVER f<n> win=<..> scores=<..>" line the launcher reads from its log. With no
 * launcher running, this file reads that same line back out of the process's own stderr file (the playlog) and posts it to the lobby
 * as the launcher does (ParseMatchOver / WireWin), so the room goes back to lobby either way.
 *
 * COOP SAVE. The host's .coopsav lives where the launcher keeps them (<save folder>/coop/<stem>.coopsav, CoopSaves.Folder): New makes a
 * fresh name, Continue takes the newest file there. The path goes into SM64DS_COOP_SAVE for the host only, as the launcher does. */
#include "lobby_client.h"

#include <windows.h>
#include <winhttp.h>
#include <io.h>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>

#include "handoff_poll.h"
#include "host_settings.h"
#include "ntr/backup.h"

extern "C" {
extern void (*port_sdat_frame_hook)(void);   /* hal/sdat/consumer.cpp: one call per game frame */
int port_scene_live_id(void);                /* hal/scene_boot.cpp: 1 = SCENE_TITLE */
void port_window_quit(int code);             /* hal/handoff_poll.cpp */
}

namespace {

/* ================================================================================================================================ */
/* A small JSON reader: a value tree, enough for the lobby's answers                                                                */
/* ================================================================================================================================ */
struct JV {
    enum T { NUL, BOOL, NUM, STR, ARR, OBJ } t = NUL;
    bool b = false;
    double n = 0;
    std::string s;
    std::vector<JV> a;
    std::vector<std::pair<std::string, JV>> o;
    const JV *get(const char *k) const
    {
        if (t != OBJ) return 0;
        for (const auto &kv : o)
            if (kv.first == k) return &kv.second;
        return 0;
    }
    long long i(const char *k, long long def = 0) const
    {
        const JV *v = get(k);
        if (!v) return def;
        if (v->t == NUM) return (long long)v->n;
        if (v->t == BOOL) return v->b ? 1 : 0;
        return def;
    }
    bool has_num(const char *k) const { const JV *v = get(k); return v && v->t == NUM; }
    std::string str(const char *k) const { const JV *v = get(k); return v && v->t == STR ? v->s : std::string(); }
    bool flag(const char *k) const { const JV *v = get(k); return v && ((v->t == BOOL && v->b) || (v->t == NUM && v->n != 0)); }
};

struct JP {
    const char *p, *e;
    int depth = 0;
    void ws() { while (p < e && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) ++p; }
    bool val(JV *v)
    {
        ws();
        if (p >= e || ++depth > 32) return false;
        bool ok = false;
        if (*p == '{') {
            ++p; v->t = JV::OBJ; ws();
            if (p < e && *p == '}') { ++p; ok = true; }
            else for (;;) {
                std::string k; JV x;
                ws();
                if (!strv(&k)) break;
                ws();
                if (p >= e || *p != ':') break;
                ++p;
                if (!val(&x)) break;
                v->o.emplace_back(k, x);
                ws();
                if (p < e && *p == ',') { ++p; continue; }
                if (p < e && *p == '}') { ++p; ok = true; }
                break;
            }
        } else if (*p == '[') {
            ++p; v->t = JV::ARR; ws();
            if (p < e && *p == ']') { ++p; ok = true; }
            else for (;;) {
                JV x;
                if (!val(&x)) break;
                v->a.push_back(x);
                ws();
                if (p < e && *p == ',') { ++p; continue; }
                if (p < e && *p == ']') { ++p; ok = true; }
                break;
            }
        } else if (*p == '"') {
            v->t = JV::STR; ok = strv(&v->s);
        } else if (e - p >= 4 && !std::strncmp(p, "true", 4)) { v->t = JV::BOOL; v->b = true; p += 4; ok = true; }
        else if (e - p >= 5 && !std::strncmp(p, "false", 5)) { v->t = JV::BOOL; p += 5; ok = true; }
        else if (e - p >= 4 && !std::strncmp(p, "null", 4)) { v->t = JV::NUL; p += 4; ok = true; }
        else {
            const char *s = p;
            while (p < e && (std::strchr("+-.eE", *p) || (*p >= '0' && *p <= '9'))) ++p;
            if (p > s) { v->t = JV::NUM; v->n = std::strtod(std::string(s, p).c_str(), 0); ok = true; }
        }
        --depth;
        return ok;
    }
    bool strv(std::string *o)
    {
        if (p >= e || *p != '"') return false;
        ++p;
        while (p < e && *p != '"') {
            char c = *p++;
            if (c == '\\' && p < e) {
                const char x = *p++;
                if (x == 'u') {
                    unsigned u = 0;
                    for (int k = 0; k < 4 && p < e; ++k, ++p)
                        u = u * 16 + (unsigned)(*p >= '0' && *p <= '9' ? *p - '0' : (*p | 0x20) - 'a' + 10);
                    c = u < 0x80 ? (char)u : '?';
                } else c = x == 'n' ? '\n' : x == 't' ? '\t' : x == 'r' ? '\r' : x == 'b' ? '\b' : x == 'f' ? '\f' : x;
            }
            o->push_back(c);
        }
        if (p >= e) return false;
        ++p;
        return true;
    }
};

bool jparse(const std::string &t, JV *v)
{
    JP j{t.data(), t.data() + t.size()};
    return j.val(v);
}

std::string jesc(const std::string &s)
{
    std::string o = "\"";
    for (char c : s) {
        if (c == '"' || c == '\\') { o += '\\'; o += c; }
        else if ((unsigned char)c < 0x20 || (unsigned char)c > 0x7e) continue;
        else o += c;
    }
    return o + "\"";
}

/* printable ASCII only, capped: everything from the server is untrusted text (the launcher's LobbyWire.Safe) */
std::string safe(const std::string &s, size_t cap)
{
    std::string o;
    for (char c : s) {
        if ((unsigned char)c < 0x20 || (unsigned char)c > 0x7e) continue;
        o += c;
        if (o.size() >= cap) break;
    }
    return o;
}

/* ================================================================================================================================ */
/* WinHTTP, loaded by name                                                                                                          */
/* ================================================================================================================================ */
struct Http {
    decltype(&WinHttpOpen) Open = 0;
    decltype(&WinHttpCrackUrl) Crack = 0;
    decltype(&WinHttpConnect) Connect = 0;
    decltype(&WinHttpOpenRequest) OpenRequest = 0;
    decltype(&WinHttpSetTimeouts) SetTimeouts = 0;
    decltype(&WinHttpSendRequest) Send = 0;
    decltype(&WinHttpReceiveResponse) Receive = 0;
    decltype(&WinHttpQueryHeaders) Query = 0;
    decltype(&WinHttpReadData) Read = 0;
    decltype(&WinHttpCloseHandle) Close = 0;
    HINTERNET session = 0;
    bool tried = false, ok = false;
    bool load()
    {
        if (tried) return ok;
        tried = true;
        HMODULE m = LoadLibraryA("winhttp.dll");
        if (!m) return false;
#define LC_GET(f, n) f = (decltype(f))GetProcAddress(m, n)
        LC_GET(Open, "WinHttpOpen"); LC_GET(Crack, "WinHttpCrackUrl"); LC_GET(Connect, "WinHttpConnect");
        LC_GET(OpenRequest, "WinHttpOpenRequest"); LC_GET(SetTimeouts, "WinHttpSetTimeouts"); LC_GET(Send, "WinHttpSendRequest");
        LC_GET(Receive, "WinHttpReceiveResponse"); LC_GET(Query, "WinHttpQueryHeaders"); LC_GET(Read, "WinHttpReadData");
        LC_GET(Close, "WinHttpCloseHandle");
#undef LC_GET
        if (!(Open && Crack && Connect && OpenRequest && SetTimeouts && Send && Receive && Query && Read && Close)) return false;
        session = Open(L"sm64ds-port-lobby/1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        ok = session != 0;
        return ok;
    }
};
Http g_http;
CRITICAL_SECTION g_http_cs;   /* load() once, from either worker */

struct Answer {
    int status = 0;             /* 0 = no answer (network) */
    bool ok = false;
    std::string error;          /* the server's error token */
    std::string message;        /* the sentence */
    JV body;
};

/* The launcher's LobbyWire.Explain, word for word for the refusals a player can meet here. */
const char *explain(int status, const std::string &code)
{
    static const struct { const char *k, *w; } t[] = {
        {"no_such_room", "That room does not exist any more."},
        {"room_full", "That room is full."},
        {"in_match", "That room is in the middle of a match."},
        {"kicked", "The host removed you from that room."},
        {"banned", "You are banned from this room."},
        {"too_many_bans", "This room cannot ban anyone else."},
        {"not_host", "Only the host can do that."},
        {"not_a_member", "You are not in that room any more."},
        {"not_in_lobby", "That cannot be changed while a match is running."},
        {"not_enough_players", "You need at least two players to start."},
        {"not_playing", "You are watching this match, not playing in it."},
        {"member_not_ready", "Someone is still getting ready."},
        {"stale_match", "That match already ended."},
        {"too_fast", "Slow down a moment and try again."},
        {"ignored", "Too many requests just now. Wait a few seconds."},
        {"full", "The lobby server is full right now. Try again shortly."},
        {"busy", "The lobby server is busy right now. Try again shortly."},
        {"bad_room", "That is not a valid room code."},
        {"bad_nick", "That name has a character the lobby will not take."},
        {"too_long", "That was too long."},
        {"bad_text", "That message has a character the lobby will not take."},
        {"bad_seat", "That player is not in the room any more."},
        {"game_mismatch", "This room runs a different game version. Everyone needs the same update."},
        {"bad_save_name", "That save name has a character the lobby will not take."},
        {"already_started", "The room has started, so its settings are locked."},
        {"unknown_game", "This game version is not one the lobby server knows. Check for an update."},
        {"bad_match_players", "That many players is more than this server's game can run yet."},
        {"bad_version", "This game and the lobby server do not agree on the rules. Check for an update."},
    };
    for (const auto &r : t)
        if (code == r.k) return r.w;
    if (!code.empty() && (code.compare(0, 4, "bad_") == 0 || code == "no_room_settings" || code == "not_vs"))
        return "This game and the lobby server do not agree on the rules. Check for an update.";
    if (status == 0) return "The lobby server could not be reached.";
    if (status >= 500) return "The lobby server had a problem. Try again shortly.";
    return "The lobby server refused that.";
}

std::wstring widen(const std::string &s) { return std::wstring(s.begin(), s.end()); }

/* One POST, blocking, on a worker thread only. timeout_s covers connect, send and the whole answer. */
Answer post(const std::string &base, const std::string &verb, const std::string &body, int timeout_s)
{
    Answer a;
    EnterCriticalSection(&g_http_cs);
    const bool up = g_http.load();
    LeaveCriticalSection(&g_http_cs);
    if (!up) { a.message = "This game could not load its network library (winhttp.dll)."; return a; }
    const std::wstring url = widen(base + "/" + verb);
    URL_COMPONENTS uc;
    std::memset(&uc, 0, sizeof uc);
    uc.dwStructSize = sizeof uc;
    wchar_t host[256], path[1024];
    uc.lpszHostName = host; uc.dwHostNameLength = 256;
    uc.lpszUrlPath = path; uc.dwUrlPathLength = 1024;
    if (!g_http.Crack(url.c_str(), 0, 0, &uc)) { a.message = "The lobby address is not a web address."; return a; }
    HINTERNET c = g_http.Connect(g_http.session, host, uc.nPort, 0);
    if (!c) { a.message = explain(0, ""); return a; }
    const bool tls = uc.nScheme == INTERNET_SCHEME_HTTPS;
    HINTERNET r = g_http.OpenRequest(c, verb == "health" ? L"GET" : L"POST", path, 0, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                     tls ? WINHTTP_FLAG_SECURE : 0);
    if (!r) { g_http.Close(c); a.message = explain(0, ""); return a; }
    const int ms = timeout_s * 1000;
    g_http.SetTimeouts(r, ms, ms, ms, ms);
    const wchar_t *hdr = L"Content-Type: application/json\r\n";
    bool sent = g_http.Send(r, hdr, (DWORD)-1L, (LPVOID)body.data(), (DWORD)body.size(), (DWORD)body.size(), 0) &&
                g_http.Receive(r, 0);
    if (!sent) {
        const DWORD e = GetLastError();
        a.message = e == ERROR_WINHTTP_TIMEOUT ? "The lobby server did not answer in time." : "The lobby server could not be reached.";
        g_http.Close(r); g_http.Close(c);
        return a;
    }
    DWORD st = 0, sz = sizeof st;
    g_http.Query(r, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &st, &sz, WINHTTP_NO_HEADER_INDEX);
    std::string text;
    char buf[8192];
    DWORD got = 0;
    bool too_big = false;
    while (g_http.Read(r, buf, sizeof buf, &got) && got) {
        text.append(buf, got);
        if (text.size() > 256 * 1024) { too_big = true; break; }
    }
    g_http.Close(r); g_http.Close(c);
    a.status = (int)st;
    if (too_big) { a.message = "The lobby server sent back more than it should have."; return a; }
    if (!jparse(text, &a.body) || a.body.t != JV::OBJ) {
        a.message = "The lobby server sent something this game did not understand.";
        return a;
    }
    if (st < 200 || st >= 300) {
        a.error = safe(a.body.str("error"), 40);
        a.message = explain((int)st, a.error);
        return a;
    }
    a.ok = true;
    return a;
}

/* ================================================================================================================================ */
/* The shared record                                                                                                                */
/* ================================================================================================================================ */
const char *const kOffOn[] = {"Off", "On"};
const char *const kVsModes[] = {"Clock", "Stars", "Luigi Infection"};
const char *const kArenas[] = {"Arena 1", "Arena 2", "Arena 3", "Arena 4"};
const char *const kNetmodes[] = {"Arena", "Classic"};

/* the coop room rules: RoomSettings.All, the coop column, in the launcher's order */
struct CoopRule { const char *key, *label, *env; int def; };
const CoopRule kCoopRules[] = {
    {"collision", "Bump into players", "SM64DS_ROOM_COLLISION", 1},
    {"interact", "Player hits", "SM64DS_ROOM_INTERACT", 1},
    {"damage", "Hits hurt", "SM64DS_ROOM_DAMAGE", 0},
    {"tp", "Teleport to friends", "SM64DS_COOP_TP", 1},
    {"soloexit", "Only star getter leaves", "SM64DS_ROOM_SOLOEXIT", 1},
};
const int kNCoop = (int)(sizeof kCoopRules / sizeof *kCoopRules);

/* the VS settings, in the order the page shows them */
enum { VS_MODE, VS_STARS, VS_ARENA, VS_PLAYERS, VS_NETMODE, VS_N };

const char *const kAlwaysClear[] = {
    "SM64DS_SCENE", "SM64DS_LEVEL", "SM64DS_BOOT_CLASSIC", "SM64DS_TITLE_ENTRY",
    "SM64DS_ADVENTURE", "SM64DS_PARTY", "SM64DS_NETMODE",
    "SM64DS_WORLD_NO", "SM64DS_ROOM_KIND", "SM64DS_COOP", "SM64DS_COOP_HOST", "SM64DS_COOP_SAVE",
    "SM64DS_COOP_SAVE_NAME", "SM64DS_SKIP_MENU", "SM64DS_BOOT_FILE",
    "SM64DS_ROOM_COLLISION", "SM64DS_ROOM_INTERACT", "SM64DS_ROOM_DAMAGE",
    "SM64DS_COOP_TP", "SM64DS_PARTY_SHARE_STARS", "SM64DS_ROOM_SOLOEXIT",
    "SM64DS_ROOM_TP", "SM64DS_ROOM_STARSHARE",
    "SM64DS_PLAYER_NAME", "SM64DS_PLAYER_COLOR", "SM64DS_PLAYER_SHOES",
};
const char *const kClearPrefixes[] = {"SM64DS_COMMS_", "SM64DS_VS_", "SM64DS_HUB_", "SM64DS_VOICE_", "SM64DS_SESSION_"};

struct Member { int seat; std::string name; std::string color, shoes; bool pre_ok, playing; };

struct Shared {
    /* identity of the session */
    std::string base, game, nick, color, shoes;
    LobbyKind kind = LOBBY_COOP;
    std::string room, token;
    int cursor = 0;
    int gen = 0;                      /* bumped on every create / join / leave: a stale worker answer is dropped */
    bool srv_ban = true;              /* the server took a v7 create / join (it has `ban`); false after the v6 fallback */
    LobbyPhase phase = LOBBY_NONE;
    std::string error;
    std::string error_head;     /* the failure screen's heading when it is not "could not get in" */
    /* the view */
    std::string state = "lobby", session_type;
    int host = 0, you = 0, max_seats = 4, dial_max = 0, arena_max = 4;
    std::vector<Member> members;
    int map = 0, match_players = 2, star_target = 0, luigi = 0;
    std::string win_mode = "time", netmode;
    bool arena_ok = false;
    bool started = false;
    int launch_no = 0;
    std::string save_name;
    int coop[8] = {1, 1, 0, 1, 1};
    /* chat, newest last */
    std::vector<std::string> chat;
    std::map<int, int> muted;         /* seat -> 1 (local only) */
    std::string server_name;
    int coop_continue = 0;
    /* the start */
    std::string match;                /* the VS match id the server armed */
    std::string readied;              /* the match this console said `ready` for */
    JV plan;                          /* the newest plan the server handed this member */
    bool plan_set = false;
    int launched_no = 0;              /* coop: the launch number this console started */
    std::string launched_match;       /* VS: the match this console started */
    bool start_wanted = false;        /* the host pressed START: keep STARTING until the launch */
};
Shared g;
CRITICAL_SECTION g_cs;
bool g_init = false;

struct Lock {
    Lock() { EnterCriticalSection(&g_cs); }
    ~Lock() { LeaveCriticalSection(&g_cs); }
};

void init_once()
{
    if (g_init) return;
    g_init = true;
    InitializeCriticalSection(&g_cs);
    InitializeCriticalSection(&g_http_cs);
}

void say(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    std::fprintf(stderr, "[lobby] ");
    std::vfprintf(stderr, fmt, ap);
    std::fprintf(stderr, "\n");
    std::fflush(stderr);
    va_end(ap);
}

void push_chat(const std::string &line)
{
    g.chat.push_back(safe(line, 95));
    while (g.chat.size() > 32) g.chat.erase(g.chat.begin());
}

/* ---- where things live ---------------------------------------------------------------------------------------------------------- */
std::string exe_dir()
{
    char exe[MAX_PATH] = "";
    GetModuleFileNameA(0, exe, (DWORD)sizeof exe);
    std::string d = exe;
    const size_t cut = d.find_last_of("/\\");
    return cut == std::string::npos ? std::string(".") : d.substr(0, cut);
}

bool read_file(const std::string &p, std::string *out)
{
    FILE *f = std::fopen(p.c_str(), "rb");
    if (!f) return false;
    char buf[4096];
    size_t n;
    while ((n = std::fread(buf, 1, sizeof buf, f)) > 0 && out->size() < 65536) out->append(buf, n);
    std::fclose(f);
    return true;
}

/* LobbyWire.GameBuildId over the bundle's version.json, the file the launcher reads */
std::string game_build_id()
{
    std::string t, ver, tip;
    JV v;
    if (read_file(exe_dir() + "/version.json", &t) && jparse(t, &v)) { ver = v.str("GameVersion"); tip = v.str("GameGitTip"); }
    auto chars = [](const std::string &s) {
        std::string o;
        for (char c : s)
            if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '.' || c == '_' || c == '-') o += c;
        return o;
    };
    ver = chars(ver);
    tip = chars(tip);
    std::string id = ver.empty() || ver == "unknown" ? "unknown" : ver;
    bool hex = tip.size() >= 8;
    for (int i = 0; hex && i < 8; ++i) hex = std::isxdigit((unsigned char)tip[i]) != 0;
    if (hex) {
        std::string t8 = tip.substr(0, 8);
        for (char &c : t8) c = (char)std::tolower((unsigned char)c);
        id += "-" + t8;
    }
    if (id.size() > 24) id.resize(24);
    return id;
}

/* The lobby address: SM64DS_LOBBY_URL, else the launcher's launcher_config.json LobbyUrl, else the live one. */
std::string lobby_base()
{
    if (const char *e = std::getenv("SM64DS_LOBBY_URL"))
        if (*e) { std::string s = e; while (!s.empty() && s.back() == '/') s.pop_back(); return s; }
    std::string t;
    JV v;
    if (read_file(exe_dir() + "/launcher_config.json", &t) && jparse(t, &v)) {
        std::string u = v.str("LobbyUrl");
        while (!u.empty() && u.back() == '/') u.pop_back();
        if (u.compare(0, 8, "https://") == 0 || u.compare(0, 7, "http://") == 0) return u;
    }
    return "https://tangos.dev/port/lobby";
}

std::string coop_folder()
{
    std::string d = ntr::backup::path();
    const size_t cut = d.find_last_of("/\\");
    return (cut == std::string::npos ? std::string(".") : d.substr(0, cut)) + "/coop";
}

bool save_name_char(char c) { return c >= 0x20 && c <= 0x7e && !std::strchr("\\/:*?\"<>|", c); }

/* LobbyWire.SaveFileStem */
std::string save_stem(const std::string &display)
{
    std::string s;
    for (char c : display)
        s += ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ' || c == '-' || c == '_') ? c : '_';
    while (!s.empty() && (s.back() == ' ' || s.back() == '.')) s.pop_back();
    while (!s.empty() && (s[0] == ' ' || s[0] == '.')) s.erase(0, 1);
    if (s.empty()) s = "coop";
    std::string up = s;
    for (char &c : up) c = (char)std::toupper((unsigned char)c);
    if (up == "CON" || up == "PRN" || up == "AUX" || up == "NUL" ||
        (up.size() == 4 && (!up.compare(0, 3, "COM") || !up.compare(0, 3, "LPT")) && up[3] >= '0' && up[3] <= '9'))
        s = "coop_" + s;
    return s;
}

/* THE INSTALL ID (lobby v7, LOBBYBAN1): 32 random lower-case hex, made once and kept in <save folder>/lobby_install.txt beside the
   save. It is never derived from the hardware or the user's name; deleting the file makes a new one on the next create or join. The
   lobby server keeps it only so a host's ban covers a restarted game. Random bytes come from advapi32's RtlGenRandom (loaded by name,
   KERNEL32 stays the only import); without it, the clock and process ids are mixed instead. */
std::string install_id()
{
    static std::string id;
    if (!id.empty()) return id;
    std::string dir = ntr::backup::path();
    const size_t cut = dir.find_last_of("/\\");
    dir = cut == std::string::npos ? std::string(".") : dir.substr(0, cut);
    const std::string file = dir + "/lobby_install.txt";
    if (FILE *f = std::fopen(file.c_str(), "rb")) {
        char b[40] = {0};
        const size_t n = std::fread(b, 1, 32, f);
        std::fclose(f);
        bool ok = n == 32;
        for (size_t i = 0; ok && i < 32; ++i) ok = (b[i] >= '0' && b[i] <= '9') || (b[i] >= 'a' && b[i] <= 'f');
        if (ok) { id.assign(b, 32); return id; }
    }
    unsigned char r[16] = {0};
    bool got = false;
    if (HMODULE a = LoadLibraryA("advapi32.dll")) {
        typedef BOOLEAN (WINAPI *GenRandom)(PVOID, ULONG);
        if (GenRandom fn = (GenRandom)(void *)GetProcAddress(a, "SystemFunction036")) got = fn(r, sizeof r) != 0;
    }
    if (!got) {
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        unsigned long long x = (unsigned long long)q.QuadPart ^ ((unsigned long long)GetCurrentProcessId() << 32) ^ GetTickCount64() ^
                               (unsigned long long)(size_t)&q;
        for (int i = 0; i < 16; ++i) {   /* splitmix64 */
            x += 0x9E3779B97F4A7C15ull;
            unsigned long long z = x;
            z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
            z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
            r[i] = (unsigned char)(z ^ (z >> 31));
        }
    }
    static const char hx[] = "0123456789abcdef";
    std::string made;
    for (unsigned char c : r) { made += hx[c >> 4]; made += hx[c & 15]; }
    CreateDirectoryA(dir.c_str(), 0);
    if (FILE *f = std::fopen(file.c_str(), "wb")) { std::fwrite(made.data(), 1, made.size(), f); std::fclose(f); }
    else say("install id: could not write lobby_install.txt in the save folder (the id lasts this run only)");
    id = made;
    return id;
}

/* the same request at v6 without `install`: the body a server older than v7 takes (a create / join it refused as bad_version) */
std::string v6_body(const std::string &v7)
{
    std::string b = v7;
    const size_t p = b.find("\"v\":7");
    if (p != std::string::npos) b.replace(p, 5, "\"v\":6");
    const size_t i = b.find(",\"install\":\"");
    if (i != std::string::npos) {
        const size_t e = b.find('"', i + 12);
        if (e != std::string::npos) b.erase(i, e + 1 - i);
    }
    return b;
}

/* CoopSaves.Existing: newest first */
std::vector<std::string> coop_saves()
{
    std::vector<std::pair<unsigned long long, std::string>> v;
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA((coop_folder() + "/*.coopsav").c_str(), &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            std::string n = fd.cFileName;
            n.resize(n.size() - 8);
            bool okname = !n.empty() && n.size() <= 24 && n[0] != ' ' && n.back() != ' ';
            for (char c : n) okname = okname && save_name_char(c);
            if (okname)
                v.emplace_back(((unsigned long long)fd.ftLastWriteTime.dwHighDateTime << 32) | fd.ftLastWriteTime.dwLowDateTime, n);
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    std::vector<std::string> o;
    while (!v.empty()) {
        size_t b = 0;
        for (size_t i = 1; i < v.size(); ++i)
            if (v[i].first > v[b].first) b = i;
        o.push_back(v[b].second);
        v.erase(v.begin() + (long)b);
    }
    return o;
}

/* The save the host plays: Continue = the newest file, New = a name not used yet ("Coop 1", "Coop 2", ...; the server name when set). */
std::string coop_save_display(bool cont, const std::string &server_name)
{
    std::vector<std::string> have = coop_saves();
    if (cont && !have.empty()) return have[0];
    auto used = [&](const std::string &n) {
        for (const std::string &h : have)
            if (save_stem(h) == save_stem(n)) return true;
        return false;
    };
    std::string base;
    for (char c : server_name)
        if (save_name_char(c)) base += c;
    while (!base.empty() && base.back() == ' ') base.pop_back();
    while (!base.empty() && base[0] == ' ') base.erase(0, 1);
    if (base.size() > 20) base.resize(20);
    if (!base.empty() && !used(base)) return base;
    if (base.empty()) base = "Coop";
    for (int i = 1; i < 1000; ++i) {
        char b[32];
        std::snprintf(b, sizeof b, "%s %d", base.c_str(), i);
        if (!used(b)) return b;
    }
    return base;
}

/* ---- identity ------------------------------------------------------------------------------------------------------------------- */
void identity(std::string *nick, std::string *body, std::string *shoes)
{
    const char *n = host_setting_online_name();
    *nick = n && *n ? n : host_setting_launcher_nick();
    std::string c;
    for (char ch : *nick)
        if (ch >= 0x20 && ch <= 0x7e && ch != ',') c += ch;
    while (!c.empty() && c.back() == ' ') c.pop_back();
    while (!c.empty() && c[0] == ' ') c.erase(0, 1);
    if (c.size() > 16) c.resize(16);
    if (c.empty()) c = "Player";
    *nick = c;
    unsigned b = 0, s = 0;
    body->clear();
    shoes->clear();
    if (host_setting_online_colours(&b, &s)) {
        char x[8];
        std::snprintf(x, sizeof x, "%06x", b & 0xFFFFFFu); *body = x;
        std::snprintf(x, sizeof x, "%06x", s & 0xFFFFFFu); *shoes = x;
    }
}

/* ================================================================================================================================ */
/* Folding an answer into the record (under the lock)                                                                              */
/* ================================================================================================================================ */
unsigned rgb6(const std::string &s, unsigned def)
{
    if (s.size() != 6) return def;
    unsigned v = 0;
    for (char c : s) {
        if (!std::isxdigit((unsigned char)c)) return def;
        v = v * 16 + (unsigned)(c <= '9' ? c - '0' : (c | 0x20) - 'a' + 10);
    }
    return v;
}

std::string member_name(int seat)
{
    for (const Member &m : g.members)
        if (m.seat == seat) return m.name;
    return "Player";
}

void fold_view(const JV &v)
{
    g.state = safe(v.str("state"), 16);
    g.session_type = safe(v.str("session_type"), 12);
    /* a joiner learns the room's kind here (it asked to join a code, not a kind) */
    if (g.session_type == "vs") g.kind = LOBBY_VS;
    else if (g.session_type == "coop") g.kind = LOBBY_COOP;
    g.host = (int)v.i("host");
    g.you = (int)v.i("you");
    int seats = (int)v.i("room_seats");
    if (seats <= 0) seats = (int)v.i("max_players");
    if (seats <= 0) seats = 4;
    g.max_seats = seats > 16 ? 16 : seats;
    g.dial_max = (int)v.i("dial_max");
    { const int am = (int)v.i("arena_max", 4); g.arena_max = am < 2 ? 4 : am > 16 ? 16 : am; }   /* the server's largest Arena match */
    g.map = (int)v.i("map");
    g.match_players = (int)v.i("match_players", 2);
    g.win_mode = safe(v.str("win_mode"), 8);
    g.star_target = (int)v.i("star_target", 0);
    g.luigi = (int)v.i("luigi", 0);
    g.netmode = safe(v.str("netmode"), 10);
    g.arena_ok = v.flag("arena_ok");
    g.started = v.flag("started");
    g.launch_no = (int)v.i("launch");
    g.save_name = safe(v.str("save_name"), 24);
    if (const JV *s = v.get("settings"))
        for (int i = 0; i < kNCoop; ++i)
            if (s->has_num(kCoopRules[i].key)) g.coop[i] = s->i(kCoopRules[i].key) ? 1 : 0;
    g.members.clear();
    if (const JV *ms = v.get("members"))
        for (const JV &m : ms->a) {
            Member x;
            x.seat = (int)m.i("seat");
            x.name = safe(m.str("display"), 20);
            x.color = m.str("color");
            x.shoes = m.str("shoes");
            x.pre_ok = m.flag("pre_ok");
            x.playing = m.flag("playing");
            g.members.push_back(x);
        }
}

void enqueue_ready(const std::string &match);
void enqueue(const std::string &verb, const std::string &body, int kind = 0);
std::string head();
void send_params_locked();

void fold_events(const JV &evs, bool resync)
{
    if (resync) g.chat.clear();
    for (const JV &e : evs.a) {
        const std::string k = safe(e.str("kind"), 16), who = safe(e.str("display"), 20);
        const int seat = (int)e.i("seat");
        const bool muted = g.muted.count(seat) && g.muted[seat];
        if (k == "chat") { if (!muted) push_chat(who + ": " + safe(e.str("text"), 200)); }
        else if (k == "joined") push_chat(who + " joined.");
        else if (k == "left") push_chat(who + (safe(e.str("why"), 16) == "kicked" ? " was removed by the host." : " left."));
        else if (k == "dropped") push_chat(who + " dropped out of the match.");
        else if (k == "host") push_chat(who + " is the host now.");
        else if (k == "arming") {
            g.match = safe(e.str("match"), 40);
            push_chat("The match is starting.");
        } else if (k == "go" && e.get("plan")) {
            g.plan = *e.get("plan");
            g.plan_set = true;
        } else if (k == "failed") {
            push_chat("The match could not start (" + safe(e.str("reason"), 24) + ").");
            g.start_wanted = false;
            if (g.phase == LOBBY_STARTING) g.phase = LOBBY_IN_ROOM;
        } else if (k == "result") push_chat("The match is over.");
        else if (k == "started") push_chat("The host started the game.");
    }
    if (g.state == "arming" && !g.match.empty() && g.readied != g.match) {
        bool playing = false;
        for (const Member &m : g.members)
            if (m.seat == g.you) playing = m.playing;
        if (playing) { g.readied = g.match; enqueue_ready(g.match); }
    }
}

/* the room's answer to a verb or a poll; 0 = fine, else the record was left (the room is gone for this console) */
void leave_room_locked(const char *why, const char *head = 0);

void fold(const Answer &a, int gen, bool from_poll)
{
    Lock l;
    if (gen != g.gen) return;   /* an answer for a room this console has left */
    if (!a.ok) {
        if (a.error == "no_such_room" || a.error == "not_a_member" || a.error == "kicked" || a.error == "banned") {
            /* the heading over the sentence: a removed player was not trying to get in */
            const bool removed = a.error == "kicked" || a.error == "banned";
            const bool was_in = g.phase == LOBBY_IN_ROOM || g.phase == LOBBY_STARTING;
            leave_room_locked(a.message.c_str(), removed ? "Removed from the room" : was_in ? "Out of the room" : 0);
            return;
        }
        g.error = a.message;
        return;
    }
    /* ONLY A POLL MOVES THE CURSOR. A verb answers with the room's newest sequence number, and taking it would skip every event the
       poll has not delivered yet (a guest's own `ready` is what moves the room to `go`, so its answer already points past the `go`). */
    if (from_poll && a.body.has_num("cursor")) g.cursor = (int)a.body.i("cursor");
    if (const JV *v = a.body.get("view")) fold_view(*v);
    if (g.session_type == "vs" && g.you != g.host && g.state == "lobby" && g.netmode == "lockstep") {
        /* a launcher-made room set to Classic: the game does not run the restart that needs in 1.0 */
        enqueue("leave", head() + "}", 3);
        leave_room_locked("Classic netcode rooms open from the launcher for now.");
        return;
    }
    if (!g.session_type.empty() && g.session_type != "vs" && g.session_type != "coop") {
        /* a party or adventure room joined by its code: phase 1 plays coop and VS rooms only */
        enqueue("leave", head() + "}", 3);
        leave_room_locked("That room is not a coop or VS room. Join it from the launcher.");
        return;
    }
    if (const JV *ev = a.body.get("events")) fold_events(*ev, a.body.flag("resync"));
    if (const JV *p = a.body.get("plan")) { g.plan = *p; g.plan_set = true; }
    if (g.state == "closed") leave_room_locked("The room closed.", "Out of the room");
}

void leave_room_locked(const char *why, const char *head)
{
    g.room.clear();
    g.token.clear();
    g.cursor = 0;
    ++g.gen;
    g.phase = why && *why ? LOBBY_FAILED : LOBBY_NONE;
    g.error = why ? why : "";
    g.error_head = head ? head : "";
    g.members.clear();
    g.plan_set = false;
    g.start_wanted = false;
    g.match.clear();
    g.readied.clear();
    say("left the room%s%s", why && *why ? ": " : "", why ? why : "");
}

/* ================================================================================================================================ */
/* The workers                                                                                                                      */
/* ================================================================================================================================ */
struct Job { std::string verb, body; int gen; int kind; };   /* kind: 0 plain, 1 create, 2 join, 3 leave */
std::vector<Job> g_jobs;
HANDLE g_job_event = 0, g_poll_event = 0;
bool g_threads = false;

void enqueue(const std::string &verb, const std::string &body, int kind)
{
    g_jobs.push_back(Job{verb, body, g.gen, kind});
    SetEvent(g_job_event);
}

/* the common head of a member's request (under the lock) */
std::string head()
{
    return "{\"v\":6,\"room\":" + jesc(g.room) + ",\"token\":" + jesc(g.token);
}

void enqueue_ready(const std::string &match) { enqueue("ready", head() + ",\"match\":" + jesc(match) + "}"); }

DWORD WINAPI action_thread(void *)
{
    for (;;) {
        WaitForSingleObject(g_job_event, 1000);
        for (;;) {
            Job j;
            std::string base;
            {
                Lock l;
                if (g_jobs.empty()) break;
                j = g_jobs.front();
                g_jobs.erase(g_jobs.begin());
                base = g.base;
            }
            Answer a = post(base, j.verb, j.body, 10);
            say("%s -> %d%s%s", j.verb.c_str(), a.status, a.error.empty() ? "" : " ", a.error.c_str());
            bool v6 = false;
            if ((j.kind == 1 || j.kind == 2) && !a.ok && a.error == "bad_version") {
                /* a lobby server older than v7 (no ban yet): the same request at v6, without the install id */
                a = post(base, j.verb, v6_body(j.body), 10);
                v6 = true;
                say("%s at v6 -> %d%s%s", j.verb.c_str(), a.status, a.error.empty() ? "" : " ", a.error.c_str());
            }
            if (j.verb == "ban" && !a.ok && (a.error == "unknown_verb" || a.error == "bad_version" || a.error == "bad_field")) {
                a.message = "This lobby server cannot ban players yet. Kick still works.";
                Lock l;
                g.srv_ban = false;
            }
            if (j.kind == 1 || j.kind == 2) {   /* create / join: a room or a sentence */
                Lock l;
                if (j.gen != g.gen) continue;
                const std::string room = a.body.str("room"), token = a.body.str("token");
                if (!a.ok || room.size() != 6 || token.size() != 32) {
                    g.phase = LOBBY_FAILED;
                    g.error = a.ok ? "The lobby server sent something this game did not understand." : a.message;
                    say("%s failed: %s", j.verb.c_str(), g.error.c_str());
                    continue;
                }
                g.room = room;
                g.token = token;
                g.cursor = 0;
                g.srv_ban = !v6;
                g.phase = LOBBY_IN_ROOM;
                g.error.clear();
                if (j.kind == 1 && g.kind == LOBBY_VS) send_params_locked();   /* the host's room asks for Arena (1.0) */
                say("%s: room %s, seat %d", j.verb == "create" ? "created" : "joined", room.c_str(), (int)a.body.i("member"));
                SetEvent(g_poll_event);
                continue;
            }
            if (j.kind == 3) continue;   /* leave: the record was cleared when it was asked */
            fold(a, j.gen, false);
            SetEvent(g_poll_event);
        }
    }
}

DWORD WINAPI poll_thread(void *)
{
    int fails = 0;
    for (;;) {
        std::string base, body;
        int gen;
        {
            Lock l;
            if (g.room.empty()) body.clear();
            else body = head() + ",\"cursor\":" + std::to_string(g.cursor) + ",\"wait\":10}";
            base = g.base;
            gen = g.gen;
        }
        if (body.empty()) { WaitForSingleObject(g_poll_event, 1000); continue; }
        Answer a = post(base, "poll", body, 25);
        if (!a.ok && a.status == 0) {
            /* the network, not the room: say so and keep the room (the server keeps the seat 45 s) */
            { Lock l; if (gen == g.gen) g.error = a.message; }
            ++fails;
            WaitForSingleObject(g_poll_event, fails < 3 ? 1000 : 3000);
            continue;
        }
        fails = 0;
        fold(a, gen, true);
        if (!a.ok) WaitForSingleObject(g_poll_event, 1000);
    }
}

void start_threads()
{
    if (g_threads) return;
    g_threads = true;
    g_job_event = CreateEventA(0, FALSE, FALSE, 0);
    g_poll_event = CreateEventA(0, FALSE, FALSE, 0);
    HANDLE a = CreateThread(0, 0, action_thread, 0, 0, 0);
    HANDLE p = CreateThread(0, 0, poll_thread, 0, 0, 0);
    if (a) CloseHandle(a);
    if (p) CloseHandle(p);
}

/* ================================================================================================================================ */
/* Starting a match                                                                                                                 */
/* ================================================================================================================================ */
struct Env { std::vector<std::string> set_name, set_value, clear; };

void env_set(Env *e, const char *n, const std::string &v)
{
    for (size_t i = 0; i < e->set_name.size(); ++i)
        if (e->set_name[i] == n) { e->set_value[i] = v; return; }
    e->set_name.push_back(n);
    e->set_value.push_back(v);
}

bool env_has(const Env &e, const std::string &n)
{
    for (const std::string &s : e.set_name)
        if (s == n) return true;
    return false;
}

/* LobbyLaunch.ClearList: AlwaysClear minus what is set, then every inherited SM64DS_COMMS_ / _VS_ / _HUB_ / _VOICE_ / _SESSION_ name */
void env_clear_list(Env *e)
{
    for (const char *n : kAlwaysClear)
        if (!env_has(*e, n)) e->clear.push_back(n);
    char *blk = GetEnvironmentStringsA();
    if (!blk) return;
    for (const char *p = blk; *p; p += std::strlen(p) + 1) {
        const char *eq = std::strchr(p + 1, '=');
        if (!eq) continue;
        const std::string n(p, eq);
        bool ours = false;
        for (const char *pre : kClearPrefixes)
            if (!n.compare(0, std::strlen(pre), pre)) ours = true;
        if (!ours || env_has(*e, n)) continue;
        bool dup = false;
        for (const std::string &c : e->clear) dup = dup || c == n;
        if (!dup) e->clear.push_back(n);
    }
    FreeEnvironmentStringsA(blk);
}

bool relay_ok(const std::string &s)
{
    if (s.empty() || s.size() > 64) return false;
    for (char c : s)
        if (!(std::isalnum((unsigned char)c) || c == '.' || c == '-' || c == ':')) return false;
    return true;
}

bool code8_ok(const std::string &s)
{
    if (s.size() != 8) return false;
    for (char c : s)
        if (!std::strchr("ABCDEFGHJKMNPQRSTUVWXYZ23456789", c)) return false;
    return true;
}

/* LobbyLaunch.PickCommsPort with the bind test of UdpPortFree */
int pick_comms_port()
{
    typedef unsigned (WINAPI * Fsocket)(int, int, int);
    typedef int(WINAPI * Fbind)(unsigned, const void *, int);
    typedef int(WINAPI * Fclose)(unsigned);
    typedef int(WINAPI * Fstartup)(unsigned short, void *);
    HMODULE w = LoadLibraryA("ws2_32.dll");
    Fsocket fs = w ? (Fsocket)GetProcAddress(w, "socket") : 0;
    Fbind fb = w ? (Fbind)GetProcAddress(w, "bind") : 0;
    Fclose fc = w ? (Fclose)GetProcAddress(w, "closesocket") : 0;
    Fstartup st = w ? (Fstartup)GetProcAddress(w, "WSAStartup") : 0;
    char wsa[512];
    if (st) st(0x0202, wsa);
    auto free_port = [&](int port) {
        if (!fs || !fb || !fc) return true;
        unsigned s = fs(2, 2, 17);
        if (s == (unsigned)~0u) return false;
        struct { unsigned short fam, port; unsigned addr; char zero[8]; } a = {2, (unsigned short)((port >> 8) | (port << 8)), 0x0100007f, {0}};
        const bool ok = fb(s, &a, (int)sizeof a) == 0;
        fc(s);
        return ok;
    };
    int base = 49152 + (int)((GetCurrentProcessId() * 16u) % 12000u);
    base -= base % 16;
    for (int i = 0; i < 32; ++i) {
        const int cand = base + i * 16;
        if (cand + 15 > 65535) break;
        if (cand <= 51770 && cand + 3 >= 51762) continue;
        bool span = true;
        for (int k = 0; k < 16 && span; ++k) span = free_port(cand + k);
        if (span) return cand;
    }
    return 51765;
}

/* The successor's way back into the room: url|room|token|cursor|launched_no|match|kind */
std::string resume_value()
{
    return g.base + "|" + g.room + "|" + g.token + "|" + std::to_string(g.cursor) + "|" + std::to_string(g.launched_no) + "|" +
           g.launched_match + "|" + (g.kind == LOBBY_VS ? "vs" : "coop") + "|" + std::to_string(g.you) + "|" + std::to_string(g.host);
}

/* the VS result reader: the offset of this process's own log when the match was started here */
long long g_log_from = -1;
std::string g_result_match;

std::string own_log_path()
{
    HANDLE h = (HANDLE)_get_osfhandle(_fileno(stderr));
    if (!h || h == INVALID_HANDLE_VALUE || GetFileType(h) != FILE_TYPE_DISK) return std::string();
    typedef DWORD(WINAPI * Fgp)(HANDLE, LPSTR, DWORD, DWORD);
    Fgp gp = (Fgp)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetFinalPathNameByHandleA");
    char p[MAX_PATH * 2] = "";
    if (!gp || !gp(h, p, (DWORD)sizeof p, 0)) return std::string();
    std::string s = p;
    if (!s.compare(0, 4, "\\\\?\\")) s = s.substr(4);
    return s;
}

long long own_log_size()
{
    std::fflush(stderr);
    const std::string p = own_log_path();
    WIN32_FILE_ATTRIBUTE_DATA fa;
    if (p.empty() || !GetFileAttributesExA(p.c_str(), GetFileExInfoStandard, &fa)) return -1;
    return ((long long)fa.nFileSizeHigh << 32) | fa.nFileSizeLow;
}

void watch_result(const std::string &match)
{
    g_result_match = match;
    g_log_from = own_log_size();
    say("watching this process's log for the match result (match %s, from byte %lld)", match.c_str(), g_log_from);
}

/* Build the request, hand it over in place, else restart with it. Called on the game thread at the title, NOT under the lock. */
void launch(const JV &plan, LobbyKind kind, bool host, const std::string &match)
{
    std::string nick, body, shoes;
    identity(&nick, &body, &shoes);
    Env e;
    const std::string relay = plan.str("relay"), code = plan.str("code");
    std::string why;
    if (!relay_ok(relay) || !code8_ok(code)) why = "the lobby's plan looked wrong";
    const char *hkind = kind == LOBBY_VS ? "vs" : "coop";
    if (why.empty() && kind == LOBBY_COOP) {
        /* LobbyLaunch.BuildAdventureEnv, the coop arm */
        env_set(&e, "SM64DS_ADVENTURE", "1");
        env_set(&e, "SM64DS_COMMS_RELAY", relay);
        env_set(&e, "SM64DS_COMMS_CODE", code);
        env_set(&e, "SM64DS_ROOM_KIND", "coop");
        env_set(&e, "SM64DS_VOICE_ALLOWED", "0");
        env_set(&e, "SM64DS_COOP", "1");
        const std::string sn = safe(plan.str("save_name"), 24);
        if (!sn.empty()) env_set(&e, "SM64DS_COOP_SAVE_NAME", sn);
        env_set(&e, "SM64DS_SKIP_MENU", "1");
        env_set(&e, "SM64DS_BOOT_FILE", "0");
        if (host) {
            env_set(&e, "SM64DS_COOP_HOST", "1");
            std::string disp;
            { Lock l; disp = coop_save_display(g.coop_continue != 0, g.server_name); }
            CreateDirectoryA(coop_folder().c_str(), 0);
            char full[MAX_PATH] = "";
            GetFullPathNameA((coop_folder() + "/" + save_stem(disp) + ".coopsav").c_str(), (DWORD)sizeof full, full, 0);
            env_set(&e, "SM64DS_COOP_SAVE", full);
            say("coop save: %s \"%s\" (%s)", g.coop_continue ? "Continue" : "New", disp.c_str(), full);
        }
        if (const JV *s = plan.get("settings"))
            for (int i = 0; i < kNCoop; ++i)
                if (s->has_num(kCoopRules[i].key)) env_set(&e, kCoopRules[i].env, s->i(kCoopRules[i].key) ? "1" : "0");
        const long long ws = plan.i("want_seat", -1);
        if (ws >= 0 && ws < 16) env_set(&e, "SM64DS_HUB_SEAT", std::to_string(ws));
    } else if (why.empty()) {
        /* LobbyLaunch.BuildChildEnv */
        const std::string role = plan.str("role");
        if (plan.str("netmode") != "arena") why = "Classic netcode rooms open from the launcher for now";
        const long long map = plan.i("map", -1), players = plan.i("players"), slot = plan.i("slot");
        if ((role != "parent" && role != "child") || map < 0 || map > 3 || players < 1 || players > 16 || slot < 0 || slot >= 16)
            why = "the lobby's VS plan looked wrong";
        else {
            env_set(&e, "SM64DS_VS_MAP", std::to_string(map));
            env_set(&e, "SM64DS_COMMS_RELAY", relay);
            env_set(&e, "SM64DS_COMMS_CODE", code);
            env_set(&e, "SM64DS_COMMS_ROLE", role);
            env_set(&e, "SM64DS_COMMS_PORT", std::to_string(pick_comms_port()));
            env_set(&e, "SM64DS_VS_PLAYERS", std::to_string(players));
            env_set(&e, "SM64DS_ADVENTURE", "0");
            if (role == "child") env_set(&e, "SM64DS_COMMS_SLOT", std::to_string(slot));
            if (plan.has_num("star_target")) env_set(&e, "SM64DS_VS_STAR_TARGET", std::to_string(plan.i("star_target")));
            if (plan.has_num("king_target")) env_set(&e, "SM64DS_VS_KING_TARGET", std::to_string(plan.i("king_target")));
            const std::string names = plan.str("names"), cols = plan.str("colors");
            if (!names.empty() && names.size() <= 271) env_set(&e, "SM64DS_VS_NAMES", safe(names, 271));
            if (cols.size() >= 3 && cols.size() <= 223 && cols.find_first_not_of(",") != std::string::npos)
                env_set(&e, "SM64DS_VS_COLORS", safe(cols, 223));
            if (plan.i("luigi") == 1) env_set(&e, "SM64DS_VS_LUIGI_INFECTION", "1");
            if (plan.has_num("seed")) env_set(&e, "SM64DS_VS_LUIGI_SEED", std::to_string(plan.i("seed")));
            env_set(&e, "SM64DS_VOICE_ALLOWED", "0");
            if (plan.has_num("digest")) env_set(&e, "SM64DS_SESSION_DIGEST", std::to_string(plan.i("digest")));
            if (plan.str("netmode") == "arena") env_set(&e, "SM64DS_VS_NETMODE", "arena");
        }
    }
    if (!why.empty()) {
        Lock l;
        g.error = "The match could not start: " + why + ".";
        g.phase = g.room.empty() ? LOBBY_FAILED : LOBBY_IN_ROOM;
        g.start_wanted = false;
        if (kind == LOBBY_VS && !match.empty() && !g.room.empty())
            enqueue("failed", head() + ",\"match\":" + jesc(match) + ",\"reason\":\"spawn_failed\"}");
        say("start refused: %s", why.c_str());
        return;
    }
    /* IdentityEnv */
    env_set(&e, "SM64DS_PLAYER_NAME", nick);
    if (body.size() == 6 && shoes.size() == 6) { env_set(&e, "SM64DS_PLAYER_COLOR", body); env_set(&e, "SM64DS_PLAYER_SHOES", shoes); }
    env_clear_list(&e);

    std::vector<const char *> sn, sv, cn;
    for (size_t i = 0; i < e.set_name.size(); ++i) { sn.push_back(e.set_name[i].c_str()); sv.push_back(e.set_value[i].c_str()); }
    for (const std::string &c : e.clear) cn.push_back(c.c_str());
    PortHandoffRequest q;
    q.kind = hkind;
    q.nonce = "0000000000000000";
    q.nset = (int)sn.size();
    q.set_name = sn.data();
    q.set_value = sv.data();
    q.nclear = (int)cn.size();
    q.clear_name = cn.empty() ? 0 : cn.data();
    say("starting %s (%s, %d name(s) set, %d cleared): in this process first", hkind, host ? "host" : "guest", q.nset, q.nclear);
    if (kind == LOBBY_VS) watch_result(match);
    const char *refusal = port_handoff_dispatch(&q);
    if (!refusal) {
        Lock l;
        g.phase = LOBBY_IN_ROOM;
        g.start_wanted = false;
        say("the running game took the %s start", hkind);
        return;
    }
    if (kind == LOBBY_VS) {
        /* 1.0: a VS match the running game cannot take is not restarted into (the restart path has not been proven) */
        Lock l;
        g.error = "The match could not start in this game. Try again.";
        g.phase = g.room.empty() ? LOBBY_FAILED : LOBBY_IN_ROOM;
        g.start_wanted = false;
        if (!match.empty() && !g.room.empty())
            enqueue("failed", head() + ",\"match\":" + jesc(match) + ",\"reason\":\"spawn_failed\"}");
        say("in place refused: %s; VS is not restarted into in 1.0, the match is reported failed", refusal);
        return;
    }
    say("in place refused: %s; restarting with the launcher's names", refusal);
    /* THE RESTART: this process's environment becomes the fresh game's (both copies), the room rides along, and the game starts itself
       again the way the launcher would start it with those names. */
    port_handoff_apply_env(&q);
    {
        Lock l;
        SetEnvironmentVariableA("SM64DS_LOBBY_RESUME", resume_value().c_str());
    }
    char exe[MAX_PATH] = "";
    GetModuleFileNameA(0, exe, (DWORD)sizeof exe);
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    std::memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    std::memset(&pi, 0, sizeof pi);
    bool ok = false;
#ifndef PORT_ROM_CLEAN
    if (!std::getenv("SM64DS_LOBBY_TEST_RELAUNCH_FAIL"))
#endif
        ok = CreateProcessA(exe, 0, 0, 0, FALSE, 0, 0, 0, &si, &pi) != 0;
    SetEnvironmentVariableA("SM64DS_LOBBY_RESUME", 0);
    if (!ok) {
        port_handoff_unset_env(&q);
        Lock l;
        g.error = "The game could not restart to start the match.";
        g.phase = LOBBY_IN_ROOM;
        g.start_wanted = false;
        if (kind == LOBBY_VS && !match.empty())
            enqueue("failed", head() + ",\"match\":" + jesc(match) + ",\"reason\":\"spawn_failed\"}");
        say("restart failed (win32 %lu)", (unsigned long)GetLastError());
        return;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    say("restarted as pid %lu; this process is ending (the successor carries the room)", (unsigned long)pi.dwProcessId);
    {
        Lock l;
        g.phase = LOBBY_STARTING;
        g.room.clear();   /* the successor owns the seat now: no leave from here, no more polls */
        g.token.clear();
        ++g.gen;
    }
    port_window_quit(0);
}

/* ---- the VS result: the launcher's ParseMatchOver + WireWin over this process's own log ------------------------------------------ */
std::string field(const std::string &line, const char *key)
{
    size_t at = line.find(key);
    if (at == std::string::npos) return std::string();
    at += std::strlen(key);
    size_t e = at;
    while (e < line.size() && line[e] != ' ' && line[e] != '\t' && line[e] != '\r' && line[e] != '\n') ++e;
    return line.substr(at, e - at);
}

void result_tick()
{
    if (g_log_from < 0 || g_result_match.empty()) return;
    const std::string p = own_log_path();
    FILE *f = p.empty() ? 0 : std::fopen(p.c_str(), "rb");
    if (!f) return;
    _fseeki64(f, g_log_from, SEEK_SET);
    std::string t;
    char buf[8192];
    size_t n;
    while ((n = std::fread(buf, 1, sizeof buf, f)) > 0 && t.size() < (1u << 22)) t.append(buf, n);
    std::fclose(f);
    size_t at = t.find("[vs] MATCH OVER f");
    if (at == std::string::npos) return;
    size_t eol = t.find('\n', at);
    if (eol == std::string::npos) return;   /* the line is not whole yet */
    const std::string line = t.substr(at, eol - at);
    std::string win = field(line, "win="), scores = field(line, "scores=");
    static const char *const kinds[] = {"time-up", "star-target", "king-target", "luigi-all-infected", "survivor-timeout"};
    bool known = false;
    for (const char *k : kinds) known = known || win == k;
    std::vector<int> sc;
    bool parsed = known && !scores.empty();
    for (size_t s = 0; parsed && s <= scores.size();) {
        size_t c = scores.find(',', s);
        if (c == std::string::npos) c = scores.size();
        const std::string one = scores.substr(s, c - s);
        if (one.empty() || one.size() > 2 || one.find_first_not_of("0123456789") != std::string::npos) parsed = false;
        else sc.push_back(std::atoi(one.c_str()));
        s = c + 1;
    }
    if (parsed && (sc.size() < 4 || sc.size() > 16)) parsed = false;
    if (!parsed) { win = "draw"; sc.assign(4, 0); }
    else if ((win == "time-up" || win == "king-target") && field(line, "winner=") == "-1") win = "draw";
    std::string arr = "[";
    for (size_t i = 0; i < sc.size(); ++i) arr += (i ? "," : "") + std::to_string(sc[i]);
    arr += "]";
    {
        Lock l;
        if (!g.room.empty())
            enqueue("result", head() + ",\"match\":" + jesc(g_result_match) + ",\"win\":" + jesc(win) + ",\"scores\":" + arr + "}");
    }
    say("match result read from this process's log: win=%s scores=%s; posted to the lobby", win.c_str(), arr.c_str());
    g_log_from = -1;
    g_result_match.clear();
}

/* ---- the frame: the start, the resume, the test knob ----------------------------------------------------------------------------- */
void (*g_prev_hook)(void) = 0;
unsigned g_title_frames = 0;

void resume_from_env()
{
    static bool done = false;
    if (done) return;
    done = true;
    const char *r = std::getenv("SM64DS_LOBBY_RESUME");
    if (!r || !*r) return;
    std::string v = r;
    SetEnvironmentVariableA("SM64DS_LOBBY_RESUME", 0);
    _putenv_s("SM64DS_LOBBY_RESUME", "");
    std::vector<std::string> f;
    for (size_t s = 0;;) {
        size_t c = v.find('|', s);
        f.push_back(v.substr(s, c == std::string::npos ? std::string::npos : c - s));
        if (c == std::string::npos) break;
        s = c + 1;
    }
    if (f.size() < 9 || f[1].size() != 6 || f[2].size() != 32) { say("SM64DS_LOBBY_RESUME does not read; ignored"); return; }
    init_once();
    {
        Lock l;
        g.base = f[0];
        g.game = game_build_id();
        g.room = f[1];
        g.token = f[2];
        g.cursor = std::atoi(f[3].c_str());
        g.launched_no = std::atoi(f[4].c_str());
        g.launched_match = f[5];
        g.kind = f[6] == "vs" ? LOBBY_VS : LOBBY_COOP;
        g.you = std::atoi(f[7].c_str());
        g.host = std::atoi(f[8].c_str());
        g.phase = LOBBY_IN_ROOM;
        ++g.gen;
    }
    start_threads();
    if (!f[5].empty()) watch_result(f[5]);
    say("resumed room %s from the restart (launch %s, match %s)", f[1].c_str(), f[4].c_str(), f[5].empty() ? "-" : f[5].c_str());
}

/* the server's plan says this console should start now */
bool launch_due(JV *plan, LobbyKind *kind, bool *host, std::string *match)
{
    Lock l;
    if (g.room.empty() || !g.plan_set) return false;
    *kind = g.kind;
    *host = g.plan.get("host") ? g.plan.flag("host") : g.you == g.host;   /* the plan's own word, as the launcher reads it */
    if (g.kind == LOBBY_COOP) {
        const int n = (int)g.plan.i("launch");
        if (!g.plan.flag("started") || n <= g.launched_no) return false;
        g.launched_no = n;
    } else {
        if (g.state != "go" && g.state != "in_match") return false;
        if (g.match.empty() || g.match == g.launched_match || !g.plan.flag("playing")) return false;
        g.launched_match = g.match;
        *match = g.match;
    }
    *plan = g.plan;
    g.phase = LOBBY_STARTING;
    return true;
}

#ifndef PORT_ROM_CLEAN
/* SM64DS_LOBBY_TEST=<step>,<step>,... (dev only): create:coop | create:vs | join:<CODE> | join:@<file> | ready | start | chat:<text> |
   kick:<seat> | ban:<seat> | seats:<n> | set:<index>=<value> | save:<0|1> | leave | mute:<seat> | wait:<frames>. One step at a time, each once the previous
   one has landed; SM64DS_LOBBY_TEST_CODEFILE=<file>: the room code is written there once the room is made (a second console's join:@).
   SM64DS_LOBBY_TEST_SEATS=<n> (default 2): "start" waits for n seats, all ready. Lines "[lobbytest] ..." say what happened. */
void test_tick()
{
    static std::vector<std::string> steps;
    static size_t at = 0;
    static int wait = 0, inited = 0;
    static bool code_written = false;
    if (!inited) {
        inited = 1;
        const char *e = std::getenv("SM64DS_LOBBY_TEST");
        if (!e || !*e) return;
        std::string s = e;
        for (size_t p = 0;;) {
            size_t c = s.find(',', p);
            steps.push_back(s.substr(p, c == std::string::npos ? std::string::npos : c - p));
            if (c == std::string::npos) break;
            p = c + 1;
        }
        std::fprintf(stderr, "[lobbytest] %d step(s): %s\n", (int)steps.size(), e);
    }
    if (steps.empty()) return;
    const LobbyState *st = lobby_state();
    if (!code_written && st->phase == LOBBY_IN_ROOM && st->code[0]) {
        code_written = true;
        if (const char *cf = std::getenv("SM64DS_LOBBY_TEST_CODEFILE")) {
            const std::string tmp = std::string(cf) + ".tmp";
            if (FILE *f = std::fopen(tmp.c_str(), "wb")) { std::fputs(st->code, f); std::fclose(f); MoveFileExA(tmp.c_str(), cf, MOVEFILE_REPLACE_EXISTING); }
        }
        std::fprintf(stderr, "[lobbytest] in room %s, seat count %d\n", st->code, st->n_seats);
    }
    static int last_seats = -1, last_ready = -1, last_chat = 0;
    {
        int rd = 0;
        for (int i = 0; i < st->n_seats; ++i) rd += st->seats[i].ready;
        if (st->n_seats != last_seats || rd != last_ready) {
            std::fprintf(stderr, "[lobbytest] seats %d ready %d phase %d host %d\n", st->n_seats, rd, (int)st->phase, st->am_host);
            last_seats = st->n_seats; last_ready = rd;
        }
        for (int i = last_chat; i < st->n_chat; ++i) std::fprintf(stderr, "[lobbytest] chat: %s\n", st->chat[i]);
        last_chat = st->n_chat;
        if (st->error[0]) {
            static std::string shown;
            if (shown != st->error) { shown = st->error; std::fprintf(stderr, "[lobbytest] error: %s\n", st->error); }
        }
    }
    if (wait > 0) { --wait; return; }
    if (at >= steps.size()) return;
    const std::string s = steps[at];
    const bool in_room = st->phase == LOBBY_IN_ROOM;
    bool done = false;
    if (s == "create:coop" || s == "create:vs") { lobby_create(s == "create:vs" ? LOBBY_VS : LOBBY_COOP); done = true; wait = 30; }
    else if (!s.compare(0, 5, "join:")) {
        std::string code = s.substr(5);
        if (!code.empty() && code[0] == '@') {
            std::string t;
            if (!read_file(code.substr(1), &t) || t.size() < 6) return;
            code = t.substr(0, 6);
        }
        lobby_join(code.c_str());
        done = true;
        wait = 30;
    } else if (s == "fail") { if (st->phase == LOBBY_FAILED) done = true; }
    else if (!s.compare(0, 5, "wait:")) { wait = std::atoi(s.c_str() + 5); done = true; }
    else if (!s.compare(0, 5, "name:")) { lobby_set_server_name(s.substr(5).c_str()); done = true; }
    else if (!in_room) return;
    else if (s == "ready") { lobby_set_ready(1); done = true; }
    else if (s == "start") {
        int need = 2;
        if (const char *n = std::getenv("SM64DS_LOBBY_TEST_SEATS")) need = std::atoi(n);
        int rd = 0;
        for (int i = 0; i < st->n_seats; ++i) rd += st->seats[i].ready;
        if (st->n_seats >= need && rd == st->n_seats) { lobby_start(); done = true; }
    } else if (!s.compare(0, 5, "chat:")) { lobby_send_chat(s.substr(5).c_str()); done = true; }
    else if (!s.compare(0, 5, "kick:")) { lobby_kick(std::atoi(s.c_str() + 5)); done = true; }
    else if (!s.compare(0, 4, "ban:")) { lobby_ban(std::atoi(s.c_str() + 4)); done = true; }
    else if (!s.compare(0, 5, "mute:")) { lobby_mute(std::atoi(s.c_str() + 5), 1); done = true; }
    else if (!s.compare(0, 4, "set:")) {
        const size_t eq = s.find('=');
        if (eq != std::string::npos) lobby_set_setting(std::atoi(s.c_str() + 4), std::atoi(s.c_str() + eq + 1));
        done = true;
    } else if (!s.compare(0, 5, "save:")) { lobby_set_coop_save(std::atoi(s.c_str() + 5)); done = true; }
    else if (s == "leave") { lobby_leave(); done = true; }
    else if (s == "seats2") { if (st->n_seats >= 2) done = true; }
    else if (s == "seats1") { if (st->n_seats == 1) done = true; }
    else if (!s.compare(0, 6, "seats:")) { if (st->n_seats >= std::atoi(s.c_str() + 6)) done = true; }
    else { std::fprintf(stderr, "[lobbytest] unknown step '%s', skipped\n", s.c_str()); done = true; }
    if (done) {
        std::fprintf(stderr, "[lobbytest] step %d '%s' done at title frame %u\n", (int)at, s.c_str(), g_title_frames);
        ++at;
        if (!wait) wait = 10;
    }
}
#endif

void frame()
{
    if (g_prev_hook) g_prev_hook();
    resume_from_env();
    if (port_scene_live_id() == 1) ++g_title_frames;
    if (!g_init) {
#ifndef PORT_ROM_CLEAN
        if (port_scene_live_id() == 1 && g_title_frames > 60) test_tick();
#endif
        return;
    }
    static unsigned f = 0;
    if (++f % 30 == 0) result_tick();
#ifndef PORT_ROM_CLEAN
    if (port_scene_live_id() == 1 && g_title_frames > 60) test_tick();
#endif
    /* a start the server delivered, at the title only (where the hand-off can take it) */
    if (port_scene_live_id() != 1 || port_handoff_busy()) return;
    JV plan;
    LobbyKind kind;
    bool host = false;
    std::string match;
    if (launch_due(&plan, &kind, &host, &match)) launch(plan, kind, host, match);
}

struct Seat {
    Seat()
    {
        g_prev_hook = port_sdat_frame_hook;
        port_sdat_frame_hook = frame;
    }
} g_seat;

/* ---- the snapshot ----------------------------------------------------------------------------------------------------------------- */
LobbyState g_snap;

void copy(char *dst, size_t cap, const std::string &s)
{
    std::snprintf(dst, cap, "%s", s.c_str());
}

void snapshot_locked()
{
    LobbyState &s = g_snap;
    s.phase = g.phase;
    s.kind = g.kind;
    s.am_host = !g.room.empty() && g.you == g.host;
    copy(s.code, sizeof s.code, g.room);
    copy(s.server_name, sizeof s.server_name, g.server_name);
    copy(s.error, sizeof s.error, g.error);
    copy(s.error_head, sizeof s.error_head, g.phase == LOBBY_FAILED ? g.error_head : std::string());
    s.max_seats = g.max_seats;
    s.n_seats = 0;
    for (const Member &m : g.members) {
        if (s.n_seats >= 16) break;
        LobbySeat &z = s.seats[s.n_seats++];
        copy(z.name, sizeof z.name, m.name);
        /* no colour picked = the ROM's own Yoshi: green body, red shoes */
        z.body_rgb = rgb6(m.color, 0x40B840);
        z.shoe_rgb = rgb6(m.shoes, 0xD83020);
        z.ready = m.pre_ok;
        z.is_host = m.seat == g.host;
        z.is_me = m.seat == g.you;
        z.muted = g.muted.count(m.seat) ? g.muted[m.seat] : 0;
        z.member = m.seat;
    }
    const bool frozen = g.started || g.state != "lobby";
    if (g.kind == LOBBY_COOP) {
        s.n_settings = kNCoop;
        for (int i = 0; i < kNCoop; ++i) {
            LobbySetting &t = s.settings[i];
            t.key = kCoopRules[i].key; t.label = kCoopRules[i].label;
            t.value = g.coop[i]; t.min = 0; t.max = 1; t.choices = kOffOn; t.n_choices = 2;
        }
    } else {
        s.n_settings = VS_N;
        LobbySetting *t = s.settings;
        /* 1.0: Arena referees Clock and Stars only, so Luigi Infection is offered only to show a launcher room that already has it */
        t[VS_MODE] = LobbySetting{"mode", "Game mode", g.luigi ? 2 : g.win_mode == "stars" ? 1 : 0, 0, g.luigi ? 2 : 1, kVsModes, g.luigi ? 3 : 2};
        t[VS_STARS] = LobbySetting{"stars", "Stars to win", g.star_target > 0 ? g.star_target : 3, 1, 5, 0, 0};
        t[VS_ARENA] = LobbySetting{"map", "Arena", g.map, 0, 3, kArenas, 4};
        int pmax = g.dial_max >= 2 ? g.dial_max : (g.max_seats >= 2 ? g.max_seats : 4);
        if (pmax > g.arena_max) pmax = g.arena_max;   /* 1.0: Arena plays 2..arena_max players */
        const int pval = g.match_players >= 2 ? g.match_players : 2;
        t[VS_PLAYERS] = LobbySetting{"players", "Players", pval, 2, pval > pmax ? pval : pmax, 0, 0};
        /* 1.0: Classic (lockstep) needs the game to restart itself, a path no row has run yet, so the in-game lobby offers Arena only */
        t[VS_NETMODE] = LobbySetting{"netmode", "Netcode", 0, 0, 0, kNetmodes, 2};
    }
    (void)frozen;
    s.coop_save_continue = g.coop_continue;
    s.n_chat = 0;
    for (const std::string &c : g.chat) copy(s.chat[s.n_chat++], sizeof s.chat[0], c);
}

/* the VS params the page edits, sent whole the way the launcher's Game settings dialog sends them */
void send_params_locked()
{
    int mode = g.luigi ? 2 : g.win_mode == "stars" ? 1 : 0;
    std::string b = head() + ",\"map\":" + std::to_string(g.map) + ",\"win_mode\":" + (mode == 1 ? "\"stars\"" : "\"time\"");
    if (mode == 1) b += ",\"star_target\":" + std::to_string(g.star_target > 0 ? g.star_target : 3);
    if (g.match_players >= 2) b += ",\"match_players\":" + std::to_string(g.match_players);
    b += ",\"luigi\":" + std::string(mode == 2 ? "1" : "0");
    if (!g.netmode.empty()) b += ",\"netmode\":" + jesc(g.netmode == "lockstep" ? "lockstep" : "arena");
    enqueue("params", b + "}");
}

bool host_editable_locked(const char *what)
{
    if (g.room.empty()) { g.error = "You are not in a room."; return false; }
    if (g.you != g.host) { g.error = "Only the host can do that."; return false; }
    if (g.started || g.state != "lobby") { g.error = "The room has started, so its settings are locked."; return false; }
    (void)what;
    return true;
}

void begin_session_locked(LobbyKind kind)
{
    ++g.gen;
    g.base = lobby_base();
    g.game = game_build_id();
    identity(&g.nick, &g.color, &g.shoes);
    g.kind = kind;
    g.room.clear();
    g.token.clear();
    g.cursor = 0;
    g.phase = LOBBY_BUSY;
    g.error.clear();
    g.error_head.clear();
    g.chat.clear();
    g.members.clear();
    g.muted.clear();
    g.plan_set = false;
    g.launched_no = 0;
    g.launched_match.clear();
    g.match.clear();
    g.readied.clear();
    g.state = "lobby";
    g.started = false;
    g.start_wanted = false;
    for (int i = 0; i < kNCoop; ++i) g.coop[i] = kCoopRules[i].def;
}

std::string colours_json_locked()
{
    if (g.color.size() == 6 && g.shoes.size() == 6) return ",\"color\":" + jesc(g.color) + ",\"shoes\":" + jesc(g.shoes);
    return std::string();
}

}  // namespace

/* ================================================================================================================================ */
/* The contract                                                                                                                     */
/* ================================================================================================================================ */
const LobbyState *lobby_state(void)
{
    if (!g_init) { std::memset(&g_snap, 0, sizeof g_snap); return &g_snap; }
    Lock l;
    snapshot_locked();
    return &g_snap;
}

void lobby_tick(void)
{
    if (!g_init) return;
    Lock l;
    snapshot_locked();
}

void lobby_create(LobbyKind kind)
{
    init_once();
    start_threads();
    Lock l;
    if (!g.room.empty()) { g.error = "Leave this room first."; return; }
    begin_session_locked(kind);
    std::string b = "{\"v\":7,\"install\":" + jesc(install_id()) + ",\"nick\":" + jesc(g.nick) + ",\"pre_ok\":false,\"game\":" +
                    jesc(g.game) + ",\"session_type\":" +
                    (kind == LOBBY_VS ? "\"vs\"" : "\"coop\"");
    if (kind == LOBBY_COOP) {
        g.coop_continue = coop_saves().empty() ? 0 : 1;
        b += ",\"save_name\":" + jesc(coop_save_display(g.coop_continue != 0, g.server_name));
        b += ",\"settings\":{";
        for (int i = 0; i < kNCoop; ++i) b += std::string(i ? "," : "") + "\"" + kCoopRules[i].key + "\":" + std::to_string(kCoopRules[i].def);
        b += "}";
    }
    b += colours_json_locked() + "}";
    if (kind == LOBBY_VS) g.netmode = "arena";
    say("create %s room on %s as \"%s\" (game %s)", kind == LOBBY_VS ? "vs" : "coop", g.base.c_str(), g.nick.c_str(), g.game.c_str());
    enqueue("create", b, 1);
}

void lobby_join(const char *code)
{
    init_once();
    start_threads();
    Lock l;
    if (!g.room.empty()) { g.error = "Leave this room first."; return; }
    std::string c;
    for (const char *p = code ? code : ""; *p; ++p)
        if (*p != ' ' && *p != '-') c += (char)std::toupper((unsigned char)*p);
    bool ok = c.size() == 6;
    for (char ch : c) ok = ok && std::strchr("ABCDEFGHJKMNPQRSTUVWXYZ23456789", ch);
    begin_session_locked(LOBBY_COOP);
    if (!ok) {
        g.phase = LOBBY_FAILED;
        g.error = "That is not a valid room code.";
        return;
    }
    std::string b = "{\"v\":7,\"install\":" + jesc(install_id()) + ",\"room\":" + jesc(c) + ",\"nick\":" + jesc(g.nick) +
                    ",\"pre_ok\":false,\"game\":" + jesc(g.game) + colours_json_locked() + "}";
    say("join %s on %s as \"%s\" (game %s)", c.c_str(), g.base.c_str(), g.nick.c_str(), g.game.c_str());
    enqueue("join", b, 2);
}

void lobby_leave(void)
{
    if (!g_init) return;
    Lock l;
    if (g.room.empty()) { g.phase = LOBBY_NONE; g.error.clear(); return; }
    enqueue("leave", head() + "}", 3);
    leave_room_locked(0);
}

void lobby_set_ready(int ready)
{
    if (!g_init) return;
    Lock l;
    if (g.room.empty()) return;
    for (Member &m : g.members)
        if (m.seat == g.you) m.pre_ok = ready != 0;   /* shown at once; the next poll confirms */
    enqueue("preflight", head() + ",\"pre_ok\":" + (ready ? "true" : "false") + "}");
}

void lobby_start(void)
{
    if (!g_init) return;
    Lock l;
    if (g.room.empty()) return;
    if (g.you != g.host) { g.error = "Only the host can do that."; return; }
    for (const Member &m : g.members)
        if (!m.pre_ok) { g.error = "Someone is still getting ready."; return; }
    g.error.clear();
    g.start_wanted = true;
    g.phase = LOBBY_STARTING;
    enqueue("start", head() + "}");
}

void lobby_set_setting(int index, int value)
{
    if (!g_init) return;
    Lock l;
    if (!host_editable_locked("setting")) return;
    if (g.kind == LOBBY_COOP) {
        if (index < 0 || index >= kNCoop) return;
        g.coop[index] = value ? 1 : 0;
        enqueue("settings", head() + ",\"settings\":{\"" + kCoopRules[index].key + "\":" + (value ? "1" : "0") + "}}");
        return;
    }
    switch (index) {
    case VS_MODE: if (value < 0 || value > 1) return; g.luigi = 0; g.win_mode = value == 1 ? "stars" : "time"; if (value == 1 && g.star_target <= 0) g.star_target = 3; break;
    case VS_STARS: if (value < 1 || value > 5) return; g.star_target = value; break;
    case VS_ARENA: if (value < 0 || value > 3) return; g.map = value; break;
    case VS_PLAYERS: if (value < 2 || value > g.arena_max) return; g.match_players = value; break;
    case VS_NETMODE: g.netmode = "arena"; (void)value; break;   /* pinned to Arena in 1.0 (see the snapshot) */
    default: return;
    }
    send_params_locked();
}

void lobby_set_coop_save(int continue_save)
{
    if (!g_init) return;
    Lock l;
    if (continue_save && coop_saves().empty()) { g.error = "There is no coop save on this PC yet."; g.coop_continue = 0; return; }
    g.coop_continue = continue_save ? 1 : 0;
}

void lobby_set_server_name(const char *name)
{
    init_once();
    Lock l;
    g.server_name = safe(name ? name : "", 47);
}

void lobby_send_chat(const char *text)
{
    if (!g_init) return;
    Lock l;
    if (g.room.empty()) return;
    const std::string t = safe(text ? text : "", 200);
    if (t.empty()) return;
    enqueue("chat", head() + ",\"text\":" + jesc(t) + "}");
}

void lobby_kick(int seat)
{
    if (!g_init) return;
    Lock l;
    if (g.room.empty() || seat < 0 || seat >= (int)g.members.size()) return;
    if (g.you != g.host) { g.error = "Only the host can do that."; return; }
    const int s = g.members[(size_t)seat].seat;
    if (s == g.you) return;
    enqueue("kick", head() + ",\"seat\":" + std::to_string(s) + "}");
}

void lobby_ban(int seat)
{
    if (!g_init) return;
    Lock l;
    if (g.room.empty() || seat < 0 || seat >= g_snap.n_seats) return;
    if (g.you != g.host) { g.error = "Only the host can do that."; return; }
    /* the row the page drew (the last snapshot), by the server's seat number: a roster that moved since then must not shift a ban
       onto the player now in that row */
    const int s = g_snap.seats[seat].member;
    bool here = false;
    for (const Member &m : g.members) here = here || m.seat == s;
    if (!here || s == g.you) { say("ban not sent: that player already left"); return; }
    if (!g.srv_ban) {
        g.error = "This lobby server cannot ban players yet. Kick still works.";
        say("ban not sent: %s", g.error.c_str());
        return;
    }
    say("ban seat %d", s);
    enqueue("ban", "{\"v\":7,\"room\":" + jesc(g.room) + ",\"token\":" + jesc(g.token) + ",\"seat\":" + std::to_string(s) + "}");
}

void lobby_mute(int seat, int muted)
{
    if (!g_init) return;
    Lock l;
    if (seat < 0 || seat >= (int)g.members.size()) return;
    g.muted[g.members[(size_t)seat].seat] = muted ? 1 : 0;
}
