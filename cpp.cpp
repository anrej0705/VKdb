#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0501
#include <SDKDDKVer.h>
#include <iostream>
#include <ios>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <string>
#include <stdint.h>
#include <conio.h>
#include <exception>
#include <Windows.h>
#include <fstream>
#include <list>
#include <array>
#include <iterator>
#include <stack>
#include <iomanip>
#include "gui.h"
#include "bridge.h"
#include "menu.h"
#include "cpp.h"

using namespace std;
uint8_t repeatTries = 0;		//Количество попыток ввода

parserParams Parp;
errorReport::errorReport()
{
	cout << "Общее исключение" << endl;
	_getch();
}
errorReport::errorReport(uint8_t reasonCode)
{
	switch (reasonCode)
	{
	case 0x04:
	{
		cout << "Исключение код 0x04\nМетод printRecord должен вызываться только при всех заполненных ячейках" << endl;
		_getch();
		exit(1);
	}
	case 0x05:
	{
		cout << "Исключение код 0x05\nМассивы не совпадают по размерам" << endl;
		_getch();
		exit(1);
	}
	case 0x06:
	{
		//cout << "Исключение код 0x06\nНеправильный формат ввода" << endl;
	}
	}
}
//---------------------------------------------------------------------------------------------------------
class debugMessage;
summary::summary()
{
	char nullMrk[5] = { 'N', 'U', 'L', 'L', NULL };
	uint8_t RecTg[6] = { 'R', 'U', 'K', 'I', 'A' };
	memset(RecTAG, 0x00, 8);
	arrcpy(RecTAG, RecTg);
	setRecID(totalObj);
	memset(vkId, '0', VKID_length);
	memset(vkUrl, 0x00, VKURL_length);
	memset(vkName, 0x00, VKUINFO_length);
	memset(vkFam, 0x00, VKUINFO_length);
	//arrcpy(vkId, nullMrk);
	arrcpy(vkUrl, nullMrk);
	arrcpy(vkName, nullMrk);
	arrcpy(vkFam, nullMrk);
	statusRegister = (uint8_t)0x00;
	memset(dtsta, 0x00, DT_length);
	memset(dtstp, 0x00, DT_length);
	memset(dtban, 0x00, DT_length);
	rulBan = 0x00;
	rul = 0x00;
	if (debugMode)
	{
		printf("0x%02X 0x%02X", getRulBits(), getRulBanBits());
		cout << endl;
	}
	memset(dbRem, 0x00, REM_length);
	arrcpy(dbRem, nullMrk);
	++completeRecord;
	totalObj++;
}
summary::~summary()
{
	totalObj--;
}
void summary::newRecord()
{
	displayDBREC();
	readInputVKMETA();
	readInputDTIME();
	readInputFLAGS();
	readInputRBIT();
	readInputREM();
	completeRecord = 1;
}
void summary::printRecord()
{
	if (completeRecord != 1)
		throw errorReport(0x04);	//при попытке вызвать отчет при неполных ячейках критуем прогу
	displayDBREC();
	displayVKMETA();
	displayDTIME();
	displayFLAGS();
	if (debugMode)
		printf("Регистр RulBits=0x%02X, \nРегистр RulBitsBan=0x%02X\n", getRulBits(), getRulBanBits());
	displayRBIT();
	displayREM();
}
void summary::importEntry()	//форма для импорта от парсера
{
	uint8_t tempFlag = 0;
	std::copy(Parp.vkid.begin(), Parp.vkid.end(), vkId);
	std::copy(Parp.vkurl.begin(), Parp.vkurl.end(), vkUrl);
	std::copy(Parp.vknam.begin(), Parp.vknam.end(), vkName);
	std::copy(Parp.vkfam.begin(), Parp.vkfam.end(), vkFam);
	dTimeImportDates(Parp.dateAdd, Parp.dateEnd, Parp.dateBan);
	Parp.active == 1 ? tempFlag |= 0x01 : tempFlag = tempFlag;
	Parp.expired == 0 ? tempFlag |= 0x02 : tempFlag = tempFlag;
	Parp.ban == 1 ? tempFlag |= 0x04 : tempFlag = tempFlag;
	//cout << "status register=";
	//printf("0x%02X", tempFlag);
	//cout << endl;
	loadFlags(tempFlag);
	importRBIT(Parp.rul, Parp.rulBan);
	std::copy(Parp.remark.begin(), Parp.remark.end(), dbRem);
}
void summary::showRECID()
{
	cout << "ИД записи: " << to_string(recordId) << endl;
}
void summary::showObjs()
{
	cout << "Всего " << totalObj << " записей" << endl;
}
void summary::printRECID()const
{
	std::setw(ios::fixed);
	cout << setw(4) << setfill('0') << right << recordId;
}
void summary::printVKID()const
{
	for (uint8_t a = 0;a < VKID_length;++a)
	{
		printf("%c", vkId[a]);
	}
}
void summary::printVKURL()const
{
	std::setw(ios::fixed);
	cout << setw(VKURL_length) << setfill(' ') << left << vkUrl;
}
void summary::printVKNAM()const
{
	std::setw(ios::fixed);
	cout << setw(VKUINFO_length) << setfill(' ') << left << vkName;
}
void summary::printVKFAM()const
{
	std::setw(ios::fixed);
	cout << setw(VKUINFO_length) << setfill(' ') << left << vkFam;
}
uint16_t summary::getRECID()
{
	return recordId;
}
string summary::getVKID()
{
	return vkId;
}
string summary::getVKNICK()
{
	return vkUrl;
}
string summary::getVKNAME()
{
	return vkName;
}
string summary::getVKFAM()
{
	return vkFam;
}
oldParser::oldParser()
{
	tabRecords = 0;
	importPos = 0;
}
void oldParser::openOldDb(string fileName)
{
	tabFile.open(fileName, ios::in);
	if (tabFile)
		cout << "Открыто: " << fileName << endl;
}
void oldParser::importTable()
{
	uint8_t tabVer;
	string dbName;
	uint16_t records;
	uint16_t bans;
	string temp;

	getline(tabFile, input);
	//cout << "input=" << input << endl;
	temp = input.substr(12, 2);
	//cout << "temp=" << temp << endl;
	tabVer = atoi(temp.c_str());
	//cout << "Версия таблицы: " << to_string(tabVer) << endl;

	getline(tabFile, input);
	//cout << "input=" << input << endl;
	temp = input.substr(9, 29);
	//cout << "temp=" << temp << endl;
	dbName = temp;
	//cout << "БД: " << dbName << endl;

	getline(tabFile, input);
	//cout << "input=" << input << endl;
	temp = input.substr(10, 4);
	//cout << "temp=" << temp << endl;
	records = atoi(temp.c_str());
	temp = input.substr(15, 4);
	//cout << "temp=" << temp << endl;
	bans = atoi(temp.c_str());
	//cout << "Всего записей: " << to_string(records) << "\nБанов: " << to_string(bans) << endl;
	tabRecords = records;
	dbH.tabVer = tabVer;
	std::copy(dbName.begin(), dbName.end(), dbH.dbName);
	dbH.records = records;
	dbH.bans = bans;
}
void oldParser::processImport()
{

	uint16_t recId;
	string vkid;
	string vkurl;
	string vknam;
	string vkfam;
	string dateAdd;
	string dateEnd;
	string dateBan;
	bool active = 0;
	bool expired = 0;
	bool ban = 0;
	string rul;
	string rulBan;
	string remark;

	int susbstroEntry = 0;
	int subStrEnd = 0;
	string idTag = "ID";
	string vkurlTag = "VK_URL";
	string vknmTag = "VN";
	string vkfamTag = "VF";
	string lockTag = "LOCK";
	string toutTag = "TOUT";
	string remTag = "REM";
	string endRecTag = "RUKIA";
	int idTagLng = 2;
	int vkurlTagLng = 6;
	int vknmTagLng = 2;
	int vkfamTagLng = 2;
	int lockTagLng = 4;
	int toutTagLng = 4;
	int remTagLng = 3;
	int stepback = 5;

	string temp;
	string temp2;
	string temp3;
	string temp4 = "23:59";
	string temp5;
	string rulBanTempl = "0";
	string DTimeTemplate = "  /  /       :  ";
	string rulsTemplate = " , , , , , , , ";
	uint8_t insertPos[12] = { 0,1,3,4,6,7,8,9,11,12,14,15 };
	uint8_t rulInsertPos[8] = { 0,2,4,6,8,10,12,14 };

	//cout << "\n\nИмпортирую записи...\n" << endl;

	getline(tabFile, input);
	//cout << "input=" << input << endl;
	temp = input.substr(2, 4);
	//cout << "temp=" << temp << endl;
	recId = atoi(temp.c_str());
	susbstroEntry = input.find(idTag);
	//cout << "Тег " << idTag << " с позиции: " << susbstroEntry << endl;
	temp = input.substr(susbstroEntry + idTagLng + 3, 10);
	vkid = temp;
	susbstroEntry = input.find(vkurlTag);
	//cout << "Тег " << vkurlTag << " с позиции: " << susbstroEntry << endl;
	for (uint8_t a = susbstroEntry + idTagLng + 3;a < 255;++a)
	{
		if (input[a] == ' ' || input[a] == '\t')
		{
			//cout << "Конец подстроки на позиции " << to_string(a) << endl;
			subStrEnd = a - (susbstroEntry + idTagLng + 3);
			break;
		}
	}
	temp = input.substr(susbstroEntry + vkurlTagLng + 1, VKURL_length - 1);
	for (uint8_t a = VKURL_length - 1;a > 0;--a)
	{
		if (temp[a] == ' ')
			temp[a] = NULL;
	}
	vkurl = temp;
	susbstroEntry = input.find(vknmTag);
	//cout << "Тег " << vknmTag << " с позиции: " << susbstroEntry << endl;
	for (uint8_t a = susbstroEntry + vknmTagLng + 3;a < 255;++a)
	{
		if (input[a] == ' ' || input[a] == '\t')
		{
			//cout << "Конец подстроки на позиции " << to_string(a) << endl;
			subStrEnd = a - (susbstroEntry + vknmTagLng + 3);
			break;
		}
	}
	temp = input.substr(susbstroEntry + vknmTagLng + 1, 20);
	for (uint8_t a = 20;a > 0;--a)
	{
		if (temp[a] == ' ')
			temp[a] = NULL;
	}
	vknam = temp;
	susbstroEntry = input.find(vkfamTag);
	//cout << "Тег " << vkfamTag << " с позиции: " << susbstroEntry << endl;
	for (uint8_t a = susbstroEntry + vkfamTagLng + 3;a < 255;++a)
	{
		if (input[a] == ' ' || input[a] == '	')
		{
			//cout << "Конец подстроки на позиции " << to_string(a) << endl;
			subStrEnd = a - (susbstroEntry + vkfamTagLng + 3);
			break;
		}
	}
	temp = input.substr(susbstroEntry + vkfamTagLng + 1, 20);
	for (uint8_t a = 20;a > 0;--a)
	{
		if (temp[a] == ' ')
			temp[a] = NULL;
	}
	vkfam = temp;
	susbstroEntry = input.find(lockTag);
	//cout << "Тег " << lockTag << " с позиции: " << susbstroEntry << endl;
	for (uint8_t a = susbstroEntry + lockTagLng + 3;a < 255;++a)
	{
		if (input[a] == ' ' || input[a] == '\t')
		{
			//cout << "Конец подстроки на позиции " << to_string(a) << endl;
			subStrEnd = a - (susbstroEntry + lockTagLng + 3);
			break;
		}
	}
	temp = input.substr(susbstroEntry + lockTagLng + 1, 16);
	//cout << "temp=" << temp << endl;
	//cout << "temp2=" << temp2 << endl;
	temp2 = DTimeTemplate;
	for (uint8_t a = 0;a < 12;++a)
	{
		temp2[insertPos[a]] = temp[insertPos[a]];
	}
	//cout << "temp2=" << temp2 << endl;
	dateAdd = temp2;
	temp2 = DTimeTemplate;
	susbstroEntry = input.find(lockTag);
	//cout << "Тег " << lockTag << " с позиции: " << susbstroEntry << endl;
	temp = input.substr(susbstroEntry + lockTagLng + 12, 5);
	temp3 = temp;
	susbstroEntry = input.find(toutTag);
	//cout << "Тег " << toutTag << " с позиции: " << susbstroEntry << endl;
	temp = input.substr(susbstroEntry + toutTagLng + 1, 10);
	//cout << "temp=" << temp << endl;
	//cout << "temp2=" << temp2 << endl;
	//cout << "temp3=" << temp3 << endl;
	temp += " ";
	temp += temp3;
	//cout << "temp=" << temp << endl;
	for (uint8_t a = 0;a < 12;++a)
	{
		temp2[insertPos[a]] = temp[insertPos[a]];
	}
	//cout << "temp2=" << temp2 << endl;
	dateEnd = temp2;
	temp2 = DTimeTemplate;
	susbstroEntry = input.find(toutTag);
	//cout << "Тег " << toutTag << " с позиции: " << susbstroEntry << endl;
	temp = input.substr(susbstroEntry + toutTagLng + 15, 10);
	//cout << "temp=" << temp << endl;
	temp += " ";
	temp += temp4;
	//cout << "temp=" << temp << endl;
	//cout << "temp2=" << temp2 << endl;
	for (uint8_t a = 0;a < 12;++a)
	{
		temp2[insertPos[a]] = temp[insertPos[a]];
	}
	//cout << "temp2=" << temp2 << endl;
	dateBan = temp2;
	susbstroEntry = input.find(lockTag);
	//cout << "Тег " << lockTag << " с позиции: " << susbstroEntry << endl;
	temp = input.substr(susbstroEntry + lockTagLng + 23, 15);
	//cout << "temp=" << temp << endl;
	for (uint8_t a = 0;a < 8;++a)
	{
		temp5 = temp[rulInsertPos[a]];
		if (atoi(temp5.c_str()))
		{
			//cout << "Правило " << to_string(a + 1) << " истинно" << endl;
			rul += to_string(a + 1);
			rul += ",";
		}
	}
	if (rul.empty())
		rul = rulBanTempl;
	if (rul[rul.size() - 1] == ',')
	{
		rul[rul.size() - 1] = NULL;
	}
	susbstroEntry = input.find(toutTag);
	//cout << "Тег " << toutTag << " с позиции: " << susbstroEntry << endl;
	temp = input.substr(susbstroEntry + toutTagLng + 31, 15);
	//cout << "temp=" << temp << endl;
	for (uint8_t a = 0;a < 8;++a)
	{
		temp5 = temp[rulInsertPos[a]];
		if (atoi(temp5.c_str()))
		{
			//cout << "Правило " << to_string(a + 1) << " истинно" << endl;
			rulBan += to_string(a + 1);
			rulBan += ",";
		}
	}
	if (rulBan.empty())
		rulBan = rulBanTempl;
	if (rulBan[rulBan.size() - 1] == ',')
	{
		rulBan[rulBan.size() - 1] = NULL;
	}
	//cout << "rulBan=" << rulBan << endl;
	susbstroEntry = input.find(remTag);
	//cout << "Тег " << remTag << " с позиции: " << susbstroEntry << endl;
	subStrEnd = input.find(endRecTag);
	temp = input.substr(susbstroEntry + remTagLng + 1, subStrEnd - (susbstroEntry + stepback));
	for (uint8_t a = subStrEnd - (susbstroEntry + stepback);a > 0;--a)
	{
		if (temp[a] == ' ')
			temp[a] = NULL;
	}
	remark = temp;
	//cout << "temp=" << temp << endl;
	susbstroEntry = input.find(toutTag);
	temp = input.substr(susbstroEntry + toutTagLng + 13, 1);
	//cout << "temp=" << temp << endl;
	char ch = temp[0];
	//cout << "ch=" << ch << endl;
	switch (ch)
	{
	case 'A':
	{
		active = 1;
		break;
	}
	case 'T':
	{
		expired = 1;
		dateBan = "00/00/0000 00:00";
		break;
	}
	case 'B':
	{
		ban = 1;
		break;
	}
	}
	if (debugMode)
	{
		cout << "ИД записи: " << to_string(recId) << endl;
		cout << "VK ID: " << vkid << endl;
		cout << "VK URL: " << vkurl << endl;
		cout << "Имя пользователя VK: " << vknam << endl;
		cout << "Фамилия пользователя VK: " << vkfam << endl;
		cout << "Дата внесения в базу: " << dateAdd << endl;
		cout << "Дата окончания срока: " << dateEnd << endl;
		cout << "Дата бана: " << dateBan << endl;
		cout << "Запись активна: ";
		active ? cout << "ИСТИНА\n" : cout << "ЛОЖЬ\n";
		cout << "Срок истёк: ";
		expired ? cout << "ИСТИНА\n" : cout << "ЛОЖЬ\n";
		cout << "Пользователь забанен: ";
		ban ? cout << "ИСТИНА\n" : cout << "ЛОЖЬ\n";
		cout << "Пункты нарушений: " << rul << endl;
		cout << "Пункты нарушений, приведшие к бану: " << rulBan << endl;
		cout << "Примечание к записи: " << remark << endl;
	}

	//сохраняем в структуру
	Parp.recId = recId;
	Parp.vkid = vkid;
	Parp.vkurl = vkurl;
	Parp.vknam = vknam;
	Parp.vkfam = vkfam;
	Parp.dateAdd = dateAdd;
	Parp.dateEnd = dateEnd;
	Parp.dateBan = dateBan;
	Parp.active = active;
	Parp.expired = expired;
	Parp.ban = ban;
	Parp.rul = rul;
	Parp.rulBan = rulBan;
	Parp.remark = remark;
	if (importPos < tabRecords)
		Parp.next = 1;
	else if (importPos == tabRecords)
		Parp.next = 0;
	else
		Parp.next = 0;

	rul.clear();
	rulBan.clear();
	active = 0;
	expired = 0;
	ban = 0;
}
uint16_t oldParser::getImportPos()
{
	return importPos;
}
uint16_t oldParser::getRecords()
{
	return tabRecords;
}
void oldParser::closeOldDb()
{
	tabFile.close();
}
dbHeader::dbHeader()
{
	this->writedRls = 0;
	string newTag = "FULCRUM";
	string RulSepTag = "SHDHRT";
	string dbNamNull = "NULL";
	testByte = 0x2D;	//На дебаг
	checkByte = 0x3E;	//На дебаг
	CRCPolynome = 0x3C;	//Полином для контрольной суммы
	std::copy(newTag.begin(), newTag.end(), fulcTag);
	std::copy(RulSepTag.begin(), RulSepTag.end(), rulSepTag);
	memset(rules, NULL, sizeof(rules));
	memset(dbName, NULL, sizeof(dbName));
	std::copy(dbNamNull.begin(), dbNamNull.end(), dbName);
	recCnt = 0;
	banCnt = 0;
	memset(rulLeng, NULL, sizeof(rulLeng));
	writedRls = 0;
	tableVer = 0;
}
void dbHeader::importOLP()
{
	recCnt = dbH.records;
	banCnt = dbH.bans;
	arrcpy(dbName, dbH.dbName);
	tableVer = dbH.tabVer;
}
void dbHeader::importDbName(string dbNam)
{
	std::copy(dbNam.begin(), dbNam.end(), dbName);
}
void dbHeader::importStats(uint16_t recs, uint16_t bans)
{
	this->recCnt = recs;
	this->banCnt = bans;
}
void dbHeader::importTableVer(uint8_t tVer)
{
	this->tableVer = tVer;
}
void dbHeader::importRules(string arg)
{
	if (writedRls > RULE_count - 1)
	{
		cout << "Больше правил нельзя импортировать!" << endl;
		return;
	}
	for (uint16_t a = 0;a < arg.size();++a)
	{
		rules[writedRls][a] = arg[a];
	}
	for (uint16_t a = RULE_length + 6;a > RULE_length - 1;--a)
	{
		rules[writedRls][a] = rulSepTag[a - RULE_length];
	}
	rulLeng[writedRls] = (uint16_t)arg.size();
	++writedRls;
}
void dbHeader::setActiveRuls(uint8_t aRl)
{
	this->writedRls = aRl;
}
uint8_t dbHeader::getRulesCount()
{
	return writedRls;
}
uint16_t dbHeader::getRecords()
{
	return recCnt;
}
uint16_t dbHeader::getBans()
{
	return banCnt;
}
char *dbHeader::getDBname()
{
	return dbName;
}
/*void dbHeader::displayDBHED()const
{
	cout << "БД: " << dbName << "(версия таблицы: " << to_string(tableVer) << ") Записей: " << this->recCnt << " Банов: " << this->banCnt << "\nАктивных правил: " << to_string(this->writedRls) << endl;
	dbCout << dbName;
	recOut << this->recCnt;
	banOut << this->banCnt;
	rulOut << to_string(this->writedRls);
}*/
void dbHeader::displaySome(uint8_t setFieldToDisplay)const
{
	switch (setFieldToDisplay)
		{
		case 0:
		{
			cout << "БД: " << dbName << endl;
			break;
		}
		case 1:
		{
			cout << "Записей: " << this->recCnt << endl;
			break;
		}
		case 2:
		{
			cout << "Банов: " << this->banCnt << endl;
			break;
		}
		case 3:
		{
			cout << "Активным правил: " << to_string(this->writedRls) << endl;
			break;
		}
		case 4:
		{
			cout << "Версия таблицы: " << to_string(this->tableVer) << endl;
			break;
		}
	}
}
void dbHeader::displayRule(uint8_t ruleNum)
{
	if (ruleNum > writedRls)
		return;
	if (debugMode)
		cout << "rulLeng=" << to_string(rulLeng[ruleNum]) << endl;
	for (uint16_t a = 0;a < rulLeng[ruleNum];++a)
	{
		printf("%c", rules[ruleNum][a]);
	}
	cout << endl;
}
void dbHeader::printAllRules()
{
	for (uint8_t a = 0;a < 80;++a)
		printf("-");
	cout << endl;
	cout << "Все существующие на данный момент правила: " << endl;
	for (uint8_t a = 0;a < this->writedRls;++a)
	{
		cout << "Правило " << to_string(a + 1) << endl;
		displayRule(a);
	}
	for (uint8_t a = 0;a < 80;++a)
		printf("-");
	cout << endl;
}

uint16_t summary::totalObj = 0;

int main(int argc, char *argv[])
{
	setupWindow();
	bool loaded = 0;
	int lookForId = 0;
	//int userImp;
	//char ch;
	string usrImport{ " " };
	string usrImport2{ " " };
	//COORD cPos;
	stack<bool> flagStack;
	stack<COORD> conStack;			//ЛОХ основной	
	stack<COORD> conStackReserve;	//ЛОХ резервный
	oldParser olp;

	mainEntry mEnt;
	mEnt.drawMainScreen();
	mEnt.menu();

	_getch();

	/*system("cls");
	cPos = get_position();
	conStack.push(cPos);

	while(1)
	{
		baseHead.displayDBHED();
		cPos = get_position();
		cout << "Меню:\n (N) - Новая запись\n (A) - Расширенный поиск\n (E) - Экспорт записи(таблицы)\n (O) - Показать список всех записей по VK ID\n (I) - Импортировать записи из текстовой таблицы\n (S) - Записать базу в файл\n (L) - Загрузить БД\n (F) - Найти записи в базе\n (R) - Добавить правило(вручную)\n (K) - Вывести все существующие правила\n (G) - Отобразить правило n\n (Z) - Описание команд\n (X) - Выход\nВаш выбор: ";
		cin >> ch;
		switch (ch)
		{
			case 'N':
			{
				//cout << to_string(activeRecords) << endl;
				summary::totalObj = activeRecords+1;
				dbList[activeRecords] = new summary();
				dbList[activeRecords]->newRecord();
				if (debugMode)
				{
					cout << "Проверяем..." << endl;
					dbList[activeRecords]->printVKID();
					cout << "Проверяем ID..." << endl;
					dbList[activeRecords]->getRecordId();
					dbList[activeRecords]->printRecord();
				}
				//cout << "activeRecords=" << to_string(activeRecords) << endl;
				++activeRecords;
				//cout << "activeRecords=" << to_string(activeRecords) << endl;
				_getch();
				cin.ignore();
				break;
			}
			case 'A':
			{
				if (!loaded)
				{
					cout << "Сначала надо загрузить базу для работы со строками!" << endl;
					_getch();
					break;
				}
				flagStack.push(loaded);
				loaded = 0;
				cout << "Поиск по: \n N - Имени пользователя\n F - Фамилии пользователя\n Y - Имени/Фамилии пользователя\n V - ВК ИД записи\n H - Нику пользователя\n Y - Году добавления\n M - Месяцу добавления\n D - Дню добавления\n A - Дате и времени\n X - Назад\n";
				cin >> ch;
				switch (ch)
				{
					case 'N':
					{
						cout << "Введите искомое имя: ";
						cin >> usrImport;
						//cout << "Принято: " << usrImport << endl;
						for (uint8_t a = 0;a < activeRecords;++a)
						{
							//cout << "Поиск, проход " << to_string(a) << endl;
							//cout << "input=" << usrImport << endl;
							//cout << "return=" << dbList[a]->getVKNAME() << endl;
							if (!usrImport.compare(dbList[a]->getVKNAME()))
							{
								loaded = 1;
								cout << "[";
								dbList[a]->printRECID();
								cout << "] ";
								dbList[a]->printVKID();
								cout << " | ";
								dbList[a]->printVKURL();
								cout << " | ";
								dbList[a]->printVKNAM();
								cout << " | ";
								dbList[a]->printVKFAM();
								cout << " | " << dbList[a]->getADDTIME();
								cout << endl;
							}
						}
						if (!loaded)
							cout << "Ничего не найдено" << endl;
						_getch();
						break;
					}
					case 'F':
					{
						cout << "Введите искомую фамилию: ";
						cin >> usrImport;
						for (uint8_t a = 0;a < activeRecords;++a)
						{
							//cout << "Поиск, проход " << to_string(a) << endl;
							//cout << "input=" << usrImport << endl;
							//cout << "return=" << dbList[a]->getVKNAME() << endl;
							if (!usrImport.compare(dbList[a]->getVKFAM()))
							{
								cout << "[";
								dbList[a]->printRECID();
								cout << "] ";
								dbList[a]->printVKID();
								cout << " | ";
								dbList[a]->printVKURL();
								cout << " | ";
								dbList[a]->printVKNAM();
								cout << " | ";
								dbList[a]->printVKFAM();
								cout << " | " << dbList[a]->getADDTIME();
								cout << endl;
							}
						}
						if (!loaded)
							cout << "Ничего не найдено" << endl;
						_getch();
						break;
					}
					case 'Y':
					{
						cout << "Введите искомое имя: ";
						cin >> usrImport;
						cout << "Введите искомую фамилию: ";
						cin >> usrImport2;
						for (uint8_t a = 0;a < activeRecords;++a)
						{
							if (!usrImport.compare(dbList[a]->getVKNAME()))
							{
								if (!usrImport2.compare(dbList[a]->getVKFAM()))
								{
									cout << "[";
									dbList[a]->printRECID();
									cout << "] ";
									dbList[a]->printVKID();
									cout << " | ";
									dbList[a]->printVKURL();
									cout << " | ";
									dbList[a]->printVKNAM();
									cout << " | ";
									dbList[a]->printVKFAM();
									cout << " | " << dbList[a]->getADDTIME();
									cout << endl;
								}
							}
						}
						if (!loaded)
							cout << "Ничего не найдено" << endl;
						_getch();
						break;
					}
					case 'V':
					{
						cout << "Empty entry" << endl;
						_getch();
						break;
					}
					case 'H':
					{
						cout << "Введите искомый ник: ";
						cin >> usrImport;
						for (uint8_t a = 0;a < activeRecords;++a)
						{
							if (!usrImport.compare(dbList[a]->getVKNICK()))
							{
								cout << "[";
								dbList[a]->printRECID();
								cout << "] ";
								dbList[a]->printVKID();
								cout << " | ";
								dbList[a]->printVKURL();
								cout << " | ";
								dbList[a]->printVKNAM();
								cout << " | ";
								dbList[a]->printVKFAM();
								cout << " | " << dbList[a]->getADDTIME();
								cout << endl;
							}
						}
						if (!loaded)
							cout << "Ничего не найдено" << endl;
						_getch();
						break;
					}
					case 'M':
					{
						cout << "Empty entry" << endl;
						_getch();
						break;
					}
					case 'D':
					{
						cout << "Empty entry" << endl;
						_getch();
						break;
					}
					case 'A':
					{
						cout << "Empty entry" << endl;
						_getch();
						break;
					}
					case 'X':
					{
						__asm
						{
							JMP BACKTOMENU
						}
					}
				}
				loaded = flagStack.top();
				flagStack.pop();
				_getch();
				BACKTOMENU:
				break;
			}
			case 'E':
			{
				cout << "Empty entry" << endl;
				_getch();
				break;
			}
			case 'O':
			{
				if (!loaded)
				{
					cout << "Сначала надо загрузить базу для работы со строками!" << endl;
					_getch();
					break;
				}
				//cout << "activeRecords=" << to_string(activeRecords) << endl;
				for (uint16_t a = 0;a < activeRecords;++a)
				{
					//cout << "a=" << to_string(a) << endl;
					cout << "|";
					dbList[a]->printRECID();
					cout << "|";
					dbList[a]->printVKID();
					cout << "|";
					dbList[a]->printVKURL();
					cout << "|";
					dbList[a]->printVKNAM();
					cout << "|";
					dbList[a]->printVKFAM();
					cout << "|";
					cout << endl;
				}
				_getch();
				break;
			}
			case 'I':
			{
				olp.openOldDb("samovar.txt");
				olp.importTable();
				baseHead.importDbName(dbH.dbName);
				baseHead.importStats(dbH.records,dbH.bans);
				baseHead.importTableVer(dbH.tabVer);
				cout << "Подготавливаю таблицу и начинаю импортировать " << to_string(olp.getRecords()) << " строк" << endl;
				activeRecords = olp.getRecords();
				//cout << "activeRecords=" << to_string(activeRecords) << endl;
				for (uint8_t a = 0;a < activeRecords;++a)
				{
					dbList[a] = new summary;
					olp.processImport();
					dbList[a]->importEntry();
				}
				cout << "Импортировано" << endl;
				_getch();
				break;
			}
			case 'S':
			{
				dbFile.open("tipadb.db", ios::out | ios::binary);
				//cout << "Сохраненная в структуре dbH статистика: " << "\n " << to_string(dbH.records) << " записей, " << to_string(dbH.bans) << " банов" << endl;
				//cout << "Статистика класса baseHead: " << to_string(baseHead.getRecords()) << " записей, " << to_string(baseHead.getBans()) << " банов" <<endl;
				dbH.records = activeRecords;
				dbH.bans = baseHead.getBans();
				baseHead.importStats(dbH.records, dbH.bans);
				//cout << "Сохраненная в структуре dbH статистика: " << "\n " << to_string(dbH.records) << " записей, " << to_string(dbH.bans) << " банов" << endl;
				//cout << "Статистика класса baseHead: " << to_string(baseHead.getRecords()) << " записей, " << to_string(baseHead.getBans()) << " банов" << endl;
				dbFile.seekg(0, ios::beg);
				dbFile.write(reinterpret_cast<char*>(&baseHead), sizeof(dbHeader));
				dbFile.seekg(sizeof(dbHeader), ios::beg);
				for (uint8_t a = 0; a < activeRecords; ++a)
				{
					cout << "Записываю -> VK ID:";
					dbList[a]->printVKID();
					cout << endl; 
				}
				dbFile.close();
				cout << "Записано " << to_string(baseHead.getRecords()) << " позиций" << endl;
				_getch();
				break;
			}
			case 'F':
			{
				if (!loaded)
				{
					cout << "Сначала надо загрузить базу для работы со строками!" << endl;
					_getch();
					break;
				}
				cout << "Введите ИД искомой записи: " << endl;
				cin >> lookForId;
				cout << "ID=" << lookForId << endl;

				//cout << "activeRecords=" << to_string(activeRecords) << endl;
				for (uint8_t a = 0;a < activeRecords;++a)
				{
					if (debugMode)
						cout << "Поиск, проход " << to_string(a) << endl;
					if (dbList[a]->getRecordId() == lookForId)
					{
						fileIO = *dbList[a];
						break;
					}
				}
				fileIO.printRecord();
				_getch();
				break;
			}
			case 'L':
			{
				cPos = get_position();
				conStack.push(cPos);
				if (activeRecords > 1)
					for (uint16_t a = 0;a < activeRecords;++a)
						delete dbList[a];
				cout << "Загружаю файл tipadb.db...";
				dbFile.open("tipadb.db", ios::in | ios::binary);
				dbFile.seekg(0, ios::beg);
				dbFile.read(reinterpret_cast<char*>(&baseHead), sizeof(dbHeader));

				cPos.X = 0;
				cPos.Y = 0;
				set_pos(cPos);
				baseHead.displayDBHED();
				cPos = conStack.top();
				set_pos(cPos);
				activeRecords = baseHead.getRecords();

				dbFile.seekg(sizeof(dbHeader), ios::beg);
				for (uint8_t a = 0; a < activeRecords;++a)
				{
					cPos = get_position();
					cout << "Читаю -> [" << to_string(a) << "] VK ID:";
					dbFile.seekg((sizeof(summary)*a)+ sizeof(dbHeader), ios::beg);
					dbFile.read(reinterpret_cast<char*>(&fileIO), sizeof(summary));
					dbList[a] = new summary(fileIO);
					dbList[a]->printVKID();
					cout << endl;
					set_pos(cPos);
				}
				cPos.Y += 1;
				set_pos(cPos);
				cout << "Прочитано " << to_string(baseHead.getRecords()) << " записей" << endl;
				SetConsoleTitle("[В РАЗРАБОТКЕ][CMD 1320x680] DB Manager by Anrej0705[github.com/anrej0705] v0.4[Базы загружены]");
				dbFile.close();
				_getch();
				cPos = conStack.top();
				set_pos(cPos);
				loaded = 1;
				break;
			}
			case 'R':
			{
				if (!loaded)
					cout << "Базы не загружены!" << endl;
				cin.ignore();
				cout << "Напишите текст правила, до 512 символов" << endl;
				getline(cin,usrImport);
				baseHead.importRules(usrImport);
				cout << "Импортировано: ";
				baseHead.displayRule(baseHead.getRulesCount()-1);
				_getch();
				break;
			}
			case 'K':
			{
				if (!loaded)
					cout << "Базы не загружены!" << endl;
				baseHead.printAllRules();
				_getch();
				break;
			}
			case 'G':
			{
				if (!loaded)
				{
					cout << "Сначала надо загрузить базу для работы со строками!" << endl;
					_getch();
					break;
				}
				cout << "Какой номер отобразить? (1-" << to_string(baseHead.getRulesCount()) << ")" << endl;
				cin >> userImp;
				baseHead.displayRule(userImp-1);
				_getch();
				break;
			}
			case 'Z':
			{
				system("cls");
				cout << "----------------------------( 1/11)--------------------------------" << endl;
				cout << " N - Новая запись\nВызывает мастера добавления новой записи. После завершения своей работы, запись"
					"\nудерживается в памяти приложения, чтобы сохранить её, используй опцию сохранения бд в файл(S)";
				cout << "\nФормат записи:" << endl;
				summary temp;
				temp.printRecord();
				_getch();
				system("cls");
				cout << "----------------------------( 2/11)--------------------------------" << endl;
				cout << " A - Расширенный поиск по выбранным полям. Результат поиска будет сохранён до следующей итерации"
					"\nДоступные поля:"
					"\n ________________ "
					"\n|-------VK-------|"
					"\n|Имя: 40 симв    |"
					"\n|Фамилия: 40 симв|"
					"\n|Ник: 32 симв    |"
					"\n|ID: 10 симв     |"
					"\n|________________|"
					"\n|---Дата/время---|"
					"\n|Год: 4 симв     |"
					"\n|Месяц: 2 симв   |"
					"\n|День: 2 симв    |"
					"\n|Часы: 2 симв    |"
					"\n|Минуты: 2 симв  |"
					"\n|________________|";
					if (loaded)
					{
						cout << "\nПримеры: ";
						cout << "\n--------------------------------\nПоиск по нику: " << dbList[0]->getVKNICK() << endl;
						usrImport = dbList[0]->getVKNICK();
						for (uint8_t a = 0;a < activeRecords;++a)
						{
							if (!usrImport.compare(dbList[a]->getVKNICK()))
							{
								cout << "[";
								dbList[a]->printRECID();
								cout << "] ";
								dbList[a]->printVKID();
								cout << " | ";
								dbList[a]->printVKURL();
								cout << " | ";
								dbList[a]->printVKNAM();
								cout << " | ";
								dbList[a]->printVKFAM();
								cout << " | " << dbList[a]->getADDTIME();
								cout << endl;
							}
						}
						cout << "\n--------------------------------\nПоиск по имени: " << dbList[0]->getVKNAME() << endl;
						usrImport = dbList[0]->getVKNAME();
						for (uint8_t a = 0;a < activeRecords;++a)
						{
							if (!usrImport.compare(dbList[a]->getVKNAME()))
							{
								cout << "[";
								dbList[a]->printRECID();
								cout << "] ";
								dbList[a]->printVKID();
								cout << " | ";
								dbList[a]->printVKURL();
								cout << " | ";
								dbList[a]->printVKNAM();
								cout << " | ";
								dbList[a]->printVKFAM();
								cout << " | " << dbList[a]->getADDTIME();
								cout << endl;
							}
						}
						if (activeRecords < 74)
						{
							cout << "\n--------------------------------\nПоиск по фамилии: " << dbList[0]->getVKFAM() << endl;
							usrImport = dbList[0]->getVKFAM();
							for (uint8_t a = 0;a < activeRecords;++a)
							{
								if (!usrImport.compare(dbList[a]->getVKNAME()))
								{
									cout << "[";
									dbList[a]->printRECID();
									cout << "] ";
									dbList[a]->printVKID();
									cout << " | ";
									dbList[a]->printVKURL();
									cout << " | ";
									dbList[a]->printVKNAM();
									cout << " | ";
									dbList[a]->printVKFAM();
									cout << " | " << dbList[a]->getADDTIME();
									cout << endl;
								}
							}
						}
						cout << "\n--------------------------------\nПоиск по фамилии: " << dbList[73]->getVKFAM() << endl;
						usrImport = dbList[73]->getVKFAM();
						for (uint8_t a = 0;a < activeRecords;++a)
						{
							if (!usrImport.compare(dbList[a]->getVKFAM()))
							{
								cout << "[";
								dbList[a]->printRECID();
								cout << "] ";
								dbList[a]->printVKID();
								cout << " | ";
								dbList[a]->printVKURL();
								cout << " | ";
								dbList[a]->printVKNAM();
								cout << " | ";
								dbList[a]->printVKFAM();
								cout << " | " << dbList[a]->getADDTIME();
								cout << endl;
							}
						}

					}
				_getch();
				system("cls");
				cout << "----------------------------( 3/11)--------------------------------" << endl;
				cout << "empty entry" << endl; //заглушка
				cout << "empty entry" << endl; //заглушка
				cout << "empty entry" << endl; //заглушка
				cout << " E - экспортирует выбранную запись или таблицу, или результат поиска в форматы:"
					"\nновой базы данных(при этом сохраняются записанные правила"
					"\nтекстовую версию базы данных"
					"\nтекстовую версию для чтения человеком";
				cout << "\nПримеры: " << endl;
				cout << "empty entry" << endl; //заглушка
				_getch();
				system("cls");
				cout << "----------------------------( 4/11)--------------------------------" << endl;
				cout << " O - отображает список всех имеющихся записей с разделением на страницы по 128 записей";
				if(activeRecords>25)
					cout << "\nПример(первые 25 записей)" << endl;
				if(activeRecords<25)
					cout << "\nПример(первые " << to_string(activeRecords) << " записей)" << endl;
				for (uint8_t a = 0;a < activeRecords;++a)
				{
					if (a == 25)
						break;
					cout << "|";
					dbList[a]->printRECID();
					cout << "|";
					dbList[a]->printVKID();
					cout << "|";
					dbList[a]->printVKURL();
					cout << "|";
					dbList[a]->printVKNAM();
					cout << "|";
					dbList[a]->printVKFAM();
					cout << "|";
					cout << endl;
				}
				_getch();
				system("cls");
				cout << "----------------------------( 5/11)--------------------------------" << endl;
				cout << " I - Импортирует текстовый вариант таблицы" << endl;
				cout << "Примеры: " << endl;
				cout << "empty entry" << endl; //заглушка
				_getch();
				system("cls");
				cout << "----------------------------( 6/11)--------------------------------" << endl;
				cout << " S - Запись таблицы, находящейся в памяти приложения в файл. Предыдущая таблица не сохраняется" << endl;
				cout << "----------------------------( 7/11)--------------------------------" << endl;
				cout << " L - Загружает таблицу из файла *.db. Таблица, удерживаемая в памяти перезаписывается новой" << endl;
				cout << "----------------------------( 8/11)--------------------------------" << endl;
				if (!loaded)
					cout << " F - Поиск записи по ИД в базе" << endl;
				if (loaded)
				{
					if (activeRecords < 32)
					{
						cout << " F - Поиск записи по ИД в базе\nПример: \nВведите ИД искомой записи: 0\n";
						dbList[0]->printRecord();
					}
					if (activeRecords > 32)
					{
						cout << " F - Поиск записи по ИД в базе\nПример: \nВведите ИД искомой записи: 32\n";
						dbList[32]->printRecord();
					}
				}
				_getch();
				system("cls");
				cout << "----------------------------( 9/11)--------------------------------" << endl;
				cout << " R - Вызывает форму, в которую необходимо вписать требуемое правило. Длина правила не может"
					"\nбыть больше 512 символов и количество правил не больше 8\nПример:\nНапишите текст правила, до 512 символов"
					"\n: Запрещён спам.\nИмпортировано: Запрещён спам." << endl;
				cout << "----------------------------(10/11)--------------------------------" << endl;
				cout << " K - Выводит на экран все существующие на данные момент правила\nПример:"
					"\n--------------------------------------------------------------------------------"
					"\nВсе существующие на данный момент правила:"
					"\nПравило 1"
					"\nЗапрещён спам"
					"\nПравило 2"
					"\nЗапрещено ссорить Россию и Украину и оскорблять одну из стран. Не будьте жертвой пропаганды."
					"\n" << endl;
				cout << "----------------------------(11/11)--------------------------------" << endl;
				cout << " G - отображает выбранное по номеру правило\nПример:"
					"\nКакой номер отобразить? (1-7)"
					"\n5"
					"\nЗапрещено личное оскорбление одного из участников беседы." << endl;
				_getch();
				_getch();
				break;
			}
			case 'X':
			{
				return 0;
			}
		}
		set_pos(cPos);
		system("cls");
	}

	olp.closeOldDb();
	_getch();
	return 0;*/
}
