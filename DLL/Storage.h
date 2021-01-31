#pragma once

#include "Action.h"
#include <map>
#include <string>
#include <vector>

#define STORAGE_FILE_PATH       "R:\\Projects\\CoD4\\Shadowjump\\timedactions.sj"

using namespace std;

class Storage
{
private:
    static Storage *s_pInstance;
    fstream *m_pFileStream;
    string m_filePath;
public:
    // This is a multimap because the key will be a timestamp, which is not necessarily unique
    static multimap<UINT, SAction> s_timedActionsMap;

public:
    static Storage *getInstance();

    //void setHeader(); // TODO
    void import();
    void flush();
    void clear();
    void add(UINT, SAction *);

private:
    Storage(string);
    vector<string> split(string, string);
};