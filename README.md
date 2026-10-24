# ttp_tak

最新补全与验收：[2026-10-24](docs/COMPLETION_20261024.md)。

原版 TTPlayer TAK Reader ABI 的独立源码实现。解码采用固定 FFmpeg 9.0.2 最小
TAK demuxer/parser/decoder，不加载旧 `tak_deco_lib.dll`。

```powershell
./build.ps1 -Package -SourcePackage -PackageVersion 2026.10.19
```

需要 MSVC x86、CMake、Python、PowerShell、Git for Windows。GNU make 4.4.1 从官方源码
在私有构建目录自举；所有依赖在构建时下载并校验 SHA-256。VC-LTL 5.3.1、YY-Thunks 1.2.2
用于统一 XP / Win7 / 现代 Windows 构建。Action 默认仅构建打包，发布必须手动选择
Release a Version；不执行或上传私有测试。

实现原 Reader 工厂、缓冲与定位、标准 IStream 和宿主扩展异步读/取消回调。输出保留
8/16/24-bit 整数 PCM 精度，24-bit 不经 float 中转。读取严格限制于宿主提供的容量，
CRC 或截断错误不作为正常 EOF。播放过程中建立帧位置索引，已读取区间的重复定位使用
直接字节定位；首次跳往尚未读过的位置仍需解码前进。

标签可在缺少宿主服务时只读解析。原版或重建版提供 `CreateStdContent` 时委托其 APEv2
读写及封面接口；不在本插件另写一套标签保存。旧宿主 Release 写盘的返回值限制仍存在。

源码包装层为 MIT；FFmpeg 为 [LGPL 2.1+](LICENSES/FFmpeg-LGPL-2.1.txt)，静态链接分发
必须连同可重建/重新链接的完整对应源码与构建方式履行 LGPL 条件；许可证保存在仓库。
`-SourcePackage` 生成独立的 `ttp_tak-版本-source.zip`，包含包装器、完整 FFmpeg 源码、
GNU make 源码及构建规则。Action 发布自动携带此附件；运行 ZIP 仍仅有 DLL 和 SHA256SUMS。
源码包解压后使用同一命令，CMake 自动使用 `third_party/ffmpeg`；更改其源码会触发重新编译，
不需要专有 TAK SDK。完整源码不提交到 Git，默认构建时仍下载固定版本。
具体构建版本、固定 URL/hash 和配置在 `cmake/build_ffmpeg.py`，不嵌入许可证资源。

详见 [验证与剩余差异](docs/VALIDATION_20261019.md)。

## 可选格式识别接口

2026-10-20：Reader 工厂增加 Probe，Reader 增加 SourceInfo；旧导出和接口槽位保持不变。声明与辅助实现位于 `include/ttpcomm/audio_format_probe.h` 和 `audio_format_probe_impl.h`，不增加运行时 DLL 依赖。协议原件在相邻 `ttpcomm/sdk/AUDIO_FORMAT_PROBE.md`；新建插件指南在相邻 `rebuild/docs/AUDIO_PLUGIN_DEVELOPMENT_GUIDE.md`。原版播放器继续使用旧接口；错误后缀候选选择和更名提示由支持新协议的重建播放器执行。
