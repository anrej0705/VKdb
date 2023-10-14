#pragma once
#ifndef _BASELOCALE_H_
#define _BASELOCALE_H_

//���� ������� ������, ������������� � ������ ����������� ������������ ����� ������
struct baseLocale
{
	char sign[15] = "������� ������";
	char appVer[18] = "������ ����������";
	char inDevSign[13] = "� ����������";
	char dbSummary[7] = "������";
	char dbName[9] = "��: NULL";
	char dbRecs[14] = "�������: NULL";
	char dbBans[12] = "�����: NULL";
	char dbActRuls[19] = "�������� ������: 0";
	char mainScrMenu[5] = "����";
	char mainScrNewRec[13] = "����� ������";
	char mainScrEdRec[11] = "���.������";
	char dbTabNo[3] = "No";
	char dbTabVKID[6] = "VK ��";
	char dbTabVKURL[15] = "VK ���(VK URL)";
	char addDate[9] = "��������";
	char dbViols[6] = "�����";
	char dbExpires[9] = "��������";
	char dbRecStatus[7] = "������";
	char dbRecRem[17] = "������� � ������";
	char mainScrConsole[8] = "�������";
	char loadedRecs[19] = "����������� ������";
	char emptySign[6] = "�����";
	char infoMsg[7] = "[����]";
	char warnMsg[7] = "[����]";
	char fatalMsg[7] = "[����]";
};
#endif