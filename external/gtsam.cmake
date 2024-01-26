message(STATUS "[SuMa] Fetching GTSAM v4.2.0.")

include(FetchContent)
FetchContent_Declare(gtsam GIT_REPOSITORY https://github.com/borglab/gtsam.git GIT_TAG 4.2.0)
                     
# some flags for building only necessary stuff from GTSAM.                     
set(GTSAM_BUILD_TESTS Off CACHE BOOL "")
set(GTSAM_BUILD_EXAMPLES_ALWAYS Off CACHE BOOL "")
set(GTSAM_BUILD_EXAMPLES Off CACHE BOOL "")
set(GTSAM_BUILD_UNSTABLE Off CACHE BOOL "")
set(GTSAM_UNSTABLE_BUILD_PYTHON Off CACHE BOOL "")
set(GTSAM_UNSTABLE_INSTALL_MATLAB_TOOLBOX Off CACHE BOOL "")
set(GTSAM_BUILD_PYTHON Off CACHE BOOL "")
set(GTSAM_INSTALL_MATLAB_TOOLBOX Off CACHE BOOL "")

set(GTSAM_USE_SYSTEM_EIGEN On CACHE BOOL "") # only relevant option!

FetchContent_MakeAvailable(gtsam)