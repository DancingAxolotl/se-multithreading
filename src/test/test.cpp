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

class IObjectDestructable {
public:
    virtual ~IObjectDestructable() {}
};

class MockIObjectDestructable : public IObjectDestructable {
public:
    MOCK_METHOD0(Die, void());
    virtual ~MockIObjectDestructable() { Die(); }
};

TEST(SomeContainer, RegistersObjects) {
    CSomeContainer<int> container;
    int someIndex = 0;
    std::auto_ptr<int> someObject(new int(5));
    container.Register(someIndex, someObject);
}

TEST(SomeContainer, QueryObjects) {
    CSomeContainer<int> container;
    int someIndex = 0;
    int expectedValue = 5;
    int* storedObject = new int(expectedValue);
    std::auto_ptr<int> someObject(storedObject);
    container.Register(someIndex, someObject);
    EXPECT_EQ(expectedValue, *container.Query(someIndex));
}

TEST(SomeContainer, ReleaseObjects) {
    CSomeContainer<int> container;
    int someIndex = 0;
    int expectedValue = 5;
    int* storedObject = new int(expectedValue);
    std::auto_ptr<int> someObject(storedObject);
    container.Register(someIndex, someObject);
    EXPECT_EQ(expectedValue, *container.Query(someIndex));
    container.Unregister(someIndex);
    EXPECT_THROW(container.Query(someIndex), std::out_of_range);
}

TEST(SomeContainer, DestroysObjectsInStore) {
    CSomeContainer<IObjectDestructable> container;
    int someIndex = 0;
    MockIObjectDestructable* storedObject = new MockIObjectDestructable;
    std::auto_ptr<IObjectDestructable> someObject(storedObject);
    container.Register(someIndex, someObject);
    EXPECT_CALL(*storedObject, Die());
}

TEST(SomeContainer, ReplaceRegistredObject) {
    CSomeContainer<IObjectDestructable> container;
    int someIndex = 0;
    MockIObjectDestructable* storedObject = new MockIObjectDestructable;
    EXPECT_CALL(*storedObject, Die());
    std::auto_ptr<IObjectDestructable> storedObjectPtr(storedObject);
    container.Register(someIndex, storedObjectPtr);
    EXPECT_EQ(storedObject, container.Query(someIndex));

    MockIObjectDestructable* secondObject = new MockIObjectDestructable;
    EXPECT_CALL(*secondObject, Die());
    std::auto_ptr<IObjectDestructable> secondObjectPtr(secondObject);
    container.Register(someIndex, secondObjectPtr);
    EXPECT_EQ(secondObject, container.Query(someIndex));
}
