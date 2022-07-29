#pragma once

#include "STL/List/List.h"
#include "STL/Array/Array.h"
#include "STL/Process/Process.h"
#include "STL/Graphics/Framebuffer.h"
#include "STL/GUI/WindowFrame.h"

class Process
{
public:

    void Render(STL::Framebuffer* Buffer, STL::Point DomainOrigin, STL::Point DomainSize);

    void HandleRequest();

    void ProcessMouseData(STL::MDATA MouseData, STL::Point DomainOrigin);

    void SendFamilyMessage(STL::PROM Message, STL::PROI Input = nullptr);

    void SendMessage(STL::PROM Message, STL::PROI Input = nullptr);

    void PushRequest(STL::PROR Request);

    void Adopt(Process* Child);

    void MoveToFront();

    Process* FindFamilyMember(const char* TargetTitle);

    Process(STL::PROC Procedure, Process* Parent);

private:

    void SetPos(STL::Point NewPos);

    void Kill();

    STL::PROR PopRequest();
    
    STL::Point Pos;
    STL::String Title;

    STL::PROC Procedure;
    STL::Framebuffer FrameBuffer;

    STL::WindowFrame WindowFrame;
    STL::Array<STL::WindowButton, 8> WindowButtons;

    uint64_t RequestAmount;
    STL::PROR Requests[16];

    STL::List<Process*> Children;
    Process* Parent;
};