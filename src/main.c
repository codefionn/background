#include "background/background.h"

static bool _readColor(const char * str, color *c) {
  if (strlen(str) != 6) {
    fprintf(stderr, "Color must be 6 characters(bytes) long.\n");
    return false;
  }

  for (size_t i = 0; i < 6; ++i) {
    if (!isxdigit(str[i])) {
        fprintf(stderr, "Color must only consist of hexadecimal characters.");
        return false;
    }
  }

  uint32_t colorInt = strtoul(str, NULL, 16 /* == HEX */);
  c->blue = (uint8_t) colorInt;
  c->green = (uint8_t) (colorInt >>= 8);
  c->red = (uint8_t) (colorInt >>= 8);

  return true;
}

int main(int argsc, char * argsv[]) {
  if (argsc < 3 + 1 || argsc > 5 + 1) {
    bgg_help(stdout);
    return EXIT_FAILURE;
  }

  bgg_proc info;
  COLOR_WHITE(info.c0);
  COLOR_BLACK(info.c1);

  // Type
  uint32_t bggTypesIndex = 0;
  while (true) {
    if (!BGG_TYPES[bggTypesIndex]) {
      fprintf(stderr, "Invalid image type. Type \"%s\" for help.\n", argsv[0]);
      return EXIT_FAILURE;
    }

    if (strcmp(BGG_TYPES[bggTypesIndex], argsv[1]) == 0) {
      info.type = bggTypesIndex;
      break;
    }

    ++bggTypesIndex;
  }

  // WIDTHxHEIGHT
  char * awidth = strtok(argsv[2], "x");
  if (!awidth) {
    fprintf(stderr, "Invalid size. Type \"%s\" for help.\n", argsv[0]);
    return EXIT_FAILURE;
  }

  info.size_width = strtoul(awidth, NULL, 10);
  char * aheight = strtok(NULL, "x");
  if (!aheight) {
    fprintf(stderr, "Invalid height. Type \"%s\" for help.\n", argsv[0]);
    return EXIT_FAILURE;
  }

  info.size_height = strtoul(aheight, NULL, 10);

  // Output file path
  info.filename = argsv[3];

  // Colors (optional)
  if (argsc > 4 && !_readColor(argsv[4], &info.c0)) {
    // Color 0
    return EXIT_FAILURE;
  }

  if (argsc > 5 && !_readColor(argsv[5], &info.c1)) {
    // Color 1
    return EXIT_FAILURE;
  }

  printf("Command: Type=%s,Width=%u,Height=%u,Output=\"%s\"\n",
      BGG_TYPES[info.type], info.size_width, info.size_height, info.filename);
  printf("Colors: 0=(%d,%d,%d), 1=(%d,%d,%d)\n",
      (int) info.c0.red, (int) info.c0.green, (int) info.c0.blue,
      (int) info.c1.red, (int) info.c1.green, (int) info.c1.blue);

  // Execute library commands
  
  uint8_t ** img;
  if (!(img = bgg_gen(&info))) {
    return EXIT_FAILURE;
  }

  if (!bgg_write(&info, img)) {
    bgg_free_img(&info, img);
    return EXIT_FAILURE;
  }

  bgg_free_img(&info, img);

  printf("\nSuccess.\n");

  return EXIT_SUCCESS;
}
