//cpp
extern "C" {
extern void func_ov007_020bfe4c(void *o, int x, int y, int z, void *out);

/* The 0xd8-byte camera func_ov007_020c023c allocates; names as in
   func_ov007_020c0128, only the slots this function reads. */
struct Ov007Camera {
    char pad_00[0x2c];
    int distance; /* 0x2c */
    char pad_30[0x88];
    /* 0xb8..0xbb: the screen rectangle in pixels; func_ov007_020c0128 sets
     x 0..0xff, y 0..0xbf (the whole 256x192 screen) */
    unsigned char viewportMinX;
    unsigned char viewportMaxX;
    unsigned char viewportMinY;
    unsigned char viewportMaxY;
};

typedef struct {
    int x, y, z;
} Vec3;

/* The 0xd0-byte touch picker func_ov007_020c690c builds; names as in
   func_ov007_020c684c, only the slots this function touches. */
struct Ov007Picker {
    char pad_00[0xa8];
    Ov007Camera *camera; /* 0xa8 */
    char pad_ac[0xc];
    Vec3 boundsMin; /* 0xb8 */
    Vec3 boundsMax; /* 0xc4 */
};

/* Unprojects the camera's screen rectangle at the camera distance into the
   picker's clamp rectangle; screen y runs down, so the two y bounds swap. */
void func_ov007_020c510c(char *c) {
    Ov007Picker *p = (Ov007Picker *)c;
    int neg;
    Ov007Camera *o = p->camera;
    neg = -o->distance;
    func_ov007_020bfe4c(o, o->viewportMinX << 0xc, o->viewportMinY << 0xc, neg, &p->boundsMin);
    func_ov007_020bfe4c(o, o->viewportMaxX << 0xc, o->viewportMaxY << 0xc, neg, &p->boundsMax);
    {
        int t = p->boundsMin.y;
        p->boundsMin.y = p->boundsMax.y;
        p->boundsMax.y = t;
    }
}
}
