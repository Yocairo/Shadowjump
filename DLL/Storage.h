#pragma once

#include "Action.h"
#include <map>
#include <string>

#define STORAGE_FILE_PATH       "R:\\Projects\\CoD4\\Shadowjump\\timedactions.sj"

using namespace std;

class Storage
{
private:
    static Storage *s_pInstance;
    // This is a multimap because the key will be a timestamp, which is not necessarily unique
    multimap<ULONGLONG, SAction> m_timedActionsMap;
    ofstream *m_pOutputFileStream;
    string m_outputFilePath;

public:
    static Storage *getInstance();

    //void setHeader(); // TODO
    void flush();
    void clear();
    void add(ULONGLONG, SAction *);

private:
    Storage(string);
};