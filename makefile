#-------------------------------------------------------------------------------
APP           := XGame
SERVER_NAME   := util
SUB_DIR_1	  := XTimer
SUB_DIR_2	  := GeoLite2
TARGET_NAME   := $(SERVER_NAME)
TARGET 	      := lib$(SERVER_NAME).a
TARSFILE_DIR  := /home/tarsproto/$(APP)/$(SERVER_NAME)
CFLAGS        += -lm
CXXFLAGS      += -lm
CONFIG        :=

INCLUDE   += -I/usr/local/cpp_modules/wbl/include
LIB       += -L/usr/local/cpp_modules/wbl/lib -lwbl

INCLUDE   += -I/usr/local/cpp_modules/protobuf/include
LIB       += -L/usr/local/cpp_modules/protobuf/lib -lprotobuf

INCLUDE   += -I/usr/local/cpp_modules/curl/include
LIB		  += -L/usr/local/cpp_modules/curl/lib -lcurl

INCLUDE   += -I/usr/local/cpp_modules/uuid/include
LIB       += -L/usr/local/cpp_modules/uuid/lib -luuid

INCLUDE   += -I/usr/local/cpp_modules/maxminddb/include
LIB       += -L/usr/local/cpp_modules/maxminddb/lib -lmaxminddb

LOCAL_SRC += $(SUB_DIR_1)/xtime_userface.cpp \
			 $(SUB_DIR_1)/xtimer.cpp \

LOCAL_SRC += $(SUB_DIR_2)/GeoLite2PP.cpp \
			 $(SUB_DIR_2)/GeoLite2PP_error_category.cpp \

#-------------------------------------------------------------------------------
include /home/tarsproto/XGame/Comm/Comm.mk

publish:
	rm -rvf $(TARSFILE_DIR)
	mkdir -vp $(TARSFILE_DIR)
	mkdir -vp $(TARSFILE_DIR)/$(SUB_DIR_1)
	@make cleanall
	@make all
	cp -rf *.h $(TARSFILE_DIR)
	cp -rf $(SUB_DIR_1)/*.h $(TARSFILE_DIR)/$(SUB_DIR_1)
	cp -rf *.tars $(TARSFILE_DIR)
	cp -rf ${TARGET} $(TARSFILE_DIR)
	echo "INCLUDE += -I$(TARSFILE_DIR)"  >  $(TARSFILE_DIR)/$(TARGET_NAME).mk;
	echo "REMOTE_OBJ += $(TARSFILE_DIR)/$(TARGET) "  >> $(TARSFILE_DIR)/$(TARGET_NAME).mk;

include /usr/local/tars/cpp/makefile/makefile.tars

#-------------------------------------------------------------------------------
