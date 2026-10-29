/* The 0x2c-byte vector tween: Ov007Tween's prefix (include/Ov007Tween.h),
 * then the from/to vectors func_ov007_020c8de0 blends (as in
 * func_ov007_020b1718). Only the slot this file writes is spelled. */
typedef struct Ov007VecTween {
    char pad_00[0x10];
    void *out; /* 0x10: Ov007Tween's out; the Vector3 func_ov007_020c8de0 writes */
    char pad_14[0x18];
} Ov007VecTween;

typedef char Ov007VecTween_size_must_be_0x2c[sizeof(Ov007VecTween) == 0x2c ? 1 : -1];

extern void* func_ov007_020c3d40(int a, int sz);
extern void func_ov007_020c90f8(void* p);
/* Allocates a vector tween bound to r5, as func_ov007_020c908c does a scalar one. */
void* func_ov007_020c8f64(void* r5){
    Ov007VecTween *o = (Ov007VecTween *)func_ov007_020c3d40(0, sizeof(Ov007VecTween));
    o->out = r5;
    func_ov007_020c90f8(o);
    return o;
}
