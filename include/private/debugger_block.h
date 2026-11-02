#ifndef PRIVATE_DEBUGGER_BLOCK_H
#define PRIVATE_DEBUGGER_BLOCK_H

/* The debugger block at main-memory address 0x027fff60. Its word at +0x1c
 * points to the debug-console buffer: func_0206cbc0 hands buf + 0x90 to the
 * cartridge sink and func_0206ca7c keeps its byte ring at buf + 0x8000. Only
 * that field is read; the block's size is not evidenced. */
struct DebuggerBlock {
    char pad_00[0x1c];
    char *buf; /* 0x1c */
};

#endif
