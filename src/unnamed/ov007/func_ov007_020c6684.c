#include "types.h"
#include "Ov007StrokePath.h"
#include "Ov007Picker.h"
#include "Ov007MenuMem.h"

void func_ov007_020c8440(void* p);
void func_ov007_020c8098(void* p);
void func_ov007_020c2bf8(void* p);

/* Frees the picker func_ov007_020c690c built: its per-group tables, the two
 * points and the link between them, the stroke path, and the picker. */
void func_ov007_020c6684(Ov007Picker *p) {
    func_ov007_020c6724(p, 1);
    func_ov007_020c3d1c(p->groupPoints);
    func_ov007_020c3d1c(p->groupPointCopies);
    func_ov007_020c3d1c(p->groups);
    func_ov007_020c3d1c(p->pointLists);
    func_ov007_020c3d1c(p->headLinkGroups);
    func_ov007_020c3d1c(p->tailLinkGroups);
    func_ov007_020c3d1c(p->headLinkPoints);
    func_ov007_020c3d1c(p->tailLinkPoints);
    func_ov007_020c3d1c(p->groupLinks);
    func_ov007_020c3d1c(p->headLinks);
    func_ov007_020c3d1c(p->tailLinks);
    func_ov007_020c8440(p->posPoint);
    func_ov007_020c8440(p->anchorPoint);
    func_ov007_020c8098(p->link);
    func_ov007_020c2bf8(p->path);
    func_ov007_020c3d1c(p->pathPoints);
    func_ov007_020c3d1c(p);
}
