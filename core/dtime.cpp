#include <string>
#include <algorithm>

#include "cpp.h"

const unsigned char templatePos[10] = { 0x00,0x03,0x06,0x08,0x0B,0x0E,0x02,0x05,0x0A,0x0D };	//������� ��� ������� ����� � ��������-�������� ������� ����-������� � ��������� �� �������� ���������� �����
uint8_t daymonth;	//���������� ����������, �������� �� ������ dTime
uint64_t symbols;	//������� ��������, ������� �� ������ dTime

string dTime::decodeDTime(char inputArg[DT_length + 1])	//����������� ������ ����-�������
{
	char DTimeTemplate[17] = "  /  /       :  ";	//������ �����
	char tempBuf[3];	//����� + 1 ���� ������
	string strBuf;
	uint8_t temp = NULL;

	if (debugMode)
	{
		cout << "��������� ������ (����-�����): [";
		for (uint8_t a = 0;a < DT_length;++a)
		{
			printf("0x%02x ", inputArg[a]);
		}
		cout << "]" << endl;
		cout << "Template=" << DTimeTemplate << endl;
	}

	for (uint8_t a = 0;a < DT_length;++a)	//�������(�����������) ����-����� � ��������-�������� ������
	{
		temp = inputArg[a];	//�������� � ����� ������������ �����
		strBuf = std::to_string(temp);
		//_itoa_s(temp, tempBuf, 10);	//���������� ���������, ����������
		strBuf.copy(tempBuf, 10);	//���������� ���������

		if (debugMode)
			cout << "temp=" << temp << " tempbuf=" << tempBuf << endl;

		if (temp < 10)	//���� ��������� ������� ������������� ��������
						//���� ����� ������ 10, �� ���� �������������,
						//�� ������� ������ � ����� ���������� ����
		{
			tempBuf[1] = tempBuf[0];
			tempBuf[0] = '0';
		}
		arrcpyw(DTimeTemplate, tempBuf, templatePos[a], 2);	//��������� ����� � ������ �� ������ �������, �� templatePos

		if (debugMode)
			cout << "Template=" << DTimeTemplate << endl;
	}

	if (debugMode)
		cout << "Template=" << DTimeTemplate << endl;

	return DTimeTemplate;

}	  //����������� ������ ����-�������
bool dTime::processInput(char *out, string inputArg)	//��������� ����� ������������ � ��������� � �������� ������
{
	string convertbuff;
	if (debugMode)
		cout << "�������� " << inputArg << endl;

	//������� ����� �� ���������������� ��������
	inputArg.erase(std::remove(inputArg.begin(), inputArg.end(), '/'), inputArg.end());

	if (debugMode)
		cout << "������� ������ ������ " << inputArg << endl;

	inputArg.erase(std::remove(inputArg.begin(), inputArg.end(), ' '), inputArg.end());

	if (debugMode)
		cout << "������� ������ ������ " << inputArg << endl;

	inputArg.erase(std::remove(inputArg.begin(), inputArg.end(), ':'), inputArg.end());

	if (debugMode)
		cout << "������� ������ ������ " << inputArg << endl;

	//��������� � �������� ������
	for (uint8_t a = 0;a < DT_length;++a)
	{
		//�������������� ������� � ����-������
		convertbuff = inputArg.substr(a * 2, 2);

		if (debugMode)
			cout << a * 2 << " " << (a * 2) + 2 << " " << endl;

		//���������� ����� � ������
		daymonth = atoi(convertbuff.c_str());

		if (debugMode)
		{
			cout << "convertbuff " << convertbuff << "\n";
			cout << "daymonth=";
			printf("0x%02x", daymonth);
			cout << endl;
		}

		//������ �����
		out[a] = daymonth;
	}
	out[DT_length + 1] = '\0';

	if (debugMode)
	{
		cout << "out=" << out << endl;

		cout << "��������� ������ (����-�����): [";
		for (uint8_t a = 0;a < DT_length;++a)
		{
			printf("0x%02x ", inputArg[a]);
		}
		cout << "]" << endl;
	}

	return 1;	//� ������ ������ ���������� �������
}
bool dTime::checkForm(string inputArg)	//������ ����� ��� �������� ������������, ���� ���-�� �� ��� ������ ����������
{
	char DTimeTemplate[17] = "  /  /       :  ";	//������ �����
	bool state = 0;
	//cout << "���������� ��������� ��������: " << symbols << endl;
	if (symbols != 17)	//��������� ���������� ��������� �������� - �� ������ ���� ���� 17 ���� 0(1)
	{
		if (symbols == 1)
			state = 0;
		throw errorReport(0x06);
	}

	for (uint8_t a = 0;a < 4; ++a)	//��������� ������������ ������������ �������, ����� �������� ����� �� ������
	{
		if (!(inputArg[templatePos[a + DT_length]] == DTimeTemplate[templatePos[a + DT_length]]))
			throw errorReport(0x06);
		//cout << "err=" << to_string(a) << "(" << to_string(a+DT_length) << ")(" << to_string(templatePos[a+DT_length]) << ") char=" << inputArg[templatePos[a + DT_length]] << "template=" << DTimeTemplate[templatePos[a + DT_length]] << endl;
	}

	//���������, ����� ���� �������� �����, � �� ����� ��� ������ �����
	daymonth = 0;
	for (uint8_t a = 0; a < DT_UInputLng - 1; a++)
	{
		if (inputArg[a] == DTimeTemplate[templatePos[daymonth + DT_length]]) //���������� ������� �������, ����� �� �������� ������ ������������
		{
			//cout << "trig skip=" << inputArg[a] << endl;
			++daymonth;
			++a;
		}
		//cout << "check char=" << inputArg[a] << endl;
		if (!isdigit(inputArg[a])) //���� ���� �� �������� ������ - ������ ����������
			throw errorReport(0x06);
	}
	state = 1;
	return state;
}
void dTime::readInputDTIME()	//���� ����-��������� ��� ���������� �����
{
	char charUserInput[DT_UInputLng];

	std::cin.ignore();

	//���� �� ������������ ���� ���������� � ����, ����� DTIME_add
DTIME_add:
	try
	{
		//������� ��������� ������ �� ������������, ������� ������ ����
		//������ ������������ �������
		cout << "������� ���� ���������� � ���� � ������� ��/��/���� ��:��: ";
		cin.getline(charUserInput, DT_UInputLng);
		if (debugMode)
		{
			cout << "��������� ������ (����-�����): [";
			for (uint8_t a = 0;a < DT_UInputLng;++a)
			{
				printf("[%c] ", charUserInput[a]);
			}
			cout << "]" << endl;
		}

		//�������� ���� � ����������� ��������� �������� � ��������� ����� ��������
		//������������ �����
		symbols = cin.gcount();
		checkForm(charUserInput);
	}
	catch (errorReport)
	{
		//� ������ ������ - �������� � ��������� ���������
		cout << "��������� �������\n������ ��/��/���� ��:��" << endl;
		__asm
		{
			JMP DTIME_add; //goto DTIME_add
		}
	}

	//�������� ���������� ���� � �������� ������������ �����
	//if (processInput(dtsta, charUserInput))
		//cout << "��������� �����: �����!" << endl;
	processInput(dtsta, charUserInput);

	if (debugMode)
	{
		cout << "��������� ������ (����-�����): [";
		for (uint8_t a = 0;a < DT_length;++a)
		{
			printf("[%c] ", dtsta[a]);
		}
		cout << "]" << endl;
	}

	//���� �� ������������ ���� ��������� ����, ����� DTIME_expires
DTIME_expires:
	try
	{
		//������� ��������� ������� �� ������������, ������� ������ ����
		//������ ������������� �������
		cout << "������� ���� ��������� ����� � ������� ��/��/��� ��:��: ";
		cin.getline(charUserInput, DT_UInputLng);
		if (debugMode)
		{
			cout << "��������� ������ (����-�����): [";
			for (uint8_t a = 0;a < DT_UInputLng;++a)
			{
				printf("[%c] ", charUserInput[a]);
			}
			cout << "]" << endl;
		}

		//�������� ���� � ���������� ��������� �������� � ��������� ����� ��������
		//������������ �����
		symbols = cin.gcount();
		checkForm(charUserInput);

	}
	catch (errorReport)
	{
		//� ������� ������ - �������� � ��������� ���������
		cout << "��������� �������\n������ ��/��/���� ��:��" << endl;
		__asm
		{
			JMP DTIME_expires;	//goto DTIME_expires
		}
	}

	//�������� ���������� ���� � �������� ������������ �����
	//if (processInput(dtstp, charUserInput))
		//cout << "��������� �����: �����!" << endl;
	processInput(dtstp, charUserInput);

	if (debugMode)
	{
		cout << "��������� ������ (����-�����): [";
		for (uint8_t a = 0;a < DT_length;++a)
		{
			printf("0x%02x ", dtstp[a]);
		}
		cout << "]" << endl;
	}

	//���� �� ������������ ���� ����, ���� ������ ���� ���� ���� �� ����, ����� DTIME_ban
DTIME_ban:
	try
	{
		cout << "������� ���� ���� � ������� ��/��/���� ��:��: \n (���� �������� ������ ���� ���� �� ����)";
		fgets(charUserInput, DT_UInputLng, stdin);
		if (strcmp(charUserInput, "\n") == 0)
		{
			cout << "[�������]" << endl;
			memset(dtban, 0x00, DT_length);

			if (debugMode)
			{
				cout << "��������� ������ (����-�����): [";
				for (uint8_t a = 0;a < DT_UInputLng;++a)
				{
					printf("[%c] ", charUserInput[a]);
				}
				cout << "]" << endl;
			}

			return;

		}
		if (debugMode)
		{
			cout << "��������� ������ (����-�����): [";
			for (uint8_t a = 0;a < DT_UInputLng;++a)
			{
				printf("[%c] ", charUserInput[a]);
			}
			cout << "]" << endl;
		}
	}
	catch (errorReport)
	{
		//� ������ ������ - �������� � ��������� ���������
		cout << "��������� �������\n������ ��/��/���� ��:��" << endl;
		__asm
		{
			JMP DTIME_ban;	//goto DTIME_ban
		}
	}
	//if (processInput(dtban, charUserInput))
		//cout << "��������� �����: �����!" << endl;
	processInput(dtban, charUserInput);

	if (debugMode)
	{
		cout << "��������� ������ (����-�����): [";
		for (uint8_t a = 0;a < DT_length;++a)
		{
			printf("0x%02x ", dtban[a]);
		}
		cout << "]" << endl;

		cout << "Entered: [" << dtsta << "]" << endl;
		cout << "Entered: [" << dtstp << "]" << endl;
		cout << "Entered: [" << dtban << "]" << endl;
	}
}
void dTime::displayDTIME()	// ������������� �����, ������� ������������ ������ � ��������-�������� ������
{
	string UOutAdd;	//string ���������� ��� �������� ������������� ��������
	string UOutExp;
	string UOutBan;
	UOutAdd = decodeDTime(dtsta);	//���������� �������� �� ��������� ������� � ��������-�������� ���
	UOutExp = decodeDTime(dtstp);
	UOutBan = decodeDTime(dtban);
	cout << "�������� � ����: " << UOutAdd << "\n��������� �����: " << UOutExp << "\n���: " << UOutBan << endl;
}
void dTime::writeDtStart(char arg[DT_length + 1])
{
	arrcpy(dtsta, arg);
}
void dTime::writeDtStop(char arg[DT_length + 1])
{
	arrcpy(dtstp, arg);
}
void dTime::writeDtBan(char arg[DT_length + 1])
{
	arrcpy(dtban, arg);
}
dTime::dTime()
{
	memset(dtsta, NULL, sizeof(dtsta));
	memset(dtstp, NULL, sizeof(dtstp));
	memset(dtban, NULL, sizeof(dtban));
}
dTime::dTime(char dStart[DT_length], char dStop[DT_length], char dBan[DT_length])
{
	arrcpy(dtsta, dStart);
	arrcpy(dtstp, dStop);
	arrcpy(dtban, dBan);
}
void dTime::dTimeImportDates(string timeStart, string timeEnd, string timeBan)
{
	processInput(dtsta, timeStart);
	processInput(dtstp, timeEnd);
	processInput(dtban, timeBan);
}
string dTime::getADDTIME()
{
	string temp;
	temp = decodeDTime(this->dtsta);
	return temp;
}
void dTime::getAddDate()
{
	string temp;
	temp = decodeDTime(this->dtsta);
	std::copy(temp.begin(), temp.end(), dbO.recAdd);
}
void dTime::getExpDate()
{
	string temp;
	temp = decodeDTime(this->dtstp);
	std::copy(temp.begin(), temp.end(), dbO.recExp);
}