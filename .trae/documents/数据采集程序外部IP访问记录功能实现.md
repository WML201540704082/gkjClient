# dataAcquisition程序修改计划

## 1. 需求分析
- 对数据进行筛选，只选取访问外部的数据
- 生成使用记录，记录对某IP访问的开始和结束时间
- 从第一次采集开始，记录目的IP首次出现的时间为访问开始时间
- 下次采集时，与上次采集的五元组数据进行对比：
  - 新增数据：记录访问开始时间
  - 重复数据：跳过
  - 上次存在本次不存在的数据：将本次采集时间记为访问结束时间
- 每次采集结束后立即发送当前五元组数据完整且访问开始时间和访问结束时间不为空的数据

## 2. 现有代码分析
- `networkmonitor.h/networkmonitor.cpp`：获取TCP和UDP连接的五元组信息
- `sendnetworkinfowork.h/sendnetworkinfowork.cpp`：采集网络信息、加密并发送到服务端
- 网络信息发送频率：每5分钟一次（300000毫秒）

## 3. 修改方案

### 3.1 添加外部IP筛选功能
- 在`networkmonitor.cpp`中添加外部IP筛选逻辑
- 内部IP范围：
  - 10.0.0.0 - 10.255.255.255
  - 172.16.0.0 - 172.31.255.255
  - 192.168.0.0 - 192.168.255.255
  - 127.0.0.0 - 127.255.255.255（回环地址）

### 3.2 添加访问记录结构体
- 在`sendnetworkinfowork.h`中添加`AccessRecord`结构体，包含：
  - 五元组信息（protocol, localAddr, localPort, remoteAddr, remotePort）
  - 访问开始时间（startTime）
  - 访问结束时间（endTime）

### 3.3 添加历史记录管理
- 在`sendNetworkInfoWork`类中添加成员变量：
  - `QList<AccessRecord> m_lastAccessRecords`：存储上次采集的访问记录

### 3.4 修改数据采集和处理逻辑
- 在`sendNetworkInfoWork::startSending`方法中：
  1. 获取当前时间作为本次采集时间
  2. 采集网络五元组数据
  3. 筛选出外部IP的五元组数据
  4. 与上次采集的记录对比：
     - 新增记录：添加到新记录列表，设置开始时间
     - 重复记录：跳过
     - 上次存在本次不存在的记录：设置结束时间，添加到待发送列表
  5. 更新历史记录列表
  6. 发送所有访问开始时间和结束时间不为空的数据

### 3.5 修改发送数据格式
- 调整JSON数据格式，包含访问开始时间和结束时间
- 确保只发送完整的访问记录（开始时间和结束时间都不为空）

## 4. 修改文件清单
1. `c:\Users\wzr\Desktop\gkjClient\dataAcquisition\model\networkmonitor.cpp`：添加外部IP筛选功能
2. `c:\Users\wzr\Desktop\gkjClient\dataAcquisition\model\http\sendnetworkinfowork.h`：添加AccessRecord结构体和历史记录成员变量
3. `c:\Users\wzr\Desktop\gkjClient\dataAcquisition\model\http\sendnetworkinfowork.cpp`：修改数据采集、处理和发送逻辑

## 5. 实现细节
- 使用`QDateTime`记录访问开始和结束时间
- 使用`QSet`或`QMap`优化对比效率
- 确保线程安全
- 保持与现有代码风格一致
- 添加必要的日志记录