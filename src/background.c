#include "background/background.h"

const char * BGG_TYPES[] = { "drop", "wall", "primes", "random", NULL };
const char * BGG_TYPES_DESC[] = {
  "Full c0 in middle, fading to c1 in circles to borders.",
  "Full c0 line from left to right in the middle fading to c1 to top and bottom.",
  "Prime number ordered in a spiral.",
  "Random pixels images",
  NULL };

void bgg_help(FILE *output) {

	fprintf(output, "\
%s\n\
Copyright (C) %d  %s\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <https://www.gnu.org/licenses/>.\n",
		"background - background generator.",
		2019, "Fionn Langhans");

  fprintf(output, "----------------------------------------------------------------------\n");
	fprintf(output, "background (type) (size) (path) [color0] [color1]\n\n");

	fprintf(output, "Background (type)s:\n\n");
	for (size_t i = 0;;) {
		if (!BGG_TYPES[i])
			break;
		
		fprintf(output, "  %s\n    %s\n\n", BGG_TYPES[i], BGG_TYPES_DESC[i]);

    ++i;
	}

	fprintf(output, "Image (size):\n");
	fprintf(output, "  WIDTHxHEIGHT\n");
	fprintf(output, "    32 <= WIDTH, HEIGHT <= 32768 and WIDTH, HEIGHT must be integers.\n\n");

	fprintf(output, "(path):\n");
	fprintf(output, "  Output path.\n\n");

  fprintf(output, "Optional [color0] and [color1]:\n");
  fprintf(output, "  Hexadecimal integer which consists of 6 signs.\n");
  fprintf(output, "  RRggBB, where RR is red, gg is green and BB is blue.\n");
}

static bool _is_prime(uint32_t num, uint32_t ** primes, size_t * primes_len) {
  uint32_t maxnum = (uint32_t) floor(sqrt((double) num));
  for (size_t i = 0; i < *primes_len; ++i) {
    if ((*primes)[i] == num)
      return true;

    if ((*primes)[i] > maxnum)
      break;

    if (num % (*primes)[i] == 0 && (*primes)[i] != 1) {
      // Not a prime
      return false;
    }
  }

  // Prime not added => add prime
  (*primes)[(*primes_len)++] = num;
  if (*primes_len % 8192 == 0)
    *primes = (uint32_t*) realloc((void*)*primes, (*primes_len + 8192) * sizeof(uint32_t));

  return true;
}

uint8_t ** bgg_gen(bgg_proc *info) {
  // alloc image
  uint8_t ** result = (uint8_t**) malloc(info->size_height * sizeof(void*));
  if (!result) {
    fprintf(stderr, "Cannot reserve memory block.\n");
    fprintf(stderr, "Reducing image size leads to less memory comsumption.\n");
    return NULL;
  }

  for (size_t i = 0; i < info->size_height; ++i) {
    result[i] = (uint8_t*) malloc(info->size_width * 3);
    if (!result[i]) {
      fprintf(stderr, "Cannot reserve memory block.\n");
      fprintf(stderr, "Reducing image size leads to less memory comsumption.\n");
      // Cleanup
      info->size_width = i; // a - hack
      goto error_img;
    }

    memset(result[i], 0, info->size_width * 3);
  }

  if (info->type == BGG_TYPE_DROP) {
    uint32_t midY = info->size_height / 2 + 1;
    uint32_t midX = info->size_width  / 2 + 1;

    for (uint32_t y = 0; y <= midY; ++y) {
      for (uint32_t x = 0; x <= midX; ++x) {
        color c;
        c.red = (uint8_t) (
            (info->c0.red * ((midY - y) / (float)midY) + info->c1.red * (y / (float)midY)) / 2.0f
          + (info->c0.red * ((midX - x) / (float)midX) + info->c1.red * (x / (float)midX)) / 2.0f);
        c.green = (uint8_t) (
            (info->c0.green * ((midY - y) / (float)midY) + info->c1.green * (y / (float)midY)) / 2.0f
          + (info->c0.green * ((midX - x) / (float)midX) + info->c1.green * (x / (float)midX)) / 2.0f);
        c.blue = (uint8_t) (
            (info->c0.blue * ((midY - y) / (float)midY) + info->c1.blue * (y / (float)midY)) / 2.0f
          + (info->c0.blue * ((midX - x) / (float)midX) + info->c1.blue * (x / (float)midX)) / 2.0f);
  
        size_t y0 = midY + y;
        size_t y1 = midY - y;
        size_t x0 = midX + x;
        size_t x1 = midX - x;
        if (y0 < info->size_height) {
          if (x0 < info->size_width) {
            result[y0][x0 * 3 + 0] = c.red;
            result[y0][x0 * 3 + 1] = c.green;
            result[y0][x0 * 3 + 2] = c.blue;
          }
          
          if (x1 <= midX) {
            result[y0][x1 * 3 + 0] = c.red;
            result[y0][x1 * 3 + 1] = c.green;
            result[y0][x1 * 3 + 2] = c.blue;
          }
        }
        
        if (y1 <= midY) {
          if (x0 < info->size_width) {
            result[y1][x0 * 3 + 0] = c.red;
            result[y1][x0 * 3 + 1] = c.green;
            result[y1][x0 * 3 + 2] = c.blue;
          }
          
          if (x1 <= midX) {
            result[y1][x1 * 3 + 0] = c.red;
            result[y1][x1 * 3 + 1] = c.green;
            result[y1][x1 * 3 + 2] = c.blue;
          }
        } 
      }
    }

    return result;
  } else if (info->type == BGG_TYPE_WALL) {
    uint32_t midY = info->size_height / 2 + 1;
    for (uint32_t y = 0; y <= midY; ++y) {
      color c;
      c.red = (uint8_t) (info->c0.red * ((midY - y) / (float)midY) + info->c1.red * (y / (float)midY));
      c.green = (uint8_t) (info->c0.green * ((midY - y) / (float)midY) + info->c1.green * (y / (float)midY));
      c.blue = (uint8_t) (info->c0.blue * ((midY - y) / (float)midY) + info->c1.blue * (y / (float)midY));

      for (size_t x = 0; x < info->size_width; ++x) {
        size_t y0 = midY + y;
        size_t y1 = midY - y;
        if (y0 < info->size_height) {
          result[y0][x * 3 + 0] = c.red;
          result[y0][x * 3 + 1] = c.green;
          result[y0][x * 3 + 2] = c.blue;
        }
        
        if (y1 <= midY) {
          result[y1][x * 3 + 0] = c.red;
          result[y1][x * 3 + 1] = c.green;
          result[y1][x * 3 + 2] = c.blue;
        } 
      }
    }

    return result;
  } else if (info->type == BGG_TYPE_PRIMES) {
    const uint32_t targetSize = info->size_width * info->size_height;
    uint32_t currentSize = 0;

    uint32_t * primes = (uint32_t*) malloc(sizeof(uint32_t) * 8192);
    size_t primes_len = 0;
    size_t num = 1;
    int32_t x = info->size_width / 2, y = info->size_height / 2;
    const uint32_t midX = x, midY = y;
    uint8_t mode = 0; // 0 = x++, 1 = y++, 2 = x--, 3 = y--

    uint32_t pixels = 1;
    uint32_t walked = 0;

    do {
      bool numIsPrime = _is_prime(num++, &primes, &primes_len);
      if (x >= 0 && x < info->size_width
          && y >= 0 && y < info->size_height) {
        if (numIsPrime) {
          result[y][x * 3 + 0] = info->c0.red;
          result[y][x * 3 + 1] = info->c0.green;
          result[y][x * 3 + 2] = info->c0.blue;
        } else {
          result[y][x * 3 + 0] = info->c1.red;
          result[y][x * 3 + 1] = info->c1.green;
          result[y][x * 3 + 2] = info->c1.blue;
        }

        ++currentSize;

        if (currentSize % (targetSize / 20) == 0) {
          // Delete last
          for (size_t i = 0; i < 80; ++i)
            putchar('\b');

          printf("Progress: %.0f",
              (float) currentSize / (float) targetSize * 100.0f);
        }
      }

      switch (mode) {
        case 0: x++; break;
        case 1: y++; break;
        case 2: x--; break;
        case 3: y--; break;
      }
      ++walked;

      if ((mode % 2 == 0 && pixels <= walked) || (mode % 2 == 1 && pixels <= walked)) {
        walked = 0;
        if (++mode == 2)
          pixels += 1;
        else if (mode > 3) {
          mode = 0;
          pixels += 1;
        }
      }

    } while (currentSize < targetSize && primes);

    putchar('\n');

    if (!primes) {
      fprintf(stderr, "Cannot reserve memory block.\n");
      fprintf(stderr, "Reducing image size leads to less memory comsumption.\n");
      goto error_img;
    }

    free(primes);

    return result;
  } else if (info->type == BGG_TYPE_RANDOM) {
    srand(time(NULL));

    for (size_t y = 0; y < info->size_height; ++y) {
      for (size_t i = 0; i < info->size_width * 3; ++i) {
        result[y][i] = rand() % 256;
      }
    }

    return result;
  }
 
error_img:
  bgg_free_img(info, result);
  return NULL;
}

void bgg_free_img(bgg_proc *info, uint8_t ** img) {
  for (size_t i = 0; i < info->size_height; ++i)
    free(img[i]);

  free(img);
}

static void user_error_fn(png_structp png_ptr, const char * msg) {
  fprintf(stderr, "libpng error: %s\n", msg);
}

static void user_warning_fn(png_structp png_ptr, const char * msg) {
  fprintf(stderr, "libpng warning: %s\n", msg);
}

static void write_row_callback(png_structp png_ptr, png_uint_32 row, int pass) {
  static uint8_t counter = 0;

  if (counter++ % 64 == 0)
    for (size_t i = 0; i < 64; ++i) putchar('\b');
  else
    putchar('-');
}

bool bgg_write(bgg_proc *info, uint8_t ** img) {
  FILE *fp = fopen(info->filename, "wb");
  if (!fp) {
    fprintf(stderr, "Cannot open file \"%s\" for write operator.",
        info->filename);
    return false;
  }

//  png_structp png_ptr = png_create_write_struct
//    (PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
//     user_error_fn, user_warning_fn);
  png_structp png_ptr = png_create_write_struct
    (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    fprintf(stderr, "Cannot reserve memory block for libpng.\n");
    return false;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
    fclose(fp);
    return false;
  }

  // Error handling
  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return false;
  }

  // Open for write
  png_init_io(png_ptr, fp);
  png_set_write_status_fn(png_ptr, write_row_callback);

  // zlib compression level
  png_set_compression_mem_level(png_ptr, Z_BEST_COMPRESSION);

  // set info_ptr
  png_set_IHDR(png_ptr, info_ptr,
    info->size_width, info->size_height,
    8 /* BIT DEPTH */, PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT);

  png_bytep * row_pointers
    = png_malloc(png_ptr, info->size_height * sizeof(png_bytep));
  for (size_t i = 0; i < info->size_height; ++i) {
    row_pointers[i] = NULL;
    if (!(row_pointers[i] = png_malloc(png_ptr, info->size_width * 3))) {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      fclose(fp);
      return false;
    }

    for (size_t x = 0; x < info->size_width; ++x) {
      row_pointers[i][x * 3 + 0] = img[i][x * 3 + 0];
      row_pointers[i][x * 3 + 1] = img[i][x * 3 + 1];
      row_pointers[i][x * 3 + 2] = img[i][x * 3 + 2];
    }
  }

  png_text text_ptr[5];
  memset((void*) text_ptr, 0, sizeof(png_text) * 5); // flush
  text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
  text_ptr[0].key = "Title";
  text_ptr[0].text = (char*) BGG_TYPES[info->type];
  text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
  text_ptr[1].key = "Author";
  text_ptr[1].text = "background";
  text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;
  text_ptr[2].key = "Description";
  text_ptr[2].text = (char*) BGG_TYPES_DESC[info->type];
  text_ptr[3].compression = PNG_TEXT_COMPRESSION_NONE;
  text_ptr[3].key = "Source";
  text_ptr[3].text = "https://github.com/codefionn/background";
  text_ptr[4].compression = PNG_TEXT_COMPRESSION_NONE;
  text_ptr[4].key = "Copyright";
  text_ptr[4].text = "Generated by GPL'ed software. Thus the terms of the GPL apply.";

  png_set_text(png_ptr, info_ptr, text_ptr, 5);

  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  // Cleanup

  // free row_pointers
  for (size_t i = 0; i < info->size_height; ++i) {
    png_free(png_ptr, row_pointers[i]);
  }
  png_free(png_ptr, row_pointers);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);

  return true;
}
