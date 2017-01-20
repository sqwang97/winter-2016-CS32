//
//  DiskMultiMap.cpp
//  Project 4
//
//  Created by Cecilia.Wang on 3/5/16.
//  Copyright © 2016 Cecilia.Wang. All rights reserved.
//

#include "DiskMultiMap.h"
#include <iostream>
using namespace std;

DiskMultiMap::DiskMultiMap()
{}

DiskMultiMap::~DiskMultiMap()
{
    close();
}

/*
 create a binary file with a head structure(numBucket and prev delete node)
 prev_delete->most recent deleted location. m_next->the deleted location before...-> -1
 
 add an array to keep track of the real offset of each bucket
 */
bool DiskMultiMap::createNew(const string& filename, unsigned int numBuckets)
{
    close();
    
    hashTable.createNew(filename);
    fn = filename;
    
    Head head;
    head.numBucket = numBuckets;
    head.prev_delete = -1;
    if (!hashTable.write(head, 0)){
        close();
        return false;
    }
    
    BinaryFile::Offset B[numBuckets];
    for (int i = 0; i < numBuckets; i++){
        B[i] = hashTable.fileLength(); //pos of the bucket itself
        if(!hashTable.write(B[i], hashTable.fileLength())){
            close();
            return false;
        }
    }
    return true;
}

bool DiskMultiMap::openExisting(const string& filename)
{
    close();
    
    fn = filename;
    if (hashTable.openExisting(filename))
        return true;
    return false;
}


void DiskMultiMap::close()
{
    hashTable.close();
}

/*
 bucket -> most recent insert. m_next -> previous insert... -> bucket offset
 */
bool DiskMultiMap::insert(const string& key, const string& value, const string& context)
{
    if (strlen(key.c_str()) > 120 || strlen(value.c_str()) > 120 || strlen(context.c_str()) > 120 || !openExisting(fn))
        return false;
    
    //open the head node
    Head head;
    hashTable.read(head, 0);
    
    //find the bucket for the key
    BinaryFile::Offset node, bucketPos;
    findBucket(key, head, node, bucketPos);
    if(!hashTable.read(node, bucketPos))
        return false;
    
    //find real pos to insert
    DiskNode temp;
    BinaryFile::Offset tempPos = head.prev_delete;
    if(tempPos == -1)                                 // no empty slot, need to expand the size
        tempPos = hashTable.fileLength();
    else{                                             //update prev_delete in head
        hashTable.read(temp, head.prev_delete);
        head.prev_delete = temp.m_next;
        hashTable.write(head, 0);
    }
    
    //initialize the new node
    strcpy(temp.m_key, key.c_str());
    strcpy(temp.m_value, value.c_str());
    strcpy(temp.m_context, context.c_str());
    temp.m_next = node;
    
    //add the new node into the list
    if(!hashTable.write(temp, tempPos))
        return false;
    
    //update hash table array
    if(!hashTable.write(tempPos, bucketPos))
        return false;
    
    //cout << "current_pos: " << tempPos << " ||m_next: " << temp.m_next << endl;
    return true;
}

DiskMultiMap::Iterator DiskMultiMap::search(const string& key)
{
    Iterator temp;
    list<BinaryFile::Offset> position;
    if(strlen(key.c_str()) > 120 || !openExisting(fn))
        return temp;
    
    Head head;
    hashTable.read(head, 0);
    BinaryFile::Offset bucket, bucketPos;
    findBucket(key, head, bucket, bucketPos);
    hashTable.read(bucket, bucketPos);
    
    if (bucket == bucketPos)
        return temp;
    
    DiskNode node;
    BinaryFile::Offset node_pos = bucket; //get the position of first node
    
    while (node_pos != bucketPos){
        hashTable.read(node, node_pos);
        if (!strcmp(key.c_str(), node.m_key))  //if the node's m_key == key
            position.push_back(node_pos);
        node_pos = node.m_next;
    }
    Iterator temp2(this, position);
    return temp2;
}

int DiskMultiMap::erase(const string& key, const string& value, const string& context)
{
    if (strlen(key.c_str()) > 120 || strlen(value.c_str()) > 120 || strlen(context.c_str()) > 120 || !openExisting(fn))
        return -1;
    
    int numErase(0);
    
    Head head;
    hashTable.read(head, 0);
    BinaryFile::Offset bucket, bucketPos;
    findBucket(key, head, bucket, bucketPos);
    hashTable.read(bucket, bucketPos);
    
    //cout << "first node pos: " << bucket << endl;
    
    if (bucket == bucketPos){
        close();
        return numErase;
    }//get the position of first node
    
    DiskNode node;
    BinaryFile::Offset prev_pos = bucketPos; //head ptr in bucket
    BinaryFile::Offset current_pos = bucket; // first node
    
    while (current_pos != bucketPos){
        hashTable.read(node, current_pos);
        BinaryFile::Offset next = node.m_next;
        
        if (!strcmp(key.c_str(), node.m_key) && !strcmp(value.c_str(), node.m_value) && !strcmp(context.c_str(), node.m_context)){
            numErase++;
            
            //cout << "next: " << next << "|| prev_delete: " << head.prev_delete << endl;
            
            //update delete position list
            node.m_next = head.prev_delete;
            head.prev_delete = current_pos;
            
            hashTable.write(head, 0);
            hashTable.write(node, current_pos);
            
            if (prev_pos == bucketPos)
                hashTable.write(next, bucketPos);
            else{
                hashTable.read(node, prev_pos);
                node.m_next = next;
                hashTable.write(node, prev_pos);
            }
        }
        else
            prev_pos = current_pos;
        current_pos = next;
    }
    //cout << "prev_delete: " << head.prev_delete << endl;
    //cout << numErase << endl << endl;
    return numErase;
}

DiskMultiMap::Iterator::Iterator():m_map(nullptr)
{}

DiskMultiMap::Iterator::Iterator(DiskMultiMap* map, list<BinaryFile::Offset> position)
:m_map(map), pos(position)
{}

bool DiskMultiMap::Iterator::isValid() const
{
    if (m_map == nullptr || pos.empty())
        return false;
    return true;
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
    if (!isValid())
        return *this;
    
    list<BinaryFile::Offset>::iterator it = pos.begin();
    it = pos.erase(it);
    return *this;
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{
    MultiMapTuple temp;
    temp.key = temp.value = temp.context = "";
    
    if (!isValid())
        return temp;
    
    DiskNode it;
    m_map->openExisting(m_map->fn);
    
    list<BinaryFile::Offset>::iterator iter = pos.begin();
    
    m_map->hashTable.read(it, *(iter));
    
    for (int i = 0; it.m_key[i] != '\0'; i++)
        temp.key += it.m_key[i];
    
    for (int i = 0; it.m_value[i] != '\0'; i++)
        temp.value += it.m_value[i];
    
    for (int i = 0; it.m_context[i] != '\0'; i++)
        temp.context += it.m_context[i];
    
    return temp;
}

void DiskMultiMap::findBucket(const string& key, const Head head, BinaryFile::Offset& bucket, BinaryFile::Offset& bucketPos)
{
    hash<string> str_hash;
    bucket = str_hash(key) % head.numBucket;
    bucketPos = sizeof(head) + bucket*sizeof(BinaryFile::Offset);
}
