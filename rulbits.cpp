#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "cpp.h"

rulBits::rulBits()
{
	rul = 0;
	rulBan = 0;
}
void rulBits::writeRulPr(uint8_t arg)
{
	rul = arg;
}
void rulBits::writeRulBan(uint8_t arg)
{
	rulBan = arg;
}
uint8_t rulBits::getRulBits()
{
	return rul;
}
uint8_t rulBits::getRulBanBits()
{
	return rulBan;
}
void rulBits::clearBits()
{
	rul = 0x00;
	rulBan = 0x00;
}
void rulBits::importRBIT(string _inputRuls, string _inputBans)
{
	rul = convertString(_inputRuls);
	rulBan = convertString(_inputBans);
}
uint8_t rulBits::convertString(string _inputArg)	//��������� ����������������� ����� ��� �������� �����
{
	string argBuf;	//��������� �����
	uint8_t stringLength = 17;	//����������� ����� ������, �� ����� ���� ������ 15(8 ����. + 7 �������)
	uint8_t _commas = 0;	//���������� �������
	uint8_t registerBit = 0;	//��������� ������������ �����

	//cout << "��������: " << _inputArg << endl;

	//���� ���� ������, �� �������
	if (_inputArg[0] == NULL)
		return 0;

	//��������� ������������ �����, ����� �����������
	for (uint8_t a = 1;a < 17;a += 2)
	{
		if (_inputArg[a] == NULL)
			break;
		if (_inputArg[a] != ',')
			throw errorReport(0x06);
	}

	//������� ������������ ���-�� ������� � ����� ��������
	for (uint8_t a = 0;a < stringLength;++a)
	{
		if (_inputArg[a] == NULL)
		{
			//cout << "�����: " << to_string(a) << "\n�������: " << to_string(_commas) << "\n�������� �����: " << to_string(a - _commas) << endl;
			_commas = a - _commas;
			stringLength = a;
		}
		if (_inputArg[a] == ',')
			++_commas;
	}

	//������ ������ �� �������
	_inputArg.erase(std::remove(_inputArg.begin(), _inputArg.end(), ','));
	//cout << "�������: " << inputArg << "\n�����: " << to_string(_commas) << endl;

	//����������� � ������ � ����
	for (uint8_t a = 0;a < _commas;++a)
	{
		argBuf = _inputArg[a];
		//cout << endl;
		registerBit |= (0x01 << (atoi(argBuf.c_str()) - 1));
	}
	//cout << "������� ������ HEX=";
	//printf("0x%02x", registerBit);
	//cout << endl;

	//������� ���� ��� ����
	return registerBit;
}
void rulBits::readInputRBIT()	//����� ��������� �� ������� ��������� �/��� ����
{
	char userInput[17]; //����� ��� ������, �� ����� ���� ������ 15(8 ����. + 7 �������)

	//�������� ����������, � ������������ 2 �������
INPUT_RUL:
	try
	{
		cout << "������� ��������� �� �������(�������� 1,3,4,7), �� ������ 8: ";
		cin >> userInput;
		rul = convertString(userInput);
	}
	catch (errorReport)
	{
		cout << "������������ ������ �����, ��������� �������" << endl;
		__asm
		{
			JMP INPUT_RUL
		}
	}

INPUT_BAN:
	try
	{
		cout << "������� ��� �� �������(�������� 2,4,5), �� ������ 8: ";
		cin >> userInput;
		rulBan = convertString(userInput);
	}
	catch (errorReport)
	{
		cout << "������������ ������ �����, ��������� �������" << endl;
		__asm
		{
			JMP INPUT_BAN
		}
	}
}
void rulBits::getRBIT()
{
	stringstream rOur;
	string rOut;
	//������ ����
	uint8_t bitMASK = 0x01;	//������� �����, �� ��� ��������� ���� �� ���
	uint8_t cursor = 1;	//������� � �������� ���� ������ ������� ������� ��������� ����� �������
	string _t;	//�����-�� �����, ���� �� ����� �� ��������
	char nullMrk[5] = { 'N', 'U', 'L', 'L', NULL };
	char decodeBuffer[17];	//8 �������� + 7 ������� + 1 �������(������� �� ���)
	decodeBuffer[16] = 0x00;

	if (debugMode)
	{
		cout << "��������� ��������� ������: " << decodeBuffer << endl;
		cout << "������� rul=";
		printf("0x%02X", rul);
		cout << endl;
		cout << "������� rulBan=";
		printf("0x%02X", rulBan);
		cout << endl;
	}

	//������ �� ������
	memset(decodeBuffer, 0x00, 15);

	if (debugMode)
	{
		cout << "��������� ��������� ������ ����� �������: " << decodeBuffer << endl;
		cout << "������� rul=";
		printf("0x%02X", rul);
		cout << endl;
	}

	//������ ������� ����� ���. �, � � ���� ��������� (������� � ��������) = 1, ���������� �����
	for (uint8_t a = 0;a < 8;++a)
	{
		if (debugMode)
		{
			cout << "������ �" << to_string(a) << " �������� ";
			printf("0x%02X", (rul >> a));
			cout << endl;
		}

		//������� ������� � ���������� ����� ������� ��� ����� �
		if (((rul >> a)&bitMASK) == 1)
		{
			if (debugMode)
				cout << "�������� ���=1 ";

			//������� ������� �����
			__asm
			{

			}
			_t = to_string(a + 1); //���������� � ����� ������������� ��������

			if (debugMode)
				cout << "��� " << to_string(a + 1) << " ";

			decodeBuffer[cursor - 1] = _t[0];	//��������� �� ������ � ���������

			if (debugMode)
				cout << "\n������ � �������� ����� [" << _t << "], ������� �������=" << to_string(cursor - 1);

			decodeBuffer[cursor] = ',';	//��������� �������

			if (debugMode)
				cout << "\n������ � �������� ����� [" << decodeBuffer << "], ������� �������=" << to_string(cursor);

			cursor += 2; //��������� ������� ��� ��������� �����

			if (debugMode)
				cout << "\n������� �������= " << to_string(cursor) << endl;
		}
	}

	if (debugMode)
		cout << "���������..." << endl;

	//������� ��� �������� ���������
	for (uint8_t a = 15;a > 0;--a)
	{
		if (debugMode)
			cout << "������ �" << to_string(a) << endl;

		if (decodeBuffer[a] == ',')	//��� ������ ����� ������ �������
		{

			if (debugMode)
			{
				cout << "�������� ������ ������� �� ������� " << to_string(a) << "\n���������� ������� " << to_string(a) << ", ���������� [";
				printf("0x%02X]", decodeBuffer[a - 1]);
				cout << endl;
			}

			decodeBuffer[a] = NULL;	//�� ������� �
			break;
		}
	}
	rOur << decodeBuffer << endl;
	rOut = rOur.str();
	std::copy(rOut.begin(), rOut.end(), dbO.viols);
}
void rulBits::displayRBIT()const	//������ ������� � ���������� ��� �������� � �� ��� ��� � �����
{
	//������ ����
	uint8_t bitMASK = 0x01;	//������� �����, �� ��� ��������� ���� �� ���
	uint8_t cursor = 1;	//������� � �������� ���� ������ ������� ������� ��������� ����� �������
	string _t;	//�����-�� �����, ���� �� ����� �� ��������
	char nullMrk[5] = { 'N', 'U', 'L', 'L', NULL };
	char decodeBuffer[17];	//8 �������� + 7 ������� + 1 �������(������� �� ���)
	decodeBuffer[16] = 0x00;

	if (debugMode)
	{
		cout << "��������� ��������� ������: " << decodeBuffer << endl;
		cout << "������� rul=";
		printf("0x%02X", rul);
		cout << endl;
		cout << "������� rulBan=";
		printf("0x%02X", rulBan);
		cout << endl;
	}

	//������ �� ������
	memset(decodeBuffer, 0x00, 15);

	if (debugMode)
	{
		cout << "��������� ��������� ������ ����� �������: " << decodeBuffer << endl;
		cout << "������� rul=";
		printf("0x%02X", rul);
		cout << endl;
	}

	if (rul == 0x00)
	{
		arrcpy(decodeBuffer, nullMrk);
		cout << "��������� �� �������: " << decodeBuffer << endl;
		__asm
		{
			JMP PRINT_RULBAN
		}
	}

	//������ ������� ����� ���. �, � � ���� ��������� (������� � ��������) = 1, ���������� �����
	for (uint8_t a = 0;a < 8;++a)
	{
		if (debugMode)
		{
			cout << "������ �" << to_string(a) << " �������� ";
			printf("0x%02X", (rul >> a));
			cout << endl;
		}

		//������� ������� � ���������� ����� ������� ��� ����� �
		if (((rul >> a)&bitMASK) == 1)
		{
			if (debugMode)
				cout << "�������� ���=1 ";

			//������� ������� �����
			__asm
			{

			}
			_t = to_string(a + 1); //���������� � ����� ������������� ��������

			if (debugMode)
				cout << "��� " << to_string(a + 1) << " ";

			decodeBuffer[cursor - 1] = _t[0];	//��������� �� ������ � ���������

			if (debugMode)
				cout << "\n������ � �������� ����� [" << _t << "], ������� �������=" << to_string(cursor - 1);

			decodeBuffer[cursor] = ',';	//��������� �������

			if (debugMode)
				cout << "\n������ � �������� ����� [" << decodeBuffer << "], ������� �������=" << to_string(cursor);

			cursor += 2; //��������� ������� ��� ��������� �����

			if (debugMode)
				cout << "\n������� �������= " << to_string(cursor) << endl;
		}
	}

	if (debugMode)
		cout << "���������..." << endl;

	//������� ��� �������� ���������
	for (uint8_t a = 15;a > 0;--a)
	{
		if (debugMode)
			cout << "������ �" << to_string(a) << endl;

		if (decodeBuffer[a] == ',')	//��� ������ ����� ������ �������
		{

			if (debugMode)
			{
				cout << "�������� ������ ������� �� ������� " << to_string(a) << "\n���������� ������� " << to_string(a) << ", ���������� [";
				printf("0x%02X]", decodeBuffer[a - 1]);
				cout << endl;
			}

			decodeBuffer[a] = NULL;	//�� ������� �
			break;
		}
	}
	cout << "��������� �� �������: " << decodeBuffer << endl;

PRINT_RULBAN:

	if (rulBan == 0x00)
	{
		arrcpy(decodeBuffer, nullMrk);
		__asm
		{
			JMP RBIT_EXIT
		}
	}

	if (debugMode)
		cout << "��������� ��������� ������: " << decodeBuffer << endl;

	//������ �� ������ � ��������������� ��������
	memset(decodeBuffer, 0x00, 15);
	cursor = 1;

	if (debugMode)
	{
		cout << "��������� ��������� ������ ����� �������: " << decodeBuffer << endl;
		cout << "������� rulBan=";
		printf("0x%02X", rulBan);
		cout << endl;
	}

	//������ ������� ����� ���. �, � � ���� ��������� (������� � ��������) = 1, ���������� �����
	for (uint8_t a = 0;a < 8;++a)
	{
		if (debugMode)
		{
			cout << "������ �" << to_string(a) << " �������� ";
			printf("0x%02X", (rul >> a));
			cout << endl;
		}

		//������� ������� � ���������� ����� ������� ��� ����� �
		if (((rulBan >> a)&bitMASK) == 1)
		{
			if (debugMode)
				cout << "�������� ���=1 ";

			//������� ������� �����
			__asm
			{

			}
			_t = to_string(a + 1); //���������� � ����� ������������� ��������

			if (debugMode)
				cout << "��� " << to_string(a + 1) << " ";

			decodeBuffer[cursor - 1] = _t[0];	//��������� �� ������ � ���������

			if (debugMode)
				cout << "\n������ � �������� ����� [" << _t << "], ������� �������=" << to_string(cursor - 1);

			decodeBuffer[cursor] = ',';	//��������� �������

			if (debugMode)
				cout << "\n������ � �������� ����� [" << decodeBuffer << "], ������� �������=" << to_string(cursor);

			cursor += 2; //��������� ������� ��� ��������� �����

			if (debugMode)
				cout << "\n������� �������= " << to_string(cursor) << endl;
		}
	}

	if (debugMode)
		cout << "���������..." << endl;


	//������� ��� �������� ���������
	for (uint8_t a = 15;a > 0;--a)
	{
		if (debugMode)
			cout << "������ �" << to_string(a) << endl;

		if (decodeBuffer[a] == ',')
		{

			if (debugMode)
			{
				cout << "�������� ������ ������� �� ������� " << to_string(a) << "\n���������� ������� " << to_string(a) << ", ���������� [";
				printf("0x%02X]", decodeBuffer[a - 1]);
				cout << endl;
			}

			decodeBuffer[a] = NULL;
			break;
		}
	}

RBIT_EXIT:

	cout << "��� �� �������: " << decodeBuffer << endl;
}