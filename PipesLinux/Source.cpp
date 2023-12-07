#include <iostream>
#include <unistd.h>
#include <pthread.h>

const int NUM_PROCESSES = 10;
const int RANGE_SIZE = 1000;

// Structura pentru transmiterea datelor prin pipe
struct PipeData {
    int start;
    int end;
};

// Funcția pentru verificarea primarității unui număr
bool IsPrime(int num) {
    if (num < 2) {
        return false;
    }

    for (int i = 2; i <= num / 2; ++i) {
        if (num % i == 0) {
            return false;
        }
    }

    return true;
}

// Funcția executată de fiecare fir de execuție
void* FindPrimes(void* arg) {
    PipeData* pipeData = static_cast<PipeData*>(arg);

    for (int num = pipeData->start; num < pipeData->end; ++num) {
        if (IsPrime(num)) {
            // Afișează numărul prim
            std::cout << num << " ";
        }
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_PROCESSES];
    int pipes[NUM_PROCESSES][2]; // Pipe-urile pentru fiecare fir de execuție

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        // Creează un pipe pentru fiecare fir de execuție
        if (pipe(pipes[i]) == -1) {
            perror("Error creating pipe");
            return 1;
        }

        // Creează o structură pentru datele pipe-ului
        PipeData pipeData;
        pipeData.start = i * RANGE_SIZE;
        pipeData.end = (i + 1) * RANGE_SIZE;

        // Lansează un nou fir de execuție
        if (pthread_create(&threads[i], NULL, FindPrimes, &pipeData) != 0) {
            perror("Error creating thread");
            return 1;
        }
    }

    // Așteaptă ca toate firele de execuție să se încheie
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
