#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
using ::testing::_;

class MockInterface {
 public:
  MOCK_METHOD(void, panic, (const std::string), ());
};

extern "C" {
void panic(MockInterface *mock_interface, const char *fmt, ...) {
  std::string non_variadic("");

  if (fmt != NULL) {
    va_list args;

    va_start(args, fmt);
    // Get length of fmt including arguments
    int nr = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char buffer[nr + 1];
    va_start(args, fmt);
    vsnprintf(buffer, nr + 1, fmt, args);
    va_end(args);

    non_variadic = std::string(buffer);
  }

  mock_interface->panic(non_variadic);
}
}

void foo(MockInterface *mock_interface, bool some_condition) {
  if (some_condition) {
    panic(mock_interface, "error message");
  }
}

TEST(Panic, isCalled) {
  MockInterface mock_interface;

  EXPECT_CALL(mock_interface, panic(_)).Times(1);
  foo(&mock_interface, /*some_condition=*/true);
}

TEST(Panic, isNotCalled) {
  MockInterface mock_interface;

  EXPECT_CALL(mock_interface, panic(_)).Times(0);
  foo(&mock_interface, /*some_condition=*/false);
}