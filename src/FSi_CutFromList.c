/* NitroSDK FSi_CutFromList(elem): unlink a file from its archive's command
 * list and clear its link. pret's pokediamond FSi_util.h has it as a static
 * inline over FSi_CutFromListCore; this older SDK calls it out of line, from
 * FSi_ReleaseCommand. */
#include "private/fs_views.h"

typedef struct FSFile FSFile;

void FSi_CutFromList(FSFile *elem) {
    FSFile *prev = elem->link.prev;
    FSFile *next = elem->link.next;
    if (prev)
        prev->link.next = next;
    if (next)
        next->link.prev = prev;
    elem->link.next = elem->link.prev = 0;
}
