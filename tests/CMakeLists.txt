if(WIN32)
	add_executable(exec-assemblyWindows "exec-assemblyWindows.cpp")
	target_link_libraries(exec-assemblyWindows ${Donut})

	add_executable(RevShellWindows "RevShellWindows.cpp")
	target_link_libraries(RevShellWindows)
else()
endif()
