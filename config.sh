echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
sudo ./utils/use_dax.sh bind
sudo ./utils/uio_setup.sh linux config
sudo ./libfs/bin/mkfs.mlfs 1
sudo ./libfs/bin/mkfs.mlfs 4

sudo rm -rf /tmp/filebench-*
sudo rm -rf /tmp/mlfs_cli.*
sudo rm -rf /tmp/digest*
