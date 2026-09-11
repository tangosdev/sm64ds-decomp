// @symbol __end__catch
// MSL C++ runtime catch-exit hook. mwccarm emits `bl __end__catch` itself at the end of
// every catch block (the source never names it), so the link needs a DEFINITION under
// that spelling. Proven by compiling a bare try/catch under the ROM CFLAGS and reading
// the object's undefined symbol table: __end__catch, with no mention in the source.
// Was func_02071ba0; the size-0 second name in config/arm9/symbols.txt could not define
// it, because a size-0 alias only reaches the link inside a dsd gap object and this
// range is carved out by a complete delinks entry. See notes/mwccarm-codegen.md 9a.
void __end__catch(int obj)
{
    int p;
    void (*fn)(int, int);
    p = *(int*)obj;
    if (p == 0) return;
    fn = *(void(**)(int, int))(obj + 8);
    if (fn == 0) return;
    fn(p, -1);
}
