#pragma once
#include <memory>

using IObject = int;

class IKeyValueStore {
public:
    virtual ~IKeyValueStore() {}
    virtual void Insert(int key, IObject* value) = 0;
    virtual IObject* Query(int key) = 0;
    virtual void Remove(int key) = 0;
};

class IKeyValueStoreFactory {
public:
    virtual ~IKeyValueStoreFactory() {}
    virtual IKeyValueStore* CreateStore() = 0;
};

class CSomeContainer {
public:
    CSomeContainer(IKeyValueStoreFactory* storageProvider);
    void Register(int objectId, std::auto_ptr<IObject> object);
    IObject* Query(int objectId);
    void Release(int objectId);
private:
    std::unique_ptr<IKeyValueStore> m_storage;
};
