MultiGateSolutionByDistance.lib为给定的静态库
MultiGateSolutionByDistance.h为函数的头文件
config.ini为锚点坐标的配置信息
coordinate.csv为用户位置信息的配置，时间为调用初始化函数后经过的时间


使用流程：
首先调用initialize()方法初始化配置信息，
然后调用getMasterInstance()及getSlaveInstance()获取当前时间的用户位置信息，
经过算法处理后，调用openGate()打开闸机，
当用户进入计费区域时，调用genPurchaseVoucher()生成计费凭证，
开门信息&计费凭证会写入当前路径下的result.txt文件中