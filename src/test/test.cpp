#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

/*
 * IKeyValueStore
 * insert(key, value)
 * at(key)
 * erase(key)
 *
 * SomeContainer:
 * - registers objects
 * - queries objects
 *  1 registred object
 *  2 unregistred object
 * - unregisters objects
 *  1 registred object
 *  2 unregistred object
 *
*/
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

class MockKeyValueStoreFactory : public IKeyValueStoreFactory {
public:
    MOCK_METHOD0(CreateStore, IKeyValueStore*());
};

class MockKeyValueStore : public IKeyValueStore {
public:
    MOCK_METHOD2(Insert, void(int, IObject*));
    MOCK_METHOD1(Query, IObject*(int));
    MOCK_METHOD1(Remove, void(int));
};

class CSomeContainer {
public:
    CSomeContainer(IKeyValueStoreFactory* storageProvider)
    {
        assert(storageProvider != nullptr && "storage provider must be non null");
        if (storageProvider != nullptr) {
            m_storage.reset(storageProvider->CreateStore());
        }
    }

    void Register(int objectId, std::auto_ptr<IObject> object)
    {
        m_storage->Insert(objectId, object.release());
    }
private:
    std::unique_ptr<IKeyValueStore> m_storage;
};

TEST(SomeContainer, UsesStorageFactory) {
    MockKeyValueStoreFactory mockStorageProvider;
    MockKeyValueStore* mockStore = new MockKeyValueStore();
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(mockStore));
    CSomeContainer container(&mockStorageProvider);
}

TEST(SomeContainer, RegistersObjects) {
    MockKeyValueStoreFactory mockStorageProvider;
    MockKeyValueStore* mockStore = new MockKeyValueStore();
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(mockStore));
    CSomeContainer container(&mockStorageProvider);

    int someIndex = 0;
    std::auto_ptr<IObject> someObject(new int(5));
    EXPECT_CALL(*mockStore, Insert(someIndex, someObject.get()));
    container.Register(someIndex, someObject);
}
