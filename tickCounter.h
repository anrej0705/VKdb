#ifndef _TICKCOUNTER_H_
#define _TICKCOUNTER_H_
class tickCounter
{
	private:
		uint64_t staCount;
		uint64_t stoCount;
	public:
		uint64_t exportCnt;
		void startCount();
		void stopCount();
		uint64_t returnValue();
};
#endif;
