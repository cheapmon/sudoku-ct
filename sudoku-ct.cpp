#include <array>
#include <cmath>
#include <cstdio>

template<typename t, int size, bool linebreak = false>
void print_array(std::array<t, size> array) {
  for (int i = 0; i < size; i++) {
    std::printf("%d", array[i]);
    if constexpr (linebreak) {
      if (i%9==8) std::printf("\n");
    }
  }
  if constexpr (!linebreak) {
    std::printf("\n");
  }
}

template<int size>
constexpr bool row_contains(const std::array<int, size> &cells, int i) {
  for (int j = 0; j < size; j++) {
    if (cells[j]==i) {
      return true;
    }
  }
  return false;
}

constexpr bool is_valid_row(const std::array<int, 9> &cells) {
  for (int i = 0; i < 9; i++) {
    if (!row_contains<9>(cells, i + 1)) {
      return false;
    }
  }
  return true;
}

constexpr bool is_valid_puzzle(const std::array<int, 81> &cells) {
  bool result = true;
  for (int i = 0; i < 9; i++) {
    const int block = 27*(i/3) + 3*(i%3);
    result &= is_valid_row({cells[i*9 + 0], cells[i*9 + 1], cells[i*9 + 2],
                            cells[i*9 + 3], cells[i*9 + 4], cells[i*9 + 5],
                            cells[i*9 + 6], cells[i*9 + 7], cells[i*9 + 8]});
    result &= is_valid_row({cells[i + 9*0], cells[i + 9*1], cells[i + 9*2],
                            cells[i + 9*3], cells[i + 9*4], cells[i + 9*5],
                            cells[i + 9*6], cells[i + 9*7], cells[i + 9*8]});
    result &= is_valid_row({cells[block + 0], cells[block + 1], cells[block + 2],
                            cells[block + 9], cells[block + 10], cells[block + 11],
                            cells[block + 18], cells[block + 19], cells[block + 20]});
  }
  return result;
}

constexpr std::array<bool, 9> possible_values(const std::array<int, 81> &cells, int cell) {
  const int row = 9*(cell/9);
  const int col = cell%9;
  const int block = 27*(cell/27) + 3*((cell%9)/3);
  std::array<int, 27> relevant_cells{cells[row + 0], cells[row + 1], cells[row + 2],
                                     cells[row + 3], cells[row + 4], cells[row + 5],
                                     cells[row + 6], cells[row + 7], cells[row + 8],
                                     cells[col + 0], cells[col + 9], cells[col + 18],
                                     cells[col + 27], cells[col + 36], cells[col + 45],
                                     cells[col + 54], cells[col + 63], cells[col + 72],
                                     cells[block + 0], cells[block + 1], cells[block + 2],
                                     cells[block + 9], cells[block + 10], cells[block + 11],
                                     cells[block + 18], cells[block + 19], cells[block + 20]};
  return {!row_contains<27>(relevant_cells, 1),
          !row_contains<27>(relevant_cells, 2),
          !row_contains<27>(relevant_cells, 3),
          !row_contains<27>(relevant_cells, 4),
          !row_contains<27>(relevant_cells, 5),
          !row_contains<27>(relevant_cells, 6),
          !row_contains<27>(relevant_cells, 7),
          !row_contains<27>(relevant_cells, 8),
          !row_contains<27>(relevant_cells, 9)};
}

constexpr int n_true_elements(const std::array<bool, 9> &cells) {
  int result = 0;
  for (int i = 0; i < 9; i++) {
    result += cells[i];
  }
  return result;
}

constexpr int true_val(const std::array<bool, 9> &cells) {
  for (int i = 0; i < 9; i++) {
    if (cells[i]) return i + 1;
  }
  return 0;
}

constexpr unsigned long bitset_to_long(const std::array<bool, 9> &cells) {
  unsigned long result = 0;
  for (int i = 0; i < 9; i++) {
    result += static_cast<int>(std::pow(10, i)*cells[i]*(i + 1));
  }
  return result;
}

constexpr int count_empty_cells(const std::array<int, 81> &cells) {
  int result = 0;
  for (int i = 0; i < 81; i++) {
    result += (cells[i]==0);
  }
  return result;
}

constexpr std::array<int, 81> solve_as_far_as_possible(const std::array<int, 81> &cells) {
  int empty_cells_prev = 0;
  int empty_cells = 0;
  std::array<int, 81> puzzle = cells;
  do {
    empty_cells_prev = count_empty_cells(puzzle);
    for (int i = 0; i < 81; i++) {
      if (puzzle[i]!=0) {
        continue;
      }
      const auto values = possible_values(puzzle, i);
      if (n_true_elements(values)==1) {
        puzzle[i] = true_val(values);
      }
    }
    empty_cells = count_empty_cells(puzzle);
  } while (empty_cells_prev!=empty_cells);
  return puzzle;
}

constexpr std::array<int, 81> solve(const std::array<int, 81> &cells) {
  std::array<int, 81> puzzle(cells);
  puzzle = solve_as_far_as_possible(puzzle);
  if (count_empty_cells(puzzle)==0) {
    return puzzle;
  }
  for (int i = 0; i < 81; i++) {
    if (puzzle[i]==0) {
      const auto values = possible_values(puzzle, i);
      for (int j = 0; j < 9; j++) {
        if (values[j]) {
          std::array<int, 81> new_puzzle(puzzle);
          new_puzzle[i] = j + 1;
          auto solution = solve_as_far_as_possible(new_puzzle);
          if (count_empty_cells(solution)==0) {
            return solution;
          }
          solution = solve(new_puzzle);
          if (count_empty_cells(solution)==0) {
            return solution;
          }
        }
      }
    }
  }
  return {};
}

int main() {
  constexpr std::array<int, 81> puzzle = {0, 0, 0, 0, 0, 0, 0, 5, 4,
                                          0, 0, 7, 2, 0, 0, 0, 0, 9,
                                          0, 3, 0, 4, 8, 0, 2, 0, 0,
                                          0, 6, 0, 8, 0, 0, 4, 9, 0,
                                          0, 0, 0, 0, 0, 0, 0, 0, 0,
                                          0, 7, 5, 0, 0, 4, 0, 8, 0,
                                          0, 0, 8, 0, 6, 1, 0, 2, 0,
                                          1, 0, 0, 0, 0, 3, 5, 0, 0,
                                          5, 9, 0, 0, 0, 0, 0, 0, 0};
  constexpr auto solution = solve(puzzle);
  print_array<int, 81, true>(solution);
  return 0;
}