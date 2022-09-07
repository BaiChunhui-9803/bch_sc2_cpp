#include <vector>
#include <functional>
#include <cassert>
#include <sc2api/sc2_common.h> //! I want to use the random functions. They are easy to use and I love them!
#include <numeric>
#include "Solution.h"

template<class T>
class GA {
	// 种群 由多个Solution构成
	using Population = std::vector<Solution<T>>;
	// 算子
	using Evaluator = std::function<float(const std::vector<T>&)>;
	// 比较
	using Compare = std::function<bool(const Solution<T>&, const Solution<T>&)>;
protected:
    //! Settings
    // 最大代数
    int m_max_generation = 20;
    //! Controls how many parents ...
    //? this setting is OK, but the process is not right, since I shouldn't let the parents to generate a pair of just the same children, instead, I should pass them to another pair of parents
    //todo Here is a todo of modification for
    // 交叉概率
    float m_cross_over_rate = 1.f;
    //! Controls how many offspring solutions should be mutated
    // 变异概率
    float m_mutate_rate = 0.2f;
    //! Controls the population size after each selection
    // 种群大小
    int m_population_size = 50;
    //! Controls the ratio between the parents population and the offspring population in each reproduction
    // 繁殖比例
    float m_reproduce_rate = 1.f;

    Compare m_compare = Solution<T>::sum_greater;
    Evaluator default_evaluator = [](const std::vector<T> variables)->float { return 0; };
    std::vector<Evaluator*> eva_v = { &default_evaluator };
    std::vector<Evaluator*> m_evaluators = eva_v; // for easy to use

    //! Runtime data
    Population m_population;

public:
	// after you calling the default constructor, I hope you can call the Initialize()
	GA() = default;
	~GA() = default;
	void SetMaxGeneration(int max_generation) {
		m_max_generation = max_generation;
	}
    void SetCrossOverRate(int crossover_rate) {
        m_cross_over_rate = crossover_rate;
    }

    void SetMutateRate(int mutate_rate) {
        m_mutate_rate = mutate_rate;
    }

    virtual void SetPopulationSize(int population_size) {
        m_population_size = population_size;
        m_population.resize(m_population_size);
    }
    void SetReproduceRate(int reproduce_rate) {
        m_reproduce_rate = reproduce_rate;
    }
    void SetCompare(Compare compare) {
        m_compare = compare;
    }
    void SetEvaluator(Evaluator* evaluator) {
        m_evaluators = evaluator;
    }
    //! run the algorithm and return the final population to choose
    virtual Population Run();
protected:
    //! simply calls the GenerateSolution() repeatedly to generate original population
    virtual void GenerateSolutions(Population& pop, int size);
    //! two parents generate a unmutated soluton
    // 交叉过程
    virtual std::vector<Solution<T>> CrossOver(const Solution<T>& a, const Solution<T>& b);
    //virtual Solution<T> Mutate(const Solution<T>& s); //? mutate must be implemented by users
    //? pure virtual
    virtual void Mutate(Solution<T>& s) = 0;
    //! a population generate another population
    // The spring_size need to be smaller than the size of parents
    // 繁殖过程
    virtual void Reproduce(const Population& parents, Population& offspring, int spring_size);
    //! Just calls the EvaluateSingleSolution() repeatedly to evaluate a population
    // 评估过程
    virtual void Evaluate(Population& p);
    // 种群排序（不考虑多目标排序）
    //! According to the compare to sort the population, no multi-objective rank being considered
    virtual void SortSolutions(Population& p, const Compare& compare);

    //! two parents generate two children by crossover and mutation
    virtual std::vector<Solution<T>> Produce(const Solution<T>& a, const Solution<T>& b);
    //! Calls those evaluators to evaluate one solution
    // 调用这些算子来求一个解
    virtual void EvaluateSingleSolution(Solution<T>& solution);
    //! Generate one solution
    //? pure virtual
    virtual Solution<T> GenerateSolution() = 0;
};

template<class T>
inline std::vector<Solution<T>> GA<T>::Run() {
    m_population.resize(m_population_size);
    GenerateSolutions(m_population, m_population_size);
    Evaluate(m_population);
    SortSolutions(m_population, m_compare);
    for (size_t i = 0; i < m_max_generation; i++)
    {
        Population offspring;
        Reproduce(m_population, offspring, m_reproduce_rate * m_population_size);
        Evaluate(offspring);
        m_population.insert(m_population.begin(), offspring.begin(), offspring.end());
        SortSolutions(m_population, m_compare);
        m_population.erase(m_population.begin() + m_population_size, m_population.end()); //
    }
    return m_population;
}

template<class T>
inline void GA<T>::GenerateSolutions(Population& pop, int size) {
    for (size_t i = 0; i < size; i++) {
        pop[i] = GenerateSolution();
    }
}

template<class T>
inline std::vector<Solution<T>> GA<T>::CrossOver(const Solution<T>& a, const Solution<T>& b) {
    //! you can use #define NDBUG to disabled assert()
    assert(a.s_variable.size() > 0 && a.s_variable.size() == b.s_variable.size());
    std::vector<Solution<T>> offspring = { a,b };
    size_t start = sc2::GetRandomInteger(0, a.s_variable.size() - 1);
    size_t end = sc2::GetRandomInteger(0, a.s_variable.size() - 1);
    if (start > end) {
        std::swap(start, end);
    }
    for (size_t i = start; i < end; i++)
    {
        std::swap(offspring[0].s_variable[i], offspring[1].variable[i]);
    }
    return offspring;
}

template<class T>
inline std::vector<Solution<T>> GA<T>::Produce(const Solution<T>& a, const Solution<T>& b) {
    std::vector<Solution<T>> children;
    if (sc2::GetRandomFraction() < m_cross_over_rate) { //? it should't be here, instead, it should be outside
        children = CrossOver(a, b);
    }
    else {
        children = { a,b };
    }
    for (Solution<T>& c : children)
    {
        if (sc2::GetRandomFraction() < m_mutate_rate) {
            Mutate(c);
        }
    }
    return children;
}

template<class T>
inline void GA<T>::Reproduce(const Population& parents, Population& offspring, int spring_size) {
    assert(spring_size <= parents.size()); //! for now, I can not reproduce a larger offspring population
    offspring.resize(spring_size);
    for (size_t i = 0; i < spring_size; i += 2)
    {
        std::vector<Solution<T>> instant_children = Produce(parents[i], parents[i + 1]);
        offspring[i] = instant_children[0];
        if (i + 1 < spring_size) {
            offspring[i + 1] = instant_children[1];
        }
    }
}

template<class T>
inline void GA<T>::Evaluate(Population& p) {
    for (Solution<T>& s : p) {
        EvaluateSingleSolution(s);
    }
}

template<class T>
inline void GA<T>::SortSolutions(Population& p, const Compare& compare) {
    std::sort(p.begin(), p.end(), compare);
}


template<class T>
inline void GA<T>::EvaluateSingleSolution(Solution<T>& solution)
{
    assert(solution.s_objectives.size() == m_evaluators.size()); //? or I can add, but this will effect the performance, so you'd better set the settings properly before the run
    for (size_t i = 0; i < m_evaluators.size(); i++)
    {
        solution.s_objectives[i] = m_evaluators[i](solution.s_variable);
    }
}