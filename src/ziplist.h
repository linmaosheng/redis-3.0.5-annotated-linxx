/*
 * Copyright (c) 2009-2012, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define ZIPLIST_HEAD 0
#define ZIPLIST_TAIL 1

/*
 * 作用：创建一个新的压缩列表
 * Algorithm complexity: O(1)
 */
unsigned char *ziplistNew(void);
/*
 * 作用：创建一个包含给定值的新节点，并将这个新节点添加到压缩列表的表头或者表尾
 * Algorithm complexity: 平均O(N)，最坏O(N^2)
 */
unsigned char *ziplistPush(unsigned char *zl, unsigned char *s, unsigned int slen, int where);
/*
 * 作用：返回压缩列表给定索引上的节点
 * Algorithm complexity: O(N)
 */
unsigned char *ziplistIndex(unsigned char *zl, int index);
/*
 * 作用：返回给定节点的下一个节点
 * Algorithm complexity: O(1)
 */
unsigned char *ziplistNext(unsigned char *zl, unsigned char *p);
/*
 * 作用：返回给定节点的前一个节点
 * Algorithm complexity: O(1)
 */
unsigned char *ziplistPrev(unsigned char *zl, unsigned char *p);
/*
 * 作用：获取给定节点的前一个节点
 * Algorithm complexity: O(1)
 */
unsigned int ziplistGet(unsigned char *p, unsigned char **sval, unsigned int *slen, long long *lval);
/*
 * 作用：将包含给定值的新节点插入到给定节点之后
 * Algorithm complexity: 平均O(N)，最坏O(N^2)
 */
unsigned char *ziplistInsert(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen);
/*
 * 作用：从压缩列表中删除给定的节点
 * Algorithm complexity: 平均O(N)，最坏O(N^2)
 */
unsigned char *ziplistDelete(unsigned char *zl, unsigned char **p);
/*
 * 作用：删除压缩列表在给定索引上的连续多个节点
 * Algorithm complexity: 平均O(N)，最坏O(N^2)
 */
unsigned char *ziplistDeleteRange(unsigned char *zl, unsigned int index, unsigned int num);
unsigned int ziplistCompare(unsigned char *p, unsigned char *s, unsigned int slen);
/*
 * 作用：在压缩列表中查找并返回包含了给定值的节点
 * Algorithm complexity: 因为节点的值可能是一个字节数组，所以检查节点值和给定值是否相等的复杂度为O(N)，而查找整个列表的复杂度则为O(N^2)
 */
unsigned char *ziplistFind(unsigned char *p, unsigned char *vstr, unsigned int vlen, unsigned int skip);
/*
 * 作用：返回压缩列表目前占用的内存字节数
 * ALgorithm complexity: O(1)
 */
unsigned int ziplistLen(unsigned char *zl);
/*
 * 作用：返回压缩列表目前占用的内存字节数
 * ALgorithm complexity: O(1)
 */
size_t ziplistBlobLen(unsigned char *zl);
