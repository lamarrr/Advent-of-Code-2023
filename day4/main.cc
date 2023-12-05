#include <algorithm>
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
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;

#define CHECK(message, ...)                                  \
  if (!(__VA_ARGS__))                                        \
  {                                                          \
    printf("Panic (cond: %s): %s\n", #__VA_ARGS__, message); \
    fflush(stdout);                                          \
    std::abort();                                            \
  }

template <typename T, typename Fn>
std::span<T> advance_to_token(std::span<T> text, Fn &&f)
{
  auto pos = std::find_if(text.begin(), text.end(), (Fn &&) f);
  return std::span<T>{pos, (size_t) (text.end() - pos)};
}

void split_lines(std::span<char const> data, std::vector<std::span<char const>> &lines)
{
  for (usize i = 0, line_begin = 0; i < data.size(); i++)
  {
    if (data[i] == '\n' || (i == data.size() - 1))
    {
      lines.push_back(data.subspan(line_begin, i - line_begin));
      line_begin = i + 1;
    }
  }
}

constexpr bool is_num(char c)
{
  return c >= '0' && c <= '9';
}
constexpr bool is_non_num(char c)
{
  return !is_num(c);
}

std::span<char> read_file(char const *file_path)
{
  FILE *file = fopen(file_path, "rb");
  CHECK("unable to open file", file != nullptr);
  fseek(file, 0, SEEK_END);
  usize file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *string = (char *) malloc(file_size);
  CHECK("alloc failed", string != nullptr);
  size_t num_read = fread(string, 1, file_size, file);
  CHECK("file read failed", num_read == file_size);
  fclose(file);
  return std::span{string, file_size};
}

int main(int argc, char const **argv)
{
  CHECK("File Not Specified", argc == 2);
  std::span                          schematic = read_file(argv[1]);
  std::vector<std::span<char const>> cards;
  std::set<i64>                      set_1;
  std::vector<std::span<char const>> set_1_str;
  std::set<i64>                      set_2;
  std::vector<std::span<char const>> set_2_str;
  std::vector<i64>                   intersection;
  split_lines(schematic, cards);
  u64 points = 0;
  u64 line   = 0;

  for (std::span<char const> card : cards)
  {
    line++;
    set_1.clear();
    set_2.clear();
    set_1_str.clear();
    set_2_str.clear();
    intersection.clear();
    std::span sets_text    = advance_to_token(card, [](char c) { return c == ':'; });
    std::span next_section = advance_to_token(sets_text, [](char c) { return c == '|'; });
    std::span set_1_text   = advance_to_token(sets_text, is_num);
    set_1_text             = set_1_text.subspan(0, next_section.data() - set_1_text.data());
    std::span set_2_text   = advance_to_token(next_section.subspan(1), is_num);

    while (!set_1_text.empty())
    {
      set_1_text        = advance_to_token(set_1_text, is_num);
      std::span num_end = advance_to_token(set_1_text, is_non_num);
      if (num_end.data() - set_1_text.data() > 0)
      {
        i64 result;
        std::from_chars(set_1_text.data(), num_end.data() + num_end.size(), result);
        set_1.emplace(result);
        set_1_text = num_end;
      }
    }

    while (!set_2_text.empty())
    {
      set_2_text        = advance_to_token(set_2_text, is_num);
      std::span num_end = advance_to_token(set_2_text, is_non_num);
      if (num_end.data() - set_2_text.data() > 0)
      {
        i64 result;
        std::from_chars(set_2_text.data(), num_end.data() + num_end.size(), result);
        set_2.emplace(result);
        set_2_text = num_end;
      }
    }

    std::set_intersection(set_1.begin(), set_1.end(), set_2.begin(), set_2.end(),
                          std::back_inserter(intersection));

    u64 curr_points =
        (intersection.size() == 0) ? 0ULL : (1ULL << (((u64) intersection.size()) - 1ULL));
    points += curr_points;
  }

  printf("points: %" PRIu64 "\n", points);

  free(schematic.data());
}