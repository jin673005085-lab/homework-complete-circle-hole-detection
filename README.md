# 作业-完整圆孔检测

使用 C++ 和 OpenCV 在单张图片中检测圆孔。程序输出候选圆的圆心坐标、半径和总数，并把结果画在图片上。红线描出圆周，绿点标记圆心。

## 编译

需要支持 C++17 的编译器、CMake 3.15 或更高版本，以及含 C++ 开发文件的 OpenCV。在项目根目录打开 PowerShell，运行：

```powershell
cmake -S . -B build
cmake --build build --config Release
```

使用 Ninja 或 Makefiles 时，Windows 可执行文件通常位于 `build/circle_holes.exe`。使用 Visual Studio 生成器时，将下面命令里的 `build\circle_holes.exe` 换成 `build\Release\circle_holes.exe`。

如果 CMake 找不到 OpenCV，重新配置时加上 `-DOpenCV_DIR="包含 OpenCVConfig.cmake 的目录"`。如果运行时提示缺少 DLL，将 OpenCV 的 DLL 所在目录加入 `PATH`。

## 运行

```powershell
# 使用仓库里的示例图
.\build\circle_holes.exe .\1111.png 20 60

# 使用自己的图片，路径含中文或空格时请加引号
.\build\circle_holes.exe "D:\图片\零件.png"

# 运行内置自检
.\build\circle_holes.exe --self-test
```

示例命令末尾的 `20 60` 是最小和最大半径，单位为像素。两个参数需要一起传入，且满足 `0 <= 最小半径 < 最大半径`。示例图使用这个范围来排除左下角约 96 像素半径的误检。换图片后，可按目标圆孔的大小调整范围。

检测结果保存在运行命令的当前目录，文件名为 `circle_result.png`。再次运行会覆盖之前的结果图。圆心坐标以图像左上角为原点，向右为 X 正方向，向下为 Y 正方向。

## 检测方法与限制

程序先对图像做灰度转换和高斯模糊，再用霍夫圆变换寻找候选圆，并排除估计圆周接触或超出图像边界的候选。轮廓不完整或物体边缘形状相似时，仍可能出现误检。半径范围已知时，建议在命令中指定。
