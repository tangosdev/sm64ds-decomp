/* NitroSDK FSi_AppendToList: unlink a file from whatever request list it is
 * on, then append it at the tail of `list` (func_0205cdf4 passes the
 * archive's pending list). FSFile starts with its FSFileLink, prev at 0 and
 * next at 4; only the link is spelled out. */
typedef struct FSFile FSFile;

typedef struct FSFileLink {
    FSFile *prev; /* 0x00 */
    FSFile *next; /* 0x04 */
} FSFileLink;

struct FSFile {
    FSFileLink link; /* 0x00 */
};

void func_0205d8d8(int node, int list)
{
    FSFile *file = (FSFile *)node;
    FSFile *prev = file->link.prev;
    FSFile *next = file->link.next;
    FSFile *tail;

    if (prev != 0)
        prev->link.next = next;
    if (next != 0)
        next->link.prev = prev;
    for (tail = (FSFile *)list; tail->link.next != 0; tail = tail->link.next) {
    }
    tail->link.next = file;
    file->link.prev = tail;
    file->link.next = 0;
}
