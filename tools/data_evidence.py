"""Collect reviewable facts about global data without inventing names or proof.

    python tools/data_evidence.py --symbol data_02075b04
    python tools/data_evidence.py --module ov047 --symbol data_ov047_02112638
    python tools/data_evidence.py --symbol data_02075b04 --size 0x10 --json build/data.json

The window ending at the next configured boundary is NOT an inferred C++ size.
Relocations are configured references, source hits are textual references, and
delinks/manifest ownership is declared ownership. None proves the final link.
No source, config, manifest, or name is changed. Private ROM bytes are sampled
only in the requested output, which should normally live under ignored build/.
"""
import argparse
import bisect
import collections
import hashlib
import json
import pathlib
import re
import struct
import subprocess

import enroll
import relocs

REPO = pathlib.Path(__file__).resolve().parent.parent
SYMBOL = re.compile(r"^(\S+)\s+kind:(\S+).*?\baddr:0x([0-9a-fA-F]+)")
SIZE = re.compile(r"\bsize=(0x[0-9a-fA-F]+)")
ALIGN = re.compile(r"\balign:(\d+)")
LINK_CONSTANT = re.compile(r"from:0x([0-9a-fA-F]+) kind:link_time_const\(([^)\s]+)\)")


def digest(data):
    return hashlib.sha256(data).hexdigest()


def address(value):
    return f"0x{value:08x}"


def repository_state(repo):
    """Record dirty input honestly; HEAD alone does not identify a working tree."""
    command = ["git", "-c", f"safe.directory={repo.as_posix()}", "-C", str(repo)]
    try:
        head = subprocess.run(command + ["rev-parse", "HEAD"], capture_output=True,
                              text=True, check=True).stdout.strip()
        status = subprocess.run(command + ["status", "--porcelain", "--untracked-files=normal"],
                                capture_output=True, text=True, check=True).stdout
    except (OSError, subprocess.CalledProcessError):
        return {"head": None, "dirty": None, "status": "unavailable"}
    return {"head": head, "dirty": bool(status), "changes": status.splitlines()}


class EvidenceIndex:
    def __init__(self, repo):
        self.repo = pathlib.Path(repo)
        self.symbols = collections.defaultdict(list)
        self.addresses = collections.defaultdict(lambda: collections.defaultdict(list))
        self.sections = collections.defaultdict(list)
        self.claims = collections.defaultdict(list)
        self.relocations = []
        self.link_constants = []
        self.inputs = {}
        self.missing_relocations = []
        for path, module in relocs.module_universe(repo=self.repo):
            for line_number, line in enumerate(self.read(path).splitlines(), 1):
                match = SYMBOL.match(line)
                if not match:
                    continue
                name, kind, raw_address = match.groups()
                size = SIZE.search(kind)
                row = {"symbol": name, "module": module, "address": int(raw_address, 16),
                       "kind": kind, "configured_size": int(size[1], 16) if size else None,
                       "file": self.relative(path), "line": line_number}
                self.symbols[name].append(row)
                self.addresses[module][row["address"]].append(row)
            dl = path.with_name("delinks.txt")
            self.read(dl)
            header, entries = enroll.read_delinks_full(dl)
            for line in header:
                match = enroll.SEC_RE.match(line)
                if match:
                    name, lo, hi, kind = match.groups()
                    align = ALIGN.search(line)
                    self.sections[module].append({"name": name, "start": int(lo, 16),
                                                  "end": int(hi, 16), "kind": kind,
                                                  "alignment": int(align[1]) if align else None})
            for source, body in entries:
                complete = any(line.strip() == "complete" for line in body)
                for line in body:
                    match = enroll.ENTRY_SEC_RE.match(line)
                    if match:
                        name, lo, hi = match.groups()
                        self.claims[module].append({"source": source, "section": name,
                                                    "start": int(lo, 16), "end": int(hi, 16),
                                                    "complete": complete, "file": self.relative(dl)})
            rp = path.with_name("relocs.txt")
            if not rp.is_file():
                self.missing_relocations.append(self.relative(rp))
                continue
            seen_sites = set()
            for line_number, line in enumerate(self.read(rp).splitlines(), 1):
                line = line.split("#", 1)[0].strip()
                if not line:
                    continue
                match = relocs._REL_RE.fullmatch(line)
                if not match:
                    constant = LINK_CONSTANT.fullmatch(line)
                    if not constant:
                        raise ValueError(f"{self.relative(rp)}:{line_number}: malformed relocation")
                    site = int(constant[1], 16)
                    if site in seen_sites:
                        raise ValueError(f"{self.relative(rp)}:{line_number}: duplicate relocation site {address(site)}")
                    seen_sites.add(site)
                    self.link_constants.append({"source_module": module, "site": address(site),
                                                "constant": constant[2], "file": self.relative(rp),
                                                "line": line_number, "target": "not-resolved"})
                    continue
                raw_site, kind, raw_target, target_module = match.groups()
                site, target = int(raw_site, 16), int(raw_target, 16)
                target_module = relocs.normalize_module(target_module)
                if site in seen_sites:
                    raise ValueError(f"{self.relative(rp)}:{line_number}: duplicate relocation site {address(site)}")
                seen_sites.add(site)
                self.relocations.append({"source_module": module, "site": site,
                                         "kind": kind, "target_module": target_module,
                                         "target": target, "file": self.relative(rp), "line": line_number})
        self.boundaries = {m: sorted(rows) for m, rows in self.addresses.items()}

    def relative(self, path):
        return pathlib.Path(path).relative_to(self.repo).as_posix()

    def read(self, path):
        data = pathlib.Path(path).read_bytes()
        self.inputs[self.relative(path)] = digest(data)
        return data.decode("utf-8")

    def select(self, symbol, module=None):
        rows = self.symbols.get(symbol, [])
        if module:
            module = relocs.normalize_module(module)
            rows = [r for r in rows if r["module"] == module]
        if len(rows) != 1:
            homes = ", ".join(f"{r['module']}:{address(r['address'])}" for r in rows)
            raise ValueError(f"{symbol}: expected one configured home, found {len(rows)}"
                             + (f" ({homes}); select --module" if rows else ""))
        row = rows[0]
        if row["kind"].startswith("function"):
            raise ValueError(f"{symbol}: function symbols are not data objects")
        return row

    def context(self, module, at):
        """Names at the nearest boundary, never a claim of data-object membership."""
        boundaries = self.boundaries.get(module, [])
        pos = bisect.bisect_right(boundaries, at) - 1
        if pos < 0:
            return None
        start = boundaries[pos]
        rows = self.addresses[module][start]
        functions = [r for r in rows if r["kind"].startswith("function")
                     and r["configured_size"] is not None
                     and at < start + r["configured_size"]]
        exact = start == at
        selected = functions or rows
        return {"symbols": sorted(r["symbol"] for r in selected), "address": address(start),
                "offset": at - start,
                "relation": "exact" if exact else "inside-configured-function" if functions
                else "after-nearest-symbol-boundary"}

    def record(self, row, size=None):
        module, start = row["module"], row["address"]
        sections = [s for s in self.sections[module] if s["start"] <= start < s["end"]]
        if len(sections) != 1:
            raise ValueError(f"{row['symbol']}: expected one containing section, found {len(sections)}")
        section = sections[0]
        following = self.boundaries[module][bisect.bisect_right(self.boundaries[module], start):]
        boundary = min(following[0] if following else section["end"], section["end"])
        if size is not None:
            length, basis = size, "user-requested-window"
        elif row["configured_size"] is not None:
            length, basis = row["configured_size"], "configured-size-claim"
        else:
            length, basis = boundary - start, "next-configured-boundary-upper-bound"
        if length <= 0 or start + length > section["end"]:
            raise ValueError(f"{row['symbol']}: requested window must be nonempty and inside {section['name']}")
        end = start + length
        aliases = sorted(r["symbol"] for r in self.addresses[module][start])
        claims = []
        for claim in self.claims[module]:
            if claim["start"] < end and start < claim["end"]:
                claims.append({**claim, "start": address(claim["start"]),
                               "end": address(claim["end"]),
                               "contains_address": claim["start"] <= start < claim["end"],
                               "overlap_start": address(max(start, claim["start"])),
                               "overlap_end": address(min(end, claim["end"])),
                               "covers_window": claim["start"] <= start and end <= claim["end"]})
        incoming, outgoing = [], []
        for reloc in self.relocations:
            incoming_hit = reloc["target_module"] == module and start <= reloc["target"] < end
            outgoing_hit = reloc["source_module"] == module and start <= reloc["site"] < end
            if not (incoming_hit or outgoing_hit):
                continue
            item = {**reloc, "site": address(reloc["site"]), "target": address(reloc["target"]),
                    "source_context": self.context(reloc["source_module"], reloc["site"]),
                    "target_context": self.context(reloc["target_module"], reloc["target"])}
            if incoming_hit:
                incoming.append({**item, "target_offset": reloc["target"] - start})
            if outgoing_hit:
                outgoing.append({**item, "site_offset": reloc["site"] - start})
        return {
            "id": f"{module}:{address(start)}", "symbol": row["symbol"], "aliases": aliases,
            "module": module, "address": address(start), "configured_kind": row["kind"],
            "symbol_location": {"file": row["file"], "line": row["line"]},
            "section": {**section, "start": address(section["start"]), "end": address(section["end"])},
            "object_size": row["configured_size"], "object_alignment": None,
            "window": {"start": address(start), "end": address(end), "size": length, "basis": basis},
            "declared_owners": claims,
            "configured_references": {"incoming": incoming, "outgoing": outgoing,
                                      "missing_files": self.missing_relocations},
            "source_text_references": [], "manifest_claims": [],
            "interpretation": {"type": "unreviewed", "name": "unreviewed", "historical_owner": "unreviewed"},
            "verification": {"compiled_bytes": "not-run", "linked_source_contribution": "not-run",
                             "storage_layout": "not-run"},
            "limits": ["Configured references are not exhaustive runtime reads/writes.",
                       "Text references include declarations/comments and cannot identify runtime access or historical ownership.",
                       "A window may include padding or unnamed neighbors; it is not a recovered object size.",
                       "Declared ownership and recorded manifest proof have not been reverified."],
        }

    def attach_text_evidence(self, records):
        by_name = collections.defaultdict(list)
        for record in records:
            for name in record["aliases"]:
                by_name[name].append(record)
        pattern = re.compile(r"(?<![A-Za-z0-9_$])(?:"
                             + "|".join(re.escape(n) for n in sorted(by_name, key=len, reverse=True))
                             + r")(?![A-Za-z0-9_$])")
        for folder in ("src", "include"):
            for path in sorted((self.repo / folder).rglob("*")):
                if path.suffix not in (".c", ".cpp", ".h", ".hpp") or not path.is_file():
                    continue
                content = path.read_bytes()
                text = content.decode("utf-8", errors="replace")
                self.inputs[self.relative(path)] = digest(content)
                if not pattern.search(text):
                    continue
                for line_number, line in enumerate(text.splitlines(), 1):
                    for name in sorted(set(pattern.findall(line))):
                        for record in by_name[name]:
                            record["source_text_references"].append({"file": self.relative(path),
                                "line": line_number, "symbol": name, "text": line.strip()})
        for path in sorted((self.repo / "config" / "tu_manifest.d").rglob("*.json")):
            text = self.read(path)
            manifest = json.loads(text)
            for record in records:
                if relocs.normalize_module(manifest.get("module", "")) != record["module"]:
                    continue
                matches = []
                for field in ("data", "rodata", "bss", "ctor", "init", "exception", "exceptix"):
                    for item in manifest.get(field, []):
                        if isinstance(item, dict) and item.get("symbol") in record["aliases"]:
                            matches.append({"section": field, "claim": item})
                if matches:
                    record["manifest_claims"].append({"file": self.relative(path), "id": manifest.get("id"),
                        "source": manifest.get("promoted_source") or manifest.get("source"),
                        "status": manifest.get("status"), "records": matches})


def attach_rom_evidence(records, registry):
    images = {relocs.normalize_module(m["name"]): m for m in registry}
    cache = {}
    for record in records:
        if record["section"]["kind"] == "bss":
            record["rom"] = {"status": "zero-fill-storage", "note": "No stored ROM payload; layout and startup initialization require separate proof."}
            continue
        image = images.get(record["module"])
        if image is None or not pathlib.Path(image["bin"]).is_file():
            record["rom"] = {"status": "unavailable"}
            continue
        key = str(image["bin"])
        if key not in cache:
            data = pathlib.Path(image["bin"]).read_bytes()
            cache[key] = data, digest(data)
        data, image_hash = cache[key]
        offset = int(record["address"], 16) - image["base"]
        size = record["window"]["size"]
        if offset < 0 or offset + size > len(data):
            record["rom"] = {"status": "window-outside-image", "module_sha256": image_hash}
            continue
        raw = data[offset:offset + size]
        sample = raw[:32]
        words = [struct.unpack_from("<I", sample, i)[0] for i in range(0, len(sample) & ~3, 4)]
        record["rom"] = {"status": "observed", "module_sha256": image_hash,
                         "module_base": address(image["base"]), "window_sha256": digest(raw),
                         "observed_bytes": len(raw), "preview_hex": sample.hex(" "),
                         "preview_u32_le": words, "preview_truncated": len(raw) > len(sample)}


def build_report(repo, symbols, module=None, size=None, read_rom=True, registry=None):
    if not symbols:
        raise ValueError("at least one symbol is required")
    if size is not None and len(symbols) != 1:
        raise ValueError("--size requires exactly one --symbol")
    index = EvidenceIndex(repo)
    records = [index.record(index.select(name, module), size) for name in dict.fromkeys(symbols)]
    index.attach_text_evidence(records)
    if read_rom:
        if registry is None:
            import modules
            if pathlib.Path(repo).resolve() != modules.REPO.resolve():
                raise ValueError("a non-default repository needs its own ROM registry")
            registry = modules.modules()
        attach_rom_evidence(records, registry)
    else:
        for record in records:
            record["rom"] = {"status": "not-requested"}
    tool_dir = pathlib.Path(__file__).resolve().parent
    tool_hashes = {name: digest((tool_dir / name).read_bytes())
                   for name in ("data_evidence.py", "enroll.py", "relocs.py", "modules.py")}
    return {"schema_version": 1, "repository": repository_state(pathlib.Path(repo)),
            "tool_sha256": tool_hashes,
            "reference_inventory": {"link_time_constants": index.link_constants,
                                     "missing_files": index.missing_relocations},
            "input_sha256": dict(sorted(index.inputs.items())), "objects": records,
            "purpose": "Evidence collection; not a source, layout, or byte-match acceptance gate."}


def summary(report):
    lines = ["Global-data evidence (compiled-byte and final-link verification: NOT RUN)"]
    for record in report["objects"]:
        window = record["window"]
        lines += [f"\n{record['id']}  {record['symbol']}",
                  f"  {record['section']['name']} window {window['size']} bytes ({window['basis']})",
                  f"  ROM observation: {record['rom']['status']}",
                  f"  Configured references: {len(record['configured_references']['incoming'])} incoming, "
                  f"{len(record['configured_references']['outgoing'])} outgoing",
                  f"  Source text hits: {len(record['source_text_references'])}; "
                  f"manifest records: {len(record['manifest_claims'])}"]
        for owner in record["declared_owners"]:
            lines.append(f"  Declared owner: {owner['source']} (complete={owner['complete']}, "
                         f"covers window={owner['covers_window']}; not reverified)")
        if not record["declared_owners"]:
            lines.append("  No declared delinks owner at this address")
        if "preview_u32_le" in record["rom"]:
            lines.append(f"  Raw little-endian words: {record['rom']['preview_u32_le']}")
        lines.append("  Type / descriptive name / historical owner: unreviewed")
    return "\n".join(lines)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--symbol", action="append", required=True, help="configured name; repeat for a batch")
    parser.add_argument("--module", help="module identity, required when a name has several homes")
    parser.add_argument("--size", type=lambda s: int(s, 0), help="explicit observation window, not proof of object size")
    parser.add_argument("--no-rom", action="store_true", help="collect only configuration and source evidence")
    parser.add_argument("--json", type=pathlib.Path, help="write complete evidence (prefer ignored build/)")
    args = parser.parse_args(argv)
    try:
        report = build_report(REPO, args.symbol, args.module, args.size, not args.no_rom)
    except (ValueError, OSError) as error:
        parser.exit(2, f"data evidence: {error}\n")
    if args.json:
        try:
            args.json.parent.mkdir(parents=True, exist_ok=True)
            args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        except OSError as error:
            parser.exit(2, f"data evidence: cannot write {args.json}: {error}\n")
    print(summary(report))
    if args.json:
        print(f"\nFull evidence: {args.json}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
