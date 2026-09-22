"""Generate isolated ROM-build configs for stock and intentional-mod profiles.

Tracked ``config/**/delinks.txt`` records the project's working enrollment state.  It
may point at ``mods/<symbol>.c`` so an experiment is convenient to run, but that must
not make the default correctness build differ from the retail ROM.  This module copies
the ARM9 config tree under ignored ``build/rombuild-config/`` and, for the stock
profile, redirects every mod entry to the matching verified ``src/`` translation unit.
If no verified counterpart exists, the generated entry is demoted so dsd fills the
range with original ROM bytes.

Nothing under ``config/`` or ``mods/`` is changed.  The generated tree is disposable
and can be handed directly to dsd, rombuild.py, and rombuild_check.py.
"""
import json
import os
import pathlib
import re
import shutil
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
SOURCE_CONFIG = REPO / "config" / "arm9"
CONFIG_TU = REPO / "config_tu" / "arm9"
BUILD = REPO / "build"
sys.path.insert(0, str(REPO / "tools"))
import asm_policy  # noqa: E402
import srcpath as SP  # noqa: E402

PROFILES = ("stock", "mods")
PATH_LINE = re.compile(r"^(\s*(?:rom_config|build_path|delinks_path|object):\s+)(\S+)(\s*)$")
CONFIG_REF_LINE = re.compile(r"^\s*(delinks|symbols|relocations):\s+(\S+)\s*$")


class ProfileError(RuntimeError):
    pass


def _rewrite_config_paths(path, source_root, generated_root,
                          source_build=None, target_build=None):
    """Retarget paths that were relative to config/arm9 to the generated tree.

    ``source_build``/``target_build`` additionally re-root every path that pointed
    inside the repository's ``build/`` at a different build directory, so a caller
    can run a completely isolated link whose delink objects, module images, linker
    script and object list never touch the shared ``build/`` outputs.  Both default
    to None, which leaves the current behaviour (targets preserved, only the
    relative spelling changes) exactly as it was.
    """
    lines = []
    for line in path.read_text(encoding="utf-8").splitlines():
        m = PATH_LINE.match(line)
        if m and m.group(2).startswith("."):
            target = (source_root / m.group(2)).resolve()
            if source_build is not None and target_build is not None:
                try:
                    inner = target.relative_to(pathlib.Path(source_build).resolve())
                except ValueError:
                    pass
                else:
                    target = (pathlib.Path(target_build).resolve() / inner)
            rel = pathlib.Path(os.path.relpath(target, generated_root)).as_posix()
            line = f"{m.group(1)}{rel}{m.group(3)}"
        lines.append(line)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def _inside(path, root):
    try:
        path.resolve().relative_to(root.resolve())
        return True
    except ValueError:
        return False


def _validate_generated_config(path, generated_root, repo, build):
    """Reject config paths that could make dsd read or write outside known roots."""
    expected = {
        "rom_config": repo / "extracted" / "dsd" / "config.yaml",
        "build_path": build,
        "delinks_path": build / "delinks",
    }
    for line in path.read_text(encoding="utf-8").splitlines():
        m = PATH_LINE.match(line)
        if m:
            key = m.group(1).strip().rstrip(":")
            target = (generated_root / m.group(2)).resolve()
            if key in expected and target != expected[key].resolve():
                raise ProfileError(f"unsafe {key} path in config.yaml: {target}")
            if key == "object" and not _inside(target, build / "build"):
                raise ProfileError(f"unsafe module object path in config.yaml: {target}")
            continue
        ref = CONFIG_REF_LINE.match(line)
        if ref:
            target = (generated_root / ref.group(2)).resolve()
            if not _inside(target, generated_root):
                raise ProfileError(f"unsafe {ref.group(1)} path in config.yaml: {target}")


def _stock_delinks(generated_root, repo):
    """Keep every mod out of a stock link.

    Prefer its verified ``src/`` counterpart. If one does not exist (or is explicitly
    NONMATCHING), remove ``complete`` from the disposable entry so dsd supplies that
    range from retail gap bytes instead.
    """
    saved = SP.set_root(repo)
    replacements, gap_fallbacks = [], []
    try:
        for path in sorted(generated_root.rglob("delinks.txt")):
            out = []
            changed = False
            demote_current = False
            for line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
                stripped = line.strip()
                if line and not line[0].isspace() and stripped.startswith("mods/") \
                        and stripped.endswith(":"):
                    demote_current = False
                    mod_rel = stripped[:-1]
                    name = pathlib.PurePosixPath(mod_rel).stem
                    src = SP.path_for(name)
                    verified = (src is not None and
                                not asm_policy.has_draft_banner(src.read_text(
                                    encoding="utf-8", errors="ignore")))
                    if verified:
                        src_rel = src.relative_to(repo).as_posix()
                        line = f"{src_rel}:"
                        replacements.append(
                            {"name": name, "modPath": mod_rel, "srcPath": src_rel})
                        changed = True
                    else:
                        demote_current = True
                        gap_fallbacks.append({"name": name, "modPath": mod_rel,
                                              "reason": "missing verified src counterpart"})
                elif line and not line[0].isspace():
                    demote_current = False
                elif demote_current and stripped == "complete":
                    changed = True
                    continue
                out.append(line)
            if changed:
                path.write_text("\n".join(out) + "\n", encoding="utf-8", newline="\n")
    finally:
        SP.set_root(saved[0])
    return replacements, gap_fallbacks


def _module_delinks_rel(module):
    """The delinks.txt path for a module id, relative to a config-arm9 root.

    Overlays live under ``overlays/<id>/``; the main module is the root itself;
    the ITCM/DTCM autoloads are their own subdirectory. This spelling is shared by
    the tracked ``config/arm9`` tree, the generated profile tree, and ``config_tu``.
    """
    if module == "main":
        return pathlib.PurePosixPath("delinks.txt")
    if module in ("itcm", "dtcm"):
        return pathlib.PurePosixPath(module) / "delinks.txt"
    if re.fullmatch(r"ov\d{3}", module):
        return pathlib.PurePosixPath("overlays") / module / "delinks.txt"
    raise ProfileError(f"unrecognized TU module id: {module!r}")


def available_tu_modules(config_tu=CONFIG_TU):
    """Module ids that have a config_tu/ TU-granular delinks root to build from."""
    if not config_tu.is_dir():
        return []
    found = []
    if (config_tu / "delinks.txt").is_file():
        found.append("main")
    for sub in ("itcm", "dtcm"):
        if (config_tu / sub / "delinks.txt").is_file():
            found.append(sub)
    ovdir = config_tu / "overlays"
    if ovdir.is_dir():
        found.extend(sorted(p.parent.name for p in ovdir.glob("*/delinks.txt")))
    return found


def _apply_tu_modules(generated_root, config_tu, tu_modules):
    """Replace each named module's per-function delinks with its config_tu TU root.

    For every module id, the generated tree's ``delinks.txt`` is overwritten by the
    config_tu TU-granular version, and a ``complete`` marker is injected under every
    ``src_tu/`` entry so dsd compiles the merged TU instead of supplying ROM bytes.
    This enforces module-level exclusivity: the module's ~N single-function ``src/``
    entries are entirely replaced by its handful of TU entries. The config_tu
    delinks already carry the correct per-section ranges; only ``complete`` is added.
    """
    applied = []
    for module in tu_modules:
        rel = _module_delinks_rel(module)
        tu_delinks = config_tu / rel
        if not tu_delinks.is_file():
            raise ProfileError(f"no config_tu delinks for module {module!r}: {tu_delinks}")
        gen_delinks = generated_root / rel
        if not gen_delinks.is_file():
            raise ProfileError(f"generated config has no delinks for module {module!r}: {gen_delinks}")
        out, entries = [], 0
        for line in tu_delinks.read_text(encoding="utf-8").splitlines():
            out.append(line)
            stripped = line.strip()
            if line and not line[0].isspace() and stripped.startswith("src_tu/") \
                    and stripped.endswith(":"):
                out.append("    complete")
                entries += 1
        if entries == 0:
            raise ProfileError(f"config_tu delinks for {module!r} names no src_tu/ entry")
        gen_delinks.write_text("\n".join(out) + "\n", encoding="utf-8", newline="\n")
        applied.append({"module": module, "entries": entries,
                        "delinks": rel.as_posix()})
    return applied


def prepare_profile(profile="stock", repo=REPO, source_config=SOURCE_CONFIG, build=BUILD,
                    out_root=None, build_root=None, tu_modules=(), config_tu=CONFIG_TU):
    """Return metadata for a fresh generated build profile.

    ``configYaml`` and ``configRoot`` are pathlib paths.  The remaining fields are JSON
    serializable and are also written beside the generated config for diagnostics.

    ``out_root`` places the generated config tree somewhere other than
    ``build/rombuild-config/<profile>/``, and ``build_root`` re-roots everything the
    build writes (delink objects, module images, linker script, object list) under a
    different directory.  Both default to None, reproducing the shared-``build/``
    layout rombuild.py has always used; ``tools/tubuild.py linkcheck`` sets them so a
    scratch link cannot disturb the real build's outputs.
    """
    if profile not in PROFILES:
        raise ProfileError(f"unknown ROM-build profile {profile!r}; choose from {PROFILES}")
    repo = pathlib.Path(repo).resolve()
    source_config = pathlib.Path(source_config).resolve()
    build = pathlib.Path(build).resolve()
    effective_build = pathlib.Path(build_root).resolve() if build_root else build
    if not (source_config / "config.yaml").is_file():
        raise ProfileError(f"missing source config: {source_config / 'config.yaml'}")
    symlinks = [p for p in source_config.rglob("*") if p.is_symlink()]
    if symlinks:
        raise ProfileError(f"config tree contains a symlink: {symlinks[0]}")

    profile_parent = (pathlib.Path(out_root).resolve() if out_root
                      else build / "rombuild-config" / profile)
    generated_root = profile_parent / "arm9"
    if profile_parent.exists():
        shutil.rmtree(profile_parent)
    generated_root.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(source_config, generated_root)
    _rewrite_config_paths(generated_root / "config.yaml", source_config, generated_root,
                          source_build=build, target_build=effective_build)
    _validate_generated_config(generated_root / "config.yaml", generated_root, repo,
                               effective_build)
    replacements, gap_fallbacks = (_stock_delinks(generated_root, repo)
                                   if profile == "stock" else ([], []))

    tu_applied = (_apply_tu_modules(generated_root, pathlib.Path(config_tu), tu_modules)
                  if tu_modules else [])

    meta = {
        "schemaVersion": 1,
        "profile": profile,
        "generated": True,
        "configRoot": generated_root,
        "configYaml": generated_root / "config.yaml",
        "buildRoot": effective_build,
        "modReplacements": replacements,
        "modGapFallbacks": gap_fallbacks,
        "tuModules": tu_applied,
    }
    serializable = {k: str(v) if isinstance(v, pathlib.Path) else v for k, v in meta.items()}
    (profile_parent / "profile.json").write_text(
        json.dumps(serializable, indent=2) + "\n", encoding="utf-8", newline="\n")
    return meta


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--profile", choices=PROFILES, default="stock")
    args = ap.parse_args()
    result = prepare_profile(args.profile)
    print(json.dumps({k: str(v) if isinstance(v, pathlib.Path) else v
                      for k, v in result.items()}, indent=2))
