/* _ZN18MovingCylinderClsn10GetOwnerIDEv at 0x0201493c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

struct Owner { char pad[4]; int id; };
struct MovingCylinderClsn { char pad[0x30]; struct Owner *owner; };
int _ZN18MovingCylinderClsn10GetOwnerIDEv(struct MovingCylinderClsn *this) {
    return this->owner->id;
}
