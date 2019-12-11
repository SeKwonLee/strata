A Stint with Hash tables in Strata
==================================
Se Kwon Lee, Manu Viswanadhan, Hochan Lee, Kartik Sathyanarayanan
[Paper](https://github.com/SeKwonLee/strata/tree/master/documents/paper.pdf) [Poster](https://github.com/SeKwonLee/strata/tree/master/documents/poster.pdf)

### Abstract ###
Due to the block-oriented nature of most storage devices, file systems have relied on block-oriented indexing structures, such as extent trees. With the arrival of Non-Volatile Memory (which is byte-addressable), it is now possible to use finer-grained indexing structures, such as hash tables. Strata is a recently proposed file system that efficiently unifies the management of multi-layered storage including NVM, SSD, and HDD. In this project, the extent tree implementation in the NVM portion of Strata to index physical blocks is replaced with a hash table and its performance evaluated.

### Building & Running Strata ###
Please refer to original Strata repository (https://github.com/ut-osa/strata) for the instructions to build and run Strata.

### Strata configuration ###
We add two kinds of compile options (`-DMLFS_HASH`, `-DLEVEL_HASH`) to `libfs/Makefile` and `kernfs/Makefile` respectively in order to enable hash tables in Strata. To apply Level hashing, please enable both `-DMLFS_HASH` and `-DLEVEL_HASH`. For BCH (Bucketized Cuckoo Hashing), please enable only `-DMLFS_HASH`. If you want to make use of original Strata based on Extent tree, please disable both `-DMLFS_HASH` and `-DLEVEL_HASH`.

##### 1. LibFS configuration ######
In `libfs/Makefile`, search `MLFS_FLAGS` as keyword
~~~~
MLFS_FLAGS = -DLIBFS -DMLFS_INFO
#MLFS_FLAGS += -DCONCURRENT
MLFS_FLAGS += -DINVALIDATION
#MLFS_FLAGS += -DKLIB_HASH
MLFS_FLAGS += -DUSE_SSD
#MLFS_FLAGS += -DUSE_HDD
#MLFS_FLAGS += -DMLFS_LOG
#MLFS_FLAGS += -DMLFS_HASH
#MLFS_FLAGS += -DLEVEL_HASH
~~~~

`DCONCURRENT` - allow parallelism in libfs <br/>
`DKLIB_HASH` - use klib hashing for log hash table <br/>
`DUSE_SSD`, `DUSE_HDD` - make LibFS to use SSD and HDD <br/>
`DMLFS_HASH` - make LibFS to use hash tables instead of extent tree when reading inode blocks in KernelFS <br/>
`DLEVEL_HASH` - make LibFS to use Level hashing instaed of BCH (Bucketized Cuckoo hash) <br/>

###### 2. KernelFS configuration ######
~~~
#MLFS_FLAGS = -DKERNFS
MLFS_FLAGS += -DBALLOC
#MLFS_FLAGS += -DDIGEST_OPT
#MLFS_FLAGS += -DIOMERGE
#MLFS_FLAGS += -DCONCURRENT
#MLFS_FLAGS += -DFCONCURRENT
#MLFS_FLAGS += -DUSE_SSD
#MLFS_FLAGS += -DUSE_HDD
#MLFS_FLAGS += -DMIGRATION
#MLFS_FLAGS += -DEXPERIMENTAL
#MLFS_FLAGS += -DMLFS_HASH
#MLFS_FALGS += -DLEVEL_HASH
~~~

`DBALLOC` - use new block allocator (use it always) <br/>
`DIGEST_OPT` - use log coalescing <br/>
`DIOMERGE` - use io merging <br/>
`DCONCURRENT` - allow concurrent digest <br/>
`DMIGRATION` - allow data migration. It requires turning on `DUSE_SSD` <br/>
`DMLFS_HASH` - make KernelFS to use hash tables instead of extent tree for indexing inode blocks <br/>
`DLEVEL_HASH` - make KernelFS to use Level hashing instaed of BCH (Bucketized Cuckoo hash) <br/>

For debugging, DIGEST_OPT, DIOMERGE, DCONCURRENT is disabled for now
