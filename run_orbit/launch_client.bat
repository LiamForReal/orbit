@echo off
:: Get current date and time in your format
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set datetime=%%I
set timestamp=%datetime:~6,2%-%datetime:~4,2%-%datetime:~0,4%_%datetime:~8,2%-%datetime:~10,2%

:: Generate a random number between 1 and 1000000
set /a RAND_NUM=%RANDOM% * 1000 + %RANDOM%

:: Ensure logs directory exists
if not exist logs mkdir logs

:: Print the random number (optional)
echo Random ID: %RAND_NUM%

cd ..\orbit_gui

:: Run GUI script without logging
start "" python "C:\Users\Magshimim\orbit\orbit_gui\gui.py" %RAND_NUM%

cd ..\run_orbit
timeout /t 2

:: Ensure the log file exists before running the Client.exe
set logFile=logs\client_log_%timestamp%.txt

echo Log file created at: %logFile%

:: Run Client.exe with PowerShell redirection to capture console output
powershell -Command "Start-Process 'C:\Users\Magshimim\orbit\src\Client\x64\Release\client.exe' -ArgumentList '%RAND_NUM%' -NoNewWindow -RedirectStandardOutput '%logFile%' "

:: Ensure the log file was written
echo Log saved as %logFile%

exit