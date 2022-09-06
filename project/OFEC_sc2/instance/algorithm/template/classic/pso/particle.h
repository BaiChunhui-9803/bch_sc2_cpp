/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 21 Sep. 2011 by Changhe Li
// Updated: 12 Dec. 2014 by Changhe Li
// Updated: 15 Mar. 2018 by Junchen Wang (wangjunchen@cug.edu.cn)
// Updated: 25.Aug. 2019 Changhe Li

#ifndef PARTICLE_H
#define PARTICLE_H

#include "../../../../../core/algorithm/individual.h"

namespace ofec {
	class Particle : public Individual<> {
	protected:
		Solution<> m_pbest;
		std::vector<Real> m_vel;
	public:
		virtual ~Particle() {}
		Particle(size_t num_obj, size_t num_con, size_t size_var);
		Particle(const Particle &rhs) = default;
        Particle(Particle &&rhs) noexcept = default;
		Particle(const Solution<>& rhs);
        Particle& operator=(const Particle &other) = default;
        Particle& operator=(Particle &&other) noexcept = default;
		Particle& operator=(const Solution<>& other);

		std::vector<Real>& velocity() { return m_vel; }
		Solution<>& pbest() { return m_pbest; }
		virtual void initVelocity(int id_pro, int id_rnd);
		virtual void initializeVmax(int id_pro, int id_rnd){}
		void initVelocity(const std::vector<std::pair<Real, Real>> &vrange, int id_rnd);
		void initVelocity(Real min, Real max, int id_rnd);
		virtual void nextVelocity(const Solution<> *lbest, Real w, Real c1, Real c2, int id_rnd);
		virtual void clampVelocity(int id_pro, int id_rnd); // set to zero by default if the particle goes out the space
		virtual void move();
		void resizeVar(size_t n);

		Real speed() const;
		const std::vector<Real>& velocity() const { return m_vel; }
		const Solution<>& pbest() const { return m_pbest; }
		const Solution<>& phenotype() const override { return m_pbest; }
	};
}

#endif // !PARTICLE_H

