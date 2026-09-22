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
                 linked C definition in port/hal/*.cpp. Selected unmatched
                 sources, named-data generator inputs, and independently rooted
                 linker aliases can also supply ownership. Unused generation
                 rules, unselected lists, and config rows alone cannot.
                 This is reference integrity, not proof that every host target links.

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
import ast
import hashlib
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
    literal: bool = False


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
        if text[i].isspace() or (i == 0 and text[i] == '\ufeff'):
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
                yield Token(value, start, literal=True)
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
                    if (ch == ";" and quoted) or ch == "$":
                        chars.append("\\")  # preserved until foreach list expansion
                    ch = {"n": "\n", "r": "\r", "t": "\t"}.get(ch, ch)
                chars.append(ch)
                i += 1
            else:
                if quoted:
                    raise ValueError(f"line {_line_at(text, start)}: unterminated quoted argument")
            # Empty arguments initialize an empty symbol list, not a source name.
            yield Token("".join(chars), start)


def _cmake_commands(text):
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
        yield command, tokens[i + 2:j - 1]
        i = j


def _cmake_values(args, variables):
    """Expand previously known literal lists; retain every unknown expression."""
    result = []
    for arg in args:
        value = arg.value
        def replace(match):
            known = variables.get(match.group(1))
            return ";".join(t.value for t in known) if known is not None else match.group()
        if not arg.literal:
            value = re.sub(r"(?<!\\)\$\{(\w+)\}", replace, value)
        for name in re.split(r"(?<!\\);", value):
            name = name.replace(r"\;", ";").replace(r"\$", "$")
            if name:
                result.append(Token(name, arg.pos))
    return result


def _cmake_helper_writes(commands):
    helpers = {}
    for i, (command, args) in enumerate(commands):
        if command not in ("function", "macro") or not args:
            continue
        end = "end" + command
        body = []
        for name, inner in commands[i + 1:]:
            if name == end:
                break
            body.append((name, inner))
        helpers[args[0].value.lower()] = ([a.value for a in args[1:]], body, command)
    return helpers


def _cmake_write_targets(name, args, variables, helpers, active=()):
    """Return possible variable writes, or None when their identity is unknown.

    This is an invalidation model, not a CMake evaluator. File contents, process
    output and helper return values never preserve a preceding literal value.
    """
    if name in helpers:
        if name in active:
            return None
        formal, body, kind = helpers[name]
        local = dict(variables)
        for index, key in enumerate(formal):
            local[key] = _cmake_values(args[index:index + 1], variables)
        local['ARGN'] = _cmake_values(args[len(formal):], variables)
        outputs = set()
        for command, inner in body:
            written = _cmake_write_targets(command, inner, local, helpers, active + (name,))
            if written is None:
                return None
            outputs.update(written)
            for key in written:
                local.pop(key, None)
        return outputs
    # Outputs occupy argument positions: an empty replacement or input must
    # not shift the following output name into another slot.
    words = [';'.join(v.value for v in _cmake_values([arg], variables)) for arg in args]
    op = words[0] if words else ''
    outputs = []
    if name in ('set', 'unset', 'option', 'find_program', 'find_file',
                'find_path', 'find_library', 'get_filename_component',
                'get_target_property', 'get_property', 'get_directory_property',
                'get_source_file_property', 'get_test_property'):
        outputs = words[:1]
    elif name == 'list':
        if op in ('LENGTH', 'GET', 'JOIN', 'FIND', 'SUBLIST'):
            outputs = words[-1:]
        elif op in ('POP_BACK', 'POP_FRONT'):
            outputs = words[1:]
        elif op == 'TRANSFORM' and 'OUTPUT_VARIABLE' in words:
            outputs = words[words.index('OUTPUT_VARIABLE') + 1:]
        else:
            outputs = words[1:2]
    elif name == 'file':
        if op in ('READ', 'STRINGS'):
            outputs = words[2:3]
        elif op in ('GLOB', 'GLOB_RECURSE'):
            outputs = words[1:2]
        elif op in ('SIZE', 'TIMESTAMP', 'REAL_PATH', 'READ_SYMLINK'):
            outputs = words[2:3]
        elif op in ('RELATIVE_PATH', 'TO_CMAKE_PATH', 'TO_NATIVE_PATH'):
            outputs = words[1:2] if op == 'RELATIVE_PATH' else words[2:3]
        elif op in ('MD5', 'SHA1', 'SHA224', 'SHA256', 'SHA384', 'SHA512',
                    'SHA3_224', 'SHA3_256', 'SHA3_384', 'SHA3_512'):
            outputs = words[2:3]
        elif op in ('DOWNLOAD', 'UPLOAD'):
            outputs = [words[i + 1] for i, word in enumerate(words[:-1])
                       if word in ('STATUS', 'LOG')]
        elif op not in ('WRITE', 'APPEND', 'TOUCH', 'TOUCH_NOCREATE', 'GENERATE',
                        'COPY', 'COPY_FILE', 'INSTALL', 'RENAME', 'REMOVE',
                        'REMOVE_RECURSE', 'MAKE_DIRECTORY', 'CHMOD', 'CHMOD_RECURSE'):
            return None
    elif name == 'string':
        if op == 'REGEX':
            outputs = words[4:5] if words[1:2] == ['REPLACE'] else words[3:4]
        elif op == 'REPLACE':
            outputs = words[3:4]
        elif op in ('APPEND', 'PREPEND', 'CONCAT', 'TIMESTAMP', 'RANDOM', 'UUID'):
            outputs = words[1:2] if op != 'RANDOM' else words[-1:]
        elif op in ('FIND', 'SUBSTRING'):
            outputs = words[3:4] if op == 'FIND' else words[4:5]
        elif op == 'COMPARE':
            outputs = words[4:5]
        elif op in ('JOIN',):
            outputs = words[2:3]
        elif op in ('STRIP', 'TOLOWER', 'TOUPPER', 'LENGTH', 'CONFIGURE',
                    'MAKE_C_IDENTIFIER', 'GENEX_STRIP', 'REPEAT', 'HEX'):
            outputs = words[3:4] if op == 'REPEAT' else words[2:3]
        elif op in ('MD5', 'SHA1', 'SHA224', 'SHA256', 'SHA384', 'SHA512'):
            outputs = words[1:2]
        else:
            return None
    elif name == 'math':
        outputs = words[1:2]
    elif name == 'execute_process':
        outputs = [words[i + 1] for i, word in enumerate(words[:-1])
                   if word in ('RESULT_VARIABLE', 'RESULTS_VARIABLE',
                               'OUTPUT_VARIABLE', 'ERROR_VARIABLE')]
    elif name in ('include', 'find_package', 'cmake_language'):
        return None  # arbitrary included/evaluated code can write any variable
    elif name not in {
            'cmake_minimum_required', '\ufeffcmake_minimum_required', 'project',
            'if', 'elseif', 'else', 'endif', 'foreach', 'endforeach', 'while',
            'endwhile', 'function', 'endfunction', 'macro', 'endmacro', 'block',
            'endblock', 'return', 'continue', 'break', 'message', 'enable_language',
            'add_executable', 'add_library', 'add_custom_command', 'add_custom_target',
            'add_dependencies', 'add_test', 'enable_testing', 'set_tests_properties',
            'target_sources', 'target_compile_definitions', 'target_compile_options',
            'target_include_directories', 'target_link_libraries', 'target_link_options',
            'target_link_directories', 'target_compile_features', 'add_compile_options',
            'add_compile_definitions', 'add_definitions', 'include_directories',
            'link_directories', 'link_libraries', 'set_property',
            'set_target_properties', 'set_source_files_properties'}:
        raise ValueError('unsupported CMake command cannot prove source selection: ' + name)
    if any(not re.fullmatch(r'\w+', key) for key in outputs):
        return None
    return set(outputs)


def _invalidate_cmake_writes(name, args, variables, helpers):
    outputs = _cmake_write_targets(name, args, variables, helpers)
    if outputs is None:
        variables.clear()
    else:
        for key in outputs:
            variables.pop(key, None)
    return outputs


def _cmake_symbol_lists(text):
    variables = {}
    depth = 0
    commands = list(_cmake_commands(text))
    helpers = _cmake_helper_writes(commands)
    skip_until = 0
    for index, (command, args) in enumerate(commands):
        if index < skip_until:
            continue
        if command == 'set' and args and args[0].value == 'PORT_SHARE_TARGETS':
            skip_until = _verified_tier_sharing(commands, index, len(commands))
            for key in list(variables):
                if key.startswith('_') or key in ('PORT_SHARE_TARGETS', 'PORT_TIER_SHARED',
                                                  'PORT_TIER_PERTGT', 'PORT_DIVERGENT_DEFINES'):
                    variables.pop(key, None)
            continue
        if command not in ('set', 'unset', 'list'):
            written = _invalidate_cmake_writes(command, args, variables, helpers)
            for key in written or ():
                if re.fullmatch(r'\w+_SYMS', key):
                    yield key, Token('${' + key + '}', args[0].pos if args else 0)
        if command in ("if", "foreach", "while", "function", "macro", "block"):
            depth += 1
        elif command in ("endif", "endforeach", "endwhile", "endfunction", "endmacro", "endblock"):
            depth = max(0, depth - 1)
        if command not in ("set", "unset", "list") or not args:
            continue
        keys = _cmake_values(args[:1], variables)
        name = keys[0].value if len(keys) == 1 else ''
        values = _cmake_values(args[1:], variables)
        if command == "set":
            if not re.fullmatch(r'\w+', name):
                variables.clear()
            elif depth or any(t.value in ("CACHE", "PARENT_SCOPE") for t in args[1:]):
                variables.pop(name, None)
            else:
                variables[name] = values
            if re.fullmatch(r"\w+_SYMS", name):
                for value in values:
                    yield name, value
        elif command == "unset":
            variables.pop(name, None)
        elif len(args) > 1:
            keys = _cmake_values(args[1:2], variables)
            name = keys[0].value if len(keys) == 1 else ''
            if not re.fullmatch(r'\w+', name):
                raise ValueError('unresolved CMake list destination cannot prove symbol references')
            if args[0].value in ("LENGTH", "GET", "JOIN", "FIND", "SUBLIST"):
                outputs = _cmake_write_targets(command, args, variables, helpers)
                _invalidate_cmake_writes(command, args, variables, helpers)
                for key in outputs or ():
                    if re.fullmatch(r'\w+_SYMS', key):
                        yield key, Token('${' + key + '}', args[-1].pos)
            elif args[0].value in ('APPEND', 'PREPEND', 'INSERT'):
                added = _cmake_values(args[3:] if args[0].value == 'INSERT' else args[2:], variables)
                if re.fullmatch(r'\w+_SYMS', name):
                    for value in added:
                        yield name, value
                if depth or name not in variables:
                    variables.pop(name, None)
                elif args[0].value == 'APPEND':
                    variables[name].extend(added)
                elif args[0].value == 'PREPEND':
                    variables[name] = added + variables[name]
                else:
                    variables.pop(name, None)
            else:
                written = _invalidate_cmake_writes(command, args, variables, helpers)
                for key in written or ():
                    if re.fullmatch(r'\w+_SYMS', key) and args[0].value not in (
                            'REMOVE_ITEM', 'REMOVE_AT', 'REMOVE_DUPLICATES', 'FILTER', 'SORT', 'REVERSE'):
                        yield key, Token('${' + key + '}', args[0].pos)


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
        expanded = {}
        selected, requests = _cmake_build_inputs(text, expanded)
        generated = set()
        for request in requests:
            if len(request) > 1 and pathlib.Path(request[1]).name in ('romdata.py', 'ovdata.py'):
                index = 2 if pathlib.Path(request[1]).name == 'romdata.py' else 3
                if len(request) > index and _generated_data_owners([request]):
                    path = pathlib.Path(request[index])
                    generated.add((path if path.is_absolute() else PORT / path).resolve())
    except ValueError as exc:
        return 0, [Failure(rel, 0, f"cannot parse CMake symbol lists: {exc}")]
    for list_name, tok in symbols:
        # Some *_SYMS variables actually hold generated C source paths. Admit
        # only expansions witnessed in the selected graph and named-data input
        # proof; an unresolved path-looking expression is still a failure.
        values = expanded.get((list_name, tok.pos), [])
        paths = {(pathlib.Path(v.value) if pathlib.Path(v.value).is_absolute()
                  else PORT / v.value).resolve() for v in values}
        if paths and paths <= selected & generated:
            checked += len(paths)
            continue
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


# This existing tier-sharing pass moves common sources into an OBJECT library
# and reinserts that library into every same target. It preserves source
# selection. Pin the reviewed command stream and its list-emitting helper;
# arbitrary target SOURCES mutations are unsupported and block ownership.
TIER_SHARING_COMMANDS = '51c6017c7d71bd5c35bbc5f8eade86dfa3b2eaa11cfd33dd0bcbef688bb59200'
TIER_SHARING_TOOL = '0ce272b764373cd019410e59f2a88c1fdea4c75a483c80ebe3d5314a87fa95d9'


def _verified_tier_sharing(commands, first, last):
    after = first + 148
    if after > last:
        raise ValueError('unsupported/incomplete tier-sharing source rewrite')
    part = commands[first:after]
    payload = [(name, [(arg.value, arg.literal, arg.punctuation) for arg in args])
               for name, args in part]
    digest = hashlib.sha256(json.dumps(payload, separators=(',', ':')).encode()).hexdigest()
    helper = PORT / 'tools/tierscan.py'
    if (digest != TIER_SHARING_COMMANDS or not helper.is_file() or
            hashlib.sha256(helper.read_text(encoding='utf-8').encode()).hexdigest() != TIER_SHARING_TOOL):
        raise ValueError('tier-sharing source rewrite changed; source preservation needs review')
    return after


def _reject_source_rewrite(name, args, variables, helpers, active=(), targets_exist=False,
                           allow_loop_control=False, allow_return=False):
    if (name == 'return' and not allow_return) or (name in ('break', 'continue') and not allow_loop_control):
        raise ValueError('unsupported CMake control flow can skip source selection: ' + name)
    if name in helpers:
        if name in active:
            raise ValueError('recursive CMake helper cannot prove source selection')
        formal, body, kind = helpers[name]
        local = dict(variables)
        for index, key in enumerate(formal):
            local[key] = _cmake_values(args[index:index + 1], variables)
        for command, inner in body:
            # Function exits stay in the unmodeled helper, whose outputs are
            # invalidated. A macro return can leave the caller's whole file.
            _reject_source_rewrite(command, inner, local, helpers, active + (name,),
                                   targets_exist, allow_loop_control or kind == 'function',
                                   allow_return or kind == 'function')
        return
    if targets_exist and name in ('include', 'find_package', 'cmake_language'):
        raise ValueError('unevaluated CMake code can rewrite existing target sources: ' + name)
    words = [arg.value for arg in _cmake_values(args, variables)]
    if name == 'set_property' and words[:1] == ['TARGET'] and 'PROPERTY' in words:
        prop = words[words.index('PROPERTY') + 1:][:1]
        if not prop or prop == ['SOURCES'] or '$' in prop[0]:
            raise ValueError('unsupported target SOURCES rewrite; cannot retain selected owners')
    if name == 'set_target_properties' and 'PROPERTIES' in words:
        props = words[words.index('PROPERTIES') + 1::2]
        if any(prop == 'SOURCES' or '$' in prop for prop in props):
            raise ValueError('unsupported target SOURCES rewrite; cannot retain selected owners')


def _cmake_build_inputs(text, symbol_expansions=None):
    """Read the literal part of the host build graph without executing CMake.

    Only unconditional commands and foreach loops over known lists supply
    inputs. Unknown conditions and expressions cannot grant ownership. The
    result identifies selected source files and their generator commands;
    a symbol table or an unused generation rule alone supplies neither.
    """
    commands = list(_cmake_commands(text))
    helpers = _cmake_helper_writes(commands)
    env = {"CMAKE_CURRENT_SOURCE_DIR": [Token(str(PORT), 0)],
           "CMAKE_SOURCE_DIR": [Token(str(PORT), 0)],
           "CMAKE_BINARY_DIR": [Token(str(REPO / "build" / "port"), 0)]}
    targets, additions, generators = {}, [], []
    starts = {"if": "endif", "foreach": "endforeach", "while": "endwhile",
              "function": "endfunction", "macro": "endmacro", "block": "endblock"}

    def block_end(i, end):
        stack = [starts[commands[i][0]]]
        j = i + 1
        while j < end and stack:
            name = commands[j][0]
            if name in starts:
                stack.append(starts[name])
            elif name == stack[-1]:
                stack.pop()
            j += 1
        if stack:
            raise ValueError("unterminated CMake block")
        return j

    def invalidate(first, last, discard_loop=False):
        for name, args in commands[first:last]:
            # An unknown loop grants no input claims, so its local break or
            # continue cannot hide a selected body. Return still escapes it.
            _reject_source_rewrite(name, args, env, helpers, targets_exist=bool(targets),
                                   allow_loop_control=discard_loop)
            _invalidate_cmake_writes(name, args, env, helpers)

    def walk(first, last):
        i = first
        while i < last:
            name, args = commands[i]
            if name == 'set' and args and args[0].value == 'PORT_SHARE_TARGETS':
                after = _verified_tier_sharing(commands, i, last)
                # The pinned pass only writes its own scratch variables and
                # these partition lists. Retain the established source graph.
                for key in list(env):
                    if key.startswith('_') or key in ('PORT_SHARE_TARGETS', 'PORT_TIER_SHARED',
                                                      'PORT_TIER_PERTGT', 'PORT_DIVERGENT_DEFINES'):
                        env.pop(key, None)
                i = after
                continue
            _reject_source_rewrite(name, args, env, helpers, targets_exist=bool(targets))
            if name in starts:
                after = block_end(i, last)
                values = _cmake_values(args, env)
                words = [v.value for v in values]
                if name == "foreach" and words:
                    if len(words) > 2 and words[1:3] == ["IN", "LISTS"]:
                        rows = []
                        for var in words[3:]:
                            if var not in env:
                                rows = None
                                break
                            rows.extend(env[var])
                    elif "IN" not in words[1:] and "RANGE" not in words[1:]:
                        rows = values[1:]
                    else:
                        rows = None
                    if any(command in ('break', 'continue')
                           for command, _ in commands[i + 1:after - 1]):
                        # No branch evaluator is provided. Discard every
                        # input claim and variable write from this loop rather
                        # than executing declarations its control flow may skip.
                        rows = None
                    if rows is not None and all("$" not in v.value for v in rows):
                        previous = env.get(words[0])
                        for row in rows:
                            env[words[0]] = [row]
                            walk(i + 1, after - 1)
                        if previous is None:
                            env.pop(words[0], None)
                        else:
                            env[words[0]] = previous
                    else:
                        invalidate(i + 1, after - 1, discard_loop=True)
                elif name not in ("function", "macro"):
                    invalidate(i + 1, after - 1, discard_loop=(name == 'while'))
                i = after
                continue
            if name not in ('set', 'unset', 'list'):
                _invalidate_cmake_writes(name, args, env, helpers)
            values = _cmake_values(args, env)
            words = [v.value for v in values]
            if symbol_expansions is not None:
                key = args[0].value if name == 'set' and args else (
                    args[1].value if name == 'list' and len(args) > 1 else '')
                if re.fullmatch(r'\w+_SYMS', key):
                    start = 1 if name == 'set' else 3 if args[0].value == 'INSERT' else 2
                    for arg in args[start:]:
                        tokens = _cmake_values([arg], env)
                        if all('$' not in token.value for token in tokens):
                            symbol_expansions.setdefault((key, arg.pos), []).extend(tokens)
            if name == "set" and args:
                keys = _cmake_values(args[:1], env)
                key = keys[0].value if len(keys) == 1 else ''
                if not re.fullmatch(r'\w+', key):
                    env.clear()
                elif any(v.value in ("CACHE", "PARENT_SCOPE") for v in args[1:]):
                    env.pop(key, None)
                else:
                    env[key] = _cmake_values(args[1:], env)
            elif name == "unset" and args:
                _invalidate_cmake_writes(name, args, env, helpers)
            elif name == "list" and len(args) > 1:
                keys = _cmake_values(args[1:2], env)
                key = keys[0].value if len(keys) == 1 else ''
                if not re.fullmatch(r'\w+', key):
                    env.clear()
                elif args[0].value in ("LENGTH", "GET", "JOIN", "FIND", "SUBLIST"):
                    _invalidate_cmake_writes(name, args, env, helpers)
                elif args[0].value == "APPEND":
                    env.setdefault(key, []).extend(_cmake_values(args[2:], env))
                elif args[0].value == "REMOVE_ITEM" and key in env:
                    remove = {v.value for v in _cmake_values(args[2:], env)}
                    if any('$' in value for value in remove):
                        env.pop(key, None)
                    else:
                        env[key] = [v for v in env[key] if v.value not in remove]
                else:
                    _invalidate_cmake_writes(name, args, env, helpers)
            elif name == "add_executable" and words and "IMPORTED" not in words:
                targets[words[0]] = words[1:]
            elif name == "target_sources" and words:
                additions.append((words[0], words[1:]))
            elif name == "add_custom_command" and words[:1] == ["OUTPUT"]:
                if words.count("COMMAND") == 1:
                    start = words.index("COMMAND")
                    stop = next((j for j in range(start + 1, len(words))
                                 if words[j] in ("COMMAND", "DEPENDS", "COMMENT", "VERBATIM",
                                                 "WORKING_DIRECTORY", "BYPRODUCTS")), len(words))
                    generators.append((words[1:start], words[start + 1:stop]))
            i += 1
    walk(0, len(commands))
    for target, inputs in additions:
        if target in targets:
            targets[target].extend(inputs)

    def source_path(value):
        if "$" in value or pathlib.Path(value).suffix not in (".c", ".cpp"):
            return None
        path = pathlib.Path(value)
        path = path if path.is_absolute() else PORT / path
        path = path.resolve()
        return path if path.is_relative_to(REPO.resolve()) else None
    selected = {p for values in targets.values() for value in values
                if (p := source_path(value)) is not None}
    requests, by_output = [], {}
    for outputs, command in generators:
        active = {source_path(output) for output in outputs} & selected
        if not active:
            continue
        if len(command) > 1 and pathlib.Path(command[1]).name in ("romdata.py", "ovdata.py"):
            output_index = 2 if pathlib.Path(command[1]).name == "romdata.py" else 3
            if len(command) <= output_index or source_path(command[output_index]) not in active:
                continue
        for output in active:
            if output in by_output and by_output[output] != command:
                raise ValueError(f"conflicting generators for {output}")
            by_output[output] = command
        requests.append(command)
    return selected, requests


def _literal_assignments(path):
    """Read generator declarations without importing code or guessing mutations."""
    tree = ast.parse(path.read_text(encoding="utf-8"))
    result, assignments = {}, set()
    for node in tree.body:
        if isinstance(node, ast.Assign):
            for target in node.targets:
                if isinstance(target, ast.Name):
                    try:
                        result[target.id] = ast.literal_eval(node.value)
                        assignments.add(id(target))
                    except (ValueError, TypeError, SyntaxError):
                        result.pop(target.id, None)
    for node in ast.walk(tree):
        if isinstance(node, ast.Name) and isinstance(node.ctx, (ast.Store, ast.Del)) and id(node) not in assignments:
            result.pop(node.id, None)
        elif isinstance(node, ast.Subscript) and isinstance(node.ctx, (ast.Store, ast.Del)) and isinstance(node.value, ast.Name):
            result.pop(node.value.id, None)
        elif isinstance(node, ast.Call) and isinstance(node.func, ast.Attribute) and isinstance(node.func.value, ast.Name):
            result.pop(node.func.value.id, None)
    return result


def _data_symbols(module):
    path = REPO / "config" / module / "symbols.txt"
    if not path.is_file():
        return {}
    result = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        match = re.fullmatch(r"(\S+)\s+kind:(?:data|bss)(?:\([^)]*\))?\s+addr:0x([0-9a-fA-F]+)(?:\s+.*)?", line)
        if match:
            result[match.group(1)] = int(match.group(2), 16)
    return result


def _generated_data_owners(requests):
    """Follow the two data emitters' selected declaration inputs.

    The generators' TABLES/NAMED/CONTIG declarations and --from-list inputs
    are their ownership policy. Config only resolves names in those inputs;
    its other rows and whole-image mounts never create named host owners.
    This is an input-reference proof; the host build still checks emission,
    linkage, layout, and loaded bytes.
    """
    owners = set()
    def valid_size(value):
        return (not value or (bool(re.fullmatch(r"0x[0-9a-fA-F]+|[1-9][0-9]*", value)) and int(value, 0) > 0))
    for command in requests:
        if len(command) < 3 or command[0] != "python":
            continue
        script = pathlib.Path(command[1])
        if script not in (PORT / "tools/romdata.py", PORT / "tools/ovdata.py") or not script.is_file():
            continue
        args = [a for a in command[2:] if a not in
                ("${ROMCLEAN_FLAG}", "${LINKONLY_FLAG}", "--rom-clean", "--link-only")]
        if any("$" in a for a in args):
            continue
        provided, valid = set(), True
        if script.name == "romdata.py" and len(args) == 1:
            data = _literal_assignments(script)
            base, end = data.get("BASE"), data.get("BSS_START")
            if not isinstance(base, int) or not isinstance(end, int) or base >= end:
                continue
            symbols = _data_symbols("arm9")
            widths = {"int": 4, "unsigned int": 4, "short": 2,
                      "unsigned short": 2, "char": 1, "unsigned char": 1}
            if not all(key in data for key in ("TABLES", "NAMED", "CONTIG")):
                continue
            for row in data["TABLES"]:
                if (not isinstance(row, (list, tuple)) or len(row) != 3
                        or not isinstance(row[0], int) or not isinstance(row[1], int)
                        or row[2] not in widths or row[1] % widths[row[2]]
                        or not base <= row[0] < row[0] + row[1] <= end):
                    valid = False
                    break
                provided.add(f"data_{row[0]:08x}")
            for entry in data["NAMED"]:
                if not isinstance(entry, str):
                    valid = False
                    break
                name, _, size = entry.partition(":")
                if name not in symbols or not base <= symbols[name] < end or not valid_size(size):
                    valid = False
                    break
                provided.add(name)
            for row in data["CONTIG"]:
                if (not isinstance(row, (list, tuple)) or len(row) != 4
                        or not all(isinstance(x, int) for x in row[1:])
                        or not base <= row[1] < row[2] <= end or row[3] <= 0):
                    valid = False
                    break
                members = sorted((addr, name) for name, addr in symbols.items() if row[1] <= addr < row[2])
                if not members or members[0][0] != row[1]:
                    valid = False
                    break
                ends = [a for a, _ in members[1:]] + [row[2]]
                if any((stop - start) % row[3] for (start, _), stop in zip(members, ends)):
                    valid = False
                    break
                provided.update(name for _, name in members)
        elif script.name == "ovdata.py" and len(args) >= 4:
            module, _, mode, listing, *flags = args
            if not re.fullmatch(r"ov\d{3}", module) or mode != "--from-list" or any(f != "--pack" for f in flags):
                continue
            path = pathlib.Path(listing).resolve()
            if not path.is_relative_to(PORT.resolve()) or not path.is_file():
                continue
            symbols = _data_symbols("arm9/overlays/" + module)
            for line in path.read_text(encoding="utf-8").splitlines():
                if line.lstrip().startswith("#"):
                    continue
                for entry in line.split():
                    if entry == "--pack":
                        continue
                    name, _, size = entry.partition(":")
                    if name not in symbols or not valid_size(size):
                        valid = False
                        break
                    provided.add(name)
        if valid:
            owners.update(provided)
    return owners


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


def _expand_storage_declarations(text):
    """Expand only a literal external byte-array macro at its active uses.

    The declaration must contain exactly the section/alignment attributes
    below. Opaque bodies and conditional definitions remain unresolved. An
    undef or redefinition ends the preceding definition's lifetime.
    """
    joined = re.sub(r"\\\r?\n", "", _mask_comments(text))
    active, lines, depth = {}, [], 0
    expected = ('__pragma(section(SEC,read,write))extern"C"'
                '__declspec(allocate(SEC))__declspec(align(ALIGN))unsignedcharNAME[COUNT]')
    for line in joined.splitlines(keepends=True):
        directive = re.match(r"\s*#\s*(\w+)", line)
        if directive and directive.group(1) in ("if", "ifdef", "ifndef"):
            depth += 1
        definition = re.match(r"\s*#\s*(define|undef)\s+(\w+)(.*)", line)
        if definition:
            kind, name, tail = definition.groups()
            active.pop(name, None)
            match = re.fullmatch(r"\((\w+),\s*(\w+),\s*(\w+),\s*(\w+)\)\s*(.*)", tail)
            if kind == "define" and not depth and match and len(set(match.groups()[:4])) == 4:
                params = dict(zip(match.groups()[:4], ("SEC", "NAME", "COUNT", "ALIGN")))
                body = "".join(params.get(m.group(), m.group()) for m in CPP_TOKEN_RE.finditer(match.group(5)))
                if body == expected:
                    active[name] = re.compile(r"\b" + re.escape(name) +
                        r'\(\s*"[^"\r\n]*"\s*,\s*([A-Za-z_]\w*)\s*,\s*'
                        r'(0x[0-9a-fA-F]+|[0-9]+)\s*,\s*(0x[0-9a-fA-F]+|[0-9]+)\s*\)')
        elif not directive and not depth:
            for call in active.values():
                def replace(m):
                    count, alignment = int(m.group(2), 0), int(m.group(3), 0)
                    if count <= 0 or alignment <= 0 or alignment & (alignment - 1):
                        return m.group()
                    return 'extern "C" unsigned char %s[%s]' % (m.group(1), m.group(2))
                line = call.sub(replace, line)
        lines.append(line)
        if directive and directive.group(1) == "endif":
            depth = max(0, depth - 1)
    return "".join(lines)


def _host_c_definitions(text, pragma_macros=(), defined_macros=()):
    """Find simple, unconditional, externally linked HAL definitions.

    This is a reference gate, not a C++ compiler or a host link proof. Be
    conservative: declarations alone, function-local storage, class members,
    anonymous namespaces, static/const or unknown-type storage and conditional definitions do
    not supply a missing src/ name. C++-decorated aliases are not satisfied by
    a C definition. Unknown declaration forms remain unresolved.
    """
    text = _expand_storage_declarations(text)
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


def _unconditional_aliases(text):
    """Literal linker aliases outside conditionals, macro bodies, and strings."""
    depth, continuation = 0, False
    lines = []
    for line in _mask_comments(text).splitlines(keepends=True):
        directive = re.match(r"\s*#\s*(\w+)", line)
        if directive and directive.group(1) in ("if", "ifdef", "ifndef"):
            depth += 1
        masked = depth or continuation or (directive and directive.group(1) != "pragma")
        lines.append(_blank(line) if masked else line)
        if directive and directive.group(1) == "endif":
            depth = max(0, depth - 1)
        continuation = bool((directive or continuation) and line.rstrip().endswith("\\"))
    clean = "".join(lines)
    literals = [(m.start(), m.end()) for m in CPP_TOKEN_RE.finditer(clean)
                if '"' in m.group() or "'" in m.group()]
    for match in PRAGMA_RE.finditer(clean):
        if any(start <= match.start() < end for start, end in literals):
            continue
        alias, separator, target = match.group(1)[len("/alternatename:"):].partition("=")
        if separator and alias and target:
            yield alias.strip(), target.strip()


def _c_linker_identifier(spelling):
    # MSVC x86 adds exactly one underscore to a C identifier. Never erase
    # C++ decoration, import indirection, stdcall suffixes, or extra underscores.
    if not spelling.startswith("_") or spelling.startswith("__imp_"):
        return None
    name = spelling[1:]
    return name if re.fullmatch(r"[A-Za-z_]\w*", name) else None


def _resolved_aliases(texts, owners):
    aliases = {}
    for text in texts:
        for alias, target in _unconditional_aliases(text):
            aliases.setdefault(alias, set()).add(target)
    targets = {_c_linker_identifier(t) for ts in aliases.values() for t in ts}
    targets.discard(None)
    # Existing decomp enrollment is also an owner; a generated name can be
    # absent from src entirely. No alias creates ownership for its own RHS.
    roots = set(owners)
    for name in targets:
        path = SP.path_for(name)
        if path is not None and path.is_file():
            roots.add(name)

    def resolves(spelling, trail):
        if _c_linker_identifier(spelling) in roots:
            return True
        if spelling in trail or len(aliases.get(spelling, ())) != 1:
            return False
        return resolves(next(iter(aliases[spelling])), trail | {spelling})
    return {spelling for spelling in aliases if resolves(spelling, set())}


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
    host definition or selected named-data input. Extern-only declarations
    and unselected generator/list files cannot supply an owner."""
    hal_dir = PORT / "hal"
    if not hal_dir.is_dir():
        return 0, [Failure("port/hal", 0, "hal directory not found")]

    # Pass 1: collect every candidate reference without checking existence
    # yet, so all the (few hundred) names can be looked up in one batched
    # `git grep` pass instead of one lookup each.
    refs = []  # (rel, lineno, name, message, C linkage)
    host_owners = set()
    cmake = PORT / "CMakeLists.txt"
    try:
        selected, requests = _cmake_build_inputs(cmake.read_text(encoding="utf-8")) if cmake.is_file() else (set(), [])
        generated = _generated_data_owners(requests)
    except (OSError, ValueError, SyntaxError, TypeError) as exc:
        return 0, [Failure("port/CMakeLists.txt", 0, f"cannot establish host inputs: {exc}")]
    host_owners.update(generated)
    texts = {p: p.read_text(encoding="utf-8", errors="ignore")
             for p in sorted(hal_dir.iterdir()) if p.suffix in CODE_SUFFIXES}
    # Only files selected by an actual executable can add unmatched owners.
    # Unused files and declarations elsewhere are not a substitute for a link input.
    unmatched = {p: p.read_text(encoding="utf-8", errors="ignore") for p in selected
                 if p.is_relative_to((PORT / "unmatched").resolve()) and p.is_file()}
    all_texts = [*texts.values(), *unmatched.values()]
    pragma_macros = _pragma_only_macros(all_texts)
    defined_macros = _defined_macro_names(all_texts)
    for path, text in unmatched.items():
        source = 'extern "C" {\n' + text + '\n}' if path.suffix == ".c" else text
        host_owners.update(_host_c_definitions(source, pragma_macros, defined_macros))
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

    # A selected alias can own its LHS only after its ultimate target has
    # an independent owner. Cycles, conflicting mappings, and stale RHS names
    # remain failures. C++ aliases are kept as exact decorated spellings.
    alias_texts = [text for path, text in {**texts, **unmatched}.items()
                   if path.resolve() in selected]
    aliases = _resolved_aliases(alias_texts, host_owners)
    alias_sites = set()
    for path, text in {**texts, **unmatched}.items():
        if path.resolve() not in selected:
            continue
        for match in PRAGMA_RE.finditer(_mask_comments(text)):
            lhs = match.group(1)[len("/alternatename:"):].partition("=")[0].strip()
            if lhs in aliases:
                alias_sites.add((path.relative_to(REPO).as_posix(),
                                 _line_at(text, match.start()), _strip_decoration(lhs)))
    host_owners.update(name for spelling in aliases
                       if (name := _c_linker_identifier(spelling)) is not None)
    # Pass 2: one batched presence check, then report.
    present = _present(name for _, _, name, _, _ in refs)
    failures = []
    seen = set()  # (file, name) already reported, so a name isn't repeated
    for rel, lineno, name, message, c_linkage in refs:
        if (name in present or (c_linkage and name in host_owners)
                or (rel, lineno, name) in alias_sites):
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
