# Source-review observations

Technical pitfalls and examples for interpreting matched SM64DS source. This is a
reference, not another review sequence or acceptance policy. [AGENTS](../AGENTS.md),
[PIPELINE](agents/PIPELINE.md), and the [TU promotion conventions](tu-promotion-conventions.md)
define the required proof and scope. The [humanizer role](agents/roles/humanizer.md)
owns source-review responsibilities and finding dispositions.

The observations below suggest questions about a concrete candidate. They do not
establish original source spelling, authorize unrelated repairs, or require every
old source-form experiment to be repeated.

## Source text can affect tools without affecting machine code

A draft banner is a match-status claim, not ordinary prose. Current
[asm_policy.py](../tools/asm_policy.py) recognizes `NONMATCHING` in the leading
header region, before the first declaration or definition. Its helper treats
leading comments, blank lines and preprocessor directives as that region. Older
tools used 200- or 400-byte windows; their historical disagreement is why the
shared helper exists. Adding a long leading comment no longer has the old fixed-
window failure, but deleting the banner or placing it below code can still change
what the tools recognize. Removing a draft banner needs the associated match
proof, not just a tidy diff.

`HAND-ASM PRIMITIVE` has a different meaning: it asserts that assembly is the
appropriate source form. A banner does not establish that provenance by itself.
The policy distinguishes unbannered `dcd` transcriptions from mnemonic assembly
that needs review; see [the assembly policy](asm-policy.md). A transcription of ROM
words can reproduce bytes without reconstructing the source. Conversely, legitimate
coprocessor, BIOS or context-switch assembly is not evidence of a failed C++ recovery.

The first-line `//cpp` marker also has tool meaning. A comment inserted before it
can change language-mode selection in tools that inspect the beginning of the
file, even though a C++ compiler would ignore both comments.

Some source audits use textual patterns rather than a compiler parser. In
[langmode_audit.py](../tools/langmode_audit.py), the laundering-site metrics now use
`delaunder.find_sites`, which masks comments and strings; the old warning that
merely mentioning laundering in prose increases that metric is obsolete. Other
counters still use text patterns, so their actual implementation matters.
[check_header_offsets.py](../tools/check_header_offsets.py) also has a limited
source parser. Its field coverage, skipped declarations and `UNPARSED` diagnostics
matter alongside its verdict. Historical zero-field false passes are a reason to
inspect coverage, not a universal claim that formatting changes break the checker.

## A matching body does not settle its declarations

Separately compiled declarations can disagree while the linker resolves the same
symbol. Parameter width, signedness, return type, pointer/reference forms and
implicit conversions can affect callers. Equal register width is not enough to
call a declaration repair byte-neutral. Shared-header changes can affect consumers
outside the visible function diff; [affected_src.py](../tools/affected_src.py) and
[declaration agreement](../tools/check_decl_agreement.py) address different parts
of this problem. The latter ratchets existing disagreements rather than proving
that every banked declaration is correct.

Demangling explains the contract asserted by a symbol spelling. Many spellings in
this repository were reconstructed, so that output alone does not recover the
original parameter types or original method name. Ordinary non-template function
manglings generally do not encode the return type. Actual definitions, callers,
RTTI/vtable evidence where available, and emitted code supply further constraints.
A callee leaving a value in a return register is not, alone, its source-level
return contract.

A useful concrete linkage example is [SharedFilePtr.h](../include/SharedFilePtr.h):
a bare C++ declaration such as `extern void _ZN13SharedFilePtr7ReleaseEv(void*);`
can mangle that already encoded identifier again. Correct bytes with an unresolved
or wrongly bound relocation are not a successful call. An existing genuine class
interface avoids inventing another ABI bridge; actual C interfaces still need
C linkage. Neither blanket removal nor blanket addition of `extern "C"` solves
source reconstruction.

Changing a free function into a non-static member can change its symbol spelling,
implicit receiver and calling convention. Likewise, converting a static callback
to a non-static member is not cosmetic. The corresponding symbol/configuration,
callers and ownership must agree with the chosen form. A `//cpp` marker around a
hand-spelled member name does not itself make the definition a C++ method.

## Clues to layout and source form

Repeated offsets from one receiver, equal-stride accesses, and parallel operations
on nearby fields can support a struct or array hypothesis. They do not determine
its full extent, element count or ownership. A constructor touching an address
establishes a live access, not necessarily the end of a component. Local shadow
views may disagree; promoting one to a shared header can change indexing and
code generation throughout the include graph. Offset comments and padding names
are claims to compare with all observed accesses, including raw accesses retained
outside the changed lines.

Fixed-point shifts such as `>> 12`, repeated narrowing casts at call sites, and
patterns such as `(u32)(x << 16) >> 16` are useful leads for investigating types.
They can also express scaling, truncation or deliberate instruction selection.
A matching typed alternative supports that formulation; it does not prove a unique
original declaration. A nonmatching alternative does not establish that every
cleaner form is impossible.

Declaration order, aggregate copies, temporary ordering and optimization pragmas
can affect instruction selection or register allocation. The measured examples
in [mwccarm-codegen.md](mwccarm-codegen.md) include load/store batching and `z, y, x`
temporary order. Their scope is the tested bodies and compiler inputs. In a TU,
a pragma that helps one function may change other members. Whole emitted output
also matters when a source-form change creates destructor variants, helpers,
vtables or RTTI that a per-function extraction would discard.

Comments that record a failed formulation, a working store order or a compiler
experiment preserve evidence that is not obvious from the final code. Correcting
stale or inaccurate comments is legitimate; preserving the experiment's identity
and limits is more useful than either deleting it as clutter or treating it as a
permanent compiler law. Readable local names can help explain intent, but renaming,
adding `this->`, or changing a cast is not an unconditional proof of unchanged
lookup, macro expansion, source metrics or code generation.

A call with an unused return value can still have side effects. Apparent redundant
loads, stores, casts or empty-looking paths need their surrounding control flow
and callers to explain them. A `(void)` cast is not a universal repair, and an
unchanged byte sequence does not by itself resolve a source-level undefined-value
hazard.

## Byte evidence has a scope

A masked comparison can hide a wrong callee or global at a relocation site.
[match.py](../tools/match.py) enables destination checks by default, but it can
print a byte-only fallback if relocation configuration cannot be initialized.
Other failure paths explicitly reject an unavailable destination check. The actual
invocation and report decide what ran; the tool's name or an isolated `MATCH` does
not. Similarly, `build_pin.verify` can receive strict relocation data from its
caller, so it should not be described as universally strict or universally blind.

Compiler pin, language mode, flags, module and range are part of the comparison.
A build under a different compiler is a diagnostic, not evidence that the assigned
pin fails. Missing inputs and an observed mismatch are different outcomes. A
per-function success also does not prove default-build enrollment or preservation
of the unisolated object's metadata; those distinctions belong to the canonical
promotion proof.

## Hardware accesses and the host port

Hardware-address searches need to recognize both `0x4000208` and `0x04000208`.
The `0x04` I/O window is only one relevant region: palette RAM, VRAM and OAM use
`0x05`, `0x06` and `0x07` prefixes. These are search leads, not automatic findings;
a similar constant may be a mask, fixed-point value or unrelated quantity. Names
should follow existing repository definitions and documented address evidence,
with descriptive names distinguished from recovered identifiers. A plausible name
is not evidence of its original spelling or of improper provenance.

The [port's own rules](../port/README.md) retain byte-verified `src/` as the source
of truth and currently target a 32-bit host first. Host convenience does not justify
breaking the cartridge build. Conversely, a justified source correction with fresh
proof is not forbidden simply because the port also benefits. The actual port
policy describes the platform guards and seams used for host-specific behavior.
Little-endian agreement or four-byte pointers do not settle aliasing, alignment,
object lifetime or signed-overflow questions.

[hostgen.py](../port/tools/hostgen.py) transforms selected literal MMIO dereferences
into `NTR_MMIO` accesses in generated host source. For example:

```cpp
*(volatile unsigned short *)0x4000280
```

is a recognized shape. A computed address such as
`*(volatile u32 *)(0x4000280 + offset)`, or a pointer saved and dereferenced later,
can escape the textual transform. The mapped latch window may service the memory
access while a write-triggered register side effect never runs. The changed form,
its selected host caller and required device behavior determine the impact;
it is neither an automatic cartridge defect nor universally harmless port debt.

The port also has literal source paths and symbol references in slice manifests,
CMake host-generation lists and HAL linkage bridges. A file move or symbol rename
can strand these even when the cartridge build passes.
[port_refcheck.py](../tools/port_refcheck.py) checks reference integrity; it is not
proof that every host executable compiles or links. The optional
[host_frontier.py](../port/tools/host_frontier.py) diagnostic uses the MSVC x86
syntax-check environment. Missing tools and intentionally substituted assembly
sources need to be distinguished from new host regressions; no PR-size threshold
makes that diagnostic an additional acceptance gate.

## Interpreting configuration and attribution changes

A same-address symbol change can be a rename, while a disappeared address or altered
extent can mean something else. Overlay identity matters: pair addresses with their
module. Tracked delinks, symbol and relocation configuration are production inputs,
not interchangeable with generated `build/` output. A configuration diff therefore
needs its source/ownership explanation rather than a blanket ban on configuration
changes.

Git similarity and file counts do not establish symbol credit after many-to-one
TU folding. The promotion conventions describe `path#symbol` attribution and
source retirement. A coherent promotion may include source moves and necessary
configuration changes together; it does not need an unrelated preliminary reorg
PR. Readability work in an already promoted TU belongs in its production owner.
