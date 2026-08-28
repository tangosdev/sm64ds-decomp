//cpp
extern "C" void func_02011dcc(unsigned int* g, unsigned int j);
extern int data_0208e430;
extern int data_0209b49c;
extern signed char data_0208e42c;
extern int data_0209b490;
extern unsigned int data_0209b4b0[];
extern signed char data_0209b470;
extern int data_0209b494;

/* The return type is `int`, not C++'s `bool`, and the width is load-bearing.
 * include/types.h:53 makes `bool` an ordinary `typedef int` in every TU this
 * tree compiles as C, and all 36 declarations of this function outside this
 * file spell the return `int` or `void`. Declared `bool` here it is a C++
 * built-in instead, one byte wide: mwcc for ARM zero-extends r0 and never
 * noticed, but MSVC returns it in AL and leaves the top 24 bits of EAX as
 * whatever the last path left there, so an `int` caller testing the result
 * reads junk. Measured on the port's own build: the `ld4` early-out reaches
 * `xor al,al` with EAX still holding the `d` argument, so PlaySub(0x28, 0x12,
 * 0x7f, 0x15ccc, 0) returned 0x15c00 -- true -- while actually reporting
 * false. Byte-identical either way (2004/b56 verified). */
extern "C" int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e){
  int x;
  if(c==0){
    if(e) goto end;
    x = data_0208e430;
    if(x >= 0 && x != (int)a) goto ret1;
    if(data_0209b49c != 0) goto end;
    if(data_0208e42c != (int)b) goto end;
    if(data_0209b490 != (int)(b<<12)) goto end;
ret1:
    return 1;
  }
  x = data_0208e430;
  if(x >= 0){
    if(!e) goto ld4;
  }
  if(x == (int)a) goto end;
  data_0208e430 = a;
  if(a != 0x35 && a != 0x36){
    if(a != 0x2e) data_0209b49c = c & 0xff;
  }
  func_02011dcc(data_0209b4b0, a);
  goto end;
ld4:
  if(x != (int)a) return 0;
end:
  data_0208e42c = (signed char)b;
  data_0209b470 = (signed char)c;
  data_0209b494 = d;
  if(data_0209b490 != (int)(b<<12)) goto ret0;
  if(data_0209b49c == (int)(c<<12)) return 1;
ret0:
  return 0;
}
