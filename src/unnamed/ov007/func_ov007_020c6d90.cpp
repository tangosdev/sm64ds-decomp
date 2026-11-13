//cpp
#include "Ov007Picker.h"
#include "Ov007MenuMem.h"

extern "C" void func_ov007_020c78dc(Ov007PointList *list);
/* A long-branch veneer to func_ov007_020c3d1c, the free hook. */
extern "C" void func_ov007_020c8098(void *p);
extern "C" void func_ov007_020c2bf8(void *path);

/* Frees committed group idx, if there is one: its point list, its points
 * and their rest copies, its links, its stroke, its head and tail links. */
void func_ov007_020c6d90(Ov007Picker *c, int idx) {
    if (c->groups[idx] == 0)
        return;
    func_ov007_020c78dc(c->pointLists[idx]);
    func_ov007_020c3d1c(c->groupPoints[idx]);
    func_ov007_020c3d1c(c->groupPointCopies[idx]);
    {
        int i;
        for (i = 0; i < c->groups[idx]->pointCount; i++)
            func_ov007_020c8098(c->groupLinks[idx][i]);
    }
    func_ov007_020c2bf8(c->groups[idx]);
    func_ov007_020c3d1c(c->groupLinks[idx]);
    func_ov007_020c8098(c->headLinks[idx]);
    func_ov007_020c8098(c->tailLinks[idx]);
    c->groups[idx] = 0;
}
