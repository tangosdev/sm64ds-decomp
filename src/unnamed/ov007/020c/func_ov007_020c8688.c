typedef struct {
    int _0;
    int _4;
    int _8;
    int _c;
    int _10;
    int _14;
    int _18;
    int _1c;
} S;

extern void func_ov007_020c86c4(S*, int);

void func_ov007_020c8688(S* p, int a) {
    p->_1c = 1;
    p->_4 = a;
    p->_14 = 0;
    func_ov007_020c86c4(p, 0);
}
