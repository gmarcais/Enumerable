##########################
# Google testing library #
##########################
# Build rules for libraries.
check_LIBRARIES = %D%/libgtest.a %D%/libgtest_main.a

%C%_libgtest_a_SOURCES = %D%/gtest/src/gtest-all.cc %D%/gtest/gtest.h %D%/gtest/test.hpp
%C%_libgtest_main_a_SOURCES = %D%/gtest/src/gtest_main.cc
%C%_libgtest_main_a_LIBADD = %D%/libgtest.a
%C%_libgtest_main_a_CXXFLAGS = -I$(srcdir)/tests

#LDADD = -L%D% -lgtest_main -lgtest
LDADD = %D%/libgtest_main.a %D%/libgtest.a
AM_CXXFLAGS += -I$(srcdir)/tests


#####################
# Unittest programs #
#####################
unittests_programs = %D%/range
check_PROGRAMS += $(unittests_programs)
TESTS += $(unittests_programs)


%C%_range_SOURCES = %D%/range.cc


