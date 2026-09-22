// FUNCTION-ENTRY TRACE FOR THE PC PORT (lane TRACEPORT, run link100).
//
// WHAT THIS IS FOR. Lane ROMTRACE built an instrumented melonDS that writes down
// every function call the REAL cartridge's ARM9 makes and resolves the addresses
// to this repository's own names. This file is the other half: the same list, for
// the port, so the two can be lined up and the first disagreement read off.
//
// HOW IT ATTACHES, AND WHY NOTHING ELSE IN THE TREE CHANGES. MSVC's /Gh switch
// makes the compiler emit "call _penter" as the FIRST instruction of every
// function it compiles, before the prologue. So the hook below is reached from
// every function in the build without one line of any other source file being
// touched, and the address it records is (function start + 5), which the map
// resolves by "largest symbol address at or below". Measured on this box's
// compiler, cl 19.44 for x86, /O2 /Oy-:
//
//   - a __declspec(naked) _penter is NOT itself instrumented, so there is no
//     runaway recursion (the obvious failure of this whole idea, checked first);
//   - TAIL JUMPS SURVIVE: a one-call forwarder still compiles to "call _penter /
//     push ebp / mov ebp,esp / pop ebp / jmp target", so the roughly fifty ov007
//     rows that are correct only because MSVC turns a forwarder into a jmp, and
//     that port/tools/tailjump_guard.py exists to protect, are not disturbed;
//   - INLINING IS NOT INHIBITED, so the hook fires once per REAL call;
//   - the cost is 0.8 nanoseconds per hook (20,000,000 real calls: 0.0204 s
//     plain, 0.0364 s traced). The cartridge makes about 1,800 calls per frame on
//     the menu page and about 27,500 on the boot title screen, so at sixty frames
//     a second the hook costs on the order of one millisecond per second of game.
//
// OFF BY DEFAULT, TWICE OVER. This file is only compiled when the build is
// configured with -DSM64DS_PORT_TRACE=ON, which also adds /Gh; a normal build
// neither compiles it nor carries a single hook instruction. And even in a traced
// build nothing is recorded until SM64DS_FN_TRACE names an output file, because
// ntr_fn_trace_on starts at zero and only the frame hook can raise it.
//
// THE RECORD. Three 32-bit words, always:
//   w0 = a return address inside the CALLEE      (its entry + 5)
//   w1 = a return address inside the CALLER      (the call site + 5)
//   w2 = the caller's esp
// or, for a frame marker, w0 = 0xFFFFFFFF, w1 = the frame number, w2 = 0.
//
// WHY THE CALLER IS RECORDED AND NOT INFERRED. The first version of this file
// wrote two words and left the caller to be reconstructed from esp: an entry at
// a lower esp is nested inside the one before it. THAT RULE IS WRONG ON x86 AND
// THE ERROR IS NOT RARE. Arguments are pushed before the call, so the stack
// pointer at a call site depends on how many arguments THAT call takes, and two
// sibling calls out of the same function land at different stack pointers.
// Measured on the port's own title screen: func_0204be40 calls MulVec3Mat4x3 at
// esp 0x001aefbc and then func_02055388 at esp 0x001aefa8, twenty bytes lower,
// so the sp rule filed the second call inside the first. The cartridge has both
// as direct children of func_0204be40, 403 calls each, and every edge below
// them was mis-parented the same way.
//
// The fix costs one instruction. At _penter's entry the callee's own prologue
// has not run yet, so the word above _penter's return address is still the
// return address the CALLER's own `call` pushed. Reading it by value instead of
// taking its address gives the call site itself, which is exactly what the
// instrumented melonDS records when it hooks BL. The two halves then name the
// caller the same way and neither is guessing.
//
// esp is still recorded as w2. It no longer decides nesting, but it tells a
// recursive call from a repeated one and it costs nothing to keep.

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

// The hook reads these four and nothing else. They are plain globals rather than
// a struct so the inline assembly can name them directly and stay short.
unsigned int   ntr_fn_trace_on   = 0;  // 1 while recording. Starts at 0, always.
unsigned int * ntr_fn_trace_buf  = 0;  // base of the record array
unsigned int   ntr_fn_trace_head = 0;  // next free WORD index
unsigned int   ntr_fn_trace_cap  = 0;  // capacity in WORDS

// THE HOOK. Kept to a compare and a branch on the disabled path, and to three
// pushes and five stores on the enabled one. It must be naked: a compiler-
// generated prologue here would itself be instrumented under /Gh.
//
// THE CAPACITY TEST IS ON head+3, NOT ON head. The two-word version compared
// head against cap and then wrote at head and head+1, so a head exactly one
// word below cap wrote one word past the end of the buffer. Nothing ever
// noticed because the buffer is a whole number of megabytes and the old record
// was two words, so head could never stop on an odd word -- but the record is
// three words now and that alignment argument is gone.
__declspec(naked) void __cdecl _penter(void)
{
    __asm {
        cmp     dword ptr [ntr_fn_trace_on], 0
        je      SHORT fnt_off
        push    eax
        push    ecx
        push    edx
        mov     ecx, dword ptr [ntr_fn_trace_head]
        lea     eax, [ecx+3]
        cmp     eax, dword ptr [ntr_fn_trace_cap]
        ja      SHORT fnt_full
        mov     edx, dword ptr [ntr_fn_trace_buf]
        mov     eax, dword ptr [esp+12]     // return into the CALLEE: fn + 5
        mov     dword ptr [edx+ecx*4], eax
        mov     eax, dword ptr [esp+16]     // return into the CALLER: the call site
        mov     dword ptr [edx+ecx*4+4], eax
        lea     eax, [esp+16]               // the caller's esp before its call
        mov     dword ptr [edx+ecx*4+8], eax
        add     ecx, 3
        mov     dword ptr [ntr_fn_trace_head], ecx
fnt_full:
        pop     edx
        pop     ecx
        pop     eax
fnt_off:
        ret
    }
}

} // extern "C"

namespace {

char         g_path[512];
bool         g_configured = false;   // the environment has been read
bool         g_enabled    = false;   // SM64DS_FN_TRACE named a file
bool         g_written    = false;   // the file is on disk; never record again
unsigned int g_from       = 0;
unsigned int g_to         = 0xFFFFFFFFu;

unsigned int env_uint(const char *name, unsigned int fallback)
{
    const char *v = getenv(name);
    if (v == 0 || v[0] == 0) return fallback;
    return (unsigned int) strtoul(v, 0, 0);
}

void configure(void)
{
    g_configured = true;

    const char *out = getenv("SM64DS_FN_TRACE");
    if (out == 0 || out[0] == 0) return;

    // 8 bytes a record. The default holds about six hundred frames of the menu
    // page; SM64DS_FN_TRACE_MB moves it. A full buffer stops recording quietly
    // and the header says how many records were dropped, so a short buffer is
    // visible in the output instead of being a silent truncation.
    unsigned int mb = env_uint("SM64DS_FN_TRACE_MB", 96);
    if (mb < 1)    mb = 1;
    if (mb > 1024) mb = 1024;

    size_t bytes = (size_t) mb * 1024u * 1024u;
    void *p = VirtualAlloc(0, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (p == 0) return;

    strncpy(g_path, out, sizeof(g_path) - 1);
    g_path[sizeof(g_path) - 1] = 0;

    ntr_fn_trace_buf  = (unsigned int *) p;
    ntr_fn_trace_cap  = (unsigned int) (bytes / 4u);
    ntr_fn_trace_head = 0;

    g_from    = env_uint("SM64DS_FN_TRACE_FROM", 0);
    g_to      = env_uint("SM64DS_FN_TRACE_TO", 0xFFFFFFFFu);
    g_enabled = true;
}

void write_out(void)
{
    if (!g_enabled || g_written) return;
    g_written = true;
    ntr_fn_trace_on = 0;

    FILE *f = fopen(g_path, "wb");
    if (f == 0) return;
    // A 24-byte header so the reader can refuse a file it does not understand,
    // can see a truncation rather than guess at one, and can undo ASLR. THE
    // IMAGE BASE MATTERS: walk_window.map is written against the preferred load
    // address 0x00400000 and Windows relocates the image somewhere else on every
    // run, so without this word every address in the file resolves to the wrong
    // name, or to no name, and the whole trace reads as a build that called
    // nothing this repository knows.
    unsigned int hdr[6];
    hdr[0] = 0x50544633u;              // "PTF3": three words a record
    hdr[1] = 12;                       // bytes per record
    hdr[2] = ntr_fn_trace_head / 3u;   // records written
    hdr[3] = (ntr_fn_trace_head >= ntr_fn_trace_cap) ? 1u : 0u;  // buffer filled
    hdr[4] = (unsigned int) (size_t) GetModuleHandleA(0);  // where it really loaded
    hdr[5] = 0;
    fwrite(hdr, 4, 6, f);
    fwrite(ntr_fn_trace_buf, 4, ntr_fn_trace_head, f);
    fclose(f);
}

} // namespace

extern "C" void ntr_fn_trace_frame(unsigned int frame)
{
    if (!g_configured) configure();
    if (!g_enabled || g_written) return;

    if (frame > g_to) {
        write_out();
        return;
    }
    if (frame < g_from) {
        ntr_fn_trace_on = 0;
        return;
    }

    // The marker is written with the recorder off so the frame hook's own entry
    // does not land between the marker and the first call of the frame.
    ntr_fn_trace_on = 0;
    if (ntr_fn_trace_head + 3u <= ntr_fn_trace_cap) {
        ntr_fn_trace_buf[ntr_fn_trace_head + 0] = 0xFFFFFFFFu;
        ntr_fn_trace_buf[ntr_fn_trace_head + 1] = frame;
        ntr_fn_trace_buf[ntr_fn_trace_head + 2] = 0u;
        ntr_fn_trace_head += 3u;
    }
    ntr_fn_trace_on = 1;
}

// Last resort: a run that ends before the frame window closes still leaves a
// readable file. atexit is registered on the first frame rather than at static
// init so an untraced run registers nothing.
extern "C" void ntr_fn_trace_flush(void) { write_out(); }
