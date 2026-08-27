"""Build a playable .nds from source.

Runs the whole ds-decomp link pipeline: every `src/` file enrolled in a
`config/**/delinks.txt` file entry is compiled with mwccarm and linked by mwldarm
into the module it belongs to; every address range NOT enrolled is supplied by a
delinked gap object carrying the original ROM bytes. The result is packaged back
into a bootable ROM.

    dsd delink   -> build/delinks/*.o      (gap objects, ROM bytes)
    mwccarm      -> build/src/*.o          (our C/C++; one or more enrolled functions)
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
    python tools/rombuild.py --partitioned-tu ov002/daObjAbuku_c
                                              # one proven merged TU, full production gates

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
import objisolate as OI  # noqa: E402
import rombuild_cache as RBK  # noqa: E402
import rombuild_check as RBC  # noqa: E402
import romdata_check as RDC  # noqa: E402
import layout_check as LAY  # noqa: E402
import rombuild_profile as RP  # noqa: E402
import srcpath as SP  # noqa: E402
import relocs as RL  # noqa: E402
import tu_manifest as TUM  # noqa: E402

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


def audit_version_pins(vers, srcs):
    """Check every `config/rombuild-versions.txt` pin still names a real source file.

    The pin is keyed by FILE STEM -- `compile_one` does
    `vers.get(pathlib.Path(rel).stem, VERSION)` -- so renaming a pinned function's
    file silently DETACHES its pin. The build then falls back to the default
    compiler and emits wrong bytes for a function whose source is perfectly
    correct.

    Nothing else in the tree can see that. `build_pin.verify`, `pr_linkcheck` and
    `eligible.py` all compile with the pin (build_pin.version_for reads this same
    table), so they keep reporting the function exact; only the full-module compare
    disagrees, by a handful of bytes, in one module, outside every per-file check.
    That is the most expensive shape of failure this pipeline can produce, and it
    cost a day on #1607. A stale pin is therefore a hard error, checked before the
    first compile.

    A pin whose file EXISTS but carries no `complete` is inert, not broken: the
    function is configured and waiting to be enrolled. That is reported, not failed.

    Returns (applied, inert) as sorted stem lists.
    """
    stems = {p.stem for root in ("src", "mods")
             for p in (REPO / root).rglob("*")
             if p.suffix in (".c", ".cpp") and p.is_file()}
    enrolled_stems = {pathlib.Path(s).stem for s in srcs}

    stale = sorted(k for k in vers if k not in stems)
    if stale:
        raise BuildError("preflight", 1,
                         "stale entry in config/rombuild-versions.txt -- no src/ or "
                         "mods/ file has this stem, so the pin no longer applies and "
                         "the build would silently use " + VERSION + " instead:\n  "
                         + "\n  ".join(stale)
                         + "\nIf the function was renamed, re-key its pin to "
                           "the new file stem in the same commit.")

    # The preflight only proves the DEFAULT compiler is installed. A pin naming a
    # service pack that is not present would otherwise fail deep in the compile with
    # a path error, or -- worse, on a tree where the pin had already detached -- not
    # at all.
    for version in sorted(set(vers.values())):
        exe = MW / version / "mwccarm.exe"
        if not exe.is_file():
            raise BuildError("preflight", 1,
                             f"config/rombuild-versions.txt pins {version}, but "
                             f"{exe} is missing - see notes/setup-mwccarm.md")

    applied = sorted(k for k in vers if k in enrolled_stems)
    inert = sorted(k for k in vers if k in stems and k not in enrolled_stems)
    return applied, inert


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


def enrolled(config_root=CONFIG_ROOT, extra_roots=()):
    """Every `src/` file carved out by a `complete` file entry in a delinks.txt.

    A file entry is an unindented path ending in ':'; the indented lines that follow
    hold `complete` and its section ranges. Without `complete`, dsd supplies the range
    from ROM bytes instead, so the file is configured but not yet source-built.

    `extra_roots` widens the set of allowed top-level source directories beyond
    `src/` and `mods/`, for a caller that has deliberately generated a scratch
    delinks tree naming sources elsewhere -- `tools/tubuild.py linkcheck` passes
    ("src_tu",) so a shadow TU can be linked without ever being written into the
    tracked config. It defaults to empty, so the production build's allowlist is
    unchanged and a tracked delinks.txt naming anything but src/ or mods/ still
    fails exactly as before. Every other safety check (no absolute path, no `..`,
    no escape from the repository, no symlink anywhere on the path) still applies.
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
                or pure.parts[0] not in ("src", "mods", *extra_roots)
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


def _isolate(obj, rel, syms, data_sink=None, compiler_only=None):
    """Reduce a compiled `src/` object to its declared function(s).

    A legacy source owns one function and takes the unchanged singular isolation path.
    A consolidated, text-only source owns several functions in ROM order and takes
    objisolate's separate fail-closed intact-object path. Returns an error or None.

    `mods/` is deliberately exempt. A mod is not a recovered ROM function: it may
    legitimately define helpers and data alongside its entry point, and isolation
    would strip them and externalise their symbols, which weak gap-object imports
    then bind to 0 at runtime. Silently producing a mod that jumps through null is
    far worse than a mod that fails to link, so mods keep the whole object.
    """
    if not rel.replace("\\", "/").startswith("src/"):
        return None
    if data_sink is not None:
        # THE ONLY MOMENT THIS IS POSSIBLE. The object still carries every `.data`
        # section mwcc emitted -- the vtable, the typeinfo record, the typeinfo name --
        # and `OI.isolate` on the next line zeroes all of them and rebinds their symbols
        # to the ROM's carved-out addresses. After that the bytes are gone from the
        # pipeline and the link compares the cartridge's own data against itself.
        #
        # Measurement only: check_object swallows its own exceptions, and nothing here
        # can fail the build. See tools/romdata_check.py for why it is not a gate.
        data_sink.extend(RDC.check_object(obj, rel))
    selected = (syms or {}).get(rel, pathlib.Path(rel).stem)
    if isinstance(selected, (list, tuple)):
        if not selected:
            return "source owns no enrolled functions"
        if len(selected) == 1:
            plan = OI.isolate(obj, selected[0])
        else:
            dead = (compiler_only or {}).get(rel.replace("\\", "/"), [])
            if dead:
                reduced, dead_plan = OI.derive_deadstrip(obj.read_bytes(), dead)
                if reduced is None:
                    return ("compiler-only deadstrip refused: "
                            + str(dead_plan.get("error")))
                obj.write_bytes(reduced)
            plan = OI.isolate_many(obj, selected)
    else:
        plan = OI.isolate(obj, selected)
    if plan.get("kind") == OI.NOT_A_FUNCTION:
        return None          # nothing to reduce; other gates judge this file
    return plan.get("error")


def _retarget(obj, rel, init_srcs):
    """retarget_text_section as a per-file verdict. Returns an error or None.

    The helper raises, which is right for a helper -- but these calls run under
    `ex.map`, where a raise escapes the result loop and ends the whole build with a
    traceback instead of failing the one file that provoked it. A malformed object
    is one source's problem; `eligible.py` reaches the same conclusion for the same
    reason, and the two now agree.
    """
    if not (init_srcs and rel in init_srcs):
        return None
    try:
        retarget_text_section(obj)
    except RuntimeError as e:
        return str(e)
    return None


def enrolled_symbols():
    """{rel: [symbols in ROM order]} for enrolled sources.

    objisolate needs every disjoint function definition a source emits, and the file
    stem is NOT that ownership contract. Grouping matters as soon as one consolidated
    source replaces several legacy files; a dict comprehension silently retained only
    the last member. Candidate ranges recognize the one exact legacy outer-owner alias
    shape and otherwise provide the linker/ROM order required by the fail-closed intact
    multi-function object path.
    """
    import enroll as E
    cands, _ = E.candidates()
    grouped = {}
    for _d, name, rel, addr, _size, _sec in cands:
        grouped.setdefault(rel, []).append((addr, _size, name))
    return {rel: _definition_symbols(rel, rows)
            for rel, rows in grouped.items()}


def compiler_only_policies(enrolled=None, manifest=None, homes=None):
    """Exact dead-strip allow-lists for promoted multi-function sources.

    The manifest is evidence, not a wildcard: every row must name one homeless,
    unlicensed function and explicitly request ``deadstrip`` with a reason.  Object
    surgery remains objisolate's job; this preflight only proves the checked-in policy
    cannot hide a function that has a retail address.
    """
    data = TUM.load() if manifest is None else manifest
    active = None if enrolled is None else {
        str(rel).replace("\\", "/") for rel in enrolled
    }
    homes = RL.load_symbol_homes() if homes is None else homes
    out, errors = {}, []
    for entry in data.get("entries", []):
        rows = entry.get("compiler_only_output") or []
        if not rows:
            continue
        source = str(entry.get("source", "")).replace("\\", "/")
        if not source:
            errors.append(f"{entry.get('id', '<unknown>')}: compiler-only policy has no source")
            continue
        if active is not None and source not in active:
            continue
        if source in out:
            errors.append(f"{source}: compiler-only policy is declared by multiple entries")
            continue
        licensed = {row.get("symbol") for row in entry.get("functions", [])}
        wanted = []
        for i, row in enumerate(rows):
            if not isinstance(row, dict):
                errors.append(f"{source}: compiler_only_output[{i}] is not an object")
                continue
            symbol = row.get("symbol")
            if not symbol:
                errors.append(f"{source}: compiler_only_output[{i}] has no symbol")
                continue
            if row.get("disposition") != "deadstrip":
                errors.append(f"{source}: {symbol} disposition must be deadstrip")
            if not str(row.get("reason", "")).strip():
                errors.append(f"{source}: {symbol} needs a non-empty reason")
            if symbol in licensed:
                errors.append(f"{source}: {symbol} is licensed by the manifest")
            if homes.get(symbol):
                errors.append(f"{source}: {symbol} has configured ROM home(s) "
                              f"{homes[symbol]}")
            if symbol in wanted:
                errors.append(f"{source}: duplicate compiler-only symbol {symbol}")
            wanted.append(symbol)
        out[source] = wanted
    if errors:
        raise BuildError("compiler-only policy", 1, "\n".join(errors))
    return out


def _definition_symbols(rel, rows):
    """Collapse owned records only for one exact legacy outer-owner alias shape.

    Some ROM functions contain independently named entry points.  The concrete case
    is ``src/unnamed/arm9/01ff/func_01ff97d8.c``: one 0xb6c outer function owns the complete range while
    four nonzero function records partition its interior.  Those records are useful
    address/ownership aliases, but mwcc emits only the outer definition.  Treating the
    five records as a consolidated TU would make the exact object fail with four
    invented missing-definition errors.

    There is exactly one safe exception to "every nonzero record needs a definition":
    the legacy source's filename stem names one candidate whose interval fully contains
    every other candidate interval.  That is direct evidence that the file's declared
    outer function owns nested address aliases, so only the outer definition is asked
    of the object.

    No widest-range or overlap heuristic is used.  A promoted TU filename does not name
    a function, and a stem owner beside any disjoint member is not the legacy alias-owner
    shape; both return every ROM-ordered record so ``isolate_many`` fails closed if the
    object does not define them.
    """
    return SP.definition_symbols(rel, rows)


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
    # Renaming the FIRST .text and returning is only correct while there is exactly
    # one. A multi-.text object -- any C++ destructor -- would get an arbitrary one
    # renamed, and dsd's `File.o(.init)` selector would then place whichever section
    # that happened to be. No .init candidate compiles to more than one .text today;
    # this fails loudly on the day one does rather than mislinking it.
    if len([n for n in names if n == ".text"]) > 1:
        raise RuntimeError(f"{obj}: {names.count('.text')} .text sections; "
                           f"cannot retarget to {section} unambiguously")
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


def compile_one(rel, vers=None, cache=None, init_srcs=None, syms=None, build_root=None,
                data_sink=None, prebuilt=None, compiler_only=None):
    """Compile one enrolled source file to the object path dsd's objects.txt names.

    Returns (rel, error-or-None, outcome), where outcome is how the object was
    obtained: "hit" straight from the cache, "miss" compiled and stored,
    "uncacheable" compiled but not storable, "tu-derived" copied from the
    manifest-backed partitioned-TU preparation, or "error" failed to compile.
    See rombuild_cache for why a hit reproduces the compile exactly.

    `build_root` must match the config's `build_path`, because that is what dsd's
    generated objects.txt names -- it defaults to this repository's `build/`, the
    only value the production build ever uses.
    """
    src = REPO / rel
    obj = (pathlib.Path(build_root) if build_root else BUILD) / pathlib.Path(rel).with_suffix(".o")
    obj.parent.mkdir(parents=True, exist_ok=True)
    if prebuilt is not None:
        source = pathlib.Path(prebuilt)
        if not source.is_file():
            return rel, f"prepared TU object is missing: {source}", "error"
        obj.write_bytes(source.read_bytes())
        return rel, None, "tu-derived"
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
            err = _retarget(obj, rel, init_srcs)
            if err:
                return rel, f"retarget: {err}", "error"
            err = _isolate(obj, rel, syms, data_sink, compiler_only)
            if err:
                return rel, f"isolate: {err}", "error"
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
        err = _retarget(obj, rel, init_srcs)
        if err:
            return rel, f"retarget: {err}", "error"
        if key is None:
            err = _isolate(obj, rel, syms, data_sink, compiler_only)
            return (rel, f"isolate: {err}", "error") if err else (rel, None, "miss")
        deps = cache.deps_from(scratch)
        if deps is None:
            err = _isolate(obj, rel, syms, data_sink, compiler_only)
            return (rel, f"isolate: {err}", "error") if err else (rel, None, "uncacheable")
        # Cache the RAW object, then isolate the working copy. Storing the reduced
        # form instead would bake this transformation into every entry, so any later
        # fix to it would be masked by isolate()'s own idempotence -- which is exactly
        # how the STB_LOPROC bug survived a rebuild and forced SCHEMA 2.
        cache.put(key, deps, obj)
        err = _isolate(obj, rel, syms, data_sink, compiler_only)
        return (rel, f"isolate: {err}", "error") if err else (rel, None, "miss")
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
    ap.add_argument("--no-data-check", action="store_true",
                    help="skip comparing emitted vtable/RTTI data against the ROM "
                         "(see tools/romdata_check.py); never affects the link")
    ap.add_argument("--arm7-bios", help="passed to dsd rom build if your dump needs it")
    ap.add_argument("--no-cache", action="store_true",
                    help="compile every enrolled file, ignoring the object cache")
    ap.add_argument("--cache-dir", default=os.environ.get("ROMBUILD_CACHE"),
                    help="object cache location (default build/objcache)")
    ap.add_argument("--cache-max-mb", type=int, default=1024,
                    help="prune the object cache back under this size after a build")
    ap.add_argument("--tu-module", action="append", default=[], metavar="MODULE",
                    help="build this module from its config_tu/ merged TUs instead of "
                         "the per-function src/ files (repeatable; e.g. --tu-module ov010)")
    ap.add_argument("--partitioned-tu", action="append", default=[], metavar="TU_ID",
                    help="production-build one partitioned-link-verified manifest TU: "
                         "compile it once, substitute its exact derived text objects, "
                         "and link its licensed reduced non-text object (repeatable)")
    ap.add_argument("--no-production-tus", action="store_true",
                    help="disable config/production-tus.json for a legacy-source control "
                         "build; explicit --partitioned-tu arguments still apply")
    args = ap.parse_args()
    if args.profile == "stock" and not args.no_production_tus:
        import tu_production as TP
        try:
            configured = TP.configured_ids()
        except TP.ProductionTuError as exc:
            ap.error(str(exc))
        args.partitioned_tu = list(dict.fromkeys(
            [*configured, *args.partitioned_tu]))
    if args.tu_module and args.partitioned_tu:
        ap.error("--tu-module and production partitioned TUs are mutually exclusive; "
                 "pass --no-production-tus for a config_tu experiment")
    if args.partitioned_tu and args.profile != "stock":
        ap.error("--partitioned-tu currently supports only the stock profile")
    if args.partitioned_tu and args.no_check:
        ap.error("--partitioned-tu cannot be combined with --no-check")

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

        profile = RP.prepare_profile(args.profile, tu_modules=args.tu_module)
        config_root = profile["configRoot"]
        config_yaml = profile["configYaml"]
        tu_prepared = None
        tu_overrides = {}
        if args.partitioned_tu:
            import tu_production as TP
            try:
                tu_prepared = TP.prepare(
                    args.partitioned_tu, config_root,
                    BUILD / "tu-production", jobs=args.jobs)
            except TP.ProductionTuError as exc:
                raise BuildError("partitioned TU preparation", 1, str(exc)) from exc
            tu_overrides = {
                rel.replace("\\", "/"): path
                for rel, path in tu_prepared["overrides"].items()
            }
            report["partitionedTus"] = TP.report_view(tu_prepared)
        report["profileConfig"] = str(config_root.relative_to(REPO))
        if profile.get("tuModules"):
            report["tuModules"] = profile["tuModules"]
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

        # A TU-built module's delinks name src_tu/ merged files; widen the allowed
        # source roots so those enroll like any other complete entry.
        extra_roots = ("src_tu",) if profile.get("tuModules") or tu_prepared else ()
        srcs = enrolled(config_root, extra_roots=extra_roots)
        vers = versions()
        # Before the first compile: a pin that no longer names a real file has quietly
        # stopped applying, and the only symptom downstream is a few wrong bytes in one
        # module that no per-file check can see.
        pins_applied, pins_inert = audit_version_pins(vers, srcs)
        n_alt = len(pins_applied)
        report["enrolledFiles"] = len(srcs)
        report["alternateToolchainFiles"] = n_alt
        # Named, not just counted. When a module compare disagrees with a per-file
        # check, the first question is which pins actually ran, and a count cannot
        # answer it -- see tools/validate_merge.py:_module_fidelity_detail.
        report["alternateToolchain"] = {
            "applied": {k: vers[k] for k in pins_applied},
            "inertNotEnrolled": {k: vers[k] for k in pins_inert}}
        cache = RBK.ObjectCache(args.cache_dir or (BUILD / "objcache"), REPO,
                                enabled=not args.no_cache)
        init_srcs = init_section_sources()
        syms = enrolled_symbols()
        compiler_only = compiler_only_policies(srcs)
        # Collected during the compile because that is the only point at which the
        # objects still carry the data mwcc emitted -- see _isolate. None switches the
        # measurement off entirely; it never affects what gets linked either way.
        data_sink = None if args.no_data_check else []
        print(f"[3/6] mwccarm: {len(srcs)} enrolled source file(s), -j{args.jobs}"
              + (f" ({n_alt} on an alternate toolchain version)" if n_alt else ""))
        for stem in pins_applied:
            print(f"      pinned {vers[stem]:<12} {stem}")
        for stem in pins_inert:
            print(f"      pinned {vers[stem]:<12} {stem}  (not enrolled - pin inert)")
        failures = []
        outcomes = {}
        if srcs:
            with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
                for rel, err, outcome in ex.map(
                        lambda s: compile_one(s, vers, cache, init_srcs, syms,
                                              data_sink=data_sink,
                                              compiler_only=compiler_only,
                                              prebuilt=tu_overrides.get(
                                                  s.replace("\\", "/"))), srcs):
                    outcomes[outcome] = outcomes.get(outcome, 0) + 1
                    if err:
                        failures.append((rel, err))
        if failures:
            detail = "\n".join(f"{rel}: {err}" for rel, err in failures[:10])
            raise BuildError("mwccarm", 1, detail)
        report["objectCache"] = cache.summary(outcomes)
        report["objectCache"]["tuDerived"] = outcomes.get("tu-derived", 0)
        if cache.enabled:
            print(f"      {outcomes.get('hit', 0)} reused from cache, "
                  f"{report['objectCache']['compiled']} compiled"
                  + (f", {outcomes.get('tu-derived', 0)} prepared from merged TU(s)"
                     if outcomes.get("tu-derived") else ""))
            if outcomes.get("miss") or outcomes.get("uncacheable"):
                cache.prune(args.cache_max_mb * 1024 * 1024)
        report["phases"].append("mwccarm")

        print("[4/6] mwldarm")
        run([*launcher(), str(MW / LD_VERSION / "mwldarm.exe"), *LDFLAGS.split(),
             f"@{BUILD / 'objects.txt'}", str(BUILD / "arm9.lcf"),
             "-o", str(BUILD / "final_link.o")], "mwldarm")
        report["phases"].append("mwldarm")

        if tu_prepared:
            verification = TP.verify_link(config_yaml, BUILD / "final_link.o", tu_prepared)
            report["partitionedTuLinkVerification"] = verification
            if not verification["ok"]:
                detail = []
                if not verification["modulesOk"]:
                    detail.append("dsd check modules --fail did not pass")
                detail.extend(f"new symbol error: {line}"
                              for line in verification["newSymbolErrors"])
                detail.extend(verification["storageAliasErrors"])
                raise BuildError("partitioned TU link verification", 1,
                                 "\n".join(detail) or "unknown verification failure")
            print("      partitioned TU gates: dsd modules PASS, zero new symbol "
                  "errors, storage aliases exact")

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
                rom_sha256 = hashlib.sha256(rom_path.read_bytes()).hexdigest()
                report["romArtifact"] = {
                    "path": str(rom_path),
                    "bytes": rom_path.stat().st_size,
                    "sha256": rom_sha256,
                }
                if tu_prepared:
                    expected = tu_prepared["baseline"]["romSha256"]
                    report["partitionedTuRom"] = {
                        "expectedSha256": expected,
                        "actualSha256": rom_sha256,
                        "identical": rom_sha256 == expected,
                    }
                    if rom_sha256 != expected:
                        raise BuildError(
                            "partitioned TU ROM comparison", 1,
                            f"built ROM sha256 {rom_sha256} differs from strict stock "
                            f"control {expected}")

        if args.no_check:
            print("[6/6] skipped (--no-check)")
            report["status"] = "unchecked"
            save_report()
            return 0
        print("[6/6] analyze module and source fidelity")
        analysis = RBC.analyze(config_root, args.profile)
        if data_sink is not None:
            # Folded in before printing, so the composition line can say how much of the
            # data it just called unreachable is nonetheless proven. Reported, never
            # gated: most of the tree fails this today for known modelling reasons, and
            # validate_merge ratchets the verified count instead.
            romdata = RDC.summarize(data_sink)
            report["romData"] = romdata
            analysis["moduleComposition"]["dataBytesVerified"] = romdata["verifiedBytes"]
        RBC.print_report(analysis)
        if data_sink is not None:
            rd = report["romData"]
            print(f"ROM data from source: {rd['verified']:,} symbol(s) verified, "
                  f"{rd['partial']:,} partial, {rd['differs']:,} differ, "
                  f"{rd['unnamed']:,} unnamed by config")
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
