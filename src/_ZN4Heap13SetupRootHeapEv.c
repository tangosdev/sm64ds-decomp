typedef unsigned int u32;
struct Heap;
struct HeapS;
extern void *_ZN6Memory15rootParamOffsetE;
extern struct HeapS *_ZN6Memory11rootHeapPtrE;
extern struct HeapS *_ZN6Memory14defaultHeapPtrE;
extern int func_02058ea0(void *g);
extern int func_02058eb4(void *g);
extern int func_02059040(void *g, int a, int b, int c);
extern int func_02058d58(void *g, int a);
extern void *func_02058cd0(void *g, int a, int b);
extern struct HeapS *_ZN4Heap14CreateRootHeapEPvj(void *mem, u32 size);

struct HeapS *_ZN4Heap13SetupRootHeapEv(void) {
  int v0 = func_02058ea0(_ZN6Memory15rootParamOffsetE);
  int v1 = func_02058eb4(_ZN6Memory15rootParamOffsetE);
  v0 = func_02059040(_ZN6Memory15rootParamOffsetE, v0, v1, 4);
  func_02058d58(_ZN6Memory15rootParamOffsetE, v0);
  int top = func_02058eb4(_ZN6Memory15rootParamOffsetE);
  int sz = top - (((v0 + 0x1f) & ~0x1f));
  void *mem = func_02058cd0(_ZN6Memory15rootParamOffsetE, sz, 0x20);
  struct HeapS *heap;
  if (mem == 0) goto fail;
  heap = _ZN4Heap14CreateRootHeapEPvj(mem, sz);
  if (heap != 0) {
    _ZN6Memory11rootHeapPtrE = heap;
    _ZN6Memory14defaultHeapPtrE = heap;
  }
  return heap;
fail:
  return (struct HeapS *)0;
}
