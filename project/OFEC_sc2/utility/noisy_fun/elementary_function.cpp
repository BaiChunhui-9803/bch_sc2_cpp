#include "elementary_function.h"

void OFEC::elementary_function_initialize(std::unique_ptr<elementary_function>& fun, const elementaryFunctionParameters& par)
{
	switch (par.m_type)
	{
	case OFEC::elementary_function::constant_fun:
		fun.reset(new constant_fun(par.m_from_x_range,par.m_to_x_range, par.m_to_y_range));
		break;
	case OFEC::elementary_function::random_fun:
		fun.reset(new random_fun(par.m_id_rnd, par.m_sample_num, par.m_from_x_range, par.m_to_x_range, par.m_to_y_range));
		break;
	case OFEC::elementary_function::cos_fun:
		fun.reset(new cos_fun(par.m_id_rnd, par.m_fun_num, par.m_from_x_range, par.m_to_x_range, par.m_to_y_range));
		break;
	default:
		break;
	}
}

void OFEC::elementary_function_initialize(std::unique_ptr<elementary_function>& fun, elementary_function::elementary_fun_type type, const std::pair<double, double>& from_x, const std::pair<double, double>& to_x, const std::pair<double, double>& to_y, int id_rnd, int fun_num, int sample_num)
{
	switch (type)
	{
	case OFEC::elementary_function::constant_fun:
		fun.reset(new constant_fun(from_x,to_x,to_y));
		break;
	case OFEC::elementary_function::random_fun:
		fun.reset(new random_fun(id_rnd,sample_num, from_x, to_x, to_y));
		break;
	case OFEC::elementary_function::cos_fun:
		fun.reset(new cos_fun(id_rnd,fun_num,from_x,to_x,to_y));
		break;
	default:
		break;
	}

}

void OFEC::elementary_function_initialize(std::unique_ptr<elementary_function>& fun, elementary_function::elementary_fun_type type, const ParamMap& v,int id_rnd)
{
	switch (type)
	{
	case OFEC::elementary_function::constant_fun:
		fun.reset(new constant_fun(v));
		break;
	case OFEC::elementary_function::random_fun:
		fun.reset(new random_fun(v,id_rnd));
		break;
	case OFEC::elementary_function::cos_fun:
		fun.reset(new cos_fun(v,id_rnd));
		break;
	default:
		break;
	}
}

bool OFEC::elementary_function_generator(std::unique_ptr<elementary_function>& fun, elementary_function::elementary_fun_type type, const std::pair<double, double>& from_x, const std::pair<double, double>& to_x, const std::pair<double, double>& to_y, int* id_rnd, int* fun_num, int* sample_num)
{

	switch (type)
	{
	case OFEC::elementary_function::constant_fun: {
		fun.reset(new constant_fun(from_x,to_x,to_y));
		return true;
	}

	case OFEC::elementary_function::random_fun:
	{
		if (id_rnd != nullptr && sample_num != nullptr) {
			fun.reset(new random_fun(*id_rnd,*sample_num, from_x, to_x, to_y));
			return true;
		}
		return false;
	}

	case OFEC::elementary_function::cos_fun: {
		if (id_rnd != nullptr && fun_num != nullptr) {
			fun.reset(new cos_fun(*id_rnd, *fun_num, from_x, to_x, to_y));
			return true;
		}
		return false;
	}
	default:
		return false;
	}

}


//void OFEC::elementary_function_paramenter(ParamMap& v, const std::pair<double, double>& from_x, const std::pair<double, double>& to_x, const std::pair<double, double>& to_y, int* fun_num, int* sample_num)
//{
//	v["from_x_down"] = from_x.first;
//	v["from_x_up"] = from_x.second;
//
//
//	v["to_x_down"] = to_x.first;
//	v["to_x_up"] = to_y.second;
//
//
//	v["to_y_down"] = to_y.first;
//	v["to_y_up"] = to_y.second;
//	if (sample_num != nullptr) {
//		v["sample_num"] = *sample_num;
//	}
//
//	if (fun_num != nullptr) {
//		v["fun_num"] = *fun_num;
//	}
//
//}

void OFEC::elementaryFunctionParameters::initialize(
	elementary_function::elementary_fun_type type, 
	const std::pair<double, double>& from_x,
	const std::pair<double, double>& to_x, 
	const std::pair<double, double>& to_y, 
	int id_rnd, int sample_num, int fun_num)
{
	m_type = type;
	m_from_x_range = from_x;
	m_to_x_range = to_x;
	m_to_y_range = to_y;
	m_id_rnd = id_rnd;
	m_sample_num = sample_num;
	m_fun_num = fun_num;
}
