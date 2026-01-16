#pragma once
#include "struct_util.h"

struct CharData {
    FIELD(0x38, int, charaID);
    FIELD(0x3B8, int, posX);
    FIELD(0x3BC, int, posY);
    FIELD(0x60C, int, attackLevel); //not set for some persona attacks (Ex Chie 5C, 5D) bc it's the persona object attacking
    FIELD(0xBC4, int, health);
    FIELD(0xBC8, int, maxHealth);
    ARRAY_FIELD(0x20C0, char[32], prevAction);
    const char* prevActGet() const;
    ARRAY_FIELD(0x20E0, char[32], currAction);
    const char* currActGet() const;
    FIELD(0x231C, int, blockstun); //frames, set on blocker
    FIELD(0x48E0, int, hitstun); //frames, set on player getting hit
    FIELD(0x4F88, int, meter);
    FIELD(0x5278, int, personaGauge); //offset is wrong. 10k per onscreen card, also used for persona break cooldown timer
};

struct Intermediate {
    CharData* ptr;
};

enum MatchState
{
    MatchState_NotStarted = 0,
    MatchState_RoundCallSign = 2,
    MatchState_Fight = 3,
    MatchState_FinishSign = 4,
    MatchState_RoundUpdateSign = 5,
    MatchState_VictoryScreen = 7
};

enum GameState
{
    GameState_ArcsysLogo = 2,
    GameState_IntroVideoPlaying = 3,
    GameState_TitleScreen = 4,
    GameState_StoryMenu = 6,
    GameState_CharacterSelectionScreen = 7,
    GameState_ArcadeSelectScreen = 8,
    GameState_ScoreAttackModeSelectScreen = 8,
    GameState_GoldenArenaSelectScreen = 8,
    GameState_VersusScreen = 11,
    GameState_InMatch = 12,
    GameState_VictoryScreen = 13,
    GameState_MainMenu = 25,
    GameState_SimpleResults = 28
};

enum GameMode
{
    GameMode_Arcade = 0,
    GameMode_ScoreAttack = 1,
    GameMode_Versus = 3,
    GameMode_Story = 4,
    // GameMode_Gallery = 6,
    GameMode_Theater = 7,
    // GameMode_TitleScreen = 8,
    GameMode_Training = 9,
    GameMode_Online = 11,
    // GameMode_GoldenArena = 12,
    // GameMode_MainMenuScreen = 13,
    GameMode_Challenge = 15,
    GameMode_Lesson = 16,
};