LOCAL_PATH := $(call my-dir)

# build libtrace library

include $(CLEAR_VARS)

pcap_intermediates := $(call intermediates-dir-for,STATIC_LIBRARIES,libpcap,,)

LOCAL_C_INCLUDES += \
        external/bzip2 \
	external/libpcap \
	external/lzo/include \
        external/zlib 
LOCAL_CFLAGS := -DHAVE_CONFIG_H
LOCAL_LDFLAGS := -llzo -lz \
	$(pcap_intermediates)/libpcap.a

LOCAL_MODULE := libtrace
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	ior-peek.c \
	format_pcapfile.c \
	protocols_pktmeta.c \
	ior-thread.c \
	ior-bzip.c \
	iow-bzip.c \
	format_erf.c \
	wandio.c \
	format_helper.c \
	protocols_transport.c \
	realloc.c \
	malloc.c \
	format_atmhdr.c \
	ior-stdio.c \
	link_wireless.c \
	format_rt.c \
	trace.c \
	iow-stdio.c \
	iow-zlib.c \
	protocols_l2.c \
	linktypes.c \
	format_pcap.c \
	format_linux.c \
	format_legacy.c \
	iow-thread.c \
	protocols_l3.c \
	format_tsh.c \
	ior-zlib.c \
	iow-lzo.c \
	protocols_ospf.c \
	strndup.c \
	format_duck.c
	#format_bpf.c \
	#format_dag24.c \
	#format_dag25.c

LOCAL_STATIC_LIBRARIES := libbz

include $(BUILD_SHARED_LIBRARY)
