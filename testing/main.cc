#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(std::time(nullptr)); // Seed random number generator
    int secret = std::rand() % 100 + 1; // Random number between 1 and 100
    int guess = 0;
    int attempts = 0;

    std::cout << "Welcome to the Number Guessing Game!\n";
    std::cout << "I'm thinking of a number between 1 and 100.\n";

    while (guess != secret) {
        std::cout << "Enter your guess: ";
        std::cin >> guess;
        attempts++;

        if (guess < secret) {
            std::cout << "Too low!\n";
        } else if (guess > secret) {
            std::cout << "Too high!\n";
        } else {
            std::cout << "Congratulations! You guessed the number in " << attempts << " attempts.\n";
        }
    }

    return 0;
}
