
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <ctime>
#include <fstream>
#include <chrono>
#include <windows.h> 
using namespace std;

// КОЛЬОРИ
const int BLACK = 0;
const int BLUE_C = 1;
const int GREEN_C = 2;
const int CYAN_C = 3;
const int RED_C = 4;
const int MAGENTA_C = 5;
const int YELLOW_C = 6;
const int WHITE = 7;

void setColor(int text, int bg = BLACK) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text + bg * 16);
}
void resetColor() { setColor(WHITE); }

void printColored(const string& s, int color, bool nl = false) {
    setColor(color);
    cout << s;
    resetColor();
    if (nl) cout << "\n";
}

char cp1251ToUpper(unsigned char c) {
    if (c >= 'a' && c <= 'z') return (char)(c - 32);
    if (c >= 224 && c <= 255) return (char)(c - 32);
  
    if (c == 186) return (char)170; 
    if (c == 179) return (char)178;
    if (c == 191) return (char)175; 
    return (char)c;
}

string toUpperCP1251(const string& s) {
    string r = s;
    for (size_t i = 0; i < r.size(); ++i)
        r[i] = cp1251ToUpper((unsigned char)r[i]);
    return r;
}

// ASCII шибениці
const vector<string> STAGES = {
    "  +---+\n  |   |\n  O   |\n /|\\  |\n / \\  |\n      |\n",
    "  +---+\n  |   |\n  O   |\n /|\\  |\n /    |\n      |\n",
    "  +---+\n  |   |\n  O   |\n /|\\  |\n      |\n      |\n",
    "  +---+\n  |   |\n  O   |\n /|   |\n      |\n      |\n",
    "  +---+\n  |   |\n  O   |\n  |   |\n      |\n      |\n",
    "  +---+\n  |   |\n  O   |\n      |\n      |\n      |\n",
    "  +---+\n  |   |\n      |\n      |\n      |\n      |\n",
    "  +---+\n      |\n      |\n      |\n      |\n      |\n"
};

void drawArt(int remaining) {
    int idx = (remaining < 0) ? 0 : (remaining > 7 ? 7 : remaining);
    int color = (idx == 0) ? RED_C : (idx < 3 ? YELLOW_C : GREEN_C);
    setColor(color);
    cout << STAGES[idx];
    resetColor();
}

struct WordBank {
    vector<string> engEasy = { "APPLE","BANANA","ORANGE","CHERRY","LEMON","MELON","GRAPE","PEACH" };
    vector<string> engHard = { "AWKWARD","BIZARRE","EMERALD","HANDLEBAR","WONDERS","PATHOLOGIST" };
    vector<string> ukrEasy = { "КНИГА","МІСТО","РІЧКА","ВЕСНА","КВІТКА","СОНЦЕ","ПТАХ","ХМАРА" };
    vector<string> ukrHard = { "ДИСТАНЦІЯ","ФІЛОСОФІЯ","ЛАБОРАТОРІЯ","ПАНОРАМА","СИМЕТРІЯ" };

    WordBank() {
        loadFromFile("words.txt");
    }

    void loadFromFile(const string& filename) {
        ifstream f(filename);
        if (!f) {
       
            printColored("[INFO] Файл \"" + filename + "\" не знайдено. "
                "Використовуються вбудовані слова.\n", YELLOW_C);
            return;
        }

        int lang, diff;
        string w;
        while (f >> lang >> diff >> w) {
            string wu = toUpperCP1251(w); 
            if (lang == 1 && diff == 1) engEasy.push_back(wu);
            else if (lang == 1 && diff == 2) engHard.push_back(wu);
            else if (lang == 2 && diff == 1) ukrEasy.push_back(wu);
            else if (lang == 2 && diff == 2) ukrHard.push_back(wu);
        }

        printColored("[INFO] Слова успішно завантажено з \"" + filename + "\".\n", GREEN_C);
    }
    string getWord(int lang, int difficulty) const {
        const vector<string>& pool =
            (lang == 1 && difficulty == 1) ? engEasy :
            (lang == 1 && difficulty == 2) ? engHard :
            (lang == 2 && difficulty == 1) ? ukrEasy : ukrHard;
        return pool[rand() % pool.size()];
    }
};

//  Статистика з обробкою помилок відкриття файлу
class Statistics {
    const string filename = "stats.txt";
public:
    void save(const string& word, bool won, int sec) {
        ofstream f(filename, ios::app);
        if (!f) {
            printColored("ПОМИЛКА Неможливо відкрити файл статистики \""
                + filename + "\" для запису.\n"
                "Перевірте права доступу до директорії.\n", RED_C);
            return;
        }
        f << (won ? "[WIN ] " : "[LOSE] ")
            << word << " | " << sec << "s\n";
        if (f.fail()) {
            printColored("[ПОМИЛКА] Помилка під час запису статистики.\n", RED_C);
        }
    }

    void show() const {
        printColored("\n--- GAME HISTORY ---\n", CYAN_C);
        ifstream f(filename);
        if (!f) {
            printColored("Ще немає записів (або файл недоступний).\n\n", YELLOW_C);
            return;
        }
        string line;
        bool hasContent = false;
        while (getline(f, line)) {
            cout << line << "\n";
            hasContent = true;
        }
        if (f.bad()) {
            printColored("ПОМИЛКА Помилка читання файлу статистики.\n", RED_C);
        }
        if (!hasContent) {
            cout << "Файл статистики порожній.\n";
        }
    }
};

// Система підказок
class HintSystem {
    int freeHints;
    int hintsUsed;
public:
    explicit HintSystem(int free = 1) : freeHints(free), hintsUsed(0) {}

    char giveHint(const string& secret, const string& display, int& remaining) {
        set<char> hidden;
        for (size_t i = 0; i < secret.size(); ++i)
            if (display[i] == '_')
                hidden.insert(secret[i]);

        if (hidden.empty()) return '\0';

        auto it = hidden.begin();
        advance(it, rand() % hidden.size());
        char ch = *it;

        if (hintsUsed < freeHints) {
            printColored("[HINT] Безкоштовна підказка! ", GREEN_C);
        }
        else {
            --remaining;
            printColored("[HINT] Штраф: -1 спроба!    ", YELLOW_C);
        }
        ++hintsUsed;

        string chStr(1, ch);
        printColored("Відкрита літера: ", WHITE);
        printColored(chStr + "\n", CYAN_C);
        return ch;
    }

    int totalUsed() const { return hintsUsed; }
};

// Результат ходу
enum class MoveResult { OK, QUIT, REPEAT };

// Клас гри
class HangmanGame {
    string  secret;
    string  display;
    int     remaining;
    bool    ukrainian;
    set<char> used;
    HintSystem hints;
    chrono::steady_clock::time_point startTime;
    int     finalSec;

    bool reveal(char ch) {
        bool found = false;
        for (size_t i = 0; i < secret.size(); ++i) {
            if (secret[i] == ch) {
                display[i] = ch;
                found = true;
            }
        }
        return found;
    }

    bool isWon() const {
        for (char c : display)
            if (c == '_') return false;
        return true;
    }
    void printBoard() const {
        system("cls");
        printColored(" HANGMAN  \n\n", MAGENTA_C);
        drawArt(remaining);

        cout << "\n  Слово:  ";
        for (size_t i = 0; i < display.size(); ++i) {
            if (display[i] == '_')
                printColored("_ ", WHITE);
            else
                printColored(string(1, display[i]) + " ", BLUE_C);
        }

        cout << "\n\n  Спроб залишилось: ";
        printColored(to_string(remaining) + "\n", YELLOW_C);

        cout << "  Введені літери:  ";
        for (char c : used) {
            bool correct = (secret.find(c) != string::npos);
            printColored(string(1, c) + " ", correct ? GREEN_C : RED_C);
        }

        cout << "\n\n";
        printColored("  [H] Підказка   [0] Вийти\n\n", CYAN_C);
    }

    MoveResult processInput() {
        cout << "  Введи літеру: ";
        string inp;
        if (!(cin >> inp)) {
            cin.clear();
            cin.ignore(1000, '\n');
            return MoveResult::OK;
        }

        if (inp == "0") return MoveResult::QUIT;

        if (inp == "H" || inp == "h") {
            char ch = hints.giveHint(secret, display, remaining);
            if (ch != '\0') {
                reveal(ch);
                used.insert(ch);
            }
            else {
                printColored("  Усі літери вже відкриті!\n", YELLOW_C);
            }
            system("pause");
            return MoveResult::OK;
        }

        // літеру до верхнього регістру
        char c = (char)cp1251ToUpper((unsigned char)inp[0]);

        if (used.count(c)) {
            printColored("  Цю літеру вже вводили!\n", RED_C);
            system("pause");
            return MoveResult::REPEAT;
        }

        used.insert(c);
        if (reveal(c)) {
            printColored("  Правильно!\n", GREEN_C);
        }
        else {
            --remaining;
            printColored("  Неправильно! Спроб залишилось: "
                + to_string(remaining) + "\n", RED_C);
        }
        system("pause");
        return MoveResult::OK;
    }

public:
    HangmanGame(const string& word, int attempts, bool ukr)
        : secret(word), remaining(attempts), ukrainian(ukr),
        hints(1), finalSec(0),
        startTime(chrono::steady_clock::now())
    {
        display = string(secret.size(), '_');
    }

    bool run() {
        while (remaining > 0 && !isWon()) {
            printBoard();
            MoveResult res = processInput();
            if (res == MoveResult::QUIT)
                throw runtime_error("QUIT");
        }

        finalSec = (int)chrono::duration_cast<chrono::seconds>(
            chrono::steady_clock::now() - startTime).count();

        system("cls");
        bool won = isWon();
        if (won) {
            printColored("   ПЕРЕМОГА! \n", GREEN_C);
            drawArt(remaining);
        }
        else {
            printColored("   ГРА ЗАКІНЧЕНА \n\n", RED_C);
            drawArt(0);
        }
        cout << "\n  Слово: ";
        printColored(secret + "\n", YELLOW_C);
        cout << "  Час:   ";
        printColored(to_string(finalSec) + "с\n", CYAN_C);
        if (hints.totalUsed())
            printColored("  Підказок використано: "
                + to_string(hints.totalUsed()) + "\n", MAGENTA_C);

        system("pause");
        return won;
    }

    int elapsed() const { return finalSec; }
};

// Вибір мови
int chooseLanguage() {
    system("cls");
    printColored("=== ВИБІР МОВИ / SELECT LANGUAGE ===\n\n", CYAN_C);
    printColored("  1. ", YELLOW_C); cout << "English\n";
    printColored("  2. ", YELLOW_C); cout << "Українська\n";
    cout << "\n  Вибір: ";
    int c = 0;
    while (!(cin >> c) || (c != 1 && c != 2)) {
        cin.clear(); cin.ignore(1000, '\n');
        cout << "  Введи 1 або 2: ";
    }
    return c;
}

// Головний контролер
class AppController {
    WordBank   bank;
    Statistics stats;

    void showMenu(int lang) const {
        system("cls");
        string label = (lang == 1) ? "English" : "Українська";
        printColored("=== HANGMAN PRO === [" + label + "]\n\n", MAGENTA_C);
        printColored("  1. ", YELLOW_C); cout << "Легко  (7 спроб)\n";
        printColored("  2. ", YELLOW_C); cout << "Важко  (5 спроб)\n";
        printColored("  3. ", YELLOW_C); cout << "Історія ігор\n";
        printColored("  4. ", CYAN_C);   cout << "Змінити мову\n";
        printColored("  5. ", RED_C);    cout << "Вихід\n";
        cout << "\n  Вибір: ";
    }

public:
    void start() {
        srand((unsigned)time(nullptr));
        int lang = chooseLanguage();
        int choice = 0;

        do {
            showMenu(lang);
            if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

            if (choice == 1 || choice == 2) {
                string word = bank.getWord(lang, choice);
                int attempts = (choice == 1) ? 7 : 5;
                bool ukr = (lang == 2);
                try {
                    HangmanGame game(word, attempts, ukr);
                    bool won = game.run();
                    stats.save(word, won, game.elapsed());
                }
                catch (const runtime_error&) {
                    printColored("  Гру перервано.\n", YELLOW_C);
                    system("pause");
                }
            }
            else if (choice == 3) {
                stats.show();
                system("pause");
            }
            else if (choice == 4) {
                lang = chooseLanguage();
            }

        } while (choice != 5);

        printColored("\n  До побачення!\n", CYAN_C);
    }
};

// main
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    AppController app;
    app.start();
    return 0;
}