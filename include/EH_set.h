#ifndef EH_SET_H
#define EH_SET_H

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iostream>

#if defined(DEBUG)
#define TRACE(x) std::cerr << x << std::endl;
#else
#define TRACE(x)
#endif

template <typename Key, size_t N = 16> class EH_set {
  public:
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = Iterator;
    using iterator = const_iterator;
    using key_equal = std::equal_to<key_type>;
    using hasher = std::hash<key_type>;

  private:
    struct Bucket {
        key_type elements[N];
        size_type l{0};     // local depth
        size_type arrsz{0}; // number of elems in Bucket

        size_type append(const key_type &elem);
        size_type find(const key_type &elem) const;
        size_type remove(const key_type &elem);
        size_type high_bit() const;
    };

    size_type sz; // actual size
    size_type d;  // global depth
    size_type nD; // 2^d
    Bucket **buckets;

    void expansion();
    void split_bucket(size_type hash);
    iterator add(key_type k, bool check = true);

  public:
    EH_set();
    EH_set(std::initializer_list<key_type> ilist);
    template <typename InputIt> EH_set(InputIt first, InputIt last);
    EH_set(const EH_set &other);

    ~EH_set();

    EH_set &operator=(const EH_set &other);
    EH_set &operator=(std::initializer_list<key_type> ilist);

    size_type size() const;
    bool empty() const;

    void insert(std::initializer_list<key_type> ilist);
    std::pair<iterator, bool> insert(const key_type &key);
    template <typename InputIt> void insert(InputIt first, InputIt last);

    void clear();

    size_type erase(const key_type &key);
    size_type count(const key_type &key) const;
    iterator find(const key_type &key) const;

    void swap(EH_set &other);

    const_iterator begin() const;
    const_iterator end() const;

    void dump(std::ostream &o = std::cerr) const;

    // goes through every key in lhs once and calls count for rhs
    // O(lhs.sz)
    friend bool operator==(const EH_set &lhs, const EH_set &rhs) {
        if (lhs.sz != rhs.sz)
            return false;
        for (const auto &key : rhs)
            if (!lhs.count(key))
                return false;
        return true;
    }
    friend bool operator!=(const EH_set &lhs, const EH_set &rhs) {
        return !(lhs == rhs);
    }
};

/*--------------------------Bucket methods----------------------------*/

// Append Element to Bucket
// returns 1 if Element could be inserted, 0 otherwise
// O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::size_type
EH_set<Key, N>::Bucket::append(const key_type &elem) {
    if (arrsz == N)
        return 0;
    elements[arrsz++] = elem;
    return 1;
}

// find Element in Bucket
// returns index of Element in Bucket, if found, and N otherwise
// O(N) = O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::size_type
EH_set<Key, N>::Bucket::find(const key_type &elem) const {
    for (size_type i{0}; i < arrsz; ++i)
        if (key_equal{}(elem, elements[i]))
            return i;
    return N;
}

// Remove Element in Bucket
// swap with last element and decrease size
// O(N) = O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::size_type
EH_set<Key, N>::Bucket::remove(const key_type &elem) {
    for (size_type i{0}; i < arrsz; ++i)
        if (key_equal{}(elem, elements[i])) {
            if (i != --arrsz)
                std::swap(elements[i], elements[arrsz]);
            return 1;
        }
    return 0;
}

// returns highest bit that bucket elems agree on
template <typename Key, size_t N>
typename EH_set<Key, N>::size_type EH_set<Key, N>::Bucket::high_bit() const {
    return 1 << l;
}

/*------------------------private methods---------------------*/

// May call expansion and split multiple times
// O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::iterator EH_set<Key, N>::add(key_type k, bool check) {
    size_type hash = hasher{}(k) & (nD - 1);
    size_type idx{0};
    if (check && (idx = buckets[hash]->find(k)) != N)
        return iterator(idx, hash, this); // if already inside, skip

    while (true) { // while key can't be inserted
        if (buckets[hash]->append(k)) {
            sz++; // successful insert
            return iterator(buckets[hash]->arrsz - 1, hash, this);
        } else {
            // bucket overflow, split (and expansion) necessary
            split_bucket(hash);
            hash = hasher{}(k) & (nD - 1);
        }
    }
}

// doubles the pointer array
// O(1)
template <typename Key, size_t N> void EH_set<Key, N>::expansion() {
    size_type new_nD = 1 << ++d;
    Bucket **new_buckets{new Bucket *[new_nD]};
    for (size_type i{0}; i < nD; ++i) {
        new_buckets[i] = buckets[i];
        new_buckets[i + nD] = buckets[i]; // pointer repeat with offset nD
    }
    delete[] buckets;
    buckets = new_buckets;
    nD = new_nD;
}

// Split Bucket buckets[hash] and reassign pointers
// O(N) = O(1)
template <typename Key, size_t N>
void EH_set<Key, N>::split_bucket(size_type hash) {
    Bucket *b = buckets[hash];
    if (b->l >= d) // ensure there is enough space to split
        expansion();

    b->arrsz = 0;             // clear Bucket
    Bucket *b1{new Bucket{}}; // 1 prefix
    b1->l = ++b->l;           // l increases by 1

    // rehash every Element from original Bucket
    // only the l+1 least significant bit must be checked, so rbitshift by l and
    // test if bit is set no temp copy needed, since we always check after newly
    // added elements
    for (size_type i{0}; i < N; ++i)
        (hasher{}(b->elements[i])) >> (b->l - 1) & 1
            ? b1->append(b->elements[i])
            : b->append(b->elements[i]);

    // get first index that should point to new Bucket (first pointer points to
    // Original, so first pointer + offset points to new)
    size_type offset{1ULL << (b->l - 1)};
    size_type first{
        (hash & (offset - 1)) +
        offset}; // first Pointer from original Bucket is hash value % offset
    offset += offset; // double old offset (offset for new buckets)

    // assign every pointer that should point to new Bucket (2 * original
    // offset)
    for (; first < nD; first += offset)
        buckets[first] = b1;
}

/*---------------------------EH_set methods-----------------------------*/

// create empty set (empty set contains 1 Bucket)
// O(1)
template <typename Key, size_t N>
EH_set<Key, N>::EH_set() : sz{0}, d{0}, nD{1}, buckets{new Bucket *[nD]} {
    for (size_t i{0}; i < nD; ++i)
        buckets[i] = new Bucket{};
}

// calls it Constructor
// O(list size)
template <typename Key, size_t N>
EH_set<Key, N>::EH_set(std::initializer_list<key_type> ilist)
    : EH_set{std::begin(ilist), std::end(ilist)} {}

// calls list insert
// O(it range)
template <typename Key, size_t N>
template <typename InputIt>
EH_set<Key, N>::EH_set(InputIt first, InputIt last) : EH_set{} {
    insert(first, last);
}

// copies all elements from other set
// O(other.nD)
template <typename Key, size_t N>
EH_set<Key, N>::EH_set(const EH_set &other)
    : sz{other.sz}, d{other.d}, nD{other.nD}, buckets{new Bucket *[nD]} {
    for (size_t i{0}; i < nD; ++i) {
        if (other.buckets[i]->high_bit() > i) {
            buckets[i] = new Bucket{*other.buckets[i]};
        } else {
            buckets[i] = buckets[i & (other.buckets[i]->high_bit() - 1)];
        }
    }
}

// Destruktor
// find out if pointer is last pointer to bucket and delete
// O(nD)
template <typename Key, size_t N> EH_set<Key, N>::~EH_set() {
    for (size_t i{0}; i < nD; ++i)
        if (i >= nD - buckets[i]->high_bit())
            delete buckets[i];
    delete[] buckets;
}

// clear all values, without losing structure and insert keys
// O(nD + other.sz)
template <typename Key, size_t N>
EH_set<Key, N> &EH_set<Key, N>::operator=(const EH_set<Key, N> &other) {
    for (size_type i{0}; i < nD; ++i)
        buckets[i & (nD - 1)]->arrsz = 0;
    sz = 0;
    for (const key_type &key : other)
        add(key, false); // insert without checking the values
    return *this;
}

// clears all values, without losing structur and inserts ilist
// O(nD + list size)
template <typename Key, size_t N>
EH_set<Key, N> &
EH_set<Key, N>::operator=(std::initializer_list<key_type> ilist) {
    for (size_type i{0}; i < nD; ++i)
        buckets[i]->arrsz = 0;
    sz = 0;
    insert(ilist);
    return *this;
}

// O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::size_type EH_set<Key, N>::size() const {
    return sz;
}
// O(1)
template <typename Key, size_t N> bool EH_set<Key, N>::empty() const {
    return (sz == 0);
}

// insert list: calls iterator insert
// O(list size)
template <typename Key, size_t N>
void EH_set<Key, N>::insert(std::initializer_list<key_type> ilist) {
    if (!ilist.size())
        return;
    insert(std::begin(ilist), std::end(ilist));
}

// calls private method add
// O(1)
template <typename Key, size_t N>
std::pair<typename EH_set<Key, N>::iterator, bool>
EH_set<Key, N>::insert(const key_type &key) {
    size_type old_sz{sz};
    return {add(key), (old_sz != sz)};
}

// iterator insert calls private method add for every item
// O(range size)
template <typename Key, size_t N>
template <typename InputIt>
void EH_set<Key, N>::insert(InputIt first, InputIt last) {
    for (auto it{first}; it != last; ++it)
        add(*it);
}

// swap with empty set
// O(nD) (because Destruktor)
template <typename Key, size_t N> void EH_set<Key, N>::clear() {
    EH_set temp{};
    swap(temp);
}

// hash and call Bucket remove
// O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::size_type EH_set<Key, N>::erase(const key_type &key) {
    if (buckets[hasher{}(key) & (nD - 1)]->remove(key)) {
        --sz;
        return 1;
    }
    return 0;
}

// hash and call Bucket find
// O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::size_type
EH_set<Key, N>::count(const key_type &key) const {
    return buckets[hasher{}(key) & (nD - 1)]->find(key) == N ? 0 : 1;
}

// hash and call Bucket find
// O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::iterator
EH_set<Key, N>::find(const key_type &key) const {
    size_type idx = buckets[hasher{}(key) & (nD - 1)]->find(key);
    return idx != N ? iterator(idx, hasher{}(key) & (nD - 1), this) : end();
}

// just uses std::swap for every instance variable
// O(1)
template <typename Key, size_t N> void EH_set<Key, N>::swap(EH_set &other) {
    using std::swap;
    swap(d, other.d);
    swap(nD, other.nD);
    swap(sz, other.sz);
    swap(buckets, other.buckets);
}

// begin-iterator is first element of first Bucket
// O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::const_iterator EH_set<Key, N>::begin() const {
    return const_iterator(0, 0, this);
}
// end-iterator is first element of (nonexistent) nDth Bucket
// O(1)
template <typename Key, size_t N>
typename EH_set<Key, N>::const_iterator EH_set<Key, N>::end() const {
    return const_iterator(this);
}

// Outputs entire set to ostream
template <typename Key, size_t N>
void EH_set<Key, N>::dump(std::ostream &o) const {
    o << "Extendible Hashing <" << typeid(Key).name() << ',' << N
      << ">, d = " << d << ", nD = " << nD << ", sz = " << sz << '\n';
    // printing...
    for (size_type i{0}; i < nD; ++i) {
        Bucket *b = buckets[i];
        size_type orig_bucket = i & (buckets[i]->high_bit() - 1);
        o << i;
        if (orig_bucket != i)
            o << " ~~> " << orig_bucket; // if pointer isnt first to a bucket
                                         // show reference to first Bucket
        o << " --> [l = ";
        o << b->l << ", offset = " << (1 << b->l) << ", arrsz = " << b->arrsz
          << " | ";
        for (size_type j{0}; j < b->arrsz; ++j)
            o << b->elements[j] << ' ';
        o << "]\n";
    }
}

/*---------------------------Iterator Class-------------------------------*/

template <typename Key, size_t N> class EH_set<Key, N>::Iterator {
  public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;

  private:
    size_type idx{0};
    const EH_set *set{nullptr};
    size_type b{0};

    // skip to the next, first ptr, that is the first occurence, to point to a
    // non-empty Bucket
    void skip() {
        while (b >= set->buckets[b]->high_bit() ||
               set->buckets[b]->arrsz == 0) {
            b++;
            if (is_end()) // if we are at the end ptr, dont increase anymore
                break;
        }
    }

    // is iterator at the end?
    bool is_end() const { return b == set->nD; }

    // returns a pointer to the current element
    pointer ptr() const { return &set->buckets[b]->elements[idx]; }

  public:
    explicit Iterator(size_type idx, size_type b, const EH_set *set)
        : idx{idx}, set{set}, b{b & (set->buckets[b]->high_bit() - 1)} {
        skip();
    }

    Iterator(const EH_set *set)
        : idx{0}, set{set}, b{set->nD} {} // for end-iterator
    Iterator() : idx{0}, set{nullptr}, b{0} {}

    reference operator*() const { return *ptr(); }
    pointer operator->() const { return ptr(); }

    Iterator &operator++() {
        if (++idx < set->buckets[b]->arrsz)
            return *this;

        // if we are at the end of current bucket
        idx = 0;
        ++b;
        if (b != set->nD) // if we are at the end ptr, dont skip
            skip();       // skip to next valid ptr
        return *this;
    }

    Iterator operator++(int) {
        auto temp{*this};
        ++(*this);
        return temp;
    }

    // returns current position of iterator in format {Bucket number, Index in
    // Bucker}, for debugging
    std::pair<unsigned, unsigned> get_pos() const { return {b, idx}; }

    friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
        if (lhs.is_end() || rhs.is_end()) // if one of the iterators is the end
                                          // iterator, test if both are
            return lhs.is_end() && rhs.is_end();
        return (lhs.ptr() ==
                rhs.ptr()); // test if they point to the same element
    }
    friend bool operator!=(const Iterator &lhs, const Iterator &rhs) {
        return !(lhs == rhs);
    }
};

template <typename Key, size_t N>
void swap(EH_set<Key, N> &lhs, EH_set<Key, N> &rhs) {
    lhs.swap(rhs);
}

#endif // EH_SET_H
