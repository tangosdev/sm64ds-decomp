//cpp
/* MSL's NMWException.cp, the .text of as much of it as this game links: the
 * default terminate handler and std::terminate, arm9 0x020731fc..0x0207322c.
 * The file's handler variable, the .data word 0x0209a6fc, is not built from
 * here (see below).
 *
 * MSL declares `static terminate_handler thandler = dthandler;` beside
 * dthandler and terminate, and that is the evidence for the file: the word
 * at 0x0209a6fc is read by terminate and holds dthandler's address. The
 * order is MSL's too: dthandler before terminate in the source, emitted
 * above it. MSL's duhandler, uhandler, set_terminate, set_unexpected and
 * unexpected are absent; nothing here calls them, so the linker dropped them.
 *
 * Not folded: __throw_catch_compare (0x02072fcc), the next MSL function in
 * the file, sits below __register_global_object (0x020731dc), which MSL
 * keeps in another file (global_destructor_chain.c; Spirit Tracks' decomp
 * has it in a file of its own with __global_destructor_chain's .bss). So
 * either that function is in this file in this MSL, or __throw_catch_compare
 * is in another; nothing in the ROM settles which.
 *
 * Only the .text is built from here. In MSL, dthandler and thandler are
 * static and thandler is defined in this file. A TU that owns .data has to
 * go through the build's intact-object path, and that needs a scratch-link
 * proof this file does not have yet. Until it does, the word at 0x0209a6fc
 * stays ROM bytes: thandler is declared extern, and dthandler has external
 * linkage because the ROM word's relocation reaches it from outside this
 * object. The mangled names are the same either way.
 *
 * The same path keeps `#pragma exceptions on` out. Retail .exceptix has
 * records for both functions (0x02073964 and 0x02073970), so MSL built this
 * file with exceptions on, and the .text still matches with the pragma.
 * But the compiler then also emits @EX@ records, and rombuild's isolate
 * refuses any section beyond .text in a text-only multi-symbol object.
 *
 * Follow-up: promote the file as an intact object that owns the .data word
 * and the two .exceptix records. Then turn exceptions on and restore MSL's
 * `static` on dthandler and thandler.
 */
extern "C" void abort(void);

namespace std {

typedef void (*terminate_handler)();

extern terminate_handler thandler; /* MSL: static, = dthandler */

// @symbol _ZSt9dthandlerv
void dthandler() {
    abort();
}

// @symbol _ZSt9terminatev
void terminate() {
    thandler();
}
}
