// func_02008028 @ 0x02008028 (Camera member, size 0x58)
// Steps the camera's pos.y (0x90) toward (pos.y + something->field_0x60) using
// Math_Function_0203b0fc with step 0x11e and unbounded maxDelta (0x7fffffff).
// First calls func_02009414(self) for side effects, then looks up an object via
// func_0200e55c(0x13) and adds its field_0x60 to the current pos.y as the target.
// The clamped value is staged on the stack and written back to pos.y. Returns 1.

typedef int Fix12i;

typedef struct Camera {
    char _pad0[0x90];
    Fix12i posY;        // 0x90 (interior of Vector3 pos @0x8c)
} Camera;

typedef struct Obj {
    char _pad0[0x60];
    Fix12i field_0x60;  // 0x60
} Obj;

extern void func_02009414(Camera* self);                                          // 0x02009414
extern Obj* func_0200e55c(int id);                                                // 0x0200e55c
extern void Math_Function_0203b0fc(int* val, int target, int step, int maxDelta); // 0x0203b0fc

int func_02008028(Camera* self) {
    Fix12i y;
    Obj* obj;

    y = self->posY;
    func_02009414(self);
    obj = func_0200e55c(0x13);
    Math_Function_0203b0fc(&y, self->posY + obj->field_0x60, 0x11e, 0x7fffffff);
    self->posY = y;
    return 1;
}