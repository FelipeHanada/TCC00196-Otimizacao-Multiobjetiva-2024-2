#ifndef KNAPSACK_H
#define KNAPSACK_H

#include "optimization.h"
#include <vector>
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <climits>


class KnapsackSolution : public Solution {
public:
    int n;
    bool *s;
    KnapsackSolution(int n);
    KnapsackSolution(const KnapsackSolution &s);
    bool get(int i) const;
    void set(int i, bool x);
    void flip(int i);
};

class KnapsackEvaluator : public Evaluator<KnapsackSolution> {
public:
    int n;  // item quantity
    long long q;  // capacity
    std::vector<int> v;  // item values
    std::vector<int> w;  // item weights
    KnapsackEvaluator(int n, long long q, std::vector<int> v, std::vector<int> w);
    long long evaluate(const KnapsackSolution &s) override;
};

class KnapsackMovement : public Movement<KnapsackSolution> {
public:
    KnapsackEvaluator &evl;
    KnapsackMovement(KnapsackEvaluator &evl);
};

class Knapsack2FlipBitMovement : public KnapsackMovement {
    int i, j;
public:
    Knapsack2FlipBitMovement(KnapsackEvaluator &evl, int i, int j);
    KnapsackSolution move(const KnapsackSolution &s) override;
};

class KnapsackIntervalFlipBitMovement : public KnapsackMovement {
    int i, j;
public:
    KnapsackIntervalFlipBitMovement(KnapsackEvaluator &evl, int i, int j);
    KnapsackSolution move(const KnapsackSolution &s) override;
};

class KnapsackInversionMovement : public KnapsackMovement {
    int i, j;
public:
    KnapsackInversionMovement(KnapsackEvaluator &evl, int i, int j);
    KnapsackSolution move(const KnapsackSolution &s) override;
};

class KnapsackMovementGenerator : public MovementGenerator<KnapsackSolution> {
public:
    KnapsackEvaluator &evl;
    KnapsackMovementGenerator(KnapsackEvaluator &evl);
};

class Knapsack2FlipBitMovementGenerator : public KnapsackMovementGenerator {
    int n;
public:
    Knapsack2FlipBitMovementGenerator(KnapsackEvaluator &evl, int n);
    std::vector<Movement<KnapsackSolution>*> generate(const KnapsackSolution &s) override;
};

class KnapsackIntervalFlipBitMovementGenerator : public KnapsackMovementGenerator {
    int n;
public:
    KnapsackIntervalFlipBitMovementGenerator(KnapsackEvaluator &evl, int n);
    std::vector<Movement<KnapsackSolution>*> generate(const KnapsackSolution &s) override;
};

class KnapsackInversionMovementGenerator : public KnapsackMovementGenerator {
    int n;
public:
    KnapsackInversionMovementGenerator(KnapsackEvaluator &evl, int n);
    std::vector<Movement<KnapsackSolution>*> generate(const KnapsackSolution &s) override;
};

KnapsackSolution cm_knapsack_greedy(const KnapsackEvaluator &evl);

KnapsackSolution cm_knapsack_random(const KnapsackEvaluator &evl, float t);

KnapsackSolution cm_knapsack_greedy_randomized(const KnapsackEvaluator &evl, float t, float a);

#endif // KNAPSACK_H
