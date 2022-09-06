#ifndef OFEC_ELEMENT_FUNCTION_H
#define OFEC_ELEMENT_FUNCTION_H

#include <cmath>
#include <vector>
#include "../core/instance_manager.h"
#include <cstdio>
#include <string>
#include <iostream>
#include "cubic_spline/cubic_spline.h"
#include <utility>

namespace ofec {
	// map to [0,1]
	class  elementary_function {
	public:
		enum elementary_fun_type { constant_fun, random_fun, cos_fun };
	protected:
		std::pair<double, double> m_from_x_range = { 0,1 };
		std::pair<double, double> m_to_x_range = { 0,1 };
		std::pair<double, double> m_to_y_range = { 0,1 };
	public:

		elementary_function(const ParamMap& v) :
			m_from_x_range(std::get<double>(v.at("from_x_down")), std::get<double>(v.at("from_x_up"))),
			m_to_x_range(std::get<double>(v.at("to_x_down")), std::get<double>(v.at("to_x_up"))),
			m_to_y_range(std::get<double>(v.at("to_y_down")), std::get<double>(v.at("to_y_up")))
		{}

		elementary_function(const std::pair<double, double>& from_x_range, const std::pair<double, double>& to_x_range, const std::pair<double, double>& to_y_range)
			:m_from_x_range(from_x_range), m_to_x_range(to_x_range), m_to_y_range(to_y_range) {};

		virtual double get_value(double x) = 0;
		virtual void set_from_x_range(double x_from, double x_to) {
			m_from_x_range.first = x_from;
			m_from_x_range.second = x_to;
		}
		virtual void set_to_x_range(double x_from, double x_to) {
			m_to_x_range.first = x_from;
			m_to_x_range.second = x_to;
		}

		virtual void set_y_range(double y_from, double y_to) {
			m_to_y_range.first = y_from;
			m_to_y_range.second = y_to;
		}
		const std::pair<double, double>& get_to_y_range() {
			return m_to_y_range;
		}
		virtual void initialize() {};
	};

	class constant_fun :public elementary_function {
	protected:
		double m_mean_value;
	public:
		constant_fun(const ParamMap& v) :elementary_function(v) {}
		template<typename ... Args>
		constant_fun(Args&& ... args) : elementary_function(std::forward<Args>(args)...) {};

		virtual double get_value(double x)override {
			return m_mean_value;
		}
		virtual void initialize()override {
			m_mean_value = (m_to_y_range.first + m_to_y_range.second) / 2.0;
		};
	};


	class random_fun :public elementary_function {
		CubicSpline<double> m_fun;
		int m_id_rnd;
		//caller m_call;
		int m_sample_num;
		
	public:

		random_fun(const ParamMap& v,int id_rnd) :elementary_function(v), m_id_rnd(id_rnd),
			//m_sample_num(v.at("sample_num"))
			m_sample_num(std::get<int>(v.at("sample_num")))
		{}

		template<typename ... Args>
		random_fun(int id_rnd, int sample_num, Args&& ... args) : elementary_function(std::forward<Args>(args)...), m_id_rnd(id_rnd), m_sample_num(sample_num) {};


		virtual double get_value(double x)override {
			double val(m_fun.Interpolate_static_inte(x));
			if (val < m_to_y_range.first) val = m_to_y_range.first;
			else if (val > m_to_y_range.second)val = m_to_y_range.second;
			return val;
		}

		virtual void initialize()override {

			std::vector<double> x(m_sample_num);
			std::vector<double> y(m_sample_num);

			double x_offset(static_cast<double>(m_from_x_range.second - m_from_x_range.first) / static_cast<double>(m_sample_num));
			double from_x(m_from_x_range.first);
			for (int sample_id(0); sample_id < m_sample_num; ++sample_id) {
				x[sample_id] = from_x + x_offset * sample_id;
				y[sample_id] = GET_RND(m_id_rnd).uniform.nextNonStd<double>(m_to_y_range.first, m_to_y_range.second);
			}
			m_fun.Initialize(x, y);
			m_fun.set_interval(x_offset, from_x);
		};
	};


	class cos_fun :public elementary_function {
	protected:
		std::vector<double> m_cos_phi;
		std::vector<double> m_cos_scale;
		double m_omega = 1.0;
		double m_x_from = 1.0;
		std::pair<double, double> m_real_y;
		double m_scale_y;
		int m_id_rnd;
		int m_fun_num;

	protected:
		double ox2y(double x) {
			double y(0);
			for (int idx(0); idx < m_cos_phi.size(); ++idx) {
				y += sin(m_cos_phi[idx] + x) * m_cos_scale[idx];
			}
			return y;
		}

	public:
		cos_fun(const ParamMap& v,int id_rnd) :elementary_function(v), m_id_rnd(id_rnd), \
			m_fun_num(std::get<int>(v.at("fun_num"))) {}
		
		template<typename ... Args>
		cos_fun(int id_rnd, int fun_num, Args&& ... args) : elementary_function(std::forward<Args>(args)...), m_id_rnd(id_rnd), m_fun_num(fun_num) {};

		virtual double get_value(double x) override {
			double y(0);
			double to_x((x - m_from_x_range.first) * m_omega + m_to_x_range.first);
			for (int idx(0); idx < m_cos_phi.size(); ++idx) {
				y += sin(m_cos_phi[idx] + to_x) * m_cos_scale[idx];
			}
			if (y > m_real_y.second) y = m_real_y.second;
			else if (y < m_real_y.first) y = m_real_y.first;
			return y;

		}
		
		virtual void initialize() override {
			m_omega = (m_to_x_range.second - m_to_x_range.first) / (m_from_x_range.second - m_from_x_range.first);
			m_x_from = m_from_x_range.first;
			m_cos_phi.resize(m_fun_num);
			for (auto& it : m_cos_phi) {
				it = GET_RND(m_id_rnd).uniform.nextNonStd<double>(0, 2 * OFEC_PI);
			}
			m_cos_phi.front() = GET_RND(m_id_rnd).uniform.nextNonStd<double>(0, 2 * OFEC_PI);

			m_cos_scale.resize(m_fun_num);
			m_cos_scale[0] = 1.0;
			for (unsigned i(1); i < m_fun_num; ++i) {
				m_cos_scale[i] = m_cos_scale[i - 1] * 0.5;
			}

			{
				double x_offset(OFEC_PI / 30.);
				m_real_y.first = 10.;
				m_real_y.second = -10.;
				double cur_x(m_to_x_range.first);
				double cur_y(0);
				while (cur_x < m_to_x_range.second) {
					cur_y = ox2y(cur_x);
					m_real_y.first = std::min(m_real_y.first, cur_y);
					m_real_y.second = std::max(m_real_y.second, cur_y);
					cur_x += x_offset;
				}
				m_scale_y = (m_to_y_range.second - m_to_y_range.first) / (m_real_y.second - m_real_y.first);
			}
		}
	};
	struct elementaryFunctionParameters {
		elementary_function::elementary_fun_type m_type;
		std::pair<double, double> m_from_x_range = { 0,1 };
		std::pair<double, double> m_to_x_range = { 0,1 };
		std::pair<double, double> m_to_y_range = { 0,1 };
		int m_id_rnd;
		int m_sample_num;
		int m_fun_num;


		void initialize(elementary_function::elementary_fun_type type,
			const std::pair<double, double>& from_x,
			const std::pair<double, double>& to_x,
			const std::pair<double, double>& to_y,
			int id_rnd, int sample_num, int fun_num
		);
	};


	extern void elementary_function_initialize(std::unique_ptr<elementary_function>& fun,
		const elementaryFunctionParameters& par
	);

	extern void elementary_function_initialize(std::unique_ptr<elementary_function>& fun,
		elementary_function::elementary_fun_type type,
		const std::pair<double, double>& from_x, const std::pair<double, double>& to_x,
		const std::pair<double, double>& to_y,
		int id_rnd, int fun_num, int sample_num );

	extern void elementary_function_initialize(std::unique_ptr<elementary_function> & fun,elementary_function::elementary_fun_type type, const ParamMap& v,int rnd_id);
	
	extern bool elementary_function_generator(std::unique_ptr<elementary_function>& fun, elementary_function::elementary_fun_type type,
		const std::pair<double,double>&from_x,const std::pair<double,double>& to_x,
		const std::pair<double,double> & to_y,
		int* id_rnd=nullptr, int * fun_num=nullptr,int * sample_num=nullptr
	);


	


}

#endif