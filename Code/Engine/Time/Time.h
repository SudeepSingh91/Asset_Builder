#ifndef TIME_H
#define TIME_H

namespace Engine
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
