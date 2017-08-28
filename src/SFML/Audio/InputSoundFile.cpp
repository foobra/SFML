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
#include <SFML/Audio/InputSoundFile.hpp>
#include <SFML/Audio/SoundFileReader.hpp>
#include <SFML/Audio/SoundFileFactory.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/FileInputStream.hpp>
#include <SFML/System/MemoryInputStream.hpp>
#include <SFML/System/Err.hpp>
#include <algorithm>


namespace sf
{
    struct InputSoundFile::impl
    {
        ////////////////////////////////////////////////////////////
        /// \brief Close the current file
        ///
        ////////////////////////////////////////////////////////////
        void close()
        {
            // Destroy the reader
            delete m_reader;
            m_reader = nullptr;

            // Destroy the stream if we own it
            if (m_streamOwned)
            {
                delete m_stream;
                m_streamOwned = false;
            }
            m_stream = nullptr;
            m_sampleOffset = 0;

            // Reset the sound file attributes
            m_sampleCount = 0;
            m_channelCount = 0;
            m_sampleRate = 0;
        }

        ////////////////////////////////////////////////////////////
        // Member data
        ////////////////////////////////////////////////////////////
        sf::SoundFileReader* m_reader; ///< Reader that handles I/O on the file's format
        sf::InputStream* m_stream; ///< Input stream used to access the file's data
        bool m_streamOwned; ///< Is the stream internal or external?
        sf::Uint64 m_sampleOffset; ///< Sample Read Position
        sf::Uint64 m_sampleCount; ///< Total number of samples in the file
        unsigned int m_channelCount; ///< Number of channels of the sound
        unsigned int m_sampleRate; ///< Number of samples per second
    };



InputSoundFile::InputSoundFile():
pimpl(std::make_unique<impl>())
{
}



////////////////////////////////////////////////////////////
InputSoundFile::~InputSoundFile()
{
    // Close the file in case it was open
    pimpl->close();
}


////////////////////////////////////////////////////////////
bool InputSoundFile::openFromFile(const std::string& filename)
{
    // If the file is already open, first close it
    pimpl->close();

    // Find a suitable reader for the file type
    pimpl->m_reader = SoundFileFactory::createReaderFromFilename(filename);
    if (!pimpl->m_reader)
        return false;

    // Wrap the file into a stream
    FileInputStream* file = new FileInputStream;
    pimpl->m_stream = file;
    pimpl->m_streamOwned = true;

    // Open it
    if (!file->open(filename))
    {
        pimpl->close();
        return false;
    }

    // Pass the stream to the reader
    SoundFileReader::Info info;
    if (!pimpl->m_reader->open(*file, info))
    {
        pimpl->close();
        return false;
    }

    // Retrieve the attributes of the open sound file
    pimpl->m_sampleCount = info.sampleCount;
    pimpl->m_channelCount = info.channelCount;
    pimpl->m_sampleRate = info.sampleRate;

    return true;
}


////////////////////////////////////////////////////////////
bool InputSoundFile::openFromMemory(const void* data, std::size_t sizeInBytes)
{
    // If the file is already open, first close it
    pimpl->close();

    // Find a suitable reader for the file type
    pimpl->m_reader = SoundFileFactory::createReaderFromMemory(data, sizeInBytes);
    if (!pimpl->m_reader)
        return false;

    // Wrap the memory file into a stream
    MemoryInputStream* memory = new MemoryInputStream;
    pimpl->m_stream = memory;
    pimpl->m_streamOwned = true;

    // Open it
    memory->open(data, sizeInBytes);

    // Pass the stream to the reader
    SoundFileReader::Info info;
    if (!pimpl->m_reader->open(*memory, info))
    {
        pimpl->close();
        return false;
    }

    // Retrieve the attributes of the open sound file
    pimpl->m_sampleCount = info.sampleCount;
    pimpl->m_channelCount = info.channelCount;
    pimpl->m_sampleRate = info.sampleRate;

    return true;
}


////////////////////////////////////////////////////////////
bool InputSoundFile::openFromStream(InputStream& stream)
{
    // If the file is already open, first close it
    pimpl->close();

    // Find a suitable reader for the file type
    pimpl->m_reader = SoundFileFactory::createReaderFromStream(stream);
    if (!pimpl->m_reader)
        return false;

    // store the stream
    pimpl->m_stream = &stream;
    pimpl->m_streamOwned = false;

    // Don't forget to reset the stream to its beginning before re-opening it
    if (stream.seek(0) != 0)
    {
        err() << "Failed to open sound file from stream (cannot restart stream)" << std::endl;
        return false;
    }

    // Pass the stream to the reader
    SoundFileReader::Info info;
    if (!pimpl->m_reader->open(stream, info))
    {
        pimpl->close();
        return false;
    }

    // Retrieve the attributes of the open sound file
    pimpl->m_sampleCount = info.sampleCount;
    pimpl->m_channelCount = info.channelCount;
    pimpl->m_sampleRate = info.sampleRate;

    return true;
}


////////////////////////////////////////////////////////////
Uint64 InputSoundFile::getSampleCount() const
{
    return pimpl->m_sampleCount;
}


////////////////////////////////////////////////////////////
unsigned int InputSoundFile::getChannelCount() const
{
    return pimpl->m_channelCount;
}


////////////////////////////////////////////////////////////
unsigned int InputSoundFile::getSampleRate() const
{
    return pimpl->m_sampleRate;
}


////////////////////////////////////////////////////////////
Time InputSoundFile::getDuration() const
{
    // Make sure we don't divide by 0
    if (pimpl->m_channelCount == 0 || pimpl->m_sampleRate == 0)
        return Time::Zero;

    return seconds(static_cast<float>(pimpl->m_sampleCount) / pimpl->m_channelCount / pimpl->m_sampleRate);
}


////////////////////////////////////////////////////////////
Time InputSoundFile::getTimeOffset() const
{
    // Make sure we don't divide by 0
    if (pimpl->m_channelCount == 0 || pimpl->m_sampleRate == 0)
        return Time::Zero;

    return seconds(static_cast<float>(pimpl->m_sampleOffset) / pimpl->m_channelCount / pimpl->m_sampleRate);
}


////////////////////////////////////////////////////////////
Uint64 InputSoundFile::getSampleOffset() const
{
    return pimpl->m_sampleOffset;
}


////////////////////////////////////////////////////////////
void InputSoundFile::seek(Uint64 sampleOffset)
{
    if (pimpl->m_reader)
    {
        // The reader handles an overrun gracefully, but we
        // pre-check to keep our known position consistent
        pimpl->m_sampleOffset = std::min(sampleOffset, pimpl->m_sampleCount);
        pimpl->m_reader->seek(pimpl->m_sampleOffset);
    }
}


////////////////////////////////////////////////////////////
void InputSoundFile::seek(Time timeOffset)
{
    seek(static_cast<Uint64>(timeOffset.asSeconds() * pimpl->m_sampleRate * pimpl->m_channelCount));
}


////////////////////////////////////////////////////////////
Uint64 InputSoundFile::read(Int16* samples, Uint64 maxCount)
{
    Uint64 readSamples = 0;
    if (pimpl->m_reader && samples && maxCount)
        readSamples = pimpl->m_reader->read(samples, maxCount);
    pimpl->m_sampleOffset += readSamples;
    return readSamples;
}


////////////////////////////////////////////////////////////

} // namespace sf
