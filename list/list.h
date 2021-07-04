#pragma once
#include <cassert>
#include <iterator>

template <typename T>
class list {
private:
	struct base_node {
		base_node* prev;
		base_node* next;
		base_node() : prev(this), next(this) {}
		virtual ~base_node() {}
	};
	struct node : base_node {
		T value;
		explicit node(T const& val) : value(val) {}
	};
	base_node last_node_;

public:
	template <class Type>
	struct template_iterator {
		using value_type = Type;
		using reference = Type&;
		using pointer = Type*;
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;
		friend list;

		template_iterator& operator++() & {
			ptr = ptr->next;
			return *this;
		}

		template_iterator& operator--() & {
			ptr = ptr->prev;
			return *this;
		}

		template_iterator operator++(int) & {
			template_iterator cur = *this;
			++(*this);
			return cur;
		}

		template_iterator operator--(int) & {
			template_iterator cur = *this;
			--(*this);
			return cur;
		}

		reference operator*() const noexcept {
			return static_cast<node*>(ptr)->value;
		}

		pointer operator->() const noexcept {
			return &static_cast<node*>(ptr)->value;
		}

		friend bool operator==(template_iterator const& lhs, template_iterator const& rhs) noexcept {
			return lhs.ptr == rhs.ptr;
		}

		friend bool operator!=(template_iterator const& lhs, template_iterator const& rhs) noexcept {
			return !(lhs.ptr == rhs.ptr);
		}

		operator template_iterator<Type const>() const {
			return template_iterator<Type const>(ptr);
		}


	private:
		template_iterator(base_node* ptr) : ptr(ptr) {}
		base_node* ptr = nullptr;
	};

	using iterator = template_iterator<T>;
	using const_iterator = template_iterator<const T>;

	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	// O(1)
	list() = default;

	// O(n), strong
	list(list const& oth) {
		try {
			for (const auto& i : oth) {
				push_back(i);
			}
		} catch (...) {
			clear();
			throw;
		}
	}

	// O(n), strong
	list& operator=(list const& oth) {
		if (this != &oth) {
			list copy(oth);
			swap(copy);
		}
		return *this;
	}

	// O(n)
	~list() noexcept {
		clear();
	}

	// O(1) ????
	bool empty() const noexcept {
		return last_node_.prev == &last_node_;
	}

	// O(1)
	T& front() noexcept {
		return static_cast<node*>(last_node_.next)->value;
	}
	// O(1)
	T const& front() const noexcept {
		return static_cast<node const*>(last_node_.next)->value;
	}

	// O(1), strong
	void push_front(T const& val) {
		insert(begin(), val);
	}
	// O(1)
	void pop_front() noexcept {
		erase(begin());
	}

	// O(1)
	T& back() noexcept {
		return static_cast<node*>(last_node_.prev)->value;
	}
	// O(1)
	T const& back() const noexcept {
		return static_cast<node const*>(last_node_.prev)->value;
	}

	// O(1), strong
	void push_back(T const& val) {
		insert(end(), val);
	}
	// O(1)
	void pop_back() noexcept {
		iterator it = end();
		it--;
		erase(it);
	}

	// O(1)
	iterator begin() noexcept {
		return iterator(last_node_.next);
	}
	// O(1)
	const_iterator begin() const noexcept {
		return const_iterator(last_node_.next);
	}

	// O(1)
	iterator end() noexcept {
		return iterator(&last_node_);
	}
	// O(1)
	const_iterator end() const noexcept {
		return const_iterator(const_cast<base_node*>(&last_node_));
	}

	// O(1)
	reverse_iterator rbegin() noexcept {
		return reverse_iterator(&last_node_);
	}
	// O(1)
	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(const_cast<base_node*>(&last_node_));
	}

	// O(1)
	reverse_iterator rend() noexcept {
		return reverse_iterator(last_node_.next);
	}
	// O(1)
	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(last_node_.next);
	}

	// O(n)
	void clear() noexcept {
		while (!empty()) {
			pop_back();
		}
	}

	// O(1), strong
	iterator insert(const_iterator pos, T const& val) {
		node* new_node = new node(val);
		base_node* pos_n = pos.ptr;
		new_node->prev = pos_n->prev;
		new_node->next = pos_n;
		pos_n->prev->next = new_node;
		pos_n->prev = new_node;

		return iterator(new_node);
	}
	// O(1)
	iterator erase(const_iterator pos) noexcept {
		base_node* pos_n = pos.ptr;
		pos_n->prev->next = pos_n->next;
		pos_n->next->prev = pos_n->prev;
		iterator ans(pos_n->next);
		delete pos_n;
		return ans;
	}
	// O(n)
	iterator erase(const_iterator first, const_iterator last) noexcept {
		iterator it = erase(first);

		while (it != last) {
			it = erase(it);
		}
		return it;
	}
	// O(1)
	void splice(const_iterator pos, list& other, const_iterator first,
														const_iterator last) noexcept {
		if (first == last) {
			return;
		}
		last--;
		base_node* first_n = first.ptr;
		base_node* last_n = last.ptr;
		base_node* pos_n = pos.ptr;
		first_n->prev->next = last_n->next;
		last_n->next->prev = first_n->prev;

		first_n->prev = pos_n->prev;
		last_n->next = pos_n;

		pos_n->prev->next = first_n;
		pos_n->prev = last_n;
	}

	void swap(list& a) noexcept {
		if (empty()) {
			last_node_.next = &a.last_node_;
			last_node_.prev = &a.last_node_;
		} else {
			last_node_.next->prev = &a.last_node_;
			last_node_.prev->next = &a.last_node_;
		}

		if (a.empty()) {
			a.last_node_.next = &last_node_;
			a.last_node_.prev = &last_node_;
		} else {
			a.last_node_.next->prev = &last_node_;
			a.last_node_.prev->next = &last_node_;
		}

		std::swap(a.last_node_, last_node_);
	}

	friend void swap(list& a, list& b) noexcept {
		a.swap(b);
	}
};
