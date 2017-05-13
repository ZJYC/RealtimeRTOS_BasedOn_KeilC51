@echo off
color 17             
echo 正在打开各种文档、、、、、、               

Start "" "user\APP_Cfg.h"
Start "" "ZOS-I\source\inc\OS.h"
Start "" "ZOS-I\source\inc\OS_CFG.h"
Start "" "ZOS-I\source\CPU\OS_CPU.h"

Start "" "user\APP.c"
Start "" "ZOS-I\source\src\Hook.c"
Start "" "ZOS-I\source\src\Int.c"
Start "" "ZOS-I\source\src\OS.c"

Start "" "ZOS-I\source\CPU\OS_ASM.a51"
Start "" "user\记录.txt"

echo 完成
goto :eof
