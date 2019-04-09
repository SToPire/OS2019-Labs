#include "crepl.h"

int cnt = 0, type = 0;
size_t len = 0;
ssize_t nread = 0;
char *input = NULL;
char output[20] = "";
char func_name[128] = "";
int calc_result = 0;

int main(int argc, char *argv[]) {
  for ( ; ; ++cnt) {
    printf(" in[%d]: ", cnt);
    sprintf(output, "out[%d]: ", cnt);

    if (getline(&input, &len, stdin) < 0) break;
    printf("%s%s\n", output, input); 

    switch (precheck()) {
      case TYPE_FUNCT:
        if (compile()) {
          printf("\33[0m" FG_GREEN "Added as function %s." "\033[0m", func_name);
        } else {
          printf("\33[0m" FG_RED "Compilation error." "\033[0m");
        }
        break;
      case TYPE_VALUE:
        if (calculate()) {
          printf("\33[0m" FG_GREEN "Result: %d." "\033[0m", calc_result);
        } else {
          printf("\33[0m" FG_RED "Calculation error." "\033[0m");
        }
        break;
      default:
        error();
        break;
    }
  }
}

int precheck() {
  size_t pos = 0;
  while (pos < len && isblank(input[pos]) == ' ') ++pos;
  if (pos >= len) return TYPE_INVAL;
  if (strncmp(input + pos, "int ", 4) == 0) {
    return TYPE_FUNCT;
  } else {
    return TYPE_VALUE;  
  }
}

void error() {
  printf("\33[0m" FG_RED "Input validation failed." "\33[0m");
}

bool compile() {
  // TODO
  return true;
}

bool calculate() {
  // TODO
  return true;
}
