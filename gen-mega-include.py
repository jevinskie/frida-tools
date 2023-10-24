#!/usr/bin/env python3

import subprocess

from path import Path

sdk_root = Path(
    subprocess.run(
        ["xcrun", "--sdk", "iphoneos", "--show-sdk-path"],
        capture_output=True,
        check=True,
        text=True,
    ).stdout.strip()
)
print(sdk_root)

frameworks_root = sdk_root / "System" / "Library" / "Frameworks"
usr_include_root = sdk_root / "usr" / "include"
cpp_include_root = usr_include_root / "c++"

denylist = tuple(
    map(
        Path,
        (
            "corpses/task_corpse.h",
            "mach/mach_eventlink.h",
            "mach/machine/asm.h",
            "mach/mach_vm.h",
            "os/_workgroup.h",
            "netinet6/in6.h",
            "unwind_arm_ehabi.h",
            "mach/arm/ndr_def.h",
            "rpcsvc/nfs_prot.h",
        ),
    )
)

with open("mega-include.h", "w") as of:
    for f in frameworks_root.walk(match="*.framework"):
        if any([p.endswith(".framework") for p in f.parts()[:-1]]):
            print(f"bad fw: {f}")
            continue
        h = f / "Headers" / f"{f.stem}.h"
        if h.exists():
            print(f"#import <{f.stem}/{f.stem}.h>", file=of)
    for i in usr_include_root.walk(match="*.h"):
        h = usr_include_root.relpathto(i)
        if "c++" == h.parts()[1]:
            print(f"bad c++: {h}")
            continue
        if h in denylist:
            print(f"denied: {h}")
            continue
        if "AppleArchive" == h.parts()[1] and not h.name == "AppleArchive.h":
            print(f"blocked: {h}")
            continue
        print(f"#include <{h}>", file=of)
