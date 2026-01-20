# P4AUxSAMMI

This mod exports Persona 4 Arena Ultimax's (PC) game state to any app you wish that is capable of reading WebSockets data. In particular: SAMMI, the streaming assistant, is targeted.
Very WIP at the moment! Expect bugs, inconsistencies, and lack of data. Please get in contact (below), if you have questions/concerns!

## Installation
1. Navigate to your P4U2 install directory. You can right click on the game in Steam and then click "browse local files" under "manage" to get to the right folder.
2. Put all three .dll files in the release zip into that folder, next to P4U2.exe
3. Launch the game. If a console window appears with some text as the game launches, then it's working.
4. Connect with your client of choice (port: 31722). This is implementation specific. SAMMI instructions to come soon (tm).

## Disclaimer
This software does not enable cheating. It only exposes a read-only version of the game's state for other software. If you make a cheat with this code you suck as a human being, and I'm not liable for your crappy life or its resultant choices.

## API overview
The mod sends out 4 (at the moment) types of events based on the state of the game. They are detailed below. Each event consists of a JSON object with multiple fields. Tabbing implies a '.'

#### State Update:
* Abstract: the general state of the game and characters. Occurs every frame.
* Fields:
	* data
		* event: "p4u2_stateUpdate"
		* eventInfo
			* p1/2
				* charaID
				* posX
				* posY
				* personaCards
				* personaGauge
				* health
				* maxHealth
				* SP
				* burst
				* sprite
				* currAction
				* prevAction
				* personaSprite
				* personaCurrAction
				* personaPrevAction
			* timerFrames (match-relative timer in frames, counts up)
			* timerSeconds (match-relative timer in seconds)
			* frameCount (non-relative timer in frames)

#### Hit Event:
* Abstract: fires every time any character gets hit.
* Fields:
	* data
		* event: "p4u2_hitEvent"
		* eventInfo
			* p1/2 (same subfields as state update)
			* whoGotHit (0 = p1, 1 = p2, 2+ = other)
			* baseDamage
			* comboDamage
			* attackLevel
			* hitstun
			* hitCount
			* actionCount
			* personaCards (of the defender)
			* attackName
			* frameCount

#### Guard Event:
* Abstract: fires every time any character blocks.
* Fields:
	* data
		* event: "p4u2_blockEvent"
		* eventInfo
			* p1/2 (same subfields as state update)
			* whoBlocking (operates same as whoGotHit on hit event)
			* attackLevel
			* blockstun
			* personaCards (of the defender)
			* attackName
			* frameCount

#### Timeout
* Abstract: fires whenever the game changes states (i.e. main menu to CSS to versus).
* Fields:
	* data
		* event: "p4u2_Timeout"
		* No other fields

## Acknowledgments
Thanks must be given to the following amazing people who've helped me thus far:
* The Persona Fight Club Discord server
* Milpy
* GrimFlash
* lj_emerald / NightWolf3348

## Contact:
Please only contact me if you have issues or major feature requests. Do not ask me how to set up stuff in SAMMI, other documentation covers that.
To reach me:
* quattrodan3 on Discord
* nleff72 on Twitter/BlueSky
* email: dan (at) lakeviewcomputerspecialists (dot) com