#pragma once
#ifndef _BASELOCALE_H_
#define _BASELOCALE_H_

//Файл базовой локали, подставляемый в случае отсустствия запрошенного файла локали
struct baseLocale
{
	char sign[15] = "Базовая локаль";
	char appVer[18] = "Версия приложения";
	char inDevSign[13] = "В РАЗРАБОТКЕ";
	char dbSummary[7] = "Сводка";
	char dbName[9] = "БД: NULL";
	char dbRecs[14] = "ЗАПИСЕЙ: NULL";
	char dbBans[12] = "БАНОВ: NULL";
	char dbActRuls[19] = "Активных правил: 0";
	char mainScrMenu[5] = "Меню";
	char mainScrNewRec[13] = "Новая запись";
	char mainScrEdRec[11] = "Ред.запись";
	char dbTabNo[3] = "No";
	char dbTabVKID[6] = "VK ИД";
	char dbTabVKURL[15] = "VK ник(VK URL)";
	char addDate[9] = "Добавлен";
	char dbViols[6] = "Наруш";
	char dbExpires[9] = "Истекает";
	char dbRecStatus[7] = "Статус";
	char dbRecRem[17] = "Пометка к записи";
	char mainScrConsole[8] = "Консоль";
	char loadedRecs[19] = "Загруженные записи";
	char emptySign[6] = "Пусто";
	char infoMsg[7] = "[ИНФО]";
	char warnMsg[7] = "[ПРЕД]";
	char fatalMsg[7] = "[КРИТ]";
};
#endif