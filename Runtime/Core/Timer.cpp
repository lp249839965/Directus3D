/*
Copyright(c) 2016-2018 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ========
#include "Timer.h"
#include "Engine.h"
#include "Settings.h"
#include <thread>
//===================

//= NAMESPACES ========
using namespace std;
using namespace chrono;
//=====================

namespace Directus
{
	Timer::Timer(Context* context) : Subsystem(context)
	{
		time_a			= high_resolution_clock::now();
		time_b			= high_resolution_clock::now();
		m_deltaTimeMs	= 0.0f;
	}

	void Timer::Tick()
	{
		// Compute work time
		time_a								= high_resolution_clock::now();
		duration<double, milli> time_work	= time_a - time_b;
		
		// Compute sleep time (fps limiting)
		bool isEditor		= !Engine::EngineMode_IsSet(Engine_Game);
		auto maxFPS_editor	= (double)Settings::Get().MaxFps_GetEditor();
		auto maxFPS_game	= (double)Settings::Get().MaxFps_GetGame();
		double maxFPS		= isEditor ? maxFPS_editor : maxFPS_game;
		double maxMs		= (1.0 / maxFPS) * 1000;
		if (time_work.count() < maxMs)
		{
			duration<double, milli> time_ms(maxMs - time_work.count());
			auto time_ms_duration = duration_cast<milliseconds>(time_ms);
			this_thread::sleep_for(milliseconds(time_ms_duration.count()));
		}

		// Compute delta
		time_b								= high_resolution_clock::now();
		duration<double, milli> time_sleep	= time_b - time_a;
		m_deltaTimeMs						= (time_work + time_sleep).count();
	}
}
