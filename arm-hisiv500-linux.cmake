SET(CROSS_COMPILE 1)
set (CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "")
IF(CROSS_COMPILE) 
SET(CMAKE_SYSTEM_NAME Linux)

SET(TOOLCHAIN_DIR "/opt/hisi-linux/x86-arm/arm-hisiv500-linux")

#SET(OPENSSL_ROOT_DIR  "${TOOLCHAIN_DIR}/target/lib" )
#SET(OPENSSL_CRYPTO_LIBRARY "${TOOLCHAIN_DIR}/target/lib")
#SET(LWS_OPENSSL_INCLUDE_DIRS  "${TOOLCHAIN_DIR}/target/include/" )
#SET(LWS_OPENSSL_LIBRARIES  "${TOOLCHAIN_DIR}/target/lib/libssl.so;${TOOLCHAIN_DIR}/target/lib/libcrypto.so" )
SET(LWS_OPENSSL_INCLUDE_DIRS  "/opt/hisi-linux/x86-arm/arm-hisiv500-linux/target/include/" CACHE PATH "Path to the OpenSSL include directory")
SET(LWS_OPENSSL_LIBRARIES  "/opt/hisi-linux/x86-arm/arm-hisiv500-linux/target/lib/libssl.so;/opt/hisi-linux/x86-arm/arm-hisiv500-linux/target/lib/libcrypto.so" CACHE PATH "Path to the OpenSSL library" )
#SET(OPENSSL_VERSION "1.0.2sss")
#SET(ZLIB_LIBRARY "${TOOLCHAIN_DIR}/target/lib")
#SET(ZLIB_INCLUDE_DIR "${TOOLCHAIN_DIR}/target/include")

message( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" )
message( "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" )
option(LWS_WITH_MINIMAL_EXAMPLES "Also build the normally standalone minimal examples, for QA" ON)
set(CMAKE_CXX_COMPILER arm-hisiv500-linux-g++)
set(CMAKE_C_COMPILER   arm-hisiv500-linux-gcc)
set(GNU_FLAGS "-mfpu=vfp -fPIC")
set(CMAKE_CXX_FLAGS "${GNU_FLAGS} ")
set(CMAKE_C_FLAGS "${GNU_FLAGS}  ")
#从来不在指定目录下查找工具程序
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#只在指定目录下查找库文件
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#只在指定目录下查找头文件
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_FIND_ROOT_PATH  
 ${TOOLCHAIN_DIR}/arm-hisiv500-linux-gnueabi/lib )

include_directories(${TOOLCHAIN_DIR}/arm-hisiv500-linux-gnueabi/include/c++/)

  
ENDIF(CROSS_COMPILE)
