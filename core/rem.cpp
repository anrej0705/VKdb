#include "cpp.h"

rem::rem()
{
	memset(dbRem, NULL, sizeof(dbRem));
}
rem::rem(char remark[REM_length])
{
	arrcpy(dbRem, remark);
}
void rem::writeRemark(char arg[REM_length])
{
	arrcpy(dbRem, arg);
}
void rem::readInputREM()
{
	cout << "������� �������: ";
	cin.ignore();
	cin.getline(dbRem, REM_length);
	//cout << "������� " << dbRem << endl;
}
void rem::displayREM()const
{
	cout << "������� � ������: " << dbRem << endl;
}
void rem::getREM()
{
	string _temp = dbRem;
	std::copy(_temp.begin(), _temp.end(), dbO.remark);
}