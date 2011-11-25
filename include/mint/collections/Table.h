/* ================================================================== *
 * Table
 * ================================================================== */

#ifndef MINT_COLLECTIONS_TABLE_H
#define MINT_COLLECTIONS_TABLE_H

#ifndef MINT_SUPPORT_ASSERTBASE_H
#include "mint/support/AssertBase.h"
#endif

//#ifndef MINT_COLLECTIONS_REFCOUNTABLE_H
//#include "mint/collections/RefCountable.h"
//#endif

namespace mint {

/** -------------------------------------------------------------------------
    Helper class that informs the Table how to handle keys of a given type.
 */
template<class KeyType>
struct DefaultKeyTraits {
  // Calculate a hash function for the key
  // static unsigned hash(const KeyType & key);

  // Equality test for the key
  // static unsigned equals(const KeyType & lkey, const KeyType & rkey);
};

/** -------------------------------------------------------------------------
    A probed hash table that holds references to garbage-collectable objects.
 */
template<typename Key, typename Value, typename KeyTraits = DefaultKeyTraits<Key> >
class Table {
public:
  typedef std::pair<Key *, Value *> value_type;

  /// Iterator class

  class iterator {
  public:
    typedef typename Table::value_type value_type;
    typedef ptrdiff_t difference_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef std::forward_iterator_tag iterator_category;

    /// Copy constructor
    iterator(const iterator & src) : _ptr(src._ptr), _end(src._end) {}

    /// Assignment
    const iterator & operator=(const iterator & src) {
      _ptr = src._ptr;
      _end = src._end;
      skipOverEmptyEntries();
      return *this;
    }

    /// Comparison
    bool operator==(const iterator & src) const {
      return _ptr == src._ptr;
    }

    /// Comparison
    bool operator!=(const iterator & src) const {
      return _ptr != src._ptr;
    }

    /// Pointer dereference operator
    value_type & operator*() const {
      return *_ptr;
    }

    /// Pointer dereference operator
    value_type * operator->() const {
      return _ptr;
    }

    /// Pre-increment
    iterator operator++() {
      _ptr += 1;
      skipOverEmptyEntries();
      return *this;
    }

    /// Post-increment
    iterator operator++(int) {
      iterator result(*this);
      ++(*this);
      return result;
    }

  private:
    friend class Table;
    //friend class Table::const_iterator;
    iterator(value_type * ptr, value_type * end) : _ptr(ptr), _end(end) {
      skipOverEmptyEntries();
    }

    void skipOverEmptyEntries() {
      Key * tombstoneKey = reinterpret_cast<Key *>(-1);
      while (_ptr < _end && (_ptr->first == NULL || _ptr->first == tombstoneKey)) {
        ++_ptr;
      }
    }

    value_type * _ptr;
    const value_type * _end;
  };

  /// Constant Iterator class

  class const_iterator {
  public:
    typedef const typename Table::value_type value_type;
    typedef ptrdiff_t difference_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef std::forward_iterator_tag iterator_category;

    /// Copy constructor
    const_iterator(const const_iterator & src) : _ptr(src._ptr), _end(src._end) {}
    const_iterator(const iterator & src) : _ptr(src._ptr), _end(src._end) {}

    /// Assignment
    const const_iterator & operator=(const const_iterator & src) {
      _ptr = src._ptr;
      _end = src._end;
      skipOverEmptyEntries();
      return *this;
    }

    const const_iterator & operator=(const iterator & src) {
      _ptr = src._ptr;
      _end = src._end;
      skipOverEmptyEntries();
      return *this;
    }

    /// Comparison
    bool operator==(const const_iterator & src) const {
      return _ptr == src._ptr;
    }

    /// Comparison
    bool operator!=(const const_iterator & src) const {
      return _ptr != src._ptr;
    }

    /// Pointer dereference operator
    const value_type & operator*() const {
      return *_ptr;
    }

    /// Pointer dereference operator
    const value_type * operator->() {
      return _ptr;
    }

    /// Pre-increment
    const_iterator operator++() {
      _ptr += 1;
      skipOverEmptyEntries();
      return *this;
    }

    /// Post-increment
    const_iterator operator++(int) {
      const_iterator result(*this);
      ++(*this);
      return result;
    }

  private:
    friend class Table;
    const_iterator(const value_type * ptr, const value_type * end) : _ptr(ptr), _end(end) {
      skipOverEmptyEntries();
    }

    void skipOverEmptyEntries() {
      Key * tombstoneKey = reinterpret_cast<Key *>(-1);
      while (_ptr < _end && (_ptr->first == NULL || _ptr->first == tombstoneKey)) {
        ++_ptr;
      }
    }

    const value_type * _ptr;
    const value_type * _end;
  };

  Table(size_t initialSize = 0) : _data(NULL), _dataSize(0), _size(0) {
    if (initialSize != 0) {
      size_t powerOfTwoSize = 16;   // Minimum size
      while (powerOfTwoSize < initialSize) {
        powerOfTwoSize <<= 1;
      }
      _data = new value_type[powerOfTwoSize]();
      _dataSize = powerOfTwoSize;
    }
  }

  ~Table() {
    delete _data;
  }

  /// Number of entries currently stored in the map.
  size_t size() const { return _size; }
  bool empty() const { return _size == 0; }

  // Iterators

  iterator begin() { return iterator(dataBegin(), dataEnd()); }
  const_iterator begin() const { return const_iterator(dataBegin(), dataEnd()); }
  iterator end() { return iterator(dataEnd(), dataEnd()); }
  const_iterator end() const { return const_iterator(dataEnd(), dataEnd()); }

  // Map operations

  iterator find(Key * key) {
    value_type * entry;
    if (lookup(key, entry)) {
      return iterator(entry, dataEnd());
    }
    return end();
  }
  const_iterator find(Key * key) const {
    value_type * entry;
    if (lookup(key, entry)) {
      return const_iterator(entry, dataEnd());
    }
    return end();
  }

  /// An alternate form of 'find' which can accept a different type of key
  /// so long as KeyTraits knows how to hash the key and compare it with
  /// the regular key type. This allows us to do searches with keys that may
  /// be much less expensive to create.
  template<class AltKey>
  const_iterator find_as(const AltKey & key) const {
    value_type * entry;
    if (lookup_as(key, entry)) {
      return const_iterator(entry, dataEnd());
    }
    return end();
  }

  /// Inserts the key and value into the map if it's not already in the map.
  /// If it is, it returns false and doesn't update the value.
  std::pair<iterator, bool> insert(const value_type & value) {
    value_type * entry;
    if (!lookup(value.first, entry)) {
      put(value.first, value.second, entry);
      return std::make_pair(iterator(entry, dataEnd()), true);
    }
    return std::make_pair(iterator(entry, dataEnd()), false);
  }

  /// Insert a range of elements
  template <typename InIter>
  void insert(InIter first, InIter last) {
    while (first < last) {
      insert(*first);
    }
  }

  /// Element access operator
  Value *& operator[](Key * key) {
    value_type * entry;
    if (lookup(key, entry)) {
      return entry->second;
    }
    put(key, NULL, entry);
    return entry->second;
  }

private:
  value_type * dataBegin() const { return &_data[0]; }
  value_type * dataEnd() const { return &_data[_dataSize]; }

  /// Attempt to find an existing entry that matches 'key'. If found, return true, along with
  /// a pointer to that entry. If not found, return false along with an unoccupied slot which
  /// can hold the new entry. If an unoccupied slot cannot be found, then return dataEnd().
  bool lookup(Key * key, value_type *& slot) const {
    if (_dataSize > 0) {
      unsigned index = KeyTraits::hash(key) & (_dataSize - 1);
      unsigned increment = 7;
      value_type * tombstone = NULL;
      Key * tombstoneKey = reinterpret_cast<Key *>(-1);
      for (;;) {
        value_type * e = &_data[index];
        if (e->first == NULL) {
          // Empty slot
          slot = e;
          return false;
        } else if (e->first == tombstoneKey) {
          // Tombstone slot - save for later
          if (!tombstone) {
            tombstone = e;
          }
        } else if (KeyTraits::equals(e->first, key)) {
          // Matching slot - return true
          slot = e;
          return true;
        }
        index = (index + increment) & (_dataSize - 1);
      }

      if (tombstone) {
        slot = tombstone;
        return false;
      };
    }

    slot = dataEnd();
    return false;
  }

  /// Similar to lookup(), except that it accepts keys of a different type, which can
  /// be compared with the normal type keys.
  template<class AltKey>
  bool lookup_as(const AltKey & key, value_type *& slot) const {
    if (_dataSize > 0) {
      unsigned index = KeyTraits::hash(key) & (_dataSize - 1);
      unsigned increment = 7;
      value_type * tombstone = NULL;
      Key * tombstoneKey = reinterpret_cast<Key *>(-1);
      for (;;) {
        value_type * e = &_data[index];
        if (e->first == NULL) {
          // Empty slot
          slot = e;
          return false;
        } else if (e->first == tombstoneKey) {
          // Tombstone slot - save for later
          if (!tombstone) {
            tombstone = e;
          }
        } else if (KeyTraits::equals(e->first, key)) {
          // Matching slot - return true
          slot = e;
          return true;
        }
        index = (index + increment) & (_dataSize - 1);
      }

      if (tombstone) {
        slot = tombstone;
        return false;
      };
    }

    slot = dataEnd();
    return false;
  }

  /// Insert the key and value into the specified table entry. If the entry == dataEnd()
  /// it means we couldn't find an empty slot within the limit of probing, in which case
  /// it's time to grow the table.
  void put(Key * key, Value * value, value_type *& slot) {
    if ((_size + 1) * 3 > _dataSize * 2) {
      grow();
      lookup(key, slot);
    }
    M_ASSERT_BASE(slot != dataEnd());
    slot->first = key;
    slot->second = value;
    ++_size;
  }

  /// Double the size of the table.
  void grow() {
    const value_type * s = &_data[0];
    const value_type * e = &_data[_dataSize];
    for (;;) {
      _dataSize = _dataSize == 0 ? 16 : _dataSize * 2;
      _data = new value_type[_dataSize]();
      // Insert all the old entries
      if (rehash(s, e)) {
        break;
      }
    }
  }

  /// Insert all of the occupied entries from s through e into the table. Returns false
  /// if any entry required too many probings.
  bool rehash(const value_type * s, const value_type * e) {
    while (s < e) {
      if (s->first != NULL && s->first != reinterpret_cast<Key *>(-1)) {
        value_type * slot;
        lookup(s->first, slot);
        if (slot == dataEnd()) {
          return false;
        }

        *slot = *s;
      }
      ++s;
    }
    return true;
  }

  value_type * _data;
  size_t _dataSize;
  size_t _size;
};

}

#endif // MINT_COLLECTIONS_TABLE_H
