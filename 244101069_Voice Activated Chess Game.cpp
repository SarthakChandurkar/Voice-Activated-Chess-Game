#include "stdafx.h"
#include "digitrecognition.cpp"
#include <string>
#include "chess.h"
#include "chess.cpp"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	/*framing_ci();
	VQ();
	create_OB();
	// initialization();
	for(int i=0;i<10;i++){
		HMM_main();
		looping();
	}*/
	Board b;
	b.setInitiallyColor();
	string s;
	bool newgame = true;
	cout << "Enter any key to continue" << endl;
	cin >> s;

	while(newgame){
		b.setBoard();
		while (b.playGame());
		cout << "Do you want to play again? (y for yes, anything else for no) ";
		cin >> s;
		if (s != "y")
			newgame = false;
	}
	return 0;
}