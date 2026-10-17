"""Observe exact source-object providers in a fresh mwldarm link.

The build runner must remove the requested outputs before snapshot_link_inputs,
invoke the linker successfully, then call audit_link. This module verifies the
actual prepared objects, module-qualified map/ELF identities, and linked bytes.
It does not reconstruct original variable names or prove startup initialization.
"""
from __future__ import annotations

import collections
import hashlib
import io
import pathlib
import re

from elftools.elf.elffile import ELFFile
from elftools.common.exceptions import ELFError

import enroll
import relocs


_MODULE = r"(?:arm9|itcm|dtcm|ov\d+)"
_MAP_HEADING = re.compile(rf"^# \.({_MODULE})\s*$", re.IGNORECASE)
_MAP_ROW = re.compile(
    r"^\s+([0-9a-fA-F]{8})\s+([0-9a-fA-F]{8})\s+(\.\S+)\s+(.+?)\t\(([^\r\n]+\.o)\)\s*$")
_EXCEPTION_SUMMARY = re.compile(
    r"^\s+[0-9a-fA-F]{8}\s+[0-9a-fA-F]{8}\s+Exception Table Index\t\(\)\s*$")
_LCF_OPEN = re.compile(rf"^\s*\.({_MODULE})\s*:\s*\{{\s*$", re.IGNORECASE)
_LCF_CLOSE = re.compile(r"^\s*}\s*>\s*(\S+)\s*$")
_SELECTOR = re.compile(r"^\s*([^\s()]+\.o)\((\.[^\s()]+)\)\s*$")


def _sha(path):
    return hashlib.sha256(pathlib.Path(path).read_bytes()).hexdigest()


def _module(value):
    return relocs.normalize_module(str(value).lower())


def _integer(value):
    if isinstance(value, bool) or not isinstance(value, (int, str)):
        raise ValueError("address and size must be integers")
    return int(value, 0) if isinstance(value, str) else int(value)


def parse_map(text):
    """Read the address table, ignoring the incomplete dependency-closure table."""
    module, rows, seen = None, [], set()
    for number, line in enumerate(text.splitlines(), 1):
        heading = _MAP_HEADING.fullmatch(line)
        if heading:
            module = _module(heading[1])
            if module in seen:
                raise ValueError(f"map:{number}: repeated module table {module}")
            seen.add(module)
            continue
        if line == "# Exception index":
            continue
        if line.startswith("# "):
            module = None
        if module is None:
            continue
        match = _MAP_ROW.fullmatch(line)
        if match:
            addr, size, section, display, obj = match.groups()
            rows.append({"module": module, "address": int(addr, 16), "size": int(size, 16),
                         "section": section, "display_name": display,
                         "object": obj, "line": number})
        elif _EXCEPTION_SUMMARY.fullmatch(line):
            # Linker-generated exception index, not an input-object provider.
            continue
        elif re.match(r"^\s+[0-9a-fA-F]{8}\s", line):
            raise ValueError(f"map:{number}: unsupported address-table row")
    if not seen:
        raise ValueError("map contains no module-qualified address tables")
    return rows


def parse_lcf_selectors(text):
    """Identify exact object/section selectors inside generated module blocks."""
    module, rows = None, []
    for number, line in enumerate(text.splitlines(), 1):
        opening = _LCF_OPEN.fullmatch(line)
        if opening:
            if module is not None:
                raise ValueError(f"LCF:{number}: nested module blocks")
            module = _module(opening[1])
            continue
        closing = _LCF_CLOSE.fullmatch(line)
        if closing and module is not None:
            if _module(closing[1]) != module:
                raise ValueError(f"LCF:{number}: output module disagrees with block")
            module = None
            continue
        selector = _SELECTOR.fullmatch(line)
        if selector and module is not None:
            rows.append({"module": module, "object": selector[1],
                         "section": selector[2], "line": number})
    if module is not None:
        raise ValueError("LCF has an unterminated module block")
    return rows


def _object_paths(path, repo):
    out = collections.defaultdict(list)
    for number, line in enumerate(pathlib.Path(path).read_text(encoding="utf-8").splitlines(), 1):
        value = line.strip()
        if not value:
            continue
        if value.startswith('"'):
            if not value.endswith('"'):
                raise ValueError(f"objects.txt:{number}: unmatched quote")
            value = value[1:-1]
        candidate = pathlib.Path(value)
        if candidate.suffix != ".o" or '"' in value:
            raise ValueError(f"objects.txt:{number}: expected a single object path")
        candidate = (candidate if candidate.is_absolute() else repo / candidate).resolve()
        out[candidate.name].append(candidate)
    return dict(out)


def object_symbol(path, symbol):
    """Exact definition in the prepared object handed to the linker."""
    elf = ELFFile(io.BytesIO(pathlib.Path(path).read_bytes()))
    if elf["e_type"] != "ET_REL":
        raise ValueError("provider is not a relocatable object")
    table = elf.get_section_by_name(".symtab")
    matches = [] if table is None else [s for s in table.iter_symbols() if s.name == symbol]
    if len(matches) != 1 or not isinstance(matches[0]["st_shndx"], int):
        raise ValueError("prepared object has no unique ordinary definition of the symbol")
    sym = matches[0]
    sec = elf.get_section(sym["st_shndx"])
    size, offset = int(sym["st_size"]), int(sym["st_value"])
    if sym["st_info"]["type"] != "STT_OBJECT" or size <= 0 \
            or offset < 0 or offset + size > sec["sh_size"]:
        raise ValueError("prepared object does not define a complete nonempty data object")
    return {"address": offset, "size": size, "section": sec.name,
            "type": sym["st_info"]["type"], "binding": sym["st_info"]["bind"],
            "section_type": sec["sh_type"], "section_size": sec["sh_size"],
            "section_alignment": int(sec["sh_addralign"] or 1)}


def linked_symbol(path, symbol, module):
    """Read final symbol metadata; final ELF sections do not contain module bytes."""
    elf = ELFFile(io.BytesIO(pathlib.Path(path).read_bytes()))
    if elf["e_type"] != "ET_EXEC":
        raise ValueError("final link is not an executable ELF")
    table = elf.get_section_by_name(".symtab")
    rows = []
    if table is not None:
        for sym in table.iter_symbols():
            if sym.name != symbol or not isinstance(sym["st_shndx"], int):
                continue
            section = elf.get_section(sym["st_shndx"])
            if _module(section.name) == module:
                rows.append({"address": int(sym["st_value"]), "size": int(sym["st_size"]),
                             "section": section.name, "type": sym["st_info"]["type"],
                             "binding": sym["st_info"]["bind"], "module_base": int(section["sh_addr"])})
    if len(rows) != 1:
        raise ValueError("final ELF has no unique symbol definition in the requested module")
    return rows[0]


def _normalize_request(raw, repo, build_root):
    if not isinstance(raw, dict):
        raise ValueError("each request must be an object")
    request = dict(raw)
    for key in ("module", "symbol", "address", "size", "source", "section"):
        if key not in request:
            raise ValueError(f"request lacks {key}")
    module = _module(request["module"])
    if not re.fullmatch(_MODULE, module):
        raise ValueError(f"unsupported module {module!r}")
    start, size = _integer(request["address"]), _integer(request["size"])
    if start < 0 or size <= 0 or start + size > 0x100000000:
        raise ValueError("requested data range must be nonempty and fit a 32-bit address space")
    section = request["section"]
    if section not in (".data", ".rodata", ".bss"):
        raise ValueError("only .data, .rodata, and .bss are supported")
    source = str(request["source"]).replace("\\", "/")
    rel = pathlib.PurePosixPath(source)
    if rel.is_absolute() or ".." in rel.parts or not rel.parts or rel.parts[0] != "src" \
            or rel.suffix not in (".c", ".cpp"):
        raise ValueError("source must be a repository-relative src/ C or C++ path")
    source_path = (repo / source).resolve()
    if not source_path.is_relative_to(repo) or not source_path.is_file():
        raise ValueError("source is missing or escapes the repository")
    directory = repo / "config" / "arm9"
    if module.startswith("ov"):
        directory = directory / "overlays" / module
    elif module != "arm9":
        directory = directory / module
    syms = directory / "symbols.txt"
    symbol = request["symbol"]
    if not isinstance(symbol, str) or not symbol or any(c.isspace() for c in symbol):
        raise ValueError("symbol must be a configured nonempty name")
    homes = [int(m[2], 16) for line in syms.read_text(encoding="utf-8").splitlines()
             if (m := relocs._SYM_RE.match(line)) and m[1] == symbol]
    if homes != [start]:
        raise ValueError(f"{module}:{symbol}: configured home does not uniquely equal the requested address")
    header, entries = enroll.read_delinks_full(directory / "delinks.txt")
    sections = []
    for line in header:
        match = enroll.SEC_RE.match(line)
        if match and match[1] == section:
            sections.append({"name": section, "start": int(match[2], 16),
                             "end": int(match[3], 16), "kind": match[4]})
    if len(sections) != 1 or not sections[0]["start"] <= start < start + size <= sections[0]["end"]:
        raise ValueError("requested range is not wholly inside its configured module section")
    owners = []
    for owner, body in entries:
        if not any(line.strip() == "complete" for line in body):
            continue
        for line in body:
            match = enroll.ENTRY_SEC_RE.match(line)
            if match and match[1] == section and int(match[2], 16) < start + size \
                    and start < int(match[3], 16):
                owners.append((owner, int(match[2], 16), int(match[3], 16)))
    if len(owners) != 1 or owners[0][0] != source or not owners[0][1] <= start < start + size <= owners[0][2]:
        raise ValueError("requested range needs exactly one complete source owner covering every byte")
    return {"module": module, "symbol": symbol, "address": start, "size": size,
            "section": section, "source": source, "config_section": sections[0],
            "expected_object": str((build_root / pathlib.Path(source).with_suffix(".o")).resolve()),
            "config_directory": str(directory), "source_path": str(source_path)}


def snapshot_link_inputs(repo, build_root, requests, module_images):
    """Bind prepared linker inputs after the runner has removed expected outputs.

    The caller separately binds source/compiler inputs before compilation. Merely
    making this snapshot does not establish that a compiler or linker ran.
    """
    repo, build_root = pathlib.Path(repo).resolve(), pathlib.Path(build_root).resolve()
    if not requests:
        raise ValueError("at least one data provenance request is required")
    normalized = [_normalize_request(r, repo, build_root) for r in requests]
    identities = [(r["module"], r["symbol"]) for r in normalized]
    if len(set(identities)) != len(identities):
        raise ValueError("duplicate module/symbol request")
    lcf, objects = build_root / "arm9.lcf", build_root / "objects.txt"
    paths = _object_paths(objects, repo)
    duplicate = [name for name, values in paths.items() if len(values) != 1]
    if duplicate:
        raise ValueError("ambiguous linker object basenames: " + ", ".join(sorted(duplicate)[:5]))
    inputs = {lcf, objects, *[p for values in paths.values() for p in values]}
    images = {}
    for module in {r["module"] for r in normalized}:
        candidates = [value for name, value in module_images.items() if _module(name) == module]
        if len(candidates) != 1:
            raise ValueError(f"{module}: expected exactly one module image record")
        value = candidates[0]
        built, retail = pathlib.Path(value["built"]).resolve(), pathlib.Path(value["retail"]).resolve()
        if not built.is_relative_to(build_root) or built == retail:
            raise ValueError("linked module output must be inside the build root and distinct from retail")
        images[module] = {"base": _integer(value["base"]), "built": str(built), "retail": str(retail)}
        inputs.add(retail)
    for request in normalized:
        inputs.add(pathlib.Path(request["source_path"]))
        directory = pathlib.Path(request["config_directory"])
        inputs.update(directory / name for name in ("symbols.txt", "delinks.txt", "relocs.txt"))
    tool_dir = pathlib.Path(__file__).resolve().parent
    inputs.update(tool_dir / name for name in ("data_provenance.py", "enroll.py", "relocs.py"))
    linker = repo / "tools" / "mwccarm" / "1.2" / "sp2p3" / "mwldarm.exe"
    if linker.is_file():
        inputs.add(linker)
    outputs = [build_root / "final_link.o", build_root / "final_link.o.xMAP",
               *[pathlib.Path(v["built"]) for v in images.values()]]
    present = [str(p) for p in outputs if p.exists()]
    if present:
        raise ValueError("remove expected generated outputs before the link snapshot: " + ", ".join(present))
    return {"schema_version": 1, "repo": str(repo), "build_root": str(build_root),
            "requests": normalized, "module_images": images,
            "object_paths": {name: [str(p) for p in values] for name, values in paths.items()},
            "input_sha256": {str(p): _sha(p) for p in sorted(inputs)},
            "expected_outputs": [str(p) for p in outputs]}


def verify_record(request, map_rows, selectors, object_paths, linked_elf_path, image):
    """Cross-check one exact requested symbol extent in already-bound artifacts."""
    module, symbol = request["module"], request["symbol"]
    start, size, section = request["address"], request["size"], request["section"]
    result = {"module": module, "symbol": symbol, "address": start, "size": size,
              "size_basis": "caller-requested extent, checked against emitted and linked symbol",
              "source": request["source"], "section": section, "status": "refused",
              "provider": None, "linked_bytes": {"status": "not-verified"},
              "storage_layout": {"status": "not-verified"},
              "pointer_target_semantics": "not-run", "startup_initialization": "not-run", "errors": []}
    try:
        overlapping = [r for r in map_rows if r["module"] == module and r["size"] > 0
                       and r["address"] < start + size and start < r["address"] + r["size"]]
        exact = [r for r in overlapping if r["address"] == start and r["size"] == size
                 and r["section"] == section]
        if not exact:
            raise ValueError("map has no exact module-qualified provider for the entire requested extent")
        providers = {r["object"] for r in overlapping}
        if len(providers) != 1:
            raise ValueError("map reports ambiguous or partial providers across the requested extent")
        provider = next(iter(providers))
        if provider.startswith("_dsd_gap@"):
            raise ValueError("requested storage is supplied by ROM-gap fallback")
        candidates = object_paths.get(provider, [])
        if len(candidates) != 1:
            raise ValueError("map provider does not resolve to exactly one objects.txt path")
        path = pathlib.Path(candidates[0]).resolve()
        if path != pathlib.Path(request["expected_object"]).resolve():
            raise ValueError("actual map provider is not the requested source's prepared object")
        selected = [r for r in selectors if r["object"] == provider and r["section"] == section]
        if len(selected) != 1 or selected[0]["module"] != module:
            raise ValueError("LCF must select the provider's section exactly once in the requested module")
        emitted = object_symbol(path, symbol)
        linked = linked_symbol(linked_elf_path, symbol, module)
        if emitted["size"] != size or emitted["section"] != section:
            raise ValueError("prepared object symbol size/section differs from the requested extent")
        if linked["address"] != start or linked["size"] != size or linked["type"] != "STT_OBJECT":
            raise ValueError("final ELF symbol address/size/type disagrees with the request and map")
        if _module(linked["section"]) != module or linked.get("module_base", image["base"]) != image["base"]:
            raise ValueError("linked ELF module identity/base disagrees with the module image")
        alignment = emitted["section_alignment"]
        if alignment <= 0 or alignment & (alignment - 1) or (start - emitted["address"]) % alignment:
            raise ValueError("linked placement violates the prepared input section's alignment")
        bounds = request["config_section"]
        if not bounds["start"] <= start < start + size <= bounds["end"]:
            raise ValueError("requested extent exceeds configured section bounds")
        result["provider"] = {"object": str(path), "sha256": _sha(path),
                              "map_rows": exact, "lcf_selector": selected[0],
                              "emitted_symbol": emitted, "linked_symbol": linked}
        if section == ".bss":
            if emitted["section_type"] != "SHT_NOBITS" or bounds["kind"] != "bss":
                raise ValueError("BSS needs actual NOBITS input storage and configured BSS bounds")
            result["linked_bytes"] = {"status": "not-applicable-no-ROM-payload"}
            result["storage_layout"] = {"status": "verified", "bytes": size, "alignment": alignment}
            result["status"] = "source-storage-layout-verified"
        else:
            if emitted["section_type"] != "SHT_PROGBITS" or bounds["kind"] == "bss":
                raise ValueError("initialized data requires stored input bytes")
            built, retail = pathlib.Path(image["built"]).read_bytes(), pathlib.Path(image["retail"]).read_bytes()
            offset = start - image["base"]
            if offset < 0 or offset + size > min(len(built), len(retail)):
                raise ValueError("requested initialized range lies outside a module image")
            candidate, reference = built[offset:offset + size], retail[offset:offset + size]
            differing = sum(a != b for a, b in zip(candidate, reference))
            result["linked_bytes"] = {"status": "differs" if differing else "verified",
                                      "compared_bytes": size, "differing_bytes": differing,
                                      "linked_sha256": hashlib.sha256(candidate).hexdigest(),
                                      "retail_sha256": hashlib.sha256(reference).hexdigest()}
            if differing:
                raise ValueError(f"{differing} linked data byte(s) differ from retail")
            result["storage_layout"] = {"status": "verified", "bytes": size, "alignment": alignment}
            result["status"] = "source-bytes-verified"
    except (OSError, ValueError, ELFError) as error:
        result["errors"].append(str(error))
    return result


def audit_link(snapshot):
    """Verify a successful fresh link without trusting old map or object receipts."""
    errors = []
    for path, expected in snapshot["input_sha256"].items():
        try:
            if _sha(path) != expected:
                errors.append(f"link input changed after snapshot: {path}")
        except OSError:
            errors.append(f"link input disappeared after snapshot: {path}")
    artifacts = {}
    for path in snapshot["expected_outputs"]:
        try:
            artifacts[path] = _sha(path)
        except OSError:
            errors.append(f"fresh link output is missing: {path}")
    records = []
    if not errors:
        root = pathlib.Path(snapshot["build_root"])
        try:
            map_rows = parse_map((root / "final_link.o.xMAP").read_text(encoding="utf-8"))
            selectors = parse_lcf_selectors((root / "arm9.lcf").read_text(encoding="utf-8"))
            for request in snapshot["requests"]:
                records.append(verify_record(request, map_rows, selectors, snapshot["object_paths"],
                                             root / "final_link.o", snapshot["module_images"][request["module"]]))
        except (OSError, ValueError) as error:
            errors.append(str(error))
        # Readers above reopen files. Bind the observations to the same bytes
        # fingerprinted before the audit, including objects and output images
        # that the runner's source/config freshness check does not cover.
        for label, fingerprints in (("link input", snapshot["input_sha256"]),
                                    ("link output", artifacts)):
            for path, expected in fingerprints.items():
                try:
                    if _sha(path) != expected:
                        errors.append(f"{label} changed during audit: {path}")
                except OSError:
                    errors.append(f"{label} disappeared during audit: {path}")
    return {"schema_version": 1, "ok": not errors and bool(records) and all(not r["errors"] for r in records),
            "errors": errors, "input_sha256": snapshot["input_sha256"],
            "artifact_sha256": artifacts, "objects": records,
            "scope": "Exact requested symbol storage only; names, original ownership, pointer semantics, and startup behavior are not proven."}
