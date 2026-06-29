/* CapEnemy::~CapEnemy (D2/base) at 0x0200651c
 *
 * CapEnemy : Enemy.  Members: capModel (Model) @0x114, capIcon (CapIcon) @0x164.
 *   [this+0] = _ZTV8CapEnemy (0x02108284)
 *   CapIcon::~CapIcon(this+0x164)         (0x020ab3a0)
 *   Model::~Model(this+0x114)             (0x02016d20, D1)
 *   Enemy::~Enemy(this)                   (0x020aed18, base dtor)
 * returns this.
 */

struct CapEnemy {
    void **vtable;          /* 0x00 */
    char pad0[0x114 - 4];
    char capModel[0x50];    /* 0x114 */
    char capIcon[0x1c];     /* 0x164 */
};

extern void *_ZTV8CapEnemy[];

extern void _ZN7CapIconD2Ev(void *icon);             /* 0x020ab3a0 */
extern void *_ZN5ModelD1Ev(void *model);             /* 0x02016d20 */
extern void *func_ov002_020aed18(struct CapEnemy *thiz);   /* 0x020aed18 */

struct CapEnemy *_ZN8CapEnemyD2Ev(struct CapEnemy *thiz)
{
    thiz->vtable = (void **)_ZTV8CapEnemy;
    _ZN7CapIconD2Ev(thiz->capIcon);
    _ZN5ModelD1Ev(thiz->capModel);
    func_ov002_020aed18(thiz);
    return thiz;
}
