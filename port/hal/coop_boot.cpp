// COOP BOOT: boot into the host's file together, the opening for everyone, the
// host leaving. Run mp1 wave 56, lane COOPBOOT1. Contract: hal/coop_boot.h.
// Wire: hal/coop_boot_wire.h. Inert unless SM64DS_COOP=1.
//
// WHERE THE BARRIERS SIT, AND WHY NOT AT THE TITLE.
// The title scene loop has no hold seam (and hal/title_entry.cpp is not this
// lane's), and nothing pumps the hub there. So the start barrier sits one step
// later, at the FIRST FRAME OF THE FILE'S LEVEL: by then every console has
// finished its own boot (title, file press, level load; the slow, uneven
// part). That is a better sync point than the press: GO releases the first
// tick of the grounds (or of the opening) on every console at once, whatever
// each one's load time was. The hold skips the game tick of the level loop
// (port_coop_boot_frame returns 1), so there is no input and no motion.
//
// The opening's end barrier sits at the opening's closing level load, which is
// the moment hal/comms_sync.cpp's port_adventure_hub_opening_held() goes from
// 1 to 0 (hal/level_change.cpp calls port_adventure_hub_level_seat() there).
// A console that sees it says ARRIVED and holds its first grounds tick until
// the host's CLOSE. Read-only use of that function; comms_sync.cpp is not
// touched.
//
// While a console is booting (no frame hook runs for a few seconds while the
// ROM loads) nobody pumps the hub. The host therefore waits a short while at
// its gate, answering the guests' save requests, before it goes to load.

#include "coop_boot.h"
#include "coop_boot_wire.h"
#include "coop_save.h"
#include "coop_wire.h"
#include "comms_hub.h"
#include "hub_presence.h"
#include "coop_world.h"
#include "ui_text.h"

#include <windows.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

extern "C" {
extern unsigned char data_0209caa0[];
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern unsigned char data_0209f21c;
extern int data_0209f2f8;
int port_adventure_hub_opening_held(void);
int port_adventure_ghost_drawpos(int slot, int out[3]);
int port_adventure_ghost_seat(int slot);
short Vec3_HorzAngle(const void *a, const void *b);
extern unsigned char data_0209f498[];   // Stage's per-player stick record, 0x18 bytes each (probe only)
extern unsigned char data_020a0de8[];   // touch records, 4 bytes each (probe only)
extern int data_020a0e58[];             // pad mirror (probe only)
extern unsigned char data_020a0e40;     // the stick slot Player::St_Wait_Main reads (probe only)
void *_ZN8dActor_c4NextEPKS_(const void *prev);
const void *port_level_fb_ptr = 0;   // set by tests/walk_window.cpp's level loop; the proof pictures (SM64DS_COOPSPAWN_SHOT) read it
}
namespace ntr { struct Framebuffer; bool ppu_write_bmp(const char *path, const Framebuffer &fb); }

namespace port {
namespace {

enum : unsigned { kBeatGatherMs = 250, kBeatMs = 500 };
enum { kWarmFrames = 4 };

struct Ctl {
    bool init = false;
    bool on = false;
    bool host = false;
    bool released = false;
    bool at_first_frame = false;   // this console reached the file's first level frame
    bool late_at_first = false;    // ...and the GO had already been given then (COOPSPAWN9: a late joiner)
    bool holding = false;
    bool exiting = false;
    // host
    unsigned ready_mask = 0;       // guests that said READY
    unsigned arrived_mask = 0;
    bool any_arrived = false;
    ULONGLONG host_ready_ms = 0;
    ULONGLONG first_arrive_ms = 0;
    bool close_sent = false;
    int phase = kCoopPhaseGather;
    unsigned req_seen = 0;         // seats that asked for the save (SYNB 0x20)
    ULONGLONG last_req_ms = 0;
    // guest
    int host_seat = -1;
    bool state_heard = false;
    CoopBootState hs = {0, 0, 0};
    bool host_left_msg = false;
    ULONGLONG hold_since = 0;
    // both
    bool intro_seen = false;
    bool arrived = false;
    int warm = 0;   // frames ticked since the closing load, before the end barrier freezes the tick
    bool closed = false;
    unsigned seq = 0;
    ULONGLONG last_beat = 0, last_ready = 0;
    ULONGLONG t_boot = 0;
    CoopBootStats st;
    // SEAMCOOP1: the room ends in place once a title exists in this process
    bool title_seen = false;       // a title (or a level) has run since this process started
    bool inplace = false;          // this room was booted in place (a launcher hand-off at the title), its gates are stepped per frame
    bool said_end = false, said_release = false;
    int ticked = 0;
    ULONGLONG gone_since = 0;
    // KNOWNNET1: a launcher-started guest waiting for the host's opening (before the ROM and the window): the room is dropped there
    bool opening_wait = false;
    bool dropped = false;
} C;
bool g_ranges_registered = false;   // SEAMCOOP1: the two SYNB ranges are registered once per process
bool g_host_atexit = false;         // and host_atexit once

ULONGLONG tick() { return GetTickCount64(); }

// Wall clock in unix milliseconds: comparable between the consoles of a proof
// run on one machine; printed on every barrier line.
unsigned long long unix_ms() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    unsigned long long t = ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    return (t - 116444736000000000ULL) / 10000ULL;
}

bool env_on(const char *n) {
    const char *e = std::getenv(n);
    return e && *e && std::strcmp(e, "0") != 0;
}
int env_int(const char *n, int dflt) {
    const char *e = std::getenv(n);
    return (e && *e) ? std::atoi(e) : dflt;
}

#define LOG(...) do { std::fprintf(stderr, "[coopboot] t=%llu ", unix_ms()); \
                      std::fprintf(stderr, __VA_ARGS__); std::fputc('\n', stderr); \
                      std::fflush(stderr); } while (0)

int my_seat() { return comms_hub_my_seat(); }
unsigned my_bit() { int s = my_seat(); return (s >= 0 && s < 4) ? (1u << s) : 0u; }
unsigned present_others() { return comms_hub_seat_mask() & 0xFu; }

bool send_sub(int sub, const CoopBootState *st) {
    unsigned char dg[32];
    int n = coop_boot_encode(sub, ++C.seq, st, dg, sizeof dg);
    return n > 0 && comms_hub_send(dg, n);
}

void send_state() {
    CoopBootState s = {C.released ? 1 : 0, C.phase, C.closed ? 1 : 0};
    if (send_sub(kCoopBootSubState, &s)) ++C.st.state_sent;
    C.last_beat = tick();
}

// ---- the popup line and the exit -------------------------------------------
// EVERY user32 / gdi32 CALL HERE IS BOUND AT RUN TIME. A new static DLL import
// anywhere in walk_window can map over the DS fixed addresses before the TLS
// callback claims them and break every run (memory: static import trap), so
// nothing in this file adds an import-table entry.
struct Gui {
    HMODULE u = nullptr, g = nullptr;
    HWND (WINAPI *CreateWindowExA_)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID) = nullptr;
    BOOL (WINAPI *PeekMessageA_)(LPMSG, HWND, UINT, UINT, UINT) = nullptr;
    BOOL (WINAPI *TranslateMessage_)(const MSG *) = nullptr;
    LRESULT (WINAPI *DispatchMessageA_)(const MSG *) = nullptr;
    BOOL (WINAPI *ShowWindow_)(HWND, int) = nullptr;
    BOOL (WINAPI *UpdateWindow_)(HWND) = nullptr;
    int (WINAPI *GetSystemMetrics_)(int) = nullptr;
    LRESULT (WINAPI *SendMessageA_)(HWND, UINT, WPARAM, LPARAM) = nullptr;
    HDC (WINAPI *GetDC_)(HWND) = nullptr;
    int (WINAPI *ReleaseDC_)(HWND, HDC) = nullptr;
    BOOL (WINAPI *PrintWindow_)(HWND, HDC, UINT) = nullptr;
    BOOL (WINAPI *GetWindowRect_)(HWND, LPRECT) = nullptr;
    HFONT (WINAPI *CreateFontA_)(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCSTR) = nullptr;
    HDC (WINAPI *CreateCompatibleDC_)(HDC) = nullptr;
    HBITMAP (WINAPI *CreateDIBSection_)(HDC, const BITMAPINFO *, UINT, void **, HANDLE, DWORD) = nullptr;
    HGDIOBJ (WINAPI *SelectObject_)(HDC, HGDIOBJ) = nullptr;
    BOOL (WINAPI *DeleteObject_)(HGDIOBJ) = nullptr;
    BOOL (WINAPI *DeleteDC_)(HDC) = nullptr;
    bool ok = false;
};
Gui G;

bool gui_load() {
    if (G.ok) return true;
    G.u = LoadLibraryA("user32.dll");
    G.g = LoadLibraryA("gdi32.dll");
    if (!G.u || !G.g) return false;
#define BIND(m, f) G.f##_ = reinterpret_cast<decltype(G.f##_)>(GetProcAddress(m, #f)); if (!G.f##_) return false
    BIND(G.u, CreateWindowExA); BIND(G.u, PeekMessageA); BIND(G.u, TranslateMessage);
    BIND(G.u, DispatchMessageA); BIND(G.u, ShowWindow); BIND(G.u, UpdateWindow);
    BIND(G.u, GetSystemMetrics); BIND(G.u, SendMessageA); BIND(G.u, GetDC);
    BIND(G.u, ReleaseDC); BIND(G.u, PrintWindow); BIND(G.u, GetWindowRect);
    BIND(G.g, CreateFontA); BIND(G.g, CreateCompatibleDC); BIND(G.g, CreateDIBSection);
    BIND(G.g, SelectObject); BIND(G.g, DeleteObject); BIND(G.g, DeleteDC);
#undef BIND
    G.ok = true;
    return true;
}

bool save_window_bmp(HWND w, const char *path) {
    RECT r;
    if (!G.GetWindowRect_(w, &r)) return false;
    const int W = r.right - r.left, H = r.bottom - r.top;
    HDC sdc = G.GetDC_(nullptr);
    HDC mdc = G.CreateCompatibleDC_(sdc);
    BITMAPINFO bi;
    std::memset(&bi, 0, sizeof bi);
    bi.bmiHeader.biSize = sizeof bi.bmiHeader;
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    void *bits = nullptr;
    HBITMAP bm = G.CreateDIBSection_(sdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    bool ok = false;
    if (bm && bits) {
        HGDIOBJ old = G.SelectObject_(mdc, bm);
        ok = G.PrintWindow_(w, mdc, 2 /*PW_RENDERFULLCONTENT*/) != 0;
        G.SelectObject_(mdc, old);
        if (ok) {
            BITMAPFILEHEADER fh;
            std::memset(&fh, 0, sizeof fh);
            fh.bfType = 0x4D42;
            fh.bfOffBits = sizeof fh + sizeof bi.bmiHeader;
            fh.bfSize = fh.bfOffBits + (DWORD)(W * H * 4);
            if (std::FILE *f = std::fopen(path, "wb")) {
                std::fwrite(&fh, sizeof fh, 1, f);
                std::fwrite(&bi.bmiHeader, sizeof bi.bmiHeader, 1, f);
                std::fwrite(bits, 4, (size_t)W * H, f);
                std::fclose(f);
            } else ok = false;
        }
    }
    if (bm) G.DeleteObject_(bm);
    G.DeleteDC_(mdc);
    G.ReleaseDC_(nullptr, sdc);
    return ok;
}

// One plain line on top of everything for 3 s, then the process ends. The
// proofs read it back with SM64DS_COOPBOOT_POPUP_SHOT=<bmp path>.
// SEAMCOOP1: only for a room that ends before any title of this process exists
// (the boot gate in main): the launcher contract for boot failures stands.
// Every later end of a room is leave() below, in place.
[[noreturn]] void leave_exit(int code, const char *line) {
    if (C.exiting) ExitProcess((UINT)code);
    C.exiting = true;
    LOG("%s (exit code %d)", line, code);
    const bool gui = gui_load();
    HWND w = nullptr;
    if (gui) {
        const int W = 460, H = 64;
        int x = (G.GetSystemMetrics_(SM_CXSCREEN) - W) / 2;
        int y = (G.GetSystemMetrics_(SM_CYSCREEN) - H) / 2;
        w = G.CreateWindowExA_(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, "STATIC", line,
                               WS_POPUP | WS_VISIBLE | WS_BORDER | SS_CENTER | SS_CENTERIMAGE,
                               x, y, W, H, nullptr, nullptr, GetModuleHandleA(nullptr), nullptr);
        if (w) {
            HFONT f = G.CreateFontA_(26, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, "Segoe UI");
            if (f) G.SendMessageA_(w, WM_SETFONT, (WPARAM)f, TRUE);
            G.ShowWindow_(w, SW_SHOWNOACTIVATE);
            G.UpdateWindow_(w);
        }
    }
    const ULONGLONG t0 = tick();
    bool shot = false;
    while (tick() - t0 < 3000) {
        if (gui) {
            MSG m;
            while (G.PeekMessageA_(&m, nullptr, 0, 0, PM_REMOVE)) { G.TranslateMessage_(&m); G.DispatchMessageA_(&m); }
            if (w && !shot && tick() - t0 > 400) {
                shot = true;
                if (const char *s = std::getenv("SM64DS_COOPBOOT_POPUP_SHOT")) {
                    bool ok = save_window_bmp(w, s);
                    LOG("popup picture %s", ok ? "saved" : "FAILED");
                }
            }
        }
        Sleep(15);
    }
    std::fflush(stderr);
    ExitProcess((UINT)code);
}

void leave(int code, const char *line);
bool coop_leave_holds();
void leave_host_left() { leave(kCoopExitHostLeft, "The host left. Room closed."); }

// ---- receive -----------------------------------------------------------------
void drop_role(int seat, int sub) {
    ++C.st.dropped_role;
    static int said;
    if (said < 24) { ++said; LOG("DROP seat %d: SYNB 0x%02x is not for this role", seat, sub); }
}

void handle_mine(int seat, const unsigned char *d, int len) {
    const int sub = coop_boot_validate(d, len);
    if (sub < 0) { ++C.st.dropped_bad; LOG("DROP seat %d: malformed SYNB 0x%02x len %d", seat, len > 6 ? d[6] : 0, len); return; }
    if (seat < 0 || seat > 3 || seat == my_seat()) { ++C.st.dropped_bad; return; }
    switch (sub) {
    case kCoopBootSubReady:
        if (!C.host) { drop_role(seat, sub); return; }
        ++C.st.ready_recv;
        if (!(C.ready_mask & (1u << seat))) LOG("READY from seat %d", seat);
        C.ready_mask |= 1u << seat;
        break;
    case kCoopBootSubArrived:
        if (!C.host) { drop_role(seat, sub); return; }
        ++C.st.arrived_recv;
        if (!(C.arrived_mask & (1u << seat))) LOG("ARRIVED from seat %d", seat);
        C.arrived_mask |= 1u << seat;
        if (!C.any_arrived) { C.any_arrived = true; C.first_arrive_ms = tick(); }
        break;
    case kCoopBootSubState: {
        if (C.host) { drop_role(seat, sub); return; }
        if (C.host_seat < 0) { C.host_seat = seat; LOG("host is seat %d", seat); }
        if (seat != C.host_seat) { drop_role(seat, sub); return; }
        CoopBootState s;
        if (!coop_boot_decode_state(d, len, &s)) { ++C.st.dropped_bad; return; }
        ++C.st.state_recv;
        const bool was_go = C.released;
        C.state_heard = true;
        C.hs = s;
        if (s.closed && !C.closed) { C.closed = true; LOG("CLOSE (state beat)"); }
        if (s.go && !was_go && C.at_first_frame) {
            C.released = true;
            LOG("GO heard");
        } else if (s.go && !was_go) {
            // a late joiner: the start already happened; no barrier for it
            C.released = true;
            LOG("GO already given (late joiner): no start barrier");
        }
        break;
    }
    case kCoopBootSubClose:
        if (C.host || seat != C.host_seat) { drop_role(seat, sub); return; }
        ++C.st.close_recv;
        if (!C.closed) LOG("CLOSE heard");
        C.closed = true;
        break;
    case kCoopBootSubHostLeft:
        if (C.host || seat != C.host_seat) { drop_role(seat, sub); return; }
        ++C.st.hostleft_recv;
        C.host_left_msg = true;
        LOG("HOST_LEFT heard");
        break;
    default: break;
    }
}

// SYNB ARRIVES IN TWO WAYS. COOPWORLD1 (hal/coop_world.cpp) owns the one hub
// handler for the kind and fans a datagram out by sub-type to whoever called
// coop_ctl_register; it installs that handler at the first level frame. Until
// then (the gate and the title) THIS file holds a stand-in handler that does
// the same fan-out for the two ranges it needs, so the save fetch and the
// barriers work before any level exists. Both paths end in the same two
// functions below.
void rebuild_and_run(int seat, int sub, const unsigned char *payload, int plen, bool save) {
    // the sub-type handlers want the whole datagram: rebuild the 12-byte head
    unsigned char dg[kHubCoopMaxDatagram];
    if (plen < 0 || plen + kHubCoopHeaderBytes > (int)sizeof dg) { ++C.st.dropped_bad; return; }
    HubCoopHead h;
    std::memset(&h, 0, sizeof h);
    h.tag = kHubTagCoopCtl; h.version = 1; h.seat = (uint8_t)seat; h.subtype = (uint8_t)sub;
    h.payload_len = (uint16_t)plen;
    std::memcpy(dg, &h, sizeof h);
    if (plen) std::memcpy(dg + kHubCoopHeaderBytes, payload, (size_t)plen);
    const int len = kHubCoopHeaderBytes + plen;
    if (save) {
        if (sub == 0x20 && seat >= 0 && seat < 4) { C.req_seen |= 1u << seat; C.last_req_ms = tick(); }
        coop_save_handle(seat, dg, len);
    } else {
        handle_mine(seat, dg, len);
    }
}

void sub_save(int seat, int sub, const unsigned char *p, int plen, void *) { rebuild_and_run(seat, sub, p, plen, true); }
void sub_boot(int seat, int sub, const unsigned char *p, int plen, void *) { rebuild_and_run(seat, sub, p, plen, false); }

void gate_on_ctl(int seat, const unsigned char *d, int len, void *) {
    if (!d || len < kHubCoopHeaderBytes) { ++C.st.dropped_bad; return; }
    const int sub = d[6];
    const unsigned char *p = d + kHubCoopHeaderBytes;
    const int plen = len - kHubCoopHeaderBytes;
    if (sub >= 0x20 && sub <= 0x2f) rebuild_and_run(seat, sub, p, plen, true);
    else if (coop_boot_sub_mine(sub)) rebuild_and_run(seat, sub, p, plen, false);
    else ++C.st.dropped_world;   // a world sub-type before any level group exists
}

// ---- host exit ---------------------------------------------------------------
void host_atexit() {
    if (!C.host || C.exiting || comms_hub_state() != kHubLive) return;
    for (int i = 0; i < 3; ++i) {
        if (send_sub(kCoopBootSubHostLeft, nullptr)) ++C.st.hostleft_sent;
        Sleep(40);
    }
    LOG("host exiting: HOST_LEFT sent %u times", C.st.hostleft_sent);
}

// ---- proof knobs ------------------------------------------------------------
// SM64DS_COOPBOOT_FORGE=<mode>[,<mode>...]: once, after the release, send one
// hand-made datagram per mode. The receivers must drop it (and say so).
void run_forge() {
    static bool done;
    if (done) return;
    const char *e = std::getenv("SM64DS_COOPBOOT_FORGE");
    done = true;
    if (!e || !*e) return;
    char buf[160];
    std::snprintf(buf, sizeof buf, "%s", e);
    for (char *m = std::strtok(buf, ","); m; m = std::strtok(nullptr, ",")) {
        unsigned char dg[64];
        unsigned char pl[16] = {0};
        int plen = 4, sub = kCoopBootSubReady;
        if (!std::strcmp(m, "badlen")) plen = 8;
        else if (!std::strcmp(m, "reserved")) pl[0] = 1;
        else if (!std::strcmp(m, "state_from_guest")) { sub = kCoopBootSubState; pl[0] = 1; pl[1] = 3; pl[2] = 1; }
        else if (!std::strcmp(m, "unknown_subtype")) sub = 0x3a;
        else if (!std::strcmp(m, "hostleft_from_guest")) sub = kCoopBootSubHostLeft;
        else if (!std::strcmp(m, "ready_from_host")) sub = kCoopBootSubReady;
        else if (!std::strcmp(m, "state_pad")) { sub = kCoopBootSubState; pl[3] = 9; }
        else continue;
        int n = hub_coop_encode(kHubTagCoopCtl, sub, ++C.seq, pl, plen, dg, sizeof dg);
        bool ok = n > 0 && comms_hub_send(dg, n);
        LOG("FORGE %s sent=%d len=%d", m, ok ? 1 : 0, n);
    }
}

// ---- the per-frame engine ------------------------------------------------------
void check_host_gone() {
    if (C.host || C.exiting) return;
    if (C.host_left_msg) leave_host_left();
    if (C.host_seat >= 0 && comms_hub_state() == kHubLive) {
        HubSeatInfo si;
        ULONGLONG &gone_since = C.gone_since;
        if (comms_hub_seat_info(C.host_seat, &si) && !si.present) {
            // a console that stalled for a few seconds drops every seat for a
            // moment and gets them straight back: only a seat that stays gone counts
            if (!gone_since) gone_since = tick();
            if (tick() - gone_since >= 1500ULL) {
                LOG("the host's seat %d is gone", C.host_seat);
                leave_host_left();
            }
        } else {
            gone_since = 0;
        }
    }
}

void service() {
    comms_hub_pump();
    check_host_gone();
    const ULONGLONG now = tick();
    if (comms_hub_state() != kHubLive) {
        // a host with no live hub plays alone: no one to wait for
        if (C.host && C.at_first_frame && !C.released &&
            (comms_hub_state() != kHubPairing || now - C.host_ready_ms > 8000)) {
            C.released = true;
            LOG("GO: no live hub, playing alone");
        }
        return;
    }
    if (C.host) {
        const unsigned others = present_others();
        const unsigned beat = (!C.released && others) ? kBeatGatherMs : kBeatMs;
        if (now - C.last_beat >= beat) send_state();
        if (C.at_first_frame && !C.released) {
            if (coop_boot_start_due(others, C.ready_mask, true, (unsigned)(now - C.host_ready_ms))) {
                C.released = true;
                if (C.phase == kCoopPhaseGather) C.phase = kCoopPhasePlaying;
                LOG("GO: members present %x ready %x after %u ms%s", others, C.ready_mask,
                    (unsigned)(now - C.host_ready_ms),
                    (others & ~C.ready_mask) ? " (timed out, not everyone ready)" : "");
                for (int i = 0; i < 3; ++i) send_state();
            }
        }
        if (C.any_arrived && !C.close_sent) {
            const unsigned all = others | my_bit();
            if (coop_boot_close_due(all, C.arrived_mask | (C.arrived ? my_bit() : 0u), true,
                                    (unsigned)(now - C.first_arrive_ms))) {
                C.close_sent = true;
                C.closed = true;
                LOG("CLOSE: present %x arrived %x after %u ms%s", all, C.arrived_mask | my_bit(),
                    (unsigned)(now - C.first_arrive_ms),
                    (all & ~(C.arrived_mask | my_bit())) ? " (timed out)" : "");
                for (int i = 0; i < 3; ++i) {
                    if (send_sub(kCoopBootSubClose, nullptr)) ++C.st.close_sent;
                    send_state();
                }
            }
        }
    } else if (C.at_first_frame && !C.released && now - C.last_ready >= 250) {
        if (send_sub(kCoopBootSubReady, nullptr)) ++C.st.ready_sent;
        C.last_ready = now;
    }
}

void gate() {
    // The host's gate: stay and answer save requests until every present
    // guest has asked (and a short quiet), at most SM64DS_COOPBOOT_GATE_MS.
    const int budget = env_int("SM64DS_COOPBOOT_GATE_MS", 3000);
    const ULONGLONG t0 = tick();
    while ((int)(tick() - t0) < budget) {
        comms_hub_pump();
        if (comms_hub_state() == kHubLive) {
            if (tick() - C.last_beat >= kBeatGatherMs) send_state();
            port_coop_save_frame();
            const unsigned others = present_others();
            if (others && (C.req_seen & others) == others && tick() - C.last_req_ms >= 500) {
                LOG("host gate: %x guest(s) served, going to load after %u ms", others, (unsigned)(tick() - t0));
                return;
            }
        }
        Sleep(10);
    }
    LOG("host gate: done after %d ms (guests present %x asked %x)", budget, present_others(), C.req_seen);
}

void guest_fail(const char *why) {
    std::fprintf(stderr, "Could not reach the host (%s)\n", why);
    if (C.inplace) {   // SEAMCOOP1: a room booted in place: the line in the window, then the title; this process stays
        LOG("%s; the room ends in place (no exit code: the title is already this process's)", why);
        leave(kCoopExitNoHost, "Could not reach the host.");
        return;
    }
    LOG("%s; exiting with code %d", why, kCoopExitNoHost);
    std::fflush(stderr);
    ExitProcess(kCoopExitNoHost);
}

void guest_gate() {
    // 15 s: a guest launched right after the host hears nothing while the
    // host loads the ROM (it only answers once its title runs).
    const int hear_ms = env_int("SM64DS_COOPBOOT_HEAR_MS", 15000);
    const ULONGLONG t0 = tick();
    while (!C.state_heard) {
        comms_hub_pump();
        if ((int)(tick() - t0) >= hear_ms) {
            char why[64];
            std::snprintf(why, sizeof why, "no word from the host in %d s", hear_ms / 1000);
            guest_fail(why);
        }
        Sleep(10);
    }
    LOG("host heard after %u ms (phase %d go %d closed %d)", (unsigned)(tick() - t0), C.hs.phase, C.hs.go, C.hs.closed);
    // A joiner must not start its own intro: wait for the host's opening to end.
    const ULONGLONG w0 = tick();
    C.opening_wait = true;
    while (C.hs.go && C.hs.phase == kCoopPhaseIntro) {
        comms_hub_pump();
        check_host_gone();
        if (C.dropped) return;   // KNOWNNET1: the host left during its opening; this process boots to the title (boot_drop_room)
        if (tick() - w0 > 900000ULL) guest_fail("the opening never ended");
        static ULONGLONG said;
        if (tick() - said > 2000) { said = tick(); LOG("joined during the opening: waiting for it to end"); }
        Sleep(20);
    }
    C.opening_wait = false;
    if (tick() - w0 > 100) LOG("the opening ended; entering after %u ms", (unsigned)(tick() - w0));
    // A gate beat can be the host's last pump before it loads the ROM, so the
    // save is only served once the host is running (seen: 30 s on a loaded
    // box). The host is known to exist here, so wait for it generously.
    const int fetch_ms = env_int("SM64DS_COOPBOOT_FETCH_MS", 45000);
    if (!coop_save_fetch(fetch_ms)) {
        char why[64];
        std::snprintf(why, sizeof why, "the host did not send the save in %d s", fetch_ms / 1000);
        guest_fail(why);
    }
}

// The state of the title/menu words, for calibrating nothing at run time: kept
// out. (No title hold; see the banner.)

}  // namespace

// ---- public ------------------------------------------------------------------
bool coop_boot_active() { return C.on; }
bool coop_boot_released() { return C.released; }
bool coop_boot_holding() { return C.holding; }
bool coop_boot_ticking() { return !C.on || (C.released && !C.holding && port_adventure_hub_opening_held() == 0); }
CoopBootStats coop_boot_stats() { return C.st; }

}  // namespace port

extern "C" void port_coop_boot_begin(void) {
    using namespace port;
    if (C.init) return;
    C.init = true;
    if (!coop_save_active()) return;
    C.on = true;
    C.host = coop_save_is_host();
    C.t_boot = tick();
    // 1. THE ENV CONTRACT: a coop room boots to the file, always; the opening
    // is never suppressed on one console only.
    _putenv_s("SM64DS_SKIP_MENU", "1");
    _putenv_s("SM64DS_BOOT_FILE", "0");
    _putenv("SM64DS_SKIP_INTRO=");
    LOG("coop boot: role=%s, SKIP_MENU=1 BOOT_FILE=0 forced, SKIP_INTRO cleared", C.host ? "host" : "guest");
    // 2. THE ONE SYNB HANDLER
    coop_save_set_external_dispatch(true);
    hub_on_kind(kHubCoopCtl, gate_on_ctl, nullptr);
    const bool r1 = coop_ctl_register(0x20, 0x2f, sub_save, nullptr);
    const bool r2 = coop_ctl_register(kCoopBootSubLo, kCoopBootSubHi, sub_boot, nullptr);
    g_ranges_registered = true;
    LOG("SYNB ranges registered with the world core: 0x20-0x2f %d, 0x30-0x3f %d", r1 ? 1 : 0, r2 ? 1 : 0);
    if (C.host) {
        coop_save_prepare();
        std::atexit(host_atexit);
        g_host_atexit = true;
        gate();
    } else {
        guest_gate();
    }
    // PROOF KNOB: a slow machine. Sit here (still pumping, so beats and save
    // answers go on) before the ROM loads.
    if (const int d = env_int("SM64DS_COOPBOOT_TEST_DELAY_MS", 0)) {
        LOG("test delay %d ms before the boot", d);
        const ULONGLONG t0 = tick();
        while ((int)(tick() - t0) < d) {
            comms_hub_pump();
            if (C.host && comms_hub_state() == kHubLive) {
                if (tick() - C.last_beat >= kBeatMs) send_state();
                port_coop_save_frame();
            }
            Sleep(10);
        }
    }
}

extern "C" int port_coop_boot_scene_frame(void) {
    using namespace port;
    if (!C.on) return 0;
    C.title_seen = true;
    // No hold here (see the banner): keep pumping so the host answers a late
    // guest's save request and keeps its beat while the title runs.
    comms_hub_pump();
    check_host_gone();
    if (C.host && comms_hub_state() == kHubLive) {
        if (tick() - C.last_beat >= kBeatMs) send_state();
        port_coop_save_frame();
    }
    return 0;
}


namespace port {
namespace {
// ---------------------------------------------------------------------------
// COOPSPAWN9 (run mp1, items 3 and 4 of the 10-13 field report): WHERE EVERYONE STANDS AFTER THE OPENING.
//
// The opening is a one-player cutscene on every console. Its hand-over (LakituBro puts the player to sleep on the spawn,
// talks, wakes him; the opening-seen bit is set as the player wakes) asks the ROM's ClosestPlayer for the body it works
// with, and coop replaces that answer with a group-wide pick. Measured with four consoles (out COOPSPAWN9, base_open and
// the b3/b4/b5 probe rows): the LakituBro pick itself already answered each console's OWN body, but from the first tick after
// the opening's closing load some consoles' bodies slid off the spawn by themselves (100 to 300 units, and in about half the
// runs a runaway of up to 2000 units west to the hedge maze, pad words all zero), at random, on the shipped build too, so
// the talk happened with Yoshi somewhere else and the group ended up scattered.
//  1. The group-wide ClosestPlayer stays OFF from the opening's start until its seen bit (port_coop_boot_opening_hold, read by
//     coop_target.cpp): the hand-over works on the local body exactly as it does for one player.
//  2. PIN: from the closing load until the bit the body is written back onto the spawn point the load gave it (record 0, the
//     same point on every console) before every tick, with the spawn's facing.
//  3. At the bit every console puts its own body on its place of the 96-unit ring around that spawn (the table of
//     port_vs_extra_start in hal/level_boot.cpp, copied here because that function is file-local): the host takes the centre,
//     guests rank 1..3 by hub seat. Wire-free: the spawn is level data, so every console computes the same places.
//
// A LATE JOINER (the GO was already given when it reached the file's first frame) has no opening: it waits for the host's
// body to show in its own level (hub presence + the follower's drawn pose, at most 4 s) and lands on the same ring around
// it. No host in the level, or no pose: it stays where the level put it.
// ---------------------------------------------------------------------------
struct SpawnState {
    bool inited = false;
    bool bit_prev = false;
    bool late = false;
    bool late_done = false;
    ULONGLONG late_since = 0;
    bool log = false;
    unsigned frame = 0;
    unsigned placed = 0;
    // after the opening
    int phase = 0;                 // 0 before the bit, 3 done
    bool have_spawn = false;
    int spawn[3] = {0, 0, 0};
    short spawn_ang = 0;
    unsigned pin_from = 0, pinned = 0, bit_frame = 0;
    double max_drift = 0.0;
} S9;

bool seen_bit() { return (data_0209caa0[8] & 0x80) != 0; }

char *spawn_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16 && me < (int)data_0209f21c) ? (char *)data_0209f394[me] : nullptr;
}

int rd_i32(const char *a, int off) { int v; std::memcpy(&v, a + off, 4); return v; }
void wr_i32(char *a, int off, int v) { std::memcpy(a + off, &v, 4); }

double hdist(const int *a, const int *b) {
    const double dx = ((double)a[0] - b[0]) / 4096.0, dz = ((double)a[2] - b[2]) / 4096.0;
    return std::sqrt(dx * dx + dz * dz);
}

// my place among the guests: hub seats other than the host's, counted from 1 (the host itself stays on the centre, rank 0)
int my_rank() {
    const int seat = my_seat();
    if (seat < 0 || seat > 3) return -1;
    if (C.host) return 0;
    int r = 1;
    for (int t = 0; t < seat; ++t) if (t != C.host_seat) ++r;
    return r;
}

// the ring of port_vs_extra_start for slots 1..3 (radius 96, bearings 0, 72, 144 degrees), 20.12 offsets
void ring_offset(int slot, int out[2]) {
    static const short kCos[5] = {4096, 1266, -3313, -3313, 1266};
    static const short kSin[5] = {0, 3895, 2408, -2408, -3895};
    out[0] = out[1] = 0;
    if (slot < 1 || slot > 5) return;
    out[0] = 96 * (int)kCos[slot - 1];
    out[1] = 96 * (int)kSin[slot - 1];
}

// Move my own body to cx,cz (+ the ring for my rank), keeping the centre's height; 20.12.
void spawn_move(const char *why, int cx, int cy, int cz) {
    char *b = spawn_body();
    const int rank = my_rank();
    if (!b || rank < 0) return;
    static const bool place_off = std::getenv("SM64DS_COOPSPAWN_PLACE") && std::getenv("SM64DS_COOPSPAWN_PLACE")[0] == '0';   // proof control
    if (place_off) { LOG("SPAWN %s: placement switched off (SM64DS_COOPSPAWN_PLACE=0)", why); return; }
    int off[2];
    ring_offset(rank, off);
    const int tx = cx + off[0], tz = cz + off[1];
    const int ox = rd_i32(b, 0x5c), oy = rd_i32(b, 0x60), oz = rd_i32(b, 0x64);
    wr_i32(b, 0x5c, tx);
    wr_i32(b, 0x60, cy);
    wr_i32(b, 0x64, tz);
    wr_i32(b, 0x68, tx);       // last frame's position too, so no sweep from the old spot
    wr_i32(b, 0x6c, cy);
    wr_i32(b, 0x70, tz);
    wr_i32(b, 0x98, 0);        // no horizontal speed carried over
    ++S9.placed;
    LOG("SPAWN %s: seat %d (rank %d) placed (%d,%d,%d) -> (%d,%d,%d) around centre (%d,%d,%d)", why, my_seat(), rank, ox >> 12,
        oy >> 12, oz >> 12, tx >> 12, cy >> 12, tz >> 12, cx >> 12, cy >> 12, cz >> 12);
}

// item 4 evidence: where LakituBro stands and which way he faces compared with the way to my body (SM64DS_COOPSPAWN_LOG)
void spawn_log_lakitu(const char *b) {
    for (const char *a = (const char *)_ZN8dActor_c4NextEPKS_(nullptr); a; a = (const char *)_ZN8dActor_c4NextEPKS_(a)) {
        unsigned short cls; std::memcpy(&cls, a + 0x0c, 2);
        if (cls != 235) continue;
        short yaw; std::memcpy(&yaw, a + 0x8e, 2);
        const short to_me = Vec3_HorzAngle(a + 0x5c, b + 0x5c);
        const int d = (int)(short)(yaw - to_me);
        LOG("SPAWN lakitu f=%u pos=(%d,%d,%d) yaw=%d toMe=%d diff=%d me=(%d,%d,%d)", S9.frame, rd_i32(a, 0x5c) >> 12,
            rd_i32(a, 0x60) >> 12, rd_i32(a, 0x64) >> 12, (int)yaw, (int)to_me, d < 0 ? -d : d, rd_i32(b, 0x5c) >> 12,
            rd_i32(b, 0x60) >> 12, rd_i32(b, 0x64) >> 12);
        return;
    }
}

// the follower's drawn pose of the body that stands for hub seat `seat` (the ghost API is indexed by BODY slot, which is not the seat)
bool seat_drawpos(int seat, int dp[3]) {
    const int nb = (int)data_0209f21c < 16 ? (int)data_0209f21c : 16;
    for (int i = 0; i < nb; ++i) {
        if (i == (int)data_0209f250 || !data_0209f394[i]) continue;
        if (port_adventure_ghost_seat(i) == seat) return port_adventure_ghost_drawpos(i, dp) != 0;
    }
    return false;
}

bool host_in_my_level(int dp[3]) {
    HubPresence p;
    return C.host_seat >= 0 && hub_presence_get(C.host_seat, &p) && p.in_level() && p.level == (int)data_0209f2f8 &&
           seat_drawpos(C.host_seat, dp);
}

void spawn_frame() {
    if (!S9.inited) {
        S9.inited = true;
        S9.log = env_on("SM64DS_COOPSPAWN_LOG");
        S9.bit_prev = seen_bit();
        S9.late = !C.host && C.late_at_first && S9.bit_prev && !C.intro_seen;
        S9.late_since = tick();
        if (S9.late) LOG("SPAWN late joiner: waits for the host's body to place itself on the ring");
    }
    ++S9.frame;
    char *b = spawn_body();
    if (!b) return;
    const bool bit = seen_bit();
    int me[3] = {rd_i32(b, 0x5c), rd_i32(b, 0x60), rd_i32(b, 0x64)};
    if (S9.log && C.intro_seen && !bit && (S9.frame % 20) == 0) spawn_log_lakitu(b);
    if (S9.log && C.intro_seen && (!bit || (S9.bit_frame && S9.frame - S9.bit_frame < 1200)) && (S9.frame % 10) == 0) {
        short ang; std::memcpy(&ang, b + 0x8e, 2);
        const int mi = (int)data_0209f250 & 3;
        const unsigned char *ct = data_0209f498 + mi * 0x18;
        short cmag, cang; unsigned short cheld; std::memcpy(&cmag, ct + 8, 2); std::memcpy(&cang, ct + 14, 2); std::memcpy(&cheld, ct + 4, 2);
        LOG("SPAWN body f=%u pos=(%d,%d,%d) spd=%d ang=%d st=%08x pause=%02x stick mag=%d ang=%d held=%04x touch=%d/%d pad=%08x mode=%d", S9.frame,
            me[0] >> 12, me[1] >> 12, me[2] >> 12, rd_i32(b, 0x98), (int)ang, (unsigned)rd_i32(b, 0x370), (unsigned)(unsigned char)b[0x13],
            (int)cmag, (int)cang, (unsigned)cheld, (int)data_020a0de8[mi * 4], (int)data_020a0de8[mi * 4 + 1], (unsigned)data_020a0e58[mi], (int)ct[0x16]);
        {
            const int si = (int)data_020a0e40 & 3;
            short smag; std::memcpy(&smag, data_0209f498 + si * 0x18 + 8, 2);
            LOG("SPAWN slot f=%u e40=%d me=%d ctrl[e40].mag=%d ctrl[0..3].mag=%d,%d,%d,%d", S9.frame, (int)data_020a0e40, (int)data_0209f250, (int)smag,
                (int)*(short *)(data_0209f498 + 8), (int)*(short *)(data_0209f498 + 0x18 + 8), (int)*(short *)(data_0209f498 + 0x30 + 8), (int)*(short *)(data_0209f498 + 0x48 + 8));
        }
    }
    if (S9.log && (bit || !C.intro_seen) && (S9.frame % 120) == 0) {
        // this console's view of every seat: my own body exactly, a friend at the pose the follower drew him at
        char v[256]; int w = 0;
        for (int s = 0; s < 4; ++s) {
            int dp[3];
            if (s == my_seat()) { dp[0] = me[0]; dp[1] = me[1]; dp[2] = me[2]; }
            else if (!seat_drawpos(s, dp)) continue;
            w += std::snprintf(v + w, sizeof v - (size_t)w, " s%d%s=(%d,%d,%d)", s, s == my_seat() ? "*" : "", dp[0] >> 12, dp[1] >> 12, dp[2] >> 12);
        }
        LOG("SPAWN view f=%u level=%d%s", S9.frame, (int)data_0209f2f8, v);
    }

    // ---- after the opening. From the closing load until the seen bit the body stays on the spawn the load gave it (record 0 of
    // the level, the same point on every console): the ROM's hand-over leaves it standing there when one player plays, but in
    // a room some consoles' bodies got shoved off it (measured: runaway slides of 2000 units west, or 100 to 300 either way),
    // so the talk with LakituBro happened with Yoshi somewhere else. The pin writes it back before every tick.
    // At the bit every console puts its own body on its place of the ring around that spawn (rank by hub seat; the host
    // takes the centre) and gives it the spawn's facing: the group stands together on the main spawn, wire-free.
    static const bool pin_off = std::getenv("SM64DS_COOPSPAWN_PIN") && std::getenv("SM64DS_COOPSPAWN_PIN")[0] == '0';   // proof control
    if (C.intro_seen && C.arrived && S9.phase == 0 && !bit && !S9.have_spawn) {
        S9.have_spawn = true;
        std::memcpy(S9.spawn, me, sizeof me);
        std::memcpy(&S9.spawn_ang, b + 0x8e, 2);
        S9.pin_from = S9.frame;
        LOG("SPAWN spawn point (%d,%d,%d) facing %d%s", me[0] >> 12, me[1] >> 12, me[2] >> 12, (int)S9.spawn_ang, pin_off ? " (pin off)" : "");
    } else if (S9.have_spawn && S9.phase == 0 && !bit && !pin_off) {
        const double drift = hdist(me, S9.spawn);
        if (drift > S9.max_drift) S9.max_drift = drift;
        if (drift > 0.0) {
            wr_i32(b, 0x5c, S9.spawn[0]);
            wr_i32(b, 0x64, S9.spawn[2]);
            wr_i32(b, 0x68, S9.spawn[0]);
            wr_i32(b, 0x70, S9.spawn[2]);
            std::memcpy(b + 0x8e, &S9.spawn_ang, 2);
            std::memcpy(b + 0x94, &S9.spawn_ang, 2);
            ++S9.pinned;
        }
    }
    if (C.intro_seen && S9.phase == 0 && bit && !S9.bit_prev) {
        S9.phase = 3;
        S9.bit_frame = S9.frame;
        LOG("SPAWN opening seen bit set on this console at (%d,%d,%d); pinned %u frames, largest drift in one frame %.0f units", me[0] >> 12,
            me[1] >> 12, me[2] >> 12, S9.pinned, S9.max_drift);
        if (S9.have_spawn) {
            spawn_move("after the opening", S9.spawn[0], S9.spawn[1], S9.spawn[2]);
            std::memcpy(b + 0x8e, &S9.spawn_ang, 2);
            std::memcpy(b + 0x94, &S9.spawn_ang, 2);
        }
    }
    S9.bit_prev = bit;

    // ---- proof pictures: SM64DS_COOPSPAWN_SHOT="t300,b60,..." writes spawn9_<mark>.bmp (the top screen) that many ticks after the
    // spawn capture (t) or after the seen bit (b); late joiners count from their first tick (l)
    if (const char *sh = std::getenv("SM64DS_COOPSPAWN_SHOT")) {
        for (const char *q = sh; *q;) {
            const char kind = *q;
            char *end = nullptr;
            const long n = std::strtol(q + 1, &end, 10);
            if (end == q + 1) break;
            unsigned base = 0; bool ok = false;
            if (kind == 't' && S9.have_spawn) { base = S9.pin_from; ok = true; }
            else if (kind == 'b' && S9.phase == 3 && S9.bit_frame) { base = S9.bit_frame; ok = true; }
            else if (kind == 'l') { base = 1; ok = true; }
            if (ok && S9.frame == base + (unsigned)n) {
                char nm[64]; std::snprintf(nm, sizeof nm, "spawn9_%c%ld.bmp", kind, n);
                const void *lfb = ::port_level_fb_ptr;
                const bool w = lfb && ::ntr::ppu_write_bmp(nm, *(const ::ntr::Framebuffer *)lfb);
                LOG("SPAWN picture %s %s at (%d,%d,%d)", nm, w ? "written" : "FAILED", me[0] >> 12, me[1] >> 12, me[2] >> 12);
            }
            q = end; while (*q == ',' || *q == ' ') ++q;
        }
    }

    // ---- a late joiner
    if (S9.late && !S9.late_done) {
        const bool give_up = tick() - S9.late_since > 4000ULL;
        int dp[3];
        if (host_in_my_level(dp)) {
            S9.late_done = true;
            spawn_move("late joiner", dp[0], dp[1], dp[2]);
        } else if (give_up) {
            S9.late_done = true;
            LOG("SPAWN late joiner: no host body in this level within 4 s, staying where the level put me");
        }
    }
}
}  // namespace
}  // namespace port

// The level loop's hold. 1 = skip this frame's game tick.
extern "C" int port_coop_boot_frame(void) {
    using namespace port;
    if (!C.on) return 0;
    C.title_seen = true;
    if (coop_leave_holds()) { comms_hub_pump(); return 1; }   // SEAMCOOP1: the line is up: nothing moves under it
    if (!C.at_first_frame) {
        C.at_first_frame = true;
        if (C.inplace)   // BROADCAST-M5-1 note 4: the time from the accepted hand-off to the room's first level frame
            LOG("first frame of the room %u ms after the hand-off was accepted (in place)", (unsigned)(tick() - C.t_boot));
        C.late_at_first = C.released;
        C.host_ready_ms = tick();
        C.last_ready = 0;
        LOG("%s at the file's first frame: %s", C.host ? "host" : "guest",
            C.released ? "go already given" : "READY");
        if (C.host) { C.phase = kCoopPhaseGather; send_state(); }
    }
    // the opening: held while the one-player seat is on; leaving that is the
    // closing level load
    const bool held = port_adventure_hub_opening_held() != 0;
    if (held) {
        if (!C.intro_seen) {
            C.intro_seen = true;
            LOG("the opening is playing here");
        }
        if (C.host && C.phase != kCoopPhaseIntro) C.phase = kCoopPhaseIntro;
    } else if (C.intro_seen && !C.arrived) {
        C.arrived = true;
        if (const int slow = env_int("SM64DS_COOPBOOT_TEST_SLOW_MS", 0)) {
            LOG("test: a slow console, %d ms before it announces", slow);
            const ULONGLONG t0 = tick();
            while ((int)(tick() - t0) < slow) { comms_hub_pump(); Sleep(10); }
        }
        const ULONGLONG now = tick();
        LOG("ARRIVED at the opening's closing load");
        if (C.host) {
            C.arrived_mask |= my_bit();
            if (!C.any_arrived) { C.any_arrived = true; C.first_arrive_ms = now; }
        } else {
            if (send_sub(kCoopBootSubArrived, nullptr)) ++C.st.arrived_sent;
            C.hold_since = now;
        }
    }
    if (C.host && C.phase == kCoopPhaseIntro && !held && (data_0209caa0[8] & 0x80)) {
        C.phase = kCoopPhaseSettled;
        LOG("host: the opening is over (seen bit set); phase settled");
    }
    if (C.host && C.phase == kCoopPhasePlaying && !C.intro_seen && !held && (data_0209caa0[8] & 0x80)) {
        C.phase = kCoopPhaseSettled;
    }
    service();
    if (C.released) run_forge();
    bool hold = false;
    bool &said_end = C.said_end;
    const char *why = nullptr;
    if (!C.released) { hold = true; why = "start barrier"; }
    else if (C.arrived && !C.closed) {
        // The load's first frames must tick (the camera, the actors and the
        // draw lists are only valid after a few ticks; a frozen first frame
        // faulted the renderer), so the freeze starts after kWarmFrames.
        if (C.warm < kWarmFrames) ++C.warm;
        else { hold = true; why = "opening end barrier"; }
    } else if (C.arrived && C.closed && !said_end) {
        said_end = true;
        if (!C.holding) LOG("RELEASED (end barrier closed before the freeze)");
    }
    if (hold && !C.host) {
        // a guest never waits for ever on a host that is still alive but silent
        static ULONGLONG since;
        if (!C.holding) since = tick();
        if (tick() - since > 30000ULL) {
            LOG("hold released after 30 s without the host's word (%s)", why);
            C.released = true;
            C.closed = true;
            hold = false;
        }
    }
    bool &said_release = C.said_release;
    if (hold != C.holding) {
        C.holding = hold;
        LOG(hold ? "HOLD: %s" : "RELEASED (first tick now)%s", hold ? why : "");
        if (!hold) { said_release = true; if (C.arrived) said_end = true; }
    }
    if (!hold && C.released) {
        if (!said_release) {
            said_release = true;
            LOG("RELEASED (first tick now)%s", " never held");
        }
        int &ticked = C.ticked;
        ++ticked;
        if (ticked == 1 || ticked == 120 || ticked == 600)
            LOG("ticked frame %d since the release", ticked);
    }
    if (!hold) spawn_frame();
    return hold ? 1 : 0;
}

// COOPSPAWN9: 1 while this console is in a new file's opening (its start until the opening-seen bit), when
// hal/coop_target.cpp leaves the ROM's own ClosestPlayer in place so the hand-over runs on the local body.
extern "C" int port_coop_boot_opening_hold(void) {
    using namespace port;
    if (!C.on) return 0;
    static const bool off = std::getenv("SM64DS_COOPSPAWN_HOLD") && std::getenv("SM64DS_COOPSPAWN_HOLD")[0] == '0';   // proof control
    if (off) return 0;
    if (port_adventure_hub_opening_held() != 0) return 1;
    return (C.intro_seen && !(data_0209caa0[8] & 0x80)) ? 1 : 0;
}

// ==== SEAMCOOP1: A COOP ROOM ENDS IN PLACE (run after093; the one-process plan's M5b) =======================================
// A coop room used to end the game: the host leaving (its HOST_LEFT, or its seat gone for 1.5 s) put a TOPMOST popup window over
// everything for 3 s on every guest and ended the process with exit code 7, a guest that could not reach the host ended with 6, and
// the host's own Save and Quit restarted the game at its front door (tests/walk_window.cpp port_front_end_quit_poll, a coop session
// was one of its refusals). The launcher then decided whether a new process starts. Now, once a title exists in this process:
//   1. THE LINE IN THE WINDOW. The one plain line ("The host left. Room closed." / "Could not reach the host.") is drawn inside the
//      game window: a black picture with the line centred in the ui_text font, handed to the presenter through the same block the
//      Joining plate takes (port_coop_line_picture). No second window, nothing TOPMOST. In a level the game tick is held while it is
//      up (port_coop_boot_frame returns 1), so nothing moves under it.
//   2. THE TITLE. After 3 s a console in a level asks the ROM for scene 1 the way the quit rows do (data_02092664 = 1, the id
//      StartSceneFade(1, 0, 0) parks); the quit poll's coop branch (port_coop_leave_refusal / port_coop_leave_begin) keeps that scene
//      change in this process, the ROM's fade and the level-to-scene crossing take the game to the title (SEAMQUIT1's path). A console
//      already on the title (a room booted in place whose gate failed) is there already. The line stays up until the title is live.
//   3. THE ROOM TORN DOWN, AT THE TITLE (the first game frame with the title live; port_sdat_frame_hook, chained): the hub
//      (SEAMHUB1's comms_hub_teardown: the I/O thread joined, the socket closed, every seat and handler forgotten) and the layers on it
//      (comms_sync / comms_conductor / hub_presence leave resets); the session record cleared and the launcher's 43 names out of BOTH
//      copies of the environment; the Adventure flag's env latch read again; the coop layer back to a process that never was in a room
//      (coop_world_session_reset, coop_save_session_reset, this file's state); the save medium back to the single player save
//      (ntr/backup.cpp select_coop(0): the ROM's "ds mario" record tag back, a guest's temp image deleted); the mods relatched to the
//      offline list (hal/mods.cpp). The host's own Save and Quit takes the same path; it says HOST_LEFT first (host_atexit's work,
//      done here because the process no longer exits) and makes the exit's last save attempt (coop_save_leave_flush).
// WHAT STILL EXITS: a room that ends before any title of this process exists, i.e. the boot gate in main of a launcher-started coop
// child (no host heard in 15 s: exit 6; the host gone while a joiner waits for its opening: the old popup and exit 7; the host's save
// never served: exit 6). The launcher contract for boot failures stands. A refusal at the title (the hub thread did not stop, the
// mods layer refused) restarts through port_online_leave_restart_hook as SEAMHUB1's leave does.
extern "C" {
extern void (*port_sdat_frame_hook)(void);                    /* hal/sdat/consumer.cpp: one call per game frame, every loop */
int port_scene_live_id(void);                                 /* hal/scene_boot.cpp: 1 = the title is the live scene */
int port_mods_relatch_after_level(const char *reason);        /* hal/mods.cpp */
int port_mods_relatch_forced(const char *reason);             /* hal/mods.cpp (SEAMCLEAN1) */
int port_mods_relatch_at_title(const char *reason);
int port_mods_multiplayer(void);
int port_mods_count(void);
const char *port_mods_id(int i);
const char *port_mods_kind(int i);
void host_setting_adventure_env_reread(void);                 /* hal/host_settings.cpp */
void port_join_settings_rearm(void);                          /* hal/menu_join_plate.cpp */
extern int port_front_end_in_place;                           /* hal/level_change.cpp (SEAMQUIT1) */
extern int (*port_online_leave_restart_hook)(void);           /* hal/online_join.cpp; tests/walk_window.cpp seats it */
extern unsigned short data_02092664;                          /* Scene::SetSceneToSpawn's pending id */
}
namespace ntr { namespace backup { void select_coop(int mode); bool coop_selected(); bool online_selected(); } }
namespace port { namespace party_hits { void session_reset(); } }   /* hal/party_hits.cpp (BROADCAST-M5-1) */

namespace port {
bool adventure_ghost_mode();                                  /* hal/host_settings.h */
namespace {

enum { kLvNone = 0, kLvLine = 1, kLvFade = 2, kLvTitle = 3 };
struct LeaveState {
    int st = kLvNone;
    ULONGLONG t0 = 0, fade_t0 = 0;
    int code = 0;
    bool from_level = false;
    bool slow_said = false;
    bool wait_title = false;   // KNOWNNET1: the line goes up when this process's first title is live (boot_drop_room)
    char text[96] = {};
} LV;

struct LinePic {
    bool up = false;
    char text[96] = {};
    int pw = 0, ph = 0;
    bool with_text = false;
    bool shot = false;
    std::vector<uint32_t> px;
} LP;

void (*g_lv_prev_hook)(void) = nullptr;
bool g_lv_chained = false;

/* The launcher's names (LobbyLaunch.cs): the same 43 hal/online_join.cpp clears at an online leave (its k_join_env 12 +
   k_session_env 31, file-static there). */
const char *const k_coop_env[] = {
    "SM64DS_ADVENTURE", "SM64DS_COMMS_RELAY", "SM64DS_COMMS_CODE", "SM64DS_ROOM_KIND", "SM64DS_VOICE_ALLOWED", "SM64DS_HUB_SEAT",
    "SM64DS_PLAYER_NAME", "SM64DS_PLAYER_COLOR", "SM64DS_PLAYER_SHOES", "SM64DS_WORLD_NO", "SM64DS_JOIN_QUIET", "SM64DS_BOOT_FILE",
    "SM64DS_COMMS_HOST", "SM64DS_COMMS_ROLE", "SM64DS_COMMS_SLOT", "SM64DS_COMMS_PORT", "SM64DS_COMMS_INPUT_DELAY",
    "SM64DS_HUB_PORT", "SM64DS_HUB_DELAY_MS", "SM64DS_PARTY", "SM64DS_PARTY_SHARE_STARS", "SM64DS_NETMODE",
    "SM64DS_COOP", "SM64DS_COOP_HOST", "SM64DS_COOP_SAVE", "SM64DS_COOP_SAVE_NAME", "SM64DS_COOP_TP",
    "SM64DS_ROOM_COLLISION", "SM64DS_ROOM_INTERACT", "SM64DS_ROOM_DAMAGE", "SM64DS_ROOM_TP", "SM64DS_ROOM_STARSHARE",
    "SM64DS_VS_PLAYERS", "SM64DS_VS_NAMES", "SM64DS_VS_COLORS", "SM64DS_VS_NETMODE", "SM64DS_SESSION_DIGEST",
    "SM64DS_SKIP_MENU", "SM64DS_VS_MAP", "SM64DS_VS_STAR_TARGET", "SM64DS_VS_KING_TARGET", "SM64DS_VS_LUIGI_INFECTION",
    "SM64DS_VS_LUIGI_SEED" };

/* a name set in either copy of the environment */
bool env_any(const char *n) {
    const char *c = std::getenv(n);
    if (c && *c) return true;
    char v[4];
    return GetEnvironmentVariableA(n, v, sizeof v) > 0;
}

void coop_frame_hook();
void reread_files(const char *why);

void lv_chain() {
    if (g_lv_chained) return;
    g_lv_prev_hook = port_sdat_frame_hook;
    port_sdat_frame_hook = coop_frame_hook;
    g_lv_chained = true;
}

void line_raise(const char *t) {
    std::snprintf(LP.text, sizeof LP.text, "%s", t);
    LP.up = true;
    LP.pw = LP.ph = 0;
    LP.px.clear();
    LP.shot = false;
}

void line_lower(const char *why) {
    if (!LP.up) return;
    LP.up = false;
    LOG("the line is down (%s)", why);
}

void build_line(int w, int h) {
    LP.px.assign((size_t)w * h, 0xFF000000u);
    LP.with_text = ui_text_ready() != 0;
    if (LP.with_text) {
        const float em = (float)h * 0.06f > 12.f ? (float)h * 0.06f : 12.f;
        const UiSurface sf = { LP.px.data(), w, w, h };
        const float tw = ui_text_width(LP.text, em);
        const float x = ((float)w - tw) * 0.5f;
        const float base = (float)h * 0.5f + ui_text_cap_height(em) * 0.5f;
        ui_text_draw(&sf, x > 0.f ? x : 0.f, base, LP.text, em, 0xFFFFFF, 255, nullptr);
    }
    LP.pw = w;
    LP.ph = h;
    LOG("the line picture %dx%d built (%s): \"%s\"", w, h, LP.with_text ? "the line drawn in the ui_text font" : "the font is not ready: black only",
        LP.text);
    // PROOF: SM64DS_COOPBOOT_POPUP_SHOT=<bmp path> (the knob the popup used) now writes the picture handed to the presenter
    if (!LP.shot && LP.with_text) {
        LP.shot = true;
        if (const char *s = std::getenv("SM64DS_COOPBOOT_POPUP_SHOT")) {
            bool ok = false;
            if (std::FILE *f = std::fopen(s, "wb")) {
                BITMAPFILEHEADER fh; BITMAPINFOHEADER bh;
                std::memset(&fh, 0, sizeof fh); std::memset(&bh, 0, sizeof bh);
                bh.biSize = sizeof bh; bh.biWidth = w; bh.biHeight = -h; bh.biPlanes = 1; bh.biBitCount = 32;
                fh.bfType = 0x4D42; fh.bfOffBits = sizeof fh + sizeof bh; fh.bfSize = fh.bfOffBits + (DWORD)(w * h * 4);
                ok = std::fwrite(&fh, sizeof fh, 1, f) == 1 && std::fwrite(&bh, sizeof bh, 1, f) == 1 &&
                     std::fwrite(LP.px.data(), 4, (size_t)w * h, f) == (size_t)w * h;
                std::fclose(f);
            }
            LOG("the line picture written to the proof path: %s", ok ? "saved" : "FAILED");
        }
    }
}

void lv_restart(const char *why, bool mods_refused = false) {
    std::fprintf(stderr, "[seamless] in place refused: %s; restarting\n", why);
    std::fflush(stderr);
    if (!port_online_leave_restart_hook || !port_online_leave_restart_hook()) {
        std::fprintf(stderr, "[seamless] the restart could not be started (win32 %lu); this process stays on the title\n",
                     (unsigned long)GetLastError());
        std::fflush(stderr);
        /* SEAMCLEAN1 (item 3 for the coop leave): the room is down and the mods relatch was refused, so the title holds the
           multiplayer list with no restart coming: the offline list goes on anyway (hal/online_join.cpp leave_restart) */
        if (mods_refused) {
            if (port_mods_relatch_forced("the restart could not be started"))
                std::fprintf(stderr, "[seamless] the title stays playable: the offline mod list is latched (%d mod(s) on)\n",
                             port_mods_count());
            else
                std::fprintf(stderr, "[seamless] the title stays on the multiplayer mod list (%d mod(s) on); close the game and start "
                             "it again for the full list\n", port_mods_count());
            std::fflush(stderr);
        }
    }
}

/* the coop layer of this file back to a process that never was in a room (the title exists: title_seen stays) */
void boot_state_reset() {
    C = Ctl{};
    C.title_seen = true;
    S9 = SpawnState{};
}

void leave_at_title() {
    LV.st = kLvTitle;
    const bool was_host = C.host;
    LOG("the title is up: ending the coop room in this process (%s)", was_host ? "host" : "guest");
    if (!comms_hub_teardown("leaving the coop room in place")) {
        lv_restart("the hub's I/O thread did not stop");
        return;
    }
    comms_sync_leave_reset();
    comms_conductor_leave_reset();
    hub_presence_leave_reset();
    const int fields = online_session_clear();
    std::string cleared;
    int names = 0;
    for (const char *n : k_coop_env) {
        if (!env_any(n)) continue;
        SetEnvironmentVariableA(n, nullptr);
        _putenv_s(n, "");
        if (names++) cleared += ' ';
        cleared += n + 7;
    }
    host_setting_adventure_env_reread();
    coop_world_session_reset();
    coop_save_session_reset();
    party_hits::session_reset();
    boot_state_reset();
    std::fprintf(stderr, "[seamless] coop room torn down: hub, ghost layer, presence; session record cleared (%d field(s)); %d launcher "
                 "name(s) cleared from both copies of the environment: %s\n", fields, names, names ? cleared.c_str() : "-");
    std::fflush(stderr);
    ntr::backup::select_coop(0);
    reread_files("the single player save");   // the title was built from the coop medium; a solo pick must read the solo files
    unsigned extra = 0;
    const bool after_level = hub_game_heap_carved(&extra);
    const int ok = after_level ? port_mods_relatch_after_level("leaving the coop room")
                               : port_mods_relatch_at_title("leaving the coop room");
    line_lower("the title is up");
    LV = LeaveState{};
    if (!ok) {
        lv_restart("the mods layer refused the switch back to the offline list", true);
        return;
    }
    std::fprintf(stderr, "[seamless] now solo after the coop room: save medium %s (coop %d); adventure %d; multiplayer mod rule %d; "
                 "%d mod(s) on; hub %s; coop %d; in-place quit flag %d\n",
                 ntr::backup::online_selected() ? "THE ONLINE SAVE" : "the single player save", ntr::backup::coop_selected() ? 1 : 0,
                 adventure_ghost_mode() ? 1 : 0, port_mods_multiplayer(), port_mods_count(),
                 comms_hub_active() ? "STILL ACTIVE" : "off", coop_save_active() ? 1 : 0, port_front_end_in_place);
    std::fflush(stderr);
    port_join_settings_rearm();
}

// KNOWNNET1: THE HOST LEFT WHILE A LAUNCHER-STARTED GUEST WAITED FOR ITS OPENING. That wait runs in port_coop_boot_begin, before the
// ROM loads and before the game window exists, so the leave used to take leave_exit's TOPMOST popup and exit code 7 although the
// player had joined a room and was about to see the game. Now the room is dropped right there the way the title's teardown drops it
// (the hub with its I/O thread, the layers on it, the session record, the launcher's names out of both environment copies, the
// Adventure latch read again, the coop layer, the save medium forced back to the single player save), the boot goes on as a plain
// game, and the same line every other host-left case draws is drawn in the game window once the title is live: 3 s, then
// leave_at_title runs the rest of the title's teardown (mods relatched to the offline list, the files read again), as for a room
// that ends at the title. A teardown refused here (the hub thread did not stop) keeps the old popup and exit.
void boot_drop_room(int code, const char *line) {
    LOG("%s during the wait for the host's opening (no title and no window yet): the room is dropped here, the game boots to "
        "its title and the line is drawn in the game window there (no popup, no exit code %d)", line, code);
    if (!comms_hub_teardown("the host left during its opening, before this guest's boot")) leave_exit(code, line);
    comms_sync_leave_reset();
    comms_conductor_leave_reset();
    hub_presence_leave_reset();
    const int fields = online_session_clear();
    int names = 0;
    for (const char *n : k_coop_env) {
        if (!env_any(n)) continue;
        SetEnvironmentVariableA(n, nullptr);
        _putenv_s(n, "");
        ++names;
    }
    host_setting_adventure_env_reread();
    coop_world_session_reset();
    coop_save_session_reset();
    party_hits::session_reset();
    ntr::backup::select_coop(0);
    C.on = false;
    C.opening_wait = false;
    C.dropped = true;
    C.exiting = true;
    LV = LeaveState{};
    LV.st = kLvLine;
    LV.code = code;
    LV.from_level = false;
    LV.wait_title = true;
    std::snprintf(LV.text, sizeof LV.text, "%s", line);
    lv_chain();
    LOG("room dropped before the boot: session record %d field(s), %d launcher name(s) cleared; the boot goes on as a plain game",
        fields, names);
}

void leave(int code, const char *line) {
    if (C.exiting) return;
    if (!C.title_seen && C.opening_wait && !C.host) { boot_drop_room(code, line); return; }
    if (!C.title_seen) leave_exit(code, line);   // the boot gate: no title yet, the launcher's exit code
    C.exiting = true;
    LV = LeaveState{};
    LV.st = kLvLine;
    LV.t0 = tick();
    LV.code = code;
    LV.from_level = port_scene_live_id() != 1;
    line_raise(line);
    lv_chain();
    LOG("%s: the line is drawn in the game window for 3 s, then this console goes to the title in this process (%s; exit code %d "
        "is no longer used once a title exists)", line, LV.from_level ? "from a level" : "already on the title", code);
}

bool coop_leave_holds() { return LV.st == kLvLine && LV.from_level; }

void leave_step() {
    if (LV.st == kLvLine && LV.wait_title) {   // KNOWNNET1 (boot_drop_room): the line waits for the title, then its 3 s start
        if (port_scene_live_id() != 1) return;
        LV.wait_title = false;
        C.title_seen = true;
        line_raise(LV.text);
        LV.t0 = tick();
        LOG("%s: the title is live; the line is drawn in the game window for 3 s, then the room's teardown finishes at the title",
            LV.text);
        return;
    }
    if (LV.st == kLvLine) {
        if (tick() - LV.t0 < 3000) return;
        if (port_scene_live_id() == 1) { leave_at_title(); return; }
        LV.st = kLvFade;
        LV.fade_t0 = tick();
        if (data_02092664 != 1) data_02092664 = 1;
        LOG("3 s are up: asked the ROM for scene 1 (the title) the way the quit rows do; the quit poll keeps it in this process");
        return;
    }
    if (LV.st == kLvFade) {
        if (port_scene_live_id() == 1) { leave_at_title(); return; }
        if (!LV.slow_said && tick() - LV.fade_t0 > 20000) {
            LV.slow_said = true;
            LOG("the title has not come up 20 s after the request (scene request %u)", (unsigned)data_02092664);
        }
    }
}

}  // namespace
}  // namespace port

/* hal/sdat/consumer.cpp's per-frame hook, chained: the leave's steps (and SEAMCOOP1's in-place boot, below) */
namespace port { namespace {
void inplace_step();
void coop_frame_hook() {
    if (g_lv_prev_hook) g_lv_prev_hook();
    inplace_step();
    leave_step();
}
}  // namespace
}  // namespace port

/* tests/walk_window.cpp's present: the line instead of the game's picture while it is up (the Joining plate's block) */
extern "C" int port_coop_line_up(void) { return port::LP.up ? 1 : 0; }
extern "C" const uint32_t *port_coop_line_picture(int w, int h) {
    using namespace port;
    if (!LP.up || w < 32 || h < 32) return nullptr;
    if (LP.pw != w || LP.ph != h || LP.px.empty() || (!LP.with_text && ui_text_ready())) build_line(w, h);
    return LP.px.data();
}

/* tests/walk_window.cpp port_front_end_quit_poll asks this when the ROM asked for scene 1 and the solo test refused: 0 = not a coop
   room (the caller keeps its own answer); 1 with *why 0 = the quit may stay in this process (port_coop_leave_begin follows); 1 with
   *why = the reason it may not (the restart, as before). */
extern "C" int port_coop_leave_refusal(const char **why) {
    using namespace port;
    static char buf[200];
    *why = nullptr;
    const bool coop = C.on || coop_save_active() || LV.st != kLvNone;
    if (!coop) return 0;
    if (LV.st == kLvFade) return 1;   // this file's own request (the host left)
    if (LV.st == kLvTitle) { *why = "a coop leave is already under way"; return 1; }
    for (int i = 0; i < port_mods_count(); ++i) {
        const char *k = port_mods_kind(i);
        if (k && !std::strcmp(k, "code")) {
            std::snprintf(buf, sizeof buf, "a code mod is loaded (%s) and a plugin cannot be unloaded", port_mods_id(i));
            *why = buf;
            return 1;
        }
    }
    if (!port_online_leave_restart_hook) { *why = "this build has no restart for a refusal at the title"; return 1; }
    return 1;
}

extern "C" void port_coop_leave_begin(int frame) {
    using namespace port;
    if (LV.st == kLvFade) {
        LOG("f%d the quit poll keeps the scene change in this process (the room is ending)", frame);
        return;
    }
    // the player's own quit out of the room (Save and Quit, a pause quit row)
    if (C.host && !C.exiting && comms_hub_state() == kHubLive) {
        for (int i = 0; i < 3; ++i)
            if (send_sub(kCoopBootSubHostLeft, nullptr)) ++C.st.hostleft_sent;
        LOG("f%d host quitting the room: HOST_LEFT sent %u times (what the exit used to send)", frame, C.st.hostleft_sent);
    }
    if (C.host) coop_save_leave_flush();
    C.exiting = true;
    line_lower("the player quit");
    LV = LeaveState{};
    LV.st = kLvFade;
    LV.fade_t0 = tick();
    LV.from_level = true;
    lv_chain();
    LOG("f%d leaving the coop room in this process (%s): the ROM's fade, then the title tears the room down", frame,
        C.host ? "host" : "guest");
}


// ==== SEAMCOOP1: A COOP ROOM BOOTED IN PLACE (the launcher hand-off, kind "coop") ===========================================
// A coop room member was a process the launcher started with the room's names (LobbyLaunch.cs BuildAdventureEnv): main() installed the
// hub from them (tests/walk_window.cpp comms_hub_install_from_env), port_coop_boot_begin forced the boot-to-file names, registered the
// SYNB ranges and ran the gate before the ROM loaded (the host 3 s answering save requests; a guest 15 s for the host's word, the wait
// for the host's opening, the save fetch), ntr/backup.cpp opened the coop medium (the host's .coopsav, a guest's temp image seeded with
// the host's record), hal/mods.cpp latched the multiplayer mod list, and the title walked itself to file A (hal/title_entry.cpp
// port_title_skip_tick: the tap, the menu row, func_ov007_020b6134(0)). port_coop_handoff_apply does the same in a running game at its
// title, in that order, from the request's set / clear lists:
//   1. refusals before anything is touched: not a coop request, a room / hub / leave still up, a scene-booted process, a loaded code
//      mod (a plugin cannot be unloaded), the title not live or not built, a host request with no .coopsav. (No heap refusal: a coop
//      room asks no hub heap, hal/comms_conductor.cpp hub_heap_need answers 0 for party / coop, so a solo-carved heap is the child's.)
//   2. the clear list out of both environment copies, the set list into both (the readers still on getenv: this file, coop_save,
//      coop_world, party, star_popup, ntr/backup, the mod rule, the Adventure flag), SKIP_INTRO cleared as the boot contract does;
//   3. the session record filled from the set list (hal/comms_hub.h) and the Adventure flag's env latch read again;
//   4. the mods relatched under the multiplayer rule (hal/mods.cpp, after a level: the files a level read count as held);
//   5. the coop layer reset, the host's file prepared (coop_save_prepare), the save medium switched (ntr/backup.cpp select_coop) and
//      the title's three files read again from it (the ROM's own command 20, func_ov007_020cc600, as hal/menu_online.cpp does);
//   6. the hub installed live (comms_hub_install_in_place, SEAMJOIN1's) and the ghost layer decided on it (comms_sync_join_in_place);
//   7. the coop boot armed (this file's state, the SYNB stand-in handler, the two ranges registered once per process) and its gates
//      stepped once per game frame instead of blocking: the host's 3 s gate; a guest's 15 s wait for the host under the Joining plate
//      (its corner line after 3 s; B / Esc cancels back to the title in place), the host's opening, the save fetch (45 s);
//   8. the title walked to file A the way port_title_skip_tick does (the tap on the title screen, func_ov007_020aec94(9) on the main
//      menu, func_ov007_020b6134(0) on the settled file select); from there the ROM's own StartFile and the level-to-scene crossing
//      enter the room's level, and port_coop_boot_frame's barriers take over exactly as in a child.
// A failure after the record is filled undoes the record, the env and the medium; the caller answers "refused" and the game quits
// with exit code 8 (the contract), so a fresh process takes the room.
extern "C" {
extern char *data_ov007_0210342c;                             /* dScDSMT_c's scene global (hal/title_entry.cpp reads it the same way) */
void func_ov007_020aec94(int choice);                         /* src/func_ov007_020aec94.c: the main menu's choice */
void func_ov007_020b6134(int slot);                           /* src/func_ov007_020b6134.cpp: the file select's A / B / C handler */
unsigned char func_ov007_020cc600(int arg);                   /* src/game/stages/d_s_dsmt.cpp: 20 = read the three files again */
void port_touch_force_press(int x, int y, int frames);        /* hal/sub_screen.cpp */
void port_join_plate_raise_in_place(int server);              /* hal/menu_join_plate.cpp */
int port_join_plate_inplace_tick(void);
int port_join_plate_inplace_up(void);
void port_join_plate_inplace_end(const char *why);
}
namespace port {
void event_client_hub_reset();    /* hal/event_client.cpp */
void star_tally_session_reset();  /* hal/star_tally.cpp */
namespace {
enum { kIpNone = 0, kIpHostGate, kIpGuestHear, kIpGuestOpening, kIpGuestFetch, kIpDrive, kIpPressed };
struct InPlace {
    int st = kIpNone;
    ULONGLONG t0 = 0, tap_t = 0, drive_t0 = 0;
    bool chosen = false, opening_said = false;
} IP;

void env_put(const char *n, const char *v) {
    SetEnvironmentVariableA(n, v);
    _putenv_s(n, v ? v : "");
}

void reread_files(const char *why) {
    if (!data_ov007_0210342c) { LOG("the title's files are not read again (%s): no title scene", why); return; }
    func_ov007_020cc600(20);
    LOG("the title's three files read again from the medium now selected (the ROM's command 20; %s)", why);
}

const char *hv(int nset, const char *const *sn, const char *const *sv, const char *n) {
    for (int i = 0; i < nset; ++i)
        if (sn[i] && !std::strcmp(sn[i], n)) return sv[i];
    return nullptr;
}

void ip_undo(int nset, const char *const *sn, bool medium, bool relatched) {
    online_session_clear();
    for (int i = 0; i < nset; ++i) if (sn[i]) env_put(sn[i], nullptr);
    host_setting_adventure_env_reread();
    if (medium) { ntr::backup::select_coop(0); reread_files("the hand-off was undone"); }
    coop_world_session_reset();
    coop_save_session_reset();
    party_hits::session_reset();
    boot_state_reset();
    if (relatched) {   // BROADCAST-SEAMCOOP1-2: a refusal after changing things puts everything back before the exit
        unsigned extra = 0;
        const bool after_level = hub_game_heap_carved(&extra);
        if (!(after_level ? port_mods_relatch_after_level("the coop hand-off was refused")
                          : port_mods_relatch_at_title("the coop hand-off was refused")))
            LOG("the mods layer refused the switch back to the offline list (the refusal's exit follows)");
    }
    LOG("the hand-off's changes are undone: record, both environment copies, coop layer%s%s", medium ? ", medium" : "",
        relatched ? ", mods" : "");
}

/* the room taken down before its level (B on the plate, a gate that failed was handled by guest_fail): the title tears it down now */
void ip_cancel(const char *why) {
    IP = InPlace{};
    if (port_join_plate_inplace_up()) port_join_plate_inplace_end(why);
    LOG("the room is cancelled in place (%s): taking it down at the title", why);
    C.exiting = true;
    leave_at_title();
}

void drive_title() {
    char *g = data_ov007_0210342c;
    if (!g) return;
    char *sp = *(char **)(g + 8);
    char *ep = *(char **)(g + 4);
    if (!sp) return;
    const int state = *(short *)sp, req = *(short *)(sp + 2), fr = *(int *)(sp + 0xc);
    if (state == 11) {
        if (fr >= 8 && tick() - IP.tap_t >= 1000) {
            IP.tap_t = tick();
            port_touch_force_press(128, 96, 2);
            LOG("title walk: the title screen waits for a touch (top-state 11): one at (128,96), as port_title_skip_tick gives");
        }
        return;
    }
    if (state != 0 || req != -1 || fr < 8) return;
    if (ep && *(short *)ep == 5 && *(short *)(ep + 2) == -1) {
        IP.st = kIpPressed;
        func_ov007_020b6134(0);
        LOG("title walk: the file select is up; pressed file A through the ROM's own func_ov007_020b6134(0), as a coop child's "
            "SM64DS_BOOT_FILE=0 does (top-state now asks for %d)", (int)*(short *)(sp + 2));
        return;
    }
    if (IP.chosen || *(int *)(g + 0x10) != 0 || !*(char **)(g + 0x28)) return;
    IP.chosen = true;
    func_ov007_020aec94(9);
    LOG("title walk: the main menu: chose the file select through the ROM's own func_ov007_020aec94(9)");
}

void host_serve() {
    if (C.host && comms_hub_state() == kHubLive) {
        if (tick() - C.last_beat >= (C.released ? (ULONGLONG)kBeatMs : (ULONGLONG)kBeatGatherMs)) send_state();
        port_coop_save_frame();
    }
}

void inplace_step() {
    if (IP.st == kIpNone) return;
    if (LV.st != kLvNone) { IP = InPlace{}; return; }   // the room is ending (guest_fail / the host left): the leave owns it now
    comms_hub_pump();
    switch (IP.st) {
    case kIpHostGate: {
        host_serve();
        const unsigned others = present_others();
        const bool served = others && (C.req_seen & others) == others && tick() - C.last_req_ms >= 500;
        if (served || tick() - IP.t0 >= 3000) {
            LOG("host gate (in place): %s after %u ms (guests present %x asked %x); walking the title to file A",
                served ? "every present guest served" : "done", (unsigned)(tick() - IP.t0), others, C.req_seen);
            IP.st = kIpDrive;
            IP.drive_t0 = tick();
        }
        return;
    }
    case kIpGuestHear:
        if (port_join_plate_inplace_tick()) { ip_cancel("the player cancelled while waiting for the host"); return; }
        if (C.state_heard) {
            LOG("host heard after %u ms (in place; phase %d go %d closed %d)", (unsigned)(tick() - IP.t0), C.hs.phase, C.hs.go, C.hs.closed);
            IP.st = kIpGuestOpening;
            IP.t0 = tick();
        } else if ((int)(tick() - IP.t0) >= env_int("SM64DS_COOPBOOT_HEAR_MS", 15000)) {
            char why[64];
            std::snprintf(why, sizeof why, "no word from the host in %d s", env_int("SM64DS_COOPBOOT_HEAR_MS", 15000) / 1000);
            port_join_plate_inplace_end("no host");
            IP.st = kIpNone;
            guest_fail(why);
        }
        return;
    case kIpGuestOpening:
        if (port_join_plate_inplace_tick()) { ip_cancel("the player cancelled while waiting for the host's opening"); return; }
        check_host_gone();
        if (C.exiting) return;
        if (C.hs.go && C.hs.phase == kCoopPhaseIntro) {
            if (!IP.opening_said) { IP.opening_said = true; LOG("joined during the opening: waiting for it to end (in place)"); }
            return;
        }
        IP.st = kIpGuestFetch;
        IP.t0 = tick();
        return;
    case kIpGuestFetch: {
        if (port_join_plate_inplace_tick()) { ip_cancel("the player cancelled while the save was fetched"); return; }
        const int r = coop_save_fetch_step(env_int("SM64DS_COOPBOOT_FETCH_MS", 45000));
        if (r < 0) {
            port_join_plate_inplace_end("no save");
            IP.st = kIpNone;
            char why[64];
            std::snprintf(why, sizeof why, "the host did not send the save in %d s", env_int("SM64DS_COOPBOOT_FETCH_MS", 45000) / 1000);
            guest_fail(why);
            return;
        }
        if (r == 0) return;
        reread_files("the host's record is in the guest's image");
        port_join_plate_inplace_end("the host answered and its save is here: the title walks to the room");
        IP.st = kIpDrive;
        IP.drive_t0 = tick();
        return;
    }
    case kIpDrive:
        host_serve();
        drive_title();
        if (IP.st == kIpDrive && tick() - IP.drive_t0 > 30000) ip_cancel("the title did not reach the file select in 30 s");
        return;
    case kIpPressed:
        host_serve();
        if (data_02092664 == 3 || C.at_first_frame) {
            LOG("the title handed the game to the room's level (in place, as a coop child's title does)");
            IP.st = kIpNone;
        }
        return;
    default: return;
    }
}

}  // namespace
}  // namespace port

/* hal/handoff_poll.cpp (the registry): the title poll only picks a request up when no session is up */
extern "C" int port_coop_session_busy(void) {
    using namespace port;
    return (C.on || LV.st != kLvNone || IP.st != kIpNone) ? 1 : 0;
}

extern "C" const char *port_coop_handoff_apply(int nset, const char *const *sn, const char *const *sv, int nclear, const char *const *cn) {
    using namespace port;
    static char buf[240];
    const char *cv = hv(nset, sn, sv, "SM64DS_COOP");
    const char *rk = hv(nset, sn, sv, "SM64DS_ROOM_KIND");
    if (!cv || std::strcmp(cv, "1") || !rk || std::strcmp(rk, "coop")) return "the request is not a coop room (SM64DS_COOP=1, SM64DS_ROOM_KIND=coop)";
    if (C.on || LV.st != kLvNone || IP.st != kIpNone) return "a coop room is still up in this process";
    if (comms_hub_active() || comms_hub_state() != kHubOff || comms_hub_arena()) return "a hub session is still up in this process";
    if (env_any("SM64DS_SCENE")) return "a scene-booted process";
    for (int i = 0; i < port_mods_count(); ++i) {
        const char *k = port_mods_kind(i);
        if (k && !std::strcmp(k, "code")) {
            std::snprintf(buf, sizeof buf, "a code mod is loaded (%s) and a plugin cannot be unloaded", port_mods_id(i));
            return buf;
        }
    }
    if (port_scene_live_id() != 1) return "the title is not the live scene";
    if (!data_ov007_0210342c) return "the title scene is not built yet";
    const char *hs = hv(nset, sn, sv, "SM64DS_COOP_HOST");
    const bool host = hs && !std::strcmp(hs, "1");
    if (host) {
        const char *p = hv(nset, sn, sv, "SM64DS_COOP_SAVE");
        const size_t pl = p ? std::strlen(p) : 0;
        if (pl < 9 || _stricmp(p + pl - 8, ".coopsav")) return "the host's request names no .coopsav file (SM64DS_COOP_SAVE)";
        /* A NEW FILE'S OPENING ON A TITLE REACHED AFTER A LEVEL (SEAMCLEAN1: the guard that refused this is gone). The
           level-to-scene crossing did not run the hub's one-player opening seat (review SEAMJOIN1 C); lane SEAMJOIN2 fixed that in
           hal/level_change.cpp (g_cross_opening: port_adventure_hub_opening_begin on the crossing, the same test as the title
           fall-through in tests/walk_window.cpp), which does not look at the room kind, so a coop host's new file picked on a
           title reached after a level plays the room's opening on the one-player seat as a boot-title host does. */
    }
    LOG("coop hand-off: booting the room in this process as the %s (%d set, %d clear)", host ? "host" : "guest", nset, nclear);
    // 2. the environment a child would be started with
    int ncl = 0;
    for (int i = 0; i < nclear; ++i) if (cn[i] && env_any(cn[i])) { env_put(cn[i], nullptr); ++ncl; }
    for (int i = 0; i < nset; ++i) if (sn[i]) env_put(sn[i], sv[i] && *sv[i] ? sv[i] : nullptr);
    env_put("SM64DS_SKIP_INTRO", nullptr);
    // 3. the session record
    online_session_begin();
    int nf = 0;
    for (int f = 0; f < kSessFields; ++f) {
        const char *v = hv(nset, sn, sv, online_session_name((OnlineSessionField)f));
        online_session_set((OnlineSessionField)f, v);
        if (v && *v) ++nf;
    }
    host_setting_adventure_env_reread();
    std::fprintf(stderr, "[seamless] coop hand-off: %d name(s) cleared and %d set in both environment copies; session record filled "
                 "(%d field(s), room %s); adventure %d, multiplayer mod rule %d\n", ncl, nset, nf, rk, adventure_ghost_mode() ? 1 : 0,
                 port_mods_multiplayer());
    std::fflush(stderr);
    // 4. the mods under the multiplayer rule
    unsigned extra = 0;
    const bool after_level = hub_game_heap_carved(&extra);
    if (!(after_level ? port_mods_relatch_after_level("booting a coop room") : port_mods_relatch_at_title("booting a coop room"))) {
        ip_undo(nset, sn, false, false);
        return "the mods layer refused the switch to the online list";
    }
    // 5. the coop layer, the host's file, the medium, the title's files
    coop_world_session_reset();
    coop_save_session_reset();
    party_hits::session_reset();
    boot_state_reset();
    if (host && !coop_save_prepare()) {
        ip_undo(nset, sn, false, true);
        return "the host's coop save could not be prepared";
    }
    ntr::backup::select_coop(host ? 2 : 1);
    reread_files("the coop medium");
    // 6. the hub and the ghost layer
    comms_conductor_leave_reset();
    hub_presence_leave_reset();
    event_client_hub_reset();
    star_tally_session_reset();
    const char *hwhy = nullptr;
    if (!comms_hub_install_in_place(&hwhy)) {
        std::snprintf(buf, sizeof buf, "the hub could not be installed in place (%s)", hwhy ? hwhy : "?");
        ip_undo(nset, sn, true, true);
        return buf;
    }
    if (!comms_sync_join_in_place()) {
        comms_hub_teardown("the ghost layer did not decide on");
        comms_sync_leave_reset();
        ip_undo(nset, sn, true, true);
        return "the ghost layer did not come up on the hub";
    }
    // 7. the coop boot, armed as port_coop_boot_begin arms it
    C.init = true;
    C.on = true;
    C.host = host;
    C.inplace = true;
    C.title_seen = true;
    C.t_boot = tick();
    coop_save_set_external_dispatch(true);
    hub_on_kind(kHubCoopCtl, gate_on_ctl, nullptr);
    if (!g_ranges_registered) {
        g_ranges_registered = true;
        const bool r1 = coop_ctl_register(0x20, 0x2f, sub_save, nullptr);
        const bool r2 = coop_ctl_register(kCoopBootSubLo, kCoopBootSubHi, sub_boot, nullptr);
        LOG("SYNB ranges registered with the world core (once per process): 0x20-0x2f %d, 0x30-0x3f %d", r1 ? 1 : 0, r2 ? 1 : 0);
    }
    if (host && !g_host_atexit) { g_host_atexit = true; std::atexit(host_atexit); }
    IP = InPlace{};
    IP.st = host ? kIpHostGate : kIpGuestHear;
    IP.t0 = tick();
    if (!host) port_join_plate_raise_in_place(0);
    port_join_settings_rearm();
    lv_chain();
    LOG("coop boot (in place): role=%s, the gate is stepped once per game frame%s", host ? "host" : "guest",
        host ? "" : " under the Joining plate (B / Esc cancels)");
    return nullptr;
}
