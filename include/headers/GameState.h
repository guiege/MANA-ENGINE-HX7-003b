#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#define MAX_CHARS             2


struct GameState {
	void Init();
	void ParseInputs();
	void Update(int inputs[], int disconnect_flags);

	int _num_players;
	// Character _characters[MAX_CHARS];

	int _framenumber;
};

#endif
