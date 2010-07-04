#include "FileHandler.h"

#include <iostream>
using namespace std;


namespace BT2 {


FileHandler::FileHandler(const std::string& diskFileName)
    : m_fileobject(diskFileName), m_algorithm(m_fileobject)
{}

FileHandler::FileHandler(const MetaData& metadata, FileObject::FileState state)
    : m_fileobject(metadata, state), m_algorithm(m_fileobject)
{}

const MetaData& FileHandler::getMetaData() const
{
    return m_fileobject.getMetaData();
}

MetaData& FileHandler::getMetaData()
{
    return m_fileobject.getMetaData();
}

const std::string& FileHandler::getFilename() const
{
    return m_fileobject.getMetaData().fileName;
}

FileObject& FileHandler::getFileObject()
{
    return m_fileobject;
}

PartialAlgorithm& FileHandler::getAlgorithm()
{
    return m_algorithm;
}

Chunk FileHandler::getChunk()
{
    return m_algorithm.getChunk();
}

bool FileHandler::putChunk(const Chunk& c)
{
    bool res = m_algorithm.addChunk(c);
    if (isDone()) {
        m_algorithm.wrapUp();
    }
    return res;
}

bool FileHandler::isEndGame()
{
    return m_algorithm.isEndGame();
}

bool FileHandler::isDone()
{
    return m_algorithm.isDone();
}


} // namespace BT2

