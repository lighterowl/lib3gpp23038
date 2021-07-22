#include <check.h>

#include "lib3gpp23038.h"

#include <stdlib.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

START_TEST(decode_default_gsm_7bit) {
  const uint8_t gsm[] = {0xc8, 0x32, 0x9b, 0xfd, 0x06};
  const uint16_t uni[] = {'H', 'e', 'l', 'l', 'o'};
  uint16_t buf[ARRAY_SIZE(uni)];

  size_t rv =
      gpp23038_7bit_to_unicode(gsm, sizeof(gsm), buf, ARRAY_SIZE(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_DEFAULT);

  ck_assert_uint_eq(rv, ARRAY_SIZE(uni));
  ck_assert_mem_eq(buf, uni, sizeof(uni));
}
END_TEST

START_TEST(decode_replaces_incomplete_escape_by_space) {
  const uint8_t gsm[] = {0x6f, 0x74, 0x38, 0xbd, 0x01};
  const uint16_t uni[] = {'o', 'h', 'a', 'i', ' '};
  uint16_t buf[ARRAY_SIZE(uni)];

  size_t rv =
      gpp23038_7bit_to_unicode(gsm, sizeof(gsm), buf, ARRAY_SIZE(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_DEFAULT);

  ck_assert_uint_eq(rv, ARRAY_SIZE(uni));
  ck_assert_mem_eq(buf, uni, sizeof(uni));
}
END_TEST

START_TEST(decode_replaces_unrecognised_escape_by_space) {
  const uint8_t gsm[] = {0xe6, 0xf7, 0x5b, 0x1c, 0x96, 0x6f, 0x0e};
  const uint16_t uni[] = {'f', 'o', 'o', 'b', 'a', 'r', ' '};
  uint16_t buf[ARRAY_SIZE(uni)];

  size_t rv =
      gpp23038_7bit_to_unicode(gsm, sizeof(gsm), buf, ARRAY_SIZE(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_DEFAULT);

  ck_assert_uint_eq(rv, ARRAY_SIZE(uni));
  ck_assert_mem_eq(buf, uni, sizeof(uni));
}
END_TEST

START_TEST(decode_default_escaped_gsm_7bit) {
  const uint8_t gsm[] = {0xe2, 0x32, 0x5d, 0xbe, 0x3f, 0xd3,
                         0x41, 0x34, 0xd9, 0xa6, 0x0c};
  const uint16_t uni[] = {'b', 'e', 't', 'r', 0xe4,  'g',
                          't', ' ', '4', '2', 0x20ac};
  uint16_t buf[ARRAY_SIZE(uni)];

  size_t rv =
      gpp23038_7bit_to_unicode(gsm, sizeof(gsm), buf, ARRAY_SIZE(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_DEFAULT);

  ck_assert_uint_eq(rv, ARRAY_SIZE(uni));
  ck_assert_mem_eq(buf, uni, sizeof(uni));
}
END_TEST

START_TEST(decode_returns_number_of_converted_characters) {
  const uint8_t gsm[] = {0xea, 0xb2, 0x38, 0x3c, 0x06, 0xc1, 0xd3, 0x73};

  uint16_t buf[2];

  size_t rv =
      gpp23038_7bit_to_unicode(gsm, sizeof(gsm), buf, ARRAY_SIZE(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_DEFAULT);

  /* there are 9 characters in the gsm bitstream, but the output buffer is only
   * two characters : the function should return the total number of characters
   * that would be written if the output buffer had sufficient space. */
  ck_assert_uint_eq(rv, 9);

  /* the same should work if we just pass 0 size and null buffer. */
  rv = gpp23038_7bit_to_unicode(gsm, sizeof(gsm), 0, 0, GPP23038_TABLE_DEFAULT,
                                GPP23038_TABLE_DEFAULT);
  ck_assert_uint_eq(rv, 9);
}
END_TEST

START_TEST(decode_uses_nondefault_alphabet_tables) {
  const uint8_t gsm[] = {0xb2, 0xe8, 0x6f, 0x43, 0x01};
  const uint16_t uni[] = {'2', 0x0a3f, '?', '^'};

  uint16_t buf[ARRAY_SIZE(uni)];
  size_t rv =
      gpp23038_7bit_to_unicode(gsm, sizeof(gsm), buf, ARRAY_SIZE(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_PUNJABI);

  ck_assert_uint_eq(rv, ARRAY_SIZE(uni));
  ck_assert_mem_eq(buf, uni, sizeof(uni));
}
END_TEST

START_TEST(decode_uses_nondefault_escape_tables) {
  const uint8_t gsm[] = {0xe0, 0xf1, 0xbb, 0xbd, 0x79, 0x83, 0xca,
                         0x73, 0xfa, 0x26, 0x3c, 0xff, 0x01};
  const uint16_t uni[] = {0xbf, 'c', 'o', 'm',  0xf3, ' ',
                          'e',  's', 't', 0xe1, 's',  '?'};

  uint16_t buf[ARRAY_SIZE(uni)];
  size_t rv =
      gpp23038_7bit_to_unicode(gsm, sizeof(gsm), buf, ARRAY_SIZE(buf),
                               GPP23038_TABLE_SPANISH, GPP23038_TABLE_DEFAULT);

  ck_assert_uint_eq(rv, ARRAY_SIZE(uni));
  ck_assert_mem_eq(buf, uni, sizeof(uni));
}
END_TEST

START_TEST(decode_can_use_different_alphabet_and_escape_tables) {
  const uint8_t gsm[] = {0xa2, 0x11, 0x29, 0xb4, 0xd9, 0x03};
  const uint16_t uni[] = {0x0c9f, 0x0ca0, 0x0caa, '!', 0xe3};

  uint16_t buf[ARRAY_SIZE(uni)];
  size_t rv = gpp23038_7bit_to_unicode(gsm, sizeof(gsm), buf, ARRAY_SIZE(buf),
                                       GPP23038_TABLE_PORTUGUESE,
                                       GPP23038_TABLE_KANNADA);

  ck_assert_uint_eq(rv, ARRAY_SIZE(uni));
  ck_assert_mem_eq(buf, uni, sizeof(uni));
}
END_TEST

START_TEST(decode_default_gsm_8bit) {
  const uint8_t gsm_unpacked[] = {0x32, 0x33, 0x00, 0x02};
  const uint16_t uni[] = {'2', '3', '@', '$'};
  uint16_t buf[ARRAY_SIZE(uni)];

  size_t rv = gpp23038_8bit_to_unicode(gsm_unpacked, sizeof(gsm_unpacked), buf,
                                       ARRAY_SIZE(buf), GPP23038_TABLE_DEFAULT,
                                       GPP23038_TABLE_DEFAULT);
  ck_assert_uint_eq(rv, ARRAY_SIZE(buf));
  ck_assert_mem_eq(buf, uni, sizeof(uni));
}
END_TEST

START_TEST(encode_default_gsm_7bit) {
  const uint8_t gsm[] = {0xc8, 0x32, 0x9b, 0xfd, 0x06};
  const uint16_t uni[] = {'H', 'e', 'l', 'l', 'o'};
  uint8_t buf[ARRAY_SIZE(gsm)];

  size_t rv =
      unicode_to_gpp23038_7bit(uni, ARRAY_SIZE(uni), buf, sizeof(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_DEFAULT);

  ck_assert_uint_eq(rv, ARRAY_SIZE(gsm));
  ck_assert_mem_eq(buf, gsm, sizeof(gsm));
}
END_TEST

START_TEST(encode_escaped_gsm_7bit) {
  const uint8_t gsm[] = {0xe2, 0x32, 0x5d, 0xbe, 0x3f, 0xd3,
                         0x41, 0x34, 0xd9, 0xa6, 0x0c};
  const uint16_t uni[] = {'b', 'e', 't', 'r', 0xe4,  'g',
                          't', ' ', '4', '2', 0x20ac};
  uint8_t buf[ARRAY_SIZE(uni)];

  size_t rv =
      unicode_to_gpp23038_7bit(uni, ARRAY_SIZE(uni), buf, sizeof(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_DEFAULT);

  ck_assert_uint_eq(rv, ARRAY_SIZE(gsm));
  ck_assert_mem_eq(buf, gsm, sizeof(gsm));
}
END_TEST

START_TEST(encode_replaces_unknown_char_with_space) {
  const uint8_t gsm[] = {0xf0, 0x70, 0x1e, 0x34, 0x6b,
                         0x82, 0x40, 0xee, 0xf7, 0x1d};
  const uint16_t uni[] = {'p',    'a', 'y', ' ', '3', 'M',
                          0x20bd, ' ', 'n', 'o', 'w'};
  uint8_t buf[ARRAY_SIZE(uni)];

  size_t rv =
      unicode_to_gpp23038_7bit(uni, ARRAY_SIZE(uni), buf, sizeof(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_DEFAULT);

  ck_assert_uint_eq(rv, ARRAY_SIZE(gsm));
  ck_assert_mem_eq(buf, gsm, sizeof(gsm));
}
END_TEST

START_TEST(encode_returns_real_size_with_smaller_buffer) {
  const uint8_t gsm[] = {0xf0, 0x70, 0x1e, 0x34, 0x6b,
                         0x82, 0x40, 0xee, 0xf7, 0x1d};
  const uint16_t uni[] = {'p',    'a', 'y', ' ', '3', 'M',
                          0x20bd, ' ', 'n', 'o', 'w'};
  uint8_t buf[2];

  size_t rv =
      unicode_to_gpp23038_7bit(uni, ARRAY_SIZE(uni), buf, sizeof(buf),
                               GPP23038_TABLE_DEFAULT, GPP23038_TABLE_DEFAULT);

  ck_assert_uint_eq(rv, ARRAY_SIZE(gsm));
  ck_assert_mem_eq(buf, gsm, 2);
}
END_TEST

START_TEST(seek_default) {
  const uint16_t uni[] = {'2', '3', '@', '$'};
  enum gpp23038_shift_table single, locking;
  int rv = gpp23038_seek_shift_table(uni, ARRAY_SIZE(uni), &single, &locking);

  ck_assert_int_eq(rv, 0);
  ck_assert_uint_eq(single, GPP23038_TABLE_DEFAULT);
  ck_assert_uint_eq(locking, GPP23038_TABLE_DEFAULT);
}
END_TEST

START_TEST(seek_default_one_escape) {
  const uint16_t uni[] = {'2', '3', 0x20ac};
  enum gpp23038_shift_table single, locking;
  int rv = gpp23038_seek_shift_table(uni, ARRAY_SIZE(uni), &single, &locking);

  ck_assert_int_eq(rv, 0);
  ck_assert_uint_eq(single, GPP23038_TABLE_DEFAULT);
  ck_assert_uint_eq(locking, GPP23038_TABLE_DEFAULT);
}
END_TEST

START_TEST(seek_no_match) {
  /* a mix of Cyrillic, Latin, Greek, and Telugu characters. */
  const uint16_t uni[] = {0x416, 'N', 0x03a9, 0x0c03, '$', '@'};
  enum gpp23038_shift_table single, locking;
  int rv = gpp23038_seek_shift_table(uni, ARRAY_SIZE(uni), &single, &locking);

  /* no match : the values in single and locking are irrelevant, though with the
   * current implementation should point at the tables which provide the
   * smallest possible representation. don't test for this as the documentation
   * explicitly states the output values are undefined in this case. */
  ck_assert_int_eq(rv, 1);
}
END_TEST

static Suite *gpp23038_suite(void) {
  Suite *s = suite_create("3GPP_23.038");

  TCase *decode_tc = tcase_create("Decode");
  tcase_add_test(decode_tc, decode_default_gsm_7bit);
  tcase_add_test(decode_tc, decode_replaces_incomplete_escape_by_space);
  tcase_add_test(decode_tc, decode_replaces_unrecognised_escape_by_space);
  tcase_add_test(decode_tc, decode_default_escaped_gsm_7bit);
  tcase_add_test(decode_tc, decode_returns_number_of_converted_characters);
  tcase_add_test(decode_tc, decode_uses_nondefault_alphabet_tables);
  tcase_add_test(decode_tc, decode_uses_nondefault_escape_tables);
  tcase_add_test(decode_tc,
                 decode_can_use_different_alphabet_and_escape_tables);
  tcase_add_test(decode_tc, decode_default_gsm_8bit);
  suite_add_tcase(s, decode_tc);

  TCase *encode_tc = tcase_create("Encode");
  tcase_add_test(encode_tc, encode_default_gsm_7bit);
  tcase_add_test(encode_tc, encode_escaped_gsm_7bit);
  tcase_add_test(encode_tc, encode_replaces_unknown_char_with_space);
  tcase_add_test(encode_tc, encode_returns_real_size_with_smaller_buffer);
  suite_add_tcase(s, encode_tc);

  TCase *seek_tc = tcase_create("Seek");
  tcase_add_test(seek_tc, seek_default);
  tcase_add_test(seek_tc, seek_default_one_escape);
  tcase_add_test(seek_tc, seek_no_match);
  suite_add_tcase(s, seek_tc);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s = gpp23038_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
