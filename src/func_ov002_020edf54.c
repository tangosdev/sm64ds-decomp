/* func_ov002_020edf54 at 0x020edf54
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void _ZN13SharedFilePtr7ReleaseEv(void *self);
extern int func_ov002_020ec628(char *r0);
extern void UnloadBlueCoinModel(char *r0);
extern char data_ov002_0210e6b0;
extern char data_ov002_0210eb78;

int func_ov002_020edf54(char *self) {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210e6b0);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210eb78);
    if (func_ov002_020ec628(self)) {
        UnloadBlueCoinModel(self);
    }
    return 1;
}
