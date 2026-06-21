/* _ZN11RaycastLine4Line3SetERK7Vector3S3_ at 0x020fea84
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

typedef struct { int x, y, z; } Vector3;

typedef struct { Vector3 a, b; } Line;

void _ZN11RaycastLine4Line3SetERK7Vector3S3_(Line *self, const Vector3 *p, const Vector3 *q) {
    self->a.x = p->x;
    self->a.y = p->y;
    self->a.z = p->z;
    self->b.x = q->x;
    self->b.y = q->y;
    self->b.z = q->z;
}
