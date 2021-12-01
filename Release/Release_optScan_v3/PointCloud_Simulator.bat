@Echo off
ECHO ****Running optScan
CALL "%~dp0optScan.exe" --configure "samples\config_commands.txt"
ECHO ****Running mergeVertices
CALL %~dp0mergeVertices.exe -c "%~dp0data\config.txt"
ECHO ***DONE
PAUSE
CLS
EXIT