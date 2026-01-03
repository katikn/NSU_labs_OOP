#pragma once

#include <vector>
#include <string>
#include <stdexcept>


class BitArray {
private:
    static constexpr int BITS_PER_WORD = 8 * sizeof(uint64_t);
    std::vector<uint64_t> data;
    int num_bits;

    int word_index(int pos) const;
    int bit_offset(int pos) const;
    int num_words() const;
    void check_range(int pos) const;
    void check_size(const BitArray& other) const;

public:
    BitArray();
    explicit BitArray(int num_bits, unsigned long value = 0);

    BitArray(const BitArray& b);
    ~BitArray();

    void swap(BitArray& b);
    BitArray& operator=(const BitArray& b);

    void resize(int new_num_bits, bool value = false);
    void clear();
    void push_back(bool bit);

    BitArray& operator&=(const BitArray& b);
    BitArray& operator|=(const BitArray& b);
    BitArray& operator^=(const BitArray& b);

    BitArray& operator<<=(int n);
    BitArray& operator>>=(int n);
    BitArray operator<<(int n) const;
    BitArray operator>>(int n) const;

    BitArray& set(int n, bool val = true);
    BitArray& set();

    BitArray& reset(int n);
    BitArray& reset();

    bool any() const;
    bool none() const;
    BitArray operator~() const;
    int count() const;
    bool operator[](int i) const;

    int size() const;
    bool empty() const;
    std::string to_string() const;

    friend bool operator==(const BitArray& a, const BitArray& b);
    friend bool operator!=(const BitArray& a, const BitArray& b);
    friend BitArray operator&(const BitArray& b1, const BitArray& b2);
    friend BitArray operator|(const BitArray& b1, const BitArray& b2);
    friend BitArray operator^(const BitArray& b1, const BitArray& b2);
};

bool operator==(const BitArray& a, const BitArray& b);
bool operator!=(const BitArray& a, const BitArray& b);

BitArray operator&(const BitArray& b1, const BitArray& b2);
BitArray operator|(const BitArray& b1, const BitArray& b2);
BitArray operator^(const BitArray& b1, const BitArray& b2);