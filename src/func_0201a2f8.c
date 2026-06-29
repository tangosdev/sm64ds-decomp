/* func_0201a2f8 at 0x0201a2f8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void func_0201fec8(void);
extern void func_02018aa4(void);
extern void func_020134c8(void);
extern int GetSoundMode(void);
extern void SetSoundMode(int mode);
extern int func_0205d94c(int a, int b);
extern void* _ZN6Memory8AllocateEj(unsigned int);
extern void LoadOverlay(int id);
extern void func_020aa420(void);
extern void UnloadOverlay(int id);
extern void func_0205d23c(void* p, int n);
extern void func_0205c91c(void);
extern void _ZN6Memory10DeallocateEPv(void*);
extern void _ZN5Sound19LoadGroupAndSetBankEii(int a, int b);
extern void _ZN5Sound16LoadInitialGroupEi(int group);
extern void LoadFont(int id);

extern int overlay_0;
extern int overlay_1;
extern int data_0208ee5c;

void func_0201a2f8(void) {
    int cond = (*(unsigned short*)0x27ffc40 == 2);
    void* ptr;
    if (cond) {
        func_0201fec8();
        func_02018aa4();
        func_020134c8();
        SetSoundMode(GetSoundMode());
    }
    ptr = 0;
    if (!cond) {
        int size = func_0205d94c(0, 0);
        ptr = _ZN6Memory8AllocateEj(size);
        func_0205d94c((int)ptr, size);
    }
    LoadOverlay((int)&overlay_0);
    func_020aa420();
    UnloadOverlay((int)&overlay_0);
    if (ptr != 0) {
        func_0205d23c(&data_0208ee5c, 3);
        func_0205c91c();
        _ZN6Memory10DeallocateEPv(ptr);
    }
    LoadOverlay((int)&overlay_1);
    if (cond) {
        _ZN5Sound19LoadGroupAndSetBankEii(0x2f, 0x36);
    } else {
        _ZN5Sound16LoadInitialGroupEi(0);
    }
    if (cond) return;
    LoadFont(0);
}