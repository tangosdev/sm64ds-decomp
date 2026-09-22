//cpp
// @symbol _ZN3IRQ12EmptyHandlerEv
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
/* IRQ::EmptyHandler() at 0x02056d60
 * Default interrupt-handler entry installed in the IRQ table for vectors that
 * have no real handler. Intentionally does nothing and returns immediately.
 * Static handler -- takes no arguments and has no `this`.
 */

namespace IRQ {

void EmptyHandler(void)
{
}

}
