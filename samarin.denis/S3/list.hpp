#ifndef LIST_HPP
#define LIST_HPP

#include <utility>

namespace samarin {

  namespace detail {
    struct NodeBase {
      NodeBase * next;

      explicit NodeBase(NodeBase * n):
        next(n)
      {}
    };

    template< class T >
    struct Node: NodeBase {
      T value;

      template< class... Args >
      Node(NodeBase * n, Args&&... args):
        NodeBase(n),
        value(std::forward< Args >(args)...)
      {}
    };
  }

  template< class T > class List;
  template< class T > class LCIter;

  template< class T >
  class LIter {
  public:
    T& operator*() const
    {
      return static_cast< detail::Node< T > * >(node_)->value;
    }

    T* operator->() const
    {
      return &static_cast< detail::Node< T > * >(node_)->value;
    }

    LIter< T >& operator++()
    {
      node_ = node_->next;
      return *this;
    }

    LIter< T > operator++(int)
    {
      LIter< T > tmp = *this;
      node_ = node_->next;
      return tmp;
    }

    bool operator==(const LIter< T >& other) const
    {
      return node_ == other.node_;
    }

    bool operator!=(const LIter< T >& other) const
    {
      return node_ != other.node_;
    }

  private:
    friend class List< T >;
    friend class LCIter< T >;
    detail::NodeBase * node_;

    LIter():
      node_(nullptr)
    {}

    explicit LIter(detail::NodeBase * n):
      node_(n)
    {}
  };

  template< class T >
  class LCIter {
  public:
    LCIter():
      node_(nullptr)
    {}

    LCIter(const LIter< T >& it):
      node_(it.node_)
    {}

    const T& operator*() const
    {
      return static_cast< const detail::Node< T > * >(node_)->value;
    }

    const T* operator->() const
    {
      return &static_cast< const detail::Node< T > * >(node_)->value;
    }

    LCIter< T >& operator++()
    {
      node_ = node_->next;
      return *this;
    }

    LCIter< T > operator++(int)
    {
      LCIter< T > tmp = *this;
      node_ = node_->next;
      return tmp;
    }

    bool operator==(const LCIter< T >& other) const
    {
      return node_ == other.node_;
    }

    bool operator!=(const LCIter< T >& other) const
    {
      return node_ != other.node_;
    }

  private:
    friend class List< T >;
    const detail::NodeBase * node_;

    explicit LCIter(const detail::NodeBase * n):
      node_(n)
    {}
  };

  template< class T >
  class List {
  public:
    List():
      head_(new detail::NodeBase(nullptr))
    {}

    List(const List< T >& other):
      head_(new detail::NodeBase(nullptr))
    {
      try {
        LIter< T > tail = before_begin();
        for (LCIter< T > it = other.cbegin(); it != other.cend(); ++it) {
          tail = insert_after(tail, *it);
        }
      } catch (...) {
        clear();
        delete head_;
        throw;
      }
    }

    List(List< T >&& other) noexcept:
      head_(other.head_)
    {
      other.head_ = nullptr;
    }

    ~List()
    {
      if (head_ != nullptr) {
        clear();
        delete head_;
      }
    }

    List< T >& operator=(const List< T >& other)
    {
      if (this != &other) {
        List< T > tmp(other);
        std::swap(head_, tmp.head_);
      }
      return *this;
    }

    List< T >& operator=(List< T >&& other) noexcept
    {
      if (this != &other) {
        clear();
        delete head_;
        head_ = other.head_;
        other.head_ = nullptr;
      }
      return *this;
    }

    void push_front(const T& v)
    {
      emplaceFront(v);
    }

    template< class... Args >
    void emplaceFront(Args&&... args)
    {
      head_->next = new detail::Node< T >(head_->next, std::forward< Args >(args)...);
    }

    LIter< T > insert_after(LIter< T > pos, const T& v)
    {
      return emplaceAfter(pos, v);
    }

    template< class... Args >
    LIter< T > emplaceAfter(LIter< T > pos, Args&&... args)
    {
      pos.node_->next = new detail::Node< T >(pos.node_->next, std::forward< Args >(args)...);
      return LIter< T >(pos.node_->next);
    }

    LIter< T > erase_after(LIter< T > pos)
    {
      detail::NodeBase * target = pos.node_->next;
      pos.node_->next = target->next;
      delete static_cast< detail::Node< T > * >(target);
      return LIter< T >(pos.node_->next);
    }

    LIter< T > before_begin()
    {
      return LIter< T >(head_);
    }

    T& front()
    {
      return static_cast< detail::Node< T > * >(head_->next)->value;
    }

    const T& front() const
    {
      return static_cast< const detail::Node< T > * >(head_->next)->value;
    }

    void pop_front()
    {
      detail::NodeBase * temp = head_->next;
      head_->next = temp->next;
      delete static_cast< detail::Node< T > * >(temp);
    }

    bool empty() const
    {
      return head_->next == nullptr;
    }

    void clear();

    LIter< T > begin()
    {
      return LIter< T >(head_->next);
    }

    LIter< T > end()
    {
      return LIter< T >(nullptr);
    }

    LCIter< T > begin() const
    {
      return LCIter< T >(head_->next);
    }

    LCIter< T > end() const
    {
      return LCIter< T >(nullptr);
    }

    LCIter< T > cbegin() const
    {
      return LCIter< T >(head_->next);
    }

    LCIter< T > cend() const
    {
      return LCIter< T >(nullptr);
    }

    LCIter< T > cbefore_begin() const
    {
      return LCIter< T >(head_);
    }

  private:
    detail::NodeBase * head_;
  };

  template< class T >
  void List< T >::clear()
  {
    detail::NodeBase * temp = head_->next;
    while (temp != nullptr) {
      detail::NodeBase * next = temp->next;
      delete static_cast< detail::Node< T > * >(temp);
      temp = next;
    }
    head_->next = nullptr;
  }

}

#endif
