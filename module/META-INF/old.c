// Telegram: @RiProG | Channel: @RiOpSo | Group: @RiOpSoDisc
// Don't use or modify this code without permission

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
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

void write_to_file(const char *path, const char *value) {
  FILE *file = fopen(path, "w");
  if (file) {
    fputs(value, file);
    fclose(file);
  }
}

void write_long_to_file(const char *path, long long value) {
  char buf[64];
  snprintf(buf, sizeof(buf), "%lld", value);
  write_to_file(path, buf);
}

int match_pattern(const char *name) {
  return strcmp(name, "kgsl-3d0") == 0 || strstr(name, ".mali") ||
         strstr(name, ".gpu");
}

int search_gpu_dir_recursive(const char *base_path, char *result,
                             size_t result_size) {
  DIR *dir = opendir(base_path);
  if (!dir)
    return 0;
  struct dirent *entry;
  char path[PATH_MAX];
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] == '.')
      continue;
    snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
    if ((entry->d_type == DT_DIR || entry->d_type == DT_UNKNOWN) &&
        is_directory(path)) {
      if (match_pattern(entry->d_name)) {
        strncpy(result, result_size > 0 ? result : "", result_size - 1);
        result[result_size - 1] = '\0';
        closedir(dir);
        return 1;
      }
      if (search_gpu_dir_recursive(path, result, result_size)) {
        closedir(dir);
        return 1;
      }
    }
  }
  closedir(dir);
  return 0;
}

int find_gpu_dir(char *gpu_dir, size_t size) {
  const char *roots[] = {"/sys/devices", "/sys/class/misc"};
  for (int i = 0; i < 2; ++i) {
    if (search_gpu_dir_recursive(roots[i], gpu_dir, size))
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
  write_long_to_file("/proc/sys/kernel/perf_cpu_time_max_percent", 3);
  write_long_to_file("/proc/sys/kernel/sched_autogroup_enabled", 1);
  write_long_to_file("/proc/sys/kernel/sched_child_runs_first", 0);
  write_long_to_file("/proc/sys/kernel/sched_tunable_scaling", 0);
  write_long_to_file("/proc/sys/kernel/sched_latency_ns", 4000000);
  write_long_to_file("/proc/sys/kernel/sched_min_granularity_ns", 100000);
  write_long_to_file("/proc/sys/kernel/sched_wakeup_granularity_ns", 2000000);
  write_long_to_file("/proc/sys/kernel/sched_migration_cost_ns", 5000000);
  write_long_to_file("/proc/sys/kernel/sched_nr_migrate", 128);
  write_long_to_file("/proc/sys/kernel/sched_schedstats", 0);
  write_long_to_file("/proc/sys/kernel/random/read_wakeup_threshold", 64);
  write_long_to_file("/proc/sys/kernel/random/write_wakeup_threshold", 128);
  write_long_to_file("/proc/sys/kernel/panic", 0);
  write_long_to_file("/proc/sys/kernel/panic_on_oops", 0);
  write_to_file("/proc/sys/kernel/printk_devkmsg", "off");
  write_long_to_file("/proc/sys/vm/dirty_background_ratio", 15);
  write_long_to_file("/proc/sys/vm/dirty_ratio", 30);
  write_long_to_file("/proc/sys/vm/dirty_expire_centisecs", 3000);
  write_long_to_file("/proc/sys/vm/dirty_writeback_centisecs", 3000);
  write_long_to_file("/proc/sys/vm/page-cluster", 0);
  write_long_to_file("/proc/sys/vm/stat_interval", 10);
  write_long_to_file("/proc/sys/vm/swappiness", 40);
  write_long_to_file("/proc/sys/vm/vfs_cache_pressure", 80);
  write_long_to_file("/proc/sys/vm/overcommit_ratio", 80);
  write_long_to_file("/proc/sys/vm/extra_free_kbytes", 10240);
  write_long_to_file("/proc/sys/vm/panic_on_oom", 0);
  for (int cpu = 0; cpu < 8; cpu++) {
    char cpudir[256], pathbuf[256], governor[32];
    snprintf(cpudir, sizeof(cpudir), "/sys/devices/system/cpu/cpu%d/cpufreq",
             cpu);
    snprintf(pathbuf, sizeof(pathbuf), "%s/scaling_available_governors",
             cpudir);
    FILE *f = fopen(pathbuf, "r");
    if (!f)
      break;
    fscanf(f, "%31s", governor);
    fclose(f);
    char *gov = strstr(governor, "schedutil") ? "schedutil" : "interactive";
    snprintf(pathbuf, sizeof(pathbuf), "%s/scaling_governor", cpudir);
    write_to_file(pathbuf, gov);
  }
  for (int i = 0; i < 4; i++) {
    char queuedir[256], path[256], sched[32];
    snprintf(queuedir, sizeof(queuedir), "/sys/block/sd%c/queue", 'a' + i);
    snprintf(path, sizeof(path), "%s/scheduler", queuedir);
    FILE *f = fopen(path, "r");
    if (!f)
      break;
    fscanf(f, "%31s", sched);
    fclose(f);
    write_to_file(path, strstr(sched, "bfq") ? "bfq" : "mq-deadline");
    snprintf(path, sizeof(path), "%s/read_ahead_kb", queuedir);
    write_long_to_file(path, 64);
    snprintf(path, sizeof(path), "%s/nr_requests", queuedir);
    write_long_to_file(path, 256);
  }
  FILE *f = fopen("/proc/sys/net/ipv4/tcp_available_congestion_control", "r");
  if (f) {
    char cong[32];
    fscanf(f, "%31s", cong);
    fclose(f);
    const char *cc = strstr(cong, "bbr")        ? "bbr"
                     : strstr(cong, "cubic")    ? "cubic"
                     : strstr(cong, "westwood") ? "westwood"
                                                : "reno";
    write_to_file("/proc/sys/net/ipv4/tcp_congestion_control", cc);
    write_long_to_file("/proc/sys/net/ipv4/tcp_ecn", 1);
    write_long_to_file("/proc/sys/net/ipv4/tcp_fastopen", 3);
    write_long_to_file("/proc/sys/net/ipv4/tcp_syncookies", 0);
  }
  write_to_file("/sys/kernel/debug/sched_features", "NEXT_BUDDY\n");
  write_to_file("/sys/kernel/debug/sched_features", "TTWU_QUEUE\n");
  write_to_file("/sys/kernel/debug/sched_features", "NO_HRTICK\n");
  write_to_file("/sys/kernel/debug/sched_features", "WAKEUP_PREEMPTION\n");
  write_long_to_file("/dev/stune/top-app/schedtune.prefer_idle", 0);
  write_long_to_file("/dev/stune/top-app/schedtune.boost", 1);
  char gpu_dir[PATH_MAX];
  if (find_gpu_dir(gpu_dir, sizeof(gpu_dir))) {
    int is_adreno = strstr(gpu_dir, "kgsl-3d0") != NULL;
    char path[PATH_MAX];
    if (is_adreno) {
      snprintf(path, sizeof(path), "%s/throttling", gpu_dir);
      write_to_file(path, "0");
      snprintf(path, sizeof(path), "%s/thermal_pwrlevel", gpu_dir);
      write_to_file(path, "0");
      snprintf(path, sizeof(path), "%s/devfreq/adrenoboost", gpu_dir);
      write_to_file(path, "0");
      snprintf(path, sizeof(path), "%s/bus_split", gpu_dir);
      write_to_file(path, "0");
      snprintf(path, sizeof(path), "%s/min_pwrlevel", gpu_dir);
      write_to_file(path, "0");
      snprintf(path, sizeof(path), "%s/force_no_nap", gpu_dir);
      write_to_file(path, "1");
      snprintf(path, sizeof(path), "%s/force_bus_on", gpu_dir);
      write_to_file(path, "1");
      snprintf(path, sizeof(path), "%s/force_clk_on", gpu_dir);
      write_to_file(path, "1");
      snprintf(path, sizeof(path), "%s/force_rail_on", gpu_dir);
      write_to_file(path, "1");
      snprintf(path, sizeof(path), "%s/popp", gpu_dir);
      write_to_file(path, "0");
      snprintf(path, sizeof(path), "%s/pwrnap", gpu_dir);
      write_to_file(path, "0");
      write_to_file("/sys/kernel/debug/sde_rotator0/clk_always_on", "1");
      if (is_directory("/sys/module/simple_gpu_algorithm/")) {
        write_to_file(
            "/sys/module/simple_gpu_algorithm/parameters/simple_gpu_activate",
            "0");
      }
    } else {
      snprintf(path, sizeof(path), "%s/dvfs", gpu_dir);
      write_to_file(path, "0");
      snprintf(path, sizeof(path), "%s/highspeed_load", gpu_dir);
      write_to_file(path, "76");
      snprintf(path, sizeof(path), "%s/highspeed_delay", gpu_dir);
      write_to_file(path, "0");
      snprintf(path, sizeof(path), "%s/power_policy", gpu_dir);
      write_to_file(path, "always_on");
      snprintf(path, sizeof(path), "%s/cl_boost_disable", gpu_dir);
      write_to_file(path, "0");
      snprintf(path, sizeof(path), "%s/tmu", gpu_dir);
      write_to_file(path, "0");
    }
    if (is_directory("/sys/module/ged/")) {
      write_to_file("/sys/module/ged/parameters/ged_boost_enable", "1");
      write_to_file("/sys/module/ged/parameters/boost_gpu_enable", "1");
      write_to_file("/sys/module/ged/parameters/boost_extra", "1");
      write_to_file("/sys/module/ged/parameters/enable_cpu_boost", "1");
      write_to_file("/sys/module/ged/parameters/enable_gpu_boost", "1");
      write_to_file("/sys/module/ged/parameters/enable_game_self_frc_detect",
                    "1");
      write_to_file("/sys/module/ged/parameters/ged_force_mdp_enable", "1");
      write_to_file("/sys/module/ged/parameters/ged_log_perf_trace_enable",
                    "0");
      write_to_file("/sys/module/ged/parameters/ged_log_trace_enable", "0");
      write_to_file("/sys/module/ged/parameters/ged_monitor_3D_fence_debug",
                    "0");
      write_to_file("/sys/module/ged/parameters/ged_monitor_3D_fence_disable",
                    "1");
      write_to_file("/sys/module/ged/parameters/ged_monitor_3D_fence_systrace",
                    "0");
      write_to_file("/sys/module/ged/parameters/ged_smart_boost", "0");
      write_to_file("/sys/module/ged/parameters/gpu_debug_enable", "0");
      write_to_file("/sys/module/ged/parameters/gpu_dvfs_enable", "0");
      write_to_file("/sys/module/ged/parameters/gx_3D_benchmark_on", "1");
      write_to_file("/sys/module/ged/parameters/gx_force_cpu_boost", "1");
      write_to_file("/sys/module/ged/parameters/gx_frc_mode", "1");
      write_to_file("/sys/module/ged/parameters/gx_game_mode", "1");
      write_to_file("/sys/module/ged/parameters/is_GED_KPI_enabled", "1");
      write_to_file("/sys/module/ged/parameters/boost_amp", "1");
      write_to_file("/sys/module/ged/parameters/gx_boost_on", "1");
      write_to_file("/sys/module/ged/parameters/gpu_idle", "0");
      write_to_file("/sys/module/ged/parameters/gpu_cust_boost_freq", "0");
    }
    if (is_directory("/proc/gpufreq/")) {
      write_to_file("/proc/gpufreq/gpufreq_opp_stress_test", "1");
      write_to_file("/proc/gpufreq/gpufreq_input_boost", "0");
      write_to_file("/proc/gpufreq/gpufreq_limited_thermal_ignore", "1");
      write_to_file("/proc/gpufreq/gpufreq_limited_oc_ignore", "1");
      write_to_file("/proc/gpufreq/gpufreq_limited_low_batt_volume_ignore",
                    "1");
      write_to_file("/proc/gpufreq/gpufreq_limited_low_batt_volt_ignore", "1");
    }
    if (is_directory("/sys/kernel/ged/")) {
      write_to_file("/sys/kernel/ged/hal/dcs_mode", "0");
    }
    if (is_directory("/proc/mali/")) {
      write_to_file("/proc/mali/dvfs_enable", "0");
      write_to_file("/proc/mali/always_on", "1");
    }
  }
  return 0;
}