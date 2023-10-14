#include <sstream>
#include <iomanip>
#include "cpp.h"

vkMeta::vkMeta()
{
	memset(vkId, NULL, sizeof(vkId));
	memset(vkUrl, NULL, sizeof(vkUrl));
	memset(vkName, NULL, sizeof(vkName));
	memset(vkFam, NULL, sizeof(vkFam));
}
vkMeta::vkMeta(char vId[VKID_length], char vUrl[VKURL_length], char vNam[VKUINFO_length], char vFam[VKUINFO_length])
{
	arrcpy(vkId, vId);
	arrcpy(vkUrl, vUrl);
	arrcpy(vkName, vNam);
	arrcpy(vkFam, vFam);
}
void vkMeta::writeId(char arg[VKID_length])
{
	arrcpy(vkId, arg);
}
void vkMeta::writeUrl(char arg[VKURL_length])
{
	arrcpy(vkUrl, arg);
}
void vkMeta::writeName(char arg[VKUINFO_length])
{
	arrcpy(vkName, arg);
}
void vkMeta::writeFam(char arg[VKUINFO_length])
{
	arrcpy(vkFam, arg);
}
void vkMeta::readInputVKMETA()
{
	char vID_RAW_Container[VKID_length + 1];
vkID_Input:
	cout << "Введите VK ID: ";
	cin >> vID_RAW_Container;
	if (!checkNcopy(vID_RAW_Container, vkId))
	{
		cout << "VK ID может содержать только цифры!\nПовторите попытку" << endl;
		__asm
		{
			JMP vkID_Input
		}
	}
	//cout << "VKID=" << vkId << endl;
	cout << "Введите VK URL: ";
	cin >> vkUrl;
	cout << "Введите Имя пользователя VK: ";
	cin >> vkName;
	cout << "Введите Фамилию пользователя VK: ";
	cin >> vkFam;
}
void vkMeta::displayVKMETA()const
{
	cout << "VK ID=";
	for (uint8_t a = 0;a < VKID_length;++a)
	{
		printf("%c", vkId[a]);
	}
	cout << "\nVK URL=" << vkUrl << "\nVK Имя=" << vkName << "\nVK Фамилия=" << vkFam << endl;
}
void vkMeta::getVKIDchar()
{
	stringstream temp2;
	temp2 << setw(4) << setfill('0') << vkId << endl;
	string temp = temp2.str();
	for (uint8_t a = 0;a < 10;++a)
		dbO.vkID[a] = temp[a];
}
void vkMeta::getVKurl()
{
	stringstream temp2;
	temp2 << vkUrl << endl;
	string temp = temp2.str();
	for (uint8_t a = 0;a < temp.length();++a)
	{
		dbO.vkurl[a] = temp[a];
	}
}
bool vkMeta::checkNcopy(char *inputArg, char *destination)
{
	uint8_t shift = 0;
	//cout << "Проверяю ввод..." << endl;
	for (uint8_t a = 0;a < VKID_length + 1;++a)
	{
		//cout << "Проверяю [" << inputArg[a] << "]" << endl;
		if (inputArg[a] == NULL)
		{
			//cout << "EOF trig stop at " << to_string(a) << "!" << endl;
			shift = a;
			break;
		}
		if (!isdigit(inputArg[a]))
			return 0;
	}
	//arrcpy(destination, inputArg);
	arrcpyw(destination, inputArg, VKID_length - shift, shift);
	//cout << "Запись...[";
	//for (uint8_t a = 0;a < VKID_length;++a)
	//{
	//	printf("%c", vkId[a]);
	//}
	//cout << "]" << endl;
	return 1;
}