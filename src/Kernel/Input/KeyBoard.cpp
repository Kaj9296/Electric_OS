#include "KeyBoard.h"
#include "Mouse.h"

namespace KeyBoard
{
    /// <summary>
    /// A table to convert from keyboard scancodes to ascii.
    /// The index in the table is the scancode value and the given element the ascii value or if an ascii.
    /// </summary>
    const char ScanCodeTable[] =
    {
         0,  0, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
        '9', '0', '-', '=', BACKSPACE,	/* Backspace */
        '\t',			/* Tab */
        'q', 'w', 'e', 'r',	/* 19 */
        't', 'y', 'u', 'i', 'o', 'p', '[', ']', ENTER,	/* Enter key */
        CONTROL,			/* 29   - Control */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
        '\'', '`', LEFT_SHIFT,		/* Left shift */
        '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
        'm', ',', '.', '/',   0,				/* Right shift */
        '*',
        0,	/* Alt */
        ' ',	/* Space bar */
        CAPS_LOCK,	/* Caps lock */
        0,	/* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,	/* < ... F10 */
        0,	/* 69 - Num lock*/
        0,	/* Scroll Lock */
        0,	/* Home key */
        ARROW_UP,	/* Up Arrow */
        PAGE_UP,	/* Page Up */
        '-',
        ARROW_LEFT,	/* Left Arrow */
        0,
        ARROW_RIGHT,	/* Right Arrow */
        '+',
        0,	/* 79 - End key*/
        ARROW_DOWN,	/* Down Arrow */
        PAGE_DOWN,	/* Page Down */
        0,	/* Insert Key */
        0,	/* Delete Key */
        0,   0,   0,
        0,	/* F11 Key */
        0,	/* F12 Key */
        0,	/* All other keys are undefined */
    };

    /// <summary>
    /// A table to convert from keyboard scancodes to ascii, while the shift key is down or while caps has been pressed.
    /// The index in the table is the scancode value and the given element the ascii value or if an ascii.
    /// </summary>
    const char ShiftedScanCodeTable[] =
    {
        0,  0, '!', '"', '#', '$', '%', '&', '/', '(',	/* 9 */
        ')', '=', '-', '=', BACKSPACE,	/* Backspace */
        '\t',			/* Tab */
        'Q', 'W', 'E', 'R',	/* 19 */
        'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', ENTER,	/* Enter key */
        CONTROL,			/* 29   - Control */
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',	/* 39 */
        '\'', '`', LEFT_SHIFT,		/* Left shift */
        '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
        'M', ',', '.', '/',   0,				/* Right shift */
        '*',
        0,	/* Alt */
        ' ',	/* Space bar */
        CAPS_LOCK,	/* Caps lock */
        0,	/* 59 - F1 key ... > */
        0,   0,   0,   0,   0,   0,   0,   0,
        0,	/* < ... F10 */
        0,	/* 69 - Num lock*/
        0,	/* Scroll Lock */
        0,	/* Home key */
        ARROW_UP,	/* Up Arrow */
        PAGE_UP,	/* Page Up */
        '-',
        ARROW_LEFT,	/* Left Arrow */
        0,
        ARROW_RIGHT,	/* Right Arrow */
        '+',
        0,	/* 79 - End key*/
        ARROW_DOWN,	/* Down Arrow */
        PAGE_DOWN,	/* Page Down */
        0,	/* Insert Key */
        0,	/* Delete Key */
        0,   0,   0,
        0,	/* F11 Key */
        0,	/* F12 Key */
        0,	/* All other keys are undefined */
    };

    /// <summary>
    /// A table of what keys are pressed.
    /// </summary>
    volatile bool KeyCache[255];

    /// <summary>
    /// The last pressed key.
    /// </summary>
    volatile uint8_t CurrentKey = 0;

    /// <summary>
    /// If the caps lock key has been pressed
    /// </summary>
    bool CapsLocked = false;

    void HandleScanCode(uint8_t ScanCode)
    {
        //Check if key is up
        bool IsUp = ScanCode & (0b10000000);
        ScanCode &= ~(0b10000000);

        //Convert to ascii
        uint8_t Key;
        if ((CapsLocked || KeyCache[LEFT_SHIFT]))
        {
            Key = ShiftedScanCodeTable[ScanCode];
        }
        else
        {
            Key = ScanCodeTable[ScanCode];
        }

        //Handle key
        switch (Key)   
        {
        case CAPS_LOCK:
        {
            if (!IsUp)
            {
                CapsLocked = !CapsLocked;
            }
        }
        break;
        case LEFT_SHIFT:
        {
            KeyCache[Key] = !IsUp;
        }
        break;
        default:
        {
            KeyCache[Key] = !IsUp;

            CurrentKey = Key * !IsUp;
        }
        break;
        }
    }

    char GetKeyPress()
    {
        char Key = CurrentKey;
        CurrentKey = 0;
        return Key;
    }

    char GetKeyHeld()
    {
        return CurrentKey;
    }

    bool IsPressed(uint8_t Key)
    {
        bool IsPressed = KeyCache[Key];
        KeyCache[Key] = false;
        if (CurrentKey == Key)
        {
            CurrentKey = 0;
        }
        return IsPressed;
    }

    bool IsHeld(uint8_t Key)
    {
        return KeyCache[Key];
    }
}
