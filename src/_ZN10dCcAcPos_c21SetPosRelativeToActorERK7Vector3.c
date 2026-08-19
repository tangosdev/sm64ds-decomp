typedef int Fix12i;
typedef short s16;

typedef struct Vector3 { Fix12i x, y, z; } Vector3;

typedef struct dActor_c {
    char pad[0x5c];
    Vector3 pos;
    char pad2[0x24];  /* 0x68 to 0x8c */
    s16 angX;
    s16 angY;
    s16 angZ;
} dActor_c;

typedef struct MovingCylinderClsnWithPos {
    char pad[0x30];
    dActor_c *owner;
    Vector3 pos;
} MovingCylinderClsnWithPos;

extern void Vec3_RotateYAndTranslate(Vector3* res, const Vector3* translation, s16 angY, const Vector3* v);

void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(MovingCylinderClsnWithPos* self, const Vector3* v) {
    dActor_c *actor = self->owner;
    Vec3_RotateYAndTranslate(&self->pos, &actor->pos, actor->angY, v);
}
