#pragma once
#include "std.h"
#include "boost/any.hpp"

// The Factory ----------------------------------------------------------------------------------------
template <class Key, class Object>
class Factory
{
    std::map<Key, boost::any> selector;
    
public:
    Factory() : selector() {}
    Factory(std::initializer_list<std::pair<const Key, boost::any>> il) : selector(il) {}

    template <typename Function>
    void add(Key key, Function &&someFunction) { selector[key] = boost::any(someFunction); };

    template <typename... Args>
    Object create(Key key, Args... args)
    {
        if (selector.find(key) != selector.end())
        {
            return boost::any_cast<boost::add_pointer_t<Object(Args...)>>(selector[key])(args...);
        }
        else
            return nullptr;
    }
};