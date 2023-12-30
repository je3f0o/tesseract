gcc -Wall -Wextra -std=c11 main.c ^
  -I"C:\\raylib\\raylib\\src"^
  -I"C:\\raylib\\w64devkit\\x86_64-w64-mingw32\\include"^
  -L"C:\\raylib\\w64devkit\\x86_64-w64-mingw32\\lib"^
  -lraylib -lgdi32 -lwinmm -lm