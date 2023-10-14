#pragma once
//#pragma message("flags.h included")
#ifndef _FLAGS_H_
#define _FLAGS_H_

#include "cpp.h"

class flags//биты состояний - баны, активы и т.п
{
	protected:
		uint8_t statusRegister; //регистр бит-статусов
		flags();
		inline void writeActiveBit(bool arg);
		inline void writeBanBit(bool arg);
		inline void writeTimeoutBit(bool arg);
		void readInputFLAGS();
		void displayFLAGS()const;	//Отчитываемся о статусах в консоль
	public:
		void loadFlags(uint8_t arg);
		void getFlags();
};

#endif