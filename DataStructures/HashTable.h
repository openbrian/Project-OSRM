/*

Copyright (c) 2013, Project OSRM, Dennis Luxen, others
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <boost/functional/hash.hpp>
#include <boost/ref.hpp>
#include <boost/unordered_map.hpp>

template<typename Key, typename Value, typename Hash = boost::hash<Key> >
class HashTable : public boost::unordered_map<Key, Value> {
private:
    typedef boost::unordered_map<Key, Value, Hash> super;
public:
    static Value default_value;

    HashTable() : super() { }

    explicit HashTable(const unsigned size) : super(size) { }

    inline void Add( Key const & key, Value const & value) {
        super::emplace(std::make_pair(key, value));
    }

    inline const Value Find(Key const & key) const
    {
        typename super::const_iterator iter = super::find(key);
        if (iter == super::end())
        {
            return boost::cref(default_value);
        }
        return boost::cref(iter->second);
    }

    inline const bool Holds( Key const & key) const
    {
        if(super::find(key) == super::end())
        {
            return false;
        }
        return true;
    }
};

template<typename Key, typename Value, typename Hash>
Value HashTable<Key, Value, Hash>::default_value;

#endif /* HASH_TABLE_H */
