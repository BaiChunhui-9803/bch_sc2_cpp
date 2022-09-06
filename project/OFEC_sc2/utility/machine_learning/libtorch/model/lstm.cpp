#include "lstm.h"

using namespace torch;

namespace ofec {
	extern Device g_device;

	LstmLinearImpl::LstmLinearImpl(int64_t input_size, int64_t hidden_size, int64_t output_size, int64_t num_layers) :
		m_hidden_size(hidden_size),
		m_num_layers(num_layers),
		m_lstm(nn::LSTMOptions(input_size, hidden_size).num_layers(num_layers).batch_first(true)),
		m_linear(hidden_size, output_size)
	{
		register_module("lstm", m_lstm);
		register_module("linear", m_linear);
	}

	Tensor LstmLinearImpl::forward(Tensor input) {
		auto h0 = ::zeros({ m_num_layers, input.size(0), m_hidden_size }).requires_grad_().to(g_device);
		auto c0 = ::zeros({ m_num_layers, input.size(0), m_hidden_size }).requires_grad_().to(g_device);
		auto opt = std::tuple<Tensor, Tensor>({ h0.detach(), c0.detach() });
		auto out = m_lstm->forward(input, opt);
		return m_linear->forward(std::get<0>(out));
	}
}