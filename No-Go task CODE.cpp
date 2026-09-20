#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <windows.h>
using namespace std;

// ---------------- CONFIGURATION ----------------
const char GO_LETTER      = 'X';   // press SPACE for this
const char NOGO_LETTER    = 'Y';   // do NOT press for this
const int  NOGO_PERCENT   = 25;    // % of trials that are No-Go
const int  STIMULUS_MS    = 500;   // how long the letter is visible
const int  RESPONSE_MS    = 1200;  // total response window per trial
const int  MAX_TRIALS     = 200;

// Illustrative interpretation thresholds (not clinical norms)
const double HIGH_COMMISSION_RATE = 25.0;  // % of No-Go trials
const double HIGH_OMISSION_RATE   = 20.0;  // % of Go trials

// ---------------- HELPERS ----------------
void clearScreen() { system("cls"); }

void showStimulus(char c) {
    clearScreen();
    cout << "\n\n\n\n            " << c << "\n";
}

void showFixation() {
    clearScreen();
    cout << "\n\n\n\n            +\n";
}

void flushKeyboard() {
    while (kbhit()) getch();
}

// Builds a shuffled sequence with an exact proportion of No-Go trials
void buildSequence(char seq[], int trials) {
    int nogoCount = trials * NOGO_PERCENT / 100;
    for (int i = 0; i < trials; i++)
        seq[i] = (i < nogoCount) ? NOGO_LETTER : GO_LETTER;

    // Fisher-Yates shuffle
    for (int i = trials - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char tmp = seq[i]; seq[i] = seq[j]; seq[j] = tmp;
    }
}

// Runs one trial. Returns true if participant responded; rt receives reaction time.
bool runTrial(char stimulus, DWORD &rt) {
    flushKeyboard();
    showStimulus(stimulus);

    DWORD start = GetTickCount();
    bool responded = false;
    bool stimulusHidden = false;
    rt = 0;

    while (GetTickCount() - start < (DWORD)RESPONSE_MS) {
        if (!stimulusHidden && GetTickCount() - start >= (DWORD)STIMULUS_MS) {
            showFixation();
            stimulusHidden = true;
        }
        if (!responded && kbhit()) {
            char key = getch();
            if (key == ' ') {
                rt = GetTickCount() - start;
                responded = true;
            }
        }
        Sleep(1);
    }
    return responded;
}

// ---------------- MAIN ----------------
int main() {
    int trials;
    char seq[MAX_TRIALS];

    cout << "=====================================\n";
    cout << "   GO / NO-GO TASK (Response Inhibition)\n";
    cout << "=====================================\n\n";
    cout << "Enter number of trials (20-" << MAX_TRIALS << ", recommended 60): ";
    cin >> trials;
    if (trials < 20) trials = 20;
    if (trials > MAX_TRIALS) trials = MAX_TRIALS;

    cout << "\nINSTRUCTIONS:\n";
    cout << " - Letters will flash one at a time.\n";
    cout << " - Press SPACE as fast as you can for every letter EXCEPT '" << NOGO_LETTER << "'.\n";
    cout << " - Do NOT press anything when you see '" << NOGO_LETTER << "'.\n";
    cout << " - Speed and accuracy both matter.\n\n";
    cout << "Press any key to begin...";
    getch();

    for (int i = 3; i >= 1; i--) {
        clearScreen();
        cout << "\n\n\n\n        Starting in " << i << "...\n";
        Sleep(1000);
    }

    srand((unsigned)time(NULL));
    buildSequence(seq, trials);

    // Results
    int impulsive_errors = 0;   // commission errors (pressed on No-Go)
    int attention_errors = 0;   // omission errors (missed a Go)
    int hits = 0;               // correct responses on Go
    int correctRejections = 0;  // correctly withheld on No-Go
    int goTrials = 0, nogoTrials = 0;
    long totalRT = 0;

    for (int i = 0; i < trials; i++) {
        DWORD rt;
        bool responded = runTrial(seq[i], rt);

        if (seq[i] == GO_LETTER) {
            goTrials++;
            if (responded) { hits++; totalRT += rt; }
            else           { attention_errors = attention_errors + 1; }
        } else {
            nogoTrials++;
            if (responded) { impulsive_errors = impulsive_errors + 1; }
            else           { correctRejections++; }
        }
    }

    // ---------------- RESULTS ----------------
    double commissionRate = nogoTrials ? (100.0 * impulsive_errors / nogoTrials) : 0;
    double omissionRate   = goTrials   ? (100.0 * attention_errors / goTrials)   : 0;
    double accuracy       = 100.0 * (hits + correctRejections) / trials;

    clearScreen();
    cout << fixed << setprecision(1);
    cout << "=============== RESULTS ===============\n\n";
    cout << "Total trials:               " << trials << "\n";
    cout << "Go trials:                  " << goTrials << "\n";
    cout << "No-Go trials:               " << nogoTrials << "\n\n";
    cout << "Hits (correct Go):          " << hits << "\n";
    cout << "Correct rejections:         " << correctRejections << "\n";
    cout << "Commission errors:          " << impulsive_errors << "  (" << commissionRate << "% of No-Go)\n";
    cout << "Omission errors:            " << attention_errors << "  (" << omissionRate << "% of Go)\n\n";
    cout << "Overall accuracy:           " << accuracy << " %\n";
    if (hits > 0)
        cout << "Mean reaction time (hits):  " << (totalRT / hits) << " ms\n";

    cout << "\n--------------- INTERPRETATION ---------------\n";
    if (commissionRate >= HIGH_COMMISSION_RATE)
        cout << "* High commission errors -> suggests IMPULSIVITY (poor response inhibition).\n";
    else
        cout << "* Commission errors within expected range -> good response inhibition.\n";

    if (omissionRate >= HIGH_OMISSION_RATE)
        cout << "* High omission errors -> suggests INATTENTION.\n";
    else
        cout << "* Omission errors within expected range -> good sustained attention.\n";

    cout << "\n(Note: thresholds are illustrative, not a clinical diagnosis.)\n";
    cout << "\nPress any key to exit...";
    flushKeyboard();
    getch();
    return 0;
}
