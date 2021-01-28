/*-
 * This code is derived from OpenBSD's libc/regex, original license follows:
 *
 * Copyright (c) 1992 Henry Spencer.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Henry Spencer of the University of Toronto.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)regex.h	8.1 (Berkeley) 6/2/93
 */

#ifndef _REGEX_IMPL_H_ // HLSL Change
#define _REGEX_IMPL_H_ // HLSL Change

#include "dxc/Support/WinAdapter.h" // HLSL Change
#include <sys/types.h>

typedef off_t llvm37_regoff_t;
typedef struct {
  llvm37_regoff_t rm_so;		/* start of match */
  llvm37_regoff_t rm_eo;		/* end of match */
} llvm37_regmatch_t;

typedef struct llvm37_regex {
  int re_magic;
  size_t re_nsub;		/* number of parenthesized subexpressions */
  const char *re_endp;	/* end pointer for REG_PEND */
  struct re_guts *re_g;	/* none of your business :-) */
} llvm37_regex_t;

/* llvm37_regcomp() flags */
#define	REG_BASIC	0000
#define	REG_EXTENDED	0001
#define	REG_ICASE	0002
#define	REG_NOSUB	0004
#define	REG_NEWLINE	0010
#define	REG_NOSPEC	0020
#define	REG_PEND	0040
#define	REG_DUMP	0200

/* llvm37_regerror() flags */
#define	REG_NOMATCH	 1
#define	REG_BADPAT	 2
#define	REG_ECOLLATE	 3
#define	REG_ECTYPE	 4
#define	REG_EESCAPE	 5
#define	REG_ESUBREG	 6
#define	REG_EBRACK	 7
#define	REG_EPAREN	 8
#define	REG_EBRACE	 9
#define	REG_BADBR	10
#define	REG_ERANGE	11
#define	REG_ESPACE	12
#define	REG_BADRPT	13
#define	REG_EMPTY	14
#define	REG_ASSERT	15
#define	REG_INVARG	16
#define	REG_ATOI	255	/* convert name to number (!) */
#define	REG_ITOA	0400	/* convert number to name (!) */

/* llvm37_regexec() flags */
#define	REG_NOTBOL	00001
#define	REG_NOTEOL	00002
#define	REG_STARTEND	00004
#define	REG_TRACE	00400	/* tracing of execution */
#define	REG_LARGE	01000	/* force large representation */
#define	REG_BACKR	02000	/* force use of backref code */

#ifdef __cplusplus
extern "C" {
#endif

int	llvm37_regcomp(llvm37_regex_t *, const char *, int);
int	llvm37_regexec(const llvm37_regex_t *, const char *, size_t, 
                     llvm37_regmatch_t [], int);
void	llvm37_regfree(llvm37_regex_t *);

//size_t	llvm37_regerror(int, const llvm37_regex_t *, char *, size_t);
//size_t  llvm37_strlcpy(char *dst, const char *src, size_t siz);

size_t	llvm37_regerror(
	int errcode,
	const llvm37_regex_t *preg,
	_Out_writes_all_(errbuf_size) char *errbuf,
	size_t errbuf_size);

size_t  llvm37_strlcpy(
	_Out_writes_z_(siz) char *dst,
	_In_z_ const char *src,
	size_t siz);

#ifdef __cplusplus
}
#endif

#endif /* !_REGEX_IMPL_H_ */ // HLSL Change
