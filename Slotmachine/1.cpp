#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h>  
#include <conio.h> 
#include <windows.h> 
#include <time.h> 
#include <string> 
#include <iostream> 
using namespace std;
//by wheel i refer to column
/////////////////
string jackpot[3]; //this will check for jackpots
string betmoneystring; //used to display betmeny, its here because spinwhellfunction is called often
string wheel[3][6]; //this will store symbols for wheels    //ask for parameter passing, especially for array passing (2d array cant, 1d when by ref - (&x)[]) do i really need it ?

int cheked[3] = { 1,1,1 }; //when stopping wheel changes to 0 , 0 means that that wheel was cheked
char key; //used for input detection

int stopwheel = 0; // couts which wheel to stop now and which to spin
int wheelspeed = 30; //speed at which wheel spins
int how_long_wheel_loop_outsideloop; //no of wheels int in loop, get variable to outside of loop

int bankrupt = 0; // 0= have money ,1=bancrupt
int endgame = 0; //1= plater want to quit game ,0= he want to play
int gamerunning = 1; // state of game, 0=done, 1=run 

int jackpotmoney; //how much person won
int startingmoney; //starting money , different to allow showing how much you lost
int money;
int betmoney = 10;//bet money

#define MAX_X        100// maximum x distance(used only in getinfo part)

HANDLE hconsole;         // handle to console 
CONSOLE_SCREEN_BUFFER_INFO con_info;   // holds screen info 
////////////////
void Draw_String(int x, int y, string s) //spawns text in defined position
{
	COORD cursor_pos;
	cursor_pos.X = x;
	cursor_pos.Y = y;
	SetConsoleCursorPosition(hconsole, cursor_pos);
	cout << s;
}
void getinfo(int& money, int& startingmoney)//welcome and initial money add validation
{
	int  player_x = 40;// fun restricted choice
	cout << "Welcome to slot machine, remember gambling is bad." << endl;
	cout << "Select your starting money, press S to increase and A to decrease, then press space bar to confirm" << endl;
	cout << "" << endl;
	cout << "|   10    |   20    |   40    |   80    |   100   |   150   |   200   |   400   |   800   |   1000  |" << endl;

	while (key != '32')
	{
		if (_kbhit())
		{
			key = toupper(_getch());

			if (key == 'A')
			{
				player_x-=2;
			}
			if (key == 'S')
			{
				player_x+=2;
			}
			if (key == 32)// restricid choice, perhaps better way to make this rather than row of ifs ?(money increment in not regular amounts to use loop for it change money)
			{
				if (player_x > 0 && player_x < 10)//10
				{
					startingmoney = 10;
					goto skipvalidation;
				}
				else if (player_x > 11 && player_x < 20)//20
				{
					startingmoney = 20;
					goto skipvalidation;
				}
				else if (player_x > 21 && player_x < 30)//40
				{
					startingmoney = 40;
					goto skipvalidation;
				}
				else if (player_x > 31 && player_x < 40)//80
				{
					startingmoney = 80;
					goto skipvalidation;
				}
				else if (player_x > 41 && player_x < 50)//100
				{
					startingmoney = 100;
					goto skipvalidation;
				}
				else if (player_x > 51 && player_x < 60)//150
				{
					startingmoney = 150;
					goto skipvalidation;
				}
				else if (player_x > 61 && player_x < 70)//200
				{
					startingmoney = 200;
					goto skipvalidation;
				}
				else if (player_x > 71 && player_x < 80)//400
				{
					startingmoney = 400;
					goto skipvalidation;
				}
				else if (player_x > 81 && player_x < 90)//800
				{
					startingmoney = 800;
					goto skipvalidation;
				}
				else if (player_x > 90 && player_x < 100)//1000 //yes 90 not 91
				{
					startingmoney = 1000;
					goto skipvalidation;
				}
				else
				{
					Draw_String(0, 6, "Please select a number not a line.");
				}
			}
		}
		if (++player_x > MAX_X)
		{
			player_x = MAX_X;
		}
		if (--player_x < 0)
		{
			player_x = 0;
		}
		Draw_String(0, 5, "                                                                                                               ");// im not using scroll to get rid of old drawings, just remove entire line
		Draw_String(player_x, 5, "^");
		Sleep(200);
	}
skipvalidation:
	money = startingmoney;
	system("cls");
}
void startgraphics()
{
	COORD console_size = { 30, 30 }; // size of console 
	srand((unsigned)time(NULL));
	hconsole = CreateFile(TEXT("CONOUT$"), GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0L, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0L);
	SetConsoleScreenBufferSize(hconsole, console_size);
	GetConsoleScreenBufferInfo(hconsole, &con_info);
}
void displaycosmetics(int money)
{
	string moneystring =to_string(money); // convert to string so can be displayed

	for (int a = 9; a < 14; a++)//cosmetics and instructions
	{
		Draw_String(1, a, "|");
		Draw_String(5, a, "|");
	}
	Draw_String(1, 11, ">");
	Draw_String(6, 11, "<");
	Draw_String(0, 0, "Your balance is: ");
	Draw_String(17, 0, moneystring );
	Draw_String(0, 1, "Press X or esc to end game");
	Draw_String(0, 2, "Press A to start slow spin");
	Draw_String(0, 3, "Press space bar to stop first wheel");
	Draw_String(0, 4, "Spin for 10"); //10 will be starting value
	Draw_String(14, 4, "(press Q to increase or W to decrease stakes");
	Draw_String(0, 5, "Press E to start spinning again (only when all wheels have been stoped)");
	Draw_String(0, 7, "-------------------------------------------------------------------------");
	for (int b = 0; b < 7; b++)
	{
		Draw_String(72, b, "|");
	}
}
void createwheels()
{
	const char symbolstock[6] = {'£','$','&','%','!','?' }; //program will choose random symbols form this array
	srand(time(NULL)); //initialize the random seed
	for (int wheelrow = 0; wheelrow < 3; wheelrow++)// loop around each wheel
	{
		for (int symbolcollumn = 0; symbolcollumn < 7; symbolcollumn++) //loops around each symbol in column
		{
			int randomindex = rand() % 6; //create randum index for stocksymbol array
			wheel[wheelrow][symbolcollumn] = symbolstock[randomindex];
		}
	}
}
void spinagain(char &key, int &stopwheel, int &gamerunning, int &wheelspeed, int &endgame)
{
	Draw_String(0, 2, "Press A to start slow spin   ");//reset message to display correct as speed resets
	while (key != 'E')//wait for player to restart or exit
	{	
		 if (_kbhit())
		 {
     	  key = toupper(_getch());
		  if (key == 'E')
		  {
			  stopwheel = 0;
			  gamerunning = 1;
			  wheelspeed = 30;
			  cheked[0] = 1;
			  cheked[1] = 1;
			  cheked[2] = 1;//reset variables
			  Draw_String(0, 3, "Press space bar to stop first wheel");
     	  }
		 }
		 if (key == 'X' || key == 27) //x or esc
		 {
			 gamerunning = 0;
			 endgame = 1;
			 goto skip;//get out of invinite loop
		 } 
		 if (bankrupt)
		 {
			 goto skip;//get out of invinite loop
		 }
	}
skip: 
	while (0)// c++ dontlike when this points at }, so i added something to exist here
	{ }
}
void jackpot_questionmark (int &money)
{
	if ((jackpot[0] == jackpot[1]) && (jackpot[1] == jackpot[2])) //3 in a row
	{
		Draw_String(0, 6, "JACKPOT! you win                        ");
		jackpotmoney = betmoney * 6;// multiply how much person betted
		money += jackpotmoney;//add to total what he won
		string moneystring = to_string(money); //convert to string
		string jackpotmoneystring = to_string(jackpotmoney);
		Draw_String(17, 0, moneystring);
		Draw_String(17, 6, jackpotmoneystring); //display current money and how much user won
		betmoney = 10; //reset betmoney to 10, its all factors of 10, so either user will have at least 10 or game will detect that he is bankrupt
		Draw_String(9, 4, "10  ");
	}
	else if ((jackpot[0] == jackpot[1]))//2 first symbols
	{
		Draw_String(0, 6, "small JACKPOT! you win                             ");
		jackpotmoney = betmoney * 3;
		money = money + jackpotmoney;
		string moneystring = to_string(money);
		string jackpotmoneystring = to_string(jackpotmoney);
		Draw_String(17, 0, moneystring);
		Draw_String(23, 6, jackpotmoneystring);
		betmoney = 10;
		Draw_String(9, 4, "10  ");
    }
	else if ((jackpot[1] == jackpot[2]))// last 2 symbols
	{
		Draw_String(0, 6, "small JACKPOT! you win                                 ");
		jackpotmoney = betmoney * 3;
		money = money + jackpotmoney;
		string moneystring = to_string(money);
		string jackpotmoneystring = to_string(jackpotmoney);
		Draw_String(17, 0, moneystring);
		Draw_String(23, 6, jackpotmoneystring);
		betmoney = 10;
		Draw_String(9, 4, "10  ");
	}
	else
	{
		Draw_String(0, 6, "Try again                           ");// no symbols in row
		betmoney = 10; // reset bet money to avoid problemw with iffs in spinnwheel
		Draw_String(9, 4, "10  ");
	    if (money < 1) //check if bancrupt (money are only in factors of 10
	    {
			bankrupt = 1; //bankrupt not pressed x (display mean bankrupt message)
			gamerunning = 0;
	    }
	}
	spinagain(key, stopwheel, gamerunning, wheelspeed, endgame);//wait for user to decide to spin again
}
void stopwheelmessages()
{
	if (stopwheel < 3)//display "stop first wheel" "stop second wheel" messages
	{
		stopwheel++; //which wheel
		if (stopwheel == 1)
		{
			Draw_String(0, 3, "Press space bar to stop second wheel                ");
		}
		else if (stopwheel == 2)
		{
			Draw_String(0, 3, "Press space bar to stop third wheel                 ");
		}
		else if (stopwheel == 3)
		{
			Draw_String(0, 3, "You have stopped all wheels              ");//these spaces are required to remove previous text
		}
	}
}
void spinwheel(int &money)
{
	for (int symbol = 7; symbol > 1; symbol--)//loops around all symbols in wheel array
	{
		for (int how_long_wheel_loop = 9; how_long_wheel_loop < 14; how_long_wheel_loop++) //loops around every character displayed
			//[change 6 to variable and ask user for size ? (do not display very last array character, modulo function dont like that)]
		{
			how_long_wheel_loop_outsideloop = how_long_wheel_loop;
			for (int noofwheels = (0 + stopwheel); noofwheels < 3; noofwheels++) //loops around every wheel, it also points at correct wheelrow
			{
				if (stopwheel == 4)
				{
					goto spinagain; //when stopping wheels it reduces value inside loop causing invinite loop witchout this, this moves program outside of loop
			    }
				Draw_String((2 + noofwheels), how_long_wheel_loop, wheel[noofwheels][((symbol + (how_long_wheel_loop - 1)) % 6)]); //modulo function is used to loop spin( a mod b = a − ( Int [a / b] * b) 
			}
			///////////////////////////////////
		}
		if (wheelspeed < 200) //min speed , 200 is perhaps too slow
		{
			wheelspeed += 1; //how long it will take to reach min speed
		}
		Sleep(wheelspeed); //speed of wheel spin

		spinagain:
		/////////////////////////////////key controlls

		if (_kbhit())
		{
			key = toupper(_getch());
			//exit
			if (key == 'X' || key == 27) //x or esc
			{
				endgame = 1;
				gamerunning = 0;
				goto end; //end game //seperate void for controlls ? , i have problems with corss function goto
			} //have to be here to work quickly

			//this will stop a single wheel when space is clicked
			if (key == 32) //space
			{
				stopwheelmessages();
				///////////////////////////////////////////////

				if (stopwheel == 1 && cheked[0]) //check what wheel and if this row is allready done
				{
					money -= betmoney; // take away money you put inside the machine
					string startingmoneystring = to_string(money);// convert to string
					Draw_String(17, 0, "    ");// reset display in case of going from 100 to 90
					Draw_String(17, 0, startingmoneystring);
					cheked[0] = 0; //once assigned value do not access this if again
					jackpot[0] = wheel[0][((symbol + (how_long_wheel_loop_outsideloop + 3)) % 6)]; //+3 because all symbols move another cycle
				}
				if (stopwheel == 2 && cheked[1])
				{
					cheked[1] = 0;
					jackpot[1] = wheel[1][((symbol + (how_long_wheel_loop_outsideloop + 3)) % 6)];
				}
				if (stopwheel == 3 && cheked[2])
				{
					cheked[2] = 0;
					jackpot[2] = wheel[2][((symbol + (how_long_wheel_loop_outsideloop + 3)) % 6)];
					jackpot_questionmark(money); //check for jackpots
				}
			}
			//in/decrease stakes
			if (key == 'Q' && stopwheel == 0)// does not allow to change stakes once paid and stooped at least 1 wheel
			{
				if (betmoney > 10)// decrease bet if money betting is more than 10
				{
					betmoney -= 10; //allow only to bet in factors of 10
					betmoneystring = to_string(betmoney);//convert to string
					Draw_String(11, 4, "  ");//empty space to clear out last 0 if goes under hunderts
					Draw_String(9, 4, betmoneystring);
				}
			}
			if (key == 'W' && stopwheel  == 0)
			{
				if (betmoney < 1000 && money > betmoney)// increase bet if have enough money for it, cap of 1000
				{
					betmoney = betmoney + 10;
					betmoneystring = to_string(betmoney);
					Draw_String(9, 4, betmoneystring);
				}
			}
			if (key == 'A')//slow spin, drasticly slows down wheel spinning time, allow to reverse it
			{
				Draw_String(0, 2, "Press S to stop slow spin   ");
				wheelspeed = 800;
			}
			if (key == 'S')
			{
				Draw_String(0, 2, "Press A to start slow spin   ");
				wheelspeed = 30;
			}
		}
	}
end: 
	if (0)
	{ }
	 // i added if becouse compiler dont like when label was pointing at }
//use goto so program wont have to wait for wheels do to full spin, set it to zero here because end: dont like to be alone.
}
int main(int startingmoney, int money)
{
	if (bankrupt || endgame)// check if required to finish game
	{
		goto endgame;
	}
	else
	{ //these calls are in else to stop them from overprinting endgame and bancrupt messages
		startgraphics(); //set up game
		getinfo(money, startingmoney);
		displaycosmetics(money);
		createwheels();
	}

    while (gamerunning) //spin wheel
	{
		spinwheel(money);
	}

endgame:

	system("cls"); //it have to be split into this if and if above because system("cls") cause problems when it was before goto.
	if (bankrupt)
	{
		cout << "Bankrupt! this is why you should not gamble" << endl; //mean message
		cout << "You lost " << startingmoney << endl;
	}
	cout << "You started with "<< startingmoney << " and finished with "<<money  << endl;
	cout << "Game over " << endl;
}