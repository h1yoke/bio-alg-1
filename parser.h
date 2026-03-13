// Микро-библиотека с общей частью для парсинга FASTQ
#pragma once

#include <string>
#include <fstream>

struct FastqRecord {
    std::string naming;   // Название (и проч. метаданные)
    std::string sequence; // Прочтение
    std::string comment;  // Комментарий ('+' и все что после)
    std::string quality;  // Строка качества Phred
};

// Парсер
bool parse_fastq(std::ifstream &filestream, FastqRecord &record);