@echo off

del /q lib\win\*.a

for /F "tokens=*" %%F in (build\win\_src.txt) do (
    echo %%F
    %%F
)

for /F "tokens=*" %%F in (build\win\_static.txt) do (
    echo %%F
    %%F
)

del /q bin\win\*.o