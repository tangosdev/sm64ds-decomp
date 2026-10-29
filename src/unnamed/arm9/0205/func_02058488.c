#include "types.h"

/* NitroSDK OSi_RemoveThreadFromList: unlink target from the thread info's
 * priority list. The OSThread / OSThreadInfo views carry func_02058308.c's
 * names for the fields used here (next at 0x68, list at 0x0c). The caller
 * (func_020581a8) passes the thread as an int. */
typedef struct OSThread {
    u8 context[0x64];      /* 0x00 -- OSContext */
    s32 state;             /* 0x64 */
    struct OSThread *next; /* 0x68 */
} OSThread;

typedef struct OSThreadInfo {
    u16 isNeedRescheduling; /* 0x00 */
    u16 max_entry;          /* 0x02 */
    u16 irqDepth;           /* 0x04 */
    u16 padding;            /* 0x06 */
    OSThread *current;      /* 0x08 */
    OSThread *list;         /* 0x0c -- priority-sorted, linked through next */
} OSThreadInfo;

extern int data_020a6134[];
#define THREAD_INFO (*(OSThreadInfo *)data_020a6134)

void func_02058488(int target) {
    OSThread *thread = (OSThread *)target;
    OSThread *cur;
    OSThread *prev;
    cur = THREAD_INFO.list;
    prev = 0;
    while (cur != 0 && cur != thread) {
        prev = cur;
        cur = cur->next;
    }
    if (prev == 0)
        THREAD_INFO.list = thread->next;
    else
        prev->next = thread->next;
}
