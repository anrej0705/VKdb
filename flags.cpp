#include <sstream>

#include "cpp.h"

const string bitMessages[6] = { "Запись активна\n", "Под наблюдением\n", "Забанен\n", "Запись неактивна\n" ,"Срок истёк\n" ,"Не забанен\n" };
const string bitMessagesTable[6] = { "Активен\n", "Активен\n", "Бан\n", "Истёк\n" ,"Истёк\n" ,"Истёк\n" };

flags::flags()
{
	statusRegister = 0;
}
inline void flags::writeActiveBit(bool arg)
{
	arg == 1 ? statusRegister |= 0x01 : statusRegister = statusRegister;
}
inline void flags::writeBanBit(bool arg)
{
	arg == 1 ? statusRegister |= 0x02 : statusRegister = statusRegister;
}
inline void flags::writeTimeoutBit(bool arg)
{
	arg == 1 ? statusRegister |= 0x04 : statusRegister = statusRegister;
}
void flags::displayFLAGS()const	//Отчитываемся о статусах в консоль
{
	uint8_t bitMASK = 0x01;
	cout << endl;
	for (uint8_t a = 0; a < 3; ++a)
	{
		cout << "Статус: ";
		statusRegister&(bitMASK << a) ? cout << bitMessages[a] : cout << bitMessages[a + 3];
	}
	cout << endl;

}
void flags::loadFlags(uint8_t arg)
{
	statusRegister = arg;
}
void flags::getFlags()
{
	stringstream _temp;
	string _tempO;
	uint8_t bitMASK = 0x01;
	//cout << endl;
	for (uint8_t a = 0; a < 3; ++a)
	{
		//statusRegister&(bitMASK << a) ? _temp << bitMessagesTable[a] : _temp << bitMessagesTable[a + 3];
		if (statusRegister&(bitMASK << a))
		{
			_temp << bitMessagesTable[a];
			break;
		}
		if (!statusRegister&(bitMASK << a))
		{
			_temp << bitMessagesTable[a + 3];
			break;
		}
	}
	_temp << endl;
	_tempO = _temp.str();
	std::copy(_tempO.begin(), _tempO.end(), dbO.status);
}
void flags::readInputFLAGS()	//Опросник оператора для получения статусов записи
{
	bool bitUserInput;	//контейнер для пользовательского ввода

	cout << "Введите статус активности(0 - неактивнен, 1 - активен): ";
	cin >> bitUserInput;
	bitUserInput == 1 ? statusRegister |= 0x01 : statusRegister = statusRegister;	//если 1, пишем бит в регистр

	cout << "Введите статус таймаута(0 - просрочен, 1 - актуален): ";
	cin >> bitUserInput;
	bitUserInput == 1 ? statusRegister |= 0x02 : statusRegister = statusRegister;	//если 1, пишем бит в регистр

	cout << "Введите статус бана(0 - бана нет, 1 - бан): ";
	cin >> bitUserInput;
	bitUserInput == 1 ? statusRegister |= 0x04 : statusRegister = statusRegister;	//если 1, пишем бит в регистр
}