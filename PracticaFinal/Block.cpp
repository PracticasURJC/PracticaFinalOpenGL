#include "Block.h"
#include "Common.h"
#include "Game.h"

SubBlock::SubBlock()
{
    m_width = 1;
    m_height = 1;
    ID = 0;
    m_game = nullptr;
}

SubBlock::SubBlock(Game* game)
{
    m_width = 1;
    m_height = 1;
    m_game = game;

    ID = game->GetCurrentBlockID();
    game->IncreaseCurrentBlockID();
}

SubBlock::~SubBlock()
{
}

void SubBlock::Delete()
{
    
}

Block::Block(uint8 _type, Game* _game, float x, float y)
{
    m_type = _type;
    m_game = _game;
    m_position.x = x;
    m_position.y = y;
    m_subBlocks.clear();
    GenerateSubBlocks();
    CalculateDimensions();
}

Block::~Block()
{
    m_subBlocks.clear();
}

void Block::GenerateSubBlocks()
{
    uint32 currentID = m_game->GetCurrentBlockID();

    // Always have 4 subBlocks
    Position* positions = Block::GetPositionsOfType(m_type);
    for (uint8 i = 0; i < NUM_BLOCK_SUBBLOCKS; i++)
    {
        SubBlock* sub = new SubBlock(m_game);
        Position pos = positions[i];
        SetColor(Block::GetColorByType(m_type));
        sub->SetColor(Block::GetColorByType(m_type));
        sub->SetPosition(pos);
        m_subBlocks.push_back(sub);

        DEBUG_LOG("SubBlock ID: %u created in position X: %f, Y: %f\n", sub->GetID(), pos.x, pos.y);
    }
}

void Block::CalculateDimensions()
{
    float minPosX = 0, maxPosX = 0;
    float minPosY = 0, maxPosY = 0;

    for (SubBlock *sub : m_subBlocks)
    {
        minPosX = std::min(minPosX, sub->GetPositionX());
        maxPosX = std::max(maxPosX, sub->GetPositionX());
        
        minPosY = std::min(minPosY, sub->GetPositionY());
        maxPosY = std::max(maxPosY, sub->GetPositionY());
    }

    // Care about overflow
    m_height = uint8(maxPosY - minPosY) + 1;
    m_width = uint8(maxPosX - minPosX) + 1;
    
    DEBUG_LOG("MinPosY = %f, MaxPosY = %f\n", minPosY, maxPosY);
    DEBUG_LOG("Dimensions of block %u. Height: %d, Width: %d\n", m_type, m_height, m_width);
}

void Block::RotateBlock()
{
    uint32 newRotation = uint32(m_position.rotation) + 90 % 360;

    float newFloatRotation = newRotation * 2* M_PI / 360.0f;

    DEBUG_LOG("rotation %d, %f\n", newRotation, newFloatRotation);


    for(SubBlock* sub : m_subBlocks)
    {
        /*
        
        [oldPosX]      [cos][-sin][0]      [newPosX]
        [oldPosY]  x   [sin][ cos][0]  =   [newPosY]
        [      1]      [  0][   0][1]      [      1]
        
        
        */

        float oldPosX = sub->GetPositionX();
        float oldPosY = sub->GetPositionY();
        float newPosX = 0.0f;
        float newPosY = 0.0f;
        float _a[1][3] = { { oldPosX, oldPosY, 1.0f } };
        float matrix[3][3] = {
            {cos(newFloatRotation), -sin(newFloatRotation), 0.0f },
            {sin(newFloatRotation),  cos(newFloatRotation), 0.0f },
            {                 0.0f,                   1.0f, 1.0f }
        };
        float a[1][3] = { { newPosX, newPosY, 1.0f } };

        uint8 c, d, k = 0;
        float sum = 0.0f;
        for (c = 0; c < 1; c++) {
            for (d = 0; d < 3; d++) {
                for (k = 0; k < 3; k++) {
                    sum = sum + _a[c][k] * matrix[k][d];
                }
 
                a[c][d] = sum;
                sum = 0;
            }
        }

        sub->SetPositionX(a[0][0]);
        sub->SetPositionY(a[0][1]);

        DEBUG_LOG("SubBlock OldPosition: (%f, %f), newPosition: (%f, %f)\n", oldPosX, oldPosY, sub->GetPositionX(), sub->GetPositionY());
    }

    m_position.rotation = float(newRotation);
    CalculateDimensions();
}

Position* Block::GetPositionsOfType(uint8 type)
{
    static Position positions[NUM_BLOCK_SUBBLOCKS];
    switch(type)
    {
        case TYPE_CUBE:
            positions[0] = {0.0f, 0.0f};
            positions[1] = {1.0f, 0.0f};
            positions[2] = {0.0f, 1.0f};
            positions[3] = {1.0f, 1.0f};
            break;
        case TYPE_PRISM:
            positions[0] = {0.0f, 0.0f};
            positions[1] = {1.0f, 0.0f};
            positions[2] = {2.0f, 0.0f};
            positions[3] = {3.0f, 0.0f};
            break;
        case TYPE_T:
            positions[0] = {0.0f, 0.0f};
            positions[1] = {1.0f, 0.0f};
            positions[2] = {2.0f, 0.0f};
            positions[3] = {1.0f, 1.0f};
            break;
        case TYPE_Z:
            positions[0] = {0.0f, 0.0f};
            positions[1] = {1.0f, 0.0f};
            positions[2] = {1.0f, 1.0f};
            positions[3] = {2.0f, 1.0f};
            break;
        case TYPE_L:
            positions[0] = {0.0f, 0.0f};
            positions[1] = {1.0f, 0.0f};
            positions[2] = {2.0f, 0.0f};
            positions[3] = {2.0f, 1.0f};
            break;
        default:
            break;
    }

    return positions;
}

void Block::Drop()
{
    float posY = m_position.y;

    bool found = false;
    for (SubBlock* sub : GetSubBlocks())
    {
        if (found)
            break;

        posY = m_position.y;
        for (posY; posY > 0.0f; posY--)
        {
            if (posY + sub->GetPositionY() <= 0.0f)
            {
                found = true;
                break;
            }

            if (SubBlock* temp = m_game->GetSubBlockInPosition(m_position.x + sub->GetPositionX(), posY + sub->GetPositionY()))
            {
                found = true;
                DEBUG_LOG("Block %d hits with block %d\n", sub->GetID(), temp->GetID());
                break;
            }
        }
    }

    if (m_position.y != posY)
        m_position.y = posY;
}

bool Block::CanDropBlock()
{
    for (SubBlock* sub : GetSubBlocks())
    {
        if (m_position.y + sub->GetPositionY() <= 0.0f)
            return false;

        if (m_game->GetSubBlockInPosition(m_position.x + sub->GetPositionX(), m_position.y + sub->GetPositionY()))
            return false;
    }

    return true;
}

void Block::MoveBlock(bool right)
{
    if (right)
    {
        if (m_position.x + m_width + 1.0f > MAX_WIDTH)
            return;

        float posX = std::min<float>(MAX_WIDTH, m_position.x + 1.0f);
        m_position.x = posX;
    }
    else
    {
        if (m_position.x + m_width - 1.0f < 0.0f)
            return;

        float posX = std::max<float>(0.0f, m_position.x - 1.0f);
        m_position.x = posX;
    }
}

uint8 Block::GetColorByType(uint8 type)
{
    uint8 color = COLOR_WHITE;
    switch (type)
    {
    case TYPE_PRISM:
        color = COLOR_GREEN;
        break;
    case TYPE_Z:
        color = COLOR_RED;
        break;
    case TYPE_L:
        color = COLOR_BLUE;
        break;
    case TYPE_T:
        color = COLOR_PINK;
        break;
    case TYPE_CUBE:
    default:
        break;
    }
    return color;
}
