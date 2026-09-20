#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include <string>

using namespace std;

void clearScreen()
{
    system("cls");
}

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printHeader()
{
    setColor(11);

    cout << "============================================================\n";
    cout << "                 N-BACK MEMORY CHALLENGE                    \n";
    cout << "============================================================\n";

    setColor(7);
}

void waitForEnter()
{
    cout << "\nPress ENTER to continue...";
    cin.ignore();
    cin.get();
}

int getInteger(string message, int minValue, int maxValue)
{
    int value;

    while (true)
    {
        cout << message;

        if (cin >> value && value >= minValue && value <= maxValue)
        {
            cin.ignore(1000, '\n');
            return value;
        }

        cout << "Invalid input. Please enter a value between "
             << minValue << " and " << maxValue << ".\n";

        cin.clear();
        cin.ignore(1000, '\n');
    }
}

void countdown()
{
    setColor(14);

    cout << "\nGet ready...\n\n";

    for (int i = 3; i >= 1; i--)
    {
        cout << "                 " << i << "\n";
        Sleep(800);
    }

    cout << "\n                 GO!\n";
    Sleep(700);

    setColor(7);
}

int main()
{
    srand((unsigned int)time(NULL));

    int nBack;
    int totalTrials;
    int displayTime;


    clearScreen();
    printHeader();

    cout << "\n              WORKING MEMORY ASSESSMENT\n\n";

    cout << "The N-Back task measures working memory by requiring\n";
    cout << "you to compare the current digit with a digit shown\n";
    cout << "N positions earlier.\n\n";

    cout << "Example for 2-Back:\n\n";

    cout << "Sequence:       4   7   4   9   7\n";
    cout << "                 ^       ^\n";
    cout << "                 |       |\n";
    cout << "              2 positions apart\n\n";

    cout << "If the current digit matches the digit shown N steps\n";
    cout << "back, press the M key.\n\n";

    cout << "If there is NO match, do not press anything.\n\n";

    setColor(10);
    cout << "IMPORTANT: Respond quickly and accurately.\n";
    setColor(7);

    cout << "\n------------------------------------------------------------\n";


    nBack = getInteger(
        "\nSelect N-Back level (1-3): ",
        1,
        3
    );

    totalTrials = getInteger(
        "Number of trials (10-100): ",
        10,
        100
    );

    displayTime = getInteger(
        "Digit presentation time in milliseconds (500-3000): ",
        500,
        3000
    );

    // --------------------------------------------------------
    // GENERATE SEQUENCE
    // --------------------------------------------------------

    vector<int> sequence(totalTrials);

    for (int i = 0; i < totalTrials; i++)
    {
        sequence[i] = rand() % 10;
    }

    // Force approximately 30% target trials.
    // This gives the participant enough matching trials.

    for (int i = nBack; i < totalTrials; i++)
    {
        if (rand() % 100 < 30)
        {
            sequence[i] = sequence[i - nBack];
        }
    }

    // --------------------------------------------------------
    // GAME START
    // --------------------------------------------------------

    clearScreen();
    printHeader();

    cout << "\nYour selected task:\n";
    cout << "N-Back Level : " << nBack << "-Back\n";
    cout << "Trials       : " << totalTrials << "\n";
    cout << "Display Time : " << displayTime << " ms\n\n";

    cout << "RULES\n";
    cout << "------------------------------------------------------------\n";
    cout << "1. A digit will appear in the center of the screen.\n";
    cout << "2. Compare it with the digit shown " << nBack << " step(s) ago.\n";
    cout << "3. Press M if they MATCH.\n";
    cout << "4. Do NOTHING if they do not match.\n";
    cout << "5. Respond as quickly as possible.\n";
    cout << "------------------------------------------------------------\n";

    waitForEnter();

    countdown();

    // --------------------------------------------------------
    // VARIABLES FOR SCORING
    // --------------------------------------------------------

    int correctHits = 0;
    int misses = 0;
    int falseAlarms = 0;
    int correctRejections = 0;

    int targetCount = 0;
    int nonTargetCount = 0;

    // --------------------------------------------------------
    // TASK
    // --------------------------------------------------------

    for (int i = 0; i < totalTrials; i++)
    {
        bool target = false;

        if (i >= nBack)
        {
            target = (sequence[i] == sequence[i - nBack]);
        }

        if (target)
            targetCount++;
        else
            nonTargetCount++;

        clearScreen();

        printHeader();

        cout << "\n\n";
        cout << "                    TRIAL "
             << (i + 1) << " / " << totalTrials << "\n\n";

        setColor(15);

        cout << "\n\n";
        cout << "                         " << sequence[i] << "\n";
        cout << "\n\n";

        setColor(8);
        cout << "              Press M if MATCH\n";
        cout << "              No key if NO MATCH\n";

        setColor(7);

        // ----------------------------------------------------
        // Capture response during digit presentation
        // ----------------------------------------------------

        bool responded = false;

        int elapsed = 0;
        const int interval = 10;

        while (elapsed < displayTime)
        {
            if (_kbhit())
            {
                char key = _getch();

                if (key == 'm' || key == 'M')
                {
                    responded = true;
                    break;
                }
            }

            Sleep(interval);
            elapsed += interval;
        }

        // ----------------------------------------------------
        // SCORE RESPONSE
        // ----------------------------------------------------

        if (target)
        {
            if (responded)
            {
                correctHits++;

                setColor(10);
                cout << "\n\n                 CORRECT!\n";
            }
            else
            {
                misses++;

                setColor(12);
                cout << "\n\n                 MISS\n";
            }
        }
        else
        {
            if (responded)
            {
                falseAlarms++;

                setColor(12);
                cout << "\n\n                 INCORRECT\n";
            }
            else
            {
                correctRejections++;

                setColor(10);
                cout << "\n\n                 CORRECT\n";
            }
        }

        setColor(7);

        Sleep(350);
    }

    // --------------------------------------------------------
    // CALCULATE RESULTS
    // --------------------------------------------------------

    int totalCorrect = correctHits + correctRejections;

    double accuracy =
        ((double)totalCorrect / totalTrials) * 100.0;

    double hitRate = 0.0;

    if (targetCount > 0)
    {
        hitRate =
            ((double)correctHits / targetCount) * 100.0;
    }

    double falseAlarmRate = 0.0;

    if (nonTargetCount > 0)
    {
        falseAlarmRate =
            ((double)falseAlarms / nonTargetCount) * 100.0;
    }

    // --------------------------------------------------------
    // RESULTS SCREEN
    // --------------------------------------------------------

    clearScreen();
    printHeader();

    cout << "\n                 ASSESSMENT COMPLETE\n\n";

    cout << "------------------------------------------------------------\n";

    cout << left << setw(30)
         << "N-Back Level:"
         << nBack << "-Back\n";

    cout << left << setw(30)
         << "Total Trials:"
         << totalTrials << "\n";

    cout << left << setw(30)
         << "Target Trials:"
         << targetCount << "\n";

    cout << left << setw(30)
         << "Non-Target Trials:"
         << nonTargetCount << "\n";

    cout << "------------------------------------------------------------\n";

    cout << left << setw(30)
         << "Correct Matches:"
         << correctHits << "\n";

    cout << left << setw(30)
         << "Missed Matches:"
         << misses << "\n";

    cout << left << setw(30)
         << "False Alarms:"
         << falseAlarms << "\n";

    cout << left << setw(30)
         << "Correct Rejections:"
         << correctRejections << "\n";

    cout << "------------------------------------------------------------\n";

    cout << fixed << setprecision(2);

    cout << left << setw(30)
         << "Overall Accuracy:"
         << accuracy << "%\n";

    cout << left << setw(30)
         << "Hit Rate:"
         << hitRate << "%\n";

    cout << left << setw(30)
         << "False Alarm Rate:"
         << falseAlarmRate << "%\n";

    cout << "------------------------------------------------------------\n\n";

    // --------------------------------------------------------
    // PERFORMANCE SUMMARY
    // --------------------------------------------------------

    setColor(11);

    cout << "PERFORMANCE SUMMARY\n\n";

    setColor(7);

    if (accuracy >= 90)
    {
        setColor(10);
        cout << "Excellent accuracy.\n";
    }
    else if (accuracy >= 75)
    {
        setColor(14);
        cout << "Good performance.\n";
    }
    else if (accuracy >= 60)
    {
        setColor(14);
        cout << "Moderate performance.\n";
    }
    else
    {
        setColor(12);
        cout << "The task may require additional practice.\n";
    }

    setColor(7);

    cout << "\nNote: This game provides task performance statistics\n";
    cout << "and should not be interpreted as a clinical diagnosis.\n";

    cout << "\n============================================================\n";

    cout << "\nPress ENTER to exit...";
    cin.get();

    return 0;
}

