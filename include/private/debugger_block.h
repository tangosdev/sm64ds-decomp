#ifndef PRIVATE_DEBUGGER_BLOCK_H
#define PRIVATE_DEBUGGER_BLOCK_H

/* The IS-NITRO debugger's 0x20 bytes of the shared work area,
 * HW_ISD_RESERVED (0x027fff60..0x027fff80; OSSystemWork::isd_reserved2).
 * The debugger fills it in; the game's debug-console code only reads it.
 * The halfword at +0x12 mirrors what was last written to the debugger's
 * bus register at 0x09fe2ffe (func_0206ce20 saves both and sets 0x20,
 * func_0206cd9c puts them back). The word at +0x14 holds the debugger's
 * flags (func_0206cf7c tests them) and the word at +0x1c points to the debug-console buffer: func_0206cbc0
 * hands buf + 0x90 to the cartridge sink and func_0206ca7c keeps its byte
 * ring at buf + 0x8000. */
struct DebuggerBlock {
    char pad_00[0x12];
    volatile unsigned short busMirror; /* 0x12 */
    int flags;                         /* 0x14 */
    char pad_18[0x1c - 0x18];
    char *buf; /* 0x1c */
};

#endif
