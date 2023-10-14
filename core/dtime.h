#pragma once
//#pragma message("dtime.h included")
#ifndef _DTIME_H_
#define _DTIME_H_

#include "cpp.h"

#define DT_length		6		//Байтовая длина поля даты/времени в бинарном формате

class dTime	//временные отметки внесения в базу, бана, истечения срока
{
	private:
		string decodeDTime(char inputArg[DT_length+1]);	//Распаковщик данных даты-времени
		bool processInput(char *out, string inputArg);	//обработка ввода пользователя и запаковка в двоичный формат
		bool checkForm(string inputArg);	//Парсер ввода для проверки правильности, если что-то не так кидаем исключение
	protected:
		char dtsta[DT_length];	//временная отметка внесения в базу
		char dtstp[DT_length];	//временная отметка бана
		char dtban[DT_length];	//временная отметка истечения срока
		void writeDtStart(char arg[DT_length + 1]);
		void writeDtStop(char arg[DT_length + 1]);
		void writeDtBan(char arg[DT_length + 1]);
	public:
		dTime();
		dTime(char dStart[DT_length], char dStop[DT_length], char dBan[DT_length]);
		void dTimeImportDates(string timeStart, string timeEnd, string timeBan);
		string getADDTIME();
		void readInputDTIME();
		void displayDTIME();	// общедоступный метод, выводит запакованные данные в человеко-читаемый формат
		void getAddDate();
		void getExpDate();
};

#endif