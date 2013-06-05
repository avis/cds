# Makefile 
# 

.PHONY: all
.SUFFIXES:.cpp .c .ro

# make_distrib.pl substitutes the correct version
VERSION=1.4.0

# boost_thread lib used by test application. 
# You may change the library name
LD_BOOST_THREAD_LIB=-lboost_thread 

# common libs
#LD_TEST_COMMON_LIBS=-lcds -lrt
#LD_TEST_COMMON_DEBUG_LIBS=-lcds-debug -lrt

ifeq ($(platform),mingw)
    LD_TEST_COMMON_LIBS=-lcds -lrtm
    LD_TEST_COMMON_DEBUG_LIBS=-lcds-debug -lrtm
    BASE_OPT = -D_REENTRANT -D_FILE_OFFSET_BITS=64 -I..
    # -Wa,--hash-size=2048
    CPP_BUILD_CDS_OPT=-DCDS_BUILD_LIB
else
    LD_TEST_COMMON_LIBS=-lcds -lrt
    LD_TEST_COMMON_DEBUG_LIBS=-lcds-debug -lrt
    BASE_OPT = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -D_FILE_OFFSET_BITS=64 -I..
    CPP_BUILD_CDS_OPT=
endif

COMP_OPT = -c $(CFLAGS) $(BASE_OPT)
CPP_COMP_OPT = -std=c++0x -c $(CXXFLAGS) $(BASE_OPT)

#LD_LIBS = -lpthread -ldl -lstdc++ 

COMPILER_ROOT = $(shell dirname `dirname \`which 	$(CXX)\``)

LD_OPTS = $(LDFLAGS)



####################################
# cds library

CDS_OBJS=$(OBJ_PATH)/hzp_gc.o \
         $(OBJ_PATH)/init.o \
         $(OBJ_PATH)/hrc_gc.o \
         $(OBJ_PATH)/ptb_gc.o \
		 $(OBJ_PATH)/urcu_gp.o \
		 $(OBJ_PATH)/urcu_sh.o \
         $(OBJ_PATH)/michael_heap.o \
         $(OBJ_PATH)/topology_hpux.o \
         $(OBJ_PATH)/topology_linux.o \
         $(OBJ_PATH)/dllmain.o

ifeq ($(platform),mingw)
    CDS_DEBUG_LIB=libcds-debug.dll
    CDS_RELEASE_LIB=libcds.dll
else
    CDS_DEBUG_LIB=libcds-debug.so
    CDS_RELEASE_LIB=libcds.so
endif

$(OBJ_PATH)/hzp_gc.o: ../src/hzp_gc.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<
$(OBJ_PATH)/hrc_gc.o: ../src/hrc_gc.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<
$(OBJ_PATH)/ptb_gc.o: ../src/ptb_gc.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<
$(OBJ_PATH)/urcu_gp.o: ../src/urcu_gp.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<
$(OBJ_PATH)/urcu_sh.o: ../src/urcu_sh.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<
$(OBJ_PATH)/init.o: ../src/init.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<
$(OBJ_PATH)/dllmain.o: ../src/dllmain.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<
$(OBJ_PATH)/michael_heap.o: ../src/michael_heap.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<
$(OBJ_PATH)/topology_hpux.o: ../src/topology_hpux.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<
$(OBJ_PATH)/topology_linux.o: ../src/topology_linux.cpp
	$(CXX) $(CPP_COMP_OPT) $(CPP_BUILD_CDS_OPT) -o $@ $<

ifeq ($(platform),mingw)
$(CDS_DEBUG_LIB): $(BIN_PATH)/$(CDS_DEBUG_LIB)

$(BIN_PATH)/$(CDS_DEBUG_LIB) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) -Wl,--out-implib,$(BIN_PATH)/$(CDS_DEBUG_LIB).a $(CDS_OBJS) -o $@ 

$(CDS_RELEASE_LIB) : $(BIN_PATH)/$(CDS_RELEASE_LIB)

$(BIN_PATH)/$(CDS_RELEASE_LIB) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) $(CDS_OBJS) -Wl,--out-implib,$(BIN_PATH)/$(CDS_RELEASE_LIB).a -o $@ 
    
debug : $(CDS_DEBUG_LIB)
release : $(CDS_RELEASE_LIB)
else
$(CDS_DEBUG_LIB).$(VERSION) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) $(CDS_PLATFORM_RELEASE_LDFLAGS) $(CDS_OBJS) -o $@ 
	mv ./$(CDS_DEBUG_LIB).$(VERSION) $(BIN_PATH)/$(CDS_DEBUG_LIB).$(VERSION)
	ln -sf $(CDS_DEBUG_LIB).$(VERSION) $(BIN_PATH)/$(CDS_DEBUG_LIB)
	ar rcs $(BIN_PATH)/libcds-debug.a $(CDS_OBJS)

$(CDS_RELEASE_LIB).$(VERSION) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) $(CDS_OBJS) $(CDS_PLATFORM_DEBUG_LDFLAGS) -o $@ 
	mv ./$(CDS_RELEASE_LIB).$(VERSION) $(BIN_PATH)/$(CDS_RELEASE_LIB).$(VERSION)
	ln -sf $(CDS_RELEASE_LIB).$(VERSION) $(BIN_PATH)/$(CDS_RELEASE_LIB)
	ar rcs $(BIN_PATH)/libcds.a $(CDS_OBJS)
    
debug : $(CDS_DEBUG_LIB).$(VERSION)
release : $(CDS_RELEASE_LIB).$(VERSION)
    
endif
    
all: debug release

##########################################
# Make tests

OBJ_TEST_PATH=$(OBJ_PATH)

TEST_COMMONHDR_SRC_DIR=../tests
TEST_COMMON_OBJS=$(TEST_COMMONHDR_SRC_DIR)/cppunit/test_main.o \
    $(TEST_COMMONHDR_SRC_DIR)/cppunit/thread.o \
    $(TEST_COMMONHDR_SRC_DIR)/unit/michael_alloc.o

$(TEST_COMMON_OBJS): %.o: %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TEST_COMMONHDR_SRC_DIR) $< -o $@

TESTHDR_SRC_DIR=../tests/test-hdr
TESTHDR_OBJS= \
    $(TESTHDR_SRC_DIR)/misc/cxx11_atomic_class.o \
    $(TESTHDR_SRC_DIR)/misc/cxx11_atomic_func.o \
    $(TESTHDR_SRC_DIR)/misc/find_option.o \
    $(TESTHDR_SRC_DIR)/misc/allocator_test.o \
    $(TESTHDR_SRC_DIR)/misc/michael_allocator.o \
    $(TESTHDR_SRC_DIR)/misc/hash_tuple.o \
    $(TESTHDR_SRC_DIR)/misc/bitop_st.o \
    $(TESTHDR_SRC_DIR)/misc/thread_init_fini.o \
    $(TESTHDR_SRC_DIR)/deque/hdr_michael_deque_hp.o \
    $(TESTHDR_SRC_DIR)/deque/hdr_michael_deque_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_rcu_gpi.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_rcu_gpb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_rcu_gpt.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_rcu_shb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_rcu_sht.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_rcu_gpb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_rcu_gpi.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_rcu_gpt.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_rcu_shb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_rcu_sht.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_rcu_gpi.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_rcu_gpb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_rcu_gpt.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_rcu_shb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_rcu_sht.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_rcu_gpi.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_rcu_gpb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_rcu_gpt.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_rcu_shb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_refinable_hashmap_hashmap_std.o \
    $(TESTHDR_SRC_DIR)/map/hdr_refinable_hashmap_boost_list.o \
    $(TESTHDR_SRC_DIR)/map/hdr_refinable_hashmap_list.o \
    $(TESTHDR_SRC_DIR)/map/hdr_refinable_hashmap_map.o \
    $(TESTHDR_SRC_DIR)/map/hdr_refinable_hashmap_boost_map.o \
    $(TESTHDR_SRC_DIR)/map/hdr_refinable_hashmap_boost_flat_map.o \
    $(TESTHDR_SRC_DIR)/map/hdr_refinable_hashmap_boost_unordered_map.o \
    $(TESTHDR_SRC_DIR)/map/hdr_refinable_hashmap_slist.o \
    $(TESTHDR_SRC_DIR)/map/hdr_skiplist_map_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_skiplist_map_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_skiplist_map_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_skiplist_map_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/map/hdr_skiplist_map_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_skiplist_map_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/map/hdr_skiplist_map_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_skiplist_map_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/map/hdr_skiplist_map_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_striped_hashmap_hashmap_std.o \
    $(TESTHDR_SRC_DIR)/map/hdr_striped_hashmap_boost_list.o \
    $(TESTHDR_SRC_DIR)/map/hdr_striped_hashmap_list.o \
    $(TESTHDR_SRC_DIR)/map/hdr_striped_hashmap_map.o \
    $(TESTHDR_SRC_DIR)/map/hdr_striped_hashmap_boost_map.o \
    $(TESTHDR_SRC_DIR)/map/hdr_striped_hashmap_boost_flat_map.o \
    $(TESTHDR_SRC_DIR)/map/hdr_striped_hashmap_boost_unordered_map.o \
    $(TESTHDR_SRC_DIR)/map/hdr_striped_hashmap_slist.o \
	$(TESTHDR_SRC_DIR)/map/hdr_striped_map_reg.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_basketqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_moirqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_msqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_tsigas_cycle_queue.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_vyukovmpmc_cycle_queue.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_basketqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_basketqueue_hzp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_basketqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_moirqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_moirqueue_hzp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_moirqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_msqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_msqueue_hzp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_msqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_optimistic_hzp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_optimistic_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_rwqueue.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_vyukov_mpmc_cyclic.o \
    $(TESTHDR_SRC_DIR)/queue/queue_test_header.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_hrc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_hrc_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_refinable_hashset_avlset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_refinable_hashset_list.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_refinable_hashset_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_refinable_hashset_sgset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_refinable_hashset_slist.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_refinable_hashset_splayset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_refinable_hashset_treapset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_refinable_hashset_uset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_hp.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_hrc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_ptb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_nogc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_hrc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_hrc_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_striped_hashset_avlset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_striped_hashset_list.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_striped_hashset_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_striped_hashset_sgset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_striped_hashset_slist.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_striped_hashset_splayset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_striped_hashset_treapset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_striped_hashset_uset.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_striped_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_hp.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_hrc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_ptb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_nogc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_lazy_hp.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_lazy_hrc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_lazy_ptb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_lazy_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_lazy_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_lazy_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_lazy_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_lazy_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/set/hdr_michael_set_lazy_nogc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_hashset_std.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_boost_flat_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_boost_list.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_boost_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_boost_stable_vector.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_boost_unordered_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_boost_vector.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_list.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_slist.o \
    $(TESTHDR_SRC_DIR)/set/hdr_refinable_hashset_vector.o \
    $(TESTHDR_SRC_DIR)/set/hdr_skiplist_set_hp.o \
    $(TESTHDR_SRC_DIR)/set/hdr_skiplist_set_hrc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_skiplist_set_ptb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_skiplist_set_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/set/hdr_skiplist_set_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_skiplist_set_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/set/hdr_skiplist_set_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_skiplist_set_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/set/hdr_skiplist_set_nogc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_hp.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_hrc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_nogc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_ptb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_lazy_hp.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_lazy_hrc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_lazy_nogc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_lazy_ptb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_lazy_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_lazy_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_lazy_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_lazy_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_splitlist_set_lazy_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_hashset_std.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_boost_flat_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_boost_list.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_boost_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_boost_stable_vector.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_boost_unordered_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_boost_vector.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_list.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_slist.o \
    $(TESTHDR_SRC_DIR)/set/hdr_striped_hashset_vector.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_intrusive_treiber_stack_hrc.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_treiber_stack_hp.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_treiber_stack_hrc.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_treiber_stack_ptb.o 

$(TESTHDR_OBJS): %.o: %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TESTHDR_SRC_DIR) -I$(TEST_COMMONHDR_SRC_DIR) $< -o $@
	
TESTHDR_OBJS_NO_OFFSETOF_WARN= \
    $(TESTHDR_SRC_DIR)/deque/hdr_intrusive_michael_deque_hp.o \
    $(TESTHDR_SRC_DIR)/deque/hdr_intrusive_michael_deque_ptb.o \
	$(TESTHDR_SRC_DIR)/map/hdr_cuckoo_map.o \
	$(TESTHDR_SRC_DIR)/set/hdr_cuckoo_set.o \
	$(TESTHDR_SRC_DIR)/set/hdr_intrusive_cuckoo_set.o \
	$(TESTHDR_SRC_DIR)/set/hdr_intrusive_cuckoo_refinable_set.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_hp.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_ptb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_nogc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_hp_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_ptb_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_nogc_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_gpi_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_gpb_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_gpt_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_shb_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_michael_set_rcu_sht_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_hp_member.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_ptb_member.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_gpi_member.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_gpb_member.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_gpt_member.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_shb_member.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_rcu_sht_member.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_skiplist_nogc_member.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_hp.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_nogc.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_ptb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_gpb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_gpi.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_gpt.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_shb.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_hp_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_nogc_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_ptb_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_gpb_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_gpi_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_gpt_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_shb_lazy.o \
    $(TESTHDR_SRC_DIR)/set/hdr_intrusive_splitlist_set_rcu_sht_lazy.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_rcu_gpb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_rcu_gpi.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_rcu_gpt.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_rcu_shb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_rcu_sht.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_list_rcu_gpb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_list_rcu_gpi.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_list_rcu_gpt.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_list_rcu_shb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_list_rcu_sht.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_basketqueue_hp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_basketqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_moirqueue_hp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_moirqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_msqueue_hp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_msqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_optimisticqueue_hp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_optimisticqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_intrusive_treiber_stack_hp.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_intrusive_treiber_stack_ptb.o

$(TESTHDR_OBJS_NO_OFFSETOF_WARN): %.o: %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TESTHDR_SRC_DIR) -I$(TEST_COMMONHDR_SRC_DIR) -Wno-invalid-offsetof $< -o $@

TEST_SRC_DIR=../tests/unit
TEST_DATA_DIR=`pwd`/../tests/data

CDSUNIT_COMMON_FILE=

CDSUNIT_MAP_FILE= \
    $(TEST_SRC_DIR)/map2/map_find_int.o \
    $(TEST_SRC_DIR)/map2/map_find_string.o \
    $(TEST_SRC_DIR)/map2/map_insdel_func.o \
    $(TEST_SRC_DIR)/map2/map_insdel_int.o \
    $(TEST_SRC_DIR)/map2/map_insdel_item_int.o \
    $(TEST_SRC_DIR)/map2/map_insdel_string.o \
    $(TEST_SRC_DIR)/map2/map_insdel_item_string.o \
	$(TEST_SRC_DIR)/map2/map_insfind_int.o \
	$(TEST_SRC_DIR)/map2/map_insdelfind.o

CDSUNIT_SET_FILE= \
	$(TEST_SRC_DIR)/set2/set_insdel_func.o \
	$(TEST_SRC_DIR)/set2/set_insdel_func2.o \
	$(TEST_SRC_DIR)/set2/set_insdel_func3.o \
	$(TEST_SRC_DIR)/set2/set_insdel_func4.o \
	$(TEST_SRC_DIR)/set2/set_insdel_func5.o \
	$(TEST_SRC_DIR)/set2/set_insdel_func6.o \
	$(TEST_SRC_DIR)/set2/set_insdel_string.o \
	$(TEST_SRC_DIR)/set2/set_insdelfind.o

CDSUNIT_QUEUE_FILE= \
    $(TEST_SRC_DIR)/queue/pop_mt.o \
    $(TEST_SRC_DIR)/queue/push_mt.o \
    $(TEST_SRC_DIR)/queue/random_mt.o \
    $(TEST_SRC_DIR)/queue/reader_writer_mt.o \
    $(TEST_SRC_DIR)/queue/Queue_bounded_empty_ST.o

CDSUNIT_STACK_FILE= \
    $(TEST_SRC_DIR)/stack/stack_push_mt.o 
	
CDSUNIT_MISC_FILE= \
    $(TEST_SRC_DIR)/alloc/hoard_threadtest.o \
    $(TEST_SRC_DIR)/alloc/larson.o \
    $(TEST_SRC_DIR)/alloc/linux_scale.o \
    $(TEST_SRC_DIR)/alloc/michael_allocator.o \
    $(TEST_SRC_DIR)/alloc/random.o \
    $(TEST_SRC_DIR)/lock/spinlock.o

TEST_OBJ_FILE= $(CDSUNIT_COMMON_FILE) $(CDSUNIT_MAP_FILE) $(CDSUNIT_SET_FILE) $(CDSUNIT_QUEUE_FILE) $(CDSUNIT_STACK_FILE) $(CDSUNIT_MISC_FILE)

$(TEST_OBJ_FILE): %.o: %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TEST_SRC_DIR) -I$(TEST_COMMONHDR_SRC_DIR) $< -o $@

CDSUNIT_MAP_EXE=$(BIN_PATH)/cdsu-map
CDSUNIT_SET_EXE=$(BIN_PATH)/cdsu-set
CDSUNIT_QUEUE_EXE=$(BIN_PATH)/cdsu-queue
CDSUNIT_STACK_EXE=$(BIN_PATH)/cdsu-stack
CDSUNIT_MISC_EXE=$(BIN_PATH)/cdsu-misc
CDSUNIT_EXE_FILES= $(CDSUNIT_MAP_EXE) $(CDSUNIT_SET_EXE) $(CDSUNIT_QUEUE_EXE) $(CDSUNIT_STACK_EXE) $(CDSUNIT_MISC_EXE)

ifeq ($(platform),mingw)
make_test : $(BIN_PATH)/test-hdr $(CDSUNIT_EXE_FILES)
	cd $(TEST_DATA_DIR); perl -X split.pl
	cp -f $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)
else
make_test : $(BIN_PATH)/test-hdr $(CDSUNIT_EXE_FILES)
	cd $(TEST_DATA_DIR); perl -X split.pl
	ln -sf $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)
endif

$(BIN_PATH)/test-hdr : $(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) \
	$(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_COMMON_OBJS) \
	$(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_LIBS) -o $@

$(CDSUNIT_MAP_EXE) : $(CDSUNIT_MAP_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_MAP_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_LIBS) -o $@

$(CDSUNIT_SET_EXE) : $(CDSUNIT_SET_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_SET_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_LIBS) -o $@

$(CDSUNIT_QUEUE_EXE) : $(CDSUNIT_QUEUE_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_QUEUE_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_LIBS) -o $@

$(CDSUNIT_STACK_EXE) : $(CDSUNIT_STACK_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_STACK_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_LIBS) -o $@

$(CDSUNIT_MISC_EXE) : $(CDSUNIT_MISC_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_MISC_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_LIBS) -o $@


CDSUNIT_MAP_EXE_DBG=$(CDSUNIT_MAP_EXE)-d
CDSUNIT_SET_EXE_DBG=$(CDSUNIT_SET_EXE)-d
CDSUNIT_QUEUE_EXE_DBG=$(CDSUNIT_QUEUE_EXE)-d
CDSUNIT_STACK_EXE_DBG=$(CDSUNIT_STACK_EXE)-d
CDSUNIT_MISC_EXE_DBG=$(CDSUNIT_MISC_EXE)-d
CDSUNIT_EXE_DBG_FILES= $(CDSUNIT_MAP_EXE_DBG) $(CDSUNIT_SET_EXE_DBG) $(CDSUNIT_QUEUE_EXE_DBG) $(CDSUNIT_STACK_EXE_DBG) $(CDSUNIT_MISC_EXE_DBG)

ifeq ($(platform),mingw)
make_debug_test : $(BIN_PATH)/test-hdr-debug $(CDSUNIT_EXE_DBG_FILES)
	cd $(TEST_DATA_DIR); perl -X split.pl
	cp -f $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)
else
make_debug_test : $(BIN_PATH)/test-hdr-debug $(CDSUNIT_EXE_DBG_FILES)
	cd $(TEST_DATA_DIR); perl -X split.pl
	ln -sf $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)
endif

$(BIN_PATH)/test-hdr-debug : $(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) \
	$(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_COMMON_OBJS) \
	$(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_DEBUG_LIBS) -o $@

$(CDSUNIT_MAP_EXE_DBG) : $(CDSUNIT_MAP_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_MAP_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_DEBUG_LIBS) -o $@

$(CDSUNIT_SET_EXE_DBG) : $(CDSUNIT_SET_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_SET_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_DEBUG_LIBS) -o $@

$(CDSUNIT_QUEUE_EXE_DBG) : $(CDSUNIT_QUEUE_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_QUEUE_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_DEBUG_LIBS) -o $@

$(CDSUNIT_STACK_EXE_DBG) : $(CDSUNIT_STACK_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_STACK_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_DEBUG_LIBS) -o $@

$(CDSUNIT_MISC_EXE_DBG) : $(CDSUNIT_MISC_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(CDSUNIT_MISC_FILE) $(CDSUNIT_COMMON_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) $(LD_TEST_COMMON_DEBUG_LIBS) -o $@

test: make_test 
test_debug: make_debug_test

test_hdr: $(BIN_PATH)/test-hdr
test_hdr_debug: $(BIN_PATH)/test-hdr-debug

##########################################
#
clean: 
	rm -f $(OBJ_PATH)/debug/*
	rm -f $(OBJ_PATH)/release/*
	rm -f $(TEST_COMMON_OBJS) $(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_OBJ_FILE)
	rm -f $(BIN_PATH)/libcds*
	rm -f $(BIN_PATH)/cdsu-*

