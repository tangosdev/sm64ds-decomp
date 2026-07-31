#include "types.h"
struct Model {
    u32 data[0x14]; /* 0x50 bytes */
};

struct CapIcon {
    u32* vTable;
    u32 actor;
    u32 objID;
    u32 next;
    u32 prev;
    u32 arrayIndex;
    u8 character;
    u8 unk19;
    u8 unk1a;
    u8 unk1b;
};

struct CapEnemy {
    u32 vtable;              /* 0x00 */
    u32 pad_4[0x44];         /* 0x04..0x113 (0x110 bytes) */
    struct Model capModel;   /* 0x114 */
    struct CapIcon capIcon;  /* 0x164 */
};

extern u32 _ZTV8CapEnemy[];
extern void _ZN5EnemyC2Ev(struct CapEnemy*);
extern void _ZN5ModelC1Ev(struct Model*);
extern void _ZN7CapIconC1Ev(struct CapIcon*);

struct CapEnemy* _ZN8CapEnemyC2Ev(struct CapEnemy* this) {
    _ZN5EnemyC2Ev(this);
    this->vtable = (u32)_ZTV8CapEnemy;
    _ZN5ModelC1Ev(&this->capModel);
    _ZN7CapIconC1Ev(&this->capIcon);
    return this;
}
