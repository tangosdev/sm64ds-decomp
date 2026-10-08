/* func_02018770 at 0x02018770
 * Unloads the archive a file read mounted for itself, if there is one
 * (data_0208eb54 holds its index), then sets the index to -1 (0xff, none).
 */

typedef signed char s8;

extern s8 data_0208eb54; /* archive a read mounted, -1 when none */
extern void UnloadArchive(int i);

void func_02018770(void) {
    if (data_0208eb54 < 0) {
        return;
    }
    UnloadArchive(data_0208eb54);
    data_0208eb54 = -1;
}
