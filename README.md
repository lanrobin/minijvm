# minijvm
This is a mini jvm

# Build
## 在Windows+Visual Studio 2019, 使用的话参照 https://blog.csdn.net/shulianghan/article/details/102831727 导入为CMake项目即可。
## Ubuntu 20.04 tested, (cmake 3.10 required), cmake and then make.
## c++ compiler with std17 required.

# 目标
## 能跑一个HelloWorld.java文件。

# Used third party source code
## https://github.com/kuba--/zip for jmob, jar file read.
## https://github.com/Tieske/pthreads-win32/tree/master/Pre-built.2 for Windows pthread
## https://github.com/gabime/spdlog for logging.
