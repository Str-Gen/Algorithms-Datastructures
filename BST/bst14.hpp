#ifndef __BST_HPP
#define __BST_HPP
#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;
#include <queue>
#include <memory>
#include <functional>

using std::cerr;
using std::make_unique;
using std::ostream;
using std::unique_ptr;
/**********************************************************************

   Class: Binary Search Tree (BST)
   
   beschrijving: BST in which no ducplicate keys are allowed
   
***************************************************************************/

template <class Key, class Data>
class BSTnode;

template <class Key, class Data>
class BST : public unique_ptr<BSTnode<Key, Data>>
{
    using unique_ptr<BSTnode<Key, Data>>::unique_ptr;

  public:
    BST<Key, Data>() { cout << "BST()" << endl; };
    ~BST<Key, Data>() { cout << "~BST()" << endl; };
    BST<Key, Data>(const BST<Key, Data> &other) { cout << "BST(const BST&)" << endl; };
    BST<Key, Data>(BST<Key, Data> &&other) { cout << "BST(BST&&)" << endl; };
    BST<Key, Data> &operator=(const BST<Key, Data> &other)
    {
        cout << "operator=(const BST&)" << endl;
        // deep copy
        cout << "deep copy still required" << endl;
        return *this;
    };
    BST<Key, Data> &operator=(BST<Key, Data> &&other)
    {
        cout << "operator=(BST&&)" << endl;
        this->unique_ptr<BSTnode<Key, Data>>::operator=(move(other));
        return *this;
    };

    BST<Key, Data>(unique_ptr<BSTnode<Key, Data>> &&nodeptr) : unique_ptr<BSTnode<Key, Data>>(move(nodeptr))
    {
        cout << "BST(unique_ptr<BSTnode>&&)" << endl;
    };

    BST<Key, Data> &operator=(unique_ptr<BSTnode<Key, Data>> &&nodeptr)
    {
        cout << "operator=(BSTnode&&)" << endl;
        this->unique_ptr<BSTnode<Key, Data>>::operator=(move(nodeptr));
        return *this;
    };

  public:
    void add(const Key &key, const Data &data);
    void rotate(bool left);
    void inorder(std::function<void(const BSTnode<Key, Data> &)> visit) const;
    void output(ostream &os) const;
    bool repOK() const;
    int depth();
    void makeImbalanced();
    void makeBalanced();

  protected:
    // The search function looks for key in the tree and return the tree containing the key or an empty tree if the key isn't in the tree
    // It also sets the parent argument (a pointer passed via reference) to that tree's parent (so long as the tree has a parent)
    void search(const Key &key, BSTnode<Key, Data> *&parent, BST<Key, Data> *&location);
};

template <class Key, class Data>
class BSTnode
{
    friend class BST<Key, Data>;

  public:
    BSTnode() : parent(0) { cout << "BSTnode()" << endl; }
    BSTnode(const Key &sl, const Data &d) : key{sl}, data(d), parent(0) { cout << "BSTnode(const Key &, const Data &)" << endl; };
    BSTnode(Key &&sl, Data &&d) : key{move(sl)}, data(move(d)), parent(0) { cout << "BSTnode(Key &&,Data &&)" << endl; };

  protected:
    Key key;
    Data data;
    BSTnode<Key, Data> *parent;
    BST<Key, Data> left, right;
};

template <class Key, class Data>
void BST<Key, Data>::add(const Key &key, const Data &data)
{
    BST<Key, Data> *location;
    BSTnode<Key, Data> *parent;
    search(key, parent, location);
    if (!*location)
    {
        //note: C++14 style
        BST<Key, Data> newnode = make_unique<BSTnode<Key, Data>>(key, data);
        // C++11 style
        //        BST<Key,Data> newnode(new BSTnode<Key,Data>(key,data));
        newnode->parent = parent;
        *location = move(newnode);
    };
};

// To learn more about the reference to pointer *& read https://www.codeproject.com/Articles/4894/Pointer-to-Pointer-and-Reference-to-Pointer
template <class Key, class Data>
void BST<Key, Data>::search(const Key &key, BSTnode<Key, Data> *&parent, BST<Key, Data> *&location)
{
    location = this;
    parent = 0;
    // Fixed while location => *location !!!
    while (*location && (*location)->key != key)
    {
        parent = location->get();
        if ((*location)->key < key)
            // location is of BST* type, so (*location) is a BST, but
            // a BST is a unique_ptr so even though it seems like a normal object,
            // you can call the -> operator on it!
            location = &(*location)->right;
        else
            location = &(*location)->left;
    };
};
template <class Key, class Data>
void BST<Key, Data>::inorder(std::function<void(const BSTnode<Key, Data> &)> visit) const
{
    if (*this)
    {
        (*this)->left.inorder(visit);
        visit(**this);
        (*this)->right.inorder(visit);
    };
}

template <class Key, class Data>
void BST<Key, Data>::output(ostream &os) const
{
    inorder([&os](const BSTnode<Key, Data> &node) {
        os << "(" << node.key << " -> " << node.data << ")";
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

#endif