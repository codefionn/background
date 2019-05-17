#ifndef BGG_BACKGROUND_H
#define BGG_BACKGROUND_H

#include "background/global.h"

#define BGG_TYPE_DROP 0
#define BGG_TYPE_WALL 1
#define BGG_TYPE_PRIMES 2
#define BGG_TYPE_RANDOM 3

//! Color c-string names. NULL terminated.
extern const char * BGG_TYPES[];
//! Color c-string descriptions. NULL terminated.
extern const char * BGG_TYPES_DESC[];

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} color;

#define COLOR_BLACK(x) x.red = 0; x.green = 0; x.blue = 0
#define COLOR_WHITE(x) x.red = 255; x.green = 255; x.blue = 255

typedef struct {
  uint32_t type;

  //! Output image width
  uint32_t size_width;
  //! Output image height
  uint32_t size_height;

  //! Color 0
  color c0;
  //! Color 1
  color c1;

  //! Output file
  char * filename;
} bgg_proc;

/*!\brief Print help
 * \param output Where to write to (e.g. stderr, stdout)
 */
void bgg_help(FILE *output);

/*!\brief Generate image.
 * \param info How to generate image (type, size_width, size_height).
 * \return Returns Array[info->size_height][info->size_width * 3] with
 * RGB colorscheme. Return NULL if error occured (dynamic allocation failed,
 * wrong type, ...); 
 */
uint8_t ** bgg_gen(bgg_proc *info);

/*!\brief Frees data used by img.
 */
void bgg_free_img(bgg_proc *info, uint8_t ** img);

/*!\brief Write img to file at info->filename.
 * \param info
 * \param img Array[info->size_height][info->size_width * 3].
 * RGB colorscheme.
 * \return Returns true, if write was successfull, otherwise false.
 */
bool bgg_write(bgg_proc *info, uint8_t ** img);

#endif /* BGG_BACKGROUND_H */
