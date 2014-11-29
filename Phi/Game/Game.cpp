#include <PreCompile.h>
#include <Game/GameEntity.h>

namespace Game
{
    Game::Game() : mCurrentGamePart(NULL)
    {
        
    }
    
    void Game::addPart(int id, GamePart *part)
    {
        mGameParts[id] = part;
    }
    
    Game::State::List Game::update(Update &update)
    {
        if (mCurrentGamePart)
        {
            int id = mCurrentGamePart->update(update);
            if (id != 0)
            {
                GameParts::iterator it = mGameParts.find(id);
                mCurrentGamePart = NULL;
                if (it != mGameParts.end())
                    mCurrentGamePart = it->second;
                
            }
        }
        
        return mCurrentGamePart ? Game::State::Continue : Game::State::Exit;
    }
    
    void    Game::destroy(Destroy &destroy)
    {
        for (auto& part : mGameParts)
        {
            part.second->destroy(destroy);
            delete part.second;
        }
        mGameParts.clear();
        mCurrentGamePart = NULL;
    }
}