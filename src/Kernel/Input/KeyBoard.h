#pragma once

#include <stdint.h>

#define ENTER 0x1C
#define BACKSPACE 0x0E
#define CONTROL 0x1D
#define LEFT_SHIFT 0x2A
#define ARROW_UP 0x48
#define ARROW_DOWN 0x50
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D
#define PAGE_UP 0x49
#define PAGE_DOWN 0x51
#define CAPS_LOCK 0x3A

namespace KeyBoard
{    
    void HandleScanCode(uint8_t ScanCode);

    /// <summary>
    /// Returns the key that was last pressed down and resets that key. 
    /// </summary>
    char GetKeyPress();

    /// <summary>
    /// Returns the key that was last pressed down.
    /// </summary>
    char GetKeyHeld();

    /// <summary>
    /// Returns the value for Key in the KeyCache and sets it to 0.
    /// </summary>
    bool IsPressed(uint8_t Key);

    /// <summary>
    /// Returns the value for Key in the KeyCache.
    /// </summary>
    bool IsHeld(uint8_t Key);
}
