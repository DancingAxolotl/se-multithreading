#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "SomeContainer.h"

/*
 * SomeContainer:
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

IObjectDestructable* RegisterDestructableObject(CSomeContainer<IObjectDestructable>& someContainer, int index) {
    MockIObjectDestructable* storedObject = new MockIObjectDestructable;
    EXPECT_CALL(*storedObject, Die());
    std::auto_ptr<IObjectDestructable> storedObjectPtr(storedObject);
    someContainer.Register(index, storedObjectPtr);
    return storedObject;
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
    CSomeContainer<IObjectDestructable> container;
    int someIndex = 0;
    IObjectDestructable* storedObject = RegisterDestructableObject(container, someIndex);
    EXPECT_EQ(storedObject, container.Query(someIndex));
    container.Unregister(someIndex);
    EXPECT_THROW(container.Query(someIndex), std::out_of_range);
}

TEST(SomeContainer, DestroysObjectsInStore) {
    CSomeContainer<IObjectDestructable> container;
    RegisterDestructableObject(container, 0);
}

TEST(SomeContainer, ReplaceRegistredObject) {
    CSomeContainer<IObjectDestructable> container;
    int someIndex = 0;
    IObjectDestructable* storedObject = RegisterDestructableObject(container, someIndex);
    EXPECT_EQ(storedObject, container.Query(someIndex));

    IObjectDestructable* secondObject = RegisterDestructableObject(container, someIndex);
    EXPECT_EQ(secondObject, container.Query(someIndex));
}
