#include <iostream>

class TestClass {
public:
    TestClass() {
        // Constructeur
    }

    void sayHello() {
        std::cout << "Hello from TestClass!" << std::endl;
    }
};

extern "C" void test_cpp_class() {
  TestClass obj;
  obj.sayHello();
}
