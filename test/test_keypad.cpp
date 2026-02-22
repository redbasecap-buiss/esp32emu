// esp32emu — test_keypad: Matrix keypad mock test
#include "Keypad.h"
#include <cassert>
#include <cstdio>

static char lastEventKey = 0;
static void keyListener(char k) { lastEventKey = k; }

int main() {
    char keys[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };
    uint8_t rowPins[4] = {1, 2, 3, 4};
    uint8_t colPins[4] = {5, 6, 7, 8};

    Keypad kp((char*)keys, rowPins, colPins, 4, 4);

    // Verify config
    assert(kp.test_getRows() == 4);
    assert(kp.test_getCols() == 4);
    assert(kp.test_getKeymap(0, 0) == '1');
    assert(kp.test_getKeymap(3, 3) == 'D');
    assert(kp.test_getKeymap(2, 1) == '8');

    // No key by default
    assert(kp.getKey() == NO_KEY);
    assert(kp.getState() == IDLE);

    // Inject key press
    kp.test_injectKey('5');
    char k = kp.getKey();
    assert(k == '5');
    assert(kp.getState() == PRESSED);
    assert(kp.isPressed('5'));
    assert(!kp.isPressed('3'));

    // After read, next getKey returns NO_KEY
    assert(kp.getKey() == NO_KEY);

    // Event listener
    kp.addEventListener(keyListener);
    lastEventKey = 0;
    kp.test_injectKey('#');
    kp.getKey();
    assert(lastEventKey == '#');

    // Hold time / debounce
    kp.setHoldTime(1000);
    assert(kp.getHoldTime() == 1000);
    kp.setDebounceTime(100);
    assert(kp.getDebounceTime() == 100);

    // State change
    kp.test_setState(HOLD);
    assert(kp.getState() == HOLD);
    kp.test_setState(RELEASED);
    assert(kp.getState() == RELEASED);

    printf("test_keypad: all assertions passed\n");
    return 0;
}
