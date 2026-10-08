extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern char data_ov085_02130480[];
extern char data_ov085_02130498[];
extern char data_ov085_02130488[];
extern char data_ov085_021304a4[];
extern char data_ov085_02130490[];
extern char data_ov085_021304b0[];
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);

typedef struct { int a, b; } P;
extern P data_ov085_0212fe40;
extern P data_ov085_0212fe48;
extern P data_ov085_0212fe30;
extern P data_ov085_0212fe38;

typedef struct {
    P p0;   /* 0x00 */
    P p1;   /* 0x08 */
    int gap; /* 0x10 */
    P p2;   /* 0x14 */
    P p3;   /* 0x1c */
} D;
extern D data_ov085_0212fe88;

void __sinit_ov085_0212f2a8(void) {
    func_02017acc(data_ov085_02130480, 0x35c);
    func_020731dc(data_ov085_02130480, func_02017ab4, data_ov085_02130498);
    _ZN13SharedFilePtr9ConstructEj(data_ov085_02130488, 0x35d);
    func_020731dc(data_ov085_02130488, SharedFilePtr_Destruct_Anim, data_ov085_021304a4);
    _ZN13SharedFilePtr9ConstructEj(data_ov085_02130490, 0x35e);
    func_020731dc(data_ov085_02130490, SharedFilePtr_Destruct_Anim, data_ov085_021304b0);
    data_ov085_0212fe88.p0 = data_ov085_0212fe40;
    data_ov085_0212fe88.p1 = data_ov085_0212fe48;
    data_ov085_0212fe88.p2 = data_ov085_0212fe30;
    data_ov085_0212fe88.p3 = data_ov085_0212fe38;
}
