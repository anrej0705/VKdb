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
	cout << "Введите пометку: ";
	cin.ignore();
	cin.getline(dbRem, REM_length);
	//cout << "Введено " << dbRem << endl;
}
void rem::displayREM()const
{
	cout << "Пометка к записи: " << dbRem << endl;
}
void rem::getREM()
{
	string _temp = dbRem;
	std::copy(_temp.begin(), _temp.end(), dbO.remark);
}