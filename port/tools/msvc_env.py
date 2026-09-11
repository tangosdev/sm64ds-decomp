"""Locate an installed x86 MSVC environment and the port's CMake/Ninja tools."""
import argparse
from dataclasses import dataclass
import os
from pathlib import Path
import shutil
import subprocess
import sys


class ToolchainError(RuntimeError):
    pass


def batch_path(path):
    value = str(Path(path).resolve())
    if any(c in value for c in '\r\n"%!^&|<>'):
        raise ToolchainError('Tool path cannot be represented safely in a batch file: ' + value)
    try:
        value.encode('ascii')
    except UnicodeEncodeError:
        raise ToolchainError('Tool path requires an unsupported batch encoding: ' + value)
    return value


@dataclass(frozen=True)
class Toolchain:
    vcvars: Path
    cmake: Path
    ninja: Path

    def batch_header(self):
        vcvars, cmake, ninja = map(batch_path, (self.vcvars, self.cmake, self.ninja))
        return ('@echo off\r\n'
                'call "%s" >nul\r\nif errorlevel 1 exit /b 1\r\n'
                'set "PORT_CMAKE=%s"\r\nset "PORT_NINJA=%s"\r\n'
                'set "PATH=%s;%s;%%PATH%%"\r\n'
                % (vcvars, cmake, ninja, Path(cmake).parent, Path(ninja).parent))


def discover_vcvars(environ=None, run=None):
    environ = os.environ if environ is None else environ
    run = subprocess.run if run is None else run
    pf = Path(environ.get('ProgramFiles(x86)', r'C:\Program Files (x86)'))
    candidates = []
    if environ.get('VSINSTALLDIR'):
        candidates.append(Path(environ['VSINSTALLDIR']))
    candidates.append(pf / 'Microsoft Visual Studio/2022/BuildTools')
    vswhere = pf / 'Microsoft Visual Studio/Installer/vswhere.exe'
    if not any((path / 'VC/Auxiliary/Build/vcvars32.bat').is_file() for path in candidates) and vswhere.is_file():
        try:
            result = run([str(vswhere), '-latest', '-products', '*', '-requires',
                          'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
                          '-property', 'installationPath'],
                         capture_output=True, text=True, timeout=15, check=False)
        except (OSError, subprocess.TimeoutExpired) as exc:
            raise ToolchainError('Visual Studio discovery failed: ' + str(exc)) from exc
        if result.returncode:
            raise ToolchainError('vswhere failed with exit code %s' % result.returncode)
        lines = [line.strip() for line in result.stdout.splitlines() if line.strip()]
        if len(lines) > 1 or any(not Path(line).is_absolute() for line in lines):
            raise ToolchainError('vswhere returned an invalid latest installation path')
        candidates.extend(Path(line) for line in lines)
    vs = next((path for path in candidates
               if (path / 'VC/Auxiliary/Build/vcvars32.bat').is_file()), None)
    if vs is None:
        raise ToolchainError('No installed Visual Studio x86 C++ toolchain provides vcvars32.bat')
    vcvars = vs / 'VC/Auxiliary/Build/vcvars32.bat'
    batch_path(vcvars)
    return vcvars.resolve()


def discover(environ=None, run=None, which=None):
    environ = os.environ if environ is None else environ
    which = shutil.which if which is None else which
    vcvars = discover_vcvars(environ, run)
    vs = vcvars.parents[3]
    bundled = vs / 'Common7/IDE/CommonExtensions/Microsoft/CMake'
    def tool(name, relative):
        path = bundled / relative
        if not path.is_file():
            found = which(name, path=environ.get('PATH', ''))
            if not found or not Path(found).is_file():
                raise ToolchainError(name + ' was not found in Visual Studio or PATH')
            path = Path(found)
        batch_path(path)
        return path.resolve()
    return Toolchain(vcvars, tool('cmake', 'CMake/bin/cmake.exe'),
                     tool('ninja', 'Ninja/ninja.exe'))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write', type=Path, required=True,
                        help='write the shared environment setup as a batch file')
    args = parser.parse_args()
    try:
        selected = discover()
        args.write.parent.mkdir(parents=True, exist_ok=True)
        args.write.write_text(selected.batch_header(), encoding='ascii', newline='')
    except (ToolchainError, OSError) as exc:
        print('MSVC setup: ' + str(exc), file=sys.stderr)
        return 1
    print('MSVC x86: ' + str(selected.vcvars))
    print('CMake: ' + str(selected.cmake))
    print('Ninja: ' + str(selected.ninja))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
