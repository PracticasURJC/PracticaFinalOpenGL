#include "Game.h"
#include <iterator>

Game::Game()
{
    m_level           = 0;
    m_points          = 0;
    m_currentBlockId  = 0;
    m_nextMoveTime    = 0;
    m_gameBlocks.clear();
}

Game::~Game()
{
    m_gameBlocks.clear();
}

Game* Game::CreateNewGame(uint32 level /*=DEFAULT_LEVEL*/)
{
    Game* newGame = new Game();
    if (!newGame)
    {
        DEBUG_LOG("Failed to create new game. Stopping...\n");
        exit(1);
    }

    DEBUG_LOG("Game succesfully created.\n");

    newGame->m_level = level;
    newGame->m_points = 0;
    newGame->m_gameBlocks.clear();
    newGame->m_nextMoveTime = newGame->GetNextMoveTime();

    return newGame;
}

void Game::StartGame()
{
    GenerateBlock();
}

void Game::Update()
{
    if ((m_nextMoveTime - clock()) <= 0)
    {
        m_nextMoveTime = GetNextMoveTime();
        HandleDropBlock();
    }

    CheckLineCompleted();
}

void Game::GenerateBlock(int32 type /*=-1*/)
{
    if (type < 0)
    {
        do
        {
            type = rand() % (MAX_BLOCK_TYPE) + 1;
        }
        while (type == m_lastBlockType);
    }

    Block* block = new Block(type, this, CENTER, MAX_HEIGHT);
    if (!block)
    {
        DEBUG_LOG("Failed to create block. Stopping...\n");
        exit(1);
    }

    m_activeBlock = block;
    m_lastBlockType = block->GetType();

    DEBUG_LOG("Block type: %d succesfully created.\n", type);
}

void Game::DestroyActiveBlock()
{
    if (!m_activeBlock)
    {
        DEBUG_LOG("Failed at destroy active block.\n");
        return;
    }

    for (SubBlock* sub : m_activeBlock->GetSubBlocks())
    {
        sub->SetPositionX(sub->GetPositionX() + m_activeBlock->GetPositionX());
        sub->SetPositionY(sub->GetPositionY() + m_activeBlock->GetPositionY());
        m_gameBlocks.push_back(sub);
    }

    delete m_activeBlock;
    m_activeBlock = nullptr;
}

void Game::HandleDropBlock()
{
    Block* block = GetActiveBlock();
    if (!block)
        return;

    if (block->CanDropBlock())
    {
        float posY = std::max(block->GetPositionY() - 1.0f, 0.0f);
        block->SetPositionY(posY);
    }
    else
    {
        DestroyActiveBlock();
        GenerateBlock();
    }
}

void Game::RotateActiveBlock()
{
    if (!m_activeBlock)
        return;

    m_activeBlock->RotateBlock();
}

uint64 Game::GetNextMoveTime()
{
    return uint64(clock() + float(DEFAULT_MILLISECONDS) * float(1.0f / m_level));
}

void Game::MoveBlock(bool right)
{
    if (!m_activeBlock)
        return;

    m_activeBlock->MoveBlock(right);
}

void Game::DropBlock()
{
    if (!m_activeBlock)
        return;

    m_activeBlock->Drop();
}

void Game::CheckLineCompleted()
{
    std::vector<uint8> linesCompleted;
    for (uint8 y = 0; y < MAX_HEIGHT; y++)
    {
        uint8 i = 0;
        for (uint8 x = 0; x < MAX_WIDTH; x++)
        {
            if (!GetSubBlockInPosition(x, y))
                break;

            i++;
        }

        // Insert line completed if all X positions are filled with subblocks
        if (i == MAX_WIDTH)
            linesCompleted.push_back(y);
    }

    if (linesCompleted.empty())
        return;

    for (uint8 i : linesCompleted)
    {
        for (uint8 x = 0; x < MAX_WIDTH; x++)
        {
            SubBlock* sub = GetSubBlockInPosition(x, i);
            if (!sub)
            {
                DEBUG_LOG("Incorrect line completed.\n");
                return;
            }

            sub->Delete();
        }
    }
}

void Game::CheckGameLost()
{
}

void Game::EndGame()
{
}

SubBlock* Game::GetSubBlockInPosition(float x, float y)
{
    SubBlock* sub = nullptr;

    std::vector<SubBlock*> subBlocks = GetSubBlockList();
    for (SubBlock* temp : subBlocks)
        if (temp->GetPositionX() == x && temp->GetPositionY() + temp->GetHeight() == y)
        {
            sub = temp;
            break;
        }

    return sub;
}
