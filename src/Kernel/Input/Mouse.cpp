#include "Mouse.h"

#include <stdint.h>

#include "Kernel/IO/IO.h"
#include "Kernel/Renderer/Renderer.h"
#include "Kernel/ProcessHandler/ProcessHandler.h"

#define PS2Leftbutton   0b00000001
#define PS2Middlebutton 0b00000100
#define PS2Rightbutton  0b00000010
#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

namespace Mouse
{ 
    /// <summary>
    /// Waits untill the mouse is ready to recive data or untill a timeout is reached.
    /// </summary>
    void MouseWait()
    {   
        uint64_t TimeOut = 100000;

        while (TimeOut--)
        {
            if ((IO::InByte(0x64) & 0b10) == 0)
            {
                return;
            }
        }
    }

    /// <summary>
    /// Waits untill the mouse is ready to send data or untill a timeout is reached.
    /// </summary>
    void MouseWaitInput()
    {   
        uint64_t TimeOut = 100000;

        while (TimeOut--)
        {
            if (IO::InByte(0x64) & 0b1)
            {
                return;
            }
        }
    }

    /// <summary>
    /// Writes a command to the mouse port (0x60).
    /// </summary>
    void MouseWrite(uint8_t Value)
    {
        MouseWait();
        IO::OutByte(0x64, 0xD4);
        MouseWait();
        IO::OutByte(0x60, Value);
    }

    /// <summary>
    /// Reads date from the mouse port (0x60).
    /// </summary>
    uint8_t MouseRead()
    {
        MouseWaitInput();
        return IO::InByte(0x60);
    }

    void InitPS2()
    {
        IO::OutByte(0x64, 0xA8); //Enable mouse
        MouseWait();

        IO::OutByte(0x64, 0x20);
        MouseWaitInput();

        uint8_t Status = IO::InByte(0x60);
        Status |= 0b10;
        MouseWait();
        IO::OutByte(0x64, 0x60);
        MouseWait();
        IO::OutByte(0x60, Status); //Set the compaq status bit.

        MouseWrite(0xF6);
        MouseRead();

        MouseWrite(0xF4);
        MouseRead();
    }
    
    void HandleMouseByte(uint8_t MouseByte)
    {           
        static STL::Point MousePos = STL::Point(500, 500);

        static uint8_t MouseCycle = 0;
        static uint8_t MousePacket[4];

        switch(MouseCycle)
        {
        case 0:
        {
            if (((MouseByte & 0b00001000) == 0))
            {
                break;
            }
            MousePacket[0] = MouseByte;
            MouseCycle++;
        }
        break;
        case 1:
        {
            MousePacket[1] = MouseByte;
            MouseCycle++;
        }
        break;
        case 2:
        {
            MousePacket[2] = MouseByte;
            MouseCycle = 0;
            
            /// If sign bit is set move in negative direction.
            if (MousePacket[0] & PS2XSign)
            {      
                MousePacket[1] = 256 - MousePacket[1];
                MousePos.X -= MousePacket[1];
            } 
            else
            {
                MousePos.X += MousePacket[1];
            }

            if (MousePacket[0] & PS2YSign)
            {
                MousePacket[2] = 256 - MousePacket[2];
                MousePos.Y += MousePacket[2];
            }
            else
            {
                MousePos.Y -= MousePacket[2];
            }

            /// Check for button presses.    
            static bool LeftHeld = false;          
            static bool MiddleHeld = false;  
            static bool RightHeld = false;
            
            bool OldLeftHeld = LeftHeld;
            bool OldMiddleHeld = MiddleHeld;
            bool OldRightHeld = RightHeld;

            LeftHeld = MousePacket[0] & PS2Leftbutton;  
            MiddleHeld = MousePacket[0] & PS2Middlebutton;
            RightHeld = MousePacket[0] & PS2Rightbutton;
            
            /// Clamp mouse pos to the screen.
            MousePos.X = STL::Clamp(MousePos.X, 0, Renderer::GetScreenSize().X - 8);
            MousePos.Y = STL::Clamp(MousePos.Y, 0, Renderer::GetScreenSize().Y - 16);
            
            STL::MDATA MouseData;
            MouseData.Pos = MousePos;
            MouseData.LeftHeld = LeftHeld;
            MouseData.MiddleHeld = MiddleHeld;
            MouseData.RightHeld = RightHeld;
            MouseData.KeyStateChanged = (LeftHeld != OldLeftHeld) || (MiddleHeld != OldMiddleHeld) || (RightHeld != OldRightHeld);

            ProcessHandler::MouseInterupt(MouseData);
        }
        break;
        }
    }
}