add_library (TUIO
	TuioClient.cpp TuioClient.h
	TuioServer.cpp TuioServer.h
	TuioTime.cpp TuioTime.h

	FlashSender.cpp FlashSender.h
	OscReceiver.cpp OscReceiver.h
	OscSender.cpp OscSender.h
	TcpReceiver.cpp TcpReceiver.h
	TuioBlob.cpp TuioBlob.h
	TuioContainer.cpp TuioContainer.h
	TuioCursor.cpp TuioCursor.h
	TuioDispatcher.cpp TuioDispatcher.h
	TuioManager.cpp TuioManager.h
	TuioObject.cpp TuioObject.h
	TuioPoint.cpp TuioPoint.h
	UdpReceiver.cpp UdpReceiver.h
	UdpSender.cpp UdpSender.h
)

if (WIN32)
  target_link_libraries(TUIO wsock32 Ws2_32 Winmm)
endif()

if (UNIX)
  target_link_libraries(TUIO pthread)
endif()
