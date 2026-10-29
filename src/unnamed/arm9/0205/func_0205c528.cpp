//cpp

namespace IRQ {
int Disable(void);
void Restore(unsigned int);
}
extern "C" {

    void OS_SleepThread(void *p);
}

/* NitroSDK FSArchive, the fields touched here; names follow the SDK's, as
 * func_0205c5e4.cpp (sync_q, flag) and func_0205c9b8.c (table_func) spell them. */
struct FSArchive;
typedef int (*FS_ARCHIVE_TABLE_FUNC)(FSArchive *p_arc, int dst, int pos, int size);
struct FSArchive {
    char pad_00[0xc];
    int sync_q; /* 0x0c -- a synchronous caller waits here */
    int flag;   /* 0x10 */
    char pad_14[0x30];
    FS_ARCHIVE_TABLE_FUNC table_func; /* 0x44 */
};

/* NitroSDK FSiSyncReadParam: the archive and the table position read next. */
struct FSiSyncReadParam {
    FSArchive *arc; /* 0x0 */
    int pos;        /* 0x4 */
};

/* Bit 0x200 of flag (FS_ARCHIVE_FLAG_IS_SYNC) marks a request still in
   flight. The if/return body is measured: `return (flag & 0x200) != 0;'
   lets the loop guard get copied. */
static inline int IsBusy(FSArchive *p_arc) {
    if (p_arc->flag & 0x200)
        return 1;
    return 0;
}

/* NitroSDK FSi_ReadTable(p, dst, len): mark the archive IS_SYNC and read len
 * bytes at p->pos through table_func. SUCCESS / FAILURE (0, 1) clear the
 * mark; PROC_ASYNC (6) sleeps on sync_q until it clears. Then advance pos. */
extern "C" void func_0205c528(FSiSyncReadParam *p, int dst, int len) {
    FSArchive *arc = p->arc;
    arc->flag |= 0x200;
    int res = arc->table_func(arc, dst, p->pos, len);
    switch (res) {
    case 0:
    case 1:
        arc->flag &= ~0x200;
        break;
    case 6: {
        unsigned int saved = (unsigned int)IRQ::Disable();
        while (IsBusy(arc)) {
            OS_SleepThread(&arc->sync_q);
        }
        IRQ::Restore(saved);
        break;
    }
    }
    p->pos += len;
}
