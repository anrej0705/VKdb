#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0501
#include <SDKDDKVer.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <string>
#include "bridge.h"
#include "menu.h"
#include "winnls.h"
#include "cpp.h"
#include <boost/date_time/posix_time/posix_time.hpp>

enum localeCodes {
	RU_ru=0x0419,
	EN_us=0x0409,
	UA_uk=0x0422,
	DE_gr=0x0407,
	FR_fr=0x040C
};
enum localeNames {
	ruLoc=0x00,
	usLoc=0x01,
	ukLoc=0x02,
	grLoc=0x03,
	frLoc=0x04
};
char *dTableStrokeName[40] =	//Наименование строк таблицы
{
	"Строка 1", "Строка 2", "Строка 3", "Строка 4", "Строка 5", "Строка 6", "Строка 7", "Строка 8", "Строка 9", "Строка 10",
	"Строка 11", "Строка 12", "Строка 13", "Строка 14", "Строка 15", "Строка 16", "Строка 17", "Строка 18", "Строка 19", "Строка 20",
	"Строка 21", "Строка 22", "Строка 23", "Строка 24", "Строка 25", "Строка 26", "Строка 27", "Строка 28", "Строка 29", "Строка 30"
};
char *dConsoleStrokeName[20] =	//Наименование строк консоли
{
	"КонсольСтрока 1", "КонсольСтрока 2", "КонсольСтрока 3", "КонсольСтрока 4", "КонсольСтрока 5",
	"КонсольСтрока 6", "КонсольСтрока 7", "КонсольСтрока 8", "КонсольСтрока 9", "КонсольСтрока 10"
};

string *locFileNames;
string localeTag = "$LOCALE";
string readLine{ " " };
uint16_t *locCodes;
string *currentLocale;
string sCOUT;
stringstream cOutLoadDb;
uint16_t activeRecords = 0;

stringstream dbCout;
stringstream rulOut;
stringstream recOut;
stringstream banOut;

const int16_t btnCloseX = 3;
const int16_t btnFullX = 5;
const int16_t btnMinizeX = 7;

bool serviceMenu = 1;

uint8_t appLNG;
uint8_t localesCntr = 0;
bool openError=0;
bool localeLoaded = 0;
using namespace std;
ifstream locFile;
ifstream locConfig;
ifstream dbFile;

dbOut dbO;
dbHead dbH;

char dbTemplate[256] = "БД: ";
char rulTemplat[32] = "Активных правил: ";
char recTemplate[32] = "ЗАПИСЕЙ: ";
char banTemplate[32] = "БАНОВ: ";
char dbCons[128];
char rulCons[32];
char recCons[32];
char banCons[32];

list<string> consoleBuffer;
dbHeader baseHead;
summary fileIO;
summary *dbList[512];	//список на сохранение в файл

uint16_t getSysLocale()
{
	//Контейнер локали системы
	uint16_t locId;

	//Костыльный лайфхак на красивый вывод
	string logPrint="Локаль системы: 0x";

	//Создаём эмулятор потока
	stringstream hexId;
	locId = GetUserDefaultUILanguage();

	//Выводим в него содержимое, только что полученное locId в формате 0xABCD 
	hexId << setw(4) << setfill('0') << uppercase << std::hex << locId;

	//Приклеиваем содержимое эмулятора потока
	logPrint = logPrint + hexId.str() + "\n";

	//Кидаем в логи
	putLog(logPrint, INFO);

	//Возвращаем ИД локали системы
	return locId;
}
void getLocaleList()
{
	//Счетчик количества вписанных локалей
	uint8_t localesCnt = 0;
	uint16_t tempLC=0;

	//Контейнер логов
	string logOut;
	stringstream coutLog;

	//Открываем файл с информацией о локалях
	locConfig.open("locales.ini");

	//Если файл отсутствует то сообщаем об этом
	//И сообщаем о языке по умолчанию
	if (!locConfig.is_open())
	{
		cout << cpTranslate("Не найден список локалей locales.ini. Установлена локаль по умолчанию: Русский", 1251) << endl;
	}
	else
	{
		//Отправляем в лог инфу об открытии
		logOut = "Файл locales.ini найден и открыт. ";
	}

	//Подсчитываем количество локалей по тегу $LOCALE
	while (getline(locConfig, readLine))
		if (readLine == localeTag)
			++localesCnt;

	//Сообщаем о количестве локалей
	coutLog << "Найдено: " << to_string(localesCnt) << " локализаций" << endl;
	logOut = logOut + coutLog.str();
	localesCntr = localesCnt;

	putLog(logOut, INFO);

	//Закрываем файл
	locConfig.close();

	//Выделяем память и сбрасываем счетчик
	locCodes = new uint16_t[localesCnt];
	locFileNames = new string[localesCnt];
	localesCnt = 0;

	//Открываем снова
	locConfig.open("locales.ini");

	//Читаем в массив
	while (getline(locConfig, readLine))
	{
		if (readLine == localeTag)
		{
			//Читаем тег $LOCALE из файда
			getline(locConfig, readLine);

			//Читаем ИД локали из конфиг файла
			getline(locConfig, readLine);

			//Преобразуем записанное текстом HEX в бинарный формат
			tempLC = (uint16_t)stoul(readLine, NULL, 16);
			locCodes[localesCnt] = tempLC;

			//Пишем в массив имя файла локали
			getline(locConfig, readLine);
			locFileNames[localesCnt] = readLine;

			//Прибавляем счетчик локалей
			++localesCnt;
		}
	}
}
void loadLocale()
{
	//Количество строчек локали
	uint8_t locStrokes = 0;

	//Получаем список локалей из файла locales.ini
	getLocaleList();

	//Ищем локаль ОС в списке известных локализацтй
	for (uint8_t a = 0;a < localesCntr;++a)
	{
		if (locCodes[a] == getSysLocale())
		{
			//В случае обнаружения записываем ИД локали и выходим из цикла досрочно
			appLNG = a;
			break;
		}
	}

	//Пытаемся найти и открыть файл с локализацией
	locFile.open(locFileNames[appLNG]);
	if (!locFile.is_open())
	{
		cout << cpTranslate("Не найден файл локализации ", 1251) << locFileNames[appLNG] << cpTranslate(". Установлен язык по умолчанию: Русский", 1251) << endl;
	}

	//Подсчитываем количество строчек
	while (getline(locFile, readLine))
	{
		++locStrokes;
		//cout << cpTranslate(const_cast<char*>(readLine.c_str()), 1251) << endl;
	}
	//cout << cpTranslate("Строчек в локали: ", 1251) << to_string(locStrokes) << endl;

	//Выделяем память в соответствии с количеством строчек в локали
	currentLocale = new string[locStrokes];

	//Я хуй знает почему но без этого getline не работает
	locFile.close();
	locFile.open(locFileNames[appLNG]);
	locStrokes = 0;

	//Перегоняем из файла в память
	while (getline(locFile, readLine))
	{
		currentLocale[locStrokes] = readLine;
		++locStrokes;
	}

	/*for(uint8_t a=0;a<locStrokes;++a)
		cout << cpTranslate(const_cast<char*>(currentLocale[a].c_str()), 1251) << endl;*/
	string localOut(currentLocale[3]);
	localOut = localOut + "\n";
	putLog(localOut, INFO);
	//cout << cpTranslate(const_cast<char*>(currentLocale[3].c_str()),1251) << endl;
	localeLoaded = 1;
}

newRecordForm::newRecordForm()
{
	sFrame.setCacheCoord(nForm.frameStart, { nForm.frameSize.X + 1,nForm.frameSize.Y + 1 });
	nFrame.addItem(ITEM_FRAME, nForm.BackColor, nForm.TextColor, nForm.frameStart, nForm.frameSize, ITEM_NAME, nForm.itemName, DOUBLE_LINE, F_HEADER_ENABLE, F_CONTROLS_ENABLE, HEADER_TEXT, "Новая запись");
	nFrame.addItem(ITEM_FRAME, nForm.FieldColor, nForm.FieldText, nForm.vkidFieldFStart, nForm.vkidFieldSize, ITEM_NAME, "VKID_field");
	nFrame.addItem(ITEM_TEXT, nForm.FieldColor, nForm.FieldText, nFrame.getTextPos(nFrame.searchByName("VKID_field")), nFrame.getTextSize(nFrame.searchByName("VKID_field")), BODY_TEXT, nForm.vkidFieldText);
	nFrame.addItem(ITEM_LINE, nForm.BackColor, nForm.TextColor, { nFrame.getTextPos(nFrame.searchByName("VKID_field")).X + nForm.vkidFieldShift-1,nFrame.getTextPos(nFrame.searchByName("VKID_field")).Y }, { nFrame.getTextPos(nFrame.searchByName("VKID_field")).X + nFrame.getTextSize(nFrame.searchByName("VKID_field")).X-2,nFrame.getTextPos(nFrame.searchByName("VKID_field")).Y });
	nFrame.addItem(ITEM_FRAME, nForm.FieldColor, nForm.FieldText, nForm.vkurlFieldFStart, nForm.vkurlFieldSize, ITEM_NAME, "VKURL_field");
	nFrame.addItem(ITEM_TEXT, nForm.FieldColor, nForm.FieldText, nFrame.getTextPos(nFrame.searchByName("VKURL_field")), nFrame.getTextSize(nFrame.searchByName("VKURL_field")), BODY_TEXT, nForm.vkurlFieldText);
	nFrame.addItem(ITEM_LINE, nForm.BackColor, nForm.TextColor, { nFrame.getTextPos(nFrame.searchByName("VKURL_field")).X + nForm.vkurlFieldShift-1,nFrame.getTextPos(nFrame.searchByName("VKURL_field")).Y }, { nFrame.getTextPos(nFrame.searchByName("VKURL_field")).X + nFrame.getTextSize(nFrame.searchByName("VKURL_field")).X - 2,nFrame.getTextPos(nFrame.searchByName("VKURL_field")).Y });
	nFrame.addItem(ITEM_FRAME, nForm.FieldColor, nForm.FieldText, nForm.vknameFieldFStart, nForm.vknameFieldSize, ITEM_NAME, "VKNAME_field");
	nFrame.addItem(ITEM_TEXT, nForm.FieldColor, nForm.FieldText, nFrame.getTextPos(nFrame.searchByName("VKNAME_field")), nFrame.getTextSize(nFrame.searchByName("VKNAME_field")), BODY_TEXT, nForm.vknameFielText);
	nFrame.addItem(ITEM_LINE, nForm.BackColor, nForm.TextColor, { nFrame.getTextPos(nFrame.searchByName("VKNAME_field")).X + nForm.vknameFieldShift-1,nFrame.getTextPos(nFrame.searchByName("VKNAME_field")).Y }, { nFrame.getTextPos(nFrame.searchByName("VKNAME_field")).X + nFrame.getTextSize(nFrame.searchByName("VKNAME_field")).X - 2,nFrame.getTextPos(nFrame.searchByName("VKNAME_field")).Y });
	nFrame.addItem(ITEM_FRAME, nForm.FieldColor, nForm.FieldText, nForm.vkfamFieldFStart, nForm.vkfamFieldSize, ITEM_NAME, "VKFAM_field");
	nFrame.addItem(ITEM_TEXT, nForm.FieldColor, nForm.FieldText, nFrame.getTextPos(nFrame.searchByName("VKFAM_field")), nFrame.getTextSize(nFrame.searchByName("VKFAM_field")), BODY_TEXT, nForm.vkfamFieldText);
	nFrame.addItem(ITEM_LINE, nForm.BackColor, nForm.TextColor, { nFrame.getTextPos(nFrame.searchByName("VKFAM_field")).X + nForm.vkfamFieldShift-1,nFrame.getTextPos(nFrame.searchByName("VKFAM_field")).Y }, { nFrame.getTextPos(nFrame.searchByName("VKFAM_field")).X + nFrame.getTextSize(nFrame.searchByName("VKFAM_field")).X - 2,nFrame.getTextPos(nFrame.searchByName("VKFAM_field")).Y });
	nFrame.addItem(ITEM_FRAME, nForm.FieldColor, nForm.FieldText, nForm.vkRlsListFStart, nForm.vkRlsListdSize, ITEM_NAME, "VKRLS_list");
	nFrame.addItem(ITEM_TEXT, nForm.FieldColor, nForm.FieldText, nFrame.getTextPos(nFrame.searchByName("VKRLS_list")), nFrame.getTextSize(nFrame.searchByName("VKRLS_list")), BODY_TEXT, nForm.vkviolsFieldText);
	nFrame.addItem(ITEM_TEXT, nForm.FieldColor, nForm.FieldText, { nFrame.getTextPos(nFrame.searchByName("VKRLS_list")).X,nFrame.getTextPos(nFrame.searchByName("VKRLS_list")).Y +1}, { nFrame.getTextSize(nFrame.searchByName("VKRLS_list")).X,nFrame.getTextSize(nFrame.searchByName("VKRLS_list")).Y +1}, BODY_TEXT, nForm.vkRulesHint);
	nFrame.addItem(ITEM_FRAME, nForm.FieldColor, nForm.FieldText, nForm.vkRemFStart, nForm.vkRemSize, ITEM_NAME, "VKREM", F_CONTROLS_DISABLE, F_HEADER_ENABLE, HEADER_TEXT, nForm.vkRemHeaderText);
	nFrame.addItem(ITEM_LINE, nForm.BackColor, nForm.TextColor, { nFrame.getTextPos(nFrame.searchByName("VKREM")).X,nFrame.getTextPos(nFrame.searchByName("VKREM")).Y }, { nFrame.getTextPos(nFrame.searchByName("VKREM")).X + nFrame.getTextSize(nFrame.searchByName("VKREM")).X - 2,nFrame.getTextPos(nFrame.searchByName("VKREM")).Y });
	nFrame.addItem(ITEM_LINE, nForm.BackColor, nForm.TextColor, { nFrame.getTextPos(nFrame.searchByName("VKREM")).X,nFrame.getTextPos(nFrame.searchByName("VKREM")).Y+1 }, { nFrame.getTextPos(nFrame.searchByName("VKREM")).X + nFrame.getTextSize(nFrame.searchByName("VKREM")).X - 2,nFrame.getTextPos(nFrame.searchByName("VKREM")).Y+1 });
	nFrame.addItem(ITEM_FRAME, nForm.btnBackColor, nForm.btnTextColor, nForm.btnSubmitStart, nForm.btnSubmitSize, ITEM_NAME, "Add_form_submit_btn");
	nFrame.addItem(ITEM_TEXT, nForm.btnBackColor, nForm.btnTextColor, nFrame.getTextPos(nFrame.searchByName("Add_form_submit_btn")), nFrame.getTextSize(nFrame.searchByName("Add_form_submit_btn")), BODY_TEXT, nForm.arrRecBtn, ALIGN_CENTER);
	nFrame.heapSort();
}
void newRecordForm::show()
{
	uint8_t symCnt = 0;
	COORD userInputCoord;
	sFrame.scan();
	nFrame.render(DRAW_LOCAL, 1);
	while (1)
	{
		userInputCoord = get_mouseEventCoord();
		if (userInputCoord.X == nForm.frameStart.X + nForm.frameSize.X - btnCloseX)
			if (userInputCoord.Y = nForm.frameStart.Y)
			{
				this->hide();
				return;
			}
		while (symCnt < 10)
		{
			++symCnt;
		}
	}
}
void newRecordForm::hide()
{
	sFrame.hide();
}

serviceMouseTestPad::serviceMouseTestPad()
{
	pixelsCtr = 0;
	sermSC.setCacheCoord(serM.frameStart, { serM.frameSize.X + 1 , serM.frameSize.Y + 1 });
	sMouse.addItem(ITEM_FRAME, serM.BackColor, serM.TextColor, serM.frameStart, serM.frameSize, ITEM_NAME, "Service_mouse_test_frame", F_CONNECT_SIMPLE, F_CONTROLS_ENABLE, F_SHADOW_ENABLE, DOUBLE_LINE, F_HEADER_ENABLE, HEADER_TEXT, serM.frameHeader);
	sMouse.heapSort();
}
void serviceMouseTestPad::show()
{
	sermSC.scan();
	sMouse.render(DRAW_LOCAL, 1);
	uint8_t input[2] = { 0x9A,0x00 };
	COORD printCoord;
	int16_t xCoord = 0;
	int16_t yCoord = 0;
	while (1)
	{
		printCoord = get_mouseEventCoord();
		xCoord = printCoord.X;
		yCoord = printCoord.Y;
		++pixelsCtr;
		sMouse.addItem(ITEM_TEXT, YELLOW, BLACK, sMouse.getTextPos(sMouse.searchByName("Service_mouse_test_frame")), serM.frameSize, CLEAR_LINE_DISABLE, COORD_ENABLE, xCoord, yCoord, BODY_TEXT, input);
		sMouse.render(DRAW_EDITED);
		if (yCoord == serM.frameStart.Y)
			if (xCoord == serM.frameSize.X+serM.frameStart.X - btnCloseX)
				break;
		//Sleep(50);
	}
	this->hide();
}
void serviceMouseTestPad::hide()
{
	sermSC.hide();
}
void serviceMouseTestPad::clearMouseBuffer()
{
	COORD userInputCoord;
	consoleDumpDone cdd;
	frameBuilder dumpMsg;
	shadowCache cddSC;
	for(;pixelsCtr>0;--pixelsCtr)
		sMouse.remove_item_back();
	cddSC.setCacheCoord(cdd.frameStart, { cdd.frameEnd.X + 1,cdd.frameEnd.Y + 1 });
	dumpMsg.addItem(ITEM_FRAME, cdd.BackColor, cdd.TextColor, cdd.frameStart, cdd.frameEnd, ITEM_NAME, "Done_msg", F_HEADER_DISABLE, F_CONTROLS_DISABLE, F_SHADOW_ENABLE);
	dumpMsg.addItem(ITEM_TEXT, cdd.okBtnBack, cdd.okBtnText, cdd.frameStart, cdd.frameEnd, BODY_TEXT, cdd.doneText, COORD_ENABLE, (cdd.frameStart.X + cdd.frameEnd.X / 2) - 3, cdd.frameStart.Y + cdd.frameEnd.Y - 5);
	dumpMsg.addItem(ITEM_TEXT, cdd.okBtnBack, cdd.okBtnText, dumpMsg.getTextPos(dumpMsg.searchByName("Done_msg")), dumpMsg.getTextSize(dumpMsg.searchByName("Done_msg")), ALIGN_CENTER, BODY_TEXT, cdd.okBtn, COORD_ENABLE, (cdd.frameStart.X + cdd.frameEnd.X / 2) - 2, cdd.frameStart.Y + cdd.frameEnd.Y - 1);
	cddSC.scan();
	dumpMsg.render(DRAW_LOCAL, 1);
	while (1)
	{
		userInputCoord = get_mouseEventCoord();
		if (userInputCoord.X >= (cdd.frameStart.X + cdd.frameEnd.X / 2) - 2 && userInputCoord.X <= (cdd.frameStart.X + cdd.frameEnd.X / 2 + 1))
			if (userInputCoord.Y == cdd.frameStart.Y + cdd.frameEnd.Y - 3)
			{
				cddSC.hide();
				return;
			}
	}
}

serviceMenu::serviceMenu()
{
	servSC.setCacheCoord(servCfg.frameStart, { servCfg.frameSize.X + 1,servCfg.frameSize.Y + 1 });
	servMen.addItem(ITEM_FRAME, servCfg.BackColor, servCfg.TextColor, servCfg.frameStart, servCfg.frameSize, ITEM_NAME, "Service_menu_window", F_CONTROLS_ENABLE, F_HEADER_ENABLE, F_SHADOW_ENABLE, DOUBLE_LINE, HEADER_TEXT, servCfg.servMenuHeader);
	servMen.addItem(ITEM_FRAME, servCfg.BtnBackColor, servCfg.BtnTextColor, servCfg.mouseTestBtnStart, servCfg.mouseTestBtnSize, ITEM_NAME, "Mouse_test_button", F_CONTROLS_DISABLE, F_HEADER_DISABLE);
	servMen.addItem(ITEM_TEXT, servCfg.BtnBackColor, servCfg.BtnTextColor, servMen.getTextPos(servMen.searchByName("Mouse_test_button")), servMen.getTextSize(servMen.searchByName("Mouse_test_button")), ALIGN_CENTER, BODY_TEXT, servCfg.mouseTestBtnName);
	servMen.addItem(ITEM_FRAME, servCfg.BtnBackColor, servCfg.BtnTextColor, servCfg.consoleDumpBtnStart, servCfg.consoleDumpBtnSize, ITEM_NAME, "Console_dump_button", F_CONTROLS_DISABLE, F_HEADER_DISABLE);
	servMen.addItem(ITEM_TEXT, servCfg.BtnBackColor, servCfg.BtnTextColor, servMen.getTextPos(servMen.searchByName("Console_dump_button")), servMen.getTextSize(servMen.searchByName("Console_dump_button")), ALIGN_CENTER, BODY_TEXT, servCfg.consoleDumpBtnName);
	servMen.addItem(ITEM_FRAME, servCfg.BtnBackColor, servCfg.BtnTextColor, servCfg.exitBtnStart, servCfg.exitBtnSize, ITEM_NAME, "Exit_button", F_CONTROLS_DISABLE, F_HEADER_DISABLE);
	servMen.addItem(ITEM_TEXT, servCfg.BtnBackColor, servCfg.BtnTextColor, servMen.getTextPos(servMen.searchByName("Exit_button")), servMen.getTextSize(servMen.searchByName("Exit_button")), ALIGN_CENTER, BODY_TEXT, servCfg.exitBtnName);
	servMen.addItem(ITEM_FRAME, servCfg.BtnBackColor, servCfg.BtnTextColor, servCfg.resetMouseBufBtnStart, servCfg.resetMouseBufBtnSize, ITEM_NAME, "Clear_input_buffer", F_CONTROLS_DISABLE, F_HEADER_DISABLE);
	servMen.addItem(ITEM_TEXT, servCfg.BtnBackColor, servCfg.BtnTextColor, servMen.getTextPos(servMen.searchByName("Clear_input_buffer")), servMen.getTextSize(servMen.searchByName("Clear_input_buffer")), ALIGN_CENTER, BODY_TEXT, servCfg.resetMouseBufBtnName);
	servMen.heapSort();
}
void serviceMenu::show()
{
	COORD userInputCoord;
	servSC.scan();
	servMen.render(DRAW_LOCAL, 1);
	while (1)
	{
		userInputCoord = get_mouseEventCoord();
		if (userInputCoord.X == servCfg.frameStart.X + servCfg.frameSize.X - btnCloseX)
			if (userInputCoord.Y = servCfg.frameStart.Y)
			{
				this->hide();
				return;
			}
		if(userInputCoord.X>=servCfg.mouseTestBtnStart.X&&userInputCoord.X<=servCfg.mouseTestBtnStart.X+servCfg.mouseTestBtnSize.X)
			if (userInputCoord.Y >= servCfg.mouseTestBtnStart.Y&&userInputCoord.Y <= servCfg.mouseTestBtnStart.Y + servCfg.mouseTestBtnSize.Y)
				serv_MTP.show();
		if (userInputCoord.X >= servCfg.consoleDumpBtnStart.X&&userInputCoord.X <= servCfg.consoleDumpBtnStart.X + servCfg.consoleDumpBtnSize.X)
			if (userInputCoord.Y >= servCfg.consoleDumpBtnStart.Y&&userInputCoord.Y <= servCfg.consoleDumpBtnStart.Y + servCfg.consoleDumpBtnSize.Y)
				consoleDump();
		if (userInputCoord.X >= servCfg.exitBtnStart.X&&userInputCoord.X <= servCfg.exitBtnStart.X + servCfg.exitBtnSize.X)
			if (userInputCoord.Y >= servCfg.exitBtnStart.Y&&userInputCoord.Y <= servCfg.exitBtnStart.Y + servCfg.exitBtnSize.Y)
				exit(1);
		if (userInputCoord.X >= servCfg.resetMouseBufBtnStart.X&&userInputCoord.X <= servCfg.resetMouseBufBtnStart.X + servCfg.resetMouseBufBtnSize.X)
			if (userInputCoord.Y >= servCfg.resetMouseBufBtnStart.Y&&userInputCoord.Y <= servCfg.resetMouseBufBtnStart.Y + servCfg.resetMouseBufBtnSize.Y)
				serv_MTP.clearMouseBuffer();
	}
	this->hide();
}
void serviceMenu::hide()
{
	servSC.hide();
}

mainMenu::mainMenu()
{
	mmuSC.setCacheCoord(mainMu.frameStart, { mainMu.frameSize.X + 1,  mainMu.frameSize.Y + 1 });
	mmenu.addItem(ITEM_FRAME, mainMu.BackColor, mainMu.TextColor, mainMu.frameStart, mainMu.frameSize, ITEM_NAME, "Main_menu_frame", F_CONNECT_SIMPLE, F_CONTROLS_ENABLE, F_SHADOW_ENABLE, DOUBLE_LINE, F_HEADER_ENABLE, HEADER_TEXT, "Меню");
	mmenu.heapSort();
}
void mainMenu::show()
{
	COORD userInputCoord;
	mmuSC.scan();
	mmenu.render(DRAW_LOCAL,1);
	while (1)
	{
		userInputCoord = get_mouseEventCoord();
		if (userInputCoord.X == mainMu.frameStart.X + mainMu.frameSize.X - btnCloseX)
			if (userInputCoord.Y = mainMu.frameStart.Y)
			{
				this->hide();
				return;
			}
	}
}
void mainMenu::hide()
{
	mmuSC.hide();
}

mainEntry::mainEntry()
{
	//Чистим участки памяти от мусора
	memset(dbCons, 0x00, sizeof(dbCons));
	memset(rulCons, 0x00, sizeof(rulCons));
	memset(recCons, 0x00, sizeof(recCons));
	memset(banCons, 0x00, sizeof(banCons));

	//Грузим локаль
	loadLocale();

	//Кешируем простанство под занимаемой областью
	mainShadowCache.scan();

	//Настраиваем макет окна
	if (serviceMenu)
	{
		mainFrame.addItem(ITEM_FRAME, GRAY, BLACK, mainCfg.serviceBtnStart, mainCfg.serviceBtnSize, ITEM_NAME, "Service_btn_entry", F_HEADER_DISABLE, F_CONTROLS_DISABLE);
		mainFrame.addItem(ITEM_TEXT, GRAY, LIGHT_CYAN, mainFrame.getTextPos(mainFrame.searchByName("Service_btn_entry")), mainFrame.getTextSize(mainFrame.searchByName("Service_btn_entry")), BODY_TEXT, "СЕРВИС", ALIGN_CENTER);
	}

	mainFrame.addItem(ITEM_FRAME, LIGHT_GREEN, BLACK, mainCfg.dbSummaryStart, mainCfg.dbSummarySize, ITEM_NAME, "Окно сводки", F_HEADER_ENABLE, HEADER_TEXT, const_cast<char*>(currentLocale[6].c_str()));
	mainFrame.addItem(ITEM_TEXT, LIGHT_GREEN, BLACK, mainFrame.getTextPos(mainFrame.searchByName("Окно сводки")), mainFrame.getTextSize(mainFrame.searchByName("Окно сводки")), ITEM_NAME, "Сводка инфо", BODY_TEXT, const_cast<char*>(currentLocale[7].c_str()));
	mainFrame.addItem(ITEM_FRAME, LIGHT_GREEN, BLACK, mainCfg.dbRecordsStart, mainCfg.dbRecordsEnd, ITEM_NAME, "Счетчик записей");
	mainFrame.addItem(ITEM_TEXT, LIGHT_GREEN, BLACK, mainFrame.getTextPos(mainFrame.searchByName("Счетчик записей")), mainFrame.getTextSize(mainFrame.searchByName("Счетчик записей")), ITEM_NAME, "Записи данные", BODY_TEXT, const_cast<char*>(currentLocale[8].c_str()));
	mainFrame.addItem(ITEM_FRAME, LIGHT_GREEN, BLACK, mainCfg.dbBansStart, mainCfg.dbBansEnd, ITEM_NAME, "Счетчик банов");
	mainFrame.addItem(ITEM_TEXT, LIGHT_GREEN, BLACK, mainFrame.getTextPos(mainFrame.searchByName("Счетчик банов")), mainFrame.getTextSize(mainFrame.searchByName("Счетчик банов")), ITEM_NAME, "Баны данные", BODY_TEXT, const_cast<char*>(currentLocale[9].c_str()));
	mainFrame.addItem(ITEM_FRAME, YELLOW, WHITE, mainCfg.rulesCntrStart, mainCfg.rulesCntrSize, ITEM_NAME, "Акт.правила");
	mainFrame.addItem(ITEM_TEXT, YELLOW, WHITE, mainFrame.getTextPos(mainFrame.searchByName("Акт.правила")), mainFrame.getTextSize(mainFrame.searchByName("Акт.правила")), ITEM_NAME, "Правила счетчик", BODY_TEXT, const_cast<char*>(currentLocale[10].c_str()));
	mainFrame.addItem(ITEM_FRAME, YELLOW, WHITE, mainCfg.menuBtnStart, mainCfg.menuBtnSize, ITEM_NAME, "Кнопка меню");
	mainFrame.addItem(ITEM_TEXT, YELLOW, WHITE, mainFrame.getTextPos(mainFrame.searchByName("Кнопка меню")), mainFrame.getTextSize(mainFrame.searchByName("Кнопка меню")), ALIGN_CENTER, BODY_TEXT, const_cast<char*>(currentLocale[11].c_str()));
	mainFrame.addItem(ITEM_FRAME, YELLOW, WHITE, mainCfg.fastAddBtnStart, mainCfg.fastAddBtnSize, ITEM_NAME, "Кнопка добавить");
	mainFrame.addItem(ITEM_TEXT, YELLOW, WHITE, mainFrame.getTextPos(mainFrame.searchByName("Кнопка добавить")), mainFrame.getTextSize(mainFrame.searchByName("Кнопка добавить")), ALIGN_CENTER, BODY_TEXT, const_cast<char*>(currentLocale[12].c_str()));
	mainFrame.addItem(ITEM_FRAME, YELLOW, WHITE, mainCfg.fastEditBtnStart, mainCfg.fastEditBtnSize, ITEM_NAME, "Кнопка редактировать");
	mainFrame.addItem(ITEM_TEXT, YELLOW, WHITE, mainFrame.getTextPos(mainFrame.searchByName("Кнопка редактировать")), mainFrame.getTextSize(mainFrame.searchByName("Кнопка редактировать")), ALIGN_CENTER, BODY_TEXT, const_cast<char*>(currentLocale[13].c_str()));
	mainFrame.addItem(ITEM_FRAME, LIGHT_CYAN, BLACK, mainCfg.dbTableGraphicStart, mainCfg.dbTableGraphicSize, ITEM_NAME, "Таблица записей", F_HEADER_ENABLE, HEADER_TEXT, const_cast<char*>(currentLocale[23].c_str()), DOUBLE_LINE);

	mainFrame.addItem(ITEM_TABLE, LIGHT_CYAN, BLACK, mainFrame.getTextPos(mainFrame.searchByName("Таблица записей")), mainFrame.getTextSize(mainFrame.searchByName("Таблица записей")),
		ITEM_NAME, "Таблица строки",
		TABLE_ROW, SINGLE_LINE, 5,
		TABLE_ROW, SINGLE_LINE, 16,
		TABLE_ROW, SINGLE_LINE, 49,
		TABLE_ROW, SINGLE_LINE, 66,
		TABLE_ROW, SINGLE_LINE, 74,
		TABLE_ROW, SINGLE_LINE, 91,
		TABLE_ROW, SINGLE_LINE, 99,
		TABLE_ROW, DOUBLE_LINE, 155,
		H_SEPARATOR, DOUBLE_LINE, 1,
		F_SCROLL_ENABLE);

	mainFrame.addItem(ITEM_TEXT, LIGHT_CYAN, BLACK, mainFrame.getTextPos(mainFrame.searchByName("Таблица записей")), mainFrame.getTextSize(mainFrame.searchByName("Таблица записей")), ITEM_NAME, "Шапка таблицы", ALIGN_CENTER, ROWS_CONFIG, mainFrame.getRowConfig(mainFrame.searchByName("Таблица строки")),
		TABLE_TEXT, const_cast<char*>(currentLocale[14].c_str()),
		TABLE_TEXT, const_cast<char*>(currentLocale[15].c_str()),
		TABLE_TEXT, const_cast<char*>(currentLocale[16].c_str()),
		TABLE_TEXT, const_cast<char*>(currentLocale[17].c_str()),
		TABLE_TEXT, const_cast<char*>(currentLocale[18].c_str()),
		TABLE_TEXT, const_cast<char*>(currentLocale[19].c_str()),
		TABLE_TEXT, const_cast<char*>(currentLocale[20].c_str()),
		TABLE_TEXT, const_cast<char*>(currentLocale[21].c_str()));

	mainFrame.addItem(ITEM_FRAME, MAGENTA, WHITE, mainCfg.dbConsoleStart, mainCfg.dbConsoleSize, ITEM_NAME, "Консоль", F_HEADER_ENABLE, DOUBLE_LINE, HEADER_TEXT, const_cast<char*>(currentLocale[22].c_str()));
	mainFrame.addItem(ITEM_BASE, BLUE, BLACK, { 0,0 }, { 0,0 });

	putLog("Макет загружен(Основной экран)\n", INFO);

	for (uint8_t a = 0;a < 29;++a)
	{
		mainFrame.addItem(
			ITEM_TEXT,
			LIGHT_CYAN,
			BLACK,
			mainFrame.getTextPos(mainFrame.searchByName("Таблица строки")),
			mainFrame.getTextSize(mainFrame.searchByName("Таблица строки")),
			ITEM_NAME, dTableStrokeName[a],
			T_BUFFER,
			ALIGN_LEFT,
			GET_HSEP_INFO, mainFrame.getHsepShift(mainFrame.searchByName("Таблица строки")),
			ROWS_CONFIG, mainFrame.getRowConfig(mainFrame.searchByName("Таблица строки")),
			TABLE_LINE, a, "0000",												//No
			TABLE_LINE, a, "0000000000",										//VK ИД
			TABLE_LINE, a, "null",												//VK ник(VK URL)
			TABLE_LINE, a, "00/00/0000 00:00",									//Добалвен
			TABLE_LINE, a, "-",													//Наруш
			TABLE_LINE, a, "00/00/0000 00:00",									//Истекает
			TABLE_LINE, a, "NULL",												//Статус
			TABLE_LINE, a, const_cast<char*>(currentLocale[24].c_str())			//Пометка 
		);
		//mainFrame.lineCtrReset(mainFrame.searchByName(dTableStrokeName[a]));
	}

	mainFrame.addItem(ITEM_TABLE, MAGENTA, WHITE, mainFrame.getTextPos(mainFrame.searchByName("Консоль")), mainFrame.getTextSize(mainFrame.searchByName("Консоль")), ITEM_NAME, "Консоль строки", TABLE_ROW, DOUBLE_LINE, mainFrame.getTextSize(mainFrame.searchByName("Консоль")).X);
	for (uint8_t a = 0;a < 7;++a)
	{
		mainFrame.addItem(
			ITEM_TEXT,
			MAGENTA,
			WHITE,
			mainFrame.getTextPos(mainFrame.searchByName("Консоль строки")),
			mainFrame.getTextSize(mainFrame.searchByName("Консоль строки")),
			ITEM_NAME, dConsoleStrokeName[a],
			T_BUFFER,
			ALIGN_LEFT,
			ROWS_CONFIG, mainFrame.getRowConfig(mainFrame.searchByName("Консоль строки"))
			//TABLE_LINE, a, "NULL"
		);
		//mainFrame.lineCtrReset(mainFrame.searchByName(dTableStrokeName[a]));
	}

	//Сортируем кучу
	mainFrame.heapSort();
	this->renderLog();
}
void mainEntry::drawMainScreen()
{
	mainFrame.render(DRAW_EDITED);
}
void mainEntry::hideMainScreen()
{
	mainShadowCache.hide();
}
void mainEntry::renderLog()
{
	string temp;
	auto it = consoleBuffer.begin();
	for (uint8_t a = 0;a < 7;++a)
	{
		if (it == consoleBuffer.end())
			break;

		//Сбрасывать счётчик необходимо после любой операции со строками
		mainFrame.lineCtrReset(mainFrame.searchByName(dConsoleStrokeName[a]));
		mainFrame.lineErase(mainFrame.searchByName(dConsoleStrokeName[a]), mainFrame.searchByName("Консоль строки"), a);
		mainFrame.lineCtrReset(mainFrame.searchByName(dConsoleStrokeName[a]));
		mainFrame.printTabLine(mainFrame.searchByName(dConsoleStrokeName[a]), a, const_cast<char*>((*it).c_str()));
		temp = *it;
		++it;
	}
	mainFrame.render(DRAW_EDITED);
}
void mainEntry::printRecTable()
{
	putLog("Построение списка записей\n", INFO);
	this->renderLog();
	for (uint8_t a = 0;a < 29;++a)
	{
		dbList[activeRecords - a - 1]->getRecID();
		dbList[activeRecords - a - 1]->getVKIDchar();
		dbList[activeRecords - a - 1]->getVKurl();
		dbList[activeRecords - a - 1]->getAddDate();
		dbList[activeRecords - a - 1]->getRBIT();
		dbList[activeRecords - a - 1]->getExpDate();
		dbList[activeRecords - a - 1]->getFlags();
		dbList[activeRecords - a - 1]->getREM();
		mainFrame.lineCtrReset(mainFrame.searchByName(dTableStrokeName[a]));
		mainFrame.lineErase(mainFrame.searchByName(dTableStrokeName[a]), mainFrame.searchByName("Таблица строки"), a);
		mainFrame.lineCtrReset(mainFrame.searchByName(dTableStrokeName[a]));
		mainFrame.printTabLine(mainFrame.searchByName(dTableStrokeName[a]), a, dbO.recId);
		mainFrame.printTabLine(mainFrame.searchByName(dTableStrokeName[a]), a, dbO.vkID);
		mainFrame.printTabLine(mainFrame.searchByName(dTableStrokeName[a]), a, dbO.vkurl);
		mainFrame.printTabLine(mainFrame.searchByName(dTableStrokeName[a]), a, dbO.recAdd);
		mainFrame.printTabLine(mainFrame.searchByName(dTableStrokeName[a]), a, dbO.viols);
		mainFrame.printTabLine(mainFrame.searchByName(dTableStrokeName[a]), a, dbO.recExp);
		mainFrame.printTabLine(mainFrame.searchByName(dTableStrokeName[a]), a, dbO.status);
		mainFrame.printTabLine(mainFrame.searchByName(dTableStrokeName[a]), a, dbO.remark);
		memset(dbO.viols, 0x00, sizeof(dbO.viols));
		memset(dbO.remark, 0x00, sizeof(dbO.remark));
		memset(dbO.vkurl, 0x00, sizeof(dbO.vkurl));
	}
	putLog("Построение завершено\n", INFO);
	this->renderLog();
}
void mainEntry::menu()
{
	COORD userInputCoord;
	this->loadDb();
	this->drawMainScreen();
	putLog("Поле \"Статус\" имеет больше одного определения\n", WARN);
	this->renderLog();
	this->printRecTable();
	while (1)
	{
		userInputCoord = get_mouseEventCoord();
		if (userInputCoord.X >= mainCfg.menuBtnStart.X&&userInputCoord.X <= mainCfg.menuBtnStart.X + mainCfg.menuBtnSize.X)
		{
			if (userInputCoord.Y >= mainCfg.menuBtnStart.Y&&userInputCoord.Y <= mainCfg.menuBtnStart.Y + mainCfg.menuBtnSize.Y)
			{
				nForm.show();
				putLog("Зарегистрировано нажатие кнопки \"МЕНЮ\"\n", INFO);
				mmu.show();
				this->renderLog();
			}
		}
		if (userInputCoord.X >= mainCfg.fastAddBtnStart.X&&userInputCoord.X <= mainCfg.fastAddBtnStart.X + mainCfg.fastEditBtnSize.X)
		{
			if (userInputCoord.Y >= mainCfg.fastAddBtnStart.Y&&userInputCoord.Y <= mainCfg.fastAddBtnStart.Y + mainCfg.fastEditBtnSize.Y)
			{
				putLog("Зарегистрировано нажатие кнопки \"НОВАЯ ЗАПИСЬ\"\n", INFO);
				nForm.show();
				this->renderLog();
			}
		}
		if (userInputCoord.X >= mainCfg.fastEditBtnStart.X&&userInputCoord.X <= mainCfg.fastEditBtnStart.X + mainCfg.fastEditBtnSize.X)
		{
			if (userInputCoord.Y >= mainCfg.fastEditBtnStart.Y&&userInputCoord.Y <= mainCfg.fastEditBtnStart.Y + mainCfg.fastEditBtnSize.Y)
			{
				putLog("Зарегистрировано нажатие кнопки \"РЕД ЗАПИСЬ\"\n", INFO);
				this->renderLog();
			}
		}
		if (serviceMenu)
		{
			if (userInputCoord.X >= mainCfg.serviceBtnStart.X&&userInputCoord.X <= mainCfg.serviceBtnStart.X + mainCfg.serviceBtnSize.X)
			{
				if (userInputCoord.Y >= mainCfg.serviceBtnStart.Y&&userInputCoord.Y <= mainCfg.serviceBtnStart.Y + mainCfg.serviceBtnSize.Y)
				{
					putLog("Вызвано сервис меню\n", INFO);
					this->renderLog();
					servMen.show();
				}
			}
		}
		Sleep(50);
	}
	mmu.show();
}
void mainEntry::loadDb()
{
	string loadDone("БД успешно загружена, прочитано записей: ");
	string RulTempl(rulTemplat);
	string RecTempl(recTemplate);
	string BanTempl(banTemplate);

	//Метод загрузки БД+визуализация
	dbLoadScr.dbLFcache.scan();
	putLog("Файл БД tipadb.db найден и открыт\n", INFO);
	this->renderLog();

	dbFile.open("tipadb.db", ios::in | ios::binary);
	dbFile.seekg(0, ios::beg);
	dbFile.read(reinterpret_cast<char*>(&baseHead), sizeof(dbHeader));

	putLog("Заголовок успешно прочитан\n", INFO);
	this->renderLog();

	//baseHead.displayDBHED();

	dbLoadScr.dbLoadFrame.heapSort();
	activeRecords = baseHead.getRecords();
	dbLoadScr.dbLoadFrame.render(DRAW_EDITED);

	RecTempl = RecTempl + to_string(baseHead.getRecords());
	BanTempl = BanTempl + to_string(baseHead.getBans());
	RulTempl = RulTempl + to_string(baseHead.getRulesCount());

	mainFrame.lineCtrReset(mainFrame.searchByName("Сводка инфо"));
	mainFrame.itemTextErase(mainFrame.searchByName("Сводка инфо"));
	mainFrame.printItemText(mainFrame.searchByName("Сводка инфо"), baseHead.getDBname(), ALIGN_LEFT);
	mainFrame.lineCtrReset(mainFrame.searchByName("Записи данные"));
	mainFrame.itemTextErase(mainFrame.searchByName("Записи данные"));
	mainFrame.printItemText(mainFrame.searchByName("Записи данные"), const_cast<char*>(RecTempl.c_str()), ALIGN_LEFT);
	mainFrame.lineCtrReset(mainFrame.searchByName("Баны данные"));
	mainFrame.itemTextErase(mainFrame.searchByName("Баны данные"));
	mainFrame.printItemText(mainFrame.searchByName("Баны данные"), const_cast<char*>(BanTempl.c_str()), ALIGN_LEFT);
	mainFrame.lineCtrReset(mainFrame.searchByName("Правила счетчик"));
	mainFrame.itemTextErase(mainFrame.searchByName("Правила счетчик"));
	mainFrame.printItemText(mainFrame.searchByName("Правила счетчик"), const_cast<char*>(RulTempl.c_str()), ALIGN_LEFT);

	dbLoadScr.dbLoadFrame.render(DRAW_EDITED);

	dbFile.seekg(sizeof(dbHeader), ios::beg);
	for (uint8_t a = 0; a < activeRecords;++a)
	{
		dbFile.seekg((sizeof(summary)*a) + sizeof(dbHeader), ios::beg);
		dbFile.read(reinterpret_cast<char*>(&fileIO), sizeof(summary));
		dbList[a] = new summary(fileIO);
		cOutLoadDb << "Читаю -> [" << setw(4) << setfill('0') << to_string(a) << "] VK ID: " << dbList[a]->getVKID().substr(0,10);
		dbLoadScr.dbLoadFrame.lineCtrReset(dbLoadScr.dbLoadFrame.searchByName("Load_container"));
		dbLoadScr.dbLoadFrame.lineErase(dbLoadScr.dbLoadFrame.searchByName("Load_container"), dbLoadScr.dbLoadFrame.searchByName("Load_scr_DB_tab"), 3);
		dbLoadScr.dbLoadFrame.lineCtrReset(dbLoadScr.dbLoadFrame.searchByName("Load_container"));
		sCOUT = cOutLoadDb.str();
		cOutLoadDb.str(std::string());
		dbLoadScr.dbLoadFrame.printTabLine(dbLoadScr.dbLoadFrame.searchByName("Load_container"), 3, const_cast<char*>(sCOUT.c_str()));
		dbLoadScr.dbLoadFrame.render(DRAW_EDITED);
	}
	dbLoadScr.dbLFcache.hide();

	loadDone = loadDone + to_string(activeRecords) + "\n";
	putLog(loadDone, INFO);
	this->renderLog();

	//dbTemplate = dbTemplate + dbOut.str();
	//RulTempl = RulTempl + recOut.str();
	//banTemplate = banTemplate + banOut.str();
	//rulTemplate = rulTemplate + rulOut.str();
	//std::copy(dbTemplate.begin(), dbTemplate.end(), dbCons);
	//std::copy(recTemplate.begin(), recTemplate.end(), recCons);
	//std::copy(banTemplate.begin(), banTemplate.end(), banCons);
	//std::copy(rulTemplate.begin(), rulTemplate.end(), rulCons);
}

dbLoadScreen::dbLoadScreen()
{
	dbLFcache.setCacheCoord(dbFrame.frameStart, { dbFrame.frameSize.X + 1, dbFrame.frameSize.Y + 1 });
	dbLoadFrame.addItem(ITEM_FRAME, GRAY, WHITE, dbFrame.frameStart, dbFrame.frameSize, ITEM_NAME, "DB_load_screen", F_SHADOW_ENABLE);
	dbLoadFrame.addItem(ITEM_TABLE, GRAY, WHITE, 
		dbLoadFrame.getTextPos(dbLoadFrame.searchByName("DB_load_screen")), 
		dbLoadFrame.getTextSize(dbLoadFrame.searchByName("DB_load_screen")), 
		ITEM_NAME, "Load_scr_DB_tab",
		TABLE_ROW, SINGLE_LINE, dbLoadFrame.getTextSize(dbLoadFrame.searchByName("DB_load_screen")).X,
		H_SEPARATOR, SINGLE_LINE, 1
	);
	dbLoadFrame.addItem(
		ITEM_TEXT,
		GRAY,
		WHITE,
		dbLoadFrame.getTextPos(dbLoadFrame.searchByName("Load_scr_DB_tab")),
		dbLoadFrame.getTextSize(dbLoadFrame.searchByName("Load_scr_DB_tab")),
		ITEM_NAME, "Title_load_DB",
		T_BUFFER,
		ALIGN_CENTER,
		ROWS_CONFIG, dbLoadFrame.getRowConfig(dbLoadFrame.searchByName("Load_scr_DB_tab")),
		TABLE_LINE, 0, "Загрузка БД"
	);
	dbLoadFrame.addItem(
		ITEM_TEXT,
		GRAY,
		WHITE,
		dbLoadFrame.getTextPos(dbLoadFrame.searchByName("Load_scr_DB_tab")),
		dbLoadFrame.getTextSize(dbLoadFrame.searchByName("Load_scr_DB_tab")),
		ITEM_NAME, "Load_stroke",
		T_BUFFER,
		ALIGN_CENTER,
		GET_HSEP_INFO, dbLoadFrame.searchByName("Load_scr_DB_tab"),
		ROWS_CONFIG, dbLoadFrame.getRowConfig(dbLoadFrame.searchByName("Load_scr_DB_tab")),
		TABLE_LINE, 1, "Загружаю запись..."
	);
	dbLoadFrame.addItem(
		ITEM_TEXT,
		GRAY,
		LIGHT_YELLOW,
		dbLoadFrame.getTextPos(dbLoadFrame.searchByName("Load_scr_DB_tab")),
		dbLoadFrame.getTextSize(dbLoadFrame.searchByName("Load_scr_DB_tab")),
		ITEM_NAME, "Load_container",
		T_BUFFER,
		ALIGN_CENTER,
		GET_HSEP_INFO, dbLoadFrame.searchByName("Load_scr_DB_tab"),
		ROWS_CONFIG, dbLoadFrame.getRowConfig(dbLoadFrame.searchByName("Load_scr_DB_tab")),
		TABLE_LINE, 3, "NULL"
	);
	putLog("Макет загружен(Экран загузки)", INFO);
}

void putLog(string inputArg, logSettings logSet)
{
	//Достаём значение системного таймера
	boost::posix_time::ptime datetime = boost::posix_time::microsec_clock::universal_time();
	//Буфер для записи значений времени
	stringstream dateCout;
	dateCout << datetime;
	//Контейнеры для хранений промежуточных значений при компоновке сообщения
	string tempLog;
	string dateLog;
	switch (logSet)
	{
		case NUL:
		{
			consoleBuffer.push_front(inputArg);
			break;
		}
		case INFO:
		{
			//Рендер сообщений помеченных как инфо
			if (!localeLoaded)
			{
				//Рендер при отсутствии локали
				dateLog = dateCout.str();
				tempLog = "[" + dateLog.substr(12, 12) + "]";
				tempLog = tempLog+"[ИНФО]";
			}
			else 
			{
				dateLog = dateCout.str();
				tempLog = "[" + dateLog.substr(12, 12) + "]";
				tempLog = tempLog + const_cast<char*>(currentLocale[25].c_str());
			}
			tempLog = tempLog + inputArg;
			consoleBuffer.push_front(tempLog);
			break;
		}
		case WARN:
		{
			//Рендер сообщений предупреждений
			if (!localeLoaded)
			{
				//Рендер при отсутствии локали
				dateLog = dateCout.str();
				tempLog = "[" + dateLog.substr(12, 12) + "]";
				tempLog = tempLog+"[ПРЕД]";
			}
			else
			{
				dateLog = dateCout.str();
				tempLog = "[" + dateLog.substr(12, 12) + "]";
				tempLog = tempLog + const_cast<char*>(currentLocale[26].c_str());
			}
			tempLog = tempLog + inputArg;
			consoleBuffer.push_front(tempLog);
			break;
		}
		case FATAL:
		{
			//Рендер критических ошибок
			if (!localeLoaded)
			{
				//Рендер при отсутствии локали
				dateLog = dateCout.str();
				tempLog = "[" + dateLog.substr(12, 12) + "]";
				tempLog = tempLog+"[КРИТ]";
			}
			else
			{
				dateLog = dateCout.str();
				tempLog = "[" + dateLog.substr(12, 12) + "]";
				tempLog = tempLog + const_cast<char*>(currentLocale[27].c_str());
			}
			tempLog = tempLog + inputArg;
			consoleBuffer.push_front(tempLog);
			break;
		}
	}
}
void consoleDump()
{
	COORD userInputCoord;
	consoleDumpDone cdd;
	frameBuilder dumpMsg;
	shadowCache cddSC;
	cddSC.setCacheCoord(cdd.frameStart, { cdd.frameEnd.X + 1,cdd.frameEnd.Y + 1 });
	dumpMsg.addItem(ITEM_FRAME, cdd.BackColor, cdd.TextColor, cdd.frameStart, cdd.frameEnd, ITEM_NAME, "Done_msg", F_HEADER_DISABLE, F_CONTROLS_DISABLE, F_SHADOW_ENABLE);
	dumpMsg.addItem(ITEM_TEXT, cdd.okBtnBack, cdd.okBtnText, cdd.frameStart, cdd.frameEnd, BODY_TEXT, cdd.doneText, COORD_ENABLE, (cdd.frameStart.X + cdd.frameEnd.X / 2) - 3, cdd.frameStart.Y + cdd.frameEnd.Y - 5);
	dumpMsg.addItem(ITEM_TEXT, cdd.okBtnBack, cdd.okBtnText, dumpMsg.getTextPos(dumpMsg.searchByName("Done_msg")),dumpMsg.getTextSize(dumpMsg.searchByName("Done_msg")),ALIGN_CENTER, BODY_TEXT, cdd.okBtn, COORD_ENABLE, (cdd.frameStart.X+cdd.frameEnd.X/2)-2, cdd.frameStart.Y+cdd.frameEnd.Y-1);
	ofstream logDump("console.log");
	logDump << "ДАМП КОНСОЛИ" << endl;
	for (auto it = consoleBuffer.begin();it != consoleBuffer.end();++it)
	{
		logDump << (*it);
	}
	cddSC.scan();
	dumpMsg.render(DRAW_LOCAL, 1);
	while (1)
	{
		userInputCoord = get_mouseEventCoord();
		if (userInputCoord.X >= (cdd.frameStart.X + cdd.frameEnd.X / 2) - 2 && userInputCoord.X <= (cdd.frameStart.X + cdd.frameEnd.X / 2+1))
			if (userInputCoord.Y == cdd.frameStart.Y + cdd.frameEnd.Y - 3)
			{
				cddSC.hide();
				return;
			}
	}
}

//0x0419 русский
//0x0409 инглиш
//0x0422 мова
//0х0407 дойчланд
//0x040C французи