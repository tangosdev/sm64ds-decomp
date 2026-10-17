"""Opt-in rombuild orchestration for fresh, requested global-data proofs.

The request's size is a reviewed assertion, not a size recovered by this tool.
Source/configuration inputs are bound before compilation, then checked again
before and after the actual link. Old output files cannot satisfy a new request.
"""
import hashlib
import json
import pathlib

import data_evidence as DE


def input_hashes(repo, extra_paths=(), extra_roots=()):
    """Include additions/removals as well as edits, without scanning private tools."""
    repo = pathlib.Path(repo)
    paths = set()
    for root in ("src", "include", "config"):
        paths.update(path for path in (repo / root).rglob("*") if path.is_file())
    for root in extra_roots:
        paths.update(path for path in pathlib.Path(root).rglob("*") if path.is_file())
    paths.update((repo / "tools").glob("*.py"))
    paths.update(pathlib.Path(path) for path in extra_paths)
    return {str(path.absolute()): hashlib.sha256(path.read_bytes()).hexdigest()
            for path in sorted(paths)}


def require_unchanged(plan):
    current = input_hashes(plan["repo"], plan["extra_paths"], plan["extra_roots"])
    previous = plan["input_sha256"]
    if current != previous:
        changed = sorted(path for path in current.keys() | previous.keys()
                         if current.get(path) != previous.get(path))
        raise ValueError("data provenance inputs changed during the build: "
                         + ", ".join(changed[:8]))


def prepare(repo, build_root, request_path, tool_paths=()):
    """Validate explicit requests against current configuration before compiling."""
    repo, build_root = pathlib.Path(repo).absolute(), pathlib.Path(build_root).absolute()
    request_path = pathlib.Path(request_path).absolute()
    extra_paths = [request_path, repo / "extracted" / "dsd" / "config.yaml",
                   *map(pathlib.Path, tool_paths)]
    initial_hashes = input_hashes(repo, extra_paths)
    rows = json.loads(request_path.read_text(encoding="utf-8"))
    if not isinstance(rows, list) or not rows:
        raise ValueError("data provenance requests must be a nonempty JSON list")
    index = DE.EvidenceIndex(repo)
    requests, images, seen = [], {}, set()
    for row in rows:
        if not isinstance(row, dict) or set(row) != {"module", "symbol", "source", "size"}:
            raise ValueError("each data provenance request needs exactly module, symbol, source, size")
        if any(not isinstance(row[key], str) or not row[key]
               for key in ("module", "symbol", "source")):
            raise ValueError("request module, symbol, and source must be nonempty strings")
        size = row["size"]
        if isinstance(size, str):
            size = int(size, 0)
        if type(size) is not int or size <= 0:
            raise ValueError("request size must be a positive integer or hexadecimal string")
        source = pathlib.PurePosixPath(row["source"])
        if (not source.parts or source.is_absolute() or ".." in source.parts or "\\" in row["source"]
                or ":" in row["source"] or source.parts[0] != "src"
                or source.suffix not in (".c", ".cpp")
                or not (repo / source).resolve().is_relative_to((repo / "src").resolve())
                or not (repo / source).is_file()):
            raise ValueError(f"request source must be an existing file inside src/: {source}")
        symbol = index.select(row["symbol"], row["module"])
        key = symbol["module"], symbol["symbol"]
        if key in seen:
            raise ValueError(f"duplicate data provenance request: {key}")
        seen.add(key)
        if symbol["configured_size"] is not None and size != symbol["configured_size"]:
            raise ValueError(f"{key}: requested size disagrees with configured size")
        record = index.record(symbol, size)
        section = record["section"]["name"]
        if section not in (".data", ".rodata", ".bss"):
            raise ValueError(f"{key}: unsupported data section {section}")
        owners = record["declared_owners"]
        if (len(owners) != 1 or owners[0]["source"] != str(source)
                or owners[0]["section"] != section or not owners[0]["complete"]
                or not owners[0]["covers_window"]):
            raise ValueError(f"{key}: request needs one complete source owner covering its entire range")
        module = symbol["module"]
        requests.append({"module": module, "symbol": symbol["symbol"],
                         "source": str(source), "size": size,
                         "address": symbol["address"], "section": section})
        if module not in images:
            sections = index.sections[module]
            base = min(s["start"] for s in sections)
            if module in ("arm9", "itcm", "dtcm"):
                filename = module + ".bin"
                retail = repo / "extracted" / "dsd" / "arm9" / filename
            else:
                filename = "arm9_" + module + ".bin"
                retail = repo / "extracted" / "dsd" / "arm9_overlays" / (module + ".bin")
            images[module] = {"base": base, "built": build_root / "build" / filename,
                              "retail": retail}
    # Include the retail reference before compilation, as well as before linking.
    for image in images.values():
        path = image["retail"]
        extra_paths.append(path)
        initial_hashes[str(path.absolute())] = hashlib.sha256(path.read_bytes()).hexdigest()
    plan = {"repo": repo, "build_root": build_root, "requests": requests,
            "module_images": images, "extra_paths": extra_paths,
            "extra_roots": [], "input_sha256": initial_hashes,
            "repository": DE.repository_state(repo)}
    require_unchanged(plan)
    return plan


def bind_generated_config(plan, config_root):
    """Bind the disposable configuration that dsd will actually consume."""
    require_unchanged(plan)
    config_root = pathlib.Path(config_root)
    if not (config_root / "config.yaml").is_file():
        raise ValueError(f"missing generated build configuration: {config_root}")
    plan["extra_roots"].append(config_root)
    plan["input_sha256"].update({str(path.absolute()): hashlib.sha256(path.read_bytes()).hexdigest()
                                 for path in config_root.rglob("*") if path.is_file()})
    require_unchanged(plan)


def before_link(plan):
    """Remove only known generated outputs, then capture the actual linker inputs."""
    import data_provenance as DP
    require_unchanged(plan)
    build = plan["build_root"].resolve()
    outputs = [build / "final_link.o", build / "final_link.o.xMAP"]
    outputs += [image["built"] for image in plan["module_images"].values()]
    for path in outputs:
        if not path.resolve().is_relative_to(build):
            raise ValueError(f"generated data proof output escapes build directory: {path}")
    for path in outputs:
        path.unlink(missing_ok=True)
    return DP.snapshot_link_inputs(plan["repo"], build, plan["requests"], plan["module_images"])


def after_link(plan, snapshot):
    """Called only after the pinned linker command returns successfully."""
    import data_provenance as DP
    require_unchanged(plan)
    report = DP.audit_link(snapshot)
    require_unchanged(plan)
    report["repository"] = plan["repository"]
    report["build_input_sha256"] = plan["input_sha256"]
    report["linker_invocation"] = "rombuild: pinned linker completed successfully"
    report["requested_size_basis"] = "caller assertion checked against configured bounds and emitted layout"
    return report
