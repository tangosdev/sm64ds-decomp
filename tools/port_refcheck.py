"""Reference-integrity gate for the MSVC port at port/.

WHY THIS EXISTS
---------------
port/ builds a standalone MSVC host executable that references decomp src/
by literal path and symbol name:
  - port/slice_gate*.txt manifests list src/ (and a couple of port/unmatched)
    files the host build compiles, one repo-root-relative path per line.
  - port/CMakeLists.txt hardcodes GATE*_SYMS lists that hostgen.py resolves
    through tools/srcpath.py, so enrolled members of a shared production TU
    resolve to that TU's physical source.
  - port/hal/*.cpp bridges MSVC linkage onto the decomp's func_XXXXXXXX /
    data_XXXXXXXX address-based naming through
    `#pragma comment(linker, "/alternatename:A=B")` and extern "C"
    declarations of those same names.

None of that is compiled or linked by the normal decomp toolchain (mwccarm,
the match/linkcheck pipeline), so a rename in src/ -- by hand, by
tools/cpp_rename.py, or a plain .c-to-.cpp / file-move -- can silently
strand a port/ reference. Nothing else catches it until someone attempts an
MSVC build.

This tool checks the port's references against the worktree's own files --
no compiler, no ROM -- so it runs in about a second and can gate every push.

CHECKS
------
1. manifests  - every non-comment, non-blank line in each of
                 port/slice_gate{1,2,3a,3b,4b,6,7,8,9}.txt names a path that
                 exists in the repo.
2. cmake syms - every symbol in port/CMakeLists.txt's hostgen `set(*_SYMS ...)`
                 lists resolves through tools/srcpath.py, the same enrollment-
                 aware lookup the CMake runs at configure time.
3. hal links  - every func_XXXXXXXX / data_XXXXXXXX name that appears as the
                 alias or target of a `#pragma comment(linker,
                 "/alternatename:...")`, or as an extern "C" declaration, in
                 port/hal/*.cpp still appears (as an identifier) somewhere
                 under src/ or include/, or has an unconditional, externally
                 linked C definition in port/hal/*.cpp. This is reference
                 integrity, not proof that every host target links.

Scope note on check 3: only bare func_/data_ address-style names are
checked (the ones a src/ rename can actually break). Itanium-mangled names
declared extern "C" (e.g. _ZN6Memory16rootHeapIteratorE) and MSVC C++-mangled
symbols the pragmas reference (e.g. ?Crash@@YAXXZ) are decomp *identifiers*
that a rename tool would not touch the address-form of, so they are out of
scope here -- see AGENTS.md/MERGE.md for the human-facing rule.

Usage:
  python tools/port_refcheck.py
  python tools/port_refcheck.py --json                 # report on stdout
  python tools/port_refcheck.py --json build/port.json # report to a file

The JSON report is the stable contract behind tools/validate_merge.py's
--port-refcheck-report: schemaVersion, ok, checked, failed, a flat
`failures` list of {check, file, line, message}, and the same failures
grouped under `checks` for a human reading the file directly.
"""
import argparse
from dataclasses import dataclass
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import srcpath as SP  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent
PORT = REPO / "port"

SLICE_GATE_SUFFIXES = ["1", "2", "3a", "3b", "4b", "6", "7", "8", "9"]
SRC_DIRS = [REPO / "src", REPO / "include"]
CODE_SUFFIXES = {".c", ".cpp", ".h", ".hpp"}

# func_XXXXXXXX / data_XXXXXXXX, with an optional ovNNN_ overlay qualifier
# (e.g. data_ov098_0213c380) -- the address-based naming a src/ rename can
# strand a port/ reference to.
IDENT_CORE = r"(?:func|data)_(?:ov\d+_)?[0-9a-fA-F]{8}"
IDENT_FULL_RE = re.compile(r"^" + IDENT_CORE + r"$")
IDENT_SCAN_RE = re.compile(r"\b" + IDENT_CORE + r"\b")

PRAGMA_RE = re.compile(r'^[ \t]*#pragma\s+comment\(\s*linker\s*,\s*"(/alternatename:[^"]*)"\s*\)', re.MULTILINE)
EXTERN_C_RE = re.compile(r'extern\s*"C"')


class Failure:
    def __init__(self, file, line, message):
        self.file = file
        self.line = line
        self.message = message

    def __str__(self):
        return f"{self.file}:{self.line}: {self.message}"

    def to_dict(self):
        return {"file": self.file, "line": self.line, "message": self.message}


def _line_at(text, pos):
    return text.count("\n", 0, pos) + 1


# C/C++ literals are single tokens: comment markers or braces inside them must
# not change linkage scope. Raw strings occur in host code too.
CPP_TOKEN_RE = re.compile(
    r'/\*.*?\*/|//(?:\\\r?\n|[^\n])*|(?:u8|[uUL])?R"([^ ()\\\t\r\n]*)\(.*?\)\1"'
    r'|(?:u8|[uUL])?"(?:\\.|[^"\\])*"'
    r"|(?:[uUL])?'(?:\\.|[^'\\])*'"
    r'|[A-Za-z_]\w*|[^\s]', re.DOTALL)


@dataclass(frozen=True)
class Token:
    value: str
    pos: int
    punctuation: bool = False


def _blank(text):
    return "".join("\n" if c == "\n" else " " for c in text)


def _mask_comments(text):
    return CPP_TOKEN_RE.sub(
        lambda m: _blank(m.group()) if m.group().startswith(("//", "/*"))
        else m.group(), text)


def _mask_literals(text, keep_linkage=False):
    def replace(match):
        value = match.group()
        if keep_linkage and value == '"C"':
            return value
        return _blank(value) if '"' in value or "'" in value else value
    return CPP_TOKEN_RE.sub(replace, text)


def _cmake_tokens(text):
    """Lex CMake arguments without interpreting variables or executing CMake.

    In particular, # comments (including bracket comments), quoted/bracket
    arguments and escaped delimiters cannot terminate a set() command.
    Positions refer to the original text for actionable diagnostics.
    """
    i = 0
    while i < len(text):
        if text[i].isspace():
            i += 1
            continue
        start = i
        comment = text[i] == "#"
        bracket = re.match(r"\[(=*)\[", text[i + int(comment):])
        if bracket:
            content = i + int(comment) + len(bracket.group())
            close = "]" + bracket.group(1) + "]"
            end = text.find(close, content)
            if end < 0:
                raise ValueError(f"line {_line_at(text, start)}: unterminated bracket argument")
            if not comment:
                value = text[content:end]
                if value.startswith("\n"):
                    value = value[1:]  # CMake ignores the first bracket newline
                yield Token(value, start)
            i = end + len(close)
        elif comment:
            end = text.find("\n", i)
            i = len(text) if end < 0 else end
        elif text[i] in "()":
            yield Token(text[i], i, punctuation=True)
            i += 1
        else:
            quoted = text[i] == '"'
            if quoted:
                i += 1
            chars = []
            while i < len(text):
                ch = text[i]
                if quoted and ch == '"':
                    i += 1
                    break
                if not quoted and (ch.isspace() or ch in "()#"):
                    break
                if ch == "\\" and i + 1 < len(text):
                    i += 1
                    ch = text[i]
                    if ch == "\n":
                        i += 1
                        continue
                    if ch == ";" and quoted:
                        chars.append("\\")  # preserved until foreach list expansion
                    ch = {"n": "\n", "r": "\r", "t": "\t"}.get(ch, ch)
                chars.append(ch)
                i += 1
            else:
                if quoted:
                    raise ValueError(f"line {_line_at(text, start)}: unterminated quoted argument")
            # Empty arguments initialize an empty symbol list, not a source name.
            yield Token("".join(chars), start)


def _cmake_symbol_lists(text):
    tokens = list(_cmake_tokens(text))
    i = 0
    while i + 1 < len(tokens):
        if not tokens[i + 1].punctuation or tokens[i + 1].value != "(":
            i += 1
            continue
        command = tokens[i].value.lower()
        depth, j = 1, i + 2
        while j < len(tokens) and depth:
            if tokens[j].punctuation:
                depth += (tokens[j].value == "(") - (tokens[j].value == ")")
            j += 1
        if depth:
            raise ValueError(f"line {_line_at(text, tokens[i].pos)}: unterminated command")
        args = tokens[i + 2:j - 1]
        if command == "set" and args and re.fullmatch(r"\w+_SYMS", args[0].value):
            for arg in args[1:]:
                # A symbol-list value is subsequently expanded by foreach(IN
                # LISTS ...), so quoted semicolon lists contain multiple names.
                for name in re.split(r"(?<!\\);", arg.value):
                    name = name.replace(r"\;", ";")
                    if name:
                        yield args[0].value, Token(name, arg.pos)
        i = j


# ---- check 1: slice manifests -----------------------------------------------

def check_manifests():
    """Every path listed in a slice_gate*.txt manifest exists in the repo."""
    failures = []
    checked = 0
    for suffix in SLICE_GATE_SUFFIXES:
        manifest = PORT / f"slice_gate{suffix}.txt"
        rel = manifest.relative_to(REPO).as_posix()
        if not manifest.exists():
            failures.append(Failure(rel, 0, "manifest file not found"))
            continue
        text = manifest.read_text(encoding="utf-8", errors="ignore")
        for lineno, raw in enumerate(text.splitlines(), start=1):
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            checked += 1
            if not (REPO / line).exists():
                failures.append(Failure(rel, lineno, f"{line} not found (renamed?)"))
    return checked, failures


# ---- check 2: CMake hostgen symbol lists ------------------------------------

def check_cmake_symbols():
    """Every symbol in a `set(GATE*_SYMS ...)` list resolves through srcpath.

    Filename probing is not authoritative once one production translation unit owns
    several symbols.  ``srcpath.path_for`` asks the live delinks enrollment first,
    which is also what the CMake hostgen resolver now does.
    """
    cmake = PORT / "CMakeLists.txt"
    rel = cmake.relative_to(REPO).as_posix()
    if not cmake.exists():
        return 0, [Failure(rel, 0, "CMakeLists.txt not found")]

    text = cmake.read_text(encoding="utf-8", errors="ignore")
    failures = []
    checked = 0
    try:
        symbols = list(_cmake_symbol_lists(text))
    except ValueError as exc:
        return 0, [Failure(rel, 0, f"cannot parse CMake symbol lists: {exc}")]
    for list_name, tok in symbols:
        sym = tok.value
        checked += 1
        lineno = _line_at(text, tok.pos)
        resolved = SP.path_for(sym)
        if resolved is None or not resolved.is_file():
            failures.append(Failure(
                rel, lineno,
                f"symbol '{sym}' ({list_name}) has no enrolled or "
                "convention-named src/ owner (renamed?)"))
    return checked, failures


# ---- check 3: port/hal linkage bridges --------------------------------------

def _strip_decoration(spelling):
    """Undo one layer of MSVC decoration on an /alternatename spelling and
    return the bare core name -- or None if it is not a bare identifier
    (still mangled, still suffixed, etc). Only a name that fully reduces to
    plain text is a candidate for the func_/data_ pattern check.

    ?data_0209b44c@@3CA          -> data_0209b44c   (C++-mangled variable)
    _data_020a0e68               -> data_020a0e68   (cdecl decoration)
    __ZN6Memory...E              -> ZN6Memory...E   (not func_/data_, skipped)
    _data_0209b44c_c             -> data_0209b44c_c (suffixed local storage,
                                                       correctly NOT matching
                                                       the bare pattern)
    """
    spelling = spelling.strip()
    if spelling.startswith("__imp_"):
        spelling = spelling[len("__imp_"):]
    if spelling.startswith("?"):
        at = spelling.find("@")
        return spelling[1:at] if at != -1 else spelling[1:]
    return spelling.lstrip("_")


def _extern_c_spans(masked_text):
    """(start, end) character spans covering each `extern "C"` occurrence in
    `masked_text` through the end of its scope: the whole `{ ... }` block for
    the block form, or the single declaration (plus its function body, if
    any) for `extern "C" <decl>;` / `extern "C" <decl> { ... }`."""
    spans = []
    n = len(masked_text)
    for m in EXTERN_C_RE.finditer(masked_text):
        j = m.end()
        while j < n and masked_text[j] in " \t\r\n":
            j += 1
        if j < n and masked_text[j] == "{":
            depth, k = 0, j
            while k < n:
                if masked_text[k] == "{":
                    depth += 1
                elif masked_text[k] == "}":
                    depth -= 1
                    if depth == 0:
                        k += 1
                        break
                k += 1
            spans.append((m.start(), k))
        else:
            k = j
            while k < n and masked_text[k] not in ";{":
                k += 1
            if k < n and masked_text[k] == "{":
                depth = 0
                while k < n:
                    if masked_text[k] == "{":
                        depth += 1
                    elif masked_text[k] == "}":
                        depth -= 1
                        if depth == 0:
                            k += 1
                            break
                    k += 1
            elif k < n and masked_text[k] == ";":
                k += 1
            spans.append((m.start(), k))
    return spans


def _defined_macro_names(texts):
    """Known remappings cannot prove ownership of the unexpanded spelling."""
    return {m.group(1) for text in texts for m in re.finditer(
        r"^\s*#\s*define\s+(\w+)", _mask_comments(text), re.MULTILINE)}


def _pragma_only_macros(texts):
    """Recognize section-marker macros by their expansion, never by name.

    HAL uses object-like macros containing only __pragma(...) statements.
    These cannot introduce storage/linkage or a C++ scope. Any conflicting
    definition or #undef disqualifies a name; no general macro expansion is
    attempted here.
    """
    candidates, rejected = set(), set()
    for text in texts:
        joined = re.sub(r"\\\r?\n", "", _mask_comments(text))
        for line in joined.splitlines():
            match = re.match(r"\s*#\s*(define|undef)\s+(\w+)(.*)", line)
            if not match:
                continue
            directive, name, tail = match.groups()
            if directive == "undef" or tail.startswith("("):
                rejected.add(name)
                continue
            # Literals can contain parentheses; match the token structure.
            values = [m.group() for m in CPP_TOKEN_RE.finditer(tail)]
            i, valid = 0, bool(values)
            while valid and i < len(values):
                if values[i:i + 2] != ["__pragma", "("]:
                    valid = False
                    break
                i, depth = i + 2, 1
                while i < len(values) and depth:
                    depth += (values[i] == "(") - (values[i] == ")")
                    i += 1
                valid = depth == 0
            (candidates if valid else rejected).add(name)
    return candidates - rejected


def _host_c_definitions(text, pragma_macros=(), defined_macros=()):
    """Find simple, unconditional, externally linked HAL definitions.

    This is a reference gate, not a C++ compiler or a host link proof. Be
    conservative: declarations alone, function-local storage, class members,
    anonymous namespaces, static/const or unknown-type storage and conditional definitions do
    not supply a missing src/ name. C++-decorated aliases are not satisfied by
    a C definition. Unknown declaration forms remain unresolved.
    """
    # Do not promote a definition from an unevaluated preprocessor branch to
    # an unconditional owner. Still scan such code for references elsewhere.
    lines = []
    depth = 0
    continuation = False
    for line in _mask_comments(text).splitlines(keepends=True):
        directive = re.match(r"\s*#\s*(\w+)", line)
        if directive and directive.group(1) in ("if", "ifdef", "ifndef"):
            depth += 1
        masked = depth > 0 or directive or continuation
        if directive and directive.group(1) == "endif":
            depth = max(0, depth - 1)
        continuation = bool((directive or continuation) and line.rstrip().endswith("\\"))
        lines.append(_blank(line) if masked else line)
    tokens = [Token(m.group(), m.start())
              for m in CPP_TOKEN_RE.finditer("".join(lines))
              if m.group() not in pragma_macros]
    owners = {}
    remapped = set(defined_macros) | _defined_macro_names([text])

    def close_group(i, end):
        closing = {"(": ")", "[": "]", "{": "}"}[tokens[i].value]
        i += 1
        while i < end:
            if tokens[i].value == closing:
                return i + 1
            if tokens[i].value in ("(", "[", "{"):
                i = close_group(i, end)
            else:
                i += 1
        return end

    def declaration(decl, c_linkage, body=False):
        values = [t.value for t in decl]
        if values[:2] == ["extern", '"C++"']:
            return
        single_linkage = values[:2] == ["extern", '"C"']
        if single_linkage:
            decl, values = decl[2:], values[2:]
        if not (c_linkage or single_linkage) or not values:
            return
        if any(v in values for v in ("static", "typedef", "using", "namespace", "class", "struct", "union", ":")):
            return
        # File-scope const can have internal linkage; without a compiler do
        # not infer exceptions or qualifiers hidden by macros/typedefs.
        if "const" in values and "extern" not in values:
            return
        external_decl = single_linkage or "extern" in values
        # Identify declarators, never identifiers in parameter lists, array
        # bounds or initializer expressions. Common plain scalar/array and
        # pointer declarations cover the HAL's hosted data tables.
        nesting = 0
        initializer = False
        for i, tok in enumerate(decl):
            value = tok.value
            if value in ("(", "[", "{"):
                nesting += 1
            elif value in (")", "]", "}"):
                nesting -= 1
            elif value == "," and nesting == 0:
                initializer = False
            elif value == "=" and nesting == 0:
                initializer = True
            elif (nesting == 0 and not initializer
                  and IDENT_FULL_RE.fullmatch(value) and value not in remapped):
                following = values[i + 1] if i + 1 < len(values) else ";"
                if i == 0 or values[i - 1] in ("::", ".", "=", "return"):
                    continue
                if following == "(":
                    # A C language-linkage block does not override static
                    # storage or x86 calling-convention decoration hidden by
                    # a prefix macro. Only explicit cdecl-compatible prefixes
                    # can supply the cdecl spelling checked by this gate.
                    prefix = set(values[:i])
                    if not prefix <= {"extern", "signed", "unsigned", "char",
                                      "short", "int", "long", "float", "double",
                                      "bool", "void", "volatile", "const", "*",
                                      "__cdecl"} or prefix & remapped:
                        continue
                    if body:
                        owners[value] = tok.pos
                elif not body and following in ("[", "=", ",", ";"):
                    # An unknown typedef or macro might hide const/internal
                    # storage. Accept fundamental types/pointers/arrays only;
                    # a compiler-backed owner index can extend this later.
                    type_end = next((k for k, v in enumerate(values)
                                     if IDENT_FULL_RE.fullmatch(v)), len(values))
                    if not set(values[:type_end]) <= {
                            "extern", "signed", "unsigned", "char", "short",
                            "int", "long", "float", "double", "bool", "void",
                            "volatile", "const", "*"} or set(values[:type_end]) & remapped:
                        continue
                    # An extern with initializer is a definition; an extern
                    # without initializer (including extern "C" T x;) is not.
                    j = i + 1
                    balance = 0
                    initialized = False
                    while j < len(values):
                        v = values[j]
                        if v in ("[", "(", "{"):
                            balance += 1
                        elif v in ("]", ")", "}"):
                            balance -= 1
                        if balance == 0 and v == ",":
                            break
                        if balance == 0 and v == "=":
                            initialized = True
                        j += 1
                    if not external_decl or initialized:
                        owners[value] = tok.pos

    def scope(start, end, c_linkage=False):
        i = start
        while i < end:
            if (i + 2 < end and tokens[i].value == "extern"
                    and tokens[i + 1].value in ('"C"', '"C++"')
                    and tokens[i + 2].value == "{"):
                after = close_group(i + 2, end)
                scope(i + 3, after - 1, tokens[i + 1].value == '"C"')
                i = after
                continue
            j = i
            while j < end and tokens[j].value != ";":
                if tokens[j].value in ("(", "["):
                    j = close_group(j, end)
                elif tokens[j].value == "{":
                    after = close_group(j, end)
                    prefix = [t.value for t in tokens[i:j]]
                    if "=" not in prefix:
                        # A function definition ends with its body. Other
                        # scopes (class/namespace) cannot provide C owners.
                        if prefix and prefix[-1] == ")":
                            declaration(tokens[i:j], c_linkage, body=True)
                        j = after
                        break
                    j = after  # brace initializer; keep the declarator
                else:
                    j += 1
            if j < end and tokens[j].value == ";":
                declaration(tokens[i:j], c_linkage)
                j += 1
            i = max(i + 1, j)

    scope(0, len(tokens))
    return owners


def _scan_symbol_index():
    """Pure-Python fallback: every func_/data_ token under src/ or include/,
    built by reading every file. Correct but slow (~1s+ on this corpus) --
    only used if git is unavailable."""
    index = set()
    for root in SRC_DIRS:
        if not root.is_dir():
            continue
        for p in root.rglob("*"):
            if p.is_file() and p.suffix.lower() in CODE_SUFFIXES:
                try:
                    text = p.read_text(encoding="utf-8", errors="ignore")
                except OSError:
                    continue
                index.update(IDENT_SCAN_RE.findall(text))
    return index


def _present(names):
    """Which of `names` (func_/data_ identifiers) appear as whole-word text
    somewhere under src/ or include/. A single `git grep -F -w` pass over the
    whole tree is ~10x faster than reading and regex-scanning ~11k files in
    Python (git grep: ~0.15s here vs ~1.2s), so it is tried first; a plain
    scan is the fallback for a non-git checkout or if git is missing."""
    names = sorted(set(names))
    if not names:
        return set()
    import subprocess
    args = ["git", "grep", "--no-color", "-F", "-w", "-h", "-o"]
    for n in names:
        args += ["-e", n]
    args += ["--"] + [d.name for d in SRC_DIRS if d.is_dir()]
    try:
        proc = subprocess.run(args, cwd=REPO, capture_output=True, text=True)
    except (OSError, subprocess.SubprocessError):
        proc = None
    if proc is not None and proc.returncode in (0, 1):
        # 0 = matches found, 1 = git grep ran fine but found nothing
        found = {line.strip() for line in proc.stdout.splitlines() if line.strip()}
        return found & set(names)
    return _scan_symbol_index() & set(names)


def check_hal_links():
    """#pragma alternatename aliases/targets and extern "C" declarations of
    func_/data_ names in port/hal/*.cpp must still name something that
    exists in src/ or include/, or has a proven unconditional C-linkage
    host definition. Extern-only declarations cannot supply an owner."""
    hal_dir = PORT / "hal"
    if not hal_dir.is_dir():
        return 0, [Failure("port/hal", 0, "hal directory not found")]

    # Pass 1: collect every candidate reference without checking existence
    # yet, so all the (few hundred) names can be looked up in one batched
    # `git grep` pass instead of one lookup each.
    refs = []  # (rel, lineno, name, message, C linkage)
    host_owners = set()
    texts = {p: p.read_text(encoding="utf-8", errors="ignore")
             for p in sorted(hal_dir.iterdir()) if p.suffix in CODE_SUFFIXES}
    pragma_macros = _pragma_only_macros(texts.values())
    defined_macros = _defined_macro_names(texts.values())
    for path in sorted(hal_dir.glob("*.cpp")):
        rel = path.relative_to(REPO).as_posix()
        text = texts[path]
        masked = _mask_comments(text)
        host_owners.update(_host_c_definitions(text, pragma_macros, defined_macros))

        pragma_text = CPP_TOKEN_RE.sub(
            lambda m: _blank(m.group()) if re.match(r'(?:u8|[uUL])?R"', m.group())
            else m.group(), masked)
        for m in PRAGMA_RE.finditer(pragma_text):
            lineno = _line_at(text, m.start())
            body = m.group(1)[len("/alternatename:"):]
            alias, _, target = body.partition("=")
            for spelling in (alias, target):
                core = _strip_decoration(spelling)
                if not core or not IDENT_FULL_RE.match(core):
                    continue
                refs.append((rel, lineno, core,
                              f"alternatename references '{core}' (from "
                              f"'{spelling.strip()}'), not found in src/ or "
                              f"include/ or an unconditional C-linkage HAL definition "
                              f"(renamed?)", not spelling.strip().removeprefix("__imp_").startswith("?")))

        code = _mask_literals(masked)
        for start, end in _extern_c_spans(_mask_literals(masked, keep_linkage=True)):
            for m in IDENT_SCAN_RE.finditer(code, start, end):
                name = m.group(0)
                lineno = _line_at(text, m.start())
                refs.append((rel, lineno, name,
                             f'extern "C" references \'{name}\', not found in '
                             f"src/ or include/ or an unconditional C-linkage HAL "
                             f"definition (renamed?)", True))

    # Pass 2: one batched presence check, then report.
    present = _present(name for _, _, name, _, _ in refs)
    failures = []
    seen = set()  # (file, name) already reported, so a name isn't repeated
    for rel, lineno, name, message, c_linkage in refs:
        if name in present or (c_linkage and name in host_owners):
            continue
        key = (rel, name)
        if key in seen:
            continue
        seen.add(key)
        failures.append(Failure(rel, lineno, message))

    return len(refs), failures


CHECKS = [
    ("manifests", check_manifests),
    ("cmake-symbols", check_cmake_symbols),
    ("hal-links", check_hal_links),
]


def build_report():
    """Run every check and return the report JSON consumers see.

    `failures` is a flat, check-tagged copy of everything under `checks`, so a
    consumer that only wants "did it pass, and what broke" -- tools/validate_merge.py
    -- never has to walk the per-check grouping.
    """
    results = {}
    tagged = []
    total_checked = 0
    for name, fn in CHECKS:
        checked, failures = fn()
        results[name] = {"checked": checked, "failures": [f.to_dict() for f in failures]}
        total_checked += checked
        tagged.extend((name, f) for f in failures)
    return {
        "schemaVersion": 1,
        "ok": not tagged,
        "checked": total_checked,
        "failed": len(tagged),
        "failures": [dict(f.to_dict(), check=name) for name, f in tagged],
        "checks": results,
    }


def main():
    ap = argparse.ArgumentParser(
        description="Check port/'s literal src/ path and symbol references "
                     "for staleness left behind by renames.",
        formatter_class=argparse.RawDescriptionHelpFormatter, epilog=__doc__)
    ap.add_argument("--json", nargs="?", const="-", metavar="PATH",
                    help="write the JSON report to PATH (stdout if PATH is "
                         "omitted or '-')")
    args = ap.parse_args()

    report = build_report()
    all_failures = report["failures"]
    total_checked = report["checked"]

    if args.json == "-":
        print(json.dumps(report, indent=2))
        return 1 if all_failures else 0
    if args.json:
        # A report file leaves stdout free for the human summary below, which is
        # all a CI log of this phase would otherwise show.
        pathlib.Path(args.json).write_text(json.dumps(report, indent=2) + "\n",
                                           encoding="utf-8", newline="\n")

    for name, fn in CHECKS:
        r = report["checks"][name]
        icon = "OK  " if not r["failures"] else "FAIL"
        print(f"  [{icon}] {name:<14} {r['checked']} reference(s) checked, "
              f"{len(r['failures'])} stale")

    if all_failures:
        print("\nport-refcheck: STALE port/ references found:", file=sys.stderr)
        for f in all_failures:
            print(f"  {f['file']}:{f['line']}: {f['message']}", file=sys.stderr)
        print(f"\nport-refcheck: {total_checked} checked - {len(all_failures)} stale",
              file=sys.stderr)
        print("A src/ rename, .c-to-.cpp migration, or file move left port/ "
              "pointing at a name that no longer exists. Update the "
              "manifest / CMakeLists.txt / port/hal/*.cpp reference (see "
              "AGENTS.md), then re-run.", file=sys.stderr)
        return 1

    print(f"port-refcheck: {total_checked} checked - all references resolve")
    return 0


if __name__ == "__main__":
    sys.exit(main())
