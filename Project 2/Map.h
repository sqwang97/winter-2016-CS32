//
//  Map.hpp
//  Project 2
//
//  Created by Cecilia.Wang on 1/21/16.
//  Copyright © 2016 Cecilia.Wang. All rights reserved.
//

#ifndef Map_hpp
#define Map_hpp

#include <string>

typedef std::string KeyType;
typedef double ValueType;
 
class Map
{
public:
    Map();
    ~Map();
    Map(const Map& other);
    Map& operator=(const Map& other);
    
    inline bool empty() const;
    inline int size() const;
    bool insert(const KeyType& key, const ValueType& value);
    bool update(const KeyType& key, const ValueType& value);
    inline bool insertOrUpdate(const KeyType& key, const ValueType& value);
    bool erase(const KeyType& key);
    bool contains(const KeyType& key) const;
    bool get(const KeyType& key, ValueType& value) const;
    bool get(int i, KeyType& key, ValueType& value) const;
    void swap(Map& other);
    
private:
    struct node
    {
        KeyType m_key;
        ValueType m_value;
        node* prev;
        node* next;
    };
    node* head;
    int m_size;
};

bool combine(const Map& m1, const Map& m2, Map& result);
void subtract(const Map& m1, const Map& m2, Map& result);

inline bool Map::empty() const
{
    if (m_size == 0)
        return true;
    else
        return false;
}

inline int Map::size() const
{
    return m_size;
}

inline bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
{
    return (update(key, value) || insert(key, value));
}

#endif /* Map_hpp */