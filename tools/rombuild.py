"""Build a playable .nds from source.

Runs the whole ds-decomp link pipeline: every `src/` file enrolled in a
`config/**/delinks.txt` file entry is compiled with mwccarm and linked by mwldarm
into the module it belongs to; every address range NOT enrolled is supplied by a
delinked gap object carrying the original ROM bytes. The result is packaged back
into a bootable ROM.

    dsd delink   -> build/delinks/*.o      (gap objects, ROM bytes)
    mwccarm      -> build/src/*.o          (our C, one function per file)
    dsd lcf      -> build/arm9.lcf + build/objects.txt
    mwldarm      -> build/final_link.o     + build/build/*.bin per region
    dsd rom config / rom build             -> build/sm64ds.nds
    rombuild_check.py                      -> byte-diff every module vs the ROM

A module is "green" when its linked bytes equal the retail module, so a green build
with N functions enrolled is proof those N functions' source is the real thing.

Usage:
    python tools/rombuild.py                 # stock ROM build + verify
    python tools/rombuild.py --profile mods  # explicitly include mods/
    python tools/rombuild.py --no-rom        # link and verify, skip .nds packaging
    python tools/rombuild.py --no-check      # skip fidelity analysis (unchecked output)
    python tools/rombuild.py -j 16           # parallel compiles
    python tools/rombuild.py --no-cache      # recompile everything, ignore build/objcache

Compiled objects are cached by content under `build/objcache`, so an incremental
build only runs mwccarm for the files a change actually reaches -- see
tools/rombuild_cache.py for the key and why it is safe to trust.

See notes/rom-build.md for the milestones and the enrollment rules.
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile

REPO = pathlib.Path(__file__).resolve().parent.parent
DSD = REPO / "tools" / "bin" / "dsd.exe"
MW = REPO / "tools" / "mwccarm"
LICENSE = MW / "license.dat"
INCLUDE = REPO / "include"
CONFIG_ROOT = REPO / "config" / "arm9"
BUILD = REPO / "build"
sys.path.insert(0, str(REPO / "tools"))
import rombuild_cache as RBK  # noqa: E402
import rombuild_check as RBC  # noqa: E402
import layout_check as LAY  # noqa: E402
import rombuild_profile as RP  # noqa: E402

# Default compiler for the ROM build — same as the matching pin (tools/match.py
# CANONICAL / notes/rom-build.md). config/rombuild-versions.txt carries per-file
# exceptions that still need a 1.2/* service pack.
VERSION = "2004/b56"
# 2004/b56 ships only mwccarm.exe - there is no mwldarm in it - so the link always
# uses the 1.2/sp2p3 linker regardless of which compiler produced the objects.
LD_VERSION = "1.2/sp2p3"
# tools/match.py's DEFAULT_FLAGS, plus one addition the match gate never needed.
# `-Cpp_exceptions off` suppresses the .exception/.exceptix unwind tables mwccarm
# otherwise emits alongside almost every function. The match gate only ever compared
# .text, so it never saw them; a ROM link does, and those sections would be duplicate
# content that grows the module and shifts every later address. Retail carries just
# 636 bytes of .exceptix in main, so the original build had them off too. Verified
# not to perturb .text codegen (see notes/rom-build.md, M2b).
CFLAGS = ("-O4,p -enum int -lang c99 -char signed -interworking "
          "-proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off")
LDFLAGS = ("-proc arm946e -nostdlib -interworking -m Entry "
           "-map closure,unused -msgstyle gcc -nodead")


VERSIONS_FILE = REPO / "config" / "rombuild-versions.txt"


def versions():
    """symbol -> mwccarm version, for functions not matched under the default.

    A function counts as matched if *some* toolchain version reproduces it, and
    match.py sweeps a list - so a single-version link produces wrong bytes for the
    ones matched elsewhere, even though the source is right. See the file's comments.
    """
    out = {}
    if VERSIONS_FILE.is_file():
        for line in VERSIONS_FILE.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if line and not line.startswith("#"):
                parts = line.split()
                if len(parts) == 2:
                    out[parts[0]] = parts[1]
    return out


def launcher():
    """Wine prefix on the Linux build box; empty on native Windows (see match.py)."""
    return os.environ.get("MWCCARM_LAUNCHER", "").split()


def cpu_quota():
    """Whole CPUs this process's cgroup is allowed, or None if uncapped."""
    # cgroup v2 keeps "<quota|max> <period>" in one file; v1 splits it across two.
    for quota_path, period_path in (
            ("/sys/fs/cgroup/cpu.max", None),
            ("/sys/fs/cgroup/cpu/cpu.cfs_quota_us", "/sys/fs/cgroup/cpu/cpu.cfs_period_us")):
        try:
            fields = pathlib.Path(quota_path).read_text().split()
            period = (fields[1] if period_path is None
                      else pathlib.Path(period_path).read_text().split()[0])
            if fields[0] == "max" or int(fields[0]) <= 0:
                return None
            return max(1, int(fields[0]) // int(period))
        except (OSError, ValueError, IndexError, ZeroDivisionError):
            continue
    return None


def default_jobs():
    """How many compiles to run at once.

    os.cpu_count() reports the host's cores and is blind to a cgroup quota, so in a
    CPU-capped container it scales with hardware the build cannot use -- harmless at
    the validator's 4-core box, pathological on a big host with a small quota.

    The cap is twice the quota rather than the quota itself. These compiles are not
    CPU-bound end to end: each one spawns a Wine process and talks to wineserver, and
    a thread parked in that startup leaves quota unspent. Running only as many threads
    as whole CPUs measurably underuses the allowance, so the default oversubscribes
    enough to keep it busy -- which is also what the previous host-core default
    happened to give this box. `-j` still overrides for anyone tuning it.
    """
    jobs = os.cpu_count() or 8
    try:
        jobs = min(jobs, len(os.sched_getaffinity(0)))
    except (AttributeError, OSError):
        pass
    quota = cpu_quota()
    if quota is not None:
        jobs = min(jobs, 2 * quota)
    return max(1, jobs)


class BuildError(RuntimeError):
    def __init__(self, phase, returncode, output):
        super().__init__(f"{phase} failed (exit {returncode})")
        self.phase = phase
        self.returncode = returncode
        self.output = output


def run(cmd, what, quiet_patterns=()):
    r = subprocess.run(cmd, capture_output=True, text=True,
                       env=dict(os.environ, LM_LICENSE_FILE=str(LICENSE)), cwd=REPO)
    out = "\n".join(l for l in (r.stdout + r.stderr).splitlines()
                    if not any(p in l for p in quiet_patterns))
    if r.returncode != 0:
        raise BuildError(what, r.returncode, out)
    return out


def enrolled(config_root=CONFIG_ROOT):
    """Every `src/` file carved out by a `complete` file entry in a delinks.txt.

    A file entry is an unindented path ending in ':'; the indented lines that follow
    hold `complete` and its section ranges. Without `complete`, dsd supplies the range
    from ROM bytes instead, so the file is configured but not yet source-built.
    """
    files, path, saw_complete = [], None, False
    for delinks in sorted(pathlib.Path(config_root).rglob("delinks.txt")):
        for line in delinks.read_text(encoding="utf-8").splitlines():
            if not line.strip():
                continue
            if not line[0].isspace():
                if path and saw_complete:
                    files.append(path)
                path = line.strip().rstrip(":") if line.rstrip().endswith(":") else None
                saw_complete = False
            elif line.strip() == "complete":
                saw_complete = True
        if path and saw_complete:
            files.append(path)
        path, saw_complete = None, False
    checked = []
    for rel in sorted(set(files)):
        pure = pathlib.PurePosixPath(rel.replace("\\", "/"))
        if (pure.is_absolute() or ".." in pure.parts or len(pure.parts) < 2
                or pure.parts[0] not in ("src", "mods")
                or pure.suffix not in (".c", ".cpp")):
            raise BuildError("profile", 1, f"unsafe complete delinks source path: {rel}")
        raw_target = REPO / pathlib.Path(*pure.parts)
        target = raw_target.resolve()
        try:
            target.relative_to(REPO.resolve())
        except ValueError:
            raise BuildError("profile", 1, f"source path escapes repository: {rel}")
        path_parts = (raw_target, *raw_target.parents)
        crosses_symlink = any(
            part.is_symlink() for part in path_parts
            if part != REPO and REPO in part.parents
        )
        if not target.is_file() or crosses_symlink:
            raise BuildError("profile", 1, f"missing or symlinked complete source: {rel}")
        checked.append(pure.as_posix())
    return checked


def init_section_sources():
    """{rel} for enrolled sources whose function lives in .init rather than .text.

    dsd's linker script selects each object's code by section name -- `File.o(.init)`
    for these -- but mwccarm always emits compiled code into `.text`, whatever the
    ROM's own layout calls it. The names never match, so ~300 otherwise-eligible
    functions, almost all __sinit_* static initialisers, could not be built from
    source at all.
    """
    import enroll as E
    cands, _ = E.candidates()
    return {rel for (_d, _name, rel, _addr, _size, sec) in cands if sec == ".init"}


def retarget_text_section(obj, section=".init"):
    """Rename an object's `.text` section header to `section`, in place.

    Only the section-header string is touched. `.text` and `.init` are both five
    bytes, so the entry is overwritten where it sits and nothing in the file moves.
    That matters: rewriting the ELF would risk perturbing the very bytes this build
    exists to compare against the ROM.

    Safe because mwccarm's .shstrtab stores `.text` as its own entry rather than as a
    suffix of `.rela.text` -- they sit at distinct offsets -- so the overwrite cannot
    corrupt the relocation section's name. `.rela.text` keeps its own name and finds
    its target through sh_info, a section index, which is unaffected.

    Idempotent: an object already carrying `section` is left alone, so a cache entry
    written before this existed gets corrected rather than served stale.
    """
    import io
    from elftools.elf.elffile import ELFFile

    raw = bytearray(obj.read_bytes())
    elf = ELFFile(io.BytesIO(bytes(raw)))
    names = [s.name for s in elf.iter_sections()]
    if section in names:
        return False                          # already retargeted
    base = elf.get_section(elf["e_shstrndx"]).header["sh_offset"]
    want = b".text\x00"
    for s in elf.iter_sections():
        if s.name != ".text":
            continue
        off = base + s.header["sh_name"]
        if bytes(raw[off:off + len(want)]) != want:
            raise RuntimeError(f"{obj}: .shstrtab entry at {off} is not '.text'")
        raw[off:off + len(section)] = section.encode()
        obj.write_bytes(bytes(raw))
        return True
    return False


def compile_one(rel, vers=None, cache=None, init_srcs=None):
    """Compile one enrolled source file to the object path dsd's objects.txt names.

    Returns (rel, error-or-None, outcome), where outcome is how the object was
    obtained: "hit" straight from the cache, "miss" compiled and stored,
    "uncacheable" compiled but not storable, "error" failed to compile. See
    rombuild_cache for why a hit reproduces the compile exactly.
    """
    src = REPO / rel
    obj = BUILD / pathlib.Path(rel).with_suffix(".o")
    obj.parent.mkdir(parents=True, exist_ok=True)
    version = (vers or {}).get(pathlib.Path(rel).stem, VERSION)
    flags = CFLAGS
    # A leading //cpp marker means C++, matched to how match.py/fdiff compile it.
    try:
        if src.read_text(encoding="utf-8").startswith("//cpp"):
            flags = flags.replace("-lang c99", "-lang c++")
    except OSError:
        pass

    exe = MW / version / "mwccarm.exe"
    key = (cache.source_key(rel, version, flags, exe)
           if cache and cache.enabled else None)
    if key is not None:
        deps = cache.manifest(key)
        if deps is not None and cache.fetch(cache.object_key(key, deps), obj):
            if init_srcs and rel in init_srcs:
                retarget_text_section(obj)
            return rel, None, "hit"

    # -MD makes mwccarm write out the headers it actually read, which is what lets the
    # next build rebuild only the files a changed header reaches. It writes that .d
    # into the working directory under the source's name, so misses run in a scratch
    # directory rather than littering (and racing in) the repository root. Neither the
    # flag nor the working directory perturbs the object bytes -- both were verified
    # byte-identical against a plain cwd=REPO compile for C and C++ sources.
    scratch = None
    if key is not None:
        try:
            scratch = tempfile.mkdtemp(dir=str(cache.scratch))
        except OSError:
            key = None  # cache directory went away; compile as if it were disabled
    cmd = [*launcher(), str(exe), *flags.split()]
    if key is not None:
        cmd.append("-MD")
    cmd += ["-i", str(INCLUDE), "-c", str(src), "-o", str(obj)]
    try:
        r = subprocess.run(cmd, capture_output=True, text=True,
                           env=dict(os.environ, LM_LICENSE_FILE=str(LICENSE)),
                           cwd=scratch or REPO)
        if r.returncode != 0 or not obj.is_file():
            detail = "\n".join(s for s in (r.stdout.strip(), r.stderr.strip()) if s)
            return rel, detail[:400], "error"
        # Before caching, so the stored object already carries the right section name
        # and a later hit needs no fixup.
        if init_srcs and rel in init_srcs:
            retarget_text_section(obj)
        if key is None:
            return rel, None, "miss"
        deps = cache.deps_from(scratch)
        if deps is None:
            return rel, None, "uncacheable"
        cache.put(key, deps, obj)
        return rel, None, "miss"
    finally:
        if scratch:
            shutil.rmtree(scratch, ignore_errors=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("-j", "--jobs", type=int, default=default_jobs())
    ap.add_argument("--profile", choices=RP.PROFILES, default="stock",
                    help="stock (default, verified src only) or mods (intentional divergences)")
    ap.add_argument("--rom-out", default=None,
                    help="output ROM (default build/sm64ds.nds or build/sm64ds-mod.nds)")
    ap.add_argument("--report-json",
                    help="structured report (default follows the selected profile)")
    ap.add_argument("--no-rom", action="store_true", help="stop after linking")
    ap.add_argument("--no-layout-check", action="store_true",
                    help="skip the src/ layout invariant gate (see tools/layout_check.py)")
    ap.add_argument("--no-check", action="store_true",
                    help="skip module/source fidelity analysis; report status is unchecked")
    ap.add_argument("--arm7-bios", help="passed to dsd rom build if your dump needs it")
    ap.add_argument("--no-cache", action="store_true",
                    help="compile every enrolled file, ignoring the object cache")
    ap.add_argument("--cache-dir", default=os.environ.get("ROMBUILD_CACHE"),
                    help="object cache location (default build/objcache)")
    ap.add_argument("--cache-max-mb", type=int, default=1024,
                    help="prune the object cache back under this size after a build")
    args = ap.parse_args()

    report_path = pathlib.Path(args.report_json or
                               BUILD / ("rombuild-report.json" if args.profile == "stock"
                                        else "rombuild-mod-report.json"))
    rom_out = args.rom_out or str(BUILD / ("sm64ds.nds" if args.profile == "stock"
                                          else "sm64ds-mod.nds"))
    report = {"schemaVersion": 1, "profile": args.profile, "status": "running",
              "romOut": rom_out, "phases": []}

    def save_report():
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(json.dumps(report, indent=2) + "\n",
                               encoding="utf-8", newline="\n")

    try:
        for tool in (DSD, MW / VERSION / "mwccarm.exe", MW / LD_VERSION / "mwldarm.exe"):
            if not tool.is_file():
                raise BuildError("preflight", 1,
                                 f"missing {tool} - see notes/setup-mwccarm.md")
        if not (REPO / "extracted" / "dsd" / "config.yaml").is_file():
            raise BuildError("preflight", 1,
                             "no extracted ROM - run tools/unpack.py on your own dump first")

        # Layout gate, before the compile rather than after: a delinks.txt naming a path
        # with no file there does NOT fail this build. dsd fills that range with retail
        # ROM bytes, so the result is still byte-identical and every check below stays
        # green -- the only trace is the source-built function count quietly dropping.
        # That is the one class of breakage this pipeline cannot self-detect, so it is
        # checked up front where it costs a second instead of a full link.
        if not args.no_layout_check:
            layout = LAY.check()
            report["layout"] = {k: len(v) for k, v in layout.items()}
            if any(layout[c] for c in LAY.ERRORS):
                LAY.print_report(layout, quiet=True)
                raise BuildError("layout", 1,
                                 "src/ layout invariants violated - see tools/layout_check.py. "
                                 "Re-run tools/enroll.py if files moved; add a documented "
                                 "waiver to config/layout-known-issues.txt only if the "
                                 "violation is genuinely pre-existing.")

        profile = RP.prepare_profile(args.profile)
        config_root = profile["configRoot"]
        config_yaml = profile["configYaml"]
        report["profileConfig"] = str(config_root.relative_to(REPO))
        report["modReplacements"] = profile["modReplacements"]
        report["modGapFallbacks"] = profile["modGapFallbacks"]

        # Gap objects first: delinks.txt drives which ranges dsd carves out for us.
        print(f"profile: {args.profile} ({len(profile['modReplacements'])} mod source replacement(s), "
              f"{len(profile['modGapFallbacks'])} ROM-gap fallback(s))")
        print("[1/6] dsd delink")
        run([str(DSD), "delink", "-c", str(config_yaml)], "dsd delink",
            quiet_patterns=("No module for relocation",))
        report["phases"].append("dsd delink")

        print("[2/6] dsd lcf")
        run([str(DSD), "lcf", "-c", str(config_yaml)], "dsd lcf")
        report["phases"].append("dsd lcf")

        srcs = enrolled(config_root)
        vers = versions()
        n_alt = sum(1 for s in srcs if pathlib.Path(s).stem in vers)
        report["enrolledFiles"] = len(srcs)
        report["alternateToolchainFiles"] = n_alt
        cache = RBK.ObjectCache(args.cache_dir or (BUILD / "objcache"), REPO,
                                enabled=not args.no_cache)
        init_srcs = init_section_sources()
        print(f"[3/6] mwccarm: {len(srcs)} enrolled source file(s), -j{args.jobs}"
              + (f" ({n_alt} on an alternate toolchain version)" if n_alt else ""))
        failures = []
        outcomes = {}
        if srcs:
            with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
                for rel, err, outcome in ex.map(
                        lambda s: compile_one(s, vers, cache, init_srcs), srcs):
                    outcomes[outcome] = outcomes.get(outcome, 0) + 1
                    if err:
                        failures.append((rel, err))
        if failures:
            detail = "\n".join(f"{rel}: {err}" for rel, err in failures[:10])
            raise BuildError("mwccarm", 1, detail)
        report["objectCache"] = cache.summary(outcomes)
        if cache.enabled:
            print(f"      {outcomes.get('hit', 0)} reused from cache, "
                  f"{report['objectCache']['compiled']} compiled")
            if outcomes.get("miss") or outcomes.get("uncacheable"):
                cache.prune(args.cache_max_mb * 1024 * 1024)
        report["phases"].append("mwccarm")

        print("[4/6] mwldarm")
        run([*launcher(), str(MW / LD_VERSION / "mwldarm.exe"), *LDFLAGS.split(),
             f"@{BUILD / 'objects.txt'}", str(BUILD / "arm9.lcf"),
             "-o", str(BUILD / "final_link.o")], "mwldarm")
        report["phases"].append("mwldarm")

        if args.no_rom:
            print("[5/6] skipped (--no-rom)")
        else:
            print("[5/6] dsd rom config + rom build")
            run([str(DSD), "rom", "config", "--elf", str(BUILD / "final_link.o"),
                 "--config", str(config_yaml)], "dsd rom config")
            cmd = [str(DSD), "rom", "build", "--config",
                   str(BUILD / "build" / "rom_config.yaml"), "--rom", rom_out]
            if args.arm7_bios:
                cmd += ["--arm7-bios", args.arm7_bios]
            run(cmd, "dsd rom build", quiet_patterns=("Compressing arm9 overlay",))
            print(f"      -> {rom_out}")
            report["phases"].append("dsd rom build")
            rom_path = pathlib.Path(rom_out)
            if not rom_path.is_absolute():
                rom_path = REPO / rom_path
            if rom_path.is_file():
                report["romArtifact"] = {
                    "path": str(rom_path),
                    "bytes": rom_path.stat().st_size,
                    "sha256": hashlib.sha256(rom_path.read_bytes()).hexdigest(),
                }

        if args.no_check:
            print("[6/6] skipped (--no-check)")
            report["status"] = "unchecked"
            save_report()
            return 0
        print("[6/6] analyze module and source fidelity")
        analysis = RBC.analyze(config_root, args.profile)
        RBC.print_report(analysis)
        report["analysis"] = analysis
        report["status"] = "passed" if analysis["passed"] else "failed"
        save_report()
        return 0 if analysis["passed"] else 1
    except (BuildError, RP.ProfileError) as exc:
        phase = exc.phase if isinstance(exc, BuildError) else "profile"
        output = exc.output if isinstance(exc, BuildError) else str(exc)
        code = exc.returncode if isinstance(exc, BuildError) else 1
        print(f"!! {phase} failed (exit {code})")
        print(output[:4000])
        report["status"] = "error"
        report["failure"] = {"phase": phase, "returncode": code, "output": output[:4000]}
        save_report()
        return 1


if __name__ == "__main__":
    sys.exit(main())
