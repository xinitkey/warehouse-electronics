cmake_minimum_required(VERSION 3.20)

project(warehouse_electronics CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Общие исходники
file(GLOB_RECURSE SRC_FILES
				src/*.cpp
)

add_executable(warehouse_app WIN32 ${SRC_FILES})

target_include_directories(warehouse_app PRIVATE
				${CMAKE_SOURCE_DIR}/include
				${CMAKE_SOURCE_DIR}/src
)

# -------- Платформенно-зависимая часть --------

if (WIN32)
	# toolchain vcpkg ты задаёшь в CMakePresets / через -DCMAKE_TOOLCHAIN_FILE,
	# здесь просто пользуемся пакетами из vcpkg
	find_package(wxWidgets CONFIG REQUIRED COMPONENTS core base)
	find_package(unofficial-sqlite3 CONFIG REQUIRED)

	target_link_libraries(warehouse_app
								PRIVATE
												wx::core
												wx::base
												unofficial::sqlite3::sqlite3
				)

elseif (UNIX)
	# Arch: wxwidgets-gtk3 и sqlite из pacman
	# sudo pacman -S wxwidgets-gtk3 wxwidgets-common sqlite

	find_package(wxWidgets REQUIRED COMPONENTS core base)
	include(${wxWidgets_USE_FILE})  # нужно для include dirs / defines [web:189]

	find_package(SQLite3 REQUIRED)

	target_link_libraries(warehouse_app
								PRIVATE
												${wxWidgets_LIBRARIES}
												SQLite::SQLite3
				)
else()
	message(FATAL_ERROR "Unsupported platform")
endif()

