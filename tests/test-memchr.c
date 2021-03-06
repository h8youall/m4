/*
 * Copyright (C) 2008 Free Software Foundation
 * Written by Eric Blake and Bruno Haible
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(expr) \
  do									     \
    {									     \
      if (!(expr))							     \
	{								     \
	  fprintf (stderr, "%s:%d: assertion failed\n", __FILE__, __LINE__); \
	  fflush (stderr);						     \
	  abort ();							     \
	}								     \
    }									     \
  while (0)

/* Calculating void * + int is not portable, so this wrapper converts
   to char * to make the tests easier to write.  */
#define MEMCHR (char *) memchr

int
main ()
{
  void *nil = NULL; /* Use to avoid gcc attribute((nonnull)) warnings.  */

  size_t n = 0x100000;
  char *input = malloc (n);
  ASSERT (input);

  input[0] = 'a';
  input[1] = 'b';
  memset (input + 2, 'c', 1024);
  memset (input + 1026, 'd', n - 1028);
  input[n - 2] = 'e';
  input[n - 1] = 'a';

  /* Basic behavior tests.  */
  ASSERT (MEMCHR (input, 'a', n) == input);

  ASSERT (MEMCHR (input, 'a', 0) == NULL);
  ASSERT (MEMCHR (nil, 'a', 0) == NULL);

  ASSERT (MEMCHR (input, 'b', n) == input + 1);
  ASSERT (MEMCHR (input, 'c', n) == input + 2);
  ASSERT (MEMCHR (input, 'd', n) == input + 1026);

  ASSERT (MEMCHR (input + 1, 'a', n - 1) == input + n - 1);
  ASSERT (MEMCHR (input + 1, 'e', n - 1) == input + n - 2);

  ASSERT (MEMCHR (input, 'f', n) == NULL);
  ASSERT (MEMCHR (input, '\0', n) == NULL);

  /* Check that a very long haystack is handled quickly if the byte is
     found near the beginning.  */
  {
    size_t repeat = 10000;
    for (; repeat > 0; repeat--)
      {
	ASSERT (MEMCHR (input, 'c', n) == input + 2);
      }
  }

  /* Alignment tests.  */
  {
    int i, j;
    for (i = 0; i < 32; i++)
      {
	for (j = 0; j < 256; j++)
	  input[i + j] = j;
	for (j = 0; j < 256; j++)
	  {
	    ASSERT (MEMCHR (input + i, j, 256) == input + i + j);
	  }
      }
  }

  free (input);

  return 0;
}
