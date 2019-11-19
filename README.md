# SIMDExtensionWrapper
Generic wrapper program, detects SIMD CPU extensions and launches an appropriate sub-process.

# Usage
Will launch one of: <ProgramName>.avx2, <ProgramName>.avx, <ProgramName>.sse42, <ProgramName>.sse41 or <ProgramName>.noext as appropriate (passing all parameters). Executables must be on the same path as the wrapper. More extensions will be added when needed.

# Building
Just compile SIMDExtensionWrapper.cpp. Define the macro "ProgramName" (... -DProgramName=MyProgramName) to decorate output messages.<br/>
Tested on Linux, MacOS and Windows using clang.

# Third-Party acknowledgements
stb_sprintf.h (https://github.com/nothings/stb/blob/master/stb_sprintf.h)

