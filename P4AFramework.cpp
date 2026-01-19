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

//hook functions here
void hook_FrameUpdate(SafetyHookContext& ctx) {
	
}

// RBX and RCX point to player getting hit
void hook_OnHit(SafetyHookContext& ctx) {
	uintptr_t gotHit = ctx.rcx;
}

// RCX has player blocking 0 = p1, 1 = p2
void hook_OnBlock(SafetyHookContext& ctx) {
	uintptr_t blockin = ctx.rcx;
}

void hook_GSChange(SafetyHookContext& ctx) {

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