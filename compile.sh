KERNFS_PATH=/home/sekwon/strata_sekwon/kernfs
LIBFS_PATH=/home/sekwon/strata_sekwon/libfs

cd $LIBFS_PATH
sudo make clean
cd $LIBFS_PATH/tests
sudo make clean

cd $KERNFS_PATH
sudo make clean
cd $KERNFS_PATH/tests
sudo make clean

cd $LIBFS_PATH
sudo make -j4
cd $LIBFS_PATH/tests
sudo make -j4

cd $KERNFS_PATH
sudo make -j4
cd $KERNFS_PATH/tests
sudo make -j4
