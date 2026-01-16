#include "pch.h"
#include "safetyhook.hpp"
#include "P4A.h"
#include "WebSockets.hpp"
#include "SAMMITypes.hpp"
#include "P4AFramework.hpp"
#include <iostream>

using json = nlohmann::json;

// globals here
HMODULE base;
SafetyHookMid FrameUpdate;
SafetyHookMid OnHit;
SafetyHookMid OnBlock;
Intermediate* P1;
Intermediate* P2;
MatchState* matchState;
GameState* gameState;
GameMode* gameMode;
int* internalFrameCounter;
unsigned long long frameCounter = 0;
worker_t worker{};

static void initalizeWSServer() {
	worker.thread = std::make_shared<std::thread>([]() {
		/* create uWebSocket worker and capture uWS::Loop, uWS::App objects. */
		worker.work();
		});
	std::cout << "P4AU2-Sammi: WS: server started" << std::endl;
}

static void sendEvent(std::string eventName, std::string customData) {
	std::string body = std::format("{{\"event\": \"{}\", \"eventInfo\": {}}}", eventName, customData);
	//send to all opened clients
	//std::this_thread::sleep_for(std::chrono::milliseconds(334)); // roughly 20 frames
	worker.loop->defer([body]() {
		worker.app->publish("broadcast", body, uWS::TEXT);
		});
}

//hook functions here
static void hook_FrameUpdate(SafetyHookContext& ctx) {
	//std::cout << "Base: " << base << ", P1: " << P1 << ", ptr: " << (void*)P1->ptr << ", P2: " << P2 << ", ptr: " << (void*)P2->ptr << std::endl;
	//std::cout << "Frame update! " << P1->ptr->currActGet() << ", " << P2->ptr->currActGet() << std::endl;

}

// RBX and RCX point to player getting hit
static void hook_OnHit(SafetyHookContext& ctx) {
	uintptr_t gotHit = ctx.rcx;
	printf("0x%llX\n", gotHit);
	if (gotHit == reinterpret_cast<uintptr_t>(P1->ptr)) {
		// then P1 got hit
		std::cout << "P1 got hit! " << P1->ptr->health << ", " << P2->ptr->attackLevel << std::endl;
	}
	else if (gotHit == reinterpret_cast<uintptr_t>(P2->ptr)) {
		// then P2 got hit
		std::cout << "P2 got hit! " << P2->ptr->health << ", " << P1->ptr->attackLevel << std::endl;
	}
	else {
		// something else got hit
		std::cout << "Something else got hit, it wasn't a player" << std::endl;
	}
}

// RCX has player blocking 0 = p1, 1 = p2
static void hook_OnBlock(SafetyHookContext& ctx) {
	uintptr_t blockin = ctx.rcx;
	if (blockin == 0) {
		// then P1 is blocking
		std::cout << "P1 is a blockin'! " << P1->ptr->blockstun << ", " << P2->ptr->attackLevel << std::endl;
	}
	else if (blockin == 1) {
		// then P2 is blocking
		std::cout << "P2 is a blockin'! " << P2->ptr->blockstun << ", " << P1->ptr->attackLevel << std::endl;
	}
	else {
		// something else is blocking
		std::cout << "Something else is blocking, it wasn't a player" << std::endl;
	}
}

void initHooks() {
	initalizeWSServer();
	base = GetModuleHandle(NULL);
	FrameUpdate = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x146B43, hook_FrameUpdate);
	OnHit = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x19F160, hook_OnHit);
	OnBlock = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1760E8C5, hook_OnBlock);
	
	// really, really, really hope these don't change, else I'm **HOSED**
	P1 = reinterpret_cast<Intermediate*>(0x1407AD1F8); //reinterpret_cast<Intermediate*>(base) + 0x7AD1F8;
	P2 = reinterpret_cast<Intermediate*>(0x1407AD200); //reinterpret_cast<Intermediate*>(base) + 0x7AD200;
	gameMode = reinterpret_cast<GameMode*>(base) + 0xD0DE80;
	gameState = reinterpret_cast<GameState*>(base) + 0xD0DE84;
	matchState = reinterpret_cast<MatchState*>(base) + 0xE88F74;
	internalFrameCounter = reinterpret_cast<int*>(base) + 0xE89FB8;

	std::cout << "Base: " << base << ", P1: " << P1 << ", ptr: " << (void*)P1->ptr << ", P2: " << P2 << ", ptr: " << (void*)P2->ptr << std::endl;
}