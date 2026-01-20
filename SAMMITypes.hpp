#pragma once
#include "json.hpp"
#include <string>


struct Player {
	int charaID{};
	int posX{};
	int posY{};
	int personaCards{}; //divide personaGauge by 250K
	int personaGauge{}; //the full gauge, for pbreak timer
	int health{};
	int maxHealth{};
	int SP{};
	int burst{};
	std::string sprite{};
	std::string currAction{};
	std::string prevAction{};
	std::string personaSprite{};
	std::string personaCurrAction{};
	std::string personaPrevAction{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player, charaID, posX, posY, personaCards, personaGauge, health, maxHealth, SP, burst, sprite, currAction, prevAction, personaSprite, personaCurrAction, personaPrevAction)

struct FrameUpdateEvent {
	int timerFrames{};
	int timerSeconds{};
	Player p1{};
	Player p2{};
	unsigned long long frameCount{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FrameUpdateEvent, timerFrames, timerSeconds, p1, p2, frameCount)

struct HitEvent {
	int whoGotHit{}; // 0 = p1, 1 = p2, 2+ = other (enum later)
	int baseDamage{};
	int comboDamage{};
	int attackLevel{};
	int hitstun{};
	int hitCount{};
	int actionCount{};
	int personaCards{}; // for alignment and it's useful ig, cards of the defender
	Player p1{};
	Player p2{};
	std::string attackName{};
	unsigned long long frameCount{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HitEvent, whoGotHit, baseDamage, comboDamage, attackLevel, hitstun, hitCount, actionCount, personaCards, p1, p2, attackName, frameCount)

struct BlockEvent {
	int whoBlocking{}; // same as HitEvent::whoGotHit
	int attackLevel{};
	int blockstun{};
	int personaCards{}; // of defender
	Player p1{};
	Player p2{};
	std::string attackName{}; //unknown if set on block
	unsigned long long frameCount{};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BlockEvent, whoBlocking, attackLevel, blockstun, personaCards, p1, p2, attackName, frameCount)
