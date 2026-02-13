# -----------------------------------------------------------------------------
# src/CMakeLists.txt
#
# Master configuration for all source modules.
# This file includes each major component:
# - module_2: Sample module for testing/development
# - preprocessor: C preprocessor implementation
# - scanner: Lexical analyzer (scanner) implementation
#
# Each subdirectory contains its own CMakeLists.txt with specific configuration.
# The preprocessor and scanner each have their own module hierarchies.
# -----------------------------------------------------------------------------

message(STATUS "(${PROJECT_NAME}) Configuring src/ modules...")



# Add preprocessor component
add_subdirectory(preprocessor)
message(STATUS " - (${PROJECT_NAME}) Added preprocessor subdirectory")

# Add scanner component
add_subdirectory(scanner)
message(STATUS " - (${PROJECT_NAME}) Added scanner subdirectory")

message(STATUS "(${PROJECT_NAME}) Finished configuring all src/ modules.")
