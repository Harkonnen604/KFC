#pragma once
#include <cassert>
#include <cstdint>
#include <iterator>
#include <utility>

template <class T>
class CompactDeque
{
  template <typename Type, typename DequeType>
  class IteratorBase
  {
    friend class CompactDeque;
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Type;
    using difference_type = uint32_t;
    using pointer = Type*;
    using reference = Type&;

    IteratorBase() = default;
    IteratorBase(DequeType* deque, uint32_t index) : deque(deque), index(index) {}

    bool operator<(const IteratorBase& other) { assert(this->deque == other.deque); return this->index < other.index; }
    bool operator==(const IteratorBase& other) { assert(this->deque == other.deque); return this->index == other.index; }
    bool operator!=(const IteratorBase& other) { assert(this->deque == other.deque); return this->index != other.index; }
    IteratorBase& operator++() { ++this->index; return *this; }
    IteratorBase operator++(int) { IteratorBase copy(*this); ++*this; return copy; }
    IteratorBase& operator--() { --this->index; return *this; }
    IteratorBase operator--(int) { IteratorBase copy(*this); --*this; return copy; }
    IteratorBase& operator+=(uint32_t n) { this->index += n; return *this; }
    IteratorBase& operator-=(uint32_t n) { this->index -= n; return *this; }
    IteratorBase operator+(IteratorBase other) { return IteratorBase(this->deque, this->index + other.index); }
    IteratorBase operator+(uint32_t n) { return IteratorBase(this->deque, this->index + n); }
    uint32_t operator-(IteratorBase other) const { return this->index - other.index; }
    IteratorBase operator-(uint32_t n) { return IteratorBase(this->deque, this->index - n); }
    Type& operator[](uint32_t i) { return (*this->deque)[this->index + i]; }
    Type& operator*() { return (*this->deque)[this->index]; }
    Type* operator->() { return &(*this->deque)[this->index]; }

  private:
    DequeType* deque = nullptr;
    uint32_t index = 0;
  };

public:
  using iterator = IteratorBase<T, CompactDeque>;
  using const_iterator = IteratorBase<const T, const CompactDeque>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  explicit CompactDeque() = default;
  explicit CompactDeque(const CompactDeque& other) { *this = other; }
  CompactDeque(CompactDeque&& other) noexcept { *this = std::move(other); }

  ~CompactDeque() noexcept
  {
    this->clear();
    this->shrink_to_fit();
  }

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, this->size()); }
  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, this->size()); }
  reverse_iterator rbegin() { return reverse_iterator(iterator(this, this->size())); }
  reverse_iterator rend() { return reverse_iterator(iterator(this, uint32_t(0))); }

  CompactDeque& operator=(const CompactDeque& other);
  CompactDeque& operator=(CompactDeque&& other) noexcept
  {
    this->clear();
    std::swap(this->_data, other._data);
    std::swap(this->_capacity, other._capacity);
    std::swap(this->_head, other._head);
    std::swap(this->_size, other._size);
    return *this;
  }

  bool empty() const { return this->_size == 0; }
  void clear();
  uint32_t capacity() const { return this->_capacity; }
  uint32_t size() const { return this->_size; }
  void reserve(uint32_t capacity);
  void shrink_to_fit();
  void resize(uint32_t size);

  template <typename ... Args>
  T& emplace_front(Args&& ... args)
  {
    if (this->_size == this->_capacity)
      this->grow(this->_capacity ? this->_capacity * 2 : minimalAllocation);
    if (--this->_head == uint32_t(-1))
      this->_head = this->_capacity - 1;
    this->_size++;
    return *new (this->_data + this->_head)T(std::forward<Args>(args) ...);
  }

  template <typename ... Args>
  T& emplace_back(Args&& ... args)
  {
    if (this->_size == this->_capacity)
      this->grow(this->_capacity ? this->_capacity * 2 : minimalAllocation);
    return *new (this->_data + this->wrap(this->_head + this->_size++))T(std::forward<Args>(args) ...);
  }

  template <typename ... Args>
  T& emplace(uint32_t index, Args&& ... args);

  void pop_front()
  {
    assert(!this->empty());
    this->_data[this->_head].~T();
    if (++this->_head == this->_capacity)
      this->_head = 0;
    if (--this->_size == 0)
      this->_head = 0;
  }

  void pop_back()
  {
    assert(!this->empty());
    this->_data[this->wrap(this->_head + this->_size - 1)].~T();
    if (--this->_size == 0)
      this->_head = 0;
  }

  void erase(uint32_t index);
  void erase(uint32_t first, uint32_t last);
  void erase(iterator first, iterator last) { this->erase(first.index, last.index); }

  T& front() { assert(!this->empty()); return this->_data[this->_head]; }
  const T& front() const { assert(!this->empty()); return this->_data[this->_head]; }

  T& back() { assert(!this->empty()); return this->_data[this->wrap(this->_head + this->_size - 1)]; }
  const T& back() const { assert(!this->empty()); return this->_data[this->wrap(this->_head + this->_size - 1)]; }

  T& operator[](uint32_t index)
  {
    assert(index <= _size);
    return this->_data[this->wrap(this->_head + index)];
  }

  const T& operator[](uint32_t index) const
  {
    assert(index <= _size);
    return this->_data[this->wrap(this->_head + index)];
  }

private:
  void move(uint32_t src, uint32_t dst, uint32_t n, bool forward);

  void grow(uint32_t capacity);

  uint32_t wrap(uint32_t index) const
  {
    assert(index < _capacity * 2);
    if (index >= this->_capacity)
      index -= this->_capacity;
    return index;
  }

  static constexpr uint32_t minimalAllocation = 4;

  T* _data = nullptr;
  uint32_t _capacity = 0;
  uint32_t _head = 0;
  uint32_t _size = 0;
};

template <class T>
CompactDeque<T>& CompactDeque<T>::operator=(const CompactDeque& other)
{
  this->clear();
  this->reserve(other.size());
  for (uint32_t i = 0; i < other.size(); i++)
    this->emplace_back(other[i]);
  return *this;
}

template <class T>
void CompactDeque<T>::clear()
{
  while (!this->empty())
    this->pop_front();
}

template <class T>
void CompactDeque<T>::reserve(uint32_t capacity)
{
  if (capacity <= this->_capacity)
    return;
  uint32_t newCapacity;
  for (newCapacity = 1; newCapacity < capacity; newCapacity <<= 1)
    ;
  this->grow(newCapacity);
}

template <class T>
void CompactDeque<T>::shrink_to_fit()
{
  if (this->empty())
    this->grow(0);
  else
  {
    uint32_t newCapacity;
    for (newCapacity = 1; newCapacity < this->_size; newCapacity <<= 1)
      ;
    if (newCapacity < this->_capacity)
      this->grow(newCapacity);
  }
}

template <class T>
void CompactDeque<T>::resize(uint32_t size)
{
  this->reserve(size);
  while (this->_size < size)
    this->emplace_back();
  while (this->_size > size)
    this->pop_back();
}

template <class T>
template <typename ... Args>
T& CompactDeque<T>::emplace(uint32_t index, Args&& ... args)
{
  assert(index <= this->_size);
  if (index == 0)
    return this->emplace_front(std::forward<Args>(args) ...);
  if (index == _size)
    return this->emplace_back(std::forward<Args>(args) ...);
  if (this->_size == this->_capacity)
    this->grow(this->_capacity ? this->_capacity * 2 : minimalAllocation);
  if (index * 2 >= this->_size)
  {
    new (this->_data + this->wrap(this->_head + this->_size))T(std::move(this->_data[this->wrap(this->_head + this->_size - 1)]));
    if (index < this->_size - 1)
      this->move(this->wrap(this->_head + index), this->wrap(_head + index + 1), this->_size - index - 1, true);
  }
  else
  {
    this->_head = this->_head ? this->_head - 1 : this->_capacity - 1;
    new (this->_data + this->_head)T(std::move(this->_data[this->wrap(this->_head + 1)]));
    if (index > 1)
      this->move(this->wrap(this->_head + 2), this->wrap(this->_head + 1), index - 1, false);
  }

  this->_size++;

  T* element = this->_data + this->wrap(this->_head + index);
  element->~T();
  return *new (element) T(std::forward<Args>(args) ...);
}

template <class T>
void CompactDeque<T>::erase(uint32_t index)
{
  assert(index < this->_size);
  if (index == 0)
  {
    this->pop_front();
    return;
  }
  if (index == this->_size - 1)
  {
    this->pop_back();
    return;
  }
  if (index * 2 >= this->_size)
  {
    this->move(this->wrap(this->_head + index + 1), this->wrap(this->_head + index), this->_size - index - 1, false);
    this->pop_back();
  }
  else
  {
    this->move(this->_head, this->wrap(this->_head + 1), index, true);
    this->pop_front();
  }
}

template <class T>
void CompactDeque<T>::erase(uint32_t first, uint32_t last)
{
  assert(first <= last && last <= this->_size);
  if (first == last)
    return;
  if (last < this->_size)
    this->move(this->wrap(this->_head + last), this->wrap(this->_head + first), this->_size - last, false);
  for (uint32_t n = last - first; n; n--)
    this->pop_back();
}

template <class T>
void CompactDeque<T>::move(uint32_t source, uint32_t destination, uint32_t count, bool forward)
{
  assert(count > 0 && count < this->_capacity);

  uint32_t firstPartSize;

  if (source <= this->wrap(source + count - 1))
  {
    if (destination <= wrap(destination + count - 1))
    {
      if (forward)
        std::move_backward(this->_data + source, this->_data + (source + count), this->_data + (destination + count));
      else
        std::move(this->_data + source, this->_data + (source + count), this->_data + destination);
      return;
    }
    firstPartSize = this->_capacity - destination;
  }
  else
    firstPartSize = this->_capacity - source;

  assert(firstPartSize > 0 && firstPartSize < count);

  if (forward)
  {
    this->move(this->wrap(source + firstPartSize), this->wrap(destination + firstPartSize), count - firstPartSize, forward);
    this->move(source, destination, firstPartSize, forward);
  }
  else
  {
    this->move(source, destination, firstPartSize, forward);
    this->move(this->wrap(source + firstPartSize), this->wrap(destination + firstPartSize), count - firstPartSize, forward);
  }
}

template <class T>
void CompactDeque<T>::grow(uint32_t capacity)
{
  if (capacity == 0)
  {
    assert(this->empty());
    free(this->_data);
    this->_data = nullptr;
    this->_capacity = 0;
    this->_head = 0;
    return;
  }

  T* newData = (T*)malloc(capacity * sizeof(T));

  for (uint32_t i = 0; i < this->_size; i++)
    new (newData + i)T(std::move((*this)[i])), (*this)[i].~T();

  free(this->_data);

  this->_data = newData;
  this->_capacity = capacity;
  this->_head = 0;
}
