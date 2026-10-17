#!/usr/bin/env python3
"""Changed-code formatting and source-pattern regression check; see notes/source-style.md.

This is not a C++ parser or a byte/relocation validator. No files are rewritten.
With --base, compare its merge base with HEAD to the working tree (including new
untracked source and header files). Renames retain their original pattern debt.
Without --base, explicit --files are checked in full, useful for fixtures.
"""
import argparse
import collections
import difflib
import json
import os
import pathlib
import re
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET
from dataclasses import dataclass

import delaunder
import msvc_arms

ROOT = pathlib.Path(__file__).resolve().parent.parent
FORMAT_VERSION = "19.1.7"
SUFFIXES = {".c", ".cpp", ".h", ".hpp"}
# Deliberately narrow patterns, independent of the published CONVERTED metric.
RULES = {
    "raw-offset": re.compile(
        r"\(\s*(?:(?:unsigned|signed)\s+)?\w+\s*\*\s*\)\s*\(\s*\w+\s*\+\s*0x[0-9a-fA-F]+"
        r"|\(\s*char\s*\*\s*\)\s*[\w()]+\s*\+\s*0x[0-9a-fA-F]+"),
    "manual-vtable": re.compile(r"(?:->|\.)\s*(?:vtable|vptr|vtbl)\s*\[[^\]\n]+\]"),
    "handspelled-member": re.compile(r"\b_ZN\w+"),
    "unknown-field": re.compile(r"\bunk_[0-9a-fA-F]+\b"),
    "modern-nullptr": re.compile(r"\bnullptr\b"),
}


class StyleError(Exception):
    """Missing inputs or unsupported configuration: never a passing check."""


class UnsafeFormat(StyleError):
    """Formatting is unsupported for these matching-sensitive inputs."""


# Preserve preprocessing token spelling/order, including literal boundaries.
# This is a C/C++03 lexer guard, not a parser or a substitute for compiler proof.
TOKEN = re.compile(
    r'//[^\n]*|/\*.*?\*/|(?:L)?"(?:\\.|[^"\\])*"|(?:L)?\'(?:\\.|[^\'\\])*\''
    r'|[A-Za-z_]\w*|(?:[0-9]|\.[0-9])(?:[eEpP][+-]|[0-9A-Za-z_.])*'
    r'|>>=|<<=|->\*|\.\*|\.\.\.|##|::|->|\+\+|--|<<|>>|<=|>=|==|!=|&&|\|\|'
    r'|[+*/%&|^!=<>-]=|\S', re.S)


def tokens(text):
    return [m.group() for m in TOKEN.finditer(text)
            if not m.group().startswith(('//', '/*'))]


def direct_format_risks(text):
    logical = text.replace('\\\r\n', '').replace('\\\n', '')
    mask = delaunder.code_mask(logical)
    code = ''.join(c if mask[i] or c == '\n' else ' ' for i, c in enumerate(logical))
    reasons = []
    if any(re.match(r'^(?://|/\*)\s*clang-format\s+off\b', m.group())
           for m in TOKEN.finditer(logical) if m.group().startswith(('//', '/*'))):
        reasons.append('clang-format off suppresses formatting coverage')
    if '%:' in code or re.search(r'\?\?[=()/\'<>!-]', code):
        reasons.append('alternate preprocessing spelling requires compiler proof')
    if re.search(r'\b(?:asm|__asm|__asm__)\b', code):
        reasons.append('assembly requires original instruction line boundaries')
    if re.search(r'\b__(?:LINE|COUNTER|DATE|TIME|TIMESTAMP|BASE_FILE|INCLUDE_LEVEL)__\b', code):
        reasons.append('line/context-sensitive preprocessing builtin')
    if re.search(r'^\s*#\s*line\b', code, re.M):
        reasons.append('line directive')
    for m in re.finditer(r'^\s*#\s*define\s+(\w+)([^\n]*)', code, re.M):
        if '##' in m.group(2):
            reasons.append('token-pasting macro ' + m.group(1))
        elif re.search(r'(?<!#)#(?!#)', m.group(2)):
            reasons.append('stringifying macro ' + m.group(1))
    return reasons


class FormatSafety:
    """Conservative local/include-context protection; unresolved includes defer.

    Headers also inspect reverse consumers, since a consumer may define a
    stringifying macro before including an otherwise innocent header.
    """
    def __init__(self, root):
        self.root = root.resolve()
        self.cache = {}
        self.reverse = None
        self.uncertain_consumers = set()

    def node(self, path, text=None):
        if text is None and path in self.cache:
            return self.cache[path]
        if text is None:
            text = (self.root / path).read_text(encoding='utf-8')
        risks = direct_format_risks(text)
        targets = []
        # Do not mistake commented-out includes for dependencies.
        view = msvc_arms.mwcc_side(text)
        view = view.replace('\\\r\n', '').replace('\\\n', '')
        view = TOKEN.sub(lambda m: re.sub(r'[^\n]', ' ', m.group())
                         if m.group().startswith(('//', '/*')) else m.group(), view)
        mask = delaunder.code_mask(view)
        for m in re.finditer(r'^[ \t]*#[ \t]*include\b[ \t]*([^\n]+)', view, re.M):
            hash_at = view.index('#', m.start(), m.end())
            if not mask[hash_at]:
                continue
            spelling = re.match(r'["<]([^">]+)[">]', m.group(1).strip())
            if spelling is None:
                risks.append('unresolved computed include')
                continue
            name = spelling.group(1)
            candidates = [self.root / 'include' / name]
            if m.group(1).lstrip().startswith('"'):
                candidates.insert(0, (self.root / path).parent / name)
            target = next((p.resolve() for p in candidates if p.is_file()), None)
            if target is None or not target.is_relative_to(self.root):
                risks.append('unresolved/external include ' + name)
            else:
                targets.append(target.relative_to(self.root).as_posix())
        self.cache[path] = (risks, targets)
        return self.cache[path]

    def build_reverse(self):
        self.reverse = {}
        pending = []
        for directory in ('src', 'include'):
            folder = self.root / directory
            if folder.exists():
                pending.extend(p.relative_to(self.root).as_posix()
                               for p in folder.rglob('*') if p.is_file() and p.suffix in SUFFIXES)
        seen = set()
        while pending:
            path = pending.pop()
            if path in seen:
                continue
            seen.add(path)
            risks, targets = self.node(path)
            if any(r.startswith('unresolved') for r in risks):
                self.uncertain_consumers.add(path)
            for target in targets:
                self.reverse.setdefault(target, set()).add(path)
                # Literal includes can be .inc or other nonstandard suffixes.
                pending.append(target)

    def reasons(self, path, text):
        # Standalone tool fixtures have no production include-context claim.
        if not in_scope(path):
            return direct_format_risks(text)
        self.node(path, text)
        queue, seen, reasons = [path], set(), []
        if pathlib.Path(path).suffix in {'.h', '.hpp'}:
            if self.reverse is None:
                self.build_reverse()
            pending, consumers = [path], set()
            while pending:
                for consumer in self.reverse.get(pending.pop(), ()):
                    if consumer not in consumers:
                        consumers.add(consumer)
                        pending.append(consumer)
            queue.extend(consumers | self.uncertain_consumers)
        while queue:
            current = queue.pop()
            if current in seen:
                continue
            seen.add(current)
            risks, includes = self.node(current)
            reasons.extend(current + ': ' + r for r in risks)
            queue.extend(includes)
        return sorted(set(reasons))


@dataclass(frozen=True)
class Finding:
    rule: str
    line: int
    match: str


def fingerprint(text):
    return re.sub(r"\s+", "", text)


def source_findings(text):
    # mwcc_side retains line numbers. Comments/literals are not source defects.
    text = msvc_arms.mwcc_side(text)
    mask = delaunder.code_mask(text)
    code = "".join(c if mask[i] or c == "\n" else " " for i, c in enumerate(text))
    found = []
    for rule, pattern in RULES.items():
        for m in pattern.finditer(code):
            found.append(Finding(rule, code.count("\n", 0, m.start()) + 1,
                                 fingerprint(m.group())))
    return sorted(found, key=lambda f: (f.line, f.rule))


def new_findings(before, after):
    bank = collections.Counter((f.rule, f.match) for f in source_findings(before))
    out = []
    for f in source_findings(after):
        key = f.rule, f.match
        if bank[key]:
            bank[key] -= 1
        else:
            out.append(f)
    return out


def added_ranges(before, after):
    old, new = before.splitlines(), after.splitlines()
    return [(j1 + 1, j2) for tag, _i1, _i2, j1, j2 in
            difflib.SequenceMatcher(None, old, new, autojunk=False).get_opcodes()
            if tag != "equal" and j2 > j1]


def git(root, *args, optional=False):
    p = subprocess.run(["git", "-C", str(root), *args], capture_output=True)
    if p.returncode and not optional:
        raise StyleError(p.stderr.decode("utf-8", errors="replace").strip())
    return p


def in_scope(path):
    p = pathlib.PurePosixPath(path)
    return (not any(c in path for c in "*?[]\\") and not p.is_absolute()
            and ".." not in p.parts and p.parts
            and p.parts[0] in {"src", "include"} and p.suffix in SUFFIXES)


def changed_paths(root, base):
    """Return (new path, original path) pairs, preserving rename identity."""
    raw = git(root, "diff", "--name-status", "-z", "--find-renames", base,
              "--", "src", "include").stdout.decode("utf-8").split("\0")
    pairs, i = {}, 0
    while i < len(raw) and raw[i]:
        status, path = raw[i:i + 2]
        i += 2
        old = path
        if status.startswith(("R", "C")):
            path = raw[i]
            i += 1
        if not status.startswith("D") and in_scope(path):
            pairs[path] = old
    extra = git(root, "ls-files", "--others", "--exclude-standard", "-z",
                "--", "src", "include").stdout.decode("utf-8").split("\0")
    for path in extra:
        if in_scope(path):
            pairs[path] = path
    return sorted(pairs.items())


def read_before(root, base, path):
    if base is None:
        return ""
    # A missing path is a new file; any other Git error is fatal.
    exists = git(root, "ls-tree", "-z", base, "--", path).stdout
    if not exists:
        return ""
    return git(root, "show", f"{base}:{path}").stdout.decode("utf-8").replace("\r\n", "\n")


def load_exceptions(path):
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
        if type(data.get("schema")) is not int or data["schema"] != 1 or not isinstance(data.get("exceptions"), list):
            raise ValueError("expected schema 1 and exceptions list")
        out = {}
        for row in data["exceptions"]:
            if set(row) != {"path", "rule", "match", "count", "reason", "evidence"}:
                raise ValueError("exception needs path, rule, match, count, reason, evidence")
            if not in_scope(row["path"]) or row["rule"] not in RULES:
                raise ValueError("unknown exception rule or source path")
            if type(row["count"]) is not int or row["count"] < 1:
                raise ValueError("exception count must be a positive integer")
            for key in ("match", "reason", "evidence"):
                if not isinstance(row[key], str) or not row[key].strip():
                    raise ValueError(f"exception {key} must be nonempty")
            key = row["path"], row["rule"], fingerprint(row["match"])
            if key in out:
                raise ValueError("duplicate exception")
            out[key] = row["count"]
        return out
    except (OSError, ValueError, TypeError, AttributeError) as exc:
        raise StyleError(f"invalid exception file {path}: {exc}") from exc


def apply_exceptions(path, findings, exceptions, before=""):
    # An exception caps the total population, not the increment in each PR.
    existing = collections.Counter((f.rule, f.match) for f in source_findings(before))
    budget = {key: max(0, count - existing[key[1:]])
              for key, count in exceptions.items() if key[0] == path}
    out = []
    for f in findings:
        key = path, f.rule, f.match
        if budget.get(key, 0):
            budget[key] -= 1
        else:
            out.append(f)
    return out


def formatter_version(executable):
    try:
        p = subprocess.run([executable, "--version"], capture_output=True, text=True)
    except OSError as exc:
        raise StyleError(f"cannot run clang-format: {exc}") from exc
    if p.returncode or not re.search(r"\bversion " + re.escape(FORMAT_VERSION) + r"(?![0-9.])", p.stdout):
        raise StyleError(f"need clang-format {FORMAT_VERSION}; got {p.stdout.strip()!r}")


def format_findings(executable, config, path, text, ranges):
    if not ranges:
        return []
    cmd = [executable, "--output-replacements-xml", f"--style=file:{config}",
           "--fallback-style=none", f"--assume-filename={path}"]
    cmd.extend(f"--lines={start}:{end}" for start, end in ranges)
    raw = text.encode("utf-8")
    p = subprocess.run(cmd, input=raw, capture_output=True)
    if p.returncode:
        raise StyleError(p.stderr.decode("utf-8", errors="replace").strip())
    try:
        tree = ET.fromstring(p.stdout)
        if tree.tag != "replacements" or tree.get("incomplete_format") == "true":
            raise ValueError("formatter did not fully parse the requested source")
        replacements = [(int(row.attrib["offset"]), int(row.attrib["length"]),
                         (row.text or "").encode("utf-8")) for row in tree.findall("replacement")]
        changed = raw
        previous = len(raw) + 1
        for offset, length, replacement in sorted(replacements, reverse=True):
            if offset < 0 or length < 0 or offset + length > len(raw) or offset + length > previous:
                raise ValueError("formatter replacement is out of range or overlapping")
            changed = changed[:offset] + replacement + changed[offset + length:]
            previous = offset
        if tokens(text) != tokens(changed.decode("utf-8")):
            raise UnsafeFormat("formatter would change preprocessing token spelling/order")
        out = []
        for row in tree.findall("replacement"):
            offset, length = int(row.attrib["offset"]), int(row.attrib["length"])
            first = raw.count(b"\n", 0, offset) + 1
            last = raw.count(b"\n", 0, offset + length) + 1
            # Whitespace ending at a newline also controls the following line's
            # indentation. Its exclusive endpoint must participate in coverage.
            affected = [max(first, start) for start, end in ranges
                        if first <= end and last >= start]
            if affected:
                out.append(Finding("format", min(affected), "run the pinned formatter on these lines"))
        return out
    except (ET.ParseError, ValueError, KeyError) as exc:
        raise StyleError(f"invalid formatter result for {path}: {exc}") from exc


def check(root, base, files, executable, patterns_only=False):
    exceptions = load_exceptions(root / "config/source-style-exceptions.json")
    if base is not None:
        base = git(root, "merge-base", base, "HEAD").stdout.decode().strip()
    if files is None:
        pairs = changed_paths(root, base)
    else:
        renames = dict(changed_paths(root, base)) if base is not None else {}
        selected = []
        for path in files:
            full = (root / path).resolve()
            if not full.is_relative_to(root.resolve()):
                raise StyleError(f"unsupported path: {path}")
            selected.append(full.relative_to(root.resolve()).as_posix())
        pairs = [(p, renames.get(p, p)) for p in selected]
    if not patterns_only:
        if not executable:
            raise StyleError("clang-format missing; install tools/requirements-style.txt")
        formatter_version(executable)
    results, deferred = [], []
    safety = FormatSafety(root)
    for path, old in pairs:
        full = (root / path).resolve()
        if not full.is_relative_to(root.resolve()) or full.suffix not in SUFFIXES:
            raise StyleError(f"unsupported path: {path}")
        after = full.read_text(encoding="utf-8")
        before = read_before(root, base, old)
        findings = apply_exceptions(path, new_findings(before, after), exceptions, before)
        ranges = added_ranges(before, after)
        if not patterns_only and ranges:
            risks = safety.reasons(path, after)
            if risks:
                deferred.append({"path": path, "reasons": risks})
            else:
                try:
                    findings += format_findings(executable, root / ".clang-format", path, after, ranges)
                except UnsafeFormat as exc:
                    deferred.append({"path": path, "reasons": [str(exc)]})
        results.extend({"path": path, "line": f.line, "rule": f.rule, "match": f.match}
                       for f in findings)
    return {"base": base, "files_checked": len(pairs), "patterns_only": patterns_only,
            "findings": results, "format_deferred": deferred}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--base", help="compare merge-base with HEAD to the working tree")
    parser.add_argument("--files", nargs="+", help="check explicit files in full, or against --base")
    parser.add_argument("--clang-format", default=os.environ.get("CLANG_FORMAT") or shutil.which("clang-format"))
    parser.add_argument("--patterns-only", action="store_true", help="explicitly omit formatting; not a full style pass")
    parser.add_argument("--json", type=pathlib.Path)
    args = parser.parse_args(argv)
    try:
        report = check(ROOT, args.base or (None if args.files else "origin/main"),
                       args.files, args.clang_format, args.patterns_only)
        if args.json:
            args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        for row in report["format_deferred"]:
            print(f"{row['path']}: FORMAT DEFERRED; retain original spelling and require compiler proof")
            for reason in row["reasons"]:
                print("  " + reason)
        for f in report["findings"]:
            print(f"{f['path']}:{f['line']}: {f['rule']}: {f['match']}")
        print(f"source-style: {report['files_checked']} file(s), {len(report['findings'])} finding(s), "
              f"{len(report['format_deferred'])} formatting deferral(s)"
              + ("; PATTERNS ONLY, formatting not checked" if args.patterns_only else ""))
        return int(bool(report["findings"]))
    except (StyleError, OSError, UnicodeError) as exc:
        print(f"source-style: ERROR: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
