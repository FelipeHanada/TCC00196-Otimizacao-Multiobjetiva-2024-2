#include "knapsack.h"


KnapsackSolution::KnapsackSolution(int n)
    : Solution()
{
    this->n = n;
    this->w = 0;
    this->s = new bool[n];
    for (int i = 0; i < n; i++)
        this->s[i] = false;
}

Solution* KnapsackSolution::clone() const {
    KnapsackSolution *s = new KnapsackSolution(this->n);
    for (int i = 0; i < this->n; i++)
        s->s[i] = this->s[i];
    s->w = this->w;

    if (this->is_evaluated())
        s->set_evaluation(this->get_last_evaluation());
    
    return s;
}

size_t KnapsackSolution::size() const {
    return this->n;
}

bool KnapsackSolution::get(int i) const {
    return this->s[i];
}

void KnapsackSolution::set(int i, bool x, KnapsackEvaluator *evl) {
    if (this->s[i] == x) return;
    this->flip(i, evl);
}

void KnapsackSolution::flip(int i, KnapsackEvaluator *evl) {
    this->s[i] = !this->s[i];

    if (evl == nullptr || !this->is_evaluated()) {
        this->clear_evaluation();
        return;
    }

    long long delta_v = evl->v[i], delta_w = evl->w[i];
    if (!this->s[i]) {
        delta_v *= -1;
        delta_w *= -1;
    }

    this->w += delta_w;
    long long new_v = this->get_last_evaluation() + delta_v;

    this->set_evaluation(new_v);
}

KnapsackEvaluator::KnapsackEvaluator(int n, long long q, std::vector<int> v, std::vector<int> w) {
    this->n = n;
    this->q = q;
    this->v = v;
    this->w = w;
}

long long KnapsackEvaluator::evaluate(const KnapsackSolution *s) const {
    long long curr_weigh = 0;
    long long curr_value = 0;
    for (int i = 0; i < this->n; i++) {
        if (s->get(i)) {
            curr_value += this->v[i];
            curr_weigh += this->w[i];
        }
    }
    s->w = curr_weigh;
    return curr_value;
}

long long KnapsackEvaluator::get_evaluation(const KnapsackSolution *s) const {
    long long evaluation = Evaluator<KnapsackSolution>::get_evaluation(s);
    
    if (s->w > this->q) return KnapsackEvaluator::PUNISHMENT;
    else return evaluation;
}

KnapsackMovement::KnapsackMovement(KnapsackEvaluator *evl) : evl(evl) {}

KnapsackMovementGenerator::KnapsackMovementGenerator(KnapsackEvaluator *evl) {
    this->evl = evl;
}

Knapsack2FlipBitMovement::Knapsack2FlipBitMovement(KnapsackEvaluator *evl, int i, int j)
    : KnapsackMovement(evl)
{
    this->i = i;
    this->j = j;
}

void Knapsack2FlipBitMovement::move(KnapsackSolution *s) {
    if (this->i == this->j) {
        s->flip(this->i, this->evl);
    } else {
        s->flip(this->i, this->evl);
        s->flip(this->j, this->evl);
    }
}

long long Knapsack2FlipBitMovement::delta(const KnapsackSolution *s) const {
    long long a = this->evl->get_evaluation(s);
    
    long long delta_v = 0;
    long long total_w = s->w;

    delta_v += ((s->get(this->i)) ? -1 : 1) * this->evl->v[this->i] ;
    total_w += ((s->get(this->i)) ? -1 : 1) * this->evl->w[this->i];
    
    if (this->i != this->j) {
        delta_v += ((s->get(this->j)) ? -1 : 1) * this->evl->v[this->j];
        total_w += ((s->get(this->j)) ? -1 : 1) * this->evl->w[this->j];
    }

    if (total_w > this->evl->q) {
        return KnapsackEvaluator::PUNISHMENT;
    }

    return delta_v;
}

KnapsackIntervalFlipBitMovement::KnapsackIntervalFlipBitMovement(KnapsackEvaluator *evl, int i, int j)
    : KnapsackMovement(evl)
{
    this->i = i;
    this->j = j;
}

void KnapsackIntervalFlipBitMovement::move(KnapsackSolution *s) {
    for (int k = this->i; k <= this->j; k++)
        s->flip(k, this->evl);
}

long long KnapsackIntervalFlipBitMovement::delta(const KnapsackSolution *s) const {
    long long a = this->evl->get_evaluation(s);
    
    long long delta_v = 0;
    long long total_w = s->w;

    for (int k = this->i; k <= this->j; k++) {
        delta_v += ((s->get(k)) ? -1 : 1) * this->evl->v[k];
        total_w += ((s->get(k)) ? -1 : 1) * this->evl->w[k];
    }

    if (total_w > this->evl->q) {
        return KnapsackEvaluator::PUNISHMENT;
    }

    return delta_v;
}

KnapsackInversionMovement::KnapsackInversionMovement(KnapsackEvaluator *evl, int i, int j)
    : KnapsackMovement(evl)
{
    this->i = i;
    this->j = j;
}

void KnapsackInversionMovement::move(KnapsackSolution *s) {
    for (int k = 0; k < (this->j - this->i + 1) / 2; k++) {
        int a = this->i + k,
            b = this->j - k;
        s->flip(a, this->evl);
        s->flip(b, this->evl);
    }
}

long long KnapsackInversionMovement::delta(const KnapsackSolution *s) const {
    long long a = this->evl->get_evaluation(s);
    
    long long delta_v = 0;
    long long total_w = s->w;

    for (int k = 0; k < (this->j - this->i + 1) / 2; k++) {
        int a = this->i + k,
            b = this->j - k;
        delta_v += ((s->get(a)) ? -1 : 1) * this->evl->v[a];
        delta_v += ((s->get(b)) ? -1 : 1) * this->evl->v[b];
        total_w += ((s->get(a)) ? -1 : 1) * this->evl->w[a];
        total_w += ((s->get(b)) ? -1 : 1) * this->evl->w[b];
    }

    if (total_w > this->evl->q) {
        return KnapsackEvaluator::PUNISHMENT;
    }

    return delta_v;
}

Knapsack2FlipBitMovementGenerator::Knapsack2FlipBitMovementGenerator(KnapsackEvaluator *evl, int n)
    : KnapsackMovementGenerator(evl)
{
    this->n = n;
    this->curr_i = 0;
    this->curr_j = 0;
}

std::vector<Movement<KnapsackSolution>*> Knapsack2FlipBitMovementGenerator::get_all(const KnapsackSolution *s) {
    std::vector<Movement<KnapsackSolution>*> movements;
    for (int i = 0; i < this->n; i++) {
        for (int j = i; j < this->n; j++) {
            movements.push_back(new Knapsack2FlipBitMovement(this->evl, i, j));
        }
    }
    return movements;
}

Movement<KnapsackSolution>* Knapsack2FlipBitMovementGenerator::get_random() {
    int i = rand() % this->n;
    int j = rand() % this->n;
    return new Knapsack2FlipBitMovement(this->evl, i, j);
}

bool Knapsack2FlipBitMovementGenerator::has_next() {
    return this->curr_i < this->n;
}

Movement<KnapsackSolution>* Knapsack2FlipBitMovementGenerator::next() {
    if (this->curr_i == this->n) return nullptr;
    Knapsack2FlipBitMovement *m = new Knapsack2FlipBitMovement(this->evl, this->curr_i, this->curr_j);
    this->curr_j++;
    if (this->curr_j == this->n) {
        this->curr_i++;
        this->curr_j = this->curr_i;
    }
    return m;
}

void Knapsack2FlipBitMovementGenerator::reset() {
    this->curr_i = 0;
    this->curr_j = 0;
}

KnapsackIntervalFlipBitMovementGenerator::KnapsackIntervalFlipBitMovementGenerator(KnapsackEvaluator *evl, int n)
    : KnapsackMovementGenerator(evl)
{
    this->n = n;
    this->curr_i = 0;
    this->curr_j = 0;
}

std::vector<Movement<KnapsackSolution>*> KnapsackIntervalFlipBitMovementGenerator::get_all(const KnapsackSolution *s) {
    std::vector<Movement<KnapsackSolution>*> movements;
    for (int i = 0; i < this->n; i++) {
        for (int j = i; j < this->n; j++) {
            movements.push_back(new KnapsackIntervalFlipBitMovement(this->evl, i, j));
        }
    }
    return movements;
}

Movement<KnapsackSolution>* KnapsackIntervalFlipBitMovementGenerator::get_random() {
    int i = rand() % this->n;
    int j = rand() % this->n;
    return new KnapsackIntervalFlipBitMovement(this->evl, i, j);
}

bool KnapsackIntervalFlipBitMovementGenerator::has_next() {
    return this->curr_i < this->n;
}

Movement<KnapsackSolution>* KnapsackIntervalFlipBitMovementGenerator::next() {
    if (this->curr_i == this->n) return nullptr;
    KnapsackIntervalFlipBitMovement *m = new KnapsackIntervalFlipBitMovement(this->evl, this->curr_i, this->curr_j);
    this->curr_j++;
    if (this->curr_j == this->n) {
        this->curr_i++;
        this->curr_j = this->curr_i;
    }
    return m;
}

void KnapsackIntervalFlipBitMovementGenerator::reset() {
    this->curr_i = 0;
    this->curr_j = 0;
}

KnapsackInversionMovementGenerator::KnapsackInversionMovementGenerator(KnapsackEvaluator *evl, int n)
    : KnapsackMovementGenerator(evl)
{
    this->n = n;
    this->curr_i = 0;
    this->curr_j = 0;
}

std::vector<Movement<KnapsackSolution>*> KnapsackInversionMovementGenerator::get_all(const KnapsackSolution *s) {
    std::vector<Movement<KnapsackSolution>*> movements;
    for (int i = 0; i < this->n; i++) {
        for (int j = i; j < this->n; j++) {
            movements.push_back(new KnapsackInversionMovement(evl, i, j));
        }
    }
    return movements;
}

Movement<KnapsackSolution>* KnapsackInversionMovementGenerator::get_random() {
    int i = rand() % this->n;
    int j = rand() % this->n;
    return new KnapsackInversionMovement(this->evl, i, j);
}

bool KnapsackInversionMovementGenerator::has_next() {
    return this->curr_i < this->n;
}

Movement<KnapsackSolution>* KnapsackInversionMovementGenerator::next() {
    if (this->curr_i == this->n) return nullptr;
    KnapsackInversionMovement *m = new KnapsackInversionMovement(this->evl, this->curr_i, this->curr_j);
    this->curr_j++;
    if (this->curr_j == this->n) {
        this->curr_i++;
        this->curr_j = this->curr_i;
    }
    return m;
}

void KnapsackInversionMovementGenerator::reset() {
    this->curr_i = 0;
    this->curr_j = 0;
}

KnapsackSolution* cm_knapsack_greedy(const KnapsackEvaluator *evl, double t) {
    auto start = std::chrono::high_resolution_clock::now();

    long long curr_Q = evl->q;
    KnapsackSolution *s = new KnapsackSolution(evl->n);

    std::vector<int> c(evl->n);
    std::iota(c.begin(), c.end(), 0);
    std::sort(c.begin(), c.end(), [&](const int x, const int y) {
        float qx = evl->v[x] / (float) evl->w[x],
              qy = evl->v[y] / (float) evl->w[y];
        return qx > qy;
    });

    for (int g : c) {
        auto current = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration<double>(current - start).count() > t)
            break;

        if (evl->w[g] > curr_Q)
            continue;

        curr_Q -= evl->w[g];
        s->set(g, true);
    }

    return s;
}

KnapsackSolution* cm_knapsack_random(const KnapsackEvaluator *evl, double t) {
    auto start = std::chrono::high_resolution_clock::now();

    long long curr_Q = evl->q;
    KnapsackSolution *s = new KnapsackSolution(evl->n);

    std::vector<int> c_weight_ordered(evl->n);
    iota(c_weight_ordered.begin(), c_weight_ordered.end(), 0);
    sort(c_weight_ordered.begin(), c_weight_ordered.end(), [&](const int x, const int y) {
        return evl->w[x] < evl->w[y];
    });

    while (c_weight_ordered.size() > 0 && curr_Q < evl->w[(*c_weight_ordered.rbegin())]) {
        c_weight_ordered.pop_back();
    }

    while (c_weight_ordered.size() > 0 && curr_Q >= evl->w[c_weight_ordered[0]]) {
        auto current = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration<double>(current - start).count() > t)
            break;

        auto g = c_weight_ordered.begin() + (rand() % c_weight_ordered.size());
        s->set(*g, true);
        curr_Q -= evl->w[*g];
        c_weight_ordered.erase(g);

        while (c_weight_ordered.size() > 0 && curr_Q < evl->w[(*c_weight_ordered.rbegin())]) {
           c_weight_ordered.pop_back();
        }
    }

    return s;
}

KnapsackSolution* cm_knapsack_greedy_randomized(const KnapsackEvaluator *evl, float a, double t) {
    auto start = std::chrono::high_resolution_clock::now();

    KnapsackSolution *s = new KnapsackSolution(evl->n);
    long long curr_Q = evl->q;

    std::vector<float> values;
    for (int i = 0; i < evl->n; i++) {
        float value = evl->v[i] / (float) evl->w[i];
        values.push_back(value);
    }

    std::vector<int> c_weight_ordered(evl->n), c_value_ordered(evl->n);
    std::iota(c_weight_ordered.begin(), c_weight_ordered.end(), 0);
    std::iota(c_value_ordered.begin(), c_value_ordered.end(), 0);

    std::sort(c_weight_ordered.rbegin(), c_weight_ordered.rend(), [&](const int x, const int y) {
        return evl->w[x] > evl->w[y];
    });

    std::sort(c_value_ordered.rbegin(), c_value_ordered.rend(), [&](const int x, const int y) {
        return values[x] < values[y];
    });

    while (c_weight_ordered.size() > 0 && evl->w[*c_weight_ordered.rbegin()] > curr_Q) {
        c_weight_ordered.pop_back();
        c_value_ordered.erase(
            std::remove(c_value_ordered.begin(), c_value_ordered.end(), *c_weight_ordered.begin()),
            c_value_ordered.end()
        );
    }

    while (c_weight_ordered.size() > 0) {
        auto current = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration<double>(current - start).count() > t)
            break;

        float min_value = values[c_value_ordered.back()],
              max_value = values[c_value_ordered.front()];

        float threshold = max_value - a * (max_value - min_value);

        auto rc_end = std::lower_bound(
            c_value_ordered.begin(),
            c_value_ordered.end(),
            threshold,
            [&](const int x, const float threshold) {
                return values[x] >= threshold;
            }
        );

        size_t rc_size = std::distance(rc_end, c_value_ordered.end());
        int g = (rc_size == 0) ? c_value_ordered[0] : c_value_ordered[std::rand() % rc_size];

        curr_Q -= evl->w[g];
        s->set(g, true);

        c_weight_ordered.erase(std::remove(c_weight_ordered.begin(), c_weight_ordered.end(), g), c_weight_ordered.end());
        c_value_ordered.erase(std::remove(c_value_ordered.begin(), c_value_ordered.end(), g), c_value_ordered.end());

        while (c_weight_ordered.size() > 0 && evl->w[*c_weight_ordered.rbegin()] > curr_Q) {
            c_weight_ordered.pop_back();
            c_value_ordered.erase(
                remove(c_value_ordered.begin(), c_value_ordered.end(), *c_weight_ordered.begin()),
                c_value_ordered.end()
            );
        }
    }

    return s;
}
