/****************************************************************************
 * apps/testing/crypto/driver/crc8.c
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <crypto/cryptodev.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef struct crypto_context
{
  int fd;
  int crypto_fd;
  struct session_op session;
  struct crypt_op cryp;
}
crypto_context;

typedef struct tb
{
  FAR char *data;
  int datalen;
  uint8_t result;
}
tb;

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const tb g_h1d_testcase[] =
{
    /* testcase 1-7: Individual testing */

    {
      "",
      0,
      0,
    },
    {
      "a",
      1,
      0xb2,
    },
    {
      "abc",
      3,
      0x9a,
    },
    {
      "message digest",
      14,
      0x2d,
    },
    {
      "abcdefghijklmnopqrstuvwxyz",
      26,
      0xd2,
    },
    {
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
      62,
      0x1f,
    },
    {
      "123456789012345678901234567890123456789"
      "01234567890123456789012345678901234567890",
      80,
      0x92,
    },

    /* testcase 8: test case 7 is divided into 8 parts */

    {
      "1234567890",
      10,
      0x92,
    }
};

static const tb g_h2f_testcase[] =
{
    /* testcase 1-7: Individual testing */

    {
      "",
      0,
      0,
    },
    {
      "a",
      1,
      0x07,
    },
    {
      "abc",
      3,
      0x41,
    },
    {
      "message digest",
      14,
      0xc1,
    },
    {
      "abcdefghijklmnopqrstuvwxyz",
      26,
      0xb4,
    },
    {
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
      62,
      0x20,
    },
    {
      "123456789012345678901234567890123456789"
      "01234567890123456789012345678901234567890",
      80,
      0x0c,
    },

    /* testcase 8: test case 7 is divided into 8 parts */

    {
      "1234567890",
      10,
      0x0c,
    }
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void syscrc8_free(FAR crypto_context *ctx)
{
  if (ctx->crypto_fd != 0)
    {
      close(ctx->crypto_fd);
      ctx->crypto_fd = 0;
    }

  if (ctx->fd != 0)
    {
      close(ctx->fd);
      ctx->fd = 0;
    }
}

static int syscrc8_init(FAR crypto_context *ctx)
{
  memset(ctx, 0, sizeof(crypto_context));
  if ((ctx->fd = open("/dev/crypto", O_RDWR, 0)) < 0)
    {
      warn("CRIOGET");
      syscrc8_free(ctx);
      return 1;
    }

  if (ioctl(ctx->fd, CRIOGET, &ctx->crypto_fd) == -1)
    {
      warn("CRIOGET");
      syscrc8_free(ctx);
      return 1;
    }

  return 0;
}

static int syscrc8_start(FAR crypto_context *ctx, uint8_t *key)
{
  ctx->session.mac = CRYPTO_CRC8;
  ctx->session.mackey = (caddr_t) key;
  ctx->session.mackeylen = sizeof(uint8_t) * 3;
  if (ioctl(ctx->crypto_fd, CIOCGSESSION, &ctx->session) == -1)
    {
      warn("CIOCGSESSION");
      syscrc8_free(ctx);
      return 1;
    }

  ctx->cryp.ses = ctx->session.ses;
  return 0;
}

static int syscrc8_update(FAR crypto_context *ctx, FAR const char *s,
                           size_t len)
{
  ctx->cryp.op = COP_ENCRYPT;
  ctx->cryp.flags |= COP_FLAG_UPDATE;
  ctx->cryp.src = (caddr_t) s;
  ctx->cryp.len = len;
  if (ioctl(ctx->crypto_fd, CIOCCRYPT, &ctx->cryp) == -1)
    {
      warn("CIOCCRYPT");
      return 1;
    }

  return 0;
}

static int syscrc8_finish(FAR crypto_context *ctx, FAR uint8_t *out)
{
  ctx->cryp.flags = 0;
  ctx->cryp.mac = (caddr_t) out;

  if (ioctl(ctx->crypto_fd, CIOCCRYPT, &ctx->cryp) == -1)
    {
      warn("CIOCCRYPT");
      return 1;
    }

  if (ioctl(ctx->crypto_fd, CIOCFSESSION, &ctx->session.ses) == -1)
    {
      warn("CIOCFSESSION");
      return 1;
    };

  return 0;
}

static int match(const uint8_t a, const uint8_t b)
{
  if (a == b)
    {
      return 0;
    }

  warnx("crc8 mismatch");

  printf("%02x\n", a);
  printf("%02x\n", b);
  printf("\n");

  return 1;
}

static void test_crc8_h1d(void **state)
{
  crypto_context crc8_ctx;
  int i;
  uint8_t output;
  uint8_t key[3] =
    {
      0x1d, 0, 0
    };

  assert_int_equal(syscrc8_init(&crc8_ctx), 0);

  /* testcase 1-7: test crc8 vector */

  for (i = 0; i < sizeof(g_h1d_testcase) / sizeof(tb) - 1; i++)
    {
      assert_int_equal(syscrc8_start(&crc8_ctx, key), 0);
      assert_int_equal(syscrc8_update(&crc8_ctx, g_h1d_testcase[i].data,
                                      g_h1d_testcase[i].datalen), 0);

      assert_int_equal(syscrc8_finish(&crc8_ctx, &output), 0);
      assert_int_equal(match(g_h1d_testcase[i].result, output), 0);
    }

  /* testcase 8: test segmented computing capabilities in crc8 mode */

  for (i = 0; i < 8; i++)
    {
      assert_int_equal(syscrc8_start(&crc8_ctx, key), 0);

      assert_int_equal(syscrc8_update(&crc8_ctx, g_h1d_testcase[7].data,
                                      g_h1d_testcase[7].datalen), 0);

      assert_int_equal(syscrc8_finish(&crc8_ctx, &key[1]), 0);
    }

  assert_int_equal(match(g_h1d_testcase[7].result, key[1]), 0);
  syscrc8_free(&crc8_ctx);
}

static void test_crc8_h2f(void **state)
{
  crypto_context crc8_ctx;
  int i;
  uint8_t output;
  uint8_t key[3] =
    {
      0x2f, 0, 0
    };

  assert_int_equal(syscrc8_init(&crc8_ctx), 0);

  /* testcase 1-7: test crc8 vector */

  for (i = 0; i < sizeof(g_h2f_testcase) / sizeof(tb) - 1; i++)
    {
      assert_int_equal(syscrc8_start(&crc8_ctx, key), 0);
      assert_int_equal(syscrc8_update(&crc8_ctx, g_h2f_testcase[i].data,
                                       g_h2f_testcase[i].datalen), 0);

      assert_int_equal(syscrc8_finish(&crc8_ctx, &output), 0);
      assert_int_equal(match(g_h2f_testcase[i].result, output), 0);
    }

  /* testcase 8: test segmented computing capabilities in crc8 mode */

  for (i = 0; i < 8; i++)
    {
      assert_int_equal(syscrc8_start(&crc8_ctx, key), 0);

      assert_int_equal(syscrc8_update(&crc8_ctx, g_h2f_testcase[7].data,
                                       g_h2f_testcase[7].datalen), 0);

      assert_int_equal(syscrc8_finish(&crc8_ctx, &key[1]), 0);
    }

  assert_int_equal(match(g_h2f_testcase[7].result, key[1]), 0);
  syscrc8_free(&crc8_ctx);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest crc8_tests[] =
    {
      cmocka_unit_test(test_crc8_h1d),
      cmocka_unit_test(test_crc8_h2f),
    };

  return cmocka_run_group_tests(crc8_tests, NULL, NULL);
}
