#pragma once
//#pragma message("rem.h included")
#ifndef _REM_H_
#define _REM_H_

#include "cpp.h"

#define REM_length		80		//�������� ����� ���� ���������� � ������

class rem	//������� � �������
{
	protected:
		char dbRem[REM_length];	//���������������� ����� � ���� � ������
	public:
		rem();
		rem(char remark[REM_length]);
		void writeRemark(char arg[REM_length]);
		void readInputREM();
		void displayREM()const;
		void getREM();
};

#endif