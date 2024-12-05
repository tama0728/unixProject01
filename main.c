#include <stdio.h>
#include <string.h>

int main(void) {
    int length = 5, strike = 0, ball = 0;
    char* solution = "64382";
    char guess[length+1];

    for (int i = 1; i < length; ++i) {
        printf("Enter your guess: ");
        scanf("%s", guess);
        if (strcmp(solution, guess) == 0) {
            printf("You win!\n");
            return i;
        }
        for (int j = 0; j < length; ++j) {
            for (int k = 0; k < length; ++k) {
                if (solution[j] == guess[k]) {
                    if (j == k) {
                        strike++;
                    } else {
                        ball++;
                    }
                }
            }
        }
        printf("%d: Strike: %d, Ball: %d\n", i, strike, ball);
        strike = 0;
        ball = 0;
    }

    return 0;
}
