#include <string>
#include <sstream>
#include <iomanip>
#include "cpp.h"
#include "stdint.h"

using namespace std;

dbRecord::dbRecord()
{
	recordId = 0;
}
void dbRecord::displayDBREC()const
{
	cout << "�� ������: " << to_string(recordId) << endl;
}
void dbRecord::readInputDBREC()
{
	cout << "������� ��: ";
	cin >> recordId;
}
uint16_t dbRecord::getRecordId()
{
	return recordId;
}
void dbRecord::setRecID(uint16_t arg)
{
	recordId = arg;
	if (debugMode)
		cout << "�������� ����� ������: " << recordId << endl;
}
void dbRecord::getRecID()
{
	stringstream temp2;
	temp2 << setw(4) << setfill('0') << to_string(recordId) << endl;
	string temp = temp2.str();
	for (uint8_t a = 0;a < 4;++a)
		dbO.recId[a] = temp[a];
}