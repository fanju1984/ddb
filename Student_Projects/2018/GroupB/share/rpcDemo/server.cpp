#include <iostream>
#include "rpc/server.h"

void foo() {
    std::cout << "foo was called!" << std::endl;
}

int main(int argc, char *argv[]) {
   // Creating a server that listens on port 8080
    rpc::server srv(8080);

    // Binding the name "foo" to free function foo.
    // note: the signature is automatically captured
    srv.bind("foo", &foo);

    // Binding a lambda function to the name "add".
    srv.bind("add", [](int a, int b) {
        return a + b;
    });

    // Run the server loop.
    srv.run();

    return 0;
}
