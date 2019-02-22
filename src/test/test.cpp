#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include "SomeContainer.h"

/*
 * SomeContainer:
 * - queries objects
 *  1 registred object
 *  2 unregistred object
 * - unregisters objects
 *  1 registred object
 *  2 unregistred object
 *
 * returns iterator
 * iterator next returns current value
 * iterator next returns next item
 * iterator end returns end when reached end
 * read/writes from other threads don't block iterator
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


class MockIObjectDestructableWithSleep : public IObjectDestructable {
public:
    MOCK_METHOD0(Foo, void());
    MOCK_METHOD0(Die, void());
    virtual ~MockIObjectDestructableWithSleep() {
        try {
            Die();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } catch (const std::exception &) {

        }
    }
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

void ModifyContainer(CSomeContainer<IObjectDestructable>& container, int index) {
    MockIObjectDestructableWithSleep* storedObject = new MockIObjectDestructableWithSleep;
    // if destructor is called multiple times, this will fail;
    // if destructor is not called gmock performs clean-up, but will display a warning about leaked mocks
    EXPECT_CALL(*storedObject, Die()).Times(1);
    EXPECT_NO_THROW(container.Register(index, std::auto_ptr<IObjectDestructable>(storedObject)));
}

TEST(SomeContainer, SynchronizesRegisterAccess) {
    CSomeContainer<IObjectDestructable> container;
    const int threadCount = 5;
    std::thread t[threadCount];
    const int someIndex = 0;
    for (int i = 0; i < threadCount; ++i) {
        t[i] = std::thread(ModifyContainer, std::ref(container), someIndex);
    }

    for (int i = 0; i < threadCount; ++i) {
        t[i].join();
    }
    EXPECT_NO_THROW(container.Query(someIndex)); // some value should be left at index
}

void ModifyContainerUnregister(CSomeContainer<IObjectDestructable>& container, int index, bool expectThrow) {
    if (expectThrow) {
        EXPECT_THROW(container.Unregister(index), std::out_of_range);
    } else {
        EXPECT_NO_THROW(container.Unregister(index));
    }
}

TEST(SomeContainer, SynchronizesUnregisterAccess) {
    CSomeContainer<IObjectDestructable> container;
    const int someIndex = 0;
    MockIObjectDestructableWithSleep* storedObject = new MockIObjectDestructableWithSleep;
    EXPECT_CALL(*storedObject, Die()).Times(1);
    container.Register(someIndex, std::auto_ptr<IObjectDestructable>(storedObject));

    std::thread t1(ModifyContainerUnregister, std::ref(container), someIndex, false);
    std::thread t2(ModifyContainerUnregister, std::ref(container), someIndex, true);
    t1.join();
    t2.join();
}

void QueryContainer(CSomeContainer<IObjectDestructable>& container, int index) {
    IObjectDestructable* item = nullptr;
    std::this_thread::sleep_for(std::chrono::milliseconds(500)); //wait for other thread to enter the destructor
    EXPECT_NO_THROW(item = container.Query(index));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    if (item != nullptr)
    {
        MockIObjectDestructableWithSleep* test = reinterpret_cast<MockIObjectDestructableWithSleep*>(item);
        EXPECT_NO_THROW(test->Foo()); // will crash if the object was deleted
    }
}

TEST(SomeContainer, SynchronizesQueryAccess) {
    CSomeContainer<IObjectDestructable> container;
    const int someIndex = 0;
    MockIObjectDestructableWithSleep* storedObject = new MockIObjectDestructableWithSleep;
    EXPECT_CALL(*storedObject, Die()).Times(1);
    container.Register(someIndex, std::auto_ptr<IObjectDestructable>(storedObject));

    std::thread t1(ModifyContainer, std::ref(container), someIndex);
    std::thread t2(QueryContainer, std::ref(container), someIndex);

    t1.join();
    t2.join();
}

TEST(SomeContainer, EmptyContainerReturnsIterator) {
    CSomeContainer<int> container;
    CSomeContainerIterator<int> start = container.Start();
    CSomeContainerIterator<int> end = container.End();
    EXPECT_EQ(start, end);
}

TEST(SomeContainerIterator, IteratorReturnsCurrentValue) {
    CSomeContainer<int> container;
    int value = 1;
    container.Register(0, std::auto_ptr<int>(new int(value)));

    CSomeContainerIterator<int> start = container.Start();
    EXPECT_EQ(**start, value);
}

TEST(SomeContainerIterator, IteratorReturnsNextValue) {
    CSomeContainer<int> container;
    int first = 1;
    int second = 2;
    container.Register(0, std::auto_ptr<int>(new int(first)));
    container.Register(1, std::auto_ptr<int>(new int(second)));

    CSomeContainerIterator<int> start = container.Start();
    EXPECT_EQ(**start, first);
    ++start;
    EXPECT_EQ(**start, second);
}

TEST(SomeContainerIterator, IteratorReturnsEndValue) {
    CSomeContainer<int> container;
    int first = 1;
    container.Register(0, std::auto_ptr<int>(new int(first)));

    CSomeContainerIterator<int> start = container.Start();
    EXPECT_EQ(**start, first);
    ++start;
    EXPECT_EQ(start, container.End());
}

TEST(SomeContainerIterator, ShouldNotBlockAccessToContainer) {
    
}
