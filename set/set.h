#include <cassert>  // assert
#include <iterator> // std::reverse_iterator
#include <utility>  // std::pair, std::swap

template <typename T>
bool equal(T const& a, T const& b) {
  return !((a < b) || (b < a));
}

template <typename T>
struct set {
private:
  struct base_node {
    base_node* l;
    base_node* r;
    base_node* pred;
    base_node() {
      l = r = pred = nullptr;
    }
    bool is_left_son() {
      return this == pred->l;
    }
  };

  struct node : base_node {
    T value;
    node(T const& value) : value(value) {}
  };

public:
  struct iterator {
    friend set;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T const;
    using pointer = T const*;
    using reference = T const&;

    iterator() = default;
    iterator(iterator const& other) : root(other.root){};

    iterator& operator=(iterator const& other) = default;

    reference operator*() const noexcept {
      return static_cast<node*>(root)->value;
    } // O(1) nothrow
    pointer operator->() const noexcept {
      return &(static_cast<node*>(root)->value);
    } // O(1) nothrow

    iterator& operator++() & {
      base_node* ans = root;
      if (ans->r != nullptr) {
        ans = ans->r;
        while (ans->l != nullptr) {
          ans = ans->l;
        }
      } else {
        if (ans->is_left_son()) {
          if (ans->pred != nullptr) {
            ans = ans->pred;
          }
        } else {
          while (ans->pred != nullptr && !ans->is_left_son()) {
            ans = ans->pred;
          }
          if (ans->pred != nullptr) {
            ans = ans->pred;
          }
        }
      }
      root = ans;
      return *this;
    } //      nothrow
    iterator operator++(int) & {
      iterator copy = *this;
      ++(*this);
      return copy;
    } //      nothrow

    iterator& operator--() & {
      base_node* ans = root;
      if (ans->l != nullptr) {
        ans = ans->l;
        while (ans->r != nullptr) {
          ans = ans->r;
        }
      } else {
        if (ans->is_left_son()) {
          while (ans->pred != nullptr && ans->is_left_son()) {
            ans = ans->pred;
          }
          if (ans->pred != nullptr) {
            ans = ans->pred;
          }
        } else {
          if (ans->pred != nullptr) {
            ans = ans->pred;
          }
        }
      }
      root = ans;
      return *this;
    } //      nothrow
    iterator operator--(int) & {
      iterator copy = *this;
      --(*this);
      return copy;
    } //      nothrow
    bool operator==(iterator const& a) const noexcept {
      return a.root == root;
    }
    bool operator!=(iterator const& a) const noexcept {
      return a.root != root;
    }

  private:
    explicit iterator(base_node* new_data) : root(new_data) {}

    base_node* root;
  };

  using const_iterator = iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  set() noexcept : last_node() {} // O(1) nothrow

  set(set const& other) : last_node() {
    set copy;
    try {
      for (T const& i : other) {
        copy.insert(i);
      }
    } catch (...) {
      copy.clear();
      throw;
    }
    swap(copy);
  } // O(n) strong

  set& operator=(set const& other) {
    if (this == &other) {
      return *this;
    }
    set copy(other);
    swap(copy);
    return *this;
  } // O(n) strong

  ~set() noexcept {
    clear();
  } // O(n) nothrow

  void clear() noexcept {
    while (!empty()) {
      erase(begin());
    }
  } // O(n) nothrow

  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  } //   nothrow

  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  } //      nothrow

  const_iterator begin() const noexcept {
    auto* ans = const_cast<base_node*>(&last_node);
    while (ans->l != nullptr) {
      ans = ans->l;
    }
    return const_iterator(ans);
  } //      nothrow

  const_iterator end() const noexcept {
    return const_iterator(const_cast<base_node*>(&last_node));
  } //      nothrow


  bool empty() noexcept {
    return last_node.l == nullptr;
  } // O(1) nothrow

  std::pair<iterator, bool> insert(T const& key) {
    auto it = find(key);
    if (it != end()) {
      return {it, false};
    }

    node* new_node = nullptr;
    try {
      new_node = new node(key);
    } catch (...) {
      throw;
    }
    if (new_node == nullptr) {
      throw;
    }

    base_node* pred(&last_node);
    base_node* ans(last_node.l);


    bool left_son = true;
    while (ans != nullptr) {
      pred = ans;
      if (key < static_cast<node*>(ans)->value) {
        ans = ans->l;
        left_son = true;
      } else {
        ans = ans->r;
        left_son = false;
      }
    }

    new_node->pred = pred;
    if (left_son) {
      pred->l = new_node;
    } else {
      pred->r = new_node;
    }

    return {iterator(static_cast<base_node*>(new_node)), true};
  } // O(h) strong

  const_iterator find(T const& key) const {
    auto* ans = const_cast<base_node*>(last_node.l);
    while (ans != nullptr) {
      if (key < static_cast<node*>(ans)->value) {
        ans = ans->l;
      } else if (static_cast<node*>(ans)->value < key) {
        ans = ans->r;
      } else {
        return const_iterator(ans);
      }
    }
    return end();
  } // O(h) strong

  const_iterator lower_bound(T const& key) const {
    const_iterator ans = find(key);
    if (ans == end()) {
      return upper_bound(key);
    }
    return ans;
  } // O(h) strong

  const_iterator upper_bound(T const& key) const {
    const_iterator ans1 = find(key);
    if (ans1 != end()) {
      ans1++;
      return ans1;
    }
    return upper_bound(key, last_node.l);
  } // O(h) strong

  void swap(set& other) noexcept {
    if (!empty() && !other.empty()) {
      std::swap(last_node.l, other.last_node.l);
      std::swap(last_node.l->pred, other.last_node.l->pred);
    } else if (!other.empty()) {
      last_node.l = other.last_node.l;
      other.last_node.l->pred = &last_node;
      other.last_node.l = nullptr;
    } else if (!empty()) {
      other.last_node.l = last_node.l;
      last_node.l->pred = &other.last_node;
      last_node.l = nullptr;
    }
  } // O(1) nothrow

  friend void swap(set& a, set& b) {
    a.swap(b);
  }

  iterator erase(iterator it) {
    base_node* ans = it.root;
    it++;
    // leaf
    if (ans->l == nullptr && ans->r == nullptr) {
      if (ans->is_left_son()) {
        ans->pred->l = nullptr;
      } else {
        ans->pred->r = nullptr;
      }
      delete static_cast<node*>(ans);
      return it;
    }
    // has left son
    if (ans->l != nullptr && ans->r == nullptr) {
      if (ans->is_left_son()) {
        ans->pred->l = ans->l;
      } else {
        ans->pred->r = ans->l;
      }
      ans->l->pred = ans->pred;
      delete static_cast<node*>(ans);
      return it;
    }
    // has right son
    if (ans->l == nullptr && ans->r != nullptr) {
      if (ans->is_left_son()) {
        ans->pred->l = ans->r;
      } else {
        ans->pred->r = ans->r;
      }
      ans->r->pred = ans->pred;
      delete static_cast<node*>(ans);
      return it;
    }
    // both
    base_node* min_right = ans->r;

    while (min_right->l != nullptr) {
      min_right = min_right->l;
    }

    if (ans->r == min_right) {
      if (ans->is_left_son()) {
        ans->pred->l = min_right;
      } else {
        ans->pred->r = min_right;
      }
      min_right->pred = ans->pred;
      min_right->l = ans->l;
      ans->l->pred = min_right;
    } else {
      if (ans->is_left_son()) {
        ans->pred->l = min_right;
      } else {
        ans->pred->r = min_right;
      }

      if (min_right->is_left_son()) {
        min_right->pred->l = min_right->r;
      } else {
        min_right->pred->r = min_right->r;
      }

      if (min_right->r != nullptr) {
        min_right->r->pred = min_right->pred;
      }

      min_right->pred = ans->pred;
      min_right->l = ans->l;
      min_right->r = ans->r;

      ans->l->pred = min_right;
      ans->r->pred = min_right;
    }
    delete static_cast<node*>(ans);
    return it;
  } // O(h) nothrow

private:
  const_iterator upper_bound(T const& key, base_node* v) const {
    if (v == nullptr) {
      return end();
    }
    if (key < static_cast<node*>(v)->value) {
      const_iterator cur_ans = upper_bound(key, v->l);
      if (cur_ans == end()) {
        return const_iterator(v);
      } else {
        return cur_ans;
      }
    } else {
      return upper_bound(key, v->r);
    }
  }

private:
  base_node last_node;
};
