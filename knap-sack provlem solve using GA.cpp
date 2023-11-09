#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <string>
#include <random>
#include <ctime>

using namespace std;

struct item {
    int value;
    int weight;
};

bool isDuplicateRow(const vector<vector<int>>& matrix, const vector<int>& newRow) {
    for (const vector<int>& row : matrix) {
        if (row == newRow) {
            return true;
        }
    }
    return false;
}

void generateRandomRow(vector<int>& row, int columns) {
    row.clear();
    bool hasOne = false; // Flag to ensure at least one '1' in the row

    for (int i = 0; i < columns; ++i) {
        int randomValue = rand() % 2; // Generate random 0 or 1
        if (!hasOne && i == columns - 1) {
            // If we haven't added '1' yet and it's the last cell, add '1'
            row.push_back(1);
        } else {
            row.push_back(randomValue);
            if (randomValue == 1) {
                hasOne = true;
            }
        }
    }
}

int main() {
    srand(time(NULL));
    freopen("knapsack_input.txt", "rt", stdin);
    int cnt = 1;
    int testCases;
    cin >> testCases;
    while (testCases--) {
        int ksw; // knapsack weight
        cin >> ksw;
        int n;   // number of items
        cin >> n;
        vector<item> items(n);
        for (int i = 0; i < n; i++) {
            cin >> items[i].value >> items[i].weight;
        }

        // Genetic Algorithm Parameters
        int populationSize = 50;
        int maxGenerations = 100;
        double mutationRate = 0.1;

        vector<vector<int>> population;
        vector<int> bestSolution;
        int bestValue = 0;
        int bestWeight = 0;

        for (int i = 0; i < populationSize; i++) {
            vector<int> chromosome;
            generateRandomRow(chromosome, n);
            population.push_back(chromosome);
        }

        for (int generation = 0; generation < maxGenerations; generation++) {
            // Evaluate fitness and find the best solution in the current population
            map<string, int> fitnessCal;
            bestValue = 0;
            bestWeight = 0;

            for (int i = 0; i < populationSize; i++) {
                int totalValue = 0;
                int totalWeight = 0;
                for (int j = 0; j < n; j++) {
                    if (population[i][j] == 1) {
                        totalValue += items[j].value;
                        totalWeight += items[j].weight;
                    }
                }

                if (totalWeight <= ksw) {
                    string chromosome = "C" + to_string(i + 1);
                    fitnessCal[chromosome] = totalValue;
                }

                if (totalValue > bestValue) {
                    bestValue = totalValue;
                    bestWeight = totalWeight;
                    bestSolution = population[i];
                }
            }

            // Perform rank-based selection
            set<int, greater<>> sortingFitness;
            for (auto itr : fitnessCal)
                sortingFitness.insert(itr.second);

            float totalRank = 0;
            for (int i = 1; i <= sortingFitness.size(); i++) {
                totalRank += i;
            }

            vector<float> cumFit;
            float rank = 1;
            float normalization = 0;
            for (int i = 0; i < sortingFitness.size(); i++) {
                normalization += rank / totalRank;
                cumFit.push_back(normalization);
                rank++;
            }

            // Create a new population through crossover and mutation
            vector<vector<int>> newPopulation;

            for (int i = 0; i < populationSize; i++) {
                int parent1Index = 0;
                int parent2Index = 0;

                // Select parents using rank-based selection
                for (int j = 0; j < 2; j++) {
                    double randomNumber = double(rand()) / RAND_MAX;
                    for (int k = 0; k < cumFit.size(); k++) {
                        if (randomNumber < cumFit[k]) {
                            if (j == 0) parent1Index = k;
                            else parent2Index = k;
                            break;
                        }
                    }
                }

                // Perform one-point crossover
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<int> distribution(1, n - 1);
                int crossover_point = distribution(gen);

                vector<int> offspring;
                offspring.insert(offspring.end(), population[parent1Index].begin(), population[parent1Index].begin() + crossover_point);
                offspring.insert(offspring.end(), population[parent2Index].begin() + crossover_point, population[parent2Index].end());

                // Mutate offspring
                random_device rd2;
                mt19937 gen2(rd2());
                uniform_real_distribution<double> distribution2(0.0, 1.0);

                for (int j = 0; j < offspring.size(); j++) {
                    if (distribution2(gen2) < mutationRate) {
                        offspring[j] = 1 - offspring[j];
                    }
                }

                newPopulation.push_back(offspring);
            }

            population = newPopulation;
        }

        // Output the best solution for the current test case
        cout << "index: " << cnt << '\n'
             << "Number of selected items: " << bestSolution.size() << '\n'
             << "Total Value: " << bestValue << '\n'
             << "Total Weight: " << bestWeight << '\n'
             << "Item values and weights:\n";
        for (int i = 0; i < n; i++) {
            if (bestSolution[i] == 1) {
                cout << "Value: " << items[i].value << ", Weight: " << items[i].weight << '\n';
            }
        }
        cout << endl;
        cnt++;
    }
    return 0;
}
