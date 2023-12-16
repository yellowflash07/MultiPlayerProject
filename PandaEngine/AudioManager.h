#pragma once

#include <fmod/fmod.hpp>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include "AudioUtils.h"
#include "MeshManager.h"
#include <iostream>

//holds information about an audio
struct sAudio
{
	std::string Name; //name of audio
	FMOD::Sound* Sound; //fmod sound
	FMOD::Channel* Channel; //fmod channel
	bool IsStreaming; //boolean to check if audio is streaming

	//sets the position and velocity of the audio
	void SetAudioPosition(glm::vec3 position, glm::vec3 velocity)
	{
		FMOD_VECTOR pos;
		FMOD_VECTOR vel;
		GLMToFMOD(position, pos);
		GLMToFMOD(velocity, vel);

		FMOD_RESULT result = Channel->set3DAttributes(&pos, &vel);
		FMODCheckError(result);
	}

	//filters
	FMOD::DSP* reverbDSP;
	FMOD::DSP* highPassDSP;
	FMOD::DSP* lowPassDSP;
	FMOD::DSP* distortionDSP;
	FMOD::DSP* chorusPassDSP;
	FMOD::DSP* echoDSP;
	FMOD::DSP* pitchShiftDSP;

	//sets the reverb of the audio
	void SetReverb(float decay, float density, float diffusion)
	{
		FMOD_RESULT result;

		result = reverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DECAYTIME, decay);
		result = reverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DENSITY, density);
		result = reverbDSP->setParameterFloat(FMOD_DSP_SFXREVERB_DIFFUSION, diffusion);

		FMODCheckError(result);
	}

	//sets the high pass of the audio
	void SetHighPass(float cutoff)
	{
		FMOD_RESULT result = highPassDSP->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, cutoff);
		FMODCheckError(result);
	}

	//sets the low pass of the audio
	void SetLowPass(float cutoff)
	{
		FMOD_RESULT result = lowPassDSP->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, cutoff);
		FMODCheckError(result);
	}

	//sets the distortion of the audio
	void SetDistortion(float distortion)
	{
		FMOD_RESULT result = distortionDSP->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, distortion);
		FMODCheckError(result);
	}

	//sets the echo of the audio
	void SetEcho(float delay, float feedback, float wetLevel, float dryLevel)
	{
		FMOD_RESULT result;

		result = echoDSP->setParameterFloat(FMOD_DSP_ECHO_DELAY, delay);
		result = echoDSP->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, feedback);
		result = echoDSP->setParameterFloat(FMOD_DSP_ECHO_WETLEVEL, wetLevel);
		result = echoDSP->setParameterFloat(FMOD_DSP_ECHO_DRYLEVEL, dryLevel);

		FMODCheckError(result);
	}

	//sets the pitch shift of the audio
	void SetPitchShift(float pitch)
	{
		FMOD_RESULT result = pitchShiftDSP->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
		FMODCheckError(result);
	}
};

//enum for filter types
enum FilterType
{
	REVERB,
	LOWPASS,
	HIGHPASS,
	DISTORTION,
	CHORUS,
	ECHO,
	PITCHSHIFT
};

//integrates FMOD and manages relevant output
class cAudioManager
{
public:
	cAudioManager(MeshManager* meshManager);
	~cAudioManager();

	void Initialize(); //initiliazes fmod
	void Update(); //updates fmod every tick

	//loads audio from a text file. boolean to set loading as streaming or not. 
	//Text files are $(SolutionDir)MusicList.txt for sampled and $(SolutionDir)MusicListStreaming.txt for streaming
	void LoadAudioFromText(std::string filePath, bool streaming); 

	void SetBasePath(std::string filePath); //sets the base path for audio files

	//loads audio at directory. boolean to set loading as streaming or not. 
	sAudio* AddAudioToPlaylist(std::string fileName, bool streaming, bool is3D);

	//plays a audio by taking an input string
	void PlayAudio(std::string name, bool loop);

	//plays a audio by going through index
	void PlayAudioByIndex(int index);

	//plays the next audio in memory/stream
	void PlayNextAudio();

	//plays the last played audio
	void PlayPreviousAudio();

	//pause the audio
	void PauseAudio(bool toggle);
	
	//releases fmod resources and other clean ups
	void Shutdown();

	//sets the pitch of the playing audio
	void SetPitch(float pitch);

	//sets the volume of the playing audio
	void SetVolume(float volume);

	//sets the pan of the playing audio
	void SetPan(float pan);

	//gets current playing audio
	sAudio* GetCurrentAudio();

	//gets current playing audio playback position in milliseconds
	void GetPlayBackPosition(unsigned int& position);

	//gets current playing audio total length in milliseconds
	void GetTotalLength(unsigned int& length);

	//boolean to check if anything is playing
	bool IsPlaying;

	//boolean to check if FMOD initialized properly.
	bool IsInitialized;

	//sets the listener attributes
	void SetListenerAttributes(const glm::vec3& position, 
								const glm::vec3& velocity,
								const glm::vec3& forward, 
								const glm::vec3& up);

	//adds geometry to the audio
	void AddGeometry(cMesh* mesh, float direct, float reverb, bool doublesided);

	//adds filter to audio channel of input name and type
	void AddFilter(std::string audioName, FilterType type);

private:
	FMOD::System* system; //fmod system
	FMOD::Geometry* geometry; //fmod geometry
	std::vector<sAudio*> audios; //a vector of audios
	FMOD::Channel* currentChannel; //the channel of current playing audio
	sAudio* currentAudio; //the current playing audio
	std::string basePath; //the base path of audio files

	MeshManager* meshManager; //pointer to meshmanager for adding geometry	

};