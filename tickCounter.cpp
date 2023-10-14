#include <stdint.h>
#include "tickCounter.h"
using namespace std;

uint64_t tickCounter::returnValue()
{
	exportCnt=stoCount-staCount;
	return exportCnt;
}
void tickCounter::startCount()
{
	register uint64_t temp;
	__asm 
	{
		CPUID
		RDTSC
		mov DWORD PTR[temp], eax
		mov DWORD PTR[temp+4], edx
	}
	staCount=temp;
}
void tickCounter::stopCount()
{
	register uint64_t temp;
	__asm 
	{
		CPUID
		RDTSC
		mov DWORD PTR[temp], eax
		mov DWORD PTR[temp+4], edx
	}
	stoCount=temp;
}
