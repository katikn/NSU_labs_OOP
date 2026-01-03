#include "../include/bitarray.h"
#include <algorithm>

// вспомогательные методы

int BitArray::word_index(int pos) const { return pos / BITS_PER_WORD; } // индекс слова
int BitArray::bit_offset(int pos) const { return pos % BITS_PER_WORD; } // смещение бита
int BitArray::num_words() const { return (num_bits + BITS_PER_WORD - 1) / BITS_PER_WORD; } // сколько нужно слов

void BitArray::check_range(int pos) const { // входит ли pos в диапозон массива
    if (pos < 0 || pos >= num_bits)
        throw std::out_of_range("BitArray index out of range");
}

void BitArray::check_size(const BitArray& other) const { // одинаковый ли размер у двух массивов
    if (num_bits != other.num_bits)
        throw std::length_error("BitArray sizes must be equal");
}

// конструкторы и деструктор

BitArray::BitArray() : num_bits(0) {} // пустой массив

BitArray::BitArray(int num_bits, unsigned long value) : num_bits(num_bits) { // задаем размер списка
    if (num_bits < 0) throw std::invalid_argument("Number of bits cannot be negative");

    int words = num_words(); // вычисление количества слов
    data.assign(words, 0ULL); // выделение памяти для слов и заполнение вектора нулями

    if (words > 0 && num_bits > 0) {
        int bits_to_copy = std::min(num_bits, BITS_PER_WORD); // min(10, 64) = 10, min(100, 64) = 64
        uint64_t mask = (bits_to_copy >= BITS_PER_WORD) ? ~0ULL : ((1ULL << bits_to_copy) - 1ULL);
        // 1ULL << n - ставит 1 на n, остальное 0, -1 заполняет младшие биты 1
        data[0] = static_cast<uint64_t>(value) & mask; // записываем только нужные биты из value
    }
}

/* BitArray b(12, 0b10110011) - 8 бит, value = 179
 * words = 1
 * data[0] = 0
 * bits_to_copy = 12
 * mask = (1ULL << 12) - 1 = 0b0000111111111111
 * data[0] = 0b10110011 & 0b0000111111111111 = 0b10110011
 * data[0] = 00000000 00000000 00000000 00000000 00000000 00000000 00000000 10110011
 */

BitArray::BitArray(const BitArray& b) = default; // глубокое копирование
BitArray::~BitArray() = default; // деструктор

// основные методы

void BitArray::swap(BitArray& b) { // меняет содержимое двух массивов
    std::swap(data, b.data);
    std::swap(num_bits, b.num_bits);
}

BitArray& BitArray::operator=(const BitArray& b) { // копирующее присваивание с защитой от самоприсваивания
    if (this != &b) {
        BitArray temp(b);
        swap(temp);
    }
    return *this;
}

void BitArray::resize(int new_num_bits, bool value) {
    if (new_num_bits < 0) throw std::invalid_argument("New size cannot be negative");

    int old_words = num_words(); //                                         старое количество слов
    int new_words = (new_num_bits + BITS_PER_WORD - 1) / BITS_PER_WORD; //  новое количество слов

    data.resize(new_words, 0ULL);
    // больше - заполняет новые слова нулями, меньше - обрезает но лишние остаются в последнем слове

    if (new_num_bits > num_bits && value) { // расширение и заполнение новыми битами == 1
        // устанавливаем новые биты напрямую в data, чтобы не вызывать check_range
        for (int i = num_bits; i < new_num_bits; ++i) {
            int j = word_index(i);
            int off = bit_offset(i);
            data[j] |= (1ULL << off);
        }
    }

    /* BitArray b(3) = 000
     * b.resize(6, true)
     * data 111000
     */

    if (new_num_bits < num_bits) { // сужение, обнуление лишних битов вручную
        if (new_num_bits == 0){
            data.clear();
        } else {
            int last_word = word_index(new_num_bits - 1); // индекс последнего слова
            int bits_in_last = bit_offset(new_num_bits); // сколько реально бит используется
            if (bits_in_last != 0 && last_word < (int) data.size()) {
                uint64_t mask = (bits_in_last >= BITS_PER_WORD) ? ~0ULL : ((1ULL << bits_in_last) -
                                                                           1ULL); // маска 0b00111111 например
                data[last_word] &= mask;
            }
            if (new_words < old_words) { // удаляем лишние слова
                data.resize(new_words);
            }
        }
    }

    num_bits = new_num_bits;
}

void BitArray::clear() { // очищает, удаляет все биты
    data.clear();
    num_bits = 0;
}

void BitArray::push_back(bool bit) { // увеличивает размер на 1 и добавляет бит
    int old_size = num_bits;
    resize(num_bits + 1);
    set(old_size, bit);
}

// битовые операции

BitArray& BitArray::operator&=(const BitArray& b) { // побитовое И с другим массивом
    check_size(b); // размеры массивов должны совпадать
    for (size_t i = 0; i < data.size(); ++i)
        data[i] &= b.data[i];
    return *this;
}

BitArray& BitArray::operator|=(const BitArray& b) { // побитовое ИЛИ с другим массивом
    check_size(b);
    for (size_t i = 0; i < data.size(); ++i)
        data[i] |= b.data[i];
    return *this;
}

BitArray& BitArray::operator^=(const BitArray& b) { // побитовое XOR
    check_size(b);
    for (size_t i = 0; i < data.size(); ++i)
        data[i] ^= b.data[i];
    return *this;
}

// сдвиги

BitArray& BitArray::operator<<=(int n) {
    if (n < 0) throw std::invalid_argument("Shift count cannot be negative"); // исключение
    if (n == 0 || num_bits == 0) return *this; // ничего не делаем
    if (n >= num_bits) { reset(); return *this; } // обнуляем массив

    int word_shift = n / BITS_PER_WORD; // сколько целых слов сдвинуть
    int bit_shift  = n % BITS_PER_WORD; // внутренний сдвиг внутри слова
    int words = num_words();

    std::vector<uint64_t> new_data(words, 0);

    // для левого сдвига: каждая целевая позиция i получает биты из src = i - word_shift
    for (int i = words - 1; i >= 0; --i) {
        int src = i - word_shift;
        unsigned long v = 0;
        if (src >= 0) {
            v = data[src] << bit_shift;
            if (bit_shift > 0 && src - 1 >= 0) {
                v |= (data[src - 1] >> (BITS_PER_WORD - bit_shift)); // сдвигаем влево из "младшего" слова в старшее
                                                                     // используя сдвиг вправо на 64 - bit_shift
                                                                     // в младшем слове
            }
        }
        new_data[i] = v;
    }
    /* data[0] = 0b11001010
     * data[1] = 0b10101100
     * n = 3
     * word_shift = 0, bit_shift = 3
     * data[0] = 0b11001010 << 3 = 0b01010000
     * data[1] = (0b10101100 << 3) | (0b11001010 >> 5)
               = 0b01100000 | 0b00000110 = 0b01100110
     * */

    data = std::move(new_data);

    int extra = num_bits % BITS_PER_WORD;
    if (extra != 0) {
        uint64_t mask = (extra >= BITS_PER_WORD) ? ~0UL : ((1ULL << extra) - 1);
        data.back() &= mask;
    } // обнуляем лишние биты в последнем слове

    return *this;
}

BitArray& BitArray::operator>>=(int n) {
    if (n < 0) throw std::invalid_argument("Shift count cannot be negative");
    if (n == 0 || num_bits == 0) return *this;
    if (n >= num_bits) { reset(); return *this; }

    int word_shift = n / BITS_PER_WORD;
    int bit_shift  = n % BITS_PER_WORD;
    int words = num_words();

    std::vector<uint64_t> new_data(words, 0);

    // для правого сдвига: каждый целевой индекс i получает биты из src = i + word_shift
    for (int i = 0; i < words; ++i) {
        int src = i + word_shift;
        unsigned long v = 0;
        if (src < words) {
            v = data[src] >> bit_shift;
            // перенос приходит из более старшего слова src+1
            if (bit_shift > 0 && src + 1 < words) {
                v |= (data[src + 1] << (BITS_PER_WORD - bit_shift));
            }
        }
        new_data[i] = v;
    }

    data = std::move(new_data);

    // маскируем старшие биты последнего слова (если размер не кратен слову)
    int extra = num_bits % BITS_PER_WORD;
    if (extra != 0) {
        uint64_t mask = (extra >= BITS_PER_WORD) ? ~0UL : ((1ULL << extra) - 1);
        data.back() &= mask;
    }

    return *this;
}


BitArray BitArray::operator<<(int n) const { return BitArray(*this) <<= n; }
BitArray BitArray::operator>>(int n) const { return BitArray(*this) >>= n; }

// манипуляция битами

BitArray& BitArray::set(int n, bool val) { // установка бита n в val
    check_range(n);
    uint64_t bit = (1ULL << bit_offset(n));
    if (val) data[word_index(n)] |= bit;
    else data[word_index(n)] &= ~bit;
    return *this;
}
/* n = 67
 * word_index = 67 / 64 = 1
 * bit_offset = 67 % 64 = 3
 * data[1] |= 1ULL << 3 = 0b1000
 * 0b000..01000
 * */

BitArray& BitArray::set() { // заполняет все биты 1
    std::fill(data.begin(), data.end(), ~0ULL);
    int extra = num_bits % BITS_PER_WORD;
    if (extra != 0) {
        uint64_t mask = (extra >= BITS_PER_WORD) ? ~0ULL : ((1ULL << extra) - 1ULL);
        data.back() &= mask;
    }
    return *this;
}
/* data[0] = 0b11...111 - 64 бита
 * data[1] = 0b11...111 - 64 бита
 * extra = 70 % 64 = 6 != 0
 * mask = (1ULL << 6) - 1 = 0b111111
 * data[1] &= 0b111111 = 0b000..0111111
 * */

BitArray& BitArray::reset(int n) { return set(n, false); } // сбрасывает бит в 0

BitArray& BitArray::reset() { // сбрасывает все биты в 0
    std::fill(data.begin(), data.end(), 0);
    return *this;
}

// запросы

bool BitArray::any() const { // true если хоть один бит 1
    for (auto word : data) if (word != 0) return true;
    return false;
}

bool BitArray::none() const { return !any(); } // все биты 0

BitArray BitArray::operator~() const { // битовая инверсия
    BitArray result(*this); // копирует массив
    for (auto& word : result.data) word = ~word; // инвертирует биты в каждом слове
    int extra = num_bits % BITS_PER_WORD; // оставшиеся биты
    if (extra != 0) { // обнуление лишних битов в конце
        uint64_t mask = (extra >= BITS_PER_WORD) ? ~0ULL : ((1ULL << extra) - 1ULL);
        result.data.back() &= mask;
    }
    return result;
}

int BitArray::count() const {
    int cnt = 0;
    for (int i = 0; i < num_bits; ++i)
        if ((*this)[i]) ++cnt;
    return cnt;
}

bool BitArray::operator[](int i) const {
    check_range(i);
    return (data[word_index(i)] >> bit_offset(i)) & 1ULL;
}

int BitArray::size() const { return num_bits; }
bool BitArray::empty() const { return num_bits == 0; }

std::string BitArray::to_string() const { // возвращает строку, начиная со старшего бита
    std::string s;
    s.reserve(num_bits);
    for (int i = num_bits - 1; i >= 0; --i)
        s += (*this)[i] ? '1' : '0';
    return s;
}

// внешние операторы

bool operator==(const BitArray& a, const BitArray& b) {
    return a.num_bits == b.num_bits && a.data == b.data;
}

bool operator!=(const BitArray& a, const BitArray& b) { return !(a == b); }

BitArray operator&(const BitArray& b1, const BitArray& b2) {
    BitArray result = b1;
    result &= b2;
    return result;
}

BitArray operator|(const BitArray& b1, const BitArray& b2) {
    BitArray result = b1;
    result |= b2;
    return result;
}

BitArray operator^(const BitArray& b1, const BitArray& b2) {
    BitArray result = b1;
    result ^= b2;
    return result;
}