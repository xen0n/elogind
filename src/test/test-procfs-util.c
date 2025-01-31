/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <errno.h>

#include "errno-util.h"
#include "format-util.h"
#include "log.h"
#include "procfs-util.h"
#include "tests.h"

int main(int argc, char *argv[]) {
#if 0 /// elogind only needs v
        char buf[CONST_MAX(FORMAT_TIMESPAN_MAX, FORMAT_BYTES_MAX)];
        nsec_t nsec;
#endif // 0
        uint64_t v;
        int r;

        log_parse_environment();
        log_open();

#if 0 /// UNSUPPORTED by elogind (we aren't init)
        assert_se(procfs_cpu_get_usage(&nsec) >= 0);
        log_info("Current system CPU time: %s", format_timespan(buf, sizeof(buf), nsec/NSEC_PER_USEC, 1));

        assert_se(procfs_memory_get_used(&v) >= 0);
        log_info("Current memory usage: %s", format_bytes(buf, sizeof(buf), v));

        assert_se(procfs_tasks_get_current(&v) >= 0);
        log_info("Current number of tasks: %" PRIu64, v);
#endif // 0

        r = procfs_tasks_get_limit(&v);
        if (r == -ENOENT || ERRNO_IS_PRIVILEGE(r))
                return log_tests_skipped("can't read /proc/sys/kernel/pid_max");

        assert_se(r >= 0);
        log_info("Limit of tasks: %" PRIu64, v);
        assert_se(v > 0);
#if 0 /// UNSUPPORTED by elogind (we aren't init)
        assert_se(procfs_tasks_set_limit(v) >= 0);

        if (v > 100) {
                uint64_t w;
                r = procfs_tasks_set_limit(v-1);
                assert_se(IN_SET(r, 0, -EPERM, -EACCES, -EROFS));

                assert_se(procfs_tasks_get_limit(&w) >= 0);
                assert_se((r == 0 && w == v - 1) || (r < 0 && w == v));

                assert_se(procfs_tasks_set_limit(v) >= 0);

                assert_se(procfs_tasks_get_limit(&w) >= 0);
                assert_se(v == w);
        }
#endif // 0

        return 0;
}
