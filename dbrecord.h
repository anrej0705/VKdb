#pragma once
//#pragma message ("dbRecord included")
#ifndef _DBRECORD_H_
#define _DBRECORD_H_

#include "cpp.h"

class dbRecord	//������� �����, �������� ������������� ������
{
	protected:
		uint16_t recordId;	//������������� ������
		dbRecord();
		dbRecord(uint16_t rId) : recordId(rId)
		{}
		void displayDBREC()const;
		void readInputDBREC();
	public:
		uint16_t getRecordId();
		void setRecID(uint16_t arg);
		void getRecID();
};

#endif