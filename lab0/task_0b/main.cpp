#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <algorithm>
#include <locale>
// #include <boost/locale.hp
// using namespace boost::locale;

class WordCounter{
private:
    std::map<std::string, int> freq; //слово: количество
    int totalWords = 0; //общее кол-во слов
    std::locale loc;
    void process_line(const std::string& line){
        std::cout << "Current locale: " << std::locale("").name() << std::endl;
        std::string word;
        for (char c : line) { 
            if (std::isalnum(c, loc)) { //является ли символ буквой или цифрой
                word.push_back(static_cast<char>(std::tolower(c, loc))); //строчная буква(static_cast<unsigned char> для перевода отриц значений)
            } else {
                if (!word.empty()) {
                    freq[word]++;
                    totalWords++;
                    word.clear();
                }
            }
        }
        if (!word.empty()) {
            freq[word]++;
            totalWords++;
        }
    }
    static bool compare(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b){
        return a.second > b.second;
    }
public:
    WordCounter() : loc("ru_RU.UTF-8") {
        std::locale::global(loc);
    }
    void read_file(const std::string& file){
        std::ifstream input(file);
        if (!input.is_open()){
            throw std::runtime_error("input file not opened!");
        }

        std::list<std::string> lines;
        std::string line;
        while (std::getline(input, line)){
            lines.push_back(line);
        }
        input.close();

        for (const auto& line : lines){
            process_line(line);
        }
    }
    void save_csv(const std::string& file){
        std::ofstream output(file);
        if (!output.is_open()){
            throw std::runtime_error("output file not opened!");
        }

        std::list<std::pair<std::string,int> > words;
        for (auto& p : freq) {
            words.push_back(p);
        }

        words.sort(compare);
        
        output << "Word, Count, Frequency %\n";
        for (auto& word : words){
            double percent = (100.0 * word.second) / totalWords;
            output << word.first << ", " << word.second << ", " << std::fixed << std::setprecision(2) << percent << "\n";
        }
        output.close();
    }
};

int main(int argc, char* argv[]){
    std::locale cp1251_locale("ru_RU.UTF-8");
    std::locale::global(cp1251_locale);
    if (argc != 3){
        std::cerr << "more or less than 3 args";
        return 1;
    }

    WordCounter wc;
    wc.read_file(argv[1]);
    wc.save_csv(argv[2]);

    return 0;
}