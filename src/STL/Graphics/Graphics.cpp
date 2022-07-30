#include "Graphics.h"

#include "STL/Array/Array.h"
#include "STL/String/cstr.h"

namespace STL
{
    struct Font
    {
        PSF_FONT* PSFFont;
        const char* Name;

        Font(PSF_FONT* NewFont, const char* FontName)
        {
            this->PSFFont = NewFont;
            this->Name = FontName;
        }

        Font() = default;
    };
    Font Fonts[16];
    uint32_t FontAmount = 0;

    uint8_t SelectedFont = 0;

    void AddFont(PSF_FONT* NewFont, const char* FontName)
    {
        if (FontAmount < 16)
        {
            Fonts[FontAmount] = Font(NewFont, FontName);
            FontAmount++;
        }
    }

    bool SetFont(const char* FontName)
    {
        for (int i = 0; i < FontAmount; i++)
        {
            if (StringCompare(Fonts[i].Name, FontName))
            {
                SelectedFont = i;
                return true;
            }
        }

        return false;
    }

    const PSF_FONT* GetSelectedFont()
    {
        if (SelectedFont >= FontAmount)
        {
            SelectedFont = 0;
        }

        return Fonts[SelectedFont].PSFFont;
    }
}