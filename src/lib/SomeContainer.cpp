#include "SomeContainer.h"
#include <cassert>

CSomeContainer::CSomeContainer(IKeyValueStoreFactory* storageProvider)
{
    assert(storageProvider != nullptr && "storage provider must be non null");
    if (storageProvider != nullptr) {
        m_storage.reset(storageProvider->CreateStore());
    }
}

void CSomeContainer::Register(int objectId, std::auto_ptr<IObject> object)
{
    m_storage->Insert(objectId, object.release());
}

IObject* CSomeContainer::Query(int objectId)
{
    return m_storage->Query(objectId);
}

void CSomeContainer::Release(int objectId)
{
    IObject* objPtr = m_storage->Query(objectId);
    m_storage->Remove(objectId);
    delete objPtr;
}
