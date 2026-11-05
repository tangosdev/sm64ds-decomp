// Shared crash probe for the gate smokes: prints the module-relative fault
// address and a frame-pointer backtrace, resolvable against the /MAP file.
#ifndef PORT_FAULT_PROBE_H
#define PORT_FAULT_PROBE_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery((LPCVOID)addr, &mbi, sizeof mbi) != sizeof mbi)
        return 0;
    if (mbi.State != MEM_COMMIT)
        return 0;
    return (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
                           PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))
           != 0;
}

/* The module window the stack scans use. It WAS a fixed 0x200000, which is
   smaller than walk_window.exe: the image is about 6 MB and its .text runs
   well past base+0x200000, so every return address above that bound was
   dropped and a "jumped to 0x0" crash recorded no caller at all (levels
   2/4/5/50 on 15fe8e32e printed three low data-looking words and nothing
   else). SizeOfImage off the module own PE header is the real bound;
   port_addr_is_exec still does the labelling, so widening the window cannot
   re-admit the .data addresses the predicate above was written to reject.
   Falls back to the old constant if the header cannot be read, which keeps
   the probe safe on a torn module. */
static unsigned port_module_span(const void *modbase)
{
    const IMAGE_DOS_HEADER *dos = (const IMAGE_DOS_HEADER *)modbase;
    if (modbase && !IsBadReadPtr((LPVOID)modbase, sizeof *dos) &&
        dos->e_magic == IMAGE_DOS_SIGNATURE) {
        const IMAGE_NT_HEADERS *nt =
            (const IMAGE_NT_HEADERS *)((const char *)modbase + dos->e_lfanew);
        if (!IsBadReadPtr((LPVOID)nt, sizeof *nt) &&
            nt->Signature == IMAGE_NT_SIGNATURE)
            return nt->OptionalHeader.SizeOfImage;
    }
    return 0x200000u;
}


/* ---- WHICH MODULE OWNS EVERY ADDRESS, AND THE FAULTING THREAD'S OWN WALK ----
   (run after093, lane CRASHWALK1.) Five dumps from one tester on 2026-10-26
   named nothing: eip sat in a DLL the dump could not name, the 32 module
   return words were empty because the faulting worker had no exe code on its
   stack, and the "FAULT ... frame N:" lines in the playlog tail were
   CaptureStackBackTrace taken INSIDE this handler, so they began at
   port_fault_probe itself and walked on into ntdll's dispatcher.

   Two pieces, both safe on a broken heap and under a held loader lock:
     - port_crash_mods_scan() reads the PEB's InMemoryOrderModuleList (the
       loader's own table: DllBase, SizeOfImage, BaseDllName) into a static
       table of at most 64 rows, the port's own image first. No API that
       allocates or locks; every node read is checked by VirtualQuery first,
       and the walk is bounded, so a torn list stops the table rather than
       the dump.
     - port_crash_walk() starts from the EXCEPTION_POINTERS context (Eip, Ebp)
       and follows the ebp chain ([ebp+4] the return, [ebp] the next frame),
       every read range-checked against the thread's StackLimit / StackBase
       from the TEB, so it needs no SEH and cannot fault. The port builds with
       /Oy- so its own frames chain; a frameless leaf in a system DLL drops its
       immediate caller from the chain, which the raw scan from Esp covers.

   Naming, one rule everywhere: an address in the port's own image prints in
   the old bare "+0x<rva>" form (tools/resolve_crash.py and
   port/tools/faultmap.py resolve it), an address in another image prints as
   "<name>+0x<rva>" (resolve_crash.py leaves those alone), and anything else
   prints raw with " (no module)". */
#define PORT_MODS_MAX 64
struct port_crash_mod { uintptr_t base, end; char name[40]; };
static struct port_crash_mod port_crash_mods[PORT_MODS_MAX];
static int port_crash_nmods;
static int port_crash_mods_more;   /* loaded modules past the table cap */

/* committed, readable, and [addr, addr+len) inside one region */
static int port_crash_readable(uintptr_t addr, unsigned len)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!addr || VirtualQuery((LPCVOID)addr, &mbi, sizeof mbi) != sizeof mbi)
        return 0;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)))
        return 0;
    return addr + len <= (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
}

static void port_crash_mods_scan(void)
{
    /* x86 layouts: TEB+0x30 PEB, PEB+0x0c Ldr, Ldr+0x14 the in-memory-order
       list head. A link points at an entry's InMemoryOrderLinks (entry+0x08):
       link+0x10 DllBase, +0x18 SizeOfImage, +0x24 BaseDllName (Length,
       MaximumLength, Buffer). */
    uintptr_t peb = *(uintptr_t *)((char *)NtCurrentTeb() + 0x30);
    uintptr_t ldr, head, link;
    int steps = 0;
    port_crash_nmods = 0;
    port_crash_mods_more = 0;
    if (!port_crash_readable(peb + 0x0c, 4))
        return;
    ldr = *(uintptr_t *)(peb + 0x0c);
    if (!port_crash_readable(ldr + 0x14, 8))
        return;
    head = ldr + 0x14;
    link = *(uintptr_t *)head;
    while (link != head && steps++ < 1024) {
        if (!port_crash_readable(link, 0x2c))
            break;
        if (port_crash_nmods < PORT_MODS_MAX) {
            struct port_crash_mod *m = &port_crash_mods[port_crash_nmods];
            unsigned short blen = *(unsigned short *)(link + 0x24);
            const unsigned short *w = *(const unsigned short **)(link + 0x28);
            unsigned i, n = 0;
            m->base = *(uintptr_t *)(link + 0x10);
            m->end = m->base + *(unsigned *)(link + 0x18);
            if (w && port_crash_readable((uintptr_t)w, blen))
                for (i = 0; i < blen / 2u && n < sizeof m->name - 1; ++i)
                    m->name[n++] = (w[i] > 0x20 && w[i] < 0x7f) ? (char)w[i] : '?';
            if (!n)
                m->name[n++] = '?';
            m->name[n] = 0;
            if (m->base)
                ++port_crash_nmods;
        } else {
            ++port_crash_mods_more;
        }
        link = *(uintptr_t *)link;
    }
}

/* table row owning addr, -1 when none (row 0 is the port's own image) */
static int port_crash_mod_of(uintptr_t a)
{
    int i;
    for (i = 0; i < port_crash_nmods; ++i)
        if (a >= port_crash_mods[i].base && a < port_crash_mods[i].end)
            return i;
    return -1;
}

static unsigned port_crash_hex8(char *dst, unsigned v)
{
    static const char h[] = "0123456789abcdef";
    int i;
    for (i = 0; i < 8; ++i)
        dst[i] = h[(v >> (28 - 4 * i)) & 0xf];
    return 8;
}

/* "+0x<rva>" (port), "<name>+0x<rva>" (other image), "<raw> (no module)";
   writes at most 64 bytes, NUL-terminated, returns the length. */
static unsigned port_crash_name_addr(char *dst, uintptr_t a)
{
    int m = port_crash_mod_of(a);
    unsigned n = 0;
    if (m < 0) {
        n += port_crash_hex8(dst, (unsigned)a);
        lstrcpyA(dst + n, " (no module)");
        return n + 12;
    }
    if (m > 0) {
        const char *q = port_crash_mods[m].name;
        while (*q) dst[n++] = *q++;
    }
    dst[n++] = '+'; dst[n++] = '0'; dst[n++] = 'x';
    n += port_crash_hex8(dst + n, (unsigned)(a - port_crash_mods[m].base));
    dst[n] = 0;
    return n;
}

/* The faulting thread's stack, from the TEB (the handler runs on it). */
static void port_crash_stack_range(uintptr_t *lo, uintptr_t *hi)
{
    NT_TIB *tib = (NT_TIB *)NtCurrentTeb();
    *lo = (uintptr_t)tib->StackLimit;
    *hi = (uintptr_t)tib->StackBase;
}

/* frames[0] = Eip, then the ebp chain's return addresses; returns the count
   and each frame's ebp in ebps[] (0 for frame 0). */
static unsigned port_crash_walk(const CONTEXT *cx, uintptr_t *frames,
                                uintptr_t *ebps, unsigned max)
{
    uintptr_t lo, hi, bp;
    unsigned n = 0;
    if (!max)
        return 0;
    port_crash_stack_range(&lo, &hi);
    frames[n] = cx->Eip; ebps[n] = 0; ++n;
    bp = cx->Ebp;
    while (n < max && bp >= lo && bp + 8 <= hi && !(bp & 3)) {
        uintptr_t ret = *(uintptr_t *)(bp + 4);
        uintptr_t next = *(uintptr_t *)bp;
        if (!ret)
            break;
        frames[n] = ret; ebps[n] = bp; ++n;
        if (next <= bp)
            break;
        bp = next;
    }
    return n;
}


/* The ROM's list walker (func_02043fdc) parks the node it is processing in
   data_020a4b68 before every callback, so at fault time node[2] names the
   actor whose phase code was running -- the question every actor-phase crash
   asks first. Weak so the probe still links in TUs without the engine. */
#ifdef __cplusplus
extern "C" int *data_020a4b68;
extern "C" __declspec(selectany) int *port_fault_no_walker = 0;
#else
extern int *data_020a4b68;
__declspec(selectany) int *port_fault_no_walker = 0;
#endif
#pragma comment(linker, "/alternatename:_data_020a4b68=_port_fault_no_walker")

/* Particle::SysTracker lives at data_0209ee74 and its callback bank starts
   0x750 into it. A "jumped to 0/1/2" through a particle callback cannot be
   read without knowing whether the pointer the caller passed is inside that
   object at all, so the probe prints the owner and the bank. Weak, like the
   walker global above, so TUs without the engine still link. */
#ifdef __cplusplus
extern "C" void *data_0209ee74;
extern "C" __declspec(selectany) void *port_fault_no_particles = 0;
#else
extern void *data_0209ee74;
__declspec(selectany) void *port_fault_no_particles = 0;
#endif
#pragma comment(linker, "/alternatename:_data_0209ee74=_port_fault_no_particles")

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
    /* the frames come from the FAULTING context (the old CaptureStackBackTrace
       here walked this handler's own frames into ntdll's dispatcher), named by
       module the same way the rich dump names them */
    port_crash_mods_scan();
    if (ep->ContextRecord) {
        uintptr_t frames[33], ebps[33], lo, hi;
        char nm[72];
        unsigned n = port_crash_walk(ep->ContextRecord, frames, ebps, 33);
        port_crash_stack_range(&lo, &hi);
        fprintf(stderr, "  thread %08lx stack %08x-%08x\n",
                GetCurrentThreadId(), (unsigned)lo, (unsigned)hi);
        for (unsigned i = 0; i < n; ++i) {
            port_crash_name_addr(nm, frames[i]);
            fprintf(stderr, "  frame %u: %s\n", i, nm);
        }
    } else {
        void *frames[12];
        unsigned n = CaptureStackBackTrace(0, 12, frames, 0);
        for (unsigned i = 0; i < n; ++i)
            fprintf(stderr, "  frame %u: +0x%08x\n", i,
                    (unsigned)((char *)frames[i] - base));
    }
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
                v < (unsigned)(uintptr_t)base + port_module_span(base) &&
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

   Installed twice: a FIRST-chance vectored handler, and the unhandled filter.
   CREATE_ALWAYS: the file describes the LAST crash. Offsets resolve against the
   /MAP file the build already writes (build/port/walk_window.map): section
   offset = printed offset - 0x1000 for .text symbols.

   THE VECTORED HANDLER IS DELIBERATELY NARROW, and it was not always. It was
   written on the assumption that "nothing here handles an AV", which was true
   the day it landed (714f22216) and stopped being true in 825e06083, the commit
   that added the per-actor quarantine: port_dispatch_guarded wraps every actor
   tick in a __try/__except that CATCHES an access violation, freezes the actor
   and lets the frame continue. A vectored handler runs FIRST-chance, before any
   __except filter, so every quarantined actor fault -- a fault the player
   survives and never sees -- reached port_crash_write_file and spent its
   one-shot `once` latch. From that moment the process could no longer report
   anything: the crash that actually killed the run later in the session wrote
   no crash.txt and no rolling dump, because the latch was already claimed by a
   fault that had been handled. The player's evidence was a dump of the wrong
   fault, or nothing at all, and it failed SILENTLY.

   So the vectored handler now claims the latch only for the two codes where the
   unhandled filter genuinely cannot be relied on to run:
     - 0xC00000FD stack overflow, where the filter may never get a stack
     - 0xC0000409 stack-buffer-overrun, raised via __fastfail/int 29h, which
       bypasses the unhandled filter entirely
   Neither is something the quarantine's __except is in a position to swallow
   and keep playing through. Every other fatal code is left to the unhandled
   filter, which by construction runs only for an exception that NOTHING
   handled -- which is the precise definition of the crash worth reporting.
   A quarantined fault is not a crash and must not be filed as one; it already
   files its own rolling dump, tagged "quarantine", from the quarantine filter
   itself (port/unmatched/func_02043fdc_hostcopy.cpp).

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
/* THE INSTALL VERDICT (hal/os_arena.cpp): what the asset folder turned out to
   be and what happened to any savestate.bin in it, decided at boot and held so
   a dump can carry it. Weak default is the empty string, which prints nothing
   -- a build with no romdata loader and no persist layer has no verdict to
   give, and an empty block is worse than no block. */
extern "C" char port_install_verdict[];
extern "C" __declspec(selectany) char port_fault_no_verdict[1] = { 0 };
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
extern char port_install_verdict[];
__declspec(selectany) char port_fault_no_verdict[1] = { 0 };
#endif
#pragma comment(linker, "/alternatename:_data_0209f2f8=_port_fault_no_level")
#pragma comment(linker, "/alternatename:_data_0209f394=_port_fault_no_players")
#pragma comment(linker, "/alternatename:_data_0209f250=_port_fault_no_pidx")
#pragma comment(linker, "/alternatename:_port_build_gittip=_port_fault_no_gittip")
#pragma comment(linker, "/alternatename:_port_playlog_path=_port_fault_no_playlog")
#pragma comment(linker, "/alternatename:_port_install_verdict=_port_fault_no_verdict")
/* Class-name resolution goes through a weak DATA function pointer, not a direct
   call. walk_window (which links hal/actor_registry.cpp) sets it to the real
   port_actor_class_name; the bare smokes -- which install the probe but do NOT
   link that HAL -- leave it null and the dump prints "?". A __declspec(selectany)
   pointer defaulting to 0 is legal (data, external linkage, one deduped body)
   and needs no alternatename gymnastics. hal/actor_registry.cpp's
   port_actor_registry_install wires it up. */
typedef const char *(*port_classname_fn)(unsigned id);
#ifdef __cplusplus
extern "C" __declspec(selectany) port_classname_fn port_classname_resolver = 0;
#else
__declspec(selectany) port_classname_fn port_classname_resolver = 0;
#endif

/* Extra crash text from hal/polelog_watch.cpp (the level 43 pole write-watch), the
   same weak data-pointer shape: null unless walk_window linked that file. */
typedef void (*port_crash_extra_fn)(char *buf, unsigned *p, unsigned cap);
#ifdef __cplusplus
extern "C" __declspec(selectany) port_crash_extra_fn port_crash_extra_text = 0;
#else
__declspec(selectany) port_crash_extra_fn port_crash_extra_text = 0;
#endif

/* port_last_frame is declared with its weak fallback later (the exit.txt
   block); the rich dump above uses it, so hoist just the DECLARATION here. The
   alternatename that gives it a weak body stays in the one place, below. */
#ifdef __cplusplus
extern "C" int port_last_frame;
#else
extern int port_last_frame;
#endif

/* Non-zero only while a test harness is deliberately raising a fault it means
   to catch: port/unmatched/func_02043fdc_hostcopy.cpp's SM64DS_TEST_QUARANTINE hook,
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

/* WHICH MODULE IS THIS ADDRESS IN (run hunt8, lane CRASHDRV093). The dump has
   always printed offsets against the game exe only, so a fault inside a Windows
   or driver DLL read as "eip-outside-module" and the stack scan, which keeps
   only exe words, came back empty: 36 reports of one crash named no DLL. This
   writes "name+0xRVA" for any address into dst (raw Win32, no stdio) and
   returns the number of bytes written. The name is the file name of the image
   that owns the page (VirtualQuery's AllocationBase), or "?" for anything that
   is not an image. */
static unsigned port_crash_modrva(char *dst, unsigned cap, uintptr_t addr)
{
    /* lane CRASHWALK1: the owner comes from the PEB table (port_crash_mods_scan)
       instead of VirtualQuery + GetModuleFileNameA, which takes the loader lock
       a crashing thread may already hold. Same output as before. */
    unsigned n = 0, i;
    int m;
    static const char hx[] = "0123456789abcdef";
    if (!port_crash_nmods)
        port_crash_mods_scan();
    m = port_crash_mod_of(addr);
    if (m < 0) {
        if (cap > 2) { dst[0] = '?'; return 1; }
        return 0;
    }
    for (i = 0; port_crash_mods[m].name[i] && n + 24 < cap; ++i)
        dst[n++] = port_crash_mods[m].name[i];
    dst[n++] = '+';
    dst[n++] = '0';
    dst[n++] = 'x';
    {
        unsigned v = (unsigned)(addr - port_crash_mods[m].base);
        int sh;
        for (sh = 28; sh >= 0; sh -= 4)
            dst[n++] = hx[(v >> sh) & 0xf];
    }
    return n;
}

/* The faulting thread's start address (NtQueryInformationThread class 9,
   resolved at call time: the probe links no ntdll import). 0 when unknown. */
static uintptr_t port_crash_thread_start(void)
{
    typedef LONG (WINAPI *qit_t)(HANDLE, int, void *, ULONG, ULONG *);
    HMODULE nt = GetModuleHandleA("ntdll.dll");
    qit_t q = nt ? (qit_t)GetProcAddress(nt, "NtQueryInformationThread") : 0;
    uintptr_t start = 0;
    if (q && q(GetCurrentThread(), 9, &start, sizeof start, 0) == 0)
        return start;
    return 0;
}

/* The rich dump itself. `ep` may be null (the orderly-exit path passes null and
   a code); `reason` is a short tag ("exception", "exit", "quarantine",
   "quarantine-fatal"). One static buffer, hand-rolled hex, one file. */
static void port_rich_dump(EXCEPTION_POINTERS *ep, unsigned code,
                           const char *reason)
{
    static char path[MAX_PATH + 64];
    static char buf[24576];   /* was 8192; the module table + walk need room */
    char *base = (char *)GetModuleHandleA(0);
    unsigned p = 0;
    SYSTEMTIME st;
    HANDLE f;
    if (!port_crash_dir[0])
        port_crash_dir_boot();
    port_crash_mods_scan();
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
    if (port_crash_extra_text)   /* level 43 pole bytes (hal/polelog_watch.cpp) */
        port_crash_extra_text(buf, &p, (unsigned)sizeof buf);
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
                v < (unsigned)(uintptr_t)base + port_module_span(base) &&
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
                v < (unsigned)(uintptr_t)base + port_module_span(base) &&
                port_addr_is_exec((uintptr_t)v)) {
                PORT_RD_STR("\r\n  +");
                PORT_RD_HEX(v - (unsigned)(uintptr_t)base);
            }
        }
    }
    PORT_RD_STR("\r\nresolve: tools/resolve_crash.py <thisfile>"
                "  (offsets -> build/port/walk_window.map)\r\n");
    /* WHICH THREAD, WHICH MODULES (lane CRASHDRV093). Every address the dump
       printed so far was relative to the exe; a fault in a DLL named nothing.
       Here: the thread id, the module that owns the faulting address, the
       thread's own start address, and every return-looking word on the stack
       that lands in ANY module (first 24 of 512 words). */
    if (ep && ep->ContextRecord) {
        unsigned *sp2 = (unsigned *)ep->ContextRecord->Esp;
        int k, got = 0;
        PORT_RD_STR("---- threads and modules ----\r\ntid ");
        PORT_RD_HEX(GetCurrentThreadId());
        PORT_RD_STR("\r\neip in ");
        if (p < sizeof buf - 80)
            p += port_crash_modrva(buf + p, (unsigned)(sizeof buf - 40 - p),
                                   (uintptr_t)ep->ExceptionRecord->ExceptionAddress);
        PORT_RD_STR("\r\nthread started at ");
        {
            uintptr_t ts = port_crash_thread_start();
            PORT_RD_HEX(ts);
            PORT_RD_STR(" ");
            if (ts && p < sizeof buf - 80)
                p += port_crash_modrva(buf + p, (unsigned)(sizeof buf - 40 - p), ts);
        }
        PORT_RD_STR("\r\nstack words in any module:");
        for (k = 0; k < 512 && got < 24 && p < sizeof buf - 120; ++k) {
            unsigned v;
            if (IsBadReadPtr(sp2 + k, 4)) break;
            v = sp2[k];
            if (v > 0x10000u && port_addr_is_exec((uintptr_t)v)) {
                PORT_RD_STR("\r\n  [");
                PORT_RD_DEC(k);
                PORT_RD_STR("] ");
                p += port_crash_modrva(buf + p, (unsigned)(sizeof buf - 40 - p), (uintptr_t)v);
                ++got;
            }
        }
        PORT_RD_STR("\r\nstack top ");
        {
            NT_TIB *tib = (NT_TIB *)NtCurrentTeb();
            PORT_RD_HEX((uintptr_t)tib->StackBase);
            PORT_RD_STR(" limit ");
            PORT_RD_HEX((uintptr_t)tib->StackLimit);
        }
        PORT_RD_STR("\r\n");
    }
    /* THE MODULE TABLE AND THE FAULTING THREAD'S WALK (lane CRASHWALK1; the
       banner above port_crash_mods_scan says why). */
    {
        static char nm[72];
        int k;
        PORT_RD_STR("---- modules ----\r\n"
                    "names: +0x<rva> = the port (first module), "
                    "<name>+0x<rva> = another module, <raw> (no module)");
        for (k = 0; k < port_crash_nmods; ++k) {
            PORT_RD_STR("\r\nmodule ");
            PORT_RD_HEX(port_crash_mods[k].base);
            PORT_RD_STR("-");
            PORT_RD_HEX(port_crash_mods[k].end);
            PORT_RD_STR(" ");
            PORT_RD_STR(port_crash_mods[k].name);
        }
        if (port_crash_mods_more) {
            PORT_RD_STR("\r\n(");
            PORT_RD_DEC(port_crash_mods_more);
            PORT_RD_STR(" more modules not listed)");
        }
        PORT_RD_STR("\r\n");
        if (ep && ep->ContextRecord) {
            CONTEXT *cx = ep->ContextRecord;
            uintptr_t frames[33], ebps[33], lo, hi;
            unsigned nf = port_crash_walk(cx, frames, ebps, 33), q;
            unsigned *sp3 = (unsigned *)cx->Esp;
            int got3 = 0;
            port_crash_stack_range(&lo, &hi);
            PORT_RD_STR("---- faulting thread ----\r\nthread ");
            PORT_RD_HEX(GetCurrentThreadId());
            PORT_RD_STR(" stack ");
            PORT_RD_HEX(lo);
            PORT_RD_STR("-");
            PORT_RD_HEX(hi);
            PORT_RD_STR("\r\nebp chain from the faulting context (eip, then returns)");
            for (q = 0; q < nf; ++q) {
                PORT_RD_STR("\r\n  frame ");
                PORT_RD_DEC(q);
                PORT_RD_STR(": ");
                port_crash_name_addr(nm, frames[q]);
                PORT_RD_STR(nm);
                if (q) {
                    PORT_RD_STR("  ebp ");
                    PORT_RD_HEX(ebps[q]);
                }
            }
            PORT_RD_STR("\r\nraw scan from esp ");
            PORT_RD_HEX(cx->Esp);
            PORT_RD_STR(" (256 words, module code only)");
            if ((uintptr_t)sp3 >= lo && (uintptr_t)sp3 < hi &&
                !((uintptr_t)sp3 & 3)) {
                for (k = 0; k < 256 && got3 < 64; ++k) {
                    uintptr_t at = (uintptr_t)(sp3 + k), v;
                    if (at + 4 > hi)
                        break;
                    v = sp3[k];
                    if (port_crash_mod_of(v) >= 0 && port_addr_is_exec(v)) {
                        PORT_RD_STR("\r\n  [");
                        PORT_RD_DEC(k);
                        PORT_RD_STR("] ");
                        port_crash_name_addr(nm, v);
                        PORT_RD_STR(nm);
                        ++got3;
                    }
                }
            } else {
                PORT_RD_STR("\r\n  esp is outside the thread's stack");
            }
            PORT_RD_STR("\r\n");
        }
    }
    /* THE INSTALL BLOCK. What the asset folder turned out to be, and what
       happened to any save state in it, both settled at boot by
       hal/romdata_loader.cpp and hal/lk7_persist.cpp.

       This exists because of the 2026-08-26 player burst -- not because it
       explains that burst, but because of how long it took to NOT explain it.
       Six dumps, every one "c0000005 +0009d322 access 00000000 at 00000024",
       every one of those facts a symptom. The env block said the asset root was
       a folder named for an old release; the metadata said the build was a new
       one; a human put the two side by side and concluded the install was
       mixed. It was not -- the folder's contents matched the exe, and the real
       defect was a save state that its own build could not reload. A dump line
       stating what the game data ACTUALLY WAS would have closed that off in an
       hour instead of sending a lane after the wrong bug.

       So the block reports measurements, never inferences: a sha the loader
       computed against what the exe was built for. The folder path is printed
       as context; it is never evidence -- the verdict is derived from the
       shas alone, never from the name.

       One line each, pre-composed at boot, so the classifier can key a family
       on "romdata ... MISMATCH" or "savestate refused" without knowing anything
       about either subsystem. It goes ABOVE the env block on purpose: the env
       block is what the process was TOLD, this is what that turned out to
       mean. */
    if (port_install_verdict[0]) {
        PORT_RD_STR("---- install ----\r\n");
        PORT_RD_STR(port_install_verdict);
        PORT_RD_STR("\r\n");
    }
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
       while the SM64DS_TEST_QUARANTINE hook (port/unmatched/func_02043fdc_hostcopy.cpp,
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
        /* run mg16 lane MP2: this name is NOT per-instance, and that is a
           measured decision rather than an oversight -- two copies of the game
           in one folder share crash.txt and exit.txt, so the second to fault
           overwrites the first. Editing this file to suffix them cost the port
           its fixed DS address ranges at startup, bisected and written up in
           the banner of port/hal/instance_tag.h. Read the per-instance rich
           dumps under %TEMP%\sm64ds-crashes when two copies are running.
           COMMENT ONLY: no code in this file changed. */
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
                v < (unsigned)(uintptr_t)base + port_module_span(base) &&
                port_addr_is_exec((uintptr_t)v)) {
                PORT_CRASH_STR("\r\n  +");
                PORT_CRASH_HEX(v - (unsigned)(uintptr_t)base);
                ++printed;
            }
        }
        /* RAW stack words, unfiltered. The filtered list above answers
           which module code addresses are lying around; it cannot answer
           what the call that just jumped to 0 pushed, because a word is
           dropped the moment it is not executable module code, and the
           first word at esp is the only one the faulting call actually
           pushed. Sixteen values as they are, so the reader can do the
           arithmetic the filter refuses to. */
        PORT_CRASH_STR("\r\nesp[0..15]");
        for (i = 0; i < 16; ++i) {
            if (IsBadReadPtr(sp + i, 4)) break;
            PORT_CRASH_STR("\r\n  ");
            PORT_CRASH_HEX(sp[i]);
        }
        /* Candidate receivers and their dispatch tables. On a jump to 0, 1
           or 2 through a virtual slot the receiver is in ECX when MSVC made
           the call and on the stack (so usually still in ESI or EDI) when
           the call came out of a shadow-struct table; dumping all three
           costs nothing and means the crash does not have to be reproduced
           to ask the second question. *reg is the vptr, and the null word is
           one of the entries below: the vptr resolves in the map to the
           class whose table it is, and the index of the 0 is the slot. */
        {
            unsigned regs[3];
            const char *names[3];
            int r;
            regs[0] = cx->Ecx; names[0] = "ecx";
            regs[1] = cx->Esi; names[1] = "esi";
            regs[2] = cx->Edi; names[2] = "edi";
            for (r = 0; r < 3; ++r) {
                unsigned *obj = (unsigned *)regs[r];
                PORT_CRASH_STR("\r\n");
                PORT_CRASH_STR(names[r]);
                PORT_CRASH_STR("-object ");
                PORT_CRASH_HEX(regs[r]);
                if (IsBadReadPtr(obj, 4)) {
                    PORT_CRASH_STR(" (unreadable)");
                    continue;
                }
                /* the words either side of the object. A vptr slot that
                   holds a small integer while a ROM data address sits one
                   or two words away is a field-offset defect, not a
                   missing binding, and the two answers need different
                   fixes; printing the neighbourhood tells them apart
                   without reproducing the crash a second time. */
                PORT_CRASH_STR(" near");
                for (i = -12; i < 12; ++i) {
                    if (IsBadReadPtr(obj + i, 4)) { PORT_CRASH_STR(" ????????"); continue; }
                    PORT_CRASH_STR(" ");
                    PORT_CRASH_HEX(obj[i]);
                }
                PORT_CRASH_STR(" vptr ");
                PORT_CRASH_HEX(obj[0]);
                {
                    unsigned *vt = (unsigned *)obj[0];
                    if (IsBadReadPtr(vt, 4)) {
                        PORT_CRASH_STR(" table unreadable");
                        continue;
                    }
                    PORT_CRASH_STR(" table");
                    for (i = 0; i < 8; ++i) {
                        if (IsBadReadPtr(vt + i, 4)) break;
                        PORT_CRASH_STR(" ");
                        PORT_CRASH_HEX(vt[i]);
                    }
                }
            }
        }
    }
    {
        char *pt = (char *)data_0209ee74;
        PORT_CRASH_STR("\r\nparticle-owner ");
        PORT_CRASH_HEX((unsigned)(uintptr_t)pt);
        if (pt && !IsBadReadPtr(pt + 0x740, 0x40)) {
            unsigned *w = (unsigned *)(pt + 0x740);
            int q;
            PORT_CRASH_STR(" bank+740");
            for (q = 0; q < 16; ++q) {
                PORT_CRASH_STR(" ");
                PORT_CRASH_HEX(w[q]);
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
    /* ONLY the codes the unhandled filter cannot be trusted to see. Anything a
       __except downstream might catch and continue through must NOT reach the
       writer from here: it would spend the one-shot latch on a fault the player
       survived and silently disable the report for the crash that follows. See
       the block comment above crash.txt for the full account.

       The codes below are not survivable-and-continued by the quarantine, and
       both can skip the unhandled filter, so first-chance is the only place
       they can be caught at all. Everything else -- access violation, in-page
       error, illegal instruction, divide by zero, privileged instruction --
       reaches port_fault_probe_with_file when, and only when, nothing handled
       it. */
    switch (ep->ExceptionRecord->ExceptionCode) {
    case 0xC00000FDu:   /* stack overflow: the UEF may never get a stack */
    case 0xC0000409u:   /* stack-buffer-overrun via __fastfail: skips the UEF */
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
                v < (unsigned)(uintptr_t)base + port_module_span(base)) {
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
    /* 8 = the launcher hand-off was refused (hal/handoff_poll.cpp): the launcher starts the game again, nothing died, so no
       exit file and no rich dump (lane SEAMCLEAN1); every other nonzero code is still filed */
    if (code != 0 && code != 8)
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

/* THE CONSOLE CLOSE IS A QUIT, NOT A CRASH (run hunt8, lane CRASHDRV093). The
   game is a console-subsystem exe, so the system gives it a console window,
   and a player who ends the game by closing that window (or Ctrl+C) makes
   Windows run the default handler, which ends the process with exit code
   c000013a. The exit probe above then filed that as a crash: 39 reports from
   17 installs on 0.9.0 and 0.9.1, none of them a fault.

   This handler takes those events (Ctrl+C, Ctrl+Break, console close, logoff,
   shutdown), asks the game window to close the way its X button does so the
   normal quit runs (settings flushed, the usual exit), waits up to four seconds
   for the process to go, and then ends it with exit code 0. The window is found
   by process id through user32, which is resolved here at call time (the kit's
   import table stays kernel32 only). The handler runs on a thread the system
   makes for the event. */
typedef BOOL (WINAPI *port_enum_t)(WNDENUMPROC, LPARAM);
typedef DWORD (WINAPI *port_tid_t)(HWND, LPDWORD);
typedef BOOL (WINAPI *port_post_t)(HWND, UINT, WPARAM, LPARAM);
static port_tid_t port_ctrl_tid;
static port_post_t port_ctrl_post;

static BOOL CALLBACK port_ctrl_close_cb(HWND h, LPARAM l)
{
    DWORD pid = 0;
    (void)l;
    port_ctrl_tid(h, &pid);
    if (pid == GetCurrentProcessId())
        port_ctrl_post(h, 0x0010 /* WM_CLOSE */, 0, 0);
    return TRUE;
}

static BOOL WINAPI port_console_ctrl(DWORD ev)
{
    switch (ev) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    {
        static const char m[] = "[exit] the console was closed or interrupted: quitting as the player asked\r\n";
        DWORD w;
        HMODULE u = LoadLibraryA("user32.dll");
        port_enum_t en = u ? (port_enum_t)GetProcAddress(u, "EnumWindows") : 0;
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), m, sizeof m - 1, &w, 0);
        port_ctrl_tid = u ? (port_tid_t)GetProcAddress(u, "GetWindowThreadProcessId") : 0;
        port_ctrl_post = u ? (port_post_t)GetProcAddress(u, "PostMessageA") : 0;
        if (en && port_ctrl_tid && port_ctrl_post) {
            en(port_ctrl_close_cb, 0);
            Sleep(4000);
        }
        ExitProcess(0);
    }
    default:
        break;
    }
    return FALSE;
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

/* Redirected stdio is fully buffered under the MSVC CRT, so a hard death
   loses every line since the last flush; the two setvbuf calls make captures
   loss-free. Harmless on a console. */
#define PORT_INSTALL_FAULT_PROBE() do { \
        setvbuf(stderr, NULL, _IONBF, 0); \
        setvbuf(stdout, NULL, _IONBF, 0); \
        port_crash_dir_boot(); /* make %TEMP%\sm64ds-crashes, prune to newest 4 */ \
        AddVectoredExceptionHandler(1, port_crash_veh); \
        SetUnhandledExceptionFilter(port_fault_probe_with_file); \
        port_install_exit_probe(); \
        SetConsoleCtrlHandler(port_console_ctrl, TRUE); \
    } while (0)

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
                v < (unsigned)(uintptr_t)base + port_module_span(base) &&
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
                v < (unsigned)(uintptr_t)base + port_module_span(base) &&
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

/* ---- SM64DS_FAULT_TEST=<n>[@frame]: raise a REAL crash on purpose ----------
   Dev builds only (compiled out of a PORT_ROM_CLEAN kit). Lane CRASHWALK1
   wrote it to prove the dump above against faults whose callers are known:
     1  a read through a null pointer + 0x30 on the main thread
     2  the same read on a worker thread the port starts for it
     3  a call into a system DLL that faults there: EnumWindows calls back into
        the port, and the callback hands kernel32's RtlMoveMemory a source of
        0x30, so eip lands in a system DLL with port frames below it (the
        shape of the 2026-10-26 tester dumps)
   The frame defaults to 60. walk_window calls PORT_FAULT_TEST_TICK once per
   frame; nothing runs unless the variable is set. */
#ifndef PORT_ROM_CLEAN
static int *volatile port_fault_test_null;   /* stays 0 */
static __declspec(noinline) int port_fault_test_read(void)
{
    return port_fault_test_null[0x30 / 4];
}
static __declspec(noinline) int port_fault_test_deep(int d)
{
    return d ? port_fault_test_deep(d - 1) + 1 : port_fault_test_read();
}
static DWORD WINAPI port_fault_test_thread(LPVOID unused)
{
    (void)unused;
    return (DWORD)port_fault_test_deep(2);
}
typedef void (WINAPI *port_fault_move_t)(void *, const void *, SIZE_T);
typedef BOOL (WINAPI *port_fault_enum_t)(WNDENUMPROC, LPARAM);
static BOOL CALLBACK port_fault_test_enum_cb(HWND h, LPARAM l)
{
    unsigned sink = 0;
    (void)h;
    ((port_fault_move_t)l)(&sink, (const void *)0x30, 4);
    return sink == 0x12345678;
}
static __declspec(noinline) void port_fault_test_fire(int mode)
{
    if (mode == 1) {
        port_fault_test_deep(2);
    } else if (mode == 2) {
        HANDLE t = CreateThread(0, 0, port_fault_test_thread, 0, 0, 0);
        if (t)
            WaitForSingleObject(t, INFINITE);
    } else if (mode == 3) {
        HMODULE k = GetModuleHandleA("kernel32.dll");
        HMODULE u = LoadLibraryA("user32.dll");
        port_fault_move_t mv = k
            ? (port_fault_move_t)GetProcAddress(k, "RtlMoveMemory") : 0;
        port_fault_enum_t en = u
            ? (port_fault_enum_t)GetProcAddress(u, "EnumWindows") : 0;
        if (mv && en)
            en(port_fault_test_enum_cb, (LPARAM)mv);
    }
}
static void port_fault_test_tick(int frame)
{
    static int mode = -1, at = 60;
    if (mode < 0) {
        const char *e = getenv("SM64DS_FAULT_TEST");
        mode = e ? atoi(e) : 0;
        if (e) {
            const char *a = e;
            while (*a && *a != '@') ++a;
            if (*a == '@') at = atoi(a + 1);
        }
        if (mode)
            fprintf(stderr, "[fault-test] mode %d at frame %d\n", mode, at);
    }
    if (mode > 0 && frame >= at) {
        int m = mode;
        mode = 0;
        fprintf(stderr, "[fault-test] firing mode %d at frame %d\n", m, frame);
        port_fault_test_fire(m);
    }
}
#define PORT_FAULT_TEST_TICK(frame) port_fault_test_tick(frame)
#else
#define PORT_FAULT_TEST_TICK(frame) ((void)0)
#endif

/* ---- external seams for the quarantine walker -----------------------------
   port/unmatched/func_02043fdc_hostcopy.cpp catches per-actor faults but must not
   include this header (it would pull the VEH/detour installers into a plain-C
   actor TU). The one TU that installs the probe (walk_window) defines
   PORT_FAULT_PROBE_DEFINE_EXPORTS before including, which emits these two
   non-static entry points wrapping the static internals above. Every other TU
   that includes this header leaves them out, so there is exactly one
   definition. The walker weak-links against them. */
#ifdef PORT_FAULT_PROBE_DEFINE_EXPORTS
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
#endif

#endif
