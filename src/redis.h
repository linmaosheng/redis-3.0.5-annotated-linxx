/*
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

#ifndef __REDIS_H
#define __REDIS_H

#include "fmacros.h"
#include "config.h"
#include "solarisfixes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <syslog.h>
#include <netinet/in.h>
#include <lua.h>
#include <signal.h>

typedef long long mstime_t; /* millisecond time type. */

#include "ae.h"      /* Event driven programming library */
#include "sds.h"     /* Dynamic safe strings */
#include "dict.h"    /* Hash tables */
#include "adlist.h"  /* Linked lists */
#include "zmalloc.h" /* total memory usage aware version of malloc/free */
#include "anet.h"    /* Networking the easy way */
#include "ziplist.h" /* Compact list data structure */
#include "intset.h"  /* Compact integer set structure */
#include "version.h" /* Version macro */
#include "util.h"    /* Misc functions useful in many places */
#include "latency.h" /* Latency monitor API */
#include "sparkline.h" /* ASII graphs API */

/* Error codes */
#define REDIS_OK                0
#define REDIS_ERR               -1

/* Static server configuration */
// 默认的服务器配置值
#define REDIS_DEFAULT_HZ        10      /* Time interrupt calls/sec. */
#define REDIS_MIN_HZ            1
#define REDIS_MAX_HZ            500
#define REDIS_SERVERPORT        6379    /* TCP port */
#define REDIS_TCP_BACKLOG       511     /* TCP listen backlog */
#define REDIS_MAXIDLETIME       0       /* default client timeout: infinite */
#define REDIS_DEFAULT_DBNUM     16
#define REDIS_CONFIGLINE_MAX    1024
#define REDIS_DBCRON_DBS_PER_CALL 16
#define REDIS_MAX_WRITE_PER_EVENT (1024*64)
#define REDIS_SHARED_SELECT_CMDS 10
/*
 * 创建共享字符串的数量(目前Redis会在初始化服务器时，创建一万个字符串对象，这些对象包含了从0到9999的所有整数值，当服务器需要用到值为0到9999的字符串对象时，服务
 * 器就会使用这些共享对象，而不是新创建对象。)
 */
#define REDIS_SHARED_INTEGERS 10000
#define REDIS_SHARED_BULKHDR_LEN 32
#define REDIS_MAX_LOGMSG_LEN    1024 /* Default maximum length of syslog messages */
#define REDIS_AOF_REWRITE_PERC  100
#define REDIS_AOF_REWRITE_MIN_SIZE (64*1024*1024)
#define REDIS_AOF_REWRITE_ITEMS_PER_CMD 64
#define REDIS_SLOWLOG_LOG_SLOWER_THAN 10000
#define REDIS_SLOWLOG_MAX_LEN 128
#define REDIS_MAX_CLIENTS 10000
#define REDIS_AUTHPASS_MAX_LEN 512
#define REDIS_DEFAULT_SLAVE_PRIORITY 100
#define REDIS_REPL_TIMEOUT 60
#define REDIS_REPL_PING_SLAVE_PERIOD 10
#define REDIS_RUN_ID_SIZE 40
#define REDIS_EOF_MARK_SIZE 40
#define REDIS_DEFAULT_REPL_BACKLOG_SIZE (1024*1024)    /* 1mb */
#define REDIS_DEFAULT_REPL_BACKLOG_TIME_LIMIT (60*60)  /* 1 hour */
#define REDIS_REPL_BACKLOG_MIN_SIZE (1024*16)          /* 16k */
#define REDIS_BGSAVE_RETRY_DELAY 5 /* Wait a few secs before trying again. */
#define REDIS_DEFAULT_PID_FILE "/var/run/redis.pid"
#define REDIS_DEFAULT_SYSLOG_IDENT "redis"
#define REDIS_DEFAULT_CLUSTER_CONFIG_FILE "nodes.conf"
#define REDIS_DEFAULT_DAEMONIZE 0
#define REDIS_DEFAULT_UNIX_SOCKET_PERM 0
#define REDIS_DEFAULT_TCP_KEEPALIVE 0
#define REDIS_DEFAULT_LOGFILE ""
#define REDIS_DEFAULT_SYSLOG_ENABLED 0
#define REDIS_DEFAULT_STOP_WRITES_ON_BGSAVE_ERROR 1
#define REDIS_DEFAULT_RDB_COMPRESSION 1
#define REDIS_DEFAULT_RDB_CHECKSUM 1
#define REDIS_DEFAULT_RDB_FILENAME "dump.rdb"
#define REDIS_DEFAULT_REPL_DISKLESS_SYNC 0
#define REDIS_DEFAULT_REPL_DISKLESS_SYNC_DELAY 5
#define REDIS_DEFAULT_SLAVE_SERVE_STALE_DATA 1
#define REDIS_DEFAULT_SLAVE_READ_ONLY 1
#define REDIS_DEFAULT_REPL_DISABLE_TCP_NODELAY 0
#define REDIS_DEFAULT_MAXMEMORY 0
#define REDIS_DEFAULT_MAXMEMORY_SAMPLES 5
#define REDIS_DEFAULT_AOF_FILENAME "appendonly.aof"
#define REDIS_DEFAULT_AOF_NO_FSYNC_ON_REWRITE 0
#define REDIS_DEFAULT_AOF_LOAD_TRUNCATED 1
#define REDIS_DEFAULT_ACTIVE_REHASHING 1
#define REDIS_DEFAULT_AOF_REWRITE_INCREMENTAL_FSYNC 1
#define REDIS_DEFAULT_MIN_SLAVES_TO_WRITE 0
#define REDIS_DEFAULT_MIN_SLAVES_MAX_LAG 10
#define REDIS_IP_STR_LEN 46 /* INET6_ADDRSTRLEN is 46, but we need to be sure */
#define REDIS_PEER_ID_LEN (REDIS_IP_STR_LEN+32) /* Must be enough for ip:port */
#define REDIS_BINDADDR_MAX 16
#define REDIS_MIN_RESERVED_FDS 32
#define REDIS_DEFAULT_LATENCY_MONITOR_THRESHOLD 0

#define ACTIVE_EXPIRE_CYCLE_LOOKUPS_PER_LOOP 20 /* Loopkups per loop. */
#define ACTIVE_EXPIRE_CYCLE_FAST_DURATION 1000 /* Microseconds */
#define ACTIVE_EXPIRE_CYCLE_SLOW_TIME_PERC 25 /* CPU max % for keys collection */
#define ACTIVE_EXPIRE_CYCLE_SLOW 0
#define ACTIVE_EXPIRE_CYCLE_FAST 1

/* Instantaneous metrics tracking. */
#define REDIS_METRIC_SAMPLES 16     /* Number of samples per metric. */
#define REDIS_METRIC_COMMAND 0      /* Number of commands executed. */
#define REDIS_METRIC_NET_INPUT 1    /* Bytes read to network .*/
#define REDIS_METRIC_NET_OUTPUT 2   /* Bytes written to network. */
#define REDIS_METRIC_COUNT 3

/* Protocol and I/O related defines */
#define REDIS_MAX_QUERYBUF_LEN  (1024*1024*1024) /* 1GB max query buffer. */
#define REDIS_IOBUF_LEN         (1024*16)  /* Generic I/O buffer size */
#define REDIS_REPLY_CHUNK_BYTES (16*1024) /* 16k output buffer */
#define REDIS_INLINE_MAX_SIZE   (1024*64) /* Max size of inline reads */
#define REDIS_MBULK_BIG_ARG     (1024*32)
#define REDIS_LONGSTR_SIZE      21          /* Bytes needed for long -> str */
// 指示AOF程序每累积这个凉的写入数据
// 就执行一次显示fsync
#define REDIS_AOF_AUTOSYNC_BYTES (1024*1024*32) /* fdatasync every 32MB */
/* When configuring the Redis eventloop, we setup it so that the total number
 * of file descriptors we can handle are server.maxclients + RESERVED_FDS + FDSET_INCR
 * that is our safety margin. */
#define REDIS_EVENTLOOP_FDSET_INCR (REDIS_MIN_RESERVED_FDS+96)

/* Hash table parameters */
#define REDIS_HT_MINFILL        10      /* Minimal hash table fill 10% */

/* Command flags. Please check the command table defined in the redis.c file
 * for more information about the meaning of every flag. */
// 命令标志
#define REDIS_CMD_WRITE 1                   /* "w" flag */
#define REDIS_CMD_READONLY 2                /* "r" flag */
#define REDIS_CMD_DENYOOM 4                 /* "m" flag */
#define REDIS_CMD_NOT_USED_1 8              /* no longer used flag */
#define REDIS_CMD_ADMIN 16                  /* "a" flag */
#define REDIS_CMD_PUBSUB 32                 /* "p" flag */
#define REDIS_CMD_NOSCRIPT  64              /* "s" flag */
#define REDIS_CMD_RANDOM 128                /* "R" flag */
#define REDIS_CMD_SORT_FOR_SCRIPT 256       /* "S" flag */
#define REDIS_CMD_LOADING 512               /* "l" flag */
#define REDIS_CMD_STALE 1024                /* "t" flag */
#define REDIS_CMD_SKIP_MONITOR 2048         /* "M" flag */
#define REDIS_CMD_ASKING 4096               /* "k" flag */
#define REDIS_CMD_FAST 8192                 /* "F" flag */

/* Object types */
// 对象类型
/*
 * 字符串对象(编码：REDIS_ENCODING_INT；对象：使用整数值实现的字符串对象。编码：REDIS_ENCODING_EMBSTR；对象：使用embstr编码的简单动态字符串实现的字符串对象
 * 。编码：REDIS_ENCODING_RAW；对象：使用简单动态字符串实现的字符串对象。)
 */
#define REDIS_STRING 0
// 列表对象(编码：REDIS_ENCODING_ZIPLIST；对象：使用压缩列表实现的列表对象。编码：REDIS_ENCODING_LINKEDLIST；对象：使用双端链表实现的列表对象。)
#define REDIS_LIST 1
// 集合对象(编码：REDIS_ENCODING_INTSET；对象：使用整数集合实现的集合对象。编码：REDIS_ENCODING_HT；对象：使用字典实现的集合对象。)
#define REDIS_SET 2
/*
 * 有序集合对象(编码：REDIS_ENCODING_ZIPLIST；对象：使用压缩列表实现的有序集合对象。编码：REDIS_ENCODING_SKIPLIST；对象：使用跳跃表和字典实现的有序集合对象
 * 。)
 */
#define REDIS_ZSET 3
// 哈希对象(编码：REDIS_ENCODING_ZIPLIST；对象：使用压缩列表实现的哈希对象。编码：REDIS_ENCODING_HT；对象：使用字典实现的哈希对象。)
#define REDIS_HASH 4

/* Objects encoding. Some kind of objects like Strings and Hashes can be
 * internally represented in multiple ways. The 'encoding' field of the object
 * is set to one of this fields for this object. */
// 对象编码
// 简单动态字符串(object encoding命令输出："raw")
#define REDIS_ENCODING_RAW 0     /* Raw representation */
// long类型的整数(object encoding命令输出："int")
#define REDIS_ENCODING_INT 1     /* Encoded as integer */
// 字典(object encoding命令输出："hashtable")
#define REDIS_ENCODING_HT 2      /* Encoded as hash table */
#define REDIS_ENCODING_ZIPMAP 3  /* Encoded as zipmap */
// 双端链表(object encoding命令输出："linkedlist")
#define REDIS_ENCODING_LINKEDLIST 4 /* Encoded as regular linked list */
// 压缩列表(object encoding命令输出："ziplist")
#define REDIS_ENCODING_ZIPLIST 5 /* Encoded as ziplist */
// 整数集合(object encoding命令输出："intset")
#define REDIS_ENCODING_INTSET 6  /* Encoded as intset */
// 跳跃表和字典(object encoding命令输出："skiplist")
#define REDIS_ENCODING_SKIPLIST 7  /* Encoded as skiplist */
// embstr编码的简单动态字符串(object encoding命令输出："embstr")
#define REDIS_ENCODING_EMBSTR 8  /* Embedded sds string encoding */

/* Defines related to the dump file format. To store 32 bits lengths for short
 * keys requires a lot of space, so we check the most significant 2 bits of
 * the first byte to interpreter the length:
 *
 * 00|000000 => if the two MSB are 00 the len is the 6 bits of this byte
 * 01|000000 00000000 =>  01, the len is 14 byes, 6 bits + 8 bits of next byte
 * 10|000000 [32 bit integer] => if it's 10, a full 32 bit len will follow
 * 11|000000 this means: specially encoded object will follow. The six bits
 *           number specify the kind of object that follows.
 *           See the REDIS_RDB_ENC_* defines.
 *
 * Lengths up to 63 are stored using a single byte, most DB keys, and may
 * values, will fit inside. */
#define REDIS_RDB_6BITLEN 0
#define REDIS_RDB_14BITLEN 1
#define REDIS_RDB_32BITLEN 2
#define REDIS_RDB_ENCVAL 3
#define REDIS_RDB_LENERR UINT_MAX

/* When a length of a string object stored on disk has the first two bits
 * set, the remaining two bits specify a special encoding for the object
 * accordingly to the following defines: */
#define REDIS_RDB_ENC_INT8 0        /* 8 bit signed integer */
#define REDIS_RDB_ENC_INT16 1       /* 16 bit signed integer */
#define REDIS_RDB_ENC_INT32 2       /* 32 bit signed integer */
#define REDIS_RDB_ENC_LZF 3         /* string compressed with FASTLZ */

/* AOF states */
#define REDIS_AOF_OFF 0             /* AOF is off */
#define REDIS_AOF_ON 1              /* AOF is on */
#define REDIS_AOF_WAIT_REWRITE 2    /* AOF waits rewrite to start appending */

/* Client flags */
#define REDIS_SLAVE (1<<0)   /* This client is a slave server */
#define REDIS_MASTER (1<<1)  /* This client is a master server */
#define REDIS_MONITOR (1<<2) /* This client is a slave monitor, see MONITOR */
#define REDIS_MULTI (1<<3)   /* This client is in a MULTI context */
#define REDIS_BLOCKED (1<<4) /* The client is waiting in a blocking operation */
#define REDIS_DIRTY_CAS (1<<5) /* Watched keys modified. EXEC will fail. */
#define REDIS_CLOSE_AFTER_REPLY (1<<6) /* Close after writing entire reply. */
#define REDIS_UNBLOCKED (1<<7) /* This client was unblocked and is stored in
                                  server.unblocked_clients */
#define REDIS_LUA_CLIENT (1<<8) /* This is a non connected client used by Lua */
#define REDIS_ASKING (1<<9)     /* Client issued the ASKING command */
#define REDIS_CLOSE_ASAP (1<<10)/* Close this client ASAP */
#define REDIS_UNIX_SOCKET (1<<11) /* Client connected via Unix domain socket */
#define REDIS_DIRTY_EXEC (1<<12)  /* EXEC will fail for errors while queueing */
#define REDIS_MASTER_FORCE_REPLY (1<<13)  /* Queue replies even if is master */
#define REDIS_FORCE_AOF (1<<14)   /* Force AOF propagation of current cmd. */
#define REDIS_FORCE_REPL (1<<15)  /* Force replication of current cmd. */
#define REDIS_PRE_PSYNC (1<<16)   /* Instance don't understand PSYNC. */
#define REDIS_READONLY (1<<17)    /* Cluster client is in read-only state. */
#define REDIS_PUBSUB (1<<18)      /* Client is in Pub/Sub mode. */

/* Client block type (btype field in client structure)
 * if REDIS_BLOCKED flag is set. */
#define REDIS_BLOCKED_NONE 0    /* Not blocked, no REDIS_BLOCKED flag set. */
#define REDIS_BLOCKED_LIST 1    /* BLPOP & co. */
#define REDIS_BLOCKED_WAIT 2    /* WAIT for synchronous replication. */

/* Client request types */
#define REDIS_REQ_INLINE 1
#define REDIS_REQ_MULTIBULK 2

/* Client classes for client limits, currently used only for
 * the max-client-output-buffer limit implementation. */
#define REDIS_CLIENT_TYPE_NORMAL 0 /* Normal req-reply clients + MONITORs */
#define REDIS_CLIENT_TYPE_SLAVE 1  /* Slaves. */
#define REDIS_CLIENT_TYPE_PUBSUB 2 /* Clients subscribed to PubSub channels. */
#define REDIS_CLIENT_TYPE_COUNT 3

/* Slave replication state. Used in server.repl_state for slaves to remember
 * what to do next. */
#define REDIS_REPL_NONE 0 /* No active replication */
#define REDIS_REPL_CONNECT 1 /* Must connect to master */
#define REDIS_REPL_CONNECTING 2 /* Connecting to master */
/* --- Handshake states, must be ordered --- */
#define REDIS_REPL_RECEIVE_PONG 3 /* Wait for PING reply */
#define REDIS_REPL_SEND_AUTH 4 /* Send AUTH to master */
#define REDIS_REPL_RECEIVE_AUTH 5 /* Wait for AUTH reply */
#define REDIS_REPL_SEND_PORT 6 /* Send REPLCONF listening-port */
#define REDIS_REPL_RECEIVE_PORT 7 /* Wait for REPLCONF reply */
#define REDIS_REPL_SEND_CAPA 8 /* Send REPLCONF capa */
#define REDIS_REPL_RECEIVE_CAPA 9 /* Wait for REPLCONF reply */
#define REDIS_REPL_SEND_PSYNC 10 /* Send PSYNC */
#define REDIS_REPL_RECEIVE_PSYNC 11 /* Wait for PSYNC reply */
/* --- End of handshake states --- */
#define REDIS_REPL_TRANSFER 12 /* Receiving .rdb from master */
#define REDIS_REPL_CONNECTED 13 /* Connected to master */

/* State of slaves from the POV of the master. Used in client->replstate.
 * In SEND_BULK and ONLINE state the slave receives new updates
 * in its output queue. In the WAIT_BGSAVE state instead the server is waiting
 * to start the next background saving in order to send updates to it. */
#define REDIS_REPL_WAIT_BGSAVE_START 14 /* We need to produce a new RDB file. */
#define REDIS_REPL_WAIT_BGSAVE_END 15 /* Waiting RDB file creation to finish. */
#define REDIS_REPL_SEND_BULK 16 /* Sending RDB file to slave. */
#define REDIS_REPL_ONLINE 17 /* RDB file transmitted, sending just updates. */

/* Slave capabilities. */
#define SLAVE_CAPA_NONE 0
#define SLAVE_CAPA_EOF (1<<0)   /* Can parse the RDB EOF streaming format. */

/* Synchronous read timeout - slave side */
#define REDIS_REPL_SYNCIO_TIMEOUT 5

/* List related stuff */
#define REDIS_HEAD 0
#define REDIS_TAIL 1

/* Sort operations */
#define REDIS_SORT_GET 0
#define REDIS_SORT_ASC 1
#define REDIS_SORT_DESC 2
#define REDIS_SORTKEY_MAX 1024

/* Log levels */
#define REDIS_DEBUG 0
#define REDIS_VERBOSE 1
#define REDIS_NOTICE 2
#define REDIS_WARNING 3
#define REDIS_LOG_RAW (1<<10) /* Modifier to log without timestamp */
#define REDIS_DEFAULT_VERBOSITY REDIS_NOTICE

/* Anti-warning macro... */
#define REDIS_NOTUSED(V) ((void) V)

#define ZSKIPLIST_MAXLEVEL 32 /* Should be enough for 2^32 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */

/* Append only defines */
#define AOF_FSYNC_NO 0
#define AOF_FSYNC_ALWAYS 1
#define AOF_FSYNC_EVERYSEC 2
#define REDIS_DEFAULT_AOF_FSYNC AOF_FSYNC_EVERYSEC

/* Zip structure related defaults */
#define REDIS_HASH_MAX_ZIPLIST_ENTRIES 512
#define REDIS_HASH_MAX_ZIPLIST_VALUE 64
#define REDIS_LIST_MAX_ZIPLIST_ENTRIES 512
#define REDIS_LIST_MAX_ZIPLIST_VALUE 64
#define REDIS_SET_MAX_INTSET_ENTRIES 512
#define REDIS_ZSET_MAX_ZIPLIST_ENTRIES 128
#define REDIS_ZSET_MAX_ZIPLIST_VALUE 64

/* HyperLogLog defines */
#define REDIS_DEFAULT_HLL_SPARSE_MAX_BYTES 3000

/* Sets operations codes */
#define REDIS_OP_UNION 0
#define REDIS_OP_DIFF 1
#define REDIS_OP_INTER 2

/* Redis maxmemory strategies */
// Redis六种最大内存策略(默认不进行置换)
// 只从设置失效(expire set)的key中选择最近最不经常使用的key进行删除，用以保存新数据
#define REDIS_MAXMEMORY_VOLATILE_LRU 0
// 只从设置失效(expire set)的key中，选出存活时间(TTL)最短的key进行删除，用以保存新数据
#define REDIS_MAXMEMORY_VOLATILE_TTL 1
// 随机从all-keys中选择一些key进行删除，用以保存新数据
#define REDIS_MAXMEMORY_VOLATILE_RANDOM 2
// 优先删除掉最近最不经常使用的key，用以保存新数据
#define REDIS_MAXMEMORY_ALLKEYS_LRU 3
// 只从设置失效(expire set)的key中，选择一些key进行删除，用以保存新数据
#define REDIS_MAXMEMORY_ALLKEYS_RANDOM 4
// 不进行置换，表示即使内存达到上限也不进行置换，所有能引起内存增加的命令都会返回error
#define REDIS_MAXMEMORY_NO_EVICTION 5
#define REDIS_DEFAULT_MAXMEMORY_POLICY REDIS_MAXMEMORY_NO_EVICTION

/* Scripting */
#define REDIS_LUA_TIME_LIMIT 5000 /* milliseconds */

/* Units */
#define UNIT_SECONDS 0
#define UNIT_MILLISECONDS 1

/* SHUTDOWN flags */
#define REDIS_SHUTDOWN_SAVE 1       /* Force SAVE on SHUTDOWN even if no save
                                       points are configured. */
#define REDIS_SHUTDOWN_NOSAVE 2     /* Don't SAVE on SHUTDOWN. */

/* Command call flags, see call() function */
#define REDIS_CALL_NONE 0
#define REDIS_CALL_SLOWLOG 1
#define REDIS_CALL_STATS 2
#define REDIS_CALL_PROPAGATE 4
#define REDIS_CALL_FULL (REDIS_CALL_SLOWLOG | REDIS_CALL_STATS | REDIS_CALL_PROPAGATE)

/* Command propagation flags, see propagate() function */
#define REDIS_PROPAGATE_NONE 0
#define REDIS_PROPAGATE_AOF 1
#define REDIS_PROPAGATE_REPL 2

/* RDB active child save type. */
#define REDIS_RDB_CHILD_TYPE_NONE 0
#define REDIS_RDB_CHILD_TYPE_DISK 1     /* RDB is written to disk. */
#define REDIS_RDB_CHILD_TYPE_SOCKET 2   /* RDB is written to slave socket. */

/* Keyspace changes notification classes. Every class is associated with a
 * character for configuration purposes. */
#define REDIS_NOTIFY_KEYSPACE (1<<0)    /* K */
#define REDIS_NOTIFY_KEYEVENT (1<<1)    /* E */
#define REDIS_NOTIFY_GENERIC (1<<2)     /* g */
#define REDIS_NOTIFY_STRING (1<<3)      /* $ */
#define REDIS_NOTIFY_LIST (1<<4)        /* l */
#define REDIS_NOTIFY_SET (1<<5)         /* s */
#define REDIS_NOTIFY_HASH (1<<6)        /* h */
#define REDIS_NOTIFY_ZSET (1<<7)        /* z */
#define REDIS_NOTIFY_EXPIRED (1<<8)     /* x */
#define REDIS_NOTIFY_EVICTED (1<<9)     /* e */
#define REDIS_NOTIFY_ALL (REDIS_NOTIFY_GENERIC | REDIS_NOTIFY_STRING | REDIS_NOTIFY_LIST | REDIS_NOTIFY_SET | REDIS_NOTIFY_HASH | REDIS_NOTIFY_ZSET | REDIS_NOTIFY_EXPIRED | REDIS_NOTIFY_EVICTED)      /* A */

/* Get the first bind addr or NULL */
#define REDIS_BIND_ADDR (server.bindaddr_count ? server.bindaddr[0] : NULL)

/* Using the following macro you can run code inside serverCron() with the
 * specified period, specified in milliseconds.
 * The actual resolution depends on server.hz. */
#define run_with_period(_ms_) if ((_ms_ <= 1000/server.hz) || !(server.cronloops%((_ms_)/(1000/server.hz))))

/* We can print the stacktrace, so our assert is defined this way: */
#define redisAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (_redisAssertWithInfo(_c,_o,#_e,__FILE__,__LINE__),_exit(1)))
#define redisAssert(_e) ((_e)?(void)0 : (_redisAssert(#_e,__FILE__,__LINE__),_exit(1)))
#define redisPanic(_e) _redisPanic(#_e,__FILE__,__LINE__),_exit(1)

/*-----------------------------------------------------------------------------
 * Data types
 *----------------------------------------------------------------------------*/

/* A redis object, that is a type able to hold a string / list / set */

/* The actual Redis Object */
/*
 * Redis对象
 */
#define REDIS_LRU_BITS 24
#define REDIS_LRU_CLOCK_MAX ((1<<REDIS_LRU_BITS)-1) /* Max value of obj->lru */
#define REDIS_LRU_CLOCK_RESOLUTION 1000 /* LRU clock resolution in ms */
typedef struct redisObject {
	// 类型
    unsigned type:4;
    // 编码
    unsigned encoding:4;
    /*
     * 对象最后一次被访问的时间(对象的空转时长)
     * OBJECT IDLETIME key命令的实现是特殊的，这个命令在访问键的值对象时，不会修改值对象的lru属性。
     * 如果服务器打开了maxmemory选项，并且服务器用于回收内存的算法为volatile-lru或者allkeys-lru，那么当服务器占用的内存数超过了maxmemory选项所设置的上限值时
     * ，空转时长较高的那部分键会优先被服务器释放，从而回收内存。
     */
    unsigned lru:REDIS_LRU_BITS; /* lru time (relative to server.lruclock) */
    /*
     * 引用计数(内存回收和对象共享)
     * 1.通过引用计数技术实现内存回收机制，程序通过跟踪每个对象的引用计数信息，在适当的时候自动释放对象并进行内存回收。对象的引用计数信息会随着对象的使用状态
     * 而不断变化：(1).在创建一个新对象时，引用计数的值会被初始化为1；(2).当对象被一个新程序使用时，它的引用计数值会被增一；(3)当对象不再被一个程序使用时，它
     * 的引用计数值会被减一；(4).当对象的引用计数值变为0时，对象所占用的内存会被释放。
     * 2.通过引用计数计数实现对象共享作用。在Redis中，让多个键共享同一个值对象需要执行以下两个步骤：1).将数据库键的值指针指向一个现有的值对象；2).将被共享的值
     * 对象的引用计数增一。
     */
    int refcount;
    /*
     * 指向底层实现数据结构的指针(由对象的encoding属性决定，见REDIS_ENCODING_*)
     * 指向实际值的指针
     */
    void *ptr;
} robj;

/* Macro used to obtain the current LRU clock.
 * If the current resolution is lower than the frequency we refresh the
 * LRU clock (as it should be in production servers) we return the
 * precomputed value, otherwise we need to resort to a function call. */
#define LRU_CLOCK() ((1000/server.hz <= REDIS_LRU_CLOCK_RESOLUTION) ? server.lruclock : getLRUClock())

/* Macro used to initialize a Redis object allocated on the stack.
 * Note that this macro is taken near the structure definition to make sure
 * we'll update it when the structure is changed, to avoid bugs like
 * bug #85 introduced exactly in this way. */
#define initStaticStringObject(_var,_ptr) do { \
    _var.refcount = 1; \
    _var.type = REDIS_STRING; \
    _var.encoding = REDIS_ENCODING_RAW; \
    _var.ptr = _ptr; \
} while(0);

/* To improve the quality of the LRU approximation we take a set of keys
 * that are good candidate for eviction across freeMemoryIfNeeded() calls.
 *
 * Entries inside the eviciton pool are taken ordered by idle time, putting
 * greater idle times to the right (ascending order).
 *
 * Empty entries have the key pointer set to NULL. */
#define REDIS_EVICTION_POOL_SIZE 16
struct evictionPoolEntry {
    unsigned long long idle;    /* Object idle time. */
    sds key;                    /* Key name. */
};

/* Redis database representation. There are multiple databases identified
 * by integers from 0 (the default database) up to the max configured
 * database. The database number is the 'id' field in the structure. */
typedef struct redisDb {
	/*
	 * 数据库键空间(key space)，保存着数据库中的所有键值对
	 * 键空间和用户所见的数据库是直接对应的：(1).键空间的键也就是数据库的键，每个键都是一个字符串对象。(2).键空间的值也就是数据库的值，每个值可以是字符串对象
	 * 、列表对象、哈希表对象、集合对象和有序集合对象中的任意一种Redis对象。
	 * 所有针对数据库的操作，实际上都是通过对键空间字典进行操作来实现的。
	 */
    dict *dict;                 /* The keyspace for this DB */
    /*
     * 过期字典，保存着键的过期时间
     * 键的过期时间，字典的键为键，字典的值为过期时间UNIX时间戳
     */
    dict *expires;              /* Timeout of keys with a timeout set */
    // 正处于阻塞状态的键
    dict *blocking_keys;        /* Keys with clients waiting for data (BLPOP) */
    // 可以解除阻塞的键
    dict *ready_keys;           /* Blocked keys that received a PUSH */
    // 正在被WATCH命令监视的键
    dict *watched_keys;         /* WATCHED keys for MULTI/EXEC CAS */
    struct evictionPoolEntry *eviction_pool;    /* Eviction pool of keys */
    // 数据库号码
    int id;                     /* Database ID */
    // 数据库的键的平均TTL，统计信息
    long long avg_ttl;          /* Average TTL, just for stats */
} redisDb;

/* Client MULTI/EXEC state */
/*
 * 事务命令
 */
typedef struct multiCmd {
	// 参数
    robj **argv;
    // 参数数量
    int argc;
    // 命令指针
    struct redisCommand *cmd;
} multiCmd;

/*
 * 事务状态
 */
typedef struct multiState {
	// 事务队列，FIFO顺序
    multiCmd *commands;     /* Array of MULTI commands */
    // 已入队命令计数
    int count;              /* Total number of MULTI commands */
    int minreplicas;        /* MINREPLICAS for synchronous replication */
    time_t minreplicas_timeout; /* MINREPLICAS timeout as unixtime. */
} multiState;

/* This structure holds the blocking operation state for a client.
 * The fields used depend on client->btype. */
// 阻塞状态
typedef struct blockingState {
    /* Generic fields. */
	// 阻塞时限
    mstime_t timeout;       /* Blocking operation timeout. If UNIX current time
                             * is > timeout then the operation timed out. */

    /* REDIS_BLOCK_LIST */
    // 造成阻塞的键
    dict *keys;             /* The keys we are waiting to terminate a blocking
                             * operation such as BLPOP. Otherwise NULL. */
    // 在被阻塞的键有新元素进入时，需要将这些新元素添加到哪里的目标键
    // 用于BRPOPLPUSH命令
    robj *target;           /* The key that should receive the element,
                             * for BRPOPLPUSH. */

    /* REDIS_BLOCK_WAIT */
    // 等待ACK的复制节点的数量
    int numreplicas;        /* Number of replicas we are waiting for ACK. */
    // 复制偏移量
    long long reploffset;   /* Replication offset to reach. */
} blockingState;

/* The following structure represents a node in the server.ready_keys list,
 * where we accumulate all the keys that had clients blocked with a blocking
 * operation such as B[LR]POP, but received new data in the context of the
 * last executed command.
 *
 * After the execution of every command or script, we run this list to check
 * if as a result we should serve data to clients blocked, unblocking them.
 * Note that server.ready_keys will not have duplicates as there dictionary
 * also called ready_keys in every structure representing a Redis database,
 * where we make sure to remember if a given key was already added in the
 * server.ready_keys list. */
// 记录解除了客户端的阻塞状态的键，以及键所在的数据库。
typedef struct readyList {
    redisDb *db;
    robj *key;
} readyList;

/* With multiplexing we need to take per-client state.
 * Clients are taken in a linked list.
 * 因为I/O复用的缘故，需要为每个客户端维持一个状态。
 *
 * 多个客户端状态被服务器用链表连接起来。
 */
typedef struct redisClient {
    uint64_t id;            /* Client incremental unique ID. */
	// 套接字描述符
    int fd;
    /*
     * 记录客户端当前正在使用的数据库(记录了客户端当前的目标数据库，这个属性是指向redisDb结构的指针。)
     * [SELECT命令实现原理]通过修改redisClient.db指针，让它指向服务器中的不同数据库，从而实现切换目标数据库的功能。
     */
    redisDb *db;
    // 当前正在使用的数据库的id(号码)
    int dictid;
    // 客户端的名字
    robj *name;             /* As set by CLIENT SETNAME */
    // 查询缓冲区
    sds querybuf;
    // 查询缓冲区长度峰值
    size_t querybuf_peak;   /* Recent (100ms or more) peak of querybuf size */
    // 参数数量
    int argc;
    // 参数对象数组
    robj **argv;
    // 记录被客户端执行的命令
    struct redisCommand *cmd, *lastcmd;
    // 请求的类型：内联命令还是多条命令
    int reqtype;
    // 剩余未读取的命令内容数量
    int multibulklen;       /* number of multi bulk arguments left to read */
    // 命令内容的长度
    long bulklen;           /* length of bulk argument in multi bulk request */
    // 回复链表
    list *reply;
    // 回复链表中对象的总大小
    unsigned long reply_bytes; /* Tot bytes of objects in reply list */
    // 已发送字节，处理short write用
    int sentlen;            /* Amount of bytes already sent in the current
                               buffer or object being sent. */
    // 创建客户端的时间
    time_t ctime;           /* Client creation time */
    // 客户端最后一次和服务器互动的时间
    time_t lastinteraction; /* time of the last interaction, used for timeout */
    // 客户端的输出缓冲区超过软性限制的时间
    time_t obuf_soft_limit_reached_time;
    // 客户端状态标志
    int flags;              /* REDIS_SLAVE | REDIS_MONITOR | REDIS_MULTI ... */
    // 当server.requirepass不为NULL时
    // 代表认证的状态
    // 0代表未认证，1代表已认证
    int authenticated;      /* when requirepass is non-NULL */
    // 复制状态
    int replstate;          /* replication state if this is a slave */
    int repl_put_online_on_ack; /* Install slave write handler on ACK. */
    // 用于保存主服务器传来的RDB文件的文件描述符
    int repldbfd;           /* replication DB file descriptor */
    // 读取主服务器传来的RDB文件的偏移量
    off_t repldboff;        /* replication DB file offset */
    // 主服务器传来的RDB文件的大小
    off_t repldbsize;       /* replication DB file size */
    sds replpreamble;       /* replication DB preamble. */
    // 主服务器的复制偏移量
    long long reploff;      /* replication offset if this is our master */
    // 从服务器最后一次发送REPLCONF ACK时的偏移量
    long long repl_ack_off; /* replication ack offset, if this is a slave */
    // 从服务器最后一次发送REPLCONF ACK的时间
    long long repl_ack_time;/* replication ack time, if this is a slave */
    long long psync_initial_offset; /* FULLRESYNC reply offset other slaves
                                       copying this slave output buffer
                                       should use. */
    // 主服务器的master run ID
    // 保存在客户端，用于执行部分重同步
    char replrunid[REDIS_RUN_ID_SIZE+1]; /* master run id if this is a master */
    // 从服务器的监听端口号
    int slave_listening_port; /* As configured with: SLAVECONF listening-port */
    int slave_capa;         /* Slave capabilities: SLAVE_CAPA_* bitwise OR. */
    // 事务状态
    multiState mstate;      /* MULTI/EXEC state */
    // 阻塞类型
    int btype;              /* Type of blocking op if REDIS_BLOCKED. */
    // 阻塞状态
    blockingState bpop;     /* blocking state */
    // 最后被写入的全局复制偏移量
    long long woff;         /* Last write global replication offset. */
    // 被监视的键
    list *watched_keys;     /* Keys WATCHED for MULTI/EXEC CAS */
    // 这个字典记录了客户端所有订阅的频道
    // 键为频道名字，值为NULL
    // 也即是，一个频道的集合
    dict *pubsub_channels;  /* channels a client is interested in (SUBSCRIBE) */
    // 链表，包含多个pubsubPattern结构
    // 记录了所有订阅频道的客户端的信息
    // 新pubsubPattern结构总是被添加到表尾
    list *pubsub_patterns;  /* patterns a client is interested in (SUBSCRIBE) */
    sds peerid;             /* Cached peer ID. */

    /* Response buffer */
    // 回复偏移量
    int bufpos;
    // 回复缓冲区
    char buf[REDIS_REPLY_CHUNK_BYTES];
} redisClient;

// 服务器的保存条件(BGSAVE自动执行的条件)
struct saveparam {
	// 多少秒之内
    time_t seconds;
    // 发生多少次修改
    int changes;
};

// 通过复用来减少内存碎片，以及减少操作耗时的共享对象
struct sharedObjectsStruct {
    robj *crlf, *ok, *err, *emptybulk, *czero, *cone, *cnegone, *pong, *space,
    *colon, *nullbulk, *nullmultibulk, *queued,
    *emptymultibulk, *wrongtypeerr, *nokeyerr, *syntaxerr, *sameobjecterr,
    *outofrangeerr, *noscripterr, *loadingerr, *slowscripterr, *bgsaveerr,
    *masterdownerr, *roslaveerr, *execaborterr, *noautherr, *noreplicaserr,
    *busykeyerr, *oomerr, *plus, *messagebulk, *pmessagebulk, *subscribebulk,
    *unsubscribebulk, *psubscribebulk, *punsubscribebulk, *del, *rpop, *lpop,
    *lpush, *emptyscan, *minstring, *maxstring,
    *select[REDIS_SHARED_SELECT_CMDS],
    *integers[REDIS_SHARED_INTEGERS],
    *mbulkhdr[REDIS_SHARED_BULKHDR_LEN], /* "*<value>\r\n" */
    *bulkhdr[REDIS_SHARED_BULKHDR_LEN];  /* "$<value>\r\n" */
};

/* ZSETs use a specialized version of Skiplists */
/*
 * 跳跃表节点(在同一个跳跃表中，各个节点保存的成员对象必须是唯一的，但是多个节点保存的分值却可以是相同的；分值相同的节点将按照成员对象在字典序中的大小来进行排
 * 序，成员对象较小的节点会排在前面(靠近表头的方向)，而成员对象较大的节点则会排在后面(靠近表尾的方向)。)
 */
typedef struct zskiplistNode {
	// 成员对象(是一个指针，指向一个保存SDS值的字符串。)
    robj *obj;
    // 分值(是一个double类型的浮点数，节点按各自所保存的分值从小到大排列。)
    double score;
    // 后退指针(指向位于当前节点的前一个节点。后退指针在程序从表尾向表头遍历时使用。用于从表尾向表头方向方位节点：跟可以一次跳过多个节点的前进指针不同，因为每
    // 个节点只有一个后退指针，所以每次只能后退至前一个节点。)
    struct zskiplistNode *backward;
    // 层(当程序从表头向表尾进行遍历时，访问会沿着层的前进指针进行。该数组可以包含多个元素，每个元素都包含一个指向其他节点的指针，程序可以用过这些层来加快方位
    // 访问节点的速度，一般来说，层的数量越多，方位其他节点的速度就越快。)
    struct zskiplistLevel {
    	// 前进指针(用于访问位于表尾方向的其他节点。)
        struct zskiplistNode *forward;
        // 跨度(记录了前进指针所指向节点和当前节点的距离：两个节点之间的跨度越大，它们相距得就越远；指向NULL的所有前进指针的跨度都为0，因为它们没有连向任何节
        // 点。)
        unsigned int span;
    } level[];
} zskiplistNode;

/*
 * 跳跃表(用于保存跳跃表节点的相关信息)
 */
typedef struct zskiplist {
	// 表头节点和表尾节点(通过这两个指针，程序定位表头节点和表尾节点的复杂度为O(1)。)
    struct zskiplistNode *header, *tail;
    // 记录表中节点的数量(跳跃表的长度，表头节点不计算在内。程序可以在O(1)复杂度内返回跳跃表的长度。)
    unsigned long length;
    // 表中层数最大的节点的层数(用于在O(1)复杂度内获取跳跃表中层高最大的那个节点的层数量，表头节点的层数不计算在内。)
    int level;
} zskiplist;

/*
 * 有序集合
 */
typedef struct zset {
	// 字典，键为成员，值为分值
	// 用于支持O(1)复杂度的按成员取分值操作
    dict *dict;
    // 跳跃表，按分值排序成员
    // 用于支持平均复杂度为O(logN)的按分值定位成员操作
    // 以及范围操作
    zskiplist *zsl;
} zset;

// 客户端缓冲区限制
typedef struct clientBufferLimitsConfig {
	// 硬限制
    unsigned long long hard_limit_bytes;
    // 软限制
    unsigned long long soft_limit_bytes;
    // 软限制时限
    time_t soft_limit_seconds;
} clientBufferLimitsConfig;

// 限制可以有多个
extern clientBufferLimitsConfig clientBufferLimitsDefaults[REDIS_CLIENT_TYPE_COUNT];

/* The redisOp structure defines a Redis Operation, that is an instance of
 * a command with an argument vector, database ID, propagation target
 * (REDIS_PROPAGATE_*), and command pointer.
 *
 * redisOp结构定义了一个Redis操作，它包含指向被执行命令的指针、命令的参数、数据库ID、传播目标(REDIS_PROPAGATE_*)。
 *
 * Currently only used to additionally propagate more commands to AOF/Replication
 * after the propagation of the executed command.
 *
 * 目前只用于在传播被执行命令之后，传播附加的其他命令到AOF或Replication中。
 */
typedef struct redisOp {
	// 参数
    robj **argv;
    // 参数数量、数据库ID、传播目标
    int argc, dbid, target;
    // 被执行命令的指针
    struct redisCommand *cmd;
} redisOp;

/* Defines an array of Redis operations. There is an API to add to this
 * structure in a easy way.
 *
 * redisOpArrayInit();
 * redisOpArrayAppend();
 * redisOpArrayFree();
 */
typedef struct redisOpArray {
    redisOp *ops;
    int numops;
} redisOpArray;

/*-----------------------------------------------------------------------------
 * Global server state
 *----------------------------------------------------------------------------*/

struct clusterState;

/* AIX defines hz to __hz, we don't use this define and in order to allow
 * Redis build on AIX we need to undef it. */
#ifdef _AIX
#undef hz
#endif

struct redisServer {
    /* General */
    pid_t pid;                  /* Main process pid. */
    // 配置文件的绝对路径
    char *configfile;           /* Absolute config file path, or NULL */
    // serverCron()每秒调用的次数
    int hz;                     /* serverCron() calls frequency in hertz */
    // 数据库(一个数组，保存着服务器中的所有数据库。该数组中的每个都是redis.h/redisDb结构，每个redisDb结构代表一个数据库。)
    redisDb *db;
    // 命令表(收到rename配置选项的作用)
    dict *commands;             /* Command table */
    // 命令表(无rename配置选项的作用)
    dict *orig_commands;        /* Command table before command renaming. */
    // 事件状态
    aeEventLoop *el;
    // 最近一次使用时钟
    unsigned lruclock:REDIS_LRU_BITS; /* Clock for LRU eviction */
    // 关闭服务器的标识
    int shutdown_asap;          /* SHUTDOWN needed ASAP */
    // 在执行serverCron()时进行渐进式rehash
    int activerehashing;        /* Incremental rehash in serverCron() */
    // 是否设置了密码
    char *requirepass;          /* Pass for AUTH command, or NULL */
    // PID文件
    char *pidfile;              /* PID file path */
    // 架构类型
    int arch_bits;              /* 32 or 64 depending on sizeof(long) */
    // serverCron()函数的运行次数计数器
    int cronloops;              /* Number of times the cron function run */
    // 本服务器的RUN ID
    char runid[REDIS_RUN_ID_SIZE+1];  /* ID always different at every exec. */
    // 服务器是否运行在SENTINEL模式
    int sentinel_mode;          /* True if this instance is a Sentinel. */
    /* Networking */
    // TCP监听端口
    int port;                   /* TCP listening port */
    int tcp_backlog;            /* TCP listen() backlog */
    // 地址
    char *bindaddr[REDIS_BINDADDR_MAX]; /* Addresses we should bind to */
    // 地址数量
    int bindaddr_count;         /* Number of addresses in server.bindaddr[] */
    // UNIX套接字
    char *unixsocket;           /* UNIX socket path */
    mode_t unixsocketperm;      /* UNIX socket permission */
    // 描述符
    int ipfd[REDIS_BINDADDR_MAX]; /* TCP socket file descriptors */
    // 描述符数量
    int ipfd_count;             /* Used slots in ipfd[] */
    // UNIX套接字文件描述符
    int sofd;                   /* Unix socket file descriptor */
    int cfd[REDIS_BINDADDR_MAX];/* Cluster bus listening socket */
    int cfd_count;              /* Used slots in cfd[] */
    // 一个链表，保存了所有客户端状态结构
    list *clients;              /* List of active clients */
    // 链表，保存了所有待关闭的客户端
    list *clients_to_close;     /* Clients to close asynchronously */
    // 链表，保存了所有从服务器，以及所有监视器
    list *slaves, *monitors;    /* List of slaves and MONITORs */
    // 服务器的当前客户端，仅用于崩溃报告
    redisClient *current_client; /* Current client, only used on crash report */
    int clients_paused;         /* True if clients are currently paused */
    mstime_t clients_pause_end_time; /* Time when we undo clients_paused */
    // 网络错误
    char neterr[ANET_ERR_LEN];   /* Error buffer for anet.c */
    // MIGRATE缓存
    dict *migrate_cached_sockets;/* MIGRATE cached sockets */
    uint64_t next_client_id;    /* Next client unique ID. Incremental. */
    /* RDB / AOF loading information */
    // 这个值为真时，表示服务器正在进行载入
    int loading;                /* We are loading data from disk if true */
    // 正在载入的数据的大小
    off_t loading_total_bytes;
    // 已载入数据的大小
    off_t loading_loaded_bytes;
    // 开始进行载入的时间
    time_t loading_start_time;
    off_t loading_process_events_interval_bytes;
    /* Fast pointers to often looked up command */
    // 常用命令的快捷连接
    struct redisCommand *delCommand, *multiCommand, *lpushCommand, *lpopCommand,
                        *rpopCommand;
    /* Fields used only for stats */
    // 服务器启动时间
    time_t stat_starttime;          /* Server start time */
    // 已处理命令的数量
    long long stat_numcommands;     /* Number of processed commands */
    // 服务器接到的连接请求数量
    long long stat_numconnections;  /* Number of connections received */
    // 已过期的键数量
    long long stat_expiredkeys;     /* Number of expired keys */
    // 因为回收内存而被释放的过期键的数量
    long long stat_evictedkeys;     /* Number of evicted keys (maxmemory) */
    // 成功查找键的次数(hits命中次数)
    long long stat_keyspace_hits;   /* Number of successful lookups of keys */
    // 查找键失败的次数(miss不命中次数)
    long long stat_keyspace_misses; /* Number of failed lookups of keys */
    // 已使用内存峰值
    size_t stat_peak_memory;        /* Max used memory record */
    // 最后一次执行fork()时消耗的时间
    long long stat_fork_time;       /* Time needed to perform latest fork() */
    double stat_fork_rate;          /* Fork rate in GB/sec. */
    // 服务器因为客户端数量过多而拒绝客户端连接的次数
    long long stat_rejected_conn;   /* Clients rejected because of maxclients */
    // 执行full sync的次数
    long long stat_sync_full;       /* Number of full resyncs with slaves. */
    // PSYNC成功执行的次数
    long long stat_sync_partial_ok; /* Number of accepted PSYNC requests. */
    // PSYNC执行失败的次数
    long long stat_sync_partial_err;/* Number of unaccepted PSYNC requests. */
    // 保存了所有慢查询日志的链表
    list *slowlog;                  /* SLOWLOG list of commands */
    // 下一条慢查询日志的ID
    long long slowlog_entry_id;     /* SLOWLOG current entry ID */
    // 服务器配置slowlog_log_slower_than选项的值
    long long slowlog_log_slower_than; /* SLOWLOG time limit (to get logged) */
    // 服务器配置slowlog_max_len选项的值
    unsigned long slowlog_max_len;     /* SLOWLOG max number of items logged */
    size_t resident_set_size;       /* RSS sampled in serverCron(). */
    long long stat_net_input_bytes; /* Bytes read from network. */
    long long stat_net_output_bytes; /* Bytes written to network. */
    /* The following two are used to track instantaneous metrics, like
     * number of operations per second, network traffic. */
    // 最后一次抽样
    struct {
    	// 最后一次进行抽样的时间
        long long last_sample_time; /* Timestamp of last sample in ms */
        // 最后一次抽样时，服务器已执行命令的数量
        long long last_sample_count;/* Count in last sample */
        // 抽样结果
        long long samples[REDIS_METRIC_SAMPLES];
        // 数组索引，用于保存抽样结果，并在需要时回绕到0
        int idx;
    } inst_metric[REDIS_METRIC_COUNT];
    /* Configuration */
    // 日志可见性
    int verbosity;                  /* Loglevel in redis.conf */
    // 客户端最大空转时间
    int maxidletime;                /* Client timeout in seconds */
    // 是否开启SO_KEEPALIVE选项
    int tcpkeepalive;               /* Set SO_KEEPALIVE if non-zero. */
    int active_expire_enabled;      /* Can be disabled for testing purposes. */
    size_t client_max_querybuf_len; /* Limit for client query buffer length */
    /*
     * 服务器的数据库数量(在初始化服务器时，程序会根据服务器状态的dbnum属性来决定应该创建多少个数据库。该属性的值由服务器的database选项决定，默认情况下，该选
     * 项的值为16，所以Redis服务器默认会出创建16个数据库。)
     */
    int dbnum;                      /* Total number of configured DBs */
    int daemonize;                  /* True if running as a daemon */
    // 客户端输出缓冲区大小限制
    // 数组的元素有REDIS_CLIENT_LIMIT_NUM_CLASSES个
    // 每个代表一类客户端：普通、从服务器、pubsub，诸如此类
    clientBufferLimitsConfig client_obuf_limits[REDIS_CLIENT_TYPE_COUNT];
    /* AOF persistence */
    // AOF状态(开启/关闭/可写)
    int aof_state;                  /* REDIS_AOF_(ON|OFF|WAIT_REWRITE) */
    // 所使用的fsync策略(每个写入/每秒/从不)
    int aof_fsync;                  /* Kind of fsync() policy */
    char *aof_filename;             /* Name of the AOF file */
    int aof_no_fsync_on_rewrite;    /* Don't fsync if a rewrite is in prog. */
    int aof_rewrite_perc;           /* Rewrite AOF if % growth is > M and... */
    off_t aof_rewrite_min_size;     /* the AOF file is at least N bytes. */
    // 最后一次执行BGREWRITEAOF时，AOF文件的大小
    off_t aof_rewrite_base_size;    /* AOF size on latest startup or rewrite. */
    // AOF文件的当前字节大小
    off_t aof_current_size;         /* AOF current size. */
    int aof_rewrite_scheduled;      /* Rewrite once BGSAVE terminates. */
    // 负责进行AOF重写的子进程ID
    pid_t aof_child_pid;            /* PID if rewriting process */
    // AOF重写缓存链表，链接着多个缓存块
    list *aof_rewrite_buf_blocks;   /* Hold changes during an AOF rewrite. */
    // AOF缓冲区
    sds aof_buf;      /* AOF buffer, written before entering the event loop */
    // AOF文件的描述符
    int aof_fd;       /* File descriptor of currently selected AOF file */
    // AOF的当前目标数据库
    int aof_selected_db; /* Currently selected DB in AOF */
    // 推迟write操作的时间
    time_t aof_flush_postponed_start; /* UNIX time of postponed AOF flush */
    // 最后一直执行fsync的时间
    time_t aof_last_fsync;            /* UNIX time of last fsync() */
    time_t aof_rewrite_time_last;   /* Time used by last AOF rewrite run. */
    // AOF重写的开始时间
    time_t aof_rewrite_time_start;  /* Current AOF rewrite start time. */
    // 最后一次执行BGREWRITEAOF的结果
    int aof_lastbgrewrite_status;   /* REDIS_OK or REDIS_ERR */
    // 记录AOF的write操作被推迟了多少次
    unsigned long aof_delayed_fsync;  /* delayed AOF fsync() counter */
    // 指示是否需要每写入一定量的数据，就主动执行一次fsync()
    int aof_rewrite_incremental_fsync;/* fsync incrementally while rewriting? */
    int aof_last_write_status;      /* REDIS_OK or REDIS_ERR */
    int aof_last_write_errno;       /* Valid if aof_last_write_status is ERR */
    int aof_load_truncated;         /* Don't stop on unexpected AOF EOF. */
    /* AOF pipes used to communicate between parent and child during rewrite. */
    int aof_pipe_write_data_to_child;
    int aof_pipe_read_data_from_parent;
    int aof_pipe_write_ack_to_parent;
    int aof_pipe_read_ack_from_child;
    int aof_pipe_write_ack_to_child;
    int aof_pipe_read_ack_from_parent;
    int aof_stop_sending_diff;     /* If true stop sending accumulated diffs
                                      to child process. */
    sds aof_child_diff;             /* AOF diff accumulator child side. */
    /* RDB persistence */
    // 自从上次SAVE执行以来，数据库被修改的次数
    long long dirty;                /* Changes to DB from the last save */
    // BGSAVE执行前的数据库被修改次数
    long long dirty_before_bgsave;  /* Used to restore dirty on failed BGSAVE */
    // 负责执行BGSAVE的子进程的ID
    // 没在执行BGSAVE时，设为-1
    pid_t rdb_child_pid;            /* PID of RDB saving child */
    struct saveparam *saveparams;   /* Save points array for RDB */
    int saveparamslen;              /* Number of saving points */
    char *rdb_filename;             /* Name of RDB file */
    int rdb_compression;            /* Use compression in RDB? */
    int rdb_checksum;               /* Use RDB checksum? */
    // 最后一次完成SAVE的时间
    time_t lastsave;                /* Unix time of last successful save */
    // 最后一次尝试执行BGSAVE的时间
    time_t lastbgsave_try;          /* Unix time of last attempted bgsave */
    // 最近一次BGSAVE执行耗费的时间
    time_t rdb_save_time_last;      /* Time used by last RDB save run. */
    // 数据库最近一次开始执行BGSAVE的时间
    time_t rdb_save_time_start;     /* Current RDB save start time. */
    int rdb_child_type;             /* Type of save by active child. */
    // 最后一次执行SAVE的状态
    int lastbgsave_status;          /* REDIS_OK or REDIS_ERR */
    int stop_writes_on_bgsave_err;  /* Don't allow writes if can't BGSAVE */
    int rdb_pipe_write_result_to_parent; /* RDB pipes used to return the state */
    int rdb_pipe_read_result_from_child; /* of each slave in diskless SYNC. */
    /* Propagation of commands in AOF / replication */
    redisOpArray also_propagate;    /* Additional command to propagate. */
    /* Logging */
    char *logfile;                  /* Path of log file */
    int syslog_enabled;             /* Is syslog enabled? */
    char *syslog_ident;             /* Syslog ident */
    int syslog_facility;            /* Syslog facility */
    /* Replication (master) */
    int slaveseldb;                 /* Last SELECTed DB in replication output */
    // 全局复制偏移量(一个累计值)
    long long master_repl_offset;   /* Global replication offset */
    // 主服务器发送PING的频率
    int repl_ping_slave_period;     /* Master pings the slave every N seconds */
    // backlog本身
    char *repl_backlog;             /* Replication backlog for partial syncs */
    // backlog的长度
    long long repl_backlog_size;    /* Backlog circular buffer size */
    // backlog中数据的长度
    long long repl_backlog_histlen; /* Backlog actual data length */
    // backlog的当前索引
    long long repl_backlog_idx;     /* Backlog circular buffer current offset */
    // backlog中可以被还原的第一个字节的偏移量
    long long repl_backlog_off;     /* Replication offset of first byte in the
                                       backlog buffer. */
    // backlog的过期时间
    time_t repl_backlog_time_limit; /* Time without slaves after the backlog
                                       gets released. */
    // 距离上一次有从服务器的时间
    time_t repl_no_slaves_since;    /* We have no slaves since that time.
                                       Only valid if server.slaves len is 0. */
    // 是否开启最小数量从服务器写入功能
    int repl_min_slaves_to_write;   /* Min number of slaves to write. */
    // 定义最小数量从服务器的最大延迟值
    int repl_min_slaves_max_lag;    /* Max lag of <count> slaves to write. */
    // 延迟良好的从服务器的数量
    int repl_good_slaves_count;     /* Number of slaves with lag <= max_lag. */
    int repl_diskless_sync;         /* Send RDB to slaves sockets directly. */
    int repl_diskless_sync_delay;   /* Delay to start a diskless repl BGSAVE. */
    /* Replication (slave) */
    // 主服务器的验证密码
    char *masterauth;               /* AUTH with this password with master */
    // 主服务器的地址
    char *masterhost;               /* Hostname of master */
    // 主服务器的端口
    int masterport;                 /* Port of master */
    // 超时时间
    int repl_timeout;               /* Timeout after N seconds of master idle */
    // 主服务器所对应的客户端
    redisClient *master;     /* Client that is master for this slave */
    // 被缓存的主服务器，PSYNC时使用
    redisClient *cached_master; /* Cached master to be reused for PSYNC. */
    int repl_syncio_timeout; /* Timeout for synchronous I/O calls */
    // 复制的状态(服务器是从服务器时使用)
    int repl_state;          /* Replication status if the instance is a slave */
    // RDB文件的大小
    off_t repl_transfer_size; /* Size of RDB to read from master during sync. */
    // 已读RDB文件内容的字节数
    off_t repl_transfer_read; /* Amount of RDB read from master during sync. */
    // 最近一次执行fsync时的偏移量
    // 用于sync_file_range函数
    off_t repl_transfer_last_fsync_off; /* Offset when we fsync-ed last time. */
    // 主服务器的套接字
    int repl_transfer_s;     /* Slave -> Master SYNC socket */
    // 保存RDB文件的临时文件的描述符
    int repl_transfer_fd;    /* Slave -> Master SYNC temp file descriptor */
    // 保存RDB文件的临时文件名字
    char *repl_transfer_tmpfile; /* Slave-> master SYNC temp file name */
    // 最近一次读入RDB内容的时间
    time_t repl_transfer_lastio; /* Unix time of the latest read, for timeout */
    int repl_serve_stale_data; /* Serve stale data when link is down? */
    // 是否只读从服务器？
    int repl_slave_ro;          /* Slave is read only? */
    // 连接断开的时长
    time_t repl_down_since; /* Unix time at which link with master went down */
    // 是否要在SYNC之后关闭NODELAY？
    int repl_disable_tcp_nodelay;   /* Disable TCP_NODELAY after SYNC? */
    // 从服务器优先级
    int slave_priority;             /* Reported in INFO and used by Sentinel. */
    // 本服务器(从服务器)当前主服务器的RUN ID
    char repl_master_runid[REDIS_RUN_ID_SIZE+1];  /* Master run id for PSYNC. */
    // 初始化偏移量
    long long repl_master_initial_offset;         /* Master PSYNC offset. */
    /* Replication script cache. */
    // 复制脚本缓存
    // 字典
    dict *repl_scriptcache_dict;        /* SHA1 all slaves are aware of. */
    // FIFO队列
    list *repl_scriptcache_fifo;        /* First in, first out LRU eviction. */
    // 缓存的大小
    unsigned int repl_scriptcache_size; /* Max number of elements. */
    /* Synchronous replication. */
    list *clients_waiting_acks;         /* Clients waiting in WAIT command. */
    int get_ack_from_slaves;            /* If true we send REPLCONF GETACK. */
    /* Limits */
    unsigned int maxclients;            /* Max number of simultaneous clients */
    unsigned long long maxmemory;   /* Max number of memory bytes to use */
    int maxmemory_policy;           /* Policy for key eviction */
    int maxmemory_samples;          /* Pricision of random sampling */
    /* Blocked clients */
    unsigned int bpop_blocked_clients; /* Number of clients blocked by lists */
    list *unblocked_clients; /* list of clients to unblock before next loop */
    list *ready_keys;        /* List of readyList structures for BLPOP & co */
    /* Sort parameters - qsort_r() is only available under BSD so we
     * have to take this state global, in order to pass it to sortCompare() */
    int sort_desc;
    int sort_alpha;
    int sort_bypattern;
    int sort_store;
    /* Zip structure config, see redis.conf for more information  */
    size_t hash_max_ziplist_entries;
    size_t hash_max_ziplist_value;
    size_t list_max_ziplist_entries;
    size_t list_max_ziplist_value;
    size_t set_max_intset_entries;
    size_t zset_max_ziplist_entries;
    size_t zset_max_ziplist_value;
    size_t hll_sparse_max_bytes;
    time_t unixtime;        /* Unix time sampled every cron cycle. */
    long long mstime;       /* Like 'unixtime' but with milliseconds resolution. */
    /* Pubsub */
    // 字典，键为频道，值为链表
    // 链表中保存了所有订阅某个频道的客户端
    // 新客户端总是被添加到链表的表尾
    dict *pubsub_channels;  /* Map channels to list of subscribed clients */
    // 这个链表记录了客户端订阅的所有模式的名字
    list *pubsub_patterns;  /* A list of pubsub_patterns */
    int notify_keyspace_events; /* Events to propagate via Pub/Sub. This is an
                                   xor of REDIS_NOTIFY... flags. */
    /* Cluster */
    int cluster_enabled;      /* Is cluster enabled? */
    mstime_t cluster_node_timeout; /* Cluster node timeout. */
    char *cluster_configfile; /* Cluster auto-generated config file name. */
    struct clusterState *cluster;  /* State of the cluster */
    int cluster_migration_barrier; /* Cluster replicas migration barrier. */
    int cluster_slave_validity_factor; /* Slave max data age for failover. */
    int cluster_require_full_coverage; /* If true, put the cluster down if
                                          there is at least an uncovered slot. */
    /* Scripting */
    // Lua环境
    lua_State *lua; /* The Lua interpreter. We use just one for all clients */
    // 复制执行Lua脚本中的Redis命令的伪客户端
    redisClient *lua_client;   /* The "fake client" to query Redis from Lua */
    // 当前正在执行EVAL命令的客户端，如果没有就是NULL
    redisClient *lua_caller;   /* The client running EVAL right now, or NULL */
    // 一个字典，值为Lua脚本，键为脚本的SHA1校验和
    dict *lua_scripts;         /* A dictionary of SHA1 -> Lua scripts */
    // Lua脚本的执行权限
    mstime_t lua_time_limit;  /* Script timeout in milliseconds */
    // 脚本开始执行的时间
    mstime_t lua_time_start;  /* Start time of script, milliseconds time */
    // 脚本是否执行过写命令
    int lua_write_dirty;  /* True if a write command was called during the
                             execution of the current script. */
    // 脚本是否执行过带有随机性质的命令
    int lua_random_dirty; /* True if a random command was called during the
                             execution of the current script. */
    // 脚本是否超时
    int lua_timedout;     /* True if we reached the time limit for script
                             execution. */
    // 是否杀死脚本
    int lua_kill;         /* Kill the script if true. */
    /* Latency monitor */
    long long latency_monitor_threshold;
    dict *latency_events;
    /* Assert & bug reporting */
    char *assert_failed;
    char *assert_file;
    int assert_line;
    int bug_report_start; /* True if bug report header was already logged. */
    int watchdog_period;  /* Software watchdog period in ms. 0 = off */
};

/*
 * 记录订阅模式的结构
 */
typedef struct pubsubPattern {
	// 订阅模式的客户端
    redisClient *client;
    // 被订阅的模式
    robj *pattern;
} pubsubPattern;

typedef void redisCommandProc(redisClient *c);
typedef int *redisGetKeysProc(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);
/*
 * Redis命令
 */
struct redisCommand {
	// 命令名字
    char *name;
    // 实现函数
    redisCommandProc *proc;
    // 参数个数
    int arity;
    // 字符串表示的FLAG
    char *sflags; /* Flags as string representation, one char per flag. */
    // 实际FLAG
    int flags;    /* The actual flags, obtained from the 'sflags' field. */
    /* Use a function to determine keys arguments in a command line.
     * Used for Redis Cluster redirect. */
    // 从命令中判断命令的键参数。在Redis集群转向时使用。
    redisGetKeysProc *getkeys_proc;
    /* What keys should be loaded in background when calling this command? */
    // 指定哪些参数是key
    int firstkey; /* The first argument that's a key (0 = no keys) */
    int lastkey;  /* The last argument that's a key */
    int keystep;  /* The step between first and last key */
    // 统计信息
    // microseconds记录了命令执行耗费的总毫微秒数
    // calls是命令被执行的总次数
    long long microseconds, calls;
};

struct redisFunctionSym {
    char *name;
    unsigned long pointer;
};

// 用于保存被排序值及其权重的结构
typedef struct _redisSortObject {
	// 被排序键的值
    robj *obj;
    // 权重
    union {
    	// 排序数字值时使用
        double score;
        // 排序字符串时使用
        robj *cmpobj;
    } u;
} redisSortObject;

// 排序操作
typedef struct _redisSortOperation {
	// 操作的类型，可以是GET、DEL、INCR或者DECR
	// 目前只实现了GET
    int type;
    // 用户给定的模式
    robj *pattern;
} redisSortOperation;

/* Structure to hold list iteration abstraction. */
/*
 * 列表迭代器对象
 */
typedef struct {
	// 列表对象
    robj *subject;
    // 对象所使用的编码
    unsigned char encoding;
    // 迭代的方向
    unsigned char direction; /* Iteration direction */
    // ziplist索引，迭代ziplist编码的列表时使用
    unsigned char *zi;
    // 链表节点的指针，迭代双端链表编码的列表时使用
    listNode *ln;
} listTypeIterator;

/* Structure for an entry while iterating over a list.
 *
 * 迭代列表时使用的记录结构，用于保存迭代器，以及迭代器返回的列表节点。
 */
typedef struct {
	// 列表迭代器
    listTypeIterator *li;
    // ziplist节点索引
    unsigned char *zi;  /* Entry in ziplist */
    // 双端链表节点指针
    listNode *ln;       /* Entry in linked list */
} listTypeEntry;

/* Structure to hold set iteration abstraction. */
/*
 * 多态集合迭代器
 */
typedef struct {
	// 被迭代的对象
    robj *subject;
    // 对象的编码
    int encoding;
    // 索引值，编码为insert时使用
    int ii; /* intset iterator */
    // 字典迭代器，编码为HT时使用
    dictIterator *di;
} setTypeIterator;

/* Structure to hold hash iteration abstraction. Note that iteration over
 * hashes involves both fields and values. Because it is possible that
 * not both are required, store pointers in the iterator to avoid
 * unnecessary memory allocation for fields/values. */
/*
 * 哈希对象的迭代器
 */
typedef struct {
	// 被迭代的哈希对象
    robj *subject;
    // 哈希对象的编码
    int encoding;

    // 域指针和值指针
    // 在迭代ZIPLIST编码的哈希对象时使用
    unsigned char *fptr, *vptr;

    // 字典迭代器和指向当前迭代字典节点的指针
    // 在迭代HT编码的哈希对象时使用
    dictIterator *di;
    dictEntry *de;
} hashTypeIterator;

#define REDIS_HASH_KEY 1
#define REDIS_HASH_VALUE 2

/*-----------------------------------------------------------------------------
 * Extern declarations
 *----------------------------------------------------------------------------*/

extern struct redisServer server;
extern struct sharedObjectsStruct shared;
extern dictType setDictType;
extern dictType zsetDictType;
extern dictType clusterNodesDictType;
extern dictType clusterNodesBlackListDictType;
extern dictType dbDictType;
extern dictType shaScriptObjectDictType;
extern double R_Zero, R_PosInf, R_NegInf, R_Nan;
extern dictType hashDictType;
extern dictType replScriptCacheDictType;

/*-----------------------------------------------------------------------------
 * Functions prototypes
 *----------------------------------------------------------------------------*/

/* Utils */
long long ustime(void);
long long mstime(void);
void getRandomHexChars(char *p, unsigned int len);
uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l);
void exitFromChild(int retcode);
size_t redisPopcount(void *s, long count);
void redisSetProcTitle(char *title);

/* networking.c -- Networking and Client related operations */
redisClient *createClient(int fd);
void closeTimedoutClients(void);
void freeClient(redisClient *c);
void freeClientAsync(redisClient *c);
void resetClient(redisClient *c);
void sendReplyToClient(aeEventLoop *el, int fd, void *privdata, int mask);
void *addDeferredMultiBulkLength(redisClient *c);
void setDeferredMultiBulkLength(redisClient *c, void *node, long length);
void processInputBuffer(redisClient *c);
void acceptHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void acceptUnixHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask);
void addReplyBulk(redisClient *c, robj *obj);
void addReplyBulkCString(redisClient *c, char *s);
void addReplyBulkCBuffer(redisClient *c, void *p, size_t len);
void addReplyBulkLongLong(redisClient *c, long long ll);
void addReply(redisClient *c, robj *obj);
void addReplySds(redisClient *c, sds s);
void addReplyError(redisClient *c, char *err);
void addReplyStatus(redisClient *c, char *status);
void addReplyDouble(redisClient *c, double d);
void addReplyLongLong(redisClient *c, long long ll);
void addReplyMultiBulkLen(redisClient *c, long length);
void copyClientOutputBuffer(redisClient *dst, redisClient *src);
void *dupClientReplyValue(void *o);
void getClientsMaxBuffers(unsigned long *longest_output_list,
                          unsigned long *biggest_input_buffer);
void formatPeerId(char *peerid, size_t peerid_len, char *ip, int port);
char *getClientPeerId(redisClient *client);
sds catClientInfoString(sds s, redisClient *client);
sds getAllClientsInfoString(void);
void rewriteClientCommandVector(redisClient *c, int argc, ...);
void rewriteClientCommandArgument(redisClient *c, int i, robj *newval);
unsigned long getClientOutputBufferMemoryUsage(redisClient *c);
void freeClientsInAsyncFreeQueue(void);
void asyncCloseClientOnOutputBufferLimitReached(redisClient *c);
int getClientType(redisClient *c);
int getClientTypeByName(char *name);
char *getClientTypeName(int class);
void flushSlavesOutputBuffers(void);
void disconnectSlaves(void);
int listenToPort(int port, int *fds, int *count);
void pauseClients(mstime_t duration);
int clientsArePaused(void);
int processEventsWhileBlocked(void);

#ifdef __GNUC__
void addReplyErrorFormat(redisClient *c, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
void addReplyStatusFormat(redisClient *c, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
void addReplyErrorFormat(redisClient *c, const char *fmt, ...);
void addReplyStatusFormat(redisClient *c, const char *fmt, ...);
#endif

/* List data type */
void listTypeTryConversion(robj *subject, robj *value);
void listTypePush(robj *subject, robj *value, int where);
robj *listTypePop(robj *subject, int where);
unsigned long listTypeLength(robj *subject);
listTypeIterator *listTypeInitIterator(robj *subject, long index, unsigned char direction);
void listTypeReleaseIterator(listTypeIterator *li);
int listTypeNext(listTypeIterator *li, listTypeEntry *entry);
robj *listTypeGet(listTypeEntry *entry);
void listTypeInsert(listTypeEntry *entry, robj *value, int where);
int listTypeEqual(listTypeEntry *entry, robj *o);
void listTypeDelete(listTypeEntry *entry);
void listTypeConvert(robj *subject, int enc);
void unblockClientWaitingData(redisClient *c);
void handleClientsBlockedOnLists(void);
void popGenericCommand(redisClient *c, int where);
void signalListAsReady(redisDb *db, robj *key);

/* MULTI/EXEC/WATCH... */
void unwatchAllKeys(redisClient *c);
void initClientMultiState(redisClient *c);
void freeClientMultiState(redisClient *c);
void queueMultiCommand(redisClient *c);
void touchWatchedKey(redisDb *db, robj *key);
void touchWatchedKeysOnFlush(int dbid);
void discardTransaction(redisClient *c);
void flagTransaction(redisClient *c);

/* Redis object implementation */
/*
 * 作用：将对象的引用计数值减一，将对象的引用计数值等于0时，释放对象
 */
void decrRefCount(robj *o);
void decrRefCountVoid(void *o);
/*
 * 作用：将对象的引用计数值增一
 */
void incrRefCount(robj *o);
/*
 * 将对象的引用计数值设置为0，但并不释放对象，这个函数通常在需要重新设置对象的引用计数器时使用
 */
robj *resetRefCount(robj *obj);
void freeStringObject(robj *o);
void freeListObject(robj *o);
void freeSetObject(robj *o);
void freeZsetObject(robj *o);
void freeHashObject(robj *o);
robj *createObject(int type, void *ptr);
robj *createStringObject(char *ptr, size_t len);
robj *createRawStringObject(char *ptr, size_t len);
robj *createEmbeddedStringObject(char *ptr, size_t len);
robj *dupStringObject(robj *o);
int isObjectRepresentableAsLongLong(robj *o, long long *llongval);
robj *tryObjectEncoding(robj *o);
robj *getDecodedObject(robj *o);
size_t stringObjectLen(robj *o);
robj *createStringObjectFromLongLong(long long value);
robj *createStringObjectFromLongDouble(long double value, int humanfriendly);
robj *createListObject(void);
robj *createZiplistObject(void);
robj *createSetObject(void);
robj *createIntsetObject(void);
robj *createHashObject(void);
robj *createZsetObject(void);
robj *createZsetZiplistObject(void);
int getLongFromObjectOrReply(redisClient *c, robj *o, long *target, const char *msg);
int checkType(redisClient *c, robj *o, int type);
int getLongLongFromObjectOrReply(redisClient *c, robj *o, long long *target, const char *msg);
int getDoubleFromObjectOrReply(redisClient *c, robj *o, double *target, const char *msg);
int getLongLongFromObject(robj *o, long long *target);
int getLongDoubleFromObject(robj *o, long double *target);
int getLongDoubleFromObjectOrReply(redisClient *c, robj *o, long double *target, const char *msg);
char *strEncoding(int encoding);
int compareStringObjects(robj *a, robj *b);
int collateStringObjects(robj *a, robj *b);
int equalStringObjects(robj *a, robj *b);
unsigned long long estimateObjectIdleTime(robj *o);
#define sdsEncodedObject(objptr) (objptr->encoding == REDIS_ENCODING_RAW || objptr->encoding == REDIS_ENCODING_EMBSTR)

/* Synchronous I/O with timeout */
ssize_t syncWrite(int fd, char *ptr, ssize_t size, long long timeout);
ssize_t syncRead(int fd, char *ptr, ssize_t size, long long timeout);
ssize_t syncReadLine(int fd, char *ptr, ssize_t size, long long timeout);

/* Replication */
void replicationFeedSlaves(list *slaves, int dictid, robj **argv, int argc);
void replicationFeedMonitors(redisClient *c, list *monitors, int dictid, robj **argv, int argc);
void updateSlavesWaitingBgsave(int bgsaveerr, int type);
void replicationCron(void);
void replicationHandleMasterDisconnection(void);
void replicationCacheMaster(redisClient *c);
void resizeReplicationBacklog(long long newsize);
void replicationSetMaster(char *ip, int port);
void replicationUnsetMaster(void);
void refreshGoodSlavesCount(void);
void replicationScriptCacheInit(void);
void replicationScriptCacheFlush(void);
void replicationScriptCacheAdd(sds sha1);
int replicationScriptCacheExists(sds sha1);
void processClientsWaitingReplicas(void);
void unblockClientWaitingReplicas(redisClient *c);
int replicationCountAcksByOffset(long long offset);
void replicationSendNewlineToMaster(void);
long long replicationGetSlaveOffset(void);
char *replicationGetSlaveName(redisClient *c);
long long getPsyncInitialOffset(void);
int replicationSetupSlaveForFullResync(redisClient *slave, long long offset);

/* Generic persistence functions */
void startLoading(FILE *fp);
void loadingProgress(off_t pos);
void stopLoading(void);

/* RDB persistence */
#include "rdb.h"

/* AOF persistence */
void flushAppendOnlyFile(int force);
void feedAppendOnlyFile(struct redisCommand *cmd, int dictid, robj **argv, int argc);
void aofRemoveTempFile(pid_t childpid);
int rewriteAppendOnlyFileBackground(void);
int loadAppendOnlyFile(char *filename);
void stopAppendOnly(void);
int startAppendOnly(void);
void backgroundRewriteDoneHandler(int exitcode, int bysignal);
void aofRewriteBufferReset(void);
unsigned long aofRewriteBufferSize(void);

/* Sorted sets data type */

/* Struct to hold a inclusive/exclusive range spec by score comparison. */
// 表示开区间/闭区间范围的结构
typedef struct {
	// 最小值和最大值
    double min, max;
    // 指示最小值和最大值是否*不*包含在范围之内
    // 值为1表示不包含，值为0表示包含
    int minex, maxex; /* are min or max exclusive? */
} zrangespec;

/* Struct to hold an inclusive/exclusive range spec by lexicographic comparison. */
typedef struct {
    robj *min, *max;  /* May be set to shared.(minstring|maxstring) */
    int minex, maxex; /* are min or max exclusive? */
} zlexrangespec;

/*
 * 作用：创建一个新的跳跃表
 * 时间复杂度：O(1)
 */
zskiplist *zslCreate(void);
/*
 * 作用：释放给定跳跃表，以及表中包含的所有节点
 * Time complexity: O(N)，N为跳跃表的长度
 */
void zslFree(zskiplist *zsl);
/*
 * 作用：将包含给定成员和分值的新节点添加到跳跃表中
 * Time complexity: O(logN)，最坏O(N)，N为跳跃表长度
 */
zskiplistNode *zslInsert(zskiplist *zsl, double score, robj *obj);
unsigned char *zzlInsert(unsigned char *zl, robj *ele, double score);
/*
 * 作用：删除跳跃表中包含给定成员和分值的节点
 * Time complexity: 平均O(logN)，最坏O(N)，N为跳跃表长度
 */
int zslDelete(zskiplist *zsl, double score, robj *obj);
/*
 * 作用：给定一个分值范围，返回跳跃表中第一个符合这个范围的节点
 * Time complexity: 平均O(logN)，最坏O(N)，N为跳跃表长度
 */
zskiplistNode *zslFirstInRange(zskiplist *zsl, zrangespec *range);
/*
 * 作用：给定一个分值范围，返回跳跃表中最后一个符合这个范围的节点
 * Time complexity: 平均O(logN)，最坏O(N)，N为跳跃表长度
 */
zskiplistNode *zslLastInRange(zskiplist *zsl, zrangespec *range);
double zzlGetScore(unsigned char *sptr);
void zzlNext(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);
void zzlPrev(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);
unsigned int zsetLength(robj *zobj);
void zsetConvert(robj *zobj, int encoding);
/*
 * 作用：返回包含给定成员和分值的节点在跳跃表中的排位
 * Time complexity: 平均O(logN)，最坏O(N)，N为跳跃表长度
 */
unsigned long zslGetRank(zskiplist *zsl, double score, robj *o);

/* Core functions */
int freeMemoryIfNeeded(void);
int processCommand(redisClient *c);
void setupSignalHandlers(void);
struct redisCommand *lookupCommand(sds name);
struct redisCommand *lookupCommandByCString(char *s);
struct redisCommand *lookupCommandOrOriginal(sds name);
void call(redisClient *c, int flags);
void propagate(struct redisCommand *cmd, int dbid, robj **argv, int argc, int flags);
void alsoPropagate(struct redisCommand *cmd, int dbid, robj **argv, int argc, int target);
void forceCommandPropagation(redisClient *c, int flags);
int prepareForShutdown();
#ifdef __GNUC__
void redisLog(int level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
void redisLog(int level, const char *fmt, ...);
#endif
void redisLogRaw(int level, const char *msg);
void redisLogFromHandler(int level, const char *msg);
void usage(void);
void updateDictResizePolicy(void);
int htNeedsResize(dict *dict);
void oom(const char *msg);
void populateCommandTable(void);
void resetCommandTableStats(void);
void adjustOpenFilesLimit(void);
void closeListeningSockets(int unlink_unix_socket);
void updateCachedTime(void);
void resetServerStats(void);
unsigned int getLRUClock(void);

/* Set data type */
robj *setTypeCreate(robj *value);
int setTypeAdd(robj *subject, robj *value);
int setTypeRemove(robj *subject, robj *value);
int setTypeIsMember(robj *subject, robj *value);
setTypeIterator *setTypeInitIterator(robj *subject);
void setTypeReleaseIterator(setTypeIterator *si);
int setTypeNext(setTypeIterator *si, robj **objele, int64_t *llele);
robj *setTypeNextObject(setTypeIterator *si);
int setTypeRandomElement(robj *setobj, robj **objele, int64_t *llele);
unsigned long setTypeSize(robj *subject);
void setTypeConvert(robj *subject, int enc);

/* Hash data type */
void hashTypeConvert(robj *o, int enc);
void hashTypeTryConversion(robj *subject, robj **argv, int start, int end);
void hashTypeTryObjectEncoding(robj *subject, robj **o1, robj **o2);
robj *hashTypeGetObject(robj *o, robj *key);
int hashTypeExists(robj *o, robj *key);
int hashTypeSet(robj *o, robj *key, robj *value);
int hashTypeDelete(robj *o, robj *key);
unsigned long hashTypeLength(robj *o);
hashTypeIterator *hashTypeInitIterator(robj *subject);
void hashTypeReleaseIterator(hashTypeIterator *hi);
int hashTypeNext(hashTypeIterator *hi);
void hashTypeCurrentFromZiplist(hashTypeIterator *hi, int what,
                                unsigned char **vstr,
                                unsigned int *vlen,
                                long long *vll);
void hashTypeCurrentFromHashTable(hashTypeIterator *hi, int what, robj **dst);
robj *hashTypeCurrentObject(hashTypeIterator *hi, int what);
robj *hashTypeLookupWriteOrCreate(redisClient *c, robj *key);

/* Pub / Sub */
int pubsubUnsubscribeAllChannels(redisClient *c, int notify);
int pubsubUnsubscribeAllPatterns(redisClient *c, int notify);
void freePubsubPattern(void *p);
int listMatchPubsubPattern(void *a, void *b);
int pubsubPublishMessage(robj *channel, robj *message);

/* Keyspace events notification */
void notifyKeyspaceEvent(int type, char *event, robj *key, int dbid);
int keyspaceEventsStringToFlags(char *classes);
sds keyspaceEventsFlagsToString(int flags);

/* Configuration */
void loadServerConfig(char *filename, char *options);
void appendServerSaveParams(time_t seconds, int changes);
void resetServerSaveParams(void);
struct rewriteConfigState; /* Forward declaration to export API. */
void rewriteConfigRewriteLine(struct rewriteConfigState *state, char *option, sds line, int force);
int rewriteConfig(char *path);

/* db.c -- Keyspace access API */
int removeExpire(redisDb *db, robj *key);
void propagateExpire(redisDb *db, robj *key);
int expireIfNeeded(redisDb *db, robj *key);
long long getExpire(redisDb *db, robj *key);
void setExpire(redisDb *db, robj *key, long long when);
robj *lookupKey(redisDb *db, robj *key);
robj *lookupKeyRead(redisDb *db, robj *key);
robj *lookupKeyWrite(redisDb *db, robj *key);
robj *lookupKeyReadOrReply(redisClient *c, robj *key, robj *reply);
robj *lookupKeyWriteOrReply(redisClient *c, robj *key, robj *reply);
void dbAdd(redisDb *db, robj *key, robj *val);
void dbOverwrite(redisDb *db, robj *key, robj *val);
void setKey(redisDb *db, robj *key, robj *val);
int dbExists(redisDb *db, robj *key);
robj *dbRandomKey(redisDb *db);
int dbDelete(redisDb *db, robj *key);
robj *dbUnshareStringValue(redisDb *db, robj *key, robj *o);
long long emptyDb(void(callback)(void*));
int selectDb(redisClient *c, int id);
void signalModifiedKey(redisDb *db, robj *key);
void signalFlushedDb(int dbid);
unsigned int getKeysInSlot(unsigned int hashslot, robj **keys, unsigned int count);
unsigned int countKeysInSlot(unsigned int hashslot);
unsigned int delKeysInSlot(unsigned int hashslot);
int verifyClusterConfigWithData(void);
void scanGenericCommand(redisClient *c, robj *o, unsigned long cursor);
int parseScanCursorOrReply(redisClient *c, robj *o, unsigned long *cursor);

/* API to get key arguments from commands */
int *getKeysFromCommand(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);
void getKeysFreeResult(int *result);
int *zunionInterGetKeys(struct redisCommand *cmd,robj **argv, int argc, int *numkeys);
int *evalGetKeys(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);
int *sortGetKeys(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);

/* Cluster */
void clusterInit(void);
unsigned short crc16(const char *buf, int len);
unsigned int keyHashSlot(char *key, int keylen);
void clusterCron(void);
void clusterPropagatePublish(robj *channel, robj *message);
void migrateCloseTimedoutSockets(void);
void clusterBeforeSleep(void);

/* Sentinel */
void initSentinelConfig(void);
void initSentinel(void);
void sentinelTimer(void);
char *sentinelHandleConfiguration(char **argv, int argc);
void sentinelIsRunning(void);

/* Scripting */
void scriptingInit(void);

/* Blocked clients */
void processUnblockedClients(void);
void blockClient(redisClient *c, int btype);
void unblockClient(redisClient *c);
void replyToBlockedClientTimedOut(redisClient *c);
int getTimeoutFromObjectOrReply(redisClient *c, robj *object, mstime_t *timeout, int unit);
void disconnectAllBlockedClients(void);

/* Git SHA1 */
char *redisGitSHA1(void);
char *redisGitDirty(void);
uint64_t redisBuildId(void);

/* Commands prototypes */
void authCommand(redisClient *c);
void pingCommand(redisClient *c);
void echoCommand(redisClient *c);
void commandCommand(redisClient *c);
void setCommand(redisClient *c);
void setnxCommand(redisClient *c);
void setexCommand(redisClient *c);
void psetexCommand(redisClient *c);
void getCommand(redisClient *c);
void delCommand(redisClient *c);
void existsCommand(redisClient *c);
void setbitCommand(redisClient *c);
void getbitCommand(redisClient *c);
void setrangeCommand(redisClient *c);
void getrangeCommand(redisClient *c);
void incrCommand(redisClient *c);
void decrCommand(redisClient *c);
void incrbyCommand(redisClient *c);
void decrbyCommand(redisClient *c);
void incrbyfloatCommand(redisClient *c);
void selectCommand(redisClient *c);
void randomkeyCommand(redisClient *c);
void keysCommand(redisClient *c);
void scanCommand(redisClient *c);
void dbsizeCommand(redisClient *c);
void lastsaveCommand(redisClient *c);
void saveCommand(redisClient *c);
void bgsaveCommand(redisClient *c);
void bgrewriteaofCommand(redisClient *c);
void shutdownCommand(redisClient *c);
void moveCommand(redisClient *c);
void renameCommand(redisClient *c);
void renamenxCommand(redisClient *c);
void lpushCommand(redisClient *c);
void rpushCommand(redisClient *c);
void lpushxCommand(redisClient *c);
void rpushxCommand(redisClient *c);
void linsertCommand(redisClient *c);
void lpopCommand(redisClient *c);
void rpopCommand(redisClient *c);
void llenCommand(redisClient *c);
void lindexCommand(redisClient *c);
void lrangeCommand(redisClient *c);
void ltrimCommand(redisClient *c);
void typeCommand(redisClient *c);
void lsetCommand(redisClient *c);
void saddCommand(redisClient *c);
void sremCommand(redisClient *c);
void smoveCommand(redisClient *c);
void sismemberCommand(redisClient *c);
void scardCommand(redisClient *c);
void spopCommand(redisClient *c);
void srandmemberCommand(redisClient *c);
void sinterCommand(redisClient *c);
void sinterstoreCommand(redisClient *c);
void sunionCommand(redisClient *c);
void sunionstoreCommand(redisClient *c);
void sdiffCommand(redisClient *c);
void sdiffstoreCommand(redisClient *c);
void sscanCommand(redisClient *c);
void syncCommand(redisClient *c);
void flushdbCommand(redisClient *c);
void flushallCommand(redisClient *c);
void sortCommand(redisClient *c);
void lremCommand(redisClient *c);
void rpoplpushCommand(redisClient *c);
void infoCommand(redisClient *c);
void mgetCommand(redisClient *c);
void monitorCommand(redisClient *c);
void expireCommand(redisClient *c);
void expireatCommand(redisClient *c);
void pexpireCommand(redisClient *c);
void pexpireatCommand(redisClient *c);
void getsetCommand(redisClient *c);
void ttlCommand(redisClient *c);
void pttlCommand(redisClient *c);
void persistCommand(redisClient *c);
void slaveofCommand(redisClient *c);
void roleCommand(redisClient *c);
void debugCommand(redisClient *c);
void msetCommand(redisClient *c);
void msetnxCommand(redisClient *c);
void zaddCommand(redisClient *c);
void zincrbyCommand(redisClient *c);
void zrangeCommand(redisClient *c);
void zrangebyscoreCommand(redisClient *c);
void zrevrangebyscoreCommand(redisClient *c);
void zrangebylexCommand(redisClient *c);
void zrevrangebylexCommand(redisClient *c);
void zcountCommand(redisClient *c);
void zlexcountCommand(redisClient *c);
void zrevrangeCommand(redisClient *c);
void zcardCommand(redisClient *c);
void zremCommand(redisClient *c);
void zscoreCommand(redisClient *c);
void zremrangebyscoreCommand(redisClient *c);
void zremrangebylexCommand(redisClient *c);
void multiCommand(redisClient *c);
void execCommand(redisClient *c);
void discardCommand(redisClient *c);
void blpopCommand(redisClient *c);
void brpopCommand(redisClient *c);
void brpoplpushCommand(redisClient *c);
void appendCommand(redisClient *c);
void strlenCommand(redisClient *c);
void zrankCommand(redisClient *c);
void zrevrankCommand(redisClient *c);
void hsetCommand(redisClient *c);
void hsetnxCommand(redisClient *c);
void hgetCommand(redisClient *c);
void hmsetCommand(redisClient *c);
void hmgetCommand(redisClient *c);
void hdelCommand(redisClient *c);
void hlenCommand(redisClient *c);
void zremrangebyrankCommand(redisClient *c);
void zunionstoreCommand(redisClient *c);
void zinterstoreCommand(redisClient *c);
void zscanCommand(redisClient *c);
void hkeysCommand(redisClient *c);
void hvalsCommand(redisClient *c);
void hgetallCommand(redisClient *c);
void hexistsCommand(redisClient *c);
void hscanCommand(redisClient *c);
void configCommand(redisClient *c);
void hincrbyCommand(redisClient *c);
void hincrbyfloatCommand(redisClient *c);
void subscribeCommand(redisClient *c);
void unsubscribeCommand(redisClient *c);
void psubscribeCommand(redisClient *c);
void punsubscribeCommand(redisClient *c);
void publishCommand(redisClient *c);
void pubsubCommand(redisClient *c);
void watchCommand(redisClient *c);
void unwatchCommand(redisClient *c);
void clusterCommand(redisClient *c);
void restoreCommand(redisClient *c);
void migrateCommand(redisClient *c);
void askingCommand(redisClient *c);
void readonlyCommand(redisClient *c);
void readwriteCommand(redisClient *c);
void dumpCommand(redisClient *c);
void objectCommand(redisClient *c);
void clientCommand(redisClient *c);
void evalCommand(redisClient *c);
void evalShaCommand(redisClient *c);
void scriptCommand(redisClient *c);
void timeCommand(redisClient *c);
void bitopCommand(redisClient *c);
void bitcountCommand(redisClient *c);
void bitposCommand(redisClient *c);
void replconfCommand(redisClient *c);
void waitCommand(redisClient *c);
void pfselftestCommand(redisClient *c);
void pfaddCommand(redisClient *c);
void pfcountCommand(redisClient *c);
void pfmergeCommand(redisClient *c);
void pfdebugCommand(redisClient *c);
void latencyCommand(redisClient *c);

#if defined(__GNUC__)
void *calloc(size_t count, size_t size) __attribute__ ((deprecated));
void free(void *ptr) __attribute__ ((deprecated));
void *malloc(size_t size) __attribute__ ((deprecated));
void *realloc(void *ptr, size_t size) __attribute__ ((deprecated));
#endif

/* Debugging stuff */
void _redisAssertWithInfo(redisClient *c, robj *o, char *estr, char *file, int line);
void _redisAssert(char *estr, char *file, int line);
void _redisPanic(char *msg, char *file, int line);
void bugReportStart(void);
void redisLogObjectDebugInfo(robj *o);
void sigsegvHandler(int sig, siginfo_t *info, void *secret);
sds genRedisInfoString(char *section);
void enableWatchdog(int period);
void disableWatchdog(void);
void watchdogScheduleSignal(int period);
void redisLogHexDump(int level, char *descr, void *value, size_t len);

#define redisDebug(fmt, ...) \
    printf("DEBUG %s:%d > " fmt "\n", __FILE__, __LINE__, __VA_ARGS__)
#define redisDebugMark() \
    printf("-- MARK %s:%d --\n", __FILE__, __LINE__)

#endif
