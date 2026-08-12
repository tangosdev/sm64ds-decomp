// Shared crash probe for the gate smokes: prints the module-relative fault
// address and a frame-pointer backtrace, resolvable against the /MAP file.
#ifndef PORT_FAULT_PROBE_H
#define PORT_FAULT_PROBE_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
/* Linux: crash forensics (VEH, detours, debug registers, CreateFileA dumps) is
   a later lane. This side only needs the header to COMPILE and to hand the rest
   of the port the same external symbols/macros, as no-op or minimal-POSIX
   stubs. Pull in the HAL shim for the Win32-alias types this header still names
   in its signatures (MAX_PATH, HANDLE, DWORD, ...). It is include-guarded
   (PORT_HOST_PLATFORM_LINUX_H) so a double-include with walk_window.cpp is
   safe. */
#include "../hal/host_platform_linux.h"
/* EXCEPTION_POINTERS is a raw-Win32 type the exported seams' signatures name;
   forward-declare a dummy so the prototypes are valid on Linux. */
#ifndef _EXCEPTION_POINTERS
struct _EXCEPTION_POINTERS;
typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS;
#endif
#endif
#include <stdio.h>

/* A stack word is only a RETURN ADDRESS if it points at code. The probe used
   to accept any word inside the module's [base, base+0x200000) window, but that
   window also covers the module's .data/.rdata/.bss: a data pointer parked on
   the stack (e.g. &ac_trap_report's static msg[128], a constant .data address)
   would be printed as a "module return word", and the crash classifier keys
   families on the FIRST such word -- so a data address seeded bogus families
   (the +0019be40 cluster, byte-identical across builds because it is data, not
   shifting code). This predicate gates labelling on executability: a candidate
   is a return address only if its page is COMMITTED and carries an EXECUTE
   protection (PAGE_EXECUTE / _READ / _READWRITE / _WRITECOPY). .data/.rdata/
   .bss (RW or RO), heap, and stack pages fail it and are no longer labelled.
   VirtualQuery is used rather than parsing PE section headers because it needs
   no module bookkeeping the probe does not already have and reflects the real
   runtime page protection of whatever module the word lands in. Cost is one
   syscall per candidate; the scans cap at 96/512 words so this stays cheap, and
   it only runs in the crash path. */
static int port_addr_is_exec(uintptr_t addr)
{
#ifdef _WIN32
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery((LPCVOID)addr, &mbi, sizeof mbi) != sizeof mbi)
        return 0;
    if (mbi.State != MEM_COMMIT)
        return 0;
    return (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
                           PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))
           != 0;
#else
    /* Linux: crash forensics is a later lane (see the header note at the top of
       this file), and the POSIX answer here is parsing /proc/self/maps for the
       x permission, which belongs with the rest of that work.
       Answering 0 rather than 1 in the meantime is the deliberate direction: 0
       means no stack word is ever LABELLED a return address, so Linux
       backtraces are unlabelled. 1 would label data addresses as code, which is
       precisely the bug this predicate was added to fix -- it is what seeded the
       bogus +0019be40 family. An unlabelled backtrace is missing information; a
       wrongly labelled one is misinformation that reaches the crash classifier
       and invents families. */
    (void)addr;
    return 0;
#endif
}

/* The ROM's list walker (func_02043fdc) parks the node it is processing in
   data_020a4b68 before every callback, so at fault time node[2] names the
   actor whose phase code was running -- the question every actor-phase crash
   asks first. Weak so the probe still links in TUs without the engine. */
#ifdef _WIN32
#ifdef __cplusplus
extern "C" int *data_020a4b68;
extern "C" __declspec(selectany) int *port_fault_no_walker = 0;
#else
extern int *data_020a4b68;
__declspec(selectany) int *port_fault_no_walker = 0;
#endif
#pragma comment(linker, "/alternatename:_data_020a4b68=_port_fault_no_walker")
#else /* Linux/g++: a plain extern decl plus a weak fallback definition, so a
         TU that never defines the real symbol still links against the weak 0. */
#ifdef __cplusplus
extern "C" int *data_020a4b68 __attribute__((weak)) = 0;
#else
int *data_020a4b68 __attribute__((weak)) = 0;
#endif
#endif

#ifdef _WIN32
static LONG WINAPI port_fault_probe(EXCEPTION_POINTERS *ep)
{
    char *base = (char *)GetModuleHandleA(0);
    fprintf(stderr, "FAULT code %08lx at +0x%08x accessing %08x\n",
            ep->ExceptionRecord->ExceptionCode,
            (unsigned)((char *)ep->ExceptionRecord->ExceptionAddress - base),
            (unsigned)(ep->ExceptionRecord->NumberParameters > 1
                       ? ep->ExceptionRecord->ExceptionInformation[1] : 0));
    if (data_020a4b68 && !IsBadReadPtr(data_020a4b68, 12)) {
        char *a = (char *)(uintptr_t)data_020a4b68[2];
        fprintf(stderr, "  walker node %p actor %p id 0x%x\n",
                (void *)data_020a4b68, (void *)a,
                (a && !IsBadReadPtr(a, 0x10)) ? *(unsigned short *)(a + 0xc)
                                              : 0xffffu);
    }
    void *frames[12];
    unsigned n = CaptureStackBackTrace(0, 12, frames, 0);
    for (unsigned i = 0; i < n; ++i)
        fprintf(stderr, "  frame %u: +0x%08x\n", i,
                (unsigned)((char *)frames[i] - base));
    /* raw return-address candidates off the FAULTING stack (the frames
       above are the handler's own); module-relative, /MAP-resolvable */
    if (ep->ContextRecord) {
        CONTEXT *cx = ep->ContextRecord;
        fprintf(stderr,
                "  regs eax=%08x ecx=%08x edx=%08x ebx=%08x esi=%08x "
                "edi=%08x ebp=%08x\n",
                (unsigned)cx->Eax, (unsigned)cx->Ecx, (unsigned)cx->Edx,
                (unsigned)cx->Ebx, (unsigned)cx->Esi, (unsigned)cx->Edi,
                (unsigned)cx->Ebp);
    }
    if (ep->ContextRecord) {
        unsigned *sp = (unsigned *)ep->ContextRecord->Esp;
        for (int i = 0; i < 96; ++i) {
            unsigned v;
            if (IsBadReadPtr(sp + i, 4)) break;
            v = sp[i];
            if (v >= (unsigned)(uintptr_t)base &&
                v < (unsigned)(uintptr_t)base + 0x200000 &&
                port_addr_is_exec((uintptr_t)v))
                fprintf(stderr, "  stack[%02d] +0x%08x\n", i,
                        (unsigned)(v - (unsigned)(uintptr_t)base));
        }
    }
    fflush(stderr);
    return EXCEPTION_EXECUTE_HANDLER;
}

/* ---- crash.txt: the host crash screen -------------------------------------
   The stderr print above needs a live CRT and a captured stderr; a crash that
   kills the CRT (stack overflow, stdio corruption) or a run without redirection
   leaves nothing. This writes the same facts to crash.txt NEXT TO THE EXE with
   raw Win32 only -- static buffers, CreateFileA/WriteFile, hand-rolled hex, no
   stdio -- so the next silent death still names an address. The DS game ships
   its own crash screen (ShowCrashScreen); this is the host's.

   Installed twice: a FIRST-chance vectored handler on the codes that are fatal
   in this codebase (nothing here handles an AV), so the file is written even
   when the unhandled-filter never runs (exhausted stack, CRT death); and from
   the unhandled filter for everything that does reach it. CREATE_ALWAYS: the
   file describes the LAST crash. Offsets resolve against the /MAP file the
   build already writes (build/port/walk_window.map): section offset =
   printed offset - 0x1000 for .text symbols.

   STATUS_STACK_BUFFER_OVERRUN (0xC0000409 via __fastfail/int 29h) never raises
   a catchable exception on modern CPUs; if crash.txt stays absent across a
   repro, a /GS cookie or fastfail path is the remaining suspect. */
static void port_crash_hex(char *dst, unsigned v)
{
    static const char h[] = "0123456789abcdef";
    int i;
    for (i = 0; i < 8; ++i)
        dst[i] = h[(v >> (28 - 4 * i)) & 0xf];
}
#endif /* _WIN32 (port_fault_probe .. port_crash_hex) */

/* ---- rotating rich dump: %TEMP%\sm64ds-crashes\crash-<ts>-<pid>.txt ---------
   crash.txt/exit.txt above stay exactly as they were -- one file next to the
   exe describing the LAST death, cheap and always present. This adds a SECOND
   sink under %TEMP%\sm64ds-crashes\ that keeps a rolling history (the newest 5)
   and carries "anything you would need": the crash.txt facts PLUS the game
   context (level, player position, walker actor id + class name), the whole
   SM64DS_* environment, the build's git tip, and a tail of the active playlog.

   Split of work so the in-crash path stays simple and re-entrancy-safe:
     - port_crash_dir_boot()   runs at STARTUP: makes the directory and prunes
       old dumps to the newest 4 (leaving room for the one a crash will add).
       Filesystem enumeration and DeleteFile happen here, never in-crash.
     - port_rich_dump()        runs IN-CRASH: builds one static buffer with
       raw Win32 + hand-rolled hex (no stdio, like crash.txt), writes ONE file,
       and only THEN -- as the very last step -- copies the playlog tail, so a
       failure reading the still-open playlog loses nothing already written.

   The context globals are weak like the walker/frame pointers so a TU that does
   not define them (a bare smoke) still links against the fallbacks. */
#ifdef _WIN32
#ifdef __cplusplus
extern "C" signed char data_0209f2f8;                      /* current level */
extern "C" __declspec(selectany) signed char port_fault_no_level = -1;
extern "C" void *data_0209f394[];                          /* per-player Actor* */
extern "C" __declspec(selectany) void *port_fault_no_players[8] = {0};
extern "C" unsigned char data_0209f250;                    /* local player idx */
extern "C" __declspec(selectany) unsigned char port_fault_no_pidx = 0;
/* the build's git tip, embedded by CMake (host-src/port_gittip.c); a weak
   fallback keeps a TU without the generated file linking. */
extern "C" const char port_build_gittip[];
extern "C" __declspec(selectany) const char port_fault_no_gittip[] = "unknown";
/* the active playlog path (walk_window's g_playlog, exposed as a pointer);
   null/"off" means stderr was left on the console, nothing to tail. */
extern "C" const char *port_playlog_path;
extern "C" __declspec(selectany) const char *port_fault_no_playlog = 0;
#else
extern signed char data_0209f2f8;
__declspec(selectany) signed char port_fault_no_level = -1;
extern void *data_0209f394[];
__declspec(selectany) void *port_fault_no_players[8] = {0};
extern unsigned char data_0209f250;
__declspec(selectany) unsigned char port_fault_no_pidx = 0;
extern const char port_build_gittip[];
__declspec(selectany) const char port_fault_no_gittip[] = "unknown";
extern const char *port_playlog_path;
__declspec(selectany) const char *port_fault_no_playlog = 0;
#endif
#pragma comment(linker, "/alternatename:_data_0209f2f8=_port_fault_no_level")
#pragma comment(linker, "/alternatename:_data_0209f394=_port_fault_no_players")
#pragma comment(linker, "/alternatename:_data_0209f250=_port_fault_no_pidx")
#pragma comment(linker, "/alternatename:_port_build_gittip=_port_fault_no_gittip")
#pragma comment(linker, "/alternatename:_port_playlog_path=_port_fault_no_playlog")
#else /* Linux/g++: weak fallback definitions so a bare-smoke TU that never
         defines the real engine symbol still links against these zeros. */
/* PORT_FAULT_PROBE_STRONG_GLOBALS: a TU that defines port_playlog_path /
   port_last_frame STRONGLY itself (walk_window: the real g_playlog pointer and
   the live frame counter) sets this before including, so the header does NOT
   also emit its weak fallback for those two -- two definitions in one TU is a
   GCC redefinition error, unlike Windows where the header only declares them for
   that TU. Every other symbol keeps its weak fallback, and bare smokes (no macro)
   still get all the weak fallbacks including these two. */
#ifdef __cplusplus
extern "C" signed char data_0209f2f8 __attribute__((weak)) = -1;
extern "C" void *data_0209f394[8] __attribute__((weak)) = {0};
extern "C" unsigned char data_0209f250 __attribute__((weak)) = 0;
extern "C" const char port_build_gittip[] __attribute__((weak)) = "unknown";
#ifndef PORT_FAULT_PROBE_STRONG_GLOBALS
extern "C" const char *port_playlog_path __attribute__((weak)) = 0;
#endif
#else
signed char data_0209f2f8 __attribute__((weak)) = -1;
void *data_0209f394[8] __attribute__((weak)) = {0};
unsigned char data_0209f250 __attribute__((weak)) = 0;
const char port_build_gittip[] __attribute__((weak)) = "unknown";
#ifndef PORT_FAULT_PROBE_STRONG_GLOBALS
const char *port_playlog_path __attribute__((weak)) = 0;
#endif
#endif
#endif
/* Class-name resolution goes through a weak DATA function pointer, not a direct
   call. walk_window (which links hal/actor_registry.cpp) sets it to the real
   port_actor_class_name; the bare smokes -- which install the probe but do NOT
   link that HAL -- leave it null and the dump prints "?". A __declspec(selectany)
   pointer defaulting to 0 is legal (data, external linkage, one deduped body)
   and needs no alternatename gymnastics. hal/actor_registry.cpp's
   port_actor_registry_install wires it up. On Linux the same shape is a weak
   definition defaulting to 0. */
typedef const char *(*port_classname_fn)(unsigned id);
#ifdef _WIN32
#ifdef __cplusplus
extern "C" __declspec(selectany) port_classname_fn port_classname_resolver = 0;
#else
__declspec(selectany) port_classname_fn port_classname_resolver = 0;
#endif
#else /* Linux/g++ */
#ifdef __cplusplus
extern "C" port_classname_fn port_classname_resolver __attribute__((weak)) = 0;
#else
port_classname_fn port_classname_resolver __attribute__((weak)) = 0;
#endif
#endif

/* port_last_frame is declared with its weak fallback later (the exit.txt
   block); the rich dump above uses it, so hoist just the DECLARATION here. The
   alternatename that gives it a weak body stays in the one place, below (Win32).
   On Linux the weak fallback definition lives HERE (the exit.txt block below is
   entirely inside #ifdef _WIN32), so a bare-smoke TU still links. */
#ifdef _WIN32
#ifdef __cplusplus
extern "C" int port_last_frame;
#else
extern int port_last_frame;
#endif
#else /* Linux/g++ */
#ifndef PORT_FAULT_PROBE_STRONG_GLOBALS
#ifdef __cplusplus
extern "C" int port_last_frame __attribute__((weak)) = -1;
#else
int port_last_frame __attribute__((weak)) = -1;
#endif
#else /* the strong-globals TU declares it; it defines it itself */
#ifdef __cplusplus
extern "C" int port_last_frame;
#else
extern int port_last_frame;
#endif
#endif
#endif

#ifdef _MSC_VER
/* Non-zero only while a test harness is deliberately raising a fault it means
   to catch: port/unmatched/func_02043fdc.cpp's SM64DS_TEST_QUARANTINE hook,
   which exists only in a PORT_TEST_HOOKS build. port_crash_write_file below
   writes nothing while it is set -- see the note there for why suppressing
   beats tagging. The definition is deliberately a duplicate of the one in that
   file rather than an extern: nothing defines the symbol strongly, both
   definitions are selectany with the same initialiser, so the linker folds them
   into one object that both sides read and write, and a target that links
   either file alone still resolves it. */
#ifdef __cplusplus
extern "C" __declspec(selectany) int port_fault_synthetic = 0;
#else
__declspec(selectany) int port_fault_synthetic = 0;
#endif

#else
/* GCC has no __declspec(selectany). The ELF equivalent of "duplicate
   definitions fold into one" is a weak definition, and the contract is the
   same: this header and func_02043fdc.cpp may both define the symbol, the
   linker keeps one object both sides read and write, and a target linking
   either file alone still resolves it. */
#ifdef __cplusplus
extern "C" __attribute__((weak)) int port_fault_synthetic = 0;
#else
__attribute__((weak)) int port_fault_synthetic = 0;
#endif
#endif  /* _MSC_VER */

#ifdef _WIN32
/* The crash-dump directory, resolved once at boot from %TEMP%. */
static char port_crash_dir[MAX_PATH];

/* Newest-N pruning: enumerate crash-*.txt, and while more than `keep` exist,
   delete the oldest by last-write time. Called at boot with keep=4 (room for
   the crash about to be written) and could be called with keep=5 after a write;
   boot-only is enough because each run adds at most one dump. Uses FindFirst/
   FindNext + DeleteFile -- no stdio, safe to call before the CRT is warm. */
static void port_crash_prune(int keep)
{
    static char pat[MAX_PATH + 32];
    WIN32_FIND_DATAA fd;
    HANDLE h;
    /* small fixed table: names + write times. Older than 64 dumps in a temp
       dir would be pathological; cap and delete the overflow oldest-first. */
    static char names[64][64];
    static FILETIME times[64];
    int n = 0, i, j;
    if (!port_crash_dir[0])
        return;
    lstrcpyA(pat, port_crash_dir);
    lstrcatA(pat, "\\crash-*.txt");
    h = FindFirstFileA(pat, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return;
    do {
        if (n < 64) {
            lstrcpynA(names[n], fd.cFileName, sizeof names[0]);
            times[n] = fd.ftLastWriteTime;
            ++n;
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    /* selection: while count > keep, find and delete the oldest remaining */
    while (n > keep) {
        int oldest = -1;
        for (i = 0; i < 64; ++i) {
            if (!names[i][0]) continue;
            if (oldest < 0 ||
                CompareFileTime(&times[i], &times[oldest]) < 0)
                oldest = i;
        }
        if (oldest < 0)
            break;
        {
            static char full[MAX_PATH + 80];
            lstrcpyA(full, port_crash_dir);
            lstrcatA(full, "\\");
            lstrcatA(full, names[oldest]);
            DeleteFileA(full);
        }
        names[oldest][0] = 0;
        --n;
    }
    (void)j;
}

/* Boot: resolve %TEMP%\sm64ds-crashes, create it, prune to 4. Idempotent. */
static void port_crash_dir_boot(void)
{
    DWORD n = GetEnvironmentVariableA("TEMP", port_crash_dir, MAX_PATH - 24);
    if (!n || n >= MAX_PATH - 24)
        n = GetEnvironmentVariableA("TMP", port_crash_dir, MAX_PATH - 24);
    if (!n || n >= MAX_PATH - 24) {
        /* last resort: next to the exe */
        n = GetModuleFileNameA(0, port_crash_dir, MAX_PATH - 24);
        while (n && port_crash_dir[n - 1] != 92) --n;
        port_crash_dir[n] = 0;
        lstrcatA(port_crash_dir, "sm64ds-crashes");
    } else {
        if (port_crash_dir[n - 1] == 92) port_crash_dir[n - 1] = 0;
        lstrcatA(port_crash_dir, "\\sm64ds-crashes");
    }
    CreateDirectoryA(port_crash_dir, 0);
    port_crash_prune(4);
}

/* Copy the last ~`maxbytes` of the active playlog into the open dump handle.
   Done as the LAST step of the dump so a failure here (the playlog is still
   open for writing by the CRT's redirected stderr; FILE_SHARE_READ lets us in)
   costs nothing already written. Raw Win32, bounded read. */
static void port_crash_append_playlog(HANDLE out)
{
    const char *pl = port_playlog_path;
    HANDLE f;
    LARGE_INTEGER sz;
    static char tail[8192];      /* ~100 lines of the terse diagnostics */
    DWORD rd, wr;
    const char *hdr = "\r\n---- playlog tail ----\r\n";
    if (!pl || pl[0] == 0 || (pl[0] == 'o' && pl[1] == 'f' && pl[2] == 'f'))
        return;
    f = CreateFileA(pl, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (f == INVALID_HANDLE_VALUE)
        return;
    WriteFile(out, hdr, (DWORD)lstrlenA(hdr), &wr, 0);
    if (GetFileSizeEx(f, &sz)) {
        LONG hi = 0;
        if (sz.QuadPart > (LONGLONG)sizeof tail)
            SetFilePointer(f, (LONG)(sz.QuadPart - (LONGLONG)sizeof tail),
                           &hi, FILE_BEGIN);
        while (ReadFile(f, tail, sizeof tail, &rd, 0) && rd)
            WriteFile(out, tail, rd, &wr, 0);
    }
    CloseHandle(f);
}

/* The rich dump itself. `ep` may be null (the orderly-exit path passes null and
   a code); `reason` is a short tag ("exception", "exit", "quarantine",
   "quarantine-fatal"). One static buffer, hand-rolled hex, one file. */
static void port_rich_dump(EXCEPTION_POINTERS *ep, unsigned code,
                           const char *reason)
{
    static char path[MAX_PATH + 64];
    static char buf[4096];
    char *base = (char *)GetModuleHandleA(0);
    unsigned p = 0;
    SYSTEMTIME st;
    HANDLE f;
    if (!port_crash_dir[0])
        port_crash_dir_boot();
    GetLocalTime(&st);
    /* crash-YYYYMMDD-HHMMSS-<pid>.txt */
    {
        char *d = path;
        lstrcpyA(d, port_crash_dir);
        d += lstrlenA(d);
        *d++ = '\\';
        lstrcpyA(d, "crash-");
        d += 6;
        /* build the timestamp digits by hand (port_crash_hex is 8-wide hex,
           wrong for decimal date fields), simple 2/4-digit decimal helper */
#define PORT_DEC(val, w) do { int _v = (int)(val), _w = (w), _k; \
        for (_k = _w - 1; _k >= 0; --_k) { d[_k] = (char)('0' + _v % 10); \
        _v /= 10; } d += _w; } while (0)
        PORT_DEC(st.wYear, 4);  PORT_DEC(st.wMonth, 2); PORT_DEC(st.wDay, 2);
        *d++ = '-';
        PORT_DEC(st.wHour, 2);  PORT_DEC(st.wMinute, 2); PORT_DEC(st.wSecond, 2);
        *d++ = '-';
        PORT_DEC(GetCurrentProcessId(), 6);
#undef PORT_DEC
        lstrcpyA(d, ".txt");
    }
#define PORT_RD_STR(s) do { const char *q = (s); \
        while (*q && p < sizeof buf - 16) buf[p++] = *q++; } while (0)
#define PORT_RD_HEX(v) do { if (p < sizeof buf - 16) { \
        port_crash_hex(buf + p, (unsigned)(v)); p += 8; } } while (0)
#define PORT_RD_DEC(v) do { int _v=(int)(v),_n=0; char _t[12]; \
        if (_v<0){ if(p<sizeof buf-16) buf[p++]='-'; _v=-_v; } \
        do { _t[_n++]=(char)('0'+_v%10); _v/=10; } while(_v&&_n<11); \
        while(_n && p<sizeof buf-16) buf[p++]=_t[--_n]; } while (0)
    PORT_RD_STR("sm64ds-decomp port crash dump\r\nreason    ");
    PORT_RD_STR(reason ? reason : "?");
    PORT_RD_STR("\r\ngittip    ");
    PORT_RD_STR(port_build_gittip);
    PORT_RD_STR("\r\ncode      ");
    PORT_RD_HEX(ep ? ep->ExceptionRecord->ExceptionCode : code);
    if (ep) {
        PORT_RD_STR("\r\naddress   ");
        PORT_RD_HEX((uintptr_t)ep->ExceptionRecord->ExceptionAddress);
        PORT_RD_STR("\r\nmodule    ");
        PORT_RD_HEX((uintptr_t)base);
        PORT_RD_STR("\r\noffset    +");
        PORT_RD_HEX((uintptr_t)ep->ExceptionRecord->ExceptionAddress
                    - (uintptr_t)base);
        if (ep->ExceptionRecord->NumberParameters > 1) {
            PORT_RD_STR("\r\naccess    ");
            PORT_RD_HEX(ep->ExceptionRecord->ExceptionInformation[0]);
            PORT_RD_STR(" at ");
            PORT_RD_HEX(ep->ExceptionRecord->ExceptionInformation[1]);
        }
    } else {
        PORT_RD_STR("\r\nmodule    ");
        PORT_RD_HEX((uintptr_t)base);
    }
    /* COSMETIC: walk_window updates port_last_frame at the END of each frame,
       so a fault early in a tick reports the previous frame's number, and a
       death before the first frame completes reports -1. Off-by-one at worst;
       accepted rather than moving the store into the hot loop's head. */
    PORT_RD_STR("\r\nframe     ");
    PORT_RD_DEC(port_last_frame);
    PORT_RD_STR("\r\nlevel     ");
    PORT_RD_DEC(data_0209f2f8);
    /* current player position: data_0209f394[local idx] is the Actor*, pos at
       actor+0x5c/+0x60/+0x64 (three fx32 words). Guarded reads. */
    {
        void *pl = 0;
        if (data_0209f250 < 8)
            pl = data_0209f394[data_0209f250];
        if (pl && !IsBadReadPtr(pl, 0x68)) {
            char *a = (char *)pl;
            PORT_RD_STR("\r\nplayer    actor ");
            PORT_RD_HEX((uintptr_t)pl);
            PORT_RD_STR(" pos ");
            PORT_RD_DEC(*(int *)(a + 0x5c) >> 12);
            PORT_RD_STR(",");
            PORT_RD_DEC(*(int *)(a + 0x60) >> 12);
            PORT_RD_STR(",");
            PORT_RD_DEC(*(int *)(a + 0x64) >> 12);
        } else {
            PORT_RD_STR("\r\nplayer    (none)");
        }
    }
    /* the walker's current actor (node[2]) + its class name. data_020a4b68's
       first word is the node the walk parked before the faulting callback. */
    if (data_020a4b68 && !IsBadReadPtr((void *)data_020a4b68, 12)) {
        char *a = (char *)(uintptr_t)data_020a4b68[2];
        unsigned id = (a && !IsBadReadPtr(a, 0x10))
                      ? *(unsigned short *)(a + 0xc) : 0xffffu;
        PORT_RD_STR("\r\nwalker    node ");
        PORT_RD_HEX((uintptr_t)data_020a4b68);
        PORT_RD_STR(" actor ");
        PORT_RD_HEX((uintptr_t)a);
        PORT_RD_STR(" id ");
        PORT_RD_DEC(id == 0xffffu ? -1 : (int)id);
        if (id != 0xffffu) {
            PORT_RD_STR(" class ");
            PORT_RD_STR(port_classname_resolver ? port_classname_resolver(id)
                                                : "?");
        }
    } else {
        PORT_RD_STR("\r\nwalker    (idle)");
    }
    /* registers + 32 stack return words (module-relative), like crash.txt but
       wider. */
    if (ep && ep->ContextRecord) {
        CONTEXT *cx = ep->ContextRecord;
        unsigned *sp;
        int i, shown;
        PORT_RD_STR("\r\neip ");  PORT_RD_HEX(cx->Eip);
        PORT_RD_STR(" esp ");     PORT_RD_HEX(cx->Esp);
        PORT_RD_STR(" ebp ");     PORT_RD_HEX(cx->Ebp);
        PORT_RD_STR("\r\neax ");  PORT_RD_HEX(cx->Eax);
        PORT_RD_STR(" ebx ");     PORT_RD_HEX(cx->Ebx);
        PORT_RD_STR(" ecx ");     PORT_RD_HEX(cx->Ecx);
        PORT_RD_STR(" edx ");     PORT_RD_HEX(cx->Edx);
        PORT_RD_STR(" esi ");     PORT_RD_HEX(cx->Esi);
        PORT_RD_STR(" edi ");     PORT_RD_HEX(cx->Edi);
        PORT_RD_STR("\r\nstack (32 module return words)");
        sp = (unsigned *)cx->Esp;
        shown = 0;
        for (i = 0; i < 512 && shown < 32; ++i) {
            unsigned v;
            if (IsBadReadPtr(sp + i, 4)) break;
            v = sp[i];
            if (v >= (unsigned)(uintptr_t)base &&
                v < (unsigned)(uintptr_t)base + 0x200000 &&
                port_addr_is_exec((uintptr_t)v)) {
                PORT_RD_STR("\r\n  +");
                PORT_RD_HEX(v - (unsigned)(uintptr_t)base);
                ++shown;
            }
        }
    } else {
        /* exit path: no context, sample the caller chain instead */
        void *frames[32];
        unsigned nn = CaptureStackBackTrace(0, 32, frames, 0), i;
        PORT_RD_STR("\r\ncallers (module return words)");
        for (i = 0; i < nn; ++i) {
            unsigned v = (unsigned)(uintptr_t)frames[i];
            if (v >= (unsigned)(uintptr_t)base &&
                v < (unsigned)(uintptr_t)base + 0x200000 &&
                port_addr_is_exec((uintptr_t)v)) {
                PORT_RD_STR("\r\n  +");
                PORT_RD_HEX(v - (unsigned)(uintptr_t)base);
            }
        }
    }
    PORT_RD_STR("\r\nresolve: tools/resolve_crash.py <thisfile>"
                "  (offsets -> build/port/walk_window.map)\r\n");
    /* the SM64DS_* environment: scan the process block, emit every SM64DS_ var */
    PORT_RD_STR("---- SM64DS_* env ----\r\n");
    {
        LPCH env = GetEnvironmentStringsA();
        if (env) {
            LPCH e = env;
            while (*e) {
                if (e[0] == 'S' && e[1] == 'M' && e[2] == '6' && e[3] == '4' &&
                    e[4] == 'D' && e[5] == 'S' && e[6] == '_') {
                    const char *q = e;
                    while (*q && p < sizeof buf - 16) buf[p++] = *q++;
                    PORT_RD_STR("\r\n");
                }
                while (*e) ++e;
                ++e;
            }
            FreeEnvironmentStringsA(env);
        }
    }
#undef PORT_RD_STR
#undef PORT_RD_HEX
#undef PORT_RD_DEC
    f = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, 0,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (f != INVALID_HANDLE_VALUE) {
        DWORD wr;
        WriteFile(f, buf, p, &wr, 0);
        /* LAST step: the playlog tail, so a read failure here loses nothing
           already committed above. */
        port_crash_append_playlog(f);
        FlushFileBuffers(f);
        CloseHandle(f);
    }
}

static void port_crash_write_file(EXCEPTION_POINTERS *ep)
{
    static char path[MAX_PATH + 16];
    static char buf[2048];
    static volatile LONG once;
    /* A SYNTHETIC FAULT IS NOT A CRASH REPORT. port_fault_synthetic is set only
       while the SM64DS_TEST_QUARANTINE hook (port/unmatched/func_02043fdc.cpp,
       and only in a PORT_TEST_HOOKS build) is deliberately raising an access
       violation it intends to catch. Writing for it would cost real evidence
       twice over: it would spend the one-shot `once` latch below, so the crash
       the operator was actually hunting later in the same run would write
       nothing, and it would push a fabricated dump into the rolling five-deep
       player crash directory that the report intake reads. Check it BEFORE the
       latch so a suppressed fault leaves the latch unclaimed. */
    if (port_fault_synthetic)
        return;
    if (InterlockedExchange((volatile LONG *)&once, 1))
        return;                          /* first crash wins; no re-entry */
    {
        DWORD n = GetModuleFileNameA(0, path, MAX_PATH);
        while (n && path[n - 1] != 92 /* '\\' */)
            --n;
        lstrcpyA(path + n, "crash.txt");
    }
    {
    char *base = (char *)GetModuleHandleA(0);
    unsigned p = 0;
#define PORT_CRASH_STR(s) do { const char *q = (s); \
        while (*q && p < sizeof buf - 12) buf[p++] = *q++; } while (0)
#define PORT_CRASH_HEX(v) do { if (p < sizeof buf - 12) { \
        port_crash_hex(buf + p, (unsigned)(v)); p += 8; } } while (0)
    PORT_CRASH_STR("walk_window crash\r\ncode      ");
    PORT_CRASH_HEX(ep->ExceptionRecord->ExceptionCode);
    PORT_CRASH_STR("\r\naddress   ");
    PORT_CRASH_HEX((uintptr_t)ep->ExceptionRecord->ExceptionAddress);
    PORT_CRASH_STR("\r\nmodule    ");
    PORT_CRASH_HEX((uintptr_t)base);
    PORT_CRASH_STR("\r\noffset    +");
    PORT_CRASH_HEX((uintptr_t)ep->ExceptionRecord->ExceptionAddress
                   - (uintptr_t)base);
    if (ep->ExceptionRecord->NumberParameters > 1) {
        PORT_CRASH_STR("\r\naccess    ");
        PORT_CRASH_HEX(ep->ExceptionRecord->ExceptionInformation[0]);
        PORT_CRASH_STR(" at ");
        PORT_CRASH_HEX(ep->ExceptionRecord->ExceptionInformation[1]);
    }
    if (ep->ContextRecord) {
        CONTEXT *cx = ep->ContextRecord;
        unsigned *sp;
        int i, printed;
        PORT_CRASH_STR("\r\neip ");  PORT_CRASH_HEX(cx->Eip);
        PORT_CRASH_STR(" esp ");     PORT_CRASH_HEX(cx->Esp);
        PORT_CRASH_STR(" ebp ");     PORT_CRASH_HEX(cx->Ebp);
        PORT_CRASH_STR("\r\neax ");  PORT_CRASH_HEX(cx->Eax);
        PORT_CRASH_STR(" ebx ");     PORT_CRASH_HEX(cx->Ebx);
        PORT_CRASH_STR(" ecx ");     PORT_CRASH_HEX(cx->Ecx);
        PORT_CRASH_STR(" edx ");     PORT_CRASH_HEX(cx->Edx);
        PORT_CRASH_STR(" esi ");     PORT_CRASH_HEX(cx->Esi);
        PORT_CRASH_STR(" edi ");     PORT_CRASH_HEX(cx->Edi);
        /* module-relative return-address candidates off the faulting stack;
           IsBadReadPtr guards the walk so a torn ESP cannot re-fault */
        PORT_CRASH_STR("\r\nstack (+module words)");
        sp = (unsigned *)cx->Esp;
        printed = 0;
        for (i = 0; i < 256 && printed < 16; ++i) {
            unsigned v;
            if (IsBadReadPtr(sp + i, 4)) break;
            v = sp[i];
            if (v >= (unsigned)(uintptr_t)base &&
                v < (unsigned)(uintptr_t)base + 0x200000 &&
                port_addr_is_exec((uintptr_t)v)) {
                PORT_CRASH_STR("\r\n  +");
                PORT_CRASH_HEX(v - (unsigned)(uintptr_t)base);
                ++printed;
            }
        }
    }
    PORT_CRASH_STR("\r\nresolve: offset -> build/port/walk_window.map\r\n");
#undef PORT_CRASH_STR
#undef PORT_CRASH_HEX
    {
        HANDLE f = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, 0,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (f != INVALID_HANDLE_VALUE) {
            DWORD wr;
            WriteFile(f, buf, p, &wr, 0);
            FlushFileBuffers(f);
            CloseHandle(f);
        }
    }
    }
    /* the rolling rich dump under %TEMP%\sm64ds-crashes\, in addition to the
       crash.txt next to the exe. The `once` guard above means the first crash
       wins here too. */
    port_rich_dump(ep, ep->ExceptionRecord->ExceptionCode, "exception");
}

static LONG WINAPI port_crash_veh(EXCEPTION_POINTERS *ep)
{
    switch (ep->ExceptionRecord->ExceptionCode) {
    case 0xC0000005u:   /* access violation */
    case 0xC0000006u:   /* in-page error */
    case 0xC000001Du:   /* illegal instruction */
    case 0xC0000094u:   /* integer divide by zero */
    case 0xC0000096u:   /* privileged instruction */
    case 0xC00000FDu:   /* stack overflow: the UEF may never get a stack */
    case 0xC0000409u:   /* stack-buffer-overrun, when raised as an exception */
        port_crash_write_file(ep);
        break;
    }
    return EXCEPTION_CONTINUE_SEARCH;    /* normal handling still runs */
}

static LONG WINAPI port_fault_probe_with_file(EXCEPTION_POINTERS *ep)
{
    port_crash_write_file(ep);
    return port_fault_probe(ep);
}

/* ---- exit.txt: the silent-death catcher -----------------------------------
   crash.txt covers deaths that raise an exception. A death that goes through
   the ORDERLY door -- exit(), _exit(), quick_exit(), abort()'s tail,
   ExitProcess() from any library -- raises nothing, so it leaves no crash.txt,
   no WER record, and (buffered or not) a stdout that just stops. The Whomp's
   Fortress wall area produced exactly that shape twice in real play and once
   in a headless soak: process gone, exit code -1, nothing anywhere.

   Every one of those doors funnels through ntdll's RtlExitUserProcess (the
   CRT's exit and ExitProcess both end there), so one 5-byte jmp detour at its
   entry catches them all regardless of which module called. The hook never
   returns to the original: it logs and then terminates through
   NtTerminateProcess, which is the same place RtlExitUserProcess was going.
   TerminateProcess(self) and __fastfail still bypass this -- they bypass
   everything -- and are called out in exit.txt's absence the way crash.txt's
   header calls out fastfail.

   INERT IN PRACTICE: exit.txt is written only for a NONZERO exit code, so
   the selftest's clean exit(0) leaves nothing. Same raw-Win32 discipline as
   crash.txt: static buffers, hand-rolled hex, CreateFileA/WriteFile. */
/* The harness's frame counter, weak like the walker pointer so TUs without a
   frame loop still link (the fallback reads -1). walk_window defines and
   feeds the real one. */
#ifdef __cplusplus
extern "C" int port_last_frame;
extern "C" __declspec(selectany) int port_fault_no_frame = -1;
#else
extern int port_last_frame;
__declspec(selectany) int port_fault_no_frame = -1;
#endif
#pragma comment(linker, "/alternatename:_port_last_frame=_port_fault_no_frame")

typedef LONG(__stdcall *port_NtTerminateProcess_t)(HANDLE, LONG);

static void port_exit_write_file(unsigned code)
{
    static char path[MAX_PATH + 16];
    static char buf[1024];
    static volatile LONG once;
    if (InterlockedExchange((volatile LONG *)&once, 1))
        return;
    {
        DWORD n = GetModuleFileNameA(0, path, MAX_PATH);
        while (n && path[n - 1] != 92 /* '\\' */)
            --n;
        lstrcpyA(path + n, "exit.txt");
    }
    {
    char *base = (char *)GetModuleHandleA(0);
    unsigned p = 0;
#define PORT_EXIT_STR(s) do { const char *q = (s); \
        while (*q && p < sizeof buf - 12) buf[p++] = *q++; } while (0)
#define PORT_EXIT_HEX(v) do { if (p < sizeof buf - 12) { \
        port_crash_hex(buf + p, (unsigned)(v)); p += 8; } } while (0)
    PORT_EXIT_STR("walk_window silent exit\r\ncode      ");
    PORT_EXIT_HEX(code);
    PORT_EXIT_STR("\r\nmodule    ");
    PORT_EXIT_HEX((uintptr_t)base);
    PORT_EXIT_STR("\r\nframe     ");
    PORT_EXIT_HEX((unsigned)port_last_frame);
    if (data_020a4b68 && !IsBadReadPtr(data_020a4b68, 12)) {
        char *a = (char *)(uintptr_t)data_020a4b68[2];
        PORT_EXIT_STR("\r\nwalker    node ");
        PORT_EXIT_HEX((uintptr_t)data_020a4b68);
        PORT_EXIT_STR(" actor ");
        PORT_EXIT_HEX((uintptr_t)a);
        PORT_EXIT_STR(" id ");
        PORT_EXIT_HEX((a && !IsBadReadPtr(a, 0x10))
                      ? *(unsigned short *)(a + 0xc) : 0xffffu);
    }
    PORT_EXIT_STR("\r\ncallers (+module)");
    {
        void *frames[24];
        unsigned n = CaptureStackBackTrace(0, 24, frames, 0);
        unsigned i;
        for (i = 0; i < n; ++i) {
            unsigned v = (unsigned)(uintptr_t)frames[i];
            PORT_EXIT_STR("\r\n  ");
            if (v >= (unsigned)(uintptr_t)base &&
                v < (unsigned)(uintptr_t)base + 0x200000) {
                PORT_EXIT_STR("+");
                PORT_EXIT_HEX(v - (unsigned)(uintptr_t)base);
            } else {
                PORT_EXIT_HEX(v);
                PORT_EXIT_STR(" (outside module)");
            }
        }
    }
    PORT_EXIT_STR("\r\nresolve: offset -> build/port/walk_window.map\r\n");
#undef PORT_EXIT_STR
#undef PORT_EXIT_HEX
    {
        HANDLE f = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, 0,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (f != INVALID_HANDLE_VALUE) {
            DWORD wr;
            WriteFile(f, buf, p, &wr, 0);
            FlushFileBuffers(f);
            CloseHandle(f);
        }
    }
    }
    /* the rolling rich dump too: an orderly nonzero exit is a death worth a
       full record. No EXCEPTION_POINTERS on this path -- pass the code. */
    port_rich_dump(0, code, "exit");
}

static void __stdcall port_exit_hook(LONG code)
{
    if (code != 0)
        port_exit_write_file((unsigned)code);
    {
        HMODULE nt = GetModuleHandleA("ntdll.dll");
        port_NtTerminateProcess_t term = nt
            ? (port_NtTerminateProcess_t)GetProcAddress(nt,
                                                        "NtTerminateProcess")
            : 0;
        if (term)
            term(GetCurrentProcess(), code);
    }
    TerminateProcess(GetCurrentProcess(), (UINT)code);   /* belt and braces */
}

static void port_install_exit_probe(void)
{
    HMODULE nt = GetModuleHandleA("ntdll.dll");
    unsigned char *fn = nt
        ? (unsigned char *)GetProcAddress(nt, "RtlExitUserProcess") : 0;
    DWORD old;
    if (!fn)
        return;
    if (!VirtualProtect(fn, 5, PAGE_EXECUTE_READWRITE, &old))
        return;
    fn[0] = 0xE9;   /* jmp rel32 to the hook; the original is never re-entered */
    *(unsigned *)(fn + 1) =
        (unsigned)((uintptr_t)port_exit_hook - ((uintptr_t)fn + 5));
    VirtualProtect(fn, 5, old, &old);
    FlushInstructionCache(GetCurrentProcess(), fn, 5);
}
#endif /* _WIN32 (port_crash_dir .. port_install_exit_probe) */

/* Redirected stdio is fully buffered under the MSVC CRT, so a hard death
   loses every line since the last flush; the two setvbuf calls make captures
   loss-free. Harmless on a console. The setvbuf pair is portable, so it runs on
   both platforms; the VEH/detour/crash-dir install is Win32-only and is a later
   lane on Linux, so it is guarded out (the macro becomes a no-op past setvbuf). */
#ifdef _WIN32
#define PORT_INSTALL_FAULT_PROBE() do { \
        setvbuf(stderr, NULL, _IONBF, 0); \
        setvbuf(stdout, NULL, _IONBF, 0); \
        port_crash_dir_boot(); /* make %TEMP%\sm64ds-crashes, prune to newest 4 */ \
        AddVectoredExceptionHandler(1, port_crash_veh); \
        SetUnhandledExceptionFilter(port_fault_probe_with_file); \
        port_install_exit_probe(); \
    } while (0)
#else /* Linux: portable setvbuf only; crash forensics is a later lane. */
#define PORT_INSTALL_FAULT_PROBE() do { \
        setvbuf(stderr, NULL, _IONBF, 0); \
        setvbuf(stdout, NULL, _IONBF, 0); \
    } while (0)
#endif

#ifdef _WIN32
/* Hang watchdog (PORT_WATCHDOG=<seconds>): a helper thread suspends the
   main thread after the deadline and prints its EIP plus a raw stack
   sample, module-relative, resolvable against the /MAP file. For loops
   the fault probe never sees. */
struct port_watchdog_args { HANDLE main; unsigned secs; };

static DWORD WINAPI port_watchdog_thread(LPVOID p)
{
    struct port_watchdog_args *a = (struct port_watchdog_args *)p;
    Sleep(a->secs * 1000);
    char *base = (char *)GetModuleHandleA(0);
    SuspendThread(a->main);
    CONTEXT ctx;
    memset(&ctx, 0, sizeof ctx);
    ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
    if (GetThreadContext(a->main, &ctx)) {
        fprintf(stderr, "WATCHDOG eip=+0x%08x esp=%08x\n",
                (unsigned)((char *)ctx.Eip - base), (unsigned)ctx.Esp);
        /* raw return-address candidates off the live stack */
        unsigned *sp = (unsigned *)ctx.Esp;
        for (int i = 0; i < 96; ++i) {
            unsigned v;
            if (IsBadReadPtr(sp + i, 4)) break;
            v = sp[i];
            if (v >= (unsigned)(uintptr_t)base &&
                v < (unsigned)(uintptr_t)base + 0x200000 &&
                port_addr_is_exec((uintptr_t)v))
                fprintf(stderr, "  stack[%02d] +0x%08x\n", i,
                        (unsigned)(v - (unsigned)(uintptr_t)base));
        }
    }
    fflush(stderr);
    TerminateProcess(GetCurrentProcess(), 3);
    return 0;
}

/* Hardware write-watch (x86 debug registers): up to 4 dword slots.
   port_watch_words(addr, n) arms DR0..DR3 on the calling thread; the
   vectored handler prints the writer's module-relative EIP and keeps
   going. For finding who stomps a host global. */
/* The watched base, so each event prints the current values: a refcount that
   drains reads as a story, a pointer that flips names its moment. The 300 cap
   replaces the old 8, which went quiet before the interesting event every
   time it mattered (the sign-SFP drain sat behind twelve legitimate loads). */
static unsigned *port_watch_base;
static LONG WINAPI port_watch_handler(EXCEPTION_POINTERS *ep)
{
    if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP)
        return EXCEPTION_CONTINUE_SEARCH;
    char *base = (char *)GetModuleHandleA(0);
    static int shown;
    if (shown < 300) {
        ++shown;
        fprintf(stderr, "[watch] write near watched words, eip=+0x%08x",
                (unsigned)((char *)ep->ExceptionRecord->ExceptionAddress -
                           base));
        if (port_watch_base)
            fprintf(stderr, "  now={%08x %08x}", port_watch_base[0],
                    port_watch_base[1]);
        fprintf(stderr, "\n");
        unsigned *sp = (unsigned *)ep->ContextRecord->Esp;
        int printed = 0;
        for (int i = 0; i < 64 && printed < 4; ++i) {
            unsigned v;
            if (IsBadReadPtr(sp + i, 4)) break;
            v = sp[i];
            if (v >= (unsigned)(uintptr_t)base &&
                v < (unsigned)(uintptr_t)base + 0x200000 &&
                port_addr_is_exec((uintptr_t)v)) {
                fprintf(stderr, "    caller? +0x%08x\n",
                        (unsigned)(v - (unsigned)(uintptr_t)base));
                ++printed;
            }
        }
        fflush(stderr);
    }
    ep->ContextRecord->EFlags |= 0x10000;   /* RF: resume past the hit */
    return EXCEPTION_CONTINUE_EXECUTION;
}

static void port_watch_words(void *addr, int nwords)
{
    port_watch_base = (unsigned *)addr;
    static int handler_in;
    if (!handler_in) {
        handler_in = 1;
        AddVectoredExceptionHandler(1, port_watch_handler);
    }
    CONTEXT ctx;
    memset(&ctx, 0, sizeof ctx);
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    HANDLE th = GetCurrentThread();
    GetThreadContext(th, &ctx);
    DWORD *dr[4] = {&ctx.Dr0, &ctx.Dr1, &ctx.Dr2, &ctx.Dr3};
    if (nwords > 4) nwords = 4;
    for (int i = 0; i < nwords; ++i) {
        *dr[i] = (DWORD)(uintptr_t)((char *)addr + 4 * i);
        ctx.Dr7 |= (1u << (2 * i));                 /* local enable */
        ctx.Dr7 |= (0x1u << (16 + 4 * i));          /* break on WRITE */
        ctx.Dr7 |= (0x3u << (18 + 4 * i));          /* len = 4 bytes */
    }
    SetThreadContext(th, &ctx);
}

static void port_install_watchdog(void)
{
    const char *e = getenv("PORT_WATCHDOG");
    if (!e) return;
    static struct port_watchdog_args a;
    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                    GetCurrentProcess(), &a.main, 0, FALSE,
                    DUPLICATE_SAME_ACCESS);
    a.secs = (unsigned)atoi(e);
    if (!a.secs) a.secs = 10;
    CreateThread(0, 0, port_watchdog_thread, &a, 0, 0);
}
#else /* Linux: the debug-register watch and the suspend-thread watchdog are
         both later lanes; walk_window.cpp still references these names, so
         provide inert no-op stubs so it compiles and links. */
static void port_watch_words(void * /*addr*/, int /*nwords*/) {}
static void port_install_watchdog(void) {}
#endif

/* ---- external seams for the quarantine walker -----------------------------
   port/unmatched/func_02043fdc.cpp catches per-actor faults but must not
   include this header (it would pull the VEH/detour installers into a plain-C
   actor TU). The one TU that installs the probe (walk_window) defines
   PORT_FAULT_PROBE_DEFINE_EXPORTS before including, which emits these two
   non-static entry points wrapping the static internals above. Every other TU
   that includes this header leaves them out, so there is exactly one
   definition. The walker weak-links against them. */
#ifdef PORT_FAULT_PROBE_DEFINE_EXPORTS
#ifdef _WIN32
extern "C" void port_rich_dump_ex(EXCEPTION_POINTERS *ep, unsigned code,
                                  const char *reason)
{
    port_rich_dump(ep, code, reason);
}
extern "C" const char *port_crash_dir_get(void)
{
    if (!port_crash_dir[0])
        port_crash_dir_boot();
    return port_crash_dir;
}
#else /* Linux: crash dumps are a later lane. The rich dump is a no-op, and the
         crash dir is %TEMP% -> /tmp. Signatures kept so the walker's weak-link
         seams resolve. */
extern "C" void port_rich_dump_ex(EXCEPTION_POINTERS * /*ep*/, unsigned /*code*/,
                                  const char * /*reason*/)
{
}
extern "C" const char *port_crash_dir_get(void)
{
    return "/tmp";
}
#endif
#endif

#endif
