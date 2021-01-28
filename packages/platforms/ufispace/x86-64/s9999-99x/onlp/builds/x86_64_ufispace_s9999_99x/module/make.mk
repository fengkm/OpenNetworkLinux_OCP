###############################################################################
#
#
#
###############################################################################
THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
x86_64_ufispace_s9999_99x_INCLUDES := -I $(THIS_DIR)inc
x86_64_ufispace_s9999_99x_INTERNAL_INCLUDES := -I $(THIS_DIR)src
x86_64_ufispace_s9999_99x_DEPENDMODULE_ENTRIES := init:x86_64_ufispace_s9999_99x ucli:x86_64_ufispace_s9999_99x

