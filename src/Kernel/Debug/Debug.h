#pragma once

namespace Debug
{    
    /// <summary>
    /// Halts the system, clears the interupt flag and prints the given message.
    /// After this function has been called the only way to return is to reboot the pc.
    /// </summary>
    void Error(const char* Message);
}
