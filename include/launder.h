#ifndef LAUNDER_H
#define LAUNDER_H

/* Compiler-steering macros. These are NOT how the original source was written.
 *
 * mwccarm folds a memory access onto a 12-bit immediate offset when it can. The ROM,
 * however, materializes a separate address register for every read-modify-write site
 * (either via a pool constant or an explicit `add rD, base, #imm`) while folding every
 * single-use load/store. Masking the address through a no-op 64-bit AND blocks that
 * fold and reproduces the ROM's shape.
 *
 * Rules:
 *   - Apply ONLY to read-modify-write sites. Laundering a single-use access diverges.
 *   - Use a DIFFERENT macro for each distinct address in one function. A single shared
 *     macro lets mwccarm CSE the laundered addresses together, which inflates the frame.
 *     The three spellings differ only in the inner cast; all are identical on a 32-bit
 *     target, but they land in different value-numbering classes.
 *
 * See notes/matching-style.md, "RMW base materialization".
 */
#define LAUNDER_A(a) (*(int *)(((long long)(int)(a))      & 0xFFFFFFFFFFFFFFFFLL))
#define LAUNDER_B(a) (*(int *)(((long long)(unsigned)(a)) & 0xFFFFFFFFFFFFFFFFLL))
#define LAUNDER_C(a) (*(int *)(((long long)(long)(a))     & 0xFFFFFFFFFFFFFFFFLL))

#endif /* LAUNDER_H */
