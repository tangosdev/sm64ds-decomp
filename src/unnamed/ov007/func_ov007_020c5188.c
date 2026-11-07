#include "common.h"
#include "nitro/fx/fx.h"
#include "Ov007Picker.h"

extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void func_ov007_020c7560(Ov007PointList *list, int stiffness, int damping);
extern void func_ov007_020c7b2c(struct Ov007Link *link, int stiffness, int damping);
extern void func_ov007_020c81a0(Ov007Follower *f, int damping, int shift);
extern void func_ov007_020c831c(Ov007Follower *f, int damping, int shift);

#define DIVR(x) FX_MUL(x, -1000)

void func_ov007_020c5188(char *sb, int arg1) {
    Ov007Picker *self = (Ov007Picker *)sb;
    int p;
    int oc;
    int count;
    int damping;
    int fp;
    int r8;
    int start_fp;
    int start_r8;
    int v18;
    int v1c;
    int v20;
    struct Vector3 backup;
    Vector3 *min;
    Vector3 *max;

    p = arg1;
    damping = self->params->damping;
    count = 1 << p;
    backup = self->pos;
    min = &self->boundsMin;
    max = &self->boundsMax;
    AddVec3(&self->posOffset, &self->pos, &self->pos);

    oc = 0;
    if (count > 0) {
        start_fp = 0;
        start_r8 = 0;
        v18 = 0;
        v1c = 0x7d0;
        v20 = 0;
        do {
            fp = start_fp;
            if (self->groupCount > 0) {
                do {
                    Ov007PointList *r5 = self->pointLists[fp];
                    func_ov007_020c7560(r5, self->springConstants[0], self->springConstants[1]);
                    if (fp >= 1) {
                        func_ov007_020c7b2c(self->headLinks[fp], self->springConstants[4],
                                            self->springConstants[5]);
                        func_ov007_020c7b2c(self->tailLinks[fp], self->springConstants[4],
                                            self->springConstants[5]);
                    }
                    r8 = start_r8;
                    if (r5->count > 0) {
                        do {
                            Ov007Follower *node = r5->items[r8];
                            func_ov007_020c7b2c(self->groupLinks[fp][r8], self->springConstants[2],
                                                self->springConstants[3]);
                            func_ov007_020c81a0(node, damping, p);
                            {
                                Vector3 *pt = node->target;
                                if (min->x > pt->x) {
                                    pt->x = min->x;
                                    node->velocity.x = DIVR(node->velocity.x);
                                } else if (max->x < pt->x) {
                                    pt->x = max->x;
                                    node->velocity.x = DIVR(node->velocity.x);
                                }
                            }
                            {
                                Vector3 *pt = node->target;
                                if (min->y > pt->y) {
                                    pt->y = min->y;
                                    node->velocity.y = DIVR(node->velocity.y);
                                } else if (max->y < pt->y) {
                                    pt->y = max->y;
                                    node->velocity.y = DIVR(node->velocity.y);
                                }
                            }
                            r8++;
                        } while (r8 < r5->count);
                    }
                    fp++;
                } while (fp < self->groupCount);
            }
            if (self->mode == 2) {
                self->posPoint->force.z = v18;
                self->posPoint->force.y = self->posPoint->force.z;
                self->posPoint->force.x = self->posPoint->force.y;
                func_ov007_020c7b2c(self->link, 0xbb8, v1c);
                func_ov007_020c831c(self->posPoint, v20, p);
            }
            oc++;
        } while (oc < count);
    }

    self->pos = backup;
}
