#pragma once
#include <memory>
#include <map>
#include <cassert>
#include <mutex>

template<typename KeyType, typename ValueType>
using  KeyValueStore = std::map<KeyType, ValueType>;

template<typename IObject>
class CSomeContainer;

template<typename IObject>
class CSomeContainerIterator {
public:
    CSomeContainerIterator(CSomeContainer<IObject>* baseContainer, const typename KeyValueStore<int, IObject*>::iterator baseIterator)
        : m_baseContainer(baseContainer)
        , m_iterator(baseIterator) {}

    bool operator==(const CSomeContainerIterator<IObject>& right) const {
        return m_iterator == right.m_iterator;
    }

    IObject* operator*() const {
        return m_baseContainer->Query(m_iterator->first);
    }

    CSomeContainerIterator<IObject>& operator++() {
        ++m_iterator;
        return *this;
    }

private:
    typename KeyValueStore<int, IObject*>::iterator m_iterator;
    CSomeContainer<IObject>* m_baseContainer;
};
