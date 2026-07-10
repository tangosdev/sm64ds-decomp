/* func_ov006_02120b30 at 0x02120b30
 *
 * Initializes a particle/effect descriptor with fixed parameters and
 * tail-calls func_ov006_02120bc8.
 */
typedef unsigned short u16;

struct Desc {
    char _pad0[4];
    int a;       /* +0x04 = 0x110000 */
    int b;       /* +0x08 = arg1 */
    int c;       /* +0x0c = 0xc0000 */
    int d;       /* +0x10 = arg1 */
    int e;       /* +0x14 = -0x4000 */
    int f;       /* +0x18 = 0 */
    u16 g;       /* +0x1c = 0x80 */
    u16 h;       /* +0x1e = arg2 */
    u16 i;       /* +0x20 = 1 */
};

extern void func_ov006_02120bc8(struct Desc *self);

void func_ov006_02120b30(struct Desc *self, int x, u16 y)
{
    self->a = 0x110000;
    self->b = x;
    self->e = -0x4000;
    self->f = 0;
    self->c = 0xc0000;
    self->d = x;
    self->g = 0x80;
    self->i = 1;
    self->h = y;
    func_ov006_02120bc8(self);
}
