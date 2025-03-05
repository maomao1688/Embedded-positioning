#pragma once

#include <string>
#include <vector>

typedef struct
{
	std::string userId;
	long distance;
} UserInfo_t;

/**
* \brief 初始化，读取配置文件
* \return 成功则返回0，否则失败
*/
int initialize();

/**
* \brief 获取范围内所有用户到当前锚点的距离信息
* \param userInfo:  用户标识&距离
* \return 成功则返回0，否则失败
*/
int getMasterInstance(std::vector<UserInfo_t>& userInfo);

/**
* \brief 获取范围内所有用户到指定锚点的距离信息
* \param anchorName:  指定的锚点标识，应与config.ini中一致
* \param userInfo:  用户标识&距离
* \return 成功则返回0，否则失败
*/
int getSlaveInstance(std::string anchorName, std::vector<UserInfo_t>& userInfo);

/**
* \brief 打开闸门，开门信息写入结果文件
* \param userId:  用户标识，应与coordinate.csv中一致
* \return 成功则返回0，否则失败
*/
int openGate(std::string userId);

/**
* \brief 生成计费凭证
* \param userId:  用户标识，应与coordinate.csv中一致
* \return 成功则返回0，否则失败
*/
int genPurchaseVoucher(std::string userId);