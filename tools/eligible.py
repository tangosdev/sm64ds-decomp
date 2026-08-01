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
  4. the function lives in a `.text` range, not `.init`;
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
from enroll import candidates, CONFIG, REPO  # noqa: E402
from rombuild import versions  # noqa: E402

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
    rel, name, addr, size, sec, known, version = job
    src = REPO / rel
    obj = BUILD / pathlib.Path(rel).with_suffix(".o")
    obj.parent.mkdir(parents=True, exist_ok=True)
    flags = CFLAGS
    try:
        if src.read_text(encoding="utf-8", errors="ignore").startswith("//cpp"):
            flags = flags.replace("-lang c99", "-lang c++")
    except OSError:
        return rel, name, "unreadable source"

    cmd = [*os.environ.get("MWCCARM_LAUNCHER", "").split(),
           str(MW / version / "mwccarm.exe"), *flags.split(),
           "-i", str(INCLUDE), "-c", str(src), "-o", str(obj)]
    r = subprocess.run(cmd, capture_output=True, text=True,
                       env=dict(os.environ, LM_LICENSE_FILE=str(LICENSE)), cwd=REPO)
    if r.returncode != 0 or not obj.is_file():
        return rel, name, "compile failed"

    if sec != ".text":
        return rel, name, f"lives in {sec}, not .text"

    try:
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
                return rel, name, f"{len(content)} .text sections (multi-function TU)"
            return rel, name, "extra sections: " + ",".join(extra or content)

        symtab = elf.get_section_by_name(".symtab")
        defined, undefined, other_defined = [], [], []
        for s in symtab.iter_symbols():
            info = s["st_info"]
            if info["bind"] not in ("STB_GLOBAL", "STB_WEAK"):
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
            return rel, name, f".text section 0x{text_sh_size:x} != declared 0x{size:x}"
        if len(defined) != 1:
            return rel, name, f"{len(defined)} defined global functions"
        dname, dsize = defined[0]
        if dname != name:
            return rel, name, f"defines {dname}, expected {name}"
        if dsize != size:
            return rel, name, f"size 0x{dsize:x} != declared 0x{size:x}"
        if other_defined:
            return rel, name, "defines non-function globals: " + ",".join(other_defined[:3])
        missing = sorted(set(undefined) - known)
        if missing:
            return rel, name, "unresolvable: " + ",".join(missing[:3])
    except Exception as e:  # a malformed object is a fail, not a crash
        return rel, name, f"elf error: {type(e).__name__}"
    return rel, name, None


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 8)
    args = ap.parse_args()

    known = defined_symbols()
    cands, _ = candidates()
    vers = versions()
    jobs = [(rel, name, addr, size, sec, known, vers.get(name, VERSION))
            for (_d, name, rel, addr, size, sec) in cands]
    print(f"classifying {len(jobs)} enrolled functions with -j{args.jobs} ...")

    results, reasons = [], collections.Counter()
    done = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, name, reason in ex.map(classify, jobs):
            results.append({"file": rel, "name": name, "reason": reason})
            reasons["ELIGIBLE" if reason is None else reason.split(":")[0]] += 1
            done += 1
            if done % 2000 == 0:
                print(f"  {done}/{len(jobs)}")

    BUILD.mkdir(exist_ok=True)
    (BUILD / "rombuild-eligibility.json").write_text(json.dumps(results, indent=1))
    passed = [r["name"] for r in results if r["reason"] is None]
    (BUILD / "eligible-names.txt").write_text("\n".join(sorted(passed)) + "\n")

    print()
    for k, v in reasons.most_common():
        print(f"{v:7d}  {k}")
    print(f"\neligible: {len(passed)} / {len(jobs)}")
    print(f"wrote {BUILD / 'eligible-names.txt'}")


if __name__ == "__main__":
    main()
