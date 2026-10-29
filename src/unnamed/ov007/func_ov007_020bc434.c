#include "types.h"

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates and
 * func_ov007_020bc47c sets up; only the slots this file resets. */
typedef struct Ov007Model {
    char pad_00[0x34];
    Vector3 pos;   /* 0x34: func_ov007_020bc968 copies it into the matrix translation */
    Vector3 scale; /* 0x40: ModelComponents::Render's scale (func_ov007_020bbff0) */
    u16 rotX;      /* 0x4c: func_ov007_020c39f8's angles (func_ov007_020bc968) */
    u16 rotY;      /* 0x4e */
    u16 rotZ;      /* 0x50 */
} Ov007Model;

void func_ov007_020bc434(void* c){
    Ov007Model *m = (Ov007Model *)c;
    m->pos.z = 0;
    m->pos.y = m->pos.z;
    m->pos.x = m->pos.y;
    m->scale.z = 0x1000;
    m->scale.y = m->scale.z;
    m->scale.x = m->scale.y;
    m->rotZ = 0;
    m->rotY = m->rotZ;
    m->rotX = m->rotY;
}
