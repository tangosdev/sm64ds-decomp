extern void Vec2_Sub(int *out, int *a, int *b);
extern int func_0203d434(int *p);
extern void func_0203d630(int *p, int x);
typedef struct S {
    int pad[4];
    int x;
    int y;
    int a;
    int b;
} S;
#pragma optimize_for_size on
void func_ov006_021094ac(S *this, int *src) {
    int v[2];
    this->a = src[0];
    this->b = src[1];
    Vec2_Sub(v, &this->a, (int *)this);
    this->x = v[0];
    this->y = v[1];
    if (func_0203d434(&this->x)) {
        func_0203d630(&this->x, 0x18000);
    }
    if (this->x < 0) this->x = -this->x;
    if (this->y < 0) this->y = -this->y;
}
