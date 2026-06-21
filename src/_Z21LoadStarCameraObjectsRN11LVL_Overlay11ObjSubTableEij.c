/* _Z21LoadStarCameraObjectsRN11LVL_Overlay11ObjSubTableEij at 0x020fe3cc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

struct ObjSubTable {
    int a;
    int **p;
};
extern int *data_02092134;

void _Z21LoadStarCameraObjectsRN11LVL_Overlay11ObjSubTableEij(struct ObjSubTable *r0, int b, unsigned int c) {
    data_02092134 = *r0->p;
}
