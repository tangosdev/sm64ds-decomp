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
                       src/game/player/Player/_ZN6Player13InitResourcesEv.cpp) lives in this gray
                       zone, so demoting it wholesale would rescind legitimate
                       matches. These are surfaced for a human to banner or
                       reclassify deliberately.
  None              -- bannered, or no asm at all.

The banner search runs over the WHOLE file, never a head window: a banner is only
ever exculpatory here, so a deep mention can excuse a file but can never flip a
real C match into a fake one.

The asm detection is lifted from the training-side `eval_match.py` passthrough
detector (that tool lives in a separate dataset repo, NOT in this tree) rather than
invented fresh, so the two gates agree on what "asm" looks like.
"""
import re

HAND_BANNER = "HAND-ASM PRIMITIVE"
DRAFT_BANNER = "NONMATCHING"


def header_region(text):
    """The file's leading banner block: everything before the first line of real code.

    Comments, blank lines and preprocessor directives are all still "header"; the region
    ends at the first declaration or definition. Banners live there by convention, after
    the includes and the `recovered:` provenance lines.
    """
    out = []
    for line in text.splitlines(True):
        s = line.strip()
        if s and not s.startswith(("//", "/*", "*", "*/", "#")):
            break
        out.append(line)
    return "".join(out)


def has_draft_banner(text):
    """Does this file declare itself a non-match? THE one rule, because there were three.

    Callers scanned `text[:200]` (chaos_db_ci, cluster_targets, coddog, enroll,
    pr_linkcheck), `text[:400]` (nearmiss_db, prepush_linkcheck), or the whole file
    (classify, below). So one file could be a draft to one gate and matched to another,
    which is exactly what happened: src/unnamed/ov091/0213/func_ov091_021339fc.c says "does NOT count as
    matched" at byte 246 -- past the 200-byte window -- and the progress bar counted it
    as matched regardless of what its author wrote.

    A fixed byte count was always going to rot, because the banner did not move; the
    recovery comments above it grew. The header region is stable against that. Measured
    when this landed: 200 bytes found 72 files, 400 found 73, header region and whole
    file both found 74 -- so this is not looser than a whole-file scan, it just also
    cannot be fooled by the word appearing deep inside a function body.
    """
    return DRAFT_BANNER in header_region(text)


# A dcd word is raw ROM data re-spelled; one is enough to make the file suspect.
_DCD_RE = re.compile(r"\bdcd\s+0x[0-9a-fA-F]")

# High-precision ARM assembly "tells" that essentially never appear in real decompiled C.
# A passthrough carries dozens; real C carries ~0. Used as a keyword-independent backstop so
# an asm transcription is caught even if it isn't wrapped in a form the `asm` keyword regex
# sees. (Lifted from the same training-side `eval_match.py`, which is not a file here.)
#
# The register-list tell must accept real STM/PUSH lists -- ``{r4,r5,lr}``, ``{ r0 - r3 }``,
# ``{r4-r11,lr}`` -- while rejecting C blocks whose first statement assigns to a local that
# happens to be named like a register: ``{ r5 = 0x10000;``, ``{ r0->a = 0; }``, ``{ r2c = 0;``.
# So after the register name we require a word boundary (kills ``r2c``) and then a list
# continuation: ``,``, the closing ``}``, or a ``-`` range whose right side is another
# register (kills ``r0->``).
_ARM_TELLS = re.compile(r"""
      \bstm(?:db|ia|fd|ea)\b | \bldm(?:db|ia|fd|ea|ib)\b     # block load/store multiple
    | \bbx\s+lr\b | \bblx\b | \bbl\s+\w                        # branch-exchange / branch-link
    | \bsp!                    | \[pc,        | \[sp,          # ARM addressing idioms
    | \bdcd\b | \bdcw\b                                        # inline data words
    | \{\s*(?:r\d+|lr|sp|pc)\b\s*(?:[,}]|-\s*(?:r\d+|ip|lr|sp|pc)\b)  # register list {r4, lr} / { r0 - r3 }
    | \b(?:ldr|str|ldrh|strh|ldrb|strb|mov|cmp|orr|eor|lsl|lsr|asr|msr|mrs)\s+r\d  # op + reg
    | \bldmia\b | \bstmia\b
""", re.X)

# Comments are stripped before asm DETECTION (never before the banner search: banners live
# in comments and are only ever exculpatory). Disassembly traces and call-map notes kept as
# documentation -- ``// mov r0,r2; bl 0x0205a61c`` -- are commentary about asm, not an asm
# body, and were firing the tells above. String literals are preserved so a ``//`` inside
# one cannot eat real code (and GCC-style ``__asm__("mrc ...")`` keeps its content).
_COMMENT_OR_STRING_RE = re.compile(
    r'"(?:\\.|[^"\\\n])*"'        # string literal (kept)
    r"|'(?:\\.|[^'\\\n])*'"       # char literal (kept)
    r"|/\*.*?\*/"                 # block comment (stripped)
    r"|//[^\n]*",                 # line comment (stripped)
    re.S)


def _strip_comments(code):
    return _COMMENT_OR_STRING_RE.sub(
        lambda m: m.group(0) if m.group(0)[0] in "\"'" else " ", code)


def is_asm_passthrough(code):
    """True if ``code`` carries an asm body rather than (only) real C.

    Two independent detectors so a novel form can't slip through:
      1. the `asm` KEYWORD -- mwccarm naked function ``asm <type> name(...) {``,
         GCC ``asm {`` / ``asm volatile``, or ``__asm``;
      2. CONTENT -- >=3 ARM-mnemonic tells, which real C never has, catching asm
         even if it isn't wrapped in a keyword form the regex above recognizes.

    Both run on comment-stripped text: a commented-out disassembly trace is
    documentation, not an asm body (the exculpatory banners are searched on the
    RAW text by ``classify``, never here)."""
    code = _strip_comments(code)
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
