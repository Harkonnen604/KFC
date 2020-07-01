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

    bool operator<(const IteratorBase& other) { assert(deque == other.deque); return index < other.index; }
    bool operator==(const IteratorBase& other) { assert(deque == other.deque); return index == other.index; }
    bool operator!=(const IteratorBase& other) { assert(deque == other.deque); return index != other.index; }
    IteratorBase& operator++() { ++index; return *this; }
    IteratorBase operator++(int) { IteratorBase copy(*this); ++*this; return copy; }
    IteratorBase& operator--() { --index; return *this; }
    IteratorBase operator--(int) { IteratorBase copy(*this); --*this; return copy; }
    IteratorBase& operator+=(uint32_t n) { index += n; return *this; }
    IteratorBase& operator-=(uint32_t n) { index -= n; return *this; }
    IteratorBase operator+(IteratorBase other) { return IteratorBase(deque, index + other.index); }
    IteratorBase operator+(uint32_t n) { return IteratorBase(deque, index + n); }
    uint32_t operator-(IteratorBase other) const { return index - other.index; }
    IteratorBase operator-(uint32_t n) { return IteratorBase(deque, index - n); }
    Type& operator[](uint32_t i) { return (*deque)[index + i]; }
    Type& operator*() { return (*deque)[index]; }
    Type* operator->() { return &(*deque)[index]; }

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
    clear();
    shrink_to_fit();
  }

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, size()); }
  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, size()); }
  reverse_iterator rbegin() { return reverse_iterator(iterator(this, size())); }
  reverse_iterator rend() { return reverse_iterator(iterator(this, uint32_t(0))); }

  CompactDeque& operator=(const CompactDeque& other);
  CompactDeque& operator=(CompactDeque&& other) noexcept
  {
    clear();
    std::swap(_data, other._data);
    std::swap(_capacity, other._capacity);
    std::swap(_head, other._head);
    std::swap(_size, other._size);
    return *this;
  }

  bool empty() const { return _size == 0; }
  void clear();
  uint32_t capacity() const { return _capacity; }
  uint32_t size() const { return _size; }
  void reserve(uint32_t capacity);
  void shrink_to_fit();
  void resize(uint32_t size);

  template <typename ... Args>
  T& emplace_front(Args&& ... args)
  {
    if (_size == _capacity)
      grow(_capacity ? _capacity * 2 : minimalAllocation);
    if (--_head == uint32_t(-1))
      _head = _capacity - 1;
    _size++;
    return *new (_data + _head)T(std::forward<Args>(args) ...);
  }

  template <typename ... Args>
  T& emplace_back(Args&& ... args)
  {
    if (_size == _capacity)
      grow(_capacity ? _capacity * 2 : minimalAllocation);
    return *new (_data + wrap(_head + _size++))T(std::forward<Args>(args) ...);
  }

  template <typename ... Args>
  T& emplace(uint32_t index, Args&& ... args);

  void pop_front()
  {
    assert(!empty());
    _data[_head].~T();
    if (++_head == _capacity)
      _head = 0;
    if (--_size == 0)
      _head = 0;
  }

  void pop_back()
  {
    assert(!empty());
    _data[wrap(_head + _size - 1)].~T();
    if (--_size == 0)
      _head = 0;
  }

  void erase(uint32_t index);
  void erase(uint32_t first, uint32_t last);
  void erase(iterator first, iterator last) { erase(first.index, last.index); }

  T& front() { assert(!empty()); return _data[_head]; }
  const T& front() const { assert(!empty()); return _data[_head]; }

  T& back() { assert(!empty()); return _data[wrap(_head + _size - 1)]; }
  const T& back() const { assert(!empty()); return _data[wrap(_head + _size - 1)]; }

  T& operator[](uint32_t index)
  {
    assert(index <= _size);
    return _data[wrap(_head + index)];
  }

  const T& operator[](uint32_t index) const
  {
    assert(index <= _size);
    return _data[wrap(_head + index)];
  }

private:
  void move(uint32_t src, uint32_t dst, uint32_t n, bool forward);

  void grow(uint32_t capacity);

  uint32_t wrap(uint32_t index) const
  {
    assert(index < _capacity * 2);
    if (index >= _capacity)
      index -= _capacity;
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
  clear();
  reserve(other.size());
  for (uint32_t i = 0; i < other.size(); i++)
    emplace_back(other[i]);
  return *this;
}

template <class T>
void CompactDeque<T>::clear()
{
  while (!empty())
    pop_front();
}

template <class T>
void CompactDeque<T>::reserve(uint32_t capacity)
{
  if (capacity <= _capacity)
    return;
  uint32_t newCapacity;
  for (newCapacity = 1; newCapacity < capacity; newCapacity <<= 1)
    ;
  grow(newCapacity);
}

template <class T>
void CompactDeque<T>::shrink_to_fit()
{
  if (empty())
    grow(0);
  else
  {
    uint32_t newCapacity;
    for (newCapacity = 1; newCapacity < _size; newCapacity <<= 1)
      ;
    if (newCapacity < _capacity)
      grow(newCapacity);
  }
}

template <class T>
void CompactDeque<T>::resize(uint32_t size)
{
  reserve(size);
  while (_size < size)
    emplace_back();
  while (_size > size)
    pop_back();
}

template <class T>
template <typename ... Args>
T& CompactDeque<T>::emplace(uint32_t index, Args&& ... args)
{
  assert(index <= _size);
  if (index == 0)
    return emplace_front(std::forward<Args>(args) ...);
  if (index == _size)
    return emplace_back(std::forward<Args>(args) ...);
  if (_size == _capacity)
    grow(_capacity ? _capacity * 2 : minimalAllocation);
  if (index * 2 >= _size)
  {
    new (_data + wrap(_head + _size))T(std::move(_data[wrap(_head + _size - 1)]));
    if (index < _size - 1)
      move(wrap(_head + index), wrap(_head + index + 1), _size - index - 1, true);
  }
  else
  {
    _head = _head ? _head - 1 : _capacity - 1;
    new (_data + _head)T(std::move(_data[wrap(_head + 1)]));
    if (index > 1)
      move(wrap(_head + 2), wrap(_head + 1), index - 1, false);
  }

  _size++;

  T* element = _data + wrap(_head + index);
  element->~T();
  return *new (element) T(std::forward<Args>(args) ...);
}

template <class T>
void CompactDeque<T>::erase(uint32_t index)
{
  assert(index < _size);
  if (index == 0)
  {
    pop_front();
    return;
  }
  if (index == _size - 1)
  {
    pop_back();
    return;
  }
  if (index * 2 >= _size)
  {
    move(wrap(_head + index + 1), wrap(_head + index), _size - index - 1, false);
    pop_back();
  }
  else
  {
    move(_head, wrap(_head + 1), index, true);
    pop_front();
  }
}

template <class T>
void CompactDeque<T>::erase(uint32_t first, uint32_t last)
{
  assert(first <= last && last <= _size);
  if (first == last)
    return;
  if (last < _size)
    move(wrap(_head + last), wrap(_head + first), _size - last, false);
  for (uint32_t n = last - first; n; n--)
    pop_back();
}

template <class T>
void CompactDeque<T>::move(uint32_t source, uint32_t destination, uint32_t count, bool forward)
{
  assert(count > 0 && count < _capacity);

  uint32_t firstPartSize;

  if (source <= wrap(source + count - 1))
  {
    if (destination <= wrap(destination + count - 1))
    {
      if (forward)
        std::move_backward(_data + source, _data + (source + count), _data + (destination + count));
      else
        std::move(_data + source, _data + (source + count), _data + destination);
      return;
    }
    firstPartSize = _capacity - destination;
  }
  else
    firstPartSize = _capacity - source;

  assert(firstPartSize > 0 && firstPartSize < count);

  if (forward)
  {
    move(wrap(source + firstPartSize), wrap(destination + firstPartSize), count - firstPartSize, forward);
    move(source, destination, firstPartSize, forward);
  }
  else
  {
    move(source, destination, firstPartSize, forward);
    move(wrap(source + firstPartSize), wrap(destination + firstPartSize), count - firstPartSize, forward);
  }
}

template <class T>
void CompactDeque<T>::grow(uint32_t capacity)
{
  if (capacity == 0)
  {
    assert(empty());
    free(_data);
    _data = nullptr;
    _capacity = 0;
    _head = 0;
    return;
  }

  T* newData = (T*)malloc(capacity * sizeof(T));

  for (uint32_t i = 0; i < _size; i++)
    new (newData + i)T(std::move((*this)[i])), (*this)[i].~T();

  free(_data);

  _data = newData;
  _capacity = capacity;
  _head = 0;
}
