#!/usr/bin/env python3
"""Report the live C++ and translation-unit state from repository authorities.

This deliberately does not copy progress numbers out of prose.  It derives them from
the inputs that the build and TU workflow actually consume:

* ``progress.synced_from_src`` (committed-source matching progress),
* ``tiers.converted`` and ``langmode_audit.audit`` (readability and semantic C++),
* git's tracked ``src/**/*.c|cpp`` set (physical source inventory),
* ``config/**/symbols.txt`` plus sibling ``delinks.txt`` files (production ownership),
* ``config/tu_manifest.d/`` (shadow/reconstruction evidence),
* ``cpp_tu_compat.audit`` (behavior-backed production readiness).

The checked-in note is a cheap agent bootstrap, not a second database.  Regenerate it
with ``--write-note`` and use ``--check-note`` to prove it still agrees with the live
inputs.
"""
from __future__ import annotations

import argparse
import bisect
import collections
import json
import pathlib
import re
import subprocess
import sys


REPO = pathlib.Path(__file__).resolve().parent.parent
NOTE = REPO / "notes" / "cpp-tu-current-state.md"
sys.path.insert(0, str(REPO / "tools"))
import langmode_audit as LANGMODE  # noqa: E402
import progress as PROGRESS  # noqa: E402
import tiers as TIERS  # noqa: E402
import cpp_tu_compat as TU_COMPAT  # noqa: E402
import tu_manifest as TUM  # noqa: E402

SOURCE_SUFFIXES = (".c", ".cpp")
FUNC_RE = re.compile(
    r"^(\S+)\s+kind:function\([^)]*\)\s+addr:0x([0-9a-fA-F]+)"
)
RANGE_RE = re.compile(
    r"\.\w+\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)"
)


class StateError(RuntimeError):
    pass


def configured_production_tus(repo: pathlib.Path) -> dict[str, list[str]]:
    """Manifest ids enabled by each default production ROM-build mode."""
    path = repo / "config" / "production-tus.json"
    if not path.is_file():
        return {"partitioned_tus": [], "derived_text_tus": []}
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise StateError(f"{path} is unreadable: {exc}") from exc
    if data.get("schema_version") != 1:
        raise StateError(f"{path} has an invalid schema_version")
    out = {}
    all_ids = []
    for key in ("partitioned_tus", "derived_text_tus"):
        rows = data.get(key, [])
        if (not isinstance(rows, list)
                or any(not isinstance(row, str) or not row for row in rows)
                or len(rows) != len(set(rows))):
            raise StateError(f"{path} has an invalid {key} registry")
        out[key] = rows
        all_ids.extend(rows)
    if len(all_ids) != len(set(all_ids)):
        raise StateError(f"{path} configures one TU in more than one mode")
    return out


def configured_partitioned_tus(repo: pathlib.Path) -> list[str]:
    """Compatibility view for the first production mode."""
    return configured_production_tus(repo)["partitioned_tus"]


def dirty_authority_inputs(repo: pathlib.Path) -> list[str]:
    """Unstaged/untracked paths that would mix index and working-tree states."""
    try:
        paths = ["src", "src_tu", "include", "config"]
        changed = subprocess.run(
            ["git", "-C", str(repo), "diff", "--name-only", "-z", "--", *paths],
            check=True, capture_output=True,
        ).stdout.decode("utf-8", errors="surrogateescape")
        untracked = subprocess.run(
            ["git", "-C", str(repo), "ls-files", "-z", "--others",
             "--exclude-standard", "--", *paths],
            check=True, capture_output=True,
        ).stdout.decode("utf-8", errors="surrogateescape")
    except (OSError, subprocess.CalledProcessError) as exc:
        raise StateError(f"cannot verify authority-input cleanliness: {exc}") from exc
    return sorted({path for path in (changed + untracked).split("\0") if path})


def tracked_sources(repo: pathlib.Path) -> list[str]:
    """Git-tracked C/C++ sources, as repo-relative POSIX paths."""
    try:
        out = subprocess.run(
            ["git", "-C", str(repo), "ls-files", "-z", "--", "src", "src_tu"],
            check=True,
            capture_output=True,
        ).stdout.decode("utf-8", errors="surrogateescape")
    except (OSError, subprocess.CalledProcessError) as exc:
        raise StateError(f"cannot read git-tracked source inventory: {exc}") from exc
    return sorted(
        p for p in out.split("\0") if pathlib.PurePosixPath(p).suffix in SOURCE_SUFFIXES
    )


def production_ownership(repo: pathlib.Path) -> tuple[
        dict[tuple[str, str], str], dict[str, list[str]],
        dict[tuple[str, str], str], dict[str, list[str]]]:
    """Return all path ownership and source-built (``complete``) ownership.

    A delinks entry, not a filename convention, is the production source of truth.
    Looking up function addresses inside every source-owned range also handles the
    future case where one source owns several functions.  But a path entry alone is
    not source-built: without an indented ``complete`` marker dsd supplies its bytes
    from the ROM.  Keeping both maps prevents the state report from calling a rombytes
    scale-test entry a decompiled/enrolled function.
    """
    path_by_symbol: dict[tuple[str, str], str] = {}
    path_by_source: dict[str, list[tuple[int, str]]] = collections.defaultdict(list)
    complete_by_symbol: dict[tuple[str, str], str] = {}
    complete_by_source: dict[str, list[tuple[int, str]]] = collections.defaultdict(list)

    for symbols_path in sorted((repo / "config").rglob("symbols.txt")):
        module = symbols_path.parent.name
        delinks_path = symbols_path.with_name("delinks.txt")
        if not delinks_path.is_file():
            continue
        functions = []
        for line in symbols_path.read_text(encoding="utf-8", errors="ignore").splitlines():
            match = FUNC_RE.match(line.strip())
            if match:
                functions.append((int(match.group(2), 16), match.group(1)))
        functions.sort()
        addresses = [address for address, _symbol in functions]

        entry = None
        complete = False
        for line in delinks_path.read_text(encoding="utf-8", errors="ignore").splitlines():
            if not line.strip():
                continue
            if not line[0].isspace():
                entry = line.strip().rstrip(":")
                complete = False
                continue
            if entry is None or not entry.startswith("src/"):
                continue
            if line.strip() == "complete":
                complete = True
                continue
            match = RANGE_RE.search(line)
            if not match:
                continue
            start, end = int(match.group(1), 16), int(match.group(2), 16)
            index = bisect.bisect_left(addresses, start)
            while index < len(functions) and functions[index][0] < end:
                address, symbol = functions[index]
                key = (module, symbol)
                previous = path_by_symbol.get(key)
                if previous is not None and previous != entry:
                    raise StateError(
                        f"{module}:{symbol} is owned by both {previous} and {entry}"
                    )
                path_by_symbol[key] = entry
                path_by_source[entry].append((address, symbol))
                if complete:
                    complete_by_symbol[key] = entry
                    complete_by_source[entry].append((address, symbol))
                index += 1

    path_inverse = {
        source: [symbol for _address, symbol in sorted(rows)]
        for source, rows in sorted(path_by_source.items())
    }
    complete_inverse = {
        source: [symbol for _address, symbol in sorted(rows)]
        for source, rows in sorted(complete_by_source.items())
    }
    return path_by_symbol, path_inverse, complete_by_symbol, complete_inverse


def semantic_language_mode(repo: pathlib.Path) -> dict[str, int]:
    """The semantic headline, delegated to the existing language-mode authority."""
    saved_repo = LANGMODE.REPO
    LANGMODE.REPO = repo
    try:
        audit = LANGMODE.audit()
    finally:
        LANGMODE.REPO = saved_repo
    language = audit["language_mode"]
    return {
        "mangled_symbol_files": language["mangled_total"],
        "unmigrated": language["unmigrated_total"],
        "genuinely_migrated": sum(
            row["migrated"] for row in language["by_kind"].values()
        ),
        "cpp_still_handspelled": language["cpp_still_handspelled"],
        "nonmatching_drafts": language["nonmatching_drafts"],
    }


def _suffix_counts(paths: list[str]) -> dict[str, int]:
    return {
        "total": len(paths),
        "c": sum(path.endswith(".c") for path in paths),
        "cpp": sum(path.endswith(".cpp") for path in paths),
    }


def _first_line(path: pathlib.Path) -> str:
    try:
        with path.open(encoding="utf-8", errors="ignore") as stream:
            return stream.readline().rstrip("\r\n")
    except OSError:
        return ""


def _matching_progress(reader) -> dict[str, int | float]:
    matched, matched_bytes, total, total_bytes = reader()
    return {
        "matched_functions": matched,
        "total_functions": total,
        "function_percent": round(100.0 * matched / total, 2) if total else 0.0,
        "matched_bytes": matched_bytes,
        "total_bytes": total_bytes,
        "byte_percent": round(100.0 * matched_bytes / total_bytes, 2) if total_bytes else 0.0,
    }


def _converted_tier(reader) -> dict:
    converted = reader()
    source_functions = converted.get("functions", converted.get("files", 0))
    source_files = converted.get("source_files", converted.get("files", 0))
    return {
        "strict_converted_functions": converted["converted"],
        "source_functions": source_functions,
        "source_files": source_files,
        "percent": converted["pct"],
        "criteria": converted["criteria"],
    }


def collect(repo: pathlib.Path = REPO, *, progress_reader=None,
            converted_reader=None, language_mode_reader=None,
            compatibility_reader=None, require_clean_inputs=True) -> dict:
    repo = pathlib.Path(repo)
    dirty_inputs = dirty_authority_inputs(repo) if require_clean_inputs else []
    if dirty_inputs:
        raise StateError(
            "authority inputs have unstaged/untracked changes; stage or stash src, "
            "src_tu, include, and config before generating project state: "
            + ", ".join(dirty_inputs[:8])
        )
    if progress_reader is None:
        if repo.resolve() != REPO.resolve():
            raise StateError("a non-default repo requires progress_reader")
        progress_reader = PROGRESS.synced_from_src
    if converted_reader is None:
        converted_reader = lambda: TIERS.converted(repo / "src")
    if language_mode_reader is None:
        language_mode_reader = lambda: semantic_language_mode(repo)
    if compatibility_reader is None:
        compatibility_reader = TU_COMPAT.audit
    tracked = tracked_sources(repo)
    tracked_set = set(tracked)
    production_tracked = [path for path in tracked if path.startswith("src/")]
    shadow_tracked = [path for path in tracked if path.startswith("src_tu/")]
    path_by_symbol, path_by_source, by_symbol, by_source = production_ownership(repo)
    path_sources = sorted(path_by_source)
    production_sources = sorted(by_source)
    multi = {source: symbols for source, symbols in by_source.items() if len(symbols) > 1}
    missing_sources = sorted(path for path in production_sources if path not in tracked_set)
    marker_missing = sorted(
        path for path in production_tracked
        if path.endswith(".cpp") and _first_line(repo / path) != "//cpp"
    )

    manifest_root = repo / "config" / "tu_manifest.d"
    if not TUM.exists(manifest_root):
        raise StateError("config/tu_manifest.d/ is missing")
    manifest = TUM.load(manifest_root)
    entries = manifest.get("entries")
    if not isinstance(entries, list):
        raise StateError("config/tu_manifest.d/ has no entries")

    statuses = collections.Counter()
    modules = set()
    manifest_functions = 0
    manifest_symbols = set()
    shadow_present = 0
    production_promoted = 0
    promotion_mismatch = []
    configured = configured_production_tus(repo)
    configured_partitioned = set(configured["partitioned_tus"])
    configured_derived = set(configured["derived_text_tus"])
    production_partitioned = 0
    production_partitioned_functions = 0
    production_derived = 0
    production_derived_functions = 0
    partitioned_mismatch = []
    seen_entries = set()
    non_text_entries = 0
    for entry in entries:
        entry_id = entry.get("id", "<missing-id>")
        seen_entries.add(entry_id)
        statuses[str(entry.get("status", "<missing-status>"))] += 1
        if entry.get("module"):
            modules.add(entry["module"])
        functions = entry.get("functions") or []
        symbols = [row.get("symbol") for row in functions if row.get("symbol")]
        manifest_functions += len(symbols)
        manifest_symbols.update(symbols)
        source = entry.get("source")
        if source in tracked_set:
            shadow_present += 1
        sections = entry.get("sections") or []
        if any(section.get("name") != ".text" for section in sections):
            non_text_entries += 1

        if entry_id in configured_partitioned:
            partition_state = (entry.get("partitioned_link") or {}).get("state")
            canonical = entry.get("source")
            if (entry.get("production_mode") == "partitioned"
                    and partition_state == "partitioned-link-verified"
                    and canonical == entry.get("promoted_source")
                    and canonical in tracked_set):
                production_partitioned += 1
                production_partitioned_functions += len(symbols)
            else:
                partitioned_mismatch.append(entry_id)

        if entry_id in configured_derived:
            derived_state = (entry.get("partial_isolation") or {}).get("state")
            canonical = entry.get("source")
            if (entry.get("production_mode") == "derived-text"
                    and derived_state == "partial-link-verified"
                    and canonical == entry.get("promoted_source")
                    and canonical in tracked_set
                    and all(section.get("name") == ".text" for section in sections)):
                production_derived += 1
                production_derived_functions += len(symbols)
            else:
                partitioned_mismatch.append(entry_id)

        promoted = entry.get("promoted_source")
        if entry.get("status") != "promoted" or not promoted or promoted not in tracked_set:
            continue
        actual = {by_symbol.get((entry.get("module"), symbol)) for symbol in symbols}
        if symbols and actual == {promoted}:
            production_promoted += 1
        else:
            promotion_mismatch.append(entry_id)
    partitioned_mismatch.extend(sorted(configured_partitioned - seen_entries))
    partitioned_mismatch.extend(sorted(configured_derived - seen_entries))

    production_counts = _suffix_counts(production_tracked)
    production_counts["cpp_marker_missing"] = len(marker_missing)
    production_counts["cpp_marker_missing_paths"] = marker_missing

    return {
        "schema_version": 1,
        "snapshot": {"authority_inputs_clean": not dirty_inputs},
        "inputs": {
            "matching_progress": "progress.synced_from_src() over committed config + src",
            "strict_converted": "tiers.converted(src)",
            "semantic_language_mode": "langmode_audit.audit()",
            "tracked_sources": "git ls-files -- src src_tu",
            "production_ownership": "config/**/symbols.txt + sibling delinks.txt",
            "tu_evidence": "config/tu_manifest.d",
        },
        "matching_progress": _matching_progress(progress_reader),
        "converted_tier": _converted_tier(converted_reader),
        "source_files": {
            "production": production_counts,
            "shadow": _suffix_counts(shadow_tracked),
        },
        "semantic_language_mode": language_mode_reader(),
        "production_tu_compatibility": compatibility_reader(),
        "production_path_ownership": {
            "functions": len(path_by_symbol),
            "source_files": len(path_sources),
            "rombytes_only_functions": len(set(path_by_symbol) - set(by_symbol)),
            "rombytes_only_source_files": len(set(path_sources) - set(production_sources)),
        },
        "production_enrollment": {
            "functions": len(by_symbol),
            "source_files": len(production_sources),
            "c_source_files": sum(path.endswith(".c") for path in production_sources),
            "cpp_source_files": sum(path.endswith(".cpp") for path in production_sources),
            "missing_source_files": len(missing_sources),
            "missing_source_paths": missing_sources,
            "multi_function_source_files": len(multi),
            "functions_in_multi_function_sources": sum(len(rows) for rows in multi.values()),
            "largest_function_count": max((len(rows) for rows in by_source.values()), default=0),
            "multi_function_sources": multi,
        },
        "tu_reconstruction": {
            "manifest_entries": len(entries),
            "manifest_functions": manifest_functions,
            "unique_manifest_functions": len(manifest_symbols),
            "modules": len(modules),
            "statuses": dict(sorted(statuses.items())),
            "shadow_sources_present": shadow_present,
            "entries_with_non_text_sections": non_text_entries,
            "production_promoted_entries": production_promoted,
            "promotion_mismatch_entries": sorted(promotion_mismatch),
            "production_partitioned_entries": production_partitioned,
            "production_partitioned_functions": production_partitioned_functions,
            "production_derived_text_entries": production_derived,
            "production_derived_text_functions": production_derived_functions,
            "partitioned_mismatch_entries": sorted(partitioned_mismatch),
        },
        "commands": {
            "refresh_this_note": "python tools/cpp_tu_state.py --write-note",
            "check_this_note": "python tools/cpp_tu_state.py --check-note",
            "machine_report": "python tools/cpp_tu_state.py --json",
            "matching_progress": "python tools/progress.py --bar --from-src",
            "language_mode_debt": "python tools/langmode_audit.py",
            "production_tu_compatibility": "python tools/cpp_tu_compat.py",
            "production_reference_check": "python tools/port_refcheck.py",
        },
    }


def _table(rows: list[tuple[str, object]]) -> list[str]:
    return ["| Measure | Live value |", "| --- | ---: |"] + [
        f"| {label} | {value} |" for label, value in rows
    ]


def render_markdown(report: dict) -> str:
    progress = report["matching_progress"]
    converted = report["converted_tier"]
    src = report["source_files"]
    language = report["semantic_language_mode"]
    ownership = report["production_path_ownership"]
    prod = report["production_enrollment"]
    tu = report["tu_reconstruction"]
    compat = report["production_tu_compatibility"]
    status = ", ".join(f"`{key}` {value}" for key, value in tu["statuses"].items())
    multi_sources = list(prod["multi_function_sources"].items())
    multi_summary = ", ".join(
        f"`{source}` ({len(symbols)} function-symbol records)"
        for source, symbols in multi_sources[:10]
    ) or "none"
    if len(multi_sources) > 10:
        multi_summary += f", and {len(multi_sources) - 10} more in `--json`"
    lines = [
        "# Current C++ and translation-unit state",
        "",
        "<!-- Generated by tools/cpp_tu_state.py. Do not hand-edit live counts. -->",
        "",
        "This is the small, current authority page for C++ language mode and translation-unit",
        "reconstruction. Its counts come from the live build inputs, not from prose plans.",
        "Generation refuses unstaged or untracked `src/`, `src_tu/`, `include/`, or",
        "`config/` inputs so the Git index and filesystem analyses describe the same",
        "snapshot. Staged inputs are allowed and become the would-be commit snapshot.",
        "Regenerate it after changing `src/`, production delinks, or the TU manifest:",
        "",
        "```text",
        report["commands"]["refresh_this_note"],
        report["commands"]["check_this_note"],
        "```",
        "",
        "## Project progress and strict readability",
        "",
        *_table([
            ("MATCHED functions", f"{progress['matched_functions']} / {progress['total_functions']} ({progress['function_percent']:.2f}%)"),
            ("MATCHED code bytes", f"{progress['matched_bytes']} / {progress['total_bytes']} ({progress['byte_percent']:.2f}%)"),
            ("Strict CONVERTED source functions", f"{converted['strict_converted_functions']} / {converted['source_functions']} ({converted['percent']:.2f}%)"),
            ("Physical production source files", converted["source_files"]),
        ]),
        "",
        "MATCHED is recomputed from committed config symbols and `src/` through",
        "`progress.synced_from_src()`; no ambient or gitignored chaos database is accepted.",
        "CONVERTED is the strict five-criterion result from `tiers.converted(src)`,",
        "weighted by enrolled function ownership so TU consolidation does not move it.",
        "",
        "## Production source and enrollment",
        "",
        *_table([
            ("Tracked production source files", src["production"]["total"]),
            ("Tracked `.c` files", src["production"]["c"]),
            ("Tracked `.cpp` files", src["production"]["cpp"]),
            ("`.cpp` files missing first-line `//cpp`", src["production"]["cpp_marker_missing"]),
            ("Mangled-symbol source files", language["mangled_symbol_files"]),
            ("Genuinely migrated C++ symbol files", language["genuinely_migrated"]),
            ("Not semantically migrated", language["unmigrated"]),
            ("`.cpp` files still hand-spelling their symbol", language["cpp_still_handspelled"]),
            ("Nonmatching C++-symbol drafts", language["nonmatching_drafts"]),
            ("Delinks path-owned function-symbol records", ownership["functions"]),
            ("Path-owned records still supplied from ROM bytes", ownership["rombytes_only_functions"]),
            ("Source-built (`complete`) function-symbol records", prod["functions"]),
            ("Source-built (`complete`) source files", prod["source_files"]),
            ("Complete sources owning more than one function", prod["multi_function_source_files"]),
            ("Source-built records inside multi-function sources", prod["functions_in_multi_function_sources"]),
            ("Largest function-symbol-records-per-source count", prod["largest_function_count"]),
            ("Complete source paths missing from git", prod["missing_source_files"]),
        ]),
        "",
        "Production path ownership is derived from `config/**/delinks.txt` ranges joined to",
        "function addresses in the sibling `symbols.txt`. Source-built counts additionally",
        "require the delinks entry's `complete` marker; entries without it are ROM-byte",
        "placeholders and are not reported as decompiled. The join remains correct when a",
        "source owns two functions; filenames are not treated as the authority.",
        "",
        f"Current complete multi-function owners: {multi_summary}.",
        "",
        "A `.cpp` extension alone proves nothing about compiler language mode; this build",
        "selects C++ only when the first line is exactly `//cpp`. The marker-missing count",
        "above exposes extension-only files. Semantic migration is stricter still and comes",
        "from the same analysis as",
        f"`{report['commands']['language_mode_debt']}`: genuinely migrated files let the compiler",
        "mangle native declarations; unmigrated files still spell a mangled symbol themselves.",
        "Nonmatching drafts are shown as a separate, overlapping warning count, not a third",
        "partition to add to the migrated and unmigrated rows.",
        "",
        "## TU reconstruction and production",
        "",
        *_table([
            ("Tracked `src_tu/` source files", src["shadow"]["total"]),
            ("Manifest entries", tu["manifest_entries"]),
            ("Functions named by manifest entries", tu["manifest_functions"]),
            ("Unique functions named by the manifest", tu["unique_manifest_functions"]),
            ("Modules represented", tu["modules"]),
            ("Manifest source files present in git", tu["shadow_sources_present"]),
            ("Entries licensing non-text sections", tu["entries_with_non_text_sections"]),
            ("Entries actually production-enrolled at `promoted_source`", tu["production_promoted_entries"]),
            ("Existing promotion paths that disagree with delinks", len(tu["promotion_mismatch_entries"])),
            ("Default partitioned production TUs", tu["production_partitioned_entries"]),
            ("Functions supplied by partitioned production TUs", tu["production_partitioned_functions"]),
            ("Default derived-text production TUs", tu["production_derived_text_entries"]),
            ("Functions supplied by derived-text production TUs", tu["production_derived_text_functions"]),
            ("Invalid configured production entries", len(tu["partitioned_mismatch_entries"])),
        ]),
        "",
        f"Manifest statuses: {status or '(none)'}.",
        "",
        "`config/tu_manifest.d/` records reconstruction evidence and licensed ranges; it does",
        "not enroll a TU by itself. Direct",
        "promotion is counted only when every manifest function is owned by the tracked",
        "`promoted_source` in live delinks. `config/production-tus.json` is the separate",
        "fail-closed authority for default TU production. Partitioned TUs compile once",
        "and supply exact derived text plus licensed compiler-emitted non-text output;",
        "derived-text TUs compile once and supply only their exact manifest functions while",
        "their explicitly inventoried extra compiler output remains unowned.",
        "",
        "## Production TU compatibility",
        "",
        f"All surfaces ready: **{'YES' if compat['allSurfacesReady'] else 'NO'}**.",
        "",
        "This is a behavior-backed result from a disposable source containing two C++",
        "methods, not a checklist copied from prose. Run",
        f"`{report['commands']['production_tu_compatibility']}` for the per-surface",
        "evidence. Current ready surfaces: "
        + (", ".join(f"`{row['surface']}`" for row in compat["surfaces"]
                     if row["status"] == "ready") or "none") + ".",
        "Current hard production blockers: "
        + (", ".join(f"`{name}`" for name in compat["blockers"]) or "none") + ".",
        "Current policy/metric gaps: "
        + (", ".join(f"`{name}`" for name in compat["policyAndMetricGaps"]) or "none") + ".",
        "",
        "## Which source answers which question",
        "",
        "| Question | Authority / command |",
        "| --- | --- |",
        f"| How much matching is done? | `{report['inputs']['matching_progress']}`; display with `{report['commands']['matching_progress']}` |",
        f"| How much source is strictly readable? | `{report['inputs']['strict_converted']}` |",
        f"| Which source does the ROM build compile? | `{report['inputs']['production_ownership']}` |",
        f"| What TU evidence has been recovered? | `{report['inputs']['tu_evidence']}` |",
        f"| Can one production source safely own two functions? | `{report['commands']['production_tu_compatibility']}` |",
        f"| Is C++ source semantically migrated? | `{report['inputs']['semantic_language_mode']}`; inspect with `{report['commands']['language_mode_debt']}` |",
        f"| Did a move/merge strand the PC port? | `{report['commands']['production_reference_check']}` |",
        "",
        "Long-form plans and pilot reports explain design and history; they are not current-count",
        "authorities. If a prose number disagrees with this generated page, regenerate this page",
        "and fix or mark the prose as historical.",
        "",
    ]
    return "\n".join(str(line) for line in lines)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--json", action="store_true", help="print the report as JSON")
    mode.add_argument("--write-note", action="store_true", help=f"rewrite {NOTE.relative_to(REPO)}")
    mode.add_argument("--check-note", action="store_true", help="fail if the checked-in note is stale")
    args = parser.parse_args(argv)

    try:
        report = collect(REPO)
    except (StateError, json.JSONDecodeError) as exc:
        print(f"cpp_tu_state: {exc}", file=sys.stderr)
        return 2

    if args.json:
        print(json.dumps(report, indent=2, sort_keys=True))
        return 0
    rendered = render_markdown(report)
    if args.write_note:
        NOTE.write_text(rendered, encoding="utf-8", newline="\n")
        print(f"wrote {NOTE.relative_to(REPO).as_posix()}")
        return 0
    if args.check_note:
        actual = NOTE.read_text(encoding="utf-8") if NOTE.is_file() else ""
        if actual != rendered:
            print(
                "cpp-tu state note is stale; run "
                "`python tools/cpp_tu_state.py --write-note`",
                file=sys.stderr,
            )
            return 1
        print("cpp-tu state note is current")
        return 0

    print(rendered, end="")
    return 0


if __name__ == "__main__":
    sys.exit(main())
