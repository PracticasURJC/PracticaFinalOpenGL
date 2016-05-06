#include "Block.h"
#include "Common.h"
#include "Game.h"

SubBlock::SubBlock()
{
    ID = 0;
    m_game = nullptr;
}

SubBlock::SubBlock(Game* game)
{
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

bool Block::CanRotateBlock()
{
    // Cube should not rotate
    if (m_type == TYPE_CUBE)
        return false;

    for (SubBlock* sub : m_subBlocks)
    {
        float oldPosX = sub->GetPositionX();
        float oldPosY = sub->GetPositionY();
        float newPosX = roundf(oldPosX * cosf(float(M_PI_2)) - oldPosY * sinf(float(M_PI_2)));
        float newPosY = roundf(oldPosX * sinf(float(M_PI_2)) + oldPosY * cosf(float(M_PI_2)));

        if (m_position.x + newPosX < 0.0f || m_position.x + newPosX > MAX_WIDTH - 1.0f)
            return false;

        if (m_position.y + newPosY < 0.0f)
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

    for(SubBlock* sub : m_subBlocks)
    {
        float oldPosX = sub->GetPositionX();
        float oldPosY = sub->GetPositionY();
        float newPosX = roundf(oldPosX * cos(float(M_PI_2)) - oldPosY * sin(float(M_PI_2)));
        float newPosY = roundf(oldPosX * sin(float(M_PI_2)) + oldPosY * cos(float(M_PI_2)));

        sub->SetPositionX(newPosX);
        sub->SetPositionY(newPosY);

        DEBUG_LOG("SubBlock OldPosition: (%f, %f), newPosition: (%f, %f)\n", oldPosX, oldPosY, sub->GetPositionX(), sub->GetPositionY());
    }
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
        case TYPE_Z_INV:
            positions[0] = {0.0f, 1.0f};
            positions[1] = {1.0f, 1.0f};
            positions[2] = {1.0f, 0.0f};
            positions[3] = {2.0f, 0.0f};
            break;
        case TYPE_L_INV:
            positions[0] = {0.0f, 0.0f};
            positions[1] = {0.0f, 1.0f};
            positions[2] = {1.0f, 0.0f};
            positions[3] = {2.0f, 0.0f};
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

        if (SubBlock* temp = m_game->GetSubBlockInPosition(m_position.x + sub->GetPositionX(), m_position.y + sub->GetPositionY() - 1.0f))
        {
            DEBUG_LOG("Block %d hits with block %d\n", sub->GetID(), temp->GetID());
            return false;
        }
    }

    return true;
}

bool Block::CanMoveBlock(bool right)
{
    for (SubBlock* sub : m_subBlocks)
    {
        if (right)
        {
            if (m_position.x + sub->GetPositionX() >= MAX_WIDTH - 1)
                return false;

            if (SubBlock* temp = m_game->GetSubBlockInPosition(m_position.x + sub->GetPositionX() + 1, m_position.y + sub->GetPositionY()))
            {
                DEBUG_LOG("Block %d hits with block %d\n", sub->GetID(), temp->GetID());
                return false;
            }
        }
        else
        {
            if (m_position.x + sub->GetPositionX() <= 0.0f)
                return false;

            if (SubBlock* temp = m_game->GetSubBlockInPosition(m_position.x + sub->GetPositionX() - 1, m_position.y + sub->GetPositionY()))
            {
                DEBUG_LOG("Block %d hits with block %d\n", sub->GetID(), temp->GetID());
                return false;
            }
        }
    }
    return true;
}

void Block::MoveBlock(bool right)
{
    if (!CanMoveBlock(right))
        return;

    m_position.x += right ? 1.0f : -1.0f;
}

uint8 Block::GetColorByType(uint8 type)
{
    uint8 color = COLOR_WHITE;
    switch (type)
    {
    case TYPE_PRISM:
        color = COLOR_CYAN;
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
        color = COLOR_YELLOW;
        break;
    case TYPE_Z_INV:
        color = COLOR_GREEN;
        break;
    case TYPE_L_INV:
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
