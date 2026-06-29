#ifndef TREE_HPP
#define TREE_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

namespace samarin {

  namespace detail {
    template< class Key, class Value >
    struct TreeNode {
      std::pair< Key, Value > data;
      TreeNode< Key, Value > * left;
      TreeNode< Key, Value > * right;
      TreeNode< Key, Value > * parent;
    };

    template< class Node >
    Node * leftmost(Node * node)
    {
      while (node != nullptr && node->left != nullptr) {
        node = node->left;
      }
      return node;
    }

    template< class Node >
    Node * successor(Node * node)
    {
      if (node->right != nullptr) {
        return leftmost(node->right);
      }
      Node * parent = node->parent;
      while (parent != nullptr && node == parent->right) {
        node = parent;
        parent = parent->parent;
      }
      return parent;
    }
  }

  template< class Key, class Value >
  class BSTConstIterator;

  template< class Key, class Value >
  class BSTIterator {
  public:
    std::pair< Key, Value > & operator*() const
    {
      return node_->data;
    }

    std::pair< Key, Value > * operator->() const
    {
      return std::addressof(node_->data);
    }

    BSTIterator< Key, Value > & operator++()
    {
      node_ = detail::successor(node_);
      return *this;
    }

    BSTIterator< Key, Value > operator++(int)
    {
      BSTIterator< Key, Value > tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const BSTIterator< Key, Value > & other) const
    {
      return node_ == other.node_;
    }

    bool operator!=(const BSTIterator< Key, Value > & other) const
    {
      return node_ != other.node_;
    }

  private:
    template< class K, class V, class C >
    friend class BSTree;
    friend class BSTConstIterator< Key, Value >;
    detail::TreeNode< Key, Value > * node_;

    explicit BSTIterator(detail::TreeNode< Key, Value > * node):
      node_(node)
    {}
  };

  template< class Key, class Value >
  class BSTConstIterator {
  public:
    BSTConstIterator(const BSTIterator< Key, Value > & other):
      node_(other.node_)
    {}

    const std::pair< Key, Value > & operator*() const
    {
      return node_->data;
    }

    const std::pair< Key, Value > * operator->() const
    {
      return std::addressof(node_->data);
    }

    BSTConstIterator< Key, Value > & operator++()
    {
      node_ = detail::successor(node_);
      return *this;
    }

    BSTConstIterator< Key, Value > operator++(int)
    {
      BSTConstIterator< Key, Value > tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const BSTConstIterator< Key, Value > & other) const
    {
      return node_ == other.node_;
    }

    bool operator!=(const BSTConstIterator< Key, Value > & other) const
    {
      return node_ != other.node_;
    }

  private:
    template< class K, class V, class C >
    friend class BSTree;
    const detail::TreeNode< Key, Value > * node_;

    explicit BSTConstIterator(const detail::TreeNode< Key, Value > * node):
      node_(node)
    {}
  };

  template< class Key, class Value, class Compare = std::less< Key > >
  class BSTree {
  public:
    using value_type = std::pair< Key, Value >;
    using iterator = BSTIterator< Key, Value >;
    using const_iterator = BSTConstIterator< Key, Value >;

    BSTree():
      root_(nullptr),
      size_(0),
      cmp_()
    {}

    BSTree(const BSTree< Key, Value, Compare > & other):
      root_(clone(other.root_, nullptr)),
      size_(other.size_),
      cmp_(other.cmp_)
    {}

    BSTree(BSTree< Key, Value, Compare > && other) noexcept:
      root_(other.root_),
      size_(other.size_),
      cmp_(std::move(other.cmp_))
    {
      other.root_ = nullptr;
      other.size_ = 0;
    }

    ~BSTree()
    {
      destroy(root_);
    }

    BSTree< Key, Value, Compare > & operator=(const BSTree< Key, Value, Compare > & other)
    {
      if (this != std::addressof(other)) {
        BSTree< Key, Value, Compare > tmp(other);
        swap(tmp);
      }
      return *this;
    }

  private:
    detail::TreeNode< Key, Value > * root_;
    std::size_t size_;
    Compare cmp_;

    static detail::TreeNode< Key, Value > * clone(const detail::TreeNode< Key, Value > * node,
        detail::TreeNode< Key, Value > * parent)
    {
      if (node == nullptr) {
        return nullptr;
      }
      detail::TreeNode< Key, Value > * fresh = new detail::TreeNode< Key, Value >{ node->data, nullptr, nullptr, parent };
      try {
        fresh->left = clone(node->left, fresh);
        fresh->right = clone(node->right, fresh);
      } catch (...) {
        destroy(fresh);
        throw;
      }
      return fresh;
    }

    static void destroy(detail::TreeNode< Key, Value > * node)
    {
      if (node == nullptr) {
        return;
      }
      destroy(node->left);
      destroy(node->right);
      delete node;
    }
  };

}

#endif
