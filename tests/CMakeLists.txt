add_test(
  NAME cli_help_long
  COMMAND $<TARGET_FILE:extendible_hashing> --help
)
add_test(
  NAME cli_help_short
  COMMAND $<TARGET_FILE:extendible_hashing> -h
)
add_test(
  NAME cli_version_long
  COMMAND $<TARGET_FILE:extendible_hashing> --version
)
add_test(
  NAME cli_version_short
  COMMAND $<TARGET_FILE:extendible_hashing> -V
)
add_test(
  NAME cli_unknown_long
  COMMAND $<TARGET_FILE:extendible_hashing> --unknown
)
add_test(
  NAME cli_unknown_short
  COMMAND $<TARGET_FILE:extendible_hashing> -u
)

set(version_regex "${PROJECT_NAME} ${PROJECT_VERSION}")
set(help_regex "${PROJECT_DESCRIPTION}")

set_property(
  TEST cli_unknown_long cli_unknown_short
  PROPERTY WILL_FAIL TRUE
)

set_property(
  TEST cli_version_long cli_version_short
  PROPERTY PASS_REGULAR_EXPRESSION "${version_regex}"
)

set_property(
  TEST cli_help_long cli_help_short
  PROPERTY PASS_REGULAR_EXPRESSION "${help_regex}"
)
