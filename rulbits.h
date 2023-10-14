#pragma once
//#pragma message("rulbits.h included")
#ifndef _RULBITS_H_
#define _RULBITS_H_

#include "cpp.h"

class rulBits	//бит нарушений
{
	private:
		uint8_t convertString(string _inputArg);	//Запаковка пользовательского ввода для экономии места
	protected:
		uint8_t rulBan;		//биты нарушений приведшие к бану
		uint8_t rul;		//биты нарушений
		//bool dummy;			//Козёл отпущения
	public:
		rulBits();
		rulBits(uint8_t r, uint8_t rBan) : rul(r), rulBan(rBan)
		{}
		void writeRulPr(uint8_t arg);
		void writeRulBan(uint8_t arg);
		uint8_t getRulBits();
		uint8_t getRulBanBits();
		void clearBits();
		void readInputRBIT();	//Опрос оператора по пунткам нарушений и/или бана
		void importRBIT(string _inputRuls, string _inputBans);
		void displayRBIT()const;	//Читаем регистр и показываем что нарушено а за что бан в ебало
		void getRBIT();
};

#endif