#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "SomeContainer.h"

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

template<typename IObject>
class MockKeyValueStoreFactory : public IKeyValueStoreFactory<IObject> {
public:
    MOCK_METHOD0_T(CreateStore, IKeyValueStore<IObject>*());
};

template<typename IObject>
class MockKeyValueStoreIterator : public IKeyValueStoreIterator<IObject> {
public:
    MOCK_METHOD0_T(Next, bool());
    MOCK_METHOD0_T(Value, IObject*());
};

template<typename IObject>
class MockKeyValueStore : public IKeyValueStore<IObject> {
public:
    MOCK_METHOD2_T(Insert, void(int, IObject*));
    MOCK_METHOD1_T(Query, IObject*(int));
    MOCK_METHOD1_T(Remove, void(int));
    MOCK_METHOD0_T(Begin, IKeyValueStoreIterator<IObject>&());
    MOCK_METHOD0(Die, void());
    virtual ~MockKeyValueStore() { Die(); }
};

class IObjectDestructable {
public:
    virtual ~IObjectDestructable() {}
};

class MockIObjectDestructable : public IObjectDestructable {
public:
    MOCK_METHOD0(Die, void());
    virtual ~MockIObjectDestructable() { Die(); }
};

TEST(SomeContainer, UsesStorageFactory) {
    MockKeyValueStoreFactory<int> mockStorageProvider;
    MockKeyValueStore<int>* mockStore = new MockKeyValueStore<int>();
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(mockStore));
    CSomeContainer<int> container(&mockStorageProvider);
}

TEST(SomeContainer, RegistersObjects) {
    MockKeyValueStoreFactory<int> mockStorageProvider;
    MockKeyValueStore<int>* mockStore = new MockKeyValueStore<int>();
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(mockStore));
    CSomeContainer<int> container(&mockStorageProvider);

    int someIndex = 0;
    std::auto_ptr<int> someObject(new int(5));
    EXPECT_CALL(*mockStore, Insert(someIndex, someObject.get()));
    container.Register(someIndex, someObject);
}

TEST(SomeContainer, QueryObjects) {
    MockKeyValueStoreFactory<int> mockStorageProvider;
    MockKeyValueStore<int>* mockStore = new MockKeyValueStore<int>();
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(mockStore));
    CSomeContainer<int> container(&mockStorageProvider);

    int someIndex = 0;
    int expectedValue = 5;
    int* storedObject = new int(expectedValue);
    std::auto_ptr<int> someObject(storedObject);
    EXPECT_CALL(*mockStore, Insert(someIndex, someObject.get()));
    container.Register(someIndex, someObject);

    EXPECT_CALL(*mockStore, Query(someIndex)).WillOnce(::testing::Return(storedObject));

    EXPECT_EQ(expectedValue, *container.Query(someIndex));
}

TEST(SomeContainer, ReleaseObjects) {
    MockKeyValueStoreFactory<int> mockStorageProvider;
    MockKeyValueStore<int>* mockStore = new MockKeyValueStore<int>();
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(mockStore));
    CSomeContainer<int> container(&mockStorageProvider);

    int someIndex = 0;
    int expectedValue = 5;
    int* storedObject = new int(expectedValue);
    std::auto_ptr<int> someObject(storedObject);
    EXPECT_CALL(*mockStore, Insert(someIndex, someObject.get()));
    container.Register(someIndex, someObject);

    EXPECT_CALL(*mockStore, Query(someIndex)).WillOnce(::testing::Return(storedObject));
    EXPECT_CALL(*mockStore, Remove(someIndex));
    container.Release(someIndex);
}

TEST(SomeContainer, DestroysObjectsInStore) {
    MockKeyValueStoreFactory<IObjectDestructable> mockStorageProvider;
    MockKeyValueStore<IObjectDestructable>* mockStore = new MockKeyValueStore<IObjectDestructable>();
    MockKeyValueStoreIterator<IObjectDestructable> mockStoreIterator;
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(mockStore));
    CSomeContainer<IObjectDestructable>* container = new CSomeContainer<IObjectDestructable>(&mockStorageProvider);

    int someIndex = 0;
    MockIObjectDestructable* storedObject = new MockIObjectDestructable;
    std::auto_ptr<IObjectDestructable> someObject(storedObject);
    EXPECT_CALL(*mockStore, Insert(someIndex, someObject.get()));
    container->Register(someIndex, someObject);
    EXPECT_CALL(*mockStore, Begin()).WillOnce(::testing::ReturnRef(mockStoreIterator));
    EXPECT_CALL(mockStoreIterator, Value()).WillOnce(::testing::Return(storedObject));
    EXPECT_CALL(mockStoreIterator, Next()).WillOnce(::testing::Return(false));
    EXPECT_CALL(*storedObject, Die());
    EXPECT_CALL(*mockStore, Die());
    delete container;
}
