#pragma once
//#pragma message("dtime.h included")
#ifndef _DTIME_H_
#define _DTIME_H_

#include "cpp.h"

#define DT_length		6		//�������� ����� ���� ����/������� � �������� �������

class dTime	//��������� ������� �������� � ����, ����, ��������� �����
{
	private:
		string decodeDTime(char inputArg[DT_length+1]);	//����������� ������ ����-�������
		bool processInput(char *out, string inputArg);	//��������� ����� ������������ � ��������� � �������� ������
		bool checkForm(string inputArg);	//������ ����� ��� �������� ������������, ���� ���-�� �� ��� ������ ����������
	protected:
		char dtsta[DT_length];	//��������� ������� �������� � ����
		char dtstp[DT_length];	//��������� ������� ����
		char dtban[DT_length];	//��������� ������� ��������� �����
		void writeDtStart(char arg[DT_length + 1]);
		void writeDtStop(char arg[DT_length + 1]);
		void writeDtBan(char arg[DT_length + 1]);
	public:
		dTime();
		dTime(char dStart[DT_length], char dStop[DT_length], char dBan[DT_length]);
		void dTimeImportDates(string timeStart, string timeEnd, string timeBan);
		string getADDTIME();
		void readInputDTIME();
		void displayDTIME();	// ������������� �����, ������� ������������ ������ � ��������-�������� ������
		void getAddDate();
		void getExpDate();
};

#endif