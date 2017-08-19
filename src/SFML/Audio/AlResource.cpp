////////////////////////////////////////////
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
#include <SFML/Audio/AlResource.hpp>
#include <SFML/System/Singleton.hpp>
#include <SFML/Audio/AudioDevice.hpp>
#include <SFML/System/SmartPointerDefines.hpp>


namespace sf
{


#define inner (Singleton<InnerType>::Get())
struct InnerType
{
    // The audio device is instantiated on demand rather than at global startup,
    // which solves a lot of weird crashes and errors.
    // It is destroyed when it is no longer needed.
    WP<sf::priv::AudioDevice> globalDevice;
};

////////////////////////////////////////////////////////////
AlResource::AlResource()
{
    // If this is the very first resource, trigger the global device initialization
    if (auto device = inner.globalDevice.lock())
    {
        deviceHolder = device;
    }
    else
    {
        deviceHolder = MakeSP<sf::priv::AudioDevice>();
        inner.globalDevice = deviceHolder;
    }
}



} // namespace sf
