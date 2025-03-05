#pragma once

#include <string>
#include <vector>

typedef struct
{
	std::string userId;
	long distance;
} UserInfo_t;

/**
* \brief ��ʼ������ȡ�����ļ�
* \return �ɹ��򷵻�0������ʧ��
*/
int initialize();

/**
* \brief ��ȡ��Χ�������û�����ǰê��ľ�����Ϣ
* \param userInfo:  �û���ʶ&����
* \return �ɹ��򷵻�0������ʧ��
*/
int getMasterInstance(std::vector<UserInfo_t>& userInfo);

/**
* \brief ��ȡ��Χ�������û���ָ��ê��ľ�����Ϣ
* \param anchorName:  ָ����ê���ʶ��Ӧ��config.ini��һ��
* \param userInfo:  �û���ʶ&����
* \return �ɹ��򷵻�0������ʧ��
*/
int getSlaveInstance(std::string anchorName, std::vector<UserInfo_t>& userInfo);

/**
* \brief ��բ�ţ�������Ϣд�����ļ�
* \param userId:  �û���ʶ��Ӧ��coordinate.csv��һ��
* \return �ɹ��򷵻�0������ʧ��
*/
int openGate(std::string userId);

/**
* \brief ���ɼƷ�ƾ֤
* \param userId:  �û���ʶ��Ӧ��coordinate.csv��һ��
* \return �ɹ��򷵻�0������ʧ��
*/
int genPurchaseVoucher(std::string userId);