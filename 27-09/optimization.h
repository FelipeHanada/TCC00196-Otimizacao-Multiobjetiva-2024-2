#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <vector>
#include <map>
#include <optional>
#include <type_traits>

template <typename SolutionClass>
class Evaluator;

class Solution {
private:
    mutable bool evaluated;
    mutable long long last_evaluation;
protected:
    bool is_evaluated() const;
    void set_evaluated(bool e) const;
    void clear_evaluation() const;

    long long get_last_evaluation() const;
    void set_evaluation(long long e) const;
public:
    Solution();

    template <typename SolutionClass>
    friend class Evaluator;

    template <typename SolutionClass>
    friend class Movement;
};

template <typename SolutionClass>
class Evaluator {
    static_assert(std::is_base_of<Solution, SolutionClass>::value, "SolutionClass must be a descendant of Solution");
protected:
    virtual long long evaluate(const SolutionClass &s) const = 0;
public:
    long long get_evaluation(const SolutionClass &s) const;
};

template <typename SolutionClass>
class Movement {
    static_assert(std::is_base_of<Solution, SolutionClass>::value, "SolutionClass must be a descendant of Solution");
public:
    virtual SolutionClass move(const SolutionClass &s) = 0;
};

template <typename SolutionClass>
class MovementGenerator {
    static_assert(std::is_base_of<Solution, SolutionClass>::value, "SolutionClass must be a descendant of Solution");
public:
    virtual std::vector<Movement<SolutionClass>*> generate(const SolutionClass &s) = 0;
};

#include "optimization.tpp"

#endif // OPTIMIZATION_H
