"""How much of the port is the real decomp, and what is still scaffolding?

The goal for the port is that it IS the decompiled game: every host file under
port/hal and port/unmatched replaced by the byte-matched translation unit it
stands in for, step by step, until there is nothing left standing in.

This measures that two ways.

  headline   how many matched TUs from src/ actually reach the binary
  queue      for every symbol a HOST file defines, whether src/ already has a
             matched TU defining the same symbol -- those are the direct
             replacement candidates, the work list for getting to 100%

The queue is the useful half. A host file whose symbols have no matched
counterpart is genuine host code (the ntr layer, the window, the SDAT host,
MSVC ABI shims for register ride-throughs and pointer-to-member dispatch) and
is not a defect. A host file whose symbols DO have matched counterparts is
scaffolding that outlived its reason, and port_stage_a_boot is the example
that motivated this file: a hand-written subset of Stage::InitResources, while
Stage::InitResources sat in src/ decompiled and unlinked.

NOT every queue symbol is scaffolding, though. This tool sorts every queue hit
into three buckets, and only the first of them is work:

  SHADOWS     a host file defines the symbol and the matched TU of that name
              is NOT in the binary. The host body is what runs. This is the
              replacement work list -- the number the port drives to zero.

  FACES       a host file defines the symbol AND the matched TU of that name
              IS in the binary. Both can coexist because they are different
              symbols to the linker: the matched .cpp compiles a real C++
              method (MSVC mangles it `?Allocate@Heap@@...`) while the host
              file defines the ROM's Itanium C name (`_ZN4Heap8AllocateEji`)
              that the ROM's own vtables and call sites use. hal/heap_vtable
              and hal/cxxname_bridge are full of these, and they are the
              OPPOSITE of scaffolding -- they are the bridge that lets a
              C-named ROM caller reach the decompiled method.
              READ THE CAVEAT: a map cannot prove forwarding. All this bucket
              knows is that both definitions are linked. That is consistent
              with a face that forwards into the matched body, and equally
              consistent with a host body that duplicates it while the matched
              TU is kept alive by some other caller -- which WOULD be
              scaffolding. Deciding takes reading the host definition. The
              bucket is listed separately so it is neither counted as work nor
              silently forgiven.

  EXCEPTIONS  a matched src TU that is an ARM asm primitive, an ARM register
              ride-through, an mwcc pointer-to-member dispatch, or a poke of DS
              hardware the ntr layer does not model cannot compile-and-behave
              under MSVC no matter how much wiring is added -- the host
              definition IS the faithful stand-in. A host definition carrying a
              `PORT_HOST_ABI:` tag in the comment right above it is one of
              these. The tag is a human ruling, so it wins over the other two
              buckets: a tagged symbol is an exception whether or not the
              matched TU also links.

THE DECORATION RULE, and the bug it replaced. This is a 32-bit MSVC link, so
an `extern "C"` function gets exactly ONE leading underscore in the /MAP:
`func_02049d60` appears as `_func_02049d60`. The ROM's C++ symbols are carried
through the port as C names that ALREADY start with an underscore, so
`_ZN4Heap8AllocateEji` appears as `__ZN4Heap8AllocateEji`. This file used to
undecorate with `sym.lstrip("_")`, which strips EVERY leading underscore and
turned that into `ZN4Heap8AllocateEji` -- a name no matched TU is called, so
every single `_ZN*`-named host definition fell out of the queue unseen. The
queue read 132 when the honest number was ~447. Strip exactly one.

THE SECOND WAY A SHADOW HIDES: THE MSVC-MANGLED NAME. Everything above joins
host definitions to matched TUs by NAME, and the name it joins on is the ROM's
Itanium spelling, because that is what a matched TU's stem is called
(`_ZN6Player8BehaviorEv.cpp`). A host copy does not have to spell it that way.
`port/unmatched/Player_Behavior.cpp` defines `Player::Behavior` as a real C++
method, so the only symbol it puts in the map is MSVC's own mangling,
`?Behavior@Player@@UAEHXZ` -- a string with no `_ZN` in it anywhere. The join
above never fires, the host copy never enters the queue, and
src/_ZN6Player8BehaviorEv.cpp sits unlinked with nothing saying why. Ten TUs
were hiding in exactly that shape when this was written.

So there is a second join, on the pair the two manglings agree about: the class
and the method. `?Behavior@Player@@UAEHXZ` and `_ZN6Player8BehaviorEv` both say
`Player::Behavior`, and if a host object provides the MSVC spelling while the
matched TU of that qualified name is not in the binary, the host body is what
runs -- the same fact a SHADOW states, reached the other way round. Those rows
are labelled MSVC-NAME so the bucket says which join found it, and so the
number the queue grew by explains itself instead of just being larger.

The PORT_HOST_ABI rule carries over: a host copy that puts the MSVC face on top
of a tagged C-name definition in the same file (PathLift_StateDispatch.cpp is
the shape) is an MSVC-NAME EXCEPTION, not work. The tag is looked up under the
matched STEM, because the tagged definition is the C name and no tag will ever
bind to a `?...@...@@` string.

Two limits are printed rather than papered over. The join is on the qualified
name only, NOT the argument list, so when src/ carries several overloads of one
method (Heap::Allocate, OAM::Render) the pair cannot say which one the host
stands in for; those are listed as AMBIGUOUS and counted as neither. And a
matched TU already surfaced through its Itanium name somewhere else in the
queue is reported as already-surfaced rather than counted twice.

    python port/tools/linkage.py [repo-root] [--queue] [--exceptions] [--faces]
  python port/tools/linkage.py --selftest        # the tag binder's fixtures
                                             [--by-module] [--msvc-names]

  --queue        list every SHADOW symbol with its matched source
  --exceptions   also print the documented host-ABI exceptions and their reasons
  --faces        also print the face bucket, host object by host object
  --by-module    split the headline per ROM module, worst first
  --msvc-names   list the MSVC-name rows in full, including the already-
                 surfaced and AMBIGUOUS ones the summary only counts

WHY --by-module. "6355 unlinked" is a number nobody can act on. Split by the
module a TU belongs to it becomes a worklist: on the 4888-linked baseline ov006
alone is 1839 of them with nothing linked at all, which says the next lane is a
whole-overlay mount and not a scatter of individual seats. The module comes
from the stem: `func_ovNNN_*` names its overlay outright, `func_02xxxxxx` is
arm9, and a named TU is looked up in config/arm9/symbols.txt and
config/arm9/overlays/*/symbols.txt. Anything still unplaced is counted in an
UNKNOWN row with a sample rather than being quietly dropped, and the columns
sum to the headline on purpose -- a breakdown that does not reconcile with the
number above it is how a lane ends up quoting two different totals for the same
build in the same status file.

With no repo-root argument this measures the checkout the script itself lives
in, not the current directory. It used to root at CWD, which reads the same
from the repo root and silently measures nothing (or another checkout's map)
from anywhere else. The resolved root and map are printed above every run, so
a pasted reading always says which tree and which binary it came from.
"""
import os
import re
import sys

HOST_DIRS = ("hal", "unmatched")

# Directories a host object's source may live in, most specific first. fs.cpp
# exists under both hal/ and ntr/; the queue's plain `fs.cpp.obj` is the hal one
# and the ntr copy is namespaced (`ntr_2x:runtime.cpp.obj`), so hal wins here.
HOST_SRC_DIRS = ("port/hal", "port/hal/sdat", "port/unmatched", "port/ntr",
                 "port/tests")

# A host definition is a documented host-ABI exception when the comment block
# right above it carries this tag. The text after the colon is the reason;
# trailing comment punctuation (*/) is stripped off.
ABI_TAG = re.compile(r"PORT_HOST_ABI:\s*(.+?)\s*(?:\*/\s*)?$")


def host_source_for(root, obj):
    """Resolve a host .obj name to its source file, or None."""
    # obj is like `cxx_aliases.cpp.obj` or `ntr_2x:runtime.cpp.obj`.
    base = obj.split(":", 1)[-1]
    stem = os.path.splitext(os.path.splitext(base)[0])[0]
    for d in HOST_SRC_DIRS:
        for ext in (".cpp", ".c"):
            p = os.path.join(root, d, stem + ext)
            if os.path.exists(p):
                return p
    return None


IDENT = re.compile(r"[A-Za-z_]\w*")
# A whole line that is one macro invocation and nothing else --
# `VS_SEAM(UnloadOverlay)`, `PORT_SEAM(func_0203d9f4)`. It DEFINES the symbol
# named inside it (the macro expands to a definition), so a tag's binding run
# ends there, the way it ends on a `{`. Without this the run walks straight
# through into the NEXT tagged row and binds that row's symbol to this row's
# reason: three rows in hal/scene_vs_menu.cpp's VS_SEAM block, two of them
# wrong, with UnloadOverlay reported as a "refused DS wireless-status wrapper".
# A definition head does not match: `void foo(int a)` has a type token before
# the identifier, and this pattern wants the identifier first. A prototype
# (`FOO(x);`) does not match either, and it should not -- a `;` line keeps the
# run open by design, because several tags sit above a prototype.
MACRO_CALL = re.compile(r"^[A-Za-z_]\w*\s*\(.*\)$")
# Type/keyword tokens that share a definition line with the real symbol name.
_SKIP_IDENT = {"extern", "static", "void", "int", "unsigned", "char", "short",
               "long", "const", "signed", "struct", "return", "if", "for",
               "while", "C"}


def _is_comment_or_blank(stripped, in_block):
    """(is_comment_or_blank, new_in_block) for a stripped source line."""
    if stripped == "":
        return True, in_block
    if in_block:
        # inside /* ... */; the block ends on the line carrying */
        return True, (not stripped.endswith("*/"))
    if stripped.startswith("//"):
        return True, False
    if stripped.startswith("/*"):
        # a /* ... */ opener; stays open unless it also closes on this line
        return True, (not stripped.endswith("*/"))
    if stripped.startswith("*"):
        return True, in_block
    return False, in_block


def _reasons_in(source_path):
    """{symbol: reason} for every PORT_HOST_ABI tag in a host source file.

    A tag documents the DEFINITION right below it. The reason binds to every
    identifier on the first real code line after the tag -- skipping the rest of
    the comment block, tracking /* ... */ state so a wrapped prose line does not
    look like code. The definition may be `extern "C" void foo(...)`, a pointer
    return `void *foo(...)`, or a plain method; the caller only ever asks about
    names that are real queue symbols, so recording every identifier is safe.
    """
    try:
        with open(source_path, errors="replace") as f:
            lines = f.readlines()
    except (OSError, TypeError):
        return {}
    out = {}
    for i, line in enumerate(lines):
        m = ABI_TAG.search(line)
        if not m:
            continue
        reason = m.group(1)
        # Is this tag line itself the tail of an open /* block? Find out by
        # scanning down for the first genuine code line.
        in_block = line.strip().startswith(("/*", "*")) and not line.strip().endswith("*/")
        # If the tag is on a `// ...` line, no block is open.
        if line.strip().startswith("//"):
            in_block = False
        j = i + 1
        # Bind the reason to every identifier from the tag down to the end of
        # the definition it documents. `func_02059824`'s definition is preceded
        # by a forward-declaration of the body it calls, and several tags sit
        # above a prototype line before the real definition; collecting the
        # whole run to the opening `{` (or the closing `;` of a one-line body)
        # catches the real symbol whichever line carries it. Extra prototype
        # names are harmless -- the caller only ever asks about queue symbols.
        # A blank line or a new comment block ends the run (the next tagged def).
        run = 0
        while j < len(lines) and run < 12:
            st = lines[j].strip()
            is_c, in_block = _is_comment_or_blank(st, in_block)
            if is_c:
                if st == "" and not in_block:
                    break
                j += 1
                continue
            for name in IDENT.findall(lines[j]):
                if name not in _SKIP_IDENT:
                    out.setdefault(name, reason)
                    # /alternatename pragmas carry the MSVC `_` prefix the
                    # map parser strips; bind the stripped form too so a tag
                    # above an alias documents the symbol the queue asks for.
                    # Same one-underscore rule as the map parser, or a tag
                    # above a `__ZN...` alias would bind a name the queue
                    # never asks about.
                    out.setdefault(undecorate(name), reason)
            # A `{` opens the definition body -- stop here. A line ending in `;`
            # is a forward-declaration/prototype the definition sits below;
            # keep going. `}` closes a one-line body -- stop. And a bare macro
            # invocation IS the definition on the rows that use one, so it
            # stops the run too (MACRO_CALL above says why).
            if "{" in st or st.endswith("}") or MACRO_CALL.match(st):
                break
            j += 1
            run += 1
    return out


_REASON_CACHE = {}


def abi_reason(source_path, sym):
    """The PORT_HOST_ABI reason tagged above sym's definition, or None."""
    if not source_path:
        return None
    reasons = _REASON_CACHE.get(source_path)
    if reasons is None:
        reasons = _reasons_in(source_path)
        _REASON_CACHE[source_path] = reasons
    return reasons.get(sym)


def matched_index(root):
    """{symbol stem: repo-relative src path} for every matched TU."""
    out = {}
    for dirpath, _d, files in os.walk(os.path.join(root, "src")):
        for fn in files:
            if fn.endswith((".c", ".cpp")):
                stem = os.path.splitext(fn)[0]
                rel = os.path.relpath(os.path.join(dirpath, fn), root)
                out.setdefault(stem, rel.replace("\\", "/"))
    return out


def undecorate(sym):
    """Undo MSVC's cdecl decoration: exactly ONE leading underscore.

    32-bit MSVC prefixes every `extern "C"` symbol with a single `_`. The ROM's
    C++ names travel through the port as C identifiers that already begin with
    an underscore, so `_ZN4Heap8AllocateEji` is emitted `__ZN4Heap8AllocateEji`
    and only the FIRST underscore is decoration. `lstrip("_")` eats both and
    loses the name; taking one character keeps `_ZN...` intact and still
    undecorates the plain `_func_020226d0` case correctly. Decorated C++ names
    (`?Allocate@Heap@@...`) carry no underscore prefix and pass through.
    """
    return sym[1:] if sym.startswith("_") else sym


# Both manglings agree about one thing, the qualified name, and these two turn
# each into it. Deliberately partial: only the shape the join needs, and None
# for everything else, so an unparsed name drops out of the second join rather
# than joining on a wrong key. In particular NEITHER handles constructors,
# destructors or operators -- `_ZN6PlayerC1Ev` and `??0Player@@QAE@XZ` are the
# same method to a human but the two encodings do not spell the name the same
# way at all, and a wrong pair here would invent a shadow.
def itanium_qual(stem):
    """`_ZN6Player8BehaviorEv` -> `Player::Behavior`, or None.

    Reads the length-prefixed components between `_ZN` and its `E`, after any
    cv-qualifier letters (`_ZNK...` is a const member). Needs at least two
    components: a one-component `_ZN...E` is a namespace-scope free function
    with no class to pair against an MSVC `?name@Scope@@` row on equal terms.
    """
    if not stem.startswith("_ZN"):
        return None
    i = 3
    while i < len(stem) and stem[i] in "KVr":
        i += 1
    parts = []
    while i < len(stem) and stem[i].isdigit():
        j = i
        while j < len(stem) and stem[j].isdigit():
            j += 1
        n = int(stem[i:j])
        if j + n > len(stem):
            return None
        parts.append(stem[j:j + n])
        i = j + n
    if i >= len(stem) or stem[i] != "E" or len(parts) < 2:
        return None
    return "::".join(parts)


def msvc_qual(sym):
    """`?Behavior@Player@@UAEHXZ` -> `Player::Behavior`, or None.

    MSVC writes the name first and the enclosing scopes after it, innermost
    first, terminated by `@@`; everything past that is the calling convention
    and the signature, which this join deliberately ignores. `??`-prefixed
    names are the special ones (constructors, destructors, operators, vftables)
    and are refused -- see the note above the pair.
    """
    if not sym.startswith("?") or sym.startswith("??"):
        return None
    k = sym.find("@@", 1)
    if k < 0:
        return None
    parts = sym[1:k].split("@")
    if len(parts) < 2 or not all(parts):
        return None
    return "::".join(list(reversed(parts[1:])) + [parts[0]])


def qual_index(matched):
    """{qualified name: [matched stem, ...]} for every Itanium-named TU.

    A list, not a single stem: src/ carries several overloads of eleven
    methods, and the qualified name cannot tell them apart. The caller decides
    what to do with a list longer than one rather than this silently keeping
    whichever it saw first.
    """
    out = {}
    for stem in matched:
        q = itanium_qual(stem)
        if q:
            out.setdefault(q, []).append(stem)
    return out


def msvc_name_rows(syms, matched, linked_stems, itanium_hits):
    """The second join: host objects providing an MSVC-mangled matched method.

    Returns (shadows, already, ambiguous), each a list of rows sorted for
    printing. A row is (object, map symbol, matched stem(s)).

      shadows    the matched TU of that qualified name is not in the binary and
                 nothing else in the queue mentions it. New work, new rows.
      already    the matched TU is already in the queue under its Itanium name
                 from some host object, so it is reported, not recounted.
      ambiguous  the qualified name covers several matched overloads whose
                 linked state is not uniform, so which TU the host stands in
                 for cannot be read off the pair.

    `itanium_hits` is the set of matched-TU names the first join already found
    a host definition for -- the same `sym in matched` test that builds the
    queue above, passed in rather than recomputed so the two cannot disagree.
    """
    quals = qual_index(matched)
    shadows, already, ambiguous = {}, {}, {}
    for sym, obj in syms:
        stem = os.path.splitext(os.path.splitext(obj)[0])[0]
        if stem in matched:
            continue                      # an object named for a matched TU
        q = msvc_qual(sym)
        if not q:
            continue
        cands = quals.get(q)
        if not cands:
            continue
        unlinked = [c for c in cands if c not in linked_stems]
        if not unlinked:
            continue                      # the matched TU is in the binary
        if len(cands) > 1 and len(unlinked) != len(cands):
            ambiguous.setdefault((obj, q), (sym, sorted(cands)))
            continue
        bucket = already if any(c in itanium_hits for c in unlinked) else shadows
        bucket.setdefault((obj, q), (sym, sorted(unlinked)))
    def rows(d):
        return [(obj, q, v[0], v[1]) for (obj, q), v in sorted(d.items())]
    return rows(shadows), rows(already), rows(ambiguous)


def split_msvc_by_tag(root, rows):
    """(tagged, untagged) for MSVC-name rows, by the PORT_HOST_ABI ruling.

    The tag is a human ruling and wins here for the same reason it wins in the
    stem queue. It is looked up under the MATCHED STEM, not the MSVC symbol,
    because that is the name a tag can actually bind to: the tagged definition
    in these files is the ROM's C name (`void _ZN8PathLift12BaseBehaviorEv(void
    *c)`), and the MSVC spelling next to it is the thin C++ face over it. A
    `?...@...@@` string is not an identifier and no tag will ever name one.
    """
    tagged, untagged = [], []
    for row in rows:
        obj, _q, _sym, stems = row
        source = host_source_for(root, obj)
        if source is None:
            source = os.path.join(root, "port", "hal", "cxx_aliases.cpp")
        reason = None
        for st in stems:
            reason = abi_reason(source, st)
            if reason:
                break
        (tagged if reason else untagged).append(row + (reason,))
    return tagged, untagged


def map_symbols(mapfile):
    """[(symbol, objfile)] out of an MSVC /MAP."""
    # REFUSE A TRUNCATED MAP RATHER THAN MEASURING IT. A failed link leaves
    # walk_window.map at zero bytes (or a stub with no symbol rows), and this
    # tool used to answer that with a serene "linked into walk_window: 0
    # (0.0%)". A lane hit exactly that: its wrapper script returned success on
    # a link failure, the exe was absent, and the only thing that caught it was
    # a by-name check further down. A measurement tool that reports a number
    # for a build that does not exist is worse than one that errors.
    if not os.path.isfile(mapfile):
        sys.exit("linkage: no map at {} -- the link did not run".format(mapfile))
    if os.path.getsize(mapfile) < 4096:
        sys.exit("linkage: map at {} is {} bytes, which is a failed or "
                 "truncated link, not a build to measure"
                 .format(mapfile, os.path.getsize(mapfile)))
    out = []
    with open(mapfile, errors="replace") as f:
        for line in f:
            # The flag column between the RVA and the object is `f`, `f i`, or
            # absent (data rows). A bare \S* placeholder backtracks into the
            # object name on flagless rows and truncates `ov010_syms.c.obj`
            # to `c.obj`, so spell the flags out and anchor the object at EOL.
            m = re.match(
                r"\s+[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+[0-9a-fA-F]{8}\s+(?:[fi]\s+)*(\S+\.obj)\s*$",
                line)
            if m:
                out.append((undecorate(m.group(1)), m.group(2)))
    return out


FUNC_OV = re.compile(r"^func_(ov\d{3})_")
# An address-named arm9 TU: func_ followed by a RAM address in arm9's range.
FUNC_ADDR = re.compile(r"^func_0[12][0-9a-fA-F]{6}$")
# `Name kind:function(arm,size=0x138) addr:0x020bfec0`
CONFIG_SYM = re.compile(r"^(\S+)\s")


def module_index(root):
    """{symbol name: module} out of the config's own symbol files.

    arm9, then itcm and dtcm, then each overlay, and setdefault keeps the first
    claim: a name that appears in both is arm9's, which is the right way round
    because an overlay symbols.txt can carry a reference to an arm9 name.

    ITCM AND DTCM ARE MODULES. They were left out of four tools in this repo
    before, each time by the same accident of writing "arm9 and the overlays"
    and forgetting that config/arm9 has three more symbol files under it. Here
    the cost would have been small and specific: DMAStartTransfer,
    DMAStartTransferFB and OSReadROMArea are itcm functions and would have read
    as UNKNOWN, which invites someone to go looking for a bug in the stem
    matcher that is not there.
    """
    out = {}
    for rel, mod in ((("config", "arm9", "symbols.txt"), "arm9"),
                     (("config", "arm9", "itcm", "symbols.txt"), "itcm"),
                     (("config", "arm9", "dtcm", "symbols.txt"), "dtcm")):
        try:
            with open(os.path.join(root, *rel), errors="replace") as f:
                for line in f:
                    m = CONFIG_SYM.match(line)
                    if m:
                        out.setdefault(m.group(1), mod)
        except OSError:
            pass
    ovdir = os.path.join(root, "config", "arm9", "overlays")
    try:
        ovs = sorted(os.listdir(ovdir))
    except OSError:
        ovs = []
    for ov in ovs:
        p = os.path.join(ovdir, ov, "symbols.txt")
        if not os.path.isfile(p):
            continue
        with open(p, errors="replace") as f:
            for line in f:
                m = CONFIG_SYM.match(line)
                if m:
                    out.setdefault(m.group(1), ov)
    return out


def by_module(root, matched, linked_stems):
    """Print the headline split per ROM module, worst first.

    Takes the SAME matched index and linked set the headline was computed from,
    passed in rather than re-derived, so the two cannot drift. A second
    enumeration of the tree would be a second answer to the same question, and
    the moment they disagree neither is quotable.
    """
    name_mod = module_index(root)

    def module_of(stem):
        m = FUNC_OV.match(stem)
        if m:
            return m.group(1)
        if FUNC_ADDR.match(stem):
            return "arm9"
        return name_mod.get(stem, "UNKNOWN")

    total_by = {}
    unlinked_by = {}
    unknown_unlinked = []
    for stem in matched:
        mod = module_of(stem)
        total_by[mod] = total_by.get(mod, 0) + 1
        if stem not in linked_stems:
            unlinked_by[mod] = unlinked_by.get(mod, 0) + 1
            if mod == "UNKNOWN":
                unknown_unlinked.append(stem)

    print("BY MODULE, worst first. A TU's module is its stem: func_ovNNN_*"
          " names its")
    print("overlay, func_02xxxxxx is arm9, a named stem is looked up in"
          " config/arm9.")
    print()
    print("  %-9s %7s %7s %8s  %6s" % ("module", "total", "linked",
                                       "unlinked", "linked"))
    rows = sorted(total_by, key=lambda k: (-unlinked_by.get(k, 0), k))
    sum_t = sum_l = sum_u = 0
    for mod in rows:
        t = total_by[mod]
        u = unlinked_by.get(mod, 0)
        sum_t += t
        sum_l += t - u
        sum_u += u
        print("  %-9s %7d %7d %8d  %5.1f%%"
              % (mod, t, t - u, u, 100.0 * (t - u) / t))
    print("  %-9s %7d %7d %8d  %5.1f%%"
          % ("TOTAL", sum_t, sum_l, sum_u, 100.0 * sum_l / sum_t))

    # The reconciliation is the point of the table, so it is checked here
    # rather than left to whoever reads it.
    want_t, want_l = len(matched), len(linked_stems)
    if (sum_t, sum_l, sum_u) != (want_t, want_l, want_t - want_l):
        print()
        print("  RECONCILIATION FAILED: the table sums to %d/%d/%d but the"
              % (sum_t, sum_l, sum_u))
        print("  headline says %d/%d/%d. Do not quote either number."
              % (want_t, want_l, want_t - want_l))
    else:
        print("  (reconciles with the headline above: %d total, %d linked,"
              " %d unlinked)" % (want_t, want_l, want_t - want_l))

    if unknown_unlinked:
        print()
        print("  UNKNOWN unlinked (%d): stems no config symbols.txt claims."
              % len(unknown_unlinked))
        print("  Sample: %s" % ", ".join(sorted(unknown_unlinked)[:12]))
    print()


SELFTEST_MACRO_BLOCK = """\
static void seam(const char *n);
#define VS_SEAM(sym)                                                           \\
    extern "C" int sym(void);                                                  \\
    extern "C" int sym(void) { seam(#sym); return 0; }
// PORT_HOST_ABI: refused DS wireless-status wrapper.
VS_SEAM(func_0203d9f4)
// PORT_HOST_ABI: nothing to unload; every hosted overlay is a static mount.
VS_SEAM(UnloadOverlay)
// PORT_HOST_ABI: the port runs no DS threads.
VS_SEAM(func_02057f38)
#undef VS_SEAM
"""

SELFTEST_PROTOTYPE_BLOCK = """\
// PORT_HOST_ABI: ARM `swp` atomic exchange, no host equivalent.
unsigned int helper_decl(unsigned int, void *);
unsigned int func_02059824(unsigned int val, void *addr,
                           void (*fn)(void))
{
    return helper_decl(val, addr);
}
"""

SELFTEST_PROSE_BLOCK = """\
// THIS ONE IS NOT AN ABI EXCEPTION, and deliberately carries no
// PORT_HOST_ABI: tag, so this tool keeps counting it as a SHADOW.
unsigned int func_02057198(unsigned int val, void *addr)
{
    return 0;
}
"""


def selftest():
    """Fixtures for the tag binder. Run as `linkage.py --selftest`.

    Every case here is a shape the tree actually contains, and the first one is
    a bug this tool shipped: hal/scene_vs_menu.cpp's three VS_SEAM rows each
    carry their own tag, and the binder used to run past a macro invocation
    into the next row, so UnloadOverlay and func_02057f38 were both reported
    with func_0203d9f4's wireless reason.
    """
    import tempfile

    failures = []

    def reasons_of(text):
        with tempfile.NamedTemporaryFile("w", suffix=".cpp", delete=False,
                                         encoding="utf-8") as f:
            f.write(text)
            path = f.name
        try:
            return _reasons_in(path)
        finally:
            os.unlink(path)

    def check(what, got, want):
        if got != want:
            failures.append("%s: got %r, want %r" % (what, got, want))

    r = reasons_of(SELFTEST_MACRO_BLOCK)
    check("VS_SEAM row 1", r.get("func_0203d9f4"),
          "refused DS wireless-status wrapper.")
    check("VS_SEAM row 2", r.get("UnloadOverlay"),
          "nothing to unload; every hosted overlay is a static mount.")
    check("VS_SEAM row 3", r.get("func_02057f38"),
          "the port runs no DS threads.")

    # A tag above a prototype still reaches the definition below it: the run
    # keeps going through a `;` line and through the wrapped argument list.
    r = reasons_of(SELFTEST_PROTOTYPE_BLOCK)
    check("prototype then definition", r.get("func_02059824"),
          "ARM `swp` atomic exchange, no host equivalent.")
    check("the prototype's own name binds too", r.get("helper_decl"),
          "ARM `swp` atomic exchange, no host equivalent.")

    # KNOWN AND NOT FIXED HERE: a definition's own header comment that spells
    # the tag in prose is read as a tag. The binder's guard against prose is a
    # blank line, and a header comment has none between itself and the body it
    # documents. hal/boot_hw.cpp had the tree's one instance and lane LOADOV
    # deleted it at the source. This case is pinned so the day someone teaches
    # the binder to tell prose from a ruling, the fixture says what changed.
    r = reasons_of(SELFTEST_PROSE_BLOCK)
    check("prose mention is still read as a tag (known)",
          r.get("func_02057198"),
          "tag, so this tool keeps counting it as a SHADOW.")

    for line in failures:
        print("linkage selftest FAIL: " + line)
    if failures:
        print("linkage selftest: %d failure(s)" % len(failures))
        return 1
    print("linkage selftest: 6 checks, 0 failed -- tag binder OK")
    return 0


def default_root():
    """The checkout this script lives in: <root>/port/tools/linkage.py."""
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


def main():
    if "--selftest" in sys.argv:
        return selftest()

    positional = [a for a in sys.argv[1:] if not a.startswith("--")]
    root = positional[0] if positional else default_root()
    show_queue = "--queue" in sys.argv
    show_exceptions = "--exceptions" in sys.argv
    show_faces = "--faces" in sys.argv
    show_modules = "--by-module" in sys.argv
    show_msvc = "--msvc-names" in sys.argv

    mapfile = os.path.join(root, "build", "port", "walk_window.map")
    if not os.path.exists(mapfile):
        print("no %s -- build walk_window first" % mapfile)
        return 2

    # Say which tree and which binary this reading came from. A linkage number
    # is only quotable next to these two lines.
    print("repo root : %s" % os.path.abspath(root))
    print("map       : %s" % os.path.abspath(mapfile))
    print()

    matched = matched_index(root)
    syms = map_symbols(mapfile)

    linked_stems = set()
    host_hits = {}
    for sym, obj in syms:
        stem = os.path.splitext(os.path.splitext(obj)[0])[0]
        if stem in matched:
            linked_stems.add(stem)
        # a host object is one whose name is not itself a matched TU
        if stem not in matched and sym in matched:
            host_hits.setdefault(obj, set()).add(sym)

    total = len(matched)
    linked = len(linked_stems)
    print("matched TUs in src/        : %d" % total)
    print("linked into walk_window    : %d  (%.1f%%)" % (linked, 100.0 * linked / total))
    print("still unlinked             : %d" % (total - linked))
    print()

    if show_modules:
        by_module(root, matched, linked_stems)

    # The second join, on (class, method) instead of the stem. `itanium_hits`
    # is exactly the set the first join found, handed over rather than derived
    # again, so "already surfaced above" means the same thing in both places.
    itanium_hits = set()
    for _s in host_hits.values():
        itanium_hits |= _s
    msvc_shadows, msvc_already, msvc_amb = msvc_name_rows(
        syms, matched, linked_stems, itanium_hits)

    if not host_hits and not msvc_shadows:
        print("no host object defines a symbol that src/ also has. Nothing to replace.")
        return 0

    # Split each host object's symbols three ways. Order matters: the
    # PORT_HOST_ABI tag is a human ruling and wins, then a matched TU that is
    # itself linked marks a face, and what is left is the work list.
    src_cache = {}
    documented = {}    # obj -> {sym: reason}   EXCEPTIONS, tagged
    faces = {}         # obj -> set(sym)        matched TU is linked too
    undocumented = {}  # obj -> set(sym)        SHADOWS, the work list
    for obj, s in host_hits.items():
        source = src_cache.get(obj)
        if source is None:
            source = host_source_for(root, obj)
            if source is None:
                # A host object with no source under the host dirs is a
                # generated seat (ovNNN_syms.c.obj). The only way a matched
                # TU's name lands in one is an /alternatename alias, and
                # cxx_aliases.cpp is the alias registry -- read the reason
                # from the tag above the alias.
                source = os.path.join(root, "port", "hal", "cxx_aliases.cpp")
            src_cache[obj] = source
        for sym in s:
            reason = abi_reason(source, sym)
            if reason:
                documented.setdefault(obj, {})[sym] = reason
            elif sym in linked_stems:
                faces.setdefault(obj, set()).add(sym)
            else:
                undocumented.setdefault(obj, set()).add(sym)

    n_all = sum(len(v) for v in host_hits.values())
    n_doc = sum(len(v) for v in documented.values())
    n_face = sum(len(v) for v in faces.values())
    n_undoc = sum(len(v) for v in undocumented.values())

    print("REPLACEMENT QUEUE: %d symbols across %d host objects are defined by a"
          % (n_all, len(host_hits)))
    print("host file while src/ carries a matched TU of the same name. Of those,")
    print("  %d are documented host-ABI exceptions (asm primitives, register"
          % n_doc)
    print("     ride-throughs, mwcc pointer-to-member, unmodelled DS hardware)")
    print("  %d are FACES -- the matched TU of that name is linked too, so the"
          % n_face)
    print("     host definition is most likely the C-name bridge INTO it, not a")
    print("     stand-in for it. Not work; not proven either (see --faces)")
    print("  %d are SHADOWS -- no matched TU of that name reaches the binary,"
          % n_undoc)
    print("     so the host body is what runs. This is the replacement work")
    print()

    # The MSVC-name half of the queue. Its own paragraph, because the count
    # above is "symbols the stem join found" and adding a differently-found row
    # into that sum would make the three buckets stop summing to it.
    n_msvc = len(msvc_shadows)
    msvc_doc, msvc_undoc = split_msvc_by_tag(root, msvc_shadows)
    print("AND %d rows the stem join above cannot see. A host copy that defines"
          % n_msvc)
    print("the method as real C++ puts only MSVC's mangling in the map")
    print("(?Behavior@Player@@UAEHXZ), which carries no _ZN for the stem join to")
    print("match. Joining on the pair those two manglings agree about -- the")
    print("class and the method -- finds them. An MSVC-NAME SHADOW is a SHADOW;")
    print("the label says which join found it, not that it is a lesser fact.")
    print("  %-4d MSVC-NAME SHADOWS  new rows, new replacement work"
          % len(msvc_undoc))
    print("  %-4d MSVC-NAME EXCEPT.  the host definition next to the MSVC face"
          % len(msvc_doc))
    print("  %-4s                    carries a PORT_HOST_ABI tag, and the tag"
          % "")
    print("  %-4s                    is a ruling: not work" % "")
    print("  %-4d already surfaced   the matched TU is in the queue above under"
          % len(msvc_already))
    print("  %-4s                    its Itanium name, so it is named here, not"
          % "")
    print("  %-4s                    counted twice" % "")
    print("  %-4d AMBIGUOUS          several matched overloads share the"
          % len(msvc_amb))
    print("  %-4s                    qualified name and disagree on linked"
          % "")
    print("  %-4s                    state, so the pair cannot say which" % "")
    print()
    print("QUEUE TOTAL: %d rows = %d found by the stem join + %d by the"
          % (n_all + n_msvc, n_all, n_msvc))
    print("(class, method) join. The %d already-surfaced and %d ambiguous rows"
          % (len(msvc_already), len(msvc_amb)))
    print("are deliberately outside that total.")
    print()

    # The shadow queue is the work list.
    if undocumented:
        print("UNDOCUMENTED QUEUE / SHADOWS (%d):" % n_undoc)
        for obj, s in sorted(undocumented.items(), key=lambda kv: -len(kv[1])):
            print("  %-40s %d symbol(s)" % (obj, len(s)))
            if show_queue:
                for sym in sorted(s):
                    print("        %-46s %s" % (sym, matched[sym]))
    else:
        print("UNDOCUMENTED QUEUE / SHADOWS (0): every queue symbol is a")
        print("documented host-ABI exception or a face over a linked matched")
        print("TU. The only way to shrink it further is a toolchain that can")
        print("run the ROM's ARM asm and hardware pokes.")

    print()
    print("MSVC-NAME SHADOWS (%d): the host object provides the MSVC mangling"
          % len(msvc_undoc))
    print("of a matched method whose Itanium-named TU is not in the binary.")
    for obj, q, sym, stems, _r in msvc_undoc:
        print("  %-40s %s" % (obj, q))
        print("        %-46s %s" % (sym, matched[stems[0]]))
    if msvc_doc:
        print()
        print("MSVC-NAME EXCEPTIONS (%d): same join, but the host definition"
              % len(msvc_doc))
        print("the MSVC face sits on carries a PORT_HOST_ABI tag.")
        for obj, q, sym, stems, reason in msvc_doc:
            print("  %-40s %s" % (obj, q))
            print("        %-46s %s" % (sym, matched[stems[0]]))
            print("        %s" % reason)
    if show_msvc:
        print()
        print("MSVC-NAME, ALREADY SURFACED (%d): counted once, above, under the"
              % len(msvc_already))
        print("Itanium name. Listed so the pair is on the record.")
        for obj, q, sym, stems in msvc_already:
            print("  %-40s %s" % (obj, q))
            print("        %-46s %s" % (sym, matched[stems[0]]))
        print()
        print("MSVC-NAME, AMBIGUOUS (%d): src/ carries several overloads of this"
              % len(msvc_amb))
        print("qualified name and they do not agree on linked state. The pair")
        print("cannot say which one the host stands in for, so neither can this.")
        for obj, q, sym, stems in msvc_amb:
            print("  %-40s %s" % (obj, q))
            print("        %s" % sym)
            for st in stems:
                print("        candidate %-36s %s" % (st, matched[st]))

    print()
    print("FACES (%d): host definition and matched TU BOTH linked. The matched"
          % n_face)
    print("code is in the binary; these carry the ROM's C name to it. A map")
    print("cannot prove the host definition forwards rather than duplicates,")
    print("so read one before quoting it as either.")
    for obj, s in sorted(faces.items(), key=lambda kv: -len(kv[1])):
        print("  %-40s %d symbol(s)" % (obj, len(s)))
        if show_faces:
            for sym in sorted(s):
                print("        %-46s %s" % (sym, matched[sym]))

    if show_exceptions:
        print()
        print("DOCUMENTED HOST-ABI EXCEPTIONS (%d):" % n_doc)
        for obj, d in sorted(documented.items(), key=lambda kv: -len(kv[1])):
            print("  %-40s %d symbol(s)" % (obj, len(d)))
            for sym in sorted(d):
                print("        %-30s %s" % (sym, d[sym]))

    if (not show_queue or not show_exceptions or not show_faces
            or not show_modules or not show_msvc):
        print()
        print("re-run with --queue for shadow sources, --faces for the face")
        print("list, --exceptions for the documented reasons, --by-module for")
        print("the per-module split of the headline, --msvc-names for the")
        print("already-surfaced and ambiguous halves of the second join")
    return 0


if __name__ == "__main__":
    sys.exit(main())
