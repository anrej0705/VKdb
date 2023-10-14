#pragma once
#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0501
#include <SDKDDKVer.h>

#include <fstream>
#include "gui.h"
#include "dbrecord.h"
#include "rulbits.h"
#include "vkmeta.h"
#include "dtime.h"
#include "rem.h"
#include "flags.h"
#include "cpp.h"

#define DBNAME_length	80		//Байтовая длина поля имени базы данных
#define DB_maxrecrods	512		//максимальное количество записей в бд, переменное колво
#define RULE_length		512		//Байтовая длина текстового описания правила
#define RULE_count		8		//Количество правил, максимальное
#define DT_UInputLng	17		//Байтовая длина пользовательского ввода
#define debugMode		0		//Режим отладки
#define textTableFMT	7		//Формат текстовой таблицы

struct parserParams	//Для парсера
{
	uint16_t recId;
	string vkid;
	string vkurl;
	string vknam;
	string vkfam;
	string dateAdd;
	string dateEnd;
	string dateBan;
	bool active;
	bool expired;
	bool ban;
	string rul;
	string rulBan;
	string remark;
	bool next;
};
struct dbHead	//Заголов базы
{
	uint8_t tabVer;
	char dbName[DBNAME_length];
	uint16_t records;
	uint16_t bans;
	uint8_t rules;
};
struct dbOut
{
	char recId[4];
	char vkID[VKID_length];
	char vkurl[VKURL_length];
	char recAdd[DT_UInputLng];
	char viols[DT_UInputLng];
	char recExp[DT_UInputLng];
	char status[DT_UInputLng];
	char remark[REM_length];
};

extern dbOut dbO;
extern dbHead dbH;
extern parserParams Parp;

template <class Arr>
void arrcpy(Arr *destination, Arr *source)	//arrcpy = strcpy(аналог не вызывающий ругани у вижлы)
{
	uint16_t srcSize = 0;
	uint16_t desSize = 0;
	while (source[srcSize - 1] != NULL)
		++srcSize;
	while (source[desSize - 1] != NULL)
		++desSize;
	if (!(srcSize == desSize))
		throw errorReport(0x05);
	while (desSize != 0)
	{
		destination[desSize - 1] = source[desSize - 1];
		--desSize;
	}
}
template <class Arr>
void arrcpyw(Arr *destination, Arr *source, uint32_t start, uint32_t length)	//Расширенная версия arrcpy с возможностью копирования заданной области
{
	uint32_t steps = length;
	while (steps != 0)
	{
		--steps;
		//cout << "shift=" << shiftsteps << " steps=" << steps;
		destination[start + steps] = source[steps];
		//cout << " destination=" << destination[shiftsteps + steps] << " source=" << source[steps] << endl;
	}
}

class errorReport
{
	public:
		errorReport();
		errorReport(uint8_t reasonCode);
};


class dbHeader
{
	private:
		char dbName[DBNAME_length];
		char rules[RULE_count][RULE_length + 6];
		char fulcTag[7];	//FULCRUM TAG
		uint8_t testByte;
		uint8_t checkByte;
		char rulSepTag[6];	//SHDHRT TAG
		uint8_t CRCPolynome;
		uint16_t recCnt;
		uint16_t banCnt;
		uint16_t rulLeng[8];
		uint8_t writedRls;
		uint8_t tableVer;
	public:
		dbHeader();
		void importOLP();
		void importDbName(string dbNam);
		void importStats(uint16_t recs, uint16_t bans);
		void importTableVer(uint8_t tVer);
		void importRules(string arg);
		void setActiveRuls(uint8_t aRl);
		uint8_t getRulesCount();
		uint16_t getRecords();
		uint16_t getBans();
		char *getDBname();
		//void displayDBHED()const;
		void displaySome(uint8_t setFieldToDisplay)const;
		void displayRule(uint8_t ruleNum);
		void printAllRules();
};
class oldParser	//парсер псевдотаблиц(устаревшее!)
{
	private:
		string input;
		string found;
		fstream tabFile;
		uint16_t tabRecords;
		uint16_t importPos;
	public:
		oldParser();
		void openOldDb(string fileName);
		void importTable();
		void processImport();
		uint16_t getImportPos();
		uint16_t getRecords();
		void closeOldDb();
};
class summary : public dbRecord, public vkMeta, public dTime, public flags, public rulBits, public rem
{
private:
	uint8_t RecTAG[5];
	bool completeRecord;	//выставляется 1 когда все ячейки таблицы записаны
public:
	static uint16_t totalObj;
	summary();
	~summary();
	void newRecord();
	void printRecord();
	void importEntry();
	void showRECID();
	static void showObjs();
	void printRECID()const;
	void printVKID()const;
	void printVKURL()const;
	void printVKNAM()const;
	void printVKFAM()const;
	uint16_t getRECID();
	string getVKID();
	string getVKNICK();
	string getVKNAME();
	string getVKFAM();
};
