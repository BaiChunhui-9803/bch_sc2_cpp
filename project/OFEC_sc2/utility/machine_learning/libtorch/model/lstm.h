#ifndef OFEC_LSTM_H
#define OFEC_LSTM_H

#undef slots
#include "torch/torch.h"
#define slots Q_SLOTS

namespace ofec {
	class LstmLinearImpl : public torch::nn::Module {
	private:
		int64_t m_hidden_size;
		int64_t m_num_layers;
		torch::nn::LSTM m_lstm;
		torch::nn::Linear m_linear;

	public:
		LstmLinearImpl(int64_t input_size, int64_t hidden_size, int64_t output_size, int64_t num_layers);
		torch::Tensor forward(torch::Tensor input);
	};
	TORCH_MODULE(LstmLinear);
}

#endif // !OFEC_LSTM_H
