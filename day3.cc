#include <charconv>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <span>
#include <vector>

typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;

#define CHECK(message, ...)                                                    \
  if (!(__VA_ARGS__)) {                                                        \
    printf("Panic (cond: %s): %s\n", #__VA_ARGS__, message);                   \
    fflush(stdout);                                                            \
    std::abort();                                                              \
  }

struct Slice {
  usize offset = 0;
  usize size = 0;

  constexpr usize end() const { return offset + size; }
};

struct PartNumber {
  i32 value = 0;
  u32 row = 0;
  u32 column = 0;
  u32 column_span = 0;
};

struct Symbol {
  char value = 0;
  u32 row = 0;
  u32 column = 0;
};

constexpr bool is_num(char c) { return c >= '0' && c <= '9'; }
constexpr bool is_symbol(char c) { return !(is_num(c) || (c == '.')); }

void split_lines(std::span<char const> data, std::vector<Slice> &lines) {
  for (usize i = 0, line_begin = 0; i < data.size(); i++) {
    if (data[i] == '\n' || (i == data.size() - 1)) {
      lines.push_back(Slice{.offset = line_begin, .size = i - line_begin});
      line_begin = i + 1;
    }
  }
}

void get_number_coordinates(std::span<char const> data,
                            std::span<Slice const> lines,
                            std::vector<std::vector<PartNumber>> &numbers) {
  numbers.resize(lines.size());

  for (u32 row = 0; row < (u32)lines.size(); row++) {
    Slice line = lines[row];
    for (usize i = line.offset; i < line.offset + line.size; i++) {
      usize num_begin = i;
      u32 num_length = 0;
      while (is_num(data[i])) {
        i++;
        num_length++;
      }

      if (num_length > 0) {
        i32 part_number;
        std::from_chars(data.data() + num_begin,
                        data.data() + num_begin + num_length, part_number);
        numbers[row].push_back(
            PartNumber{.value = part_number,
                       .row = row,
                       .column = (u32)(num_begin - line.offset),
                       .column_span = num_length});
      }
    }
  }
}

void get_symbol_coordinates(std::span<char const> data,
                            std::span<Slice const> lines,
                            std::vector<std::vector<Symbol>> &symbols) {
  symbols.resize(lines.size());
  for (u32 row = 0; row < (u32)lines.size(); row++) {
    Slice line = lines[row];
    for (usize i = line.offset; i < line.offset + line.size; i++) {
      if (is_symbol(data[i])) {
        symbols[row].push_back(Symbol{
            .value = data[i], .row = row, .column = (u32)(i - line.offset)});
      }
    }
  }
}

i64 sum_part_numbers(std::span<std::vector<PartNumber> const> part_numbers,
                     std::span<std::vector<Symbol> const> symbols) {
  CHECK("logic error", part_numbers.size() == symbols.size());
  u32 num_rows = part_numbers.size();
  i64 sum = 0;

  for (u32 row = 0; row < num_rows; row++) {
    for (PartNumber const &part_number : part_numbers[row]) {
      bool is_valid = false;
      for (u32 search_row = (row == 0) ? row : (row - 1);
           (search_row < (row + 2)) && (search_row < num_rows); search_row++) {
        for (Symbol const &symbol : symbols[search_row]) {
          if ((symbol.column + 1) >= part_number.column &&
              symbol.column <
                  (part_number.column + part_number.column_span + 1)) {
            is_valid = true;
            break;
          }
        }

        if (is_valid) {
          break;
        }
      }

      if (is_valid) {
        sum += part_number.value;
      }
    }
  }

  return sum;
}

i64 sum_gear_ratios(std::span<std::vector<PartNumber> const> part_numbers,
                    std::span<std::vector<Symbol> const> symbols) {
  u32 num_rows = part_numbers.size();
  i64 sum = 0;

  for (u32 row = 0; row < num_rows; row++) {
    for (Symbol const &symbol : symbols[row]) {
      i64 ratios[2] = {0, 0};
      u32 num_ratios = 0;
      bool possible_gear = true;

      if (symbol.value == '*') {
        for (u32 search_row = (row == 0) ? row : (row - 1);
             (search_row < (row + 2)) && (search_row < num_rows);
             search_row++) {
          for (PartNumber const &part_number : part_numbers[search_row]) {
            if ((symbol.column + 1) >= part_number.column &&
                symbol.column <
                    (part_number.column + part_number.column_span + 1)) {
              if (num_ratios + 1 > 2) {
                possible_gear = false;
                break;
              }

              ratios[num_ratios] = part_number.value;
              num_ratios++;
            }
          }
        }
      }

      if (possible_gear && num_ratios == 2) {
        sum += ratios[0] * ratios[1];
      }
    }
  }

  return sum;
}

std::span<char> read_file(char const *file_path) {
  FILE *file = fopen(file_path, "r");
  CHECK("unable to open file", file != nullptr);
  fseek(file, 0, SEEK_END);
  usize file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *string = (char *)malloc(file_size);
  CHECK("alloc failed", string != nullptr);
  CHECK("file read failed", fread(string, 1, file_size, file) == file_size);
  fclose(file);
  return std::span{string, file_size};
}

int main(int argc, char const **argv) {
  CHECK("File Not Specified", argc == 2);
  std::span schematic = read_file(argv[1]);
  std::vector<Slice> lines;
  std::vector<std::vector<PartNumber>> part_numbers;
  std::vector<std::vector<Symbol>> symbols;
  split_lines(schematic, lines);
  get_number_coordinates(schematic, lines, part_numbers);
  get_symbol_coordinates(schematic, lines, symbols);
  i64 part_sum = sum_part_numbers(part_numbers, symbols);
  printf("part sum: %" PRIi64 "\n", part_sum);
  i64 gear_ratio_sum = sum_gear_ratios(part_numbers, symbols);
  printf("gear ratio sum: %" PRIi64 "\n", gear_ratio_sum);
  free(schematic.data());
}
