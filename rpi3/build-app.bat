@echo off
REM
REM  Customized from:
REM     https://github.com/LdB-ECM/Raspberry-Pi/blob/master/SD_FAT32/Pi3-64.bat
REM


@REM COMPILER COMMAND LINE
@echo off
set "bindir=D:\Program Files (x86)\gcc-arm-8.2-2019.01-i686-mingw32-aarch64-elf\bin\"
set "cpuflags=-O3 -march=armv8-a+simd -mtune=cortex-a53 -mstrict-align -fno-tree-loop-vectorize -fno-tree-slp-vectorize"
set "asmflags=-nostdlib -nostartfiles -ffreestanding -fno-asynchronous-unwind-tables -fomit-frame-pointer -Wa,-a>output/app.txt"
set "linkerflags=-Wl,-gc-sections -Wl,--build-id=none -Wl,-Bdynamic -Wl,-Map,output/app.map"
set "outflags=-o output/app.elf"
set "libflags=-lc -lm -lgcc"
@echo on
"%bindir%aarch64-elf-gcc.exe" %cpuflags% %asmflags% %linkerflags% -Wl,-T,linker/app.ld ^
  src/app/main.c ^
  %outflags% %libflags%

@echo off
if %errorlevel% EQU 1 (goto build_fail)

@REM LINKER COMMAND LINE
@echo on
"%bindir%aarch64-elf-objcopy.exe" output/app.elf -O binary output/app.img
"%bindir%aarch64-elf-objcopy.exe" output/app.elf -O binary output/echo.bin
@echo off
if %errorlevel% EQU 1 (goto build_fail)
echo BUILD COMPLETED NORMALLY
exit /b 0

:build_fail
echo ********** BUILD FAILURE **********
exit /b 1
