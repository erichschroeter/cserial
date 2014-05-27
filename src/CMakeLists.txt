set(CSERIAL_SRCS
cserial.c
)
if(WIN32)
	set(CSERIAL_SRCS ${CSERIAL_SRCS} ${PROJECT_BINARY_DIR}/version.rc)
endif(WIN32)
add_library(cserial SHARED ${CSERIAL_SRCS})
add_library(cserial_static STATIC ${CSERIAL_SRCS})
install(TARGETS cserial DESTINATION lib COMPONENT libraries)

install(FILES
cserial.h
DESTINATION include
COMPONENT headers)
