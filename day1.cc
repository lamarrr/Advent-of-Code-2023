#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>

#define CHECK(message, ...)                                                    \
  if (!(__VA_ARGS__)) {                                                        \
    printf("Panic (cond: %s): %s\n", #__VA_ARGS__, message);                   \
    fflush(stdout);                                                            \
    std::abort();                                                              \
  }

constexpr bool is_ascii_num(char c) { return c >= '0' && c <= '9'; }
constexpr bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

enum class State { Empty, FoundFirst, FoundSecond };

constexpr std::string_view NUMBERS[10] = {"one",  "two", "three", "four",
                                          "five", "six", "seven", "eight",
                                          "nine", "ten"};
constexpr int MAX_NUM_LENGTH = 5;

// single-pass
int64_t sum_calibration(char const *str, size_t str_size) {
  State state = State::Empty;
  int64_t sum = 0;
  int64_t first_num = 0;
  int64_t latest_num = 0;

  for (size_t i = 0; i < str_size; i++) {
    char c = str[i];
    if (is_ascii_num(c)) {
      switch (state) {
      case State::Empty:
        first_num = c - '0';
        state = State::FoundFirst;
        break;
      case State::FoundFirst:
      case State::FoundSecond:
        latest_num = c - '0';
        state = State::FoundSecond;
        break;
      }
    } else if (is_alpha(c)) {
      // look-ahead
      for (size_t inum = 0; inum < std::size(NUMBERS); inum++) {
        if ((i + NUMBERS[inum].size()) > str_size) {
          continue;
        }
        if (NUMBERS[inum] == std::string_view{str + i, NUMBERS[inum].size()}) {
          int64_t const parsed_num = (int64_t)inum + 1;
          switch (state) {
          case State::Empty:
            first_num = parsed_num;
            state = State::FoundFirst;
            break;
          case State::FoundFirst:
          case State::FoundSecond:
            latest_num = parsed_num;
            state = State::FoundSecond;
            break;
          }
          break;
        }
      }
    }

    if (c == '\n' || i == (str_size - 1)) {
      switch (state) {
      case State::FoundFirst:
        sum += first_num * 10 + first_num;
        break;
      case State::FoundSecond:
        sum += first_num * 10 + latest_num;
        break;
      }
      state = State::Empty;
    }
  }

  return sum;
}

int main(int argc, char const **argv) {
  CHECK("File Not Specified", argc == 2);

  FILE *file = fopen(argv[1], "r");
  CHECK("unable to open file", file != nullptr);
  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *string = (char *)malloc(file_size);
  CHECK("alloc failed", string != nullptr);
  CHECK("file read failed", fread(string, 1, file_size, file) == file_size);
  printf("calibration sum: %" PRIi64 "\n", sum_calibration(string, file_size));
  free(string);
  fclose(file);
}