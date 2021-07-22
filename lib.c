#include "lib3gpp23038.h"

#include "tables.c"

#include <stdlib.h>
#include <string.h>

struct lang_table {
  const uint16_t *gsm2uni;
  const struct mapping_entry *uni2gsm;
  size_t num_entries;
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

#define DEFINE_TABLE(x, y)                                                     \
  [GPP23038_TABLE_##x] = {.gsm2uni = y##_gsm2uni,                              \
                          .uni2gsm = y##_uni2gsm,                              \
                          .num_entries = ARRAY_SIZE(y##_uni2gsm)}

static const struct lang_table full_tables[] = {
    DEFINE_TABLE(DEFAULT, default),     DEFINE_TABLE(TURKISH, turkish),
    DEFINE_TABLE(SPANISH, default),     DEFINE_TABLE(PORTUGUESE, portuguese),
    DEFINE_TABLE(BENGALI, bengali),     DEFINE_TABLE(GUJARATI, gujarati),
    DEFINE_TABLE(HINDI, hindi),         DEFINE_TABLE(KANNADA, kannada),
    DEFINE_TABLE(MALAYALAM, malayalam), DEFINE_TABLE(ORIYA, oriya),
    DEFINE_TABLE(PUNJABI, punjabi),     DEFINE_TABLE(TAMIL, tamil),
    DEFINE_TABLE(TELUGU, telugu),       DEFINE_TABLE(URDU, urdu),
};

#undef DEFINE_TABLE

#define DEFINE_ESCAPE_TABLE(x, y)                                              \
  [GPP23038_TABLE_##x] = {.gsm2uni = y##_escape_gsm2uni,                       \
                          .uni2gsm = y##_escape_uni2gsm,                       \
                          .num_entries = ARRAY_SIZE(y##_escape_uni2gsm)}

static const struct lang_table escape_tables[] = {
    DEFINE_ESCAPE_TABLE(DEFAULT, default),
    DEFINE_ESCAPE_TABLE(TURKISH, turkish),
    DEFINE_ESCAPE_TABLE(SPANISH, spanish),
    DEFINE_ESCAPE_TABLE(PORTUGUESE, portuguese),
    DEFINE_ESCAPE_TABLE(BENGALI, bengali),
    DEFINE_ESCAPE_TABLE(GUJARATI, gujarati),
    DEFINE_ESCAPE_TABLE(HINDI, hindi),
    DEFINE_ESCAPE_TABLE(KANNADA, kannada),
    DEFINE_ESCAPE_TABLE(MALAYALAM, malayalam),
    DEFINE_ESCAPE_TABLE(ORIYA, oriya),
    DEFINE_ESCAPE_TABLE(PUNJABI, punjabi),
    DEFINE_ESCAPE_TABLE(TAMIL, tamil),
    DEFINE_ESCAPE_TABLE(TELUGU, telugu),
    DEFINE_ESCAPE_TABLE(URDU, urdu),
};

#undef DEFINE_ESCAPE_TABLE

#define GSM_ESCAPE_CHAR 0x1b

static void save_char(uint8_t gsmchar, const struct lang_table *single,
                      const struct lang_table *locking, uint16_t *output,
                      size_t outsiz, int *in_escape, size_t *output_chars) {
  uint16_t unichar = 0;
  if (gsmchar == GSM_ESCAPE_CHAR) {
    *in_escape = 1;
  } else if (*in_escape) {
    *in_escape = 0;
    unichar = single->gsm2uni[gsmchar];
  } else {
    unichar = locking->gsm2uni[gsmchar];
  }

  if (!*in_escape) {
    if (*output_chars < outsiz) {
      output[*output_chars] = (unichar != 0) ? unichar : ' ';
    }
    *output_chars += 1;
  }
}

size_t gpp23038_7bit_to_unicode(const uint8_t *packed, size_t num_octets,
                                uint16_t *output, size_t outsiz,
                                enum gpp23038_shift_table single_shift,
                                enum gpp23038_shift_table locking_shift) {
  const struct lang_table *const single = &escape_tables[single_shift];
  const struct lang_table *const locking = &full_tables[locking_shift];

  uint16_t shiftreg = 0;
  unsigned int valid_bits = 0;
  size_t output_chars = 0;
  int in_escape = 0;

  for (unsigned int i = 0; i < num_octets; ++i) {
    while (valid_bits >= 7) {
      uint8_t gsmchar = (shiftreg & 0x7f);
      shiftreg >>= 7;
      valid_bits -= 7;

      save_char(gsmchar, single, locking, output, outsiz, &in_escape,
                &output_chars);
    }
    shiftreg |= ((packed[i]) << valid_bits);
    valid_bits += 8;
  }

  if (valid_bits >= 7) {
    uint8_t gsmchar = (shiftreg & 0x7f);
    save_char(gsmchar, single, locking, output, outsiz, &in_escape,
              &output_chars);

    if (in_escape) {
      if (output_chars < outsiz) {
        output[output_chars] = ' ';
      }
      ++output_chars;
    }
  }

  return output_chars;
}

size_t gpp23038_8bit_to_unicode(const uint8_t *unpacked, size_t num_octets,
                                uint16_t *output, size_t outsiz,
                                enum gpp23038_shift_table single_shift,
                                enum gpp23038_shift_table locking_shift) {
  const struct lang_table *const single = &escape_tables[single_shift];
  const struct lang_table *const locking = &full_tables[locking_shift];

  int in_escape = 0;
  size_t output_chars = 0;

  for (unsigned int i = 0; i < num_octets; ++i) {
    uint8_t gsmchar = unpacked[i] & 0x7f;
    save_char(gsmchar, single, locking, output, outsiz, &in_escape,
              &output_chars);
  }

  return output_chars;
}

static int compare_mappings(const void *a, const void *b) {
  const struct mapping_entry *const entryA = a;
  const struct mapping_entry *const entryB = b;
  return entryA->uni - entryB->uni;
}

static int seek_mapping(uint16_t unichar, uint8_t *gsmchar,
                        const struct lang_table *lang) {
  struct mapping_entry seek;
  seek.uni = unichar;
  const struct mapping_entry *found =
      bsearch(&seek, lang->uni2gsm, lang->num_entries, sizeof(*lang->uni2gsm),
              compare_mappings);
  if (found) {
    *gsmchar = found->gsm;
    return 1;
  } else {
    return 0;
  }
}

struct shift_tables_rank {
  enum gpp23038_shift_table locking;
  enum gpp23038_shift_table single;
  unsigned int missed_chars;
  unsigned int used_escapes;
};

static unsigned int rank_get_score(const struct shift_tables_rank *rank) {
  /* get the "score" of a given shift table ranking. the lower, the better. */
  unsigned int udh_bytes = 0;
  if (rank->single != GPP23038_TABLE_DEFAULT) {
    udh_bytes += 3;
  }
  if (rank->locking != GPP23038_TABLE_DEFAULT) {
    udh_bytes += 3;
  }
  return (1000 * rank->missed_chars) + (100 * udh_bytes) + rank->used_escapes;
}

static int compare_ranks(const void *a, const void *b) {
  const struct shift_tables_rank *const rankA = a;
  const struct shift_tables_rank *const rankB = b;
  return rank_get_score(rankA) - rank_get_score(rankB);
}

static void rank_tables(struct shift_tables_rank *rank, const uint16_t *input,
                        size_t insiz, enum gpp23038_shift_table single,
                        enum gpp23038_shift_table locking) {
  rank->locking = locking;
  rank->single = single;
  rank->missed_chars = 0;
  rank->used_escapes = 0;
  for (unsigned int i = 0; i < insiz; ++i) {
    uint8_t gsmchar;
    if (seek_mapping(input[i], &gsmchar, &full_tables[locking]) == 0) {
      if (seek_mapping(input[i], &gsmchar, &escape_tables[single]) != 0) {
        rank->used_escapes++;
      } else {
        rank->missed_chars++;
      }
    }
  }
}

int gpp23038_seek_shift_table(const uint16_t *input, size_t insiz,
                              enum gpp23038_shift_table *single_shift,
                              enum gpp23038_shift_table *locking_shift) {
  struct shift_tables_rank ranks[GPP23038_TABLE__LAST * GPP23038_TABLE__LAST] =
      {0};

  for (enum gpp23038_shift_table locking = GPP23038_TABLE_DEFAULT;
       locking < GPP23038_TABLE__LAST; ++locking) {
    for (enum gpp23038_shift_table single = GPP23038_TABLE_DEFAULT;
         single < GPP23038_TABLE__LAST; ++single) {

      struct shift_tables_rank *rank =
          &ranks[(locking * GPP23038_TABLE__LAST) + single];

      rank_tables(rank, input, insiz, single, locking);

      if (rank->missed_chars == 0 && rank->used_escapes == 0 &&
          locking == GPP23038_TABLE_DEFAULT &&
          single == GPP23038_TABLE_DEFAULT) {
        /* can't get better than that. */
        *single_shift = single;
        *locking_shift = locking;
        return 0;
      }
    }
  }

  qsort(ranks, ARRAY_SIZE(ranks), sizeof(*ranks), compare_ranks);
  const struct shift_tables_rank *const best_rank = &ranks[0];
  *single_shift = best_rank->single;
  *locking_shift = best_rank->locking;

  return best_rank->missed_chars != 0;
}

#define GSM_SPACE_CHAR 0x20

size_t unicode_to_gpp23038_7bit(const uint16_t *input, size_t insiz,
                                uint8_t *output, size_t outsiz,
                                enum gpp23038_shift_table single_shift,
                                enum gpp23038_shift_table locking_shift) {
  const struct lang_table *const single = &escape_tables[single_shift];
  const struct lang_table *const locking = &full_tables[locking_shift];

  uint32_t shiftreg = 0;
  unsigned int valid_bits = 0;
  size_t out_idx = 0;

  for (unsigned int i = 0; i < insiz; ++i) {
    uint8_t gsmchar;
    if (seek_mapping(input[i], &gsmchar, locking) == 0) {
      if (seek_mapping(input[i], &gsmchar, single)) {
        shiftreg |= (GSM_ESCAPE_CHAR << valid_bits);
        valid_bits += 7;
      } else {
        gsmchar = GSM_SPACE_CHAR;
      }
    }

    shiftreg |= (gsmchar << valid_bits);
    valid_bits += 7;

    while (valid_bits >= 8) {
      valid_bits -= 8;
      if (out_idx < outsiz) {
        output[out_idx] = (shiftreg & 0xff);
      }
      out_idx++;
      shiftreg >>= 8;
    }
  }

  if (valid_bits > 0) {
    if (out_idx < outsiz) {
      output[out_idx] = (shiftreg & 0xff);
    }
    out_idx++;
  }

  return out_idx;
}
