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
uint8_t rulBits::convertString(string _inputArg)	//Запаковка пользовательского ввода для экономии места
{
	string argBuf;	//Контейнер ввода
	uint8_t stringLength = 17;	//Изначальная длина записи, не может быть больше 15(8 симв. + 7 запятых)
	uint8_t _commas = 0;	//Количество запятых
	uint8_t registerBit = 0;	//Хранилище запакованных битов

	//cout << "Получено: " << _inputArg << endl;

	//Если ввод пустой, то выходим
	if (_inputArg[0] == NULL)
		return 0;

	//Проверяем правильность ввода, иначе исключаемся
	for (uint8_t a = 1;a < 17;a += 2)
	{
		if (_inputArg[a] == NULL)
			break;
		if (_inputArg[a] != ',')
			throw errorReport(0x06);
	}

	//Считаем подсчитываем кол-во запятых и колво символов
	for (uint8_t a = 0;a < stringLength;++a)
	{
		if (_inputArg[a] == NULL)
		{
			//cout << "Длина: " << to_string(a) << "\nЗапятых: " << to_string(_commas) << "\nИтоговая длина: " << to_string(a - _commas) << endl;
			_commas = a - _commas;
			stringLength = a;
		}
		if (_inputArg[a] == ',')
			++_commas;
	}

	//Чистим строку от запятых
	_inputArg.erase(std::remove(_inputArg.begin(), _inputArg.end(), ','));
	//cout << "Очистка: " << inputArg << "\nДлина: " << to_string(_commas) << endl;

	//Преобразуем и пакуем в биты
	for (uint8_t a = 0;a < _commas;++a)
	{
		argBuf = _inputArg[a];
		//cout << endl;
		registerBit |= (0x01 << (atoi(argBuf.c_str()) - 1));
	}
	//cout << "Регистр флагов HEX=";
	//printf("0x%02x", registerBit);
	//cout << endl;

	//Вертаем взад наш байт
	return registerBit;
}
void rulBits::readInputRBIT()	//Опрос оператора по пунткам нарушений и/или бана
{
	char userInput[17]; //Буфер для записи, не может быть больше 15(8 симв. + 7 запятых)

	//Начинаем опрашивать, у пользователя 2 попытки
INPUT_RUL:
	try
	{
		cout << "Введите нарушения по пунктам(например 1,3,4,7), не больше 8: ";
		cin >> userInput;
		rul = convertString(userInput);
	}
	catch (errorReport)
	{
		cout << "Неправильный формат ввода, повторите попытку" << endl;
		__asm
		{
			JMP INPUT_RUL
		}
	}

INPUT_BAN:
	try
	{
		cout << "Введите бан по пунктам(например 2,4,5), не больше 8: ";
		cin >> userInput;
		rulBan = convertString(userInput);
	}
	catch (errorReport)
	{
		cout << "Неправильный формат ввода, повторите попытку" << endl;
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
	//всякий хлам
	uint8_t bitMASK = 0x01;	//Битовая маска, по ней проверяем есть ли бит
	uint8_t cursor = 1;	//Позиция с котороый надо начать ставить запятые пропуская место символа
	string _t;	//Какая-то хрень, лишь бы вижла не ругалась
	char nullMrk[5] = { 'N', 'U', 'L', 'L', NULL };
	char decodeBuffer[17];	//8 символов + 7 запятых + 1 запятая(костыль на баг)
	decodeBuffer[16] = 0x00;

	if (debugMode)
	{
		cout << "Контейнер выходного потока: " << decodeBuffer << endl;
		cout << "регистр rul=";
		printf("0x%02X", rul);
		cout << endl;
		cout << "регистр rulBan=";
		printf("0x%02X", rulBan);
		cout << endl;
	}

	//Чистим от мусора
	memset(decodeBuffer, 0x00, 15);

	if (debugMode)
	{
		cout << "Контейнер выходного потока после очистки: " << decodeBuffer << endl;
		cout << "регистр rul=";
		printf("0x%02X", rul);
		cout << endl;
	}

	//Читаем регистр через лог. И, и в если выражение (регистр И битмаска) = 1, записываем пункт
	for (uint8_t a = 0;a < 8;++a)
	{
		if (debugMode)
		{
			cout << "Проход №" << to_string(a) << " смещение ";
			printf("0x%02X", (rul >> a));
			cout << endl;
		}

		//Двигаем регистр и сравниваем самый младший бит через И
		if (((rul >> a)&bitMASK) == 1)
		{
			if (debugMode)
				cout << "Выходной бит=1 ";

			//Стрёмная асмовая хуйня
			__asm
			{

			}
			_t = to_string(a + 1); //Записываем в буфер очеловеченное значение

			if (debugMode)
				cout << "Бит " << to_string(a + 1) << " ";

			decodeBuffer[cursor - 1] = _t[0];	//Переносим из буфера в контейнер

			if (debugMode)
				cout << "\nЗапись в выходной поток [" << _t << "], позиция курсора=" << to_string(cursor - 1);

			decodeBuffer[cursor] = ',';	//Добавляем запятую

			if (debugMode)
				cout << "\nЗапись в выходной поток [" << decodeBuffer << "], позиция курсора=" << to_string(cursor);

			cursor += 2; //Пропускам позицию для следующей цифры

			if (debugMode)
				cout << "\nПозиция курсора= " << to_string(cursor) << endl;
		}
	}

	if (debugMode)
		cout << "Коррекция..." << endl;

	//Костыль для сокрытия говнокода
	for (uint8_t a = 15;a > 0;--a)
	{
		if (debugMode)
			cout << "Проход №" << to_string(a) << endl;

		if (decodeBuffer[a] == ',')	//Как тольно нашли лишнюю запятую
		{

			if (debugMode)
			{
				cout << "Удаление лишней запятой из позиции " << to_string(a) << "\nПредыдущая позиция " << to_string(a) << ", содержимое [";
				printf("0x%02X]", decodeBuffer[a - 1]);
				cout << endl;
			}

			decodeBuffer[a] = NULL;	//То убираем её
			break;
		}
	}
	rOur << decodeBuffer << endl;
	rOut = rOur.str();
	std::copy(rOut.begin(), rOut.end(), dbO.viols);
}
void rulBits::displayRBIT()const	//Читаем регистр и показываем что нарушено а за что бан в ебало
{
	//всякий хлам
	uint8_t bitMASK = 0x01;	//Битовая маска, по ней проверяем есть ли бит
	uint8_t cursor = 1;	//Позиция с котороый надо начать ставить запятые пропуская место символа
	string _t;	//Какая-то хрень, лишь бы вижла не ругалась
	char nullMrk[5] = { 'N', 'U', 'L', 'L', NULL };
	char decodeBuffer[17];	//8 символов + 7 запятых + 1 запятая(костыль на баг)
	decodeBuffer[16] = 0x00;

	if (debugMode)
	{
		cout << "Контейнер выходного потока: " << decodeBuffer << endl;
		cout << "регистр rul=";
		printf("0x%02X", rul);
		cout << endl;
		cout << "регистр rulBan=";
		printf("0x%02X", rulBan);
		cout << endl;
	}

	//Чистим от мусора
	memset(decodeBuffer, 0x00, 15);

	if (debugMode)
	{
		cout << "Контейнер выходного потока после очистки: " << decodeBuffer << endl;
		cout << "регистр rul=";
		printf("0x%02X", rul);
		cout << endl;
	}

	if (rul == 0x00)
	{
		arrcpy(decodeBuffer, nullMrk);
		cout << "Нарушения по пунктам: " << decodeBuffer << endl;
		__asm
		{
			JMP PRINT_RULBAN
		}
	}

	//Читаем регистр через лог. И, и в если выражение (регистр И битмаска) = 1, записываем пункт
	for (uint8_t a = 0;a < 8;++a)
	{
		if (debugMode)
		{
			cout << "Проход №" << to_string(a) << " смещение ";
			printf("0x%02X", (rul >> a));
			cout << endl;
		}

		//Двигаем регистр и сравниваем самый младший бит через И
		if (((rul >> a)&bitMASK) == 1)
		{
			if (debugMode)
				cout << "Выходной бит=1 ";

			//Стрёмная асмовая хуйня
			__asm
			{

			}
			_t = to_string(a + 1); //Записываем в буфер очеловеченное значение

			if (debugMode)
				cout << "Бит " << to_string(a + 1) << " ";

			decodeBuffer[cursor - 1] = _t[0];	//Переносим из буфера в контейнер

			if (debugMode)
				cout << "\nЗапись в выходной поток [" << _t << "], позиция курсора=" << to_string(cursor - 1);

			decodeBuffer[cursor] = ',';	//Добавляем запятую

			if (debugMode)
				cout << "\nЗапись в выходной поток [" << decodeBuffer << "], позиция курсора=" << to_string(cursor);

			cursor += 2; //Пропускам позицию для следующей цифры

			if (debugMode)
				cout << "\nПозиция курсора= " << to_string(cursor) << endl;
		}
	}

	if (debugMode)
		cout << "Коррекция..." << endl;

	//Костыль для сокрытия говнокода
	for (uint8_t a = 15;a > 0;--a)
	{
		if (debugMode)
			cout << "Проход №" << to_string(a) << endl;

		if (decodeBuffer[a] == ',')	//Как тольно нашли лишнюю запятую
		{

			if (debugMode)
			{
				cout << "Удаление лишней запятой из позиции " << to_string(a) << "\nПредыдущая позиция " << to_string(a) << ", содержимое [";
				printf("0x%02X]", decodeBuffer[a - 1]);
				cout << endl;
			}

			decodeBuffer[a] = NULL;	//То убираем её
			break;
		}
	}
	cout << "Нарушения по пунктам: " << decodeBuffer << endl;

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
		cout << "Контейнер выходного потока: " << decodeBuffer << endl;

	//Чистим от мусора и восстанавливаем значения
	memset(decodeBuffer, 0x00, 15);
	cursor = 1;

	if (debugMode)
	{
		cout << "Контейнер выходного потока после очистки: " << decodeBuffer << endl;
		cout << "регистр rulBan=";
		printf("0x%02X", rulBan);
		cout << endl;
	}

	//Читаем регистр через лог. И, и в если выражение (регистр И битмаска) = 1, записываем пункт
	for (uint8_t a = 0;a < 8;++a)
	{
		if (debugMode)
		{
			cout << "Проход №" << to_string(a) << " смещение ";
			printf("0x%02X", (rul >> a));
			cout << endl;
		}

		//Двигаем регистр и сравниваем самый младший бит через И
		if (((rulBan >> a)&bitMASK) == 1)
		{
			if (debugMode)
				cout << "Выходной бит=1 ";

			//Стрёмная асмовая хуйня
			__asm
			{

			}
			_t = to_string(a + 1); //Записываем в буфер очеловеченное значение

			if (debugMode)
				cout << "Бит " << to_string(a + 1) << " ";

			decodeBuffer[cursor - 1] = _t[0];	//Переносим из буфера в контейнер

			if (debugMode)
				cout << "\nЗапись в выходной поток [" << _t << "], позиция курсора=" << to_string(cursor - 1);

			decodeBuffer[cursor] = ',';	//Добавляем запятую

			if (debugMode)
				cout << "\nЗапись в выходной поток [" << decodeBuffer << "], позиция курсора=" << to_string(cursor);

			cursor += 2; //Пропускам позицию для следующей цифры

			if (debugMode)
				cout << "\nПозиция курсора= " << to_string(cursor) << endl;
		}
	}

	if (debugMode)
		cout << "Коррекция..." << endl;


	//Костыль для сокрытия говнокода
	for (uint8_t a = 15;a > 0;--a)
	{
		if (debugMode)
			cout << "Проход №" << to_string(a) << endl;

		if (decodeBuffer[a] == ',')
		{

			if (debugMode)
			{
				cout << "Удаление лишней запятой из позиции " << to_string(a) << "\nПредыдущая позиция " << to_string(a) << ", содержимое [";
				printf("0x%02X]", decodeBuffer[a - 1]);
				cout << endl;
			}

			decodeBuffer[a] = NULL;
			break;
		}
	}

RBIT_EXIT:

	cout << "Бан по пунктам: " << decodeBuffer << endl;
}