#pragma once

#include "STL/List/List.h"
#include "STL/Process/Process.h"
#include "STL/Graphics/Framebuffer.h"

class Process
{
public:

    void Render(STL::Framebuffer* Buffer, STL::Point DomainOrigin, STL::Point DomainSize);

    void HandleRequest();

    void HandleMouseData(STL::MDATA MouseData);

    void SendFamilyMessage(STL::PROM Message, STL::PROI Input = nullptr);

    void SendMessage(STL::PROM Message, STL::PROI Input = nullptr);

    void PushRequest(STL::PROR Request);

    void Adopt(Process* Child);

    Process(STL::PROC Procedure, Process* Parent);

private:
    
    void Kill();

    STL::PROR PopRequest();
    
    STL::Point Pos;
    STL::String Title;

    STL::PROC Procedure;
    STL::Framebuffer FrameBuffer;

    STL::Point FrameSize;
    void(*RenderFrameFunction)(STL::Framebuffer* Buffer, STL::Point ScreenPos, STL::Point WindowSize, STL::String& Title);

    uint64_t RequestAmount;
    STL::PROR Requests[16];

    STL::List<Process*> Children;
    Process* Parent;
};