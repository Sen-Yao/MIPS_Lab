MIPS_CPU 为 Vivado 对应的项目文件夹，其中的 xpr 文件就是 Vivado 的项目文件

代码为 Verilog 格式，存储在 "MIPS_CPU/MIPS_CPU.srcs/sources_1/new" 路径下

注意，如果直接将整个项目导入 Vivado，会出现文件名为红色的情况。这是因为 Vivado 找不到对应的 v 文件造成的。

解决方法就是在红色的文件名上右键，然后 Replace File，找到本项目文件夹所在的位置，并且在 "MIPS_CPU/MIPS_CPU.srcs/sources_1/new" 路径下双击对应名字的 v 文件即可。