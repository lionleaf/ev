option(sanitizer.asan "Enable address sanitizer" ON)
option(sanitizer.ubsan "Enable undefined behavior sanitizer" OFF)
option(sanitizer.tsan "Enable thread sanitizer" OFF)
option(sanitizer.msan "Enable memory sanitizer" OFF)
option(sanitizer.recovery "Continue on failure" ON)

if(sanitizer.asan OR sanitizer.ubsan OR sanitizer.tsan OR sanitizer.msan)
  set(sanitizer.frame_pointer ON CACHE INTERNAL "")
endif()

add_library(sanitizers INTERFACE)
target_compile_options(sanitizers INTERFACE
  $<$<BOOL:${sanitizer.asan}>:fsanitize=address>
  $<$<BOOL:${sanitizer.ubsan}>:fsanitize=undefined>
  $<$<BOOL:${sanitizer.tsan}>:fsanitize=thread>
  $<$<BOOL:${sanitizer.msan}>:fsanitize=memory>
  $<$<BOOL:${sanitizer.frame_pointer}>:fno-omit-frame-pointer>
  $<$<NOT:$<BOOL:${sanitizer.recovery}>>:fno-sanitize-recover=all>
)