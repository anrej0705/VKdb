#pragma once
//#pragma message("rulbits.h included")
#ifndef _RULBITS_H_
#define _RULBITS_H_

#include "cpp.h"

class rulBits	//��� ���������
{
	private:
		uint8_t convertString(string _inputArg);	//��������� ����������������� ����� ��� �������� �����
	protected:
		uint8_t rulBan;		//���� ��������� ��������� � ����
		uint8_t rul;		//���� ���������
		//bool dummy;			//���� ���������
	public:
		rulBits();
		rulBits(uint8_t r, uint8_t rBan) : rul(r), rulBan(rBan)
		{}
		void writeRulPr(uint8_t arg);
		void writeRulBan(uint8_t arg);
		uint8_t getRulBits();
		uint8_t getRulBanBits();
		void clearBits();
		void readInputRBIT();	//����� ��������� �� ������� ��������� �/��� ����
		void importRBIT(string _inputRuls, string _inputBans);
		void displayRBIT()const;	//������ ������� � ���������� ��� �������� � �� ��� ��� � �����
		void getRBIT();
};

#endif