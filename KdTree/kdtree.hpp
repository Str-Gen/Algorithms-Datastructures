#ifndef __KDTREE_HPP
#define __KDTREE_HPP

#include <tuple>
using std::tuple;
#include <vector>
using std::vector;
#include <utility>
using std::move;
#include <memory>
using std::make_unique;
using std::unique_ptr;
#include <cmath>
using std::floor;
using std::max;
#include <algorithm>
using std::sort;

template <class... Key>
class KdNode;

template <class... Key>
class KdTree : public unique_ptr<KdNode<Key...>>
{
  public:
    KdTree() = default;
    virtual ~KdTree() = default;
    KdTree(const KdTree<Key...> &) = default;
    KdTree<Key...> &operator=(const KdTree<Key...> &) = default;
    KdTree(KdTree<Key...> &&) = default;
    KdTree<Key...> &operator=(KdTree<Key...> &&) = default;
    KdTree(int _dim) : unique_ptr<KdNode<Key...>>(make_unique<KdNode<Key...>>()), dimensions{_dim} {}

    int depth() const;

    void insert(const vector<tuple<Key...>> &);
    void insert(vector<tuple<Key...>> &&, int);
    void insert(const tuple<Key...> &);
    void insert(tuple<Key...> &&);
    tuple<Key...> *search(tuple<Key...> &);

  private:
    int dimensions;
};

template <class... Key>
int KdTree<Key...>::depth() const
{
    if (!*this)
    {
        return -1;
    }
    return max((*this)->left.depth(), (*this)->right.depth()) + 1;
}

template <class... Key>
tuple<tuple<Key...>, vector<tuple<Key...>>, vector<tuple<Key...>>> split(vector<tuple<Key...>> &&vec, const int &dimension)
{
    sort(begin(vec), end(vec), [&dimension](auto const &t1, auto const &t2) {
        // TERRIBLE HACK THAT ESSENTIALLY LIMITS THE TUPLES TO 4 DIMENSIONS MAX
        switch (dimension)
        {
        case 0:
            return std::get<0>(t1) < std::get<0>(t2);
            break;
        case 1:
            return std::get<1>(t1) < std::get<1>(t2);
            break;
        case 2:
            return std::get<2>(t1) < std::get<2>(t2);
            break;
        case 3:
            return std::get<3>(t1) < std::get<3>(t2);
            break;
        default:
            return std::get<0>(t1) < std::get<0>(t2);
            break;
        }
    });
    int median_index = floor(vec.size() / 2.0);
    return std::make_tuple<tuple<Key...>, vector<tuple<Key...>>, vector<tuple<Key...>>>(tuple<Key...>(move(vec[median_index])), vector<tuple<Key...>>(vec.begin(), vec.begin() + median_index), vector<tuple<Key...>>(vec.begin() + median_index + 1, vec.end()));
}

template <class... Key>
void KdTree<Key...>::insert(vector<tuple<Key...>> &&data, int dim)
{
    auto [median, left_part, right_part] = move(split(move(data), dim % dimensions));
  
        (*this)->dimension_keys = move(median);
    if (left_part.size() > 0)
    {
        (*this)->left = KdTree<Key...>(dimensions);
        (*this)->left.insert(move(left_part), dim + 1);
    }
    if (right_part.size() > 0)
    {
        (*this)->right = KdTree<Key...>(dimensions);
        (*this)->right.insert(move(right_part), dim + 1);
    }
}

#endif