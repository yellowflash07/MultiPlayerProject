#include "AudioManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "AudioUtils.h"
#include <iostream>
#include <glm/glm.hpp>

cAudioManager::cAudioManager(MeshManager* meshManager)
{
    currentChannel = 0;
    currentAudio = 0;
    IsPlaying = false;
    this->meshManager = meshManager;
}

cAudioManager::~cAudioManager()
{
}

void cAudioManager::Initialize()
{
    FMOD_RESULT result;
    system = NULL;

    result = FMOD::System_Create(&system);      // Create the main system object.
    if (result != FMOD_OK)
    {
        std::cerr << "FMOD creation error!" << std::endl; //Handle Error
        return;
    }

    result = system->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        std::cerr << "FMOD initialization error!" << std::endl; //Handle Error
        return;
    }

    audios.resize(0);

    result = system->set3DSettings(.5f, .5f, 0.1f);
    result = system->setGeometrySettings(1000.0f);
    FMODCheckError(result);
    result = system->createGeometry(10000,100000, &geometry);

    IsInitialized = true; //Initialized!
}

void cAudioManager::Update()
{
    if (!IsInitialized) return; 
    system->update(); //updates the fmod system
}

void cAudioManager::LoadAudioFromText(std::string filePath, bool streaming)
{
    if (!IsInitialized) return;
    std::ifstream inputFile(filePath); //stream input filepath

    //check if file opening was successful
    if (!inputFile.is_open()) 
    {
        std::cerr << "Failed to open the file." << std::endl; 
        return;
    }

    //read each line
    std::string line;

    while (std::getline(inputFile, line)) 
    {
        std::cout << "Reading " << line << std::endl;
        //parse the filepath at line and load it
        AddAudioToPlaylist(line, streaming, false);
    }

    //closes stream
    inputFile.close();

}

void cAudioManager::SetBasePath(std::string filePath)
{
    this->basePath = filePath;
}

void cAudioManager::SetListenerAttributes(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up)
{
    if (!IsInitialized) return;
	FMOD_RESULT result;
    FMOD_VECTOR fmodPosition;
    FMOD_VECTOR fmodVelocity;
    FMOD_VECTOR fmodForward;
    FMOD_VECTOR fmodUp;

    GLMToFMOD(position, fmodPosition);
    GLMToFMOD(velocity, fmodVelocity);
    GLMToFMOD(glm::normalize(forward), fmodForward);
    GLMToFMOD(up, fmodUp);

    if (isnan(fmodForward.x) || isnan(fmodForward.y) || isnan(fmodForward.z))
    {
        fmodForward.x = 0;
        fmodForward.y = 0;
        fmodForward.z = 1;
    }
	//set the listener attributes
	result = system->set3DListenerAttributes(0, 
                                            &fmodPosition, 
                                            &fmodVelocity,
                                            &fmodForward, 
                                            &fmodUp);
//    std::cout << fmodForward.x << "," << fmodForward.y << "," << fmodForward.z << std::endl;
	FMODCheckError(result);
}

void cAudioManager::AddGeometry(cMesh* mesh, float direct, float reverb, bool doublesided)
{
    if (!IsInitialized) return;
	FMOD_RESULT result;
    sModelDrawInfo drawInfo;

    if (!meshManager->GetModelDrawInfo(mesh->friendlyName, drawInfo))
    {
		std::cout << "Could not find mesh " << mesh->friendlyName << std::endl;
		return;
	}

    FMOD_VECTOR* vertices = new FMOD_VECTOR[drawInfo.numberOfVertices];

    for (size_t i = 0; i < drawInfo.numberOfVertices; i++)
    {
        glm::vec3 vertexPos = glm::vec3(drawInfo.pVertices[i].x, 
                                        drawInfo.pVertices[i].y, 
                                        drawInfo.pVertices[i].z);
        GLMToFMOD(vertexPos, vertices[i]);
    }

    result = geometry->addPolygon(direct, reverb, doublesided,
                                    drawInfo.numberOfVertices,
                                    vertices,0);
    FMODCheckError(result);

    FMOD_VECTOR fmodPosition;
    GLMToFMOD(mesh->drawPosition, fmodPosition);
    result = geometry->setPosition(&fmodPosition);
    FMODCheckError(result);

    FMOD_VECTOR fmodScale;
    GLMToFMOD(mesh->drawScale, fmodScale);
    result = geometry->setScale(&fmodScale);
    FMODCheckError(result);

    geometry->setActive(true);
}

sAudio* cAudioManager::AddAudioToPlaylist(std::string fileName, bool streaming, bool is3D)
{
    std::string filePath = basePath + "/" + fileName; //concatenate the base path and the file name
    std::ifstream inputFile(filePath); //stream input filepath

    //check if file opening was successful
    if (!inputFile.is_open())
    {
        std::cerr << "Failed to open the filepath." << std::endl;
        inputFile.close();
        return nullptr;
    }

    //init a sound
    FMOD::Sound* sound = 0;
    FMOD_RESULT result;
    
    if (streaming) //is this audio supposed to stream?
    {
        //creates audio as stream
        if (is3D)
        {
            result = system->createStream(filePath.c_str(), FMOD_3D, 0, &sound);
        }
        else
        {
            result = system->createStream(filePath.c_str(), FMOD_DEFAULT, 0, &sound);
        }
    }
    else
    {
        //creates audio as sample
        if (is3D)
        {
            result = system->createSound(filePath.c_str(), FMOD_3D, 0, &sound);
        }
        else
        {
            result = system->createSound(filePath.c_str(), FMOD_DEFAULT, 0, &sound);
        }
    }
    FMODCheckError(result);

    sAudio* audio = new sAudio(); //init an audio struct

    //takes the music file name out instead of the entire directory path
    std::istringstream ss(filePath);
    std::string name = filePath.substr(filePath.find_last_of("/\\")+1);

    //set the music file name
    audio->Name = name;
    //set the audio created
    audio->Sound = sound;
   
    audio->IsStreaming = streaming;

    //successful result messages
    std::string loadedOrStreaming = streaming ? "Streaming " : "Loaded ";
    std::cout << loadedOrStreaming << audio->Name << std::endl;

    //adds the successfully created audio to a vector of audio
    audios.push_back(audio);

    return audio;
}

void cAudioManager::PlayAudio(std::string name, bool loop)
{
    if (!IsInitialized) return;
    if (audios.size() == 0) return;
    for (int i = 0; i < audios.size(); i++)
    {
        if (audios[i]->Name == name)
        {
            //play the sound on match
            FMOD_RESULT result = system->playSound(audios[i]->Sound, 0, false, 
                                                    &audios[i]->Channel);
            FMODCheckError(result);
            std::cout << "Playing " << audios[i]->Name << std::endl;

            //set the current channel, audio to the playing channel
            currentChannel = audios[i]->Channel;
            currentAudio = audios[i];

            if (loop)
            {
                FMOD_MODE mode;
                audios[i]->Sound->getMode(&mode);
                mode = FMOD_3D | FMOD_LOOP_NORMAL;
                result = audios[i]->Sound->setMode(mode);
                FMODCheckError(result);
                result = audios[i]->Channel->setMode(mode);
                FMODCheckError(result);
            }

            IsPlaying = true;
        }
    }
}

void cAudioManager::PlayAudioByIndex(int index)
{
    if (!IsInitialized) return;
    if (audios.size() == 0 || index  < 0 || index > audios.size()) return;
    //plays a valid input index of the audio array
    FMOD_RESULT result = system->playSound(audios[index]->Sound, 0, false, &audios[index]->Channel);
    FMODCheckError(result);
    currentChannel = audios[index]->Channel;
    currentAudio = audios[index];
}

void cAudioManager::PlayNextAudio()
{

    if (!IsInitialized) return;
    if (audios.size() == 0) return;
    for (int i = 0; i < audios.size(); i++)
    {
        if (audios[i]->Name == currentAudio->Name)
        {
            if (i + 1 < audios.size())
            {
                currentChannel->stop();
                PlayAudio(audios[i + 1]->Name, false);
                break;
            }
        }
    }
}

void cAudioManager::PlayPreviousAudio()
{
    if (!IsInitialized) return;
    if (audios.size() == 0) return;
    for (int i = 0; i < audios.size(); i++)
    {
        if (audios[i]->Name == currentAudio->Name)
        {
            if (i - 1 >= 0)
            {
                currentChannel->stop();
                PlayAudio(audios[i - 1]->Name, false);
                break;
            }
        }
    }
}

void cAudioManager::PauseAudio(bool toggle)
{
    if (!IsInitialized) return;
    //set fmod channel pause value
    FMOD_RESULT result = currentChannel->setPaused(toggle);
    FMODCheckError(result);
}


void cAudioManager::Shutdown()
{
    if (!IsInitialized) return;
    FMOD_RESULT result;

    if (currentChannel)
    {
        //stop the channel
        result = currentChannel->stop();
        FMODCheckError(result);
    }
  
    if (currentAudio)
    {
        //release the audio
        result = currentAudio->Sound->release();
        FMODCheckError(result);
    }


	for (int i = 0; i < audios.size(); i++)
	{
        //release all loaded sounds
        audios[i]->Sound->release();
        //delete the struct holding the sound
		delete audios[i];
	}
    //release the system
    result = system->release();
    FMODCheckError(result);

    std::cout << "Audio Manager Shutdown" << std::endl;
}

void cAudioManager::SetPitch(float pitch)
{
    if (!IsInitialized) return;
 
    FMOD_RESULT result = currentChannel->setPitch(pitch);
    FMODCheckError(result);
}

void cAudioManager::SetVolume(float volume)
{
    if (!IsInitialized && !currentChannel) return;
    FMOD_RESULT result = currentChannel->setVolume(volume);
    FMODCheckError(result);
}

void cAudioManager::SetPan(float pan)
{
    if (!IsInitialized || !currentChannel) return;
    FMOD_RESULT result = currentChannel->setPan(pan);
    FMODCheckError(result);
}

sAudio* cAudioManager::GetCurrentAudio()
{
    if (!IsInitialized || !currentAudio) return nullptr;
    return currentAudio;
}

void cAudioManager::GetPlayBackPosition(unsigned int& position)
{
    if (!IsInitialized || !currentChannel) return;
   currentChannel->getPosition(&position, FMOD_TIMEUNIT_MS);
}

void cAudioManager::GetTotalLength(unsigned int& length)
{
    if (!IsInitialized || !currentAudio) return;
    currentAudio->Sound->getLength(&length, FMOD_TIMEUNIT_MS);
}

void cAudioManager::AddFilter(std::string audioName, FilterType type)
{
    for (int i = 0; i < audios.size(); i++)
    {
        if (audios[i]->Name == audioName)
        {
            switch (type)
            {
                case REVERB:
                {
				    FMOD_RESULT result;
				    result = system->createDSPByType(FMOD_DSP_TYPE_SFXREVERB, &audios[i]->reverbDSP);
				    FMODCheckError(result);
				    result = audios[i]->Channel->addDSP(0, audios[i]->reverbDSP);
				    FMODCheckError(result);
				    break;
                }
                case CHORUS:
                {
                    FMOD_RESULT result;
                    result = system->createDSPByType(FMOD_DSP_TYPE_CHORUS, &audios[i]->chorusPassDSP);
                    FMODCheckError(result);
                    result = audios[i]->Channel->addDSP(0, audios[i]->chorusPassDSP);
                    FMODCheckError(result);
                    break;
                }
                case HIGHPASS:
                {
					FMOD_RESULT result;
					result = system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &audios[i]->highPassDSP);
					FMODCheckError(result);
					result = audios[i]->Channel->addDSP(0, audios[i]->highPassDSP);
					FMODCheckError(result);
					break;
				}
                case LOWPASS:
                {
                    FMOD_RESULT result;
                    result = system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &audios[i]->lowPassDSP);
                    FMODCheckError(result);
                    result = audios[i]->Channel->addDSP(0, audios[i]->lowPassDSP);
                    FMODCheckError(result);
                    break;
                }
                case DISTORTION:
                {
                    FMOD_RESULT result;
                    result = system->createDSPByType(FMOD_DSP_TYPE_DISTORTION, &audios[i]->distortionDSP);
                    FMODCheckError(result);
                    result = audios[i]->Channel->addDSP(0, audios[i]->distortionDSP);
                    FMODCheckError(result);
                    break;
                }
                case ECHO:
                {
                    FMOD_RESULT result;
					result = system->createDSPByType(FMOD_DSP_TYPE_ECHO, &audios[i]->echoDSP);
					FMODCheckError(result);
					result = audios[i]->Channel->addDSP(0, audios[i]->echoDSP);
					FMODCheckError(result);
					break;
                }
                case PITCHSHIFT:
                {
                    FMOD_RESULT result;
                    result = system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &audios[i]->pitchShiftDSP);
                    FMODCheckError(result);
                    result = audios[i]->Channel->addDSP(0, audios[i]->pitchShiftDSP);
                    FMODCheckError(result);
                    break;
                }
            }
        }
    }
}


