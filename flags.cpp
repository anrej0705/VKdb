#include <sstream>

#include "cpp.h"

const string bitMessages[6] = { "������ �������\n", "��� �����������\n", "�������\n", "������ ���������\n" ,"���� ����\n" ,"�� �������\n" };
const string bitMessagesTable[6] = { "�������\n", "�������\n", "���\n", "����\n" ,"����\n" ,"����\n" };

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
void flags::displayFLAGS()const	//������������ � �������� � �������
{
	uint8_t bitMASK = 0x01;
	cout << endl;
	for (uint8_t a = 0; a < 3; ++a)
	{
		cout << "������: ";
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
void flags::readInputFLAGS()	//�������� ��������� ��� ��������� �������� ������
{
	bool bitUserInput;	//��������� ��� ����������������� �����

	cout << "������� ������ ����������(0 - ����������, 1 - �������): ";
	cin >> bitUserInput;
	bitUserInput == 1 ? statusRegister |= 0x01 : statusRegister = statusRegister;	//���� 1, ����� ��� � �������

	cout << "������� ������ ��������(0 - ���������, 1 - ��������): ";
	cin >> bitUserInput;
	bitUserInput == 1 ? statusRegister |= 0x02 : statusRegister = statusRegister;	//���� 1, ����� ��� � �������

	cout << "������� ������ ����(0 - ���� ���, 1 - ���): ";
	cin >> bitUserInput;
	bitUserInput == 1 ? statusRegister |= 0x04 : statusRegister = statusRegister;	//���� 1, ����� ��� � �������
}