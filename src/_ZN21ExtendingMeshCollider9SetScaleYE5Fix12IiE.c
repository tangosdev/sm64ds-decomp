typedef int Fix12i;

typedef struct ExtendingMeshCollider {
    char _pad[0x1c8];
    Fix12i scaleY;
    Fix12i invScaleY;
} ExtendingMeshCollider;

extern Fix12i func_02053200(Fix12i x);

void _ZN21ExtendingMeshCollider9SetScaleYE5Fix12IiE(ExtendingMeshCollider* this, Fix12i newScaleY) {
    this->scaleY = newScaleY;
    if (this->scaleY == 0) return;
    this->invScaleY = func_02053200(this->scaleY);
}