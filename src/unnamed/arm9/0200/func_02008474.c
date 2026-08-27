struct Camera;
struct Vector3_16;
struct dActor_c;

extern int func_02008500(struct dActor_c *actor, const char *data);
extern int func_020084b0(struct Camera *self, const unsigned char *data, int a, int b);

int func_02008474(struct Camera *self, struct Vector3_16 *v, int a, int b) {
    func_02008500((struct dActor_c *)self, (const char *)v);
    return func_020084b0(self, (const unsigned char *)((const unsigned char *)v + 6), a, b);
}
