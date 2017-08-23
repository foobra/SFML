////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2017 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Audio/AudioDevice.hpp>
#include <SFML/Audio/ALCheck.hpp>
#include <SFML/Audio/Listener.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/System/Singleton.hpp>
#include <memory>


#define inner (Singleton<InnerType>::Get())
struct InnerType
{
    ALCdevice*  audioDevice  = nullptr;
    ALCcontext* audioContext = nullptr;

    float        listenerVolume = 100.f;
    sf::Vector3f listenerPosition {0.f, 0.f, 0.f};
    sf::Vector3f listenerDirection{0.f, 0.f, -1.f};
    sf::Vector3f listenerUpVector {0.f, 1.f, 0.f};
};

namespace sf::priv
{
////////////////////////////////////////////////////////////
AudioDevice::AudioDevice()
{
    // Create the device
    inner.audioDevice = alcOpenDevice(nullptr);

    if (inner.audioDevice)
    {
        // Create the context
        inner.audioContext = alcCreateContext(inner.audioDevice, nullptr);

        if (inner.audioContext)
        {
            // Set the context as the current one (we'll only need one)
            alcMakeContextCurrent(inner.audioContext);

            // Apply the listener properties the user might have set
            float orientation[] = {inner.listenerDirection.x,
                                   inner.listenerDirection.y,
                                   inner.listenerDirection.z,
                                   inner.listenerUpVector.x,
                                   inner.listenerUpVector.y,
                                   inner.listenerUpVector.z};
            alCheck(alListenerf(AL_GAIN, inner.listenerVolume * 0.01f));
            alCheck(alListener3f(AL_POSITION, inner.listenerPosition.x, inner.listenerPosition.y, inner.listenerPosition.z));
            alCheck(alListenerfv(AL_ORIENTATION, orientation));
        }
        else
        {
            err() << "Failed to create the audio context" << std::endl;
        }
    }
    else
    {
        err() << "Failed to open the audio device" << std::endl;
    }
}


////////////////////////////////////////////////////////////
AudioDevice::~AudioDevice()
{
    // Destroy the context
    alcMakeContextCurrent(nullptr);
    if (inner.audioContext)
        alcDestroyContext(inner.audioContext);

    // Destroy the device
    if (inner.audioDevice)
        alcCloseDevice(inner.audioDevice);
}


////////////////////////////////////////////////////////////
bool AudioDevice::isExtensionSupported(const std::string& extension)
{
    // Create a temporary audio device in case none exists yet.
    // This device will not be used in this function and merely
    // makes sure there is a valid OpenAL device for extension
    // queries if none has been created yet.
    std::unique_ptr<AudioDevice> device;
    if (!inner.audioDevice)
        device.reset(new AudioDevice);

    if ((extension.length() > 2) && (extension.substr(0, 3) == "ALC"))
        return alcIsExtensionPresent(inner.audioDevice, extension.c_str()) != AL_FALSE;
    else
        return alIsExtensionPresent(extension.c_str()) != AL_FALSE;
}


////////////////////////////////////////////////////////////
int AudioDevice::getFormatFromChannelCount(unsigned int channelCount)
{
    // Create a temporary audio device in case none exists yet.
    // This device will not be used in this function and merely
    // makes sure there is a valid OpenAL device for format
    // queries if none has been created yet.
    std::unique_ptr<AudioDevice> device;
    if (!inner.audioDevice)
        device.reset(new AudioDevice);

    // Find the good format according to the number of channels
    int format = 0;
    switch (channelCount)
    {
        case 1:  format = AL_FORMAT_MONO16;                    break;
        case 2:  format = AL_FORMAT_STEREO16;                  break;
        case 4:  format = alGetEnumValue("AL_FORMAT_QUAD16");  break;
        case 6:  format = alGetEnumValue("AL_FORMAT_51CHN16"); break;
        case 7:  format = alGetEnumValue("AL_FORMAT_61CHN16"); break;
        case 8:  format = alGetEnumValue("AL_FORMAT_71CHN16"); break;
        default: format = 0;                                   break;
    }

    // Fixes a bug on OS X
    if (format == -1)
        format = 0;

    return format;
}


////////////////////////////////////////////////////////////
void AudioDevice::setGlobalVolume(float volume)
{
    if (inner.audioContext)
        alCheck(alListenerf(AL_GAIN, volume * 0.01f));

    inner.listenerVolume = volume;
}


////////////////////////////////////////////////////////////
float AudioDevice::getGlobalVolume()
{
    return inner.listenerVolume;
}


////////////////////////////////////////////////////////////
void AudioDevice::setPosition(const Vector3f& position)
{
    if (inner.audioContext)
        alCheck(alListener3f(AL_POSITION, position.x, position.y, position.z));

    inner.listenerPosition = position;
}


////////////////////////////////////////////////////////////
Vector3f AudioDevice::getPosition()
{
    return inner.listenerPosition;
}


////////////////////////////////////////////////////////////
void AudioDevice::setDirection(const Vector3f& direction)
{
    if (inner.audioContext)
    {
        float orientation[] = {direction.x, direction.y, direction.z, inner.listenerUpVector.x, inner.listenerUpVector.y, inner.listenerUpVector.z};
        alCheck(alListenerfv(AL_ORIENTATION, orientation));
    }

    inner.listenerDirection = direction;
}


////////////////////////////////////////////////////////////
Vector3f AudioDevice::getDirection()
{
    return inner.listenerDirection;
}


////////////////////////////////////////////////////////////
void AudioDevice::setUpVector(const Vector3f& upVector)
{
    if (inner.audioContext)
    {
        float orientation[] = {inner.listenerDirection.x, inner.listenerDirection.y, inner.listenerDirection.z, upVector.x, upVector.y, upVector.z};
        alCheck(alListenerfv(AL_ORIENTATION, orientation));
    }

    inner.listenerUpVector = upVector;
}


////////////////////////////////////////////////////////////
Vector3f AudioDevice::getUpVector()
{
    return inner.listenerUpVector;
}

} // namespace sf::priv
