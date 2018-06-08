[顶层目录结构]
- Document 各种文档
- IDE 工程使用的开发环境
- Libraries 各种官方提供的代码或连接库
- Modules 不依赖硬件的功能模块
- Devices 不依赖MCU的外部芯片的代码，如外部EEPROM，AD/DA等
- Platform 依赖MCU和PCB的代码部分
- Application 上层应用
- preinc.h 所有重要的全局的预处理和宏定义

[基本原则和考虑]
- Document 里面只保存和工程相关的文档
- IDE 可以根据具体IDE修改子目录名称，如IAR，CodeWarrior等
- Libraries 内的官方库没有特殊情况，不得修改，应设为只读
- preinc.h 包含的预处理和宏定义必须在开发环境中加入引用

