# CMakeList.txt: LinuxTCPServer에 대한 CMake 프로젝트, 여기에 소스를 포함하고
# 프로젝트 특정 논리를 정의합니다.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("LinuxTCPServer")

# 이 프로젝트의 실행 파일에 소스를 추가합니다.
add_executable (LinuxTCPServer "LinuxTCPServer.cpp" "Common.h" "SocketEvent.cpp" "SocketEvent.h" "Common.cpp"  "Application.h" "Application.cpp" "UserData.h" "UserData.cpp" "Message.h" "Message.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LinuxTCPServer PROPERTY CXX_STANDARD 20)
endif()

# TODO: 필요한 경우 테스트를 추가하고 대상을 설치합니다.
