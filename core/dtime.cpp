#include <string>
#include <algorithm>

#include "cpp.h"

const unsigned char templatePos[10] = { 0x00,0x03,0x06,0x08,0x0B,0x0E,0x02,0x05,0x0A,0x0D };	//Позиции для вставки чисел в человеко-читаемом шаблоне даты-времени и указатели на элементы правильной формы
uint8_t daymonth;	//магическая переменная, вынесена из класса dTime
uint64_t symbols;	//счётчик символов, вынесен из класса dTime

string dTime::decodeDTime(char inputArg[DT_length + 1])	//Распаковщик данных даты-времени
{
	char DTimeTemplate[17] = "  /  /       :  ";	//шаблон формы
	char tempBuf[3];	//буфер + 1 байт резерв
	string strBuf;
	uint8_t temp = NULL;

	if (debugMode)
	{
		cout << "Контейнер данных (ДАТА-ВРЕМЯ): [";
		for (uint8_t a = 0;a < DT_length;++a)
		{
			printf("0x%02x ", inputArg[a]);
		}
		cout << "]" << endl;
		cout << "Template=" << DTimeTemplate << endl;
	}

	for (uint8_t a = 0;a < DT_length;++a)	//Выводим(преобразуем) дату-время в человеко-читаемый формат
	{
		temp = inputArg[a];	//помещаем в буфер запакованное число
		strBuf = std::to_string(temp);
		//_itoa_s(temp, tempBuf, 10);	//собственно распакова, устаревшее
		strBuf.copy(tempBuf, 10);	//собственно распакова

		if (debugMode)
			cout << "temp=" << temp << " tempbuf=" << tempBuf << endl;

		if (temp < 10)	//блок коррекции формата отображаемого значения
						//если число меньше 10, то есть одноразрядное,
						//то смещаем вправо и влево дописываем ноль
		{
			tempBuf[1] = tempBuf[0];
			tempBuf[0] = '0';
		}
		arrcpyw(DTimeTemplate, tempBuf, templatePos[a], 2);	//вставляем число в шаблон на нужную позицию, см templatePos

		if (debugMode)
			cout << "Template=" << DTimeTemplate << endl;
	}

	if (debugMode)
		cout << "Template=" << DTimeTemplate << endl;

	return DTimeTemplate;

}	  //Распаковщик данных даты-времени
bool dTime::processInput(char *out, string inputArg)	//обработка ввода пользователя и запаковка в двоичный формат
{
	string convertbuff;
	if (debugMode)
		cout << "Получено " << inputArg << endl;

	//очистка ввода от пользовательских символов
	inputArg.erase(std::remove(inputArg.begin(), inputArg.end(), '/'), inputArg.end());

	if (debugMode)
		cout << "Очистка первый проход " << inputArg << endl;

	inputArg.erase(std::remove(inputArg.begin(), inputArg.end(), ' '), inputArg.end());

	if (debugMode)
		cout << "Очистка второй проход " << inputArg << endl;

	inputArg.erase(std::remove(inputArg.begin(), inputArg.end(), ':'), inputArg.end());

	if (debugMode)
		cout << "Очистка третий проход " << inputArg << endl;

	//запаковка в двоичный формат
	for (uint8_t a = 0;a < DT_length;++a)
	{
		//преобразования символа в байт-формат
		convertbuff = inputArg.substr(a * 2, 2);

		if (debugMode)
			cout << a * 2 << " " << (a * 2) + 2 << " " << endl;

		//подготовка байта к записи
		daymonth = atoi(convertbuff.c_str());

		if (debugMode)
		{
			cout << "convertbuff " << convertbuff << "\n";
			cout << "daymonth=";
			printf("0x%02x", daymonth);
			cout << endl;
		}

		//запись байта
		out[a] = daymonth;
	}
	out[DT_length + 1] = '\0';

	if (debugMode)
	{
		cout << "out=" << out << endl;

		cout << "Контейнер данных (ДАТА-ВРЕМЯ): [";
		for (uint8_t a = 0;a < DT_length;++a)
		{
			printf("0x%02x ", inputArg[a]);
		}
		cout << "]" << endl;
	}

	return 1;	//в случае успеха возвращаем единицу
}
bool dTime::checkForm(string inputArg)	//Парсер ввода для проверки правильности, если что-то не так кидаем исключение
{
	char DTimeTemplate[17] = "  /  /       :  ";	//шаблон формы
	bool state = 0;
	//cout << "Количество введенных символов: " << symbols << endl;
	if (symbols != 17)	//Проверяем количество введенных символов - их должно быть либо 17 либо 0(1)
	{
		if (symbols == 1)
			state = 0;
		throw errorReport(0x06);
	}

	for (uint8_t a = 0;a < 4; ++a)	//Проверяем соответствие заполненному шаблону, иначе сообщаем юзеру об ошибке
	{
		if (!(inputArg[templatePos[a + DT_length]] == DTimeTemplate[templatePos[a + DT_length]]))
			throw errorReport(0x06);
		//cout << "err=" << to_string(a) << "(" << to_string(a+DT_length) << ")(" << to_string(templatePos[a+DT_length]) << ") char=" << inputArg[templatePos[a + DT_length]] << "template=" << DTimeTemplate[templatePos[a + DT_length]] << endl;
	}

	//Проверяем, чтобы были записаны цифры, а не буквы или другие знаки
	daymonth = 0;
	for (uint8_t a = 0; a < DT_UInputLng - 1; a++)
	{
		if (inputArg[a] == DTimeTemplate[templatePos[daymonth + DT_length]]) //Пропускаем символы шаблона, чтобы не вызывали ложных срабатываний
		{
			//cout << "trig skip=" << inputArg[a] << endl;
			++daymonth;
			++a;
		}
		//cout << "check char=" << inputArg[a] << endl;
		if (!isdigit(inputArg[a])) //если ввод не является цифрой - кидаем исключение
			throw errorReport(0x06);
	}
	state = 1;
	return state;
}
void dTime::readInputDTIME()	//Мост юзер-программа для заполнения полей
{
	char charUserInput[DT_UInputLng];

	std::cin.ignore();

	//Ввод от пользователя даты добавления в базу, метка DTIME_add
DTIME_add:
	try
	{
		//Пробуем прочитать строку от пользователя, строчка должна быть
		//строго определённого формата
		cout << "Введите дату добавления в базу в формате ДД/ММ/ГГГГ ЧЧ:ММ: ";
		cin.getline(charUserInput, DT_UInputLng);
		if (debugMode)
		{
			cout << "Контейнер данных (ДАТА-ВРЕМЯ): [";
			for (uint8_t a = 0;a < DT_UInputLng;++a)
			{
				printf("[%c] ", charUserInput[a]);
			}
			cout << "]" << endl;
		}

		//Получаем инфо о количествах введенных символов и запускаем метод проверки
		//правильности ввода
		symbols = cin.gcount();
		checkForm(charUserInput);
	}
	catch (errorReport)
	{
		//В случае ошибки - сообщаем и повторяем процедуру
		cout << "Повторите попытку\nФормат ДД/ММ/ГГГГ ЧЧ:ММ" << endl;
		__asm
		{
			JMP DTIME_add; //goto DTIME_add
		}
	}

	//Пытаемся обработать ввод и записать запакованное число
	//if (processInput(dtsta, charUserInput))
		//cout << "Обработка ввода: успех!" << endl;
	processInput(dtsta, charUserInput);

	if (debugMode)
	{
		cout << "Контейнер данных (ДАТА-ВРЕМЯ): [";
		for (uint8_t a = 0;a < DT_length;++a)
		{
			printf("[%c] ", dtsta[a]);
		}
		cout << "]" << endl;
	}

	//Ввод от пользователя даты окончания срок, метка DTIME_expires
DTIME_expires:
	try
	{
		//Пробуем прочитать строчку от пользователя, строчка должна быть
		//строго определенного формата
		cout << "Введите дату окончания срока в формате ДД/ММ/ГГГ ЧЧ:ММ: ";
		cin.getline(charUserInput, DT_UInputLng);
		if (debugMode)
		{
			cout << "Контейнер данных (ДАТА-ВРЕМЯ): [";
			for (uint8_t a = 0;a < DT_UInputLng;++a)
			{
				printf("[%c] ", charUserInput[a]);
			}
			cout << "]" << endl;
		}

		//Получаем инфо о количестве введенных символов и запускаем метод проверки
		//правильности ввода
		symbols = cin.gcount();
		checkForm(charUserInput);

	}
	catch (errorReport)
	{
		//В слдучае ошибки - сообщаем и повторяем процедуру
		cout << "Повторите попытку\nФормат ДД/ММ/ГГГГ ЧЧ:ММ" << endl;
		__asm
		{
			JMP DTIME_expires;	//goto DTIME_expires
		}
	}

	//Пытаемся обработать ввод и записать запакованное число
	//if (processInput(dtstp, charUserInput))
		//cout << "Обработка ввода: успех!" << endl;
	processInput(dtstp, charUserInput);

	if (debugMode)
	{
		cout << "Контейнер данных (ДАТА-ВРЕМЯ): [";
		for (uint8_t a = 0;a < DT_length;++a)
		{
			printf("0x%02x ", dtstp[a]);
		}
		cout << "]" << endl;
	}

	//Ввод от пользователя даны бана, либо пустое поле если бана не было, метка DTIME_ban
DTIME_ban:
	try
	{
		cout << "Введите дату бана в формате ДД/ММ/ГГГГ ЧЧ:ММ: \n (либо оставьте пустым если бана не было)";
		fgets(charUserInput, DT_UInputLng, stdin);
		if (strcmp(charUserInput, "\n") == 0)
		{
			cout << "[Пропуск]" << endl;
			memset(dtban, 0x00, DT_length);

			if (debugMode)
			{
				cout << "Контейнер данных (ДАТА-ВРЕМЯ): [";
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
			cout << "Контейнер данных (ДАТА-ВРЕМЯ): [";
			for (uint8_t a = 0;a < DT_UInputLng;++a)
			{
				printf("[%c] ", charUserInput[a]);
			}
			cout << "]" << endl;
		}
	}
	catch (errorReport)
	{
		//В случае ошибки - сообщаем и повторяем процедуру
		cout << "Повторите попытку\nФормат ДД/ММ/ГГГГ ЧЧ:ММ" << endl;
		__asm
		{
			JMP DTIME_ban;	//goto DTIME_ban
		}
	}
	//if (processInput(dtban, charUserInput))
		//cout << "Обработка ввода: успех!" << endl;
	processInput(dtban, charUserInput);

	if (debugMode)
	{
		cout << "Контейнер данных (ДАТА-ВРЕМЯ): [";
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
void dTime::displayDTIME()	// общедоступный метод, выводит запакованные данные в человеко-читаемый формат
{
	string UOutAdd;	//string переменные для хранения распакованных значений
	string UOutExp;
	string UOutBan;
	UOutAdd = decodeDTime(dtsta);	//распаковка значений из байтового формата в человеко-читаемый вид
	UOutExp = decodeDTime(dtstp);
	UOutBan = decodeDTime(dtban);
	cout << "Добавлен в базу: " << UOutAdd << "\nИстечение срока: " << UOutExp << "\nБан: " << UOutBan << endl;
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