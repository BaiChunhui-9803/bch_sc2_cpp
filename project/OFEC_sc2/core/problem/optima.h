/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class Optima stores a set of optimal solutions for a given problem
*
*********************************************************************************/
#ifndef OFEC_OPTIMA_H
#define OFEC_OPTIMA_H

#include "encoding.h"
#include "../../utility/functional.h"

namespace ofec {

    template<typename T = VarVec<Real>>
    class Optima {
        using VarEnc = T;
    public:
        Optima() = default;

        void appendVar(const VarEnc &x) {
            m_var.push_back(x);
        }

        void appendObj(const std::vector<Real> &obj) {
            m_obj.push_back(obj);
        }

        void appendObj(const Real obj) {
            m_obj.push_back(std::vector<Real>(1, obj));
        }

        bool isVariableGiven() const {
            return m_variable_given;
        }

        bool isObjectiveGiven() const {
            return m_objective_given;
        }

        void resizeObjective(size_t n) {// reallocate memory for each objecitve
            for (auto &i : m_obj) {
                i.resize(n);
            }
        }

        void resizeVariableSet(size_t n) {
            m_var.resize(n);
        }

        void resizeObjectiveSet(size_t n) {
            m_obj.resize(n);
        }

        const VarEnc& variable(size_t i)const {
            return m_var[i];
        }

        VarEnc& variable(size_t i) {
            return m_var[i];
        }

        void setVariable(const VarEnc &x, size_t i) {
            m_var[i] = x;
        }

        void setObjective(const std::vector<Real> &obj, size_t i) {
            m_obj[i] = obj;
        }

        const std::vector<Real>& objective(size_t i = 0) const {
            return m_obj[i];
        }

        /* return IGD (Inverted Generational Distance) of popularion to the Pareto front
        R. Venkata Rao and Vivek Patel,
        "A multi-objective improved teaching-learning based optimization algorithm for unconstrained and constrained optimization problems,"
        International Journal of Industrial Engineering Computations 5 (2014) 1:C22
        */
        template<typename Population>
        Real invertGenDist(const Population &pop)const {
            Real distance = 0;
            for (auto &i : m_obj) {
                Real min_d = std::numeric_limits<Real>::max();
                for (int j = 0; j < pop.size(); ++j) {
                    Real d = euclideanDistance(pop[j].objective().begin(), pop[j].objective().end(), i.begin());
                    if (d<min_d)  min_d = d;
                }
                distance += min_d;
            }
            return distance / m_obj.size();
        }

        void clear() {
            m_var.clear();
            m_obj.clear();
            m_variable_given = false;
            m_objective_given = false;
        }

        size_t numberVariables() const {
            return m_var.size();
        }

        size_t numberObjectives() const {
            return m_obj.size();
        }

        bool in_PF(const std::vector<Real> &obj,const std::vector<OptMode> &mode) {
            for (auto &i : m_obj) {
                if (objectiveCompare(i, obj, mode) == Dominance::kDominant) return false;
            }
            return true;
        }

        void setVariableGiven(bool flag) {
            m_variable_given = flag;
        }

        void setObjectiveGiven(bool flag) {
            m_objective_given = flag;
        }

    private:
        std::vector<VarEnc> m_var;
        std::vector<std::vector<Real>> m_obj;
        bool m_variable_given = false, m_objective_given = false;
    };


}
#endif // !OFEC_OPTIMA_H