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

NOT every queue symbol is scaffolding, though. A matched src TU that is an ARM
asm primitive, an ARM register ride-through, an mwcc pointer-to-member dispatch,
or a poke of DS hardware the ntr layer does not model cannot compile-and-behave
under MSVC no matter how much wiring is added -- the host definition IS the
faithful stand-in. Those are host-ABI EXCEPTIONS, not unknowns, and this tool
tells them apart: a host definition carrying a `PORT_HOST_ABI:` tag in the
comment right above it is a documented exception; one without a tag is real
replacement work still to be understood. The headline the port drives to zero
is the UNDOCUMENTED queue, not the raw one.

    python port/tools/linkage.py [repo-root] [--queue] [--exceptions]

  --queue        list every queue symbol with its matched source
  --exceptions   also print the documented host-ABI exceptions and their reasons
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
                    out.setdefault(name.lstrip("_"), reason)
            # A `{` opens the definition body -- stop here. A line ending in `;`
            # is a forward-declaration/prototype the definition sits below;
            # keep going. `}` closes a one-line body -- stop.
            if "{" in st or st.endswith("}"):
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


def map_symbols(mapfile):
    """[(symbol, objfile)] out of an MSVC /MAP."""
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
                out.append((m.group(1).lstrip("_"), m.group(2)))
    return out


def main():
    positional = [a for a in sys.argv[1:] if not a.startswith("--")]
    root = positional[0] if positional else "."
    show_queue = "--queue" in sys.argv
    show_exceptions = "--exceptions" in sys.argv

    mapfile = os.path.join(root, "build", "port", "walk_window.map")
    if not os.path.exists(mapfile):
        print("no %s -- build walk_window first" % mapfile)
        return 2

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

    if not host_hits:
        print("no host object defines a symbol that src/ also has. Nothing to replace.")
        return 0

    # Split each host object's symbols into documented host-ABI exceptions
    # (a PORT_HOST_ABI tag sits above the definition) and undocumented ones.
    src_cache = {}
    documented = {}    # obj -> {sym: reason}
    undocumented = {}  # obj -> set(sym)
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
            else:
                undocumented.setdefault(obj, set()).add(sym)

    n_all = sum(len(v) for v in host_hits.values())
    n_doc = sum(len(v) for v in documented.values())
    n_undoc = sum(len(v) for v in undocumented.values())

    print("REPLACEMENT QUEUE: %d symbols across %d host objects are defined by a"
          % (n_all, len(host_hits)))
    print("host file while src/ carries a matched TU of the same name. Of those,")
    print("  %d are documented host-ABI exceptions (asm primitives, register"
          % n_doc)
    print("     ride-throughs, mwcc pointer-to-member, unmodelled DS hardware)")
    print("  %d are undocumented -- the real replacement work" % n_undoc)
    print()

    # The undocumented queue is the work list.
    if undocumented:
        print("UNDOCUMENTED QUEUE (%d):" % n_undoc)
        for obj, s in sorted(undocumented.items(), key=lambda kv: -len(kv[1])):
            print("  %-40s %d symbol(s)" % (obj, len(s)))
            if show_queue:
                for sym in sorted(s):
                    print("        %-46s %s" % (sym, matched[sym]))
    else:
        print("UNDOCUMENTED QUEUE (0): every queue symbol is a documented")
        print("host-ABI exception. The only way to shrink it further is a")
        print("toolchain that can run the ROM's ARM asm and hardware pokes.")

    if show_exceptions:
        print()
        print("DOCUMENTED HOST-ABI EXCEPTIONS (%d):" % n_doc)
        for obj, d in sorted(documented.items(), key=lambda kv: -len(kv[1])):
            print("  %-40s %d symbol(s)" % (obj, len(d)))
            for sym in sorted(d):
                print("        %-30s %s" % (sym, d[sym]))

    if not show_queue and not show_exceptions:
        print()
        print("re-run with --queue for sources, --exceptions for the reasons")
    return 0


if __name__ == "__main__":
    sys.exit(main())
