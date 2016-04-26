#include "Game.h"
#include <iterator>

Game::Game()
{
    m_level             = 0;
    m_points            = 0;
    m_currentBlockId    = 0;
    m_nextMoveTime      = 0;
    m_pausedTime        = 0;
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
        exit(EXIT_FAILURE);
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
    GenerateBlock(true);
    GenerateBlock(false);
}

void Game::Update()
{
    if (int64(m_nextMoveTime - clock()) <= 0)
    {
        //DebugBlockPositions();
        m_nextMoveTime = GetNextMoveTime();
        HandleDropBlock();
    }
}

void Game::PauseGame()
{
    //m_pausedTime = clock();
}

void Game::ResumeGame()
{
    m_nextMoveTime = GetNextMoveTime();
}

Block* Game::GenerateBlock(bool active, int32 type /*=-1*/)
{
    if (type < 0)
    {
        // Prevent generate the same block twice in a row
        do
        {
            type = rand() % (MAX_BLOCK_TYPE) + 1;
        }
        while (type == m_lastBlockType);
    }

    float pos[2][2] = { { CENTER, MAX_HEIGHT}, { NEXT_BLOCK_X, NEXT_BLOCK_Y} }; 

    Block* block = new Block(type, this, pos[!active][0], pos[!active][1]);
    if (!block)
    {
        DEBUG_LOG("Failed to create block. Stopping...\n");
        exit(EXIT_FAILURE);
    }

    m_lastBlockType = block->GetType();
    if (!active)
    {
        if (m_activeBlock)
        {
            m_activeBlock->SetPositionX(CENTER);
            m_activeBlock->SetPositionY(MAX_HEIGHT);
        }
        m_nextBlock = block;
    }
    else
        m_activeBlock = block;

    DEBUG_LOG("Block type: %d succesfully created.\n", type);
    return block;
}

void Game::DestroyActiveBlock(bool withSave /*=true*/)
{
    if (!m_activeBlock)
    {
        DEBUG_LOG("Failed at destroy active block.\n");
        return;
    }

    if (withSave)
    {
        for (SubBlock* sub : m_activeBlock->GetSubBlocks())
        {
            sub->SetPositionX(sub->GetPositionX() + m_activeBlock->GetPositionX());
            sub->SetPositionY(sub->GetPositionY() + m_activeBlock->GetPositionY());
            m_gameBlocks.push_back(sub);
            sub->DebugPosition();
        }

        m_activeBlock = m_nextBlock;
        GenerateBlock(false);
    }
    else
        GenerateBlock(true);
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
        CheckLineCompleted();
    }
    else
    {
        DestroyActiveBlock();
        CheckLineCompleted();
        CheckGameLost();
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
    DestroyActiveBlock();
    CheckLineCompleted();
    CheckGameLost();
}

void Game::CheckLineCompleted()
{
    std::vector<float> linesCompleted;
    for (float y = 0.0f; y < MAX_HEIGHT; y += 1.0f)
    {
        uint8 i = 0;
        for (float x = 0.0f; x < MAX_WIDTH; x += 1.0f)
        {
            if (SubBlock* sub = GetSubBlockInPosition(x, y))
            {
                DEBUG_LOG("CheckLine: Cube %u in Position X:%f, Y: %f\n", sub->GetID(), x, y);
            }
            else
            {
                DEBUG_LOG("CheckLine: Empty Position X:%f, Y: %f\n", x, y);
                break;
            }

            i++;
        }
        DEBUG_LOG("CheckLine: Coincidences: %u\n", i);

        // Insert line completed if all X positions are filled with subblocks
        if (i >= MAX_WIDTH)
            linesCompleted.push_back(y);
    }

    if (linesCompleted.empty())
        return;

    DEBUG_LOG("Lines completed: ");
    for (float i : linesCompleted)
    {
        for (float x = 0.0f; x < MAX_WIDTH; x += 1.0f)
        {
            SubBlock* sub = GetSubBlockInPosition(x, i);
            if (!sub)
            {
                DEBUG_LOG("Incorrect line completed.\n");
                exit(EXIT_FAILURE);
            }

            sub->Delete();
        }
        DEBUG_LOG("%u", i);
    }
    DEBUG_LOG("\n");

    std::sort(m_gameBlocks.begin(), m_gameBlocks.end());

    for (SubBlock* &sub : m_gameBlocks)
    {
        sub->DebugPosition();
        for (float i = 0.0f; i < linesCompleted.size(); i++)
            if (sub->CanDropSubBlock())
                sub->SetPositionY(sub->GetPositionY() - 1.0f);
    }
}

void Game::CheckGameLost()
{
    if (!m_activeBlock)
    {
        DEBUG_LOG("Active block not found. Stopping...");
        exit(EXIT_FAILURE);
    }

    if (!m_activeBlock->CanDropBlock())
        EndGame();
}

void Game::EndGame()
{
}

SubBlock* Game::GetSubBlockInPosition(float x, float y)
{
    SubBlock* sub = nullptr;

    std::vector<SubBlock*> subBlocks = GetSubBlockList();
    for (auto itr = subBlocks.begin(); itr != subBlocks.end(); itr++)
    {
        SubBlock* temp = *(itr);
        if (temp->GetPositionX() == x && temp->GetPositionY() == y)
        {
            sub = temp;
            break;
        }
    }

    return sub;
}

void Game::ChangeBlock()
{
    DestroyActiveBlock(false);
}

void Game::DebugBlockPositions()
{
    if (m_activeBlock)
        m_activeBlock->DebugPosition();

    for (SubBlock* sub : m_gameBlocks)
    {
        sub->DebugPosition();
    }
}

void Game::DeleteSubBlock(SubBlock* subBlock)
{
    m_gameBlocks.erase(std::find(m_gameBlocks.begin(), m_gameBlocks.end(), subBlock));
}

void Game::IncreaseBlockSpeed()
{
}
