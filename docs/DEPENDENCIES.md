# 固定依赖与对应源码

- FFmpeg 9.0.2 官方源码：<https://ffmpeg.org/releases/ffmpeg-9.0.2.tar.xz>
  SHA-256：`8c3850283eb25fa026482078a04051e0be17347b09ef81a0849bec15a96e002e`。
- GNU make 4.4.1 官方源码：<https://ftp.gnu.org/gnu/make/make-4.4.1.tar.gz>
  SHA-256：`dd16fb1d67bfab79a72f5e8390735c49e3e8e70b4945a15ab1f81ddb78658fb3`。
- VC-LTL 5.3.1、YY-Thunks 1.2.2 的固定 URL/hash 见 `cmake/legacy_windows.cmake`。

TAK 仅启用所需 demuxer/parser/decoder，禁用自动探测、网络、无关组件、GPL、nonfree。
构建脚本检查生成配置的许可证开关。SSE2、MSVC、XP CRT、WinAPI 声明及 YY-Thunks 的
构建参数均包含在对应源码包，用户可修改 LGPL 部分并重新编译/链接 DLL。

共享构建脚本含 RM、TwinVQ 的条件修正，但 TAK 不启用相应宏且不编译对应组件；
它们不会改变 TAK 解码。无 TAK 数学解码内核私有补丁。源码附件完整携带实际源文件，
不包含私有测试、运行样本、反编译证据或编译缓存。

运行包 `ttp_tak-版本.zip` 只有 `AddIn/ttp_tak.dll`、`SHA256SUMS.txt`；独立
`ttp_tak-版本-source.zip` 包含对应源码、完整许可证、GNU make 源码及构建步骤。
`build.ps1 -SourcePackage` 同时更新外部校验文件；Action 的 Release a Version 自动发布两个包。
