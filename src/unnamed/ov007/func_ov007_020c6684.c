#include "types.h"
#include "Ov007StrokePath.h"

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in that
 * file, only the slots this function frees. +0x30 is not freed: it is a
 * plain word that func_ov007_020c690c clears. */
typedef struct Ov007Picker {
    char pad_00[0x20];
    Ov007StrokePath *path;      /* 0x20: func_ov007_020c2c68 */
    Vector3 *pathPoints;        /* 0x24 */
    Vector3 **groupPoints;      /* 0x28: func_ov007_020c6e68, per group */
    Vector3 **groupPointCopies; /* 0x2c: func_ov007_020c6e68 copies groupPoints */
    char pad_30[4];
    void ***groupLinks;       /* 0x34: per group (func_ov007_020c6e68) */
    Ov007StrokePath **groups; /* 0x38: committed strokes (func_ov007_020c6d20) */
    void **pointLists;        /* 0x3c: func_ov007_020c798c, per group */
    void **headLinks;         /* 0x40 */
    void **tailLinks;         /* 0x44 */
    u16 *headLinkGroups;      /* 0x48 */
    u16 *tailLinkGroups;      /* 0x4c */
    u16 *headLinkPoints;      /* 0x50 */
    u16 *tailLinkPoints;      /* 0x54 */
    char pad_58[0x14];
    void *posPoint; /* 0x6c: func_ov007_020c844c's Point over pos */
    char pad_70[0x24];
    void *anchorPoint; /* 0x94: func_ov007_020c844c's Point over anchorPos */
    char pad_98[0xc];
    void *link; /* 0xa4: func_ov007_020c80a4; from anchorPoint to posPoint */
} Ov007Picker;

typedef char Ov007Picker_link_must_be_at_0xa4[(int)&((Ov007Picker *)0)->link == 0xa4 ? 1 : -1];

void func_ov007_020c6724(char* self, int flag);
void func_ov007_020c3d1c(void* p);
void func_ov007_020c8440(void* p);
void func_ov007_020c8098(void* p);
void func_ov007_020c2bf8(void* p);

/* Frees the picker func_ov007_020c690c built: its per-group tables, the two
 * points and the link between them, the stroke path, and the picker. */
void func_ov007_020c6684(char* c){
    Ov007Picker *p = (Ov007Picker *)c;
    func_ov007_020c6724(c, 1);
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
    func_ov007_020c3d1c(c);
}
