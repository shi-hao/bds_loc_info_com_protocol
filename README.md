# bds_loc_info_com_protocol
警用北斗全球卫星导航系统 第6部分：定位信息通信协议及数据格式

1.  说明：
  本项目为警用北斗全球卫星导航系统 第6部分：定位信息通信协议的一种实现方案，遵循BSD开源协议。

2. 编译方法
    >linux
    make
    生成可执行文件main。
    ./main
    执行可执行文件。

>win
    请将源码添加到相应的开发环境中编译。

3.  源码说明
使用C语言结构体定义各种通信指令，结构体成员均采用char类型数组避免结构体对齐产生的冗余字节的问题，使用get16，get32和put16，put32来转换大小端字节序。

bds_data_proc.c  bds_data_proc.h -->主要接口实现  
bds_DataStruct.h                 -->主要结构体定义  
main_test.c                      -->测试范例  
Makefile                         -->编译makefile
