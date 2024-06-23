@echo off
echo 正在清理各种缓存垃圾文件，请稍等......
@echo off 
echo 清空IE临时文件目录...
del /f /s /q "%userprofile%\AppData\Local\Microsoft\Windows\Temporary Internet Files\*.*"
cls
echo.
echo 清理IE缓存完毕
echo.
del /f /s /q "%temp%\*.*"
del "c:\Windows\Server.dll"
rem clean dns cache
ipconfig /flushdns
echo ~~~------~~~~~
echo 已经清空DNS缓存
echo.
echo.等待5秒后自动关闭
TIMEOUT /T 1