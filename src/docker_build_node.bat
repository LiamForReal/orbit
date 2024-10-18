@echo off

title Docker build node

REM --> Check for permissions
    IF "%PROCESSOR_ARCHITECTURE%" EQU "amd64" (
>nul 2>&1 "%SYSTEMROOT%\SysWOW64\cacls.exe" "%SYSTEMROOT%\SysWOW64\config\system"
) ELSE (
>nul 2>&1 "%SYSTEMROOT%\system32\cacls.exe" "%SYSTEMROOT%\system32\config\system"
)

REM --> If error flag set, we do not have admin.
if '%errorlevel%' NEQ '0' (
    echo Requesting administrative privileges...
    goto UACPrompt
) else ( goto gotAdmin )

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    set params= %*
    echo UAC.ShellExecute "cmd.exe", "/c ""%~s0"" %params:"=""%", "", "runas", 1 >> "%temp%\getadmin.vbs"

    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:gotAdmin
    pushd "%CD%"
    CD /D "%~dp0"

REM check if Docker already runs
tasklist /fi "ImageName eq com.docker.backend.exe" /fo csv 2>NUL | find /I "com.docker.backend.exe">NUL

if "%ERRORLEVEL%"=="0"  ( goto DockerRuns ) else ( goto DockerNotRuns )

:DockerRuns
    echo "Process / Application is running"
    goto skipDockerRun

:DockerNotRuns
    echo "Process / Application is not running"
    "C:\Program Files\Docker\Docker\Docker Desktop.exe"
    timeout /t 10 /nobreak 

:skipDockerRun
    docker build -t node .
    echo "Finished building node Docker successfully!"

pause