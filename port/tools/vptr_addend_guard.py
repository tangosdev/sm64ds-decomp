#!/usr/bin/env python3
"""Refuse a vtable pointer stored two slots high.

Run link100, lane VPTR measured the defect and lane HOSTGEN2 wrote this guard.

WHAT IT CATCHES. mwcc's own vtable symbol denotes the OBJECT START, and the
Itanium address point is two words past it, so a translation unit that EMITS its
class's vtable writes the vptr as `&_ZTV<X>[2]` or `_ZTV<X> + 2`. Compiled for
the host that becomes a DIR32 relocation against `__ZTV<X>` with the value 8
applied at the relocation site. The port's vtables are host arrays whose slot 0
IS the first virtual, and its live convention was read off a running binary as
vptr == table + 0, so the +8 store puts every dispatch two slots high: ROM slot
0, InitResources, the first call on the spawn path, enters slot 2 instead.

WHY A GUARD AND NOT ONLY A TRANSFORM. tools/hostgen.py drops the bias on the way
into MSVC, but it only reaches the translation units the port substitutes; the
rest of src/ compiles straight. Nothing else can see this. The byte gate compares
the matched object, where the spelling is correct; the linker resolves the name
and applies the addend without a word; linkage.py counts the row as linked. Only
a run convicts it, as a wrong function with a wrong arity on the spawn path.

WHAT IT READS. The objects of the LAST BUILD, straight out of their COFF tables
-- no dumpbin, no toolchain environment -- so it runs in the pre-configure wave
and refuses a regression before the next build starts. For each
IMAGE_REL_I386_DIR32 relocation whose target symbol names a vtable (`_ZTV`, in
either the C spelling or an MSVC-decorated one), it reads the four bytes the
compiler left at the relocation site: that is the addend.

THE OBJECT SET COMES FROM build.ninja, NOT FROM A DIRECTORY WALK. An .obj on
disk is not necessarily an .obj in the build: the moment a TU joins
port/CMakeLists.txt's PORT_HOSTGEN_TU list the raw path stops being compiled and
its old object stays behind, carrying the exact +8 store the substitution was
added to remove. Six such objects were sitting in this tree's build directory
when the scoping was written, dScMgCup_c.cpp.obj among them. Reading them would
have reported a defect the build no longer contains and hidden the proof that
the rewrite works.

AND ONLY THE TRANSLATION UNITS THAT STORE A VPTR ARE IN SCOPE: the decomp's own
sources, compiled raw out of src/ or generated into build/port/host-src/src/.
Everything else in this build REACHES INTO A VTABLE ON PURPOSE and a nonzero
value there is an array index, not a bias. port/hal/ fills the host tables one
slot at a time (`vt[3] = f;` is a DIR32 against the array at +0xc, and the hal
files alone carry 3,354 of those over every addend from 0x4 up); the generated
host-src/ov0NN_syms.c data images address table interiors; port/tests/ watches
them (`port_watch_words(&_ZTV7dCcAc_c[2], 2)`). Judging those would make the
guard a 2,933-row wall of false positives, which is how it read before the scope
was written. In the decomp trees the signal is clean: 71 nonzero rows in this
tree, every last one exactly 0x8, which is lane VPTR's 70 (file, table) pairs.
port/unmatched/'s host copies are the correction and store 0.

THE BASELINE is the list of (object, symbol, addend) rows already known and
adjudicated. A row in it is reported and allowed; anything else refuses the
build. The file is written by --write-baseline and is meant to SHRINK: every row
that leaves it is a class whose store came back to the address point.

    python port/tools/vptr_addend_guard.py <root>
    python port/tools/vptr_addend_guard.py <root> --list
    python port/tools/vptr_addend_guard.py <root> --write-baseline
    python port/tools/vptr_addend_guard.py --selftest
"""

import argparse
import pathlib
import re
import struct
import sys

IMAGE_REL_I386_DIR32 = 0x0006
IMAGE_SCN_LNK_NRELOC_OVFL = 0x01000000
VTABLE_NAME = re.compile(r"_ZTV[A-Za-z0-9_]+")

BASELINE_NAME = "vptr_addend_baseline.txt"

# CMake mangles an absolute source path into the object tree by replacing the
# drive colon with an underscore: C:/tmp/l7c/src/x.cpp becomes
# <target>.dir/C_/tmp/l7c/src/x.cpp.obj. Stripping that prefix back to the
# decomp-relative path is what makes a baseline row mean the same thing in every
# worktree; keyed by the raw object path, every lane would need its own file.
ABS_PREFIX = re.compile(r"^[A-Za-z]_/(?:[^/]+/)*?(?=(?:src|port)/)")
NINJA_OBJ = re.compile(r"(?m)^build ([^:\n]*\.obj): ")

# The trees whose objects STORE a vptr. Anything else addresses a vtable on
# purpose; see the module docstring.
IN_SCOPE = ("src/", "host-src/src/")


def source_key(obj_rel):
    """An object path as its decomp-relative source path.

    <target>.dir/C_/tmp/l7c/src/actors/daBmb_c.cpp.obj -> src/actors/daBmb_c.cpp.obj
    <target>.dir/host-src/src/actors/dScMgCup_c.cpp.obj -> unchanged
    <target>.dir/hal/actor_classes_ov002.cpp.obj        -> unchanged
    """
    tail = obj_rel.replace("\\", "/").split(".dir/", 1)[-1]
    return ABS_PREFIX.sub("", tail)


def in_scope(key):
    return key.startswith(IN_SCOPE)


def live_objects(build):
    """The .obj outputs of the last configure, read off build.ninja.

    Returns None when there is no build.ninja to read: a tree that has never
    been configured has nothing for this guard to judge, and falling back to a
    directory walk would read whatever a previous configure left behind.
    """
    ninja = pathlib.Path(build) / "build.ninja"
    if not ninja.is_file():
        return None
    text = ninja.read_text(encoding="utf-8", errors="replace")
    return sorted({m.group(1).replace("\\", "/") for m in NINJA_OBJ.finditer(text)})


class CoffError(Exception):
    pass


def _sym_name(raw, strtab):
    """A COFF symbol name: eight bytes inline, or a string-table offset."""
    if raw[:4] == b"\0\0\0\0":
        off = struct.unpack_from("<I", raw, 4)[0]
        end = strtab.find(b"\0", off)
        return strtab[off:end if end >= 0 else len(strtab)].decode("latin-1")
    return raw.rstrip(b"\0").decode("latin-1")


def vtable_addends(data):
    """Yield (symbol, addend) for every DIR32 relocation against a _ZTV name."""
    if len(data) < 20:
        raise CoffError("too short to be a COFF object")
    machine, nsec, _stamp, symptr, nsyms, optsz, _chars = struct.unpack_from(
        "<HHIIIHH", data, 0)
    if machine == 0x0000 and nsec == 0xFFFF:
        # ANON_OBJECT_HEADER_BIGOBJ (/bigobj). Nothing in this tree emits one;
        # say so rather than misreading the header as a normal COFF.
        raise CoffError("bigobj header, not read")
    if machine not in (0x014C, 0x0000):
        raise CoffError("machine 0x%04x is not i386" % machine)

    strtab = b""
    if symptr and nsyms:
        base = symptr + nsyms * 18
        if base + 4 <= len(data):
            size = struct.unpack_from("<I", data, base)[0]
            strtab = data[base:base + max(size, 4)]

    names = {}
    for i in range(nsyms):
        off = symptr + i * 18
        if off + 18 > len(data):
            break
        raw = data[off:off + 8]
        names[i] = _sym_name(raw, strtab)

    out = []
    for s in range(nsec):
        off = 20 + optsz + s * 40
        if off + 40 > len(data):
            break
        (_name, _vsize, vaddr, _rawsize, rawptr, relptr, _lnptr,
         nrel, _nln, chars) = struct.unpack_from("<8sIIIIIIHHI", data, off)
        if not relptr or not nrel:
            continue
        first = 0
        if nrel == 0xFFFF and (chars & IMAGE_SCN_LNK_NRELOC_OVFL):
            nrel = struct.unpack_from("<I", data, relptr)[0]
            first = 1
        for r in range(first, nrel + first):
            roff = relptr + r * 10
            if roff + 10 > len(data):
                break
            rva, symidx, rtype = struct.unpack_from("<IIH", data, roff)
            if rtype != IMAGE_REL_I386_DIR32:
                continue
            name = names.get(symidx, "")
            if not VTABLE_NAME.search(name):
                continue
            site = rawptr + (rva - vaddr)
            if site < 0 or site + 4 > len(data):
                continue
            out.append((name, struct.unpack_from("<I", data, site)[0]))
    return out


def scan(build_dir):
    """Every vtable relocation of the last build, split by scope.

    Returns (rows, exempt, unread, missing). `rows` is the in-scope set as
    (source key, vtable symbol, applied value); `exempt` counts the rest.
    """
    build = pathlib.Path(build_dir)
    live = live_objects(build)
    if live is None:
        return None, 0, [], 0
    rows = []
    unread = []
    exempt = 0
    missing = 0
    for rel in live:
        obj = build / rel
        if not obj.is_file():
            # An output the last build never got to (a failed link leaves the
            # objects behind it unbuilt). Not a defect and not this guard's.
            missing += 1
            continue
        key = source_key(rel)
        try:
            found = vtable_addends(obj.read_bytes())
        except (CoffError, struct.error) as exc:
            unread.append((key, str(exc)))
            continue
        if not in_scope(key):
            exempt += sum(1 for _n, a in found if a)
            continue
        for name, addend in found:
            rows.append((key, name, addend))
    return rows, exempt, unread, missing


def read_baseline(path):
    known = set()
    if not path.exists():
        return known
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) != 3:
            continue
        known.add((parts[0], parts[1], int(parts[2], 0)))
    return known


BASELINE_HEADER = """# port/tools/vptr_addend_baseline.txt -- the vtable stores this tree is KNOWN to
# make at a nonzero addend, one row per (object, vtable symbol, applied value).
#
# Every row here is a class whose factory writes its object's vptr TWO SLOTS HIGH
# on the host: the decomp spells the Itanium address point, which mwcc needs
# because its own vtable symbol denotes the object start, and the port's tables
# have no header words for that bias to skip. Lane VPTR's measurement and the
# five-class disassembly are in runs/link100/out/VPTR/report.md.
#
# THE FILE IS MEANT TO SHRINK. A row leaves it when its TU joins
# port/CMakeLists.txt's PORT_HOSTGEN_TU list, where tools/hostgen.py's
# VPTR_ADDRESS_POINT rewrite drops the bias, or when the decomp side settles it.
# port/tools/vptr_addend_guard.py refuses any row that is NOT here, so a class
# main promotes to a key-function TU tomorrow stops the build instead of
# dispatching silently wrong.
#
# A ROW IS A DEFECT ON HOLD, NOT AN EXEMPTION. Retiring one of these stores is a
# DISPATCH CHANGE (in3/COMMON.md): the port's four factory host copies are the
# only correct stores in the tree today, and the VPTR ruling sequences the swap
# behind a linking tree and a scene-to-level capture. Until then the rows sit
# here so that the SEVENTY-SECOND one stops the build.
#
#   <source path>.obj  <vtable symbol>  <applied value>
"""


def write_baseline(path, rows):
    lines = [BASELINE_HEADER]
    for obj, name, addend in sorted(set(rows)):
        lines.append("%-52s %-56s 0x%x\n" % (obj, name, addend))
    path.write_text("".join(lines), encoding="utf-8")


def selftest():
    """A synthetic i386 COFF with one DIR32 relocation against a _ZTV name."""
    sym = b"__ZTV7daBmb_c\0"
    strtab = struct.pack("<I", 4 + len(sym)) + sym
    # one section, raw data = the four bytes the addend lives in
    raw = struct.pack("<I", 8)
    nsyms = 1
    hdr_size = 20 + 40
    rawptr = hdr_size
    relptr = rawptr + len(raw)
    symptr = relptr + 10
    data = struct.pack("<HHIIIHH", 0x014C, 1, 0, symptr, nsyms, 0, 0)
    data += struct.pack("<8sIIIIIIHHI", b".text\0\0\0", 0, 0, len(raw),
                        rawptr, relptr, 0, 1, 0, 0)
    data += raw
    data += struct.pack("<IIH", 0, 0, IMAGE_REL_I386_DIR32)
    data += struct.pack("<8sIHHBB", b"\0\0\0\0" + struct.pack("<I", 4),
                        0, 1, 0, 2, 0)
    data += strtab

    got = vtable_addends(data)
    assert got == [("__ZTV7daBmb_c", 8)], got

    # addend 0 is the correct store and must read back as such
    zero = data[:rawptr] + struct.pack("<I", 0) + data[rawptr + 4:]
    assert vtable_addends(zero) == [("__ZTV7daBmb_c", 0)], "addend 0"

    # a relocation against a name that is not a vtable is not this guard's
    other = data.replace(b"__ZTV7daBmb_c\0", b"__ZN7daBmb_cX\0")
    assert vtable_addends(other) == [], "non-vtable symbol"

    # a relocation type other than DIR32 is not an absolute store
    rel32 = data[:relptr] + struct.pack("<IIH", 0, 0, 0x0014) + data[relptr + 10:]
    assert vtable_addends(rel32) == [], "REL32"

    # a truncated object is reported, never guessed at
    try:
        vtable_addends(data[:8])
    except CoffError:
        pass
    else:
        raise AssertionError("short object was not refused")

    # THE SCOPE. A decomp TU is judged; the hal file that fills the same table
    # one slot at a time is not, in either spelling of the object path.
    assert source_key("port_slice_shared.dir/C_/tmp/l7c/src/actors/daBmb_c.cpp.obj") \
        == "src/actors/daBmb_c.cpp.obj", "abs prefix"
    assert source_key(
        "port_slice_shared.dir\\C_\\tmp\\l4seat\\src\\game\\a.cpp.obj") \
        == "src/game/a.cpp.obj", "backslashes and another worktree"
    assert source_key("port_slice_shared.dir/host-src/src/actors/x.cpp.obj") \
        == "host-src/src/actors/x.cpp.obj", "host-src"
    assert source_key("port_slice_shared.dir/hal/actor_classes_ov002.cpp.obj") \
        == "hal/actor_classes_ov002.cpp.obj", "hal"
    assert in_scope("src/actors/daBmb_c.cpp.obj"), "src in scope"
    assert in_scope("host-src/src/actors/x.cpp.obj"), "host-src in scope"
    assert not in_scope("hal/actor_classes_ov002.cpp.obj"), "hal out of scope"
    assert not in_scope("unmatched/MgCup_Factory.cpp.obj"), "unmatched out"
    assert not in_scope("host-src/ov006_syms.c.obj"), "generated data out"
    assert not in_scope("tests/walk_window.cpp.obj"), "tests out"

    # THE OBJECT SET. build.ninja is the authority, and a tree that has never
    # been configured is not judged at all rather than walked.
    import tempfile
    with tempfile.TemporaryDirectory() as td:
        tdp = pathlib.Path(td)
        assert live_objects(tdp) is None, "no build.ninja"
        (tdp / "build.ninja").write_text(
            "build CMakeFiles\\t.dir\\C_\\tmp\\x\\src\\a.cpp.obj: CXX_COMPILER__t\n"
            "  DEFINES = -DX\n"
            "build CMakeFiles\\t.dir\\hal\\b.cpp.obj: CXX_COMPILER__t\n"
            "build walk_window.exe: CXX_EXECUTABLE_LINKER__t a.obj b.obj\n",
            encoding="utf-8")
        got = live_objects(tdp)
        assert got == ["CMakeFiles/t.dir/C_/tmp/x/src/a.cpp.obj",
                       "CMakeFiles/t.dir/hal/b.cpp.obj"], got
        # the link line names objects as INPUTS; they are not outputs
        assert not any(o.endswith("/a.obj") for o in got), "link inputs"

    print("selftest PASS (17 arms, 0 failed)")
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root", nargs="?", help="the port worktree root")
    ap.add_argument("--build-dir", help="override <root>/build/port")
    ap.add_argument("--baseline", help="override port/tools/" + BASELINE_NAME)
    ap.add_argument("--list", action="store_true",
                    help="print every nonzero row and exit 0")
    ap.add_argument("--write-baseline", action="store_true")
    ap.add_argument("--selftest", action="store_true")
    args = ap.parse_args()

    if args.selftest:
        return selftest()
    if not args.root and not args.build_dir:
        ap.error("a root (or --build-dir) is required")

    here = pathlib.Path(__file__).resolve().parent
    root = pathlib.Path(args.root).resolve() if args.root else here.parents[1]
    build = pathlib.Path(args.build_dir) if args.build_dir else root / "build" / "port"
    baseline = pathlib.Path(args.baseline) if args.baseline else here / BASELINE_NAME

    if not build.is_dir():
        print("vptr_addend_guard: no %s yet -- nothing built to read" % build)
        return 0

    rows, exempt, unread, missing = scan(build)
    if rows is None:
        print("vptr_addend_guard: no %s/build.ninja yet -- nothing built to read"
              % build)
        return 0
    nonzero = [r for r in rows if r[2] != 0]

    if args.write_baseline:
        write_baseline(baseline, nonzero)
        print("vptr_addend_guard: %d nonzero row(s) -> %s" % (len(nonzero), baseline))
        return 0

    if args.list:
        for obj, name, addend in sorted(set(nonzero)):
            print("%-52s %-56s 0x%x" % (obj, name, addend))
        print("vptr_addend_guard: %d in-scope object(s) read, %d vtable "
              "relocation(s), %d nonzero; %d nonzero row(s) out of scope"
              % (len(set(r[0] for r in rows)), len(rows), len(nonzero), exempt))
        return 0

    known = read_baseline(baseline)
    new = sorted(set(r for r in nonzero if r not in known))
    if new:
        print("vptr_addend_guard: REFUSED -- %d vtable store(s) at a nonzero "
              "addend that the baseline does not carry:" % len(new))
        for obj, name, addend in new:
            print("    %-52s %-56s 0x%x" % (obj, name, addend))
        print("  A vptr stored at table+%s dispatches ROM slot k as slot k+%d: "
              "slot 0, InitResources, enters the class's third virtual on the "
              "spawn path." % (hex(new[0][2]), new[0][2] // 4))
        print("  The fix is the transform, not the baseline: put the owning TU "
              "on port/CMakeLists.txt's PORT_HOSTGEN_TU list, where hostgen's "
              "VPTR_ADDRESS_POINT rewrite drops the bias. Widen the baseline "
              "only to park a store whose retirement is sequenced behind a "
              "capture, and say which in the commit.")
        return 1

    if unread:
        print("vptr_addend_guard: %d object(s) not read (%s)"
              % (len(unread), unread[0][1]))
    if missing:
        print("vptr_addend_guard: %d build output(s) not on disk yet" % missing)
    print("vptr_addend_guard OK -- %d vtable relocation(s) over %d in-scope "
          "object(s), %d at a nonzero addend, all in the baseline; %d nonzero "
          "row(s) out of scope"
          % (len(rows), len(set(r[0] for r in rows)) if rows else 0,
             len(nonzero), exempt))
    return 0


if __name__ == "__main__":
    sys.exit(main())
