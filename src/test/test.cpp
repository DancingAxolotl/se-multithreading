#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SomeContainer.h>

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

TEST(SomeContainer, QueryObjects) {
    MockKeyValueStoreFactory mockStorageProvider;
    MockKeyValueStore* mockStore = new MockKeyValueStore();
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(mockStore));
    CSomeContainer container(&mockStorageProvider);

    int someIndex = 0;
    int expectedValue = 5;
    IObject* storedObject = new int(expectedValue);
    std::auto_ptr<IObject> someObject(storedObject);
    EXPECT_CALL(*mockStore, Insert(someIndex, someObject.get()));
    container.Register(someIndex, someObject);

    EXPECT_CALL(*mockStore, Query(someIndex)).WillOnce(::testing::Return(storedObject));

    EXPECT_EQ(expectedValue, *container.Query(someIndex));
}
