"""Decide which enrolled functions may be compiled into the ROM (`complete`).

A `src/` file byte-matches its function, but that is not sufficient to *link* it in
place of the ROM's own bytes. The object has to occupy exactly the address range the
file entry declares and contribute nothing else, or it shifts every later address in
the module. So this is a whitelist, not a blacklist:

  1. exactly one content section, named `.text` - mwccarm emits one `.text` section per
     function, so a translation unit defining more than one function (any C++ dtor, which
     emits D0/D1/D2 plus thunks) would place all of them, at addresses only one of which
     is right;
  2. exactly one defined global FUNC symbol, and its `st_size` equals the declared size;
  3. no `.rodata` / `.data` / `.bss` / `.init` / `.ctor` content - the ROM's copy of that
     data stays in the gap object, so ours would be a duplicate that grows the module;
  4. the function lives in a `.text` or `.init` range. `.init` is buildable because
     `rombuild.retarget_text_section` renames the compiled object's `.text` header to
     `.init`, so dsd's `File.o(.init)` selector finds it; nothing renames any other
     section, so every other section is still a rejection. This is the *ROM's* section
     for the function's address, and is unrelated to rule 3, which is about what the
     compiled object itself contains;
  5. every undefined reference names a symbol that config/**/symbols.txt actually
     defines - an invented name has no address, and because gap objects import
     carved-out symbols *weakly* it would link silently to 0 rather than error.

Passing all five means the object is a drop-in replacement for the ROM's bytes. Whether
it is the *right* replacement is then settled by `dsd check modules`.

Usage:
    python tools/eligible.py                 # classify, write the pass list
    python tools/eligible.py -j 16
    python tools/eligible.py --verify        # also byte-check the built modules

Writes build/rombuild-eligibility.json (per-file verdict + reason) and
build/eligible-names.txt (the pass list, for `enroll.py --complete-list`).

For a source owning several functions, the report calls the placement mode
``derived``: each member is proven after fail-closed object isolation, and rombuild
places the matching derived sections together.  That is deliberately distinct from
an intact-object or partitioned text+data proof.
"""
import argparse
import collections
import concurrent.futures
import io
import json
import os
import pathlib
import re
import subprocess
import sys

from elftools.elf.elffile import ELFFile

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import build_pin as BP  # noqa: E402
import objisolate as OI  # noqa: E402
from enroll import candidates, CONFIG, REPO  # noqa: E402

MW = REPO / "tools" / "mwccarm"
LICENSE = MW / "license.dat"
INCLUDE = REPO / "include"
BUILD = REPO / "build"
from rombuild import VERSION  # noqa: E402  (default compiler)
# Imported, never copied: classifying with different flags than the build compiles with
# would pass files the build then breaks on.
from rombuild import CFLAGS  # noqa: E402

ANYSYM = re.compile(r"^(\S+)\s+kind:")
IGNORE_SECTIONS = (".comment", ".debug", ".line", ".note")


def placement_mode(symbols, isolate=True):
    """Name the object evidence being classified for one enrolled source."""
    symbols = list(symbols)
    if not isolate:
        return "raw"
    return "derived" if len(symbols) > 1 else "isolated"


def module_label(module_dir):
    """Stable public module label for an enroll.candidates() config directory."""
    rel = pathlib.Path(module_dir).relative_to(CONFIG)
    parts = rel.parts
    if parts == ("arm9",):
        return "arm9"
    if len(parts) == 2 and parts[0] == "arm9":
        return parts[1]              # itcm / dtcm
    if len(parts) == 3 and parts[:2] == ("arm9", "overlays"):
        return parts[2]              # ovNNN
    return "/".join(parts)


def load_report(path=None):
    """The eligibility report, as (entries, commit, dirty).

    Reads both the stamped shape and the bare list the report used to be, so a
    consumer does not care which vintage it is handed."""
    path = path or (BUILD / "rombuild-eligibility.json")
    d = json.loads(pathlib.Path(path).read_text(encoding="utf-8"))
    if isinstance(d, list):
        return d, None, None
    return d["files"], d.get("commit"), d.get("dirty")


def defined_symbols():
    """Every symbol name config/**/symbols.txt declares, in any module."""
    names = set()
    for sym in CONFIG.rglob("symbols.txt"):
        for line in sym.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = ANYSYM.match(line)
            if m:
                names.add(m.group(1))
    return names


def classify(job):
    rel, name, addr, size, sec, known, version, isolate = job
    src = REPO / rel
    # One object PER JOB, in this tool's own scratch root, for two reasons that both
    # bit in practice:
    #   1. A delinks entry can span several functions (src/func_01ff97d8.c covers five),
    #      so candidates() legitimately yields several jobs for ONE source file. Keyed
    #      by file alone they all compiled to the same -o path concurrently: mwccarm
    #      invocations failed on the locked/half-written file and objisolate mutated an
    #      object another job was still reading. That made the verdict depend on thread
    #      interleaving -- func_01ff97d8 flapped in and out of the pass list between
    #      back-to-back runs on an identical tree.
    #   2. build/<rel>.o is ALSO where rombuild.compile_one puts the real build's
    #      objects, so the classifier was clobbering the build tree it shares -- the
    #      "concurrent builds invent link errors" hazard. A separate root removes this
    #      tool from that collision surface entirely.
    # The suffix is the job's own symbol name (mangled identifiers are filesystem-safe),
    # so the path is unique per (file, function) and the verdict is the same one the
    # serialized run always produced. Appended ONLY when it differs from the file stem:
    # almost every file is named after its one symbol, and doubling a 97-character
    # mangled name pushed the path past Windows' 260-character limit -- mwccarm's -o
    # open failed and a real eligible function read as "compile failed".
    p = pathlib.Path(rel)
    fname = f"{p.stem}.o" if p.stem == name else f"{p.stem}.{name}.o"
    obj = BUILD / "eligible-scratch" / p.parent / fname
    obj.parent.mkdir(parents=True, exist_ok=True)
    flags = CFLAGS
    try:
        if src.read_text(encoding="utf-8", errors="ignore").startswith("//cpp"):
            flags = flags.replace("-lang c99", "-lang c++")
    except OSError:
        return rel, name, "unreadable source", []

    cmd = [*os.environ.get("MWCCARM_LAUNCHER", "").split(),
           str(MW / version / "mwccarm.exe"), *flags.split(),
           "-i", str(INCLUDE), "-c", str(src), "-o", str(obj)]
    r = subprocess.run(cmd, capture_output=True, text=True,
                       env=dict(os.environ, LM_LICENSE_FILE=str(LICENSE)), cwd=REPO)
    if r.returncode != 0 or not obj.is_file():
        return rel, name, "compile failed", []

    # `.init` is buildable now. mwccarm always emits compiled code into `.text`,
    # whatever the ROM's layout calls it, and dsd's linker script selects these by
    # name -- `File.o(.init)` -- so the names never matched and ~300 functions were
    # unreachable. rombuild.retarget_text_section renames the section header in the
    # object after compiling, in place and without moving a byte. Any OTHER section
    # is still a rejection: nothing renames those, so the lcf would not find them.
    if sec not in (".text", ".init"):
        return rel, name, f"lives in {sec}, not .text", []

    # A C++ destructor cannot be compiled alone: mwcc emits D0/D1/D2 plus the class
    # vtable and its RTTI into one object, because the Itanium ABI puts the vtable in
    # the TU defining the key function. That object is not placeable -- the lcf's
    # `File.o(.text)` matches all three code sections -- so it is reduced to the one
    # function this file declares before being judged. See tools/objisolate.py; 81
    # enrolled files sat at "extra sections: .data" for exactly this reason.
    #
    # Isolating HERE and in rombuild.compile_one both, from the same module, because a
    # classifier that judges a different object than the build compiles is worse than
    # no classifier: it passes files the build then breaks on. Same reason CFLAGS is
    # imported from rombuild rather than copied.
    try:
        # Inside the try: a malformed object -- a corrupt cache entry, a compiler that
        # emitted something unparseable -- must be one file's verdict, not a traceback
        # that ends the whole classification run.
        plan = OI.isolate(obj, name) if isolate else {}
        if plan.get("error") and plan.get("kind") != OI.NOT_A_FUNCTION:
            return rel, name, f"isolate: {plan['error']}", []

        elf = ELFFile(io.BytesIO(obj.read_bytes()))
        content = []
        for s in elf.iter_sections():
            h = s.header
            if h["sh_type"] not in ("SHT_PROGBITS", "SHT_NOBITS"):
                continue
            if h["sh_size"] == 0 or any(s.name.startswith(p) for p in IGNORE_SECTIONS):
                continue
            content.append(s.name)
        # Section size, not just symbol size: a padded .text would place extra bytes and
        # shift the module while st_size still looked right.
        text_sh_size = next((s.header["sh_size"] for s in elf.iter_sections()
                             if s.name == ".text" and s.header["sh_size"]), None)
        if len(content) != 1 or content[0] != ".text":
            extra = sorted(set(content) - {".text"})
            if len(content) > 1 and not extra:
                return rel, name, f"{len(content)} .text sections (multi-function TU)", []
            return rel, name, "extra sections: " + ",".join(extra or content), []

        symtab = elf.get_section_by_name(".symtab")
        defined, undefined, other_defined = [], [], []
        for s in symtab.iter_symbols():
            info = s["st_info"]
            # STB_LOPROC is mwcc's COMDAT binding, and it is how the OUT-OF-LINE
            # COPY OF AN INLINE FUNCTION arrives. A destructor defined in the class
            # body gets it -- which is the shape the ROM's own sources used, since
            # every derived destructor inlines its base's vptr store. Treating it as
            # invisible reported "0 defined global functions" for a file whose
            # symbol is defined, the right size, and in the kept section.
            #
            # It is deduplication metadata, not a weaker definition: exactly one
            # object in this build defines any given address, so there is nothing to
            # dedupe against. rombuild's byte compare is what stands behind that.
            if info["bind"] not in ("STB_GLOBAL", "STB_WEAK", "STB_LOPROC"):
                continue
            if s["st_shndx"] == "SHN_UNDEF":
                if s.name:
                    undefined.append(s.name)
            elif info["type"] == "STT_FUNC":
                defined.append((s.name, s["st_size"]))
            elif info["type"] == "STT_OBJECT":
                # A defined global object in the .text section would be placed too, and
                # the FUNC-only count above would never notice it.
                other_defined.append(s.name)

        if text_sh_size is not None and text_sh_size != size:
            return rel, name, f".text section 0x{text_sh_size:x} != declared 0x{size:x}", []
        if len(defined) != 1:
            return rel, name, f"{len(defined)} defined global functions", []
        dname, dsize = defined[0]
        if dname != name:
            return rel, name, f"defines {dname}, expected {name}", []
        if dsize != size:
            return rel, name, f"size 0x{dsize:x} != declared 0x{size:x}", []
        if other_defined:
            return rel, name, "defines non-function globals: " + ",".join(other_defined[:3]), []
        # Only the imports the kept function actually reaches. Isolation leaves dead
        # ones behind -- `_ZN4CoinD2Ev` (the ROM has no D2 variant at all) and the
        # `abi::__class_type_info` vtables the dropped RTTI records referenced -- and
        # rule 5's hazard is a weak gap-object import binding to 0, which needs a live
        # reference to bind. Nothing names these, so nothing can bind to them.
        # `live` is already "undefined AND reached by the kept function", so it is
        # used directly rather than intersected with `undefined`. The intersection
        # silently dropped the case that matters: `undefined` is collected from the
        # STB_GLOBAL/STB_WEAK loop above, so an isolated function-local static --
        # `table$8` and its guard in func_ov002_020bd664, both STB_LOCAL -- was never
        # in it, and the file passed rule 5 while referencing a symbol no ROM module
        # defines. It linked, and wrote the address of the function itself.
        live = (OI.referenced_undefined(obj.read_bytes(), name) if isolate
                else set(undefined))
        missing = sorted(live - known)
        if missing:
            # `reason` is truncated for the histogram a human reads; `missing` is the
            # complete list, because a consumer that acts on it needs all of them. A
            # tool fed only the first three silently leaves the rest behind.
            return rel, name, "unresolvable: " + ",".join(missing[:3]), missing
    except Exception as e:  # a malformed object is a fail, not a crash
        return rel, name, f"elf error: {type(e).__name__}", []
    return rel, name, None, []


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 8)
    # A/B switch for the isolation step, so its effect on the pass list is a
    # measurement rather than a claim. Not a build option: rombuild always
    # isolates, so classifying without it describes an object the build will
    # not produce.
    ap.add_argument("--no-isolate", action="store_true",
                    help="classify raw objects (comparison only, not what rombuild builds)")
    args = ap.parse_args()

    known = defined_symbols()
    cands, _ = candidates()
    # build_pin, not `versions().get(name, VERSION)`: rombuild.compile_one looks the
    # override up by FILE STEM, so keying this by symbol classifies a file under a
    # compiler the build would not use for it whenever the two spellings differ. They
    # coincide today, which is exactly why the divergence would go unnoticed until the
    # day they did not.
    jobs = [(rel, name, addr, size, sec, known, BP.version_for(rel, name) or VERSION,
             not args.no_isolate)
            for (_d, name, rel, addr, size, sec) in cands]
    # Do not key this by symbol alone. Overlays occupy the same address window and
    # may legitimately repeat a symbol spelling; the enrolled source path is the
    # disambiguator already carried through classify().
    identities = {(rel, name): (module_label(d), addr)
                  for (d, name, rel, addr, _size, _sec) in cands}
    members_by_source = collections.defaultdict(list)
    for _d, name, rel, _addr, _size, _sec in cands:
        members_by_source[rel].append(name)
    print(f"classifying {len(jobs)} enrolled functions with -j{args.jobs} ...")

    results, reasons = [], collections.Counter()
    done = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, name, reason, missing in ex.map(classify, jobs):
            module, addr = identities[(rel, name)]
            results.append({"file": rel, "name": name, "module": module,
                            "addr": f"0x{addr:08x}", "reason": reason,
                            "missing": missing,
                            "objectMode": placement_mode(
                                members_by_source[rel], not args.no_isolate),
                            "sourceMembers": members_by_source[rel]})
            reasons["ELIGIBLE" if reason is None else reason.split(":")[0]] += 1
            done += 1
            if done % 2000 == 0:
                print(f"  {done}/{len(jobs)}")

    BUILD.mkdir(exist_ok=True)
    # Stamp the tree this describes. A consumer that gates CI on this report must be
    # able to tell it apart from one left over from an earlier commit; without a stamp
    # a stale report fails open, which is the worst way for a gate to fail.
    try:
        head = subprocess.run(["git", "rev-parse", "HEAD"], cwd=REPO,
                              capture_output=True, text=True).stdout.strip() or None
        dirty = bool(subprocess.run(["git", "status", "--porcelain", "--untracked-files=no",
                                     "--", "src", "include", "config"], cwd=REPO,
                                    capture_output=True, text=True).stdout.strip())
    except Exception:
        head, dirty = None, True
    (BUILD / "rombuild-eligibility.json").write_text(json.dumps(
        {"commit": head, "dirty": dirty, "files": results}, indent=1))
    passed = [r["name"] for r in results if r["reason"] is None]
    (BUILD / "eligible-names.txt").write_text("\n".join(sorted(passed)) + "\n")

    print()
    for k, v in reasons.most_common():
        print(f"{v:7d}  {k}")
    print(f"\neligible: {len(passed)} / {len(jobs)}")
    print(f"wrote {BUILD / 'eligible-names.txt'}")


if __name__ == "__main__":
    main()
