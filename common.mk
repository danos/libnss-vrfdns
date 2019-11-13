AM_CPPFLAGS = -include build_config.h # Details of the build configuration
AM_CFLAGS = -Wall -Wextra -Werror \
	-Wno-missing-field-initializers \
	-Wmissing-prototypes \
	-Wundef \
	-Wredundant-decls
AM_CXXFLAGS = -Wall -Wextra -Werror \
	-Wundef \
	-Wlogical-op \
	-Wredundant-decls
AM_LDFLAGS =
