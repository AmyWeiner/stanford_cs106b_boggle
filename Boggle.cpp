/*
 * File: Boggle.cpp
 * ----------------
 * Name: [TODO: Amy Weiner]
 * Section: [TODO: Sammy Nguyen]
 * This file is the main starter file for Assignment #4, Boggle.
 * [TODO: extend the documentation]
 */
 
#include <iostream>
#include "gboggle.h"
#include "graphics.h"
#include "grid.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
const int MINIMUM_WORD_LENGTH = 4;


const string STANDARD_CUBES[16]  = {
   "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
   "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
   "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
   "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};
 
const string BIG_BOGGLE_CUBES[25]  = {
   "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
   "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
   "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
   "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
   "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void welcome();
bool wantsInstructions();
void giveInstructions();
bool isReady();
void getBoardSize(int & numRows, int & numColumns); 
bool isForcedConfiguration();
string getCubeLetters();
void shakeBoggleCubes(Grid<char> & boggleBoard, Vector<string> & boggleCubes, const string * pointer, int cubeSize);
void shakeBoggleCubesForced(Grid<char> & boggleBoard, Vector<char> & boggleCubes, string cubeLetters);
void depositLetters(Grid<char> & boggleBoard, Vector<string> & boggleCubes);
void depositLettersForced(Grid<char> & boggleBoard, Vector<char> & boggleCubes);
void takeHumanTurn(Lexicon & english, Player name, Grid<char> & boggleBoard, Set<string> & wordList);
bool isOnBoard(string currentWord, Grid<char> & boggleBoard, Grid<bool> & hasVisited);
bool isOnBoardRec(string currentWord, Grid<char> & boggleBoard, int startRow, int startColumn, Grid<bool> & hasVisited);
void displayWord(Grid<bool> & hasVisited);
void takeComputerTurn(Lexicon & english, Player name, Grid<char> & boggleBoard, Set<string> & wordList);
void getValidWord(Lexicon & english, Grid<char> & boggleBoard, int currentRow, int currentColumn, string word, Set<string> & computerWordList, Set<string> & wordList);
void displayComputerWords(Set<string> computerWordList, Player name);

/* Main program */

int main() {
    initGraphics(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    welcome();
    int numRows = 0;
    int numColumns = 0;
        if (wantsInstructions()) {
        giveInstructions();
        getBoardSize(numRows,numColumns);
        drawBoard(numRows, numColumns);
    } else if (isReady()){
        getBoardSize(numRows,numColumns);
        drawBoard(numRows, numColumns);
    }
    Grid<char> boggleBoard(numRows, numColumns); 
    Vector<string> boggleCubes;
    if (isForcedConfiguration()) {
        string cubeLetters = getCubeLetters();
        cubeLetters = toUpperCase(cubeLetters);
        Vector<char> boggleCubes;
        shakeBoggleCubesForced(boggleBoard, boggleCubes, cubeLetters);
    } else {
        int standardCubeSize = sizeof STANDARD_CUBES / sizeof STANDARD_CUBES[0];
        int bigBoggleCubeSize = sizeof BIG_BOGGLE_CUBES / sizeof BIG_BOGGLE_CUBES[0];
        const string* p = STANDARD_CUBES;
        const string* q = BIG_BOGGLE_CUBES;
        if (numRows == 4) {
            shakeBoggleCubes(boggleBoard, boggleCubes, p, standardCubeSize);
        } else {
            shakeBoggleCubes(boggleBoard, boggleCubes, q, bigBoggleCubeSize);
        }
    }
    Set<string> wordList;
    Lexicon english("EnglishWords.dat");
    Player name = HUMAN;
    takeHumanTurn(english, name, boggleBoard, wordList);
    name = COMPUTER;
    takeComputerTurn(english, name, boggleBoard, wordList);
    return 0;
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */
void welcome() {
   cout << "Welcome!  You're about to play an intense game ";
   cout << "of mind-numbing Boggle.  The good news is that ";
   cout << "you might improve your vocabulary a bit.  The ";
   cout << "bad news is that you're probably going to lose ";
   cout << "miserably to this little dictionary-toting hunk ";
   cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: wantsInstructions
 * Usage: if (wantsInstructions()). . . 
 * ------------------------------------
 * This function returns whether or not 
 * the user wants the instrucitons of 
 * the game. 
 */
bool wantsInstructions() {
    string instructions = getLine("Do you need instructions? ");
    return(instructions[0] == 'y' || instructions[0] == 'Y');
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions 
 * for the user.
 */
void giveInstructions() {
   cout << endl;
   cout << "The boggle board is a grid onto which I ";
   cout << "will randomly distribute cubes. These ";
   cout << "6-sided cubes have letters rather than ";
   cout << "numbers on the faces, creating a grid of ";
   cout << "letters on which you try to form words. ";
   cout << "You go first, entering all the words you can ";
   cout << "find that are formed by tracing adjoining ";
   cout << "letters. Two letters adjoin if they are next ";
   cout << "to each other horizontally, vertically, or ";
   cout << "diagonally. A letter can only be used once ";
   cout << "in each word. Words must be at least four ";
   cout << "letters long and can be counted only once. ";
   cout << "You score points based on word length: a ";
   cout << "4-letter word is worth 1 point, 5-letters ";
   cout << "earn 2 points, and so on. After your puny ";
   cout << "brain is exhausted, I, the supercomputer, ";
   cout << "will find all the remaining words and double ";
   cout << "or triple your paltry score." << endl << endl;
   cout << "Hit return when you're ready...";
   getLine();
}

/*
 * Function: isReady
 * Usage: if (isReady()). . . 
 * --------------------------
 * Prompts to determine whether
 * or not the user is ready to
 * begin play, and if so, 
 * prompts for the board size.
 */
bool isReady() {
    string ready = getLine("Hit return when you're ready... ");
    return (ready == "");     
}

/*
 * Function: getBoardSize
 * Usage: getBoardSize();
 * ----------------------------
 * Prompts to determine whether
 * or not the user would like
 * to play Big Boggle, which in
 * turn determines the size of
 * the Boggle game board. 
 */
void getBoardSize(int & numRows, int & numColumns) {
    cout << endl;
    cout << "You can choose standard Boggle (4x4 grid)" << endl;
    cout << "or Big Boggle (5x5)." << endl;
    string bigBoggle = getLine("Would you like Big Boggle? ");
    if (bigBoggle[0] == 'y' || bigBoggle[0] == 'Y') {
        numRows = 5;
        numColumns = 5;
    } else if (bigBoggle[0] == 'n' || bigBoggle[0] == 'N') {
        numRows = 4;
        numColumns = 4;
    }
}

/*
 * Function: isForcedConfiguraiton
 * Usage: if (isForcedConfiguraiton()). . .
 * ----------------------------------------
 * Returns whether or not the user wants to
 * force the board configuration.
 */
bool isForcedConfiguration() {
    cout << endl;
    cout << "I'll give you a chance to set up the board to your specification, which makes it easier to confirm your boggle program is working." << endl << endl;
    string configuration = getLine("Do you want to force the board configuration? "); 
    return (configuration[0] == 'y' || configuration[0] == 'Y');
}

/*
 * Function: getCubeLetters
 * Usage: string cubeLetters = getCubeLetters();
 * ---------------------------------------------
 * This function returns the cube letters to be
 * used on the boggle board if the user chooses
 * to force the board configuraiton. 
 */
string getCubeLetters(){
    cout << "Enter a 16-character string to identify which letters you want on the cubes. " << endl;
    cout << "The first 4 letters are the cubes on the top row from left to right, the next 4 letters are the second row, and so on." << endl;
    string cubeLetters = getLine("Enter the string: ");
    return cubeLetters;
}

/*
 * Function: shakeBoggleCubes
 * Usage: shakeBoggleCubes();
 * --------------------------
 * This function randomizes 
 * the arrangement of the
 * boggle cubes on the board.
 */
void shakeBoggleCubes(Grid<char> & boggleBoard, Vector<string> & boggleCubes, const string * pointer, int cubeSize) {
    for (int i = 0; i < cubeSize; i ++) {
        boggleCubes.add(pointer[i]);
    }
    for (int i = 0; i < boggleCubes.size(); i++) {
        int randomIndex = randomInteger(i, 15);
        string a = boggleCubes[i];
        string b = boggleCubes[randomIndex];
        boggleCubes[i] = b;
        boggleCubes[randomIndex] = a;
    }
    depositLetters(boggleBoard, boggleCubes);
}

/*
 * Function: shakeBoggleCubesFOrced
 * Usage: shakeBoggleCubesForced();
 * --------------------------------
 * This function randomizes the 
 * arrangement of the letters entered
 * by the user.
 */
void shakeBoggleCubesForced(Grid<char> & boggleBoard, Vector<char> & boggleCubes, string cubeLetters) {
    for (int i = 0; i < cubeLetters.length(); i ++) {
        boggleCubes += cubeLetters[i];
    }
    for (int i = 0; i < boggleCubes.size(); i++) {
        int randomIndex = randomInteger(i, 15);
        char a = boggleCubes[i];
        char b = boggleCubes[randomIndex];
        boggleCubes[i] = b;
        boggleCubes[randomIndex] = a;
    }
    depositLettersForced(boggleBoard, boggleCubes);
}


/*
 * Function: depositLetters
 * Usage: depositLetters();
 * ------------------------
 * This function randomizes
 * the orientation of each
 * boggle cube once it is
 * arranged on the board. 
 */
void depositLetters(Grid<char> & boggleBoard, Vector<string> & boggleCubes) {
    for (int i = 0; i < boggleCubes.size(); i ++) {
        int randomIndex = randomInteger(0, 5);
        string cubeLetters = boggleCubes[i];
        char randomLetter = cubeLetters[randomIndex];
        int row = i / boggleBoard.numRows();
        int column = i % boggleBoard.numCols();
        boggleBoard[row][column] = randomLetter;
        labelCube(row, column, randomLetter);
    }
}

/*
 * Function: depositLettersForced
 * Usage: depositLettersForced();
 * ------------------------------
 * This function randomizes fills
 * the boggle grid in with the
 * randomized letters. 
 */
void depositLettersForced(Grid<char> & boggleBoard, Vector<char> & boggleCubes) {
    for (int i = 0; i < boggleCubes.size(); i ++) {
        char letter = boggleCubes[i];
        int row = i / boggleBoard.numRows();
        int column = i % boggleBoard.numCols();
        boggleBoard[row][column] = letter;
        labelCube(row, column, letter);
    }
}

/*
 * Function: takeHumanTurn
 * Usage: takeHumanTurn();
 * --------------------------
 * This function prompts the
 * user to begin entering
 * words as part of the
 * user player's turn. 
 */
void takeHumanTurn(Lexicon & english, Player name, Grid<char> & boggleBoard, Set<string> & wordList) {
    cout << endl;
    cout << "Ok, take all the time you want and find all the words you can!  Signal that you're finished by entering an empty line." << endl;
    while (name == HUMAN) {
        Grid<bool> hasVisited(boggleBoard.numRows(), boggleBoard.numCols());
        string currentWord = getLine("Enter a word: ");
        currentWord = toUpperCase(currentWord);
        if (currentWord == "") {
            break;
        }
        if (currentWord.length() < MINIMUM_WORD_LENGTH) {
            cout << "I'm sorry, but we have our standards." << endl;
            cout << "That word doesn't meet the minimum word length." << endl;
        }
        if (!english.contains(currentWord)) {
            cout << "That's not a word!" << endl;
        }
        if (wordList.contains(currentWord)) {
            cout << "You've already guessed that!" << endl;
        }
        if (isOnBoard(currentWord, boggleBoard, hasVisited)) {
            wordList.add(currentWord);
            recordWordForPlayer(currentWord, name);
            displayWord(hasVisited);
        } else {
            cout << "That's not on the board!" << endl;
        }
    }
}

/*
 * Function: isOnBoard
 * Usage: if (isOnBoard()). . .
 * ----------------------------
 * Returns whether or not the 
 * word that the user entered
 * exists on the game board.
 */
bool isOnBoard(string currentWord, Grid<char> & boggleBoard, Grid<bool> & hasVisited) {
    int startRow = 0;
    int startColumn = 0;
    for (int i = 0; i < boggleBoard.numRows(); i ++) {
        for (int j = 0; j < boggleBoard.numCols(); j ++) {
            if (boggleBoard[i][j] == currentWord[0]) {
                startRow = i;
                startColumn = j;
                if(isOnBoardRec(currentWord, boggleBoard, startRow, startColumn, hasVisited)) return true;
            }
        }
    }
    return false;
}

/*
 * Function: isOnBoardRec
 * Usage: if (isOnBoardRec()). . .
 * -------------------------------
 * Returns whether or not the word
 * that the user entered exists on
 * the game board.
 */
bool isOnBoardRec(string currentWord, Grid<char> & boggleBoard, int startRow, int startColumn, Grid<bool> & hasVisited) {
    if (currentWord.size() <= 1) {
        return true;
    } else {
        char nextLetter = currentWord[1];
        for (int i = -1; i <= 1; i ++) {
            for (int j = -1; j <= 1; j ++) {
                if(!(i == 0 && j == 0) && boggleBoard.inBounds(startRow + i, startColumn + j)) {
                    if (boggleBoard[startRow + i][startColumn + j] == nextLetter) {
                        if (hasVisited[startRow + i][startColumn + j] == true) {
                            return false;
                        }
                        hasVisited[startRow + i][startColumn + j] = true;
                        if(isOnBoardRec(currentWord.substr(1), boggleBoard, startRow + i, startColumn + j, hasVisited)) return true;
                    }
                    hasVisited[startRow + i][startColumn + j] = false;
                }
            }
        }
    }
    return false;
}

/*
 * Function: displayWord
 * Usage: displayWord();
 * ---------------------
 * 
 */
void displayWord(Grid<bool> & hasVisited) {
    for (int i = 0; i < hasVisited.numRows(); i ++) {
        for (int j = 0; j < hasVisited.numCols(); j ++) {
            if (hasVisited[i][j] == true) {
                highlightCube(i, j, true);
            }
        }
    }
}    

/*
 * Function: takeComputerTurn
 * Usage: takeComputerTurn();
 * --------------------------
 * Initiates the computer
 * player's turn.
 */
void takeComputerTurn(Lexicon & english, Player name, Grid<char> & boggleBoard, Set<string> & wordList) {
    Set<string> computerWordList;
    for (int i = 0; i < boggleBoard.numRows(); i ++) {
        for (int j = 0; j < boggleBoard.numCols(); j ++) {
            int currentRow = i;
            int currentColumn = j;
            string word = "";
            getValidWord(english, boggleBoard, currentRow, currentColumn, word, computerWordList, wordList);
        }
    }
    displayComputerWords(computerWordList, name);
}

/*
 * Function: getValidWord
 * Usage: getValidWord();
 * --------------------------
 * Finds all of the valid
 * words on the game board
 * that were not found by the
 * user. 
 */
void getValidWord(Lexicon & english, Grid<char> & boggleBoard, int currentRow, int currentColumn, string word, Set<string> & computerWordList, Set<string> & wordList) {
    if (!english.containsPrefix(word)) {
        return;
    } else if (english.contains(word) && word.length() >= MINIMUM_WORD_LENGTH) {
            computerWordList.add(word);
    }
        for (int i = -1; i <= 1; i ++) {
            for (int j = -1; j <= 1; j ++) {
                if(!(i == 0 && j == 0) && boggleBoard.inBounds(currentRow + i, currentColumn + j)) {
                    char nextLetter = boggleBoard[currentRow + i][currentColumn + j];
                    string newWord = word + nextLetter;
                    getValidWord(english, boggleBoard, currentRow + i, currentColumn + j, newWord, computerWordList, wordList);
                }
            }
        
    }
}

/*
 * Function: displayComputerWords
 * Usage: displayComputerWords();
 * ------------------------------
 * Displays the words the computer
 * found on the screen. 
 */
void displayComputerWords(Set<string> computerWordList, Player name) {
    foreach(string word in computerWordList) {
        string currentWord = word;
        recordWordForPlayer(currentWord, name);
    }
}





