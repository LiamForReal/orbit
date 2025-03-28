@echo off
:: Generate a random number between 1 and 1000000
set /a RAND_NUM=%RANDOM% * 1000 + %RANDOM%

:: Print the random number (optional)
echo Random ID: %RAND_NUM%

cd ..\orbit_gui
:: Run GUI script with the random number as an argument
start "" python "C:\Users\Magshimim\orbit\orbit_gui\gui.py" %RAND_NUM%

cd ..
timeout /t 2

:: Run Client.exe with the random number as an argument
start "" "C:\Users\Magshimim\orbit\src\Client\x64\Release\client.exe" %RAND_NUM%

exit