#pragma once
#pragma message("menu.h included")
#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0501
#ifndef _MENU_H_
#define _MENU_H_
#include <string>

#include "bridge.h"

using namespace std;

class dbLoadScreen;
class mainEntry;

extern string *currentLocale;
extern uint8_t appLNG;
extern bool openError;
extern bool localeLoaded;

enum logSettings
{
	NUL		= 0x00,
	INFO	= 0x01,
	WARN	= 0x02,
	FATAL	= 0x03
};

struct mainScreenGraphConfig
{
	COORD dbSummaryStart = { 1,1 };
	COORD dbSummarySize = { 130,3 };
	COORD dbRecordsStart = { 130,1 };
	COORD dbRecordsEnd = { 15,3 };
	COORD dbBansStart = { 144,1 };
	COORD dbBansEnd = { 15,3 };
	COORD rulesCntrStart = { 1,5 };
	COORD rulesCntrSize = { 21,3 };
	COORD menuBtnStart = { 109,5 };
	COORD menuBtnSize = { 10,3 };
	COORD serviceBtnStart = { 70,5 };
	COORD serviceBtnSize = { 20,3 };
	COORD fastAddBtnStart = { 121,5 };
	COORD fastAddBtnSize = { 18,3 };
	COORD fastEditBtnStart = { 141,5 };
	COORD fastEditBtnSize = { 18,3 };
	COORD dbTableGraphicStart = { 1,9 };
	COORD dbTableGraphicSize = { 158,33 };
	COORD dbConsoleStart = { 1,43 };
	COORD dbConsoleSize = { 158,9 };
	COORD dbTableStart = { 0,0 };
	COORD dbTableEnd = { 0,0 };
};
struct addFormGraphConfig
{
	COORD frameStart{ 40,10 };
	COORD frameSize{ 46,29 };
	COORD vkidFieldFStart{ frameStart.X + 2,frameStart.Y + 2 };
	COORD vkidFieldSize{ 18,3 };
	COORD vkurlFieldFStart{ frameStart.X + 21,frameStart.Y + 2 };
	COORD vkurlFieldSize{ 23,3 };
	COORD vknameFieldFStart{ frameStart.X + 2, frameStart.Y + 6 };
	COORD vknameFieldSize{ 42,3 };
	COORD vkfamFieldFStart{ frameStart.X + 2, frameStart.Y + 10 };
	COORD vkfamFieldSize{ 42,3 };
	COORD vkRlsListFStart{ frameStart.X + 2, frameStart.Y + 14 };
	COORD vkRlsListdSize{ 42,4 };
	COORD vkRemFStart{ frameStart.X + 2, frameStart.Y + 19 };
	COORD vkRemSize{ 42,4 };
	COORD btnSubmitStart{ frameStart.X + 10, frameStart.Y + 24 };
	COORD btnSubmitSize{ 25,3 };
	CMDcolor BackColor = GREEN;
	CMDcolor TextColor = BLACK;
	CMDcolor FieldColor = LIGHT_GREEN;
	CMDcolor FieldText = BLACK;
	CMDcolor btnBackColor = LIGHT_YELLOW;
	CMDcolor btnTextColor = BLACK;
	uint8_t vkidFieldShift = 7;
	uint8_t vkurlFieldShift = 8;
	uint8_t vknameFieldShift = 8;
	uint8_t vkfamFieldShift = 10;
	char itemName[15] = "Add_form_frame";
	char vkidFieldText[7] = "VK ИД:";
	char vkurlFieldText[8] = "VK URL:";
	char vknameFielText[8] = "VK имя:";
	char vkfamFieldText[10] = "VK фам-я:";
	char vkviolsFieldText[40] = "1[ ] 2[ ] 3[ ] 4[ ] 5[ ] 6[ ] 7[ ] 8[ ]";
	char vkRulesHint[42] = "ЛКМ по цифре правила чтобы показать его";
	char vkRemHeaderText[22] = "Примечание к записи";
	char arrRecBtn[16] = "Добавить запись";
};
struct mainMenuGraphConfig
{
	COORD frameStart{ 40,10 };
	COORD frameSize{ 80,30 };
	CMDcolor BackColor=YELLOW;
	CMDcolor TextColor = BLACK;
};
struct bdLoadScreenGraphConfig
{
	COORD frameStart{ 49,15 };
	COORD frameSize{ 62,10 };
};
struct consoleDumpDone
{
	COORD frameStart{ 65,20 };
	COORD frameEnd{ 30,7 };
	COORD okBtnStart{ 75,23 };
	COORD okBtnSize{ 10,1 };
	CMDcolor BackColor = RED;
	CMDcolor TextColor = WHITE;
	CMDcolor okBtnBack = CYAN;
	CMDcolor okBtnText = WHITE;
	char doneText[7] = "ГОТОВО";
	char okBtn[5] = " OK ";
};
struct serviceMenuGraphConfig
{
	COORD frameStart{ 60,13 };
	COORD frameSize{ 40,20 };
	COORD mouseTestBtnStart{ 62,15 };
	COORD mouseTestBtnSize{ 36,3 };
	COORD consoleDumpBtnStart{ 62,19 };
	COORD consoleDumpBtnSize{ 36,3 };
	COORD exitBtnStart{ 62,23 };
	COORD exitBtnSize{ 36,3 };
	COORD resetMouseBufBtnStart{ 62,27 };
	COORD resetMouseBufBtnSize{ 36,3 };
	CMDcolor BackColor = LIGHT_BLUE;
	CMDcolor TextColor = WHITE;
	CMDcolor BtnBackColor = LIGHT_MAGENTA;
	CMDcolor BtnTextColor = BLACK;
	char servMenuHeader[15] = "СЕРВИСНОЕ МЕНЮ";
	char mouseTestBtnName[14] = "ПРОВЕРКА МЫШИ";
	char consoleDumpBtnName[13] = "ДАМП КОНСОЛИ";
	char exitBtnName[18] = "ЗАКРЫТЬ ПРОГРАММУ";
	char resetMouseBufBtnName[21] = "ОЧИСТИТЬ БУФЕР ВВОДА";
};
struct serviceMouseTest
{
	COORD frameStart{ 20,3 };
	COORD frameSize{ 120,45 };
	CMDcolor BackColor = LIGHT_BLUE;
	CMDcolor TextColor = BLACK;
	char frameHeader[21] = "Проверка работы мыши";
};

class newRecordForm
{
	private:
		addFormGraphConfig nForm;
		frameBuilder nFrame;
		shadowCache sFrame;
	public:
		newRecordForm();
		void show();
		void hide();
};
class serviceMouseTestPad
{
	private:
		uint16_t pixelsCtr;
		serviceMouseTest serM;
		shadowCache sermSC;
	public:
		frameBuilder sMouse;
		serviceMouseTestPad();
		void show();
		void hide();
		void clearMouseBuffer();
};
class serviceMenu
{
	private:
		serviceMenuGraphConfig servCfg;
		serviceMouseTestPad serv_MTP;
		shadowCache servSC;
	public:
		frameBuilder servMen;
		serviceMenu();
		void show();
		void hide();
};
class mainMenu
{
	private:
		mainMenuGraphConfig mainMu;
		shadowCache mmuSC;
	public:
		frameBuilder mmenu;
		mainMenu();
		void show();
		void hide();
};
class dbLoadScreen
{
	public:
		bdLoadScreenGraphConfig dbFrame;
		frameBuilder dbLoadFrame;
		shadowCache dbLFcache;
		dbLoadScreen();
};
class mainEntry
{
	private:
		mainScreenGraphConfig mainCfg;
		shadowCache mainShadowCache;
		dbLoadScreen dbLoadScr;
		mainMenu mmu;
		serviceMenu servMen;
		newRecordForm nForm;
	public:
		//Контейнеры графики
		frameBuilder mainFrame;
		mainEntry();
		void drawMainScreen();
		void hideMainScreen();
		void renderLog();
		void printRecTable();
		void menu();
		void loadDb();
};

uint16_t getSysLocale();
void putLog(string logInput, logSettings logSet);
void consoleDump();
void loadLocale();
void getLocaleList();

#endif
