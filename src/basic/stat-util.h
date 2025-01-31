/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include "macro.h"
#include "missing_stat.h"

#if 0 /// UNNEEDED by elogind
int is_symlink(const char *path);
#endif // 0
int is_dir(const char *path, bool follow);
int is_dir_fd(int fd);
#if 0 /// UNNEEDED by elogind
int is_device_node(const char *path);

int dir_is_empty_at(int dir_fd, const char *path);
static inline int dir_is_empty(const char *path) {
        return dir_is_empty_at(AT_FDCWD, path);
}

static inline int dir_is_populated(const char *path) {
        int r;
        r = dir_is_empty(path);
        if (r < 0)
                return r;
        return !r;
}
#endif // 0

bool null_or_empty(struct stat *st) _pure_;
int null_or_empty_path(const char *fn);
#if 0 /// UNNEEDED by elogind
int null_or_empty_fd(int fd);
#endif // 0

int path_is_read_only_fs(const char *path);
#if 0 /// UNNEEDED by elogind
#endif // 0

int files_same(const char *filea, const char *fileb, int flags);

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
typedef typeof(((struct statfs*)NULL)->f_type) statfs_f_type_t;

bool is_fs_type(const struct statfs *s, statfs_f_type_t magic_value) _pure_;
int fd_is_fs_type(int fd, statfs_f_type_t magic_value);
int path_is_fs_type(const char *path, statfs_f_type_t magic_value);

bool is_temporary_fs(const struct statfs *s) _pure_;
bool is_network_fs(const struct statfs *s) _pure_;

#if 0 /// UNNEEDED by elogind
int fd_is_temporary_fs(int fd);
int fd_is_network_fs(int fd);

int path_is_temporary_fs(const char *path);
#endif // 0

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

int stat_verify_regular(const struct stat *st);
int fd_verify_regular(int fd);

int stat_verify_directory(const struct stat *st);
int fd_verify_directory(int fd);

/* glibc and the Linux kernel have different ideas about the major/minor size. These calls will check whether the
 * specified major is valid by the Linux kernel's standards, not by glibc's. Linux has 20bits of minor, and 12 bits of
 * major space. See MINORBITS in linux/kdev_t.h in the kernel sources. (If you wonder why we define _y here, instead of
 * comparing directly >= 0: it's to trick out -Wtype-limits, which would otherwise complain if the type is unsigned, as
 * such a test would be pointless in such a case.) */

#define DEVICE_MAJOR_VALID(x)                                           \
        ({                                                              \
                typeof(x) _x = (x), _y = 0;                             \
                _x >= _y && _x < (UINT32_C(1) << 12);                   \
                                                                        \
        })

#define DEVICE_MINOR_VALID(x)                                           \
        ({                                                              \
                typeof(x) _x = (x), _y = 0;                             \
                _x >= _y && _x < (UINT32_C(1) << 20);                   \
        })

int device_path_make_major_minor(mode_t mode, dev_t devno, char **ret);
int device_path_make_canonical(mode_t mode, dev_t devno, char **ret);
int device_path_parse_major_minor(const char *path, mode_t *ret_mode, dev_t *ret_devno);

int proc_mounted(void);

bool stat_inode_unmodified(const struct stat *a, const struct stat *b);

int statx_fallback(int dfd, const char *path, int flags, unsigned mask, struct statx *sx);

#if HAS_FEATURE_MEMORY_SANITIZER
#  warning "Explicitly initializing struct statx, to work around msan limitation. Please remove as soon as msan has been updated to not require this."
#  define STRUCT_STATX_DEFINE(var)              \
        struct statx var = {}
#  define STRUCT_NEW_STATX_DEFINE(var)          \
        union {                                 \
                struct statx sx;                \
                struct new_statx nsx;           \
        } var = {}
#else
#  define STRUCT_STATX_DEFINE(var)              \
        struct statx var
#  define STRUCT_NEW_STATX_DEFINE(var)          \
        union {                                 \
                struct statx sx;                \
                struct new_statx nsx;           \
        } var
#endif
