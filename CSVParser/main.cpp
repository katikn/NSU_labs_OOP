#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <sstream>
#include <exception>

class CSVException : public std::exception {
    std::string message;
public:
    CSVException(const std::string& msg, size_t row, size_t col) {
        std::stringstream ss;
        ss << "CSV Error at line " << row << ", column " << col << ": " << msg;
        message = ss.str();
    }
    const char* what() const noexcept override {
        return message.c_str();
    }
};

template<typename Tuple, size_t Index>
typename std::enable_if<Index == std::tuple_size<Tuple>::value>::type
printTupleHelper(std::ostream&, const Tuple&) {}

template<typename Tuple, size_t Index>
typename std::enable_if<Index < std::tuple_size<Tuple>::value>::type
printTupleHelper(std::ostream& os, const Tuple& t) {
    if (Index > 0) os << ", ";
    os << std::get<Index>(t);
    printTupleHelper<Tuple, Index + 1>(os, t);
}

template<typename... Args>
std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t) {
    os << "(";
    printTupleHelper<std::tuple<Args...>, 0>(os, t);
    return os << ")";
}


template<typename... Args>
class CSVParser {
    std::istream& file;
    size_t skipLines;
    char colDelim;
    char rowDelim;
    char escapeChar;

public:
    CSVParser(std::istream& file, size_t skip = 0, char colDelim = ',', char rowDelim = '\n', char escape = '"')
        : file(file), skipLines(skip), colDelim(colDelim), rowDelim(rowDelim), escapeChar(escape) {
        if (colDelim == rowDelim) {
            throw std::invalid_argument("Column delimiter equals row delimiter");
        }
        if (colDelim == escapeChar) {
            throw std::invalid_argument("Column delimiter equals escape character");
        }
        if (rowDelim == escapeChar) {
            throw std::invalid_argument("Row delimiter equals escape character");
        }
    }

    class Iterator {
        CSVParser* parser;
        std::tuple<Args...> currentRowTuple;
        size_t currentLineNum;
        bool isEnd;

        template <typename T>
        T convert(const std::string& s, size_t colIdx) {
            T value;
            std::stringstream ss(s);
            if constexpr (std::is_same_v<T, std::string>) {
                return s;
            } else {
                ss >> value;
                if (ss.fail() || !ss.eof()) {
                    throw CSVException("Conversion failed for value: '" + s + "'", currentLineNum, colIdx);
                }
                return value;
            }
        }

        template<size_t Index>
        typename std::enable_if<Index == std::tuple_size<std::tuple<Args...>>::value>::type
        fillTuple(const std::vector<std::string>&) {}

        template<size_t Index>
        typename std::enable_if<Index < std::tuple_size<std::tuple<Args...>>::value>::type
        fillTuple(const std::vector<std::string>& data) {
            if (Index >= data.size()) {
                throw CSVException("Not enough columns in CSV row", currentLineNum, Index + 1);
            }
            using ColumnType = typename std::tuple_element<Index, std::tuple<Args...>>::type;

            std::get<Index>(currentRowTuple) = convert<ColumnType>(data[Index], Index + 1);

            fillTuple<Index + 1>(data);
        }

        void readNext() {
            if (parser->file.eof()) {
                isEnd = true;
                return;
            }

            std::string line;
            std::string rawRow;

            while (std::getline(parser->file, line, parser->rowDelim)) {
                currentLineNum++;
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                rawRow += line;

                bool inQuotes = false;

                for (size_t i = 0; i < rawRow.size(); ++i) {
                    if (rawRow[i] == parser->escapeChar) {
                        if (!inQuotes) {
                            if (i != 0 && rawRow[i - 1] != parser->colDelim) {
                                throw CSVException(
                                    "Unexpected quote inside unquoted field",
                                    currentLineNum,
                                    0
                                );
                            }
                            inQuotes = true;
                        } else {
                            if (i + 1 < rawRow.size() && rawRow[i + 1] == parser->escapeChar) {
                                ++i;
                            } else {
                                inQuotes = false;
                            }
                        }
                    }

                }

                if (!inQuotes) {
                    break;
                }
                rawRow += parser->rowDelim;
            }

            if (rawRow.empty() && parser->file.eof()) {
                isEnd = true;
                return;
            }

            std::vector<std::string> cells;
            std::string currentCell;
            bool inQuotes = false;

            for (size_t i = 0; i < rawRow.length(); ++i) {
                char c = rawRow[i];

                if (c == parser->escapeChar) {
                    if (!inQuotes) {
                        inQuotes = true;
                    } else {
                        if (i + 1 < rawRow.size() && rawRow[i + 1] == parser->escapeChar) {
                            currentCell += parser->escapeChar;
                            ++i;
                        } else if (
                            i + 1 < rawRow.size() &&
                            rawRow[i + 1] != parser->colDelim &&
                            rawRow[i + 1] != parser->rowDelim
                        ) {
                            currentCell += parser->escapeChar;
                        } else {
                            inQuotes = false;
                        }
                    }
                } else if (c == parser->colDelim && !inQuotes) {
                    cells.push_back(currentCell);
                    currentCell.clear();
                } else {
                    currentCell += c;
                }
            }
            cells.push_back(currentCell);

            try {
                fillTuple<0>(cells);
            } catch (const CSVException&) {
                throw;
            } catch (const std::exception& e) {
                throw CSVException(e.what(), currentLineNum, 0);
            }
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::tuple<Args...>;
        using difference_type = std::ptrdiff_t;
        using pointer = std::tuple<Args...>*;
        using reference = std::tuple<Args...>&;

        Iterator(CSVParser* p, bool isEnd) : parser(p), isEnd(isEnd), currentLineNum(0) {
            if (!this->isEnd) {
                std::string dummy;
                for (size_t i = 0; i < parser->skipLines; ++i) {
                   if (!std::getline(parser->file, dummy, parser->rowDelim)) {
                       this->isEnd = true;
                       return;
                   }
                   currentLineNum++;
                }
                readNext();
            }
        }

        Iterator& operator++() {
            readNext();
            return *this;
        }

        reference operator*() {
            return currentRowTuple;
        }

        pointer operator->() {
            return &currentRowTuple;
        }

        bool operator!=(const Iterator& other) const {
            return isEnd != other.isEnd;
        }

        bool operator==(const Iterator& other) const {
            return isEnd == other.isEnd;
        }
    };

    Iterator begin() {
        return Iterator(this, false);
    }

    Iterator end() {
        return Iterator(this, true);
    }
};

int main() {
    std::ofstream testFile("test_data.csv");
    testFile << "id,string,double\n";
    testFile << "1,\"test_string\",3.14\n";
    testFile << "2,\"escaped comma\", 3.4\n";
    testFile << "3,\"multi\nline\nstring\",100.5\n";
    testFile << "4,\"say \"\"hello\"\"!\",5.55\n";
    testFile << "5,,0\n";
    testFile << "6,\"He said \"\"Hello\"\" World\",6.66\n";
    testFile.close();

    std::ifstream file("test_data.csv");

    try {
        CSVParser<int, std::string, double> parser(file, 1);

        for (const auto& row : parser) {
            std::cout << row << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
