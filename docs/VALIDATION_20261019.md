# TAK 恢复与验证，2026-10-19

## 原版基线

原 `AddIn/ttp_tak.dll` SHA-256：
`eed31287e60cb64172b0615f52f2cd0569d36acaea3c9dd32ce404cc53ee432e`。
原 wrapper 的 `100015D7` 对 buffer 取容量但未用于限制请求长度；读取后直接返回数据，
解码状态 0/3 按成功/结束处理。新实现保留 PCM 数据及 Reader ABI，并补容量、样本对齐和错误检查。

原版依赖 `tak_SSD_Create_FromStream`、`ReadAudio`、`Seek` 等专有 SDK 入口。
重建使用 FFmpeg 9.0.2 开源内核；这属于同格式兼容实现，不宣称还原了 TAK 专有算法源码。

## 已验证

- Takc 2.3.3 生成 8/16/24-bit × mono/stereo × p0/p4 共 12 个样本，每个含 48,017 帧，
  专门含低位随机信息；原插件与新插件各解码一次，**24/24 输出与源 PCM 完全相同**。
- 12 个契约测试全部通过：7 字节碎读、4097 字节非整除缓冲、零/不足一帧容量、
  多次前向和反向定位与原 PCM 逐字节对比、超过结尾定位、Stop/Start、APEv2 Title 读取。
- 原版推荐缓冲约四分之一秒；新实现保持这一推荐值，同时支持更小的合法缓冲。
- Release x86 导入审计通过：4 个系统 DLL，145 个导入，无 VC 2012 或专有 TAK DLL。
- XP 与 Win7 的原生 TAK 契约测试已通过；24-bit stereo 样本完成短读、容量、定位和取消检查。
  15 秒 24-bit stereo 样本在原版、重建版宿主的 Win11 / XP / Win7 六组 GUI 验证均通过，播放时间大于 0。

测试证据只在 `rebuild/tests/audio_plugins_fix_20261019/tak`，不加入仓库或 Action。

## 仍需区分

- 首次定位未知区间为解码前进；已建立索引范围可直接跳转。尚未恢复专有 SDK 的完整预建索引性能。
- 未把短样本通过等同于历史所有 TAK 版本、所有压缩模式或全部多声道变体通过。
- 宿主 APEv2 写回/封面转发需要实际播放器测试；本轮无宿主探针只验证只读标签路径。
- FFmpeg 限制不自动转为专有 SDK 回退；未支持格式明确失败。
- 上述 GUI 样本以外的完整功能、旧系统全部 12 样本矩阵及 HTTP 非寻址输入仍需分别测试。

## 固定依赖

官方 https://ffmpeg.org/releases/ffmpeg-9.0.2.tar.xz
SHA-256 `8c3850283eb25fa026482078a04051e0be17347b09ef81a0849bec15a96e002e`。
GNU make 4.4.1 官方源码 SHA-256 `dd16fb1d67bfab79a72f5e8390735c49e3e8e70b4945a15ab1f81ddb78658fb3`。
禁用网络、自动依赖探测和无关 demuxer/decoder，只保留 TAK 所需组件。

## 独立对应源码附件

运行 ZIP 仍只包含 DLL / SHA256SUMS；Release 另附 `ttp_tak-版本-source.zip`。
源码包包含完整固定 FFmpeg 源码、适配修改、GNU make 官方源码和完整许可证；
不含本项目私有测试、编译缓存、DLL/EXE/PDB，Git 不提交上游源码。
FFmpeg 自身的 `tests/Makefile` 是上游构建入口的依赖，随完整对应源码保留，Action 不执行测试。

已将源码附件解压到私有独立目录，从包内 FFmpeg / make 源码重新配置、完整编译，
生成 DLL 导入审计通过；解码/短读/定位契约测试输出 288102 字节 24-bit stereo PCM，PASS。
此验证不重新链接已冻结的发行 DLL。
