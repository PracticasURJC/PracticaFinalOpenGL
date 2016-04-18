#include "Block.h"
#include "Common.h"
#include "Game.h"

SubBlock::SubBlock()
{
    m_width = 1;
    m_height = 1;
    ID = 0;
}

SubBlock::SubBlock(uint32 &id)
{
    m_width = 1;
    m_height = 1;
    ID = id++;
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
        SubBlock* sub = new SubBlock(currentID);
        Position pos = positions[i];
        SetColor(Block::GetColorByType(m_type));
        sub->SetColor(Block::GetColorByType(m_type));
        sub->SetPosition(pos);
        m_subBlocks.push_back(sub);

        DEBUG_LOG("SubBlock created in position X: %f, Y: %f\n", pos.x, pos.y);
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
        sub->SetPositionX(sub->GetPositionX() + 1 * cos(newFloatRotation));
        sub->SetPositionY(sub->GetPositionY() + 1 * sin(newFloatRotation));
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
    if (m_position.y == 0.0f)
        return;

    float posY = m_position.y;

    for (posY; posY > 0.0f; posY--)
    {
        if (m_game->GetSubBlockInPosition(m_position.x, posY - 1.0f))
            break;
    }

    if (m_position.y != posY)
        m_position.y = posY;
}

bool Block::CanDropBlock()
{
    if (m_position.y == 0.0f)
        return false;

    if (m_game->GetSubBlockInPosition(m_position.x, m_position.y - 1))
        return false;

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
    case TYPE_CUBE:
        color = COLOR_WHITE;
        break;
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
    default:
        break;
    }
    return color;
}
