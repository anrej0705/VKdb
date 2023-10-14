#pragma once
#ifndef _BASELOCALE_H_
#define _BASELOCALE_H_

//Файл базовой локали, подставляемый в случае отсустствия запрошенного файла локали
struct baseLocale
{
	char sign[15] = "Базовая локаль";
};
#endif