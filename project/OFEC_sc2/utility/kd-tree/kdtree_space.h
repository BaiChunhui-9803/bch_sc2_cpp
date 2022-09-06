//*****************************************************************************************************************************************

//part of the source code is from nanoflann lib at https://github.com/jlblancoc/nanoflann

#ifndef  NANOFLANN_HPP_
#define  NANOFLANN_HPP_

#include <vector>
#include <list>
#include <set>
#include <cassert>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <utility>
#include <iostream>
#include "../../core/definition.h"
#include <map>
#include <memory>

namespace nanoflann
{
	/** @addtogroup memalloc_grp Memory allocation
	* @{ */

	/**
	* Allocates (using C's malloc) a generic type T.
	*
	* Params:
	*     count = number of instances to allocate.
	* Returns: pointer (of type T*) to memory buffer
	*/
	template <typename T>
	inline T* allocate(size_t count = 1)
	{
		T* mem = static_cast<T*>(::malloc(sizeof(T) * count));
		return mem;
	}


	/**
	* Pooled storage allocator
	*
	* The following routines allow for the efficient allocation of storage in
	* small chunks from a specified pool.  Rather than allowing each structure
	* to be freed individually, an entire pool of storage is freed at once.
	* This method has two advantages over just using malloc() and free().  First,
	* it is far more efficient for allocating small objects, as there is
	* no overhead for remembering all the information needed to free each
	* object or consolidating fragmented memory.  Second, the decision about
	* how long to keep an object is made at the time of allocation, and there
	* is no need to track down all the objects to free them.
	*
	*/

	const size_t     WORDSIZE = 16;
	const size_t     BLOCKSIZE = 8192;

	class PooledAllocator
	{
		/* We maintain memory alignment to word boundaries by requiring that all
		allocations be in multiples of the machine wordsize.  */
		/* Size of machine word in bytes.  Must be power of 2. */
		/* Minimum number of bytes requested at a time from	the system.  Must be multiple of WORDSIZE. */


		size_t remaining;  /* Number of bytes left in current block of storage. */
		void* base;     /* Pointer to base of current block of storage. */
		void* loc;      /* Current location in block to next allocate memory. */

		void internalInit()
		{
			remaining = 0;
			base = NULL;
			used_memory = 0;
			wasted_memory = 0;
		}

	public:
		size_t  used_memory;
		size_t  wasted_memory;

		/**
		Default constructor. Initializes a new pool.
		*/
		PooledAllocator() {
			internalInit();
		}

		/**
		* Destructor. Frees all the memory allocated in this pool.
		*/
		~PooledAllocator() {
			freeAll();
		}

		/** Frees all allocated memory chunks */
		void freeAll()
		{
			while (base != NULL) {
				void* prev = *(static_cast<void**>(base)); /* Get pointer to prev block. */
				::free(base);
				base = prev;
			}
			internalInit();
		}

		/**
		* Returns a pointer to a piece of new memory of the given size in bytes
		* allocated from the pool.
		*/
		void* malloc(const size_t req_size)
		{
			/* Round size up to a multiple of wordsize.  The following expression
			only works for WORDSIZE that is a power of 2, by masking last bits of
			incremented size to zero.
			*/
			const size_t size = (req_size + (WORDSIZE - 1)) & ~(WORDSIZE - 1);

			/* Check whether a new block must be allocated.  Note that the first word
			of a block is reserved for a pointer to the previous block.
			*/
			if (size > remaining) {

				wasted_memory += remaining;

				/* Allocate new storage. */
				const size_t blocksize = (size + sizeof(void*) + (WORDSIZE - 1) > BLOCKSIZE) ?
					size + sizeof(void*) + (WORDSIZE - 1) : BLOCKSIZE;

				// use the standard C malloc to allocate memory
				void* m = ::malloc(blocksize);
				if (!m) {
					fprintf(stderr, "Failed to allocate memory.\n");
					return NULL;
				}

				/* Fill first word of new block with pointer to previous block. */
				static_cast<void**>(m)[0] = base;
				base = m;

				size_t shift = 0;
				//int size_t = (WORDSIZE - ( (((size_t)m) + sizeof(void*)) & (WORDSIZE-1))) & (WORDSIZE-1);

				remaining = blocksize - sizeof(void*) - shift;
				loc = (static_cast<char*>(m) + sizeof(void*) + shift);
			}
			void* rloc = loc;
			loc = static_cast<char*>(loc) + size;
			remaining -= size;

			used_memory += size;

			return rloc;
		}

		/**
		* Allocates (using this pool) a generic type T.
		*
		* Params:
		*     count = number of instances to allocate.
		* Returns: pointer (of type T*) to memory buffer
		*/
		template <typename T>
		T* allocate(const size_t count = 1)
		{
			T* mem = static_cast<T*>(this->malloc(sizeof(T) * count));
			return mem;
		}

	};
	/** @} */

	template <typename ElementType, typename IndexType = size_t>
	class PartitioningKDTree
	{
	private:
		/** Hidden copy constructor, to disallow copying indices (Not implemented) */
		PartitioningKDTree(const PartitioningKDTree<ElementType, IndexType>&) = delete;
	protected:

		/**
		*  Array of indices to vectors in the dataset.
		*/
		std::vector<IndexType> m_vind;

		/**
		* The dataset used by this index
		*/
		std::vector<std::vector<ElementType>> m_pointdata; //!< The source of our data
		std::vector<ElementType>m_ratiodata;

		size_t m_size; //!< Number of current points in the dataset
		size_t m_dim;  //!< Dimensionality of each data point
		size_t m_relative_depth = 0;

		/*--------------------- Internal Data Structures --------------------------*/
		struct BoundingBox {
			std::vector<std::pair<ElementType, ElementType>> box;
			ofec::Real rat = 1.0, volume;
			size_t depth = 1;
		};

		struct Node
		{
			/** Union used because a node can be either a LEAF node or a non-leaf node, so both data fields are never used simultaneously */
			union {
				struct {
					IndexType    idx_region;
					BoundingBox* bbox;
				}lr;
				struct {
					int          divfeat; //!< Dimension used for subdivision.
					ElementType pivot; // pivot value for division
					IndexType    idx_sample;		// index
					ElementType low, high;	//boundary for the box to be cutted.
				}sub;
			};
			size_t depth;
			Node* child1, * child2, * parent;  //!< Child nodes (both=NULL mean its a leaf node)
		};
		typedef Node* NodePtr;

		/** The KD-tree used to find regions */
		NodePtr m_root;

		BoundingBox m_rootBbox;
		std::vector<std::unique_ptr<BoundingBox>> m_regions;
		std::set<IndexType> m_shelvedIndices;
		std::list<NodePtr> m_shelvedNodes;

		/**
		* Pooled memory allocator.
		*
		* Using a pooled memory allocator is more efficient
		* than allocating memory directly when there is a large
		* number small of memory allocations.
		*/
		PooledAllocator m_pool;
		ofec::Real m_lrat = 0, m_srat = 1;
		int m_lbox = 0, m_sbox = 0;
		int m_mode;
	public:

		//constructor based on empty data 
		PartitioningKDTree(int dim, int mode) : m_root(NULL), m_dim(dim), m_mode(mode) {}

		//constructor based on the pointdata
		PartitioningKDTree(int dimensionality, const std::vector<std::vector<ElementType>>& inputData, const std::vector<std::pair<ElementType, ElementType>>& initBBox) :
			m_pointdata(inputData), m_root(NULL), m_mode(1)//???
		{
			m_size = inputData.size();
			m_dim = dimensionality;
			m_rootBbox.box = initBBox;
			// Create a permutable array of indices to the input vectors.
			initVindPoint();
		}

		PartitioningKDTree(int dimensionality, const std::vector<std::vector<ElementType>>& inputData) :
			m_pointdata(inputData), m_root(NULL), m_mode(1)
		{
			m_size = inputData.size();
			m_dim = dimensionality;

			// Create a permutable array of indices to the input vectors.
			initVindPoint();
		}

		//constructor based on the ratio
		PartitioningKDTree(const int dimensionality, const std::vector<ElementType>& inputData, const std::vector<std::pair<ElementType, ElementType>>& initBBox) :
			m_ratiodata(inputData), m_root(NULL), m_mode(2)
		{
			m_size = inputData.size();
			m_dim = dimensionality;
			m_rootBbox.box = initBBox;
			initVindRatio();
		}

		PartitioningKDTree(const int dimensionality, const std::vector<ElementType>& inputData) :
			m_ratiodata(inputData), m_root(NULL), m_mode(2)
		{
			m_size = inputData.size();
			m_dim = dimensionality;
			initVindRatio();
		}

		void inputRatioData(const std::vector<ElementType>& inputData) {
			m_ratiodata = inputData;
			m_size = inputData.size();
			initVindRatio();
		}

		void inputRatioData(std::vector<ElementType>&& inputData) {
			m_ratiodata = inputData;
			m_size = inputData.size();
			initVindRatio();
		}

		void inputPointData(const std::vector<std::vector<ElementType>>& inputData) {
			m_pointdata = inputData;
			m_size = inputData.size();
			initVindPoint();
		}

		void setInitBox(const std::vector<std::pair<ElementType, ElementType>>& initBBox) {
			m_rootBbox.box = initBBox;
		}

		std::vector<IndexType> regionsIndices() const {
			std::vector<IndexType> idxs;
			for (auto i = 0; i < m_regions.size(); ++i)
				if (m_shelvedIndices.count(i) == 0)
					idxs.push_back(i);
			return idxs;
		}

		//Printout the idxBoxes
		void regionShow() {
			for (auto i = 0; i < m_regions.size(); ++i) {
				if (m_shelvedIndices.count(i) == 0) {
					for (auto j = 0; j < m_dim; ++j) {
						std::cout << "(" << m_regions[i]->box[j].first << " , " << m_regions[i]->box[j].second << ") ";
					}
					std::cout << std::endl;
				}
			}
		}

		/** Standard destructor */
		~PartitioningKDTree() { }

		/** Frees the previously-built index. Automatically called within buildIndex(). */
		void freeIndex()
		{
			m_pool.freeAll();
			m_root = NULL;
			m_regions.clear();
		}

		/**
		* Builds the index
		*/
		void buildIndex()
		{
			freeIndex();
			if (m_mode == 1) // randomly constr
			{
				initVindPoint();
				m_root = divideTree(0, m_size, m_rootBbox);   // construct the tree
			}
			else
			{
				initVindRatio();
				m_root = ratioDivideTree(0, m_size, m_rootBbox);
			}
		}

		/** Returns number of leaf nodes  */
		size_t size() const {
			if (m_mode == 1) return m_size + 1;
			else if (m_mode == 2) 	return m_size;
		}

		/** Returns the length of each point in the dataset */
		size_t veclen() const {
			return m_dim;
		}

		/**
		* Computes the inde memory usage
		* Returns: memory used by the index
		*/
		size_t usedMemory() const
		{
			return m_pool.used_memory + m_pool.wasted_memory + m_pointdata.size() * sizeof(IndexType);  // pool memory and vind array memory
		}

		int getRegionIdx(const std::vector<ElementType>& p) const {
			return enqury(p, m_root);
		}

		const BoundingBox& getRootBox() const {
			return m_rootBbox;
		}
		int smallestBox() const { return m_sbox; }
		int largestBox() const { return m_lbox; }
		const std::vector<std::pair<ElementType, ElementType>>& getBox(IndexType idx) const {
			return m_regions[idx]->box;
		}
		ofec::Real getBoxVolume(IndexType idx) const {
			return m_regions[idx]->volume;
		}
		size_t getDepth(IndexType idx) const {
			return m_regions[idx]->depth;
		}

		void setRelativeDepth(size_t depth) {
			m_relative_depth = depth;
		}

		int splitRegion(IndexType idx, int* dim = nullptr, ElementType* split_pivot = nullptr) {
			NodePtr node = NULL;
			leafNode(idx, m_root, node);
			if (node == NULL) return -1;
			NodePtr node1, node2;
			if (!m_shelvedNodes.empty()) {
				node1 = m_shelvedNodes.front();
				m_shelvedNodes.pop_front();
			}
			else
				node1 = m_pool.allocate<Node>();
			if (!m_shelvedNodes.empty()) {
				node2 = m_shelvedNodes.front();
				m_shelvedNodes.pop_front();
			}
			else
				node2 = m_pool.allocate<Node>();
			node1->depth = node->depth + 1;
			node2->depth = node->depth + 1;
			node1->parent = node;
			node2->parent = node;
			node1->child1 = node1->child2 = NULL;
			node2->child1 = node2->child2 = NULL;
			node->child1 = node1;
			node->child2 = node2;
			if (dim != nullptr)
				node->sub.divfeat = *dim;
			else
				node->sub.divfeat = node->depth % m_dim;
			node->sub.low = m_regions[idx]->box[node->sub.divfeat].first;
			node->sub.high = m_regions[idx]->box[node->sub.divfeat].second;
			if (!split_pivot || *split_pivot < node->sub.low || *split_pivot > node->sub.high)
				node->sub.pivot = (node->sub.low + node->sub.high) / 2;
			else
				node->sub.pivot = *split_pivot;
			node1->lr.idx_region = idx;
			m_regions[node1->lr.idx_region]->depth++;
			if (m_shelvedIndices.empty()) {
				node2->lr.idx_region = m_regions.size();
				m_regions.emplace_back(new BoundingBox(*m_regions[node1->lr.idx_region]));
			}
			else {
				node2->lr.idx_region = *m_shelvedIndices.begin();
				m_shelvedIndices.erase(m_shelvedIndices.begin());	
				m_regions[node2->lr.idx_region]->depth = m_regions[node1->lr.idx_region]->depth;
			}
			m_regions[node1->lr.idx_region]->box[node->sub.divfeat].second = node->sub.pivot;
			m_regions[node2->lr.idx_region]->box[node->sub.divfeat].first = node->sub.pivot;
			node1->lr.bbox = m_regions[node1->lr.idx_region].get();
			node2->lr.bbox = m_regions[node2->lr.idx_region].get();
			auto pre_vol = m_regions[node1->lr.idx_region]->volume;
			m_regions[node1->lr.idx_region]->volume = pre_vol * ((node->sub.pivot - node->sub.low) / (node->sub.high - node->sub.low));
			m_regions[node2->lr.idx_region]->volume = pre_vol - m_regions[node1->lr.idx_region]->volume;
			m_size++;
			return node2->lr.idx_region;
		}

		void mergeRegion(IndexType idx, std::list<IndexType>& merged_regs) {      // Merge the region and its sibling region
			NodePtr node = NULL;
			leafNode(idx, m_root, node);
			if (node == NULL || node == m_root) return;
			mergeNode(node, merged_regs);
		}

		void unionsAtDepth(size_t depth, std::vector<std::vector<IndexType>>& region_unions) const {
			if (!region_unions.empty()) region_unions.clear();
			findUnionsAtDepth(depth, m_root, region_unions);
		}

		void findNeighbor(IndexType idx, std::list<IndexType>& neighbors) const {
			if (!neighbors.empty()) neighbors.clear();
			neighborCheck(idx, m_root, neighbors);
		}

		bool checkAdjacency(int idx1, int idx2) const {
			bool result = true;
			const auto& box1 = m_regions[idx1]->box;
			const auto& box2 = m_regions[idx2]->box;
			for (size_t j = 0; j < m_dim; ++j) {
				if (box1[j].second < box2[j].first || box2[j].second < box1[j].first) {
					result = false;
					break;
				}
			}
			return result;
		}

		void addSubtree(IndexType idx, PartitioningKDTree& subtree, std::map<IndexType, IndexType>& add_id) {
			NodePtr node = NULL;
			leafNode(idx, m_root, node);
			if (node == NULL)	return;
			if (node->lr.bbox->box != subtree.m_rootBbox.box)	return;
			auto root = subtree.m_root;
			if (root->child1 == NULL && root->child2 == NULL) return;
			m_shelvedIndices.insert(idx);
			m_size--;
			copyLeafNodes(node, root, add_id);
		}


	private:
		/** Make sure the auxiliary list \a vind has the same size than the current dataset, and re-generate if size has changed. */
		void initVindPoint()
		{
			// Create a permutable array of indices to the input vectors.
			m_size = m_pointdata.size();
			if (m_vind.size() != m_size) m_vind.resize(m_size);
			size_t k = 0;
			for (auto& i : m_vind) i = k++;
		}

		/** Make sure the auxiliary list \a vind has the same size than the current ratiodata, and re-generate if size has changed. */
		void initVindRatio()
		{
			// Create a permutable array of indices to the input vectors.
			m_size = m_ratiodata.size();
			if (m_vind.size() != m_size) m_vind.resize(m_size);
			size_t k = 0;
			for (auto& i : m_vind) i = k++;
		}

		/// Helper accessor to the dataset points:
		ElementType getDataset(size_t idx, int component) const {
			return m_pointdata[idx][component];
		}

		/// Helper accessor to the ratiodata:
		ElementType getRatiodata(size_t idx) const {
			return m_ratiodata[idx];
		}


		/**
		* Create a tree node that subdivides the list of vecs from vind[first]
		* to vind[last].  The routine is called recursively on each sublist.
		*
		* @param left index of the first vector
		* @param right index of the last vector
		*/
		NodePtr divideTree(const IndexType left, const IndexType right, BoundingBox& bbox, int depth = 0)
		{
			NodePtr node = m_pool.allocate<Node>(); // allocate memory

			/*a leaf node,create a sub-region. */
			if ((right - left) <= 0) {
				node->child1 = node->child2 = NULL;    /* Mark as leaf node. */
				node->lr.idx_region = m_regions.size();
				node->depth = depth;
				m_regions.emplace_back(new BoundingBox(bbox));
				boxRatio(*m_regions.back(), m_regions.size() - 1);
			}
			else {
				IndexType idx;
				int cutfeat;
				ElementType cutval;
				middleSplit_(&m_vind[0] + left, right - left, idx, cutfeat, cutval, bbox, depth);

				node->sub.idx_sample = m_vind[left + idx];
				node->sub.divfeat = cutfeat;
				node->sub.low = bbox.box[cutfeat].first;
				node->sub.high = bbox.box[cutfeat].second;
				node->depth = depth;
				BoundingBox left_bbox(bbox);
				left_bbox.box[cutfeat].second = cutval;
				//node->child1 = divideTree(left, left + idx, left_bbox, depth + 1);
				NodePtr temp = divideTree(left, left + idx, left_bbox, depth + 1);
				node->child1 = temp;
				temp->parent = node;

				BoundingBox right_bbox(bbox);
				right_bbox.box[cutfeat].first = cutval;
				//node->child2 = divideTree(left + idx + 1, right, right_bbox, depth + 1);
				temp = divideTree(left + idx + 1, right, right_bbox, depth + 1);
				node->child2 = temp;
				temp->parent = node;

				node->sub.pivot = cutval;
			}
			return node;
		}


		NodePtr ratioDivideTree(const IndexType left, const IndexType right, BoundingBox& bbox, int depth = 0)
		{
			NodePtr node = m_pool.allocate<Node>(); // allocate memory
			if ((right - left) <= 1)
			{
				node->child1 = node->child2 = NULL;    /* Mark as leaf node. */
				node->lr.idx_region = m_regions.size();
				node->depth = depth;
				m_regions.emplace_back(new BoundingBox(bbox));                //�����ά����������
				boxRatio(*m_regions.back(), m_regions.size() - 1);//region.back():����region����ĩβԪ�ص�����		
				node->lr.bbox = m_regions.back().get();
			}
			else {
				IndexType idx;
				int cutfeat;
				ElementType cutval;
				midSplit(&m_vind[0] + left, right - left, idx, cutfeat, cutval, bbox, depth);

				node->sub.idx_sample = m_vind[idx];//????
				node->sub.divfeat = cutfeat;
				node->sub.low = bbox.box[cutfeat].first;
				node->sub.high = bbox.box[cutfeat].second;
				node->depth = depth;

				BoundingBox left_bbox(bbox);
				left_bbox.box[cutfeat].second = cutval;
				left_bbox.depth = depth + 1;
				NodePtr temp = ratioDivideTree(left, idx, left_bbox, depth + 1);
				node->child1 = temp;
				temp->parent = node;
				//node->child1 = ratioDivideTree(left, idx, left_bbox, depth + 1);

				BoundingBox right_bbox(bbox);
				right_bbox.box[cutfeat].first = cutval;
				right_bbox.depth = depth + 1;
				temp = ratioDivideTree(idx, right, right_bbox, depth + 1);
				node->child2 = temp;
				temp->parent = node;
				//node->child2 = ratioDivideTree(idx, right, right_bbox, depth + 1);
				node->sub.pivot = cutval;
			}
			return node;
		}


		void computeMinMax(IndexType* ind, IndexType count, int element, ElementType& min_elem, ElementType& max_elem)
		{
			min_elem = getDataset(ind[0], element);
			max_elem = getDataset(ind[0], element);
			for (IndexType i = 1; i < count; ++i) {
				ElementType val = getDataset(ind[i], element);
				if (val < min_elem) min_elem = val;
				if (val > max_elem) max_elem = val;
			}
		}

		void middleSplit_(IndexType* ind, IndexType count, IndexType& index, int& cutfeat, ElementType& cutval, const BoundingBox& bbox, int depth)
		{

			cutfeat = depth % m_dim;
			// for a balanced kd-tree, split in the median value
			std::vector<IndexType> cur_idx(count);
			for (IndexType i = 0; i < count; ++i) {
				cur_idx[i] = ind[i];
			}
			std::nth_element(cur_idx.begin(), cur_idx.begin() + cur_idx.size() / 2, cur_idx.end(), [this, &cutfeat](const IndexType a, const IndexType b) {
				return this->getDataset(a, cutfeat) < this->getDataset(b, cutfeat);
				});
			ElementType split_val = getDataset(cur_idx[cur_idx.size() / 2], cutfeat);
			//.....

			ElementType min_elem, max_elem;
			computeMinMax(ind, count, cutfeat, min_elem, max_elem);

			if (split_val < min_elem) cutval = min_elem;
			else if (split_val > max_elem) cutval = max_elem;
			else cutval = split_val;

			IndexType lim1, lim2;
			planeSplit(ind, count, cutfeat, cutval, lim1, lim2);

			if (lim1 > count / 2) index = lim1;
			else if (lim2 < count / 2) index = lim2;
			else index = count / 2;
		}

		void midSplit(IndexType* ind, IndexType count, IndexType& index, int& cutfeat, ElementType& cutval, const BoundingBox& bbox, int depth)
		{
			ofec::Real sum1 = 0.0;
			ofec::Real sum2 = 0.0;
			cutfeat = (m_relative_depth + depth) % m_dim;
			//cutfeat = rand() % m_dim;
			std::vector<IndexType> cur_idx(count);
			for (IndexType i = 0; i < count; ++i)
			{
				cur_idx[i] = ind[i];
			}

			index = cur_idx[cur_idx.size() / 2];

			for (auto& i : cur_idx)
				sum1 += m_ratiodata[i];

			for (auto j = 0; j < cur_idx.size() / 2; ++j)
			{
				sum2 += m_ratiodata[cur_idx[j]];
			}

			cutval = bbox.box[cutfeat].first + (bbox.box[cutfeat].second - bbox.box[cutfeat].first) * (sum2 / sum1);
		}
		/**
		*  Subdivide the list of points by a plane perpendicular on axe corresponding
		*  to the 'cutfeat' dimension at 'cutval' position.
		*
		*  On return:
		*  dataset[ind[0..lim1-1]][cutfeat]<cutval
		*  dataset[ind[lim1..lim2-1]][cutfeat]==cutval
		*  dataset[ind[lim2..count]][cutfeat]>cutval
		*/
		void planeSplit(IndexType* ind, const IndexType count, int cutfeat, ElementType cutval, IndexType& lim1, IndexType& lim2)
		{
			/* Move vector indices for left subtree to front of list. */
			IndexType left = 0;
			IndexType right = count - 1;
			for (;;) {
				while (left <= right && getDataset(ind[left], cutfeat) < cutval) ++left;
				while (right && left <= right && getDataset(ind[right], cutfeat) >= cutval) --right;
				if (left > right || !right) break;  // "!right" was added to support unsigned Index types
				std::swap(ind[left], ind[right]);
				++left;
				--right;
			}
			/* If either list is empty, it means that all remaining features
			* are identical. Split in the middle to maintain a balanced tree.
			*/
			lim1 = left;
			right = count - 1;
			for (;;) {
				while (left <= right && getDataset(ind[left], cutfeat) <= cutval) ++left;
				while (right && left <= right && getDataset(ind[right], cutfeat) > cutval) --right;
				if (left > right || !right) break;  // "!right" was added to support unsigned Index types
				std::swap(ind[left], ind[right]);
				++left;
				--right;
			}
			lim2 = left;
		}

		int enqury(const std::vector<ElementType>& p, NodePtr node) const {
			if (node->child1 == NULL && node->child2 == NULL) {
				return node->lr.idx_region;
			}
			if (m_mode == 1) {
				if (p[node->sub.divfeat] < getDataset(node->sub.idx_sample, node->sub.divfeat)) {
					return enqury(p, node->child1);
				}
				else {
					return enqury(p, node->child2);
				}
			}
			else if (m_mode == 2) {
				if (p[node->sub.divfeat] < node->sub.pivot) {
					return enqury(p, node->child1);
				}
				else {
					return enqury(p, node->child2);
				}
			}
			return -1;
		}

		void leafParent(IndexType idx_region, NodePtr node, NodePtr parent, NodePtr& result) {
			if (node->child1 == NULL && node->child2 == NULL) {
				if (node->lr.idx_region == idx_region) {
					if (node != m_root) result = parent;
					else {
						result = m_root;
					}
				}
				return;
			}
			if (node->child1 != NULL && result == NULL)  leafParent(idx_region, node->child1, node, result);
			if (node->child2 != NULL && result == NULL)  leafParent(idx_region, node->child2, node, result);
		}

		void boxRatio(BoundingBox& it, unsigned idx) {
			it.rat = 1;
			for (int i = 0; i < m_dim; ++i) {
				it.rat *= (it.box[i].second - it.box[i].first) / (m_rootBbox.box[i].second - m_rootBbox.box[i].first);
			}
			if (it.rat > m_lrat) {
				m_lrat = it.rat;
				m_lbox = idx;
			}
			if (it.rat < m_srat) {
				m_srat = it.rat;
				m_sbox = idx;
			}

			it.volume = 1;
			for (int i = 0; i < m_dim; ++i)
				it.volume *= (it.box[i].second - it.box[i].first);

			//it.volume = 0;
			//for (int i = 0; i < m_dim; ++i) {
			//	it.volume += (it.box[i].second - it.box[i].first) * (it.box[i].second - it.box[i].first);
			//}
			//it.volume = std::sqrt(it.volume);
		}

		void leafNode(IndexType idx_region, NodePtr node, NodePtr& leafnode) {
			if (node->child1 == NULL && node->child2 == NULL && node->lr.idx_region == idx_region) {
				leafnode = node;
				return;
			}
			if (node->child1 != NULL)  leafNode(idx_region, node->child1, leafnode);
			if (node->child2 != NULL)  leafNode(idx_region, node->child2, leafnode);
		}

		void getLeafRegions(NodePtr node, std::vector<size_t>& idx_regions) const {
			if (node->child1 == NULL && node->child2 == NULL) {
				idx_regions.push_back(node->lr.idx_region);
				return;
			}
			if (node->child1 != NULL)
				getLeafRegions(node->child1, idx_regions);
			if (node->child2 != NULL)
				getLeafRegions(node->child2, idx_regions);
		}

		void findUnionsAtDepth(size_t depth, NodePtr node, std::vector<std::vector<IndexType>>& region_unions) const {
			if (node->depth == depth) {
				std::vector<IndexType> idx_regions;
				getLeafRegions(node, idx_regions);
				region_unions.push_back(std::move(idx_regions));
				return;
			}
			if (node->child1 != NULL)
				findUnionsAtDepth(depth, node->child1, region_unions);
			if (node->child2 != NULL)
				findUnionsAtDepth(depth, node->child2, region_unions);
		}

		void neighborCheck(IndexType idx, NodePtr node, std::list<IndexType>& neighbors) const {
			if (node->child1 == NULL && node->child2 == NULL && idx != node->lr.idx_region) {
				neighbors.emplace_back(node->lr.idx_region);
				return;
			}
			if (node->child1 != NULL) {
				if (!(m_regions[idx]->box[node->sub.divfeat].second < node->sub.low || node->sub.pivot < m_regions[idx]->box[node->sub.divfeat].first))
					neighborCheck(idx, node->child1, neighbors);
			}
			if (node->child2 != NULL) {
				if (!(m_regions[idx]->box[node->sub.divfeat].second < node->sub.pivot || node->sub.high < m_regions[idx]->box[node->sub.divfeat].first))
					neighborCheck(idx, node->child2, neighbors);
			}
		}

		void mergeNode(NodePtr node, std::list<IndexType>& merged_regs) {
			if (node == NULL) return;
			if (node->child1 != NULL)
				mergeNode(node->child1, merged_regs);
			NodePtr parent = node->parent;
			NodePtr sibling = parent->child1 == node ? parent->child2 : parent->child1;
			if (sibling->child1 != NULL)
				mergeNode(sibling->child1, merged_regs);
			m_regions[node->lr.idx_region]->volume += m_regions[sibling->lr.idx_region]->volume;
			m_regions[node->lr.idx_region]->depth--;
			m_regions[node->lr.idx_region]->box[parent->sub.divfeat].first = std::min(m_regions[node->lr.idx_region]->box[parent->sub.divfeat].first, m_regions[sibling->lr.idx_region]->box[parent->sub.divfeat].first);
			m_regions[node->lr.idx_region]->box[parent->sub.divfeat].second = std::max(m_regions[node->lr.idx_region]->box[parent->sub.divfeat].second, m_regions[sibling->lr.idx_region]->box[parent->sub.divfeat].second);
			m_size--;
			parent->lr.idx_region = node->lr.idx_region;
			m_shelvedNodes.push_back(parent->child1);
			m_shelvedNodes.push_back(parent->child2);
			parent->child1 = NULL;
			parent->child2 = NULL;
			merged_regs.push_back(sibling->lr.idx_region);
			m_shelvedIndices.insert(sibling->lr.idx_region);
		}

		/* Copy leaf nodes of p2 to p1 */
		void copyLeafNodes(NodePtr p1, NodePtr p2, std::map<IndexType, IndexType>& add_id) {
			if (p2->child1 == NULL && p2->child2 == NULL) {
				if (m_shelvedIndices.empty()) {
					p1->lr.idx_region = m_regions.size();
					m_regions.emplace_back(new BoundingBox);
				}
				else {
					p1->lr.idx_region = *m_shelvedIndices.begin();
					m_shelvedIndices.erase(m_shelvedIndices.begin());
				}
				m_regions[p1->lr.idx_region]->depth = p1->depth;
				m_regions[p1->lr.idx_region]->box = p2->lr.bbox->box;
				p1->lr.bbox = m_regions[p1->lr.idx_region].get();
				p1->child1 = p1->child2 = NULL;
				add_id[p2->lr.idx_region] = p1->lr.idx_region;
				m_size++;
			}
			else {
				p1->sub.divfeat = p2->sub.divfeat;
				p1->sub.pivot = p2->sub.pivot;
				p1->sub.low = p2->sub.low;
				p1->sub.high = p2->sub.high;

				if (!m_shelvedNodes.empty()) {
					p1->child1 = m_shelvedNodes.front();
					m_shelvedNodes.pop_front();
				}
				else
					p1->child1 = m_pool.allocate<Node>();
				p1->child1->depth = p1->depth + 1;
				p1->child1->parent = p1;
				copyLeafNodes(p1->child1, p2->child1, add_id);

				if (!m_shelvedNodes.empty()) {
					p1->child2 = m_shelvedNodes.front();
					m_shelvedNodes.pop_front();
				}
				else
					p1->child2 = m_pool.allocate<Node>();
				p1->child2->depth = p1->depth + 1;
				p1->child2->parent = p1;
				copyLeafNodes(p1->child2, p2->child2, add_id);
			}
		}
	};
	/** @} */ // end of grouping
} // end of NS


#endif /* NANOFLANN_HPP_ */
