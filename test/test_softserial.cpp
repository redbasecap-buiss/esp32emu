#include "SoftwareSerial.h"
#include <cassert>
#include <cstdio>

int main() {
    SoftwareSerial ss(10, 11);
    ss.begin(9600);

    assert((bool)ss);
    assert(ss.isListening());
    assert(ss.available() == 0);
    assert(ss.read() == -1);

    // Inject data and read back
    ss.injectByte('H');
    ss.injectByte('i');
    assert(ss.available() == 2);
    assert(ss.peek() == 'H');
    assert(ss.read() == 'H');
    assert(ss.read() == 'i');
    assert(ss.available() == 0);

    // Write
    assert(ss.write('A') == 1);
    assert(ss.print("OK") == 2);
    assert(ss.println("!") == 2);

    ss.end();
    printf("test_softserial: all assertions passed\n");
    return 0;
}
