#ifndef Q_AHC_H
#define Q_AHC_H

#include <vector>
#include <list>
#include <utility>
#include <map>
#include <set>

namespace ofec {
	class Q_AHC {
		struct Cluster {
			int cluster_id;
			std::vector<int> member;
		};

		std::vector< std::pair<std::pair<int, int>, double >> m_Q;
		std::vector<size_t> m_seq_of_heap;
		std::vector<int> m_pos_in_seq;
		std::vector<double> m_a;
		int m_num_cluster;
		std::vector<Cluster> m_cluster;
	public:
		int cluster_size()const { return m_cluster.size(); }
		const std::vector<Cluster>& clusters()const { return m_cluster; }
		void initialize(const std::vector<std::vector<bool>>& adjmatrix);
		void clear();
		void build_heap();
		void erase_heap(size_t id);
		bool interpolate_heap(size_t id);
		void update_Q(size_t row, size_t column, size_t id, bool flag, const std::vector<std::vector<bool>>& adjmatrix);
		void update_heap(size_t max_index, const std::vector<std::vector<bool>>& adjmatrix);
		void clustering(const int& n, const std::vector<std::vector<bool>>& adjmatrix);
	};
}

#endif //!Q_AHC_H
