#ifndef EAE6320_TIME_H
#define EAE6320_TIME_H

namespace eae6320
{
	namespace Time
	{
		float GetElapsedSecondCount_total();
		float GetElapsedSecondCount_duringPreviousFrame();

		void OnNewFrame();
		bool Initialize();
		bool CleanUp();
	}
}

#endif
