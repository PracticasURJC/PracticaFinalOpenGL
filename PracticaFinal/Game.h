#ifndef GAME_H
#define GAME_H

#include "Common.h"
#include "Block.h"


#define DEFAULT_LEVEL 1
#define DEFAULT_MILLISECONDS 1000

class Game
{
public:
    Game();
    ~Game();

    static Game* CreateNewGame(uint32 level = DEFAULT_LEVEL);

    void StartGame();
    void Update();
    void EndGame();

    void GenerateBlock(int32 type = -1);

    void DestroyActiveBlock(bool withSave = true);

    uint64 GetNextMoveTime();

    void RotateActiveBlock();

    void MoveBlock(bool right);
    void DropBlock();
    void HandleDropBlock();
    void ChangeBlock();

    void CheckLineCompleted();
    void CheckGameLost();

    SubBlock* GetSubBlockInPosition(float x, float y);
    
    std::vector<SubBlock*> GetSubBlockList() const { return m_gameBlocks; }

    uint32 GetPoints() const { return m_points; }
    void SetPoints(uint32 _points) { m_points = _points; }

    uint32 GetLevel() const { return m_level; }
    void SetLevel(uint32 _level) { m_level = _level; }

    uint32 GetCurrentBlockID() { return m_currentBlockId; }
    void SetCurrentBlockID(uint32 _currentBlockId) { m_currentBlockId = _currentBlockId; }
    void IncreaseCurrentBlockID() { m_currentBlockId++; }

    Block* GetActiveBlock() { return m_activeBlock; }
    const Block* GetActiveBlock() const { return m_activeBlock; }

private:
    Block* m_activeBlock;
    std::vector<SubBlock*> m_gameBlocks;
    uint32 m_points;
    uint32 m_level;
    uint32 m_currentBlockId;

    uint64 m_nextMoveTime;

    uint8 m_lastBlockType;
};

#endif
