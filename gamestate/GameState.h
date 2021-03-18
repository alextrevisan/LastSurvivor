enum class GameStates{
    MainMenu,
    Level01
};

struct GameState
{
    inline GameStates GetCurrentGameState() const
    {   
        return CurrentGameState;
    }

    void SetGameState(GameStates gameState)
    {
            CurrentGameState = gameState;
    }
private:
    GameStates CurrentGameState = GameStates::Level01;
};
