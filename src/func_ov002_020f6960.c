/* func_ov002_020f6960 at 0x020f6960
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void* _ZN5Model8LoadFileER13SharedFilePtr(void *self);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b);

int func_ov002_020f6960(char *self, void *fp, int c) {
    void *bmd;
    *(void**)(self + 0x5c) = fp;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(void**)(self + 0x5c));
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(self, bmd, 1, c)) {
        return 1;
    }
    return 0;
}
