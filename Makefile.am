
AM_CFLAGS = -O2 -Wall -Wno-unused-function

lib_LIBRARIES = libhigherc.a

libhigherc_a_SOURCES = alloc.c byte.c bytewise.c crc32.c fatal.c io.c	\
    list.c n.c pbuf.c rand.c s.c sha1.c str.c tagid.c

include_HEADERS = higherc/alloc.h higherc/byte.h higherc/bytewise.h	\
    higherc/crc32.h higherc/higherc.h higherc/list.h higherc/n.h	\
    higherc/pbuf.h higherc/rand.h higherc/s.h higherc/sha1.h		\
    higherc/str.h higherc/tagid.h higherc/tok.h

bin_PROGRAMS = sample_asciionly sample_cat sample_cat2 test_0 test_1	\
    test_2 test_3 test_4 test_5 test_byte test_bytewise test_crc32	\
    test_list_0 test_n_0 test_n_1 test_pbuf_0 test_pbuf_1 test_rand	\
    test_s_0 test_s_1 test_s_2 test_s_3 test_s_4 test_sha1 test_tagid	\
    test_tok_0 test_sha1_1

LDADD = libhigherc.a

test_0_SOURCES = test_0.c
test_1_SOURCES = test_1.c
test_2_SOURCES = test_2.c
test_3_SOURCES = test_3.c
test_4_SOURCES = test_4.c test_4.h
test_5_SOURCES = test_5.c test_5.h
test_byte_SOURCES = test_byte.c
test_bytewise_SOURCES = test_bytewise.c
test_crc32_SOURCES = test_crc32.c
test_list_0_SOURCES = test_list_0.c
test_n_0_SOURCES = test_n_0.c
test_n_1_SOURCES = test_n_1.c
test_pbuf_0_SOURCES = test_pbuf_0.c
test_pbuf_1_SOURCES = test_pbuf_1.c
test_rand_SOURCES = test_rand.c
test_s_0_SOURCES = test_s_0.c
test_s_1_SOURCES = test_s_1.c
test_s_2_SOURCES = test_s_2.c
test_s_3_SOURCES = test_s_3.c
test_s_4_SOURCES = test_s_4.c
test_sha1_SOURCES = test_sha1.c
test_sha1_1_SOURCES = test_sha1_1.c
test_tagid_SOURCES = test_tagid.c
test_tok_0_SOURCES = test_tok_0.c
sample_asciionly_SOURCES = sample_asciionly.c
sample_cat_SOURCES = sample_cat.c
sample_cat2_SOURCES = sample_cat2.c

EXTRA_DIST = bignum.escript--c_verify bignum2.escript--c_verify
