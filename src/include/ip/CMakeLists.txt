if (WIN32)
	set(PLATFORM_DIR "win32")
elseif(UNIX)
	set(PLATFORM_DIR "posix")
endif()

add_library (ip
	./${PLATFORM_DIR}/NetworkingUtils.cpp NetworkingUtils.h
	./${PLATFORM_DIR}/UdpSocket.cpp	UdpSocket.h
)
