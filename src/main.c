extern void func_02059788(void);
extern void func_02059bc0(void);
extern void func_02059578(int a, int b);
extern void func_0201a054(void);
extern void func_020197b8(void);
extern void _ZN4Heap9SetNodeIDEj(unsigned int a, unsigned int b);

extern int func_0201a03c;
extern int data_020a621c;
extern int _ZN6Memory11rootHeapPtrE;

void main(void) {
    func_02059788();
    func_02059bc0();
    func_02059578((int)&func_0201a03c, (int)&data_020a621c);
    {
        unsigned int tmp = *(unsigned int *)&_ZN6Memory11rootHeapPtrE;
        _ZN4Heap9SetNodeIDEj(tmp, 2u);
    }
    func_0201a054();
    {
        unsigned int tmp = *(unsigned int *)&_ZN6Memory11rootHeapPtrE;
        _ZN4Heap9SetNodeIDEj(tmp, 3u);
    }
    func_020197b8();
}
