"""List source files affected by changed local headers.

The PR validation worker normally compiles changed ``src/*.c|*.cpp`` files. Once
sources share headers, a header-only edit can change the codegen of unchanged
translation units too. This tool computes the reverse, transitive include closure
so the worker can add every affected source file to the validation set.

Usage:
    python tools/affected_src.py include/types.h
    python tools/affected_src.py include/types.h include/Timer.hpp --json
"""
import argparse
import json
import pathlib
import re


REPO = pathlib.Path(__file__).resolve().parent.parent
INCLUDE_RE = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.MULTILINE)
SOURCE_SUFFIXES = {".c", ".cpp"}
HEADER_SUFFIXES = {".h", ".hpp"}


def _repo_path(path, repo):
    path = pathlib.Path(path)
    if not path.is_absolute():
        path = repo / path
    return path.resolve()


def _include_target(source, spelling, repo):
    local = (source.parent / spelling).resolve()
    if local.exists():
        return local
    return (repo / "include" / spelling).resolve()


def affected_sources(changed, repo=REPO):
    """Return sorted repo-relative source paths affected by ``changed`` paths."""
    repo = pathlib.Path(repo).resolve()
    files = []
    for root in (repo / "include", repo / "src"):
        if root.is_dir():
            files.extend(p for p in root.rglob("*")
                         if p.is_file() and p.suffix.lower() in SOURCE_SUFFIXES | HEADER_SUFFIXES)

    reverse = {}
    for source in files:
        try:
            text = source.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        for spelling in INCLUDE_RE.findall(text):
            target = _include_target(source, spelling, repo)
            reverse.setdefault(target, set()).add(source.resolve())

    queue = [_repo_path(path, repo) for path in changed]
    seen = set(queue)
    affected = {p for p in queue if p.suffix.lower() in SOURCE_SUFFIXES}
    while queue:
        dependency = queue.pop()
        for consumer in reverse.get(dependency, ()):
            if consumer.suffix.lower() in SOURCE_SUFFIXES:
                affected.add(consumer)
            if consumer not in seen:
                seen.add(consumer)
                queue.append(consumer)

    return sorted(p.relative_to(repo).as_posix() for p in affected)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("changed", nargs="+", help="changed source/header paths")
    ap.add_argument("--json", action="store_true", help="emit a JSON array")
    args = ap.parse_args()
    result = affected_sources(args.changed)
    if args.json:
        print(json.dumps(result, indent=2))
    else:
        print("\n".join(result))


if __name__ == "__main__":
    main()
