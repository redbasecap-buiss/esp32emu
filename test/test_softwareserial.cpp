// esp32emu — SoftwareSerial mock test
#include "SoftwareSerial.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Basic construction and begin
    SoftwareSerial ss(10, 11);
    ss.begin(9600);

    // Initially empty
    assert(ss.available() == 0);
    assert(ss.read() == -1);
    assert(ss.peek() == -1);

    // Inject data and read back
    ss.injectByte('H');
    ss.injectByte('i');
    assert(ss.available() == 2);
    assert(ss.peek() == 'H');
    assert(ss.read() == 'H');
    assert(ss.available() == 1);
    assert(ss.read() == 'i');
    assert(ss.available() == 0);

    // Write produces output (goes to stdout, we just check return values)
    assert(ss.write('A') == 1);
    uint8_t buf[] = {0x41, 0x42, 0x43};
    assert(ss.write(buf, 3) == 3);

    // Print/println
    assert(ss.print("hello") == 5);
    assert(ss.println("world") == 6);
    assert(ss.print(42) > 0);
    assert(ss.println(99) > 0);

    // Boolean operator
    assert((bool)ss == true);

    // Listen
    assert(ss.listen() == true);
    assert(ss.isListening() == true);

    // Inverse logic parameter accepted
    SoftwareSerial ss2(4, 5, true);
    ss2.begin(115200);
    assert(ss2.available() == 0);

    printf("test_softwareserial: all assertions passed\n");
    return 0;
}
