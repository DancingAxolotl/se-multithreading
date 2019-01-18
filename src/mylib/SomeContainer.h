#pragma once
#include <memory>
#include <map>
#include <cassert>

template<typename IObject>
class IKeyValueStoreIterator {
public:
    virtual ~IKeyValueStoreIterator() {}
    virtual bool Next() = 0;
    virtual IObject* Value() = 0;
};

template<typename IObject>
class IKeyValueStore {
public:
    virtual ~IKeyValueStore() {}
    virtual void Insert(int key, IObject* value) = 0;
    virtual IObject* Query(int key) = 0;
    virtual void Remove(int key) = 0;
    virtual IKeyValueStoreIterator<IObject>& Begin() = 0;
};

template<typename IObject>
class IKeyValueStoreFactory {
public:
    virtual ~IKeyValueStoreFactory() {}
    virtual IKeyValueStore<IObject>* CreateStore() = 0;
};

template<typename IObject>
class CSomeContainer {
public:
    CSomeContainer(IKeyValueStoreFactory<IObject>* storageProvider);
    ~CSomeContainer();
    void Register(int objectId, std::auto_ptr<IObject> object);
    IObject* Query(int objectId);
    void Release(int objectId);
private:
    std::unique_ptr<IKeyValueStore<IObject>> m_storage;
};

template<typename IObject>
CSomeContainer<IObject>::CSomeContainer(IKeyValueStoreFactory<IObject>* storageProvider)
{
    assert(storageProvider != nullptr && "storage provider must be non null");
    if (storageProvider != nullptr) {
        m_storage.reset(storageProvider->CreateStore());
    }
}

template<typename IObject>
CSomeContainer<IObject>::~CSomeContainer()
{
    try {
        IKeyValueStoreIterator<IObject>& iterator = m_storage->Begin();
        do {
            IObject* val = iterator.Value();
            if (val != nullptr) {
                delete val;
            }
        } while(iterator.Next());
    } catch (const std::exception &) {
        //
    }
}

template<typename IObject>
void CSomeContainer<IObject>::Register(int objectId, std::auto_ptr<IObject> object)
{
    m_storage->Insert(objectId, object.release());
}

template<typename IObject>
IObject* CSomeContainer<IObject>::Query(int objectId)
{
    return m_storage->Query(objectId);
}

template<typename IObject>
void CSomeContainer<IObject>::Release(int objectId)
{
    IObject* objPtr = m_storage->Query(objectId);
    m_storage->Remove(objectId);
    delete objPtr;
}
