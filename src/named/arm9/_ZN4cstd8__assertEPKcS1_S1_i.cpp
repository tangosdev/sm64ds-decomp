//cpp
// @symbol _ZN4cstd8__assertEPKcS1_S1_i
/* cstd::__assert(const char*, const char*, const char*, int) at 0x0206cd44 --
 * assertion failure path used by the game's cstd helpers.
 *
 * When `line` is non-zero, print with format data_020868a0 and trap forever;
 * when zero, print with format data_020868d4 and return. Formats and the trap
 * live at fixed ROM addresses; the helper is still called by its address name.
 *
 * Language-mode migration (Phase 1 layout-free namespace): real
 * `namespace cstd { void __assert(...) }` so mwccarm mangles the symbol.
 * Itanium substitution compresses the repeated `const char*` parameters to
 * `EPKcS1_S1_i` (same pattern as cstd::strcmp's enrolled name). The previous
 * TU hand-spelled the uncompressed form `EPKcPKcPKci`. */
extern "C" {
extern char data_020868a0;
extern char data_020868d4;
void func_0206ccd8(const char* fmt, const char* a, const char* b,
                   const char* c);
}

namespace cstd {

void __builtin_trap();

void __assert(const char* file, const char* expr, const char* func, int line)
{
    if (line) {
        func_0206ccd8(&data_020868a0, file, expr, func);
        __builtin_trap();
        for (;;) {
        }
    } else {
        func_0206ccd8(&data_020868d4, file, expr, func);
    }
}

}
