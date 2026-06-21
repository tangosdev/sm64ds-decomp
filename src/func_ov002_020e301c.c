/* func_ov002_020e301c at 0x020e301c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct Camera;
struct Vector3;
extern void func_0200d8c8(struct Camera* cam, const struct Vector3* v, int strength);
extern void* data_0209f318;

void func_ov002_020e301c(int* self)
{
    if (self[2] != 2) return;
    if (self[0x684/4] - self[0x60/4] < 0x190000) return;
    func_0200d8c8((struct Camera*)data_0209f318,
                  (const struct Vector3*)((char*)self + 0x5c), 0x320000);
}
