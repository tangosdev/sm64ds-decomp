struct Vector3;
extern int func_02007d0c(struct Vector3 *self, struct Vector3 *src,
                         int stepX, int stepY, int stepZ);

int func_02007cec(int a, int b, int c) {
    return func_02007d0c((struct Vector3 *)a,
                         (struct Vector3 *)b,
                         c, c, c);
}
