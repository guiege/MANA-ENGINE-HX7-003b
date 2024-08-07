#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_


struct GameState {
	void Init();
	void ParseInputs();
	void Update(int inputs[], int disconnect_flags);
};

#endif
