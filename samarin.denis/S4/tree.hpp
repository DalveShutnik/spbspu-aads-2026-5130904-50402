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

}

#endif
