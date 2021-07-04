#pragma once
#include <cstddef>

template <typename T>
struct vector {
	using iterator = T*;
	using const_iterator = T const*;

	vector() = default; // O(1) nothrow

	vector(vector const& other) : vector() {
		change_buffer(other.size_, other);
	} // O(N) strong

	vector& operator=(vector const& other) {
		if (&other == this) {
			return *this;
		}
		vector tmp(other);
		swap(tmp);
		return *this;
	} // O(N) strong

	~vector() {
		clear();
		operator delete(data_);
	} // O(N) nothrow

	T& operator[](size_t i) {
		return data_[i];
	} // O(1) nothrow

	T const& operator[](size_t i) const {
		return data_[i];
	} // O(1) nothrow

	T* data() {
		return data_;
	} // O(1) nothrow
	T const* data() const {
		return data_;
	} // O(1) nothrow
	size_t size() const {
		return size_;
	} // O(1) nothrow

	T& front() {
		return data_[0];
	} // O(1) nothrow
	T const& front() const {
		return data_[0];
	} // O(1) nothrow

	T& back() {
		return data_[size_ - 1];
	} // O(1) nothrow
	T const& back() const {
		return data_[size_ - 1];
	} // O(1) nothrow
	void push_back(T const& val) {
		if (capacity_ == size_) {
			T tmp = val;
			change_buffer(capacity_ == 0 ? 1 : capacity_ * 2);
			add_element_without_check(tmp);
		} else {
			add_element_without_check(val);
		}
	} // O(1)* strong
	void pop_back() {
		size_--;
		data_[size_].~T();
	} // O(1) nothrow

	bool empty() const {
		return (size_ == 0);
	} // O(1) nothrow

	size_t capacity() const {
		return capacity_;
	} // O(1) nothrow
	void reserve(size_t new_cap) {
		change_buffer(std::max(new_cap, capacity_));
	} // O(N) strong
	void shrink_to_fit() {
		change_buffer(size_);
	} // O(N) strong

	void clear() {
		clean_prefix(size_, data_);
		size_ = 0;
	} // O(N) nothrow

	void swap(vector& other) {
		std::swap(other.size_, size_);
		std::swap(other.capacity_, capacity_);
		std::swap(other.data_, data_);
	} // O(1) nothrow

	iterator begin() {
		return data_;
	} // O(1) nothrow
	iterator end() {
		return data_ + size_;
	} // O(1) nothrow

	const_iterator begin() const {
		return data_;
	} // O(1) nothrow
	const_iterator end() const {
		return data_ + size_;
	} // O(1) nothrow

	iterator insert(const_iterator pos, T const& val) {
		size_t index = pos - begin();
		push_back(val);
		for (size_t i = size_ - 1; i > index; i--) {
			std::swap(data_[i], data_[i - 1]);
		}
		return begin() + index;
	} // O(N) strong

	iterator erase(const_iterator pos) {
		return erase(pos, pos + 1);
	} // O(N) nothrow(swap)

	iterator erase(const_iterator first, const_iterator last) {
		size_t first_index = first - begin();
		size_t len = last - first;

		for (size_t i = first_index; i + len < size(); ++i) {
			std::swap(data_[i], data_[i + len]);
		}
		for (size_t i = 0; i < len; ++i) {
			pop_back();
		}
		return begin() + first_index;
	} // O(N) nothrow(swap)

private:
	T* data_{nullptr};
	size_t size_{0};
	size_t capacity_{0};

	void add_element_without_check(T const& val) {
		new (data_ + size_) T(val);
		size_++;
	}

	void change_buffer(size_t new_capacity, vector const& other) {
		T* new_data = nullptr;
		if (new_capacity != 0) {
			new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
			for (size_t i = 0; i != other.size_; ++i) {
				try {
					new (new_data + i) T(other[i]);
				} catch (...) {
					clean_prefix(i, new_data);
					operator delete(new_data);
					throw;
				}
			}
		}
		clean_prefix(size_, data_);
		operator delete(data_);

		data_ = new_data;
		size_ = other.size_;
		capacity_ = new_capacity;
	}

	void change_buffer(size_t new_capacity) {
		if (capacity_ == new_capacity) {
			return;
		}
		change_buffer(new_capacity, *this);
	}

	void clean_prefix(size_t ind, T* data) {
		for (; ind != 0; ind--) {
			data[ind - 1].~T();
		}
	}
};
