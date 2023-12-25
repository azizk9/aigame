#include <cstdlib>
#include <iostream>
#include <conio.h>
#include <string.h>
#include <time.h>

#define RESET "\033[0;0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define BACKBLACK "\033[0;40m"
#define BACKRED "\033[0;41m"
#define BACKGREEN "\033[0;42m"
#define BACKYELLOW "\033[0;43m"
#define BACKBLUE "\033[0;44m"
#define BACKMAGENTA "\033[0;45m"
#define BACKCYAN "\033[0;46m"
#define BACKWHITE "\033[0;47m"

#define GAMESIZE 10
int AisComputer, BisComputer;
int board[GAMESIZE][GAMESIZE];
bool used[GAMESIZE][GAMESIZE];
int scoreA = 0, scoreB = 0;
int jokerA, jokerB;
bool turn = 0; // 0 is Player A, 1 is Player B
int selectible;
char ErrorMessage[100];
char AutoPlayer();

bool isSelectible(int i, int j);

void SetErrorMessage(const char *str)
{
    strcpy(ErrorMessage, str);
}

void Initiate()
{
    srand(time(NULL));
    for (int i = 0; i < GAMESIZE; i++)
        for (int j = 0; j < GAMESIZE; j++)
        {
            board[i][j] = rand() % 100 - 50;
            if (board[i][j] >= 0)
                board[i][j] += 1;
            used[i][j] = false;
        }
    selectible = GAMESIZE / 2;
    SetErrorMessage("");
    jokerA = jokerB = GAMESIZE / 3;
}

void DrawLine()
{
    printf("       --------------------------------------------------------\n");
}

void PrintBoard()
{
    printf("       %s", !turn ? YELLOW : "");
    for (int i = 0; i < GAMESIZE; i++)
        printf("     %c ", 65 + i);
    printf("\n" RESET);
    DrawLine();

    for (int i = 0; i < GAMESIZE; i++)
    {
        printf("    %s%c: " RESET, turn ? YELLOW : "", 65 + i);
        for (int j = 0; j < GAMESIZE; j++)
            if (isSelectible(i, j))
            {
                if (!used[i][j])
                    printf(YELLOW "[%5d]" RESET, board[i][j]);
                else
                    printf("       ");
            }
            else
            {
                if (!used[i][j])
                    printf("%6d ", board[i][j]);
                else
                    printf("       ");
            }
        printf("\n");
    }
}

bool isEmpty(int rowcol)
{
    if (!turn)
    {
        for (int i = 0; i < GAMESIZE; i++)
            if (!used[rowcol][i])
                return false;
    }
    else
    {
        for (int i = 0; i < GAMESIZE; i++)
            if (!used[i][rowcol])
                return false;
    }

    return true;
}

void PrintScores()
{
    printf("\n %c [%c] Player A: %5d   ", !turn ? '*' : ' ', AisComputer ? 'C' : 'H', scoreA);
    for (int i = 0; i < jokerA; i++)
        printf("J");

    printf("\n %c [%c] Player B: %5d   ", turn ? '*' : ' ', BisComputer ? 'C' : 'H', scoreB);
    for (int i = 0; i < jokerB; i++)
        printf("J");
    printf("\n\n");
    DrawLine();
    printf(" *: Joker\n");
    printf(" X: Exit\n");

    printf(BACKRED "%s \n" RESET, ErrorMessage);
    printf("Select A row or column\n");
    DrawLine();
}

bool isSelectible(int i, int j)
{
    if (!turn)
        return (i == selectible);
    else
        return (j == selectible);
}

void changeSelectible()
{
    int nonempty[GAMESIZE];
    int last = 0;

    for (int i = 0; i < GAMESIZE; i++)
        if (!isEmpty(i))
            nonempty[last++] = i;

    selectible = nonempty[rand() % last];
}

void SelectPlayers()
{
    char c;
    system("cls");
    printf("Select Player A: (H)uman or (C)omputer: ");
    c = toupper(getch());
    AisComputer = c == 'C';

    printf("\nSelect Player B: (H)uman or (C)omputer: ");
    c = toupper(getch());
    BisComputer = c == 'C';
}

char AutoPlayer()
{
    char selection;
    do
    {
        // Level 0: Random selection within the boundaries of the board
        if (rand() % 2 == 0)
        {
            selection = 65 + rand() % GAMESIZE;
        }
        else
        {
            // Level 1: Avoid selecting a number that has been chosen before
            do
            {
                selection = 65 + rand() % GAMESIZE;
            } while ((turn ? used[selection - 65][selectible] : used[selectible][selection - 65]));
        }

        // Level 2: Choose the best option to win or block opponent
        if (rand() % 2 == 1)
        {
            int bestScore = -1;
            char bestSelection = 'A';

            for (int i = 0; i < GAMESIZE; i++)
            {
                char currentSelection = 65 + i;
                if (!(turn ? used[i][selectible] : used[selectible][i]))
                {
                    int currentScore = turn ? board[i][selectible] : board[selectible][i];

                    // Prioritize blocking opponent's key positions
                    if (turn && currentScore > bestScore)
                    {
                        int opponentScore = 0;
                        for (int j = 0; j < GAMESIZE; j++)
                        {
                            if (!used[j][i])
                            {
                                opponentScore += board[j][i];
                            }
                        }
                        currentScore += opponentScore; // Consider opponent's potential score
                    }

                    if (currentScore > bestScore)
                    {
                        bestScore = currentScore;
                        bestSelection = currentSelection;
                    }
                }
            }

            if (bestScore > -1)
            {
                selection = bestSelection;
            }
            else
            {
                // If no good move found, fallback to random selection
                selection = 65 + rand() % GAMESIZE;
            }
        }

        // Level 3: Use the Joker at the right time
        if (rand() % 2 == 1)
        {
            if (!turn && jokerA > 0 && (rand() % 2 == 1)) // Use Joker if available with 50% probability
            {
                selection = '*';
            }
            else if (turn && jokerB > 0 && (rand() % 2 == 1)) // Use Joker if available with 50% probability
            {
                selection = '*';
            }
        }

        // Level 4: Consider opponent's moves
        if (rand() % 2 == 1)
        {
            int opponentPredictedMove = rand() % GAMESIZE;

            char temp = selection;
            selection = 65 + opponentPredictedMove;
            opponentPredictedMove = temp - 65;

            if (used[opponentPredictedMove][selectible])
            {
                // If the predicted move is in the same column/row, choose a different one
                do
                {
                    selection = 65 + rand() % GAMESIZE;
                } while ((turn ? used[selection - 65][selectible] : used[selectible][selection - 65]));
            }
        }

    } while ((turn ? used[selection - 65][selectible] : used[selectible][selection - 65]));

    return selection;
}

void MainLoop()
{
    char c = 0;
    while (c != 'X')
    {
        system("cls");
        PrintBoard();
        PrintScores();
        if (isEmpty(selectible))
        {
            printf(YELLOW "Game Over\n");
            if (scoreA > scoreB)
                printf("Player A WIN!!");
            else if (scoreB > scoreA)
                printf("Player B WIN!!");
            else // ==
                printf("Draw Game!");
            printf(RESET);
            break;
        }

        if (!turn)
            c = AisComputer ? AutoPlayer() : toupper(getch());
        else
            c = BisComputer ? AutoPlayer() : toupper(getch());

        if (c == 'X')
            break;
        if (c == '*')
        {
            SetErrorMessage("");
            if (!turn)
            {
                if (jokerA == 0)
                {
                    SetErrorMessage("You are out of Jokers");
                    continue;
                }
                jokerA--;
            }
            else
            {
                if (jokerB == 0)
                {
                    SetErrorMessage("You are out of Jokers");
                    continue;
                }
                jokerB--;
            }
            turn = !turn;
            changeSelectible();
            continue;
        }
        if ((c < 65) || (c > 64 + GAMESIZE))
        {
            SetErrorMessage("Select a row or column in range");
            continue;
        }
        int idx = c - 65;

        if ((!turn && used[selectible][idx]) || (turn && used[idx][selectible]))
        {
            SetErrorMessage("This cell is already used. Please select another cell");
            continue;
        }

        if (!turn) // Player A
        {
            scoreA += board[selectible][idx];
            used[selectible][idx] = true;
        }
        else // Player B
        {
            scoreB += board[idx][selectible];
            used[idx][selectible] = true;
        }

        selectible = idx;
        SetErrorMessage("");
        turn = !turn;
    }
}

int main(int argc, char **argv)
{
    SelectPlayers();
    Initiate();
    MainLoop();
    return 0;
}

