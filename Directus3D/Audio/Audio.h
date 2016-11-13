/*
Copyright(c) 2016 Panos Karabelas

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

#pragma once

//= LINKING =========================
#pragma comment(lib, "fmod64_vc.lib")
//===================================

//= INCLUDES ==============
#include "../Core/Object.h"
#include "fmod.hpp"
#include <map>
//=========================

struct SoundHandle
{
	FMOD::Sound* sound;
	FMOD::Channel* channel;
	bool isSound;
};

class Audio : public Object
{
public:
	Audio(Context* context);
	~Audio();

	bool Initialize();
	void Update();

	void CreateSound(const std::string& filePath);
	void CreateStream(const std::string& filePath);
	bool Play(const std::string& filePath);
	bool Stop(const std::string& filePath);
	bool SetVolume(float volume, const std::string& filePath);

private:
	FMOD_RESULT m_result;
	FMOD::System* m_fmodSystem;
	int m_maxChannels;
	float m_distanceFactor;
	bool m_initialized;
	
	std::map<std::string, SoundHandle*> m_sounds;
	SoundHandle* GetSoundByPath(const std::string& filePath);
};