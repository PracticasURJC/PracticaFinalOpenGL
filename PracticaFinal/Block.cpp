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
    m_game->DeleteSubBlock(this);
}

bool SubBlock::CanDropSubBlock()
{
    if (m_position.y <= 0.0f)
    {
        DEBUG_LOG("Block %d hit, can't be dropped (<0).\n", ID);
        return false;
    }

    if (SubBlock* temp = m_game->GetSubBlockInPosition(m_position.x, m_position.y - 1.0f))
    {
        DEBUG_LOG("Block %d hits with block %d\n", ID, temp->GetID());
        return false;
    }

    return true;
}

Block::Block(uint8 type, Game* game, float x, float y)
{
    m_type = type;
    m_game = game;
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

bool Block::CanRotateBlock()
{
    // Cube should not rotate
    if (m_type == TYPE_CUBE)
        return false;

    for (SubBlock* sub : m_subBlocks)
    {
        float oldPosX = sub->GetPositionX();
        float oldPosY = sub->GetPositionY();
        float newPosX = oldPosX * cos(M_PI_2) - oldPosY * sin(M_PI_2);
        float newPosY = oldPosX * sin(M_PI_2) + oldPosY * cos(M_PI_2);

        if (m_position.x + newPosX < 0 || m_position.x + newPosX >= MAX_WIDTH - 1)
            return false;

        if (m_position.y + newPosY < 0)
            return false;

        if (m_game->GetSubBlockInPosition(m_position.x + newPosX, m_position.y + newPosY))
            return false;
    }
    return true;
}


void Block::RotateBlock()
{
    if (!CanRotateBlock())
        return;

    uint32 newRotation = uint32(m_position.rotation) + 90 % 360;

    DEBUG_LOG("rotation %d\n", newRotation);

    for(SubBlock* sub : m_subBlocks)
    {
        float oldPosX = sub->GetPositionX();
        float oldPosY = sub->GetPositionY();
        float newPosX = oldPosX * cos(M_PI_2) - oldPosY * sin(M_PI_2);
        float newPosY = oldPosX * sin(M_PI_2) + oldPosY * cos(M_PI_2);

        sub->SetPositionX(newPosX);
        sub->SetPositionY(newPosY);

        DEBUG_LOG("SubBlock OldPosition: (%f, %f), newPosition: (%f, %f)\n", oldPosX, oldPosY, sub->GetPositionX(), sub->GetPositionY());
    }

    CalculateDimensions();
    m_position.rotation = float(newRotation);
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
            positions[0] = {-1.0f, 0.0f};
            positions[1] = {0.0f, 0.0f};
            positions[2] = {1.0f, 0.0f};
            positions[3] = {2.0f, 0.0f};
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
    bool found = false;

    float posY = m_position.y;
    for (posY; posY > 0.0f; posY -= 1.0f)
    {
        if (found)
            break;

        for (SubBlock* sub : GetSubBlocks())
        {
            if (posY + sub->GetPositionY() - 1.0f <= 0.0f)
            {
                found = true;
                break;
            }

            if (SubBlock* temp = m_game->GetSubBlockInPosition(m_position.x + sub->GetPositionX(), posY + sub->GetPositionY() - 2.0f))
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
        {
            DEBUG_LOG("Block %d hits can't be dropped (<0).\n", sub->GetID());
            return false;
        }

        if (SubBlock* temp = m_game->GetSubBlockInPosition(m_position.x + sub->GetPositionX(), m_position.y + sub->GetPositionY() - 2.0f))
        {
            DEBUG_LOG("Block %d hits with block %d\n", sub->GetID(), temp->GetID());
            return false;
        }
    }

    return true;
}

void Block::MoveBlock(bool right)
{
    if (right)
    {
        for (SubBlock* sub : m_subBlocks)

            if (m_position.x + sub->GetPositionX() >= MAX_WIDTH - 1)
                return;

        float posX = std::min<float>(MAX_WIDTH, m_position.x + 1.0f);
        m_position.x = posX;
    }
    else
    {
        for (SubBlock* sub : m_subBlocks)
            if (m_position.x + sub->GetPositionX() <= 0.0f)
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
        color = COLOR_ORANGE;
        break;
    default:
        break;
    }
    return color;
}

void SubBlock::DebugPosition()
{
    DEBUG_LOG("Block %u, Position [%f, %f, %f]\n", ID, m_position.x, m_position.y, m_position.z);
}

void Block::DebugPosition()
{
    for (SubBlock* sub : m_subBlocks)
        DEBUG_LOG("Block %u (ActiveBlock), Position [%f, %f, %f]\n", sub->GetID(), m_position.x + sub->GetPositionX(), m_position.y + sub->GetPositionY(), m_position.z + sub->GetPositionZ());
}
