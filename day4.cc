#include <charconv>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <set>
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

template <typename T, typename Fn>
std::span<T> advance_to_token(std::span<T> text, Fn &&f) {
  T *pos = std::find_if(text.data(), text.data() + text.size(), (Fn &&)f);
  return std::span<T>{pos, (text.data() + text.size()) - pos};
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
  std::vector<std::span<char const>> cards;
  std::vector<i64> set_1;
  std::vector<std::span<char const>> set_1_str;
  std::vector<i64> set_2;
  std::vector<std::span<char const>> set_2_str;
  std::vector<i64> intersection;
  split(schematic, '\n', cards);
  u64 points = 0;

  for (std::span<char const> card : cards) {
    set_1.clear();
    set_2.clear();
    set_1_str.clear();
    set_2_str.clear();
    intersection.clear();
    std::span sets_text =
        advance_to_token(card, [](char c) { return c == ':'; });
    std::span next_section =
        advance_to_token(sets_text, [](char c) { return c == '|'; });
    std::span set_1_text = sets_text.subspan(
        0, next_section.begin() - sets_text.subspan(1).begin());
    std::span set_2_text = next_section.subspan(2);
    
    while(!set_1_text.is_empty()){
        advance_to_token(card, [](char c){ return c >  })
    }
    

    for (auto t : set_2_str) {
      std::cout << std::string_view(t.data(), t.size()) << std::endl;
    }
    std::transform(
        set_1_str.begin(), set_1_str.end(), std::back_inserter(set_1),
        [&](std::span<char const> const &slice) {
          i64 result;
          std::from_chars(slice.data(), slice.data() + slice.size(), result);
          return result;
        });
    std::transform(
        set_2_str.begin(), set_2_str.end(), std::back_inserter(set_2),
        [&](std::span<char const> const &slice) {
          i64 result;
          std::from_chars(slice.data(), slice.data() + slice.size(), result);
          return result;
        });
    printf("num set 1: %d\n", (int)set_1.size());
    printf("num set 2: %d\n", (int)set_2.size());
    std::set_intersection(set_1.begin(), set_1.end(), set_2.begin(),
                          set_2.end(), std::back_inserter(intersection));

    if (intersection.size() == 0) {
      continue;
    }

    points += 1ULL << (((u64)intersection.size()) - 1ULL);
  }

  printf("points: %" PRIu64 "\n", points);

  free(schematic.data());
}