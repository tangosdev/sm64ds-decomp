/* func_020075b4 @ 0x020075b4, size 0x78, ARM.
 * Camera helper: measures the dist / vert-angle / horz-angle from the camera
 * owner's position to the camera's saved look-at point (func_02007c9c), then
 * forwards the result (with biased horizontal angle and fixed magnitudes) into
 * func_020070e8. Always returns 1.
 */

typedef struct { int x, y, z; } Vector3;

typedef struct Camera {
    char _pad0[0xb0];
    Vector3 savedLookAt;   /* 0xb0 */
    char _pad1[0x110 - 0xbc];
    void   *owner;         /* 0x110 (dActor_c*, pos @ +0x5c) */
} Camera;

extern void func_02007c9c(const Vector3 *v0, const Vector3 *v1,
                          int *outDist, short *outVertAng, short *outHorzAng);
extern void func_020070e8(Camera *cam, int dist, int a3, short vertAng,
                          short a5, short horzAng, short a7);

int func_020075b4(Camera *cam) {
    int dist;
    short vertAng;
    short horzAng;

    func_02007c9c((const Vector3 *)((char *)cam->owner + 0x5c),
                  &cam->savedLookAt, &dist, &vertAng, &horzAng);
    func_020070e8(cam, dist, 0x199, vertAng,
                  0x20, (short)(horzAng + 0x1200), 0x14);
    return 1;
}
