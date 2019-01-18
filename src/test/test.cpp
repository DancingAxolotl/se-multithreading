#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
*/
using IObject = void;

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

class MockKeyValueStore {
public:
    MOCK_METHOD2(Insert, void(int, IObject*));
    MOCK_METHOD1(Query, IObject*(int));
    MOCK_METHOD1(Remove, void(int));
};

TEST(SomeContainer, RegistersObjects) {
    MockKeyValueStoreFactory mockStorageProvider;
    MockKeyValueStore mockStore;
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(&mockStore));
    CSomeContainer container(&mockStorageProvider);
    int someIndex = 0;
    std::auto_ptr<IObject> someObject(new int(5));
    EXPECT_CALL(mockStore, Insert(someIndex, someObject.get()));
    container.Register(someIndex, someObject);
}
