//
//  DiskMultiMap.hpp
//  Project 4
//
//  Created by Cecilia.Wang on 3/5/16.
//  Copyright © 2016 Cecilia.Wang. All rights reserved.
//

#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include <list>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

class DiskMultiMap
{
public:
    
    class Iterator
    {
    public:
        Iterator();
        Iterator(DiskMultiMap* map, list<BinaryFile::Offset> position);
        bool isValid() const;
        Iterator& operator++();
        MultiMapTuple operator*();
        
    private:
        std::list<BinaryFile::Offset> pos;
        DiskMultiMap* m_map;
    };
    
    DiskMultiMap();
    ~DiskMultiMap();
    bool createNew(const std::string& filename, unsigned int numBuckets);
    bool openExisting(const std::string& filename);
    void close();
    bool insert(const std::string& key, const std::string& value, const std::string& context);
    Iterator search(const std::string& key);
    int erase(const std::string& key, const std::string& value, const std::string& context);
    
private:
    struct Head{
        unsigned int numBucket;
        BinaryFile::Offset prev_delete;
    };
    struct DiskNode{
        char m_key[121];
        char m_value[121];
        char m_context[121];
        BinaryFile::Offset m_next;
    };
    BinaryFile hashTable;
    std::string fn;
    
    void findBucket(const std::string& key, const Head head, BinaryFile::Offset& bucket, BinaryFile::Offset& bucketPos);
};

#endif // DISKMULTIMAP_H_