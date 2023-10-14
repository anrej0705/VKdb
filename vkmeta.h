#pragma once
//#pragma message("vkmeta.h included")
#ifndef _VKMETA_H_
#define _VKMETA_H_

#include "cpp.h"

#define VKID_length		10		//Байтовая длина поля ID в вк
#define VKURL_length	32		//Байтовая длина ника в вк
#define VKUINFO_length	40		//Байтовая длина имени/фамилии в вк

class vkMeta//контейнер данных из ВК
{
	private:
		bool checkNcopy(char *inputArg, char *destination); //проверяем чтобы в потоке были только цифры
	protected:
		char vkId[VKID_length];	//ИД пользователя
		char vkUrl[VKURL_length];	//ник пользователя
		char vkName[VKUINFO_length];	//Имя пользователя
		char vkFam[VKUINFO_length];	//Фамилия пользователя
		vkMeta();
		vkMeta(char vId[VKID_length], char vUrl[VKURL_length], char vNam[VKUINFO_length], char vFam[VKUINFO_length]);
		void writeId(char arg[VKID_length]);
		void writeUrl(char arg[VKURL_length]);
		void writeName(char arg[VKUINFO_length]);
		void writeFam(char arg[VKUINFO_length]);
		void readInputVKMETA();
		void displayVKMETA()const;
	public:
		void getVKIDchar();
		void getVKurl();
};

#endif