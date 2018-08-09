#ifndef __SPLAYTREE_HPP
#define __SPLAYTREE_HPP

#define CONSTRUCTION_LOGGING 0

#include "splaynode.hpp"
#include <iostream>
using std::cout;
using std::endl;
using std::ostream;
#include <iomanip>
#include <algorithm>
#include <functional>
#include <memory>
using std::make_unique;
using std::unique_ptr;
#include <sstream>
#include <stack>
#include <tuple>

template <class Key, class Data>
class SplayNode;

template <class Key, class Data>
using SplayNodeptr = unique_ptr<SplayNode<Key, Data>>;

template <class Key, class Data>
class SplayTree : public SplayNodeptr<Key, Data>
{
  public:
    SplayTree() : SplayNodeptr<Key, Data>{nullptr}
    {
        if (CONSTRUCTION_LOGGING)
            cout << "SplayTree()\n";
    };
    SplayTree(const Key &_k, const Data &_d) : SplayNodeptr<Key, Data>{make_unique<SplayNode<Key, Data>>(_k, _d)}
    {
        if (CONSTRUCTION_LOGGING)
            cout << "SplayTree(Key&,Data&)\n";
    };
    virtual ~SplayTree() = default;

    SplayTree(const SplayTree<Key, Data> &other) //: SplayNodeptr<Key, Data>{nullptr}
    {
        if (CONSTRUCTION_LOGGING)
            cout << "SplayTree(const SplayTree&)\n";

        if (other)
        {
            this->reset(new SplayNode<Key, Data>(*other));
        }
    };
    SplayTree &operator=(const SplayTree<Key, Data> &other)
    {
        if (CONSTRUCTION_LOGGING)
            cout << "operator=(const SplayTree&)\n";

        SplayTree<Key, Data> temp{other};
        temp.swap(*this);
        return *this;
    };

    SplayTree(SplayTree<Key, Data> &&other) : SplayNodeptr<Key, Data>{move(other)}
    {
        if (CONSTRUCTION_LOGGING)
            cout << "SplayTree(SplayTree&&)\n";
    };

    SplayTree &operator=(SplayTree<Key, Data> &&other)
    {
        if (CONSTRUCTION_LOGGING)
            cout << "operator=(SplayTree&&)\n";

        this->swap(other);
        return *this;
    };

    int depth() const;
    void rotate(bool);
    void insert_bottom_up(const Key &, const Data &);
    void insert_top_down(const Key &, const Data &);
    void delete_bottom_up(const Key &);
    void delete_top_down(const Key &);
    bool repOK() const;    

    void inorder(std::function<void(const SplayNode<Key, Data> &)> visit) const;
    void preorder(std::function<void(const SplayNode<Key, Data> &)> visit) const;
    void postorder(std::function<void(const SplayNode<Key, Data> &)> visit) const;
    void output(ostream &os) const;
    void pretty_print(int indent = 0) const;

  protected:
    std::tuple<SplayTree<Key, Data> *, SplayNode<Key, Data> *> search(const Key &);
    std::tuple<SplayTree<Key, Data> *, SplayTree<Key, Data> *, SplayTree<Key, Data> *> get_family_pointers(const SplayTree<Key, Data> *const);
    void insert_bu_fixup(SplayTree<Key, Data> *);
    void transplant(SplayTree<Key, Data> &&);    
};

template <class Key, class Data>
std::tuple<SplayTree<Key, Data> *, SplayNode<Key, Data> *> SplayTree<Key, Data>::search(const Key &key)
{
    SplayTree<Key, Data> *location = this;
    SplayNode<Key, Data> *parent = nullptr;

    while (*location && (*location)->key != key)
    {
        parent = location->get();
        if (key > (*location)->key)
        {
            location = &(*location)->right;
        }
        else
        {
            location = &(*location)->left;
        }
    }
    return std::tuple(location, parent);
}

template <class Key, class Data>
std::tuple<SplayTree<Key, Data> *, SplayTree<Key, Data> *, SplayTree<Key, Data> *> SplayTree<Key, Data>::get_family_pointers(const SplayTree<Key, Data> *const location)
{
    SplayTree<Key, Data> *parent = nullptr;
    SplayTree<Key, Data> *grandparent = nullptr;
    SplayTree<Key, Data> *uncle = nullptr;
    if (location && *location && (*location)->parent && (*location)->parent->parent)
    {
        if (!((*location)->parent->parent->parent))
        {
            grandparent = this;
        }
        else
        {
            grandparent = (*location)->parent->parent->parent->getChild((*location)->parent->parent->isLeftChild());
        }
    }

    bool grandparent_to_parent_direction = (*location)->parent->isLeftChild();
    parent = (*grandparent)->getChild(grandparent_to_parent_direction);
    uncle = (*grandparent)->getChild(!grandparent_to_parent_direction);
    return std::make_tuple(grandparent, parent, uncle);
}

template <class Key, class Data>
int SplayTree<Key, Data>::depth() const
{
    if (!*this)
    {
        return -1;
    }
    return std::max((*this)->left.depth(), (*this)->right.depth()) + 1;
}

template <class Key, class Data>
void SplayTree<Key, Data>::rotate(bool left)
{
    if (!*this)
    {
        return;
    }
    SplayTree<Key, Data> child = move(*((*this)->getChild(!left)));
    if (child)
    {
        SplayTree<Key, Data> subchild = move(*(child->getChild(left)));
        if (subchild)
        {
            *((*this)->getChild(!left)) = move(subchild);
        }
        child->parent = (*this)->parent;
        (*this)->parent = child.get();
        *(child->getChild(left)) = move(*this);
        *this = move(child);
    }
}

template <class Key, class Data>
void SplayTree<Key, Data>::insert_bottom_up(const Key &key, const Data &data)
{
    auto [location, parent] = search(key);
    if (*location)
    {
        return;
    }
    *location = SplayTree<Key, Data>(key, data);
    (*location)->parent = parent;  
}

template <class Key, class Data>
void SplayTree<Key, Data>::insert_bu_fixup(SplayTree<Key, Data> *location)
{   
    
}

template <class Key, class Data>
void SplayTree<Key, Data>::transplant(SplayTree<Key, Data> &&other)
{
    if ((*this)->parent)
    {
        if ((*this)->isLeftChild())
        {
            (*this)->parent->left = move(other);
        }
        else
        {
            (*this)->parent->right = move(other);
        }
    }
}

template <class Key, class Data>
void SplayTree<Key, Data>::delete_bottom_up(const Key &key)
{
}

template <class Key, class Data>
void SplayTree<Key, Data>::inorder(std::function<void(const SplayNode<Key, Data> &)> visit) const
{
    if (*this)
    {
        (*this)->left.inorder(visit);
        visit(**this);
        (*this)->right.inorder(visit);
    };
}

template <class Key, class Data>
void SplayTree<Key, Data>::preorder(std::function<void(const SplayNode<Key, Data> &)> visit) const
{
    if (*this)
    {
        visit(**this);
        (*this)->left.preorder(visit);
        (*this)->right.preorder(visit);
    }
}

template <class Key, class Data>
void SplayTree<Key, Data>::postorder(std::function<void(const SplayNode<Key, Data> &)> visit) const
{
    if (*this)
    {
        (*this)->left.postorder(visit);
        (*this)->right.postorder(visit);
        visit(**this);
    }
}

template <class Key, class Data>
void SplayTree<Key, Data>::output(ostream &os) const
{
    inorder([&os](const SplayNode<Key, Data> &node) {
        os << "(" << node.key << " -> " << node.data <<  ")";
        os << "\n  Left child: ";
        if (node.left)
            os << node.left->key;
        else
            os << "-----";
        os << "\n  Right child: ";
        if (node.right)
            os << node.right->key;
        else
            os << "-----";
        os << "\n";
    });
}

template <class Key, class Data>
void SplayTree<Key, Data>::pretty_print(int indent) const
{
    if (*this)
    {
        if ((*this)->right)
            (*this)->right.pretty_print(indent + 8);

        if (indent)
        {
            cout << std::setw(indent) << ' ';
        }
        cout << (*this)->key << ',' << (*this)->data <<  '\n';

        if ((*this)->left)
            (*this)->left.pretty_print(indent + 8);
    }
}

#endif