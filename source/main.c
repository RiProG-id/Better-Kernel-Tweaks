// Telegram: @RiProG | Channel: @RiOpSo | Group: @RiOpSoDisc
//  Do not redistribute without permission

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int is_file(const char *path) {
  struct stat st;
  return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

int is_directory(const char *path) {
  struct stat st;
  return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

void write_file(const char *path, const char *value) {
  if (!path || !value)
    return;

  char buf[64];
  int is_numeric = 1;
  for (size_t i = 0; value[i] != '\0'; i++) {
    if (!isdigit((unsigned char)value[i]) && value[i] != '-' &&
        value[i] != '+') {
      is_numeric = 0;
      break;
    }
  }

  FILE *file = fopen(path, "w");
  if (file) {
    if (is_numeric) {
      snprintf(buf, sizeof(buf), "%s", value);
      fputs(buf, file);
    } else {
      fputs(value, file);
    }
    fclose(file);
  }
}

int is_gpu_device(const char *name) {
  return strcmp(name, "kgsl-3d0") == 0 || strstr(name, ".mali") ||
         strstr(name, ".gpu");
}

int find_gpu_path_recursive(const char *search_path, char *gpu_path,
                            size_t path_buffer_size) {
  DIR *dir = opendir(search_path);
  if (!dir)
    return 0;
  struct dirent *entry;
  char current_path[PATH_MAX];
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] == '.')
      continue;
    snprintf(current_path, sizeof(current_path), "%s/%s", search_path,
             entry->d_name);
    if ((entry->d_type == DT_DIR || entry->d_type == DT_UNKNOWN) &&
        is_directory(current_path)) {
      if (is_gpu_device(entry->d_name)) {
        strncpy(gpu_path, current_path, path_buffer_size - 1);
        gpu_path[path_buffer_size - 1] = '\0';
        closedir(dir);
        return 1;
      }
      if (find_gpu_path_recursive(current_path, gpu_path, path_buffer_size)) {
        closedir(dir);
        return 1;
      }
    }
  }
  closedir(dir);
  return 0;
}

int get_gpu_path(char *gpu_path, size_t path_buffer_size) {
  const char *search_roots[] = {"/sys/devices", "/sys/class/misc"};
  for (int i = 0; i < 2; ++i) {
    if (find_gpu_path_recursive(search_roots[i], gpu_path, path_buffer_size))
      return 1;
  }
  return 0;
}

int main() {

  printf("\nTelegram: @RiProG | Channel: @RiOpSo | Group: @RiOpSoDisc\n\n");

  int null_fd = open("/dev/null", O_WRONLY);
  if (null_fd != -1) {
    dup2(null_fd, STDOUT_FILENO);
    dup2(null_fd, STDERR_FILENO);
    close(null_fd);
  }

  write_file("/proc/sys/kernel/perf_cpu_time_max_percent", "3");
  write_file("/proc/sys/kernel/sched_autogroup_enabled", "1");
  write_file("/proc/sys/kernel/sched_child_runs_first", "0");
  write_file("/proc/sys/kernel/sched_tunable_scaling", "0");
  write_file("/proc/sys/kernel/sched_latency_ns", "4000000");
  write_file("/proc/sys/kernel/sched_min_granularity_ns", "100000");
  write_file("/proc/sys/kernel/sched_wakeup_granularity_ns", "2000000");
  write_file("/proc/sys/kernel/sched_migration_cost_ns", "5000000");
  write_file("/proc/sys/kernel/sched_nr_migrate", "128");
  write_file("/proc/sys/kernel/sched_schedstats", "0");
  write_file("/proc/sys/kernel/random/read_wakeup_threshold", "64");
  write_file("/proc/sys/kernel/random/write_wakeup_threshold", "128");
  write_file("/proc/sys/kernel/panic", "0");
  write_file("/proc/sys/kernel/panic_on_oops", "0");
  write_file("/proc/sys/kernel/printk_devkmsg", "off");

  write_file("/proc/sys/vm/dirty_background_ratio", "15");
  write_file("/proc/sys/vm/dirty_ratio", "30");
  write_file("/proc/sys/vm/dirty_expire_centisecs", "3000");
  write_file("/proc/sys/vm/dirty_writeback_centisecs", "3000");
  write_file("/proc/sys/vm/page-cluster", "0");
  write_file("/proc/sys/vm/stat_interval", "10");
  write_file("/proc/sys/vm/swappiness", "40");
  write_file("/proc/sys/vm/vfs_cache_pressure", "80");
  write_file("/proc/sys/vm/overcommit_ratio", "80");
  write_file("/proc/sys/vm/extra_free_kbytes", "10240");
  write_file("/proc/sys/vm/panic_on_oom", "0");

  for (int cpu = 0; cpu < 8; cpu++) {
    char cpu_path[256], path_buffer[256], governors_list[64];
    snprintf(cpu_path, sizeof(cpu_path),
             "/sys/devices/system/cpu/cpu%d/cpufreq", cpu);
    snprintf(path_buffer, sizeof(path_buffer), "%s/scaling_available_governors",
             cpu_path);
    FILE *f = fopen(path_buffer, "r");
    if (!f)
      break;
    fread(governors_list, 1, sizeof(governors_list) - 1, f);
    fclose(f);
    char *active_governor =
        strstr(governors_list, "schedutil") ? "schedutil" : "interactive";
    snprintf(path_buffer, sizeof(path_buffer), "%s/scaling_governor", cpu_path);
    write_file(path_buffer, active_governor);
  }

  for (int i = 0; i < 4; i++) {
    char block_path[256], path_buffer[256], schedulers_list[64];
    snprintf(block_path, sizeof(block_path), "/sys/block/sd%c/queue", 'a' + i);
    snprintf(path_buffer, sizeof(path_buffer), "%s/scheduler", block_path);
    FILE *f = fopen(path_buffer, "r");
    if (!f)
      break;
    fread(schedulers_list, 1, sizeof(schedulers_list) - 1, f);
    fclose(f);
    write_file(path_buffer,
               strstr(schedulers_list, "bfq") ? "bfq" : "mq-deadline");
    snprintf(path_buffer, sizeof(path_buffer), "%s/read_ahead_kb", block_path);
    write_file(path_buffer, "64");
    snprintf(path_buffer, sizeof(path_buffer), "%s/nr_requests", block_path);
    write_file(path_buffer, "256");
  }

  FILE *f = fopen("/proc/sys/net/ipv4/tcp_available_congestion_control", "r");
  if (f) {
    char congestion_list[64];
    fread(congestion_list, 1, sizeof(congestion_list) - 1, f);
    fclose(f);
    const char *active_congestion = strstr(congestion_list, "bbr")     ? "bbr"
                                    : strstr(congestion_list, "cubic") ? "cubic"
                                    : strstr(congestion_list, "westwood")
                                        ? "westwood"
                                        : "reno";
    write_file("/proc/sys/net/ipv4/tcp_congestion_control", active_congestion);
    write_file("/proc/sys/net/ipv4/tcp_ecn", "1");
    write_file("/proc/sys/net/ipv4/tcp_fastopen", "3");
    write_file("/proc/sys/net/ipv4/tcp_syncookies", "0");
  }

  write_file("/sys/kernel/debug/sched_features", "NEXT_BUDDY\n");
  write_file("/sys/kernel/debug/sched_features", "TTWU_QUEUE\n");
  write_file("/sys/kernel/debug/sched_features", "NO_HRTICK\n");
  write_file("/sys/kernel/debug/sched_features", "WAKEUP_PREEMPTION\n");
  write_file("/dev/stune/top-app/schedtune.prefer_idle", "0");
  write_file("/dev/stune/top-app/schedtune.boost", "1");

  char gpu_device_path[PATH_MAX];
  if (get_gpu_path(gpu_device_path, sizeof(gpu_device_path))) {
    int is_adreno = strstr(gpu_device_path, "kgsl-3d0") != NULL;
    char path_buffer[PATH_MAX];
    if (is_adreno) {
      snprintf(path_buffer, sizeof(path_buffer), "%s/throttling",
               gpu_device_path);
      write_file(path_buffer, "0");
      snprintf(path_buffer, sizeof(path_buffer), "%s/force_clk_on",
               gpu_device_path);
      write_file(path_buffer, "1");
      snprintf(path_buffer, sizeof(path_buffer), "%s/force_rail_on",
               gpu_device_path);
      write_file(path_buffer, "1");
      snprintf(path_buffer, sizeof(path_buffer), "%s/force_no_nap",
               gpu_device_path);
      write_file(path_buffer, "1");
    } else {
      snprintf(path_buffer, sizeof(path_buffer), "%s/dvfs", gpu_device_path);
      write_file(path_buffer, "0");
      snprintf(path_buffer, sizeof(path_buffer), "%s/power_policy",
               gpu_device_path);
      write_file(path_buffer, "always_on");
    }

    const char *gpu_frequency_path = NULL;
    if (is_directory("/proc/gpufreq/"))
      gpu_frequency_path = "/proc/gpufreq/";
    else if (is_directory("/proc/gpufreqv2/"))
      gpu_frequency_path = "/proc/gpufreqv2/";

    if (gpu_frequency_path != NULL) {
      snprintf(path_buffer, sizeof(path_buffer), "%saging_mode",
               gpu_frequency_path);
      write_file(path_buffer, "disable");
      snprintf(path_buffer, sizeof(path_buffer), "%slimit_table",
               gpu_frequency_path);
      write_file(path_buffer, "0 0 0");
      snprintf(path_buffer, sizeof(path_buffer), "%sgpm_mode",
               gpu_frequency_path);
      write_file(path_buffer, "1");
      snprintf(path_buffer, sizeof(path_buffer), "%smfgsys_power_control",
               gpu_frequency_path);
      write_file(path_buffer, "1");
      snprintf(path_buffer, sizeof(path_buffer), "%sopp_stress_test",
               gpu_frequency_path);
      write_file(path_buffer, "0");
      snprintf(path_buffer, sizeof(path_buffer), "%sfix_target_opp_index",
               gpu_frequency_path);
      write_file(path_buffer, "-1");
    }

    if (is_directory("/sys/module/ged/")) {
      write_file("/sys/module/ged/parameters/ged_boost_enable", "1");
      write_file("/sys/module/ged/parameters/boost_gpu_enable", "1");
      write_file("/sys/module/ged/parameters/gx_boost_on", "1");
    }
  }

  return 0;
}