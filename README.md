# Hangman  (C++)

Console-based implementation of the classic Hangman game using **Object-Oriented Programming (OOP)** principles in C++. The game supports multiple languages, difficulty levels, and a hint system.

##  Features
* **Bilingual Support**: Play in English or Ukrainian.
* **Difficulty Levels**: 
    * **Easy**: 7 attempts, simpler words.
    * **Hard**: 5 attempts, complex vocabulary.
* **Hint System**: One free hint per game (penalty applies for further hints).
* **Statistics Tracking**: Saves game results (word, outcome, and time) to a local file.
* **Dynamic Word Bank**: Loads words from an external file or uses built-in defaults.
* **Visual ASCII Art**: Dynamic hangman drawing that changes color based on remaining attempts.

## Technical Details
* **Language**: C++
* **Principles**: OOP (Encapsulation, Classes, Exception Handling).
* **Encoding**: Windows-1251 (for Ukrainian Cyrillic support in the console).
* **Libraries**: `iostream`, `vector`, `string`, `chrono`, `fstream`, `windows.h`.

## Project Structure
* `project2.cpp`: Main source code containing the game logic and classes.
* `words.txt`: External dictionary file.
* `stats.txt`: Auto-generated file for game history.

##  How to Run
1.  Ensure you are using a Windows environment (due to `windows.h` and console color calls).
2.  Compile the source code using a C++ compiler (e.g., MinGW or Visual Studio):
    ```bash
    g++ project2.cpp -o Hangman.exe
    ```
3.  Make sure `words.txt` is in the same directory as the executable.
4.  Run `Hangman.exe`.

## 🧑‍💻 Developer
**ByteBloom357**
