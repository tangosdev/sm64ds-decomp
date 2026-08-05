"""One shared answer to "is this source file an asm transcription?".

Tree policy (notes/asm-policy.md): an asm body in src/ is legitimate only under an
explicit banner. ``HAND-ASM PRIMITIVE`` says the ORIGINAL was assembly -- a BIOS
call, coprocessor op, context switch, or SDK primitive that C cannot express -- so
the asm block is the faithful source and counts as matched. ``NONMATCHING`` says
the file is a declared draft and does not count. Everything else claiming to be a
match must be real C.

The failure mode this module exists to catch is the VACUOUS match. mwccarm's
``dcd 0x...`` directive emits the literal word you type, so a whole-function dcd
dump assembles to the ROM bytes by definition: the "match" proves only that the
author copied the disassembly, and counting it puts a lie in the progress bar.
PR #1072 landed 8 such files as +8 matched functions / +8,208 bytes / contributor
credit. The byte gate cannot catch these -- byte equality is exactly what they
game -- so this textual gate is the honesty mechanism.

``classify`` deliberately draws two tiers:

  "transcribed"     -- the file contains ``dcd 0x`` words and NO banner. Demoted
                       from matched everywhere, and a PR adding or touching one
                       hard-fails validation. There is no legitimate reason for an
                       unbannered dcd blob: a hand-asm primitive gets the HAND-ASM
                       banner, a draft gets NONMATCHING.
  "unbannered-asm"  -- the file has an asm body (mnemonics, no dcd) and no banner.
                       WARN only: real C with an embedded asm hatch (CP15
                       intrinsics inside a C function, e.g.
                       src/_ZN6Player13InitResourcesEv.cpp) lives in this gray
                       zone, so demoting it wholesale would rescind legitimate
                       matches. These are surfaced for a human to banner or
                       reclassify deliberately.
  None              -- bannered, or no asm at all.

The banner search runs over the WHOLE file, never a head window: a banner is only
ever exculpatory here, so a deep mention can excuse a file but can never flip a
real C match into a fake one.

The asm detection is lifted from tools/dataset/eval_match.py's passthrough
detector (the training-side guard against the same reward hack) rather than
invented fresh, so the two gates agree on what "asm" looks like.
"""
import re

HAND_BANNER = "HAND-ASM PRIMITIVE"
DRAFT_BANNER = "NONMATCHING"

# A dcd word is raw ROM data re-spelled; one is enough to make the file suspect.
_DCD_RE = re.compile(r"\bdcd\s+0x[0-9a-fA-F]")

# High-precision ARM assembly "tells" that essentially never appear in real decompiled C.
# A passthrough carries dozens; real C carries ~0. Used as a keyword-independent backstop so
# an asm transcription is caught even if it isn't wrapped in a form the `asm` keyword regex
# sees. (Lifted from tools/dataset/eval_match.py.)
_ARM_TELLS = re.compile(r"""
      \bstm(?:db|ia|fd|ea)\b | \bldm(?:db|ia|fd|ea|ib)\b     # block load/store multiple
    | \bbx\s+lr\b | \bblx\b | \bbl\s+\w                        # branch-exchange / branch-link
    | \bsp!                    | \[pc,        | \[sp,          # ARM addressing idioms
    | \bdcd\b | \bdcw\b                                        # inline data words
    | \{\s*(?:r\d|lr|sp|pc)                                    # register list  {r4, lr}
    | \b(?:ldr|str|ldrh|strh|ldrb|strb|mov|cmp|orr|eor|lsl|lsr|asr|msr|mrs)\s+r\d  # op + reg
    | \bldmia\b | \bstmia\b
""", re.X)


def is_asm_passthrough(code):
    """True if ``code`` carries an asm body rather than (only) real C.

    Two independent detectors so a novel form can't slip through:
      1. the `asm` KEYWORD -- mwccarm naked function ``asm <type> name(...) {``,
         GCC ``asm {`` / ``asm volatile``, or ``__asm``;
      2. CONTENT -- >=3 ARM-mnemonic tells, which real C never has, catching asm
         even if it isn't wrapped in a keyword form the regex above recognizes."""
    if "__asm" in code:
        return True
    for m in re.finditer(r"(?<![\w])asm(?![\w])", code):          # `asm` as a standalone token
        tail = code[m.end():m.end() + 120]
        # followed by `volatile`, an opening `{`, or a `(...)  {` function signature
        if re.match(r"\s*(volatile\b|\{|[\w:*&<>\[\], \n]*\([^;{]*\)\s*\{)", tail):
            return True
    return len(_ARM_TELLS.findall(code)) >= 3                     # content backstop


def classify(text):
    """"transcribed" (unbannered dcd blob: demote + fail), "unbannered-asm"
    (asm body, no banner, no dcd: warn only), or None (bannered or no asm)."""
    if HAND_BANNER in text or DRAFT_BANNER in text:
        return None
    if _DCD_RE.search(text):
        return "transcribed"
    if is_asm_passthrough(text):
        return "unbannered-asm"
    return None
