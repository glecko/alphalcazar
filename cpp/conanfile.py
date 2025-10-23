from conan import ConanFile

class AlphalcazarConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "spdlog/1.16.0", "fmt/12.0.0"
    generators = "CMakeDeps", "CMakeToolchain"

    default_options = {
        # Disable exception handling when building spdelog.
        # The project disables exception handling (see @ref cpp/CMakeLists.txt),
        # so dependencies linked to its libs/exes also need to be built without EH
        "spdlog/*:no_exceptions": True,
    }

    def requirements(self):
        self.test_requires("gtest/1.17.0")
