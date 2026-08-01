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
BUILD = REPO / "build"
sys.path.insert(0, str(REPO / "tools"))
import srcpath as SP  # noqa: E402

PROFILES = ("stock", "mods")
PATH_LINE = re.compile(r"^(\s*(?:rom_config|build_path|delinks_path|object):\s+)(\S+)(\s*)$")
CONFIG_REF_LINE = re.compile(r"^\s*(delinks|symbols|relocations):\s+(\S+)\s*$")


class ProfileError(RuntimeError):
    pass


def _rewrite_config_paths(path, source_root, generated_root):
    """Retarget paths that were relative to config/arm9 to the generated tree."""
    lines = []
    for line in path.read_text(encoding="utf-8").splitlines():
        m = PATH_LINE.match(line)
        if m and m.group(2).startswith("."):
            target = (source_root / m.group(2)).resolve()
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
                                "NONMATCHING" not in src.read_text(
                                    encoding="utf-8", errors="ignore")[:200])
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


def prepare_profile(profile="stock", repo=REPO, source_config=SOURCE_CONFIG, build=BUILD):
    """Return metadata for a fresh generated build profile.

    ``configYaml`` and ``configRoot`` are pathlib paths.  The remaining fields are JSON
    serializable and are also written beside the generated config for diagnostics.
    """
    if profile not in PROFILES:
        raise ProfileError(f"unknown ROM-build profile {profile!r}; choose from {PROFILES}")
    repo = pathlib.Path(repo).resolve()
    source_config = pathlib.Path(source_config).resolve()
    build = pathlib.Path(build).resolve()
    if not (source_config / "config.yaml").is_file():
        raise ProfileError(f"missing source config: {source_config / 'config.yaml'}")
    symlinks = [p for p in source_config.rglob("*") if p.is_symlink()]
    if symlinks:
        raise ProfileError(f"config tree contains a symlink: {symlinks[0]}")

    generated_root = build / "rombuild-config" / profile / "arm9"
    profile_parent = generated_root.parent
    if profile_parent.exists():
        shutil.rmtree(profile_parent)
    generated_root.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(source_config, generated_root)
    _rewrite_config_paths(generated_root / "config.yaml", source_config, generated_root)
    _validate_generated_config(generated_root / "config.yaml", generated_root, repo, build)
    replacements, gap_fallbacks = (_stock_delinks(generated_root, repo)
                                   if profile == "stock" else ([], []))

    meta = {
        "schemaVersion": 1,
        "profile": profile,
        "generated": True,
        "configRoot": generated_root,
        "configYaml": generated_root / "config.yaml",
        "modReplacements": replacements,
        "modGapFallbacks": gap_fallbacks,
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
