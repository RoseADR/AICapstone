#include "iostream"
#include "irrKlang.h"

using namespace irrklang;

class MyIrrklang {
public:
	void SoundPlayer() {
		ISoundEngine* engine = createIrrKlangDevice();
		if (!engine) {
			return;
		}

		char i = 0;

		while (i != 'q')
		{
			std::cout << "Press any key to play some sound, press 'q' to quit.\n";
		engine->play2D("C:/Users/devon/Music/GameJam (Clockwork)/Alert.wav");
		std::cin >> i; // wait for user to press some key
		}


		engine->drop();
		return;

	}


	
};
