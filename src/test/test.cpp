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

TEST(SomeContainer, RegistersObjects) {
    IKeyValueStoreFactory<MockKeyValueStore> mockStorageProvider;
    MockKeyValueStore mockStore;
    EXPECT_CALL(mockStorageProvider, CreateStore()).WillOnce(::testing::Return(mockStore));
    CSomeContainer container(mockStorageProvider);
    int someIndex = 0;
    int* someObject = new int(5);
    EXPECT_CALL(mockStore, insert(someIndex, someObject));
    container.Register(someIndex, someObject);

}
