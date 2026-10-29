extern int data_020a6134[];

/* NitroSDK OSThread, the fields read here; names and offsets as
 * func_02058200.cpp (OS_CreateThread) spells them. */
typedef struct OSThread {
    char pad_00[0x68];
    struct OSThread *next; /* 0x68 */
    unsigned int id;       /* 0x6c */
    unsigned int priority; /* 0x70 */
} OSThread;

/* NitroSDK OSi_InsertThreadToList: link node into the thread list (word 3
 * of data_020a6134) ahead of the first thread with a priority no higher
 * than its own, keeping the list sorted by priority. */
void func_020584d0(char* node)
{
    /* cur/prev are pointers, not ints: with int-typed nodes mwcc CSEs the
       prev->next address into r2 for the read and the write (addne r2,ip,#0x68 /
       ldrne r1,[r2] / strne r0,[r2]), one word over the cartridge's
       ldrne r1,[ip,#0x68] / strne r0,[ip,#0x68]. */
    OSThread *thread = (OSThread *)node;
    OSThread *cur = (OSThread *)data_020a6134[3];
    OSThread *prev = 0;

    while (cur != 0 && cur->priority < thread->priority) {
        prev = cur;
        cur = cur->next;
    }

    if (prev == 0) {
        thread->next = (OSThread *)data_020a6134[3];
        data_020a6134[3] = (int)thread;
    } else {
        thread->next = prev->next;
        prev->next = thread;
    }
}
