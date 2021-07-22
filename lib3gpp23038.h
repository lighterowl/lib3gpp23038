#ifndef THREE_GPP_23038_H
#define THREE_GPP_23038_H

/**
 * @file lib3gpp23038.h
 * @brief Routines for converting the 7-bit GSM character encoding into Unicode
 * and back.
 * @see GSM 03.38 and 3GPP TS 23.038.
 */

#include <stddef.h>
#include <stdint.h>

/**
 * @brief An enumeration expressing the National Language Identifier as defined
 * by 3GPP TS 23.038 V16.0.0 (2020-07).
 * @note The NLI was introduced by 3GPP TS 23.038 V8.0.0, released in March
 * 2008 . Before that, the only available NLI was the "default" one, pointing to
 * the default GSM alphabet.
 * @note Support for messages encoded with non-default shift or locking tables
 * is not very widespread, and the range of encodable characters is rather
 * limited. Unless you have a very specific need to do that, it is recommended
 * that you encode your message with UCS-2 if it's not representable in the
 * default GSM character set.
 * @warning Messages using non-default shift tables must indicate this by using
 * appropriate User Data Header structures, sent alongside the message. You're
 * recommended to consult 3GPP TS 23.040 on how to encode the National Language
 * information into the UDH.
 */
enum gpp23038_shift_table {
  GPP23038_TABLE_DEFAULT,    /**< Default / no language */
  GPP23038_TABLE_TURKISH,    /**< Turkish */
  GPP23038_TABLE_SPANISH,    /**< Spanish */
  GPP23038_TABLE_PORTUGUESE, /**< Portuguese */
  GPP23038_TABLE_BENGALI,    /**< Bengali */
  GPP23038_TABLE_GUJARATI,   /**< Gujarati */
  GPP23038_TABLE_HINDI,      /**< Hindi */
  GPP23038_TABLE_KANNADA,    /**< Kannada */
  GPP23038_TABLE_MALAYALAM,  /**< Malayalam */
  GPP23038_TABLE_ORIYA,      /**< Oriya */
  GPP23038_TABLE_PUNJABI,    /**< Punjabi */
  GPP23038_TABLE_TAMIL,      /**< Tamil */
  GPP23038_TABLE_TELUGU,     /**< Telugu */
  GPP23038_TABLE_URDU,       /**< Urdu */
  GPP23038_TABLE__LAST
};

/**
 * @brief Decodes 7-bit packed GSM characters into Unicode.
 * @param packed Pointer to an array containing a GSM character bitstream,
 * packed according to Subclause 6.1.2.1.1 "Packing of 7-bit characters" of 3GPP
 * TS 23.038, i.e. concatenated 7-bit entities representing GSM characters.
 * @param num_octets The number of octets in @p packed .
 * @param output An array to write the decoded codepoints into.
 * @param outsiz The size of the output in 16-bit units.
 * @param single_shift The "single shift" table to use when decoding.
 * @param locking_shift The "locking shift" table to use when decoding.
 * @note Unrecognised characters are replaced by the space character.
 * @return The number of processed characters. If this number is greater than
 * @p outsiz , then the message in @p output is truncated as the buffer was too
 * small to hold the entire result.
 */
size_t gpp23038_7bit_to_unicode(const uint8_t *packed, size_t num_octets,
                                uint16_t *output, size_t outsiz,
                                enum gpp23038_shift_table single_shift,
                                enum gpp23038_shift_table locking_shift);

/**
 * @brief Does the same as @link gpp23038_7bit_to_unicode @endlink , but works
 * on unpacked 7-bit GSM characters, i.e. each input octet is assumed to contain
 * the encoded GSM character with the highest bit set to zero.
 */
size_t gpp23038_8bit_to_unicode(const uint8_t *unpacked, size_t num_octets,
                                uint16_t *output, size_t outsiz,
                                enum gpp23038_shift_table single_shift,
                                enum gpp23038_shift_table locking_shift);

/**
 * @brief Tries to locate the "best" shift tables combination to encode the
 * given input. "Best" is defined as "one that can encode all characters and
 * uses the least number of bits possible, including potential content of the
 * UDH".
 * @param input The sequence of code points to inspect.
 * @param insiz Number of code points in @p input .
 * @param single_shift Pointer to a variable where the single shift language
 * table, if found, will be written.
 * @param locking_shift Pointer to a variable where the locking shift language
 * table, if found, will be written.
 * @return Zero if the input can be encoded losslessly with the shift tables
 * written into @p single_shift and @p locking_shift . A nonzero value means
 * that there is no combination of single/locking shift tables which would
 * result in each character of the input being represented in the encoded GSM
 * output, and the values of @p single_shift and @p locking_shift are generally
 * undefined. You're best off encoding your message with UCS-2 in that case.
 */
int gpp23038_seek_shift_table(const uint16_t *input, size_t insiz,
                              enum gpp23038_shift_table *single_shift,
                              enum gpp23038_shift_table *locking_shift);

/**
 * @brief Encodes a sequence of Unicode code points into a 7-bit GSM character
 * bitstream, using the given combination of single/locking shift tables.
 * @param input Pointer to a sequence of Unicode code points to encode.
 * @param insiz Number of code points in @p input .
 * @param output Where to write the output bitstream into.
 * @param outsiz Number of octets in @p output .
 * @param single_shift The "single shift" table to use.
 * @param locking_shift The "locking shift" table to use.
 * @return The number of used octets in the output. If larger than @p outsiz ,
 * the buffer was too small and the bitstream located in there is truncated.
 */
size_t unicode_to_gpp23038_7bit(const uint16_t *input, size_t insiz,
                                uint8_t *output, size_t outsiz,
                                enum gpp23038_shift_table single_shift,
                                enum gpp23038_shift_table locking_shift);

#endif
