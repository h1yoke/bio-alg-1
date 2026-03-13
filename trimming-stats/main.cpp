#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "../parser.h"

// `trimmomatic SE SLIDINGWINDOW:ww:tq`
std::pair<FastqRecord, bool> SLIDINGWINDOW(
    const FastqRecord &record,
    int ww,
    float tq
) {
    // Считаем качество (getQualityAsInteger в trimmomatic)
    std::vector<int32_t> quals(record.quality.length());
    for (size_t i = 0; i < record.quality.length(); i++) {
        record.sequence[i] == 'N'
            ? quals[i] = 0
            : quals[i] = record.quality[i] - '!';
    }

    float sum_quality = tq * ww;
    int total = 0;
    for (int i = 0; i < ww; i++) {
        total += quals[i];
    }
    if (total < sum_quality) {
        return { FastqRecord(), false };
    }

    // Ищем начало окна (lengthToKeep в trimmomatic)
    int32_t result_i = quals.size();
    for (size_t i = 0; i < quals.size() - ww; i++) {
        total = total - quals[i] + quals[i + ww];
        if (total < sum_quality) {
            result_i = i + ww;
            break;
        }
    }

    // Максимально обрезаем с конца (lastBaseQuality в trimmomatic)
    int32_t i = result_i;
    int32_t last_quality = quals[i - 1];
    while (last_quality < tq && i > 1) {
        i--;
        last_quality = quals[i - 1];
    }

    if (i == 0) {
        return { FastqRecord(), false };
    }

    if (i < int32_t(quals.size())) {
        FastqRecord trimmed(record);
        trimmed.sequence = record.sequence.substr(0, i);
        trimmed.quality = record.quality.substr(0, i);
        return { trimmed, true };
    }
    return { record, true };
}

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

    int32_t dropped1 = 0;

    FastqRecord record;
    std::vector<FastqRecord> after;

    // Запускаем отсчет для красивого таймера
    auto start_time = std::chrono::steady_clock::now();
    std::cout << std::fixed << std::setprecision(2);

    while (parse_fastq(file, record)) {
        auto [trimmed, is_valid] = SLIDINGWINDOW(record, 5, 30);

        if (is_valid) {
            after.push_back(trimmed);
        } else {
            dropped1++;
        }

        auto parse_duration = (std::chrono::steady_clock::now() - start_time) / 1'000'000'000.0;
        std::cout
            << "\rЗатраченное время             "
            << parse_duration.count()
            << std::flush;
    }

    // `SLIDINGWINDOW:5:30` графа Dropped из trimmomatic
    std::cout << "\nПодверглось триммингу         " << dropped1 << "\n";

    int32_t min_len = 100000;
    int32_t max_len = 0;
    int64_t sum_len = 0;
    int32_t surviving2 = 0;

    for (const auto &rec: after) {
        sum_len += rec.sequence.length();
        min_len = std::min(min_len, (int32_t)rec.sequence.length());
        max_len = std::max(max_len, (int32_t)rec.sequence.length());

        if (rec.sequence.length() >= 60) {
            surviving2++;
        }
    }

    std::cout << "Минимальная длина равна       " << min_len << "\n";
    std::cout << "Средняя длина равна           " << double(sum_len) / after.size() << "\n";
    std::cout << "Максимальная длина равна      " << max_len << "\n";

    // `SLIDINGWINDOW:5:30 MINLEN:60` графа Surviving из trimmomatic
    std::cout << "Осталось после тримминга >=60 " << surviving2 << "\n";

    file.close();
    return 0;
}
