//
//  IntelWeb.hpp
//  Project 4
//
//  Created by Cecilia.Wang on 3/6/16.
//  Copyright © 2016 Cecilia.Wang. All rights reserved.
//

#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include "DiskMultiMap.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <set>

class IntelWeb
{
public:
    IntelWeb();
    ~IntelWeb();
    bool createNew(const std::string& filePrefix, unsigned int maxDataItems);
    bool openExisting(const std::string& filePrefix);
    void close();
    bool ingest(const std::string& telemetryFile);
    unsigned int crawl(const std::vector<std::string>& indicators, unsigned int minPrevalenceToBeGood,
                       std::vector<std::string>& badEntitiesFound, std::vector<InteractionTuple>& interactions);
    bool purge(const std::string& entity);
    
private:
    unsigned int numBuckets;
    std::string fp;
    
    //DiskMultimap
    DiskMultiMap KeytoValue;
    std::string fn1;
    DiskMultiMap ValuetoKey;
    std::string fn2;
    
    std::unordered_set<string> bad;
    std::set<InteractionTuple> interaction;
    
    int getPrevalence(const std::string& key);
};
#endif // INTELWEB_H_