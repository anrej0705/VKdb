#pragma once
//#pragma message("vkmeta.h included")
#ifndef _VKMETA_H_
#define _VKMETA_H_

#include "cpp.h"

#define VKID_length		10		//�������� ����� ���� ID � ��
#define VKURL_length	32		//�������� ����� ���� � ��
#define VKUINFO_length	40		//�������� ����� �����/������� � ��

class vkMeta//��������� ������ �� ��
{
	private:
		bool checkNcopy(char *inputArg, char *destination); //��������� ����� � ������ ���� ������ �����
	protected:
		char vkId[VKID_length];	//�� ������������
		char vkUrl[VKURL_length];	//��� ������������
		char vkName[VKUINFO_length];	//��� ������������
		char vkFam[VKUINFO_length];	//������� ������������
		vkMeta();
		vkMeta(char vId[VKID_length], char vUrl[VKURL_length], char vNam[VKUINFO_length], char vFam[VKUINFO_length]);
		void writeId(char arg[VKID_length]);
		void writeUrl(char arg[VKURL_length]);
		void writeName(char arg[VKUINFO_length]);
		void writeFam(char arg[VKUINFO_length]);
		void readInputVKMETA();
		void displayVKMETA()const;
	public:
		void getVKIDchar();
		void getVKurl();
};

#endif