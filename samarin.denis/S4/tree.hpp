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
  }

}

#endif
