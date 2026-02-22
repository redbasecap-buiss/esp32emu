// esp32emu — test_usb_hid_keyboard: USB HID Keyboard mock test
#include "USB.h"
#include "USBHIDKeyboard.h"
#include <cassert>
#include <cstdio>

int main() {
    // USB stack
    assert(!USB.isBegun());
    USB.manufacturerName("esp32emu");
    USB.productName("TestKeyboard");
    USB.serialNumber("001");
    assert(USB.begin());
    assert(USB.isBegun());
    assert(USB.getManufacturer() == "esp32emu");
    assert(USB.getProduct() == "TestKeyboard");
    assert(USB.getSerial() == "001");

    // Keyboard
    USBHIDKeyboard kb;
    assert(!kb.test_isBegun());
    kb.begin();
    assert(kb.test_isBegun());

    // Press/release
    kb.press('a');
    assert(kb.test_isPressed('a'));
    kb.release('a');
    assert(!kb.test_isPressed('a'));

    // Modifiers
    kb.press(KEY_LEFT_CTRL);
    assert(kb.test_getModifiers() & 0x01);
    kb.press(KEY_LEFT_SHIFT);
    assert(kb.test_getModifiers() & 0x02);
    kb.releaseAll();
    assert(kb.test_getModifiers() == 0);
    assert(!kb.test_isPressed('a'));

    // Write (press + release)
    kb.test_clearLog();
    kb.write('x');
    auto& log = kb.test_getPressLog();
    assert(log.size() == 1);
    assert(log[0] == 'x');

    // Special keys
    kb.test_clearLog();
    kb.press(KEY_RETURN);
    assert(kb.test_isPressed(KEY_RETURN));
    kb.release(KEY_RETURN);
    assert(!kb.test_isPressed(KEY_RETURN));

    // End
    kb.end();
    assert(!kb.test_isBegun());
    USB.end();
    assert(!USB.isBegun());

    printf("test_usb_hid_keyboard: all assertions passed\n");
    return 0;
}
