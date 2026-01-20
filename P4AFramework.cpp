#define WIN32_LEAN_AND_MEAN
#define NOMCX
#define NOIME
#include "P4A.h"
#include "P4AFramework.hpp"
#include "safetyhook.hpp"
#include "SAMMITypes.hpp"
#include "WebSockets.hpp"
#include <iostream>
#include <windows.h>

using json = nlohmann::json;

// globals here
SafetyHookMid FrameUpdate;
SafetyHookMid OnHit;
SafetyHookMid OnBlock;
SafetyHookMid GSChange;
unsigned long long frameCounter = 0;
worker_t worker{};
PlayerVars* players = nullptr;
GameVars* game = nullptr;
MatchVars* match = nullptr;
BurstVars* bursts = nullptr;

void initalizeWSServer() {
	worker.thread = std::make_shared<std::thread>([]() {
		/* create uWebSocket worker and capture uWS::Loop, uWS::App objects. */
		worker.work();
		});
	std::cout << "P4AU2-Sammi: WS: server started" << std::endl;
}

void sendEvent(std::string eventName, std::string customData) {
	std::string body = std::format("{{\"event\": \"{}\", \"eventInfo\": {}}}", eventName, customData);
	//send to all opened clients
	//std::this_thread::sleep_for(std::chrono::milliseconds(334)); // roughly 20 frames
	worker.loop->defer([body]() {
		worker.app->publish("broadcast", body, uWS::TEXT);
		});
}

void fillInPlayerStruct(Player* p1, Player* p2) {
	p1->burst = bursts->p1;
	p1->charaID = players->p1->charaID;
	p1->currAction = players->p1->currActGet();
	p1->health = players->p1->health;
	p1->maxHealth = players->p1->maxHealth;
	p1->personaCards = players->p1->personaGauge / 250000;
	p1->personaGauge = players->p1->personaGauge;
	p1->personaCurrAction = players->p1->persona->currActGet();
	p1->personaPrevAction = players->p1->persona->prevActGet();
	p1->personaSprite = players->p1->persona->spriteGet();
	p1->posX = players->p1->posX;
	p1->posY = players->p1->posY;
	p1->prevAction = players->p1->prevActGet();
	p1->SP = players->p1->meter;
	p1->sprite = players->p1->spriteGet();

	p2->burst = bursts->p2;
	p2->charaID = players->p2->charaID;
	p2->currAction = players->p2->currActGet();
	p2->health = players->p2->health;
	p2->maxHealth = players->p2->maxHealth;
	p2->personaCards = players->p2->personaGauge / 250000;
	p2->personaGauge = players->p2->personaGauge;
	p2->personaCurrAction = players->p2->persona->currActGet();
	p2->personaPrevAction = players->p2->persona->prevActGet();
	p2->personaSprite = players->p2->persona->spriteGet();
	p2->posX = players->p2->posX;
	p2->posY = players->p2->posY;
	p2->prevAction = players->p2->prevActGet();
	p2->SP = players->p2->meter;
	p2->sprite = players->p2->spriteGet();
}

//hook functions here
void hook_FrameUpdate(SafetyHookContext& ctx) {
	FrameUpdateEvent fu{};
	fu.frameCount = frameCounter; // potentially a "for now", since I don't have round start/end events rn
	frameCounter += 1;
	fu.timerFrames = match->matchTimerFrames;
	fu.timerSeconds = match->matchTimerSeconds;
	fillInPlayerStruct(&fu.p1, &fu.p2);
	json j = fu;
	std::thread(sendEvent, "p4u2_stateUpdate", j.dump()).detach();
}

// RBX and RCX point to player getting hit
void hook_OnHit(SafetyHookContext& ctx) {
	uintptr_t gotHit = ctx.rcx;
	HitEvent he{};
	he.frameCount = match->frameCount;
	fillInPlayerStruct(&he.p1, &he.p2);
	if (gotHit == (uintptr_t)players->p1) {
		he.actionCount = players->p2->incomingComboActsCount;
		if (players->p1->attackLevel == 0) {
			he.attackLevel = players->p1->persona->attackLevel;
		}
		else {
			he.attackLevel = players->p1->attackLevel;
		}
		he.attackName = players->p2->incomAtkGet();
		he.baseDamage = players->p2->baseDmgTaken;
		he.comboDamage = players->p2->incomingComboDmg;
		he.hitCount = players->p2->incomingComboHitsCount;
		he.hitstun = players->p2->hitstun;
		he.personaCards = players->p2->personaGauge / 250000;
		he.whoGotHit = 0;
	}
	else if (gotHit == (uintptr_t)players->p2) {
		he.actionCount = players->p1->incomingComboActsCount;
		if (players->p2->attackLevel == 0) {
			he.attackLevel = players->p2->persona->attackLevel;
		}
		else {
			he.attackLevel = players->p2->attackLevel;
		}
		he.attackName = players->p1->incomAtkGet();
		he.baseDamage = players->p1->baseDmgTaken;
		he.comboDamage = players->p1->incomingComboDmg;
		he.hitCount = players->p1->incomingComboHitsCount;
		he.hitstun = players->p1->hitstun;
		he.personaCards = players->p1->personaGauge / 250000;
		he.whoGotHit = 0;
	}
	else {
		he.whoGotHit = 3;
		he.attackName = "Unknown";
	}
	json j = he;
	std::thread(sendEvent, "p4u2_hitEvent", j.dump()).detach();
}

// RCX has player blocking 0 = p1, 1 = p2
void hook_OnBlock(SafetyHookContext& ctx) {
	uintptr_t blockin = ctx.rcx;
	BlockEvent be{};
	be.frameCount = match->frameCount;
	be.whoBlocking = (int)blockin;
	fillInPlayerStruct(&be.p1, &be.p2);
	if (blockin == 0) {
		if (players->p2->attackLevel == 0) {
			be.attackLevel = players->p2->persona->attackLevel;
		}
		else {
			be.attackLevel = players->p2->attackLevel;
		}
		be.attackName = players->p1->incomAtkGet();
		be.blockstun = players->p1->blockstun;
		be.personaCards = players->p1->personaGauge / 250000;
	}
	else if (blockin == 1) {
		if (players->p1->attackLevel == 0) {
			be.attackLevel = players->p1->persona->attackLevel;
		}
		else {
			be.attackLevel = players->p1->attackLevel;
		}
		be.attackName = players->p2->incomAtkGet();
		be.blockstun = players->p2->blockstun;
		be.personaCards = players->p2->personaGauge / 250000;
	}
	json j = be;
	std::thread(sendEvent, "p4u2_blockEvent", j.dump()).detach();
}

void hook_GSChange(SafetyHookContext& ctx) {
	std::thread(sendEvent, "p4u2_Timeout", "").detach();
	frameCounter = 0;
}

void initHooks() {
	initalizeWSServer();
	uintptr_t base = (uintptr_t)GetModuleHandle(NULL);
	//printf("base: 0x%llX. Offs: 0x%llX\n", base, base+0x7AD1F8);
	FrameUpdate = safetyhook::create_mid(base + 0x146B43, hook_FrameUpdate);
	OnHit = safetyhook::create_mid(base + 0x19F166, hook_OnHit);
	OnBlock = safetyhook::create_mid(base + 0x1760E8CB, hook_OnBlock);
	GSChange = safetyhook::create_mid(base + 0x10BD16, hook_GSChange);
	
	players = (PlayerVars*)(base + 0x7AD1F0);
	game = (GameVars*)(base + 0xD0DDF0);
	match = (MatchVars*)(base + 0xE88EF8);
	bursts = (BurstVars*)(base + 0xE9A410);

	//std::cout << "P1: " << P1 << ", ptr: " << (void*)P1->ptr << ", P2: " << P2 << ", ptr: " << (void*)P2->ptr << std::endl;
}