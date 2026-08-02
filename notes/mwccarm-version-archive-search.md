# Metrowerks `mwccarm` Version Archive Search Playbook

Last updated: 2026-07-27

## Purpose

This document is a search and preservation guide for locating historically
accurate Metrowerks CodeWarrior ARM compiler releases associated with Nintendo
IRIS, NITRO, and early Nintendo DS development.

The immediate goal is to find additional `mwccarm.exe` compiler builds from the
2003-2004 period, especially compiler builds 46, 49, 50, and 56. The corresponding
Nintendo releases range from NITRO V0.5 through NITRO V1.0.

Finding a compiler that reproduces one function is strong evidence for that
function, but it does **not** establish that the entire game was built with that
compiler. A compiler change can improve one function and regress another, and a
game may contain objects or libraries built with different tool revisions.

This guide therefore treats each recovered compiler as another candidate to add
to the repository's compiler matrix, not as an automatic project-wide
replacement.

## Important correction: compiler builds versus linker builds

Nintendo's CodeWarrior build rules record separate build numbers for the C/C++
compiler and linker:

- `SDK_CW_BUILD_NUMBER_CC`: compiler build
- `SDK_CW_BUILD_NUMBER_LD`: linker build

The previously discussed values 58, 61, and 62 are linker builds, not compiler
builds. The compiler builds paired with them are 46, 49, and 50.

| Nintendo release | Compiler build | Linker build |
|---|---:|---:|
| IRIS V0.2 Hotfix 1 | 0038 | 0050 |
| NITRO V0.3 | 0038 | 0056 |
| NITRO V0.4.1 | 0043 | 0057 |
| NITRO V0.5 | 0046 | 0058 |
| NITRO V0.5 + HotFix1 | 0049 | 0061 |
| NITRO V0.6 | 0050 | 0062 |
| NITRO V0.6.1 | 0050 | 0063 |
| NITRO V0.7 | 0050 | 0063 |
| NITRO V1.0 | 0056 | 0067 |
| NINTENDO DS V1.0.2 | 0068 | 0068 |
| NINTENDO DS V1.2 | 0073 | 0072 |
| NINTENDO DS V1.2 SP1 | 0074 | 0072 |
| NINTENDO DS V1.2 SP2 + patch | 0080 | 0074 |
| NINTENDO DS V1.2 SP3 | 0084 | 0077 |
| NINTENDO DS V1.2 SP4 | 0087 | 7702 |

Source:
[Nintendo `modulerules.cctype.CW` build mapping](https://git.randommeaninglesscharacters.com/rvtr/twl_wrapsdk/src/commit/756532724089085f84f3a82ab5f4b65d37422509/build/buildtools/modulerules.cctype.CW)

### Current search priority

1. Compiler build 56 / linker build 67: NITRO V1.0
2. Compiler build 50 / linker build 63: NITRO V0.6.1 or V0.7
3. Compiler build 50 / linker build 62: NITRO V0.6
4. Compiler build 49 / linker build 61: NITRO V0.5 + HotFix1
5. Compiler build 46 / linker build 58: NITRO V0.5
6. Compiler build 68 / linker build 68: useful later boundary candidate

Compiler build 56 has already been recovered from the public ARM update
`CW_ARM_2.1.1_Update.exe`. This makes it a particularly plausible candidate, but
does not by itself prove that it is the exact compiler used for SM64DS.

Known build-56 banner:

```text
Metrowerks C/C++ for Embedded ARM.
Version 2.0 build 56 (build 0056)
Runtime Built: Sep 16 2004 13:20:23
```

Known build-56 `mwccarm.exe`:

```text
Size:   2,248,704 bytes
SHA-1:  8eb0b9653ea1c9a589c3a4399e37e2780059a818
SHA-256: c6aafc210c78e91b123e8e949b8e26faa76849225b9f639780fcc9db6cdff0b7
```

## Primary historical clues

Nintendo's September 2004 Ensata setup guide instructs developers to download
"CodeWarrior for NITRO" from WarioWorld and names several useful search
fingerprints:

- `Developer_Notes - NITRO_Tools.txt`
- `CWFolder_NITRO`
- `license.dat`

Source:
[Official Ensata setup guide](https://twlsdk.randommeaninglesscharacters.com/download/Ensata/SetupGuide_for_ensata.pdf)

An Internet Archive collection preserves approximately 5.1 GB from the former
Metrowerks updates FTP. Its description traces the preservation effort to a
BetaArchive discussion:

- [Internet Archive Metrowerks FTP collection](https://archive.org/details/ftp_metrowerks_updates.7z)
- [BetaArchive Metrowerks FTP preservation thread](https://www.betaarchive.com/forum/viewtopic.php?t=36389)

The BetaArchive thread identifies several former FTP or mirror locations:

```text
ftp://ftp.metrowerks.com/pub/updates/
ftp://ftp2.metrowerks.com/pub/updates/
ftp://206.204.30.115/pub/updates/
ftp://ftp.promo.de/pub/Metrowerks/mirror/updates/
ftp://ftp.cs.tu-berlin.de/
ftp://ftp.padua.org/pub/mac/lang/Metrowerks/
```

The archived directories reportedly contained `index.html` inventories. Those
indexes are valuable even when the corresponding installer is missing because
they may reveal its exact filename.

## Search method

Use one query at a time. Begin with every distinctive term quoted, then remove
one constraint at a time if there are no results.

Recommended order:

1. Exact Nintendo product name
2. Compiler and linker build-number pair
3. Exact banner text
4. Installer filename
5. Internal executable or documentation filename
6. Installation path
7. Archived FTP directory or `index.html`
8. Source-tree and repository signatures
9. Message-board and preservation-community outreach
10. Non-English searches

Use multiple search engines. Executables are rarely indexed by content, and
different engines have very different coverage of old forums, FTP indexes, and
foreign-language download pages.

## Highest-priority queries

```text
"CodeWarrior for NITRO V1.0"
"CodeWarrior for NITRO V1.0" mwccarm
"CodeWarrior for NITRO V0.6.1"
"CodeWarrior for NITRO V0.6" mwccarm
"CodeWarrior for NITRO V0.5 + HotFix1"
"CodeWarrior for NITRO V0.5"
"CW for NINTENDO DS V1.0.2"
"CodeWarrior for NINTENDO DS V1.0"
"CodeWarrior for NITRO" archive
"CodeWarrior for NITRO" installer
"CodeWarrior for NITRO" download
"CodeWarrior for NITRO" ISO
"Metrowerks C/C++ for Embedded ARM" "build 56"
"Version 2.0 build 56 (build 0056)"
"Runtime Built: Sep 16 2004 13:20:23"
"SDK_CW_BUILD_NUMBER_CC" "0056"
"SDK_CW_BUILD_NUMBER_CC" "0050"
"SDK_CW_BUILD_NUMBER_CC" "0049"
"SDK_CW_BUILD_NUMBER_CC" "0046"
"ARM_Tools/Command_Line_Tools/mwccarm.exe"
```

## Compiler and linker fingerprint queries

Searching the compiler and linker numbers together is more distinctive than
searching either one alone.

```text
"SDK_CW_BUILD_NUMBER_CC" "0046" "SDK_CW_BUILD_NUMBER_LD" "0058"
"SDK_CW_BUILD_NUMBER_CC" "0049" "SDK_CW_BUILD_NUMBER_LD" "0061"
"SDK_CW_BUILD_NUMBER_CC" "0050" "SDK_CW_BUILD_NUMBER_LD" "0062"
"SDK_CW_BUILD_NUMBER_CC" "0050" "SDK_CW_BUILD_NUMBER_LD" "0063"
"SDK_CW_BUILD_NUMBER_CC" "0056" "SDK_CW_BUILD_NUMBER_LD" "0067"
"SDK_CW_BUILD_NUMBER_CC" "0068" "SDK_CW_BUILD_NUMBER_LD" "0068"

"Metrowerks C/C++ for Embedded ARM" "build 46"
"Metrowerks C/C++ for Embedded ARM" "build 49"
"Metrowerks C/C++ for Embedded ARM" "build 50"
"Metrowerks C/C++ for Embedded ARM" "build 56"
"Metrowerks C/C++ for Embedded ARM" "build 68"

"Version 2.0 build 46" mwccarm
"Version 2.0 build 49" mwccarm
"Version 2.0 build 50" mwccarm
"Version 2.0 build 56" mwccarm
"Version 2.0 build 68" mwccarm

"Version 2.0 build 0046" ARM
"Version 2.0 build 0049" ARM
"Version 2.0 build 0050" ARM
"Version 2.0 build 0056" ARM
"Version 2.0 build 0068" ARM

"Metrowerks C/C++ for ARM v1.0a1"
"Runtime Built" mwccarm 2004
"Runtime Built:" "Metrowerks C/C++ for Embedded ARM"
```

Also try variations such as:

```text
CC=0056
CC 0056
CC_BUILD=0056
BUILD_NUMBER_CC=0056
SDK_CW_BUILD_NUMBER_CC=0056
SDK_CW_BUILD_NUMBER_CC 56
```

Repeat those variations for builds 46, 49, 50, and 68.

## Installer filename searches

### Confirmed filename

```text
"CW_ARM_2.1.1_Update.exe"
"CW_ARM_2.1.1_Update.exe" Metrowerks
"CW_ARM_2.1.1_Update.exe" archive
"CW_ARM_2.1.1_Update.exe" ftp
```

### Generated public ARM filename permutations

These are search candidates, not confirmed historical filenames:

```text
"CW_ARM_2.1_Update.exe"
"CW_ARM_2.1.0_Update.exe"
"CW_ARM_2.0.1_Update.exe"
"CW_ARM_2.0_Update.exe"
"CW_ARM_2.0_Update"
"CWARM_2.1_Update.exe"
"CWARM_2.0_Update.exe"
"CWARM21_Update.exe"
"CWARM20_Update.exe"
"CWARM2_Update.exe"
```

### Generated Nintendo filename permutations

These are also search candidates rather than confirmed filenames:

```text
"CW_NINTENDO_DS_R"
"CW_NINTENDO_DS_R1.exe"
"CW_NINTENDO_DS_R1_0.exe"
"CW_NINTENDO_DS_R1.0.exe"
"CW_NINTENDO_DS_R1_0_2.exe"
"CW_NINTENDO_DS_R0_7.exe"
"CW_NINTENDO_DS_R0_6_1.exe"
"CW_NINTENDO_DS_R0.6.1.exe"
"CW_NINTENDO_DS_R061.exe"
"CW_NINTENDO_DS_R0_6.exe"
"CW_NINTENDO_DS_R0_5.exe"

"NITRO_Tools.exe"
"NITRO Tools" CodeWarrior installer
"Developer_Notes - NITRO_Tools.txt"
"Developer Notes NITRO Tools" CodeWarrior
```

When an exact filename produces no results, search fragments:

```text
"CW_ARM_" Metrowerks
"CW_NINTENDO_DS_"
"NINTENDO_DS_R" CodeWarrior
"NITRO_Tools" Metrowerks
```

## Internal filenames and installation paths

These queries can locate directory listings, manifests, installer logs, and
source trees where the complete compiler directory was accidentally preserved.

```text
"ARM_Tools/Command_Line_Tools/mwccarm.exe"
"ARM_Tools\Command_Line_Tools\mwccarm.exe"
"sdk/cw/ARM_Tools/Command_Line_Tools"
"C:/Program Files/Metrowerks/CW for NINTENDO DS"
"Program Files/Metrowerks/CW for NINTENDO DS V1.2"

"mwccarm.exe" "mwldarm.exe"
"mwccarm.exe" "mwasmarm.exe"
"mwccarm.exe" "mwldarm.exe" "mwasmarm.exe"
"mwccarm.exe" "ELFIO.dll"
"mwccarm.exe" "MSL_All-DLL80_x86.dll"
"mwccarm.exe" "lmgr8c.dll"

"ARM_Compiler_Notes.txt"
"ARM_Compiler_Notes.txt" Metrowerks
"ARM_EABI_Support" mwccarm
"verinfo.cw.cc"
"verinfo.cw.ld"
"commondefs.cctype.CW"
"modulerules.cctype.CW"

"CWFOLDER_IRIS"
"CWFOLDER_NITRO"
"CWFolder_NITRO"
"CW_NITROSDK_ROOT"
"NITROSDK_ROOT" "CWFolder_NITRO"
"LM_LICENSE_FILE" "CWFolder_NITRO"
```

## Public ARM product-line searches

Nintendo-branded development media may be scarce, but the same compiler lineage
appeared in public CodeWarrior ARM products. Old evaluation CDs, hardware-board
bundles, university mirrors, magazine cover discs, and secondhand boxed copies
may therefore contain the required compiler builds.

```text
"CodeWarrior Development Studio ARM ISA Edition Version 2"
"CodeWarrior Development Studio, ARM ISA Edition Version 2"
"CodeWarrior Development Studio for ARM Embedded Systems"
"CodeWarrior ARM ISA Edition" 2004
"CodeWarrior ARM ISA Edition" update
"CodeWarrior ARM ISA Edition" installer
"CodeWarrior ARM ISA Edition" ISO
"CodeWarrior ARM ISA Edition" CD
"CodeWarrior ARM ISA Edition" evaluation
"CodeWarrior ARM ISA Edition" "30-day evaluation"
"Metrowerks ARM ISA Edition" download
"Metrowerks ARM ISA Edition" archive

"CWARM" Metrowerks
"CWARM2BHB"
"CWARMHBB"
"CWS-ARM-HWBBU-CX"
"CWA-ARM-LWAPP-CX"
"CodeWarrior ARM" "Hardware Board Bring-Up"
"CodeWarrior ARM ISA" "Board Bring-Up"

"M9328MX1ADS" CodeWarrior
"i.MX21ADS" CodeWarrior ARM
"OMAP" "CodeWarrior ARM ISA Edition"
"XScale" "CodeWarrior ARM ISA Edition"
"Quadros RTXC" "CodeWarrior ARM"
"Virtio" "CodeWarrior ARM ISA"
```

## Archive and preservation-site searches

```text
site:archive.org/details "CodeWarrior ARM ISA"
site:archive.org/details "CodeWarrior ARM"
site:archive.org/details Metrowerks CWARM
site:archive.org/details mwccarm
site:archive.org/download "CW_ARM"
site:archive.org/download "CWARM"
site:archive.org/download "NITRO_Tools"
site:archive.org/download "mwccarm.exe"

site:betaarchive.com "CodeWarrior ARM"
site:betaarchive.com/forum "CodeWarrior ARM"
site:betaarchive.com/forum Metrowerks FTP
site:forum.winworldpc.com "CodeWarrior ARM"
site:forum.winworldpc.com Metrowerks ARM

site:discmaster.textfiles.com mwccarm
site:discmaster.textfiles.com "mwccarm.exe"
site:discmaster.textfiles.com "ARM_Compiler_Notes.txt"
site:discmaster.textfiles.com "CodeWarrior ARM ISA"
site:discmaster.textfiles.com CWARM
```

Search the literal former FTP locations:

```text
"ftp.metrowerks.com/pub/updates" ARM
"ftp.metrowerks.com/pub/updates" CWARM
"ftp2.metrowerks.com/pub/updates" ARM
"206.204.30.115/pub/updates" Metrowerks
"ftp.promo.de/pub/Metrowerks/mirror/updates" ARM
"ftp.padua.org/pub/mac/lang/Metrowerks" ARM
"ftp.cs.tu-berlin.de" Metrowerks ARM
"index.html" "CW_ARM_2"
"index.html" "CodeWarrior ARM ISA"
```

Try these URL patterns directly in the Wayback Machine:

```text
https://web.archive.org/web/*/ftp://ftp.metrowerks.com/pub/updates/*
https://web.archive.org/web/*/http://ftp.metrowerks.com/pub/updates/*
https://web.archive.org/web/*/http://www.metrowerks.com/*ARM*
https://web.archive.org/web/*/http://www.warioworld.com/*nitro*
https://web.archive.org/web/*/https://www.warioworld.com/*nitro*
```

Captured directory indexes may be more useful than captured product pages.

## Source-tree and code-hosting searches

```text
site:github.com "CWFOLDER_NITRO"
site:github.com "CWFolder_NITRO"
site:github.com "SDK_CW_BUILD_NUMBER_CC"
site:github.com "ARM_Tools/Command_Line_Tools"
site:github.com "mwccarm.exe" Nintendo
site:gitlab.com "CWFolder_NITRO"
site:gitlab.com "mwccarm.exe"
site:gitee.com "mwccarm.exe"

"retsam_00jupc" mwccarm
"retsam_00jupc" "ARM_Tools"
"master_cpuj00" "ARM_Tools"
"master_cpuj00" mwccarm
"pokemon_dp" "sdk/cw"
"pokemon_dp" mwccarm
"Paladin" "mwccarm.exe"
"gigaleak_20231201" "ARM_Tools"
"twl_wrapsdk" CodeWarrior
"ctr_firmware" mwccarm
"NitroSDK" "CodeWarrior for NITRO V1.0"
"irisSDK-20031203.tar.gz"
```

GitHub Code Search queries:

```text
path:ARM_Tools/Command_Line_Tools
path:mwccarm.exe
path:ARM_Compiler_Notes.txt
"CWFolder_NITRO"
"CWFOLDER_IRIS"
"SDK_CW_BUILD_NUMBER_CC"
"CodeWarrior for NITRO V1.0"
```

Check more than the repository's default branch:

- forks
- old tags
- release assets
- Git LFS pointers
- deleted-file history
- archived repository bundles
- source packages attached to forum posts

## Message-board and newsgroup searches

```text
site:groups.google.com/g/codewarrior.embedded mwccarm
site:groups.google.com/g/codewarrior.embedded "ARM ISA"
site:groups.google.com "CW_ARM_2.1"
site:groups.google.com "CodeWarrior for NITRO"
site:mactech.com Metrowerks "ARM ISA"
site:community.nxp.com "CodeWarrior ARM ISA"
site:community.nxp.com mwccarm

site:forum.vcfed.org "CodeWarrior ARM"
site:vogons.org "CodeWarrior ARM"
site:obscuregamers.com "Nitro SDK" CodeWarrior
site:gbatemp.net NitroSDK CodeWarrior
site:projectpokemon.org NitroSDK CodeWarrior

"MW Ron" "ARM ISA Edition"
"Ron Liechty" "CodeWarrior ARM"
"MWRon@metrowerks.com" ARM
"CodeWarrior ARM ISA Edition" announcement
"CodeWarrior ARM ISA Edition" release notes
```

Potential communities for a preservation request:

- BetaArchive
- WinWorld Offers & Requests
- Vintage Computer Federation
- classiccmp
- VOGONS
- ObscureGamers and ASSEMBLERgames archives
- GBATemp and Project Pokémon
- `r/DataHoarder`
- `r/vintagecomputing`
- `r/decomps`
- `r/embedded`
- former Metrowerks, Freescale, or NXP developer communities

## Non-English searches

### Japanese

```text
"CodeWarrior for NITRO" ダウンロード
"CodeWarrior for NITRO V1.0"
コードウォリア NITRO 開発環境
ニンテンドーDS 開発環境 CodeWarrior
NITRO-SDK CodeWarrior 開発ツール
"mwccarm.exe" ニンテンドーDS
Metrowerks ARM コンパイラ 2004
CodeWarrior ARM 評価版
```

### Chinese

```text
"CodeWarrior for NITRO" 下载
Metrowerks CodeWarrior ARM 下载
"mwccarm.exe" 下载
任天堂 NITRO 开发工具 CodeWarrior
NDS 开发包 CodeWarrior
CodeWarrior ARM 光盘
CodeWarrior ARM 安装包
```

### Russian

```text
CodeWarrior ARM скачать
Metrowerks ARM ISA скачать
Nintendo Nitro SDK CodeWarrior скачать
"mwccarm.exe" скачать
CodeWarrior ARM образ диска
```

### German and French

```text
CodeWarrior ARM ISA Download 2004
CodeWarrior ARM ISA Installations-CD
télécharger CodeWarrior ARM ISA Metrowerks
CodeWarrior ARM ISA CD-ROM Metrowerks
```

Download portals found through these queries should be treated as leads rather
than trusted software sources.

## Hash and exact-size searches

Search the known build-56 values verbatim:

```text
"8eb0b9653ea1c9a589c3a4399e37e2780059a818"
"c6aafc210c78e91b123e8e949b8e26faa76849225b9f639780fcc9db6cdff0b7"
"mwccarm.exe" "2248704"
"CW_ARM_2.1.1_Update.exe" "24337350"
```

Exact hashes can surface:

- package manifests
- malware-analysis inventories
- old torrent descriptions
- duplicate archive collections
- software catalogs
- preservation databases

## False-positive filters

Broad `CodeWarrior` searches are dominated by later Freescale/NXP products and
non-ARM targets. Append this suffix when necessary:

```text
-Kinetis -ColdFire -PowerPC -S12 -HC12 -MCU10 -PSP -PlayStation -Minecraft
```

Do not use those exclusions on archive collections. A mixed CodeWarrior CD may
contain several architectures, including the desired ARM tools.

Also distinguish the ARM Ltd compiler `armcc` from the Metrowerks executable
`mwccarm.exe`.

## Shareable preservation request

> Seeking preservation information or media for Metrowerks CodeWarrior for
> NITRO / CodeWarrior ARM ISA, circa 2003-2004. Highest-priority compiler
> builds are 46, 49, 50, and 56, associated with NITRO V0.5 through V1.0.
> Useful evidence includes installer filenames, CD or FTP directory listings,
> `mwccarm.exe -version` output, file size, PE timestamp, and SHA-256. No
> license key is requested. Even an old screenshot or file inventory could
> identify the missing installer.

Suggested locations for owners of old development machines or backups to check:

```text
C:\Program Files\Metrowerks\
ARM_Tools\Command_Line_Tools\
sdk\cw\ARM_Tools\Command_Line_Tools\
```

Also ask people to inspect:

- retired development PCs
- NAS and workstation backups
- burned CD-Rs and DVDs
- board-support and evaluation CDs
- university FTP mirrors
- magazine cover discs
- old source checkout archives
- installer caches

## Candidate-handling safety

Do not execute an unknown installer or compiler directly on a normal
workstation.

For every candidate:

1. Preserve the original archive unchanged.
2. Calculate SHA-256 and file size.
3. Record the source URL and download date.
4. Extract it offline where possible.
5. Inspect PE version resources, timestamps, imports, and printable strings.
6. Scan it with multiple malware-analysis tools.
7. Run it only in an isolated disposable VM if execution is necessary.
8. Capture the complete `mwccarm.exe -version` output.
9. Preserve adjacent DLLs, release notes, linker, assembler, headers, and
   directory structure.
10. Never request or publish a private license key.

Useful evidence can often be gathered without executing anything:

- archive file listing
- installer metadata
- embedded version strings
- PE version resources
- digital-signature information
- executable file size
- cryptographic hashes
- release notes

## Hunt ledger template

Keep a shared ledger so searches are not repeatedly rediscovered:

| Date | Query or source | Result | Artifact | Build/banner | Size | SHA-256 | Follow-up |
|---|---|---|---|---|---:|---|---|
| YYYY-MM-DD | Search query or URL | none / lead / recovered | filename | version text | bytes | hash | next action |

For an archive containing many possible files, record both the outer archive
hash and the inner executable hash.

## Success criteria

A recovered compiler candidate is useful when at least the following are known:

- original or preserved installer/archive filename
- `mwccarm.exe` file size
- SHA-256
- compiler banner and build number
- adjacent runtime DLLs required to launch it
- provenance or source URL

It becomes a strong SM64DS compiler candidate only after it is added to a
controlled compiler matrix and tested across multiple already-matched functions.
The correct conclusion may be that different object groups correspond to
different compiler builds.
