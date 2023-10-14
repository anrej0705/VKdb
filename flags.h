#pragma once
//#pragma message("flags.h included")
#ifndef _FLAGS_H_
#define _FLAGS_H_

#include "cpp.h"

class flags//���� ��������� - ����, ������ � �.�
{
	protected:
		uint8_t statusRegister; //������� ���-��������
		flags();
		inline void writeActiveBit(bool arg);
		inline void writeBanBit(bool arg);
		inline void writeTimeoutBit(bool arg);
		void readInputFLAGS();
		void displayFLAGS()const;	//������������ � �������� � �������
	public:
		void loadFlags(uint8_t arg);
		void getFlags();
};

#endif