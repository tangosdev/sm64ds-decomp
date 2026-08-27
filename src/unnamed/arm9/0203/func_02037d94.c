/* NONMATCHING -- this file does not compile, and cannot become a match as written.
 *
 * It reconstructs the thunk's BYTES from a synthetic hierarchy: the classes below
 * are placeholders (`Base2`, `Derived`) chosen to make mwcc emit a this-adjusting
 * thunk of the right shape, not the ROM's real types. The object therefore defines
 * _ZThn16_N7DerivedD0Ev / D1Ev and _ZN7DerivedD0Ev / D1Ev / D2Ev -- five functions,
 * none of which any any symbols.txt under config/ names -- rather than the single symbol
 * `func_02037d94` this file claims.
 *
 * So it fails eligibility three ways at once: five .text sections where one is
 * allowed, a symbol name the delink entry does not declare, and undefined
 * references nothing defines. It can never be enrolled, and until it is bannered
 * it counts toward the project's matched total on the strength of its filename.
 *
 * Recovering it for real needs the ROM's actual derived class, which is the
 * vtable-naming workstream, not a byte experiment.
 */
/* func_02037d94 is a compiler-generated this-adjusting virtual-destructor
 * thunk (CodeWarrior _ZThn16_...D1Ev form): ldr ip,[pc,#4]; add r0,r0,ip; b D1
 * The thunk adjusts `this` by -16 (the size of the primary base subobject) then
 * tail-branches to the complete object destructor (dBgCh_SphCrr::~dBgCh_SphCrr @0x2037cb0).
 * It cannot be expressed as plain C; the byte-identical thunk is emitted by
 * compiling the C++ multiple-inheritance class below (secondary base at offset 16).
 * Matched symbol: _ZThn16_N7DerivedD1Ev  (== func_02037d94).
 *
 * compile as C++:  mwccarm -lang=c++ -O4,p -enum int -char signed -interworking -proc arm946e -gccext,on
 */
struct Base1 { int a; int b; int c; virtual ~Base1(); virtual void f1(); };
struct Base2 { int b; virtual ~Base2(); virtual void g1(); };
struct Derived : Base1, Base2 { virtual ~Derived(); };
Derived::~Derived() {}
