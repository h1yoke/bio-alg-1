// Микро-библиотека с общей частью для парсинга FASTQ
#include <fstream>
#include <iostream>
#include <string>

#include "parser.h"

// Парсер одной записи (возвращает true если продолжаем чтение в файле)
bool parse_fastq(
    std::ifstream &fs,
    FastqRecord &record
) {
    std::string s;

    // Пробуем прочесть название
    if (!std::getline(fs, s)) {
        return false; // Конец файла
    }
    if (s[0] != '@') {
        std::cout << "Строка названия должна начинаться с '@'\n";
        return false;
    }
    record.naming = s.substr(1);

    // Пробуем прочесть последовательность
    if (!std::getline(fs, s)) {
        std::cout << "Ожидалась строка последовательности\n";
        return false;
    }
    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];
        std::vector<char> allowed_chars = {
            'A', 'C', 'G', 'T', 'U',                              // обычные
            'R', 'Y', 'K', 'M', 'S', 'W', 'B', 'D', 'H', 'V', 'N' // IUPAC
        };

        if (std::find(allowed_chars.begin(), allowed_chars.end(), c) == allowed_chars.end()) {
            std::cout << "Строка последовательности содержит неизвестный символ";
            return false;
        }
    }
    record.sequence = s;
 
    // Пробуем прочесть комментарий
    if (!std::getline(fs, s)) {
        std::cout << "Ожидалась строка комментария\n";
        return false;
    }
    if (s[0] != '+') {
        std::cout << "Раздел должен начинаться с '+'";
        return false;
    }
    record.comment = s;

    // Пробуем прочесть качество
    if (!std::getline(fs, s)) {
        std::cout << "Оиждалась строка качества прочтений\n";
        return false;
    }
    record.quality = s;

    return true; // Прочли 4 правильные строки - продолжаем парсить записи
}

