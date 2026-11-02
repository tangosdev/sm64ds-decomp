/* NitroSDK FSi_AppendToList(elem, list): unlink a file from whatever command
 * list it is on, then append it at the tail of `list`. pret's pokediamond
 * FSi_util.h has it as a static inline; this older SDK calls it out of line,
 * from FSi_SendCommand, which passes the archive's command list. FSFile
 * starts with its FSFileLink, prev at 0 and next at 4. */
#include "private/fs_views.h"

typedef struct FSFile FSFile;

void FSi_AppendToList(FSFile *elem, FSFile *list) {
    FSFile *prev = elem->link.prev;
    FSFile *next = elem->link.next;

    if (prev != 0)
        prev->link.next = next;
    if (next != 0)
        next->link.prev = prev;
    while (list->link.next != 0)
        list = list->link.next;
    list->link.next = elem;
    elem->link.prev = list;
    elem->link.next = 0;
}
