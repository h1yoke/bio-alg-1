#!/bin/bash

echo "Шаг 1. Парсер FASTQ"
read -n 1 -s -r -p "Нажмите любую клавишу чтобы продолжить..."
echo
g++ fastq-stats/main.cpp parser.cpp && ./a.out a.fastq

echo "Шаг 2. GC-состав"
read -n 1 -s -r -p "Нажмите любую клавишу чтобы продолжить..."
echo
g++ gc-stats/main.cpp parser.cpp && ./a.out a.fastq

echo "Шаг 3. Среднее значание Phred"
read -n 1 -s -r -p "Нажмите любую клавишу чтобы продолжить..."
echo
g++ phred-stats/main.cpp parser.cpp && ./a.out a.fastq

echo "Шаг 4. Тримминг"
read -n 1 -s -r -p "Нажмите любую клавишу чтобы продолжить..."
echo
g++ -Wc++17-extensions trimming-stats/main.cpp parser.cpp && ./a.out a.fastq