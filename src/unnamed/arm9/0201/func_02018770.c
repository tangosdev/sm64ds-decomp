/* func_02018770 at 0x02018770
 * Conditional UnloadArchive: unloads archive only if global state >= 0,
 * then sets the state byte to -1 (0xff).
 */

typedef signed char s8;

extern s8 data_0208eb54; /* global state byte */
extern void UnloadArchive(void);

void func_02018770(void) {
    if (data_0208eb54 < 0) {
        return;
    }
    UnloadArchive();
    data_0208eb54 = -1;
}
