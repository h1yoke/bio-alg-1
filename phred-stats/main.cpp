#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "../parser.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Ожидалось название FASTQ файла\n";
        return -1;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cout << "Файл не доступен\n";
        return -1;
    }

    int32_t line = 0;
    int64_t sum = 0;
    int64_t i = 0;

    FastqRecord record;

    // Запускаем отсчет для красивого таймера
    auto start_time = std::chrono::steady_clock::now();
    std::cout << std::fixed << std::setprecision(2);

    while (parse_fastq(file, line, record)) {
        sum += record.quality[9] - '!';
        i++;

        auto parse_duration = (std::chrono::steady_clock::now() - start_time) / 1'000'000'000.0;
        std::cout
            << "\rЗатраченное время "
            << parse_duration.count()
            << std::flush;
    }

    std::cout << "\nСреднее качество  " << sum / i << "\n";

    file.close();
    return 0;
}
