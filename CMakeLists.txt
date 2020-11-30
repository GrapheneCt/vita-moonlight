cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{DOLCESDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{DOLCESDK}/share/dolce.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define DOLCESDK to point to your SDK path!")
  endif()
endif()

project(moonlight)
include("${DOLCESDK}/share/dolce.cmake" REQUIRED)

dolce_gen_libs(SceIniFileProcessor_stubs
  SceIniFileProcessor.yml
  LIB SceIniFileProcessor_stub_weak)
  
dolce_gen_libs(ScePsp2Compat_stubs
  ScePsp2Compat.yml
  LIB ScePsp2Compat_stub_weak)
  
dolce_gen_libs(SceLibc_stubs
  SceLibc.yml
  LIB SceLibc_stub_weak)

set(VITA_APP_NAME "Moonlight")
set(VITA_TITLEID  "GRVA00010")
set(VITA_VERSION  "01.00")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -O3 -std=c99")

link_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_SOURCE_DIR}/third_party/mdns
	${CMAKE_SOURCE_DIR}/third_party/h264bitstream
	${CMAKE_SOURCE_DIR}/third_party/libuuid
	${CMAKE_SOURCE_DIR}/third_party/expat
	)

include_directories(
	include/
	third_party/moonlight-common-c/src/
	third_party/moonlight-common-c/reedsolomon/
	libgamestream/
	third_party/libuuid/src/
	third_party/h264bitstream/
	third_party/enet/include/
	third_party/mdns/
)

add_executable(${PROJECT_NAME}
	src/config.c
	src/input/mapping.c
	src/connection.c
	src/global.c
	src/debug.c
	src/loop.c
	src/main.c
	src/platform.c
	src/util.c
	src/device.c

	src/audio/vita.c
	src/video/vita.c
	src/input/vita.c
	src/input/keyboard.c
	src/power/vita.c
	src/font.c
	src/gui/guilib.c
	src/gui/ime.c
	src/gui/ui.c
	src/gui/ui_settings.c
	src/gui/ui_connect.c
	src/gui/ui_device.c
	src/ini.c

	libgamestream/client.c
	libgamestream/http.c
	libgamestream/mkcert.c
	libgamestream/sps.c
	libgamestream/xml.c

	third_party/moonlight-common-c/src/AudioStream.c
	third_party/moonlight-common-c/src/ByteBuffer.c
	third_party/moonlight-common-c/src/Connection.c
	third_party/moonlight-common-c/src/ControlStream.c
	third_party/moonlight-common-c/src/FakeCallbacks.c
	third_party/moonlight-common-c/src/InputStream.c
	third_party/moonlight-common-c/src/LinkedBlockingQueue.c
	third_party/moonlight-common-c/src/Misc.c
	third_party/moonlight-common-c/src/Platform.c
	third_party/moonlight-common-c/src/PlatformSockets.c
	third_party/moonlight-common-c/src/RtpReorderQueue.c
	third_party/moonlight-common-c/src/RtspConnection.c
	third_party/moonlight-common-c/src/RtspParser.c
	third_party/moonlight-common-c/src/SdpGenerator.c
	third_party/moonlight-common-c/src/VideoDepacketizer.c
	third_party/moonlight-common-c/src/VideoStream.c
	third_party/moonlight-common-c/src/RtpFecQueue.c
	third_party/moonlight-common-c/src/SimpleStun.c
	third_party/moonlight-common-c/reedsolomon/rs.c

	third_party/enet/callbacks.c
	third_party/enet/compress.c
	third_party/enet/host.c
	third_party/enet/list.c
	third_party/enet/packet.c
	third_party/enet/peer.c
	third_party/enet/protocol.c
	third_party/enet/unix.c
)

target_link_libraries(${PROJECT_NAME}
	crypto
	opus

	SceDisplay_stub
	SceDisplayUser_stub
	SceCtrl_stub
	SceTouch_stub
	SceNet_stub
	SceSysmodule_stub
	SceNetCtl_stub
	ScePower_stub
	SceVideodecUser_stub
	SceVideodecLowDelayUser_stub
	SceVideodecAsyncUser_stub
	SceSysmodule_stub
	SceAppMgr_stub
	SceAudio_stub
	SceIme_stub

	SceGxm_stub
	ScePgf_stub
	ScePvf_stub
	SceCommonDialog_stub
	SceLibm_stub
	SceDbg_stub
	SceGxmInternalForVsh_stub
	SceSharedFb_stub
	SceKernelDmacMgr_stub
	SceCodecengine_stub
	SceAppMgr_stub
	SceIme_stub
	SceSystemgesture_stub
	SceVshBridge_stub
	SceHid_stub
	SceIniFileProcessor_stub_weak
	ScePsp2Compat_stub_weak
	SceLibc_stub_weak
	taihen_stub
	vita2d_sys_stub
	mdns_stub
	uuid_stub
	h264bitstream_stub
	libexpat_stub
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/boot_param.bin
  COMMAND dolce-make-bootparam attribute 0x02 ${CMAKE_CURRENT_BINARY_DIR}/boot_param.bin
)

set (DOLCE_ELF_CREATE_FLAGS "${DOLCE_ELF_CREATE_FLAGS} -h 33554432")

dolce_create_self(eboot.bin ${PROJECT_NAME} UNSAFE BOOT_PARAM ${CMAKE_CURRENT_BINARY_DIR}/boot_param.bin)

add_executable(exit_module
  module/source/main.c
)

target_compile_options(exit_module PRIVATE -std=gnu99)

target_link_libraries(exit_module
  taihenForKernel_stub
  taihenModuleUtils_stub
  "-nostdlib"
)

dolce_create_self(exit_module.skprx exit_module CONFIG module/exit_module.yml UNSAFE)

set(DOLCE_MKSFOEX_FLAGS "${DOLCE_MKSFOEX_FLAGS} -d ATTRIBUTE=561160 -d ATTRIBUTE2=1 -s CATEGORY=gdc")

dolce_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} eboot.bin
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
	CONTENT_ID_LABEL "MOONLIGHTVITA000"
	FILE sce_sys/icon0.png sce_sys/icon0.png
	FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
	FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
	FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
	FILE assets/nerdfont.ttf assets/nerdfont.ttf
	FILE ${CMAKE_CURRENT_BINARY_DIR}/exit_module.skprx module/kernel/exit_module.skprx
	FILE third_party/mdns/mdns.suprx module/user/mdns.suprx
	FILE third_party/libuuid/uuid.suprx module/user/uuid.suprx
	FILE third_party/h264bitstream/h264bitstream.suprx module/user/h264bitstream.suprx
	FILE third_party/vita2d_sys/vita2d_sys.suprx module/user/vita2d_sys.suprx
	FILE third_party/expat/libexpat.suprx module/user/libexpat.suprx
    )
