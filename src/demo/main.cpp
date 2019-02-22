#include <thread>
#include <iostream>
#include "SomeContainer.h"

class DummyObject {
public:
    DummyObject(int i) : i(i) {
        std::cout << "Start to insert\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "insert object " << i << "\n";
    }

    ~DummyObject() {
        std::cout << "Start to delete\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "delete object " << i << "\n";
    }

    void doSomething() {
        std::cout << "Start to do something\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        std::cout << "I am object " << i << "\n";
    }

private:
    int i;
};

void insertItems(CSomeContainer<DummyObject>& container, int start, int count) {
    for (int i = start; i < start+count; ++i) {
        container.Register(i, std::auto_ptr<DummyObject>(new DummyObject(i)));
    }
}

void removeItems(CSomeContainer<DummyObject>& container, int start, int count) {
    for (int i = start; i < start+count; ++i) {
        container.Unregister(i);
    }
}

void iterate(CSomeContainer<DummyObject>& container) {
    int i = 0;
    for (auto iter = container.Start(); !(iter == container.End()); ++iter) {
        if (*iter != nullptr) {
            (*iter)->doSomething();
        }
        ++i;
    }
}

int main() {
    CSomeContainer<DummyObject> container;
    std::cout << "populating container...\n";
    insertItems(container, 0, 10);
    std::cout << "Done.\nStarting threads...\n";
    std::thread t1(insertItems, std::ref(container), 10, 100);
    std::thread t2(iterate, std::ref(container));
    std::thread t3(removeItems, std::ref(container), 5, 20);
    t1.join();
    t2.join();
    t3.join();
    std::cout << "Done.\n";
    return 0;
}
