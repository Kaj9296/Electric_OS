#include "Tetris.h"

#include "STL/Graphics/Framebuffer.h"
#include "STL/String/String.h"
#include "STL/String/cstr.h"
#include "STL/System/System.h"
#include "STL/GUI/Button.h"
#include "STL/GUI/Label.h"
#include "STL/Math/Math.h"

#define BLOCK_SIZE 30
#define GRID_WIDTH 10
#define GRID_HEIGHT 20

#define GRID_OUTLINE_COLOR STL::ARGB(200)
#define GRID_OUTLINE_SHADOW STL::ARGB(60)
#define GRID_OUTLINE_HIGHLIGHT STL::ARGB(255)

#define LIGHT_BLUE_COLOR STL::ARGB(255, 0, 255, 255)
#define LIGHT_BLUE_SHADOW STL::ARGB(255, 0, 180, 179)
#define LIGHT_BLUE_HIGHLIGHT STL::ARGB(255, 180, 255, 255)

#define DARK_BLUE_COLOR STL::ARGB(255, 0, 0, 171)
#define DARK_BLUE_SHADOW STL::ARGB(255, 0, 0, 120)
#define DARK_BLUE_HIGHLIGHT STL::ARGB(255, 180, 181, 231)

#define ORANGE_COLOR STL::ARGB(255, 255, 120, 0)
#define ORANGE_SHADOW STL::ARGB(255, 179, 82, 0)
#define ORANGE_HIGHLIGHT STL::ARGB(255, 255, 215, 180)

#define YELLOW_COLOR STL::ARGB(255, 255, 255, 0)
#define YELLOW_SHADOW STL::ARGB(255, 179, 180, 0)
#define YELLOW_HIGHLIGHT STL::ARGB(255, 255, 255, 180)

#define PURPLE_COLOR STL::ARGB(255, 205, 0, 205)
#define PURPLE_SHADOW STL::ARGB(255, 143, 0, 143)
#define PURPLE_HIGHLIGHT STL::ARGB(255, 240, 181, 240)

#define GREEN_COLOR STL::ARGB(255, 0, 255, 0)
#define GREEN_SHADOW STL::ARGB(255, 0, 180, 0)
#define GREEN_HIGHLIGHT STL::ARGB(255, 179, 255, 179)

#define RED_COLOR STL::ARGB(255, 255, 0, 0)
#define RED_SHADOW STL::ARGB(255, 181, 0, 0)
#define RED_HIGHLIGHT STL::ARGB(255, 255, 181, 180)

#define WHITE_COLOR STL::ARGB(255)

#define PIECE_BLOCK_COUNT 4

#define START_FALL_RATE 100

namespace Tetris
{
    enum class BlockType
    {
        EMPTY,
        GRID_OUTLINE,   
        LIGHT_BLUE,
        DARK_BLUE, 
        ORANGE,
        YELLOW,
        PURPLE,
        GREEN,
        RED
    };

    BlockType Grid[GRID_WIDTH * GRID_HEIGHT];

    uint64_t CurrentFallRate = START_FALL_RATE;
    
    void(*CurrentAnimation)(STL::Framebuffer*);
    uint64_t AnimationCounter = 0;

    void Reset();

    inline void StartAnimation(void(*Animation)(STL::Framebuffer*))
    {
        AnimationCounter = 0;
        CurrentAnimation = Animation;
    }

    STL::Point GetScreenPosition(STL::Point Pos)
    {
        return (Pos + 1) * BLOCK_SIZE;
    }

    void DrawBlock(STL::Framebuffer* Buffer, STL::Point GridPos, STL::ARGB Color, STL::ARGB Shadow, STL::ARGB HighLight)
    {
        STL::Point ScreenPos = GetScreenPosition(GridPos);

        Buffer->DrawRectEdge(ScreenPos + RAISED_WIDTH, ScreenPos + BLOCK_SIZE - RAISED_WIDTH, HighLight, Shadow);
        Buffer->DrawRect(ScreenPos + RAISED_WIDTH, ScreenPos + BLOCK_SIZE - RAISED_WIDTH, Color);
    }

    void DrawBlock(STL::Framebuffer* Buffer, STL::Point GridPos, BlockType Type)
    {
        switch (Type)
        {
        case BlockType::GRID_OUTLINE:
        {
            DrawBlock(Buffer, GridPos, GRID_OUTLINE_COLOR, GRID_OUTLINE_SHADOW, GRID_OUTLINE_HIGHLIGHT);
        }   
        break;
        case BlockType::LIGHT_BLUE:
        {
            DrawBlock(Buffer, GridPos, LIGHT_BLUE_COLOR, LIGHT_BLUE_SHADOW, LIGHT_BLUE_HIGHLIGHT);
        }   
        break;        
        case BlockType::DARK_BLUE:
        {
            DrawBlock(Buffer, GridPos, DARK_BLUE_COLOR, DARK_BLUE_SHADOW, DARK_BLUE_HIGHLIGHT);
        }   
        break;           
        case BlockType::ORANGE:
        {
            DrawBlock(Buffer, GridPos, ORANGE_COLOR, ORANGE_SHADOW, ORANGE_HIGHLIGHT);
        }   
        break;             
        case BlockType::YELLOW:
        {
            DrawBlock(Buffer, GridPos, YELLOW_COLOR, YELLOW_SHADOW, YELLOW_HIGHLIGHT);
        }   
        break;             
        case BlockType::PURPLE:
        {
            DrawBlock(Buffer, GridPos, PURPLE_COLOR, PURPLE_SHADOW, PURPLE_HIGHLIGHT);
        }   
        break;         
        case BlockType::GREEN:
        {
            DrawBlock(Buffer, GridPos, GREEN_COLOR, GREEN_SHADOW, GREEN_HIGHLIGHT);
        }   
        break;         
        case BlockType::RED:
        {
            DrawBlock(Buffer, GridPos, RED_COLOR, RED_SHADOW, RED_HIGHLIGHT);
        }   
        break;
        case BlockType::EMPTY:
        {        
            STL::Point ScreenPos = GetScreenPosition(GridPos);
            Buffer->DrawRect(ScreenPos, ScreenPos + BLOCK_SIZE, STL::ARGB(0));
        }   
        break;
        }
    }

    STL::Point RotatePoint(STL::Point P, uint8_t Steps)
    {
        STL::Point Result = P;

        for (int i = 0; i < Steps; i++)
        {
            Result = STL::Point(Result.Y, Result.X * -1);
        }

        return Result;
    }

    const STL::Point* GetShape(BlockType Type, uint8_t RotationSteps)
    {
        switch (Type)
        {
        case BlockType::LIGHT_BLUE:
        {    
            static const STL::Point Piece[] = {STL::Point(-1, 0), STL::Point(0, 0), STL::Point(1, 0), STL::Point(2, 0)};
            static STL::Point RotatedPiece[PIECE_BLOCK_COUNT] = {};

            for (int i = 0; i < PIECE_BLOCK_COUNT; i++)
            {
                RotatedPiece[i] = RotatePoint(Piece[i], RotationSteps);
            }

            return RotatedPiece;
        }   
        break;        
        case BlockType::DARK_BLUE:
        {
            static const STL::Point Piece[] = {STL::Point(0, 1), STL::Point(0, 0), STL::Point(1, 0), STL::Point(2, 0)};
            static STL::Point RotatedPiece[PIECE_BLOCK_COUNT] = {};

            for (int i = 0; i < PIECE_BLOCK_COUNT; i++)
            {
                RotatedPiece[i] = RotatePoint(Piece[i], RotationSteps);
            }

            return RotatedPiece;
        }   
        break;           
        case BlockType::ORANGE:
        {
            static const STL::Point Piece[] = {STL::Point(-1, 0), STL::Point(0, 0), STL::Point(1, 0), STL::Point(1, 1)};
            static STL::Point RotatedPiece[PIECE_BLOCK_COUNT] = {};

            for (int i = 0; i < PIECE_BLOCK_COUNT; i++)
            {
                RotatedPiece[i] = RotatePoint(Piece[i], RotationSteps);
            }

            return RotatedPiece;
        }   
        break;             
        case BlockType::YELLOW:
        {
            static const STL::Point Piece[] = {STL::Point(1, 1), STL::Point(0, 0), STL::Point(1, 0), STL::Point(0, 1)};
            static STL::Point RotatedPiece[PIECE_BLOCK_COUNT] = {};

            for (int i = 0; i < PIECE_BLOCK_COUNT; i++)
            {
                RotatedPiece[i] = RotatePoint(Piece[i], RotationSteps);
            }

            return RotatedPiece;
        }   
        break;             
        case BlockType::PURPLE:
        {
            static const STL::Point Piece[] = {STL::Point(-1, 0), STL::Point(0, 0), STL::Point(0, 1), STL::Point(1, 0)};
            static STL::Point RotatedPiece[PIECE_BLOCK_COUNT] = {};

            for (int i = 0; i < PIECE_BLOCK_COUNT; i++)
            {
                RotatedPiece[i] = RotatePoint(Piece[i], RotationSteps);
            }

            return RotatedPiece;
        }   
        break;         
        case BlockType::GREEN:
        {
            static const STL::Point Piece[] = {STL::Point(-1, 0), STL::Point(0, 0), STL::Point(0, 1), STL::Point(1, 1)};
            static STL::Point RotatedPiece[PIECE_BLOCK_COUNT] = {};

            for (int i = 0; i < PIECE_BLOCK_COUNT; i++)
            {
                RotatedPiece[i] = RotatePoint(Piece[i], RotationSteps);
            }

            return RotatedPiece;
        }   
        break;         
        case BlockType::RED:
        {
            static const STL::Point Piece[] = {STL::Point(-1, 1), STL::Point(0, 1), STL::Point(0, 0), STL::Point(1, 0)};
            static STL::Point RotatedPiece[PIECE_BLOCK_COUNT] = {};

            for (int i = 0; i < PIECE_BLOCK_COUNT; i++)
            {
                RotatedPiece[i] = RotatePoint(Piece[i], RotationSteps);
            }

            return RotatedPiece;
        }   
        break;
        default:
        {        
            return nullptr;
        }   
        break;
        }        
    }

    void WritePiece(STL::Point GridPos, uint8_t RotationSteps, BlockType ShapeType, BlockType ColorType)
    {
        const STL::Point* Shape = GetShape(ShapeType, RotationSteps);

        if (Shape == nullptr)
        {
            return;
        }

        for (uint32_t i = 0; i < 4; i++)
        {
            STL::Point BlockPos = GridPos + Shape[i];
            Grid[BlockPos.X + BlockPos.Y * GRID_WIDTH] = ColorType;
        }
    }

    void DrawGridOutline(STL::Framebuffer* Buffer)
    {
        for (int i = -1; i < GRID_HEIGHT + 1; i ++)
        {
            DrawBlock(Buffer, STL::Point(-1, i), BlockType::GRID_OUTLINE);
            DrawBlock(Buffer, STL::Point(GRID_WIDTH, i), BlockType::GRID_OUTLINE);
        }        
        
        for (int i = -1; i < GRID_WIDTH + 1; i++)
        {
            DrawBlock(Buffer, STL::Point(i, -1), BlockType::GRID_OUTLINE);
            DrawBlock(Buffer, STL::Point(i, GRID_HEIGHT), BlockType::GRID_OUTLINE);
        }
    }

    void DrawGrid(STL::Framebuffer* Buffer)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            for (int y = 0; y < GRID_HEIGHT; y++)
            {
                DrawBlock(Buffer, STL::Point(x, y), Grid[x + y * GRID_WIDTH]);
            }
        }
    }
    
    void ClearGrid()
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            for (int y = 0; y < GRID_HEIGHT; y++)
            {
                Grid[x + y * GRID_WIDTH] = BlockType::EMPTY;
            }
        }
    }

    void GameOverAnimation(STL::Framebuffer* Buffer)
    {
        static bool DrawText = true;

        if (AnimationCounter <= 1)
        {
            DrawGridOutline(Buffer);
            
            Reset();

            DrawGrid(Buffer); 
            DrawText = true;
        }
        else if (AnimationCounter % 35 == 0)
        {
            DrawText = !DrawText;            
        }

        if (DrawText)
        {
            const uint8_t Scale = 3;
            const uint8_t TextWidth = 8 * Scale;
            STL::Point TextPos = STL::Point(Buffer->Width / 2 - (TextWidth * 11) / 2, Buffer->Height - Buffer->Height / 3);

            Buffer->Print("PRESS ENTER", TextPos, Scale, GRID_OUTLINE_COLOR, STL::ARGB(0)); 
        }
        else
        {
            DrawGrid(Buffer);                   
        }

        const uint8_t Scale = 4;
        const uint8_t TextWidth = 8 * Scale;
        STL::Point TextPos = STL::Point(Buffer->Width / 2 - TextWidth * (8 / 2), Buffer->Height / 5);

        Buffer->PutChar('G', TextPos + STL::Point(TextWidth * 0, 0), Scale, RED_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('A', TextPos + STL::Point(TextWidth * 1, 0), Scale, ORANGE_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('M', TextPos + STL::Point(TextWidth * 2, 0), Scale, YELLOW_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('E', TextPos + STL::Point(TextWidth * 3, 0), Scale, GREEN_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('O', TextPos + STL::Point(TextWidth * 4, 0), Scale, LIGHT_BLUE_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('V', TextPos + STL::Point(TextWidth * 5, 0), Scale, DARK_BLUE_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('E', TextPos + STL::Point(TextWidth * 6, 0), Scale, PURPLE_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('R', TextPos + STL::Point(TextWidth * 7, 0), Scale, RED_COLOR, STL::ARGB(0)); 

    }

    void LineClearAnimation(STL::Framebuffer* Buffer)
    {
        const uint8_t AnimationSpeed = 2;

        bool LineFound = false;

        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            bool LineFull = true;
            for (int x = 0; x < GRID_WIDTH; x++)
            {
                if (Grid[x + y * GRID_WIDTH] == BlockType::EMPTY)
                {
                    LineFull = false;
                    break;
                }
            }

            if (LineFull)
            {                          
                LineFound = true;
                uint8_t Left = 5 - (AnimationCounter / AnimationSpeed);
                uint8_t Right = 4 + (AnimationCounter / AnimationSpeed);

                DrawBlock(Buffer, STL::Point(Left, y), STL::ARGB(0), STL::ARGB(0), STL::ARGB(0));
                DrawBlock(Buffer, STL::Point(Right, y), STL::ARGB(0), STL::ARGB(0), STL::ARGB(0));

                if (Left == 0)
                {
                    for (int y2 = y; y2 >= 1; y2--)
                    {
                        for (int x = 0; x < GRID_WIDTH; x++)
                        {
                            Grid[x + y2 * GRID_WIDTH] = Grid[x + (y2 - 1) * GRID_WIDTH];
                        }
                    }
                }
            }
        }

        if (!LineFound)
        {                    
            StartAnimation(nullptr);
        }
    }

    void SplashScreenAnimation(STL::Framebuffer* Buffer)
    {
        static bool DrawText = true;

        if (AnimationCounter <= 1)
        {
            DrawGridOutline(Buffer);

            DrawGrid(Buffer); 
            DrawText = true;
        }
        else if (AnimationCounter % 35 == 0)
        {
            DrawText = !DrawText;            
        }

        if (DrawText)
        {
            const uint8_t Scale = 3;
            const uint8_t TextWidth = 8 * Scale;
            STL::Point TextPos = STL::Point(Buffer->Width / 2 - (TextWidth * 11) / 2, Buffer->Height - Buffer->Height / 3);

            Buffer->Print("PRESS ENTER", TextPos, Scale, GRID_OUTLINE_COLOR, STL::ARGB(0)); 
        }
        else
        {
            DrawGrid(Buffer);                   
        }

        const uint8_t Scale = 5;
        const uint8_t TextWidth = 8 * Scale;
        STL::Point TitlePos = STL::Point(Buffer->Width / 2 - TextWidth * (6 / 2), Buffer->Height / 5);

        Buffer->PutChar('T', TitlePos + STL::Point(TextWidth * 0, 0), Scale, RED_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('E', TitlePos + STL::Point(TextWidth * 1, 0), Scale, ORANGE_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('T', TitlePos + STL::Point(TextWidth * 2, 0), Scale, YELLOW_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('R', TitlePos + STL::Point(TextWidth * 3, 0), Scale, GREEN_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('I', TitlePos + STL::Point(TextWidth * 4, 0), Scale, LIGHT_BLUE_COLOR, STL::ARGB(0)); 
        Buffer->PutChar('S', TitlePos + STL::Point(TextWidth * 5, 0), Scale, DARK_BLUE_COLOR, STL::ARGB(0)); 
    }

    namespace CurrentPiece
    {
        STL::Point Position;
        BlockType Type;
        uint8_t RotationSteps = 0;

        void Draw(STL::Framebuffer* Buffer)
        {
            const STL::Point* Shape = GetShape(Type, RotationSteps);
            if (Shape == nullptr)
            {
                return;
            }
            for (uint32_t i = 0; i < 4; i++)
            {
                STL::Point BlockPos = Position + Shape[i];
                DrawBlock(Buffer, BlockPos, Type);
            }
        }

        void Write()
        {
            WritePiece(Position, RotationSteps, Type, Type);
        }

        void Generate()
        {            
            RotationSteps = 0;
            Position = STL::Point(GRID_WIDTH / 2, 0);
            Type = (BlockType)(STL::Rand() % 7 + 2);            
            
            const STL::Point* Shape = GetShape(Type, RotationSteps);
            if (Shape == nullptr)
            {
                return;
            }
            for (uint32_t i = 0; i < 4; i++)
            {
                STL::Point BlockPos = Position + Shape[i];
                if (Grid[BlockPos.X + BlockPos.Y * GRID_WIDTH] != BlockType::EMPTY)
                {            
                    StartAnimation(GameOverAnimation);
                }
            }
        }

        bool Move(STL::Point Offset)
        {            
            if (Type == BlockType::EMPTY)
            {
                Generate();
            }

            STL::Point NewPos = Position + Offset;

            const STL::Point* Shape = GetShape(Type, RotationSteps);
            if (Shape == nullptr)
            {
                return false;
            }
            for (uint32_t i = 0; i < 4; i++)
            {
                STL::Point BlockPos = NewPos + Shape[i];
                if (BlockPos.X < 0 || BlockPos.X >= GRID_WIDTH || BlockPos.Y < 0 || BlockPos.Y >= GRID_HEIGHT || Grid[BlockPos.X + BlockPos.Y * GRID_WIDTH] != BlockType::EMPTY)
                {            
                    return false;
                }
            }

            Position = NewPos;

            return true;
        }

        void Rotate()
        {                       
            if (Type == BlockType::EMPTY)
            {
                Generate();
            }

            const STL::Point* NewShape = GetShape(Type, RotationSteps + 1);
            if (NewShape == nullptr)
            {
                return;
            }
            for (uint32_t i = 0; i < 4; i++)
            {
                STL::Point BlockPos = Position + NewShape[i];
                if (BlockPos.X < 0 || BlockPos.X >= GRID_WIDTH || BlockPos.Y < 0 || BlockPos.Y >= GRID_HEIGHT || Grid[BlockPos.X + BlockPos.Y * GRID_WIDTH] != BlockType::EMPTY)
                {            
                    return;
                }
            }

            RotationSteps++;
        }

        void DrawOutline(STL::Framebuffer* Buffer)
        {
            const STL::Point* Shape = GetShape(Type, RotationSteps);
            if (Shape == nullptr || Type == BlockType::EMPTY)
            {
                return;
            }

            int8_t Offset = 0;

            while (true)
            {            
                for (uint32_t i = 0; i < 4; i++)
                {
                    STL::Point BlockPos = Position + Shape[i] + STL::Point(0, Offset);
                    if (BlockPos.Y >= GRID_HEIGHT || Grid[BlockPos.X + BlockPos.Y * GRID_WIDTH] != BlockType::EMPTY)
                    {            
                        Offset--;
                        for (uint32_t j = 0; j < 4; j++)
                        {                    
                            STL::Point ScreenPos = GetScreenPosition(Position + Shape[j] + STL::Point(0, Offset));
                            Buffer->DrawRectEdge(ScreenPos + RAISED_WIDTH, ScreenPos + BLOCK_SIZE - RAISED_WIDTH, WHITE_COLOR, WHITE_COLOR);
                        }
                        return;
                    }
                }
                Offset++;     
            }
        }
    }

    void Reset()
    {
        CurrentPiece::Type = BlockType::EMPTY;
        ClearGrid();
    }

    STL::PROR Procedure(STL::PROM Message, STL::PROI Input)
    {
        switch(Message)
        {
        case STL::PROM::INIT:
        {
            STL::PINFO* Info = (STL::PINFO*)Input;
            Info->Type = STL::PROT::WINDOWED;
            Info->Depth = 1;
            Info->Left = 360;
            Info->Top = 200;
            Info->Width = (GRID_WIDTH + 2) * BLOCK_SIZE;
            Info->Height = (GRID_HEIGHT + 2) * BLOCK_SIZE;
            Info->Title = "Tetris";

            Reset();

            ClearGrid();

            StartAnimation(SplashScreenAnimation);
        }
        break;
        case STL::PROM::DRAW:
        {
            STL::Framebuffer* Buffer = (STL::Framebuffer*)Input;           
            
            if (CurrentAnimation != nullptr)
            {
                CurrentAnimation(Buffer);
                AnimationCounter++;

                if (CurrentAnimation == nullptr)
                {
                    return STL::PROR::DRAW;
                }
            }
            else
            {
                DrawGrid(Buffer);
                CurrentPiece::Draw(Buffer);
                CurrentPiece::DrawOutline(Buffer);
            }   
        }
        break;        
        case STL::PROM::TICK:
        {   
            uint64_t CurrentTick = *(uint64_t*)Input;

            if (CurrentAnimation != nullptr)
            {
                return STL::PROR::DRAW;
            }
            else if (CurrentTick % CurrentFallRate == 0)
            {   
                if (!CurrentPiece::Move(STL::Point(0, 1)))
                {
                    CurrentPiece::Write();            
                    StartAnimation(LineClearAnimation);
                    CurrentPiece::Generate();
                }
                return STL::PROR::DRAW;
            }
        }
        break;
        case STL::PROM::KEYPRESS:
        {
            uint8_t Key = *(uint8_t*)Input;

            if (CurrentAnimation != nullptr)
            {
                if (Key == ENTER)
                {
                    StartAnimation(nullptr);
                }
            }

            switch (Key)
            {
            case 's':
            {
                if (!CurrentPiece::Move(STL::Point(0, 1)))
                {                    
                    CurrentPiece::Write();                    
                    StartAnimation(LineClearAnimation);
                    CurrentPiece::Generate();
                }
            }
            break;
            case 'a':
            {
                if (!CurrentPiece::Move(STL::Point(-1, 0)))
                {
                    //CurrentPiece::Generate();
                }
            }
            break;
            case 'd':
            {
                if (!CurrentPiece::Move(STL::Point(1, 0)))
                {
                    //CurrentPiece::Generate();
                }
            }
            break;            
            case 'r':
            {
                CurrentPiece::Rotate();
            }
            break;            
            }

            return STL::PROR::DRAW;
        }
        break;        
        case STL::PROM::MOUSE:
        {
            STL::MDATA MouseInfo = *(STL::MDATA*)Input;

            return STL::PROR::DRAW;
        }
        break;
        default:
        {

        }
        break;
        }

        return STL::PROR::SUCCESS;
    }
}