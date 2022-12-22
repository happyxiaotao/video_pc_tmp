::自动生成安装包的脚本


::表示不回显执行的命令
@echo off
:: 编码换成UTF-8，避免中文乱码
chcp 65001

::设置变量，使用时需要用一对%包起来
::set filename=%date:~0,4%%date:~5,2%%date:~8,2%%time:~0,2%%time:~3,2%%time:~6,2%
::format_time格式：YYYYMMDDHHmmSS
set format_time=%date:~3,4%%date:~8,2%%date:~11,2%%time:~0,2%%time:~3,2%%time:~6,2%
set "format_time=%format_time: =0%"
echo %format_time%

::安装包所在基础目录
set base_dir=E:\apk
:: 编译好的aged.exe程序所在目录
set release_exe_path=E:\code\qt\build-video_pc_tmp-Desktop_Qt_5_15_2_MSVC2019_64bit-Release\release\video_pc_tmp.exe
:: ffmpeg和ssl依赖库目录
set sdk_dll_dir=E:\code\github\video_pc_tmp\run_dll
:: 资源目录所在目录
set resource_dir=E:\code\github\video_pc_tmp\resource
:: 配置文件所在目录
set config_dir=E:\code\github\video_pc_tmp\conf
::windeployqt程序路径
set windeployqt_exe_path=D:\Qt\5.15.2\msvc2019_64\bin\windeployqt.exe
::binarycreator程序路径
set binarycreator_exe_path=D:\Qt\Tools\QtInstallerFramework\4.4\bin\binarycreator.exe
::参考配置文件所在目录（需要拷贝config.xml、installscript.qs和package.xml）
set install_reference_dir=E:\code\github\video_pc_tmp\build_install

:: 创建基础目录
md %base_dir%
cd %base_dir%

:: 创建安装包目录
set install_dir=%base_dir%\video_pc_%format_time%
md %install_dir%
cd %install_dir%

:: 拷贝参考目录
xcopy %install_reference_dir% %install_dir% /e
::删除此文件
del build_install.bat

:: 下面是填充data目录数据
set data_dir=%install_dir%\packages\org.tuguanproject.video_pc\data

echo "-----------------------------------------------"
echo %data_dir%
echo %release_exe_path%

cd %data_dir%
::拷贝aged.exe
copy %release_exe_path% video_pc.exe
::调用D:\Qt\5.15.2\msvc2019\bin\windeployqt.exe自动补充qt相关依赖
%windeployqt_exe_path% video_pc.exe
:: 拷贝穿透科技依赖库
copy %sdk_dll_dir%\avcodec-59.dll .
copy %sdk_dll_dir%\avutil-57.dll .
copy %sdk_dll_dir%\libcrypto-1_1-x64.dll .
copy %sdk_dll_dir%\libssl-1_1-x64.dll .
copy %sdk_dll_dir%\swresample-4.dll .
copy %sdk_dll_dir%\swscale-6.dll .
::拷贝资源目录
md resource
xcopy %resource_dir% resource /e
::拷贝配置文件
md conf
xcopy %config_dir% conf /e

:: 调用binarycreator.exe生成安装包
cd %install_dir%
%binarycreator_exe_path% -c config/config.xml -p packages video_pc_install.exe -v