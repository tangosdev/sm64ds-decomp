"""Move the production source corpus into the evidence-backed domain tree.

This is a rename-only migration.  It does not merge functions, change language mode,
or claim original Nintendo filenames.  Named sources are grouped by runtime/game/UI
domain; address-named sources remain explicitly unnamed and are grouped by ROM module
plus a browse-only address band.

Run without ``--apply`` to inspect the complete plan.  Applying also rewrites literal
``src/...c(pp)`` references in tracked text files.  The caller must run the repository's
ROM, port, attribution, and reference gates afterwards.
"""
from __future__ import annotations

import argparse
import collections
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"
sys.path.insert(0, str(REPO / "tools"))
import srcpath as SP  # noqa: E402

SOURCE_SUFFIXES = {".c", ".cpp"}
TEXT_SUFFIXES = {
    ".c", ".cpp", ".h", ".hpp", ".json", ".jsonl", ".md", ".txt", ".tsv",
    ".yaml", ".yml", ".cmake", ".ps1", ".py",
}
POSIX_SOURCE = re.compile(r"src/[^\s\"'`<>|:]+?\.(?:c|cpp)")
WINDOWS_SOURCE = re.compile(r"src\\[^\s\"'`<>|:]+?\.(?:c|cpp)")


def git(*args: str) -> str:
    result = subprocess.run(
        ["git", *args], cwd=REPO, check=True, capture_output=True, text=True,
        encoding="utf-8", errors="replace",
    )
    return result.stdout


def tracked_sources() -> list[pathlib.Path]:
    return [
        REPO / rel
        for rel in git("ls-files", "--", "src").splitlines()
        if pathlib.PurePosixPath(rel).suffix in SOURCE_SUFFIXES
    ]


def special_class(symbol: str) -> str | None:
    """Class owner for thunks and class artifacts that ``srcpath.class_of`` rejects."""
    cls = SP.class_of(symbol)
    if cls:
        return cls
    if symbol.startswith(("_ZTh", "_ZTv")):
        nested = symbol.find("_N")
        if nested >= 0:
            return SP.class_of("_ZN" + symbol[nested + 2:])
    if symbol.startswith(("_ZTV", "_ZTI", "_ZTS")):
        rest = symbol[4:]
        match = re.match(r"(\d+)", rest)
        if match:
            size = int(match.group(1))
            tail = rest[len(match.group(1)):]
            if len(tail) >= size:
                return tail[:size]
    return None


def class_for(path: pathlib.Path) -> str | None:
    classes = {
        cls for symbol in SP.symbols_for(path)
        if (cls := special_class(symbol)) is not None
    }
    if len(classes) == 1:
        return classes.pop()
    return special_class(path.stem)


def actor_classes(paths: list[pathlib.Path]) -> set[str]:
    out = set()
    for path in paths:
        if path.stem.endswith("_Spawn"):
            cls = SP.class_of(path.stem)
            if cls:
                out.add(cls)
    return out


def contains_any(value: str, needles: tuple[str, ...]) -> bool:
    return any(needle in value for needle in needles)


def domain_for(stem: str, cls: str | None, actors: set[str]) -> pathlib.PurePosixPath:
    """Return the Candidate-2 domain for a named source.

    Ordering matters: player/camera/stage/minigame classes can also appear in the actor
    spawn table, so the more specific role wins before the actor fallback.
    """
    # Once a class is known, classify the class rather than a method name such as
    # ``SaveData::GetMinigame...`` or ``Actor::GetPlayer...``.
    value = (cls or stem).lower()

    if (cls and (cls.startswith(("dScMg", "Mg")) or "minigame" in value)) or \
            contains_any(value, ("minigame", "picturepoker", "pairagone", "roulette")):
        return pathlib.PurePosixPath("minigames")
    if contains_any(value, ("player", "mario", "luigi", "wario", "yoshi")):
        return pathlib.PurePosixPath("game/player")
    if contains_any(value, ("camera", "viewobj", "viewmatrix", "clipper")):
        return pathlib.PurePosixPath("game/camera")
    if (cls and cls.startswith("dSc")) or contains_any(
            value, ("scene", "stage", "level", "course", "areaoverlay")):
        return pathlib.PurePosixPath("game/stages")

    if cls in actors:
        return pathlib.PurePosixPath("game/actors")

    if contains_any(value, ("message", "msg", "textnarcs", "dialog")):
        return pathlib.PurePosixPath("ui/messages")
    if contains_any(value, ("hud", "minimap", "oamsprite")):
        return pathlib.PurePosixPath("ui/hud")
    if contains_any(value, ("menu", "title", "fileselect", "starselect", "pause")):
        return pathlib.PurePosixPath("ui/menus")

    if contains_any(value, ("memory", "heap", "allocate", "deallocate", "allocator")):
        return pathlib.PurePosixPath("runtime/memory")
    if contains_any(
            value,
            ("vector", "vec", "matrix", "quaternion", "fix12", "math", "angle", "random", "rng",
             "sin", "cos", "sqrt", "div", "mul")):
        return pathlib.PurePosixPath("runtime/math")
    if contains_any(value, ("sound", "audio", "music", "bgm", "sequence", "wave", "midi")):
        return pathlib.PurePosixPath("runtime/audio")
    if contains_any(
            value,
            ("file", "filesystem", "archive", "narc", "overlay", "decompress", "uncomp",
             "lz77", "huff", "loadcompressed", "bitunpack", "savedata")):
        return pathlib.PurePosixPath("runtime/filesystem")
    if contains_any(
            value,
            ("fader", "model", "animation", "texture", "material", "render", "graphics",
             "geometry", "palette", "vram", "sprite", "color", "light")):
        return pathlib.PurePosixPath("runtime/graphics")

    if cls:
        return pathlib.PurePosixPath("game/actors")
    if contains_any(value, ("entry", "autoload", "processor", "context", "interrupt", "crash")):
        return pathlib.PurePosixPath("runtime")
    return pathlib.PurePosixPath("game/objects")


def target_for(path: pathlib.Path, actors: set[str]) -> pathlib.Path:
    stem = path.stem
    rel = path.relative_to(SRC).as_posix()
    if rel.startswith("actors/"):
        return SRC / "game" / "actors" / pathlib.PurePosixPath(rel).relative_to("actors")
    if rel.startswith("engine/fader/"):
        return SRC / "runtime" / "graphics" / "fader" / path.name
    if rel.startswith("engine/message/"):
        return SRC / "ui" / "messages" / path.name
    module = SP.module_of(stem)
    if module:
        band = SP.address_band(stem)
        assert band is not None
        return SRC / "unnamed" / module / band / path.name

    cls = class_for(path)
    domain = domain_for(stem, cls, actors)
    # A class directory gives readers a usable method list and keeps large domain
    # buckets browsable.  A reconstructed source owning several classes stays at the
    # domain root because forcing one class owner would invent a TU relationship.
    owned_classes = {
        owner for symbol in SP.symbols_for(path)
        if (owner := special_class(symbol)) is not None
    }
    if cls and len(owned_classes) <= 1:
        return SRC / domain / cls / path.name
    return SRC / domain / path.name


def plan() -> dict[str, str]:
    paths = tracked_sources()
    actors = actor_classes(paths)
    moves = {}
    destinations = {}
    for old in paths:
        new = target_for(old, actors)
        old_rel = old.relative_to(REPO).as_posix()
        new_rel = new.relative_to(REPO).as_posix()
        if old_rel == new_rel:
            continue
        if new_rel in destinations:
            raise ValueError(f"destination collision: {destinations[new_rel]} and {old_rel} -> {new_rel}")
        if new.exists() and new.resolve() != old.resolve():
            raise ValueError(f"destination already exists: {new_rel}")
        destinations[new_rel] = old_rel
        moves[old_rel] = new_rel
    return moves


def rewrite_references(moves: dict[str, str]) -> list[str]:
    win_moves = {old.replace("/", "\\"): new.replace("/", "\\")
                 for old, new in moves.items()}
    edited = []
    original_tracked = git("ls-files").splitlines()
    actual_paths = [moves.get(rel, rel) for rel in original_tracked]
    for rel in actual_paths:
        path = REPO / rel
        if not path.is_file() or path.suffix.lower() not in TEXT_SUFFIXES:
            continue
        try:
            text = path.read_text(encoding="utf-8")
        except (OSError, UnicodeDecodeError):
            continue
        updated = POSIX_SOURCE.sub(lambda match: moves.get(match.group(0), match.group(0)), text)
        updated = WINDOWS_SOURCE.sub(
            lambda match: win_moves.get(match.group(0), match.group(0)), updated)
        if updated != text:
            path.write_text(updated, encoding="utf-8", newline="\n")
            edited.append(rel)
    return edited


def apply(moves: dict[str, str]) -> list[str]:
    # Validate every resolved target before the first mutation.
    src_root = SRC.resolve()
    for old_rel, new_rel in moves.items():
        old = (REPO / old_rel).resolve()
        new = (REPO / new_rel).resolve()
        if not old.is_file() or SRC.resolve() not in old.parents:
            raise ValueError(f"source is not a file under src: {old_rel}")
        if src_root not in new.parents:
            raise ValueError(f"target escapes src: {new_rel}")
    for old_rel, new_rel in sorted(moves.items()):
        old = REPO / old_rel
        new = REPO / new_rel
        new.parent.mkdir(parents=True, exist_ok=True)
        old.rename(new)
    return rewrite_references(moves)


def print_summary(moves: dict[str, str], sample: int) -> None:
    groups = collections.Counter()
    for new in moves.values():
        parts = pathlib.PurePosixPath(new).parts
        key = "/".join(parts[1:3]) if len(parts) >= 3 else new
        groups[key] += 1
    print(f"planned source moves: {len(moves)}")
    for group, count in sorted(groups.items(), key=lambda item: (-item[1], item[0])):
        print(f"  {count:5d}  src/{group}/")
    if sample:
        print("sample:")
        for old, new in list(sorted(moves.items()))[:sample]:
            print(f"  {old} -> {new}")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--sample", type=int, default=20)
    args = parser.parse_args(argv)
    moves = plan()
    print_summary(moves, args.sample)
    if not args.apply:
        print("dry run; pass --apply to move files and rewrite tracked path references")
        return 0
    edited = apply(moves)
    print(f"rewrote literal paths in {len(edited)} tracked text file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
