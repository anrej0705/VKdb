#pragma once
//#pragma message("rem.h included")
#ifndef _REM_H_
#define _REM_H_

#include "cpp.h"

#define REM_length		80		//Байтовая длина поля примечания к записи

class rem	//пометки в записях
{
	protected:
		char dbRem[REM_length];	//пользовательская метка в базе о записи
	public:
		rem();
		rem(char remark[REM_length]);
		void writeRemark(char arg[REM_length]);
		void readInputREM();
		void displayREM()const;
		void getREM();
};

#endif