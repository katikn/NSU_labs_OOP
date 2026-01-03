#include <gtest/gtest.h>
#include "../include/bitarray.h"
#include <stdexcept>
#include <string>

// тесты на конструктор
TEST(BitArray_ctor, DefaultConstructsEmpty) {
    BitArray b;
    EXPECT_EQ(b.size(), 0);
    EXPECT_TRUE(b.empty());
    EXPECT_EQ(b.to_string(), "");
}

TEST(BitArray_ctor, ConstructWithValue) {
    BitArray b(8, 0b10110011UL);
    EXPECT_EQ(b.size(), 8);
    EXPECT_EQ(b.to_string(), "10110011"); // msb..lsb
    EXPECT_EQ(b.count(), 5);
}

TEST(BitArray_ctor, NegativeSizeThrows) {
    EXPECT_THROW(BitArray(-1, 0), std::invalid_argument);
}

// copy, assign, swap
TEST(BitArray_copy, CopyConstructorAndAssign) {
    BitArray a(10, 0b1111);
    BitArray b(a); // copy ctor
    EXPECT_EQ(a, b);
    BitArray c;
    c = a; // assign
    EXPECT_EQ(a, c);
}

TEST(BitArray_swap, SwapContents) {
    BitArray a(4, 0b1010);
    BitArray b(4, 0b0101);
    a.swap(b);
    EXPECT_EQ(a.to_string(), "0101");
    EXPECT_EQ(b.to_string(), "1010");
}

// size / empty / clear / resize / push_back
TEST(BitArray_size_empty_clear, Basic) {
    BitArray b(3, 0);
    EXPECT_EQ(b.size(), 3);
    b.clear();
    EXPECT_EQ(b.size(), 0);
    EXPECT_TRUE(b.empty());
}

TEST(BitArray_resize_increase, IncreaseWithValueTrue) {
    BitArray b(3, 0);
    b.resize(6, true);
    EXPECT_EQ(b.size(), 6);
    EXPECT_EQ(b.count(), 3); // previously 0, new 3 ones
    // check string length and content of last bits
    EXPECT_EQ((int)b.to_string().size(), 6);
}

TEST(BitArray_resize_decrease, DecreaseAndTrim) {
    BitArray b(10, 0);
    b.set(); // all ones
    b.resize(5);
    EXPECT_EQ(b.size(), 5);
    EXPECT_EQ(b.count(), 5);
    // ensure no extra bits left
    for (int i = 0; i < b.size(); ++i) EXPECT_TRUE(b[i]);
}

TEST(BitArray_resize_negative, Throws) {
    BitArray b(2,0);
    EXPECT_THROW(b.resize(-5), std::invalid_argument);
}

TEST(BitArray_push_back, AppendBits) {
    BitArray b;
    b.push_back(true);
    b.push_back(false);
    b.push_back(true);
    EXPECT_EQ(b.size(), 3);
    EXPECT_EQ(b.to_string(), "101");
}

// set/reset
TEST(BitArray_set_reset_single, SingleBitOperations) {
    BitArray b(5, 0);
    b.set(0, true);
    b.set(4, true);
    EXPECT_TRUE(b[0]);
    EXPECT_TRUE(b[4]);
    EXPECT_EQ(b.count(), 2);
    b.reset(4);
    EXPECT_FALSE(b[4]);
    EXPECT_EQ(b.count(), 1);
}

TEST(BitArray_set_reset_all, SetAndResetAll) {
    BitArray b(7, 0);
    b.set();
    EXPECT_TRUE(b.any());
    EXPECT_EQ(b.count(), 7);
    b.reset();
    EXPECT_TRUE(b.none());
    EXPECT_EQ(b.count(), 0);
}

// operator[]
TEST(BitArray_index_range, OutOfRangeThrows) {
    BitArray b(4, 0);
    EXPECT_THROW(b[4], std::out_of_range);
    EXPECT_THROW(b[-1], std::out_of_range);
    EXPECT_THROW(b.set(100, true), std::out_of_range);
    EXPECT_THROW(b.reset(10), std::out_of_range);
}

// operator~
TEST(BitArray_not_and_masking, InversionAndMaskLastWord) {
    int n = 70;
    BitArray b(n, 0);
    b.set(); // all ones
    EXPECT_EQ(b.count(), n);
    BitArray inv = ~b;
    EXPECT_EQ(inv.count(), 0); // all inverted -> zeros
    // partial size
    BitArray a(5, 0b10101);
    BitArray inv2 = ~a;
    EXPECT_EQ(inv2.size(), 5);
    EXPECT_EQ(inv2.count(), 2);
}

// битовые операции
TEST(BitArray_bitwise_and_or_xor, OperatorsResult) {
    BitArray a(8, 0b10101010);
    BitArray b(8, 0b11001100);
    BitArray andr = a & b;
    BitArray orr = a | b;
    BitArray xorr = a ^ b;
    EXPECT_EQ(andr, BitArray(8, 0b10001000));
    EXPECT_EQ(orr, BitArray(8, 0b11101110));
    EXPECT_EQ(xorr, BitArray(8, 0b01100110));
}

TEST(BitArray_bitwise_mismatch, ThrowsLengthError) {
    BitArray a(8, 0);
    BitArray b(7, 0);
    EXPECT_THROW(a &= b, std::length_error);
    EXPECT_THROW(a |= b, std::length_error);
    EXPECT_THROW(a ^= b, std::length_error);
}

// сдвиги
TEST(BitArray_shift_left_right, NonMutating) {
    BitArray b(8, 0b00001111);
    EXPECT_EQ((b << 2), BitArray(8, 0b00111100));
    EXPECT_EQ((b >> 2), BitArray(8, 0b00000011));
}

TEST(BitArray_shift_mutating, MutatingShifts) {
    BitArray b(8, 0b00010001);
    b <<= 1;
    EXPECT_EQ(b, BitArray(8, 0b00100010));
    b >>= 2;
    EXPECT_EQ(b, BitArray(8, 0b00001000));
}

TEST(BitArray_shift_exceptions_and_edge, ShiftEdgeCases) {
    BitArray b(10, 0b1111111111);
    EXPECT_THROW(b <<= -1, std::invalid_argument);
    EXPECT_THROW(b >>= -2, std::invalid_argument);
    // shift >= size => reset/zero
    BitArray c(5, 0b11111);
    c <<= 5;
    EXPECT_TRUE(c.none());
    c = BitArray(5, 0b11111);
    c >>= 5;
    EXPECT_TRUE(c.none());
}

// count / any / none / to_string / size / empty
TEST(BitArray_queries, CountAnyNoneToString) {
    BitArray b(6, 0);
    EXPECT_FALSE(b.any());
    EXPECT_TRUE(b.none());
    b.set(1, true);
    EXPECT_TRUE(b.any());
    EXPECT_EQ(b.count(), 1);
    EXPECT_EQ(b.to_string().size(), 6);
    EXPECT_EQ(b.size(), 6);
    EXPECT_FALSE(b.empty());
}

// operator== and !=
TEST(BitArray_equality_ops, Equality) {
    BitArray a(8, 0b1010);
    BitArray b(8, 0b1010);
    BitArray c(8, 0b0101);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}

// count
TEST(BitArray_count_manual, ManualCount) {
    BitArray b(20, 0);
    for (int i = 0; i < b.size(); ++i) if (i % 3 == 0) b.set(i, true);
    int manual = 0;
    for (int i = 0; i < b.size(); ++i) if (b[i]) ++manual;
    EXPECT_EQ(b.count(), manual);
}
