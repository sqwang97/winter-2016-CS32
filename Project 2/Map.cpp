//
//  Map.cpp
//  Project 2
//
//  Created by Cecilia.Wang on 1/21/16.
//  Copyright © 2016 Cecilia.Wang. All rights reserved.
//

#include "Map.h"
#include <iostream>
using namespace std;

Map::Map()
{
    //create a dummy node
    head = new node;
    head->prev = head;
    head->next = head;
    m_size = 0;
}

Map::~Map()
{
    //delete every node excpet dummy node
    node* p = head->next;
    while (p != head){
        node* n = p->next;
        delete p;
        p = n;
    }
    //delete dummy node
    delete p;
}

Map::Map(const Map& other)
{
    head = new node;
    head->prev = head;
    head->next = head;
    m_size = 0;
    
    node* p = other.head->prev;
    while(p != other.head){
        insert(p->m_key, p->m_value);
        p = p->prev;
    }
}

Map& Map::operator= (const Map& other)
{
    if (this != &other){
        Map temp(other);
        swap(temp);
    }
    return *this;
}

bool Map::insert(const KeyType& key, const ValueType& value)
{
    node* temp = head->next;
    while(temp != head){
        if(temp->m_key == key)
            return false;
        temp = temp->next;
    }
    node* p = new node;
    p->m_key = key;
    p->m_value = value;
    p->prev = head;
    p->next = head->next;
    head->next = p;
    p->next->prev = p;
    m_size++;
    return true;
}

bool Map::update(const KeyType& key, const ValueType& value)
{
    node* p = head->next;
    while (p != head){
        if (p->m_key == key){
            p->m_value = value;
            return true;
        }
        p = p->next;
    }
    return false;
}

bool Map::erase(const KeyType& key)
{
    node* p = head->next;
    while (p != head){
        if (p->m_key == key){
            node* t = p->prev;
            t->next = p->next;
            p->next->prev = t;
            delete p;
            m_size--;
            return true;
        }
        p = p->next;
    }
    return false;
}

bool Map::contains(const KeyType& key) const
{
    node* p = head->next;
    while (p != head){
        if (p->m_key == key)
            return true;
        p = p->next;
    }
    return false;
}

bool Map::get(const KeyType& key, ValueType& value) const
{
    node* p = head->next;
    while (p != head){
        if (p->m_key == key){
            value = p->m_value;
            return true;
        }
        p = p->next;
    }
    return false;
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
    if (i < 0 || i >= size())
        return false;
    
    node* p = head->next;
    for (int t = 0; t < i; t++){
        p = p->next;
    }
    key = p->m_key;
    value = p->m_value;
    return true;
}

void Map::swap(Map& other)
{
    int t = m_size;
    m_size = other.m_size;
    other.m_size = t;
    
    node* temp = head;
    head = other.head;
    other.head = temp;
}

bool combine(const Map& m1, const Map& m2, Map& result)
{
    bool t = true;
    KeyType key;
    ValueType val1, val2;
    Map temp(m1);
    
    for (int i = 0; i < m2.size(); i++){
        m2.get(i, key, val1);
        if (!temp.contains(key))
            temp.insert(key, val1);
        else if (temp.get(key, val2) && val1 != val2 ){
            temp.erase(key);
            t = false;
        }
    }
    result = temp;
    return t;
}

void subtract(const Map& m1, const Map& m2, Map& result)
{
    KeyType key;
    ValueType val;
    Map temp;
    
    for (int i = 0; i < m1.size(); i++){
        m1.get(i, key, val);
        if (!m2.contains(key))
            temp.insert(key, val);
    }
    result = temp;
}

